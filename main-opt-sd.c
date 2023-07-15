#include "opt.h"
#include "helpers-for-main-opt.h"
#include "ctools/random.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <getopt.h>

static void
usage(char *progname)
{
    printf("usage: %s [options] target-structure\n"
           "Options:\n"
           "  --maxsteps n      do a maximum number of n steepest descent steps\n"
           "  --nprint n        print progress information every n steps\n"
           "  --kpi k           set kpi constant to k\n"
           "  --kpa k           set kpa constant to k\n"
           "  --kneg k          set kneg constant to k\n"
           "  --kpur k          set kpur constant to k\n"
           "  --khet k          set khet to k\n"
           "  --het-window w    set window that heterogeneity terms is applied to to w bases left and right\n"
           "  --wiggle w        set wiggle (initial deviation from equidistribution) to w\n"
           "  --movie           activate output that can be used to make a movie out of it\n"
           "  --quiet           minimise output\n"
           "  --seed s          set seed of random number generator to s\n",
           progname);
}

int
main(int argc, char **argv)
{
    char *vienna = NULL;

    /* default settings */
    uint maxsteps = 20000, nprint = 1000;
    double wiggle = 0.1;
    double kpi, kpa, kneg, kpur, khet;
    uint het_window;
    bool do_movie_output = false, verbose = true;
    ulong seed = random_get_seedval_from_current_time();
    set_dss_force_constants_defaults(&kpi, &kpa, &kneg, &kpur, &khet, &het_window);

    /* parse command-line args */
    int c;
    while (1) {
        int opt_idx = 0;
        static struct option long_opts[] = {
            {"maxsteps",   required_argument, 0, 0},
            {"nprint",     required_argument, 0, 0},
            {"kpi",        required_argument, 0, 0},
            {"kpa",        required_argument, 0, 0},
            {"kneg",       required_argument, 0, 0},
            {"kpur",       required_argument, 0, 0},
            {"wiggle",     required_argument, 0, 0},
            {"movie",      no_argument,       0, 0},
            {"khet",       required_argument, 0, 0},
            {"het-window", required_argument, 0, 0},
            {"seed",       required_argument, 0, 0},
            {"quiet",      no_argument,       0, 0},
            {0, 0, 0, 0} /* end marker */
        };

        c = getopt_long(argc, argv, "", long_opts, &opt_idx);
        if (c == -1)
            break;
        switch(c) {
        case 0:
            /* long options (--foo) */
            switch(opt_idx) {

            case 0: maxsteps = atoi(optarg);
                break;
            case 1: nprint = atoi(optarg);
                break;
            case 2: kpi = atof(optarg);
                break;
            case 3: kpa = atof(optarg);
                break;
            case 4: kneg = atof(optarg);
                break;
            case 5: kpur = atof(optarg);
                break;
            case 6: wiggle = atof(optarg);
                break;
            case 7: do_movie_output = true;
                break;
            case 8: khet = atof(optarg);
                break;
            case 9: het_window = atoi(optarg);
                break;
            case 10: seed = atol(optarg);
                break;
            case 11: verbose = false;
                break;
            default:
                printf("ERROR in getopt parsing\n");
                exit(EXIT_FAILURE);
                break;
            }
            break;
        case '?':
            /* unknown option, getopt has already output an error msg */
            usage(argv[0]);
            exit(EXIT_FAILURE);
            break;
        default:
            printf ("?? getopt returned character code 0%o ??\n", c);
            exit(EXIT_FAILURE);
            break;
        }
    }
    if (argc - optind == 1) {
        vienna = argv[optind];
    } else {
        printf("%s: wrong number of arguments for target structure\n",
               argv[0]);
        usage(argv[0]);
        exit(EXIT_FAILURE);
    }

    /* sanity checking of command-line args */
    /* TODO: force kneg, wiggle to be positive ? */
    x_ensure_positive(argv[0], "kpi", kpi);
    x_ensure_positive(argv[0], "kpa", kpa);

    /* run */
    if (verbose) {
        printf("optimisation by steepest descent\n");
        printf("seed = %lu\n", seed);
    }
    random_seed(seed);
    char *designed_seq = xmalloc((strlen(vienna) + 1) * sizeof(*designed_seq));
    int status = run_sd(vienna, maxsteps, nprint, wiggle, kpi, kpa, kpur, kneg,
                      khet, het_window, do_movie_output, verbose, designed_seq);
    if (verbose) {
        printf("seq    = %s\n", designed_seq);
    } else {
        printf("%s", designed_seq);
    }
    free(designed_seq);
    return status;
}
