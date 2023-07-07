#include "dss.h"
#include "ctools/num_utils.h"
#include "ctools/dary.h"
#include "na.h"
#include <math.h>

/* which include file for alloca() */
#ifdef __linux__
/* alloca.h on linux */
#include <alloca.h>
#else
#ifdef __APPLE__
/* alloca.h on mac */
#include <alloca.h>
#else
#ifdef _WIN32
/* windows doesn't have alloca.h, alloca() is in malloc.h */
#include <malloc.h>
#else
/* stdlib.h on freebsd and others */
#include <stdlib.h>
#endif
#endif
#endif

/* additional "energy" functions for dynamics in sequence space */

/* U_pa: for each i, keep \sum_j p[i][j] close to 1 */
double
dss_calc_U_pa(double **p, uint n, uint ndim, double k)
{
    uint i, j;
    double U = 0, sum;
    for (i = 0; i < n; i++) {
        /* TODO: replace by sum = dary_sum(p[i], ndim); */
        sum = 0;
        for (j = 0; j < ndim; j++)
            sum += p[i][j];
        U += 0.5 * k * (sum - 1) * (sum - 1);
    }
    return U;
}

void
dss_calc_gradU_pa(double **p, uint n, uint ndim, double k, double **gradU)
{
    uint i, j;
    double sum;
    for (i = 0; i < n; i++) {
        /* TODO: replace by sum = dary_sum(p[i], ndim); */
        sum = 0;
        for (j = 0; j < ndim; j++)
            sum += p[i][j];
        for (j = 0; j < ndim; j++)
            gradU[i][j] += k * (sum - 1);
    }
}

/* U_pi: keep p[i][j] between 0 and 1 */
double
dss_calc_U_pi(double **p, uint n, uint ndim, double k)
{
    uint i, j;
    double U = 0;
    for (i = 0; i < n; i++)
        for (j = 0; j < ndim; j++)
            if (p[i][j] < 0)
                U += 0.5 * k * p[i][j] * p[i][j];
            else if (p[i][j] > 1)
                U += 0.5 * k * (p[i][j] - 1) * (p[i][j] - 1);
    return U;
}

void
dss_calc_gradU_pi(double **p, uint n, uint ndim, double k, double **gradU)
{
    uint i, j;
    for (i = 0; i < n; i++)
        for (j = 0; j < ndim; j++)
            if (p[i][j] < 0)
                gradU[i][j] += k * p[i][j];
            else if (p[i][j] > 1)
                gradU[i][j] += k * (p[i][j] - 1);
}

double
dss_calc_U_pur_cauchy(double **p, uint n, uint ndim, double kpur)
{
    uint i, j, k;
    double xlen2;
    const double pi = 3.141592653589793;
    const double gamma = 0.2, inv_gamma2 = 1 / (gamma * gamma);
    double U = 0;
    for (i = 0; i < n; i++) {
        for (j = 0; j < ndim; j++) {
            /* xlen2 = norm^2(p[i,:] - unit_j[:]) (unit_j is j-th unit vec) */
            xlen2 = 0;
            for (k = 0; k < ndim; k++) {
                if (k == j)
                    xlen2 += (p[i][k] - 1) * (p[i][k] - 1);
                else
                    xlen2 += p[i][k] * p[i][k];
            }
            U += -0.5 * kpur / (pi * gamma * (1 + inv_gamma2 * xlen2));
        }
    }
    return U;
}

void
dss_calc_gradU_pur_cauchy(double **p, uint n, uint ndim, double kpur,
                          double **gradU)
{
    uint i, j, k;
    double xlen2, xk, f;
    const double pi = 3.141592653589793;
    const double gamma = 0.2, inv_gamma2 = 1 / (gamma * gamma);
    for (i = 0; i < n; i++) {
        for (j = 0; j < ndim; j++) {
            /* xlen2 = norm^2(p[i,:] - unit_j[:]) (unit_j is j-th unit vec) */
            xlen2 = 0;
            for (k = 0; k < ndim; k++) {
                if (k == j)
                    xlen2 += (p[i][k] - 1) * (p[i][k] - 1);
                else
                    xlen2 += p[i][k] * p[i][k];
            }
            f = 1 / (pi * gamma * (1 + inv_gamma2 * xlen2));
            /* xk = (p[i,:] - unit_j[:])[k] (kth component of p[i,:]-unit_j) */
            for (k = 0; k < ndim; k++) {
                xk = p[i][k];
                if (k == j)
                    xk -= 1;
                gradU[i][k] += kpur * (pi/gamma) * f * f * xk;
            }
        }
    }
}

