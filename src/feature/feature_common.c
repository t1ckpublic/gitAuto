#include "gitauto.h"
#include "env.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <signal.h>
#include <time.h>

/* global variables */
Config g_cfg;
volatile int running = 1;

/* ---------------- build commit message ---------------- */

void build_commit_msg(char *buf, size_t size, const char *mode)
{
    time_t t = time(NULL);
    struct tm tm_now;
    localtime_s(&tm_now, &t);

    snprintf(
        buf, size,
        "gitAuto %s push @ %04d-%02d-%02d %02d:%02d:%02d",
        mode,
        tm_now.tm_year + 1900,
        tm_now.tm_mon + 1,
        tm_now.tm_mday,
        tm_now.tm_hour,
        tm_now.tm_min,
        tm_now.tm_sec
    );
}

/* ---------------- logging ---------------- */

static void set_color(WORD c) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

void log_info(const char *fmt, ...) {
    va_list ap;
    printf("[gitAuto] ");
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
    printf("\n");
}

void log_warn(const char *fmt, ...) {
    set_color(FOREGROUND_RED | FOREGROUND_GREEN);
    printf("[WARN] ");
    set_color(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    va_list ap;
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
    printf("\n");
}

void log_error(const char *fmt, ...) {
    set_color(FOREGROUND_RED);
    printf("[ERROR] ");
    set_color(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    va_list ap;
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
    printf("\n");
}

/* ---------------- git ---------------- */

int git_run(const char *cmd, bool quiet) {
    char buf[512];
    FILE *fp = _popen(cmd, "r");
    if (!fp) return -1;

    while (fgets(buf, sizeof(buf), fp)) {
        if (!quiet) {
            set_color(FOREGROUND_INTENSITY);
            printf("[git] %s", buf);
            set_color(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        }
    }
    return _pclose(fp);
}

bool is_git_repo(void) {
    return system("git rev-parse --is-inside-work-tree >nul 2>nul") == 0;
}

bool has_commit(void) {
    return system("git rev-parse --verify HEAD >nul 2>nul") == 0;
}

/* ---------------- config ---------------- */

void ensure_config(void) {
    FILE *f = fopen(CONFIG_PATH, "r");
    if (f) {
        fclose(f);
        return;
    }

    log_warn("config not found, creating default");

    f = fopen(CONFIG_PATH, "w");
    fprintf(f,
        "countdown=5\n"
        "watch_whitelist=src/,include/\n"
        "watch_blacklist=.git/,build/\n");
    fclose(f);
}

void load_config(Config *cfg) {
    FILE *f = fopen(CONFIG_PATH, "r");
    char line[256];

    while (fgets(line, sizeof(line), f)) {
        if (sscanf(line, "countdown=%d", &cfg->countdown) == 1) continue;
        if (sscanf(line, "watch_whitelist=%511[^\n]", cfg->whitelist) == 1) continue;
        if (sscanf(line, "watch_blacklist=%511[^\n]", cfg->blacklist) == 1) continue;
    }
    fclose(f);
}

/* ---------------- gitignore ---------------- */

void ensure_gitignore(void) {
    FILE *f = fopen(GITIGNORE_PATH, "r");
    if (f) {
        char buf[1024];
        while (fgets(buf, sizeof(buf), f)) {
            if (strstr(buf, ">>> gitAuto")) {
                fclose(f);
                return;
            }
        }
        fclose(f);
    }

    f = fopen(GITIGNORE_PATH, "a");
    fprintf(f,
        "\n# >>> gitAuto\n"
        "gitAuto.exe\n"
        "# <<< gitAuto\n");
    fclose(f);
}