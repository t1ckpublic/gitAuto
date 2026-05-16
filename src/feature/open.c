#include "gitauto.h"
#include "env.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#define OPEN_CMD "start"
#elif __APPLE__
#define OPEN_CMD "open"
#else
#define OPEN_CMD "xdg-open"
#endif

/* ---------------- get remote ---------------- */

static int get_remote_url(char *buf, size_t size) {

    FILE *fp = popen("git config --get remote.origin.url", "r");
    if (!fp) return 1;

    if (!fgets(buf, size, fp)) {
        pclose(fp);
        return 1;
    }

    pclose(fp);

    buf[strcspn(buf, "\r\n")] = 0;
    return 0;
}

/* ---------------- normalize url ---------------- */

static void normalize_url(char *url) {

    /* remove .git suffix */
    char *dot = strstr(url, ".git");
    if (dot) *dot = 0;

    /* SSH -> HTTPS */
    if (strncmp(url, "git@", 4) == 0) {

        char tmp[512];

        /* git@github.com:user/repo */
        char *colon = strchr(url, ':');
        if (!colon) return;

        *colon = '/';

        snprintf(tmp, sizeof(tmp),
                 "https://%s", url + 4);

        strcpy(url, tmp);
    }
}

/* ---------------- core ---------------- */

int gitauto_open(bool quiet) {

    if (!is_git_repo()) {
        log_error("not a git repository");
        return 1;
    }

    char url[512];

    if (get_remote_url(url, sizeof(url)) != 0) {
        log_error("failed to get origin url");
        return 1;
    }

    normalize_url(url);

    char cmd[600];

    snprintf(cmd, sizeof(cmd),
             "%s %s",
             OPEN_CMD, url);

    system(cmd);

    if (!quiet) {
        printf("[gitAuto] opened repo: %s\n", url);
    }

    return 0;
}

/* ---------------- handler ---------------- */
int cmd_open(int argc, char **argv) {

    bool quiet = false;

    for (int i = 2; i < argc; i++) {
        if (!strcmp(argv[i], "-q") ||
            !strcmp(argv[i], "--quiet")) {
            quiet = true;
        }
    }

    if (!is_git_repo()) {
        log_error("not a git repository");
        return 1;
    }

    ensure_config();
    load_config(&g_cfg);

    return gitauto_open(quiet);
}