/* TODO: not tested / does not work */
double
dss_calc_U_negdesign_het(double **p, uint n, uint ndim, double kneg)
{
    uint i, j;
    double U = 0, f;
    for (j = 0; j < ndim; j++) {
        f = 0;
        for (i = 0; i < n; i++)
            f += p[i][j];
        f /= n;
        U += plogp_sane(f);
    }
    return kneg * U / log(ndim);
}

/* TODO: not tested / does not work */
void
dss_calc_gradU_negdesign_het(double **p, uint n, uint ndim, double kneg,
                             double **gradU)
{
    uint i, j;
    double f;
    for (j = 0; j < ndim; j++) {
        f = 0;
        for (i = 0; i < n; i++)
            f += p[i][j];
        f /= n;
        for (i = 0; i < n; i++)
            gradU[i][j] += kneg / log(ndim) / n * p[i][j] * (1 + log(f));
    }
}

/* calculate: x^T * A * y, where x^T denotes the transpose of x */
/* TODO: move to library */
static double
xAy(double *x, double **A, double *y, uint n)
{
    uint i, j;
    double v = 0, tmp;
    for (i = 0; i < n; i++) {
        tmp = 0;
        for (j = 0; j < n; j++)
            tmp += x[j] * A[j][i];
        v += y[i] * tmp;
    }
    return v;
}

/* TODO: move malloc out of this function */
/* TODO: might want to exclude bases less than hairpin-min-size away */
double
dss_calc_U_negdesign_nj(double **p, uint n, uint ndim,
                        double kneg, double **K_nj, uint *pairs)
{
    uint i, j;
    double U;
#if 0
    double *p_sum = xmalloc(ndim * sizeof(*p_sum));
#endif
    double *p_sum = alloca(ndim * sizeof(*p_sum));

    dary_set(p_sum, ndim, 0);

    for (i = 0; i < n; i++)
        for (j = 0; j < ndim; j++)
            p_sum[j] += p[i][j];
    U = xAy(p_sum, K_nj, p_sum, ndim);

    for (i = 0; i < n; i++)
        U -= xAy(p[i], K_nj, p[i], ndim);

    for (i = 0; i < n; i++)
        if (pairs[i] != NA_UNPAIRED)
            U -= xAy(p[i], K_nj, p[pairs[i]], ndim);
#if 0
    free(p_sum);
#endif
    return 0.5 * kneg / n * U;
}

/* TODO: move malloc out of this function, it gets called a lot */
/* TODO: might want to exclude bases less than hairpin-min-size away */
void
dss_calc_gradU_negdesign_nj(double **p, uint n, uint ndim, double kneg,
                            double **K_nj, uint *pairs, double **gradU)
{
    uint i, j, k;
#if 0
    double *g = xmalloc(ndim * sizeof(*g));
#endif
    double *g     = alloca(ndim * sizeof(*g));
    double *p_sum = alloca(ndim * sizeof(*g));

    dary_set(p_sum, ndim, 0);
    for (i = 0; i < n; i++)
        for (k = 0; k < ndim; k++)
            p_sum[k] += p[i][k];

    for (i = 0; i < n; i++) {
        for (k = 0; k < ndim; k++)
            g[k] = p_sum[k] - p[i][k];

        if (pairs[i] != NA_UNPAIRED)
            for (k = 0; k < ndim; k++)
                g[k] -= p[pairs[i]][k];

        for (j = 0; j < ndim; j++)
            for (k = 0; k < ndim; k++)
                gradU[i][j] += kneg / n * K_nj[j][k] * g[k];
    }
#if 0
    free(g);
#endif
}

double
dss_calc_U_het(double **p, uint n, uint ndim, double khet,
               uint het_window, uint *pairs)
{
    uint i, j, k, window_start, window_end;
    double U = 0, window_size = 2 * het_window;
    double d, pi_len2, pj_len2;
    for (i = 0; i < n; i++) {
        if (i >= het_window)
            window_start = i - het_window;
        else
            window_start = 0;
        if (i + het_window < n)
            window_end = i + het_window;
        else
            window_end = n - 1;

        for (j = window_start; j <= window_end; j++) {
            if (i == j)
                continue;
            /* U += khet / window_size * p[i]^T * p[j] / ( |p[i]| * |p[j]| )
               or
               U += khet / window_size * cos(p[i], p[j]) */

            /* TODO: could just do U += (p[i] - p[j])^2 */
            d = 0;
            pi_len2 = 0;
            pj_len2 = 0;
            for (k = 0; k < ndim; k++) {
                /* TODO: divide by actual window size here ?
                   (window_end - window_start) (edge effects at start/end of seq) */
                d += p[i][k] * p[j][k];
                pi_len2 += p[i][k] * p[i][k];
                pj_len2 += p[j][k] * p[j][k];
            }
            U += khet / window_size * d / (sqrt(pi_len2 * pj_len2));
        }
    }
    return U;
}

