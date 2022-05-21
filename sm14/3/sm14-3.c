#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

volatile sig_atomic_t mode = 0;

void handler(int signum) {
    mode = signum;
}

int main() {
    struct sigaction sa = {.sa_handler = handler, .sa_flags = SA_RESTART};

    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGUSR2, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

    sigset_t mask;

    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    sigaddset(&mask, SIGUSR2);
    sigaddset(&mask, SIGTERM);

    int value1 = 0;
    int value2 = 0;

    sigset_t omask;
    sigprocmask(SIG_BLOCK, &mask, &omask);

    printf("%d\n", getpid());
    fflush(stdout);

    while (1) {
        sigsuspend(&omask);

        if (mode == SIGUSR1) {
            printf("%d %d\n", value1++, value2);
            fflush(stdout);
        } else if (mode == SIGUSR2) {
            ++value2;
        } else if (mode == SIGTERM) {
            exit(0);
        }
    }
}