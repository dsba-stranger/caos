#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

enum Constants {
    SHIFT_STATUS = 128
};

int mysys(const char *str) {
    pid_t pid = fork();

    if (pid < 0) {
        return -1;
    }

    if (!pid) {
        execl("/bin/sh", "sh", "-c", str, NULL);
        _exit(127);
    }

    int stat = 0;
    waitpid(pid, &stat, 0);
    if (WIFSIGNALED(stat)) {
        return SHIFT_STATUS + WTERMSIG(stat);
    } else {
        return WEXITSTATUS(stat);
    }
}

#ifdef TESTING
int main() {
    mysys();
}
#endif