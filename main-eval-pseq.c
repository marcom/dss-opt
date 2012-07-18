#include "na.h"
#include "ctools/dary.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* TODO: pull this from a header file */
#define NDIM 4

int main(int argc, char **argv)
{
    uint n, seq_len;
    char *vienna, *seq;
    double **p, G;
    struct nn_inter *inter;

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
    printf("\n");
    print_interactions(inter);
    printf("\n");

    xmalloc2d_one_chunk(p, n, NDIM);

    dary2d_set(n, NDIM, p, 0.0);
    xstr_to_pseq(n, NDIM, seq, p);
    G = calc_interactions_pseq(inter, p);
    printf("G = %5.2f\n", G * 0.01);

    free(p);
    nn_inter_delete(inter);
    return EXIT_SUCCESS;
}
