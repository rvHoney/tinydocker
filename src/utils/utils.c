#include "utils.h"

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int write_str_to_file(const char *path, const char *fmt, ...)
{
    FILE *f = fopen(path, "w");
    if (!f)
    {
        fprintf(stderr, "Error: fopen failed: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    va_list args;
    va_start(args, fmt);
    if (vfprintf(f, fmt, args) < 0)
    {
        fprintf(stderr, "Error: vfprintf failed: %s\n", strerror(errno));
        va_end(args);
        fclose(f);
        return EXIT_FAILURE;
    }
    va_end(args);
    fclose(f);
    return EXIT_SUCCESS;
}