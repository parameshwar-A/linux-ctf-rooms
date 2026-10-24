/* Linux CTF - Level 05 Initializer (Excavation) */
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "[-] Missing flag parameter\n");
        return 1;
    }

    /* 1. Setup MOTD room environment config */
    FILE *fenv = fopen("/etc/linux_ctf_room.env", "w");
    if (fenv) {
        fprintf(fenv, "ROOM_ID=\"05\"\n");
        fprintf(fenv, "ROOM_TITLE=\"Excavation\"\n");
        fprintf(fenv, "ROOM_COMMANDS=\"mkdir, rm, rmdir\"\n");
        fprintf(fenv, "ROOM_MISSION=\"The storage area 'sandbox/clutter' is full of junk. Create the directory 'sandbox/secure_vault', move 'sandbox/clutter/secret_key.txt' into it, and clean up all remaining debris and empty directories in 'sandbox/clutter' using 'rm' and 'rmdir'. Run './verify_vault' to reveal the flag!\"\n");
        fprintf(fenv, "ROOM_HINT=\"Use 'man -k <keyword>' for keyword based searches. Search man pages for keywords like directories / make / remove\"\n");
        fclose(fenv);
        chmod("/etc/linux_ctf_room.env", 0644);
    }

    /* 2. Setup puzzle layout */
    mkdir("/home/learner/sandbox", 0755);
    mkdir("/home/learner/sandbox/clutter", 0755);
    mkdir("/home/learner/sandbox/clutter/empty_trash", 0755);
    mkdir("/home/learner/sandbox/clutter/old_logs", 0755);
    mkdir("/home/learner/sandbox/clutter/cache_dump", 0755);

    /* Generate clutter files */
    char dummy_path[256];
    for (int i = 1; i <= 15; i++) {
        snprintf(dummy_path, sizeof(dummy_path), "/home/learner/sandbox/clutter/sys_log_%02d.log", i);
        FILE *f = fopen(dummy_path, "w");
        if (f) { fprintf(f, "LOG_EVENT_%02d: Diagnostic event recorded.\n", i); fclose(f); }
    }
    for (int i = 1; i <= 15; i++) {
        snprintf(dummy_path, sizeof(dummy_path), "/home/learner/sandbox/clutter/temp_cache_%02d.tmp", i);
        FILE *f = fopen(dummy_path, "w");
        if (f) { fprintf(f, "CACHE_BLOCK_%02d: 0x%08X\n", i, i * 4096); fclose(f); }
    }
    for (int i = 1; i <= 10; i++) {
        snprintf(dummy_path, sizeof(dummy_path), "/home/learner/sandbox/clutter/dump_%02d.bak", i);
        FILE *f = fopen(dummy_path, "w");
        if (f) { fprintf(f, "CRASH_DUMP_%02d: MEMORY_OFFSET_OK\n", i); fclose(f); }
    }
    for (int i = 1; i <= 10; i++) {
        snprintf(dummy_path, sizeof(dummy_path), "/home/learner/sandbox/clutter/trash_archive_%02d.old", i);
        FILE *f = fopen(dummy_path, "w");
        if (f) { fprintf(f, "TRASH_RECORD_%02d: ARCHIVED_DEBRIS\n", i); fclose(f); }
    }

    /* Secret key in clutter */
    FILE *fk = fopen("/home/learner/sandbox/clutter/secret_key.txt", "w");
    if (fk) {
        fprintf(fk, "=========================================================\n");
        fprintf(fk, "  ENCRYPTED VAULT KEY CONTAINER\n");
        fprintf(fk, "=========================================================\n");
        fprintf(fk, "KEY_ID: VAULT-TOKEN-AUTHENTICATED\n");
        fprintf(fk, "STATUS: LOCKED_ENCRYPTED\n\n");
        fprintf(fk, "NOTICE: Move this key into 'sandbox/secure_vault/', clean up remaining clutter, and run './verify_vault'.\n");
        fclose(fk);
        chmod("/home/learner/sandbox/clutter/secret_key.txt", 0644);
    }

    /* 3. Link pre-compiled verifier */
    unlink("/home/learner/verify_vault");
    symlink("/usr/local/bin/verify_vault", "/home/learner/verify_vault");

    return 0;
}
