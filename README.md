<div align="center">

# gitAuto - Lightweight git Automation Tool
#### [中文README](README_ZH.md)

<img src="https://img.shields.io/badge/gitauto-lightweight%20git%20automation-red?style=for-the-badge"/>

<br/>

<img src="https://img.shields.io/badge/platform-Windows-blue?logo=windows"/>
<img src="https://img.shields.io/badge/language-C-informational?logo=c"/>
<img src="https://img.shields.io/badge/tool-git%20automation-orange?logo=git"/>
<img src="https://img.shields.io/badge/status-active-success"/>
<img src="https://img.shields.io/badge/license-MIT-green"/>

</div>

<br/>

`gitAuto` is a **lightweight Git automation CLI tool (Windows only)**.

It does not replace Git. Instead, it provides **minimal wrappers + automation enhancements** for frequent Git workflows, making daily development faster and less mentally demanding.

---

## ⚡ Core Philosophy

- Reduce and simplify repetitive Git operations
- Safe by default (dangerous actions are explicitly separated)
- Keep Git behavior predictable
- Provide more intuitive command semantics
- Automate workflows without hiding Git fundamentals

---

## 🚀 Feature Overview

---

### 🧭 Repository & Basic Operations

| Command | Description |
|--------|-------------|
| `gitauto init` | Initialize repo + first commit + push + set upstream |
| `gitauto clone <url>` | Clone a repository |
| `gitauto link <url>` | Add origin remote and fetch |
| `gitauto open` | Open remote repository in browser |
| `gitauto version` | Show Git and gitAuto version |

---

### 🚀 Push & Sync

| Command | Description |
|--------|-------------|
| `gitauto push` | Smart push (pull + add + commit + push) |
| `gitauto push -m "msg"` | Custom commit message |
| `gitauto push -f` | Safe force push (--force-with-lease) |
| `gitauto push -ff` | Dangerous force push (--force) |
| `gitauto pull` | Smart pull (rebase + autostash) |
| `gitauto pull -f` | Force sync to remote |
| `gitauto pull -ff` | Hard reset local repository |
| `gitauto sync` | Pull + push synchronization |

---

### 👀 Watch Mode (Auto Sync)

| Command | Description |
|--------|-------------|
| `gitauto watch` | File watcher with auto sync |
| `gitauto watch -q` | Quiet background watch mode |

---

### 🌿 Branch Management

| Command | Description |
|--------|-------------|
| `gitauto branch` | List branches |
| `gitauto checkout <branch>` | Switch branch (create if not exists) |
| `gitauto current` | Show current branch |

---

### 💾 Stash / Workspace

| Command | Description |
|--------|-------------|
| `gitauto save` | Stash current changes |
| `gitauto save -m "msg"` | Stash with message |
| `gitauto restore` | Pop latest stash |
| `gitauto restore -a` | Apply stash (keep stash) |
| `gitauto restore <N>` | Restore specific stash |
| `gitauto restore list` | List all stashes |

---

### 🧹 Cleanup & Reset

| Command | Description |
|--------|-------------|
| `gitauto clean` | Preview and clean untracked files |
| `gitauto clean -f` | Force clean |
| `gitauto clean -ff` | Remove ignored + untracked files |
| `gitauto reset` | Hard reset to HEAD |
| `gitauto reset <N>` | Reset back N commits |

---

### ⏪ Undo System

| Command | Description |
|--------|-------------|
| `gitauto undo` | Soft reset HEAD~1 |
| `gitauto undo mixed` | Mixed reset |
| `gitauto undo hard` | Hard reset |
| `gitauto undo <N>` | Undo N commits |

---

### 🧠 Commit Enhancement

| Command | Description |
|--------|-------------|
| `gitauto amend` | Amend last commit |
| `gitauto amend -m` | Amend commit message |

---

### 🧹 Ignore Management

| Command | Description |
|--------|-------------|
| `gitauto ignore node` | Add Node.js ignore template |
| `gitauto ignore add "<rule>"` | Add custom ignore rule |

---

### 🧪 Repository Diagnostics

| Command | Description |
|--------|-------------|
| `gitauto doctor` | Check repository health |
| `gitauto doctor -f` | Auto-fix mode |

---

## 🧠 Design Philosophy

### push = Automated Sync Model

```bash
gitauto push
↓
git pull --rebase --autostash
git add .
git commit
git push
````

---

### watch = Continuous Development Loop

```text
File changes
→ Auto commit
→ Auto push
→ Continue watching
```

---

### undo = Time Rewind System

```text
soft   → keep staged changes
mixed  → unstage changes
hard   → discard everything
```

---

## ⚙️ Internal Command Mapping

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

## 📁 Configuration System

Path:

```text
.git/gitauto.conf
```

Example:

```conf
countdown=5
watch_whitelist=src/,include/
watch_blacklist=.git/,build/
```

Features:

* Auto-generated if missing
* No restart required
* Live applied in watch mode

---

## 🧱 .gitignore Management Strategy

gitAuto manages a safe isolated section:

```gitignore
# >>> gitAuto
/node_modules/
/build/
# <<< gitAuto
```

Rules:

* Does not modify user-defined rules
* Prevents duplicate entries
* Only edits managed block

---

## ⚠️ Output Levels

* `[ERROR]` → Critical errors
* `[WARN]` → Warnings
* `[gitAuto]` → Tool status logs
* `[git]` → Native Git output

---

## 🪟 Platform Support

* Windows only
* Based on Win32 file system watcher API

---

## 🚧 Future Plans

* Linux / macOS support
* Conflict resolution assistant
* One-click installer & auto updater

---

## 📜 License

MIT License
