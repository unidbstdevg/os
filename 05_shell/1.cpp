// shold be before readline include
#include <stdio.h>

#include <readline/readline.h>

#include <readline/history.h>

#include <fcntl.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#include <array>
#include <list>

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
#define PIPE_WORD "|"
#define PROMPT "box_to_process_commands> "
#define MAX_ARGS_COUNT 512

template <typename T, std::size_t N>
inline static decltype(auto) to_raw_array(const std::array<T, N>& arr_v) {
    return reinterpret_cast<const T(&)[N]>(*arr_v.data());
}

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

        std::list<std::array<char*, MAX_ARGS_COUNT>> linevs;

        std::array<char*, MAX_ARGS_COUNT> linev;
        linev[0] = strtok(line, " ");
        int linec = 1;

    start_parse_line:
        int redirectsymboli = 0;
        while((linev[linec] = strtok(NULL, " "))) {
            if(!strcmp(linev[linec], PIPE_WORD)) {
                linev[linec] = NULL;
                linevs.push_back(linev);
                linec = 0;

                goto start_parse_line;
            }

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

        linevs.push_back(linev);

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

        int last_pipe_descriptor = -1;
        std::list<std::array<int, 2>> pipes;
        for(int i = 1; i < linevs.size(); i++) {
            int pipe_channels[2];
            if(pipe(pipe_channels)) {
                printf("Fatal error: Can't allocate pipe\n");
                exit(2);
            }
            last_pipe_descriptor = pipe_channels[1];

            std::array<int, 2> ar;
            std::move(std::begin(pipe_channels), std::end(pipe_channels),
                      ar.begin());
            pipes.push_back(ar);
            // printf("DEBUG: vect: %i, %i\n", pipes.back()[0],
            // pipes.back()[1]);
        }

        // printf("DEBUG: pipes count: %lu\n", pipes.size());

        int count_childs = 0;
        {
            std::array<int, 2> prev_pip;
            bool first_cmd = true;

            while(true) {
                // it is guaranteed that there is at least one element
                linev = linevs.front();
                linevs.pop_front();

                bool empty_pipes = false;
                std::array<int, 2> pip;
                if(!pipes.empty()) {
                    pip = pipes.front();
                    pipes.pop_front();
                } else {
                    empty_pipes = true;
                }

                if(!fork()) {
                    // child

                    if(!empty_pipes) {
                        // fprintf(stderr, "DEBUG: %s stdout to %i\n", linev[0],
                        //         pip[1]);

                        dup2(pip[1], STDOUT_FILENO);
                        close(pip[1]);
                    }

                    if(!first_cmd) {
                        // fprintf(stderr, "DEBUG: %s stdin to %i\n", linev[0],
                        //         prev_pip[0]);

                        dup2(prev_pip[0], STDIN_FILENO);
                        close(prev_pip[0]);
                    }

                    if(empty_pipes && redirectsymboli) {
                        char* filename = linev[redirectsymboli + 1];
                        // printf("DEBUG: redirect output to %s\n", filename);
                        int fd =
                            open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0600);
                        dup2(fd, STDOUT_FILENO);
                        close(fd);
                    }

                    // close all pipes
                    for(int i = 3; i <= last_pipe_descriptor; i++) {
                        close(i);
                    }

                    execvp(linev[0], to_raw_array(linev));

                    // if execvp is ok, all next is not reachable

                    fprintf(stderr, "Error: Command not found: %s\n", linev[0]);

                    return 0;
                } else {
                    // main
                    count_childs++;
                }

                prev_pip = pip;
                first_cmd = false;

                // loop exit conditions
                if(linevs.empty())
                    break;
            }
        }

        // close all pipes
        for(int i = 3; i <= last_pipe_descriptor; i++) {
            close(i);
        }

        // wait for all childs
        for(int i = 0; i < count_childs; i++) {
            // printf("DEBUG: wait for %i\n", i);
            wait(0);
        }
        // printf("DEBUG_WAIT: wait end\n");

        // readline malloc'ed it
        free(line);
    }

    return 0;
}
