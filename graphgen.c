#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "graph.h"
#include "graphgen.h"

void generate_label(graph *g)
{
    int pow_of_2;
    if((g->num_vertices & (g->num_vertices - 1)) == 0)
        pow_of_2 = g->num_vertices;
    else
        pow_of_2 = next_power_of_2(g->num_vertices);

    g->label = 0;
    while(pow_of_2) {
        g->label++;
        pow_of_2 >>= 1;
    }
    g->label--;
}

graph *generate_graph(int num_vertices, int num_edges, FILE *debug_file)
{
    int i;
    graph *g = (graph *)malloc(sizeof(graph));

    g->num_vertices = num_vertices;
    g->num_edges = num_edges;

    g->root = 0;

    generate_label(g);

    log_debug("allocating %d vertices\n", g->num_vertices);
    g->vertices = (node *)malloc(sizeof(node) * (num_vertices+1));
    for(i = 0; i <= num_vertices; i++) {
        g->vertices[i].neighbors = (int *)malloc(sizeof(int) * 8);
        g->vertices[i].alloc = 8;
        g->vertices[i].degree = 0;
        g->vertices[i].parent = i;
    }

    log_debug("allcating adjacency matrix\n")
    g->amat = (char **)malloc(sizeof(char *) * num_vertices);
    for(i = 0; i < num_vertices; i++) {
        g->amat[i] = (char *)malloc(sizeof(char) * num_vertices);
        memset(g->amat[i], 0, sizeof(char) * num_vertices);
    }

    log_debug("generating edge list\n");
    int edges_left = num_edges;
    while(edges_left) {
        int _un = rand()%num_vertices, _vn = rand()%num_vertices;
        int un = _un < _vn ? _un : _vn;
        int vn = _un < _vn ? _vn : _un;

        if(un == vn)
            continue;

        if(!g->amat[un][vn]) {
            add_edge(g, un, vn);

            edges_left--;
        }
    }

    return g;
}

graph *read_graph_file(FILE *graph_file, FILE *debug_file)
{
    int i, j;
    graph *g = (graph *)malloc(sizeof(graph));

    log_debug("reading graph file\n");
    fscanf(graph_file, "%d\n", &g->num_vertices);

    fscanf(graph_file, "%d\n", &g->root);

    generate_label(g);

    log_debug("allocating %d vertices\n", g->num_vertices);
    g->vertices = (node *)malloc(sizeof(node) * (g->num_vertices+1));
    for(i = 0; i <= g->num_vertices; i++) {
        g->vertices[i].neighbors = (int *)malloc(sizeof(int) * 8);
        g->vertices[i].alloc = 8;
        g->vertices[i].degree = 0;
    }

    log_debug("allcating adjacency matrix\n")
    g->amat = (char **)malloc(sizeof(char *) * g->num_vertices);
    for(i = 0; i < g->num_vertices; i++) {
        g->amat[i] = (char *)malloc(sizeof(char) * g->num_vertices);
        memset(g->amat[i], 0, sizeof(char) * g->num_vertices);
    }

    g->num_edges = 0;
    for(i = 0; i < g->num_vertices; i++) {
        for(j = 0; j < g->num_vertices; j++) {
            if(fgetc(graph_file) == '1' && i <= j) {
                add_edge(g, i, j);
                g->num_edges++;
            }
        }
        fgetc(graph_file); /* skip the newline */
    }
    g->num_edges /= 2; /* because we double conted */

    for(i = 0; i < g->num_vertices; i++)
        fscanf(graph_file, "%d", &(g->vertices[i].parent));

    return g;
}

void print_graph(graph *g)
{
    int i, j;

    printf("Generated graph: %dx%d with %d edges\n",
           g->num_vertices, g->num_vertices, g->num_edges);

    printf("    |");
    for(int i = 0; i < g->num_vertices; i++)
        printf(" %2d |", i);
    printf("\n");

    printf("----+");
    for(int i = 0; i < g->num_vertices; i++)
        printf("----+", i);
    printf("\n");

    for(i = 0; i < g->num_vertices; i++) {
        printf(" %2d |", i);
        for(j = 0; j < g->num_vertices; j++)
            printf(" %s |", g->amat[i][j] ? "**" : "  ");
        printf("\n");
    }
}
