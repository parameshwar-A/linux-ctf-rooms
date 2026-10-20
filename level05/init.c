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
        fprintf(fenv, "ROOM_MISSION=\"The storage area 'sandbox/clutter' is full of junk. Locate 'sandbox/clutter/secret_*.txt' amidst the clutter, create the directory 'sandbox/secure_vault', and move the key into it. Run './verify_vault' to decrypt the vault and reveal the flag!\"\n");
        fprintf(fenv, "ROOM_HINT=\"Use 'man -k <keyword>' for keyword based searches. Search man pages for keywords like directories / make / remove\"\n");
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
    mkdir("/home/learner/sandbox/clutter/cache_dump", 0755);

    /* Generate 50 dummy clutter files to simulate realistic directory debris */
    char dummy_path[256];
    for (int i = 1; i <= 15; i++) {
        snprintf(dummy_path, sizeof(dummy_path), "/home/learner/sandbox/clutter/sys_log_%02d.log", i);
        FILE *f = fopen(dummy_path, "w");
        if (f) { fprintf(f, "LOG_EVENT_%02d: Diagnostic event recorded.\n", i); fclose(f); }
    }
    for (int i = 1; i <= 15; i++) {
        snprintf(dummy_path, sizeof(dummy_path), "/home/learner/sandbox/clutter/temp_cache_%02d.tmp", i);
        FILE *f = fopen(dummy_path, "w");
        if (f) { fprintf(f, "CACHE_BLOCK_%02d: 0x%08X\n", i, i * 4096); fclose(f); }
    }
    for (int i = 1; i <= 10; i++) {
        snprintf(dummy_path, sizeof(dummy_path), "/home/learner/sandbox/clutter/dump_%02d.bak", i);
        FILE *f = fopen(dummy_path, "w");
        if (f) { fprintf(f, "CRASH_DUMP_%02d: MEMORY_OFFSET_OK\n", i); fclose(f); }
    }
    for (int i = 1; i <= 10; i++) {
        snprintf(dummy_path, sizeof(dummy_path), "/home/learner/sandbox/clutter/trash_archive_%02d.old", i);
        FILE *f = fopen(dummy_path, "w");
        if (f) { fprintf(f, "TRASH_RECORD_%02d: ARCHIVED_DEBRIS\n", i); fclose(f); }
    }

    /* Plant hardened secret key (contains encrypted token, NOT plaintext flag) */
    char token[128];
    snprintf(token, sizeof(token), "VAULT-TOKEN-%s", salt);

    FILE *fk = fopen("/home/learner/sandbox/clutter/secret_key.txt", "w");
    if (fk) {
        fprintf(fk, "=========================================================\n");
        fprintf(fk, "  ENCRYPTED VAULT KEY CONTAINER\n");
        fprintf(fk, "=========================================================\n");
        fprintf(fk, "KEY_ID: %s\n", token);
        fprintf(fk, "STATUS: LOCKED_ENCRYPTED\n\n");
        fprintf(fk, "NOTICE: This encrypted key requires vault verification to decrypt.\n");
        fprintf(fk, "Move this key into 'sandbox/secure_vault/' and run './verify_vault'.\n");
        fclose(fk);
        chmod("/home/learner/sandbox/clutter/secret_key.txt", 0644);
    }

    /* Compile hardened verification binary */
    FILE *fvc = fopen("/tmp/verify_vault.c", "w");
    if (fvc) {
        fprintf(fvc,
            "#include <stdio.h>\n"
            "#include <stdlib.h>\n"
            "#include <string.h>\n"
            "#include <unistd.h>\n"
            "#include <sys/stat.h>\n"
            "int main() {\n"
            "    struct stat st;\n"
            "    if (stat(\"sandbox/secure_vault\", &st) != 0 || !S_ISDIR(st.st_mode)) {\n"
            "        printf(\"[-] Vault directory 'sandbox/secure_vault' not found!\\n\");\n"
            "        printf(\"    Please create the secure vault directory first.\\n\");\n"
            "        return 1;\n"
            "    }\n"
            "    if (access(\"sandbox/clutter/secret_key.txt\", F_OK) == 0) {\n"
            "        printf(\"[-] 'secret_key.txt' still exists in 'sandbox/clutter/'!\\n\");\n"
            "        printf(\"    Please move the key into the secure vault.\\n\");\n"
            "        return 1;\n"
            "    }\n"
            "    FILE *f = fopen(\"sandbox/secure_vault/secret_key.txt\", \"r\");\n"
            "    if (!f) {\n"
            "        printf(\"[-] Key file 'sandbox/secure_vault/secret_key.txt' not found!\\n\");\n"
            "        printf(\"    Locate the secret key amidst the clutter and move it into the vault.\\n\");\n"
            "        return 1;\n"
            "    }\n"
            "    char line[256];\n"
            "    int token_ok = 0;\n"
            "    while (fgets(line, sizeof(line), f)) {\n"
            "        if (strstr(line, \"KEY_ID: %s\")) {\n"
            "            token_ok = 1;\n"
            "            break;\n"
            "        }\n"
            "    }\n"
            "    fclose(f);\n"
            "    if (!token_ok) {\n"
            "        printf(\"[-] Vault key corrupted or invalid token!\\n\");\n"
            "        return 1;\n"
            "    }\n"
            "    printf(\"[+] [SUCCESS] Vault successfully created and key authenticated!\\n\");\n"
            "    printf(\"[+] Vault payload decrypted.\\n\");\n"
            "    printf(\"[+] FLAG: %s\\n\");\n"
            "    printf(\"[*] Run 'submit-flag \\\"%s\\\"' to complete Room 05!\\n\");\n"
            "    return 0;\n"
            "}\n", token, flag, flag);
        fclose(fvc);
        system("gcc -O2 -s /tmp/verify_vault.c -o /home/learner/verify_vault && rm -f /tmp/verify_vault.c");
        chmod("/home/learner/verify_vault", 0755);
    }

    system("chown -R learner:learner /home/learner");
    return 0;
}
