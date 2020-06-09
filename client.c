#include <sys/time.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define SERVER_PORT 1234
#define BUFF_SIZE 1024

int socket_fd;
void *buf;
int file_fd;

void error_exit(char *error) {
    char message[strlen(error) + 7];
    sprintf(message, "Error - %s", error);
    perror(message);
    exit(errno);
}

void handle_sigint() {
    free(buf);
    close(file_fd);
    close(socket_fd);
}
 
int main() {
    struct sockaddr_in servaddr;

    if ((socket_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        error_exit("socket");
    }

    const struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(SERVER_PORT),
        .sin_addr = {
            .s_addr = inet_addr("127.0.0.1")
        }
    };

    if (connect(socket_fd, (struct sockaddr*) &addr, sizeof(addr)) < 0)
        error_exit("connect error");

    write(socket_fd, "ball.gif", 8);

    buf = calloc(BUFF_SIZE, sizeof(void));
    
    char *filename = "ball.gif";
    file_fd = open(filename, O_WRONLY);

    int readed = read(file_fd, buf, BUFF_SIZE);
    while (readed > 0) {
        write(socket_fd, buf, BUFF_SIZE);
        readed = read(file_fd, buf, BUFF_SIZE);
    }

    sprintf(buf, "end");
    write(socket_fd, buf, 3);

    handle_sigint();
}