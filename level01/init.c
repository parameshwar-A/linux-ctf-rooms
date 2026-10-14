/* Linux CTF - Level 01 Initializer (The Awakening) */
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "[-] Missing session parameters\n");
        return 1;
    }
    const char *flag = argv[1];
    const char *salt = argv[2];
    const char *hash = argv[3];

    /* 1. Setup MOTD room environment config */
    FILE *fenv = fopen("/etc/linux_ctf_room.env", "w");
    if (fenv) {
        fprintf(fenv, "ROOM_ID=\"01\"\n");
        fprintf(fenv, "ROOM_TITLE=\"The Awakening\"\n");
        fprintf(fenv, "ROOM_COMMANDS=\"pwd, ls\"\n");
        fprintf(fenv, "ROOM_MISSION=\"Locate the hidden flag (.hidden_flag) concealed in your home directory.\"\n");
        fprintf(fenv, "ROOM_HINT=\"Files starting with a '.' are hidden by default. Check 'man ls' to see how to list all files.\"\n");
        fclose(fenv);
        chmod("/etc/linux_ctf_room.env", 0644);
    }

    /* 2. Compile hardened submit-flag binary (~70ms) */
    char compile_cmd[1024];
    snprintf(compile_cmd, sizeof(compile_cmd),
        "gcc -O2 -s -DEXPECTED_HASH='\"%s\"' -DSALT='\"%s\"' -DROOM_NAME='\"Level 01 - The Awakening\"' "
        "/opt/linux_ctf/submit_flag.c -o /usr/local/bin/submit-flag",
        hash, salt);
    if (system(compile_cmd) != 0) {
        fprintf(stderr, "[-] Failed to compile validator\n");
    }
    chmod("/usr/local/bin/submit-flag", 0755);

    /* 3. Setup puzzle layout */
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

    /* Set ownership */
    system("chown -R learner:learner /home/learner");

    return 0;
}
