/* Linux CTF - Level 03 Initializer (Ink and Paper) */
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <ctf_rand.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "[-] Missing session flag parameter\n");
        return 1;
    }

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

    /* 2. Randomize configuration keys per session */
    const char *modes[] = {"ACTIVE", "STANDBY", "DEPLOY", "ONLINE", "DISPATCH"};
    const char *targets[] = {"PRODUCTION", "CLUSTER_A", "DATACENTER", "PRIMARY", "SECURE_ZONE"};
    int m_idx = ctf_rand(5);
    int t_idx = ctf_rand(5);
    char line1[128], line2[128];
    snprintf(line1, sizeof(line1), "MODE=%s", modes[m_idx]);
    snprintf(line2, sizeof(line2), "TARGET=%s", targets[t_idx]);

    /* Write expected answer to root-only /run/ctf/answer */
    FILE *fa = fopen("/run/ctf/answer", "w");
    if (fa) {
        fprintf(fa, "%s\n%s\n", line1, line2);
        fclose(fa);
        chmod("/run/ctf/answer", 0600);
    }

    /* 3. Setup puzzle layout */
    FILE *fn = fopen("/home/learner/INSTRUCTIONS.txt", "w");
    if (fn) {
        fprintf(fn, "=========================================================\n");
        fprintf(fn, "  Mission: Configure the Dispatch Daemon\n");
        fprintf(fn, "=========================================================\n");
        fprintf(fn, "The dispatch daemon requires 'dispatch.conf' with exactly:\n\n");
        fprintf(fn, "Line 1: %s\n", line1);
        fprintf(fn, "Line 2: %s\n\n", line2);
        fprintf(fn, "Write the required configuration keys into 'dispatch.conf'.\n");
        fprintf(fn, "Ensure both lines are present and exact.\n");
        fprintf(fn, "Once ready, execute: ./verify_dispatch\n");
        fclose(fn);
        chmod("/home/learner/INSTRUCTIONS.txt", 0644);
    }

    /* 4. Link pre-compiled thin-client verifier (zero flags embedded) */
    unlink("/home/learner/verify_dispatch");
    symlink("/usr/local/bin/verify_dispatch", "/home/learner/verify_dispatch");

    return 0;
}
