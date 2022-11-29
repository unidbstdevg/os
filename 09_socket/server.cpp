#include "shared.h"
#include <algorithm>
#include <random>
#include <string.h>
#include <string>
#include <unistd.h>

int main() {
    int sfd = create_socket();

    unlink(FILE_SOCKET_PATH);
    if(bind(sfd, (struct sockaddr*)&sockname, sizeof(sockname)) < 0) {
        perror("bind");
        exit(1);
    }
    printf("Bounded to %s\n", FILE_SOCKET_PATH);

    if(listen(sfd, 5) < 0) {
        perror("listen");
        exit(1);
    }
    printf("Listening...\n");

    while(true) {
        int nsfd = accept(sfd, NULL, 0);
        if(nsfd < 0) {
            continue;
        }

        printf("New client connected. New socket id: %i\n", nsfd);

        if(!fork()) {
            // close parent socket
            close(sfd);

            while(true) {
                char buf[MAXBUF];
                if(recv(nsfd, &buf, MAXBUF, 0) < 0) {
                    perror("recv");
                    return 1;
                }
                printf("Client %i sayed: %s\n", nsfd, buf);

                std::string mess(buf);

                std::random_device rd;
                std::mt19937 g(rd());

                std::shuffle(mess.begin(), mess.end(), g);

                char resp[MAXBUF] = "It's very noisy in here, did you say: \"";
                strncat(resp, mess.data(), MAXBUF);
                strncat(resp, "\"?", MAXBUF);
                if(send(nsfd, resp, strlen(resp) + 1, 0) <= 0) {
                    perror("send");
                    return 1;
                }
            }
            return 0;
        }

        // TODO: shoud we?
        close(nsfd);
    }

    return 0;
}
