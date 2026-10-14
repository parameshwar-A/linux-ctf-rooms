/* Linux CTF - Level 09 Initializer (By the Numbers) */
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
        fprintf(fenv, "ROOM_ID=\"09\"\n");
        fprintf(fenv, "ROOM_TITLE=\"By the Numbers\"\n");
        fprintf(fenv, "ROOM_COMMANDS=\"wc\"\n");
        fprintf(fenv, "ROOM_MISSION=\"One file in 'transcripts/' contains exactly 42 lines. Find it using 'wc -l' and extract the flag!\"\n");
        fprintf(fenv, "ROOM_HINT=\"Run 'wc -l transcripts/*' to count lines, identify the 42-line file, and read it with 'cat'.\"\n");
        fclose(fenv);
        chmod("/etc/linux_ctf_room.env", 0644);
    }

    /* 2. Compile hardened submit-flag binary */
    char compile_cmd[1024];
    snprintf(compile_cmd, sizeof(compile_cmd),
        "gcc -O2 -s -DEXPECTED_HASH='\"%s\"' -DSALT='\"%s\"' -DROOM_NAME='\"Level 09 - By the Numbers\"' "
        "/opt/linux_ctf/submit_flag.c -o /usr/local/bin/submit-flag",
        hash, salt);
    if (system(compile_cmd) != 0) {
        fprintf(stderr, "[-] Failed to compile validator\n");
    }
    chmod("/usr/local/bin/submit-flag", 0755);

    /* 3. Setup transcripts files */
    mkdir("/home/learner/transcripts", 0755);

    int line_counts[] = {15, 23, 38, 42, 55, 61, 74, 80};
    for (int idx = 0; idx < 8; idx++) {
        char filename[128];
        snprintf(filename, sizeof(filename), "/home/learner/transcripts/report_%02d.txt", idx + 1);
        FILE *fp = fopen(filename, "w");
        if (fp) {
            int target_lines = line_counts[idx];
            for (int l = 1; l <= target_lines; l++) {
                if (target_lines == 42 && l == 42) {
                    fprintf(fp, "[DEBRIEF CONCLUSION] AUTHENTICATED KEY: %s\n", flag);
                } else {
                    fprintf(fp, "Line %02d: Transcript telemetry entry #%d - Log status normal\n", l, l * 13);
                }
            }
            fclose(fp);
            chmod(filename, 0644);
        }
    }

    system("chown -R learner:learner /home/learner");
    return 0;
}
