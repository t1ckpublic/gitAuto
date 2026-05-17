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
    {"branch", "br", cmd_branch},
    {"checkout", "co", cmd_checkout},
    {"backup", "b", cmd_backup},
    {"open", "o", cmd_open},
    {"version", "v", cmd_version},
    {"ignore", "ig", cmd_ignore},
    {"save", "sv", cmd_save},
    {"restore", "re", cmd_restore},
    {"clean", "cl", cmd_clean},
    {"undo", "u", cmd_undo},
    {"amend", "a", cmd_amend},
    {"reset", "r", cmd_reset},
    {"current", "cur", cmd_current},
    {"doctor", "dr", cmd_doctor},
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
        "  gitauto init/i                         Initialize repository (init + add + commit + push)\n"
        "  gitauto clone/c <url>                  Clone repo and initialize workflow\n"
        "  gitauto link/l <url>                   Bind remote repository (set origin + fetch)\n"
        "\n"
        "  gitauto push/p                         Smart push (pull --rebase + add + commit + push)\n"
        "  gitauto push/p -m <msg>                Commit with custom message\n"
        "  gitauto push/p -f                      Force push (--force-with-lease, safe mode)\n"
        "  gitauto push/p -ff                     Dangerous force push (--force)\n"
        "\n"
        "  gitauto pull/pl                        Smart pull (rebase + autostash)\n"
        "  gitauto pull/pl -f                     Hard sync (reset to origin/<branch>)\n"
        "  gitauto pull/pl -ff                    Full reset (clean + hard reset)\n"
        "\n"
        "  gitauto sync/s                         Pull + push bidirectional sync\n"
        "\n"
        "  gitauto watch/w                        Auto sync on file changes\n"
        "  gitauto watch/w -quiet/-q              Quiet mode (minimal output)\n"
        "\n"
        "  gitauto branch/br                      Show branch list\n"
        "  gitauto checkout/co <branch>           Switch branch (auto create if not exist)\n"
        "\n"
        "  gitauto backup/b                       Create backup branch snapshot\n"
        "  gitauto backup/b restore/re            Restore from backup branch\n"
        "  gitauto backup/b restore/re <branch>   Restore specific branch snapshot\n"
        "\n"
        "  gitauto save/sv                        Stash working directory (stash push -u)\n"
        "  gitauto save/sv -m <msg>               Stash with message\n"
        "\n"
        "  gitauto restore/re                     Restore latest stash (pop)\n"
        "  gitauto restore/re -a                  Apply stash (keep stash)\n"
        "  gitauto restore/re <N>                 Restore stash by index\n"
        "  gitauto restore/re list/l              List all stash entries\n"
        "\n"
        "  gitauto clean/cl                       Preview and clean untracked files\n"
        "  gitauto clean/cl -f                    Force clean untracked files\n"
        "  gitauto clean/cl -ff                   Deep clean (including ignored files)\n"
        "\n"
        "  gitauto ignore/ig node                 Add Node.js .gitignore template\n"
        "  gitauto ignore/ig add <rule>           Append custom ignore rule\n"
        "\n"
        "  gitauto undo/u                         Undo last commit (soft reset)\n"
        "  gitauto undo/u mixed/m                 Mixed reset (unstage changes)\n"
        "  gitauto undo/u hard/h                  Hard reset (discard changes)\n"
        "  gitauto undo/u <N>                     Undo last N commits\n"
        "\n"
        "  gitauto amend/a                        Amend last commit\n"
        "  gitauto amend -m <msg>                 Amend with new message\n"
        "\n"
        "  gitauto reset/r                        Hard reset to HEAD\n"
        "  gitauto reset/r <N>                    Reset back N commits\n"
        "\n"
        "  gitauto current/cur                    Show current branch\n"
        "  gitauto open/o                         Open remote repository in browser\n"
        "  gitauto version/v                      Show version info\n"
        "\n"
        "  gitauto doctor/dr                      Diagnose repository issues\n"
        "  gitauto doctor/dr -fix/-f              Auto-fix common issues\n"
        "\n"
        "Notes:\n"
        "  - All commands operate on current branch by default\n"
        "  - -f = force (safe mode where applicable)\n"
        "  - -ff = destructive operation (irreversible)\n"
        "  - watch runs continuously until interrupted\n"
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

