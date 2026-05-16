#include "gitauto.h"
#include "env.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define popen  _popen
#define pclose _pclose

/* ---------------- branch ---------------- */

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

/* ---------------- confirm ---------------- */

static bool confirm_clean_pull(void) {
    char input[32];

    printf(
        "[WARN] This will DELETE all local changes "
        "and untracked files.\n"
        "Type \"clean\" to continue: "
    );

    if (!fgets(input, sizeof(input), stdin)) {
        return false;
    }

    input[strcspn(input, "\r\n")] = 0;

    return strcmp(input, "clean") == 0;
}

/* ---------------- pull ---------------- */

int gitauto_pull(PullOptions *opt) {

    gitauto_ensure_ssh_key();
    
    char branch[128];

    if (get_current_branch(branch, sizeof(branch)) != 0) {
        log_error("failed to get current branch");
        return 1;
    }

    log_info("pulling branch %s...", branch);

    /* ---------------- clean mode ---------------- */

    if (opt->clean) {

        if (git_run(
            "git reset --hard",
            opt->quiet
        ) != 0)
        {
            return 1;
        }

        if (git_run(
            "git clean -fd",
            opt->quiet
        ) != 0)
        {
            return 1;
        }

        char cmd[512];

        snprintf(
            cmd,
            sizeof(cmd),
            "git fetch origin && "
            "git reset --hard origin/%s",
            branch
        );

        return git_run(cmd, opt->quiet);
    }

    /* ---------------- force sync ---------------- */

    if (opt->force) {

        if (git_run(
            "git fetch origin",
            opt->quiet
        ) != 0)
        {
            return 1;
        }

        char cmd[512];

        snprintf(
            cmd,
            sizeof(cmd),
            "git reset --hard origin/%s",
            branch
        );

        return git_run(cmd, opt->quiet);
    }

    /* ---------------- normal pull ---------------- */

    return git_run(
        "git pull --rebase --autostash",
        opt->quiet
    );
}

/* ---------------- handler ---------------- */

int cmd_pull(int argc, char **argv) {

    PullOptions opt = {0};

    for (int i = 2; i < argc; i++) {

        if (!strcmp(argv[i], "-q") ||
            !strcmp(argv[i], "--quiet"))
        {
            opt.quiet = true;
        }

        else if (!strcmp(argv[i], "-f")) {
            opt.force = true;
        }

        else if (!strcmp(argv[i], "-ff")) {
            opt.clean = true;
        }

        else {
            log_error("unknown option");
            return 1;
        }
    }

    if (opt.clean) {
        if (!confirm_clean_pull()) {
            log_warn("clean pull cancelled");
            return 1;
        }
    }

    if (!is_git_repo()) {
        log_error("not a git repository");
        return 1;
    }

    ensure_config();
    load_config(&g_cfg);

    return gitauto_pull(&opt);
}