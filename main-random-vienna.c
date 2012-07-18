#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "na.h"
#include "ctools/random.h"

int main(int argc, char **argv)
{
    uint n, hpmin, *pairs;
    char *vienna;

    if (argc != 3) {
        printf("ERROR: wrong number of arguments\n"
               "usage: %s length hpmin\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    n = atoi(argv[1]);
    hpmin = atoi(argv[2]);
    pairs = xmalloc(n * sizeof(*pairs));
    vienna = xmalloc((n + 1) * sizeof(*vienna));

    random_seed_with_current_time();
    random_pairs(n, pairs, hpmin);
    xpairs_to_vienna(n, pairs, vienna);
    printf("%s\n", vienna);

    free(pairs);
    free(vienna);
    return EXIT_SUCCESS;
}
