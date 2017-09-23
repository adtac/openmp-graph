#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#include "ompdist/graph.h"
#include "ompdist/graph_gen.h"
#include "ompdist/utils.h"

#include "config.h"

typedef struct {
    int x;              // candidate leader
    int new_x;          // temporary value
} processor;

/**
 * initialize_graph - Initializes the graph with basic data
 *
 * @g: a pointer to the graph object
 */
void initialize_graph(graph* g) {
    for (int i = 0; i < g->N; i++) {
        node* cur = elem_at(&g->vertices, i);

        processor* p = malloc(sizeof(processor));

        p->x = i;

        cur->data = p;
    }
}

/**
 * calculate_temporary_x - Calculates the temporary x value based on each
 * neighbor's x value.
 *
 * @g: a pointer to the graph
 */
void calculate_temporary_x(graph* g) {
    #pragma omp parallel for schedule(SCHEDULING_METHOD)
    for (int i = 0; i < g->N; i++) {
        node* cur = elem_at(&g->vertices, i);
        processor* p = cur->data;

        int new_x = p->x;
        for (int j = 0; j < cur->degree; j++) {
            node* neighbor = *((node**) elem_at(&cur->neighbors, j));
            processor* neighbor_p = neighbor->data;

            if (new_x < neighbor_p->x)
                new_x = neighbor_p->x;
        }

        p->new_x = new_x;
    }
}

/**
 * propagate_temporary_x - Takes the temporararily calculated `x` value
 * and makes it permanent.
 *
 * @g: a pointer to the graph
 *
 * Returns 0 is none of the vertices had a change (i.e. the temporary value
 * was the same as the original value), or returns 1 if there was a change.
 */
int propagate_temporary_x(graph* g) {
    int something_changed = 0;
    #pragma omp parallel for schedule(SCHEDULING_METHOD)
    for (int i = 0; i < g->N; i++) {
        node* cur = elem_at(&g->vertices, i);
        processor* p = cur->data;

        if (p->new_x != p->x)
            something_changed = 1;

        p->x = p->new_x;
    }

    return something_changed;
}

/**
 * verify_and_print_solution - Verifies the computed solution and prints the
 * agreed upon leader, if there's no disagreement.
 *
 * @g: a pointer to the graph
 */
void verify_and_print_solution(graph* g) {
    int disagreement = 0;
    int leader = -1;

    /**
     * Note: there's no `#pragma omp parallel` required here - this is not a
     * part of the solution computation.
     */
    for (int i = 0; i < g->N; i++) {
        node* cur = elem_at(&g->vertices, i);
        processor* p = cur->data;

        if (leader == -1)
            leader = p->x;

        if (leader != p->x) {
            INFO("Node %d disagrees; wants %d as leader instead of %d\n", i, p->x, leader);
            disagreement = 1;
        }
    }

    if (!disagreement)
        INFO("Correct! All nodes have agreed to have %d as the leader.\n", leader);
}

/**
 * Uses the distributed leader elect algorithm due to David Peleg (1990)
 * to determine a leader for any general network in a distributed fashion.
 */
int main(int argc, char* argv[]) {
    int N = 16;
    int M = 64;

    if (argc > 1) {
        sscanf(argv[1], "%d", &N);
        sscanf(argv[2], "%d", &M);
    }

    graph* g = generate_new_connected_graph(N, M);

    initialize_graph(g);

    int something_changed = 1;
    int num_rounds = 0;
    while (something_changed) {
        calculate_temporary_x(g);

        something_changed = propagate_temporary_x(g);

        num_rounds++;
    }

    INFO("%d rounds elapsed\n", num_rounds);

    // print_graph(g);

    verify_and_print_solution(g);

    return 0;
}
