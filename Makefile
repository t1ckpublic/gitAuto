CC = gcc
CFLAGS = -Iinclude -Wall -O2
CFLAGS += -Wno-format-truncation

OUT_DIR = build
OUT = $(OUT_DIR)/gitAuto.exe
ALIAS = $(OUT_DIR)/ga.exe

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
    src/feature/clone.c \
    src/feature/branch.c \
    src/feature/checkout.c \
    src/feature/backup.c \
    src/feature/open.c \
    src/feature/version.c \
    src/utils/env.c

all: $(OUT) $(ALIAS)

$(OUT): $(SRC)
	mkdir -p $(OUT_DIR)
	$(CC) $(CFLAGS) $^ -o $@

$(ALIAS): $(OUT)
	cp $(OUT) $(ALIAS)

clean:
	rm -rf $(OUT_DIR)