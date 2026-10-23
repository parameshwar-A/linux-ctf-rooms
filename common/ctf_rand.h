#ifndef CTF_RAND_H
#define CTF_RAND_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/random.h>

static inline uint32_t ctf_rand(uint32_t n) {
    if (n <= 1) return 0;
    uint32_t x = 0;
    if (getrandom(&x, sizeof(x), 0) != sizeof(x)) {
        abort();
    }
    return x % n;
}

static inline void ctf_rand_hex(char *out, size_t num_bytes) {
    uint8_t buf[64];
    if (num_bytes > sizeof(buf)) num_bytes = sizeof(buf);
    if (getrandom(buf, num_bytes, 0) != (ssize_t)num_bytes) {
        abort();
    }
    for (size_t i = 0; i < num_bytes; i++) {
        sprintf(out + (i * 2), "%02x", buf[i]);
    }
}

static inline void ctf_rand_ip(char *out, size_t len) {
    snprintf(out, len, "10.%u.%u.%u", ctf_rand(256), ctf_rand(256), 1 + ctf_rand(254));
}

#endif
