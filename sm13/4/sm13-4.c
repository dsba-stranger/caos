#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

int main() {
    int fds[2];

    if (pipe(fds) == -1) {
        return 1;
    }

    pid_t pid = fork();

    if (pid < 0) {
        return 1;
    } else if (!pid) {
        close(fds[1]);
        pid_t child_pid = fork();

        if (child_pid < 0) {
            exit(1);
        } else if (!child_pid) {
            long long sum = 0;
            int x;

            while (read(fds[0], &x, sizeof(x)) > 0) {
                sum += x;
            }

            printf("%lld\n", sum);
            close(fds[0]);
            exit(0);
        }

        close(fds[0]);
        wait(NULL);
        exit(0);
    }

    int ret_code = 0;
    int x = 0;
    int exit_code = 0;
    while ((ret_code = scanf("%d", &x)) != EOF) {
        if (ret_code != 1) {
            exit_code = 1;
            break;
        }
        write(fds[1], &x, sizeof(x));
#ifdef TESTING
        printf("write %d\n", x);
#endif
    }

    close(fds[1]);
    wait(NULL);
    return exit_code;
}