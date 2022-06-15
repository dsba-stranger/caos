#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <limits.h>
#include <stdio.h>

int interact(const char* cmd, const char* input, char* outbuf) {
    enum { ERROR_CODE = 127, CHUNK_SIZE = 1 << 10 };

    int fd[2];
    int fd2[2];

    if (pipe(fd)) {
        return ERROR_CODE;
    }

    if (pipe(fd2)) {
        close(fd[0]);
        close(fd[1]);
        return ERROR_CODE;
    }

    FILE *cmd_write_pipe = fdopen(fd[1], "wt");
    fprintf(cmd_write_pipe, "%s", input);
    fclose(cmd_write_pipe);

    pid_t pid;
    if (!(pid = fork())) {
        dup2(fd[0], STDIN_FILENO);
        dup2(fd2[1], STDOUT_FILENO);

        close(fd[0]);
        close(fd2[1]);

        close(fd2[0]);

        execlp(cmd, cmd, NULL);
        _exit(ERROR_CODE);
    }

    close(fd[0]);
    close(fd2[1]);

    FILE *read_pipe = fdopen(fd2[0], "rt");

    size_t n;
    size_t idx = 0;
    while ((n = fread(outbuf + idx, sizeof(*outbuf), CHUNK_SIZE, read_pipe))) {
        idx += n;
    }
    outbuf[idx] = '\0';
    fclose(read_pipe);

    int status;
    waitpid(pid, &status, 0);
    return WIFEXITED(status) ? WEXITSTATUS(status) : ERROR_CODE;
}

#ifdef TESTING
#include <stdio.h>
#include <assert.h>
int main() {
    char outbuf[64] = {};
    int x = interact("tac", "abc\ndef\n", outbuf);
    printf("Interact finished with %d\n_____________", x);
    printf("%s", outbuf);
    assert(x == 0);
    assert(!strcmp(outbuf, "def\nabc\n"));
}
#endif