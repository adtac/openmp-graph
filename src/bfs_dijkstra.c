#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#include "utils.h"
#include "ompdist/vector.h"
#include "ompdist/graph.h"
#include "ompdist/graph_gen.h"

#include "config.h"

#define START 1
#define JOIN  2

typedef struct {
    int parent_label;
    int phase_discovered;
} payload;

int main(int argc, char* argv[]) {
    int N = 16;
    int M = 64;

    if (argc > 1) {
        sscanf(argv[1], "%d", &N);
        sscanf(argv[2], "%d", &M);
    }

    graph* g = generate_new_connected_graph(N, M);

    // allocate the data field for each node
    #pragma omp parallel for schedule(SCHEDULING_METHOD)
    for (int i = 0; i < N; i++) {
        node* cur = elem_at(g->vertices, i);

        payload* data = malloc(sizeof(payload));

        data->parent_label = -1;
        data->phase_discovered = -1;

        cur->data = data;
    }

    // set the root node to be a part of T_1 at the beginning
    node* root = elem_at(g->vertices, 0);
    payload* data = root->data;
    data->phase_discovered = 0;

    int p = 0;

    int nobody_was_discovered = 0;
    while (!nobody_was_discovered) {
        nobody_was_discovered = 1;

        // broadcast "start p" within T_p
        #pragma omp parallel for schedule(SCHEDULING_METHOD)
        for (int i = 0; i < N; i++) {
            node* cur = elem_at(g->vertices, i);
            payload* data = cur->data;

            // this node was just discovered in phase `p`
            if (data->phase_discovered == p) {
                // we send a "join p+1" message to all quiet neighbors
                for (int j = 0; j < cur->degree; j++) {
                    node* neighbor = *((node**) elem_at(cur->neighbors, j));
                    payload* neighbor_data = neighbor->data;

                    if (neighbor_data->phase_discovered < 0) {
                        neighbor_data->phase_discovered = p+1;
                        neighbor_data->parent_label = cur->label;
                        nobody_was_discovered = 0;
                    }
                }
            }
        }

        p += 1;
    }

    int max_distance = 0;
    for (int i = 0; i < N; i++) {
        node* cur = elem_at(g->vertices, i);
        payload* data = cur->data;

        if (data->phase_discovered > max_distance)
            max_distance = data->phase_discovered;

        INFO("node_%d: parent = %d, dist = %d\n", cur->label, data->parent_label, data->phase_discovered);
    }

    INFO("max_distance = %d\n", max_distance);
}
