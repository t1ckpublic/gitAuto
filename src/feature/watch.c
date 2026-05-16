#include "gitauto.h"
#include "env.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ---------------- watch ---------------- */

static BOOL watch_change(void) {
    HANDLE h = CreateFileA(
        ".",
        FILE_LIST_DIRECTORY,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS,
        NULL);

    char buf[2048];
    DWORD ret;
    BOOL ok = ReadDirectoryChangesW(
        h, buf, sizeof(buf), TRUE,
        FILE_NOTIFY_CHANGE_LAST_WRITE |
        FILE_NOTIFY_CHANGE_FILE_NAME,
        &ret, NULL, NULL);

    CloseHandle(h);
    return ok;
}

int gitauto_start(void)
{
    gitauto_ensure_ssh_key();
    return 0;
}

void watch_loop(bool quiet) {
    // 在启动前确保 SSH 密钥已加载

    gitauto_start();

    while (running) {
        log_info("waiting...");
        watch_change();

        int left = g_cfg.countdown;
        while (left > 0 && running) {
            log_info("change detected, countdown %d sec", left--);
            Sleep(1000);
        }

        int retry = 3;
        while (retry--) {
            char msg[128];
            build_commit_msg(msg, sizeof(msg), "auto");
            char cmd[512];
            snprintf(
                cmd, sizeof(cmd),
                "git pull --rebase --autostash && git add . && git commit -m \"%s\" && git push",
                msg
            );
            int rc = git_run(cmd, quiet);
            if (rc == 0) break;
            if (retry == 0) {
                log_error("push failed");
                return;
            }
            log_warn("retry push");
        }
    }
}

/* ---------------- handler ---------------- */
int cmd_watch(int argc, char **argv)
{
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
    git_run("git pull", quiet);
    watch_loop(quiet);
    return 0;
}