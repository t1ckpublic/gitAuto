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

/* ---------------- commit ---------------- */

static bool has_staged_changes(void) {
    int ret = system(
        "git diff --cached --quiet >nul 2>nul"
    );

    return ret != 0;
}

/* ---------------- confirm ---------------- */

static bool confirm_force_push(void) {
    char input[32];

    printf(
        "[WARN] About to force push.\n"
        "Type \"force\" to continue: "
    );

    if (!fgets(input, sizeof(input), stdin)) {
        return false;
    }

    input[strcspn(input, "\r\n")] = 0;

    return strcmp(input, "force") == 0;
}

/* ---------------- push ---------------- */

int gitauto_push(PushOptions *opt) {
    // 确保 SSH 密钥已加载
    gitauto_ensure_ssh_key();

    char branch[128];

    if (get_current_branch(branch, sizeof(branch)) != 0) {
        log_error("failed to get current branch");
        return 1;
    }

    bool need_upstream = !has_upstream();

    if (need_upstream) {
        log_warn("no upstream branch, will configure automatically");
    }

    // pull
    if (git_run(
        "git pull --rebase --autostash",
        opt->quiet
    ) != 0)
    {
        return 1;
    }

    // add
    if (git_run(
        "git add .",
        opt->quiet
    ) != 0)
    {
        return 1;
    }

    // commit
    if (has_staged_changes()) {

        char msg[128];

        if (opt->message) {
            strncpy(msg, opt->message, sizeof(msg) - 1);
            msg[sizeof(msg) - 1] = 0;
        }
        else {
            build_commit_msg(
                msg,
                sizeof(msg),
                "manual"
            );
        }

        char commit_cmd[512];

        snprintf(
            commit_cmd,
            sizeof(commit_cmd),
            "git commit -m \"%s\"",
            msg
        );

        if (git_run(commit_cmd, opt->quiet) != 0) {
            log_error("commit failed");
            return 1;
        }
    }
    else {
        log_warn("nothing to commit");
    }

    // push
    char push_cmd[512];

    if (need_upstream) {

        if (opt->force) {

            snprintf(
                push_cmd,
                sizeof(push_cmd),
                "git push -u origin %s --force",
                branch
            );
        }
        else if (opt->force_with_lease) {

            snprintf(
                push_cmd,
                sizeof(push_cmd),
                "git push -u origin %s --force-with-lease",
                branch
            );
        }
        else {

            snprintf(
                push_cmd,
                sizeof(push_cmd),
                "git push -u origin %s",
                branch
            );
        }
    }
    else {

        if (opt->force) {

            snprintf(
                push_cmd,
                sizeof(push_cmd),
                "git push --force"
            );
        }
        else if (opt->force_with_lease) {

            snprintf(
                push_cmd,
                sizeof(push_cmd),
                "git push --force-with-lease"
            );
        }
        else {

            snprintf(
                push_cmd,
                sizeof(push_cmd),
                "git push"
            );
        }
    }

    return git_run(push_cmd, opt->quiet);
}

/* ---------------- handler ---------------- */

int cmd_push(int argc, char **argv) {

    PushOptions opt = {0};

    for (int i = 2; i < argc; i++) {

        if (!strcmp(argv[i], "-q") ||
            !strcmp(argv[i], "--quiet"))
        {
            opt.quiet = true;
        }

        else if (!strcmp(argv[i], "-f")) {
            opt.force_with_lease = true;
        }

        else if (!strcmp(argv[i], "-ff")) {
            opt.force = true;
        }

        else if (!strcmp(argv[i], "-m")) {

            if (i + 1 >= argc) {
                log_error("missing commit message");
                return 1;
            }

            opt.message = argv[++i];
        }

        else {
            log_error("unknown option: %s", argv[i]);
            return 1;
        }
    }

    if (opt.force) {
        if (!confirm_force_push()) {
            log_warn("force push cancelled");
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

    return gitauto_push(&opt);
}