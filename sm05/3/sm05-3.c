#include <stdint.h>

uint32_t satsum(uint32_t v1, uint32_t v2) {
    uint32_t value = v1 + v2;
    // безнаковые типы ведут себя как вычеты
    if (value < v1) {
        return -1;
    }
    return value;
}

#ifdef TESTING
#include <stdio.h>

int main() {
    printf("%u", satsum(100, 0xffffffff));
    return 0;
}

#endif