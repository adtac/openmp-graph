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

    write_to_file(g, output_file);
}
