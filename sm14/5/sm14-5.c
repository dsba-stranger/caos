#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/signalfd.h>
#include <sys/wait.h>

enum {
    PID_NONE = -1
};

void ping_pong(int proc_num, pid_t brother_pid, int pipe_fds[2], int signal_fd, int n) {
    FILE *read_pipe = fdopen(pipe_fds[0], "rt");
    FILE *write_pipe = fdopen(pipe_fds[1], "wt");
    struct signalfd_siginfo si = {};

    pid_t this_pid = getpid();

    if (brother_pid == PID_NONE) {
        if (fscanf(read_pipe, "%d", &brother_pid) != 1) {
            return;
        }

        fprintf(write_pipe, "%d\n", proc_num);
        fflush(write_pipe);

        kill(getpid(), SIGUSR1);
    } else {
        fprintf(write_pipe, "%d\n", this_pid);
        fflush(write_pipe);
    }

    int x = 0;

    while (x < n && read(signal_fd, &si, sizeof(si)) > 0) {
        if (fscanf(read_pipe, "%d", &x) != 1) {
            kill(brother_pid, SIGKILL);
            return;
        }

        if (x == n + 1) {
            return;
        }

        printf("%d %d\n", proc_num, x);
        fflush(stdout);

        fprintf(write_pipe, "%d\n", ++x);
        fflush(write_pipe);
        kill(brother_pid, SIGUSR1);
    }
}

int main(int argc, char **argv) {
    if (argc < 2) {
        return 1;
    }

    int n = strtol(argv[1], NULL, 10);

    int pipe_fds[2];

    if (pipe(pipe_fds)) {
        perror("pipe");
        return 1;
    }

    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);

    sigprocmask(SIG_BLOCK, &mask, NULL);
    int signal_fd;

    if ((signal_fd = signalfd(-1, &mask, 0)) < 0) {
        perror("signalfd");
        close(pipe_fds[0]);
        close(pipe_fds[1]);
        return 1;
    }

    pid_t pid;

    if ((pid = fork()) == 0) {
        ping_pong(1, PID_NONE, pipe_fds, signal_fd, n);
        _exit(0);
    }

    if (fork() == 0) {
        ping_pong(2, pid, pipe_fds, signal_fd, n);
        _exit(0);
    }

    close(pipe_fds[0]);
    close(pipe_fds[1]);
    close(signal_fd);

    wait(NULL);
    wait(NULL);
}