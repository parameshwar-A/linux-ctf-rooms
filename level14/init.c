/* Linux CTF - Level 14 Initializer (The Vault) */
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
        fprintf(fenv, "ROOM_ID=\"14\"\n");
        fprintf(fenv, "ROOM_TITLE=\"The Vault\"\n");
        fprintf(fenv, "ROOM_COMMANDS=\"tar, gzip\"\n");
        fprintf(fenv, "ROOM_MISSION=\"Extract 'classified_vault.tar.gz' using 'tar -xzf' to uncover the flag inside!\"\n");
        fprintf(fenv, "ROOM_HINT=\"Run 'tar -xzf classified_vault.tar.gz' then read 'vault_data/credentials.txt'.\"\n");
        fclose(fenv);
        chmod("/etc/linux_ctf_room.env", 0644);
    }

    /* 2. Compile hardened submit-flag binary */
    char compile_cmd[1024];
    snprintf(compile_cmd, sizeof(compile_cmd),
        "gcc -O2 -s -DEXPECTED_HASH='\"%s\"' -DSALT='\"%s\"' -DROOM_NAME='\"Level 14 - The Vault\"' "
        "/opt/linux_ctf/submit_flag.c -o /usr/local/bin/submit-flag",
        hash, salt);
    if (system(compile_cmd) != 0) {
        fprintf(stderr, "[-] Failed to compile validator\n");
    }
    chmod("/usr/local/bin/submit-flag", 0755);

    /* 3. Setup archive */
    system("mkdir -p /tmp/vault_pack/vault_data");
    FILE *fc = fopen("/tmp/vault_pack/vault_data/credentials.txt", "w");
    if (fc) {
        fprintf(fc, "=====================================================\n");
        fprintf(fc, "  VAULT PAYLOAD RECOVERY COMPLETE\n");
        fprintf(fc, "=====================================================\n");
        fprintf(fc, "RECOVERED FLAG: %s\n", flag);
        fclose(fc);
    }
    system("tar -czf /home/learner/classified_vault.tar.gz -C /tmp/vault_pack vault_data");
    system("rm -rf /tmp/vault_pack");
    chmod("/home/learner/classified_vault.tar.gz", 0644);

    system("chown -R learner:learner /home/learner");
    return 0;
}
