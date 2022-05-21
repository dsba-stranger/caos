#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>

enum { FD_NONE = -1 };

void kill_all(pid_t *pids, size_t to_index) {
    for (size_t i = 0; i < to_index; ++i) {
        kill(pids[i], SIGKILL);
    }
}

void wait_all() {
    while (wait(NULL) > 0) {}
}

void kill_and_exit(int fds[2], pid_t *pids, size_t to_index) {
    close(fds[0]);
    close(fds[1]);

    if (to_index > 0) {
        kill_all(pids, to_index);
        wait_all();
    }
    exit(1);
}

void create_process_chain(char **argv, size_t num_procs) {
    pid_t *pids = calloc(num_procs, sizeof(*pids));

    if (!pids) {
        exit(1);
    }

    int fds[2] = {FD_NONE, FD_NONE};
    int prev_read_fd = FD_NONE;

    for (size_t proc_index = 0; proc_index < num_procs; ++proc_index) {
        if (pipe(fds) < 0) {
            kill_and_exit(fds, pids, proc_index);
        }
#ifdef TESTING
        printf("(%lu) pipe %d %d\n", proc_index, fds[0], fds[1]);
#endif
        pid_t pid = fork();
        pids[proc_index] = pid;

        if (pid < 0) {
            kill_and_exit(fds, pids, proc_index);
        } else if (!pid) {
#ifdef TESTING
            printf("(%lu) Going to run %s\n", proc_index, argv[proc_index]);
#endif
            close(fds[0]);

            if (proc_index > 0) {
#ifdef TESTING
                printf("(%lu) Reading from %d\n", proc_index, prev_read_fd);
#endif
                dup2(prev_read_fd, STDIN_FILENO);
                close(prev_read_fd);
            }

            if (proc_index + 1 != num_procs && num_procs != 1) {
#ifdef TESTING
                printf("(%lu) Writing to %d\n", proc_index, fds[1]);
#endif
                dup2(fds[1], STDOUT_FILENO);
            }

            close(fds[1]);
            execlp(argv[proc_index], argv[proc_index], NULL);
            _exit(1);
        }

        if (prev_read_fd != FD_NONE) {
            close(prev_read_fd);
        }

        close(fds[1]);
        prev_read_fd = fds[0];

#ifdef TESTING
        printf("[%d is started]\n", pid);
#endif
    }

    close(fds[0]);
    wait_all();
    free(pids);
}

int main(int argc, char **argv) {
    if (argc == 1) {
        return 0;
    }

    create_process_chain(argv + 1, argc - 1);
}