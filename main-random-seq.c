#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "na.h"
#include "ctools/random.h"

int main(int argc, char **argv)
{
    uint i, n, *pairs, *useq;
    char *vienna, *str;

    if (argc != 2 && argc != 3) {
        printf("ERROR: wrong number of arguments\n"
               "usage: %s vienna [seed]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    vienna = argv[1];
    n = strlen(vienna);
    pairs = xmalloc(n * sizeof(*pairs));
    useq = xmalloc(n * sizeof(*useq));
    str = xmalloc((n + 1) * sizeof(*str));

    if (argc == 3) {
        random_seed(atoi(argv[2]));
    } else {
        random_seed_with_current_time();
    }

    xvienna_to_pairs(n, vienna, pairs);
    random_useq(n, pairs, useq);
    xuseq_to_str(n, useq, str);
    printf("%s\n", str);

    free(str);
    free(pairs);
    free(useq);
    return EXIT_SUCCESS;
}
