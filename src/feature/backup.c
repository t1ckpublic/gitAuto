#include "gitauto.h"
#include "env.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ---------------- utils ---------------- */

static int branch_exists_local(const char *branch) {
    char cmd[512];
    snprintf(cmd, sizeof(cmd),
             "git show-ref --verify --quiet refs/heads/%s",
             branch);
    return system(cmd) == 0;
}

static int branch_exists_remote(const char *branch) {
    char cmd[512];
    snprintf(cmd, sizeof(cmd),
             "git show-ref --verify --quiet refs/remotes/origin/%s",
             branch);
    return system(cmd) == 0;
}

static int get_current_branch(char *buf, size_t size) {
    FILE *fp = popen(
        "git branch --show-current",
        "r"
    );

    if (!fp) {
        return 1;
    }

    if (!fgets(buf, size, fp)) {
        pclose(fp);
        return 1;
    }

    pclose(fp);

    buf[strcspn(buf, "\r\n")] = 0;

    return 0;
}

/* ---------------- core ---------------- */

static int gitauto_backup_core(bool quiet) {

    char current[128];
    get_current_branch(current, sizeof(current));

    if (!current[0]) {
        log_error("cannot get current branch");
        return 1;
    }

    char backup[256];
    snprintf(backup, sizeof(backup),
             "%s-backup",
             current);

    char cmd[512];

    /* ---------------- 1. stage changes ---------------- */
    git_run("git add -A", true);

    /* ---------------- 2. commit snapshot ---------------- */
    git_run("git commit -m \"gitauto backup snapshot\"", true);

    /* ---------------- 3. refresh remote info ---------------- */
    git_run("git fetch origin", true);

    int local_exists  = branch_exists_local(backup);
    int remote_exists = branch_exists_remote(backup);

    /* ---------------- 4. sync logic ---------------- */

    if (local_exists && remote_exists) {

        /* update backup branch pointer */
        snprintf(cmd, sizeof(cmd),
                 "git branch -f %s HEAD",
                 backup);
        git_run(cmd, true);

        /* sync remote safely */
        snprintf(cmd, sizeof(cmd),
                 "git push origin %s --force-with-lease",
                 backup);
        git_run(cmd, true);
    }
    else if (!local_exists && remote_exists) {

        /* fetch remote backup first */
        snprintf(cmd, sizeof(cmd),
                 "git checkout -b %s origin/%s",
                 backup, backup);
        git_run(cmd, true);

        /* move to latest HEAD */
        snprintf(cmd, sizeof(cmd),
                 "git branch -f %s HEAD",
                 backup);
        git_run(cmd, true);

        snprintf(cmd, sizeof(cmd),
                 "git push origin %s --force-with-lease",
                 backup);
        git_run(cmd, true);
    }
    else if (local_exists && !remote_exists) {

        /* only local exists → create remote */
        snprintf(cmd, sizeof(cmd),
                 "git branch -f %s HEAD",
                 backup);
        git_run(cmd, true);

        snprintf(cmd, sizeof(cmd),
                 "git push -u origin %s",
                 backup);
        git_run(cmd, true);
    }
    else {

        /* brand new backup branch */
        snprintf(cmd, sizeof(cmd),
                 "git branch -f %s HEAD",
                 backup);
        git_run(cmd, true);

        snprintf(cmd, sizeof(cmd),
                 "git push -u origin %s",
                 backup);
        git_run(cmd, true);
    }

    if (!quiet) {
        printf("[gitAuto] backup synced -> %s\n", backup);
    }

    return 0;
}

/* ---------------- handler ---------------- */

int cmd_backup(int argc, char **argv) {

    bool quiet = false;

    for (int i = 2; i < argc; i++) {
        if (!strcmp(argv[i], "-q") ||
            !strcmp(argv[i], "--quiet")) {
            quiet = true;
        }
    }

    if (!is_git_repo()) {
        log_error("not a git repository");
        return 1;
    }

    ensure_config();
    load_config(&g_cfg);

    return gitauto_backup_core(quiet);
}