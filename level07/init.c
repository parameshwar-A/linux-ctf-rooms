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

    /* Record decoys in /run/ctf/decoys */
    FILE *fd = fopen("/run/ctf/decoys", "w");
    if (fd) {
        for (int i = 0; i < 5; i++) {
            if (i != target_idx) {
                fprintf(fd, "FLAG{l07_decoy_suspect_%d}\n", i + 1);
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
            /* Binary file with decoy stream */
            if (i % 2 == 0) {
                /* Gzip compressed data */
                char tmp[256];
                snprintf(tmp, sizeof(tmp), "/tmp/plain_%d.txt", i);
                FILE *fp = fopen(tmp, "w");
                if (fp) {
                    fprintf(fp, "BINARY STREAM DUMP ... FLAG{l07_decoy_suspect_%d}\n", i + 1);
                    fclose(fp);
                    char cmd[512];
                    snprintf(cmd, sizeof(cmd), "gzip -c %s > %s && rm -f %s", tmp, path, tmp);
                    system(cmd);
                    chmod(path, 0644);
                }
            } else {
                /* Binary audio/raw bytes */
                FILE *fb = fopen(path, "wb");
                if (fb) {
                    unsigned char dummy[] = {0xFF, 0xFB, 0x90, 0x44, 0x00, 0x12, 0x34, 0x56};
                    fwrite(dummy, 1, sizeof(dummy), fb);
                    for (int b = 0; b < 64; b++) fputc((b * 37) % 256, fb);
                    fprintf(fb, "RAW_STREAM_FLAG{l07_decoy_suspect_%d}\x00\xFF", i + 1);
                    fclose(fb);
                    chmod(path, 0644);
                }
            }
        }
    }

    return 0;
}
