#include <pthread.h>
#include <stdlib.h>
#include <memory.h>

typedef void (*BinaryOp)(void* state, void* el);

typedef struct {
    const void *begin;
    size_t num_elements;
    size_t size;
    void *init_state;
    void *state;
    BinaryOp op;
} Task;

#ifdef TESTING
#include <stdio.h>
#endif

void *create_state(size_t size, void* init_state) {
    void *state = calloc(1, size);
    memcpy(state, init_state, size);
    return state;
}

void *f(void *arg) {
    Task *task = (Task*)arg;
    void *from = (void*)task->begin;
    task->state = create_state(task->size, task->init_state);

    for (size_t i = 0; i < task->num_elements; ++i, from += task->size) {
        task->op(task->state, from);
    }

    return NULL;
}

size_t min(size_t x, size_t y) {
    return x < y ? x : y;
}

void init_task(Task *task, const void *begin, size_t num_elements, size_t size, void *init_state, BinaryOp op) {
    task->begin = begin;
    task->num_elements = num_elements;
    task->size = size;
    task->init_state = init_state;
    task->op = op;
}

void std_accumulate(void* result, const void* begin, size_t size, size_t n, void* init_state, void (*binary_op)(void* state, void* el)) {
    enum {MAX_NUM_THREADS = 4, TASKS_PER_THREAD = 5};
    size_t num_threads = min(n / TASKS_PER_THREAD + 1, MAX_NUM_THREADS);
    size_t batch_size = n / num_threads;

    pthread_t *threads = calloc(num_threads - 1, sizeof(*threads));

    size_t num_tasks = num_threads;
    Task *tasks = calloc(num_tasks, sizeof(*tasks));
    size_t total = 0;
    const void *from = begin;

    for (size_t i = 0; i < num_threads - 1; ++i) {
        init_task(&tasks[i], from, batch_size, size, init_state, binary_op);
        pthread_create(&threads[i], NULL, f, &tasks[i]);
        total += batch_size;
        from += batch_size * size;
    }

    if (n > total) {
        init_task(&tasks[num_tasks - 1], from, n - total, size, init_state, binary_op);
        f(&tasks[num_tasks - 1]);
    }

    for (int i = 0; i < num_threads - 1; ++i) {
        pthread_join(threads[i], NULL);
    }

    void *state = create_state(size, init_state);

    for (size_t i = 0; i < num_tasks; ++i) {
        binary_op(state, tasks[i].state);
        free(tasks[i].state);
    }

    memcpy(result, state, size);
    free(state);

    free(tasks);
    free(threads);
}

#ifdef TESTING
#define N 5
void bin_op(void *state, void *el) {
    int *s = (int*)state;
    *s += *(int*)el;
}
#include <stdio.h>
int main() {
    int result;
    int nums[N] = {1, 2, 3, 4, 5};
    int init = 0;
    std_accumulate(&result, nums, sizeof(int), N, &init, &bin_op);
    printf("%d", result);
}
#endif