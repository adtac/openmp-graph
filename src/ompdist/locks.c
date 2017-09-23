#include <stdlib.h>

#include "locks.h"
#include "utils.h"

omp_lock_t* new_locks(int N) {
    DEBUG("allocating %d locks\n", N);
    omp_lock_t* locks = malloc(N * sizeof(omp_lock_t));

    DEBUG("initializing locks\n");
    for (int i = 0; i < N; i++)
        omp_init_lock(locks+i);

    return locks;
}

void free_locks(omp_lock_t* locks, int N) {
    DEBUG("destroying %d locks\n", N);
    for(int i = 0; i < N; i++)
        omp_destroy_lock(locks+i);

    DEBUG("freeing up allocated memory\n");
    free(locks);
}
