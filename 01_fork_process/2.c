#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

/*
+ 1. То же, что и первая часть задания.
+ 2. После порождающего цикла сделать цикл (от 0 до N) ожидания дочерних
+    процессов — wait(0).
*/

int main(int argc, char* argv[]) {
    if(argc < 2) {
        printf("Usage: %s {number}\n", argv[0]);
        printf("Where {number} is count of child processes\n");

        return 1;
    }

    // will be zero if not a number
    int N = atoi(argv[1]);

    int pid = getpid();
    int ppid = getppid();
    printf("main (pid=%i, ppid=%i) started\n", pid, ppid);

    for(int i = 0; i < N; i++) {
        if(!fork()) {
            // child
            int pid = getpid();
            int ppid = getppid();

            printf("child (pid=%i, ppid=%i) started\n", pid, ppid);
            sleep(2);
            printf("child (pid=%i, ppid=%i) finished\n", pid, ppid);

            return 0;
        } else {
            // main
        }
    }

    for(int i = 0; i < N; i++) {
        wait(0);
    }

    printf("main (pid=%i, ppid=%i) finished\n", pid, ppid);

    return 0;
}
