#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<inttypes.h>
#include "mysocket.h"

#define COMMANDLEN 128
#define	REPLYLEN    50
#define TOUT 	    18 

char * prog_name;

int main(int argc, char *argv[]){

	char     		buf[COMMANDLEN];	   	
	short			port;
	SOCKET			s;
	int 			result;
	struct sockaddr_in	saddr;			/*server address structure*/
	char 			command[COMMANDLEN], reply[REPLYLEN];	/* transmission buffer */
	char			filename[132], size[132];
	int 			n, len, size2;
	int			i, j;
        fd_set			cset;
	int                     nleft,nread;
        FILE                    *rfp;

	/* for errlib to know the program name */
	prog_name = argv[0];

	/*check arguments*/
	if(argc != 3)
		err_fatal("usage: <address> <port>\n");

	port=atoi(argv[2]);
	printf("%d\n", port);
	printf("%s\n", argv[1]);
	

	/*initialize socket API if needed*/
	//SockStartup();
	
	/*create the socket*/
	printf("Socket creation\n");
	s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(s==INVALID_SOCKET)
		err_fatal("socket() failed\n");
	printf("Done. Socket fd number: %d\n", s);

	/*prepare address structure*/
	saddr.sin_family 	= AF_INET;
	saddr.sin_port	 	= htons(port);
	saddr.sin_addr.s_addr	= inet_addr(argv[1]);

	/*connect*/
	//showAddr("Connecting to target address", &saddr);
	result = connect(s, (struct sockaddr *)&saddr, sizeof(saddr));
	if(result == -1)
		err_fatal("connect() failed");
	printf("Done\n");

	
	/*main client loop*/
	for(;;){
                            FD_ZERO(&cset);
	                    FD_SET(s, &cset);
                            FD_SET(fileno(stdin), &cset);
                            n = select(FD_SETSIZE, &cset, NULL, NULL, NULL);
	if(FD_ISSET(s,&cset)){
		printf("Waiting for response...\n");
		result = recv(s, reply, REPLYLEN, 0);		
		if((strcmp(reply, "-ERR\n") == 0)){
			printf("Error message from server received.Communication channel closed.\n");
                        printf("=========================================\n");
			break;
		}
	
		else{
			printf("%s\n", reply);	
			for(i=3, j=0; i<strlen(command); i++){
				size[j] = reply[i];
				j++;				
		       }
			size[j]='\0';
			printf("Requested filename: %s size: %s\n", filename, size);		
			size2=atoi(size);
		        if((rfp=fopen(filename, "wt")) == NULL)
				 {
					 perror("file Error:\n");
				 }
				 for(nleft=size;nleft>0;)
					 {
						nread=recv(s,reply,sizeof(reply),0);
						reply[sizeof(reply)]='\0';
						//printf("Recieve message size:%d\n", nread);
						//printf("Data Buffer====%s\n",reply);
	
						if(nread>0)
						{
							nleft -=nread;
							fprintf(rfp, "%s",reply);
							//printf("left size:%d\n",nleft);
						}									   
						 else   
							printf("error\n");
							break;

					}
					
					fclose(rfp);
	
		
			}

		
                }
        if(FD_ISSET(fileno(stdin),&cset)){
                fgets(command, sizeof(command), stdin);
		len=strlen(command);
                 if(strncmp(command, "GET",3) == 0){
                    strcpy(reply, command);
		    printf("%s\n",reply);
		    if(writen(s, reply, n) != n)
		     printf("Write error while sending command\n");
		    printf("command sent\n");
                   }                 
		
		 else if(strcmp(command,"\n")==0){
                   strcpy(reply, "QUIT\n");
		   //command="QUIT\n";
                   if(writen(s, command, n) != n)
		     printf("Write error while sending command\n");
		   printf("command sent\n");                    
                  }        
                else if(strcmp((command),"A\n")==0)
                  {
                    closesocket(s);
	           //SockCleanup();
	            exit(0);                                
                   }
                 else
                  {
                    printf("invalid command\n");
                    break;
                  }
               
         }
		
	
 }
	closesocket(s);
	//SockCleanup();
	exit(0);
}

void err_fatal(char *mes) {
printf("%s, errno=%d\n", mes);
perror(""); exit(1);
}

