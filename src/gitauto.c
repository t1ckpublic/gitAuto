#include "gitauto.h"
#include "env.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <signal.h>
#include <time.h>

void print_usage(void)
{
    printf(
        "gitauto - lightweight git automation tool\n"
        "\n"
        "Usage:\n"
        "  gitauto init              Initialize git repository\n"
        "  gitauto push              Commit & push immediately\n"
        "  gitauto watch             Auto push on file changes\n"
        "  gitauto watch --quiet     Auto push with minimal output\n"
        "\n"
    );
}

/* ---------------- main ---------------- */

static void on_sig(int _) {
    running = 0;
}

int main(int argc, char **argv) {
    signal(SIGINT, on_sig);

    bool auto_mode = false;
    bool quiet = false;

    if (argc == 1) {
        print_usage();
        return 0;
    }

    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "init")) {
            return gitauto_init();
        }
        else if (!strcmp(argv[i], "watch")) auto_mode = true;
        else if (!strcmp(argv[i], "push")) auto_mode = false;
        else if (!strcmp(argv[i], "--quiet")) quiet = true;
        
        else {
            printf("unknown option");
            print_usage();
            return 1;
        }
    }

    if (!is_git_repo()) {
        log_error("not a git repository");
        return 1;
    }

    ensure_config();
    ensure_gitignore();
    load_config(&g_cfg);


    if (auto_mode) {
        git_run("git pull", quiet);
        watch_loop(quiet);
    } else {
        return gitauto_push(quiet);
    }
    return 0;
}
