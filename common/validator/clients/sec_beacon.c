#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

int main(int argc, char *argv[]) {
    printf("=====================================================\n");
    printf("  SEC-BEACON SYSTEM DIAGNOSTICS ACTIVE\n");
    printf("=====================================================\n");
    if (argc < 2) {
        printf("[-] Authentication Error: Location parameter missing!\n");
        printf("    Usage: sec-beacon <current_path_or_keyword>\n");
        return 1;
    }
    int s = socket(AF_UNIX, SOCK_STREAM, 0);
    if (s < 0) return 1;
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, "/run/ctf.sock", sizeof(addr.sun_path) - 1);
    if (connect(s, (struct sockaddr *)&addr, sizeof(addr)) != 0) {
        fprintf(stderr, "[-] Beacon validation service unavailable.\n");
        close(s);
        return 1;
    }
    char req[256];
    snprintf(req, sizeof(req), "CHECK 06 %s\n", argv[1]);
    if (write(s, req, strlen(req)) < 0) {
        close(s);
        return 1;
    }
    char buf[512] = {0};
    ssize_t n = read(s, buf, sizeof(buf) - 1);
    close(s);
    if (n <= 0) return 1;
    if (strncmp(buf, "OK ", 3) == 0) {
        char *flag = buf + 3;
        while (*flag == ' ') flag++;
        char *nl = strchr(flag, '\n'); if (nl) *nl = 0;
        printf("[+] BEACON LOCATED & AUTHENTICATED!\n");
        printf("[+] FLAG: %s\n", flag);
        printf("[*] Run 'submit-flag \"%s\"' to claim victory!\n", flag);
        return 0;
    } else {
        printf("[-] Authentication failed.\n");
        return 1;
    }
}
