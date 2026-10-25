# Linux CTF - Challenge Rooms (Author Repository)

This repository contains the source code, puzzle environments, in-container validation daemons, and build automation for all **Linux CTF** challenge rooms (Levels 01 through 15).

Challenge images built from this repository are published to container registries (such as Docker Hub) and pulled on-demand by the player's client application.

---

## Repositories & Ecosystem

* **Challenge Rooms & Author Repo**: [github.com/parameshwar-A/linux-ctf-rooms](https://github.com/parameshwar-A/linux-ctf-rooms) (`git@github.com:parameshwar-A/linux-ctf-rooms.git`)
* **Player App & TUI Engine**: [github.com/parameshwar-A/linux-ctf](https://github.com/parameshwar-A/linux-ctf) (`git@github.com:parameshwar-A/linux-ctf.git`)

---

## Architectural & Security Design Principles

### 1. Kerckhoffs's Principle in Room Design
A fundamental design goal of Linux CTF is adhering to **Kerckhoffs's Principle**: *a system should remain secure even if everything about it—except the key—is public knowledge.*

Because both Linux CTF repositories are 100% public and open-source:
* **Zero Baked Flags**: Images never contain static flags, hardcoded salts, or pre-computed hashes in image layers.
* **Per-Session Dynamic Entropy**: Every challenge container session is injected with an ephemeral 32-character hex flag (`FLAG{l<room>_<32_hex_chars>}`) generated at runtime via cryptographically secure pseudorandom number generators (`secrets.token_hex(16)` and kernel `getrandom()`).
* **Dynamic Puzzle Generation**: Reading `init.c` on GitHub reveals the learning objectives and mechanics, but **cannot reveal the flag**:
  * In Room 06, `sec-beacon` is randomly installed into `/usr/local/bin`, `/usr/bin`, or `/bin`.
  * In Room 07, the genuine ASCII file is randomly assigned among 5 decoy binary streams.
  * In Room 08, line 500 contains the real flag amidst 999 decoy lines.
  * In Room 09, the 42-line report is randomly assigned among 8 candidate files.
  * In Room 10, the unique transaction token is randomly placed among 200 duplicate records.
  * In Room 15, the attacker IP is generated dynamically from 16.6 million possible combinations.
  * To obtain the flag, the player **must execute the intended commands** inside their running container.

---

### 2. In-Container Daemon & Thin-Client Verifiers (`ctf-checkd`)
In earlier prototypes, verifier binaries held the flag or compiled hashes at container startup. In our hardened architecture, **no verifier binary contains flags or hashes**:
* **Root Daemon Isolation**: At container boot, the entrypoint starts `/usr/local/bin/ctf-checkd` as root in the background. The daemon binds to `/run/ctf.sock` with `0666` permissions.
* **Thin-Client Verifiers**: Verifiers like `submit-flag`, `verify_dispatch`, `check_production`, `verify_vault`, `sec-beacon`, and `report_attacker` are lightweight AF_UNIX socket clients. They simply send requests (e.g. `CHECK 03`, `SUBMIT <flag>`) over `/run/ctf.sock` and print the daemon's response.
* **Anti-Bruteforce Throttling**: The daemon enforces a **300ms delay** (`usleep(300000)`) on failed submissions or decoys, neutralizing automated enumeration loops while maintaining instant response times for genuine player solutions.
* **Non-Blocking Regular File Verification**: To prevent single-threaded daemon lockups, file inspection uses `safe_open_regular_read()` with `O_RDONLY | O_NOFOLLOW | O_NONBLOCK` and `S_ISREG()` checks. Named pipes (`mkfifo`) and symlinks fail closed immediately without hanging the daemon.
* **Fail-Closed Configuration**: If root-only answer keys (`/run/ctf/answer`) are unreadable or missing, the daemon strictly fails closed.

---

### 3. Decoy Flag Entropy (`common/ctf_rand.h`)
In inspection rooms (Rooms 07–13), decoys are generated using `common/ctf_rand.h` via the Linux kernel CSPRNG (`getrandom()`):
* Decoys are formatted identically to genuine flags: `FLAG{l<room>_<32_hex_chars>}`.
* Predictable substrings like `_decoy_` or numeric patterns are completely eliminated, preventing players from identifying genuine flags using regex grep filters.

---

### 4. Privilege Separation & In-Container Hardening
The container entrypoint (`common/scripts/entrypoint.sh`) enforces strict containment:
1. Creates root-only secrets storage: `/run/ctf` (mode `0700`, owned by `root:root`).
2. Extracts flag from Podman's tmpfs secret mount (`/run/secrets/ctf_flag`, mode `0400`).
3. Executes `/usr/local/bin/init-room` (mode `0700`) as root to construct puzzle layouts, then locks down permissions.
4. Starts `/usr/local/bin/ctf-checkd` (mode `0700`) in the background.
5. Clears sensitive variables (`unset ACTUAL_FLAG CTF_FLAG CTF_SALT CTF_HASH`).
6. Drops privileges using `setpriv`:
   ```bash
   exec setpriv --reuid=learner --regid=learner --init-groups \
        --inh-caps=-all --bounding-set=-all --no-new-privs \
        env -i HOME=/home/learner USER=learner TERM="${TERM:-xterm-256color}" PATH=/usr/local/bin:/usr/bin:/bin \
        bash --login
   ```
   The interactive learner shell operates as non-root `learner` with zero Linux capabilities, unable to read `/run/ctf/flag` or inspect daemon process memory via `/proc`.

---

## Prerequisites & Author Setup

### System Requirements
* **Operating System**: Linux (Ubuntu, Debian, Fedora, Arch Linux, openSUSE) or **macOS** (Apple Silicon / Intel)
* **Container Engine**: [Podman](https://podman.io/) (v4.0+)
* **Build Tools**: `gcc`, `libc6-dev`, `make`, `bash`, `coreutils`, `git`

### Platform Installation Commands

#### Ubuntu / Debian (22.04 LTS, 24.04 LTS, Debian 12+)
```bash
sudo apt update
sudo apt install -y podman build-essential git uidmap
```

#### Fedora / RHEL / CentOS Stream (Fedora 38+, RHEL 9+)
```bash
sudo dnf install -y podman gcc glibc-devel git shadow-utils
```

#### Arch Linux / Manjaro
```bash
sudo pacman -S --needed podman base-devel git shadow
```

#### macOS (Homebrew)
```bash
brew install podman git

# Initialize and start the Podman Linux virtual machine
podman machine init
podman machine start
```

---

### Clone the Repository

```bash
# Clone via SSH (recommended):
git clone git@github.com:parameshwar-A/linux-ctf-rooms.git
cd linux-ctf-rooms

# Or clone via HTTPS:
git clone https://github.com/parameshwar-A/linux-ctf-rooms.git
cd linux-ctf-rooms
```

---

## Beginner Tier Syllabus (Foundational Mastery)

The Beginner Tier features 15 progressive hands-on challenge rooms designed to build command-line muscle memory across core GNU/Linux utilities and workflows. Rather than static guides, learners solve realistic terminal challenges requiring navigation, text transformation, path inspection, and archiving.

### Core Commands & Competencies Covered
* **Filesystem Navigation & Inspection**: `pwd`, `ls` (`-a`, `-l`), `cd`, `file`
* **File Viewing & Manipulation**: `cat`, `cp`, `mv`, `mkdir`, `rm`, `rmdir`
* **Shell Redirection & Piping**: `echo`, `>`, `>>`, `|`
* **Binary & Environment Discovery**: `which`, `whereis`, `$PATH` exploration
* **Text Stream Processing**: `head`, `tail`, `wc` (`-l`), `tr`
* **Pattern Matching & Filtering**: `grep` (`-i`, `-n`), `sort`, `uniq` (`-u`, `-c`), `cut`
* **System Search & Archiving**: `find` (size and qualifier filters), `tar`, `gzip`
* **Manuals & Discovery**: `man`, `man -k` (apropos keyword searches)

Each room challenges learners to apply these commands in context to locate dynamic flags (`FLAG{...}`) while learning to overcome decoy traps and noise.

---

## Build Automation Utility (`build.sh`)

Use `./build.sh` to build, test, and push images:

```bash
# 1. Build the shared base image (Debian Bookworm + coreutils + man-db + ctf-checkd + submit-flag)
./build.sh build-base

# 2. Build a specific challenge room (e.g. Room 01)
./build.sh build 01

# 3. Build all challenge rooms (01 through 15)
./build.sh build-all

# 4. Run automated validation test on a room with dynamic Podman secret injection
./build.sh test 01

# 5. Tag and push a room image to Docker Hub
./build.sh push 01 paramesh273

# 6. Tag and push all built room images to Docker Hub
./build.sh push-all paramesh273

# 7. Clean up all locally built challenge images
./build.sh clean-all
```

---

## Challenge Room Structure

```
02_01_linux_ctf_rooms/
├── build.sh                      # Release & build automation utility
├── README.md                     # Author documentation
├── common/                       # Shared base image assets & validators
│   ├── Containerfile.base        # Base Debian Bookworm image definition
│   ├── ctf_rand.h                # Kernel CSPRNG helper (getrandom)
│   ├── scripts/
│   │   ├── entrypoint.sh         # Hardened daemon startup & setpriv privilege drop
│   │   ├── motd_banner.sh        # In-container login MOTD generator
│   │   └── hint.sh               # In-container hint helper
│   └── validator/
│       ├── ctf_checkd.c          # In-container validation daemon
│       ├── submit_flag_client.c  # Thin-client submit-flag executable
│       └── clients/              # Pre-compiled room-specific verifiers (0711)
│           ├── verify_dispatch.c # Room 03 verifier client
│           ├── check_production.c# Room 04 verifier client
│           ├── verify_vault.c    # Room 05 verifier client
│           ├── sec_beacon.c      # Room 06 verifier client
│           └── report_attacker.c # Room 15 verifier client
├── level01/ ... level15/
│   ├── Containerfile             # Multi-stage/room image definition
│   ├── init.c                    # C binary puzzle initializer
│   └── metadata.json             # Room title, commands, mission, hint (zero secrets)
└── .gitignore                    # Git exclusions
```

---

## Creating a New Room

To create a new challenge level (e.g. `level16`):

1. **Create Room Directory**:
   ```bash
   mkdir -p level16
   ```

2. **Define Metadata (`level16/metadata.json`)**:
   ```json
   {
     "id": "16",
     "slug": "level16",
     "title": "Room Title",
     "tier": "Beginner",
     "commands": ["command1", "command2"],
     "mission": "Briefing mission description for the learner.",
     "hint": "Command hint referencing man pages."
   }
   ```

3. **Implement Initializer (`level16/init.c`)**:
   * Reads dynamic flag from `argv[1]`.
   * Writes room environment variables to `/etc/linux_ctf_room.env` for the login MOTD.
   * Uses `ctf_rand.h` for any decoy generation or dynamic parameter selection.
   * Constructs puzzle filesystem layout in `/home/learner/`.
   * If using `ctf_checkd` verification, writes expected answer to root-only `/run/ctf/answer` (mode `0600`).

4. **Define Containerfile (`level16/Containerfile`)**:
   ```dockerfile
   FROM linux-ctf-base:latest

   USER root
   COPY level16/init.c /tmp/init.c
   RUN gcc -O2 -s /tmp/init.c -o /usr/local/bin/init-room && rm -f /tmp/init.c

   WORKDIR /home/learner
   CMD ["/bin/bash"]
   ```

5. **Build, Test, and Verify**:
   ```bash
   ./build.sh build 16
   ./build.sh test 16
   ```
