#ifndef _GRAPHGEN_H
#define _GRAPHGEN_H

#include "graph.h"

graph *generate_graph(int num_vertices, int num_edges, FILE *debug_file);

graph *read_graph_file(FILE *graph_file, FILE *debug_file);

void print_graph(graph *g);

#endif
