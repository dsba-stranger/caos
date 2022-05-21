#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/wait.h>

enum CommonConstants {
    SIGNAL_SHIFT_CODE = 1024
};

char *next_token(const char **cmd) {
    while (isspace(**cmd)) {
        ++*cmd;
    }

    char *from = *cmd;

    while (**cmd && !isspace(**cmd)) {
        ++(*cmd);
    }

    return from;
}

size_t count_tokens(const char *cmd) {
    size_t num_tokens = 0;

    while (*cmd) {
        char *from = next_token(&cmd);

        if (from != cmd) {
            ++num_tokens;
        }
    }

    return num_tokens;
}

char *read_token(char **cmd) {
    char *from = next_token((const char**)cmd);

    if (isspace(**cmd)) {
        char *to = (*cmd)++;
        *to = '\0';
    }

    return from;
}

int mysystem(const char *cmd) {
    if (!*cmd) {
        return -1;
    }

    size_t num_tokens = count_tokens(cmd);

    if (num_tokens == 0) {
        return -1;
    }

    pid_t pid = fork();

    if (pid < 0) {
        return -1;
    } else if (!pid) {
        char *token;
        char **tokens = calloc(num_tokens + 1, sizeof(token));

        if (!tokens) {
            _exit(-1);
        }

        char *cmd_mut = strdup(cmd);

        if (!cmd_mut) {
            _exit(-1);
        }

        size_t i = 0;
        while (*(token = read_token(&cmd_mut))) {
            tokens[i++] = token;
        }

        tokens[num_tokens] = NULL;

        execvp(tokens[0], tokens);
        _exit(1);
    }

    int status = 0;
    waitpid(pid, &status, 0);

    if (WIFSIGNALED(status)) {
        return SIGNAL_SHIFT_CODE + WTERMSIG(status);
    } else {
        return WEXITSTATUS(status);
    }
}

#ifdef TESTING
int main() {
    printf("%d\n", mysystem("ls -la"));
    printf("%d\n", mysystem(""));
    printf("%d\n", mysystem("ls ."));
    printf("%d\n", mysystem("               ls               .                "));
    printf("%d\n", mysystem("echo I love caos"));
    printf("%d\n", mysystem("cho I love caos"));
    printf("%d\n", mysystem("                 "));
}
#endif