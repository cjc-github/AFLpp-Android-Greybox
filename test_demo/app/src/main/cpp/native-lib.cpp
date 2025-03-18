#include <jni.h>
#include <string>

extern "C" JNIEXPORT void JNICALL
Java_com_example_test_1demo_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */, jstring data) {
    // 将 jstring 转换为 C 字符串
    const char* str = env->GetStringUTFChars(data, nullptr);

    // 获取字符串长度
    jsize length = env->GetStringUTFLength(data);

    // 分配一个小的缓冲区
    char buffer[10];

    // 如果字符串长度超过 10，则触发堆溢出
    if (length > 10) {
        // 使用 strcpy 造成堆溢出
        strcpy(buffer, str);  // 这将会导致未定义行为
    }

    // 释放字符串
    env->ReleaseStringUTFChars(data, str);
    return;
}