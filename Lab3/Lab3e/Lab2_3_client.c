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
		
		fgets(command, sizeof(command), stdin);
		len=strlen(command);
		for(i=0, j=0; i<len; i++){
			if(command[i] != 'G' && command[i] != 'E' && command[i] != 'T' && command[i] != '\n'){
				filename[j] = command[i];
				j++;
			}
		}
		// strncpy(filename, command + 3, strlen(command) - 4);
		filename[j]='\0';
		
		if(writen(s, command, len) != len){
			printf("Write error\n");
			break;
		}

                printf("%s\n",filename);
		
		printf("Waiting for response...\n");

		result = recv(s, reply, REPLYLEN, 0);

			

		if((strcmp(reply, "QUIT") == 0)) {
			printf("Communication channel closed\n");
			printf("=========================================\n");
			break;
		}
		else if((strcmp(reply, "-ERR\n") == 0)){
			printf("nonCommunication channel closed\n");
                        
			printf("=========================================\n");
			break;
		}
	
		else{
			printf("%s\n", reply);	
			for(i=0, j=0; i<strlen(command); i++){
				if(reply[i] != 'O' && reply[i] != 'K' && reply[i] != '+' && reply[i] != '\n'){
					size[j] = reply[i];
					j++;
				}
			}
			size[j]='\0';
			printf("%s\n", size);		
			printf("File transfer performed: filename: %s size: %s\n", filename, size);
			size2 = atoi(size);
			printf("Received file of size %d Byte\n", size2);
		    result = recv(s, reply, REPLYLEN, 0);
		    if(result)
				printf("%s", reply);
			else
			{
				printf("Error while receiving\n");
				break;
			}
			printf("\nFile transmission completed\n");
			    
		}
		if(result <= 0){
			printf("Read error/Connection closed\n");
			closesocket(s);
			//SockCleanup();
			exit(1);
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

