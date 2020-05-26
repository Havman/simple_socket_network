#include "shared.h"

void send_message(int sockfd, ssize_t size, void *data) {
    if(size > 0) {
        write(sockfd, data, size);
    }
}

ssize_t receive_message(int sockfd, void *data) {
    ssize_t size = 0;
    if(data != NULL) {
        void *pointer = malloc(size);
        for(int i = 0; i < size; i++) {
            if(read(sockfd, pointer + i, 1) < 1) return -1;
        }
        data = pointer;
    } else {
        printf("Data cannot be null");
    }
    return size;
}

void error_exit(char *error) {
    char message[strlen(error) + 7];
    sprintf(message, "Error - %s", error);
    perror(message);
    exit(errno);
}