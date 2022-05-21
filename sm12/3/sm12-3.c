#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int run(const char *cmd) {
    pid_t pid = fork();

    if (pid < 0) {
        return 0;
    } else if (!pid) {
        execlp(cmd, cmd, NULL);
        _exit(1);
    }

    int status = 0;
    waitpid(pid, &status, 0);
    return WIFEXITED(status) && !WEXITSTATUS(status);
}

int main(int argc, char **argv) {
    if (argc < 3) {
        return 1;
    }
    return !((run(argv[1]) || run(argv[2])) && run(argv[3]));
}