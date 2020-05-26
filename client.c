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
char *filename = "cpy-ball.gif";

void error_exit(char *error) {
    char message[strlen(error) + 7];
    sprintf(message, "Error - %s", error);
    perror(message);
    exit(errno);
}
 
int main() {
    printf("Client starting...");
    int file = open(filename, O_WRONLY | O_CREAT);
    if (file < 0) {
        error_exit("open");
    }

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

    if (bind(socket_fd, (struct sockaddr*) &addr, sizeof(addr)) < 0) {
        error_exit("connect");
    }

    void *buf = calloc(BUFF_SIZE, sizeof(void));

    int readed = read(socket_fd, buf, BUFF_SIZE);
    while (readed > 0) {
        printf(".");
        if (write(file, buf, BUFF_SIZE) < 0) {
            error_exit("write-file");
        };
        readed = read(socket_fd, buf, BUFF_SIZE);
    }

    free(buf);
    close(file);
    close(socket_fd);
}
