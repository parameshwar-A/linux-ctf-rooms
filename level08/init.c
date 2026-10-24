/* Linux CTF - Level 08 Initializer (Head and Shoulders) */
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
        fprintf(fenv, "ROOM_ID=\"08\"\n");
        fprintf(fenv, "ROOM_TITLE=\"Head and Shoulders\"\n");
        fprintf(fenv, "ROOM_COMMANDS=\"head, tail\"\n");
        fprintf(fenv, "ROOM_MISSION=\"The telemetry stream 'sensor_stream.log' contains 1,000 lines of data. The mission flag is recorded precisely at line 500. Use any commands but we recommend to use 'head' and 'tail' to extract line 500!\"\n");
        fprintf(fenv, "ROOM_HINT=\"Feed the output from head back to tail and do some combined processing. Try reading the bash to see if they have any way to link output of one command to another. Use that\"\n");
        fclose(fenv);
        chmod("/etc/linux_ctf_room.env", 0644);
    }

    /* Record sample decoys in /run/ctf/decoys */
    FILE *fd = fopen("/run/ctf/decoys", "w");

    /* 2. Setup 1,000 line telemetry log with 32-hex flag decoys on every line */
    FILE *fl = fopen("/home/learner/sensor_stream.log", "w");
    if (fl) {
        for (int i = 1; i <= 1000; i++) {
            if (i == 500) {
                fprintf(fl, "[SENSOR-500] %s\n", flag);
            } else {
                char d_tok[33];
                ctf_rand_hex(d_tok, 16);
                fprintf(fl, "[SENSOR-%03d] FLAG{l08_%s}\n", i, d_tok);
            }
        }
        fclose(fl);
        chmod("/home/learner/sensor_stream.log", 0644);
    }
    if (fd) fclose(fd);

    return 0;
}
