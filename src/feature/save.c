#include "gitauto.h"
#include "env.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ---------------- core ---------------- */

int gitauto_save(const char *msg, bool quiet) {

    if (!is_git_repo()) {
        log_error("not a git repository");
        return 1;
    }

    char cmd[512];

    /* ---------------- with message ---------------- */
    if (msg && strlen(msg) > 0) {

        snprintf(cmd, sizeof(cmd),
                 "git stash push -u -m \"gitauto: %s\"",
                 msg);
    }
    else {

        /* ---------------- default save ---------------- */
        snprintf(cmd, sizeof(cmd),
                 "git stash push -u");
    }

    return git_run(cmd, quiet);
}

/* ---------------- handler ---------------- */
int cmd_save(int argc, char **argv) {

    bool quiet = false;
    const char *msg = NULL;

    for (int i = 2; i < argc; i++) {

        if (!strcmp(argv[i], "-q") ||
            !strcmp(argv[i], "--quiet")) {
            quiet = true;
        }
        else if (!strcmp(argv[i], "-m") && i + 1 < argc) {
            msg = argv[i + 1];
            i++;  // skip next arg
        }
        else if (argv[i][0] != '-') {
            /* fallback: allow gitauto save "msg" */
            msg = argv[i];
        }
    }

    if (!is_git_repo()) {
        log_error("not a git repository");
        return 1;
    }

    ensure_config();
    load_config(&g_cfg);

    return gitauto_save(msg, quiet);
}