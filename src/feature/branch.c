#include "gitauto.h"
#include "env.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define popen  _popen
#define pclose _pclose

/* ---------------- colors ---------------- */

#define CLR_RESET  "\x1b[0m"
#define CLR_RED    "\x1b[31m"
#define CLR_GREEN  "\x1b[32m"
#define CLR_YELLOW "\x1b[33m"

/* ---------------- model ---------------- */

typedef struct {
    char name[256];
    int is_current;
} LocalBranch;

typedef struct {
    char name[256]; // origin/xxx
} RemoteBranch;

typedef struct {
    char local[256];   // xxx
    char remote[256];  // origin/xxx

    int is_local;
    int is_remote;
    int is_current;
} BranchView;

/* ---------------- trim ---------------- */

static const char* trim(char *s) {
    while (*s == ' ' || *s == '\t') s++;

    char *end = s + strlen(s);
    while (end > s &&
          (*(end - 1) == ' ' ||
           *(end - 1) == '\t' ||
           *(end - 1) == '\r' ||
           *(end - 1) == '\n')) {
        *(--end) = 0;
    }

    return s;
}

/* ---------------- local ---------------- */

static int collect_local(LocalBranch *list, int *count) {

    FILE *fp = popen("git branch", "r");
    if (!fp) return 1;

    char line[256];
    *count = 0;

    while (fgets(line, sizeof(line), fp)) {

        if (*count >= 128) break;

        char *clean = (char*)trim(line);

        LocalBranch *b = &list[*count];
        memset(b, 0, sizeof(*b));

        if (clean[0] == '*') {
            b->is_current = 1;
            snprintf(b->name, sizeof(b->name), "%s", clean + 2);
        } else {
            snprintf(b->name, sizeof(b->name), "%s", clean);
        }

        (*count)++;
    }

    pclose(fp);
    return 0;
}

/* ---------------- remote ---------------- */

static int collect_remote(RemoteBranch *list, int *count) {

    FILE *fp = popen("git branch -r", "r");
    if (!fp) return 1;

    char line[256];
    *count = 0;

    while (fgets(line, sizeof(line), fp)) {

        if (*count >= 128) break;
        if (strstr(line, "->")) continue;

        char *clean = (char*)trim(line);

        RemoteBranch *b = &list[*count];
        snprintf(b->name, sizeof(b->name), "%s", clean);

        (*count)++;
    }

    pclose(fp);
    return 0;
}

/* ---------------- status safety ---------------- */

static int ref_exists(const char *ref) {

    if (!ref || !ref[0]) return 0;

    char cmd[512];
    snprintf(cmd, sizeof(cmd),
        "git rev-parse --verify %s >nul 2>nul",
        ref
    );

    return system(cmd) == 0;
}

/* ---------------- status ---------------- */

static const char* get_status(const char *local, const char *remote) {

    if (!local && remote) return "remote only";
    if (local && !remote) return "local only";
    if (!local && !remote) return "unknown";

    if (!ref_exists(local) || !ref_exists(remote)) {
        return "unknown";
    }

    char cmd[512];
    snprintf(cmd, sizeof(cmd),
        "git rev-list --left-right --count %s...%s",
        local,
        remote
    );

    FILE *fp = popen(cmd, "r");
    if (!fp) return "unknown";

    int a = 0, b = 0;
    fscanf(fp, "%d %d", &a, &b);
    pclose(fp);

    if (a == 0 && b == 0) return "synced";
    if (a > 0 && b == 0) return "ahead";
    if (a == 0 && b > 0) return "behind";
    return "diverged";
}

/* ---------------- main ---------------- */

int gitauto_branch(bool quiet) {
    git_run("git fetch --all --prune", true);
    (void)quiet;

    LocalBranch locals[128];
    RemoteBranch remotes[128];
    BranchView views[256];

    int lcount = 0, rcount = 0, vcount = 0;

    if (collect_local(locals, &lcount) != 0) {
        log_error("failed to get local branches");
        return 1;
    }

    if (collect_remote(remotes, &rcount) != 0) {
        log_error("failed to get remote branches");
        return 1;
    }

    /* ---------------- build views (local first) ---------------- */

    for (int i = 0; i < lcount; i++) {

        BranchView *v = &views[vcount++];
        memset(v, 0, sizeof(*v));

        snprintf(v->local, sizeof(v->local), "%s", locals[i].name);
        v->is_local = 1;

        if (locals[i].is_current) {
            v->is_current = 1;
        }
    }

    /* ---------------- merge remote ---------------- */

    for (int i = 0; i < rcount; i++) {

        const char *r = remotes[i].name;
        const char *name = strrchr(r, '/');
        name = name ? name + 1 : r;

        int found = 0;

        for (int j = 0; j < vcount; j++) {

            if (views[j].is_local &&
                strcmp(views[j].local, name) == 0) {

                snprintf(views[j].remote, sizeof(views[j].remote), "%s", r);
                views[j].is_remote = 1;
                found = 1;
                break;
            }
        }

        if (!found) {

            BranchView *v = &views[vcount++];
            memset(v, 0, sizeof(*v));

            snprintf(v->remote, sizeof(v->remote), "%s", r);
            v->is_remote = 1;
        }
    }

    /* ---------------- print ---------------- */

    printf(
        "Local                     Remote                   Status\n"
        "---------------------------------------------------------------\n"
    );

    for (int i = 0; i < vcount; i++) {

        const char *local  = views[i].is_local  ? views[i].local  : NULL;
        const char *remote = views[i].is_remote ? views[i].remote : NULL;

        const char *status = get_status(local, remote);

        const char *color = CLR_RESET;

        if (!strcmp(status, "synced") || !strcmp(status, "ahead")) {
            color = CLR_GREEN;
        }
        else if (!strcmp(status, "behind") || !strcmp(status, "diverged")) {
            color = CLR_YELLOW;
        }
        else {
            color = CLR_RED;
        }

        printf(
            "%c %-24s %-24s %s%s%s\n",
            views[i].is_current ? '*' : ' ',
            local  ? local  : "Does not exist",
            remote ? remote : "Does not exist",
            color,
            status,
            CLR_RESET
        );
    }

    return 0;
}

/* ---------------- handler ---------------- */

int cmd_branch(int argc, char **argv) {

    (void)argv;

    if (!is_git_repo()) {
        log_error("not a git repository");
        return 1;
    }

    ensure_config();
    load_config(&g_cfg);

    return gitauto_branch(false);
}
