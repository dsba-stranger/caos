#include <pthread.h>
#include <stdio.h>
#include <inttypes.h>

int is_prime(uint64_t x) {
    if (x == 2) {
        return 1;
    }

    if (x <= 1 || x % 2 == 0) {
        return 0;
    }

    for (uint64_t i = 3; i * i <= x; i += 2) {
        if (x % i == 0) {
            return 0;
        }
    }

    return 1;
}

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condvar = PTHREAD_COND_INITIALIZER;
uint64_t cur_prime = 0;

void *find_primes(void *arg) {
    uint64_t base = *(uint64_t*)arg;

    for (;;++base) {
        if (is_prime(base)) {
            pthread_mutex_lock(&mutex);
            cur_prime = base;
            pthread_cond_signal(&condvar);
            pthread_mutex_unlock(&mutex);
        }
    }

    return NULL;
}

int main() {
    uint64_t base;
    int32_t count;
    if (scanf("%" PRIu64  " %" PRId32, &base, &count) != 2) {
        return 1;
    }

    pthread_t tid;
    pthread_create(&tid, NULL, find_primes, &base);

    while (count--) {
        uint64_t x = 0;
        pthread_mutex_lock(&mutex);

        while (!cur_prime) {
            pthread_cond_wait(&condvar, &mutex);
        }

        x = cur_prime;
        cur_prime = 0;
        pthread_mutex_unlock(&mutex);

        printf("%" PRIu64 "\n", x);
    }
}