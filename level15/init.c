/* Linux CTF - Level 15 Initializer (Master of the Shell) */
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <ctf_rand.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "[-] Missing flag parameter\n");
        return 1;
    }

    /* 1. Setup MOTD room environment config */
    FILE *fenv = fopen("/etc/linux_ctf_room.env", "w");
    if (fenv) {
        fprintf(fenv, "ROOM_ID=\"15\"\n");
        fprintf(fenv, "ROOM_TITLE=\"Master of the Shell\"\n");
        fprintf(fenv, "ROOM_COMMANDS=\"|, cut, sort, uniq, grep\"\n");
        fprintf(fenv, "ROOM_MISSION=\"Capstone Challenge! Analyze the telemetry file 'network_traffic.csv'. Extract the client IP addresses (column 2), filter by port '443', count occurrences with 'uniq -c', and find the IP with the highest traffic volume. Once identified, run './report_attacker <IP>' to earn the master flag!\"\n");
        fprintf(fenv, "ROOM_HINT=\"If you came this far, you know what to do! That's right it is in man pages, but this time it won't be a single command, But cocktail of few items so start hacking!\"\n");
        fclose(fenv);
        chmod("/etc/linux_ctf_room.env", 0644);
    }

    /* 2. Randomize attacker IP per session (Finding 11) */
    char attacker_ip[64];
    ctf_rand_ip(attacker_ip, sizeof(attacker_ip));

    /* Write answer to root-only /run/ctf/answer */
    FILE *fa = fopen("/run/ctf/answer", "w");
    if (fa) {
        fprintf(fa, "%s\n", attacker_ip);
        fclose(fa);
        chmod("/run/ctf/answer", 0600);
    }

    /* 3. Setup CSV data with top attacker IP (occurs 35 times on port 443) */
    FILE *fc = fopen("/home/learner/network_traffic.csv", "w");
    if (fc) {
        fprintf(fc, "TIMESTAMP,CLIENT_IP,PORT,STATUS,BYTES\n");
        char noise_ip1[64], noise_ip2[64], noise_ip3[64];
        ctf_rand_ip(noise_ip1, sizeof(noise_ip1));
        ctf_rand_ip(noise_ip2, sizeof(noise_ip2));
        ctf_rand_ip(noise_ip3, sizeof(noise_ip3));
        const char *noise_ips[] = {noise_ip1, noise_ip2, noise_ip3, "192.168.1.10", "192.168.1.15"};

        for (int i = 0; i < 100; i++) {
            if (i < 35) {
                /* Top attacker IP on 443 (35 hits) */
                fprintf(fc, "2026-10-14T10:%02d:00,%s,443,ESTABLISHED,%d\n",
                    i % 60, attacker_ip, (i * 123) % 4096);
            } else if (i < 55) {
                /* Noise IP 1 on 443 (20 hits) */
                fprintf(fc, "2026-10-14T10:%02d:00,%s,443,ESTABLISHED,%d\n",
                    i % 60, noise_ips[0], (i * 90) % 2048);
            } else if (i < 70) {
                /* Attacker IP on port 80 (not 443) */
                fprintf(fc, "2026-10-14T10:%02d:00,%s,80,ESTABLISHED,%d\n",
                    i % 60, attacker_ip, 512);
            } else {
                /* General background noise */
                fprintf(fc, "2026-10-14T10:%02d:00,%s,443,CLOSED,%d\n",
                    i % 60, noise_ips[i % 5], (i * 37) % 1024);
            }
        }
        fclose(fc);
        chmod("/home/learner/network_traffic.csv", 0644);
    }

    /* 4. Link pre-compiled verifier (zero flags embedded) */
    unlink("/home/learner/report_attacker");
    symlink("/usr/local/bin/report_attacker", "/home/learner/report_attacker");

    return 0;
}
