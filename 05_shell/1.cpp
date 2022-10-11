// shold be before readline include
#include <stdio.h>

#include <readline/readline.h>

#include <readline/history.h>

#include <fcntl.h>
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
#define CD_WORD "cd"
#define REDIRECT_WORD ">"
#define PROMPT "box_to_process_commands> "
#define MAX_ARGS_COUNT 512

void catch_sigint(int sig_num) {
    printf("\nType 'exit' or press C-d to exit\n");

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

        char* linev[MAX_ARGS_COUNT];
        int linec = 0;
        linev[linec++] = strtok(line, " ");
        int redirectsymboli = 0;
        while((linev[linec] = strtok(NULL, " "))) {
            if(!redirectsymboli && linec > 0 &&
               !strcmp(linev[linec], REDIRECT_WORD)) {
                redirectsymboli = linec;
                linev[linec] = NULL;
            }

            linec++;
            if(linec == (MAX_ARGS_COUNT - 1)) {
                printf("Warning: arguments limit reached. Tail cut\n");
                break;
            }
        }
        linev[linec] = NULL;

        if(redirectsymboli && redirectsymboli + 1 >= linec) {
            printf("Error: no target to redirect to\n");
            continue;
        }

        if(linec > 0 && !strcmp(linev[0], CD_WORD)) {
            if(linec == 1) {
                printf("Usage: %s {path}\n", CD_WORD);
                continue;
            }
            if(chdir(linev[1])) {
                printf("Error: no such path: %s\n", linev[1]);
            }
            continue;
        }

        if(!fork()) {
            // child

            if(redirectsymboli) {
                char* filename = linev[redirectsymboli + 1];
                printf("redir %s\n", filename);
                int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0600);
                dup2(fd, 1); /*дублируем дескриптор (на стандартный вывод)*/
                close(fd); /*старый дескриптор больше не нужен*/
            }
            execvp(linev[0], linev);

            // if execvp is ok, all next is not reachable

            printf("Error: Command not found: %s\n", linev[0]);

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
