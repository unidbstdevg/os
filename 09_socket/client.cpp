#include "shared.h"
#include <unistd.h>

#include <readline/readline.h>

#define PROMPT "> "
#define RESPONSE_PROMPT "> "

int main() {
    int sfd = create_socket();
    if(connect(sfd, (struct sockaddr*)&sockname, sizeof(sockname)) < 0) {
        perror("connect");
        exit(1);
    }
    printf("Connected to %s\n", FILE_SOCKET_PATH);

    while(char* line = readline(PROMPT)) {
        if(send(sfd, line, strlen(line) + 1, 0) <= 0) {
            perror("send");
            return 1;
        }

        char buf[MAXBUF];
        if(recv(sfd, &buf, MAXBUF, 0) < 0) {
            perror("recv");
            exit(-1);
        }
        printf("%s%s\n", RESPONSE_PROMPT, buf);

        // free readline string
        free(line);
    }

    return 0;
}
