/* Linux CTF - Level 12 Initializer (The Log Whisperer) */
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
        fprintf(fenv, "ROOM_ID=\"12\"\n");
        fprintf(fenv, "ROOM_TITLE=\"The Log Whisperer\"\n");
        fprintf(fenv, "ROOM_COMMANDS=\"grep\"\n");
        fprintf(fenv, "ROOM_MISSION=\"The 'logs/' directory contains dozens of access logs with thousands of web requests. Find the suspicious entry tagged with error code 'ERR_LEAK_9918' and extract the leaked flag!\"\n");
        fprintf(fenv, "ROOM_HINT=\"Find a command which can help us to search through files. Use 'man -k <keyword>' for keyword based searches. Search the man pages for keywords like: search for patterns / match pattern / search\"\n");
        fclose(fenv);
        chmod("/etc/linux_ctf_room.env", 0644);
    }

    /* 2. Setup log directories with decoys */
    mkdir("/home/learner/logs", 0755);
    mkdir("/home/learner/logs/app", 0755);
    mkdir("/home/learner/logs/auth", 0755);
    mkdir("/home/learner/logs/audit", 0755);

    FILE *fdec = fopen("/run/ctf/decoys", "w");

    for (int i = 1; i <= 6; i++) {
        char path[128];
        snprintf(path, sizeof(path), "/home/learner/logs/app/server_%02d.log", i);
        FILE *fp = fopen(path, "w");
        if (fp) {
            for (int j = 1; j <= 50; j++) {
                if (i == 4 && j == 27) {
                    fprintf(fp, "[2026-10-14 14:02:11] [ERROR] [ERR_LEAK_9918] Authentication token leaked: %s\n", flag);
                } else if (j == 10 || j == 35) {
                    char decoy[128];
                    snprintf(decoy, sizeof(decoy), "FLAG{l12_decoy_err_leak_noise_srv%02d_j%02d}", i, j);
                    fprintf(fp, "[2026-10-14 14:01:%02d] [WARN] [ERR_NOISE_%04d] Leaked session candidate: %s\n",
                        j % 60, (i * 100 + j), decoy);
                    if (fdec) fprintf(fdec, "%s\n", decoy);
                } else {
                    fprintf(fp, "[2026-10-14 14:01:%02d] [INFO] GET /api/v1/status HTTP/1.1 200 OK - client: 192.168.1.%d\n",
                        j % 60, (j * 7) % 250);
                }
            }
            fclose(fp);
            chmod(path, 0644);
        }
    }
    if (fdec) fclose(fdec);

    return 0;
}
