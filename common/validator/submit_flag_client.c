#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/run/ctf.sock"

static void trim(char *s) {
    if (!s) return;
    int len = strlen(s);
    while (len > 0 && (s[len - 1] == '\n' || s[len - 1] == '\r' || s[len - 1] == ' ' || s[len - 1] == '\t')) {
        s[len - 1] = '\0';
        len--;
    }
}

int main(int argc, char *argv[]) {
    char flag[256] = {0};

    if (argc > 1) {
        strncpy(flag, argv[1], sizeof(flag) - 1);
    } else {
        printf("\033[1;36m[?] Enter Flag: \033[0m");
        fflush(stdout);
        if (fgets(flag, sizeof(flag), stdin) == NULL) {
            fprintf(stderr, "\033[1;31m[-] Error reading input.\033[0m\n");
            return 1;
        }
    }

    trim(flag);
    if (strlen(flag) == 0) {
        printf("\033[1;33m[!] Usage: submit-flag <FLAG>\033[0m\n");
        return 1;
    }

    int s = socket(AF_UNIX, SOCK_STREAM, 0);
    if (s < 0) {
        fprintf(stderr, "[-] Failed to create socket\n");
        return 1;
    }

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    if (connect(s, (struct sockaddr *)&addr, sizeof(addr)) != 0) {
        fprintf(stderr, "\033[1;31m[-] Validation service unavailable. Please check that the room is running.\033[0m\n");
        close(s);
        return 1;
    }

    char req[512];
    snprintf(req, sizeof(req), "SUBMIT %s\n", flag);
    if (write(s, req, strlen(req)) < 0) {
        perror("write");
        close(s);
        return 1;
    }

    char resp[1024];
    ssize_t n = read(s, resp, sizeof(resp) - 1);
    close(s);

    if (n <= 0) {
        fprintf(stderr, "[-] No response from validator daemon\n");
        return 1;
    }
    resp[n] = '\0';

    char *body = strchr(resp, '\n');
    if (body) {
        *body = '\0';
        body++;
    } else {
        body = resp;
    }

    int is_success = (strcmp(resp, "SUCCESS") == 0);
    printf("%s", body);
    return is_success ? 0 : 1;
}
