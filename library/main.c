#include <stdio.h>
#include <stdlib.h>

int target_func(char *buf, int size) {
    printf("buffer:%p, size:%p\n", buf, size);
    switch (buf[0]) {
        case 1:
            puts("222");
            if (buf[1] == '0') {
                puts("null ptr deference");
                *(char *)(0) = 1;  // 空指针解引用
            }
            break;
        case 0:
            if (buf[2] == '0') {
                if (buf[1] == '0') {
                    puts("crash....");
                    *(char *)(0xdeadbeef) = 1;  // 无效内存访问
                }
            }
            break;
        default:
            puts("default action");
            break;
    }
    return 1;
}

// 主函数
int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    const char *filename = argv[1];
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Failed to open file");
        return 1;
    }

    // 读取文件内容
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    rewind(file);

    char *buffer = (char *)malloc(length);
    if (!buffer) {
        perror("Failed to allocate memory");
        fclose(file);
        return 1;
    }

    fread(buffer, 1, length, file);
    fclose(file);

    // 调用 target_func
    target_func(buffer, length);

    // 清理
    free(buffer);
    return 0;
}
