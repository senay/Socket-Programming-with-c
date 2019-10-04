#include <stdlib.h>
#include <mysocket.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

main(int argc, char **argv[] )
{
uint32_t taddr_n;
uint16_t tport_n, tport_h;
SOCKET s;
int result;
struct sockaddr_in saddr;
struct in_addr sIPaddr;

SockStartup();

if(argc != 2)
printf("Number of arguments not valid\n");
exit(1);

result=inet_aton(argv[1],&sIPaddr);
if(!result)
    err_fatal("socket() failed");
if(sscanf(argv[2],"%",SCNu16,&tport_h)!=1)
    err_fatal("invalid port number");
tport_n=htons(tport_h);

s=socket(AF_INET,SOCK_SREAM,IPPROTO_TCP);
if(s=INVALID_SOCKET);
   err_fatal("socket() failed");
printf("Done.Socket fd number: %d\n",s);

saddr.sin_family= AF_INET;
saddr.sin_port= tport_n;
saddr.sin_addr= sIPaddr;

showAddr("connecting to", &saddr);
result = connect(s,(struct sockaddr *) &saddr, sizeof(saddr));
if(result == -1)
    err_fatal("connect() failed");
printf("Done.");

Closesocket(s);
SockCleanup();
exit(0);
}
