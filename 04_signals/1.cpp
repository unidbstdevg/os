#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

/*
+ 1. Создать 3 функции-обработчики сигналов: реакцию на завершение главного
+    процесса, реакцию на завершение дочернего процесса (для него самого и для
+    его родителя).
+ 2. Сменить диспозицию сигналов SIGINT, SIGCHLD и SIGKILL, завязав их на
+    соответствующие функции.
+ 3. Запустить дочерний процесс, вывести на экран PID родительского и дочернего
+    процессов, поставить процессы на паузу — pause().
*/
void catch_sigint(int sig_num) { printf("sig int (pid=%i)\n", getpid()); }
void catch_sigchld(int sig_num) { printf("sig chld (pid=%i)\n", getpid()); }
void catch_sigkill(int sig_num) {
    printf("sig kill (pid=%i). P.S. it will never work. It doesnt possible to "
           "catch sigkill\n",
           getpid());
}

int main() {
    signal(SIGINT, catch_sigint);
    signal(SIGCHLD, catch_sigchld);
    signal(SIGKILL, catch_sigkill);

    printf("main (pid=%i) started\n", getpid());

    if(!fork()) {
        // child
        printf("child (pid=%i) started\n", getpid());

        pause();

        printf("child (pid=%i) finished\n", getpid());

        return 0;
    } else {
        // main
    }

    pause();

    printf("main (pid=%i) finished\n", getpid());

    return 0;
}
