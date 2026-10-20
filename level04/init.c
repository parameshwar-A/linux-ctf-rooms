/* Linux CTF - Level 04 Initializer (Housekeeping) */
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
        fprintf(fenv, "ROOM_ID=\"04\"\n");
        fprintf(fenv, "ROOM_TITLE=\"Housekeeping\"\n");
        fprintf(fenv, "ROOM_COMMANDS=\"cp, mv\"\n");
        fprintf(fenv, "ROOM_MISSION=\"An archive payload was misplaced as 'staging/temp_payload.dat'. Move it to 'production/app_payload.dat', and create a duplicate backup copy at 'backups/app_payload.dat'. Run './check_production' to verify!\"\n");
        fprintf(fenv, "ROOM_HINT=\"Use 'man -k <keyword>' for keyword based searches. Search man pages for keywords like copy files / copy / move files\"\n");
        fclose(fenv);
        chmod("/etc/linux_ctf_room.env", 0644);
    }

    /* 2. Compile hardened submit-flag binary */
    char compile_cmd[1024];
    snprintf(compile_cmd, sizeof(compile_cmd),
        "gcc -O2 -s -DEXPECTED_HASH='\"%s\"' -DSALT='\"%s\"' -DROOM_NAME='\"Level 04 - Housekeeping\"' "
        "/opt/linux_ctf/submit_flag.c -o /usr/local/bin/submit-flag",
        hash, salt);
    if (system(compile_cmd) != 0) {
        fprintf(stderr, "[-] Failed to compile validator\n");
    }
    chmod("/usr/local/bin/submit-flag", 0755);

    /* 3. Setup puzzle layout */
    mkdir("/home/learner/staging", 0755);
    mkdir("/home/learner/production", 0755);
    mkdir("/home/learner/backups", 0755);

    FILE *fp = fopen("/home/learner/staging/temp_payload.dat", "w");
    if (fp) {
        fprintf(fp, "PAYLOAD_MAGIC=0x7F992B\nORIGIN=STAGING_INBOX\nDATA_INTEGRITY=VALID\n");
        fclose(fp);
        chmod("/home/learner/staging/temp_payload.dat", 0644);
    }

    FILE *fn = fopen("/home/learner/README.txt", "w");
    if (fn) {
        fprintf(fn, "A critical payload arrived in 'staging/temp_payload.dat'.\n");
        fprintf(fn, "Tasks:\n");
        fprintf(fn, "1. Move 'staging/temp_payload.dat' to 'production/app_payload.dat'\n");
        fprintf(fn, "2. Copy 'production/app_payload.dat' to 'backups/app_payload.dat'\n");
        fprintf(fn, "3. Run './check_production' to verify placement and receive the flag!\n");
        fclose(fn);
        chmod("/home/learner/README.txt", 0644);
    }

    /* Compile verification binary */
    FILE *fvc = fopen("/tmp/check_production.c", "w");
    if (fvc) {
        fprintf(fvc,
            "#include <stdio.h>\n"
            "#include <unistd.h>\n"
            "int main() {\n"
            "    if (access(\"staging/temp_payload.dat\", F_OK) == 0) {\n"
            "        printf(\"[-] 'staging/temp_payload.dat' still exists! Please move it to the production directory.\\n\");\n"
            "        return 1;\n"
            "    }\n"
            "    if (access(\"production/app_payload.dat\", F_OK) != 0) {\n"
            "        printf(\"[-] 'production/app_payload.dat' not found! Please place the file in the production directory.\\n\");\n"
            "        return 1;\n"
            "    }\n"
            "    if (access(\"backups/app_payload.dat\", F_OK) != 0) {\n"
            "        printf(\"[-] 'backups/app_payload.dat' not found! Please create a duplicate copy in backups/.\\n\");\n"
            "        return 1;\n"
            "    }\n"
            "    printf(\"[+] [SUCCESS] Deployment structure verified!\\n\");\n"
            "    printf(\"[+] FLAG: %s\\n\");\n"
            "    printf(\"[*] Run 'submit-flag \\\"%s\\\"' to complete Room 04!\\n\");\n"
            "    return 0;\n"
            "}\n", flag, flag);
        fclose(fvc);
        system("gcc -O2 -s /tmp/check_production.c -o /home/learner/check_production && rm -f /tmp/check_production.c");
        chmod("/home/learner/check_production", 0755);
    }

    system("chown -R learner:learner /home/learner");
    return 0;
}
