#include <errno.h>
#include <stdio.h>
// log信息调试
#include <android/log.h>

#include "jenv.h"

#define BUFFER_SIZE 256

// 加上log信息
#define LOG_TAG "jni_fuzz"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

/* Target function */
extern jobject Java_qb_blogfuzz_NativeHelper_fuzzMeArray(JNIEnv *, jclass,
                                                         jbyteArray);

// JavaVM *vm = NULL;
// JNIEnv *env = NULL;

/* Java context */
static JavaCTX ctx;

/* Persistent loop */
void fuzz_one_input(const uint8_t *buffer, size_t length)
{
  jbyteArray jBuffer = (*ctx.env)->NewByteArray(ctx.env, length);
  (*ctx.env)->SetByteArrayRegion(ctx.env, jBuffer, 0, length,
                                 (const jbyte *)buffer);
  // 增加log信息调试
  LOGI("[+] fuzz_one_input called");
  Java_qb_blogfuzz_NativeHelper_fuzzMeArray(ctx.env, NULL, jBuffer);
  LOGI("[+] Finish fuzz_one_input called");

  (*ctx.env)->DeleteLocalRef(ctx.env, jBuffer);
}

int main(void)
{
  int status;
  const uint8_t buffer[BUFFER_SIZE];

  ssize_t rlength = fread((void *)buffer, 1, BUFFER_SIZE, stdin);
  if (rlength == -1)
    return errno;

  if ((status = init_java_env(&ctx, NULL, 0)) != 0)
  {
    return status;
  }

  // log信息调试
  LOGI("[+] Read %zd bytes from stdin", rlength);
  fuzz_one_input(buffer, rlength);

  return 0;
}
