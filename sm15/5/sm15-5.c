#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <inttypes.h>
#include <sys/signal.h>
#include <sys/wait.h>

enum {
    BACKLOG = 5,
    ERROR_ANS = -1
};

void handler(int signum) {
    killpg(0, SIGTERM);
    while (wait(NULL) != -1) {}
    _exit(0);
}

void pipe_handler(int signum) {
    _exit(0);
}

int main(int argc, char **argv) {
    if (argc < 3) {
        return 1;
    }

    struct sigaction sigact_term = {.sa_handler = handler, .sa_flags = SA_RESTART};
    sigaction(SIGTERM, &sigact_term, NULL);

    struct sigaction sigact_pipe = {.sa_handler = pipe_handler, .sa_flags = SA_RESTART};
    sigaction(SIGPIPE, &sigact_pipe, NULL);

    struct sigaction sigact_children = {.sa_handler = SIG_DFL, .sa_flags = SA_NOCLDWAIT};
    sigaction(SIGCHLD, &sigact_children, NULL);

    uint16_t port = strtol(argv[1], NULL, 10);

    int fd = socket(PF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("socket");
        return 1;
    }

    int val = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));
    setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &val, sizeof(val));

    struct sockaddr_in sa = {};
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = INADDR_ANY;
    sa.sin_port = htons(port);
    if (bind(fd, (struct sockaddr *) &sa, sizeof(sa)) < 0) {
        perror("bind");
        close(fd);
        return 1;
    }

    if (listen(fd, BACKLOG) < 0) {
        perror("listen");
        close(fd);
        return 1;
    }

    char *key = argv[2];
    int32_t serial = 0;
    setpgid(0, 0);

    while (1) {
        struct sockaddr_in ss;
        socklen_t sz = sizeof(ss);
        int afd = accept(fd, (struct sockaddr*)&ss, &sz);

        if (afd < 0) {
            perror("accept");
            close(fd);
            return 1;
        }

        ++serial;

        if (!fork()) {
            close(fd);

            int afd2 = dup(afd);
            FILE *rf = fdopen(afd, "r");
            FILE *wf = fdopen(afd2, "w");

            if (fprintf(wf, "%s\r\n", key) < 0 || fflush(wf) < 0) {
                perror("fprintf key");
                goto finish;
            }

            if (fprintf(wf, "%" PRId32 "\r\n", serial) < 0 || fflush(wf) < 0) {
                perror("fprintf serial");
                goto finish;
            }

            int32_t max;
            if (fscanf(rf, "%" PRId32, &max) != 1) {
                perror("fscanf max");
                goto finish;
            }

            if (fprintf(wf, "%" PRId32 "\r\n", max) < 0 || fflush(wf) < 0) {
                perror("fprintf max");
                goto finish;
            }

            int32_t num;
            int res;

            while ((res = fscanf(rf, "%" PRId32, &num)) != EOF) {
                if (res != 1) {
                    perror("scanf num");
                    goto finish;
                }
                int32_t sum;
                if (num > max || __builtin_add_overflow(num, serial, &sum)) {
                    if (fprintf(wf, "%" PRId32 "\r\n", ERROR_ANS) < 0 || fflush(wf) < 0) {
                        perror("fprintf err");
                    }
                    goto finish;
                }

                if (fprintf(wf, "%" PRId32 "\r\n", sum) < 0 || fflush(wf) < 0) {
                    perror("fprintf num + serial");
                    goto finish;
                }
            }

finish:
            fclose(rf);
            fclose(wf);
            _exit(0);
        }

        close(afd);
    }

}