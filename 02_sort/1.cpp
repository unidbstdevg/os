#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h>
#include <sys/wait.h>
#include <time.h>
using namespace std;

/*
+ 1. Взять с консоли длину массива.
+ 2. Создать массив указанной длины, заполнить его псевдослучайными числами —
+    rand(), записать его в файл — open(), dprintf(), close().
+ 3. Запустить 3 дочерних процесса, по одному на каждую сортировку.
+ 4. В коде дочерних процессов:
+   1) вызвать функцию сортировки для созданного массива;
+   2) записать в файл уже отсортированный массив;
+   3) вызвать функцию times(), передав ей структурную переменную типа tms;
+   4) из полей структуры взять tms_utime и tms_stime, сложить их и вывести
+      время работы процесса.
+ 5. Родительский процесс должен подождать все дочерние процессы.
*/

int main(int argc, char* argv[]) {
    if(argc < 2) {
        printf("Usage: %s {number}\n", argv[0]);
        printf("Where {number} is array length\n");

        return 1;
    }

    // will be zero if not a number
    int N = atoi(argv[1]);
    if(N == 0) {
        printf("number should be > 0\n");
        return 2;
    }

    // random seed
    srand(time(0));

    printf("generate array\n");
    int* ar = new int[N];
    for(int i = 0; i < N; i++) {
        ar[i] = rand();
    }

    save2file((char*)"out_01_main.txt", ar, N);

    tms mytms;

    printf("sort\n");
    // sort
    if(!fork()) {
        // quick
        printf("\tquick start\n");
        quicks(ar, N);
        save2file((char*)"out_02_quick.txt", ar, N);

        times(&mytms);
        long t = mytms.tms_utime + mytms.tms_stime;
        printf("\tquick done. tms: %lu\n", t);

        return 0;
    }
    if(!fork()) {
        // shell
        printf("\tshell start\n");
        shell(ar, N);
        save2file((char*)"out_03_shell.txt", ar, N);

        times(&mytms);
        long t = mytms.tms_utime + mytms.tms_stime;
        printf("\tshell done. tms: %lu\n", t);

        return 0;
    }
    if(!fork()) {
        // bubble
        printf("\tbubble start\n");
        bubble(ar, N);
        save2file((char*)"out_04_bubble.txt", ar, N);

        times(&mytms);
        long t = mytms.tms_utime + mytms.tms_stime;
        printf("\tbubble done. tms: %lu\n", t);

        return 0;
    }

    printf("waiting for childs\n");
    // wait for childs
    for(int i = 0; i < 3; i++) {
        wait(0);
    }
    printf("all done\n");

    delete[] ar;
    return 0;
}
