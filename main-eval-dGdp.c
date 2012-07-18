#include "na.h"
#include "dss.h"
#include "helpers-for-main.h"
#include "helpers-for-main-opt.h"
#include "ctools/libctools.h"
#include "ctools/dary.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* check gradients by numerically integrating from one pure sequence
   to another in a fixed number of steps, changing one base at a
   time */

/* TODO: check each gradient term individually */


/* TODO: pull this from a header file */
#define NDIM 4

static double
my_calc_G_pseq(struct nn_inter *inter, double **p,
               uint n, uint ndim, double kpa, double kpi,
               double kneg, double **K_nj,
               double khet, uint het_window, double kpur)
{
    double G = 0;
    G  = calc_interactions_pseq(inter, p);
    G += dss_calc_U_negdesign_nj(p, n, ndim, kneg, K_nj, inter->pairs);
    G += dss_calc_U_het(p, n, ndim, khet, het_window, inter->pairs);
    G += dss_calc_U_pa(p, n, ndim, kpa);
    G += dss_calc_U_pi(p, n, ndim, kpi);
    G += dss_calc_U_pur_cauchy(p, n, ndim, kpur);
    return G;
}

static void
my_calc_dGdp_pseq(struct nn_inter *inter, double **p, double **dGdp,
                  uint n, uint ndim, double kpa, double kpi,
                  double kneg, double **K_nj,
                  double khet, uint het_window, double kpur)
{
    calc_interactions_dGdp_pseq(inter, p, dGdp);
    dss_calc_gradU_negdesign_nj(p, n, ndim, kneg, K_nj, inter->pairs, dGdp);

    dss_calc_gradU_het(p, n, ndim, khet, het_window, inter->pairs, dGdp);

#if 0
    for (uint i = 0; i < n; i++) {
        for (uint j = 0; j < ndim; j++) {
            printf("%5.2f ", dGdp[i][j]);
        }
        printf("\n");
    }
#endif

    dss_calc_gradU_pa(p, n, ndim, kpa, dGdp);
    dss_calc_gradU_pi(p, n, ndim, kpi, dGdp);
    dss_calc_gradU_pur_cauchy(p, n, ndim, kpur, dGdp);
}

double
integrate_from_pseq_to_pseq(struct nn_inter *inter, uint n,
                            double **p, double **p2, uint nsteps,
                            double kpa, double kpi,
                            double kneg, double **K_nj,
                            double khet, uint het_window, double kpur)
{
    uint i, j1, j2, k;
    double dG = 0, **dGdp;
    xmalloc2d_one_chunk(dGdp, n, NDIM);

    for (i = 0; i < n; i++) {
        if (dary_equal(p[i], p2[i], NDIM))
            continue;
        /* find index that is one -- assumes the sequences are pure */
        /* TODO: make this more generic by using the difference vector
                 p[i] - p2[i] */
        for (j1 = 0; j1 < NDIM; j1++)
            if (p[i][j1] == 1)
                break;
        for (j2 = 0; j2 < NDIM; j2++)
            if (p2[i][j2] == 1)
                break;

        /* TODO: two ways to move between sequences 
         1. change both p[i][j1], p[i][j2] simultaneously 
            good: works, bad: needs many integration steps
         2. first move p[i][j1], afterwards move p[i][j2] up
            good: fast, bad: doesn't work with het term as it blows up if p[i] = 0 */

        /* move p1[i][j1] down, p1[i][j2] up */
        for (k = 0; k < nsteps; k++) {
            dary2d_set(n, NDIM, dGdp, 0);
            my_calc_dGdp_pseq(inter, p, dGdp,
                              n, NDIM, kpa, kpi,
                              kneg, K_nj,
                              khet, het_window, kpur);
            dG += - (1.0 / nsteps) * dGdp[i][j1];
            p[i][j1] -= 1.0 / nsteps;
            dG += (1.0 / nsteps) * dGdp[i][j2];
            p[i][j2] += 1.0 / nsteps;
        }
        p[i][j1] = 0;
        p[i][j2] = 1;

#if 0
        /* move p1[i][j1] down */
        for (k = 0; k < nsteps; k++) {
            dary2d_set(n, NDIM, dGdp, 0);
            my_calc_dGdp_pseq(inter, p, dGdp,
                              n, NDIM, kpa, kpi,
                              kneg, K_nj,
                              khet, het_window, kpur);
            dG += - (1.0 / nsteps) * dGdp[i][j1];
            p[i][j1] -= 1.0 / nsteps;
        }
        p[i][j1] = 0;
        /* move p1[i][j2] up */
        for (k = 0; k < nsteps; k++) {
            dary2d_set(n, NDIM, dGdp, 0);
            my_calc_dGdp_pseq(inter, p, dGdp,
                              n, NDIM, kpa, kpi,
                              kneg, K_nj,
                              khet, het_window, kpur);
            dG += (1.0 / nsteps) * dGdp[i][j2];
            p[i][j2] += 1.0 / nsteps;
        }
        p[i][j2] = 1;



#endif
    }

    free(dGdp);
    return dG;
}

