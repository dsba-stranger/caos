#include <stdlib.h>

enum Constants {
    RG_MULT = 1103515245,
    RG_INC = 12345,
    RG_MOD = 1UL << 31
};

struct RandomGenerator;

typedef struct RandomOperations {
    int (*next)(struct RandomGenerator*);
    void (*destroy)(struct RandomGenerator*);
} RandomOperations;

typedef struct RandomGenerator {
    int cur_value;
    const RandomOperations *ops;
} RandomGenerator;

int next(RandomGenerator *g) {
    g->cur_value = ((long long)g->cur_value * RG_MULT + RG_INC) % RG_MOD;
    return g->cur_value;
}

void destroy(RandomGenerator *g) {
    free(g);
}

static const RandomOperations kRandomGeneratorOps = {next, destroy};

RandomGenerator *random_create(int seed) {
    RandomGenerator *g = calloc(1, sizeof(*g));
    if (!g) {
        return NULL;
    }
    g->cur_value = seed;
    g->ops = &kRandomGeneratorOps;
    return g;
}

#ifdef TESTING
#include <stdio.h>
int main() {
    RandomGenerator *rr = random_create(1234);
    for (int j = 0; j < 100; ++j) {
        printf("%d\n", rr->ops->next(rr));
    }
    rr->ops->destroy(rr);
    return 0;
}
#endif