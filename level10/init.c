/* Linux CTF - Level 10 Initializer (The Lone Survivor) */
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
        fprintf(fenv, "ROOM_ID=\"10\"\n");
        fprintf(fenv, "ROOM_TITLE=\"The Lone Survivor\"\n");
        fprintf(fenv, "ROOM_COMMANDS=\"sort, uniq\"\n");
        fprintf(fenv, "ROOM_MISSION=\"The file 'transaction_hashes.txt' contains 200 transaction tokens with duplicates. Exactly one token is unique without any duplicate. Find a command which helps to identify the duplicates and use that to identify the proper token.\"\n");
        fprintf(fenv, "ROOM_HINT=\"Use 'man -k <keyword>' for keyword based searches. Search the man pages for keywords like: repeated lines / report lines / sort\"\n");
        fclose(fenv);
        chmod("/etc/linux_ctf_room.env", 0644);
    }

    /* 2. Setup transaction hashes log with 32-hex flag duplicates */
    char decoys[10][64];
    for (int d = 0; d < 10; d++) {
        char h[33];
        ctf_rand_hex(h, 16);
        snprintf(decoys[d], sizeof(decoys[d]), "FLAG{l10_%s}", h);
    }

    FILE *fdec = fopen("/run/ctf/decoys", "w");
    if (fdec) {
        for (int d = 0; d < 10; d++) {
            fprintf(fdec, "%s\n", decoys[d]);
        }
        fclose(fdec);
    }

    int unique_pos = ctf_rand(200);

    FILE *fl = fopen("/home/learner/transaction_hashes.txt", "w");
    if (fl) {
        for (int i = 0; i < 200; i++) {
            if (i == unique_pos) {
                fprintf(fl, "%s\n", flag);
            } else {
                fprintf(fl, "%s\n", decoys[i % 10]);
            }
        }
        fclose(fl);
        chmod("/home/learner/transaction_hashes.txt", 0644);
    }

    return 0;
}
