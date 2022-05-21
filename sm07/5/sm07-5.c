#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#ifdef TESTING
#include <stdio.h>
#endif

typedef void (*finalizer_t)(void *ptr, size_t size);

typedef struct Allocation {
    void *ptr; // указатель на область памяти
    size_t size; // размер
    finalizer_t finalizer; // функция, которую следует вызвать перед free
    bool alive; // достижимость
    struct Allocation *prev; // предыдущая аллокация
    struct Allocation *next; // следующая аллокация
} Allocation;

typedef struct GC {
    uintptr_t bottom; // дно стека
    Allocation *head; // голова списка аллокаций
} GC;

static GC GLOBAL_GC = {0, NULL};

static bool points_to(void *ptr, Allocation *alloc) {
    uintptr_t uptr = (uintptr_t)ptr, aptr = (uintptr_t)alloc->ptr;
    return (uptr >= aptr) && (uptr - aptr <= alloc->size);
}

static void push_allocation(Allocation *alloc) {
    alloc->next = GLOBAL_GC.head;
    if (GLOBAL_GC.head) {
        GLOBAL_GC.head->prev = alloc;
    }
    GLOBAL_GC.head = alloc;
}

// prev используется для удаления из середины списка
static Allocation *remove_allocation(Allocation *alloc) {
    if (alloc->prev) {
        alloc->prev->next = alloc->next;
    }

    if (alloc->next) {
        alloc->next->prev = alloc->prev;
    }

    if (alloc == GLOBAL_GC.head) {
        GLOBAL_GC.head = alloc->next;
    }

    return alloc->next;
}

static Allocation *find_allocation(void *ptr) {
    Allocation *cur = GLOBAL_GC.head;

    while (cur) {
        if (points_to(ptr, cur)) {
            return cur;
        }
        cur = cur->next;
    }

    return NULL;
}

void gc_init(char **argv) {
    GLOBAL_GC.bottom = (uintptr_t)argv;
}

void *gc_malloc(size_t size, finalizer_t finalizer) {
    Allocation *alloc = calloc(1, sizeof(*alloc));

    if (!alloc) {
        return NULL;
    }

    alloc->ptr = calloc(1, size);

    if (!alloc->ptr) {
        free(alloc);
        return NULL;
    }

    alloc->size = size;
    alloc->alive = false;
    alloc->finalizer = finalizer;

    push_allocation(alloc);

    return alloc->ptr;
}

static void mark(uintptr_t from_addr, uintptr_t to_addr) {

    for (uintptr_t addr = from_addr; addr + sizeof(void *) <= to_addr; addr += sizeof(void *)) {
        void* ptr;
        memcpy(&ptr, (void*)addr, sizeof(ptr));

        Allocation *alloc = find_allocation(ptr);

        if (alloc && !alloc->alive) {
#ifdef TESTING
            printf("FOUND ALLOCATION for %p at addr=%p\n", (void*)ptr, (void*)addr);
#endif
            alloc->alive = true;
            mark((uintptr_t)alloc->ptr, (uintptr_t)(alloc->ptr + alloc->size));
        }
    }
}

#ifdef TESTING
void print_list() {
    Allocation *cur = GLOBAL_GC.head;

    if (!cur) {
        printf("()");
    }

    while (cur) {
        printf("(%p(alive=%d))->", cur->ptr, cur->alive);
        cur = cur->next;
    }
    printf("\n");
}
#endif

void gc_collect_impl(uintptr_t stack_top) {
    for (Allocation *alloc = GLOBAL_GC.head; alloc; alloc = alloc->next) {
        alloc->alive = false;
    }
#ifdef TESTING
    print_list();
#endif
    mark(stack_top, GLOBAL_GC.bottom);
#ifdef TESTING
    print_list();
#endif

    for (Allocation *alloc = GLOBAL_GC.head; alloc;) {
        if (!alloc->alive) {
            if (alloc->finalizer) {
                alloc->finalizer(alloc->ptr, alloc->size);
            }
            free(alloc->ptr);
            Allocation *next = remove_allocation(alloc);
#ifdef TESTING
            print_list();
#endif
            free(alloc);
            alloc = next;
        } else {
            alloc = alloc->next;
        }
    }
}

#ifdef TESTING

typedef struct Foo {
    int bar;
    struct Foo* next;
} Foo;

void foo_print(Foo *foo, int bar) {
    foo->bar = bar;
    printf("[%p] with bar=%d is created\n", foo, foo->bar);
}

void foo_release(void *foo, size_t size) {
    printf("[%p] released (sz=%u)\n", foo, size);
}

void calculate() {
    Foo *foo = gc_malloc(sizeof(*foo), &foo_release);
    foo_print(foo, 2);
}

void gc_collect();

int main(int argc, char **argv) {
    gc_init(argv);
    void* buf = gc_malloc(1, NULL);

    Foo *prev = NULL;

    for (int i = 0; i < 5; ++i) {
        Foo *foo = gc_malloc(sizeof(*foo), &foo_release);
        foo_print(foo, i);
        foo->next = prev;
        prev = foo;
    }

    // calculate();

    gc_collect();
}
#endif