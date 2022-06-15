#include <sys/syscall.h>
#include <unistd.h>
#include <stdint.h>

int make_syscall(int eax, int ebx, int ecx, int edx) {
    volatile int result = 0;
    asm volatile (
        "mov    %1, %%eax\n"
        "mov    %2, %%ebx\n"
        "mov    %3, %%ecx\n"
        "mov    %4, %%edx\n"
        "int    $0x80\n"
        "mov    %%eax, %0\n"
        :"=g"(result)
        :"g"(eax), "g"(ebx), "g"(ecx), "g"(edx)
        :"eax", "ebx", "ecx", "edx"
    );
    return result;
}

int std_write(int fd, char *ptr, int size) {
    return make_syscall(__NR_write, fd, (int)ptr, size);
}

void std_exit(int code) {
    make_syscall(__NR_exit, code, 0, 0);
}

void _start() {
    // обратиться к системному вызову write
    // обратиться к системному вызову exit
    enum {STR_LEN = 12};
    char *str = "hello world\n";
    std_write(STDOUT_FILENO, str, STR_LEN);
    std_exit(0);
}