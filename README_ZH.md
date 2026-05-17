<div align="center">

# gitAuto - 轻量 & 直觉语义 Git 自动化工具

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

## ⚡ 极简使用！！！

gitAuto 的**终极**设计目标：**让 Git 操作可以缩短到几乎“打字级别”**

支持 alias 后，你可以用极短命令完成完整 workflow：

```bash
gitauto push      → ga p
gitauto checkout  → ga co
gitauto restore   → ga re
gitauto branch    → ga br
````

甚至日常开发可以压缩为：

```bash
g p   # pull + commit + push
g w   # 自动监听同步
g u   # undo
```

👉 **让 Git 操作从“命令”变成“肌肉记忆”！！**

---

## 🚀 功能总览
```
gitauto init/i                         初始化仓库（init + add + commit + push）
gitauto clone/c <url>                  克隆仓库并初始化工作流
gitauto link/l <url>                   绑定远程仓库（设置 origin + fetch）

gitauto push/p                         智能推送（pull --rebase + add + commit + push）
gitauto push/p -m <msg>                使用自定义提交信息
gitauto push/p -f                      安全强制推送（--force-with-lease）
gitauto push/p -ff                     危险强制推送（--force）

gitauto pull/pl                        智能拉取（rebase + autostash）
gitauto pull/pl -f                     强制同步（reset 到 origin/<branch>）
gitauto pull/pl -ff                    完全重置（clean + hard reset）

gitauto sync/s                         双向同步（pull + push）

gitauto watch/w                        文件变动自动同步
gitauto watch/w -q                     静默模式（最少输出）

gitauto branch/br                      查看分支列表
gitauto checkout/co <branch>           切换分支（不存在则自动创建）

gitauto backup/b                       创建备份分支快照
gitauto backup/b restore/re            从备份分支恢复
gitauto backup/b restore/re <branch>   恢复指定分支快照

gitauto save/sv                        暂存工作区（stash push -u）
gitauto save/sv -m <msg>               暂存并附带信息

gitauto restore/re                     恢复最近一次暂存（pop）
gitauto restore/re -a                  应用暂存（保留 stash）
gitauto restore/re <N>                 按索引恢复暂存
gitauto restore/re list/l              查看所有暂存记录

gitauto clean/cl                       预览并清理未跟踪文件
gitauto clean/cl -f                    强制清理未跟踪文件
gitauto clean/cl -ff                   深度清理（包括 ignored 文件）

gitauto ignore/ig node                 添加 Node.js .gitignore 模板
gitauto ignore/ig add <rule>           添加自定义 ignore 规则

gitauto undo/u                         撤销最近一次提交（soft reset）
gitauto undo/u mixed/m                 混合撤销（取消 staged）
gitauto undo/u hard/h                  强制撤销（丢弃修改）
gitauto undo/u <N>                     撤销最近 N 次提交

gitauto amend/a                        修改最近一次提交
gitauto amend -m <msg>                 修改提交信息

gitauto reset/r                        重置到 HEAD（强制）
gitauto reset/r <N>                    回退 N 次提交

gitauto current/cur                    查看当前分支
gitauto open/o                         在浏览器打开远程仓库
gitauto version/v                      查看版本信息

gitauto doctor/dr                      诊断仓库问题
gitauto doctor/dr -fix/-f              自动修复常见问题
```


---


## 🧠 命令设计哲学

### ⚡ 直觉语义系统（Intuitive Command Design）

gitAuto 的核心设计不是“简化 Git 命令”，而是：

> **用自然行为映射替代 Git 语法记忆**

---

### 🚀 push = “把当前状态同步出去”

```bash
gitauto push
```

不再是：

* add？
* commit？
* pull？
* rebase？

而是一个统一语义：

> **“我现在要把本地状态同步到远端”**

因此 push 自动完成整个同步链路。

---

### 🌿 checkout = “切换到某个状态”

```bash
gitauto checkout <branch>
```

语义不是：

* git checkout
* git switch
* git branch -b

而是：

> **“我想进入这个分支的工作状态”**

如果不存在 → 自动创建
因为“进入一个状态”本身就可以被推断为“存在即切换，不存在即创建”

---

### 💾 save / restore = “暂停与恢复当前状态”

```bash
gitauto save
gitauto restore
```

不是 stash 的抽象概念，而是：

> **save = 暂停当前工作**
> **restore = 回到刚才的工作状态**

用户不需要知道 stash 机制，只需要理解：

* 暂停
* 恢复

---

### ⏪ undo = “撤销程度，而不是 Git 参数”

```text
undo
undo mixed
undo hard
```

语义模型是：

> 不是 reset 参数差异
> 而是“我要撤销到什么程度”

* soft → 还想保留
* mixed → 回到修改状态
* hard → 彻底不要了

👉 用户思考的是“意图”，不是 Git 行为模型

---

### 👀 watch = “进入自动工作模式”

```bash
gitauto watch
```

不是“监听文件变化”，而是：

> **进入持续同步状态**

行为被抽象为：

* 我进入 watch 模式
* 系统负责同步

而不是用户负责每一步 Git 操作

---

### 🧠 设计本质总结

gitAuto 的语义系统遵循三条规则：

* 不暴露 Git 内部模型
* 命令表达“意图”而不是“步骤”
* 默认用人类行为语言建模（进入 / 暂停 / 同步 / 撤销）

---

### ⚡ 一句话总结

> gitAuto 不是 Git 快捷键工具。  
> 它是一个 **用于表示 Git 意图的自然语言映射层**。

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
