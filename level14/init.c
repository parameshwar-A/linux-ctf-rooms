/* Linux CTF - Level 14 Initializer (The Vault) */
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
        fprintf(fenv, "ROOM_ID=\"14\"\n");
        fprintf(fenv, "ROOM_TITLE=\"The Vault\"\n");
        fprintf(fenv, "ROOM_COMMANDS=\"tar, gzip\"\n");
        fprintf(fenv, "ROOM_MISSION=\"A compressed archive 'classified_vault.tar.gz' has arrived. Decompress and unpack the archive using 'tar' to inspect 'vault_data/credentials.txt' and retrieve the flag!\"\n");
        fprintf(fenv, "ROOM_HINT=\"Read more about tar command using the man tar command. Just read you will figure it out!\"\n");
        fclose(fenv);
        chmod("/etc/linux_ctf_room.env", 0644);
    }

    /* 2. Setup archive */
    mkdir("/tmp/vault_pack", 0755);
    mkdir("/tmp/vault_pack/vault_data", 0755);

    FILE *fc = fopen("/tmp/vault_pack/vault_data/credentials.txt", "w");
    if (fc) {
        fprintf(fc, "=====================================================\n");
        fprintf(fc, "  VAULT PAYLOAD RECOVERY COMPLETE\n");
        fprintf(fc, "=====================================================\n");
        fprintf(fc, "RECOVERED FLAG: %s\n", flag);
        fclose(fc);
    }

    system("tar -czf /home/learner/classified_vault.tar.gz -C /tmp/vault_pack vault_data && rm -rf /tmp/vault_pack");
    chmod("/home/learner/classified_vault.tar.gz", 0644);

    return 0;
}
