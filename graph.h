#ifndef _GRAPH_H
#define _GRAPH_H

typedef struct {
    int color;
    int *neighbors;
    int alloc, degree;
} node;

typedef struct {
    node *vertices;
    char **amat;  /* adjacency matrix */
    int num_vertices, num_edges;
    int label;
} graph;

#endif
