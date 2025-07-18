# AFLpp-Android-Greybox项目



# 一、下载相关项目

## 1.1 下载相关依赖

```bash
#　依赖
apt update
apt install cmake curl unzip xxd git
```



## 1.2 下载相关项目

```bash
#　下载aflplusplus项目
curl https://codeload.github.com/AFLplusplus/AFLplusplus/zip/refs/tags/v4.20c --output 4.20c.zip
unzip 4.20c.zip

# 下载ndk
curl https://dl.google.com/android/repository/android-ndk-r25c-linux.zip --output ndk.zip
unzip ndk.zip

# 将cmakelist移动到afl++中
# 如果是x86_64架构
cp CMakeLists_x86_64.txt AFLplusplus-4.20c/CMakeLists.txt
# 如果是x86架构
cp CMakeLists_x86.txt AFLplusplus-4.20c/CMakeLists.txt
# 如果是arm32架构
cp CMakeLists_arm.txt AFLplusplus-4.20c/CMakeLists.txt
# 如果是arm64架构
cp CMakeLists_arm64.txt AFLplusplus-4.20c/CMakeLists.txt
```



注意：如果想要更改frida的版本的话，修改CMakeLists.txt文件中的

```
if (NOT FRIDA_VERSION)
    set(FRIDA_VERSION "16.6.6")
endif()

测试环境为:
AFLplusplus-4.20c -> AFLplusplus-4.31c
frida 16.0.13

frida-trace.so使用4.20c生成，afl-fuzz使用4.31c
```



注意：

<font style=“color=red">使用afl++其他版本可能会导致出现崩溃后，速度回不去了。</font>



## 1.3 构建

```bash
cd AFLplusplus-4.20c/
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



## 1.4 复制到设备

```bash
# 将afl-fuzz和afl-frida-trace.so文件复制到android设备中
adb push afl-fuzz afl-frida-trace.so /data/local/tmp
```



# 二、测试

## 2.1 C/C++项目

```bash
# 下载被测目标
git clone https://github.com/fuzzstati0n/fuzzgoat.git

cd fuzzgoat/

#修改Makfile文件的第一行
CC=../android-ndk-r25c/toolchains/llvm/prebuilt/linux-x86_64/bin/x86_64-linux-android26-clang

# 然后make
make

adb push fuzzgoat fuzzgoat_ASAN in /data/local/tmp
```



### 2.1.1 黑盒模糊测试

运行命令如下：

```bash
adb shell
cd /data/local/tmp/
# 执行黑盒
./afl-fuzz -i in -o out -n -- ./fuzzgoat @@
```

![image-20250318102116860](README.assets/image-20250318102116860.png)



注意：如果ctrl+c啥的退不出当前终端，可以考虑新开一个终端，然后输入killall -9 afl-fuzz来杀死当前进程。

```
killall -9 afl-fuzz
```



### 2.1.2 灰盒模糊测试

运行命令如下：

```bash
adb shell
cd /data/local/tmp/

# 先测试
LD_PRELOAD=./afl-frida-trace.so ./fuzzgoat ./in/seed

# 注意：如果测试出现问题，一般都是afl.js文件的问题，将其删除或者进行合理化的修改

#　执行灰盒
./afl-fuzz -i in -o out -O -- ./fuzzgoat @@
```

![image-20250318102935254](README.assets/image-20250318102935254.png)



## 2.2 Android项目

test_demo是Android项目，生成的apk文件默认位于项目目录下的app/build/outputs/apk/文件夹中

这个具体为

```
这个具体为test_demo/app/build/outputs/apk/debug/app-debug.apk
```



运行Android设备

```bash
cd android/
# 1. 移动被测apk
adb push app-debug.apk /data/local/tmp/target-app.apk

# 2. 移动so库
unzip -d ./app-debug app-debug.apk

