/*
 * 	File Server0u.c
 *	ECHO UDP SERVER with the following features:
 *      - Gets port from keyboard 
 *      - SEQUENTIAL: serves one request at a time
 */


#include    <stdio.h>
#include    <stdlib.h>
#include    <string.h>
#include    <inttypes.h>
#include    "mysocket.h"

#define BUFLEN		65536
char prog_name;

main(int argc, char* argv[])
{
         if(argc != 2){
printf("Invalid number of arguments");
exit(2);
}
	char	 		buf[BUFLEN];		/* reception buffer */
	uint16_t 		lport_n, lport_h;	/* port where server listens */
	SOCKET			s;
	int			result, addrlen, n;
	struct sockaddr_in	saddr, from;

         prog_name = argv[0];



	if (sscanf(argv[1], "%" SCNu16, &lport_h)!=1){
		printf("Invalid port number");
                      exit(1);
}
  	lport_n = htons(lport_h);

	/* create the socket */
	printf("creating socket\n");
	s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (s == INVALID_SOCKET){
		printf("socket() failed");
                      exit(1);
}
	printf("done, socket number %u\n",s);

	/* bind the socket to all local IP addresses */
	saddr.sin_family      = AF_INET;
	saddr.sin_port        = lport_n;
	saddr.sin_addr.s_addr = INADDR_ANY;
	//showAddr("Binding to address", &saddr);
	result = bind(s, (struct sockaddr *) &saddr, sizeof(saddr));
	if (result == -1){
		printf("bind() failed");
                      exit(1);
}
	printf("done.\n");

	/* main server loop */
	for (;;)
	{
             prog_name = argv[0];
	    addrlen = sizeof(struct sockaddr_in);
	    n=recvfrom(s, buf, BUFLEN-1, 0, (struct sockaddr *)&from, &addrlen);
	    if (n != -1)
	    {
	    	buf[n] = '\0';
	    	//showAddr("Received message from", &from);
	    	printf(": [%s]\n", buf);

	    	if(sendto(s, buf, n, 0, (struct sockaddr *)&from, addrlen) != n)
			printf("Write error while replying\n");
	        else
			printf("Reply sent\n");
	    }
	}
}


