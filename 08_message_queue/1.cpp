#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
// #include <string.h>

#include <readline/readline.h>

#define MSG_QUEUE_UNIQ_KEY 72136
#define MSG_TEXT_MAX 1024

struct my_msgbuf {
        long mtype;
        char mtext[MSG_TEXT_MAX];
};

void print_usage(char* argv0) {
    printf("Usage:\n");
    printf("\t%s c        - Create message queue\n", argv0);
    printf("\t%s s <type> - Send message \n", argv0);
    printf("\t%s r <type> - Receive message\n", argv0);
    printf("\t%s p <type> - Peek message\n", argv0);
    printf("\t%s d        - Delete message queue\n", argv0);
}

int mode_create() {
    int fd = msgget(MSG_QUEUE_UNIQ_KEY, IPC_CREAT | IPC_EXCL | 0644);
    if(fd < 0) {
        printf("msgget() error on create\n");
        return 2;
    }
    printf("message queue created\n");
    return 0;
}

int mode_delete(int fd) {
    msgctl(fd, IPC_RMID, 0);
    printf("message queue deleted\n");
    return 0;
}

int main(int argc, char* argv[]) {
    if(argc < 2) {
        print_usage(argv[0]);

        return 1;
    }
    char mode = argv[1][0];

    if(mode == 'c') {
        return mode_create();
    }

    int fd = msgget(MSG_QUEUE_UNIQ_KEY, 0644);
    if(fd < 0) {
        printf("msgget() error on open\n");
        return 3;
    }

    if(mode == 'd') {
        return mode_delete(fd);
    }

    if(mode != 's' && mode != 'r' && mode != 'p') {
        printf("Unknown mode: %c\n", mode);
        print_usage(argv[0]);
        return 1;
    }

    if(argc < 3) {
        printf("Message type required\n");
        print_usage(argv[0]);
        return 1;
    }
    int type = atoi(argv[2]);

    printf("TODO\n");
    return 33;

    return 0;
}