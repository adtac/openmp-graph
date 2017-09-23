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

/**
 * initialize_graph - Initializes the graph with basic data
 *
 * @g: a pointer to the graph object
 */
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

/**
 * generate_random_field - Generates the random field -- a double in the 
 * range [0, 1) -- for each node.
 *
 * @g: a pointer to the graph object
 */
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

/**
 * decide_mis_entry - Depending on each node's neighbors' random values, decide
 * if that node is entering the MIS.
 *
 * @g: a pointer to the graph object
 */
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

/**
 * remove_mis_adjacent_nodes - If a node has entered the MIS, its neighbors can never
 * be a part of the MIS. Remove them.
 *
 * @g: a pointer to the graph object
 */
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

/**
 * do_present_nodes_exist - Checks if there are any nodes in the graph that are
 * still undecided on whether to enter the MIS or not.
 *
 * @g: a pointer to the graph object
 *
 * Returns 1 if there are such nodes, returns 0 otherwise.
 */
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

/**
 * verify_and_print_solution - Verifies if the computed solution is correct and
 * prints the solution.
 *
 * @g: a pointer to the graph object
 */
void verify_and_print_solution(graph* g) {
    INFO("Elements in MIS: ");
    for (int i = 0; i < g->N; i++) {
        node* cur = elem_at(g->vertices, i);
        payload* data = cur->data;

        if (data->in_mis)
            fprintf(stderr, "%d ", cur->label);
    }
    fprintf(stderr, "\n");

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
        INFO("MIS solution verified to be correct\n");
    else
        INFO("MIS solution incorrect\n");
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
