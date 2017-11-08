#include <string.h>

#include "utils.h"

void swap(int* a, int* b) {
    int t = *a;
    *a = *b;
    *b = t;
}

/**
 * input_through_argv - Checks if the input is to be generated or read in from
 * an input file.
 *
 * @argc: the number of input arguments
 * @argv: the list of input arguments
 *
 * Returns 0 if the input is to be generated. Returns 1 if the input is to be
 * read in from a file.
 */
int input_through_argv(int argc, char* argv[]) {
    if (argc < 3)
        return 0;

    if (strcmp(argv[1], "-") == 0)
        return 1;

    return 0;
}
