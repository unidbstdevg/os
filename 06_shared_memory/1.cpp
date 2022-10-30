#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/ipc.h>
#include <sys/shm.h>

void print_usage(char* argv0) {
    printf("Usage:\n");
    printf("\t%s Key c     - Create Shared memory\n", argv0);
    printf("\t%s Key s Msg - Send message to Shared memory\n", argv0);
    printf("\t%s Key r     - Read message from Shared memory\n", argv0);
    printf("\t%s Key d     - Remove Shared memory\n", argv0);
}

int main(int argc, char* argv[]) {
    if(argc < 3) {
        print_usage(argv[0]);

        return 1;
    }

    // will be zero if not a number
    int key = atoi(argv[1]);
    if(key == 0) {
        printf("Key should be non-zero number\n");
        return 1;
    }
    char mode = argv[2][0];

    int fd = -1;
    if(mode == 'c') {
        fd = shmget(key, 1024, IPC_CREAT | 0644);
        if(fd < 0) {
            printf("shmget() error on create\n");
            return 2;
        }
        printf("shm created\n");
        return 0;
    } else {
        fd = shmget(key, 1024, 0644);
        if(fd < 0) {
            printf("shmget() error on open\n");
            return 3;
        }
    }

    char* addr = NULL;

    switch(mode) {
    // handled earlier, look up
    // case 'c':
    //     break;
    case 's':
        if(argc < 4) {
            print_usage(argv[0]);
            return 1;
        }

        addr = (char*)shmat(fd, 0, 0);
        strcpy(addr, argv[3]);
        shmdt((char*)addr);
        break;
    case 'r':
        addr = (char*)shmat(fd, 0, 0);
        printf("shm content: \"%s\"\n", addr);
        shmdt((char*)addr);
        break;
    case 'd':
        shmctl(fd, IPC_RMID, 0);
        printf("shm deleted\n");
        break;
    default:
        printf("Unknown mode: %c\n", mode);
        return 1;
    }

    return 0;
}
