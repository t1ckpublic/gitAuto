#include "gitauto.h"
#include "env.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ------------- utils ------------ */
static int file_contains(FILE *f, const char *text) {

    char buf[1024];

    fseek(f, 0, SEEK_SET);

    while (fgets(buf, sizeof(buf), f)) {
        if (strstr(buf, text)) {
            return 1;
        }
    }

    return 0;
}

/* ---------------- insert into gitauto block ---------------- */
static int insert_rule(const char *rule) {

    FILE *f = fopen(GITIGNORE_PATH, "r+");
    if (!f) return 1;

    char content[8192];
    size_t len = fread(content, 1, sizeof(content) - 1, f);
    content[len] = 0;

    char *start = strstr(content, "# >>> gitAuto");
    char *end   = strstr(content, "# <<< gitAuto");

    if (!start || !end) {
        fclose(f);
        return 1;
    }

    /* duplicate check */
    if (strstr(start, rule)) {
        fclose(f);
        return 0;
    }

    char *insert_pos = strstr(start, "\n");
    if (!insert_pos) {
        fclose(f);
        return 1;
    }
    insert_pos++;

    FILE *tmp = fopen("gitignore.tmp", "w");
    if (!tmp) {
        fclose(f);
        return 1;
    }

    /* before block insert point */
    fwrite(content, 1, insert_pos - content, tmp);

    fprintf(tmp, "%s\n", rule);

    /* rest */
    fwrite(insert_pos, 1, (content + len) - insert_pos, tmp);

    fclose(tmp);
    fclose(f);

    remove(GITIGNORE_PATH);
    rename("gitignore.tmp", GITIGNORE_PATH);

    return 0;
}

/* ---------------- ignore add ---------------- */
int gitauto_ignore_add(const char *rule, bool quiet) {

    if (!is_git_repo()) {
        log_error("not a git repository");
        return 1;
    }

    ensure_gitignore();

    /* basic validation */
    if (!rule || !rule[0]) {
        log_error("empty rule");
        return 1;
    }

    /* optional: strip quotes */
    char clean[512];
    snprintf(clean, sizeof(clean), "%s", rule);

    /* remove surrounding quotes if exists */
    size_t len = strlen(clean);
    if (len >= 2 &&
        ((clean[0] == '"' && clean[len-1] == '"') ||
         (clean[0] == '\'' && clean[len-1] == '\''))) {
        clean[len-1] = 0;
        memmove(clean, clean + 1, len - 1);
    }

    if (insert_rule(clean) != 0) {
        log_error("failed to insert rule");
        return 1;
    }

    if (!quiet) {
        printf("[gitAuto] ignore added: %s\n", clean);
    }

    return 0;
}

/* ---------------- node template ---------------- */
static const char *node_rules[] = {
    "node_modules/",
    "dist/",
    "build/",
    ".env",
    "npm-debug.log*",
    "yarn-debug.log*",
    "pnpm-debug.log*"
};

/* ---------------- public API ---------------- */
int gitauto_ignore_node(bool quiet) {

    if (!is_git_repo()) {
        log_error("not a git repository");
        return 1;
    }

    ensure_gitignore();

    int n = sizeof(node_rules) / sizeof(node_rules[0]);

    for (int i = 0; i < n; i++) {
        insert_rule(node_rules[i]);
    }

    if (!quiet) {
        printf("[gitAuto] node ignore rules added\n");
    }

    return 0;
}

/* ---------------- handler ---------------- */
int cmd_ignore(int argc, char **argv) {

    bool quiet = false;
    const char *subcmd = NULL;
    const char *arg = NULL;

    for (int i = 2; i < argc; i++) {

        if (!strcmp(argv[i], "-q") ||
            !strcmp(argv[i], "--quiet")) {
            quiet = true;
        }
        else if (!subcmd) {
            subcmd = argv[i];
        }
        else {
            arg = argv[i];
        }
    }

    if (!subcmd) {
        log_error("usage: gitauto ig <node|add>");
        return 1;
    }

    /* ---------------- node ---------------- */
    if (!strcmp(subcmd, "node")) {
        return gitauto_ignore_node(quiet);
    }

    /* ---------------- add ---------------- */
    if (!strcmp(subcmd, "add")) {

        if (!arg) {
            log_error("usage: gitauto ig add \"rule\"");
            return 1;
        }

        return gitauto_ignore_add(arg, quiet);
    }

    log_error("unknown ignore command");
    return 1;
}