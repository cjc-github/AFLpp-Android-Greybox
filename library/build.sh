gcc -o demo.so -shared -fPIC demo.c 

../android-ndk-r25c/toolchains/llvm/prebuilt/linux-x86_64/bin/x86_64-linux-android26-clang -o demo.so -shared -fPIC demo.c 

# gcc -o demo demo.c 

# afl-gcc -o demo demo.c
# afl-fuzz -i in -o out -- ./demo @@

g++ -o harness harness.cpp -ldl

../android-ndk-r25c/toolchains/llvm/prebuilt/linux-x86_64/bin/x86_64-linux-android26-clang -o harness harness.cpp -ldl

# run the fuzzing
# afl-fuzz -i in -o out -n ./harness ./demo.so @@
