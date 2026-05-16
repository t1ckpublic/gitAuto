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

/* pull */
typedef struct {
    bool quiet;

    bool force;
    bool clean;
} PullOptions;
int gitauto_pull(PullOptions *opt);
int cmd_pull(int argc, char **argv);

/* sync */
int gitauto_sync(bool quiet);
int cmd_sync(int argc, char **argv);

/* watch */
void watch_loop(bool quiet);
int cmd_watch(int argc, char **argv);

/* link */
int gitauto_link(const char *url, bool quiet);
int cmd_link(int argc, char **argv);

/* clone */
int gitauto_clone(const char *url, bool quiet);
int cmd_clone(int argc, char **argv);

/* branch */
int gitauto_branch(bool quiet);
int cmd_branch(int argc, char **argv);

/* checkout */
int gitauto_checkout(const char *branch, bool quiet);
int cmd_checkout(int argc, char **argv);

/* backup */
int gitauto_backup(bool quiet);
int cmd_backup(int argc, char **argv);

/* ssh-agent */
int gitauto_ensure_ssh_key(void);

/* open */
int gitauto_open(bool quiet);
int cmd_open(int argc, char **argv);

/* version */
int gitauto_version(bool quiet);
int cmd_version(int argc, char **argv);

#endif