#include "gitauto.h"
#include "env.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define popen  _popen
#define pclose _pclose

/* ---------------- utils ---------------- */

static int extract_repo_name(const char *url, char *out, size_t size) {
    const char *end = url + strlen(url);

    while (end > url && *(end - 1) == '/') {
        end--;
    }

    const char *start = end;

    while (start > url && *(start - 1) != '/') {
        start--;
    }

    const char *dot = strstr(start, ".git");
    size_t len = dot ? (size_t)(dot - start) : strlen(start);

    if (len >= size) return 1;

    strncpy(out, start, len);
    out[len] = '\0';

    return 0;
}

/* ---------------- clone ---------------- */

int gitauto_clone(const char *url, bool quiet) {

    if (!url) {
        log_error("missing repository url");
        return 1;
    }

    char cmd[512];

    snprintf(
        cmd,
        sizeof(cmd),
        "git clone %s",
        url
    );

    log_info("cloning repository...");

    if (git_run(cmd, quiet) != 0) {
        log_error("git clone failed");
        return 1;
    }

    char repo[128];

    if (extract_repo_name(url, repo, sizeof(repo)) != 0) {
        log_warn("failed to parse repo name");
        return 0;
    }

    char cd_cmd[256];

    snprintf(
        cd_cmd,
        sizeof(cd_cmd),
        "cd %s",
        repo
    );


    log_info("enter repository: %s", repo);

    log_info("syncing repository...");

    /* 这里假设 clone 后直接 sync */
    snprintf(
        cmd,
        sizeof(cmd),
        "cd %s && gitauto sync",
        repo
    );

    return system(cmd);
}

/* ---------------- handler ---------------- */

int cmd_clone(int argc, char **argv) {

    if (argc < 3) {
        log_error("usage: gitauto clone <url>");
        return 1;
    }

    bool quiet = false;
    const char *url = NULL;

    for (int i = 2; i < argc; i++) {

        if (!strcmp(argv[i], "-q") ||
            !strcmp(argv[i], "--quiet"))
        {
            quiet = true;
        }
        else {
            url = argv[i];
        }
    }

    if (!url) {
        log_error("missing url");
        return 1;
    }

    return gitauto_clone(url, quiet);
}