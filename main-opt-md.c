#include "opt.h"
#include "helpers-for-main-opt.h"
#include "ctools/random.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <getopt.h>

static void
usage(char *progname)
{
    printf("usage: %s [options] target-structure\n"
           "Options:\n"
           "  --seq-constraints-hard    hard sequence constraints\n"
           "  --timestep t              timestep t for integration\n"
           "  --T-start T               starting temperature T\n"
           "  --time-total t            total simulation time t\n"
           "  --time-print t            print system status every t time units\n"
           "  --time-cool t             start cooling the system at time t\n"
           "  --kpi k                   set kpi constant to k\n"
           "  --kpa k                   set kpa constant to k\n"
           "  --kneg k                  set kneg constant to k\n"
           "  --khet k                  set khet to k\n"
           "  --het-window w            set window that heterogeneity terms is applied to to w bases left and right\n"
           "  --cooling-type            can be either exponential or linear, default is linear\n"
           "  --time-pur t              start purification at time t\n"
           "  --kpur-end k              ending value for kpur\n"
           "  --movie                   activate output that can be used to make a movie out of it\n"
           "  --quiet                   minimise output\n"
           "  --seed s                  set seed of random number generator to s\n",
           progname);
}

int
main(int argc, char **argv)
{
    uint nsteps, nprint, ncool, npur;
    char *vienna = NULL, *seq_constraints_hard = NULL;

    /* default settings */
    bool do_exp_cool = false, do_movie_output = false, verbose = true;
    double timestep   = DSSOPT_DEFAULT_OPT_MD_timestep;
    double T_start    = DSSOPT_DEFAULT_OPT_MD_T_start;
    double time_total = DSSOPT_DEFAULT_OPT_MD_time_total;
    double time_print = DSSOPT_DEFAULT_OPT_MD_time_print;
    double time_cool  = dssopt_default_opt_md_time_cool(time_total);
    double time_pur   = dssopt_default_opt_md_time_pur(time_total);
    bool has_arg_time_cool = false, has_arg_time_pur = false;
    double kpi, kpa, kneg, kpur_end, khet;
    uint het_window;
    ulong seed = random_get_seedval_from_current_time();
    set_dss_force_constants_defaults(&kpi, &kpa, &kneg, &kpur_end, &khet, &het_window);

    /* parse command-line args */
    int c;
    while (1) {
        int opt_idx = 0;
        static struct option long_opts[] = {
            {"timestep",              required_argument, 0, 0},
            {"kpi",                   required_argument, 0, 0},
            {"kpa",                   required_argument, 0, 0},
            {"kneg",                  required_argument, 0, 0},
            {"khet",                  required_argument, 0, 0},
            {"T-start",               required_argument, 0, 0},
            {"time-total",            required_argument, 0, 0},
            {"time-print",            required_argument, 0, 0},
            {"time-cool",             required_argument, 0, 0},
            {"cooling-type",          required_argument, 0, 0},
            {"time-pur",              required_argument, 0, 0},
            {"kpur-end",              required_argument, 0, 0},
            {"movie",                 no_argument,       0, 0},
            {"seq-constraints-hard",  required_argument, 0, 0},
            {"het-window",            required_argument, 0, 0},
            {"seed",                  required_argument, 0, 0},
            {"quiet",                 no_argument,       0, 0},
            {0, 0, 0, 0} /* end marker */
        };

        c = getopt_long(argc, argv, "", long_opts, &opt_idx);
        if (c == -1)
            break;
        switch (c) {
        case 0:
            /* long options (--foo) */
            switch(opt_idx) {
            case 0: timestep = atof(optarg);
                break;
            case 1: kpi = atof(optarg);
                break;
            case 2: kpa = atof(optarg);
                break;
            case 3: kneg = atof(optarg);
                break;
            case 4: khet = atof(optarg);
                break;
            case 5: T_start = atof(optarg);
                break;
            case 6: time_total = atof(optarg);
                break;
            case 7: time_print = atof(optarg);
                break;
            case 8: time_cool = atof(optarg);
                has_arg_time_cool = true;
                break;
            case 9:
                if (strcmp(optarg, "exponential") == 0) {
                    do_exp_cool = true;
                } else if (strcmp(optarg, "linear") == 0) {
                    do_exp_cool = false;
                } else {
                    printf("ERROR: cooling-type can only be exponential or linear\n");
                    exit(EXIT_FAILURE);
                }
                break;
            case 10: time_pur = atof(optarg);
                has_arg_time_pur = true;
                break;
            case 11: kpur_end = atof(optarg);
                break;
            case 12: do_movie_output = true;
                break;
	    case 13: seq_constraints_hard = optarg;
                break;
            case 14: het_window = atoi(optarg);
                break;
            case 15: seed = atol(optarg);
                break;
            case 16: verbose = false;
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
            printf("?? getopt returned character code 0%o ??\n", c);
            exit(EXIT_FAILURE);
            break;
        }
    }
    if (argc - optind == 1) {
        vienna = argv[optind];
    } else {
        printf("%s: wrong number of arguments for target structure\n", argv[0]);
        usage(argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Unless set explicitly, set default time_cool, time_pur depending on total_time */
    if (! has_arg_time_cool) {
        time_cool = dssopt_default_opt_md_time_cool(time_total);
    }
    if (! has_arg_time_pur) {
        time_pur = dssopt_default_opt_md_time_pur(time_total);
    }

    /* sanity checking of command-line args */
    /* TODO: force kneg to be positive ? */
    x_ensure_positive(argv[0], "timestep", timestep);
    x_ensure_positive(argv[0], "kpi", kpi);
    x_ensure_positive(argv[0], "kpa", kpa);
    x_ensure_positive(argv[0], "T-start", T_start);
    x_ensure_positive(argv[0], "time-total", time_total);
    x_ensure_positive(argv[0], "time-print", time_print);
    x_ensure_positive(argv[0], "time-cool", time_cool);
    x_ensure_positive(argv[0], "time-pur", time_pur);

    nsteps = round(time_total / timestep);
    nprint = round(time_print / timestep);
    ncool  = round(time_cool  / timestep);
    npur   = round(time_pur   / timestep);

    if (seq_constraints_hard && strlen(vienna) != strlen(seq_constraints_hard)) {
        printf("ERROR: length of sequence constraints and target structure must be equal\n");
        exit(EXIT_FAILURE);
    }


    /* run */
    if (verbose) {
        printf("\n");
        printf("optimisation by md with cooling\n");
        printf("===============================\n");
        printf("\n");

        printf("seed = %lu\n", seed);
    }
    random_seed(seed);

    char *designed_seq = xmalloc((strlen(vienna) + 1) * sizeof(*designed_seq));
    int status = run_md(vienna, seq_constraints_hard, nsteps, nprint, ncool, npur,
                        timestep, T_start, kpi, kpa, kneg, khet, het_window, kpur_end,
                        do_exp_cool, do_movie_output, verbose, designed_seq);
    if (verbose) {
        printf("seq    = %s\n", designed_seq);
    } else {
        printf("%s", designed_seq);
    }
    free(designed_seq);
    return status;
}
