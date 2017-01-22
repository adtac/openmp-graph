#include "utils.h"

int next_power_of_2(int x)
{
    int bitres = 0;

    while(x) {
        x >>= 1;
        bitres++;
    }

    return 1 << bitres;
}
