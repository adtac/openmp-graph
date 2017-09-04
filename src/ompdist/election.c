#include <stdlib.h>

#include "election.h"

/**
 * generate_nodes - Generates a bunch of nodes with randomized IDs.
 *
 * @N: The number of nodes.
 *
 * Returns a pointer to an array of `process` nodes.
 */
process* generate_nodes(int N) {
    process* processes = malloc(N * sizeof(process));

    int* ids = malloc(N * sizeof(int));
    for (int i = 0; i < N; i++)
        ids[i] = i;

    for (int i = 0; i < N; i++) {
        int j = rand() % (N - i);
        int t = ids[i];
        ids[i] = ids[j];
        ids[j] = t;
    }

    for (int i = 0; i < N; i++) {
        processes[i].id = ids[i];
        processes[i].received = -1;
        processes[i].send = ids[i];
        processes[i].status = 0;
        processes[i].leader = ids[i];
    }

    free(ids);

    return processes;
}
