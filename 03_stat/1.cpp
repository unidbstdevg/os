#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
using namespace std;

/*
+ 1. Взять с командной строки имя файла.
+ 2. Вызвать функцию stat(), передав ей имя файла и буфер для записи информации.
+ 3. Для вывода на экран нужной информации использовать следующие поля структуры
+    struct stat и макросы:
+    ** cut **
+ 4. Для получения символьных имен пользователя и группы используйте вызовы
+    getpwuid() и getgrgid():
+    ** cut **
+ 5. Время последней модификации файла привести в удобочитаемый вид функцией
+    ctime().
*/

void format_time(time_t* time, char* out, int out_len) {
    struct tm lt;
    localtime_r(time, &lt);
    strftime(out, out_len, "%c", &lt);
}
void format_permissions(mode_t st_mode, char* out, int out_len) {
    snprintf(
        out, out_len, "%s%s%s%s%s%s%s%s%s", ((st_mode & S_IRUSR) ? "r" : "-"),
        ((st_mode & S_IWUSR) ? "w" : "-"), ((st_mode & S_IXUSR) ? "x" : "-"),
        ((st_mode & S_IRGRP) ? "r" : "-"), ((st_mode & S_IWGRP) ? "w" : "-"),
        ((st_mode & S_IXGRP) ? "x" : "-"), ((st_mode & S_IROTH) ? "r" : "-"),
        ((st_mode & S_IWOTH) ? "w" : "-"), ((st_mode & S_IXOTH) ? "x" : "-"));
}

int main(int argc, char* argv[]) {
    if(argc < 2) {
        printf("Usage: %s {filename}\n", argv[0]);
        printf("Where {filename} is name of file you want to stat\n");

        return 1;
    }
    char* filename = argv[1];

    struct stat sb;
    if(stat(filename, &sb)) {
        printf("No such file or directory\n");
        return 0;
    }

    if(S_ISREG(sb.st_mode)) {
        printf("regular file\n");
    }
    if(S_ISDIR(sb.st_mode))
        printf("directory\n");

    {
        char permbuf[80];
        format_permissions(sb.st_mode, permbuf, sizeof(permbuf));
        printf("Permitions: %o (%s)\n", sb.st_mode, permbuf);
    }
    {
        struct passwd* pwp_uid = getpwuid(sb.st_uid);
        struct passwd* pwp_gid = getpwuid(sb.st_gid);
        printf("UID:: %u (%s)\n", sb.st_uid, pwp_uid->pw_name);
        printf("GID:: %u (%s)\n", sb.st_gid, pwp_gid->pw_name);
    }
    printf("Size: %lu bytes\n", sb.st_size);
    printf("Access time (atime): %s", ctime(&sb.st_atim.tv_sec));
    printf("Modify time (mtime): %s", ctime(&sb.st_mtim.tv_sec));

    return 0;
}
