#ifndef _ARGPARSE_H
#define _ARGPARSE_H

void argparse(int argc, char *argv[],
              int *num_vertices,
              int *num_edges,
              FILE **debug_file,
              int *print_amat);

#endif
