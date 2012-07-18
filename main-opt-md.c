#include "na.h"
#include "nj_param.h"
#include "dss.h"
#include "helpers-for-main.h"
#include "helpers-for-main-opt.h"
#include "md/md.h"
#include "ctools/libctools.h"
#include "ctools/dary.h"
#include "ctools/random.h"
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <getopt.h>

/* TODO: duplications in the helper functions from main-opt-sd.c, put into library */

/* TODO: pull this stuff from some header file */
const uint ndim = 4;

static
uint
x_parse_seq_constraints_hard(uint n, uint *hard, char *constraint_str,
                             uint *pairs)
{
    uint i, j, n_constr = 0;
    char ci, cj;
    if (! constraint_str) {
        for (i = 0; i < n; i++)
            hard[i] = NA_BASE_N;
        return 0;
    }
    for (i = 0; i < n; i++) {
        hard[i] = NA_BASE_N;
        ci = toupper(constraint_str[i]);
        /* skip over unconstrained positions */
        if (ci == '.' || ci == 'N')
            continue;
        /* we have a constraint */
        n_constr++;
        switch (ci) {
        case 'A':
            hard[i] = NA_BASE_A;
            break;
        case 'C':
            hard[i] = NA_BASE_C;
            break;
        case 'G':
            hard[i] = NA_BASE_G;
            break;
        case 'U':
            hard[i] = NA_BASE_U;
            break;
        default:
            printf("ERROR: illegal character '%c' in hard sequence"
                   " constraints\n", ci);
            exit(EXIT_FAILURE);
            break;
        }
        /* check that the constraint is satisfiable with target
           structure (no impossible base pairs) */
        j = pairs[i];
        if (j != NA_UNPAIRED && i > j) {
            /* TODO: factor out test above as a library function */
            /* this is the closing base of a base pair */
            cj = constraint_str[j];
            /* continue if base pair partner is unconstrained */
            if (hard[j] == NA_BASE_N)
                continue;
            if (NA_2BASES_TO_PAIRTYPE[hard[j]][hard[i]] == NA_UNDEF) {
                /* TODO: factor out this function is_legal_base_pair() */
                /* this base pair is unsatisfiable */
                printf("ERROR: illegal base pair (%c%u, %c%u) cannot be satisfied.\n",
                       cj, j, ci, i);
                exit(EXIT_FAILURE);
            }
        }
    }
    return n_constr;
}

