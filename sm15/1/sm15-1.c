#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>

enum {
    MAX_LINE_SIZE = 1024
};

void parse_host_service(char *str, char **host, char **service) {
    *host = str;
    while (*str && *str++ != ' ');
    *(str - 1) = '\0';
    *service = str;
    while (*str && *str++ != '\n');
    if (*(str - 1) == '\n') {
        *(str - 1) = '\0';
    }
}

void print_addr(struct addrinfo *info) {
    uint32_t min_ip_value = -1;
    struct sockaddr_in *min_inet = NULL;

    for (struct addrinfo *ai = info; ai; ai = ai->ai_next) {
        if (ai->ai_family == AF_INET) {
            struct sockaddr_in *inet = (struct sockaddr_in*)ai->ai_addr;
            uint32_t ip_value = ntohl(inet->sin_addr.s_addr);
            if (!min_inet || ip_value < min_ip_value) {
                min_ip_value = ip_value;
                min_inet = inet;
            }
        }
    }

    if (min_inet) {
        printf("%s:%hu\n", inet_ntoa(min_inet->sin_addr), ntohs(min_inet->sin_port));
    }
}

int main(int argc, char **argv) {
    char host[MAX_LINE_SIZE] = {};
    char service[MAX_LINE_SIZE] = {};

    while (scanf("%1000s %1000s", host, service) != EOF) {
        struct addrinfo hints = {};
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;

        struct addrinfo *addr = NULL;
        int gai_err;
        if ((gai_err = getaddrinfo(host, service, &hints, &addr))) {
            printf("%s\n", gai_strerror(gai_err));
            continue;
        }

        print_addr(addr);
        freeaddrinfo(addr);
    }

}