#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#include "ompdist/graph.h"
#include "ompdist/graph_gen.h"
#include "ompdist/utils.h"

#include "config.h"

typedef struct {
    int present;
    int in_mis;
    double r;
} payload;

double randnum() {
    return ((double) rand()) / ((double) RAND_MAX);
}

void initialize_graph(graph* g) {
    DEBUG("Initializing graph with payload\n");
    for (int i = 0; i < g->N; i++) {
        node* cur = elem_at(g->vertices, i);

        payload* data = malloc(sizeof(payload));

        data->present = 1;
        data->in_mis = 0;
        data->r = 0;

        cur->data = data;
    }
}

void generate_random_field(graph* g) {
    #pragma omp parallel for schedule(SCHEDULING_METHOD)
    for (int i = 0; i < g->N; i++) {
        node* cur = elem_at(g->vertices, i);
        payload* data = cur->data;

        if (!data->present)
            continue;

        data->r = randnum();
    }
}

void decide_mis_entry(graph* g) {
    #pragma omp parallel for schedule(SCHEDULING_METHOD)
    for (int i = 0; i < g->N; i++) {
        node* cur = elem_at(g->vertices, i);
        payload* data = cur->data;

        if (!data->present)
            continue;
        
        int enter = 1;
        for (int i = 0; i < cur->degree; i++) {
            node* neighbor = *((node**) elem_at(cur->neighbors, i));
            payload* neighbor_data = neighbor->data;

            if (data->r > neighbor_data->r) {
                enter = 0;
                break;
            }
        }

        if (enter) {
            data->present = 0;
            data->in_mis = 1;
        }
    }
}

void remove_mis_adjacent_nodes(graph* g) {
    #pragma omp parallel for schedule(SCHEDULING_METHOD)
    for (int i = 0; i < g->N; i++) {
        node* cur = elem_at(g->vertices, i);
        payload* data = cur->data;

        if (data->in_mis) {
            for (int i = 0; i < cur->degree; i++) {
                node* neighbor = *((node**) elem_at(cur->neighbors, i));
                payload* neighbor_data = neighbor->data;

                neighbor_data->present = 0;
            }
        }
    }
}

int do_present_nodes_exist(graph* g) {
    int keep_going = 0;

    #pragma omp parallel for schedule(SCHEDULING_METHOD)
    for (int i = 0; i < g->N; i++) {
        node* cur = elem_at(g->vertices, i);
        payload* data = cur->data;

        if (data->present)
            keep_going = 1;
    }

    return keep_going;
}

void verify_and_print_solution(graph* g) {
    printf("Elements in MIS: ");
    for (int i = 0; i < g->N; i++) {
        node* cur = elem_at(g->vertices, i);
        payload* data = cur->data;

        if (data->in_mis)
            printf("%d ", cur->label);
    }
    printf("\n");

    printf("Verifying... ");
    int correct = 1;
    for (int i = 0; i < g->N; i++) {
        node* cur = elem_at(g->vertices, i);
        payload* data = cur->data;

        if (!data->in_mis)
            continue;

        for (int j = 0; j < cur->degree; j++) {
            node* neighbor = *((node**) elem_at(cur->neighbors, j));
            payload* neighbor_data = neighbor->data;

            if (neighbor_data->in_mis) {
                correct = 0;
                break;
            }
        }
    }

    if (correct)
        printf("Correct\n");
    else
        printf("Wrong\n");
}

/**
 * Creates a MIS from a randomly generated graph in a distributed manner.
 *
 * Links:
 *   - https://disco.ethz.ch/courses/podc_allstars/lecture/chapter7.pdf
 *     See Algorithm 7.12 (Fast MIS-2)
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

    int keep_going = 1;

    while (keep_going) {
        keep_going = 0;

        generate_random_field(g);

        decide_mis_entry(g);

        remove_mis_adjacent_nodes(g);

        keep_going = do_present_nodes_exist(g);
    }

    // print_graph(g);

    verify_and_print_solution(g);

    return 0;
}
