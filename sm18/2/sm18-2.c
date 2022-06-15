#include <stdlib.h>
#include <stdint.h>
#include <stdatomic.h>

#ifdef TESTING
#include <unistd.h>
#include <sys/syscall.h>
#include <linux/futex.h>

void futex_wait(int *addr, int val) {
    // atomically: block on addr if (*addr == val)
    syscall(SYS_futex, addr, FUTEX_WAIT, val, NULL, NULL, 0);
}

void futex_wake(int *addr, int num) {
    // wake up to num threads blocked on addr
    syscall(SYS_futex, addr, FUTEX_WAKE, num, NULL, NULL, 0);
}
#endif

typedef struct {
    _Atomic uint32_t lock;
    _Atomic uint32_t waiters;
} caos_mutex_t;

void caos_mutex_init(caos_mutex_t *m) {
    m->lock = 0;
    m->waiters = 0;
}

uint32_t cmpxchg(_Atomic uint32_t *var, uint32_t expected, uint32_t desired) {
    uint32_t *expected_ptr = &expected;
    atomic_compare_exchange_strong(var, expected_ptr, desired);
    return *expected_ptr;
}

void caos_mutex_lock(caos_mutex_t *m) {
    while (cmpxchg(&m->lock, 0, 1)) {
        ++m->waiters;
        futex_wait((int*)&m->lock, 1);
        --m->waiters;
    }
}

void caos_mutex_unlock(caos_mutex_t *m) {
    m->lock = 0;
    if (m->waiters > 0) {
        futex_wake((int*)&m->lock, 1);
    }
}

#ifdef TESTING
#include <pthread.h>
#include <stdio.h>

long long sum = 0;
caos_mutex_t mutex;

void *f(void *arg) {
    for (int i = 0; i < 1000000; ++i) {
        caos_mutex_lock(&mutex);
        ++sum;
        caos_mutex_unlock(&mutex);
    }
}

enum { N = 10 };

int main() {
    caos_mutex_init(&mutex);
    pthread_t threads[N];
    for (int i = 0; i < N; ++i) {
        pthread_create(&threads[i], NULL, f, NULL);
    }
    for (int i = 0; i < N; ++i) {
        pthread_join(threads[i], NULL);
    }
    printf("%lld\n", sum);
}

#endif