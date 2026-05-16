#include "gitauto.h"
#include "env.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define popen  _popen
#define pclose _pclose

/* ---------------- upstream ---------------- */

static bool has_upstream(void) {
    int ret = system(
        "git rev-parse --abbrev-ref --symbolic-full-name @{u} "
        ">nul 2>nul"
    );

    return ret == 0;
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

/* ---------------- changes ---------------- */

static bool has_staged_changes(void) {
    int ret = system(
        "git diff --cached --quiet >nul 2>nul"
    );

    return ret != 0;
}

/* ---------------- sync ---------------- */

int gitauto_sync(bool quiet) {

    // 确保 SSH 密钥已加载
    gitauto_ensure_ssh_key();

    char branch[128];

    if (get_current_branch(branch, sizeof(branch)) != 0) {
        log_error("failed to get current branch");
        return 1;
    }

    bool need_upstream = !has_upstream();

    if (need_upstream) {
        log_warn(
            "no upstream branch, "
            "will configure automatically"
        );
    }

    log_info("syncing branch %s...", branch);

    /* ---------------- pull ---------------- */

    if (git_run(
        "git pull --rebase --autostash",
        quiet
    ) != 0)
    {
        return 1;
    }

    /* ---------------- add ---------------- */

    if (git_run(
        "git add .",
        quiet
    ) != 0)
    {
        return 1;
    }

    /* ---------------- commit ---------------- */

    bool committed = false;

    if (has_staged_changes()) {

        char msg[128];

        build_commit_msg(
            msg,
            sizeof(msg),
            "sync"
        );

        char commit_cmd[512];

        snprintf(
            commit_cmd,
            sizeof(commit_cmd),
            "git commit -m \"%s\"",
            msg
        );

        if (git_run(commit_cmd, quiet) != 0) {
            log_error("commit failed");
            return 1;
        }

        committed = true;
    }

    /* ---------------- push ---------------- */

    char push_cmd[512];

    if (need_upstream) {

        snprintf(
            push_cmd,
            sizeof(push_cmd),
            "git push -u origin %s",
            branch
        );
    }
    else {

        snprintf(
            push_cmd,
            sizeof(push_cmd),
            "git push"
        );
    }

    if (git_run(push_cmd, quiet) != 0) {
        return 1;
    }

    /* ---------------- result ---------------- */

    if (committed) {
        log_info("repository synced");
    }
    else {
        log_info("repository already up to date");
    }

    return 0;
}

/* ---------------- handler ---------------- */

int cmd_sync(int argc, char **argv) {

    bool quiet = false;

    for (int i = 2; i < argc; i++) {

        if (!strcmp(argv[i], "-q") ||
            !strcmp(argv[i], "--quiet"))
        {
            quiet = true;
        }

        else {
            log_error("unknown option");
            return 1;
        }
    }

    if (!is_git_repo()) {
        log_error("not a git repository");
        return 1;
    }

    ensure_config();
    ensure_gitignore();
    load_config(&g_cfg);

    return gitauto_sync(quiet);
}