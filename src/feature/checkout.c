#include "gitauto.h"
#include "env.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define popen  _popen
#define pclose _pclose

/* ---------------- utils ---------------- */

static int branch_exists_local(const char *branch) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd),
             "git show-ref --verify --quiet refs/heads/%s",
             branch);
    return system(cmd) == 0;
}

static int branch_exists_remote(const char *branch) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd),
             "git show-ref --verify --quiet refs/remotes/origin/%s",
             branch);
    return system(cmd) == 0;
}

/* ---------------- core ---------------- */

int gitauto_checkout(const char *branch, bool quiet) {

    if (!branch) {
        log_error("missing branch name");
        return 1;
    }

    char cmd[512];

    /* ---------------- CASE 1: local branch exists ---------------- */
    if (branch_exists_local(branch)) {

        snprintf(cmd, sizeof(cmd),
                 "git checkout %s",
                 branch);

        git_run(cmd, quiet);
    }

    /* ---------------- CASE 2: remote branch exists ---------------- */
    else if (branch_exists_remote(branch)) {

        snprintf(cmd, sizeof(cmd),
                 "git checkout -b %s origin/%s",
                 branch, branch);

        git_run(cmd, quiet);
    }

    /* ---------------- CASE 3: new branch ---------------- */
    else {

        snprintf(cmd, sizeof(cmd),
                 "git checkout -b %s",
                 branch);

        git_run(cmd, quiet);
    }

    /* ---------------- SAFE SYNC LOGIC ---------------- */

    if (branch_exists_remote(branch)) {

        /* remote exists → bind tracking safely */
        snprintf(cmd, sizeof(cmd),
                 "git branch --set-upstream-to=origin/%s %s",
                 branch, branch);

        git_run(cmd, true);

        git_run("git pull", true);
    }
    else {

        /* remote not exists → create upstream properly */
        snprintf(cmd, sizeof(cmd),
                 "git push -u origin %s",
                 branch);

        git_run(cmd, true);
    }

    return 0;
}

/* ---------------- handler ---------------- */

int cmd_checkout(int argc, char **argv) {

    bool quiet = false;
    const char *branch = NULL;

    for (int i = 2; i < argc; i++) {

        if (!strcmp(argv[i], "-q") ||
            !strcmp(argv[i], "--quiet")) {
            quiet = true;
        }
        else {
            branch = argv[i];
        }
    }

    if (!is_git_repo()) {
        log_error("not a git repository");
        return 1;
    }

    ensure_config();
    load_config(&g_cfg);

    if (!branch) {
        log_error("usage: gitauto checkout <branch>");
        return 1;
    }

    return gitauto_checkout(branch, quiet);
}