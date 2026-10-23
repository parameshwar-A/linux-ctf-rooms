/* Linux CTF - Level 10 Initializer (The Lone Survivor) */
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
        fprintf(fenv, "ROOM_ID=\"10\"\n");
        fprintf(fenv, "ROOM_TITLE=\"The Lone Survivor\"\n");
        fprintf(fenv, "ROOM_COMMANDS=\"sort, uniq\"\n");
        fprintf(fenv, "ROOM_MISSION=\"The file 'transaction_hashes.txt' contains 200 transaction tokens with duplicates. Exactly one token is unique without any duplicate. Find a command which helps to identify the duplicates and use that to identify the proper token.\"\n");
        fprintf(fenv, "ROOM_HINT=\"Use 'man -k <keyword>' for keyword based searches. Search the man pages for keywords like: repeated lines / report lines / sort\"\n");
        fclose(fenv);
        chmod("/etc/linux_ctf_room.env", 0644);
    }

    /* 2. Setup transaction hashes log with flag-shaped duplicates */
    const char *decoys[] = {
        "FLAG{l10_tx_hash_8f1a990b23de}",
        "FLAG{l10_tx_hash_44b2c819f901}",
        "FLAG{l10_tx_hash_91c3e8a02102}",
        "FLAG{l10_tx_hash_50a1f78d321a}",
        "FLAG{l10_tx_hash_b819e02f7c33}",
        "FLAG{l10_tx_hash_33a9d104e9f2}",
        "FLAG{l10_tx_hash_77e1a29d4411}",
        "FLAG{l10_tx_hash_1209b8cf1904}",
        "FLAG{l10_tx_hash_f19920a38b71}",
        "FLAG{l10_tx_hash_aa81920f3e44}"
    };

    FILE *fdec = fopen("/run/ctf/decoys", "w");
    if (fdec) {
        for (int d = 0; d < 10; d++) {
            fprintf(fdec, "%s\n", decoys[d]);
        }
        fclose(fdec);
    }

    FILE *fl = fopen("/home/learner/transaction_hashes.txt", "w");
    if (fl) {
        for (int i = 0; i < 200; i++) {
            if (i == 137) {
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
