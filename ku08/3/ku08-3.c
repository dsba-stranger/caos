#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

void some_hash(const char* data, size_t n, char* out);
size_t get_hash_size();

#ifdef TESTING
size_t get_hash_size() {
    return 10;
}

void some_hash(const char* data, size_t n, char* out) {
    for (size_t i = 0; i < get_hash_size() - 1; ++i) {
        out[i] = data[0];
    }
    out[get_hash_size() - 1] = '\0';
}
#endif

typedef struct {
    size_t block_size;
    const char *data;
    char *out;
} Task;

#define min(a, b) ((a) < (b) ? (a) : (b))

void *f(void *arg) {
    Task *task = (Task*)arg;
    task->out = calloc(1, get_hash_size());
    some_hash(task->data, task->block_size, task->out);
    return NULL;
}

int main(int argc, char **argv) {
    if (argc < 4) {
        return 1;
    }

    long seed = strtol(argv[1], NULL, 10);
    size_t msg_len = strtol(argv[2], NULL, 10);
    size_t block_size = strtol(argv[3], NULL, 10);

    if (msg_len == 0 || block_size == 0) {
        return 0;
    }

    enum { NUM_THREADS = 4 };

    pthread_t *threads = calloc(NUM_THREADS, sizeof(*threads));
    Task *tasks = calloc(NUM_THREADS, sizeof(*tasks));

    srand(seed);
    char *data = calloc(NUM_THREADS * block_size, sizeof(*data));

    if (!data) {
        return 1;
    }

    int id = 1;

    for (size_t iter_len = 0; iter_len < msg_len; iter_len += NUM_THREADS * block_size) {
        size_t len = min(NUM_THREADS * block_size, msg_len - iter_len);

        for (size_t i = 0; i < len; ++i) {
            data[i] = (char)rand();
        }

        const char *data_ptr = data;
        size_t iter_num_threads = len / block_size + (len % block_size > 0);
        size_t total = 0;
        for (size_t i = 0; i < iter_num_threads; ++i) {
            Task *t = &tasks[i];
            t->data = data_ptr;
            t->block_size = min(block_size, len - total);
            pthread_create(&threads[i], NULL, f, &tasks[i]);
            data_ptr += t->block_size;
            total += t->block_size;
        }

        for (size_t i = 0; i < iter_num_threads; ++i) {
            pthread_join(threads[i], NULL);
        }

        for (size_t i = 0; i < iter_num_threads; ++i, ++id) {
            printf("[%d]\t%s\n", id, tasks[i].out);
            free(tasks[i].out);
        }
    }

    free(data);
    free(tasks);
    free(threads);
}