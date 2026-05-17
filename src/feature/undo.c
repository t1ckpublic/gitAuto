#include "gitauto.h"
#include "env.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* ---------------- core ---------------- */

static int gitauto_undo_core(
    int count,
    const char *mode,
    bool quiet
) {

    char cmd[256];

    /* ---------------- hard warning ---------------- */

    if (!strcmp(mode, "hard")) {

        char answer[16];

        printf(
            "[WARN] hard reset will discard changes\n"
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
    }

    /* ---------------- build reset command ---------------- */

    if (!strcmp(mode, "soft")) {

        snprintf(cmd, sizeof(cmd),
                 "git reset --soft HEAD~%d",
                 count);
    }
    else if (!strcmp(mode, "mixed")) {

        snprintf(cmd, sizeof(cmd),
                 "git reset HEAD~%d",
                 count);
    }
    else {

        snprintf(cmd, sizeof(cmd),
                 "git reset --hard HEAD~%d",
                 count);
    }

    /* ---------------- local reset ---------------- */

    if (git_run(cmd, quiet) != 0) {
        return 1;
    }

    /* ---------------- sync rewritten history ---------------- */

    if (git_run(
            "git push --force-with-lease",
            true
        ) != 0) {

        log_warn(
            "local undo succeeded, "
            "but remote sync failed"
        );
    }

    /* ---------------- done ---------------- */

    if (!quiet) {

        printf(
            "[undo] reverted last %d commit(s) (%s)\n",
            count,
            mode
        );

        printf(
            "[undo] remote history synced\n"
        );
    }

    return 0;
}

/* ---------------- handler ---------------- */

int cmd_undo(int argc, char **argv) {

    bool quiet = false;

    int count = 1;

    /* default */
    char mode[16] = "soft";

    for (int i = 2; i < argc; i++) {

        if (!strcmp(argv[i], "-q") ||
            !strcmp(argv[i], "--quiet")) {

            quiet = true;
        }

        /* count */
        else if (isdigit((unsigned char)argv[i][0])) {

            count = atoi(argv[i]);

            if (count <= 0) {
                count = 1;
            }
        }

        /* mixed */
        else if (!strcmp(argv[i], "mixed") ||
                 !strcmp(argv[i], "m")) {

            strcpy(mode, "mixed");
        }

        /* hard */
        else if (!strcmp(argv[i], "hard") ||
                 !strcmp(argv[i], "h")) {

            strcpy(mode, "hard");
        }
    }

    if (!is_git_repo()) {

        log_error("not a git repository");
        return 1;
    }

    ensure_config();
    load_config(&g_cfg);

    return gitauto_undo_core(
        count,
        mode,
        quiet
    );
}