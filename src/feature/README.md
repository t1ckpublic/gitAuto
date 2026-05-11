# src/feature/ 目录说明

这个目录包含了 gitauto 工具的所有功能模块，每个功能都以独立的 C 文件实现，便于维护和扩展。

## 现有功能模块

### feature_common.c
公共功能模块，包含：
- `git_run()`: 执行 git 命令的统一接口
- 日志函数: `log_info()`, `log_warn()`, `log_error()`
- 配置管理: 加载和保存配置
- 提交消息生成: `build_commit_msg()`
- 全局变量: `g_cfg` (配置), `running` (运行状态)

### feature_init.c
仓库初始化功能：
- `gitauto_init()`: 初始化 git 仓库，设置远程仓库 URL
- 检查仓库状态，确保 SSH 密钥已加载

### feature_push.c
手动推送功能：
- `gitauto_push()`: 执行 pull、add、commit、push 的完整流程
- 支持自动生成提交消息

### feature_sshagent.c
SSH 代理管理：
- `gitauto_ensure_ssh_key()`: 确保 SSH 代理运行并加载密钥
- 支持从配置文件或自动扫描加载 SSH 密钥
- 跨平台支持 (Windows/Linux)

### feature_watch.c
文件监控功能：
- `watch_loop()`: 监控文件变化，自动触发推送
- `gitauto_start()`: 启动前的准备工作
- 使用 Windows API 监控目录变化

## 关于扩展新功能

### 1. 创建新的功能文件
在 `src/feature/` 目录下创建新的 `.c` 文件，命名格式为 `feature_xxx.c`，其中 `xxx` 是功能名称。

### 2. 实现功能函数
- 包含必要的头文件：`#include "gitauto.h"` 和 `#include "env.h"`
- 实现主要功能函数，函数命名格式为 `gitauto_xxx()`
- 使用 `git_run()` 执行 git 命令
- 使用日志函数记录信息和错误
- 如果需要配置，访问全局 `g_cfg` 变量

### 3. 在 gitauto.h 中声明函数
在 `include/gitauto.h` 中添加新函数的声明：
```c
// 新功能声明
int gitauto_xxx(bool quiet);
```

### 4. 在主程序中添加命令处理
在 `src/gitauto.c` 的 `main()` 函数中添加新的命令处理：
```c
if (!strcmp(argv[i], "xxx")) {
    return gitauto_xxx(quiet);
}
```

### 5. 更新使用说明
在 `print_usage()` 函数中添加新命令的说明。

### 6. 编译和测试
- 确保新文件包含在编译命令中
- 测试功能是否正常工作
- 检查错误处理和日志输出

## 扩展建议

- **保持模块化**: 每个功能独立，避免耦合
- **错误处理**: 所有函数都应返回适当的错误码
- **配置支持**: 如果功能需要配置，在 `.git/gitauto.conf` 中添加相应配置项
- **跨平台兼容**: 使用条件编译 (`#ifdef _WIN32`) 处理平台差异
- **SSH 集成**: 如果功能涉及远程操作，确保调用 `gitauto_ensure_ssh_key()`

## 示例：添加 status 功能

1. 创建 `feature_status.c`:
```c
#include "gitauto.h"
#include "env.h"

int gitauto_status(bool quiet) {
    return git_run("git status", quiet);
}
```

2. 在 `gitauto.h` 中添加声明:
```c
int gitauto_status(bool quiet);
```

3. 在 `main()` 中添加:
```c
else if (!strcmp(argv[i], "status")) {
    return gitauto_status(quiet);
}
```

4. 更新 `print_usage()`:
```c
printf("  gitauto status              Show git status\n");
```

这样就可以通过 `gitauto status` 查看 git 状态了。