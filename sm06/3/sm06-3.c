#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

enum ArrayConstants {
    GROW_FACTOR = 2,
    INITIAL_CAPACITY = 8
};

typedef struct Array {
    char *buffer;
    size_t size;
    size_t capacity;
} Array;

bool init_array(Array *arr) {
    if (!(arr->buffer = malloc(INITIAL_CAPACITY * sizeof(*arr->buffer)))) {
        return false;
    }
    arr->size = 0;
    arr->capacity = INITIAL_CAPACITY;
    return true;
}

bool push_back(Array *arr, char value) {
    if (arr->size == arr->capacity) {
        arr->capacity *= GROW_FACTOR;

        // Привычка всегда инициализировать переменные,
        // даже если они определяются на следующей строчке
        char *new_buf = NULL;
        if (!(new_buf = realloc(arr->buffer, arr->capacity * sizeof(*arr->buffer)))) {
            free(arr->buffer);
            return false;
        }

        arr->buffer = new_buf;
    }
    arr->buffer[arr->size++] = value;
    return true;
}

char *getline2(FILE *f) {
    Array arr = {};
    int c = 0;

    while ((c = getc(f)) != EOF) {
        if (!arr.size) {
            if (!init_array(&arr)) {
                return NULL;
            }
        }

        if (!push_back(&arr, c)) {
            return NULL;
        }

        if (c == '\n') {
            break;
        }
    }

    if (arr.buffer) {
        push_back(&arr, '\0');
    }

    return arr.buffer;
}

#ifdef TESTING
int main() {
    FILE *f = fopen("input.txt", "r");
    char *data = getline2(f);
    printf("%s", data);
    free(data);
}
#endif