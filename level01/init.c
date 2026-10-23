/* Linux CTF - Level 01 Initializer (The Awakening) */
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
    const char *flag = argv[1];

    /* 1. Setup MOTD room environment config */
    FILE *fenv = fopen("/etc/linux_ctf_room.env", "w");
    if (fenv) {
        fprintf(fenv, "ROOM_ID=\"01\"\n");
        fprintf(fenv, "ROOM_TITLE=\"The Awakening\"\n");
        fprintf(fenv, "ROOM_COMMANDS=\"pwd, ls\"\n");
        fprintf(fenv, "ROOM_MISSION=\"The flag is in a hidden files in your home directory. Find a way to see the hidden file and fetch the flag.\"\n");
        fprintf(fenv, "ROOM_HINT=\"Read the man page for ls command like this: man ls. Read it and you will find the answer!\"\n");
        fclose(fenv);
        chmod("/etc/linux_ctf_room.env", 0644);
    }

    /* 2. Setup puzzle layout */
    mkdir("/home/learner/workspace", 0755);
    mkdir("/home/learner/docs", 0755);

    FILE *fw = fopen("/home/learner/welcome.txt", "w");
    if (fw) {
        fprintf(fw, "======================================================\n");
        fprintf(fw, "  Welcome to Linux CTF!\n");
        fprintf(fw, "======================================================\n");
        fprintf(fw, "This is an isolated sandbox environment.\n");
        fprintf(fw, "Your goal is to use Linux commands to locate the flag.\n");
        fprintf(fw, "A flag always looks like: FLAG{...}\n\n");
        fprintf(fw, "Run 'submit-flag <YOUR_FLAG>' once you discover it!\n");
        fclose(fw);
        chmod("/home/learner/welcome.txt", 0644);
    }

    FILE *fd = fopen("/home/learner/docs/guide.txt", "w");
    if (fd) {
        fprintf(fd, "Remember to test different command flags.\nRun 'man ls' or 'ls --help' for details.\n");
        fclose(fd);
        chmod("/home/learner/docs/guide.txt", 0644);
    }

    /* Plant dynamic hidden flag */
    FILE *ff = fopen("/home/learner/.hidden_flag", "w");
    if (ff) {
        fprintf(ff, "%s\n", flag);
        fclose(ff);
        chmod("/home/learner/.hidden_flag", 0644);
    }

    return 0;
}
