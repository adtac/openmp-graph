#include <string.h>

#include "utils.h"

void swap(int* a, int* b) {
    int t = *a;
    *a = *b;
    *b = t;
}

int input_through_argv(int argc, char* argv[]) {
    if (argc < 3)
        return 0;

    if (strcmp(argv[1], "-") == 0)
        return 1;

    return 0;
}
