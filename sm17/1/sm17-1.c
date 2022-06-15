#include <pthread.h>
#include <stdio.h>

enum {
    NUM_THREADS = 10
};

pthread_t threads[NUM_THREADS];
int ids[NUM_THREADS] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

void *f(void *data) {
    int tnum = *(int*)data;

    if (tnum > 0) {
        pthread_join(threads[tnum - 1], NULL);
    }

    printf("%d\n", tnum);
    return NULL;
}

int main() {
    for (int i = 0; i < NUM_THREADS; ++i) {
        pthread_create(&threads[i], NULL, &f, &ids[i]);
    }

    pthread_join(threads[NUM_THREADS - 1], NULL);
}