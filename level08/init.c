/* Linux CTF - Level 08 Initializer (Head and Shoulders) */
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
        fprintf(fenv, "ROOM_ID=\"08\"\n");
        fprintf(fenv, "ROOM_TITLE=\"Head and Shoulders\"\n");
        fprintf(fenv, "ROOM_COMMANDS=\"head, tail\"\n");
        fprintf(fenv, "ROOM_MISSION=\"Extract the secret recorded precisely at line 500 of 'sensor_stream.log' using 'head' and 'tail'!\"\n");
        fprintf(fenv, "ROOM_HINT=\"Run 'head -n 500 sensor_stream.log | tail -n 1' to isolate line 500.\"\n");
        fclose(fenv);
        chmod("/etc/linux_ctf_room.env", 0644);
    }

    /* 2. Compile hardened submit-flag binary */
    char compile_cmd[1024];
    snprintf(compile_cmd, sizeof(compile_cmd),
        "gcc -O2 -s -DEXPECTED_HASH='\"%s\"' -DSALT='\"%s\"' -DROOM_NAME='\"Level 08 - Head and Shoulders\"' "
        "/opt/linux_ctf/submit_flag.c -o /usr/local/bin/submit-flag",
        hash, salt);
    if (system(compile_cmd) != 0) {
        fprintf(stderr, "[-] Failed to compile validator\n");
    }
    chmod("/usr/local/bin/submit-flag", 0755);

    /* 3. Setup 1,000 line telemetry log */
    FILE *fl = fopen("/home/learner/sensor_stream.log", "w");
    if (fl) {
        for (int i = 1; i <= 1000; i++) {
            if (i == 500) {
                fprintf(fl, "[SENSOR-500] CRITICAL PAYLOAD DETECTED: %s\n", flag);
            } else {
                fprintf(fl, "[SENSOR-%03d] Telemetry packet OK - Voltage: 3.%02dV, Temp: 2%d.%dC\n",
                    i, (i * 7) % 30, (i % 5) + 20, (i * 3) % 9);
            }
        }
        fclose(fl);
        chmod("/home/learner/sensor_stream.log", 0644);
    }

    system("chown -R learner:learner /home/learner");
    return 0;
}
