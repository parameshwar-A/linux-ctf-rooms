/* Linux CTF - Level 05 Initializer (Excavation) */
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
        fprintf(fenv, "ROOM_ID=\"05\"\n");
        fprintf(fenv, "ROOM_TITLE=\"Excavation\"\n");
        fprintf(fenv, "ROOM_COMMANDS=\"mkdir, rm, rmdir\"\n");
        fprintf(fenv, "ROOM_MISSION=\"Excavate 'sandbox/clutter', find 'secret_key.txt', create directory 'sandbox/secure_vault', and move the key into it. Then run './verify_vault'!\"\n");
        fprintf(fenv, "ROOM_HINT=\"Use 'mkdir sandbox/secure_vault', then move the secret key into it. Read the key with 'cat'.\"\n");
        fclose(fenv);
        chmod("/etc/linux_ctf_room.env", 0644);
    }

    /* 2. Compile hardened submit-flag binary */
    char compile_cmd[1024];
    snprintf(compile_cmd, sizeof(compile_cmd),
        "gcc -O2 -s -DEXPECTED_HASH='\"%s\"' -DSALT='\"%s\"' -DROOM_NAME='\"Level 05 - Excavation\"' "
        "/opt/linux_ctf/submit_flag.c -o /usr/local/bin/submit-flag",
        hash, salt);
    if (system(compile_cmd) != 0) {
        fprintf(stderr, "[-] Failed to compile validator\n");
    }
    chmod("/usr/local/bin/submit-flag", 0755);

    /* 3. Setup puzzle layout */
    mkdir("/home/learner/sandbox", 0755);
    mkdir("/home/learner/sandbox/clutter", 0755);
    mkdir("/home/learner/sandbox/clutter/empty_trash", 0755);
    mkdir("/home/learner/sandbox/clutter/old_logs", 0755);

    FILE *fd1 = fopen("/home/learner/sandbox/clutter/crash.dump", "w");
    if (fd1) { fprintf(fd1, "MEMORY_DUMP: 0xDEADBEEF 0xCAFEBABE\n"); fclose(fd1); }
    FILE *fd2 = fopen("/home/learner/sandbox/clutter/corrupt.tmp", "w");
    if (fd2) { fprintf(fd2, "GARBAGE DATA\n"); fclose(fd2); }

    /* Plant secret key holding the flag */
    FILE *fk = fopen("/home/learner/sandbox/clutter/secret_key.txt", "w");
    if (fk) {
        fprintf(fk, "VAULT KEY ENCRYPTED PAYLOAD:\n%s\n", flag);
        fclose(fk);
        chmod("/home/learner/sandbox/clutter/secret_key.txt", 0644);
    }

    /* Compile verification binary */
    FILE *fvc = fopen("/tmp/verify_vault.c", "w");
    if (fvc) {
        fprintf(fvc,
            "#include <stdio.h>\n"
            "#include <unistd.h>\n"
            "int main() {\n"
            "    if (access(\"sandbox/secure_vault/secret_key.txt\", F_OK) == 0) {\n"
            "        printf(\"[+] [SUCCESS] Vault successfully created and key secured!\\n\");\n"
            "        printf(\"[+] FLAG: %s\\n\");\n"
            "        printf(\"[*] Run 'submit-flag \\\"%s\\\"' to complete Room 05!\\n\");\n"
            "        return 0;\n"
            "    } else {\n"
            "        printf(\"[-] 'sandbox/secure_vault/secret_key.txt' not found!\\n\");\n"
            "        printf(\"    Create 'sandbox/secure_vault' and move the key there.\\n\");\n"
            "        return 1;\n"
            "    }\n"
            "}\n", flag, flag);
        fclose(fvc);
        system("gcc -O2 -s /tmp/verify_vault.c -o /home/learner/verify_vault && rm -f /tmp/verify_vault.c");
        chmod("/home/learner/verify_vault", 0755);
    }

    system("chown -R learner:learner /home/learner");
    return 0;
}
