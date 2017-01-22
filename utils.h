#ifndef _UTILS_H
#define _UTILS_H

#include <stdio.h>

#define log_debug(...) if(debug_file) {\
    fprintf(debug_file, "[DEBUG] ");\
    fprintf(debug_file, __VA_ARGS__);\
}

int next_power_of_2(int x);

#endif
