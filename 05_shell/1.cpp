// shold be before readline include
#include <stdio.h>

#include <readline/readline.h>

#include <readline/history.h>

#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

/*
+ 1. Сделать бесконечный цикл запросов строк на ввод. Придумать собственное
+    слово для выхода (например, “by”). Когда пользователь вводит это слово —
+    цикл заканчивается, программа завершается.
+ 2. Все остальные строки, введенные пользователем на каждой итерации цикла,
+    считаются предложениями и подлежат разбору на отдельные слова (команда + ее
+    параметры через пробел). Для разбора использовать функцию strtok().
+ 3. Пример такого разбора: из строки "ls -l -a" должен получиться такой массив
+    строк вида:
+    *cut*
+    Проследите, что последний элемент массива строк равен null, т.к. системный
+    вызов exec не знает размер передаваемого массива и считывает его до первого
+    попавшегося null-а.
+ 4. Когда разбор строки на слова закончен и протестирован (!), создаем дочерний
+    процесс — fork() и заменяем его код кодом той команды, которую ввел
+    пользователь.
+ 5. Если команда не выполнилась (exec закончился неудачей) — выдать сообщение
+    об ошибке, что команда не распознана.
*/

#define EXIT_WORD "exit"
#define PROMPT "box_to_process_commands> "
#define MAX_ARGS_COUNT 5

void catch_sigint(int sig_num) {
    printf("\nType 'exit' to exit\n");

    // Regenerate the prompt on a newline
    rl_on_new_line();
    // Clear the previous text
    rl_replace_line("", 0);

    rl_redisplay();
}

int main() {
    signal(SIGINT, catch_sigint);

    while(true) {
        char* line = readline(PROMPT);
        if(!line || !strcmp(line, EXIT_WORD)) {
            printf("Bye!\n");
            break;
        }

        if(strlen(line) == 0)
            continue;

        add_history(line);

        char* argv[MAX_ARGS_COUNT];
        int i = 0;
        argv[i++] = strtok(line, " ");
        while((argv[i] = strtok(NULL, " "))) {
            i++;
            if(i == (MAX_ARGS_COUNT - 1)) {
                printf(":arguments limit reached. Tail cut\n");
                break;
            }
        }
        argv[i] = NULL;

        if(!fork()) {
            // child
            execvp(argv[0], argv);

            // all next is not reachable if execvp is ok

            printf("Error: Command not found: %s\n", argv[0]);

            return 0;
        } else {
            // main
        }

        // wait for child
        wait(0);

        // readline malloc'ed it
        free(line);
    }

    return 0;
}
