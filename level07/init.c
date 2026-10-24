/* Linux CTF - Level 07 Initializer (Disguised Identities) */
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
        fprintf(fenv, "ROOM_ID=\"07\"\n");
        fprintf(fenv, "ROOM_TITLE=\"Disguised Identities\"\n");
        fprintf(fenv, "ROOM_COMMANDS=\"file\"\n");
        fprintf(fenv, "ROOM_MISSION=\"Inspect the files inside 'suspects/'. Files inside the suspects have misleading extensions (.png, .mp3, .pdf, .bin, .tar). Exactly one file is an ASCII text file containing the mission flag. Find a way to detect the actual type of the file rather than the file extension.\"\n");
        fprintf(fenv, "ROOM_HINT=\"Use 'man -k <keyword>' for keyword based searches. Search the man pages for keywords like: file type / file / determining file content type\"\n");
        fclose(fenv);
        chmod("/etc/linux_ctf_room.env", 0644);
    }

    /* 2. Setup puzzle layout with decoys */
    mkdir("/home/learner/suspects", 0755);

    const char *filenames[] = {"photo.png", "audio.mp3", "manual.pdf", "backup.tar", "firmware.bin"};
    int target_idx = ctf_rand(5);

    /* Generate 32-hex random decoy tokens */
    char decoy_tokens[5][33];
    for (int i = 0; i < 5; i++) {
        ctf_rand_hex(decoy_tokens[i], 16);
    }

    /* Record decoys in /run/ctf/decoys */
    FILE *fd = fopen("/run/ctf/decoys", "w");
    if (fd) {
        for (int i = 0; i < 5; i++) {
            if (i != target_idx) {
                fprintf(fd, "FLAG{l07_%s}\n", decoy_tokens[i]);
            }
        }
        fclose(fd);
    }

    for (int i = 0; i < 5; i++) {
        char path[256];
        snprintf(path, sizeof(path), "/home/learner/suspects/%s", filenames[i]);

        if (i == target_idx) {
            /* The one genuine ASCII text file */
            FILE *ff = fopen(path, "w");
            if (ff) {
                fprintf(ff, "CLASSIFIED INVESTIGATION MEMORANDUM\n");
                fprintf(ff, "The file extension was intentionally disguised to mislead casual observers.\n");
                fprintf(ff, "GENUINE MISSION FLAG:\n%s\n", flag);
                fclose(ff);
                chmod(path, 0644);
            }
        } else {
            /* Non-ASCII binary / extended data stream with decoy flag */
            FILE *fb = fopen(path, "wb");
            if (fb) {
                unsigned char magic[8];
                if (i == 0) {
                    unsigned char h[] = {0x89, 0xFE, 0xED, 0xFA, 0xCE, 0xDE, 0xAD, 0xBE};
                    memcpy(magic, h, 8);
                } else if (i == 1) {
                    unsigned char h[] = {0xFF, 0xFB, 0x90, 0x44, 0xBC, 0xDE, 0xF0, 0x12};
                    memcpy(magic, h, 8);
                } else if (i == 2) {
                    unsigned char h[] = {0x80, 0x91, 0xA2, 0xB3, 0xC4, 0xD5, 0xE6, 0xF7};
                    memcpy(magic, h, 8);
                } else {
                    unsigned char h[] = {0xEB, 0xFE, 0x90, 0x90, 0xCD, 0x19, 0xFA, 0xF4};
                    memcpy(magic, h, 8);
                }
                fwrite(magic, 1, 8, fb);
                fprintf(fb, "\n[SUSPECT EVIDENCE TELEMETRY DUMP]\n");
                fprintf(fb, "FLAG{l07_%s}\n", decoy_tokens[i]);
                for (int b = 0; b < 32; b++) {
                    fputc(0x80 + (b * 13) % 120, fb);
                }
                fputc('\n', fb);
                fclose(fb);
                chmod(path, 0644);
            }
        }
    }

    return 0;
}
