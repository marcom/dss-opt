#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "na.h"
#include "ctools/random.h"

int main(int argc, char **argv)
{
    uint i, n, *pairs, *useq;
    char *vienna;

    if (argc != 2 && argc != 3) {
        printf("ERROR: wrong number of arguments\n"
               "usage: %s vienna [seed]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    vienna = argv[1];
    n = strlen(vienna);
    pairs = xmalloc(n * sizeof(*pairs));
    useq = xmalloc(n * sizeof(*useq));

    if (argc == 3) {
        random_seed(atoi(argv[2]));
    } else {
        random_seed_with_current_time();
    }

    xvienna_to_pairs(n, vienna, pairs);
    random_useq(n, pairs, useq);
    /* TODO: move this to useq_to_str function */
    for (i = 0; i < n; i++)
        printf("%c", NA_BASE_NAMES[useq[i]]);
    printf("\n");

    free(pairs);
    free(useq);
    return EXIT_SUCCESS;
}
