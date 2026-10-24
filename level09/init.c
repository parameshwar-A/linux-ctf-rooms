/* Linux CTF - Level 09 Initializer (By the Numbers) */
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <ctf_rand.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "[-] Missing flag parameter\n");
        return 1;
    }
    const char *flag = argv[1];

    /* 1. Setup MOTD room environment config */
    FILE *fenv = fopen("/etc/linux_ctf_room.env", "w");
    if (fenv) {
        fprintf(fenv, "ROOM_ID=\"09\"\n");
        fprintf(fenv, "ROOM_TITLE=\"By the Numbers\"\n");
        fprintf(fenv, "ROOM_COMMANDS=\"wc\"\n");
        fprintf(fenv, "ROOM_MISSION=\"The folder 'transcripts/' holds multiple candidate files. The authentic classified debrief has exactly 42 lines. There is a tool called wc which can do some word and file related checks. Read the man page for that and find the actual file which has 42 lines.\"\n");
        fprintf(fenv, "ROOM_HINT=\"Visit man page of 'wc' command, check for available option related to line counts\"\n");
        fclose(fenv);
        chmod("/etc/linux_ctf_room.env", 0644);
    }

    /* 2. Setup transcripts files with decoys */
    mkdir("/home/learner/transcripts", 0755);

    FILE *fd = fopen("/run/ctf/decoys", "w");

    int decoy_counts[] = {15, 23, 31, 38, 55, 61, 74, 80};
    int target_idx = ctf_rand(8);

    for (int idx = 0; idx < 8; idx++) {
        char filename[128];
        snprintf(filename, sizeof(filename), "/home/learner/transcripts/report_%02d.txt", idx + 1);
        FILE *fp = fopen(filename, "w");
        if (fp) {
            int target_lines = (idx == target_idx) ? 42 : decoy_counts[idx];
            for (int l = 1; l < target_lines; l++) {
                fprintf(fp, "Line %02d: Transcript telemetry entry #%d - Log status normal\n", l, l * 13);
            }
            if (idx == target_idx) {
                fprintf(fp, "[DEBRIEF CONCLUSION] AUTHENTICATED KEY: %s\n", flag);
            } else {
                char decoy_tok[33];
                ctf_rand_hex(decoy_tok, 16);
                char decoy[128];
                snprintf(decoy, sizeof(decoy), "FLAG{l09_%s}", decoy_tok);
                fprintf(fp, "[DEBRIEF CONCLUSION] AUTHENTICATED KEY: %s\n", decoy);
                if (fd) fprintf(fd, "%s\n", decoy);
            }
            fclose(fp);
            chmod(filename, 0644);
        }
    }
    if (fd) fclose(fd);

    return 0;
}
