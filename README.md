![GitAuto](https://img.shields.io/badge/gitauto-lightweight%20git%20automation-red?style=for-the-badge)

![Windows Only](https://img.shields.io/badge/platform-Windows-blue?logo=windows)
![Language](https://img.shields.io/badge/language-C/C++-informational?logo=c)
![Tool](https://img.shields.io/badge/tool-git%20automation-orange?logo=git)
![Status](https://img.shields.io/badge/status-active-success)
![License](https://img.shields.io/badge/license-MIT-green)
![Build](https://img.shields.io/badge/build-passing-brightgreen)

# gitAuto

`gitAuto` is a **Lightweight Git Automation CLI Tool (Windows only)**
designed to simplify repetitive Git workflows such as commit, push, pull, and repository management.

It does not replace Git — it enhances it by reducing low-value repetitive operations.

---

## ✨ Feature Status

> ✔ = implemented now
> ⏳ = planned / future feature

| Status | Command              | Description                                  |
| ------ | -------------------- | -------------------------------------------- |
| ✔      | `gitauto init`       | Initialize repo, commit & push first version |
| ✔      | `gitauto link <url>` | Bind remote repository (origin + fetch)      |
| ✔      | `gitauto push`       | Smart push (rebase + add + commit + push)    |
| ✔      | `gitauto push -m`    | Commit with custom message                   |
| ✔      | `gitauto push -f`    | Safe force push (`--force-with-lease`)       |
| ✔      | `gitauto push -ff`   | Dangerous force push (`--force`)             |
| ✔      | `gitauto pull`       | Smart pull (rebase + autostash)              |
| ✔      | `gitauto pull -f`    | Hard reset to remote                         |
| ✔      | `gitauto pull -ff`   | Full clean reset (destructive)               |
| ✔      | `gitauto watch`      | Auto sync on file changes                    |
| ✔      | `gitauto watch -q`   | Quiet watch mode                             |
| ✔      | `gitauto sync`       | Pull + push synchronization                  |

| Status | Command                     | Description                   |
| ------ | --------------------------- | ----------------------------- |
| ⏳      | `gitauto clone <url>`       | Clone repo + auto setup       |
| ⏳      | `gitauto status`            | Pretty git status table       |
| ⏳      | `gitauto status --short`    | Compact status view           |
| ⏳      | `gitauto log`               | Pretty commit history         |
| ⏳      | `gitauto branch`            | Better branch viewer          |
| ⏳      | `gitauto checkout <branch>` | Smart checkout (auto create)  |
| ⏳      | `gitauto backup`            | Quick commit + push backup    |
| ⏳      | `gitauto save`              | Stash shortcut                |
| ⏳      | `gitauto restore`           | Pop stash                     |
| ⏳      | `gitauto ignore <rule>`     | Quick .gitignore editing      |
| ⏳      | `gitauto doctor`            | Diagnose git repo issues      |
| ⏳      | `gitauto clean`             | Remove untracked files        |
| ⏳      | `gitauto undo`              | Undo last commit (soft reset) |
| ⏳      | `gitauto amend`             | Amend last commit             |
| ⏳      | `gitauto reset`             | Hard reset HEAD               |
| ⏳      | `gitauto current`           | Show current branch           |
| ⏳      | `gitauto changed`           | Show changed files            |
| ⏳      | `gitauto open`              | Open remote repository        |
| ⏳      | `gitauto version`           | Show git & gitauto version    |

---

## 🚀 Core Philosophy

* Reduce repetitive Git commands
* Keep Git behavior predictable
* Provide safe defaults for destructive operations
* Offer automation without replacing Git itself

---

## 🚀 Usage

### View Help

```bash
gitauto
```

---

### Initialize Repository

```bash
gitauto init
```

Behavior:

* Initializes git repo if needed
* Performs first commit + push
* Configures upstream branch automatically
* Generates `.git/gitauto.conf`

---

### Smart Push

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

---

### Custom Commit

```bash
gitauto push -m "message"
```

---

### Force Push

```bash
gitauto push -f
gitauto push -ff
```

* `-f` → safe force (`--force-with-lease`)
* `-ff` → destructive force (`--force`)

---

### Smart Pull

```bash
gitauto pull
```

---

### Hard Sync

```bash
gitauto pull -f
gitauto pull -ff
```

* `-f` → reset to remote
* `-ff` → clean + reset (fully destructive)

---

### Watch Mode

```bash
gitauto watch
gitauto watch -q
```

* Watches file changes
* Auto commit + push after countdown
* `-q` reduces logs

---

### Sync Mode

```bash
gitauto sync
```

Pull → Push full sync cycle.

---

## ⚙️ Configuration File

```text
.git/gitauto.conf
```

Example:

```conf
countdown=5
watch_whitelist=src/,include/
watch_blacklist=.git/,build/
```

* Auto-generated if missing
* Hot-reloaded (no restart required)

---

## 📁 .gitignore Management

gitAuto manages a dedicated block:

```gitignore
# >>> gitAuto
/build/
/node_modules/
# <<< gitAuto
```

Rules:

* No duplicate entries
* Does not touch unrelated rules
* Only modifies managed block

---

## ⚠️ Output Levels

* `[ERROR]` → critical failures
* `[WARN]` → non-fatal warnings
* `[gitAuto]` → tool status output
* `[git]` → raw git output (hidden in quiet mode)

---

## 🪟 Platform

* Windows only (Win32 API based file watching)

---

## 📜 License

MIT License

---

## 🚧 Future Work

More features are actively being developed:

* Visual git history viewer
* Interactive branch switching UI
* Advanced conflict resolution assistant
* Cross-platform support (Linux/macOS)
* Plugin system for custom workflows

---
