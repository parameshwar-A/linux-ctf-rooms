/* Linux CTF - Level 06 Initializer (Compass and Map) */
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
        fprintf(fenv, "ROOM_ID=\"06\"\n");
        fprintf(fenv, "ROOM_TITLE=\"Compass and Map\"\n");
        fprintf(fenv, "ROOM_COMMANDS=\"which, whereis\"\n");
        fprintf(fenv, "ROOM_MISSION=\"A custom diagnostic binary 'sec-beacon' is installed on the system PATH. Your task is to find the location of the file. Normal files we can use find command, but for the installed files or executables path there is another commadn find that from man pages\"\n");
        fprintf(fenv, "ROOM_HINT=\"Use manpage -k <keyword> command for keyword based searches, Search the man pages for keywords: full path / executable path\"\n");
        fclose(fenv);
        chmod("/etc/linux_ctf_room.env", 0644);
    }

    /* 2. Compile hardened submit-flag binary */
    char compile_cmd[1024];
    snprintf(compile_cmd, sizeof(compile_cmd),
        "gcc -O2 -s -DEXPECTED_HASH='\"%s\"' -DSALT='\"%s\"' -DROOM_NAME='\"Level 06 - Compass and Map\"' "
        "/opt/linux_ctf/submit_flag.c -o /usr/local/bin/submit-flag",
        hash, salt);
    if (system(compile_cmd) != 0) {
        fprintf(stderr, "[-] Failed to compile validator\n");
    }
    chmod("/usr/local/bin/submit-flag", 0755);

    /* 3. Setup puzzle binary on system PATH */
    FILE *fb = fopen("/tmp/sec_beacon.c", "w");
    if (fb) {
        fprintf(fb,
            "#include <stdio.h>\n"
            "int main() {\n"
            "    printf(\"=====================================================\\n\");\n"
            "    printf(\"  SEC-BEACON SYSTEM DIAGNOSTICS ACTIVE\\n\");\n"
            "    printf(\"=====================================================\\n\");\n"
            "    printf(\"[+] Diagnostic Beacon Online.\\n\");\n"
            "    printf(\"[+] TRANSMISSION FLAG: %s\\n\");\n"
            "    printf(\"[*] Run 'submit-flag \\\"%s\\\"' to verify!\\n\");\n"
            "    return 0;\n"
            "}\n", flag, flag);
        fclose(fb);
        system("gcc -O2 -s /tmp/sec_beacon.c -o /usr/local/bin/sec-beacon && rm -f /tmp/sec_beacon.c");
        chmod("/usr/local/bin/sec-beacon", 0755);
    }

    FILE *fn = fopen("/home/learner/BRIEFING.txt", "w");
    if (fn) {
        fprintf(fn, "Mission: An operational utility 'sec-beacon' was deployed on this system.\n");
        fprintf(fn, "Find where it is located using 'which' or 'whereis', and run it.\n");
        fclose(fn);
        chmod("/home/learner/BRIEFING.txt", 0644);
    }

    system("chown -R learner:learner /home/learner");
    return 0;
}
