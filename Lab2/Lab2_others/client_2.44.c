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
#include <rpc/xdr.h>

#include  "mysocket.h"
//#include  <mysocket.h>
#include "errlib.h"
#include "sockwrap.h"

#ifdef TRACE
#define trace(x) x
#else
#define trace(x)
#endif

char *prog_name;

main(int argc, char* argv[] )
{

char buf[20];
char rbuf[20];
char input;
uint32_t taddr_n;
uint16_t tport_n, tport_h;
int s,i,j,size;

int result, sum;
struct sockaddr_in saddr;
struct in_addr sIPaddr;
XDR xdrc_r, xdrc_w;

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

        FILE *stream_socket_r = fdopen(s, "r");
	if (stream_socket_r == NULL)
		err_sys ("(%s) error - fdopen() failed", prog_name);
	xdrstdio_create(&xdrc_r, stream_socket_r, XDR_DECODE);

	FILE *stream_socket_w = fdopen(s, "w");
	if (stream_socket_w == NULL)
		err_sys ("(%s) error - fdopen() failed", prog_name);
	xdrstdio_create(&xdrc_w, stream_socket_w, XDR_ENCODE);

	trace( err_msg("(%s) - waiting for operands ...", prog_name) );

        /* send the numbers */
	xdr_int(&xdrc_w, &i);
        xdr_int(&xdrc_w, &j);
	fflush(stream_socket_w);

	trace( err_msg("(%s) --- operands just sent", prog_name) );

	/* get the result */
	if ( ! xdr_int(&xdrc_r, &sum) ) {
		trace( err_msg("(%s) - cannot read sum with XDR", prog_name) );		
	} else {
		trace( err_msg("(%s) - read sum = %d", prog_name, sum) );
	}

	
	

printf("the sum is %s\n",sum);
closesocket(s);
exit(0);
}
