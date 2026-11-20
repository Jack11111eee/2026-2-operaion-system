
#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

// 判断是否为分隔符
int
is_delimiter(char c)
{
    return c == '-' || c == ' ' ||
           c == '\r' || c == '\t' ||
           c == '\n' || c == '.' ||
           c == '/' || c == ',';
}

// 判断并输出符合条件的数字
void
check_number(int num)
{
    if (num % 5 == 0 || num % 6 == 0) {
        printf("%d\n", num);
    }
}

// 从指定文件描述符读取并处理数字
int
process_input(int fd)
{
    char buf[128];
    int n;
    int num = 0;
    int in_number = 0;

    while ((n = read(fd, buf, sizeof(buf))) > 0) {
        for (int i = 0; i < n; i++) {
            char c = buf[i];

            if (c >= '0' && c <= '9') {
                num = num * 10 + (c - '0');
                in_number = 1;
            } else {
                if (in_number) {
                    check_number(num);
                    num = 0;
                    in_number = 0;
                }
            }
        }
    }

    if (n < 0)
        return -1;

    // 处理文件末尾的数字
    if (in_number)
        check_number(num);

    return 0;
}

int
main(int argc, char *argv[])
{
    int fd;

    // 没有文件参数时，从标准输入读取
    if (argc == 1) {
        if (process_input(0) < 0) {
            fprintf(2, "sixfive: read error\n");
            exit(1);
        }
        exit(0);
    }

    // 依次处理所有输入文件
    for (int i = 1; i < argc; i++) {
        fd = open(argv[i], O_RDONLY);

        if (fd < 0) {
            fprintf(2, "sixfive: cannot open %s\n", argv[i]);
            exit(1);
        }

        if (process_input(fd) < 0) {
            fprintf(2, "sixfive: read error\n");
            close(fd);
            exit(1);
        }

        close(fd);
    }

    exit(0);
}
