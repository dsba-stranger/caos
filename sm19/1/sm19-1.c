#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

int is_ub_sum(int a, int b) {
    return (a > 0 && b > INT_MAX - a) || (a < 0 && b < INT_MIN - a);
}

int is_ub_div(int a, int b) {
    return (a == INT_MIN && b == -1) || b == 0;
}

int main(int argc, char **argv) {
    int val1 = strtol(argv[1], NULL, 10);
    int val2 = strtol(argv[2], NULL, 10);
    printf("%d\n", is_ub_sum(val1, val2));
    printf("%d\n", is_ub_div(val1, val2));
}