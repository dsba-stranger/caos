int mystrcmp(const char *str1, const char *str2) {
    while (*str1 && *str2 && *str1 == *str2) {
        ++str1;
        ++str2;
    }

    return *(const unsigned char*)str1 - *(const unsigned char*)str2;
}

#ifdef TESTING
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

char* generate() {
    int length = rand() % 20;
    char* buffer = malloc(length);

    for (int i = 0; i < length; ++i) {
        buffer[i] = 'a' + (rand() % 26);
    }

    return buffer;
}

int main() {
    srand(time(NULL));

    for (int i = 0; i < 5; ++i) {
        const char* a = generate();
        const char* b = generate();
        int my = mystrcmp(a, b), their = strcmp(a, b);
        if (my != their) {
            printf("%s %d %d\n", a, my, their);
        }
    }


    return 0;
}
#endif