/* Linux CTF - Level 07 Initializer (Disguised Identities) */
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
        fprintf(fenv, "ROOM_ID=\"07\"\n");
        fprintf(fenv, "ROOM_TITLE=\"Disguised Identities\"\n");
        fprintf(fenv, "ROOM_COMMANDS=\"file\"\n");
        fprintf(fenv, "ROOM_MISSION=\"Inspect the files inside 'suspects/'. Files inside the suspects have wrong extensions added. All files have misleading extensions (.png, .mp3, .pdf, .bin, .tar). Exactly one file is an ASCII text file containing the mission flag. Find a way to detect the actual type of the file rather than the file extension.\"\n");
        fprintf(fenv, "ROOM_HINT=\"Use 'man -k <keyword>' for keyword based searches. Search the man pages for keywords like: file type / file / determining file content type\"\n");
        fclose(fenv);
        chmod("/etc/linux_ctf_room.env", 0644);
    }

    /* 2. Compile hardened submit-flag binary */
    char compile_cmd[1024];
    snprintf(compile_cmd, sizeof(compile_cmd),
        "gcc -O2 -s -DEXPECTED_HASH='\"%s\"' -DSALT='\"%s\"' -DROOM_NAME='\"Level 07 - Disguised Identities\"' "
        "/opt/linux_ctf/submit_flag.c -o /usr/local/bin/submit-flag",
        hash, salt);
    if (system(compile_cmd) != 0) {
        fprintf(stderr, "[-] Failed to compile validator\n");
    }
    chmod("/usr/local/bin/submit-flag", 0755);

    /* 3. Setup puzzle layout */
    mkdir("/home/learner/suspects", 0755);

    /* photo.png is actually gzip compressed data */
    system("echo 'dummy binary compressed stream' | gzip > /home/learner/suspects/photo.png");

    /* audio.mp3 is binary data */
    FILE *fa = fopen("/home/learner/suspects/audio.mp3", "wb");
    if (fa) {
        unsigned char dummy[64] = {0xDE, 0xAD, 0xBE, 0xEF, 0x00, 0xFF, 0x12, 0x34};
        fwrite(dummy, 1, sizeof(dummy), fa);
        fclose(fa);
    }

    /* manual.pdf is an ELF binary (copy of /bin/true) */
    system("cp /bin/true /home/learner/suspects/manual.pdf");

    /* backup.tar is random binary bytes */
    system("head -c 128 /dev/urandom > /home/learner/suspects/backup.tar");

    /* firmware.bin is SECRETLY plain ASCII text holding the flag! */
    FILE *ff = fopen("/home/learner/suspects/firmware.bin", "w");
    if (ff) {
        fprintf(ff, "CLASSIFIED MEMORANDUM\n");
        fprintf(ff, "The firmware file extension was chosen to deter casual observers.\n");
        fprintf(ff, "AUTHENTICATED FLAG:\n%s\n", flag);
        fclose(ff);
        chmod("/home/learner/suspects/firmware.bin", 0644);
    }

    system("chown -R learner:learner /home/learner");
    return 0;
}
