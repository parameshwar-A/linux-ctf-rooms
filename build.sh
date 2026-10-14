#!/usr/bin/env bash
# ==============================================================================
# Linux CTF - Author / Challenges Build & Release Utility
# This script manages challenge building and pushing images to Docker Hub.
# ==============================================================================

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

DOCKER_USER="${DOCKER_USER:-paramesh273}"

usage() {
    echo "Linux CTF - Author Build Utility"
    echo "Usage: $0 [command] [args]"
    echo ""
    echo "Commands:"
    echo "  build-base                  Build shared base image (linux-ctf-base:latest)"
    echo "  build <room_id>             Build specific room challenge (e.g. 01, level01)"
    echo "  build-all                   Build all room challenge images"
    echo "  test <room_id>              Run dynamic validation test on a room"
    echo "  push <room_id> [username]   Tag and push room image to Docker Hub"
    echo "  push-all [username]         Tag and push all built room images to Docker Hub"
    echo "  clean-all                   Clean up all built room images"
    echo ""
    exit 1
}

normalize_room() {
    local raw="$1"
    raw="${raw#level}"
    printf "%02d" "$((10#$raw))"
}

cmd="${1:-}"
shift || true

case "$cmd" in
    build-base)
        echo "[*] Building linux-ctf-base:latest..."
        podman build -t linux-ctf-base:latest -f common/Containerfile.base .
        echo "[+] Base image built successfully."
        ;;
    build)
        room_num=$(normalize_room "${1:-01}")
        room_dir="level${room_num}"
        if [ ! -d "$room_dir" ]; then
            echo "[-] Room directory $room_dir does not exist."
            exit 1
        fi
        if ! podman image exists "linux-ctf-base:latest"; then
            echo "[*] Base image missing. Building base image first..."
            podman build -t linux-ctf-base:latest -f common/Containerfile.base .
        fi
        echo "[*] Building challenge image for Room ${room_num}..."
        podman build -t "linux-ctf-level${room_num}:latest" -f "${room_dir}/Containerfile" .
        echo "[+] Room ${room_num} image built successfully and is READY."
        ;;
    build-all)
        if ! podman image exists "linux-ctf-base:latest"; then
            echo "[*] Building base image..."
            podman build -t linux-ctf-base:latest -f common/Containerfile.base .
        fi
        for d in level*; do
            if [ -d "$d" ]; then
                rname=$(basename "$d")
                rnum=$(normalize_room "$rname")
                echo "[*] Building Room ${rnum}..."
                podman build -t "linux-ctf-level${rnum}:latest" -f "${d}/Containerfile" .
            fi
        done
        echo "[+] All challenge images built successfully."
        ;;
    test)
        room_num=$(normalize_room "${1:-01}")
        tag="linux-ctf-level${room_num}:latest"
        if ! podman image exists "$tag"; then
            echo "[-] Room level${room_num} is not built yet. Run: $0 build ${room_num}"
            exit 1
        fi

        test_token=$(head -c 6 /dev/urandom | xxd -p)
        test_flag="FLAG{l${room_num}_${test_token}}"
        test_salt="$(head -c 8 /dev/urandom | xxd -p)"
        test_hash=$(echo -n "${test_flag}::${test_salt}" | sha256sum | cut -d' ' -f1)

        echo "[*] Testing Room ${room_num} with dynamic flag..."
        
        # Test incorrect flag
        podman run --rm \
            -e CTF_FLAG="$test_flag" \
            -e CTF_SALT="$test_salt" \
            -e CTF_HASH="$test_hash" \
            "$tag" "/usr/local/bin/submit-flag 'FLAG{wrong_guess}'" || true
        
        # Test correct flag
        podman run --rm \
            -e CTF_FLAG="$test_flag" \
            -e CTF_SALT="$test_salt" \
            -e CTF_HASH="$test_hash" \
            "$tag" "/usr/local/bin/submit-flag '$test_flag'"
        echo "[+] Test passed for Room ${room_num}!"
        ;;
    push)
        room_num=$(normalize_room "${1:-01}")
        user="${2:-$DOCKER_USER}"
        local_tag="linux-ctf-level${room_num}:latest"
        remote_tag="docker.io/${user}/linux-ctf-level${room_num}:latest"
        echo "[*] Tagging ${local_tag} as ${remote_tag}..."
        podman tag "$local_tag" "$remote_tag"
        echo "[*] Pushing ${remote_tag} to Docker Hub..."
        podman push "$remote_tag"
        echo "[+] Successfully pushed ${remote_tag}."
        ;;
    push-all)
        user="${1:-$DOCKER_USER}"
        for d in level*; do
            if [ -d "$d" ]; then
                rname=$(basename "$d")
                rnum=$(normalize_room "$rname")
                local_tag="linux-ctf-level${rnum}:latest"
                if podman image exists "$local_tag"; then
                    remote_tag="docker.io/${user}/linux-ctf-level${rnum}:latest"
                    echo "[*] Pushing ${remote_tag}..."
                    podman tag "$local_tag" "$remote_tag"
                    podman push "$remote_tag"
                fi
            fi
        done
        echo "[+] All built room images pushed to Docker Hub (${user})."
        ;;
    clean-all)
        echo "[*] Removing all built challenge images..."
        podman images --filter "reference=*linux-ctf-level*" --format "{{.ID}}" | xargs -r podman rmi -f
        echo "[+] Cleaned all room images."
        ;;
    *)
        usage
        ;;
esac
