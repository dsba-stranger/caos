#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/signal.h>
#include <sys/un.h>
#include <sys/epoll.h>
#include <errno.h>

enum {
    BACKLOG = 5,
    MAX_EVENTS = 10,
    BUF_SIZE = 1024
};

enum HashSetConstants {
    CAPACITY = 32,
    LOAD_FACTOR = 80,
    FREE_ENTRY = -1,
    MULTIPLIER = 2
};

struct hashset_st {
    size_t capacity;
    int *items;
    size_t nitems;
};

typedef struct hashset_st *hashset_t;
void hashset_destroy(hashset_t set);
int hashset_add(hashset_t set, int value);

hashset_t hashset_create_with_capacity(size_t capacity) {
    hashset_t set;
    if (!(set = calloc(1, sizeof(*set)))) {
        return NULL;
    }
    set->capacity = capacity;
    set->items = calloc(set->capacity, sizeof(*set->items));
    if (!set->items) {
        hashset_destroy(set);
        return NULL;
    }
    memset(set->items, FREE_ENTRY, set->capacity * sizeof(*set->items));
    set->nitems = 0;
    return set;
}

hashset_t hashset_create() {
    return hashset_create_with_capacity(CAPACITY);
}

size_t hashset_num_items(hashset_t set) {
    return set->nitems;
}

void hashset_destroy(hashset_t set) {
    if (set) {
        free(set->items);
    }
    free(set);
}

// Maybe set will be used later
// Stolen from here https://stackoverflow.com/a/12996028
static size_t hash_fn(hashset_t set, unsigned int x) {
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = (x >> 16) ^ x;
    return x;
}

static int needs_rehash(hashset_t set) {
    return set->nitems >= set->capacity * (double) LOAD_FACTOR / 100;
}

static int rehash(hashset_t set) {
    hashset_t new_set;

    if (!(new_set = hashset_create_with_capacity(set->capacity * MULTIPLIER))) {
        return 0;
    }

    for (size_t i = 0; i < set->capacity; ++i) {
        if (set->items[i] != FREE_ENTRY) {
            // rehash never happens, no duplicates => no need for checking return value
            hashset_add(new_set, set->items[i]);
        }
    }

    free(set->items);
    memcpy(set, new_set, sizeof(*set));
    free(new_set);
    return 1;
}

int hashset_add(hashset_t set, int value) {
    if (needs_rehash(set)) {
        if (!rehash(set)) {
            return 0;
        }
    }

    size_t hash = hash_fn(set, value);
    size_t index = hash % set->capacity;

    while (set->items[index] != FREE_ENTRY) {
        if (set->items[index] == value) {
            return 0;
        }
        index = (index + 1) % set->capacity;
    }

    set->items[index] = value;
    ++set->nitems;
    return 1;
}

int hashset_find(hashset_t set, int value, size_t *res) {
    size_t hash = hash_fn(set, value);
    size_t index = hash % set->capacity;

    while (set->items[index] != FREE_ENTRY) {
        if (set->items[index] == value) {
            *res = index;
            return 1;
        }
        index = (index + 1) % set->capacity;
    }

    return 0;
}

int hashset_remove(hashset_t set, int value) {
    size_t index;
    if (!hashset_find(set, value, &index)) {
        return 0;
    }
    set->items[index] = FREE_ENTRY;
    --set->nitems;
    return 1;
}

int hashset_contains(hashset_t set, int value) {
    size_t index;
    return hashset_find(set, value, &index);
}

void make_reusable(int sock) {
    int val = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));
    setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, &val, sizeof(val));
}

int setup_tcp_ip6(uint16_t port) {
    int ip6_sock = socket(PF_INET6, SOCK_STREAM, 0);
    if (ip6_sock < 0) {
        perror("socket");
        return -1;
    }

    make_reusable(ip6_sock);

    struct sockaddr_in6 sa = {};
    sa.sin6_family = AF_INET6;
    sa.sin6_addr = in6addr_any;
    sa.sin6_port = htons(port);
    if (bind(ip6_sock, (struct sockaddr *) &sa, sizeof(sa)) < 0) {
        perror("bind");
        close(ip6_sock);
        return -1;
    }

    if (listen(ip6_sock, BACKLOG) < 0) {
        perror("listen");
        close(ip6_sock);
        return -1;
    }

    return ip6_sock;
}

