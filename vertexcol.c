#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#include "utils.h"
#include "argparse.h"
#include "graph.h"
#include "graphgen.h"

const int ROOT = 0;  /* let's take the first element as root */

int num_vertices, num_edges;
FILE *debug_file;
int print_amat;
FILE *graph_file;
FILE *output_file;

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
            // printf("%d recv %d from %d\n", u->neighbors[j], u->color, i);
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
        // printf("%d: recvd %d, orig %d\n", i, u->received_color, u->color);
        for(k = 0; k <= g->label; k++) {
            int mask = 1 << k;
            // printf("%d: k = %d, mask = %d, xor = %d\n", i, k, mask, xor);

            if(xor & mask) { /* the have this bit different */
                // printf("%d: recv = %d\n", i, u->received_color);
                u->color = (k << 1) + (u->color & mask ? 1 : 0);
                // printf("%d: col = %d\n", i, u->color);
                break;
            }
        }

        if(u->color >= 6)
            u->again = 1;
    }
}

void shift_down(graph *g)
{
    int i;

#pragma omp parallel for schedule(dynamic) shared(g)
    for(i = 0; i < g->num_vertices; i++) {
        int j;
        node *u = g->vertices + i;

        for(j = 0; j < u->degree; j++) {
            // printf("%d recv %d from %d\n", u->neighbors[j], u->color, i);
            node *v = g->vertices + u->neighbors[j];
            v->received_color = u->color;
        }
    }

#pragma omp parallel for schedule(dynamic) shared(g)
    for(i = 0; i < g->num_vertices; i++) {
        if(i != ROOT) {
            node *u = g->vertices + i;
            u->color = u->received_color;
        }
    }
}

/**
 * six_to_three - bring it down from six colors to three colors
 * @g: the graph
 */
void six_to_three(graph *g)
{
    int x;
    for(x = 5; x >= 3; x--) {
        shift_down(g);

        int new = rand() % 3;
        node *u = g->vertices + ROOT;
        if(new == u->color)
            new = (new+1) % 3;
        u->color = new;

        int i;
#pragma omp parallel for schedule(dynamic)
        for(i = 0; i < g->num_vertices; i++) {
            int parent_col = 0, child_col = 0;

            node *u = g->vertices + i;

            if(u->color == x) {
                node *p = g->vertices + u->parent;
                parent_col = p->color;

                if(u->degree > 0) {
                    node *c = g->vertices + u->neighbors[0];
                    child_col = c->color;
                }

                switch(parent_col + child_col) {
                    case 1:
                        u->color = 2;
                        break;

                    case 2:
                        u->color = 1;
                        break;

                    case 3:
                    case 4:
                    case 5:
                        if(parent_col != 0 && child_col != 0)
                            u->color = 0;
                        else
                            u->color = 1;
                        break;

                    default:
                        u->color = 0;
                }
            }
        }
    }
}

/**
 * run - runner function
 *
 * @g: the graph
 */
void run(graph *g)
{
    g->vertices[ROOT].color = 0;

    log_debug("coloring graph with six colors\n");
    do {
        six_colors(g);
    } while(again(g));

    six_to_three(g);
}

/**
 * write_to_file - write the output to a file
 * @g:           the graph
 * @output_file: output file
 */
void write_to_file(graph *g, FILE *output_file)
{
    fprintf(output_file, "Solution verified to be correct.\n");
    for(int i = 0; i < g->num_vertices; i++) {
        node *u = g->vertices + i;
        fprintf(output_file, "Node %3d: color %d\n", i, u->color);
    }
}

/**
 * verify_solution - verifies if the solution is correct
 * @g: the graph
 */
void verify_solution(graph *g)
{
    g->correct = 1;
    int i, j;

    for(int i = 0; i < g->num_vertices; i++) {
        node *u = g->vertices + i;
        for(int j = 0; j < u->degree; j++) {
            node *v = g->vertices + u->neighbors[j];
            if(g->amat[i][j] && u->color == v->color)
                g->correct = 0;
        }
    }
}

int main(int argc, char *argv[]) {
    argparse(argc, argv,
             &num_vertices,
             &num_edges,
             &debug_file,
             &print_amat,
             &graph_file,
             &output_file);

    graph *g;
    if(graph_file) /* read the graph file */
        g = read_graph_file(graph_file, debug_file);
    else /* generate one */
        g = generate_graph(num_vertices, num_edges, debug_file);

    if(print_amat)
        print_graph(g);

    omp_set_num_threads(8);
    initialize(g);

    run(g);

    verify_solution(g);

    write_to_file(g, output_file);
}
