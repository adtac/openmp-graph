#include <stdio.h>

#include "utils.h"
#include "argparse.h"

int num_vertices, num_edges;
FILE *debug_file;

int main(int argc, char *argv[]) {
    argparse(argc, argv,
             &num_vertices,
             &num_edges,
             &debug_file);
}
