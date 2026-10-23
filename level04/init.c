/* Linux CTF - Level 04 Initializer (Housekeeping) */
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

    /* 1. Setup MOTD room environment config */
    FILE *fenv = fopen("/etc/linux_ctf_room.env", "w");
    if (fenv) {
        fprintf(fenv, "ROOM_ID=\"04\"\n");
        fprintf(fenv, "ROOM_TITLE=\"Housekeeping\"\n");
        fprintf(fenv, "ROOM_COMMANDS=\"cp, mv\"\n");
        fprintf(fenv, "ROOM_MISSION=\"An archive payload was misplaced as 'staging/temp_payload.dat'. Move it to 'production/app_payload.dat', and create a duplicate backup copy at 'backups/app_payload.dat'. Run './check_production' to verify!\"\n");
        fprintf(fenv, "ROOM_HINT=\"Use 'man -k <keyword>' for keyword based searches. Search man pages for keywords like copy files / copy / move files\"\n");
        fclose(fenv);
        chmod("/etc/linux_ctf_room.env", 0644);
    }

    /* 2. Setup puzzle layout */
    mkdir("/home/learner/staging", 0755);
    mkdir("/home/learner/production", 0755);
    mkdir("/home/learner/backups", 0755);

    /* Misplaced payload in staging */
    FILE *fp = fopen("/home/learner/staging/temp_payload.dat", "w");
    if (fp) {
        fprintf(fp, "PAYLOAD_ID: APP-PAYLOAD-99214\n");
        fprintf(fp, "STATUS: PRODUCTION_READY\n");
        fprintf(fp, "CHECKSUM: 0xA7F3092C\n");
        fclose(fp);
        chmod("/home/learner/staging/temp_payload.dat", 0644);
    }

    FILE *fr = fopen("/home/learner/README.txt", "w");
    if (fr) {
        fprintf(fr, "Deployment Pipeline Maintenance:\n");
        fprintf(fr, "1. Move 'staging/temp_payload.dat' to 'production/app_payload.dat'.\n");
        fprintf(fr, "2. Copy 'production/app_payload.dat' to 'backups/app_payload.dat'.\n");
        fprintf(fr, "3. Run './check_production' to verify compliance.\n");
        fclose(fr);
        chmod("/home/learner/README.txt", 0644);
    }

    /* 3. Link pre-compiled verifier */
    unlink("/home/learner/check_production");
    symlink("/usr/local/bin/check_production", "/home/learner/check_production");

    return 0;
}