int main(int argc, char **argv)
{
    uint n, seq1_len, seq2_len, nsteps;
    char *vienna, *seq1, *seq2;
    double **p1, **p2, G1, G2, dG;
    struct nn_inter *inter;

    double kpa, kpi, kneg, **K_nj, khet, kpur;
    uint het_window;
    set_dss_force_constants_defaults(&kpi, &kpa, &kneg, &kpur, &khet, &het_window);
    K_nj = helper_make_K_nj_alloc(NDIM);

    kpi = 0;
    kpa = 0;
    kneg = 0;
    kpur = 0;
    khet = 1000;

    if (argc != 5) {
        printf("ERROR: wrong number of arguments\n"
               "usage: %s vienna seq1 seq2 nsteps\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    vienna = argv[1];
    seq1 = argv[2];
    seq2 = argv[3];
    nsteps = atoi(argv[4]);
    n = strlen(vienna);
    seq1_len = strlen(seq1);
    seq2_len = strlen(seq2);
    if (n != seq1_len || n != seq2_len) {
        printf("ERROR: structure and sequences have different lengths"
               " (lengths %u, %u and %u)\n", n, seq1_len, seq2_len);
        exit(EXIT_FAILURE);
    }

    inter = nn_inter_xnew(n);
    xvienna_to_pairs(n, vienna, inter->pairs);
    find_interactions(inter);

    printf("vienna = %s\n", vienna);
    printf("seq1   = %s\n", seq1);
    printf("seq2   = %s\n", seq2);
    printf("\n");
    print_interactions(inter);
    printf("\n");

    printf("kpi         = %f\n", kpi);
    printf("kpa         = %f\n", kpa);
    printf("kneg        = %f\n", kneg);
    printf("khet        = %f\n", khet);
    printf("het_window  = %u\n", het_window);
    printf("kpur        = %f\n", kpur);


    xmalloc2d_one_chunk(p1, n, NDIM);
    xmalloc2d_one_chunk(p2, n, NDIM);

    xstr_to_pseq(n, NDIM, seq1, p1);
    xstr_to_pseq(n, NDIM, seq2, p2);
    G1 = my_calc_G_pseq(inter, p1,
                        n, NDIM, kpa, kpi,
                        kneg, K_nj,
                        khet, het_window, kpur);
    G2 = my_calc_G_pseq(inter, p2,
                        n, NDIM, kpa, kpi,
                        kneg, K_nj,
                        khet, het_window, kpur);
    printf("G1 = %5.2f\n", G1 * 0.01);
    printf("G2 = %5.2f\n", G2 * 0.01);

    dG = integrate_from_pseq_to_pseq(inter, n, p1, p2, nsteps,
                                     kpa, kpi,
                                     kneg, K_nj,
                                     khet, het_window, kpur);
    printf("G2 - G1         = %5.2f\n", (G2 - G1) * 0.01);
    printf("dG (integrated) = %5.2f\n", dG * 0.01);

    free(K_nj);
    free(p1);
    free(p2);
    nn_inter_delete(inter);
    return EXIT_SUCCESS;
}
