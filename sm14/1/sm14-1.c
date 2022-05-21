#include <unistd.h>
#include <signal.h>
#include <stdio.h>

volatile int value = 0;

void handler(int signum) {
    enum Constants { MAX_VALUE = 4 };

    printf("%d\n", value++);
    fflush(stdout);

    if (value == MAX_VALUE) {
        _exit(0);
    }
}

int main() {
    struct sigaction sa = {};
    sa.sa_handler = handler;
    sa.sa_flags = SA_RESTART;
    sigaction(SIGINT, &sa, NULL);

    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);

    sigprocmask(SIG_BLOCK, &mask, NULL);

    printf("%d\n", getpid());
    fflush(stdout);

    sigprocmask(SIG_UNBLOCK, &mask, NULL);

    while (1) {
        pause();
    }
}