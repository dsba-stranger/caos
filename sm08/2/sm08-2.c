#include <stdio.h>
#include <string.h>

enum Constants {
    SIGN_BITS = 1,
    MANT_BITS = 23,
    EXP_BITS = 8,
    ALL_BITS = 32,
};

// Utility fn for exponent extraction
static unsigned get_exp(float num) {
    unsigned exp;
    memcpy(&exp, &num, sizeof(num));
    return exp << SIGN_BITS >> (SIGN_BITS + MANT_BITS);
}

// Utility fn for mant extraction
static unsigned get_mant(float num) {
    unsigned mant;
    memcpy(&mant, &num, sizeof(num));
    int to_remove = SIGN_BITS + EXP_BITS;
    return mant << (to_remove) >> to_remove;
}

// Utility fn for sign extraction
static unsigned get_sign(float num) {
    unsigned sign;
    memcpy(&sign, &num, sizeof(num));
    return sign >> (ALL_BITS - SIGN_BITS);
}

int main() {
    int ret_code = 0;
    float x;

    while ((ret_code = scanf("%f", &x)) != EOF) {
        if (ret_code != 1) {
            return 1;
        }
        printf("%u %u %x\n", get_sign(x), get_exp(x), get_mant(x));
    }
}