#include "dss.h"
#include "opt.h"
#include "helpers-for-main-opt.h"
#include <stdio.h>

void
x_ensure_positive(char *progname, char *optname, double val)
{
    if (! (val > 0)) {
        printf("%s: %s must be > 0 (is %f)\n",
               progname, optname, val);
        exit(EXIT_FAILURE);
    }
}

void
set_dss_force_constants_defaults(double *kpi, double *kpa, double *kneg,
                                 double *kpur, double *khet, uint *het_window)
{
    *kpi  = DSSOPT_DEFAULT_kpi;
    *kpa  = DSSOPT_DEFAULT_kpa;
    *kneg = DSSOPT_DEFAULT_kneg;
    *kpur = DSSOPT_DEFAULT_kpur;
    *khet = DSSOPT_DEFAULT_khet;
    *het_window = DSSOPT_DEFAULT_het_window;
}

void
print_design_score_info_for_seq(struct nn_inter *inter, char *seq,
                                uint n, uint ndim, double **K_nj, double kpi,
                                double kpa, double kpur, double kneg,
                                double khet, uint het_window)
{
    double **p, score_total, score_nn, score_pa, score_pi, score_pur, score_neg, score_het;
    xmalloc2d_one_chunk(p, n, ndim);
    xstr_to_pseq(n, ndim, seq, p);

    printf("design score\n");
    /* print out constants as these will of course influence the score
       and they might have changed from the beginning of the
       optimisation (e.g. in the case of opt-md) */
    printf("kpi         = %f\n", kpi);
    printf("kpa         = %f\n", kpa);
    printf("kpur        = %f\n", kpur);
    printf("kneg        = %f\n", kneg);
    printf("khet        = %f\n", khet);
    printf("het_window  = %u\n", het_window);
    score_nn  = calc_interactions_pseq(inter, p);
    score_pa  = dss_calc_U_pa(p, n, ndim, kpa);
    score_pi  = dss_calc_U_pi(p, n, ndim, kpi);
    score_pur = dss_calc_U_pur_cauchy(p, n, ndim, kpur);
    score_neg = dss_calc_U_negdesign_nj(p, n, ndim, kneg, K_nj, inter->pairs);
    score_het = dss_calc_U_het(p, n, ndim, khet, het_window, inter->pairs);
    score_total = score_nn + score_pa + score_pi + score_pur + score_neg + score_het;
    printf("score_nn    = % 5.2f\n", score_nn * 0.01);
    printf("score_pa    = % 5.2f\n", score_pa * 0.01);
    printf("score_pi    = % 5.2f\n", score_pi * 0.01);
    printf("score_pur   = % 5.2f\n", score_pur * 0.01);
    printf("score_neg   = % 5.2f\n", score_neg * 0.01);
    printf("score_het   = % 5.2f\n", score_het * 0.01);
    printf("score_total = % 5.2f\n", score_total * 0.01);
    printf("\n");

    free(p);
}
