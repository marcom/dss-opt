#include "libctools.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

void *
xmalloc(size_t size)
{
    void *p;
    if ((p = malloc(size)) == NULL)
        xfail("ERROR: xmalloc(): malloc(%lu) failed\n", (ulong) size);
    return p;
}

#if 0
/* allocate an upper triangular matrix, where diag is the first
   diagonal that is inside the array and size is the length of the
   0-diagonal */
void **
xmalloc_utriag(size_t size, int diag)
{
    
}
#endif

void
CT_ATTRIBUTE_FORMAT(__printf__, 1, 2)
xfail(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    printf(format, args);
    va_end(args);
    exit(EXIT_FAILURE);
}
