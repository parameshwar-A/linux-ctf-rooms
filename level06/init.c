/* Linux CTF - Level 06 Initializer (Compass and Map) */
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "[-] Missing flag parameter\n");
        return 1;
    }

    /* 1. Setup MOTD room environment config */
    FILE *fenv = fopen("/etc/linux_ctf_room.env", "w");
    if (fenv) {
        fprintf(fenv, "ROOM_ID=\"06\"\n");
        fprintf(fenv, "ROOM_TITLE=\"Compass and Map\"\n");
        fprintf(fenv, "ROOM_COMMANDS=\"which, whereis, echo $PATH\"\n");
        fprintf(fenv, "ROOM_MISSION=\"A mission diagnostics binary named 'sec-beacon' was installed in one of the directories on your executable system PATH. Use 'which' or inspect '$PATH' to discover its location, then execute it with any location parameter to trigger the beacon!\nExample: sec-beacon /usr/local/bin\"\n");
        fprintf(fenv, "ROOM_HINT=\"Use 'man -k <keyword>' for keyword based searches. Search man pages for keywords like: which / locate command / locate binary\"\n");
        fclose(fenv);
        chmod("/etc/linux_ctf_room.env", 0644);
    }

    /* 2. Setup guide note */
    FILE *fr = fopen("/home/learner/README.txt", "w");
    if (fr) {
        fprintf(fr, "System Diagnostic Beacon Alert:\n");
        fprintf(fr, "The beacon utility 'sec-beacon' is active in your system PATH.\n");
        fprintf(fr, "Locate where it lives, and run it with the path as an argument to authenticate.\n");
        fclose(fr);
        chmod("/home/learner/README.txt", 0644);
    }

    return 0;
}
