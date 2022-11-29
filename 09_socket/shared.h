// # include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>

int create_socket() {
    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if(fd < 0) {
        perror("socket");
        exit(1);
    }
    printf("Socket id: %i\n", fd);

    return fd;
}

#define FILE_SOCKET_PATH "./file.socket"
sockaddr_un sockname = {AF_UNIX, FILE_SOCKET_PATH};

#define MAXBUF 1000
