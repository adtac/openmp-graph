#ifndef _GRAPH_H
#define _GRAPH_H

typedef struct {
    int *neighbors;
    int alloc, degree;

    int color, received_color;
    int again;

    int parent;
} node;

typedef struct {
    node *vertices;
    char **amat;  /* adjacency matrix */
    int num_vertices, num_edges;
    int label;
    int root;
    int correct;
} graph;

void add_edge(graph *g, int un, int vn);

#endif