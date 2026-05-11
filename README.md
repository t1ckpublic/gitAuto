[中文版本README](#-特性)


---

# gitAuto

`gitAuto` is a **Lightweight Git Auto-Commit / Push Tool (Windows only)**  
designed to automatically execute the **Pull → Wait → Countdown → Commit → Push** workflow when local files change.

Its goal is not to replace Git, but to **reduce repetitive, low-value Git operations**.

---

## ✨ Features

- 🚀 **Auto Mode**: Detects file changes, counts down, and automatically pushes  
- ⚡ **Manual Mode**: One command to commit & push immediately  
- 🔄 **Automatic Retry on Push Failure** (with interactive confirmation)  
- 🔕 **Quiet Mode**: Reduces Git output while keeping runtime status visible  
- 🧠 **Smart Initialization**: Guides repository setup on first use  
- 🧩 **Auto-Generated Config File** (located in `.git/`)  
- 🪟 **Windows Only** (uses Win32 API)  

---

## 🚀 Usage

### View Help

```bash
gitauto
```

### Initialize Repository (First-Time Setup)

```bash
gitauto init
```

Behavior:
- If the current directory is not a Git repository, it initializes one
- If it's the first commit, it will guide you to enter the remote repository URL
- Automatically generates:
  - `.git/gitauto.conf`
  - `.gitignore` (with a gitAuto managed section)

### Manual Push (Commit Immediately)

```bash
gitauto push
```

Equivalent to:

```bash
git pull --rebase --autostash
git add .
git commit
git push
```

### Auto-Push Mode

```bash
gitauto watch
```

Workflow:
1. Starts with a `git pull`
2. Enters waiting state
3. Detects file changes
4. Begins countdown (configurable)
5. Automatically commits & pushes after countdown
6. Returns to waiting state

### Quiet Mode

```bash
gitauto -A --quiet
```

- Suppresses native Git output
- Retains `gitAuto` status messages
- Suitable for long-term background operation

---

## ⚙️ Configuration File

Configuration file location:
```text
.git/gitauto.conf
```

Example content:
```conf
# gitAuto config

countdown=5        # Auto-push countdown (seconds)
watch_whitelist=src/,include/        # Whitelist of folders to watch for changes
watch_blacklist=.git/,build/        # Blacklist of folders to ignore
```
> Configuration file is auto-generated if missing  
> Changes take effect immediately; no restart required

---

## 📁 .gitignore Management Strategy
`gitAuto` maintains a dedicated block within `.gitignore`:
```gitignore
# >>> gitAuto
/build/
# <<< gitAuto
```

Notes:
- Will not write duplicate entries
- Does not interfere with your existing ignore rules
- Only modifies this block during reset operations

---

## ⚠️ Output & Prompt Legend

- `[ERROR]`: Red, high-risk errors (e.g., not a repo, push failure)
- `[WARN]`: Yellow, warnings that do not interrupt the workflow
- `[gitAuto]`: Tool's own status output
- `[git]`: Git output (can be hidden in quiet mode)

## 📜 License
MIT License

## More features are under development
---

# gitAuto

`gitAuto` 是一个 **轻量级 Git 自动提交 / 推送工具（Windows only）**，  
用于在本地文件发生变动时，自动完成 **拉取 → 等待 → 计时 → 提交 → 推送** 的流程。

它的目标不是取代 Git，而是**减少重复、低价值的 Git 操作**。

---

## ✨ 特性

- 🚀 自动模式：检测文件变动，倒计时后自动 push  
- ⚡ 手动模式：一条命令立即 commit & push  
- 🔄 Push 失败自动重试（可交互确认）  
- 🔕 Quiet 模式：减少 git 输出，但保留运行状态  
- 🧠 智能初始化：首次使用可引导配置仓库  
- 🧩 配置文件自动生成（位于 `.git/`）  
- 🪟 **仅支持 Windows**（使用 Win32 API）  

---


## 🚀 使用方法

### 查看使用说明

```bash
gitauto
```

### 初始化仓库（首次使用）

```bash
gitauto init
```

行为说明：

- 若当前目录不是 Git 仓库，会进行初始化
- 若是首次 commit，会引导你输入远程仓库 URL
- 会自动生成：
  - `.git/gitauto.conf`
  - `.gitignore`（包含 gitAuto 管理区块）

### 手动推送（立即提交）

```bash
gitauto push
```

等价于：

```bash
git pull --rebase --autostash
git add .
git commit
git push
```

### 自动推送模式

```bash
gitauto watch
```

流程：
1. 启动时先执行一次 `git pull`
2. 进入等待态
3. 检测到文件变动
4. 开始倒计时（可配置）
5. 倒计时结束后自动 commit & push
6. 返回等待态
   
### Quiet 模式

```bash
gitauto -A --quiet
```

- 抑制 Git 原生输出
- 保留 `gitAuto` 状态信息
- 适合长时间后台运行

---

## ⚙️ 配置文件

配置文件位置：
```text
.git/gitauto.conf
```

示例内容：
```conf
# gitAuto config

countdown=5        # 自动推送倒计时（秒）
watch_whitelist=src/,include/        # 检测变动文件夹白名单
watch_blacklist=.git/,build/        # 检测变动文件夹黑名单
```
> 配置文件不存在时会自动生成  
> 修改后无需重启 gitAuto

---

## 📁 .gitignore 管理策略
`gitAuto` 会在 `.gitignore` 中维护一个独立区块：
```gitignore
# >>> gitAuto
/build/
# <<< gitAuto
```

说明：
- 不会重复写入
- 不影响你已有的 ignore 规则
- 重置时只操作该区块

---

## ⚠️ 输出与提示说明

- `[ERROR]`：红色，高风险错误（如非仓库、push 失败）
- `[WARN]`：黄色，警告但不中断流程
- `[gitAuto]`：工具自身状态输出
- `[git]`：Git 输出（可在 quiet 模式下隐藏）

## 📜 License
MIT License

## 更多功能正在开发中