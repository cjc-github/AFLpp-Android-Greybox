#include <errno.h>
#include <stdint.h>
#include <stdio.h>
// log信息调试
#include <android/log.h>

#define BUFFER_SIZE 256
// 加上log信息
#define LOG_TAG "jni_fuzz"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

/* Target function */
extern void _Z6fuzzMePKai(const uint8_t *, uint64_t);

/* Persistent loop */
void fuzz_one_input(const uint8_t *buf, int len)
{
  // 增加log信息调试
  LOGI("[+] fuzz_one_input called");
  _Z6fuzzMePKai(buf, len);
  LOGI("[+] Finish fuzz_one_input called");
}

int main(void)
{
  const uint8_t buffer[BUFFER_SIZE];

  ssize_t rlength = fread((void *)buffer, 1, BUFFER_SIZE, stdin);
  if (rlength == -1)
    return errno;

  // log信息调试
  LOGI("[+] Read %zd bytes from stdin", rlength);
  fuzz_one_input(buffer, rlength);

  return 0;
}
