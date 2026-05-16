<div align="center">

# gitAuto - 轻量 Git 自动化工具


<img src="https://img.shields.io/badge/gitauto-lightweight%20git%20automation-red?style=for-the-badge"/>

<br/>

<img src="https://img.shields.io/badge/platform-Windows-blue?logo=windows"/>
<img src="https://img.shields.io/badge/language-C-informational?logo=c"/>
<img src="https://img.shields.io/badge/tool-git%20automation-orange?logo=git"/>
<img src="https://img.shields.io/badge/status-active-success"/>
<img src="https://img.shields.io/badge/license-MIT-green"/>

</div>
<br>

`gitAuto` 是一个 **轻量级 Git 自动化命令行工具（仅 Windows）**
用于简化 Git 中重复的提交、拉取、推送、分支管理等操作。

它不会取代 Git，而是对 Git 的高频操作进行封装与增强。

---

## ✨ 功能状态总览

> ✔ = 已实现
> ⏳ = 计划中 / 未实现

| 状态 | 命令                   | 功能                                 |
| -- | -------------------- | ---------------------------------- |
| ✔  | `gitauto init`       | 初始化仓库并首次提交推送                       |
| ✔  | `gitauto link <url>` | 绑定远程仓库并 fetch                      |
| ✔  | `gitauto push`       | 智能推送（rebase + add + commit + push） |
| ✔  | `gitauto push -m`    | 自定义提交信息                            |
| ✔  | `gitauto push -f`    | 安全强制推送（--force-with-lease）         |
| ✔  | `gitauto push -ff`   | 强制覆盖推送（危险操作）                       |
| ✔  | `gitauto pull`       | 智能拉取（rebase + autostash）           |
| ✔  | `gitauto pull -f`    | 强制同步远程                             |
| ✔  | `gitauto pull -ff`   | 完全重置仓库                             |
| ✔  | `gitauto watch`      | 文件变动自动同步                           |
| ✔  | `gitauto watch -q`   | 静默自动同步模式                           |
| ✔  | `gitauto sync`       | 拉取 + 推送同步                          |

| 状态 | 命令                          | 功能             |
| -- | --------------------------- | -------------- |
| ⏳  | `gitauto clone <url>`       | 克隆仓库并自动初始化     |
| ⏳  | `gitauto status`            | 美化 Git 状态输出    |
| ⏳  | `gitauto status --short`    | 简洁状态输出         |
| ⏳  | `gitauto log`               | 美化 commit 历史   |
| ⏳  | `gitauto branch`            | 分支信息美化展示       |
| ⏳  | `gitauto checkout <branch>` | 智能切换分支（不存在则创建） |
| ⏳  | `gitauto backup`            | 快速备份提交         |
| ⏳  | `gitauto save`              | stash 快捷保存     |
| ⏳  | `gitauto restore`           | 恢复 stash       |
| ⏳  | `gitauto ignore <rule>`     | 快速添加 ignore 规则 |
| ⏳  | `gitauto doctor`            | 仓库健康检查         |
| ⏳  | `gitauto clean`             | 清理未跟踪文件        |
| ⏳  | `gitauto undo`              | 撤销最近一次提交       |
| ⏳  | `gitauto amend`             | 修改最近提交         |
| ⏳  | `gitauto reset`             | 强制重置到 HEAD     |
| ⏳  | `gitauto current`           | 查看当前分支         |
| ⏳  | `gitauto changed`           | 查看变更文件         |
| ⏳  | `gitauto open`              | 打开远程仓库         |
| ⏳  | `gitauto version`           | 查看版本信息         |

---

## 🚀 设计理念

* 减少重复 Git 操作
* 保持 Git 行为可预测
* 默认安全，危险操作需要确认
* 自动化，但不“替代 Git”

---

## 🚀 使用方法

### 查看帮助

```bash
gitauto
```

---

### 初始化仓库

```bash
gitauto init
```

行为：

* 如果当前目录不是 Git 仓库，则自动初始化
* 执行首次 commit + push
* 自动设置 upstream 分支
* 生成 `.git/gitauto.conf`

---

### 智能推送

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

---

### 自定义提交信息

```bash
gitauto push -m "提交信息"
```

---

### 强制推送

```bash
gitauto push -f
gitauto push -ff
```

* `-f`：安全强制推送（--force-with-lease）
* `-ff`：危险强制推送（--force）

---

### 智能拉取

```bash
gitauto pull
```

---

### 强制同步

```bash
gitauto pull -f
gitauto pull -ff
```

* `-f`：重置为远程版本
* `-ff`：清理 + 重置（完全丢弃本地修改）

---

### 自动监听模式

```bash
gitauto watch
gitauto watch -q
```

流程：

1. 启动时执行一次 pull
2. 进入文件监听状态
3. 检测文件变化
4. 倒计时
5. 自动 commit + push
6. 回到监听状态

---

### 同步模式

```bash
gitauto sync
```

执行：

```bash
git pull --rebase && git push
```

---

## ⚙️ 配置文件

路径：

```text
.git/gitauto.conf
```

示例：

```conf
countdown=5
watch_whitelist=src/,include/
watch_blacklist=.git/,build/
```

说明：

* 文件不存在时自动生成
* 修改后即时生效，无需重启

---

## 📁 .gitignore 管理策略

gitAuto 会维护 `.gitignore` 中的独立区域：

```gitignore
# >>> gitAuto
/build/
/node_modules/
# <<< gitAuto
```

规则：

* 不重复写入
* 不影响用户原有规则
* 只操作标记区域

---

## ⚠️ 输出等级说明

* `[ERROR]`：严重错误（如非 Git 仓库、push 失败）
* `[WARN]`：警告信息（不影响流程）
* `[gitAuto]`：工具自身状态输出
* `[git]`：原生 Git 输出（可在 quiet 模式隐藏）

---

## 🪟 平台支持

* Windows only（基于 Win32 文件监听 API）

---

## 📜 License

MIT License

---

## 🚧 开发中功能

未来计划：

* 可视化 Git 历史查看
* 交互式分支管理 UI
* 冲突解决辅助工具
* Linux / macOS 支持
* 插件系统（自定义 workflow 扩展）