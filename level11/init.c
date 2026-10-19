/* Linux CTF - Level 11 Initializer (Cipher Shift) */
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

void rot13(const char *in, char *out) {
    while (*in) {
        char c = *in;
        if (c >= 'a' && c <= 'z') out[0] = 'a' + (c - 'a' + 13) % 26;
        else if (c >= 'A' && c <= 'Z') out[0] = 'A' + (c - 'A' + 13) % 26;
        else out[0] = c;
        in++; out++;
    }
    *out = '\0';
}

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
        fprintf(fenv, "ROOM_ID=\"11\"\n");
        fprintf(fenv, "ROOM_TITLE=\"Cipher Shift\"\n");
        fprintf(fenv, "ROOM_COMMANDS=\"tr\"\n");
        fprintf(fenv, "ROOM_MISSION=\"An intercepted transmission 'intercepted.enc' was scrambled using a classic ROT13 cipher. Decipher the message using 'tr' to reveal the plaintext flag!\"\n");
        fprintf(fenv, "ROOM_HINT=\"Ensure to read more about ROT13 and then try how can we use the tr command through man tr.\"\n");
        fclose(fenv);
        chmod("/etc/linux_ctf_room.env", 0644);
    }

    /* 2. Compile hardened submit-flag binary */
    char compile_cmd[1024];
    snprintf(compile_cmd, sizeof(compile_cmd),
        "gcc -O2 -s -DEXPECTED_HASH='\"%s\"' -DSALT='\"%s\"' -DROOM_NAME='\"Level 11 - Cipher Shift\"' "
        "/opt/linux_ctf/submit_flag.c -o /usr/local/bin/submit-flag",
        hash, salt);
    if (system(compile_cmd) != 0) {
        fprintf(stderr, "[-] Failed to compile validator\n");
    }
    chmod("/usr/local/bin/submit-flag", 0755);

    /* 3. Setup puzzle files */
    char enc_flag[256];
    rot13(flag, enc_flag);

    FILE *fe = fopen("/home/learner/intercepted.enc", "w");
    if (fe) {
        char msg[512];
        snprintf(msg, sizeof(msg),
            "TOP SECRET INTERCEPT:\n"
            "The authorization code has been encrypted with ROT13.\n"
            "Plaintext token: %s\n", flag);
        char enc_msg[1024];
        rot13(msg, enc_msg);
        fprintf(fe, "%s", enc_msg);
        fclose(fe);
        chmod("/home/learner/intercepted.enc", 0644);
    }

    FILE *fn = fopen("/home/learner/README.txt", "w");
    if (fn) {
        fprintf(fn, "An intercepted communication was caught in 'intercepted.enc'.\n");
        fprintf(fn, "All alphabet characters were rotated by 13 positions (ROT13).\n");
        fprintf(fn, "Use 'tr' to reverse the rotation and read the message.\n");
        fclose(fn);
        chmod("/home/learner/README.txt", 0644);
    }

    system("chown -R learner:learner /home/learner");
    return 0;
}
