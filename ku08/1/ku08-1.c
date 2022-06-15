#include <stdbool.h>
#include <stdlib.h>
#include <stdatomic.h>

bool activate_service(size_t user_id, void* service);

bool place_order(_Atomic unsigned accounts[], size_t user_id, unsigned price, void* service) {
    unsigned old_balance;
    while ((old_balance = accounts[user_id])) {
        if (old_balance < price) {
            return false;
        }
        unsigned new_balance = old_balance - price;
        if (atomic_compare_exchange_weak(&accounts[user_id], &old_balance, new_balance)) {
            if (activate_service(user_id, service)) {
                return true;
            }
            atomic_fetch_add(&accounts[user_id], price);
            return false;
        }
    }
    return false;
}

#ifdef TESTING
#include <stdio.h>
#include <pthread.h>

bool activate_service(size_t user_id, void* service) {
    //printf("service activated\n");
}

#define N 10
#define NTHREADS 10

_Atomic unsigned int accounts[N] = {10000, 200, 150, 50, 30, 100, 20, 100, 20, 0};

void *f(void *arg) {
    int tid = *(int*)arg;
    for (int i = 0; i < 1; ++i) {
        printf("tid=%d, status=%d, balance=%u\n", tid, place_order(accounts, 0, 100, NULL), accounts[0]);
    }
}

int main() {
    pthread_t tids[NTHREADS];
    int ids[NTHREADS];

    for (int i = 0; i < NTHREADS; ++i) {
        ids[i] = i;
        pthread_create(&tids[i], NULL, f, &ids[i]);
    }
    for (int i = 0; i < NTHREADS; ++i) {
        pthread_join(tids[i], NULL);
    }
}
#endif