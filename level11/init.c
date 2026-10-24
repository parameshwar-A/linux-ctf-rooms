/* Linux CTF - Level 11 Initializer (Cipher Shift) */
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <ctf_rand.h>

static void rot13(const char *in, char *out) {
    while (*in) {
        char c = *in;
        if (c >= 'a' && c <= 'z') out[0] = 'a' + ((c - 'a' + 13) % 26);
        else if (c >= 'A' && c <= 'Z') out[0] = 'A' + ((c - 'A' + 13) % 26);
        else out[0] = c;
        in++; out++;
    }
    *out = '\0';
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "[-] Missing flag parameter\n");
        return 1;
    }
    const char *flag = argv[1];

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

    /* Generate 32-hex decoy */
    char d_tok[33];
    ctf_rand_hex(d_tok, 16);
    char decoy[128];
    snprintf(decoy, sizeof(decoy), "FLAG{l11_%s}", d_tok);

    /* Record decoy */
    FILE *fdec = fopen("/run/ctf/decoys", "w");
    if (fdec) {
        fprintf(fdec, "%s\n", decoy);
        fclose(fdec);
    }

    /* 2. Setup intercepted transmission */
    char full_plain[2048];
    snprintf(full_plain, sizeof(full_plain),
        "TRANSMISSION INTERCEPT CH-1:\n"
        "STATUS: NOISE_BURST\n"
        "MESSAGE: %s\n\n"
        "TRANSMISSION INTERCEPT CH-2:\n"
        "AUTHENTICATED PAYLOAD RECOVERY:\n"
        "The authorization code has been encrypted with ROT13.\n"
        "Plaintext token: %s\n", decoy, flag);

    char full_enc[4096];
    rot13(full_plain, full_enc);

    FILE *fe = fopen("/home/learner/intercepted.enc", "w");
    if (fe) {
        fprintf(fe, "%s", full_enc);
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

    return 0;
}
