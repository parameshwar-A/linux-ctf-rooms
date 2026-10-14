# Linux CTF - Challenge Rooms (Author Repository)

This repository contains the source code, puzzle environments, and build automation for **Linux CTF** challenge images. Challenge images built here are published to Docker Hub and pulled by the player client application.

---

## Security & Architecture Principles

1. **No Baked Flags in Images**:
   Challenge images never contain static flags, salts, or hashes. Flags are generated dynamically at runtime by the player's client application and injected at container startup.
2. **Compiled Binary Initializers (`init.c`)**:
   Puzzle directories, decoy files, and flag placements are written in C (`init.c`) and compiled into a stripped binary (`/usr/local/bin/init-room`) during image build. The source `init.c` is wiped from the image layer.
   * `podman image history` shows only `RUN gcc -O2 -s ...`.
   * Players pulling the image from Docker Hub cannot read shell scripts or file paths to spoil the puzzle.
3. **Hardened In-Container Validator (`submit-flag`)**:
   On container boot, the entrypoint executes `/usr/local/bin/init-room`, which compiles `/usr/local/bin/submit-flag` with the dynamic salted SHA-256 hash in ~70ms. The binary is stripped (`-s`) with no plaintext flags visible via `strings`.

---

## Room Structure

Each challenge level is organized as follows:

```
.
├── build.sh                  # Release & build automation utility
├── README.md                 # Author documentation
├── common/                   # Shared base image assets & validator source
│   ├── Containerfile.base    # Base Debian Bookworm image definition
│   ├── scripts/              # In-container entrypoint & motd banner
│   └── validator/            # submit_flag.c validator source
├── level01/
│   ├── Containerfile         # Builds from linux-ctf-base:latest
│   ├── init.c                # C binary puzzle generator & submit-flag compiler
│   └── metadata.json         # Title, commands, mission, hint (no secrets)
└── level02/
    ├── Containerfile
    ├── init.c
    └── metadata.json
```

---

## Challenge Authoring Utility (`build.sh`)

Use `./build.sh` to build, test, and push images to Docker Hub:

```bash
# 1. Build the shared base image (Debian Bookworm + coreutils + man-db + gcc)
./build.sh build-base

# 2. Build a specific room challenge image
./build.sh build 01

# 3. Build all room challenge images
./build.sh build-all

# 4. Run automated dynamic validation test on a room
./build.sh test 01

# 5. Tag and push a room image to Docker Hub
./build.sh push 01 paramesh

# 6. Tag and push all built room images to Docker Hub
./build.sh push-all paramesh

# 7. Clean up all locally built room images
./build.sh clean-all
```

---

## Creating a New Room

To create a new level (e.g. `level03`):

1. **Create directory**: `mkdir -p level03`
2. **Write `metadata.json`**:
   ```json
   {
     "id": "03",
     "slug": "level03",
     "title": "Room Title",
     "tier": "Beginner",
     "commands": ["echo", ">", ">>"],
     "mission": "Briefing description for learner.",
     "hint": "Command hint."
   }
   ```
3. **Write `init.c`**:
   - Takes `argv[1]` (`flag`), `argv[2]` (`salt`), `argv[3]` (`hash`).
   - Writes `/etc/linux_ctf_room.env` (for in-container login MOTD).
   - Compiles `/usr/local/bin/submit-flag` with `-DEXPECTED_HASH` and `-DSALT`.
   - Generates directory layout and files in `/home/learner/`.
   - Plants `$flag` into puzzle destination.
   - Sets ownership `chown -R learner:learner /home/learner`.
4. **Write `Containerfile`**:
   ```dockerfile
   FROM linux-ctf-base:latest

   USER root
   COPY level03/init.c /tmp/init.c
   RUN gcc -O2 -s /tmp/init.c -o /usr/local/bin/init-room && rm -f /tmp/init.c

   WORKDIR /home/learner
   CMD ["/bin/bash"]
   ```
5. **Test and Release**:
   ```bash
   ./build.sh build 03
   ./build.sh test 03
   ./build.sh push 03 paramesh
   ```
