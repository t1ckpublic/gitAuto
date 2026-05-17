<div align="center">

# gitAuto - Lightweight & Intuitive Semantic Git Automation Tool
#### [中文README](README_ZH.md)

<img src="https://img.shields.io/badge/gitauto-lightweight%20git%20automation-red?style=for-the-badge"/>


<br/>

<img src="https://img.shields.io/badge/platform-Windows-blue?logo=windows"/>
<img src="https://img.shields.io/badge/language-C-informational?logo=c"/>
<img src="https://img.shields.io/badge/tool-Git%20Automation-orange?logo=git"/>
<img src="https://img.shields.io/badge/status-active-success"/>
<img src="https://img.shields.io/badge/license-MIT-green"/>

</div>

<br/>

`gitAuto` is a **lightweight Git automation CLI tool (Windows only)**.

It does not replace Git. Instead, it provides a **minimal abstraction layer with workflow automation**, helping developers move faster with less cognitive overhead.

---

## ⚡ Ultra Minimal Usage!!!

The ultimate design goal of gitAuto is simple:

> **Reduce Git workflows to near single-keystroke operations.**

With aliases enabled:

```bash
gitauto push      → ga p
gitauto checkout  → ga co
gitauto restore   → ga re
gitauto branch    → ga br
````

In daily usage, workflows can even be reduced to:

```bash
g p   # pull + commit + push
g w   # auto watch & sync
g u   # undo changes
```

👉 **Git operations become muscle memory instead of commands.**

---

## 🚀 Feature Overview

```
gitauto init/i                         Initialize repository (init + add + commit + push)
gitauto clone/c <url>                  Clone repo and initialize workflow
gitauto link/l <url>                   Bind remote repository (set origin + fetch)

gitauto push/p                         Smart push (pull --rebase + add + commit + push)
gitauto push/p -m <msg>                Commit with custom message
gitauto push/p -f                      Safe force push (--force-with-lease)
gitauto push/p -ff                     Dangerous force push (--force)

gitauto pull/pl                        Smart pull (rebase + autostash)
gitauto pull/pl -f                     Hard sync (reset to origin/<branch>)
gitauto pull/pl -ff                    Full reset (clean + hard reset)

gitauto sync/s                         Bidirectional sync (pull + push)

gitauto watch/w                        Auto sync on file changes
gitauto watch/w -q                     Quiet mode (minimal output)

gitauto branch/br                      Show branch list
gitauto checkout/co <branch>           Switch branch (auto-create if not exists)

gitauto backup/b                       Create backup branch snapshot
gitauto backup/b restore/re            Restore from backup branch
gitauto backup/b restore/re <branch>   Restore specific snapshot

gitauto save/sv                        Stash working directory (stash push -u)
gitauto save/sv -m <msg>               Stash with message

gitauto restore/re                     Restore latest stash (pop)
gitauto restore/re -a                  Apply stash (keep stash)
gitauto restore/re <N>                 Restore stash by index
gitauto restore/re list/l              List stash entries

gitauto clean/cl                       Preview and clean untracked files
gitauto clean/cl -f                    Force clean untracked files
gitauto clean/cl -ff                   Deep clean (including ignored files)

gitauto ignore/ig node                 Add Node.js .gitignore template
gitauto ignore/ig add <rule>           Append custom ignore rule

gitauto undo/u                         Undo last commit (soft reset)
gitauto undo/u mixed/m                 Mixed reset (unstage changes)
gitauto undo/u hard/h                  Hard reset (discard changes)
gitauto undo/u <N>                     Undo last N commits

gitauto amend/a                        Amend last commit
gitauto amend -m <msg>                 Amend commit message

gitauto reset/r                        Hard reset to HEAD
gitauto reset/r <N>                    Reset back N commits

gitauto current/cur                    Show current branch
gitauto open/o                         Open remote repository in browser
gitauto version/v                      Show version info

gitauto doctor/dr                      Diagnose repository issues
gitauto doctor/dr -fix/-f              Auto-fix common issues
```

---

## 🧠 Command Design Philosophy

### ⚡ Intuitive Semantic System

gitAuto is not designed to simply shorten Git commands.

Instead:

> **It replaces Git syntax with human intent.**

---

### 🚀 push = “sync my current state”

```bash
gitauto push
```

Not:

* add?
* commit?
* pull?
* rebase?

But a single intent:

> **“I want to synchronize my current state to remote.”**

So gitAuto automatically handles the full workflow pipeline.

---

### 🌿 checkout = “enter a working state”

```bash
gitauto checkout <branch>
```

Not:

* git checkout
* git switch
* git branch -b

But:

> **“I want to enter this branch’s working state.”**

If the branch does not exist → it is created automatically.

Because entering a state implies:

> existence is optional, intention is primary.

---

### 💾 save / restore = “pause and resume work”

```bash
gitauto save
gitauto restore
```

Not a stash abstraction.

But:

> **save = pause current work**
> **restore = resume previous work**

Users don’t need to understand Git stash mechanics—only the mental model:

* pause
* resume

---

### ⏪ undo = “degree of rollback, not Git parameters”

```text
undo
undo mixed
undo hard
```

Semantic model:

> Not reset flags, but **how far do I want to go back?**

* soft → keep changes
* mixed → unstage changes
* hard → discard everything

👉 Focus is intent, not Git internals.

---

### 👀 watch = “enter automated workflow mode”

```bash
gitauto watch
```

Not “file watching”.

But:

> **a persistent synchronized working mode**

Meaning:

* enter watch mode
* system handles sync automatically

No manual Git workflow needed anymore.

---

### 🧠 Design Principles Summary

The semantic system follows three rules:

* Do not expose Git internal mechanics
* Commands express **intent, not steps**
* Use human mental models (enter / pause / sync / rewind)

---

### ⚡ One-line Summary

> gitAuto is not a Git shortcut tool.  
> It is a **natural intent mapping layer over Git workflows.**

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

* Auto-generated
* No restart required
* Dynamically applied in watch mode

---

## 🧱 .gitignore Strategy

gitAuto manages a dedicated safe block:

```gitignore
# >>> gitAuto
/node_modules/
/build/
# <<< gitAuto
```

Rules:

* Never modifies user-defined rules
* No duplication
* Only manages isolated section

---

## ⚠️ Output Levels

* `[ERROR]` → critical failures
* `[WARN]` → warnings
* `[gitAuto]` → tool logs
* `[git]` → native Git output

---

## 🪟 Platform Support

* Windows only
* Built on Win32 file system watcher API

---

## 🚧 Future Roadmap

* Linux / macOS support
* Conflict resolution assistant
* One-click installer & auto updater

---

## 📜 License

MIT License