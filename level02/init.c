/* Linux CTF - Level 02 Initializer (The Reading Room) */
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
        fprintf(fenv, "ROOM_ID=\"02\"\n");
        fprintf(fenv, "ROOM_TITLE=\"The Reading Room\"\n");
        fprintf(fenv, "ROOM_COMMANDS=\"cd, cat\"\n");
        fprintf(fenv, "ROOM_MISSION=\"Navigate into 'archives/classified/2026' and read the secret document.\"\n");
        fprintf(fenv, "ROOM_HINT=\"Use manpage -k 'keyword' command for keyword based searches, Search for keywords like changing directories/ printing contents / standard output\"\n");
        fclose(fenv);
        chmod("/etc/linux_ctf_room.env", 0644);
    }

    /* 2. Compile hardened submit-flag binary (~70ms) */
    char compile_cmd[1024];
    snprintf(compile_cmd, sizeof(compile_cmd),
        "gcc -O2 -s -DEXPECTED_HASH='\"%s\"' -DSALT='\"%s\"' -DROOM_NAME='\"Level 02 - The Reading Room\"' "
        "/opt/linux_ctf/submit_flag.c -o /usr/local/bin/submit-flag",
        hash, salt);
    if (system(compile_cmd) != 0) {
        fprintf(stderr, "[-] Failed to compile validator\n");
    }
    chmod("/usr/local/bin/submit-flag", 0755);

    /* 3. Setup puzzle layout */
    mkdir("/home/learner/archives", 0755);
    mkdir("/home/learner/archives/classified", 0755);
    mkdir("/home/learner/archives/classified/2026", 0755);

    FILE *fr = fopen("/home/learner/archives/readme.txt", "w");
    if (fr) {
        fprintf(fr, "Classified documents are stored in the archive subdirectories.\n");
        fprintf(fr, "Find your way into 2026 to view the briefing.\n");
        fclose(fr);
        chmod("/home/learner/archives/readme.txt", 0644);
    }

    /* Plant secret document with spaces in filename */
    const char *secret_path = "/home/learner/archives/classified/2026/top secret document.txt";
    FILE *fs = fopen(secret_path, "w");
    if (fs) {
        fprintf(fs, "%s\n", flag);
        fclose(fs);
        chmod(secret_path, 0644);
    }

    /* Set ownership */
    system("chown -R learner:learner /home/learner");

    return 0;
}
