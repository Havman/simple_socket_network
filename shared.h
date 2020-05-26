#ifndef SHARED_H
#define SHARED_H

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

void send_message(int, ssize_t, void*);
ssize_t receive_message(int, void*);
void error_exit(char *);

#endif