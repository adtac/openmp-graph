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
 * print_help - prints the help message
 */
void print_help()
{
    printf("-h\tdisplay this help message\n");
    printf("-N\tnumber of vertices (default: 1000)\n");
    printf("-E\tnumber of edges (default: 100000)\n");

    exit(0);
}

/**
 * argparse - parses command line arguments
 * @argc: number of arguments
 * @argv: argument list
 */
void argparse(int argc, char *argv[],
              int *num_vertices,
              int *num_edges)
{
    /* Default values */
    *num_vertices = 1000;
    *num_edges    = 100000;

    int i;
    for(i = 1; i < argc; i++) {
        if(strcmp(argv[i], "-h") == 0)
            print_help();

        if(strcmp(argv[i], "-N") == 0)
            *num_vertices = int_arg(argc, argv, i);

        if(strcmp(argv[i], "-E") == 0)
            *num_edges = int_arg(argc, argv, i);
    }
}
