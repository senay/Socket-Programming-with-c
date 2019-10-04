/*
 *	File Client1u.C
 *      ECHO UDP CLIENT with the following feqatures:
 *      - Gets server IP address and port from keyboard 
 *      - LINE/ORIENTED:
 *        > continuously reads lines from keyboard
 *        > sends each line to the server
 *        > waits for response (at most for a fixed amount of time) and diaplays it
 *      - Terminates when the "close" or "stop" line is entered
 */


#include     <stdio.h>
#include     <stdlib.h>
#include     <string.h>
#include     <inttypes.h>
#include     "mysocket.h"
#include     "sockwrap.h"
#include     "errlib.h"
#define BUFLEN 31
#define TIMEOUT 3

char prog_name;

main(int argc, char* argv[])
{

        if(argc != 4){
printf("Invalid number of arguments");
exit(2);
}
	//char     		buf[BUFLEN];	   /* transmission buffer */
	char	 		rbuf[BUFLEN];	   /* reception buffer */

	uint32_t		taddr_n, taddr_h;  /* server IP addr. (net/host byte order resp.) */
	uint16_t		tport_n, tport_h;  /* server port number */

	SOCKET			s;
	struct sockaddr_in	saddr, caddr;
	fd_set			cset;
	struct timeval		tval;
   	int 			result, namelen;

      prog_name = argv[0]; 

	
	taddr_n = inet_addr(argv[1]);
	if (taddr_n == INADDR_NONE){
		printf("Invalid address");
                      exit(1);
}
	taddr_h = ntohl(taddr_n);

	
	if (sscanf(argv[2], "%" SCNu16, &tport_h)!=1)
		{
                   printf("Invalid port number");
                                         exit(1);
}
  	tport_n = htons(tport_h);

	/* create the socket */
    	printf("Creating socket\n");
	s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (s == INVALID_SOCKET){
		printf("socket() failed");
                      exit(1);
}
	printf("done. Socket number: %d\n",s);

	/* prepare client address structure */
    	caddr.sin_family      = AF_INET;
	caddr.sin_port        = htons(0);  /* ask bind for an unused port number */
	caddr.sin_addr.s_addr = htonl(INADDR_ANY);

	/* bind */
	printf("Binding to an unused port number\n");
	result = bind(s, (struct sockaddr *) &caddr, sizeof(caddr) );
	if (result == -1){
		printf("bind() failed");
                      exit(1);
}
	namelen = sizeof(caddr);
	getsockname(s, (struct sockaddr *) &caddr, &namelen );
	//showAddr("done. Bound to addr: ", &caddr);

	/* prepare server address structure */
    	saddr.sin_family      = AF_INET;
	saddr.sin_port        = tport_n;
	saddr.sin_addr.s_addr = taddr_n;

            int			n;
	    struct sockaddr_in 	from;
	    int 		fromlen;
        
            n=sendto(s, argv[3], strlen(argv[3]), 0, (struct sockaddr *) &saddr, sizeof(saddr));
	    if (n != strlen(argv[3]))
	    {
		printf("Write error\n");
		exit(1);
	    }


	    int count=0;
            while(count<5)//to control maximum number of retransmission due to no response from the server in 3 seconds
   	   {       
	    printf("waiting for response...\n");
	    FD_ZERO(&cset);
	    FD_SET(s, &cset);
	    tval.tv_sec = TIMEOUT;
	    tval.tv_usec = 0;
	     
		
            n = select(FD_SETSIZE, &cset, NULL, NULL, &tval);
	    if (n == -1){
		printf("select() failed");
                      exit(1);
             }
	    if (n>0)
            {
               
		/* receive datagram */
		fromlen = sizeof(struct sockaddr_in);
	    	n=recvfrom(s,rbuf,BUFLEN-1,0,(struct sockaddr *)&from,&fromlen);//recvfrom - because of UDP
                if (n != -1)
	    	{
			rbuf[n] = '\0';
			//showAddr("Received response from", &from);
			printf("Data received from server!\n");
			printf(": [%s]\n", rbuf);//print received text from server
                        break;

	    	}
		else
		{ 
                  printf("Error in receiving response\n");
                  count++;                  
                }
               }
		
	    
     
	   else printf("No response received after %d seconds\n",TIMEOUT);
                  count++;
	    printf("=======================================================\n");  
     }
if (count==5)
printf("No reply from server\n");

       
        	closesocket(s);
		exit(0);
}


