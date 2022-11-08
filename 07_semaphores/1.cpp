#include <stdio.h>
#include <stdlib.h>

// #include <fcntl.h>
// #include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#include <sys/ipc.h>
#include <sys/shm.h>

void print_usage(char* argv0) {
    printf("Usage:\n");
    printf("\t%s N s - Use semaphore\n", argv0);
    printf("\t%s N w - Do not use semaphore\n", argv0);
}

void init_arr(int* arr, int val, int N) {
    for(int* arr_end = arr + N; arr < arr_end; arr++) {
        *arr = val;
    }
}

void print_arr(int* arr, int N) {
    printf("array: ");
    for(int* arr_end = arr + N; arr < arr_end; arr++) {
        printf("%i ", *arr);
    }
    printf("\n");
}

int g_shm_fd = -1;

void catch_sigint(int code) {
    if(g_shm_fd != -1) {
        shmctl(g_shm_fd, IPC_RMID, 0);
    }
    exit(code);
}

void mode_w(int N) {
    g_shm_fd = shmget(IPC_PRIVATE, N * sizeof(int) + 1, IPC_CREAT | 0644);
    if(g_shm_fd < 0) {
        printf("shmget() error on create\n");
        exit(2);
    }
    signal(SIGINT, catch_sigint);

    void* addr = shmat(g_shm_fd, 0, 0);
    int* cur_id = (int*)addr;
    int* arr = (int*)addr + 1;

    *cur_id = 0;
    init_arr(arr, -1, N);
    print_arr(arr, N);

    // first child
    if(!fork()) {
        int p1_count = 0;

        while(*cur_id < N) {
            p1_count++;
            if(*cur_id % 2 == 0) {
                arr[*cur_id] = *cur_id;
                (*cur_id)++;
            }
        }

        printf("p1 count: %i\n", p1_count);
        exit(0);
    }

    // second child
    if(!fork()) {
        int p2_count = 0;
        while(*cur_id < N) {
            p2_count++;
            if(*cur_id % 2 == 1) {
                arr[*cur_id] = *cur_id;
                (*cur_id)++;
            }
        }

        printf("p2 count: %i\n", p2_count);
        exit(0);
    }

    // wait for childs
    for(int i = 0; i < 2; i++) {
        wait(0);
    }

    print_arr(arr, N);

    shmdt(addr);
    shmctl(g_shm_fd, IPC_RMID, 0);
}

void mode_s(int N) {
    // TODO:
    printf("TODO: not yet implemented\n");
}

int main(int argc, char* argv[]) {
    if(argc < 3) {
        print_usage(argv[0]);
        return 1;
    }

    // will be zero if not a number
    int N = atoi(argv[1]);
    if(N == 0) {
        printf("Key should be non-zero number\n");
        return 1;
    }
    char mode = argv[2][0];

    switch(mode) {
    case 'w':
        mode_w(N);
        break;
    case 's':
        mode_s(N);
        break;
    default:
        printf("Unknown mode: %c\n", mode);
        return 1;
    }

    return 0;
}
