#include <setjmp.h>
#include <stdlib.h>

enum {
    MAX_RECURSION_DEPTH = 1024
};

typedef struct {
    jmp_buf env;
    int id;
} handler;

typedef struct {
    handler handlers[MAX_RECURSION_DEPTH];
    size_t top;
} exception_state;

exception_state state;

jmp_buf* try(int exception) {
    state.handlers[state.top].id = exception;
    return &state.handlers[state.top++].env;
}

void endtry() {
    --state.top;
}

void throw(int exception) {
    while (state.top) {
        handler h = state.handlers[--state.top];
        if (h.id == exception) {
            jmp_buf *env = &h.env;
            longjmp(*env, 1);
        }
    }
    if (!state.top) {
        exit(13);
    }
}

#ifdef TESTING
#include <stdio.h>
int main() {
    if (!setjmp(*try(42))) { // добавили обработчик
        puts("level 1");
        if (!setjmp(*try(1))) { // и ещё один
            puts("level 2");
            //throw(1); // выбросили исключение
            puts("this string is not displayed");
            endtry();
        } else {
            puts("handle exception 1");
        }
        throw(42);
        endtry();
    } else {
        puts("handle exception 42");
    }
}
#endif