#include "gitauto.h"
#include "env.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define popen  _popen
#define pclose _pclose

/* ---------------- ANSI colors ---------------- */

#define CLR_RESET  "\x1b[0m"
#define CLR_RED    "\x1b[31m"
#define CLR_GREEN  "\x1b[32m"

/* ---------------- tags ---------------- */

static const char* ok_tag(void) {
    return CLR_GREEN "[HEALTHY]" CLR_RESET;
}

static const char* bad_tag(void) {
    return CLR_RED "[SICK]" CLR_RESET;
}

/* ---------------- utils ---------------- */

static int run_cmd(const char *cmd, bool quiet) {
    return git_run(cmd, quiet);
}

/* ---------------- HEAD check ---------------- */

static void check_head(bool *ok) {

    FILE *fp = popen("git rev-parse --abbrev-ref HEAD", "r");
    if (!fp) {
        log_error("failed to read HEAD");
        *ok = false;
        return;
    }

    char buf[128] = {0};
    fgets(buf, sizeof(buf), fp);
    pclose(fp);

    buf[strcspn(buf, "\r\n")] = 0;

    if (!strcmp(buf, "HEAD")) {
        printf("%s HEAD: detached\n", bad_tag());
        *ok = false;
    } else {
        printf("%s HEAD: %s\n", ok_tag(), buf);
    }
}

/* ---------------- conflicts ---------------- */

static void check_conflicts(bool *ok) {

    FILE *fp = popen("git diff --name-only --diff-filter=U", "r");
    if (!fp) return;

    char line[256];
    int has = 0;

    while (fgets(line, sizeof(line), fp)) {

        if (!has) {
            printf("%s unresolved conflicts:\n", bad_tag());
            has = 1;
            *ok = false;
        }

        line[strcspn(line, "\r\n")] = 0;
        printf("   - %s\n", line);
    }

    pclose(fp);

    if (!has) {
        printf("%s no conflicts\n", ok_tag());
    }
}

/* ---------------- state check ---------------- */

static void check_state(bool *ok) {

    FILE *fp;

    fp = popen("if exist .git\\rebase-merge (echo 1)", "r");
    if (fp && fgets((char[16]){0}, 16, fp)) {
        printf("%s rebase in progress\n", bad_tag());
        *ok = false;
    }
    if (fp) pclose(fp);

    fp = popen("if exist .git\\rebase-apply (echo 1)", "r");
    if (fp && fgets((char[16]){0}, 16, fp)) {
        printf("%s rebase apply in progress\n", bad_tag());
        *ok = false;
    }
    if (fp) pclose(fp);

    fp = popen("if exist .git\\MERGE_HEAD (echo 1)", "r");
    if (fp && fgets((char[16]){0}, 16, fp)) {
        printf("%s merge in progress\n", bad_tag());
        *ok = false;
    }
    if (fp) pclose(fp);
}

/* ---------------- upstream ---------------- */

static void check_upstream(bool *ok) {

    int ret = system(
        "git rev-parse --abbrev-ref @{u} >nul 2>nul"
    );

    if (ret != 0) {
        printf("%s no upstream configured\n", bad_tag());
        *ok = false;
    } else {
        printf("%s upstream OK\n", ok_tag());
    }
}

/* ---------------- remote ---------------- */

static void check_remote(bool *ok) {

    int ret = system("git fetch -q --dry-run >nul 2>nul");

    if (ret != 0) {
        printf("%s remote check failed\n", bad_tag());
        *ok = false;
    } else {
        printf("%s remote reachable\n", ok_tag());
    }
}

/* ---------------- fix ---------------- */

static void run_fix(bool quiet) {

    printf("\n=== auto fix ===\n");

    run_cmd("git fetch origin --prune", quiet);
    run_cmd("git rebase --abort", true);
    run_cmd("git merge --abort", true);

    printf("%s attempted cleanup (fetch + abort)\n", ok_tag());
}

/* ---------------- main ---------------- */

int gitauto_doctor(bool fix, bool quiet) {

    bool ok = true;

    printf("=== gitAuto doctor ===\n\n");

    check_head(&ok);
    check_conflicts(&ok);
    check_state(&ok);
    check_upstream(&ok);
    check_remote(&ok);

    printf("\n======================\n");

    if (ok) {
        printf("%s repo looks healthy\n", ok_tag());
    } else {
        printf("%s issues detected\n", bad_tag());
    }

    if (fix) {
        run_fix(quiet);
    }

    return ok ? 0 : 1;
}

/* ---------------- handler ---------------- */

int cmd_doctor(int argc, char **argv) {

    bool fix = false;
    bool quiet = false;

    for (int i = 2; i < argc; i++) {

        if (!strcmp(argv[i], "-f") ||
            !strcmp(argv[i], "--fix")) {
            fix = true;
        }

        else if (!strcmp(argv[i], "-q") ||
                 !strcmp(argv[i], "--quiet")) {
            quiet = true;
        }

        else {
            log_warn("unknown option: %s", argv[i]);
        }
    }

    if (!is_git_repo()) {
        log_error("not a git repository");
        return 1;
    }

    ensure_config();
    load_config(&g_cfg);

    return gitauto_doctor(fix, quiet);
}