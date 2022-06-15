#include <stdio.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdlib.h>
#include <zconf.h>

enum {
    NUM_THREADS = 100,
    NUM_ITERS = 1000
};

typedef struct Item {
    struct Item *next;
    long long value;
} Item;

_Atomic (Item*) head;

void list_add(int value) {
    Item *item = calloc(1, sizeof(*item));
    item->value = value;
    atomic_exchange(&head, item)->next = item;
    sched_yield();
}

void *f(void *arg) {
    int id = *(int*)arg;

    for (int i = 0; i < NUM_ITERS; ++i) {
        list_add(1000 * id + i);
    }

    return NULL;
}

int main() {
    pthread_t tids[NUM_THREADS];
    int ids[NUM_THREADS];

    Item root = {0, 0};
    head = &root;

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setstacksize(&attr, PTHREAD_STACK_MIN);

    for (int i = 0; i < NUM_THREADS; ++i) {
        ids[i] = i;
        pthread_create(&tids[i], &attr, f, &ids[i]);
    }

    pthread_attr_destroy(&attr);

    for (int i = 0; i < NUM_THREADS; ++i) {
        pthread_join(tids[i], NULL);
    }

    for (Item *cur = root.next, *next = NULL; cur; cur = next) {
        printf("%lld\n", cur->value);
        next = cur->next;
        free(cur);
    }
}