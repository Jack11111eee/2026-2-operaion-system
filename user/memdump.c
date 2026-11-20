#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

void memdump(char *fmt, char *data);

int
main(int argc, char *argv[])
{
  if(argc == 1){
    printf("Example 1:\n");
    int a[2] = { 61810, 2025 };
    memdump("ii", (char*) a);
    
    printf("Example 2:\n");
    memdump("S", "a string");
    
    printf("Example 3:\n");
    char *s = "another";
    memdump("s", (char *) &s);

    struct sss {
      char *ptr;
      int num1;
      short num2;
      char byte;
      char bytes[8];
    } example;
    
    example.ptr = "hello";
    example.num1 = 1819438967;
    example.num2 = 100;
    example.byte = 'z';
    strcpy(example.bytes, "xyzzy");
    
    printf("Example 4:\n");
    memdump("pihcS", (char*) &example);
    
    printf("Example 5:\n");
    memdump("sccccc", (char*) &example);
  } else if(argc == 2){
    // format in argv[1], up to 512 bytes of data from standard input.
    char data[512];
    int n = 0;
    memset(data, '\0', sizeof(data));
    while(n < sizeof(data)){
      int nn = read(0, data + n, sizeof(data) - n);
      if(nn <= 0)
        break;
      n += nn;
    }
    memdump(argv[1], data);
  } else {
    printf("Usage: memdump [format]\n");
    exit(1);
  }
  exit(0);
}


void
memdump(char *fmt, char *data)
{
    while (*fmt) {
        switch (*fmt) {

        case 'i':
            // 读取 4 字节，作为 32 位整数输出
            printf("%d\n", *(int *)data);
            data += sizeof(int);
            break;

        case 'p':
            // 读取 8 字节，作为 64 位整数，以十六进制输出
            printf("%lx\n", *(uint64 *)data);
            data += sizeof(uint64);
            break;

        case 'h':
            // 读取 2 字节，作为 16 位整数输出
            printf("%d\n", *(short *)data);
            data += sizeof(short);
            break;

        case 'c':
            // 读取 1 字节，作为 ASCII 字符输出
            printf("%c\n", *(char *)data);
            data += sizeof(char);
            break;

        case 's':
            // 读取 8 字节，取得字符串指针并输出字符串
            printf("%s\n", *(char **)data);
            data += sizeof(char *);
            break;

        case 'S':
            // 剩余数据本身就是字符串
            printf("%s\n", data);
            return;

        default:
            fprintf(2, "memdump: unknown format '%c'\n", *fmt);
            return;
        }

        // 处理下一个格式字符
        fmt++;
    }
}