int setup_unix(char *filename) {
    int unix_sock = socket(PF_UNIX, SOCK_STREAM, 0);
    if (unix_sock < 0) {
        perror("socket");
        return -1;
    }

    struct sockaddr_un sa = {};
    sa.sun_family = AF_UNIX;
    strcpy(sa.sun_path, filename);

    if (bind(unix_sock, (struct sockaddr *) &sa, sizeof(sa)) < 0) {
        perror("bind");
        close(unix_sock);
        return -1;
    }

    if (listen(unix_sock, BACKLOG) < 0) {
        perror("listen");
        close(unix_sock);
        return -1;
    }

    return unix_sock;
}

void handler(int signum) {
    _exit(0);
}

int main(int argc, char **argv) {
    if (argc < 3) {
        return 1;
    }

    struct sigaction sigact = {.sa_handler = handler, .sa_flags = SA_RESTART};
    sigaction(SIGTERM, &sigact, NULL);
    signal(SIGPIPE, SIG_IGN);

    uint16_t tcp_port = strtol(argv[1], NULL, 10);

    int tcp_sock;
    if ((tcp_sock = setup_tcp_ip6(tcp_port)) < 0) {
        return 1;
    }

    char *unix_socket_filename = argv[2];

    int unix_sock;
    if ((unix_sock = setup_unix(unix_socket_filename)) < 0) {
        close(tcp_sock);
        return 1;
    }

    int epoll_fd;
    if ((epoll_fd = epoll_create1(0)) < 0) {
        perror("epoll_create1");
        close(tcp_sock);
        close(unix_sock);
        return 1;
    }

    hashset_t clients = NULL;

    struct epoll_event tcp_event = {.events = EPOLLIN, .data.fd = tcp_sock};
    struct epoll_event unix_event = {.events = EPOLLIN, .data.fd = unix_sock};

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, tcp_sock, &tcp_event) < 0) {
        perror("epoll_ctl");
        goto error;
    }

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, unix_sock, &unix_event) < 0) {
        perror("epoll_ctl");
        goto error;
    }

    struct epoll_event events[MAX_EVENTS] = {};

    if (!(clients = hashset_create())) {
        perror("hashset_create");
        goto error;
    }

    for (;;) {
        int nfds;
        if ((nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1)) == -1) {
            perror("epoll_wait");
            goto error;
        }

        for (int i = 0; i < nfds; ++i) {
            if (events[i].data.fd == tcp_sock || events[i].data.fd == unix_sock) {
                int conn_sock;
                if ((conn_sock = accept(events[i].data.fd, NULL, 0)) == -1) {
                    perror("accept");
                    goto error;
                }

                struct epoll_event ev = {.events = EPOLLIN, .data.fd = conn_sock};

                if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, conn_sock, &ev) == -1) {
                    perror("epoll_ctl: conn_sock");
                    goto error;
                }

#ifdef TESTING
                printf("client %d (type=%d)\n", events[i].data.fd, events[i].data.fd == unix_sock);
                fflush(stdout);
#endif
                hashset_add(clients, conn_sock);
            } else {

                char buffer[BUF_SIZE];
                ssize_t ret = read(events[i].data.fd, buffer, sizeof(buffer) - 1);

                if (ret < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
                    continue;
                } else if (ret <= 0) {
                    close(events[i].data.fd);
                    hashset_remove(clients, events[i].data.fd);
                } else {
                    buffer[ret] = '\0';
#ifdef TESTING
                    printf("read:\n%s\n", buffer);
                    fflush(stdout);
#endif
                    for (int j = 0; j < clients->capacity; ++j) {
                        if (clients->items[j] != FREE_ENTRY && events[i].data.fd != clients->items[j]) {
                            if (write(clients->items[j], buffer, ret) < 0) {
                                close(clients->items[j]);
                                clients->items[j] = FREE_ENTRY;
                                --clients->nitems;
                                //hashset_remove(clients, clients->items[j]);
                            }
                        }
                    }
                }

            }
        }
    }

error:
    hashset_destroy(clients);
    close(tcp_sock);
    close(unix_sock);
    close(epoll_fd);
    return 1;
}