
#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "kernel/param.h"
#include "user/user.h"

// 执行指定命令，并将找到的文件路径作为最后一个参数
void
run_command(char *path, char **cmd_argv, int cmd_argc)
{
    int pid;
    char *args[MAXARG];
    int i;

    pid = fork();

    if (pid < 0) {
        fprintf(2, "find: fork failed\n");
        return;
    }

    if (pid == 0) {
        // 复制原命令参数
        for (i = 0; i < cmd_argc; i++) {
            args[i] = cmd_argv[i];
        }

        // 将找到的文件路径追加到参数末尾
        args[i++] = path;
        args[i] = 0;

        // 执行命令
        exec(cmd_argv[0], args);

        fprintf(2, "find: exec %s failed\n", cmd_argv[0]);
        exit(1);
    }

    // 父进程等待子进程结束
    wait(0);
}

// 递归查找文件
void
find(char *path, char *target, char **cmd_argv, int cmd_argc)
{
    char buf[512], *p, *name;
    int fd;
    struct dirent de;
    struct stat st;

    // 打开当前路径
    if ((fd = open(path, 0)) < 0) {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    // 获取文件状态
    if (fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    // 普通文件：比较文件名
    if (st.type == T_FILE) {
        name = path;

        for (p = path; *p; p++) {
            if (*p == '/')
                name = p + 1;
        }

        if (strcmp(name, target) == 0) {
            if (cmd_argv == 0) {
                // 普通模式：打印路径
                printf("%s\n", path);
            } else {
                // -exec 模式：执行指定命令
                run_command(path, cmd_argv, cmd_argc);
            }
        }
    }

    // 目录：递归遍历
    else if (st.type == T_DIR) {

        if (strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)) {
            fprintf(2, "find: path too long\n");
            close(fd);
            return;
        }

        strcpy(buf, path);
        p = buf + strlen(buf);
        *p++ = '/';

        while (read(fd, &de, sizeof(de)) == sizeof(de)) {

            if (de.inum == 0)
                continue;

            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;

            // 跳过当前目录和父目录
            if (strcmp(p, ".") == 0 ||
                strcmp(p, "..") == 0)
                continue;

            // 递归搜索
            find(buf, target, cmd_argv, cmd_argc);
        }
    }

    close(fd);
}

int
main(int argc, char *argv[])
{
    char **cmd_argv = 0;
    int cmd_argc = 0;

    // 普通 find：find <directory> <filename>
    if (argc == 3) {
        find(argv[1], argv[2], 0, 0);
    }

    // find -exec：find <directory> <filename> -exec <command> [args...]
    else if (argc >= 5 && strcmp(argv[3], "-exec") == 0) {

        cmd_argv = &argv[4];
        cmd_argc = argc - 4;

        // 为文件路径和 NULL 预留参数位置
        if (cmd_argc > MAXARG - 2) {
            fprintf(2, "find: too many command arguments\n");
            exit(1);
        }

        find(argv[1], argv[2], cmd_argv, cmd_argc);
    }

    else {
        fprintf(2, "Usage: find <directory> <filename> [-exec command [args...]]\n");
        exit(1);
    }

    exit(0);
}