void
dss_calc_gradU_het(double **p, uint n, uint ndim, double khet,
                   uint het_window, uint *pairs, double **gradU)
{
    uint i, j, k, window_start, window_end;
    double window_size = 2 * het_window;
    double d, pi_len2, pj_len2;
    for (i = 0; i < n; i++) {
        if (i >= het_window)
            window_start = i - het_window;
        else
            window_start = 0;
        if (i + het_window < n)
            window_end = i + het_window;
        else
            window_end = n - 1;
        for (j = window_start; j <= window_end; j++) {
            if (i == j)
                continue;

            /* grad_x U = khet / window_size * (p[j] - p[i]^T .dot. p[j] * p[i] / |p[i]|^2) / ( |p[i]|* |p[j]| )
             */
            d = 0;
            pi_len2 = 0;
            pj_len2 = 0;
            for (k = 0; k < ndim; k++) {
                d += p[i][k] * p[j][k];
                pi_len2 += p[i][k] * p[i][k];
                pj_len2 += p[j][k] * p[j][k];
            }

            for (k = 0; k < ndim; k++) {
                gradU[i][k] += khet / window_size
                    * (1 / sqrt(pi_len2 * pj_len2)) * (p[j][k] / sqrt(pj_len2) - d / pi_len2 * p[i][k]);

                gradU[j][k] += khet / window_size
                    * (1 / sqrt(pi_len2 * pj_len2)) * (p[i][k] / sqrt(pi_len2) - d / pj_len2 * p[j][k]);
            }
        }
    }
}



#if 0
/* idea: correct the gradients so that probability is conserved
   problem might be if individual components are pushed below 0 */
void
dss_correct_gradU(uint n, uint ndim, uint *pairs, double **gradU)
{
    uint i, k;
    double g;
    /* correct loss of conservation of probability */
    for (i = 0; i < n; i++) {
        g = 0;
        for (k = 0; k < ndim; k++)
            g += gradU[i][k];
        for (k = 0; k < ndim; k++)
            gradU[i][k] -= g/ndim;
    }

    /* TODO maybe: correct loss of conservation of base pair probability */
}
#endif



#if 0
double
dss_calc_U_het(double **p, uint n, uint ndim, double khet,
               uint het_window, uint *pairs)
{
    uint i, j, k, window_start, window_end;
    double U = 0, window_size = 2 * het_window;
    for (i = 0; i < n; i++) {
        /*
        if (pairs[i] != NA_UNPAIRED)
            continue;
        */
        if (i >= het_window)
            window_start = i - het_window;
        else
            window_start = 0;
        if (i + het_window < n)
            window_end = i + het_window;
        else
            window_end = n - 1;
        for (j = window_start; j <= window_end; j++) {
            if (i == j)
                continue;
            /* U += khet / window_size * p[i]^T * p[j] */
            for (k = 0; k < ndim; k++)
                /* TODO: divide by actual window size here instead of later (before return) ?
                   (window_end - window_start) (edge effects at start/end) */
                U += khet / window_size * p[i][k] * p[j][k];
        }
    }
    return U;
}

void
dss_calc_gradU_het(double **p, uint n, uint ndim, double khet,
                   uint het_window, uint *pairs, double **gradU)
{
    uint i, j, k, window_start, window_end;
    double window_size = 2 * het_window;
    for (i = 0; i < n; i++) {
        /*
        if (pairs[i] != NA_UNPAIRED)
            continue;
        */
        if (i >= het_window)
            window_start = i - het_window;
        else
            window_start = 0;
        if (i + het_window < n)
            window_end = i + het_window;
        else
            window_end = n - 1;
        for (j = window_start; j <= window_end; j++) {
            if (i == j)
                continue;
            for (k = 0; k < ndim; k++) {
                gradU[i][k] += khet * p[j][k] / window_size;
                gradU[j][k] += khet * p[i][k] / window_size;
            }
        }
    }
}
#endif
