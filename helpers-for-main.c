#include "helpers-for-main.h"
#include "nj_param.h"
#include "na.h"
#include "ctools/dary.h"
#include <string.h>
#include <stdio.h>
#include <math.h>

/* TODO: hack for now, do this nicer */
double **
helper_make_K_nj_alloc(size_t ndim)
{
    uint i, j;
    double **K_nj;
    xmalloc2d_one_chunk(K_nj, ndim, ndim);
    for (i = 0; i < ndim; i++)
        for (j = 0; j < ndim; j++)
            K_nj[i][j] = K_NJ[i][j];
    return K_nj;
}

bool
system_is_exploded(double **p, size_t n1, size_t n2)
{
    size_t i, j;
    if (! dary2d_is_finite(p, n1, n2))
        return true;
    for (i = 0; i < n1; i++)
        for (j = 0; j < n2; j++)
            if (p[i][j] < -1.0 || p[i][j] > 2.0)
                return true;
    return false;
}

/* calculate an entropy-like measure for the sequence
   s = - 1/N * \sum_{i,j} p_ij * log(p_ij)
   the base of the logarithm should be the alphabet size (n2) */
double
calc_entropy(double **p, size_t n1, size_t n2)
{
    size_t i, j;
    double s = 0;
    for (i = 0; i < n1; i++)
        for (j = 0; j < n2; j++)
            s += plogp_sane(p[i][j]);
    s = - s / n1 / log(n2);
    return s;
}

bool
is_bad_bp(char *seq, uint i, uint j)
{
    char bp[3];
    bp[2] = '\0';
    bp[0] = seq[i];
    bp[1] = seq[j];
    if (strcmp(bp, "CG") && strcmp(bp, "GC") &&
        strcmp(bp, "GU") && strcmp(bp, "UG") &&
        strcmp(bp, "AU") && strcmp(bp, "UA")) {
        return true;
    }
    return false;
}

/* TODO: list of permissible pairs is hardcoded */
void
show_bad_bp(char *seq, uint *pairs, size_t n)
{
    size_t i;
    for (i = 0; i < n; i++) {
        if (pairs[i] == NA_UNPAIRED || i > pairs[i])
            continue;
        if (is_bad_bp(seq, i, pairs[i]))
            printf("bad bp: %zu %u: %c%c\n", i, pairs[i],
                   seq[i], seq[pairs[i]]);
    }
}

size_t
fix_bad_bp(char *seq, uint *pairs, size_t n)
{
    size_t i, nfix = 0;
    for (i = 0; i < n; i++) {
        if (pairs[i] == NA_UNPAIRED || i > pairs[i])
            continue;
        if (is_bad_bp(seq, i, pairs[i])) {
            /* TODO: really only a hack */
            seq[i] = 'G';
            seq[pairs[i]] = 'C';
            nfix++;
        }
    }
    return nfix;
}

static double
calc_dmin(double **p, size_t ndim, size_t i)
{
    size_t j, k;
    double d, di_min = 0;
    /* calculate the minimum distance to each 'pure' corner
       (0, ..., 0, 1, 0, ..., 0) of the hypercube */
    for (j = 0; j < ndim; j++) {
        d = 0;
        for (k = 0; k < ndim; k++) {
            if (k != j)
                d += p[i][k] * p[i][k];
            else
                d += (p[i][k] - 1) * (p[i][k] - 1);
        }
        d = sqrt(d);
        if (j == 0 || d < di_min)
            di_min = d;
    }
    return di_min;
}

void
show_bad_prob(double **p, size_t n, size_t ndim, bool verbose)
{
    size_t i;
    double di_min, d_max = 0, d_avg = 0;
    for (i = 0; i < n; i++) {
        di_min = calc_dmin(p, ndim, i);
        if (di_min > d_max)
            d_max = di_min;
        d_avg += di_min;
    }
    d_avg /= n;
    if (verbose) {
        printf("p: d_max = %f\n", d_max);
        printf("p: d_avg = %f\n", d_avg);
        printf("\n");
    }
#if 0
    size_t j;
    for (i = 0; i < n; i++) {
        for (j = 0; j < ndim; j++) {
            printf(" % f", p[i][j]);
        }
        di_min = calc_dmin(p, ndim, i);
        if (di_min < 0.01)
            printf("    (%4s)", "-");
        else
            printf("    (% .4f)", di_min);
        printf("\n");
    }
    printf("\n");
    /*
    printf("p:\n");
    dary2d_print(p, n, ndim);
    printf("\n");
    */
#endif
}

void
print_for_movie(double **p, size_t n, size_t ndim, char *seq)
{
    pseq_to_str(p, n, ndim, seq);
    printf("%s\n", seq);
    dary2d_print(p, n, ndim);
}
