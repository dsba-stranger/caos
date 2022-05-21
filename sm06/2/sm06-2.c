#ifdef TESTING
typedef int STYPE;
typedef unsigned int UTYPE;
#endif

int bitcount(STYPE value) {
    int res = 0;
    UTYPE unsigned_value = value;

    while (unsigned_value) {
        res += unsigned_value & 1;
        unsigned_value >>= 1;
    }

    return res;
}

#ifdef TESTING
#define Test(x) x, bitcount(x)
#include <stdio.h>
int main() {
    printf("%x %d", Test(-1234));
}
#endif