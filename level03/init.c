/* Linux CTF - Level 03 Initializer (Ink and Paper) */
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
        fprintf(fenv, "ROOM_ID=\"03\"\n");
        fprintf(fenv, "ROOM_TITLE=\"Ink and Paper\"\n");
        fprintf(fenv, "ROOM_COMMANDS=\"echo, >, >>\"\n");
        fprintf(fenv, "ROOM_MISSION=\"The server launch daemon is awaiting configuration in 'dispatch.conf'. Use a command to write the required configuration keys. Check the instructions file for the configuration keys. Once edited the file, execute './verify_dispatch' to reveal your flag!\"\n");
        fprintf(fenv, "ROOM_HINT=\"Check the man page of bash, read it and find if there is anything related to writing output, redirection of data\"\n");
        fclose(fenv);
        chmod("/etc/linux_ctf_room.env", 0644);
    }

    /* 2. Compile hardened submit-flag binary (~70ms) */
    char compile_cmd[1024];
    snprintf(compile_cmd, sizeof(compile_cmd),
        "gcc -O2 -s -DEXPECTED_HASH='\"%s\"' -DSALT='\"%s\"' -DROOM_NAME='\"Level 03 - Ink and Paper\"' "
        "/opt/linux_ctf/submit_flag.c -o /usr/local/bin/submit-flag",
        hash, salt);
    if (system(compile_cmd) != 0) {
        fprintf(stderr, "[-] Failed to compile validator\n");
    }
    chmod("/usr/local/bin/submit-flag", 0755);

    /* 3. Setup puzzle layout */
    FILE *fn = fopen("/home/learner/INSTRUCTIONS.txt", "w");
    if (fn) {
        fprintf(fn, "=========================================================\n");
        fprintf(fn, "  Mission: Configure the Dispatch Daemon\n");
        fprintf(fn, "=========================================================\n");
        fprintf(fn, "The dispatch daemon requires 'dispatch.conf' with exactly:\n\n");
        fprintf(fn, "Line 1: MODE=ACTIVE\n");
        fprintf(fn, "Line 2: TARGET=PRODUCTION\n\n");
        fprintf(fn, "Use 'echo' with '>' (overwrite) and '>>' (append).\n");
        fprintf(fn, "Once ready, execute: ./verify_dispatch\n");
        fclose(fn);
        chmod("/home/learner/INSTRUCTIONS.txt", 0644);
    }

    /* Compile verification binary */
    FILE *fvc = fopen("/tmp/verify_dispatch.c", "w");
    if (fvc) {
        fprintf(fvc,
            "#include <stdio.h>\n"
            "#include <string.h>\n"
            "int main() {\n"
            "    FILE *f = fopen(\"dispatch.conf\", \"r\");\n"
            "    if (!f) {\n"
            "        printf(\"[-] 'dispatch.conf' not found! Create it using echo and redirection.\\n\");\n"
            "        return 1;\n"
            "    }\n"
            "    char l1[128] = {0}, l2[128] = {0};\n"
            "    if (!fgets(l1, sizeof(l1), f) || !fgets(l2, sizeof(l2), f)) {\n"
            "        printf(\"[-] 'dispatch.conf' must contain at least 2 lines.\\n\");\n"
            "        fclose(f);\n"
            "        return 1;\n"
            "    }\n"
            "    fclose(f);\n"
            "    l1[strcspn(l1, \"\\r\\n\")] = 0;\n"
            "    l2[strcspn(l2, \"\\r\\n\")] = 0;\n"
            "    if (strcmp(l1, \"MODE=ACTIVE\") == 0 && strcmp(l2, \"TARGET=PRODUCTION\") == 0) {\n"
            "        printf(\"[+] [SUCCESS] Dispatch configuration verified!\\n\");\n"
            "        printf(\"[+] FLAG: %s\\n\");\n"
            "        printf(\"[*] Run 'submit-flag \\\"%s\\\"' to record victory!\\n\");\n"
            "        return 0;\n"
            "    } else {\n"
            "        printf(\"[-] Verification failed!\\n\");\n"
            "        printf(\"    Line 1 got: '%%s' (expected 'MODE=ACTIVE')\\n\", l1);\n"
            "        printf(\"    Line 2 got: '%%s' (expected 'TARGET=PRODUCTION')\\n\", l2);\n"
            "        return 1;\n"
            "    }\n"
            "}\n", flag, flag);
        fclose(fvc);
        system("gcc -O2 -s /tmp/verify_dispatch.c -o /home/learner/verify_dispatch && rm -f /tmp/verify_dispatch.c");
        chmod("/home/learner/verify_dispatch", 0755);
    }

    system("chown -R learner:learner /home/learner");
    return 0;
}
