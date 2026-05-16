#include "gitauto.h"
#include "env.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <signal.h>
#include <time.h>


/* Command definition with aliases support */
typedef int (*command_func)(int argc, char **argv);   // 加了这行
typedef struct {
    const char *name;
    const char *alias;    /* single character alias or NULL */
    command_func func;
} Command;

/* Command table */
static Command commands[] = {
    {"init",  "i", cmd_init},
    {"push",  "p", cmd_push},
    {"watch", "w", cmd_watch},
    {"link",  "l", cmd_link},
    {NULL, NULL, NULL}  /* sentinel */
};

/* Find command by name or alias */
static Command* find_command(const char *arg) {
    for (int i = 0; commands[i].name != NULL; i++) {
        if (strcmp(arg, commands[i].name) == 0) {
            return &commands[i];
        }
        if (commands[i].alias && strcmp(arg, commands[i].alias) == 0) {
            return &commands[i];
        }
    }
    return NULL;
}

void print_usage(void)
{
    printf(
        "gitauto - lightweight git automation tool\n"
        "\n"
        "Usage:\n"
        "  gitauto init/i             Initialize git repository\n"
        "  gitauto push/p             Commit & push immediately\n"
        "  gitauto watch/w            Auto push on file changes\n"
        "  gitauto watch/w --quiet    Auto push with minimal output\n"
        "\n"
    );
}

/* ---------------- main ---------------- */

static void on_sig(int _) {
    running = 0;
}

int main(int argc, char **argv)
{
    signal(SIGINT, on_sig);

    if (argc < 2) {
        print_usage();
        return 0;
    }

    Command *cmd = find_command(argv[1]);

    if (!cmd) {
        printf("unknown command: %s\n", argv[1]);
        return 1;
    }

    return cmd->func(argc, argv);
}

