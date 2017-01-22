#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "graph.h"
#include "graphgen.h"

graph *generate_graph(int num_vertices, int num_edges, FILE *debug_file)
{
    int i;
    graph *g = (graph *)malloc(sizeof(graph));

    g->num_vertices = num_vertices;
    g->num_edges = num_edges;

    log_debug("allocating vertices\n");
    g->vertices = (node *)malloc(sizeof(node) * (num_vertices+1));
    for(i = 0; i <= num_vertices; i++) {
        g->vertices[i].neighbors = (int *)malloc(sizeof(int) * 8);
        g->vertices[i].alloc = 8;
        g->vertices[i].degree = 0;
    }

    log_debug("allcating adjacency matrix\n")
    g->amat = (char **)malloc(sizeof(char *) * num_vertices);
    for(i = 0; i < num_edges; i++) {
        g->amat[i] = (char *)malloc(sizeof(char) * num_vertices);
        memset(g->amat[i], 0, sizeof(char) * num_vertices);
    }

    log_debug("generating edge list\n");
    int edges_left = num_edges;
    while(edges_left) {
        int un = rand()%num_vertices, vn = rand()%num_vertices;
        if(un == vn)
            continue;

        if(!g->amat[un][vn]) {
            g->amat[un][vn] = 1;
            g->amat[vn][un] = 1;

            node *u = g->vertices + un, *v = g->vertices + vn;

            while(u->degree >= u->alloc) {
                u->alloc *= 2;
                u->neighbors = (int *)realloc(u->neighbors, sizeof(int) * u->alloc);
            }
            while(v->degree >= v->alloc) {
                v->alloc *= 2;
                v->neighbors = (int *)realloc(v->neighbors, sizeof(int) * v->alloc);
            }

            *(u->neighbors + u->degree) = vn;
            *(v->neighbors + v->degree) = un;
            u->degree++;
            v->degree++;

            edges_left--;
        }
    }

    return g;
}
