#include <stdlib.h>

#include "graph.h"

/**
 * add_edge - add an edge from un to vn
 * @g:  the graph
 * @un: the source vertex ID
 * @vn: the destination vertex ID
 */
void add_edge(graph *g, int un, int vn)
{
    g->amat[un][vn] = 1;

    node *u = g->vertices + un, *v = g->vertices + vn;

    while(u->degree >= u->alloc) {
        u->alloc *= 2;
        u->neighbors = (int *)realloc(u->neighbors, sizeof(int) * u->alloc);
    }

    *(u->neighbors + u->degree) = vn;
    u->degree++;
}
