#include <stdio.h>

#include "argparse.h"

int num_vertices, num_edges;

int main(int argc, char *argv[]) {
    argparse(argc, argv,
             &num_vertices,
             &num_edges);
}
