#include <stdint.h>
#include <string.h>

#ifdef TESTING
typedef enum FPClass
{
    FFP_ZERO, FFP_DENORMALIZED, FFP_NORMALIZED, FFP_INF, FFP_NAN
} FPClass;
#endif

enum Constants {
    SIGN_BITS = 1,
    MANT_BITS = 23,
    EXP_BITS = 8,
    ALL_BITS = 32,
    MAX_EXP = (1 << EXP_BITS) - 1
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

FPClass fpclassf(float value, int *psign) {
    unsigned sign = get_sign(value);
    unsigned exp = get_exp(value);
    unsigned mant = get_mant(value);
    *psign = sign;

    if (exp == MAX_EXP) {
        if (mant != 0) {
            *psign = 0;
            return FFP_NAN;
        }
        return FFP_INF;
    } else if (exp == 0) {
        if (mant == 0) {
            return FFP_ZERO;
        }
        return FFP_DENORMALIZED;
    }
    return FFP_NORMALIZED;
}

#ifdef TESTING
#include <stdio.h>
void test(float value) {
    int sign;
    const char* names[] = {"ZERO", "DENORMALIZED", "NORMALIZED", "INF", "NAN"};
    printf("value: %f fpclassf: %s sign:%d\n", value, names[fpclassf(value, &sign)], sign);
}
int main() {
    test(0.0f);
    test(1.0f / 0.0f);
    test(2.0f);
    test(-2.0f);
    test(-100000.0f);
    test(0.00001f);
}
#endif