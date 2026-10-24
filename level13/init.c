/* Linux CTF - Level 13 Initializer (Needle in the Haystack) */
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
        fprintf(fenv, "ROOM_ID=\"13\"\n");
        fprintf(fenv, "ROOM_TITLE=\"Needle in the Haystack\"\n");
        fprintf(fenv, "ROOM_COMMANDS=\"find\"\n");
        fprintf(fenv, "ROOM_MISSION=\"The 'storage/' directory contains deeply nested folders and decoy files. Find the file named 'access.key' that has a file size greater than 50 bytes using 'find' to discover the flag!\"\n");
        fprintf(fenv, "ROOM_HINT=\"Read more and indepth features of find using man find command.\"\n");
        fclose(fenv);
        chmod("/etc/linux_ctf_room.env", 0644);
    }

    /* 2. Setup nested directories */
    mkdir("/home/learner/storage", 0755);
    mkdir("/home/learner/storage/vol1", 0755);
    mkdir("/home/learner/storage/vol1/dept_a", 0755);
    mkdir("/home/learner/storage/vol1/dept_a/node1", 0755);
    mkdir("/home/learner/storage/vol1/dept_b", 0755);
    mkdir("/home/learner/storage/vol1/dept_b/node2", 0755);
    mkdir("/home/learner/storage/vol2", 0755);
    mkdir("/home/learner/storage/vol2/secure", 0755);
    mkdir("/home/learner/storage/vol2/secure/keys", 0755);
    mkdir("/home/learner/storage/vol3", 0755);
    mkdir("/home/learner/storage/vol3/archive", 0755);
    mkdir("/home/learner/storage/vol3/archive/raw", 0755);

    /* Candidate paths for access.key */
    const char *paths[] = {
        "/home/learner/storage/vol1/dept_a/node1/access.key",
        "/home/learner/storage/vol1/dept_b/node2/access.key",
        "/home/learner/storage/vol2/secure/keys/access.key",
        "/home/learner/storage/vol3/archive/raw/access.key"
    };

    int target_idx = ctf_rand(4);

    /* Generate 32-hex random decoy tokens */
    char decoy_tokens[4][33];
    for (int i = 0; i < 4; i++) {
        ctf_rand_hex(decoy_tokens[i], 16);
    }

    /* Record decoys in /run/ctf/decoys */
    FILE *fdec = fopen("/run/ctf/decoys", "w");
    if (fdec) {
        for (int i = 0; i < 4; i++) {
            if (i != target_idx) {
                fprintf(fdec, "FLAG{l13_%s}\n", decoy_tokens[i]);
            }
        }
        fclose(fdec);
    }

    for (int i = 0; i < 4; i++) {
        FILE *fk = fopen(paths[i], "w");
        if (fk) {
            if (i == target_idx) {
                /* Genuine key: size strictly greater than 50 bytes holding the authentic flag */
                fprintf(fk, "=====================================================\n");
                fprintf(fk, "  AUTHENTICATED STORAGE MASTER ACCESS KEY\n");
                fprintf(fk, "=====================================================\n");
                fprintf(fk, "AUTHORIZED CREDENTIAL: %s\n", flag);
            } else {
                /* Small decoy key: size strictly under 50 bytes (43 bytes) */
                fprintf(fk, "FLAG{l13_%s}\n", decoy_tokens[i]);
            }
            fclose(fk);
            chmod(paths[i], 0644);
        }
    }

    return 0;
}
