#include "gitauto.h"
#include "env.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define popen  _popen
#define pclose _pclose

/* ---------------- model ---------------- */

typedef struct {
    char name[256];
    char status[16];
} BranchItem;

/* ---------------- utils ---------------- */

static int max_int(int a, int b) {
    return a > b ? a : b;
}

/* ---------------- collect ---------------- */

static int collect_branches(BranchItem *items, int max_items, int *count) {

    FILE *fp = popen("git branch -a", "r");
    if (!fp) return 1;

    char line[256];
    *count = 0;

    while (fgets(line, sizeof(line), fp)) {

        if (*count >= max_items) break;

        line[strcspn(line, "\r\n")] = 0;

        BranchItem *it = &items[*count];
        memset(it, 0, sizeof(*it));

        /* ---------------- remove HEAD symbolic ref ---------------- */
        if (strstr(line, "HEAD ->")) {
            continue;
        }

        /* ---------------- current branch ---------------- */
        if (line[0] == '*') {
            snprintf(it->name, sizeof(it->name), "%s", line + 2);
            strcpy(it->status, "current");
            (*count)++;
            continue;
        }

        /* ---------------- remote branches ---------------- */
        if (strstr(line, "remotes/")) {

            const char *p = strstr(line, "remotes/");
            p += strlen("remotes/");

            /* origin/main -> main */
            const char *slash = strchr(p, '/');
            if (slash) {
                snprintf(it->name, sizeof(it->name), "%s", slash + 1);
            } else {
                snprintf(it->name, sizeof(it->name), "%s", p);
            }

            strcpy(it->status, "remote");
            (*count)++;
            continue;
        }

        /* ---------------- local branches ---------------- */
        snprintf(it->name, sizeof(it->name), "%s", line);
        strcpy(it->status, "local");

        (*count)++;
    }

    pclose(fp);
    return 0;
}

/* ---------------- draw helpers ---------------- */

static void draw_line(int name_w, int status_w) {
    printf("┌");
    for (int i = 0; i < name_w + 2; i++) printf("─");
    printf("┬");
    for (int i = 0; i < status_w + 2; i++) printf("─");
    printf("┐\n");
}

static void draw_sep(int name_w, int status_w) {
    printf("├");
    for (int i = 0; i < name_w + 2; i++) printf("─");
    printf("┼");
    for (int i = 0; i < status_w + 2; i++) printf("─");
    printf("┤\n");
}

static void draw_bottom(int name_w, int status_w) {
    printf("└");
    for (int i = 0; i < name_w + 2; i++) printf("─");
    printf("┴");
    for (int i = 0; i < status_w + 2; i++) printf("─");
    printf("┘\n");
}

/* ---------------- branch ---------------- */

int gitauto_branch(bool quiet) {

    (void)quiet;

    BranchItem items[128];
    int count = 0;

    if (collect_branches(items, 128, &count) != 0) {
        log_error("failed to get branches");
        return 1;
    }

    int name_w = (int)strlen("Branch");
    int status_w = (int)strlen("Status");

    for (int i = 0; i < count; i++) {
        name_w = max_int(name_w, (int)strlen(items[i].name));
        status_w = max_int(status_w, (int)strlen(items[i].status));
    }

    /* ---------------- header ---------------- */

    draw_line(name_w, status_w);

    printf("│ %-*s │ %-*s │\n",
           name_w, "Branch",
           status_w, "Status");

    draw_sep(name_w, status_w);

    /* ---------------- rows ---------------- */

    for (int i = 0; i < count; i++) {
        printf("│ %-*s │ %-*s │\n",
               name_w,
               items[i].name,
               status_w,
               items[i].status);
    }

    /* ---------------- footer ---------------- */

    draw_bottom(name_w, status_w);

    return 0;
}

/* ---------------- handler ---------------- */

int cmd_branch(int argc, char **argv) {

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

    return gitauto_branch(quiet);
}