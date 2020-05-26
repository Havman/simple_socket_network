#include "shared.h"

int socket_fd;
char *filename = "copy-dab.gif";
 
int main() {
    int file = open(filename, O_WRONLY | O_CREAT);
    if (file < 0) {
        error_exit("open");
    }

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

    int readed = receive_message(socket_fd, buf);
    while (readed > 0) {
        if (write(file, buf, BUFF_SIZE) == -1) {
            error_exit("write-file");
        }
        receive_message(socket_fd, buf);
    }
}
