#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>

void ping_pong(int proc_num, int read_fd, int write_fd, int n) {
    FILE *read_pipe = fdopen(read_fd, "rt");
    FILE *write_pipe = fdopen(write_fd, "wt");

    int x;
    while (fscanf(read_pipe, "%d", &x) == 1) {
        if (x == n) {
            return;
        }
        printf("%d %d\n", proc_num, x);
        fflush(stdout);
        fprintf(write_pipe, "%d\n", x + 1);
        fflush(write_pipe);
    }

    fclose(read_pipe);
    fclose(write_pipe);
}

int main(int argc, char **argv) {
    if (argc < 2) {
        return -1;
    }
    int n = strtol(argv[1], NULL, 10);

    int fds1[2], fds2[2];

    if (pipe(fds1)) {
        perror("pipe1");
        return 1;
    }

    if (pipe(fds2)) {
        perror("pipe2");
        return 1;
    }

    if (fork() == 0) {
        close(fds1[1]);
        close(fds2[0]);
        ping_pong(1, fds1[0], fds2[1], n);
        _exit(0);
    }

    close(fds2[1]);
    close(fds1[0]);

    if (fork() == 0) {
        ping_pong(2, fds2[0], fds1[1], n);
        _exit(0);
    }

    FILE *write_pipe = fdopen(fds1[1], "wt");
    fprintf(write_pipe, "%d\n", 1);
    fflush(write_pipe);

    close(fds1[1]);
    close(fds2[0]);

    wait(NULL);
    wait(NULL);
    printf("Done\n");
}