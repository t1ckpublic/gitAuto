#include "gitauto.h"
#include "env.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ---------------- repo init ---------------- */

int gitauto_init(void)
{
    // 确保 SSH 密钥已加载
    gitauto_ensure_ssh_key();

    char url[256];
    char cmd[512];

    /* 如果不是 git 仓库，则初始化 */
    if (!is_git_repo()) {
        log_info("initializing git repository...");

        if (git_run("git init", false) != 0) {
            log_error("git init failed");
            return 1;
        }
    }

    /* 已存在 commit */
    if (git_run("git rev-parse --verify HEAD >nul 2>&1", true) == 0) {
        log_warn("repository already initialized");
        return 0;
    }

    /* 输入远程仓库 */
    printf("Enter remote repository URL: ");
    fflush(stdout);

    if (!fgets(url, sizeof(url), stdin)) {
        log_error("failed to read url");
        return 1;
    }

    url[strcspn(url, "\r\n")] = 0;

    if (url[0] == '\0') {
        log_error("empty url");
        return 1;
    }

    /* main 分支 */
    git_run("git branch -M main", false);

    /* 添加 remote */
    snprintf(cmd, sizeof(cmd),
             "git remote add origin %s", url);

    if (git_run(cmd, false) != 0) {
        log_error("failed to add remote");
        return 1;
    }

    /* fetch 远程 */
    log_info("fetching remote repository...");
    git_run("git fetch origin", false);

    /*
     * 检查远程 main 是否存在
     * 存在则说明远程不是空仓库
     */
    if (git_run("git rev-parse --verify origin/main >nul 2>&1", true) == 0) {

        log_warn("remote repository is not empty");
        log_info("pulling remote history...");

        /*
         * 合并远程历史
         * --allow-unrelated-histories
         * 防止 root commit 冲突
         */
        if (git_run(
                "git pull origin main "
                "--allow-unrelated-histories "
                "--rebase --autostash",
                false) != 0) {

            log_error("failed to pull remote repository");
            return 1;
        }
    }

    /* 创建初始 commit */
    if (git_run(
            "git commit --allow-empty "
            "-m \"[init] initial commit\"",
            false) != 0) {

        log_error("failed to create initial commit");
        return 1;
    }

    /* push */
    log_info("pushing to remote repository...");

    if (git_run("git push -u origin main", false) != 0) {
        log_error("initial push failed");
        return 1;
    }

    log_info("repository initialized");

    return 0;
}

/* ---------------- handler ---------------- */
int cmd_init(int argc, char **argv)
{
    return gitauto_init();
}