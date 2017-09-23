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

    for (int i = 0; i < N; i++) {
        node* cur = elem_at(g->vertices, i);

        processor* p = malloc(sizeof(processor));

        p->x = i;

        cur->data = p;
    }

    int something_changed = 1;
    int num_rounds = 0;
    while (something_changed) {
        num_rounds++;
        something_changed = 0;

#pragma omp parallel for schedule(SCHEDULING_METHOD)
        for (int i = 0; i < N; i++) {
            node* cur = elem_at(g->vertices, i);
            processor* p = cur->data;

            int new_x = p->x;
            for (int j = 0; j < cur->degree; j++) {
                node* neighbor = *((node**) elem_at(cur->neighbors, j));
                processor* neighbor_p = neighbor->data;

                if (new_x < neighbor_p->x)
                    new_x = neighbor_p->x;
            }

            p->new_x = new_x;
        }

#pragma omp parallel for schedule(SCHEDULING_METHOD)
        for (int i = 0; i < N; i++) {
            node* cur = elem_at(g->vertices, i);
            processor* p = cur->data;

            if (p->new_x != p->x)
                something_changed = 1;

            p->x = p->new_x;
        }
    }

    print_graph(g);

    INFO("%d rounds elapsed\n", num_rounds);

    int disagreement = 0;
    int leader = -1;
    for (int i = 0; i < N; i++) {
        node* cur = elem_at(g->vertices, i);
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
    
    return 0;
}
