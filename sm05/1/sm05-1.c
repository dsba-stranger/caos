void normalize_path(char *buf) {
    if (!*buf) {
        return;
    }

    char *ptr = buf + 1;

    while (*ptr) {
        if (*buf != '/' || *ptr != '/') {
            *(++buf) = *ptr;
        }
        ++ptr;
    }

    *(buf + 1) = '\0';
}

#ifdef TESTING
#include <stdio.h>

int main() {
    char buffer[128] = "a/b///////////c/d/////e";
    normalize_path(buffer);
    printf("%s", buffer);
    return 0;
}

#endif