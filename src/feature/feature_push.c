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