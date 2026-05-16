#ifndef GITAUTO_H
#define GITAUTO_H

#include <windows.h>
#include <stdbool.h>

#define CONFIG_PATH ".git/gitauto.conf"
#define GITIGNORE_PATH ".gitignore"

typedef struct {
    int countdown;
    char whitelist[512];
    char blacklist[512];
} Config;


/* global variables */
extern Config g_cfg;
extern volatile int running;

/* common */
void build_commit_msg(char *buf, size_t size, const char *mode);

/* logging */
void log_info(const char *fmt, ...);
void log_warn(const char *fmt, ...);
void log_error(const char *fmt, ...);

/* git */
int git_run(const char *cmd, bool quiet);
bool is_git_repo(void);
bool has_commit(void);

/* config */
void ensure_config(void);
void load_config(Config *cfg);

/* ignore */
void ensure_gitignore(void);

/* init */
int gitauto_init(void);
int cmd_init(int argc, char **argv);

/* push */
typedef struct {
    bool quiet;

    bool force_with_lease;
    bool force;

    const char *message;
} PushOptions;
int gitauto_push(PushOptions *opt);
int cmd_push(int argc, char **argv);

/* watch */
void watch_loop(bool quiet);
int cmd_watch(int argc, char **argv);

/* link */
int gitauto_link(const char *url, bool quiet);
int cmd_link(int argc, char **argv);

/* ssh-agent */
int gitauto_ensure_ssh_key(void);






#endif