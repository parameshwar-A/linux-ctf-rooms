/* Linux CTF - Level 13 Initializer (Needle in the Haystack) */
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
        fprintf(fenv, "ROOM_ID=\"13\"\n");
        fprintf(fenv, "ROOM_TITLE=\"Needle in the Haystack\"\n");
        fprintf(fenv, "ROOM_COMMANDS=\"find\"\n");
        fprintf(fenv, "ROOM_MISSION=\"Find 'access.key' with size greater than 50 bytes in 'storage/' using 'find'!\"\n");
        fprintf(fenv, "ROOM_HINT=\"Run 'find storage/ -name \"access.key\" -size +50c' then inspect the file.\"\n");
        fclose(fenv);
        chmod("/etc/linux_ctf_room.env", 0644);
    }

    /* 2. Compile hardened submit-flag binary */
    char compile_cmd[1024];
    snprintf(compile_cmd, sizeof(compile_cmd),
        "gcc -O2 -s -DEXPECTED_HASH='\"%s\"' -DSALT='\"%s\"' -DROOM_NAME='\"Level 13 - Needle in the Haystack\"' "
        "/opt/linux_ctf/submit_flag.c -o /usr/local/bin/submit-flag",
        hash, salt);
    if (system(compile_cmd) != 0) {
        fprintf(stderr, "[-] Failed to compile validator\n");
    }
    chmod("/usr/local/bin/submit-flag", 0755);

    /* 3. Setup nested directories */
    system("mkdir -p /home/learner/storage/vol1/dept_a/node1");
    system("mkdir -p /home/learner/storage/vol1/dept_b/node2");
    system("mkdir -p /home/learner/storage/vol2/secure/keys");
    system("mkdir -p /home/learner/storage/vol3/archive/raw");

    /* Empty decoy keys (0 bytes or 10 bytes) */
    system("touch /home/learner/storage/vol1/dept_a/node1/access.key");
    system("echo 'EMPTY' > /home/learner/storage/vol1/dept_b/node2/access.key");
    system("echo 'DUMMY KEY' > /home/learner/storage/vol3/archive/raw/access.key");

    /* True key with size > 50 bytes holding the flag */
    FILE *fk = fopen("/home/learner/storage/vol2/secure/keys/access.key", "w");
    if (fk) {
        fprintf(fk, "=====================================================\n");
        fprintf(fk, "  AUTHENTICATED STORAGE MASTER ACCESS KEY\n");
        fprintf(fk, "=====================================================\n");
        fprintf(fk, "AUTHORIZED CREDENTIAL: %s\n", flag);
        fclose(fk);
        chmod("/home/learner/storage/vol2/secure/keys/access.key", 0644);
    }

    system("chown -R learner:learner /home/learner");
    return 0;
}
