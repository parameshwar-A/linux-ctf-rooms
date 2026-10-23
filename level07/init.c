/* Linux CTF - Level 07 Initializer (Disguised Identities) */
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
    const char *flag = argv[1];

    /* 1. Setup MOTD room environment config */
    FILE *fenv = fopen("/etc/linux_ctf_room.env", "w");
    if (fenv) {
        fprintf(fenv, "ROOM_ID=\"07\"\n");
        fprintf(fenv, "ROOM_TITLE=\"Disguised Identities\"\n");
        fprintf(fenv, "ROOM_COMMANDS=\"file\"\n");
        fprintf(fenv, "ROOM_MISSION=\"Inspect the files inside 'suspects/'. Files inside the suspects have wrong extensions added. All files have misleading extensions (.png, .mp3, .pdf, .bin, .tar). Exactly one file is an ASCII text file containing the mission flag. Find a way to detect the actual type of the file rather than the file extension.\"\n");
        fprintf(fenv, "ROOM_HINT=\"Use 'man -k <keyword>' for keyword based searches. Search the man pages for keywords like: file type / file / determining file content type\"\n");
        fclose(fenv);
        chmod("/etc/linux_ctf_room.env", 0644);
    }

    /* 2. Setup puzzle layout with decoys */
    mkdir("/home/learner/suspects", 0755);

    /* Record decoys in /run/ctf/decoys */
    FILE *fd = fopen("/run/ctf/decoys", "w");
    if (fd) {
        fprintf(fd, "FLAG{l07_decoy_in_compressed_photo}\n");
        fprintf(fd, "FLAG{l07_decoy_in_audio_waveform}\n");
        fprintf(fd, "FLAG{l07_decoy_in_pdf_executable}\n");
        fprintf(fd, "FLAG{l07_decoy_in_backup_archive}\n");
        fclose(fd);
    }

    /* photo.png: gzip compressed data with embedded decoy */
    FILE *fp = fopen("/tmp/photo_plain.txt", "w");
    if (fp) {
        fprintf(fp, "BINARY IMAGE STREAM ... FLAG{l07_decoy_in_compressed_photo}\n");
        fclose(fp);
        system("gzip -c /tmp/photo_plain.txt > /home/learner/suspects/photo.png && rm -f /tmp/photo_plain.txt");
    }

    /* audio.mp3: binary data with embedded decoy */
    FILE *fa = fopen("/home/learner/suspects/audio.mp3", "wb");
    if (fa) {
        unsigned char dummy[] = {0xFF, 0xFB, 0x90, 0x44, 0x00, 0x12, 0x34, 0x56};
        fwrite(dummy, 1, sizeof(dummy), fa);
        fprintf(fa, "ID3_HEADER_AUDIO_STREAM_FLAG{l07_decoy_in_audio_waveform}\x00\xFF");
        fclose(fa);
    }

    /* manual.pdf: copy of ELF binary with decoy appended */
    system("cp /bin/true /home/learner/suspects/manual.pdf && echo 'FLAG{l07_decoy_in_pdf_executable}' >> /home/learner/suspects/manual.pdf");

    /* backup.tar: random binary bytes with decoy */
    FILE *fb = fopen("/home/learner/suspects/backup.tar", "wb");
    if (fb) {
        for (int i = 0; i < 64; i++) fputc((i * 37) % 256, fb);
        fprintf(fb, "USTAR_STREAM_FLAG{l07_decoy_in_backup_archive}\x00");
        fclose(fb);
    }

    /* firmware.bin is SECRETLY plain ASCII text holding the genuine flag! */
    FILE *ff = fopen("/home/learner/suspects/firmware.bin", "w");
    if (ff) {
        fprintf(ff, "CLASSIFIED MEMORANDUM\n");
        fprintf(ff, "The firmware file extension was chosen to deter casual observers.\n");
        fprintf(ff, "AUTHENTICATED FLAG:\n%s\n", flag);
        fclose(ff);
        chmod("/home/learner/suspects/firmware.bin", 0644);
    }

    return 0;
}
