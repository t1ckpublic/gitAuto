CC = gcc
CFLAGS = -Iinclude -Wall -O2
OUT = build/gitAuto.exe

SRC = src/gitauto.c src/feature/feature_common.c src/feature/feature_init.c src/feature/feature_push.c src/feature/feature_watch.c src/feature/feature_sshagent.c src/utils/env.c

all: $(OUT)

$(OUT): $(SRC)
	mkdir -p build
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -rf build
