#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <ctype.h>

#define SOCKET_PATH "/run/ctf.sock"
#define FLAG_PATH "/run/ctf/flag"
#define ANSWER_PATH "/run/ctf/answer"
#define DECOYS_PATH "/run/ctf/decoys"

static char g_flag[256] = {0};
static char g_answer[256] = {0};

static void trim(char *s) {
    if (!s) return;
    int len = strlen(s);
    while (len > 0 && (s[len - 1] == '\n' || s[len - 1] == '\r' || s[len - 1] == ' ' || s[len - 1] == '\t')) {
        s[len - 1] = '\0';
        len--;
    }
}

static int ct_equal(const char *a, const char *b) {
    if (!a || !b) return 0;
    size_t la = strlen(a);
    size_t lb = strlen(b);
    if (la != lb) return 0;
    unsigned char diff = 0;
    for (size_t i = 0; i < la; i++) {
        diff |= (unsigned char)(a[i] ^ b[i]);
    }
    return diff == 0;
}

static void read_file_secret(const char *path, char *buf, size_t max_len) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    if (fgets(buf, max_len, f)) {
        trim(buf);
    }
    fclose(f);
}

static int is_decoy(const char *input) {
    FILE *f = fopen(DECOYS_PATH, "r");
    if (!f) return 0;
    char line[256];
    int matched = 0;
    while (fgets(line, sizeof(line), f)) {
        trim(line);
        if (strlen(line) > 0 && ct_equal(input, line)) {
            matched = 1;
            break;
        }
    }
    fclose(f);
    return matched;
}

static void handle_submit(int client_fd, const char *arg) {
    char clean_flag[256] = {0};
    strncpy(clean_flag, arg, sizeof(clean_flag) - 1);
    trim(clean_flag);

    if (g_flag[0] == '\0') {
        read_file_secret(FLAG_PATH, g_flag, sizeof(g_flag));
    }

    if (strlen(clean_flag) > 0 && ct_equal(clean_flag, g_flag)) {
        dprintf(client_fd,
            "SUCCESS\n"
            "\033[1;32m============================================================\033[0m\n"
            "  \033[1;32m[+] SUCCESS! FLAG ACCEPTED!\033[0m\n"
            "  \033[1;32m[+] Room completed successfully.\033[0m\n"
            "\033[1;32m============================================================\033[0m\n");
    } else if (is_decoy(clean_flag)) {
        dprintf(client_fd,
            "DECOY\n"
            "\033[1;33m============================================================\033[0m\n"
            "  \033[1;33m[-] That's a decoy flag! Re-read the room mission.\033[0m\n"
            "\033[1;33m============================================================\033[0m\n");
    } else {
        dprintf(client_fd,
            "INCORRECT\n"
            "\033[1;31m============================================================\033[0m\n"
            "  \033[1;31m[-] INCORRECT FLAG! Keep exploring...\033[0m\n"
            "  \033[1;33m[*] Tip: Re-read the room briefing and check the man pages.\033[0m\n"
            "\033[1;31m============================================================\033[0m\n");
    }
}

static void handle_check_03(int client_fd) {
    FILE *f = fopen("/home/learner/dispatch.conf", "r");
    if (!f) {
        dprintf(client_fd, "FAIL [-] 'dispatch.conf' not found! Please create the configuration file first.\n");
        return;
    }
    char l1[128] = {0}, l2[128] = {0};
    if (!fgets(l1, sizeof(l1), f) || !fgets(l2, sizeof(l2), f)) {
        dprintf(client_fd, "FAIL [-] 'dispatch.conf' must contain at least 2 lines.\n");
        fclose(f);
        return;
    }
    fclose(f);
    trim(l1);
    trim(l2);

    /* Read expected answer if dynamic answer file present, else default */
    char exp1[128] = "MODE=ACTIVE";
    char exp2[128] = "TARGET=PRODUCTION";
    FILE *fa = fopen(ANSWER_PATH, "r");
    if (fa) {
        char al1[128] = {0}, al2[128] = {0};
        if (fgets(al1, sizeof(al1), fa) && fgets(al2, sizeof(al2), fa)) {
            trim(al1); trim(al2);
            if (strlen(al1) > 0 && strlen(al2) > 0) {
                strncpy(exp1, al1, sizeof(exp1) - 1);
                strncpy(exp2, al2, sizeof(exp2) - 1);
            }
        }
        fclose(fa);
    }

    if (strcmp(l1, exp1) == 0 && strcmp(l2, exp2) == 0) {
        dprintf(client_fd, "OK %s\n", g_flag);
    } else {
        dprintf(client_fd, "FAIL [-] Verification failed!\n    Line 1 got: '%s' (expected '%s')\n    Line 2 got: '%s' (expected '%s')\n",
            l1, exp1, l2, exp2);
    }
}

static void handle_check_04(int client_fd) {
    struct stat s_prod, s_bak;
    if (stat("/home/learner/production/app_payload.dat", &s_prod) != 0) {
        dprintf(client_fd, "FAIL [-] 'production/app_payload.dat' not found! Did you move it?\n");
        return;
    }
    if (stat("/home/learner/backups/app_payload.dat", &s_bak) != 0) {
        dprintf(client_fd, "FAIL [-] 'backups/app_payload.dat' not found! Did you create the backup copy?\n");
        return;
    }
    if (access("/home/learner/staging/temp_payload.dat", F_OK) == 0) {
        dprintf(client_fd, "FAIL [-] Original file 'staging/temp_payload.dat' still exists! Remember to MOVE (mv) it, not copy.\n");
        return;
    }
    if (s_prod.st_size <= 0 || s_prod.st_size != s_bak.st_size) {
        dprintf(client_fd, "FAIL [-] Production and backup payloads differ in size or are empty.\n");
        return;
    }
    dprintf(client_fd, "OK %s\n", g_flag);
}

