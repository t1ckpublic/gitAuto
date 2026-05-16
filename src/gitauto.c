#include "gitauto.h"
#include "env.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <signal.h>
#include <time.h>

/* Command types */
typedef enum {
    CMD_INIT,
    CMD_PUSH,
    CMD_WATCH,
} CommandType;

/* Command definition with aliases support */
typedef int (*command_func)(int argc, char **argv);
typedef struct {
    const char *name;
    const char *alias;    /* single character alias or NULL */
    CommandType type;
} Command;

/* Command table */
static Command commands[] = {
    {"init",  "i", CMD_INIT},
    {"push",  "p", CMD_PUSH},
    {"watch", "w", CMD_WATCH},
    {NULL, NULL, -1}  /* sentinel */
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

int main(int argc, char **argv) {
    signal(SIGINT, on_sig);

    bool quiet = false;
    Command *cmd = NULL;

    if (argc == 1) {
        print_usage();
        return 0;
    }

    /* Parse command and options */
    for (int i = 1; i < argc; i++) {
        if (cmd == NULL) {
            /* First non-option argument should be command */
            cmd = find_command(argv[i]);
            if (cmd == NULL && argv[i][0] != '-') {
                printf("unknown command: %s\n", argv[i]);
                print_usage();
                return 1;
            }
            if (cmd != NULL) {
                continue;
            }
        }
        
        /* Handle options */
        if (strcmp(argv[i], "--quiet") == 0 || strcmp(argv[i], "-q") == 0) {
            quiet = true;
        } else if (argv[i][0] == '-') {
            printf("unknown option: %s\n", argv[i]);
            print_usage();
            return 1;
        }
    }

    if (cmd == NULL) {
        printf("no command specified\n");
        print_usage();
        return 1;
    }

    /* Execute command */
    switch (cmd->type) {
        case CMD_INIT:
            return gitauto_init();
        
        case CMD_PUSH: {
            if (!is_git_repo()) {
                log_error("not a git repository");
                return 1;
            }
            ensure_config();
            ensure_gitignore();
            load_config(&g_cfg);
            return gitauto_push(quiet);
        }
        
        case CMD_WATCH: {
            if (!is_git_repo()) {
                log_error("not a git repository");
                return 1;
            }
            ensure_config();
            ensure_gitignore();
            load_config(&g_cfg);
            git_run("git pull", quiet);
            watch_loop(quiet);
            return 0;
        }
        
        default:
            printf("unknown command type\n");
            return 1;
    }
}

