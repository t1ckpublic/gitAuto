#include "gitauto.h"
#include "env.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <signal.h>
#include <time.h>


/* Command definition with aliases support */
typedef int (*command_func)(int argc, char **argv);
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
    {"pull",  "pl", cmd_pull},
    {"sync",  "s", cmd_sync},
    {"clone", "c", cmd_clone},
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
        "gitAuto - lightweight git automation tool\n"
        "\n"
        "Usage:\n"
        "  gitauto init/i                   Initialize git repository, commit & push first version\n"
        "  gitauto link/l                   Bind remote repository (set origin + fetch)\n"
        "\n"
        "  gitauto push/p                   Smart push (pull --rebase + add + commit + push)\n"
        "  gitauto push -m <msg>            Commit with custom message\n"
        "  gitauto push -f                  Force push (safe mode: --force-with-lease)\n"
        "  gitauto push -ff                 Dangerous force push (--force)\n"
        "\n"
        "  gitauto pull/pl                  Smart pull (rebase + autostash)\n"
        "  gitauto pull -f                  Hard sync (reset to origin, discard local changes)\n"
        "  gitauto pull -ff                 Full reset (clean + hard reset to origin)\n"
        "\n"
        "  gitauto watch/w                  Auto sync on file changes (push on change)\n"
        "  gitauto watch/w -q               Quiet mode (minimal output)\n"
        "\n"
        "  gitauto sync/s                   Pull + push (full bidirectional sync)\n"
        "\n"
        "Notes:\n"
        "  - All commands use current branch unless specified\n"
        "  - -f and -ff are destructive operations\n"
        "  - watch mode continuously monitors working directory\n"
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

