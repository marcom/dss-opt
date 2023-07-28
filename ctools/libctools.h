#ifndef DSSOPT_HEADER_LIBCTOOLS_H
#define DSSOPT_HEADER_LIBCTOOLS_H

#include <stdlib.h>

/* TODO: some typedefs not needed for C99/C++ ?, maybe should be
   defined conditionally */

typedef unsigned char uchar;
typedef unsigned int uint;
typedef unsigned long ulong;

#ifdef __GNUC__
#define CT_ATTRIBUTE_FORMAT(type, idx_format, idx_first_to_check) \
    __attribute__((format (type, idx_format, idx_first_to_check)))
#else
#define CT_ATTRIBUTE_FORMAT(type, idx_format, idx_first_to_check)
#endif

#define MIN(a,b) ( (a) < (b) ? (a) : (b) )
#define MAX(a,b) ( (a) > (b) ? (a) : (b) )


void *xmalloc(size_t size);


/* allocate a 2-dimensional array as one chunk that can be accessed by
   ptr[i][j] */
#define xmalloc2d_one_chunk(ptr, n1, n2)                        \
do {                                                            \
    ptr = xmalloc(n1 * sizeof(*ptr) + n1 * n2 * sizeof(**ptr)); \
    for (size_t ii = 0; ii < n1; ii++)                          \
        ptr[ii] =  ((__typeof__(*ptr)) (ptr + n1)) + ii * n2;   \
} while(0)

/* A debug version that actually does not just allocate one chunk so
   that some row/column violations can actually be seen under
   valgrind.  It does mean though that the array now must be freed
   iteratively, and this will probably cause memory leaks when used as
   a drop-in for the real one chunk allocation. */
#define xmalloc2d_one_chunk_debug(ptr, n1, n2)                  \
do {                                                            \
    ptr = xmalloc(n1 * sizeof(*ptr));                           \
    for (size_t ii = 0; ii < n1; ii++)                          \
        ptr[ii] = xmalloc(n2 * sizeof(**ptr));                  \
} while(0)


void xfail(const char *format, ...)
    CT_ATTRIBUTE_FORMAT(__printf__, 1, 2);




#endif /* DSSOPT_HEADER_LIBCTOOLS_H */
