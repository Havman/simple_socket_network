#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <strings.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define SERVER_PORT 80
#define MAXLINE 4096
#define SA struct sockaddr

int main(int argc, char **argv){
    int udp_socket;
    int sendbytes, n;
    struct sockaddr_in servaddr;
    char sendline[MAXLINE];
    char recvline[MAXLINE];
    if (argc != 2 )
        printf("%s", argv[0]);
        perror("Usage: <server address>");
    if( udp_socket = socket(AF_INET, SOCK_DGRAM, 0) < 0 )
        perror("socket error");

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERVER_PORT);

    if( inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <=0)
        perror("inet_pton error");

    if( connect(udp_socket, (SA*) &servaddr, sizeof(servaddr)) < 0)
        perror("connect error");

    sprintf(sendline, "GET / HTTP/1.1\r\n\r\n");
    sendbytes = strlen(sendline);

    if( write(udp_socket, sendline, sendbytes) != sendbytes)
        perror("write error");

    memset(recvline, 0, MAXLINE);

    while( (n = read(udp_socket, recvline, MAXLINE-1)) > 0){
        printf("%s", recvline);
    }
    if(n < 0)
        perror("read error");

    exit(0);
}