#include "dss.h"
#include "na.h"
#include "ctools/dary.h"
#include "helpers-for-main.h"
#include "helpers-for-main-opt.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* TODO: pull this from a header file */
#define NDIM 4

int main(int argc, char **argv)
{
    uint n, seq_len;
    char *vienna, *seq;
    double **p, G, G_nn, G_pa, G_pi, G_pur, G_neg;
    double kpi, kpa, kneg, kpur, khet;
    uint het_window;
    double **K_nj = helper_make_K_nj_alloc(NDIM);
    struct nn_inter *inter;
    set_dss_force_constants_defaults(&kpi, &kpa, &kneg, &kpur, &khet, &het_window);

    if (argc != 3) {
        printf("ERROR: wrong number of arguments\n"
               "usage: %s vienna seq\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    vienna = argv[1];
    seq = argv[2];
    n = strlen(vienna);
    seq_len = strlen(seq);
    if (n != seq_len) {
        printf("ERROR: structure and sequence have different lengths"
               " (lengths %u and %u)\n", n, seq_len);
        exit(EXIT_FAILURE);
    }

    inter = nn_inter_xnew(n);
    xvienna_to_pairs(n, vienna, inter->pairs);
    find_interactions(inter);

    printf("vienna = %s\n", vienna);
    printf("seq    = %s\n", seq);
    printf("kpi        = %f\n", kpi);
    printf("kpa        = %f\n", kpa);
    printf("kpur       = %f\n", kpur);
    printf("kneg       = %f\n", kneg);
    printf("khet       = %f\n", khet);
    printf("het_window = %u\n", het_window);
    printf("\n");
    print_interactions(inter);
    printf("\n");

    print_design_score_info_for_seq(inter, seq, n, NDIM, K_nj, kpi, kpa,
                                    kpur, kneg, khet, het_window);
    nn_inter_delete(inter);
    return EXIT_SUCCESS;
}
