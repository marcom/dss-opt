#include "na.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
    uint n, seq_len, *useq;
    char *vienna, *seq;
    int G;
    bool quiet = false;
    struct nn_inter *inter;

    if (argc == 3) {
        vienna = argv[1];
        seq = argv[2];
    } else if (argc == 4 && (strcmp(argv[1], "-q") == 0)) {
        quiet = true;
        vienna = argv[2];
        seq = argv[3];
    } else {
        printf("ERROR: wrong number of arguments\n"
               "usage: %s [-q] vienna seq\n", argv[0]);
        exit(EXIT_FAILURE);
    }
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

    if (! quiet) {
        printf("vienna = %s\n", vienna);
        printf("seq    = %s\n", seq);
        printf("\n");
        print_interactions(inter);
        printf("\n");
    }

    xstr_to_useq(n, seq, useq);
    G = calc_interactions_useq(inter, useq);
    if (! quiet) {
        printf("G = ");
    }
    printf("%5.2f\n", G * 0.01);

    free(useq);
    nn_inter_delete(inter);
    return EXIT_SUCCESS;
}
