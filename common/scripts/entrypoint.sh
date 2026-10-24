#!/bin/bash
# Linux CTF - Hardened Daemon & Privilege Drop Entrypoint
set -euo pipefail

# 1. Setup root-only secrets directory
install -d -m 0700 /run/ctf

# 2. Extract flag from secret mount or environment
FLAG_FILE=/run/secrets/ctf_flag
ACTUAL_FLAG=""
if [ -f "$FLAG_FILE" ]; then
    install -m 0600 "$FLAG_FILE" /run/ctf/flag
    ACTUAL_FLAG="$(cat /run/ctf/flag)"
elif [ -n "${CTF_FLAG:-}" ]; then
    echo -n "$CTF_FLAG" > /run/ctf/flag
    chmod 0600 /run/ctf/flag
    ACTUAL_FLAG="$CTF_FLAG"
elif [ -f /run/ctf/flag ]; then
    ACTUAL_FLAG="$(cat /run/ctf/flag)"
fi

# 3. If compiled init-room exists, run it as root to prepare the room layout & decoys
if [ -x /usr/local/bin/init-room ]; then
    chmod 0700 /usr/local/bin/init-room
    /usr/local/bin/init-room "$ACTUAL_FLAG" "${CTF_SALT:-}" "${CTF_HASH:-}"
    chmod 0700 /usr/local/bin/init-room
fi

# 4. Start CTF check daemon as root (if installed)
if [ -x /usr/local/bin/ctf-checkd ]; then
    chmod 0700 /usr/local/bin/ctf-checkd
    /usr/local/bin/ctf-checkd &
    # Wait up to 2 seconds for socket to be ready
    for i in {1..20}; do
        [ -S /run/ctf.sock ] && break
        sleep 0.1
    done
fi

# 5. Ensure learner home directory is properly owned, but lock root-owned verifiers to 0711
if [ -d /home/learner ]; then
    chown -R learner:learner /home/learner
    # If any verifiers exist in home directory, ensure they are root-owned and execute-only
    for v in /home/learner/verify_* /home/learner/check_* /home/learner/report_*; do
        if [ -f "$v" ]; then
            chown root:root "$v"
            chmod 0711 "$v"
        fi
    done
fi

# 6. Wipe sensitive variables from root's environment
unset ACTUAL_FLAG CTF_FLAG CTF_SALT CTF_HASH

cd /home/learner

# 7. Drop privileges via setpriv
if [ $# -gt 1 ]; then
    exec setpriv --reuid=learner --regid=learner --init-groups \
         --inh-caps=-all --bounding-set=-all --no-new-privs \
         env -i HOME=/home/learner USER=learner TERM="${TERM:-xterm-256color}" PATH=/usr/local/bin:/usr/bin:/bin \
         "$@"
elif [ $# -eq 1 ]; then
    exec setpriv --reuid=learner --regid=learner --init-groups \
         --inh-caps=-all --bounding-set=-all --no-new-privs \
         env -i HOME=/home/learner USER=learner TERM="${TERM:-xterm-256color}" PATH=/usr/local/bin:/usr/bin:/bin \
         bash -c "$1"
else
    exec setpriv --reuid=learner --regid=learner --init-groups \
         --inh-caps=-all --bounding-set=-all --no-new-privs \
         env -i HOME=/home/learner USER=learner TERM="${TERM:-xterm-256color}" PATH=/usr/local/bin:/usr/bin:/bin \
         bash --login
fi
