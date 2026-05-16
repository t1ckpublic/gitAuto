#include "gitauto.h"
#include "env.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ---------------- push ---------------- */

int gitauto_push(bool quiet) {
    // 确保 SSH 密钥已加载
    gitauto_ensure_ssh_key();

    char msg[128];
    build_commit_msg(msg, sizeof(msg), "manual");
    char cmd[512];
    snprintf(
        cmd, sizeof(cmd),
        "git pull --rebase --autostash && git add . && git commit -m \"%s\" && git push",
        msg
    );
    return git_run(cmd, quiet);
}

/* ---------------- handler ---------------- */
int CMD_PUSH(int argc, char **argv) {
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