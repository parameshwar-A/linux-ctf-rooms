#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

int main(void) {
    int s = socket(AF_UNIX, SOCK_STREAM, 0);
    if (s < 0) return 1;
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, "/run/ctf.sock", sizeof(addr.sun_path) - 1);
    if (connect(s, (struct sockaddr *)&addr, sizeof(addr)) != 0) {
        fprintf(stderr, "\033[1;31m[-] Validation service unavailable. Ensure CTF environment is active.\033[0m\n");
        close(s);
        return 1;
    }
    if (write(s, "CHECK 03\n", 9) < 0) {
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
        printf("\033[1;32m[+] [SUCCESS] Dispatch configuration verified!\033[0m\n");
        printf("\033[1;32m[+] FLAG: %s\033[0m\n", flag);
        printf("[*] Run 'submit-flag \"%s\"' to record victory!\n", flag);
        return 0;
    } else {
        char *msg = strncmp(buf, "FAIL ", 5) == 0 ? buf + 5 : buf;
        printf("\033[1;31m%s\033[0m", msg);
        return 1;
    }
}
