
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
    int ticks;

    // 检查参数数量
    if (argc != 2) {
        fprintf(2, "Usage: sleep <ticks>\n");
        exit(1);
    }

    // 将字符串转换为整数
    ticks = atoi(argv[1]);

    // 检查睡眠时间
    if (ticks <= 0) {
        fprintf(2, "sleep: invalid ticks value\n");
        exit(1);
    }

    // 调用 xv6 系统调用
    pause(ticks);

    // 正常退出
    exit(0);
}

