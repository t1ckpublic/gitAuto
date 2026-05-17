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

<br/>

`gitAuto` 是一个 **轻量级 Git 自动化 CLI 工具（Windows only）**

它不会替代 Git，而是对高频 Git 操作进行“极简封装 + 自动化增强”，让日常开发更快、更少思考。

---

## ⚡ 核心理念

- 减少并简化重复 Git 操作
- 默认安全（危险操作显式分级）
- 保持 Git 行为可预测
- 提供“更直觉”的命令语义
- 自动化但不隐藏 Git 本质

---

## 🚀 功能总览

#### 🧭 仓库与基础操作

| 命令 | 功能 |
|------|------|
| `gitauto init` | 初始化仓库 + 首次 commit + push + upstream |
| `gitauto clone <url>` | 克隆仓库 |
| `gitauto link <url>` | 绑定 origin 并 fetch |
| `gitauto open` | 打开远程仓库页面 |
| `gitauto version` | 查看 Git + gitAuto 版本 |

---

#### 🚀 推送与同步

| 命令 | 功能 |
|------|------|
| `gitauto push` | 智能 push（pull + add + commit + push） |
| `gitauto push -m "msg"` | 自定义 commit message |
| `gitauto push -f` | 安全强推（--force-with-lease） |
| `gitauto push -ff` | 危险强推（--force） |
| `gitauto pull` | 智能 pull（rebase + autostash） |
| `gitauto pull -f` | 强制对齐远程 |
| `gitauto pull -ff` | 重置并清空本地 |
| `gitauto sync` | pull + push 双向同步 |

---

#### 👀 自动化监听

| 命令 | 功能 |
|------|------|
| `gitauto watch` | 文件监听自动同步 |
| `gitauto watch -q` | 静默后台同步 |

---

#### 🌿 分支管理

| 命令 | 功能 |
|------|------|
| `gitauto branch` | 分支列表 |
| `gitauto checkout <branch>` | 切换分支（不存在则创建） |
| `gitauto current` | 查看当前分支 |

---

#### 💾 stash / 工作区

| 命令 | 功能 |
|------|------|
| `gitauto save` | stash 保存 |
| `gitauto save -m "msg"` | 带备注 stash |
| `gitauto restore` | pop 最近 stash |
| `gitauto restore -a` | apply stash |
| `gitauto restore <N>` | 指定 stash 恢复 |
| `gitauto restore list` | 查看 stash 列表 |

---

#### 🧹 清理与重置

| 命令 | 功能 |
|------|------|
| `gitauto clean` | 预览并清理未跟踪文件 |
| `gitauto clean -f` | 强制清理 |
| `gitauto clean -ff` | 清理 ignored + untracked |
| `gitauto reset` | hard reset |
| `gitauto reset <N>` | 回退 N 次 commit |

---

#### ⏪ 撤销系统

| 命令 | 功能 |
|------|------|
| `gitauto undo` | soft reset HEAD~1 |
| `gitauto undo mixed` | mixed reset |
| `gitauto undo hard` | hard reset |
| `gitauto undo <N>` | 撤销 N 次 commit |

---

#### 🧠 提交增强

| 命令 | 功能 |
|------|------|
| `gitauto amend` | 修改最近 commit |
| `gitauto amend -m` | 修改 commit message |

---

#### 🧹 ignore 管理

| 命令 | 功能 |
|------|------|
| `gitauto ignore node` | 写入 Node.js ignore 模板 |
| `gitauto ignore add "<rule>"` | 添加 ignore 规则 |

---

#### 🧪 仓库诊断

| 命令 | 功能 |
|------|------|
| `gitauto doctor` | 仓库健康检查 |
| `gitauto doctor -f` | 自动修复模式 |

---

## 🧠 命令设计哲学

### push = 自动同步模型

```bash
gitauto push
↓
pull --rebase --autostash
add .
commit
push
````

---

### watch = 自动开发循环

```text
文件变动
→ 自动 commit
→ 自动 push
→ 继续监听
```

---

### undo = Git 时间回退系统

```text
soft   → 保留 staged
mixed  → 取消 staged
hard   → 全部丢弃
```

---

## ⚙️ GitAuto 内部命令映射

```c
init, i
push, p
watch, w
link, l
pull, pl
sync, s
clone, c
branch, br
checkout, co
backup, b
open, o
version, v
ignore, ig
save, sv
restore, re
clean, cl
undo, u
amend, a
reset, r
current, cur
doctor, dr
```

---

## 📁 配置系统

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

特点：

* 自动生成
* 无需重启生效
* watch 模式可配置

---

## 🧱 .gitignore 管理策略

gitAuto 使用安全分区：

```gitignore
# >>> gitAuto
/node_modules/
/build/
# <<< gitAuto
```

规则：

* 不污染用户原有规则
* 不重复写入
* 仅管理标记区块

---

## ⚠️ 输出等级

* `[ERROR]`：严重错误
* `[WARN]`：警告信息
* `[gitAuto]`：工具状态
* `[git]`：原生 Git 输出

---

## 🪟 平台支持

* Windows only
* 基于 Win32 文件监听 API

---

## 🚧 未来计划

* Linux / macOS 支持
* 冲突自动辅助解决
* 一键安装与自动更新

---

## 📜 License

MIT License
