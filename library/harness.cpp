#include <iostream>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <limits.h>  // 添加此行以包含 PATH_MAX 的定义

char input_pathname[PATH_MAX];

typedef int (*target_func)(char *buf, int size);

char *read_file(const char *path, unsigned long *length) {
    unsigned long len;
    char *buf;

    FILE *fp = fopen(path, "rb");
    if (!fp) {
        perror("Failed to open file");
        return nullptr;
    }

    fseek(fp, 0, SEEK_END);
    len = ftell(fp);
    rewind(fp);
    
    buf = (char *)malloc(len);
    if (buf == nullptr) {
        perror("Failed to allocate memory");
        fclose(fp);
        return nullptr;
    }

    fread(buf, 1, len, fp);
    fclose(fp);
    *length = len;
    return buf;
}

int main(int argc, char **argv) {
    if (argc < 3) {
        puts("usage: ./harness library_path input_file_path");
        exit(0);
    }

    const char *lib_path = argv[1];
    strcpy(input_pathname, argv[2]);
    
    // 动态加载库
    void *handle = dlopen(lib_path, RTLD_LAZY);
    if (handle == nullptr) {
        perror("Cannot load library");
        exit(EXIT_FAILURE);
    }

    // 获取目标函数的指针
    target_func p_target_func = (target_func)dlsym(handle, "target_func");
    if (p_target_func == nullptr) {
        perror("Cannot find target function");
        dlclose(handle);
        exit(EXIT_FAILURE);
    }

    // 读取输入文件
    unsigned long len = 0;
    char *data = read_file(input_pathname, &len);
    if (data == nullptr) {
        dlclose(handle);
        exit(EXIT_FAILURE);
    }

    // 调用目标函数
    p_target_func(data, len);

    // 清理
    free(data);
    dlclose(handle);
    
    return 0;
}
