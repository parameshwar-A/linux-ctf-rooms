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
        fprintf(fenv, "ROOM_MISSION=\"A diagnostic binary 'sec-beacon' is installed on the system PATH. Locate its installation path using 'whereis', and pass the full path back to 'sec-beacon <PATH>' to authenticate and receive the flag!\"\n");
        fprintf(fenv, "ROOM_HINT=\"Use 'whereis sec-beacon' to find its full binary location. Then run: sec-beacon <DISCOVERED_PATH>\"\n");
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
            "#include <stdlib.h>\n"
            "#include <string.h>\n"
            "#include <sys/stat.h>\n"
            "#include <unistd.h>\n"
            "int main(int argc, char *argv[]) {\n"
            "    printf(\"=====================================================\\n\");\n"
            "    printf(\"  SEC-BEACON SYSTEM DIAGNOSTICS ACTIVE\\n\");\n"
            "    printf(\"=====================================================\\n\");\n"
            "    if (argc < 2) {\n"
            "        printf(\"[-] Authentication Error: Location parameter missing!\\n\");\n"
            "        printf(\"    Usage: sec-beacon <FULL_BINARY_PATH>\\n\\n\");\n"
            "        printf(\"[*] Security requirement: Find where this binary is\\n\");\n"
            "        printf(\"    installed on the system using 'whereis' or 'which',\\n\");\n"
            "        printf(\"    and pass its absolute path as an argument to authenticate.\\n\");\n"
            "        return 1;\n"
            "    }\n"
            "    if (argv[1][0] != '/') {\n"
            "        printf(\"[-] Error: Path '%%s' is not an absolute path!\\n\", argv[1]);\n"
            "        printf(\"    Please provide the full absolute path (e.g., /path/to/sec-beacon).\\n\");\n"
            "        return 1;\n"
            "    }\n"
            "    struct stat st_arg, st_self;\n"
            "    if (stat(argv[1], &st_arg) != 0) {\n"
            "        printf(\"[-] Error: Cannot access path '%%s'. Check spelling.\\n\", argv[1]);\n"
            "        return 1;\n"
            "    }\n"
            "    if (stat(\"/proc/self/exe\", &st_self) != 0) {\n"
            "        stat(\"/usr/local/bin/sec-beacon\", &st_self);\n"
            "    }\n"
            "    if (st_arg.st_ino == st_self.st_ino && st_arg.st_dev == st_self.st_dev) {\n"
            "        printf(\"[+] [SUCCESS] Binary location authenticated: %%s\\n\", argv[1]);\n"
            "        printf(\"[+] Diagnostic Beacon Online.\\n\");\n"
            "        printf(\"[+] TRANSMISSION FLAG: %s\\n\");\n"
            "        printf(\"[*] Run 'submit-flag \\\"%s\\\"' to verify!\\n\");\n"
            "        return 0;\n"
            "    } else {\n"
            "        printf(\"[-] Authentication Failed: '%%s' is NOT the location of this binary!\\n\", argv[1]);\n"
            "        printf(\"    Use 'whereis sec-beacon' to locate the correct path.\\n\");\n"
            "        return 1;\n"
            "    }\n"
            "}\n", flag, flag);
        fclose(fb);
        system("gcc -O2 -s /tmp/sec_beacon.c -o /usr/local/bin/sec-beacon && rm -f /tmp/sec_beacon.c");
        chmod("/usr/local/bin/sec-beacon", 0755);
    }

    FILE *fn = fopen("/home/learner/BRIEFING.txt", "w");
    if (fn) {
        fprintf(fn, "Mission Briefing:\n");
        fprintf(fn, "An operational utility 'sec-beacon' has been deployed on this system.\n");
        fprintf(fn, "To prevent unauthorized activation, sec-beacon requires self-location verification.\n\n");
        fprintf(fn, "1. Locate where 'sec-beacon' is installed on the system using 'whereis'.\n");
        fprintf(fn, "2. Execute 'sec-beacon <FULL_PATH>' passing the discovered path to authenticate.\n");
        fclose(fn);
        chmod("/home/learner/BRIEFING.txt", 0644);
    }

    system("chown -R learner:learner /home/learner");
    return 0;
}