static int
run_md(char *vienna, char *seq_constraints_hard,
       uint nsteps, uint nprint, uint ncool, uint npur,
       double timestep, double T_start, double kpi, double kpa, double kneg,
       double khet, uint het_window, double kpur_end,
       bool do_exp_cool, bool do_movie_output)
{
    uint i, j, step, n, *hard_constraints, n_hard_constr;
    double G, Gnn, **p, **v, **dGdp, *mass, kb = 1, ndof,
        T, T_target, T_end = 0.1, cool_rate, kpur = 0, pur_rate,
        ekin, estart = 0;
    double **K_nj = helper_make_K_nj_alloc(ndim);
    char *seq;
    size_t nfixed_bp;
    struct nn_inter *inter;

    n = strlen(vienna);
    inter = nn_inter_xnew(n);
    xvienna_to_pairs(n, vienna, inter->pairs);
    find_interactions(inter);

    hard_constraints = xmalloc(n * sizeof(*hard_constraints));
    n_hard_constr = x_parse_seq_constraints_hard(n, hard_constraints, seq_constraints_hard, inter->pairs);

    ndof = n * ndim - (n_hard_constr * ndim);    /* TODO: or ndof = n * (ndim - 1); ? */
    xmalloc2d_one_chunk(p, n, ndim);
    xmalloc2d_one_chunk(v, n, ndim);
    xmalloc2d_one_chunk(dGdp, n, ndim);
    mass = xmalloc(n * sizeof(*mass));
    seq = xmalloc((n + 1) * sizeof(*seq));

    dary2d_set(n, ndim, p, 1.0 / ndim);
    dary_set(mass, n, 1.0);
    md_init_velocities_random(n, ndim, v, mass, kb, ndof, T_start);

    if (do_exp_cool)
        cool_rate = pow(T_end / T_start, 1.0 / (nsteps - ncool));
    else
        cool_rate = (T_end - T_start) / (nsteps - ncool);
    /* only linear purification schedule */
    pur_rate = kpur_end / (nsteps - npur);

    printf("target           = %s\n", vienna);
    printf("n_hard_constr    = %u\n", n_hard_constr);
    printf("hard constraints = %s\n", seq_constraints_hard);
    printf("\n");
    printf("timestep    = %f\n", timestep);
    printf("T-start     = %f\n", T_start);
    printf("T-end       = %f\n", T_end);
    printf("cool_type   = %s\n", do_exp_cool ? "exponential" : "linear");
    printf("cool_rate   = %f\n", cool_rate);
    printf("het_window  = %u\n", het_window);
    printf("kpi         = %f\n", kpi);
    printf("kpa         = %f\n", kpa);
    printf("kneg        = %f\n", kneg);
    printf("khet        = %f\n", khet);
    printf("kpur_end    = %f\n", kpur_end);
    printf("pur_rate    = %f\n", pur_rate);
    printf("\n");
    printf("time-total  = %f\n", nsteps * timestep);
    printf("time-print  = %f\n", nprint * timestep);
    printf("time-cool   = %f\n", ncool  * timestep);
    printf("time-pur    = %f\n", npur   * timestep);
    printf("nsteps      = %u\n", nsteps);
    printf("nprint      = %u\n", nprint);
    printf("ncool       = %u\n", ncool);
    printf("npur        = %u\n", npur);
    printf("\n");
    printf("ndof        = %f\n", ndof);
    printf("\n");

    printf("    step        time            G_nn          Epot    "
           "      Ekin         Etotal           T         delta E"
           "      \"entropy\"         kpur\n");
    printf("----------  ------------    ------------  ------------"
           "  ------------  ------------  ------------  ------------"
           "  ------------  ------------\n");
    printf("START\n");

    T_target = T_start;
    for (step = 0; step <= nsteps; step++) {
        dary2d_set(n, ndim, dGdp, 0);

        /* calculate gradient dGdp */
        calc_interactions_dGdp_pseq(inter, p, dGdp);
        dss_calc_gradU_pa(p, n, ndim, kpa, dGdp);
        dss_calc_gradU_pi(p, n, ndim, kpi, dGdp);
        dss_calc_gradU_pur_cauchy(p, n, ndim, kpur, dGdp);
        dss_calc_gradU_negdesign_nj(p, n, ndim, kneg, K_nj, inter->pairs, dGdp);
        dss_calc_gradU_het(p, n, ndim, khet, het_window, inter->pairs, dGdp);

        /* periodic printouts */
        if (step % nprint == 0) {
            G = 0;
            G = Gnn = calc_interactions_pseq(inter, p);
            G += dss_calc_U_pa(p, n, ndim, kpa);
            G += dss_calc_U_pi(p, n, ndim, kpi);
            G += dss_calc_U_pur_cauchy(p, n, ndim, kpur);
            G += dss_calc_U_negdesign_nj(p, n, ndim, kneg, K_nj, inter->pairs);
            G += dss_calc_U_het(p, n, ndim, khet, het_window, inter->pairs);

            /* TODO: factor this out into function */
            ekin = 0;
            for (i = 0; i < n; i++)
                for (j = 0; j < ndim; j++)
                    ekin += mass[i] * v[i][j] * v[i][j];
            ekin *= 0.5;

            if (step == 0)
                estart = G + ekin;

            T = md_calc_temperature(n, ndim, v, mass, kb, ndof);

            if (do_movie_output) {
                print_for_movie(p, n, ndim, seq);
            } else {
                printf("%10u  %12.5f    % 12.3f  % 12.3f  % 12.3f  % 12.3f"
                       "  % 12.3f  % 12.3f  % 12.3f  % 12.3f\n",
                       step, step * timestep, Gnn, G, ekin, G + ekin, T,
                       G + ekin - estart, calc_entropy(p, n, ndim), kpur);
            }
        }

        /* check for numerical explosion */
        if (system_is_exploded(p, n, ndim)) {
            printf("END\n\n");
            show_bad_prob(p, n, ndim);
            printf("\n");
            printf("vienna = %s\n", vienna);
            pseq_to_str(p, n, ndim, seq);
            printf("seq    = %s\n", seq);
            printf("\nstep = %u\n", step);
            printf("BOOOOOM --- system exploded\n");
            return EXIT_FAILURE;
        }

        /* perform integration timestep */
        md_integrate_step_leapfrog(n, ndim, p, v, dGdp, mass, timestep);\

        /* enforce hard constraints */
        for (i = 0; i < n; i++) {
            if (hard_constraints[i] != NA_BASE_N) {
                for (j = 0; j < ndim; j++) {
                    p[i][j] = 0;
                    v[i][j] = 0;
                }
                p[i][hard_constraints[i]] = 1;
            }
        }

        /* cooling and purification schedule */
        if (step >= ncool) {
            if (do_exp_cool)
                T_target *= cool_rate;
            else
                T_target += cool_rate;
        }
        if (step >= npur)
            kpur += pur_rate;

        /* thermostat */
        md_rescale_temperature(n, ndim, v, mass, kb, ndof, T_target);
    }
    printf("END\n\n");

    pseq_to_str(p, n, ndim, seq);
    show_bad_prob(p, n, ndim);
    show_bad_bp(seq, inter->pairs, n);
    printf("before = %s\n", seq);
    printf("fixing bad base pairs\n");
    nfixed_bp = fix_bad_bp(seq, inter->pairs, n);
    printf("nfixed_bp = %zu\n", nfixed_bp);
    printf("\n");

    print_design_score_info_for_seq(inter, seq, n, ndim, K_nj, kpi, kpa,
                                    kpur, kneg, khet, het_window);

    printf("vienna = %s\n", vienna);
    printf("seq    = %s\n", seq);

    free(K_nj);
    nn_inter_delete(inter);
    free(hard_constraints);
    free(seq);
    free(p);
    free(v);
    free(dGdp);
    free(mass);
    return EXIT_SUCCESS;
}

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
           "  --seed s                  set seed of random number generator to s\n",
           progname);
}

int
main(int argc, char **argv)
{
    uint nsteps, nprint, ncool, npur;
    char *vienna = NULL, *seq_constraints_hard = NULL;

    /* default settings */
    bool do_exp_cool = false, do_movie_output = false;
    double timestep = 0.0015, T_start = 40.0,
        time_total = 50.0, time_print = 2.5, time_cool = 0.1 * time_total,
        time_pur = 0.8 * time_total;
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
    printf("\n");
    printf("optimisation by md with cooling\n");
    printf("===============================\n");
    printf("\n");

    printf("seed = %lu\n", seed);
    random_seed(seed);
    return run_md(vienna, seq_constraints_hard, nsteps, nprint, ncool, npur,
                  timestep, T_start, kpi, kpa, kneg, khet, het_window, kpur_end,
                  do_exp_cool, do_movie_output);
}
