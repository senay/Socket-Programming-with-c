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

#define RCOMMANDLEN 128
#define REPLYLEN     50
#define FILELEN	    132

char *prog_name;
int  childCounter;
int NumberOfProc;
char command[RCOMMANDLEN];	/*reception buffer*/
void sig_chld(int signo);
void service(int s);
void pool_proc(int conn_request_skt);

int main(int argc, char *argv[]){

	int			conn_request_skt;	/*socket where connections are accepted*/
	
	//char			filename[FILELEN];
	//char			reply[REPLYLEN];
	//char			file[FILELEN];
        int			pidSet[NumberOfProc];
	uint16_t		lport_n, lport_h;	/*port where the server listens(net/host byte ord resp.)*/
	int			bklog = 2; 		/*listening backlog*/
	int			s;
	int 			result, addrlen, n;
	struct sockaddr_in	saddr, caddr;		/*server and client address structures*/
	short 			port;
	int 			i;
	//FILE			*fp;
	//long			size;
	int                     status;
        pid_t                   childpid;
	


	/*check arguments*/
	if(argc != 3)
	{
		err_fatal("usage: <port> <children_number>\n");
		return;
	}
	
	port=atoi(argv[1]);	
	printf("%d\n", port);
	//NumberOfProc=argv[2]; 			// ERROR!!!!!!!! argv[2] is a string (char*), NOT an int!
	NumberOfProc = strtol (argv[2], NULL, 0);	// You must convert it by means of a proper function (like strtol, atoi, ecc...)


        //int nleft; ssize_t nwritten;
	printf("Creating socket\n");
	s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(s < 0)
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

	/* create processes */
	for (i=0; i<NumberOfProc; i++)
	{
	    if((childpid=fork())<0)
		err_fatal("fork() failed");
            
	    else if (childpid == 0)
	    {
		/* child process */
		pool_proc(conn_request_skt);	/* enter service loop */
	    }
            else if(childpid > 0)
               pidSet[i]=childpid;
	}
	printf("Process pool created\n");
	for(i=0; i<NumberOfProc; i++)
		  waitpid(pidSet[i], NULL, 0);
	
	
}


void
sig_chld(int signo)
{
    pid_t   pid;
    int     stat;    
    while ( (pid = waitpid(-1, &stat, WNOHANG)) > 0) {
        printf("child %d terminated\n", pid);
        childCounter--;
    }
    return;
}

/* Executes the service loop */
void pool_proc(int conn_request_skt)
{
	int		        s;
	int			addrlen;
	struct sockaddr_in	c_addr;
	
	/* main loop */
	for (;;)
	{
	    /* accept new connection */
	    addrlen = sizeof(struct sockaddr_in);
	    s = accept(conn_request_skt, (struct sockaddr *) &c_addr, &addrlen);
	    if (s < 0)
	    	err_fatal("accept() failed");
	    //showAddr("Accepted connection from", &c_addr);
	    service(s);			/* serve client */
	}
}

void service(int s){
	char			filename[FILELEN];  
	char			file[FILELEN];  
	char			reply[REPLYLEN];      
	int                     n;
	int 			i,j;
	FILE			*fp;
	int nleft; ssize_t nwritten;	
	int			size;
        for(;;){
		 memset(command,0,RCOMMANDLEN);              
		n=recv(s, command, RCOMMANDLEN-1, 0);
		if(n < 0){
			printf("Read error\n");
			close(s);
			printf("Socket %d closed\n", s);
			break;
		}
		else if(n == 0){
			printf("Connection closed by party on socket %d\n", s);
			close(s);
			break;
		}
		else{
			printf("Received line from socket %03d: \n", s);
			if(strncmp(command, "GET", 3) == 0)
				{
				    for(i=3, j=0; i<strlen(command); i++){
					
							filename[j] = command[i];
							j++;
						
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
					printf("Send file content %d\n", (int)size);
					if((fp=fopen(file, "rt")) != NULL){
						for(nleft=size;nleft>0;)
						  { 
							 fread(reply, sizeof(reply),1,fp);
							 nwritten=send(s,reply,sizeof(reply),0);
				
							 if(nwritten>0)
							  {
								nleft -=nwritten;
							  }
							else if(nwritten<=0)
								break;
						  }
						  printf("End of transfer\n");
						  printf("===========================================\n");
						  fclose (fp);
				
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
	                


                        else if(strcmp(command, "QUIT\n") == 0){
                                        printf("Client requested to quit!\n");
					close(s);
			}
                        else{
				printf("The requested command is invalid!!!\n");
				strcpy(reply, "-ERR\n");
				if(writen(s, reply, n) != n)
					printf("Write error while replying\n");
				else
					printf("Reply sent\n");
                            
			}

		
		}
	
	}
}

