#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/signal.h>

enum {
    BUF_SIZE = 65507
};

void handler(int signum) {
    _exit(0);
}

int main(int argc, char **argv) {
    if (argc < 2) {
        return 1;
    }

    struct sigaction sa = {.sa_flags = SA_RESTART, .sa_handler = handler};
    sigaction(SIGTERM, &sa, NULL);

    uint16_t port = strtol(argv[1], NULL, 10);

    int fd;
    if ((fd = socket(AF_INET6, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
        return 1;
    }

    struct sockaddr_in6 addr = {.sin6_family = AF_INET6, .sin6_port = htons(port), .sin6_addr = in6addr_any};

    if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(fd);
        return 1;
    }

    struct sockaddr_in6 client_addr = {};
    socklen_t client_addr_size = sizeof(client_addr);
    ssize_t bytes_read;

    char buf[BUF_SIZE] = {};
    while (1) {
        if ((bytes_read = recvfrom(fd, buf, sizeof(buf), 0, (struct sockaddr*)&client_addr, &client_addr_size)) < 0) {
            perror("recvfrom");
            close(fd);
            return 1;
        }
        buf[bytes_read] = '\0';

        char ip_str[INET6_ADDRSTRLEN] = {};
        printf("[%s:%hu] %s\n",
               inet_ntop(AF_INET6, &client_addr.sin6_addr, ip_str, sizeof(ip_str)),
               ntohs(client_addr.sin6_port),
               buf);
        fflush(stdout);
    }

    close(fd);
}