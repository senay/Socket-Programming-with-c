#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <signal.h>
#include <sys/wait.h>

#include "errlib.h"
#include "sockwrap.h"

#define COMMANDLEN 128
#define	REPLYLEN    50
#define TOUT 	    18 

char * prog_name;

int main(int argc, char *argv[]){

	char     		buf[COMMANDLEN];	   	
	short			port;
	int			s;
	int 			result;
	struct sockaddr_in	saddr;			/*server address structure*/
	char 			command[COMMANDLEN], reply[REPLYLEN];	/* transmission buffer */
	char			filename[132], size[132];
	int 			n, len, size2;
	int			i, j;
        int                     nleft,nread;
        FILE                    *rfp;
        void *ptr;


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
	if(s < 0)
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
		
		fgets(command, sizeof(command), stdin);
		len=strlen(command);
		if(strncmp(command,"GET",3)==0){		
			
			for(i=3, j=0; i<len; i++){			
					filename[j] = command[i];
					j++;
			}		
			filename[j]='\0';
		}
                if(strcmp(command,"QUIT\n")==0){		
			if(writen(s, command, len) != len)
		             printf("Write error\n");
                        close(s);
		        exit(1);
		}
		
		if(writen(s, command, len) != len){
			printf("Write error\n");
			break;
		}

                printf("Request made for the transfer of %s\n",filename);
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
		        for(nleft=size2;nleft>0;)
			 {
				//nread=recv(s,reply,sizeof(reply),0);
				//reply[sizeof(reply)]='\0';
				//printf("Recieve message size:%d\n", nread);
				//printf("Data Buffer====%s\n",reply);
				if(nleft>sizeof(reply)){
				 nread=recv(s,reply,sizeof(reply),0);
				reply[sizeof(reply)]='\0';}
				else{
				 nread=recv(s,reply,nread,0);
				 reply[nleft]='\0';
				}
				if(nread>0)
				{
					nleft -=nread;
					fprintf(rfp, "%s",reply);
					//printf("left size:%d\n",nleft);
				}									   
				 else{
 
					printf("error\n");
					break;

  				}
			}
			
			fclose(rfp);
	
		
			
		}
  	}


	close(s);	
	exit(0);
}
