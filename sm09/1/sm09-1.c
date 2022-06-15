#include <stdio.h>

int main() {
    asm ("mov $1, %eax\n"
         "cpuid\n"
    );
}