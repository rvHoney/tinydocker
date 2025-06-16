#include "utils.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

int write_str_to_file(const char *path, const char *fmt, ...)
{
    FILE *f = fopen(path, "w");
    if (!f)
    {
        perror("fopen");
        return EXIT_FAILURE;
    }

    va_list args;
    va_start(args, fmt);
    if (vfprintf(f, fmt, args) < 0)
    {
        perror("vfprintf");
        va_end(args);
        fclose(f);
        return EXIT_FAILURE;
    }
    va_end(args);
    fclose(f);
    return EXIT_SUCCESS;
}