#include "na.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
    uint n, seq_len, *useq;
    char *vienna, *seq;
    int G;
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
    useq = xmalloc(n * sizeof(*useq));

    inter = nn_inter_xnew(n);
    xvienna_to_pairs(n, vienna, inter->pairs);
    find_interactions(inter);

    printf("vienna = %s\n", vienna);
    printf("seq    = %s\n", seq);
    printf("\n");
    print_interactions(inter);
    printf("\n");

    xstr_to_seq(n, seq, useq);
    G = calc_interactions(inter, useq);
    printf("G = %5.2f\n", G * 0.01);

    free(useq);
    nn_inter_delete(inter);
    return EXIT_SUCCESS;
}
