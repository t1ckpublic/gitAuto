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
    FILE *fp = popen("git branch --show-current", "r");

    if (!fp) return 1;

    if (!fgets(buf, size, fp)) {
        pclose(fp);
        return 1;
    }

    pclose(fp);

    buf[strcspn(buf, "\r\n")] = 0;
    return 0;
}

/* ---------------- core backup ---------------- */

static int gitauto_backup_core(bool quiet) {

    char current[128];
    if (get_current_branch(current, sizeof(current))) {
        log_error("cannot get current branch");
        return 1;
    }

    if (!current[0]) {
        log_error("invalid branch");
        return 1;
    }

    char backup[256];
    snprintf(backup, sizeof(backup),
             "%s-backup",
             current);

    char cmd[512];

    /* stage */
    git_run("git add -A", true);

    /* snapshot commit */
    git_run("git commit -m \"gitAuto backup snapshot\"", true);

    /* sync remote state */
    git_run("git fetch origin", true);

    int local_exists  = branch_exists_local(backup);
    int remote_exists = branch_exists_remote(backup);

    if (local_exists && remote_exists) {

        snprintf(cmd, sizeof(cmd),
                 "git branch -f %s HEAD",
                 backup);
        git_run(cmd, true);

        snprintf(cmd, sizeof(cmd),
                 "git push origin %s --force-with-lease",
                 backup);
        git_run(cmd, true);
    }
    else if (!local_exists && remote_exists) {

        snprintf(cmd, sizeof(cmd),
                 "git checkout -b %s origin/%s",
                 backup, backup);
        git_run(cmd, true);

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

/* ---------------- core restore ---------------- */
static int gitauto_backup_restore_core(
    const char *target,
    bool quiet
) {

    char current[128];

    if (get_current_branch(current, sizeof(current))) {
        log_error("cannot get current branch");
        return 1;
    }

    char from_branch[256];

    /* ---------------- default restore ---------------- */
    if (!target || target[0] == '\0') {

        snprintf(from_branch, sizeof(from_branch),
                 "%s-backup",
                 current);
    }
    else {

        /* restore from arbitrary branch */
        snprintf(from_branch, sizeof(from_branch),
                 "%s",
                 target);
    }

    /* ---------------- existence check ---------------- */
    if (!branch_exists_local(from_branch)) {
        log_error("branch not found: %s", from_branch);
        return 1;
    }

    char cmd[512];

    /* ---------------- IMPORTANT ----------------
     * reset CURRENT branch
     * do NOT checkout target
     * ------------------------------------------ */

    /* restore snapshot */
    snprintf(cmd, sizeof(cmd),
            "git reset --hard refs/heads/%s",
            from_branch);

    if (git_run(cmd, quiet) != 0) {
        return 1;
    }

    /* force sync restore */
    snprintf(cmd, sizeof(cmd),
            "git push --force-with-lease");

    if (git_run(cmd, quiet) != 0) {
        return 1;
    }

    if (!quiet) {
        printf(
            "[gitAuto] restore synced from %s\n",
            from_branch
        );
    }

    return 0;

    /* create restore checkpoint */
    git_run(
        "git commit --allow-empty "
        "-m \"gitAuto restore snapshot\"",
        true
    );

    return 0;
}

/* ---------------- handler ---------------- */

int cmd_backup(int argc, char **argv) {

    bool assume_yes = false;
    bool quiet = false;

    /* action */
    int is_restore = 0;
    const char *target = NULL;

    for (int i = 2; i < argc; i++) {

        if (!strcmp(argv[i], "-q") ||
            !strcmp(argv[i], "--quiet")) {

            quiet = true;
        }

        /* skip confirm */
        else if (!strcmp(argv[i], "-y") ||
                 !strcmp(argv[i], "--yes")) {

            assume_yes = true;
        }

        /* alias: restore / re */
        else if (!strcmp(argv[i], "restore") ||
                 !strcmp(argv[i], "re")) {

            is_restore = 1;
        }

        /* target branch */
        else {

            target = argv[i];
        }
    }

    if (!is_git_repo()) {
        log_error("not a git repository");
        return 1;
    }

    ensure_config();
    load_config(&g_cfg);

    /* ---------------- confirm dangerous op ---------------- */

    if (is_restore && !assume_yes) {

        char answer[16];

        printf(
            "[WARN] restore will overwrite current branch\n"
            "[WARN] and force push remote history\n"
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

    /* ---------------- dispatch ---------------- */

    if (is_restore) {
        return gitauto_backup_restore_core(
            target,
            quiet
        );
    }

    return gitauto_backup_core(quiet);
}