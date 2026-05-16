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
        ">nul 2>&1"
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

    // 去掉换行
    buf[strcspn(buf, "\r\n")] = 0;

    return 0;
}

/* ---------------- push ---------------- */

int gitauto_push(bool quiet) {
    // 确保 SSH 密钥已加载
    gitauto_ensure_ssh_key();

    // 自动配置 upstream
    if (!has_upstream()) {
        char branch[128];

        if (get_current_branch(branch, sizeof(branch)) != 0) {
            log_error("failed to get current branch");
            return 1;
        }

        char upstream_cmd[512];

        snprintf(
            upstream_cmd,
            sizeof(upstream_cmd),
            "git branch --set-upstream-to=origin/%s %s",
            branch,
            branch
        );

        log_warn("no upstream branch, auto configuring...");

        if (git_run(upstream_cmd, quiet) != 0) {
            log_error("failed to configure upstream");
            return 1;
        }
    }

    // pull
    if (git_run(
        "git pull --rebase --autostash",
        quiet
    ) != 0)
    {
        return 1;
    }

    // add
    if (git_run(
        "git add .",
        quiet
    ) != 0)
    {
        return 1;
    }

    // commit
    char msg[128];
    build_commit_msg(msg, sizeof(msg), "manual");

    char commit_cmd[256];

    snprintf(
        commit_cmd,
        sizeof(commit_cmd),
        "git commit -m \"%s\"",
        msg
    );

    int commit_ret = git_run(commit_cmd, quiet);

    // commit 失败不一定是真失败
    // 可能只是 nothing to commit
    if (commit_ret != 0) {
        log_warn("nothing to commit, skipping commit");
    }

    // push
    return git_run(
        "git push",
        quiet
    );
}

/* ---------------- handler ---------------- */

int cmd_push(int argc, char **argv) {
    bool quiet = false;

    for (int i = 2; i < argc; i++) {
        if (!strcmp(argv[i], "--quiet") ||
            !strcmp(argv[i], "-q"))
        {
            quiet = true;
        }
    }

    if (!is_git_repo()) {
        log_error("not a git repository");
        return 1;
    }

    ensure_config();
    ensure_gitignore();
    load_config(&g_cfg);

    return gitauto_push(quiet);
}