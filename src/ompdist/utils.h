#ifndef _UTILS_H_
#define _UTILS_H_

#include <stdio.h>
#include <libgen.h>

/*
 * LOG_LEVEL - Determines what goes into the screen and what doesn't.
 *
 * 0 - Suppress all non-essential output.
 * 1 - Display only warnings.
 * 2 - Display only warnings and information.
 * 3 - Display everything: warnings, information, and debug statements
 */
#define LOG_LEVEL 3

#if defined(LOG_LEVEL) && LOG_LEVEL > 0
#define WARN(fmt, ...)                                                  \
    do {                                                                \
        fprintf(stdout, "[warning] %s:%d:%s(): " fmt,                   \
                basename(__FILE__), __LINE__, __func__, ##__VA_ARGS__); \
    } while (0)
#else
#define WARN(fmt, args...)
#endif

#if defined(LOG_LEVEL) && LOG_LEVEL > 1
#define INFO(fmt, ...)                                                  \
    do {                                                                \
        fprintf(stdout, "[info] %s:%d:%s(): " fmt,                      \
                basename(__FILE__), __LINE__, __func__, ##__VA_ARGS__); \
    } while (0)
#else
#define INFO(fmt, args...)
#endif

#if defined(LOG_LEVEL) && LOG_LEVEL > 2
#define DEBUG(fmt, ...)                                                 \
    do {                                                                \
        fprintf(stdout, "[debug] %s:%d:%s(): " fmt,                     \
                basename(__FILE__), __LINE__, __func__, ##__VA_ARGS__); \
    } while (0)
#else
#define DEBUG(fmt, args...)
#endif

void swap(int*, int*);

int input_through_argv(int, char**);

#endif // _UTILS_H_
