/* Linux CTF - Level 12 Initializer (The Log Whisperer) */
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
        fprintf(fenv, "ROOM_ID=\"12\"\n");
        fprintf(fenv, "ROOM_TITLE=\"The Log Whisperer\"\n");
        fprintf(fenv, "ROOM_COMMANDS=\"grep\"\n");
        fprintf(fenv, "ROOM_MISSION=\"Locate the error code 'ERR_LEAK_9918' across logs in 'logs/' using 'grep' to find the flag!\"\n");
        fprintf(fenv, "ROOM_HINT=\"Run 'grep -rn \"ERR_LEAK_9918\" logs/' to find the file and matching line.\"\n");
        fclose(fenv);
        chmod("/etc/linux_ctf_room.env", 0644);
    }

    /* 2. Compile hardened submit-flag binary */
    char compile_cmd[1024];
    snprintf(compile_cmd, sizeof(compile_cmd),
        "gcc -O2 -s -DEXPECTED_HASH='\"%s\"' -DSALT='\"%s\"' -DROOM_NAME='\"Level 12 - The Log Whisperer\"' "
        "/opt/linux_ctf/submit_flag.c -o /usr/local/bin/submit-flag",
        hash, salt);
    if (system(compile_cmd) != 0) {
        fprintf(stderr, "[-] Failed to compile validator\n");
    }
    chmod("/usr/local/bin/submit-flag", 0755);

    /* 3. Setup log directories */
    mkdir("/home/learner/logs", 0755);
    mkdir("/home/learner/logs/app", 0755);
    mkdir("/home/learner/logs/auth", 0755);
    mkdir("/home/learner/logs/audit", 0755);

    for (int i = 1; i <= 6; i++) {
        char path[128];
        snprintf(path, sizeof(path), "/home/learner/logs/app/server_%02d.log", i);
        FILE *fp = fopen(path, "w");
        if (fp) {
            for (int j = 1; j <= 50; j++) {
                if (i == 4 && j == 27) {
                    fprintf(fp, "[2026-10-14 14:02:11] [ERROR] [ERR_LEAK_9918] Authentication token leaked: %s\n", flag);
                } else {
                    fprintf(fp, "[2026-10-14 14:01:%02d] [INFO] GET /api/v1/status HTTP/1.1 200 OK - client: 192.168.1.%d\n",
                        j % 60, (j * 7) % 250);
                }
            }
            fclose(fp);
            chmod(path, 0644);
        }
    }

    system("chown -R learner:learner /home/learner");
    return 0;
}
