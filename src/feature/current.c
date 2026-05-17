#include "gitauto.h"
#include "env.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define popen  _popen
#define pclose _pclose

/* ---------------- core ---------------- */

static int gitauto_current_core(void) {

    FILE *fp = popen(
        "git branch --show-current",
        "r"
    );

    if (!fp) {
        log_error("failed to get current branch");
        return 1;
    }

    char branch[128];

    if (!fgets(branch, sizeof(branch), fp)) {

        pclose(fp);

        log_error("failed to read branch name");
        return 1;
    }

    pclose(fp);

    branch[strcspn(branch, "\r\n")] = 0;

    if (!branch[0]) {

        log_error("detached HEAD");
        return 1;
    }

    printf("%s\n", branch);

    return 0;
}

/* ---------------- handler ---------------- */

int cmd_current(int argc, char **argv) {

    (void)argc;
    (void)argv;

    if (!is_git_repo()) {
        log_error("not a git repository");
        return 1;
    }

    return gitauto_current_core();
}