static void handle_check_05(int client_fd) {
    if (access("/home/learner/sandbox/clutter/secret_key.txt", F_OK) == 0) {
        dprintf(client_fd, "FAIL [-] 'secret_key.txt' still exists in 'sandbox/clutter/'!\n    Please move the key into the secure vault.\n");
        return;
    }
    FILE *f = fopen("/home/learner/sandbox/secure_vault/secret_key.txt", "r");
    if (!f) {
        dprintf(client_fd, "FAIL [-] Key file 'sandbox/secure_vault/secret_key.txt' not found!\n    Locate the secret key amidst the clutter and move it into the vault.\n");
        return;
    }
    fclose(f);
    dprintf(client_fd, "OK %s\n", g_flag);
}

static void handle_check_06(int client_fd, const char *arg) {
    if (!arg || strlen(arg) == 0) {
        dprintf(client_fd, "FAIL [-] Authentication Error: Location parameter missing!\n    Usage: sec-beacon <FULL_BINARY_PATH>\n");
        return;
    }
    char clean_path[256] = {0};
    strncpy(clean_path, arg, sizeof(clean_path) - 1);
    trim(clean_path);

    char exp_path[256] = "/usr/local/bin/sec-beacon";
    FILE *fa = fopen(ANSWER_PATH, "r");
    if (fa) {
        if (fgets(exp_path, sizeof(exp_path), fa)) {
            trim(exp_path);
        }
        fclose(fa);
    }

    if (strcmp(clean_path, exp_path) == 0 ||
        (strcmp(exp_path, "/usr/local/bin/sec-beacon") == 0 && strcmp(clean_path, "/usr/local/bin") == 0)) {
        dprintf(client_fd, "OK %s\n", g_flag);
    } else {
        dprintf(client_fd,
            "FAIL [-] Authentication Failed: '%s' is NOT where this binary is installed!\n"
            "    Hint: Use 'which sec-beacon' to locate the binary on your PATH.\n",
            clean_path);
    }
}

static void handle_check_15(int client_fd, const char *arg) {
    char input_ip[128] = {0};
    if (arg) strncpy(input_ip, arg, sizeof(input_ip) - 1);
    trim(input_ip);

    if (strlen(input_ip) == 0) {
        dprintf(client_fd, "FAIL [-] Usage: report_attacker <ATTACKER_IP>\n");
        return;
    }

    char exp_ip[128] = "10.0.4.77";
    FILE *fa = fopen(ANSWER_PATH, "r");
    if (fa) {
        if (fgets(exp_ip, sizeof(exp_ip), fa)) {
            trim(exp_ip);
        }
        fclose(fa);
    }

    if (strcmp(input_ip, exp_ip) == 0) {
        dprintf(client_fd, "OK %s\n", g_flag);
    } else {
        dprintf(client_fd, "FAIL [-] Incorrect IP address: '%s'. Check your pipeline!\n", input_ip);
    }
}

int main(void) {
    read_file_secret(FLAG_PATH, g_flag, sizeof(g_flag));
    read_file_secret(ANSWER_PATH, g_answer, sizeof(g_answer));

    int s = socket(AF_UNIX, SOCK_STREAM, 0);
    if (s < 0) {
        perror("socket");
        return 1;
    }

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    unlink(SOCKET_PATH);
    if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) != 0) {
        perror("bind");
        close(s);
        return 1;
    }

    chmod(SOCKET_PATH, 0666);
    if (listen(s, 16) != 0) {
        perror("listen");
        close(s);
        return 1;
    }

    for (;;) {
        int client_fd = accept(s, NULL, NULL);
        if (client_fd < 0) continue;

        char buf[512] = {0};
        ssize_t n = read(client_fd, buf, sizeof(buf) - 1);
        if (n > 0) {
            buf[n] = '\0';
            trim(buf);

            if (strncmp(buf, "SUBMIT ", 7) == 0) {
                handle_submit(client_fd, buf + 7);
            } else if (strncmp(buf, "CHECK 03", 8) == 0) {
                handle_check_03(client_fd);
            } else if (strncmp(buf, "CHECK 04", 8) == 0) {
                handle_check_04(client_fd);
            } else if (strncmp(buf, "CHECK 05", 8) == 0) {
                handle_check_05(client_fd);
            } else if (strncmp(buf, "CHECK 06", 8) == 0) {
                char *arg = buf + 8;
                while (*arg == ' ') arg++;
                handle_check_06(client_fd, arg);
            } else if (strncmp(buf, "CHECK 15", 8) == 0) {
                char *arg = buf + 8;
                while (*arg == ' ') arg++;
                handle_check_15(client_fd, arg);
            } else {
                dprintf(client_fd, "FAIL Unknown command\n");
            }
        }
        close(client_fd);
    }

    close(s);
    unlink(SOCKET_PATH);
    return 0;
}
