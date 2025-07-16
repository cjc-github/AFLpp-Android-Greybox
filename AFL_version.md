更换版本修改v4.31c

```bash
这里arm64用的是4.31c

curl https://codeload.github.com/AFLplusplus/AFLplusplus/zip/refs/tags/v4.31c --output 4.31c.zip
unzip 4.31c.zip

# 如果是x86_64架构
cp CMakeLists_x86_64.txt AFLplusplus-4.31c/CMakeLists.txt
# 如果是x86架构
cp CMakeLists_x86.txt AFLplusplus-4.31c/CMakeLists.txt
# 如果是arm32架构
cp CMakeLists_arm.txt AFLplusplus-4.31c/CMakeLists.txt
# 如果是arm64架构
cp CMakeLists_arm64.txt AFLplusplus-4.31c/CMakeLists.txt

cd AFLplusplus-4.31c
rm -rf build && mkdir build && cd build

# 注意，android-ndk-r25c的地址
# 如果是arm64架构
cmake -DANDROID_PLATFORM=26 -DCMAKE_TOOLCHAIN_FILE=../../android-ndk-r25c/build/cmake/android.toolchain.cmake -DANDROID_ABI=arm64-v8a ..
# 如果是arm架构
cmake -DANDROID_PLATFORM=26 -DCMAKE_TOOLCHAIN_FILE=../../android-ndk-r25c/build/cmake/android.toolchain.cmake -DANDROID_ABI=armeabi-v7a ..
# 如果是x86_64架构
cmake -DANDROID_PLATFORM=26 -DCMAKE_TOOLCHAIN_FILE=../../android-ndk-r25c/build/cmake/android.toolchain.cmake -DANDROID_ABI=x86_64 ..
# 如果是x86架构
cmake -DANDROID_PLATFORM=26 -DCMAKE_TOOLCHAIN_FILE=../../android-ndk-r25c/build/cmake/android.toolchain.cmake -DANDROID_ABI=x86 ..

# 然后构建
make
```



交叉编译得到的afl-fuzz

```bash
# 如果是arm64架构
/home/test/tcl/AFLpp-Android-Greybox/android-ndk-r25c/toolchains/llvm/prebuilt/linux-x86_64/bin/aarch64-linux-android26-clang -DANDROID_DISABLE_FANCY=1 -O3 -funroll-loops -Wall -D_FORTIFY_SOURCE=2 -g -Wno-pointer-sign -I include/ -DAFL_PATH=\"/usr/local/lib/afl\" -DBIN_PATH=\"/usr/local/bin\" -DDOC_PATH=\"/usr/local/share/doc/afl\" -Wno-unused-function src/afl-fuzz*.c src/afl-common.c src/afl-sharedmem.c src/afl-forkserver.c src/afl-performance.c -o afl-fuzz -ldl -lm -w

# 如果是arm架构
/home/test/tcl/AFLpp-Android-Greybox/android-ndk-r25c/toolchains/llvm/prebuilt/linux-x86_64/bin/armv7a-linux-androideabi26-clang -DANDROID_DISABLE_FANCY=1 -O3 -funroll-loops -Wall -D_FORTIFY_SOURCE=2 -g -Wno-pointer-sign -I include/ -DAFL_PATH=\"/usr/local/lib/afl\" -DBIN_PATH=\"/usr/local/bin\" -DDOC_PATH=\"/usr/local/share/doc/afl\" -Wno-unused-function src/afl-fuzz*.c src/afl-common.c src/afl-sharedmem.c src/afl-forkserver.c src/afl-performance.c -o afl-fuzz -ldl -lm -w

# 如果是x86架构
/home/test/tcl/AFLpp-Android-Greybox/android-ndk-r25c/toolchains/llvm/prebuilt/linux-x86_64/bin/i686-linux-android26-clang -DANDROID_DISABLE_FANCY=1 -O3 -funroll-loops -Wall -D_FORTIFY_SOURCE=2 -g -Wno-pointer-sign -I include/ -DAFL_PATH=\"/usr/local/lib/afl\" -DBIN_PATH=\"/usr/local/bin\" -DDOC_PATH=\"/usr/local/share/doc/afl\" -Wno-unused-function src/afl-fuzz*.c src/afl-common.c src/afl-sharedmem.c src/afl-forkserver.c src/afl-performance.c -o afl-fuzz -ldl -lm -w

# 如果是x86_64架构
/home/test/tcl/AFLpp-Android-Greybox/android-ndk-r25c/toolchains/llvm/prebuilt/linux-x86_64/bin/x86_64-linux-android26-clang -DANDROID_DISABLE_FANCY=1 -O3 -funroll-loops -Wall -D_FORTIFY_SOURCE=2 -g -Wno-pointer-sign -I include/ -DAFL_PATH=\"/usr/local/lib/afl\" -DBIN_PATH=\"/usr/local/bin\" -DDOC_PATH=\"/usr/local/share/doc/afl\" -Wno-unused-function src/afl-fuzz*.c src/afl-common.c src/afl-sharedmem.c src/afl-forkserver.c src/afl-performance.c -o afl-fuzz -ldl -lm -w
```



4.31c版本修改，注释掉afl-fuzz-bitmap.c

```
      // classify_counts_mem((u64 *)afl->san_fsrvs[0].trace_bits,
      //                    afl->fsrv.map_size);
```

