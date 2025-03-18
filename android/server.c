#include "vm.h"
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <dlfcn.h>

#define BUFFER_SIZE 1024 * 1024 * 128

JavaVM *vm = NULL;
JNIEnv *env = NULL;

typedef void (*jni0_t)(JNIEnv *env, jobject obj, jstring i1);

char* read_file_content(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf(" [!] Could not open file: %s\n", filename);
        return NULL;
    }

    // 为内容分配内存
    char* buffer = (char*)malloc(BUFFER_SIZE);
    if (!buffer) {
        printf(" [!] Memory allocation failed\n");
        fclose(file);
        return NULL;
    }

    // 读取文件内容
    size_t bytes_read = fread(buffer, 1, BUFFER_SIZE - 1, file);
    buffer[bytes_read] = '\0'; // 确保字符串以 null 结尾

    fclose(file);
    return buffer;
}

void fuzz_one_input(const char* input_string, jclass class0, jobject obj0, jni0_t jni0) {
    jstring par0 = (*env)->NewStringUTF(env, input_string);
    jni0(env, obj0, par0);
}



int main(int argc, char** argv)
{
    if (argc < 2) {
        printf(" [!] Usage: %s <input_string>\n", argv[0]);
        return 1;
    }

    const char *input_file = argv[1];
    char *input_string = read_file_content(input_file);
    if (!input_string) {
        return 1; // 文件读取失败
    }


    jstring str0 = "/data/local/tmp/libtest_demo.so";
    printf(" [+] Loading target: [%s] \n", input_string);
    void *handle0 = dlopen(str0, RTLD_LAZY);
    if (handle0 == NULL) {
        printf(" [!] Could not dlopen the file! (%s)\n", dlerror());
        return -1;
    }

    printf(" [+] Native Harness\n");
    int status = init_jvm(&vm, &env);
    if (status == 0) {
        printf(" [+] Initialization success (vm=%p, env=%p)\n", vm, env);
    } else {
        printf(" [!] Initialization failure (%i)\n", status);
        return 1;
    }

    jni0_t jni0 = dlsym(handle0, "Java_com_example_test_1demo_MainActivity_stringFromJNI");

    if (jni0 == NULL) {
        printf(" [!] Could not dlsym the file! (%s) \n", dlerror());
        return -2;
    } else {
        printf(" [+] Found the function (%p)\n", jni0);
    }

    jclass class0 = (*env)->FindClass(env, "com/example/test_demo/MainActivity");
    if (class0 == NULL) {
        printf(" [!] Could not find the class on the class path\n");
        return -3;
    } else {
        printf(" [+] Found the Class: [%p]\n", class0);
    }

    jobject obj0 = (*env)->AllocObject(env, class0);
    
#ifdef __AFL_HAVE_MANUAL_CONTROL
    __AFL_INIT();
#endif

    printf("Fuzz testing ...\n");
    // 调用新的函数
    fuzz_one_input(input_string, class0, obj0, jni0);

    // jstring par0 = (*env)->NewStringUTF(env, input_string);  // 使用传入的字符串
    // jni0(env, obj0, par0);

    printf(" [+] Cleaning up VM\n");
    (*vm)->DestroyJavaVM(vm);

    printf(" [+] Closing target library\n");
    dlclose(handle0);

    return 0;
}
