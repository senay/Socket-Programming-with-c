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
        pid_t                   child;
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
        
       for(;;){
		
		fgets(command, sizeof(command), stdin);
                len=strlen(command);          
                
                if(strncmp(command,"GET",3)==0){ 
                        if((child=fork())==0){
		                j=0;
				for(i=3; i<len; i++){
					filename[j]=command[i]; 
				        j++;   			
				}
			
				filename[j]='\0';
				printf("%s\n",filename);                        		        
						
				if(send(s, command, len,0) != len){
					printf("Write error\n");
					break;
				}
				printf("Waiting for response...\n");
				result = recv(s, reply, REPLYLEN, 0);

		                if((strcmp(reply, "-ERR\n") == 0)){
					printf("Communication channel closed. Error Replied from server.\n");
				        printf("=========================================\n");
					break;
		                }
                                
				else{
					for(i=3,j=0; i<strlen(reply); i++){
					size[j]=reply[i]; 
				        j++;   			
				}
				size[j]='\0';
				printf("Requested File Name: %s size: %s\n", filename, size);
				
				if((rfp=fopen(filename, "wt")) == NULL){
						 perror("file Error:\n");
				 }
					 
				for(nleft=size;nleft>0;){
					nread=recv(s,reply,sizeof(reply),0);
					reply[sizeof(reply)]='\0';
					printf("Recieve message size:%d\n", nread);
					printf("Data Buffer====%s\n",reply);

					if(nread>0){
						nleft -=nread;
						fprintf(rfp, "%s",reply);
						printf("left size:%d\n",nleft);
					}									   
					else if(nread<=0)
						break;
					else
					        break;
				}
						
				fclose(rfp);
                             
				
				
				}

                             
                        }
                        

                

		}

                else if(strcmp(command,"QUIT\n")==0){
                        printf("Communication channel closed\n");
			printf("=========================================\n");
			break; 		
		}
		else if(strcmp(command,"A\n")==0){
		        printf("Communication channel closed and file transfer aborted\n");
			printf("=========================================\n");
                        kill(child);
			break; 		
		}
		else {
                        printf("Invalid Command!\n");
			printf("=========================================\n");
			break; 		
		}
                




      }
               
			
		
closesocket(s);
exit(0);
}

void err_fatal(char *mes) {
printf("%s, errno=%d\n", mes);
perror(""); exit(1);
}

