#include <fcntl.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int run(const char *cmd, const char *output) {
    pid_t pid = fork();

    if (pid < 0) {
        return 0;
    } else if (!pid) {
        if (output) {
            int fd = open(output, O_RDWR | O_CREAT | O_CLOEXEC | O_TRUNC, 0666);
            if (fd < 0) {
                _exit(1);
            }
            dup2(fd, STDOUT_FILENO);
        }
        execlp(cmd, cmd, NULL);
        _exit(1);
    }

    int status = 0;
    waitpid(pid, &status, 0);
    return WIFEXITED(status) && !WEXITSTATUS(status);
}

int main(int argc, char **argv) {
    if (argc < 4) {
        return 1;
    }
    if (!run(argv[1], NULL)) {
        return !run(argv[2], argv[3]);
    }
}