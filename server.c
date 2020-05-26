#include "shared.h"

#define SERVER_PORT 1234
#define BUFF_SIZE 1024

int socket_fd;
char *filename = "dab.gif";
 
int main() {
    int file = open(filename, O_RDONLY);
    char *buf = (char *) calloc(BUFF_SIZE, sizeof(char));
    
    int readed = read(fd_in, buf, BUFF_SIZE);
    while (readed > 0) {
        //printf("%s\n\n", buf);
        if (write(fd_out, buf, readed) < 0) {
            perror("Error while writing");
        }

        readed = read(fd_in, buf, BUFF_SIZE);
    }
}