#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <stdbool.h>

enum ArrayConstants {
    GROW_FACTOR = 2,
    INITIAL_CAPACITY = 8
};

typedef struct Array {
    char **buffer;
    size_t size;
    size_t capacity;
} Array;

bool init_array(Array *arr) {
    if (!(arr->buffer = malloc(INITIAL_CAPACITY * sizeof(*arr->buffer)))) {
        return false;
    }
    arr->size = 0;
    arr->capacity = INITIAL_CAPACITY;
    return true;
}

bool push_back(Array *arr, char *value) {
    if (arr->size == arr->capacity) {
        arr->capacity *= GROW_FACTOR;

        char **new_buf = NULL;
        if (!(new_buf = realloc(arr->buffer, arr->capacity * sizeof(*arr->buffer)))) {
            free(arr->buffer);
            return false;
        }

        arr->buffer = new_buf;
    }
    arr->buffer[arr->size++] = value;
    return true;
}

void print_array(Array *arr) {
    printf("[");
    for (int i = 0; i < arr->size; ++i) {
        printf("\"%s\"", arr->buffer[i]);
        if (i + 1 < arr->size)
            printf(",");
    }
    printf("]\n");
}

int run_env(const char* cmd) {
    enum { EXIT_CODE = 127 };
    pid_t pid = fork();

    if (pid < 0) {
        return -1;
    } else if (!pid) {
        char *cmd_mut = strdup(cmd);
        Array args;
        init_array(&args);
        clearenv();

        while (*cmd_mut) {
            while (isspace(*cmd_mut)) {
                ++cmd_mut;
            }

            bool is_var = false;
            char *from = cmd_mut;
            while (*cmd_mut && !isspace(*cmd_mut)) {
                if (*cmd_mut == '=' && args.size == 0) {
                    is_var = true;
                }
                ++cmd_mut;
            }

            if (isspace(*cmd_mut)) {
                char *to = cmd_mut++;
                *to = '\0';
            }

            if (is_var) {
                putenv(from);
            } else {
                push_back(&args, from);
            }

            is_var = false;
            //printf("[%s]\n", from);
        }

        if (args.size == 0) {
            _exit(EXIT_CODE);
        }

        push_back(&args, NULL);

        //print_array(&args);

        if (!fork()) {
            execv(args.buffer[0], args.buffer);
            //execve(args.buffer[0], args.buffer, envs.buffer);
            _exit(EXIT_CODE);
        }

        free(args.buffer);

        int status = 0;
        wait(&status);

        if (WIFEXITED(status)) {
            _exit(WEXITSTATUS(status));
        }

        _exit(EXIT_CODE);
    }

    int status;
    wait(&status);

    if (WIFEXITED(status)) {
        return WEXITSTATUS(status);
    }

    return -1;
}

#ifdef TESTING
int main(int argc, char **argv) {
    //printf("%d\n", run_env("="));
    printf("%d\n", run_env("FOO=Hello BAR=Hello=world /home/kkorolev/caos/caos/cmake-build-debug/ku05-2test 1 2 3 4="));
}
#endif