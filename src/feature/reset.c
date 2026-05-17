#include "gitauto.h"
#include "env.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ---------------- confirm ---------------- */

static bool confirm_reset(int count) {

    char input[32];

    if (count <= 1) {

        printf(
            "[WARN] all uncommitted changes will be lost\n"
            "continue? (y/N): "
        );
    }
    else {

        printf(
            "[WARN] this will discard last %d commit(s)\n"
            "[WARN] all uncommitted changes will be lost\n"
            "continue? (y/N): ",
            count
        );
    }

    if (!fgets(input, sizeof(input), stdin)) {
        return false;
    }

    input[strcspn(input, "\r\n")] = 0;

    return (
        input[0] == 'y' ||
        input[0] == 'Y'
    );
}

/* ---------------- core ---------------- */

static int gitauto_reset_core(
    int count,
    bool quiet
) {

    if (!confirm_reset(count)) {

        printf("cancelled\n");
        return 1;
    }

    char cmd[256];

    if (count <= 0) {
        count = 1;
    }

    /* ---------------- build command ---------------- */

    if (count == 1) {

        snprintf(
            cmd,
            sizeof(cmd),
            "git reset --hard HEAD"
        );
    }
    else {

        snprintf(
            cmd,
            sizeof(cmd),
            "git reset --hard HEAD~%d",
            count
        );
    }

    /* ---------------- execute ---------------- */

    if (git_run(cmd, quiet) != 0) {
        return 1;
    }

    /* ---------------- done ---------------- */

    if (!quiet) {

        if (count == 1) {

            printf(
                "[reset] reverted working tree to HEAD\n"
            );
        }
        else {

            printf(
                "[reset] reverted repository to HEAD~%d\n",
                count
            );
        }
    }

    return 0;
}

/* ---------------- handler ---------------- */

int cmd_reset(int argc, char **argv) {

    bool quiet = false;
    int count = 1;

    for (int i = 2; i < argc; i++) {

        if (!strcmp(argv[i], "-q") ||
            !strcmp(argv[i], "--quiet")) {

            quiet = true;
        }

        else {

            count = atoi(argv[i]);

            if (count <= 0) {
                log_error(
                    "invalid reset count: %s",
                    argv[i]
                );
                return 1;
            }
        }
    }

    if (!is_git_repo()) {
        log_error("not a git repository");
        return 1;
    }

    ensure_config();
    load_config(&g_cfg);

    return gitauto_reset_core(
        count,
        quiet
    );
}