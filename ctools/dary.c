#include "ctools/dary.h"
#include <math.h>
#include <stdio.h>

/* sets all elements of an array */
void
dary_set(double *a, size_t n, double val)
{
    size_t i;
    for (i = 0; i < n; i++)
        a[i] = val;
}

/* check if two arrays are equal */
bool
dary_equal(double *a, double *b, size_t n)
{
    size_t i;
    for (i = 0; i < n; i++)
        if (a[i] != b[i])
            return false;
    return true;
}

/* print an array */
void
dary_print(double *a, size_t n)
{
    size_t i;
    for (i = 0; i < n; i++)
        printf("% f\n", a[i]);
}

/* check if all elements in an array are finite floating-point numbers,
   i.e. they are not NaNs or infinities */
bool
dary_is_finite(double *a, size_t n)
{
    size_t i;
    for (i = 0; i < n; i++)
        if (! isfinite(a[i]))
            return false;
    return true;
}




/* set all elements of a 2-dimensional array */
void
dary2d_set(size_t n1, size_t n2, double **a, double val)
{
    size_t i, j;
    for (i = 0; i < n1; i++)
        for (j = 0; j < n2; j++)
            a[i][j] = val;
}

/* check if two 2-dimensional arrays are equal */
bool
dary2d_equal(double **a, double **b, size_t n1, size_t n2)
{
    size_t i, j;
    for (i = 0; i < n1; i++)
        for (j = 0; j < n2; j++)
            if (a[i][j] != b[i][j])
                return false;
    return true;
}

/* print all elements of a 2-dimensional array */
void
dary2d_print(double **a, size_t n1, size_t n2)
{
    size_t i, j;
    for (i = 0; i < n1; i++) {
        for (j = 0; j < n2; j++)
            printf(" % f", a[i][j]);
        printf("\n");
    }
}

/* check if all elements in a 2-dimensional array are finite floating-point
   numbers, i.e. they are not NaNs or infinities */
bool
dary2d_is_finite(double **a, size_t n1, size_t n2)
{
    size_t i, j;
    for (i = 0; i < n1; i++)
        for (j = 0; j < n2; j++)
            if (! isfinite(a[i][j]))
                return false;
    return true;
}
