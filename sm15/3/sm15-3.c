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

enum {
    BACKLOG = 5
};

int main(int argc, char **argv) {
    if (argc < 2) {
        return 1;
    }

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

    int32_t res = 0;

    while (1) {
        struct sockaddr_in ss;
        socklen_t sz = sizeof(ss);
        int afd = accept(fd, (struct sockaddr*)&ss, &sz);

        if (afd < 0) {
            perror("accept");
            close(fd);
            return 1;
        }

        int32_t x;
        if (read(afd, &x, sizeof(x)) < 0) {
            perror("read");
            close(afd);
            close(fd);
            return 1;
        }
        close(afd);

        if (x == 0) {
            break;
        }

        res += (int32_t)ntohl(x);
    }

    printf("%" PRId32 "\n", res);
    close(fd);
}