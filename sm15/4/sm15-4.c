#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/wait.h>

enum {
    BACKLOG = 5
};

void handler(int signum) {
    _exit(0);
}

void child_handler(int signum) {}

int main(int argc, char **argv) {
    if (argc < 3) {
        return 1;
    }

    struct sigaction sigact = {.sa_handler = handler, .sa_flags = SA_RESTART};
    sigaction(SIGTERM, &sigact, NULL);

    struct sigaction sigact_children = {.sa_handler = child_handler, .sa_flags = SA_RESTART | SA_NOCLDWAIT};
    sigaction(SIGCHLD, &sigact_children, NULL);

    int fd = socket(PF_INET6, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("socket");
        return 1;
    }

    int val = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));
    setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &val, sizeof(val));

    uint16_t port = strtol(argv[1], NULL, 10);
    struct sockaddr_in6 sa = {};
    sa.sin6_family = AF_INET6;
    sa.sin6_addr = in6addr_any;
    sa.sin6_port = htons(port);

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

    while (1) {
        struct sockaddr_in ss;
        socklen_t sz = sizeof(ss);
        int afd = accept(fd, (struct sockaddr*)&ss, &sz);

        if (afd < 0) {
            perror("accept");
            close(fd);
            return 1;
        }

        if (!fork()) {
            close(fd);

            int afd2 = dup(afd);
            dup2(afd2, STDIN_FILENO);
            dup2(afd, STDOUT_FILENO);
            close(afd);
            close(afd2);

            int num_args = argc - 1;
            char **args = calloc(num_args, sizeof(*args));
            for (int i = 0; i < num_args - 1; ++i) {
                args[i] = argv[i + 2];
            }

            execvp(args[0], args);
            _exit(1);
        }

        close(afd);
    }

    close(fd);
}