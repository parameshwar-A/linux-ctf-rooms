/* Linux CTF - Level 10 Initializer (The Lone Survivor) */
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
        fprintf(fenv, "ROOM_ID=\"10\"\n");
        fprintf(fenv, "ROOM_TITLE=\"The Lone Survivor\"\n");
        fprintf(fenv, "ROOM_COMMANDS=\"sort, uniq\"\n");
        fprintf(fenv, "ROOM_MISSION=\"The file 'transaction_hashes.txt' contains 200 transaction tokens with duplicates. Exactly one token is unique without any duplicate. Find a command which helps to identify the duplicates and use that to identify the proper token.\"\n");
        fprintf(fenv, "ROOM_HINT=\"Use 'man -k <keyword>' for keyword based searches. Search the man pages for keywords like: repeated lines / report lines / sort\"\n");
        fclose(fenv);
        chmod("/etc/linux_ctf_room.env", 0644);
    }

    /* 2. Compile hardened submit-flag binary */
    char compile_cmd[1024];
    snprintf(compile_cmd, sizeof(compile_cmd),
        "gcc -O2 -s -DEXPECTED_HASH='\"%s\"' -DSALT='\"%s\"' -DROOM_NAME='\"Level 10 - The Lone Survivor\"' "
        "/opt/linux_ctf/submit_flag.c -o /usr/local/bin/submit-flag",
        hash, salt);
    if (system(compile_cmd) != 0) {
        fprintf(stderr, "[-] Failed to compile validator\n");
    }
    chmod("/usr/local/bin/submit-flag", 0755);

    /* 3. Setup transaction hashes log */
    FILE *fl = fopen("/home/learner/transaction_hashes.txt", "w");
    if (fl) {
        const char *decoys[] = {
            "TX_HASH_8F1A990B23DE",
            "TX_HASH_44B2C819F901",
            "TX_HASH_91C3E8A02102",
            "TX_HASH_50A1F78D321A",
            "TX_HASH_B819E02F7C33",
            "TX_HASH_33A9D104E9F2",
            "TX_HASH_77E1A29D4411",
            "TX_HASH_1209B8CF1904",
            "TX_HASH_F19920A38B71",
            "TX_HASH_AA81920F3E44"
        };
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

    system("chown -R learner:learner /home/learner");
    return 0;
}
