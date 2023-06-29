/* TODO: needed headers copied from main-opt-md.c, check if all are needed */
#include "na.h"
#include "dss.h"
#include "helpers-for-main.h"
#include "helpers-for-main-opt.h"
#include "ctools/libctools.h"
#include "ctools/dary.h"
#include "ctools/num_utils.h"
#include "ctools/random.h"
#include <gsl/gsl_multimin.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <getopt.h>

/* TODO: duplications in the helper functions from main-opt-md.c, put into library */

/* TODO: pull this stuff from some header file */
const uint ndim = 4;



struct dss_params {
    uint n, ndim, het_window;
    double **p, **dGdp;
    double kpa, kpi, kneg, kpur, **K_nj, khet;
    struct nn_inter *inter;
};

/* the function to optimize */
double
my_f(const gsl_vector *v, void *params)
{
#if 0
    printf("in f\n");
#endif
    double G;
    struct dss_params *d = params;
    /* copy v to d->p */
    uint i, j, k = 0;
    for (i = 0; i < d->n; i++) {
        for (j = 0; j < d->ndim; j++) {
            d->p[i][j] = gsl_vector_get(v, k);
            k++;
        }
    }

#if 0
    printf("f: before calc\n");
#endif
    G = calc_interactions_pseq(d->inter, d->p);
    G += dss_calc_U_pa(d->p, d->n, d->ndim, d->kpa);
    G += dss_calc_U_pi(d->p, d->n, d->ndim, d->kpi);
    G += dss_calc_U_pur_cauchy(d->p, d->n, d->ndim, d->kpur);
    G += dss_calc_U_negdesign_nj(d->p, d->n, d->ndim, d->kneg, d->K_nj, d->inter->pairs);
    G += dss_calc_U_het(d->p, d->n, d->ndim, d->khet, d->het_window, d->inter->pairs);
#if 0
    printf("f: after calc\n");
#endif
    return G;
}

/* the gradient of the function to optimize */
void
my_df(const gsl_vector *v, void *params, gsl_vector *df)
{
#if 0
    printf("in df\n");
#endif
    struct dss_params *d = params;
    /* copy v to d->p */
    uint i, j, k = 0;
    for (i = 0; i < d->n; i++)
        for (j = 0; j < d->ndim; j++) {
            d->p[i][j] = gsl_vector_get(v, k);
            k++;
        }

    dary2d_set(d->n, d->ndim, d->dGdp, 0);
    calc_interactions_dGdp_pseq(d->inter, d->p, d->dGdp);
    dss_calc_gradU_pa(d->p, d->n, d->ndim, d->kpa, d->dGdp);
    dss_calc_gradU_pi(d->p, d->n, d->ndim, d->kpi, d->dGdp);
    dss_calc_gradU_pur_cauchy(d->p, d->n, d->ndim, d->kpur, d->dGdp);
    dss_calc_gradU_negdesign_nj(d->p, d->n, d->ndim, d->kneg, d->K_nj, d->inter->pairs, d->dGdp);
    dss_calc_gradU_het(d->p, d->n, d->ndim, d->khet, d->het_window, d->inter->pairs, d->dGdp);

    /* copy d->dGdp to df */
    k = 0;
    for (i = 0; i < d->n; i++)
        for (j = 0; j < d->ndim; j++) {
            gsl_vector_set(df, k, d->dGdp[i][j]);
            k++;
        }
}

/* calculate gradient and function value in one go */
void
my_fdf(const gsl_vector *x, void *params, double *f, gsl_vector *df)
{
    *f = my_f(x, params);
    my_df(x, params, df);
}


/* steepest descent optimisation */
/* TODO: calc of energies and derivatives the same as in md code,
   other overlaps as well */
