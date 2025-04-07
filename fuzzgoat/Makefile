# CC=afl-gcc
CC=../android-ndk-r25c/toolchains/llvm/prebuilt/linux-x86_64/bin/x86_64-linux-android26-clang
DEPS=main.c fuzzgoat.c
ASAN=-fsanitize=address
CFLAGS=-I.
LIBS=-lm

all: $(DEPS)
	$(CC) -o fuzzgoat $(CFLAGS) $^ $(LIBS)
	$(CC) $(ASAN) -o fuzzgoat_ASAN $(CFLAGS) $^ $(LIBS)

afl: fuzzgoat
	afl-fuzz -i in -o out ./fuzzgoat @@

.PHONY: clean

clean:
	rm ./fuzzgoat ./fuzzgoat_ASAN
