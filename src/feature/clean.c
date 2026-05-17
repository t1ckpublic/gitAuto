#include "gitauto.h"
#include "env.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ---------------- core ---------------- */

static int gitauto_clean_core(int force_level, bool quiet) {

    char cmd[256];

    /* ---------------- normal mode ----------------
     * preview first
     * -------------------------------------------- */
    if (force_level == 0) {

        printf(
            "[gitAuto] preview clean files:\n\n"
        );

        git_run("git clean -fdn", false);

        char answer[16];

        printf(
            "\ncontinue? (y/N): "
        );

        if (!fgets(answer, sizeof(answer), stdin)) {
            return 1;
        }

        if (answer[0] != 'y' &&
            answer[0] != 'Y') {

            printf("cancelled\n");
            return 1;
        }

        snprintf(cmd, sizeof(cmd),
                 "git clean -fd");
    }

    /* ---------------- force clean ---------------- */
    else if (force_level == 1) {

        snprintf(cmd, sizeof(cmd),
                 "git clean -fd");
    }

    /* ---------------- nuclear clean ----------------
     * remove ignored files too
     * ---------------------------------------------- */
    else {

        printf(
            "[WARN] ignored files will also be removed\n"
            "[WARN] this may delete build/cache/env files\n"
        );

        char answer[16];

        printf(
            "continue? (y/N): "
        );

        if (!fgets(answer, sizeof(answer), stdin)) {
            return 1;
        }

        if (answer[0] != 'y' &&
            answer[0] != 'Y') {

            printf("cancelled\n");
            return 1;
        }

        snprintf(cmd, sizeof(cmd),
                 "git clean -fdx");
    }

    return git_run(cmd, quiet);
}

/* ---------------- handler ---------------- */

int cmd_clean(int argc, char **argv) {

    bool quiet = false;

    /* 0=normal 1=-f 2=-ff */
    int force_level = 0;

    for (int i = 2; i < argc; i++) {

        if (!strcmp(argv[i], "-q") ||
            !strcmp(argv[i], "--quiet")) {

            quiet = true;
        }

        else if (!strcmp(argv[i], "-f")) {

            force_level = 1;
        }

        else if (!strcmp(argv[i], "-ff")) {

            force_level = 2;
        }
    }

    if (!is_git_repo()) {
        log_error("not a git repository");
        return 1;
    }

    ensure_config();
    load_config(&g_cfg);

    return gitauto_clean_core(
        force_level,
        quiet
    );
}