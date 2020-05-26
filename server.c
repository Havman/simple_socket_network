#include "shared.h"

int socket_fd;
char *filename = "dab.gif";
 
int main() {
    int file = open(filename, O_RDONLY);
    struct sockaddr_in servaddr;

    if ((socket_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        error_exit("socket");
    }

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERVER_PORT);

    if (connect(socket_fd, (struct sockaddr*) &servaddr, sizeof(servaddr)) < 0)
        perror("connect error");

    void *buf = calloc(BUFF_SIZE, sizeof(void));

    int readed = read(file, buf, BUFF_SIZE);
    while (readed > 0) {
        readed = read(file, buf, BUFF_SIZE);
        send_message(socket_fd, BUFF_SIZE, buf);
    }
}