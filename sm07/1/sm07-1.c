#include <stdint.h>

int32_t satsum(int32_t v1, int32_t v2) {
    int32_t res = 0;
    if (__builtin_add_overflow(v1, v2, &res)) {
        static const int32_t max_int = ((uint32_t)~0 << 1) >> 1;
        return res > 0 ? ~max_int : max_int;
    }
    return res;
}

#ifdef TESTING
#include <stdio.h>
int main() {
    printf("%d\n", satsum(4, 2));
    printf("%d\n", satsum(0x7ffffff5, 1235));
    printf("%d\n", satsum(0x7fffffff, 1));
    printf("%d\n", satsum(0x80000000, -1));
    printf("%d\n", satsum(0x80000010, -1337));
}
#endif