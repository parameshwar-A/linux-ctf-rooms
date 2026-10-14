/* Linux CTF - Level 15 Initializer (Master of the Shell) */
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
        fprintf(fenv, "ROOM_ID=\"15\"\n");
        fprintf(fenv, "ROOM_TITLE=\"Master of the Shell\"\n");
        fprintf(fenv, "ROOM_COMMANDS=\"|, cut, sort, uniq, grep\"\n");
        fprintf(fenv, "ROOM_MISSION=\"Analyze 'network_traffic.csv', identify the highest volume IP on port 443, and run './report_attacker <IP>'!\"\n");
        fprintf(fenv, "ROOM_HINT=\"Combine pipelines: grep ',443,' network_traffic.csv | cut -d',' -f2 | sort | uniq -c | sort -nr | head -n 1\"\n");
        fclose(fenv);
        chmod("/etc/linux_ctf_room.env", 0644);
    }

    /* 2. Compile hardened submit-flag binary */
    char compile_cmd[1024];
    snprintf(compile_cmd, sizeof(compile_cmd),
        "gcc -O2 -s -DEXPECTED_HASH='\"%s\"' -DSALT='\"%s\"' -DROOM_NAME='\"Level 15 - Master of the Shell\"' "
        "/opt/linux_ctf/submit_flag.c -o /usr/local/bin/submit-flag",
        hash, salt);
    if (system(compile_cmd) != 0) {
        fprintf(stderr, "[-] Failed to compile validator\n");
    }
    chmod("/usr/local/bin/submit-flag", 0755);

    /* 3. Setup CSV data with top attacker IP: 10.0.4.77 (occurs 35 times on 443) */
    FILE *fc = fopen("/home/learner/network_traffic.csv", "w");
    if (fc) {
        fprintf(fc, "TIMESTAMP,CLIENT_IP,PORT,STATUS,BYTES\n");
        const char *ips[] = {"192.168.1.10", "192.168.1.15", "10.0.1.5", "172.16.0.44", "10.0.4.77"};
        for (int i = 0; i < 100; i++) {
            if (i < 35) {
                fprintf(fc, "2026-10-14T10:%02d:00,10.0.4.77,443,ESTABLISHED,%d\n", i % 60, (i * 123) % 4096);
            } else if (i < 55) {
                fprintf(fc, "2026-10-14T10:%02d:00,192.168.1.10,443,ESTABLISHED,%d\n", i % 60, (i * 90) % 2048);
            } else if (i < 70) {
                fprintf(fc, "2026-10-14T10:%02d:00,10.0.4.77,80,ESTABLISHED,%d\n", i % 60, 512);
            } else {
                fprintf(fc, "2026-10-14T10:%02d:00,%s,443,CLOSED,%d\n", i % 60, ips[i % 5], (i * 37) % 1024);
            }
        }
        fclose(fc);
        chmod("/home/learner/network_traffic.csv", 0644);
    }

    /* Compile verification reporting binary */
    FILE *fr = fopen("/tmp/report_attacker.c", "w");
    if (fr) {
        fprintf(fr,
            "#include <stdio.h>\n"
            "#include <string.h>\n"
            "int main(int argc, char *argv[]) {\n"
            "    if (argc < 2) {\n"
            "        printf(\"Usage: %%s <ATTACKER_IP>\\n\", argv[0]);\n"
            "        return 1;\n"
            "    }\n"
            "    if (strcmp(argv[1], \"10.0.4.77\") == 0) {\n"
            "        printf(\"=====================================================\\n\");\n"
            "        printf(\"  [+] ATTACK PATTERN ACCURATELY IDENTIFIED!\\n\");\n"
            "        printf(\"=====================================================\\n\");\n"
            "        printf(\"[+] CAPSTONE COMPLETE! MASTER FLAG: %s\\n\");\n"
            "        printf(\"[*] Run 'submit-flag \\\"%s\\\"' to claim victory!\\n\");\n"
            "        return 0;\n"
            "    } else {\n"
            "        printf(\"[-] Incorrect IP address: '%%s'. Check your pipeline!\\n\", argv[1]);\n"
            "        return 1;\n"
            "    }\n"
            "}\n", flag, flag);
        fclose(fr);
        system("gcc -O2 -s /tmp/report_attacker.c -o /home/learner/report_attacker && rm -f /tmp/report_attacker.c");
        chmod("/home/learner/report_attacker", 0755);
    }

    system("chown -R learner:learner /home/learner");
    return 0;
}
