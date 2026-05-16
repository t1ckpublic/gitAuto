#include "gitauto.h"
#include "env.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#define DEVNULL "nul"
#define HOME_VAR "USERPROFILE"
#define PATH_SEP "\\"

// Windows 上的 setenv 实现


#else
#define DEVNULL "/dev/null"
#define HOME_VAR "HOME"
#define PATH_SEP "/"

// Unix 上使用标准 setenv
#define set_env(name, value) setenv(name, value, 1)

#endif

/* -------------------------------- */
/* key detection and loading */
/* -------------------------------- */

// 标准密钥名称
static const char *standard_keys[] = {
    "id_ed25519",
    "id_rsa",
    "id_ecdsa",
    "id_dsa",
    NULL
};

// 检查文件是否存在
static int file_exists(const char *path)
{
    FILE *f = fopen(path, "r");
    if (f) {
        fclose(f);
        return 1;
    }
    return 0;
}

// 从配置读取 SSH 密钥（支持多个）
static int read_ssh_keys_from_config(char **keys, int max_keys)
{
    FILE *fp;
    char line[512];
    int count = 0;

    fp = fopen(".git/gitauto.conf", "r");
    if (!fp) {
        return 0;  // 配置不存在，使用自动扫描
    }

    while (fgets(line, sizeof(line), fp) && count < max_keys) {
        // 支持 ssh.key=path 格式（多行）
        if (strncmp(line, "ssh.key=", 8) == 0) {
            char *path = line + 8;
            path[strcspn(path, "\r\n")] = '\0';
            
            if (path[0] != '\0' && file_exists(path)) {
                keys[count] = malloc(strlen(path) + 1);
                strcpy(keys[count], path);
                count++;
            }
        }
        // 支持 ssh.keys=path1;path2;path3 格式（单行）
        else if (strncmp(line, "ssh.keys=", 9) == 0) {
            char *paths = line + 9;
            paths[strcspn(paths, "\r\n")] = '\0';
            
            char *copy = malloc(strlen(paths) + 1);
            strcpy(copy, paths);
            
            char *token = strtok(copy, ";");
            while (token && count < max_keys) {
                // 去除空格
                while (*token == ' ') token++;
                char *end = token + strlen(token) - 1;
                while (end > token && *end == ' ') *(end--) = '\0';
                
                if (*token != '\0' && file_exists(token)) {
                    keys[count] = malloc(strlen(token) + 1);
                    strcpy(keys[count], token);
                    count++;
                }
                token = strtok(NULL, ";");
            }
            free(copy);
        }
    }

    fclose(fp);
    return count;
}

// 自动扫描标准密钥位置
static int scan_standard_keys(char **keys, int max_keys)
{
    char keypath[512];
    int count = 0;

    // 获取 HOME 目录
    const char *home_dir = getenv(HOME_VAR);
    if (!home_dir) {
        log_warn("cannot determine home directory");
        return 0;
    }

    // 扫描标准密钥
    for (int i = 0; standard_keys[i] && count < max_keys; i++) {
        snprintf(keypath, sizeof(keypath),
                 "%s%s.ssh%s%s",
                 home_dir, PATH_SEP, PATH_SEP, standard_keys[i]);

        if (file_exists(keypath)) {
            keys[count] = malloc(strlen(keypath) + 1);
            strcpy(keys[count], keypath);
            log_info("found key: %s", keypath);
            count++;
        }
    }

    return count;
}

// 获取所有要加载的密钥
static int get_ssh_keys(char **keys, int max_keys)
{
    int count = 0;

    // 优先从配置读取
    count = read_ssh_keys_from_config(keys, max_keys);
    
    if (count > 0) {
        log_info("loaded %d key(s) from config", count);
        return count;
    }

    // 如果配置中没有，自动扫描
    count = scan_standard_keys(keys, max_keys);
    
    if (count > 0) {
        log_info("auto-detected %d key(s)", count);
    } else {
        log_warn("no ssh keys found");
    }

    return count;
}

/* -------------------------------- */
/* check ssh-agent */
/* -------------------------------- */

static int ssh_agent_has_identity(void)
{
    char cmd[256];

    snprintf(cmd, sizeof(cmd),
             "ssh-add -l >%s 2>&1",
             DEVNULL);

    return git_run(cmd, true) == 0;
}

/* -------------------------------- */
/* start ssh-agent */
/* -------------------------------- */

static int ssh_agent_start(void)
{
#ifdef _WIN32
    log_info("checking ssh-agent service (Windows)...");

    // Windows OpenSSH agent = service model
    int ret = git_run("sc query ssh-agent", true);
    if (ret != 0) {
        log_warn("ssh-agent service not available");
        return 0;
    }

    // 尝试启动（如果已运行会失败但无所谓）
    git_run("sc start ssh-agent", true);

    log_info("ssh-agent service started or already running");

    // Windows 不需要 SSH_AUTH_SOCK / SSH_AGENT_PID
    return 1;

#else
    log_info("starting ssh-agent (Unix)...");

    FILE *fp = popen("ssh-agent -s 2>&1", "r");
    if (!fp) return 0;

    char line[256];
    int ok = 0;

    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, "SSH_AUTH_SOCK=", 14) == 0) {
            char *val = line + 14;
            val[strcspn(val, ";\n")] = '\0';
            set_env("SSH_AUTH_SOCK", val);
            ok = 1;
        }
        else if (strncmp(line, "SSH_AGENT_PID=", 14) == 0) {
            char *val = line + 14;
            val[strcspn(val, ";\n")] = '\0';
            set_env("SSH_AGENT_PID", val);
        }
    }

    pclose(fp);
    return ok;
#endif
}

/* -------------------------------- */
/* load ssh keys */
/* -------------------------------- */

static int load_ssh_key(const char *keypath)
{
    char cmd[1024];

    snprintf(cmd, sizeof(cmd),
             "ssh-add \"%s\" 2>&1",
             keypath);

    int ret = git_run(cmd, false);
    return ret;
}

/* -------------------------------- */
/* public */
/* -------------------------------- */

int gitauto_ensure_ssh_key(void)
{
    // 检查是否已有密钥
    if (ssh_agent_has_identity()) {
        log_info("ssh-agent already has identity");
        return 0;
    }

    // 启动 ssh-agent
    if (!ssh_agent_start()) {
    #ifdef _WIN32
        log_warn("ssh-agent service unavailable or not started");
    #else
        log_warn("ssh-agent failed to start");
    #endif
        return 1;
    }

    // 获取要加载的密钥
    char *keys[16];
    int count = get_ssh_keys(keys, 16);

    if (count == 0) {
        log_warn("no ssh keys configured or found");
        return 1;
    }

    // 加载所有密钥
    log_info("loading %d key(s) to ssh-agent...", count);
    
    for (int i = 0; i < count; i++) {
        if (load_ssh_key(keys[i]) == 0) {
            log_info("loaded key: %s", keys[i]);
        } else {
            log_warn("failed to load key: %s", keys[i]);
        }
        free(keys[i]);
    }

    return 0;
}