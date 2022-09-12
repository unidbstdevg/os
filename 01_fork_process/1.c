#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/*
+ 1. Взять с командной строки количество создаваемых процессов.
+ 2. Запустить цикл (от 0 до N), в котором вызвать функцию fork().
+ 3. Дочерние процессы должны доложиться о себе и заснуть на короткое время —
+    sleep(5). После пробуждения доложиться о том, что проснулись.
+ 4. И родительский, и дочерние процессы должны выводить свои PID и PPID —
+    getpid() и getppid() соответственно.
+ 5. Дочерние процессы не пускать (!!!) на новую итерацию цикла.
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

            sleep(5);

            pid = getpid();
            ppid = getppid();
            printf("child (pid=%i, ppid=%i) finished\n", pid, ppid);

            return 0;
        } else {
            // main
        }
    }

    printf("main (pid=%i, ppid=%i) finished\n", pid, ppid);

    return 0;
}
