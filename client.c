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

#define IP_ADDRESS "127.0.0.1"
#define SERVER_PORT 1235
#define MESSAGE_SIZE 32
#define BUFF_SIZE 1000

int socket_fd;
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
    close(file_fd);
    close(socket_fd);
}

int get_count(int num) {
    int count = 0;
    do {
        count++;
        num /= 10;
    } while(num != 0);
    return count+1;
}

void connect_socket() {
    if ((socket_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        error_exit("socket");
    }
    addrlen  = sizeof(addr);
    addr.sin_family = AF_INET; 
    addr.sin_port = htons(SERVER_PORT); 
    addr.sin_addr.s_addr = inet_addr(IP_ADDRESS); 
}

void receive_file(char *newfilename) {
    printf("Creating file %s\n", newfilename);
    int file_fd = open(newfilename, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (file_fd < 0) {
        error_exit("open");
    }
    int readed;
    int count;
    char* end; // use for strtol
    void *buf = malloc(BUFF_SIZE);

    for (int package_number = 1;;package_number++) {
        
        bzero(buf, BUFF_SIZE);

        // Receive data from server
        readed = recvfrom(socket_fd, buf, BUFF_SIZE, 0, (struct sockaddr*)&addr, &addrlen); 

        // If readed is less than 0 it means there was an error
        if (readed < 0) {
            error_exit("recvfrom");
        }

        // Get package number from data where protocol is <number>:<message>
        package_number = strtol(buf, &end, 0);
        count = get_count(package_number);

        printf("Received package nr: %d\n", package_number);

        // Write to file received data
        if (write(file_fd, buf+count, readed-count) < 0) {
            error_exit("write-file");
        }

        // Send to sever confirment that package was received
        if (sendto(socket_fd, "confirment", MESSAGE_SIZE, 0, (struct sockaddr*)&addr, addrlen) < 0) {
            error_exit("sendto-respond");
        }

        // If readed is less than BUFF_SIZE, but greater or equal to 0 it means, that it's end of file
        if (readed < BUFF_SIZE) break;
    }
    printf("File downloaded properly\n");
    free(buf);
}
 
int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Need filename, and new filename");
        return -1;
    }
    signal(SIGINT, handle_sigint);
    connect_socket();
    if (sendto(socket_fd, argv[1], MESSAGE_SIZE, 0, (struct sockaddr*)&addr, addrlen) < 0) {
        error_exit("sendto");
    }
    receive_file(argv[2]);
    handle_sigint();
}