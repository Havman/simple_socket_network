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

#define SERVER_PORT 1235
#define MESSAGE_SIZE 32
#define BUFF_SIZE 1024 

int socket_fd;
char *filename;
int file_fd;
struct sockaddr_in addr;
socklen_t addrlen;

void error_exit(char *error) {
    char message[strlen(error) + 7];
    sprintf(message, "Error - %s\n", error);
    perror(message);
    exit(errno);
}

void handle_sigint() {
    close(socket_fd);
    printf("Server closen properly\n");
    exit(0);
}


void bind_socket() {
    if ((socket_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        error_exit("socket");
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(SERVER_PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    addrlen  = sizeof(addr);

    if (bind(socket_fd, (struct sockaddr*) &addr, sizeof(addr)) < 0) {
        error_exit("connect");
    }
}

void send_file(char *filename) {
    printf("Trying to send %s\n", filename);
    void *buf = calloc(BUFF_SIZE, sizeof(void));
    int file_fd = open(filename, O_RDONLY);
    if (file_fd < 0) {
        error_exit("open");
    }
    int readed;
    while (1) {
        printf(".");
        readed = read(file_fd, buf, BUFF_SIZE);
        sendto(socket_fd, buf, readed, 0, (struct sockaddr*)&addr, addrlen);
        if (readed <= 0) {
            break;
        }
    }
    printf(".\n");
    free(buf);
    close(file_fd);
    printf("Sending complete!\n");
}
 
int main() {
    bind_socket();
    signal(SIGINT, handle_sigint); 

    char message[MESSAGE_SIZE];
    while(1) {
        if (recvfrom(socket_fd, message, MESSAGE_SIZE, 0, (struct sockaddr*)&addr, &addrlen) < 0) {
            error_exit("reccvfrom");
        } 
        send_file(message);
    };

    handle_sigint();
    return 0;
}