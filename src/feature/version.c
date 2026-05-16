#include "gitauto.h"
#include "env.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GITAUTO_VERSION "v0.2.1"

/* ---------------- get git version ---------------- */

static void print_git_version(void) {

    char buf[256];

    FILE *fp = popen("git --version", "r");
    if (!fp) return;

    if (fgets(buf, sizeof(buf), fp)) {
        printf("%s", buf);
    }

    pclose(fp);
}

/* ---------------- core ---------------- */

int gitauto_version(bool quiet) {

    if (!quiet) {
        print_git_version();
        printf("gitAuto version %s\n", GITAUTO_VERSION);
    }

    return 0;
}

/* ---------------- handler ---------------- */
int cmd_version(int argc, char **argv) {

    bool quiet = false;

    for (int i = 2; i < argc; i++) {
        if (!strcmp(argv[i], "-q") ||
            !strcmp(argv[i], "--quiet")) {
            quiet = true;
        }
    }

    return gitauto_version(quiet);
}