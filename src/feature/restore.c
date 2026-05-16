#include "gitauto.h"
#include "env.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* =========================================================
 * stash list
 * ========================================================= */
static int restore_list(bool quiet) {
    return git_run("git stash list", quiet);
}

/* =========================================================
 * restore by message match
 * ========================================================= */
static int restore_by_msg(const char *msg, bool quiet) {

    if (!msg) {
        log_error("missing message");
        return 1;
    }

    FILE *fp = popen("git stash list", "r");
    if (!fp) return 1;

    char line[512];
    int index = 0;
    int found = -1;

    while (fgets(line, sizeof(line), fp)) {

        if (strstr(line, msg)) {
            found = index;
            break;
        }

        index++;
    }

    pclose(fp);

    if (found == -1) {
        log_error("stash not found");
        return 1;
    }

    char cmd[256];
    snprintf(cmd, sizeof(cmd),
             "git stash pop stash@{%d}",
             found);

    return git_run(cmd, quiet);
}

/* =========================================================
 * index restore (pop / apply)
 * ========================================================= */
static int restore_index(const char *target, bool apply, bool quiet) {

    char cmd[256];

    if (!target) {

        snprintf(cmd, sizeof(cmd),
                 apply ? "git stash apply"
                       : "git stash pop");
    }
    else {

        snprintf(cmd, sizeof(cmd),
                 apply ? "git stash apply stash@{%s}"
                       : "git stash pop stash@{%s}",
                 target);
    }

    return git_run(cmd, quiet);
}

/* =========================================================
 * main dispatcher
 * ========================================================= */
int gitauto_restore(const char *target, bool apply, bool quiet) {

    if (!is_git_repo()) {
        log_error("not a git repository");
        return 1;
    }

    /* list */
    if (target && strcmp(target, "list") == 0) {
        return restore_list(quiet);
    }

    /* message restore */
    if (target && target[0] != '\0') {

        /* number = index restore */
        int is_number = (strspn(target, "0123456789") == strlen(target));

        if (!is_number) {
            return restore_by_msg(target, quiet);
        }
    }

    /* default index restore */
    return restore_index(target, apply, quiet);
}

/* =========================================================
 * CLI handler: ga re
 * ========================================================= */
int cmd_restore(int argc, char **argv) {

    bool quiet = false;
    bool apply = false;
    const char *arg = NULL;

    for (int i = 2; i < argc; i++) {

        /* quiet */
        if (!strcmp(argv[i], "-q") ||
            !strcmp(argv[i], "--quiet")) {
            quiet = true;
        }

        /* short apply */
        else if (!strcmp(argv[i], "-a") ||
                 !strcmp(argv[i], "--apply")) {
            apply = true;
        }

        /* list alias */
        else if (!strcmp(argv[i], "list") ||
                 !strcmp(argv[i], "l")) {
            arg = "list";
        }

        /* normal arg */
        else {
            arg = argv[i];
        }
    }

    if (!is_git_repo()) {
        log_error("not a git repository");
        return 1;
    }

    ensure_config();
    load_config(&g_cfg);

    return gitauto_restore(arg, apply, quiet);
}