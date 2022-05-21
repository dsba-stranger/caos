#include <unistd.h>
#include <signal.h>
#include <stdio.h>

enum Mode {
    NEGATIVE_MODE = SIGUSR1,
    SQUARE_MODE = SIGUSR2
};

volatile sig_atomic_t mode = NEGATIVE_MODE;

void handler(int signum) {
    mode = signum;
}

int main() {

    struct sigaction sa = {};
    sa.sa_handler = handler;
    sa.sa_flags = SA_RESTART;
    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGUSR2, &sa, NULL);

    printf("%d\n", getpid());
    fflush(stdout);

    int value;
    int ret_val;
    while ((ret_val = scanf("%d", &value)) != EOF) {
        if (ret_val != 1) {
            return 1;
        }
        long long ans = mode == NEGATIVE_MODE ? -(long long)value : (long long)value * value;
        printf("%lld\n", ans);
        fflush(stdout);
    }
}