static int
run_sd(char *vienna, uint maxsteps, uint nprint, double wiggle,
       double kpi, double kpa, double kpur, double kneg, double khet,
       uint het_window, bool do_movie_output, bool verbose,
       char **designed_seq)
{
    uint i, j, step, n;
    double G, Gnn, **p, **dGdp, Gstart = 0, Gold, delta, tmp;
    double scale = 0.01, scale_min = 1e-8,
        scale_decrease = 0.5, scale_increase = 1.05;
    double **K_nj = helper_make_K_nj_alloc(ndim);
    size_t nfixed_bp;
    struct nn_inter *inter;

    n = strlen(vienna);
    inter = nn_inter_xnew(n);
    xvienna_to_pairs(n, vienna, inter->pairs);
    find_interactions(inter);

    xmalloc2d_one_chunk(p, n, ndim);
    xmalloc2d_one_chunk(dGdp, n, ndim);

    /* random perturbation (wiggle) from equidistribution */
    dary2d_set(n, ndim, p, 1.0 / ndim);
    for (i = 0; i < n; i++) {
        for (j = 0; j < ndim; j++)
            p[i][j] += wiggle * (2 * random_double_uniform() - 1);
    }
    /* rescale probabilities so that p[i][j] in [0,1] and \sum_j p[i][j] = 1 */
    for (i = 0; i < n; i++) {
        tmp = 0;
        for (j = 0; j < ndim; j++) {
            p[i][j] = MAX(p[i][j], 0);
            p[i][j] = MIN(p[i][j], 1);
            tmp += p[i][j];
        }
        for (j = 0; j < ndim; j++)
            p[i][j] /= tmp;
    }        

    if (verbose) {
        printf("target     = %s\n", vienna);
        printf("wiggle     = %f\n", wiggle);
        printf("kpi        = %f\n", kpi);
        printf("kpa        = %f\n", kpa);
        printf("kpur       = %f\n", kpur);
        printf("kneg       = %f\n", kneg);
        printf("khet       = %f\n", khet);
        printf("het_window = %u\n", het_window);
        printf("\n");
        printf("maxsteps   = %u\n", maxsteps);
        printf("nprint     = %u\n", nprint);
        printf("\n");
    }

    struct dss_params params;
    params.n = n;
    params.ndim = ndim;
    params.het_window = het_window;
    params.p = p;
    params.dGdp = dGdp;
    params.kpa = kpa;
    params.kpi = kpi;
    params.kneg = kneg;
    params.kpur = kpur;
    params.K_nj = K_nj;
    params.khet = khet;
    params.inter = inter;

    gsl_multimin_function_fdf my_func;
    my_func.n = n * ndim;
    my_func.f = my_f;
    my_func.df = my_df;
    my_func.fdf = my_fdf;
    my_func.params = &params;

    gsl_vector *x, *old_x;
    x = gsl_vector_alloc(n * ndim);
    old_x = x;
    /* copy p to x */
    uint k = 0;
    for (i = 0; i < n; i++) {
        for (j = 0; j < ndim; j++) {
            gsl_vector_set(x, k, p[i][j]);
            k++;
        }
    }

    gsl_multimin_fdfminimizer *sd;
    sd = gsl_multimin_fdfminimizer_alloc(gsl_multimin_fdfminimizer_steepest_descent, n * ndim);
    gsl_multimin_fdfminimizer *cg;
    cg = gsl_multimin_fdfminimizer_alloc(gsl_multimin_fdfminimizer_conjugate_pr, n * ndim);
    double step_size = 0.01;
    double sd_tol = 1e-1;
    double sd_tol_gradient = 1e-0;
    double cg_tol = 1e-1;
    double cg_tol_gradient = 1e-0;

    uint cycle, ncycles = 2, iter, alliter = 0;
    uint maxsteps_sd = 1000;
    uint maxsteps_cg = 1000;
    int status;
    for (cycle = 0; cycle < ncycles; cycle++) {
        /* steepest descent */
        if (verbose) {
            printf ("'%s' minimizer\n", gsl_multimin_fdfminimizer_name(sd));
        }
        gsl_multimin_fdfminimizer_set(sd, &my_func, x, step_size, sd_tol);
        iter = 0;
        do {
            status = gsl_multimin_fdfminimizer_iterate(sd);
            if (status)
                break;
            status = gsl_multimin_test_gradient(sd->gradient, sd_tol_gradient);
            if (verbose) {
                if (status == GSL_SUCCESS)
                    printf ("SD: Minimum found at:\n");
                if (iter % nprint == 0)
                    printf ("SD: %7d % 12.4f\n", iter, sd->f);
            }
            iter++;
            alliter++;
        } while (status == GSL_CONTINUE && iter < maxsteps_sd && alliter < maxsteps);
        if (verbose) {
            printf ("SD: %7d % 12.4f\n", iter, sd->f);
        }
        x = sd->x;

        /* conjugate gradients */
        if (verbose) {
            printf ("'%s' minimizer\n", gsl_multimin_fdfminimizer_name(cg));
        }
        gsl_multimin_fdfminimizer_set(cg, &my_func, x, step_size, cg_tol);
        iter = 0;
        do {
            status = gsl_multimin_fdfminimizer_iterate(cg);
            if (status)
                break;
            status = gsl_multimin_test_gradient(cg->gradient, cg_tol_gradient);
            if (verbose) {
                if (status == GSL_SUCCESS)
                    printf ("CG: Minimum found at:\n");
                if (iter % nprint == 0)
                    printf ("CG: %7d % 12.4f\n", iter, cg->f);
            }
            iter++;
            alliter++;
        } while (status == GSL_CONTINUE && iter < maxsteps_cg && alliter < maxsteps);
        if (verbose) {
            printf ("CG: %7d % 12.4f\n", iter, cg->f);
        }
        x = cg->x;
    }

    gsl_multimin_fdfminimizer_free(sd);
    gsl_multimin_fdfminimizer_free(cg);
    gsl_vector_free(old_x);

    if (verbose) {
        printf("END\n\n");
    }

    /* TODO: all this post-optimisation output is the same in opt-md,
       move to common lib function */
    pseq_to_str(p, n, ndim, *designed_seq);
    show_bad_prob(p, n, ndim, verbose);
    show_bad_bp(*designed_seq, inter->pairs, n);
    if (verbose) {
        printf("before = %s\n", *designed_seq);
        printf("fixing bad base pairs\n");
    }
    nfixed_bp = fix_bad_bp(*designed_seq, inter->pairs, n);
    if (verbose) {
        printf("nfixed_bp = %zu\n", nfixed_bp);
        printf("\n");

        print_design_score_info_for_seq(inter, *designed_seq, n, ndim, K_nj, kpi, kpa,
                                        kpur, kneg, khet, het_window);
        printf("vienna = %s\n", vienna);
    }
    free(K_nj);
    nn_inter_delete(inter);
    free(p);
    free(dGdp);
    return EXIT_SUCCESS;
}

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
                        khet, het_window, do_movie_output, verbose,
                        &designed_seq);
    if (verbose) {
        printf("seq    = %s\n", designed_seq);
    } else {
        printf("%s", designed_seq);
    }
    free(designed_seq);
    return status;
}
