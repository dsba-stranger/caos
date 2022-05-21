#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>


int main(int argc, char **argv) {
    if (argc < 3) {
        return 1;
    }

    int fds[2];

    if (pipe(fds)) {
        perror("pipe");
        return 1;
    }

    pid_t pid_1 = fork();

    if (pid_1 < 0) {
        return 1;
    } else if (!pid_1) {
        dup2(fds[1], 1);
        close(fds[0]);
        close(fds[1]);
        execlp(argv[1], argv[1], NULL);
        _exit(1);
    }

    close(fds[1]);
    pid_t pid_2 = fork();

    if (pid_2 < 0) {
        kill(pid_1, SIGKILL);
        waitpid(pid_1, NULL, 0);
        return 1;
    } else if (!pid_2) {
        dup2(fds[0], 0);
        close(fds[0]);
        execlp(argv[2], argv[2], NULL);
        _exit(1);
    }

    close(fds[0]);
    waitpid(pid_1, NULL, 0);
    waitpid(pid_2, NULL, 0);
}