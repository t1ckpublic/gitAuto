#include "gitauto.h"
#include "env.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ---------------- link ---------------- */

int gitauto_link(const char *url, bool quiet) {
    char cmd[1024];

    /* 移除旧 origin（忽略错误） */
    git_run(
        "git remote remove origin >nul 2>nul",
        true
    );

    /* 添加新的 origin */
    snprintf(
        cmd,
        sizeof(cmd),
        "git remote add origin \"%s\"",
        url
    );

    if (git_run(cmd, quiet) != 0) {
        return 1;
    }

    log_info("fetching remote refs...");

    /* 拉取远程 refs */
    if (git_run(
        "git fetch origin --prune",
        quiet
    ) != 0)
    {
        log_error("failed to fetch remote refs");
        return 1;
    }

    log_info("remote repository linked successfully");

    return 0;
}

/* ---------------- handler ---------------- */

int cmd_link(int argc, char **argv) {
    bool quiet = false;
    char url[512];

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

    printf("Enter remote repository URL: ");

    if (!fgets(url, sizeof(url), stdin)) {
        log_error("failed to read repository URL");
        return 1;
    }

    /* 去掉换行 */
    url[strcspn(url, "\r\n")] = 0;

    if (strlen(url) == 0) {
        log_error("repository URL cannot be empty");
        return 1;
    }

    ensure_config();
    ensure_gitignore();
    load_config(&g_cfg);

    return gitauto_link(url, quiet);
}
