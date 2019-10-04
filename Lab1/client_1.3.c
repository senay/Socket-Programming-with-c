#include <stdlib.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <ctype.h>

#include  "mysocket.h"
//#include  <mysocket.h>
#include "errlib.h"
#include "sockwrap.h"

char *prog_name;

main(int argc, char* argv[] )
{

char buf[20];
char rbuf[20];
char input;
uint32_t taddr_n;
uint16_t tport_n, tport_h;
int s,i,j;
int result;
struct sockaddr_in saddr;
struct in_addr sIPaddr;

prog_name = argv[0];

//SockStartup();

if(argc != 3){
printf("Number of arguments not valid\n");
 exit(1);
}
result=inet_aton(argv[1],&sIPaddr);
if(!result){
    printf("socket() failed\n");
   exit(4);
}
if(sscanf(argv[2],"%" SCNu16,&tport_h)!=1){
  printf("invalid port number\n");
exit(2);
}
tport_n=htons(tport_h);

s=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
if(s<0){
   printf("socket() failed\n");
  exit(3);
}
printf("Done.Socket fd number: %d\n",s);

memset(&saddr,0,sizeof(saddr));
saddr.sin_family= AF_INET;
//saddr.sin_port= htons(2800);

saddr.sin_port= tport_n;
saddr.sin_addr= sIPaddr;

//showAddr("connecting to", &saddr);
result = connect(s,(struct sockaddr *) &saddr, sizeof(saddr));
if(result == -1)
{ 
   printf("connect() failed\n");
   exit(5);
}
printf("Done.\n");

printf("enter operand 1\n");
scanf("%d %d",&i,&j);
memset(buf,'\0',20);
sprintf(buf,"%d %d\r\n",i,j);
printf("%s\n",buf);
printf("%d\n",i);
printf("%d\n",j);
result=write(s,buf,strlen(buf));
if(result<0){ 
printf("send error");
exit(1);
}
memset(rbuf,'\0',20);
result=read(s,rbuf,10);
if(result<0)
{ 
printf("read error");
 exit(1);
}
printf("the sum is %s\n",rbuf);
closesocket(s);
exit(0);
}
