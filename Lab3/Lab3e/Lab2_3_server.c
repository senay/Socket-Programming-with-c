#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<inttypes.h>
#include "mysocket.h"

#define RCOMMANDLEN 128
#define REPLYLEN     50
#define FILELEN	    132

char *prog_name;

int main(int argc, char *argv[]){

	SOCKET			conn_request_skt;	/*socket where connections are accepted*/
	char 			command[RCOMMANDLEN];	/*reception buffer*/
	char			filename[FILELEN];
	char			reply[REPLYLEN];
	char			file[FILELEN];
	uint16_t		lport_n, lport_h;	/*port where the server listens(net/host byte ord resp.)*/
	int			bklog = 2; 		/*listening backlog*/
	SOCKET			s;
	int 			result, addrlen, n;
	struct sockaddr_in	saddr, caddr;		/*server and client address structures*/
	short 			port;
	int 			i,j;
	FILE			*fp;
	long			size;
	


	/*check arguments*/
	if(argc != 2)
		err_fatal("usage: <port>\n");
	
	port=atoi(argv[1]);	
	printf("%d\n", port);


	/*Initialize socket API if needed*/
	//SockStartup();
	
	/*create the socket*/
	printf("Creating socket\n");
	s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(s == INVALID_SOCKET)
		err_fatal("socket() failed\n");
	printf("Done, socket number %u\n", s);

	/*bind socket to any local IP address*/
	saddr.sin_family=AF_INET;
	saddr.sin_port=htons(port);
	saddr.sin_addr.s_addr=INADDR_ANY;
	//showAddr("Binding to address", &saddr);
	result=bind(s, (struct sockaddr *)&saddr, sizeof(saddr));
	if(result == -1)
		err_fatal("bind() failed\n");
	printf("Done\n");

	/*listen*/
	printf("Listening a socket %d with backlog = %d\n", s, bklog);
	result=listen(s, bklog);
	if(result == -1)
		err_fatal("listen() failed");
	printf("Done\n");
	
	conn_request_skt = s;
	
	/*main server loop*/
	for(;;){
	
		/*accept next connection*/
		addrlen = sizeof(struct sockaddr_in);
               
                
		s = accept(conn_request_skt, (struct sockaddr *) &caddr, &addrlen);
		if(s==INVALID_SOCKET)
			err_fatal("accept() failed");
		//showAddr("Accepted connection from", &caddr),
		printf("new socket: %u\n", s);
		
		/*serve the client on socket s*/
              
             
                closesocket(conn_request_skt);
		for(;;){
		                       
			n=recv(s, command, RCOMMANDLEN-1, 0);
			
			if(n < 0){
				printf("Read error\n");
				closesocket(s);
				printf("Socket %d closed\n", s);
				break;
			}
			else if(n == 0){
				printf("Connection closed by party on socket %d\n", s);
				closesocket(s);
				break;
			}
			else{
				printf("Received line from socket %03d: \n", s);
				command[n]=0;
				printf("[%s]\n", command);
				if(strncmp(command, "GET", 3) != 0 && strcmp(command, "QUIT\n") != 0){						
					printf("The requested command is invalid!!!\n");
					strcpy(reply, "-ERR\n");
					if(writen(s, reply, n) != n)
						printf("Write error while replying\n");
					else
						printf("Reply sent\n");
				}
				else{	
					if(strncmp(command, "GET", 3) == 0)
					{
						for(i=0, j=0; i<strlen(command); i++){
							if(command[i] != 'G' && command[i] != 'E' && command[i] != 'T' && command[i] != '\n'){
								filename[j] = command[i];
								j++;
							}
						}
						filename[j]='\0';
						//strncpy(filename, command + 3, strlen(command) - 4);
				
						printf("%s\n", filename);
						strcpy(file, filename);
						printf("%s\n", file);
						printf("Command received by the client: %s\n", command);
						if((fp=fopen(file, "rt")) != NULL){
							fseek (fp, 0, SEEK_END);   
	    						size=ftell (fp);
	   						fclose (fp);
							printf("%ld\n", size);
							//strcpy(reply, "+OK");
							sprintf(reply, "+OK%d", size);
							if(writen(s, reply, n) != n)
								printf("Write error while replying\n");
							else
								
								printf("Reply sent\n");
							printf("Send file content %d\n", size);
							if((fp=fopen(file, "rt")) != NULL){
								i=0;
								
									fgets(reply, size, fp);
									
									//printf("%s", reply);
									if(writen(s, reply, size) != size)
										printf("Write error while replying\n");
									else
										printf("%s", reply);
									printf("\nEnd transmission\n");
									fclose(fp);
								
							}
							else
							{
								printf("Error while openng the file\n");
							}
									
													
						}
						else{							
							printf("The requested file doesn't exist!!!\n");
							strcpy(reply, "-ERR\n");
                                                        printf("error%s\n",reply);
							if(writen(s, reply, n) != n)
								printf("Write error while replying\n");
							else
								printf("Reply sent\n");
						}
					}
					
				
					if(strcmp((command), "QUIT\n") == 0)
					{
						strcpy(reply, "QUIT");
						if(writen(s, reply, n) != n)
							printf("Write error while replying\n");
						else
							printf("Reply sent\n");
					}		
					
				}	

				
			}
			
			
			
		}
           
		
	}
	

}

void err_fatal(char *mes) {
printf("%s, errno=%d\n", mes);
perror(""); exit(1);
}

