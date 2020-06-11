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
#define BUFF_SIZE 1000

int socket_fd;
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

// Returns number of digits + 1, because protocol contains ':'
int get_count(int num) {
    int count = 0;
    do {
        count++;
        num /= 10;
    } while(num != 0);
    return count+1;
}

int create_and_send_packet(int num, void *buf, int file_fd) {
    int count = get_count(num);
    void *number = malloc(count);
    sprintf(number, "%d:", num);

    // Read data from file
    int readed = read(file_fd, buf+count, BUFF_SIZE-count);
    if (readed < 0) {
        error_exit("read-file");
    }
    strncpy(buf,number, count);

    // If readed is less than 0 it means there was an error       
    if (sendto(socket_fd, buf, readed+count, 0, (struct sockaddr*)&addr, addrlen) < 0) {
        error_exit("sento-socket");
    }
    free(number);
    return readed+count;
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
    int file_fd = open(filename, O_RDONLY);
    if (file_fd < 0) {
        error_exit("open");
    }
    int readed;
    void* buf = malloc(BUFF_SIZE);
    for (int iterator = 1;;iterator++) {
        bzero(buf, BUFF_SIZE);

        // Reading file, create and sending package 
        readed = create_and_send_packet(iterator, buf, file_fd);
        printf("Sending package nr: %d - size: %d\n", iterator, readed);

        // Get from client respond that he received data package
        char respond[MESSAGE_SIZE];
        if (recvfrom(socket_fd, &respond, MESSAGE_SIZE, 0, (struct sockaddr*)&addr, &addrlen) < 0) {
            error_exit("reccvfrom-respond");
        }

        // Check if the message was confirment
        if (strcmp(respond, "confirment") != 0) {
            printf("Client does not confirmed receiving package.");
            break;
        }

        // If readed is less than BUFF_SIZE, it means, that it's end of file
        if (readed < BUFF_SIZE) break;
    }
    free(buf);
    close(file_fd);
    printf("Sending complete!\n");
}
 
int main() {
    bind_socket();
    signal(SIGINT, handle_sigint); 

    char message[MESSAGE_SIZE];
    while(1) {
        // Receive from client name of the file
        if (recvfrom(socket_fd, message, MESSAGE_SIZE, 0, (struct sockaddr*)&addr, &addrlen) < 0) {
            error_exit("reccvfrom");
        } 
        // Start sending procedure
        send_file(message);
    };

    handle_sigint();
    return 0;
}