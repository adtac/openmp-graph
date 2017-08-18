#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "argparse.h"

/**
 * int_arg - returns the integer argument value
 * @argc: number of arguments
 * @argv: argument list
 * @i:    index of the argument name
 *
 * Returns the integer if valid. Exits if any errors are encountered.
 */
int int_arg(int argc, char *argv[], int i)
{
    if(i + 1 >= argc || argv[i+1] == NULL) {
        fprintf(stderr, "error: no value provided for `%s'\n", argv[i]);
        exit(1);
    }

    char *c = argv[i+1];
    int result = 0;
    while(*c) {
        if(*c >= '0' && *c <= '9')
            result = result*10 + *c - '0';
        else {
            fprintf(stderr,
                    "error: argument provided for `%s' is not a number\n",
                    argv[i]);
            exit(1);
        }
        c++;
    }

    return result;
}

/**
 * file_arg - returns the file handle for the given file
 * @argc: number of arguments
 * @argv: argument list
 * @i:    index of the argument name
 * @read: read-only or not
 *
 * Returns the integer if valid. Exits if any errors are encountered.
 */
FILE *file_arg(int argc, char *argv[], int i, int read)
{
    if(i + 1 >= argc || argv[i+1] == NULL) {
        fprintf(stderr, "error: no value provided for `%s'\n", argv[i]);
        exit(1);
    }

    FILE *ret;
    
    if(read)
        ret = fopen(argv[i+1], "r");
    else
        ret = fopen(argv[i+1], "w");

    if(ret == NULL) {
        fprintf(stderr, "error: can't open file given for `%s'", argv[i]);
        exit(1);
    }

    return ret;
}

/**
 * print_help - prints the help message
 */
void print_help()
{
    printf("-h          display this help message\n");
    printf("-d <arg>    debug file (off by default, use /dev/stdout for stdout)\n");
    printf("-f <arg>    input graph file (if not given, a graph will be generated)\n");
    printf("-g          print the adjacency matrix (warning: this may produce large output)\n");
    printf("-o <arg>    output file (default: stdout)\n");
    printf("-N <arg>    number of vertices (default: 16)\n");
    printf("-E <arg>    number of edges (default: 80)\n");

    printf("\n");

    printf("Make sure your input is a tree (has no cycles).\n");

    exit(0);
}

/**
 * argparse - parses command line arguments
 * @argc:       number of arguments
 * @argv:       argument list
 * @debug_file: debug file
 * @print_amat: whether or not to print the adjacency matrix
 */
void argparse(int argc, char *argv[],
              int *num_vertices,
              int *num_edges,
              FILE **debug_file,
              int *print_amat,
              FILE **graph_file,
              FILE **output_file)
{
    /* Default values */
    *num_vertices = 16;
    *num_edges    = 80;
    *debug_file   = fopen("/dev/null", "w");
    *print_amat   = 0;
    *graph_file   = NULL;
    *output_file  = fopen("/dev/stdout", "w");

    int i;
    for(i = 1; i < argc; i++) {
        if(strcmp(argv[i], "-h") == 0)
            print_help();

        if(strcmp(argv[i], "-N") == 0)
            *num_vertices = int_arg(argc, argv, i);

        if(strcmp(argv[i], "-E") == 0)
            *num_edges = int_arg(argc, argv, i);

        if(strcmp(argv[i], "-d") == 0)
            *debug_file = file_arg(argc, argv, i, 0);

        if(strcmp(argv[i], "-g") == 0)
            *print_amat = 1;

        if(strcmp(argv[i], "-f") == 0)
            *graph_file = file_arg(argc, argv, i, 1);

        if(strcmp(argv[i], "-o") == 0)
            *output_file = file_arg(argc, argv, i, 0);
    }
}
