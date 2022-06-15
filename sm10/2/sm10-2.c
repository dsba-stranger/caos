#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char **argv) {
    long long sz = 0;
    for (int i = 1; i < argc; ++i) {
        struct stat st;
        if (lstat(argv[i], &st) >= 0 && st.st_nlink == 1 && S_ISREG(st.st_mode)) {
            sz += st.st_size;
        }
    }
    printf("%lld\n", sz);
    return 0;
}