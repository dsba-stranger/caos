#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>

int main() {
    pid_t me = getpid();

    int x;
    while (scanf("%d", &x) > 0) {
        pid_t pid = fork();
        if (pid < 0) {
            _exit(1);
        } else if (!pid) {
            continue;
        } else {
            int status;
            waitpid(pid, &status, 0);

            if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
                printf("%d\n", x);
                fflush(stdout);
                _exit(0);
            }

            if (getpid() == me) {
                printf("-1\n");
                fflush(stdout);
                _exit(0);
            }

            _exit(1);
        }
    }
}