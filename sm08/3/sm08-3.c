#include <stdio.h>
#include <inttypes.h>

enum {
    DEFAULT_VALUE = -1,
    MANT = 23
};

int is_repr(unsigned value) {
    size_t a = DEFAULT_VALUE;
    size_t b = DEFAULT_VALUE;

    for (size_t i = 0; value; ++i, value >>= 1) {
        if (value & 1) {
            a = a != DEFAULT_VALUE ? a : i;
            b = i;
        }
    }

    return b - a <= MANT;
}

#ifdef TESTING
void test() {
    int n = 1;
    for (int i = 0; i != 31; ++i) {
        int num = 1U << i;
        // степени 2 могут быть представлены в float
        // Но если к ним что-то прибавить и использовать мантиссу, то тут
        // уже возникают проблемы
        num += 5;
        // не всегда выполняется
        // но вот в double все int помещаются уже
        printf("%u %d\n", num, num == (int)(float)num);
    }
}
#endif

int main() {
#ifdef TESTING
    test();
#endif
    int ret_code = 0;
    unsigned x;

    while ((ret_code = scanf("%u", &x)) != EOF) {
        if (ret_code != 1) {
            return 1;
        }
        printf("%d\n", is_repr(x));
    }
}