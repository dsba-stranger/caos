#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/signal.h>

void handler(int signum) {
    _exit(0);
}

int main(int argc, char **argv) {
    if (argc < 4) {
        return 1;
    }

    struct sigaction sa = {.sa_handler = handler, .sa_flags = SA_RESTART};
    sigaction(SIGPIPE, &sa, NULL);

    char *host = argv[1];
    char *service = argv[2];
    char *key = argv[3];

    struct addrinfo hints = {.ai_family = AF_INET, .ai_socktype = SOCK_STREAM};
    struct addrinfo *addrs = NULL;

    int gai_err;
    if ((gai_err = getaddrinfo(host, service, &hints, &addrs))) {
        printf("%s\n", gai_strerror(gai_err));
        return 1;
    }

    int fd;
    if ((fd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        freeaddrinfo(addrs);
        return 1;
    }

    if (connect(fd, addrs->ai_addr, sizeof(struct sockaddr_in)) < 0) {
        perror("connect");
        close(fd);
        freeaddrinfo(addrs);
        return 1;
    }
    freeaddrinfo(addrs);

    int fd2 = dup(fd);
    FILE *rf = fdopen(fd, "r");
    FILE *wf = fdopen(fd2, "w");

    if (fprintf(wf, "%s\n", key) < 0 || fflush(wf) < 0) {
        perror("printf key");
        goto finish;
    }

    unsigned long long k;
    if (fscanf(rf, "%llu", &k) != 1 || fflush(rf) < 0) {
        perror("scanf k");
        goto finish;
    }

    for (unsigned long long i = 0; i <= k; ++i) {
        if (fprintf(wf, "%llu\n", i) < 0 || fflush(wf) < 0) {
            perror("printf i");
            goto finish;
        }
    }

    if (fscanf(rf, "%llu", &k) != 1 || fflush(rf) < 0) {
        perror("scanf last");
        goto finish;
    }

    printf("%llu\n", k);
finish:
    fclose(rf);
    fclose(wf);
}