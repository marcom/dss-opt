#ifndef GUARD_MAIN_HELPERS_H
#define GUARD_MAIN_HELPERS_H

#include "ctools/libctools.h"
#include "ctools/num_utils.h"
#include <stdlib.h>
#include <stdbool.h>

double **helper_make_K_nj_alloc(size_t ndim);
bool system_is_exploded(double **p, size_t n1, size_t n2);
double calc_entropy(double **p, size_t n1, size_t n2);
bool is_bad_bp(char *seq, uint i, uint j);
void show_bad_bp(char *seq, uint *pairs, size_t n, bool verbose);
size_t fix_bad_bp(char *seq, uint *pairs, size_t n);
void show_bad_prob(double **p, size_t n1, size_t n2, bool verbose);
void print_for_movie(double **p, size_t n, size_t ndim, char *seq);

#endif /* GUARD_MAIN_HELPERS_H */
