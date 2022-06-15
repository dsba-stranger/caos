#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

typedef double (*math_function_t)(double);

typedef struct {
    const char *name;
    const math_function_t function;
} TableEntry;

const static TableEntry table[] =
{
    {"sin", sin},
    {"cos", cos},
    {"exp", exp},
    {"log", log},
    {"tan", tan},
    {"sqrt", sqrt}
};

int main() {
    char *name = NULL;
    double x;

    int ret;
    while ((ret = scanf("%ms %lf", &name, &x)) != EOF) {
        math_function_t f = NULL;
        for (size_t i = 0; i < sizeof(table) / sizeof(*table); ++i) {
            if (strcmp(table[i].name, name) == 0) {
                f = table[i].function;
                break;
            }
        }
        double res = f ? f(x) : NAN;
        printf("%a\n", res);
        free(name);
    }

}