# x86_64架构
adb push ./app-debug/lib/x86_64/* /data/local/tmp
# arm架构
adb push ./app-debug/lib/arm64-v8a/* /data/local/tmp

# 3. 构建驱动程序,并移动到设备中

# x86_64架构
../android-ndk-r25c/toolchains/llvm/prebuilt/linux-x86_64/bin/x86_64-linux-android26-clang -o server server.c lib/vm.c -I lib -L lib -Wall -O3 -ldl -Wl,--export-dynamic
# arm架构
../android-ndk-r25c/toolchains/llvm/prebuilt/linux-x86_64/bin/aarch64-linux-android26-clang -o server server.c lib/vm.c -I lib -L lib -Wall -O3 -ldl -Wl,--export-dynamic

adb push server /data/local/tmp/

# 4. 测试
adb shell
cd /data/local/tmp/
./server in/seed1
```

运行结果如下：

![image-20250318111823420](README.assets/image-20250318111823420.png)



### 2.2.1 黑盒模糊测试

```bash
adb shell
cd /data/local/tmp
./afl-fuzz -i in -o out -t 10000+ -m none -n -- ./server @@
```

![image-20250318112006375](README.assets/image-20250318112006375.png)



### 2.2.2 灰盒模糊测试

```bash
adb shell
cd /data/local/tmp
./afl-fuzz -i in -o out -t 10000+ -m none -O -- ./server @@

# 如果确保./server在执行种子时不会导致崩溃，那么灰盒模糊测试失败的原因就在于server中使用了dlopen函数
```

实际上，这个灰盒模糊测试还在测试中。等待修复成功。



# 三、android fuzzing arm

## 3.1 jenv

按照章节一进行arm64架构配置

```bash
cd jenv
rm -rf build && mkdir build && cd build
# toollcain_file的地址需要精准
cmake -DANDROID_PLATFORM=26 -DCMAKE_TOOLCHAIN_FILE=../../android-ndk-r25c/build/cmake/android.toolchain.cmake -DANDROID_ABI=arm64-v8a ..
make

adb push libjenv.so /data/local/tmp
```

生成arm64架构的libjenv.so文件



## 3.2 native

```bash
cd native
cp ../apk/qb.blogfuzz/lib/arm64-v8a/libblogfuzz.so ./lib/
cp ../jenv/build/libjenv.so ./lib/

rm -rf build && mkdir build && cd build

# toollcain_file的地址需要精准
cmake -DANDROID_PLATFORM=26 -DCMAKE_TOOLCHAIN_FILE=../../android-ndk-r25c/build/cmake/android.toolchain.cmake -DANDROID_ABI=arm64-v8a ..

make

adb push fuzz /data/local/tmp
adb push ../lib/libblogfuzz.so /data/local/tmp
# 不考虑afl.js文件
adb push ../afl.js /data/local/tmp

# 验证
adb shell
cd /data/local/tmp
rm -rf in out
mkdir in
dd if=/dev/urandom of=in/sample.bin bs=1 count=16
# 放一个漏洞进去
echo "Quarksl4bfuzzMe!" > in/crash

# 运行灰盒模糊测试
./afl-fuzz -i in -o out -O -G 256 -- ./fuzz
```

运行截图：



4.31c的afl-frida-trace.so和afl-fuzz

![image-20250715172547853](README.assets/image-20250715172547853.png)

4.20c的afl-frida-trace.so，4.31c的afl-fuzz

![image-20250716174121776](README.assets/image-20250716174121776.png)

验证:

```bash
adb shell
killall -9 afl-fuzz
cd /data/local/tmp
# 复现
LD_PRELOAD=./afl-frida-trace.so ./fuzz ./out/default/crashes/id***
```



## 3.3 slinked_jni 强链接

```bash
cd slinked_jni/

cp ../jenv/build/libjenv.so ./lib/
cp ../apk/qb.blogfuzz/lib/arm64-v8a/libblogfuzz.so ./lib/

rm -rf build && mkdir build && cd build
# toollcain_file的地址需要精准
cmake -DANDROID_PLATFORM=26 -DCMAKE_TOOLCHAIN_FILE=../../android-ndk-r25c/build/cmake/android.toolchain.cmake -DANDROID_ABI=arm64-v8a ..
make

# 步骤３:移植
cd ../
# java版本1.8.0_301
javac Wrapper.java
# dm版本29.0.2
/home/test/Android/Sdk/build-tools/29.0.2/d8 Wrapper.class
mv classes.dex mock.dex

cd build
adb push ../mock.dex /data/local/tmp
adb push fuzz /data/local/tmp
adb push ../afl.js ../lib/libblogfuzz.so ../lib/libjenv.so /data/local/tmp

# 验证
adb shell
cd /data/local/tmp
mkdir in
dd if=/dev/urandom of=in/sample.bin bs=1 count=16

# 放一个漏洞进去
echo "Quarksl4bfuzzMe!" > in/crash

# 验证是否触发漏洞
# 方法1:
cat in/crash | ./fuzz
# 方法2:
./afl-fuzz -i in -o out -n -- ./fuzz

# debug方法，这个流程必须不能报错
cat in/sample.bin | LD_PRELOAD=./afl-frida-trace.so ./fuzz

# 这里加上超时选项
./afl-fuzz -i in -o out -O -G 256 -t 10000+ -- ./fuzz
```

运行截图：

4.31c的afl-frida-trace.so和afl-fuzz

![image-20250715174659386](README.assets/image-20250715174659386.png)

4.20c的afl-frida-trace.so，4.31c的afl-fuzz

![image-20250716174905750](README.assets/image-20250716174905750.png)

## 3.4 wliked_jni 弱链接

```bash
cd wlinked_jni/

cp ../apk/qb.blogfuzz/lib/arm64-v8a/libblogfuzz.so ./lib/
cp ../jenv/build/libjenv.so ./lib/

rm -rf build && mkdir build && cd build
# toollcain_file的地址需要精准 
cmake -DANDROID_PLATFORM=31 -DCMAKE_TOOLCHAIN_FILE=../../android-ndk-r25c/build/cmake/android.toolchain.cmake -DANDROID_ABI=arm64-v8a ..

make

# 移植
adb push fuzz /data/local/tmp
adb push ../lib/libblogfuzz.so ../lib/libjenv.so /data/local/tmp
adb push ../afl.js /data/local/tmp/afl.js

# 验证
adb shell
cd /data/local/tmp
mkdir in
dd if=/dev/urandom of=in/sample.bin bs=1 count=16

# 放一个漏洞进去
echo "Quarksl4bfuzzMe!" > in/crash

# 验证是否触发漏洞
# 方法1:
cat in/crash | ./fuzz
# 方法2:
./afl-fuzz -i in -o out -n -t 10000+ -- ./fuzz

# 运行模糊测试
./afl-fuzz -i in -o out -O -G 256 -t 10000+ -- ./fuzz
```

运行截图：

4.31c的afl-frida-trace.so和afl-fuzz

![image-20250715175722047](README.assets/image-20250715175722047.png)



4.20c的afl-frida-trace.so，4.31c的afl-fuzz

![image-20250716175809214](README.assets/image-20250716175809214.png)



# 四、Android fuzzing x86_64

## 4.1 jenv

按照章节一进行x86_64架构配置

```bash
cd jenv
rm -rf build && mkdir build && cd build
cmake -DANDROID_PLATFORM=26 -DCMAKE_TOOLCHAIN_FILE=../../android-ndk-r25c/build/cmake/android.toolchain.cmake -DANDROID_ABI=x86_64 ..
make

adb push libjenv.so /data/local/tmp
```

生成x86_64架构的libjenv.so文件





注意：如果是x86架构的话，jenv需要进行修改，因为在下面的Fuzz过程中，使用adb logcat查看命令时，会出现下面的问题

```
04-08 09:23:26.844  7006  7006 V jenv    : [+] Initialize Java environment
04-08 09:23:26.844  7006  7006 E jenv    : [!] undefined symbol: JniInvocationCreate
```

这意味着代码在x86架构中是存在问题的。





注意： 在虚拟机Pixel 6 API 31上面没有触发这个问题，正常运行。





## 4.2 native

```bash
cd native
cp ../apk/qb.blogfuzz/lib/x86_64/libblogfuzz.so ./lib/
rm -rf build && mkdir build && cd build

# toollcain_file的地址需要精准
cmake -DANDROID_PLATFORM=26 -DCMAKE_TOOLCHAIN_FILE=../../android-ndk-r25c/build/cmake/android.toolchain.cmake -DANDROID_ABI=x86_64 ..

make

adb push fuzz /data/local/tmp
adb push ../lib/libblogfuzz.so /data/local/tmp/
# 将4.1生成的libjenv.so复制到Android设备中
adb push ../../jenv/build/libjenv.so /data/local/tmp
# afl插桩文件
adb push ../afl_x86.js /data/local/tmp/afl.js

# 验证
adb shell
su
cd /data/local/tmp
mkdir in
dd if=/dev/urandom of=in/sample.bin bs=1 count=16
# 放一个漏洞进去
echo "Quarksl4bfuzzMe!" > in/crash

# 验证是否触发漏洞
# 方法1:
cat in/crash | ./fuzz
# 方法2:
./afl-fuzz -i in -o out -n -- ./fuzz


# 运行灰盒模糊测试
./afl-fuzz -i in -o out -O -G 256 -- ./fuzz

```

运行截图：

没有把crash放入时的截图：

![image-20250715143426464](README.assets/image-20250715143426464.png)

如果把漏洞放进入或者跑出了漏洞，速度就极速下降了。从32.2k/sec -> 1108/sec

![image-20250715142731629](README.assets/image-20250715142731629.png)



afl++ v.31c afl-frida-trace.so 4.20c

![image-20250717165335492](README.assets/image-20250717165335492.png)



黑盒模糊测试

```
./afl-fuzz -i in -o out -n -- ./fuzz
```

运行截图如下：

![image-20250715153855026](README.assets/image-20250715153855026.png)



## 4.3 slinked_jni 强链接

```bash
# 步骤1: 移动相关信息到待编译库中
cd slinked_jni
cp ../apk/qb.blogfuzz/lib/x86_64/libblogfuzz.so ./lib/
cp ../jenv/build/libjenv.so ./lib/

# 步骤２: 编译并构建fuzz
rm -rf build && mkdir build && cd build

# toollcain_file的地址需要精准
cmake -DANDROID_PLATFORM=26 -DCMAKE_TOOLCHAIN_FILE=../../android-ndk-r25c/build/cmake/android.toolchain.cmake -DANDROID_ABI=x86_64 ..

make

# 步骤３:移植
cd ../
# java版本1.8.0_301
javac Wrapper.java
# dm版本29.0.2
/home/test/Android/Sdk/build-tools/29.0.2/d8 Wrapper.class
mv classes.dex mock.dex


# 移植
cd build
adb push ../mock.dex /data/local/tmp
adb push fuzz /data/local/tmp
adb push ../lib/libblogfuzz.so ../lib/libjenv.so /data/local/tmp
adb push ../afl_x86.js /data/local/tmp/afl.js

# 验证
adb shell
cd /data/local/tmp
mkdir in
dd if=/dev/urandom of=in/sample.bin bs=1 count=16

# 放一个漏洞进去
echo "Quarksl4bfuzzMe!" > in/crash

# 验证是否触发漏洞
# 方法1:
cat in/crash | ./fuzz
# 方法2:
./afl-fuzz -i in -o out -n -- ./fuzz

# debug方法，这个流程必须不能报错
cat in/sample.bin | LD_PRELOAD=./afl-frida-trace.so ./fuzz

# 运行灰盒模糊测试
./afl-fuzz -i in -o out -O -G 256 -- ./fuzz
```

运行截图：

这个有没有漏洞，速度都一样了。

![image-20250715150558599](README.assets/image-20250715150558599.png)



afl++ v.31c afl-frida-trace.so 4.20c

![image-20250717170123560](README.assets/image-20250717170123560.png)



## 4.4 wlinked_jni 弱链接

```bash
cd wlinked_jni
cp ../apk/qb.blogfuzz/lib/x86_64/libblogfuzz.so ./lib/
cp ../jenv/build/libjenv.so ./lib/

# 编译
rm -rf build && mkdir build && cd build
cmake -DANDROID_PLATFORM=26 -DCMAKE_TOOLCHAIN_FILE=../../android-ndk-r25c/build/cmake/android.toolchain.cmake -DANDROID_ABI=x86_64 ..

make

# 移动到Android设备
adb push fuzz /data/local/tmp
adb push ../lib/libblogfuzz.so ../lib/libjenv.so /data/local/tmp
adb push ../afl_x86.js /data/local/tmp/afl.js

# 验证
adb shell
cd /data/local/tmp
mkdir in
dd if=/dev/urandom of=in/sample.bin bs=1 count=16

# 放一个漏洞进去
echo "Quarksl4bfuzzMe!" > in/crash

# 验证是否触发漏洞
# 方法1:
cat in/crash | ./fuzz
# 方法2:
./afl-fuzz -i in -o out -n -- ./fuzz

# debug方法，这个流程必须不能报错
cat in/sample.bin | LD_PRELOAD=./afl-frida-trace.so ./fuzz

# 运行灰盒模糊测试
./afl-fuzz -i in -o out -O -G 256 -t 1000+ -- ./fuzz
```

运行截图：

![image-20250715151810361](README.assets/image-20250715151810361.png)



afl++ v.31c afl-frida-trace.so 4.20c

![image-20250717170237365](README.assets/image-20250717170237365.png)



# 五、统计



## 5.1 统计

统计表

|                                                              | 原始      | x86_64    | arm64     |
| ------------------------------------------------------------ | --------- | --------- | --------- |
| **Standard native function**                                 | ~10k/sec  | 22.9k/sec | 78.8/sec  |
| **Weakly linked JNI function**                               | ~9k/sec   | 16.2k/sec | 1.65/sec  |
| **Strongly linked JNI function**                             | ~5k/sec   | 23.5k/sec | 21.26/sec |
| **Strongly linked JNI function (with Java hook)**, 修改afl.js | ~3.5k/sec |           |           |

 

## 5.2 疑问：

### 问题1：

AFL++ Frida持久模式的修复

```
参考文献：
https://github.com/AFLplusplus/AFLplusplus/commit/1369cf7176c552286eb8c12de70cf3bbdddc4981
```

相关参考

https://github.com/AFLplusplus/AFLplusplus/issues/2298

里面提及Afl.setInstrumentNoDynamicLoad()

### 问题2：

如果崩溃触发的太慢的话，可以考虑将崩溃种子作为输入。



### 问题3：

常见的查错办法: `AFL_NO_FORKSRV=1` ，``AFL_DEBUG=1`，`AFL_DEBUG_CHILD=1`等。



### 问题4：

如果遇见当前终端无法触发该漏洞的情况，可能是共享内存啥导致的，退出终端，新开一个。或者进去退出su。



### 问题5：

正常来说，如果刚开启fuzz的速度很高，但是一出漏洞，速度直线下降，这意味着，afl-frida-trace.so的版本出问题了。这种情况下，需要对这个版本进行比对。



## 5.3 Android各个版本测试

Android Fuzz支持情况

```shell
adb shell getprop ro.build.version.release
```



前提准备

```
cd /home/test/TCL/AFLpp-Android-Greybox/AFLplusplus-4.20c/build
adb push afl-frida-trace.so afl-fuzz /data/local/tmp/
```



