#include <omp.h>
#include <stdio.h>

#include "utils.h"
#include "argparse.h"
#include "graph.h"
#include "graphgen.h"

const int ROOT = 0;  /* let's take the first element as root */

int num_vertices, num_edges;
FILE *debug_file;
int print_amat;

/**
 * initialize - sets the initial values for various vertex parameters
 *
 * @g: the graph
 */
void initialize(graph *g) {
    int i;

    log_debug("initializing node params\n")
#pragma omp parallel for schedule(dynamic)
    for(i = 0; i < g->num_vertices; i++) {
        g->vertices[i].color = i;
        g->vertices[i].received_color = 0;
        g->vertices[i].again = 0;
    }
}

/**
 * again - check if any of the vertices are scheduled for another check
 *
 * @g: the graph
 *
 * Returns 1 if any of the vertices are scheduled for another check;
 * 0 otherwis
 */
int again(graph *g) {
    int i;

    /* We can't parallelize this since we're returning mid-way. */
    for(i = 0; i < g->num_vertices; i++) {
        if(g->vertices[i].again == 1)
            return 1;
    }
    return 0;
}

/**
 * six_colors - colors the graph with six colors
 *
 * @g: the graph
 */
void six_colors(graph *g) {
    int i;

#pragma omp parallel for schedule(dynamic) shared(g)
    for(i = 0; i < g->num_vertices; i++) {
        int j;
        node *u = g->vertices + i;

        for(j = 0; j < u->degree; j++) {
            node *v = g->vertices + u->neighbors[j];
            v->received_color = u->color;
        }
    }

#pragma omp parallel for schedule(dynamic)
    for(i = 0; i < g->num_vertices; i++) {
        if(i == ROOT)
            continue;

        node *u = g->vertices + i;
        int k;

        u->again = 0;

        int xor = u->received_color ^ u->color;
        for(k = 0; k <= g->label; k++) {
            int mask = 1 << k;

            if(xor & mask) { /* the have this bit different */
                u->color = (mask << 1) + (u->color & mask);
                break;
            }
        }

        if(u->color >= 6)
            u->again = 1;
    }
}

/**
 * run - runner function
 *
 * @g: the graph
 */
void run(graph *g) {
    g->vertices[ROOT].color = 0;

    log_debug("coloring graph with six colors\n");
    do {
        six_colors(g);
    } while(again(g));

    for(int i = 0; i < g->num_vertices; i++)
        printf("[%d] %d\n", i, g->vertices[i].color);
    printf("\n");
}

int main(int argc, char *argv[]) {
    argparse(argc, argv,
             &num_vertices,
             &num_edges,
             &debug_file,
             &print_amat);

    graph *g = generate_graph(num_vertices, num_edges, debug_file);

    if(print_amat)
        print_graph(g);

    omp_set_num_threads(8);
    initialize(g);

    run(g);
}
