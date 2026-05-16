CC = gcc
CFLAGS = -Iinclude -Wall -O2
OUT = build/gitAuto.exe

SRC = \
    src/gitauto.c \
    src/feature/common.c \
    src/feature/init.c \
    src/feature/push.c \
    src/feature/watch.c \
    src/feature/sshagent.c \
    src/feature/link.c \
    src/feature/pull.c \
    src/feature/sync.c \
    src/utils/env.c

all: $(OUT)

$(OUT): $(SRC)
	mkdir -p build
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -rf build
