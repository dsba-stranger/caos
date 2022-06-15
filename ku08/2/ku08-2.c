#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void fun0(int no);
void fun1(void);
int get_result_state(void);

enum { TASKS_PER_THREAD = 5 };

#ifdef TESTING

void fun0(int no) {
    printf("fun0 %d\n", no);
}

void fun1() {
    printf("fun1\n");
}

int get_result_state() {
    return 42;
}
#endif

void *f(void *arg) {
    int k = *(int*)arg;
    char *line = NULL;
    size_t line_size;

    for (int i = 0; i < k; ++i) {
        if (getline(&line, &line_size, stdin) < 0) {
            return NULL;
        }

        if (line[0] == '0') {
            fun0(atoi(line + 2));
        } else {
            fun1();
        }
    }

    free(line);
    return NULL;
}

int min(int x, int y) {
    return x < y ? x : y;
}

int main(int argc, char **argv) {
    // freopen("input.txt", "r", stdin);

    if (argc < 2) {
        return 1;
    }

    int max_num_threads = atoi(argv[1]);

    int k;
    if (scanf("%d\n", &k) != 1) {
        return 1;
    }

    int num_threads = min(k / TASKS_PER_THREAD + 1, max_num_threads);
    int batch_size = num_threads != max_num_threads ? TASKS_PER_THREAD : k / num_threads;
    pthread_t *threads = calloc(num_threads - 1, sizeof(*threads));
    int total = 0;

    for (int i = 0; i < num_threads - 1; ++i) {
        pthread_create(&threads[i], NULL, f, &batch_size);
        total += batch_size;
    }

    int size = k - total;
    f(&size);

    for (int i = 0; i < num_threads - 1; ++i) {
        pthread_join(threads[i], NULL);
    }

    printf("%d\n", get_result_state());
    free(threads);
}