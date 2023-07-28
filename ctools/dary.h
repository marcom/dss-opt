#ifndef DSSOPT_HEADER_DARY_H
#define DSSOPT_HEADER_DARY_H

#include <stdlib.h>
#include <stdbool.h>

void dary_set(double *a, size_t n, double val);
bool dary_equal(double *a, double *b, size_t n);
void dary_print(double *a, size_t n);
bool dary_is_finite(double *a, size_t n);

void dary2d_set(size_t n1, size_t n2, double **a, double val);
bool dary2d_equal(double **a, double **b, size_t n1, size_t n2);
void dary2d_print(double **a, size_t n1, size_t n2);
bool dary2d_is_finite(double **a, size_t n1, size_t n2);

#endif /* DSSOPT_HEADER_DARY_H */
