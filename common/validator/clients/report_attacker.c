#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <ATTACKER_IP>\n", argv[0]);
        return 1;
    }
    int s = socket(AF_UNIX, SOCK_STREAM, 0);
    if (s < 0) return 1;
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, "/run/ctf.sock", sizeof(addr.sun_path) - 1);
    if (connect(s, (struct sockaddr *)&addr, sizeof(addr)) != 0) {
        fprintf(stderr, "[-] Analysis validation service unavailable.\n");
        close(s);
        return 1;
    }
    char req[256];
    snprintf(req, sizeof(req), "CHECK 15 %s\n", argv[1]);
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
        printf("=====================================================\n");
        printf("  [+] ATTACK PATTERN ACCURATELY IDENTIFIED!\n");
        printf("=====================================================\n");
        printf("[+] CAPSTONE COMPLETE! MASTER FLAG: %s\n", flag);
        printf("[*] Run 'submit-flag \"%s\"' to claim victory!\n", flag);
        return 0;
    } else {
        char *msg = strncmp(buf, "FAIL ", 5) == 0 ? buf + 5 : buf;
        printf("%s", msg);
        return 1;
    }
}
