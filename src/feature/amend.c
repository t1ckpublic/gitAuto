#include "gitauto.h"
#include "env.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ---------------- utils ---------------- */

static bool has_changes(void) {

    int ret1 = system(
        "git diff --quiet >nul 2>nul"
    );

    int ret2 = system(
        "git diff --cached --quiet >nul 2>nul"
    );

    return ret1 != 0 || ret2 != 0;
}

/* ---------------- core ---------------- */

static int gitauto_amend_core(
    const char *message,
    bool quiet
) {

    /* no message + no changes */

    if (!message && !has_changes()) {

        log_warn("nothing to amend");
        return 0;
    }

    /* ---------------- stage ---------------- */

    if (has_changes()) {

        if (git_run(
                "git add .",
                quiet
            ) != 0) {

            return 1;
        }
    }

    /* ---------------- amend ---------------- */

    char cmd[512];

    if (message) {

        snprintf(
            cmd,
            sizeof(cmd),
            "git commit --amend -m \"%s\"",
            message
        );
    }
    else {

        snprintf(
            cmd,
            sizeof(cmd),
            "git commit --amend --no-edit"
        );
    }

    if (git_run(cmd, quiet) != 0) {
        return 1;
    }

    /* ---------------- sync remote ---------------- */

    if (git_run(
            "git push --force-with-lease",
            quiet
        ) != 0) {

        log_warn(
            "local amend succeeded, "
            "but remote sync failed"
        );

        return 1;
    }

    /* ---------------- done ---------------- */

    if (!quiet) {

        printf(
            "[amend] updated latest commit\n"
        );

        printf(
            "[amend] remote history synced\n"
        );
    }

    return 0;
}

/* ---------------- handler ---------------- */

int cmd_amend(int argc, char **argv) {

    bool quiet = false;
    const char *message = NULL;

    for (int i = 2; i < argc; i++) {

        if (!strcmp(argv[i], "-q") ||
            !strcmp(argv[i], "--quiet")) {

            quiet = true;
        }

        else if (!strcmp(argv[i], "-m")) {

            if (i + 1 >= argc) {
                log_error("missing commit message");
                return 1;
            }

            message = argv[++i];
        }

        else {

            log_error(
                "unknown option: %s",
                argv[i]
            );

            return 1;
        }
    }

    if (!is_git_repo()) {
        log_error("not a git repository");
        return 1;
    }

    ensure_config();
    load_config(&g_cfg);

    return gitauto_amend_core(
        message,
        quiet
    );
}