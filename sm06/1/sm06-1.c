#include <stdio.h>
#include <stdint.h>

uint32_t satavg(uint32_t v1, uint32_t v2) {
    uint32_t m = (v1 & 1) && (v2 & 1);
    v1 /= 2;
    v2 /= 2;
    uint32_t value = v1 + v2 + m;
    if (value < v1) {
        return -1;
    }
    return value;
}

int main() {
    unsigned int a, b;
    if (scanf("%u %u", &a, &b) == 2) {
        printf("%u\n", satavg(a, b));
    }
    return 0;
}