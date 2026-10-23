/* Linux CTF - Level 03 Initializer (Ink and Paper) */
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "[-] Missing session flag parameter\n");
        return 1;
    }

    /* 1. Setup MOTD room environment config */
    FILE *fenv = fopen("/etc/linux_ctf_room.env", "w");
    if (fenv) {
        fprintf(fenv, "ROOM_ID=\"03\"\n");
        fprintf(fenv, "ROOM_TITLE=\"Ink and Paper\"\n");
        fprintf(fenv, "ROOM_COMMANDS=\"echo, >, >>\"\n");
        fprintf(fenv, "ROOM_MISSION=\"The server launch daemon is awaiting configuration in 'dispatch.conf'. Use a command to write the required configuration keys. Check the instructions file for the configuration keys. Once edited the file, execute './verify_dispatch' to reveal your flag!\"\n");
        fprintf(fenv, "ROOM_HINT=\"Check the man page of bash, read it and find if there is anything related to writing output, redirection of data\"\n");
        fclose(fenv);
        chmod("/etc/linux_ctf_room.env", 0644);
    }

    /* 2. Setup puzzle layout */
    FILE *fn = fopen("/home/learner/INSTRUCTIONS.txt", "w");
    if (fn) {
        fprintf(fn, "=========================================================\n");
        fprintf(fn, "  Mission: Configure the Dispatch Daemon\n");
        fprintf(fn, "=========================================================\n");
        fprintf(fn, "The dispatch daemon requires 'dispatch.conf' with exactly:\n\n");
        fprintf(fn, "Line 1: MODE=ACTIVE\n");
        fprintf(fn, "Line 2: TARGET=PRODUCTION\n\n");
        fprintf(fn, "Write the required configuration keys into 'dispatch.conf'.\n");
        fprintf(fn, "Ensure both lines are present and exact.\n");
        fprintf(fn, "Once ready, execute: ./verify_dispatch\n");
        fclose(fn);
        chmod("/home/learner/INSTRUCTIONS.txt", 0644);
    }

    /* 3. Link pre-compiled thin-client verifier (zero flags embedded) */
    unlink("/home/learner/verify_dispatch");
    symlink("/usr/local/bin/verify_dispatch", "/home/learner/verify_dispatch");

    return 0;
}
