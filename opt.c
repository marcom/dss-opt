/* TODO: duplications in run_md, run_sd ? */

/* TODO: calc of energies and derivatives the same in md/sd code,
   maybe other overlaps as well */

#include "opt.h"
#include "dss.h"
#include "na.h"
#include "nj_param.h"
#include "helpers-for-main.h"
#include "helpers-for-main-opt.h"
#include "md/md.h"
#include "ctools/libctools.h"
#include "ctools/dary.h"
#include "ctools/random.h"
#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include <string.h>

const uint    DSSOPT_DEFAULT_ndim       = 4u;
const double  DSSOPT_DEFAULT_kpi        = 50000.0;
const double  DSSOPT_DEFAULT_kpa        = 50000.0;
const double  DSSOPT_DEFAULT_kneg       = 1.0;
const double  DSSOPT_DEFAULT_kpur       = 0.0;
const double  DSSOPT_DEFAULT_khet       = 10.0;
const uint    DSSOPT_DEFAULT_het_window = 3u;

const double  DSSOPT_DEFAULT_OPT_MD_time_total = 50.0;
const double  DSSOPT_DEFAULT_OPT_MD_time_print = 2.5;
const double  DSSOPT_DEFAULT_OPT_MD_timestep   = 0.0015;
const double  DSSOPT_DEFAULT_OPT_MD_T_start    = 40.0;
double dssopt_default_opt_md_time_cool(double time_total) { return 0.1 * time_total; }
double dssopt_default_opt_md_time_pur(double time_total) { return 0.8 * time_total; }

const uint    DSSOPT_DEFAULT_OPT_SD_maxsteps = 20000;
const uint    DSSOPT_DEFAULT_OPT_SD_nprint   = 1000;
const double  DSSOPT_DEFAULT_OPT_SD_wiggle   = 0.1;

uint
x_parse_seq_constraints_hard(uint n, uint *hard, const char *constraint_str,
                             const uint *pairs)
{
    bool verbose = true;
    uint n_hard = 0;
    int retcode = parse_seq_constraints_hard(n, hard, &n_hard, constraint_str,
                                             verbose, pairs);
    if (retcode == EXIT_FAILURE) {
        exit(EXIT_FAILURE);
    }
    return n_hard;
}

int
parse_seq_constraints_hard(uint n, uint *hard, uint *n_hard,
                           const char *constraint_str, bool verbose,
                           const uint *pairs)

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
            if (verbose) {
                printf("ERROR: illegal character '%c' in hard sequence"
                       " constraints\n", ci);
            }
            return EXIT_FAILURE;
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
                if (verbose) {
                    printf("ERROR: illegal base pair (%c%u, %c%u) cannot be satisfied.\n",
                           cj, j, ci, i);
                }
                return EXIT_FAILURE;
            }
        }
    }
    *n_hard = n_constr;
    return EXIT_SUCCESS;
}

/* optimisation by dynamical simulated annealing, i.e. dynamics with a
   cooling scheme */
int
run_md(const char *vienna, const char *seq_constraints_hard,
       uint nsteps, uint nprint, uint ncool, uint npur,
       double timestep, double T_start, double kpi, double kpa, double kneg,
       double khet, uint het_window, double kpur_end,
       bool do_exp_cool, bool do_movie_output, bool verbose,
       char *designed_seq)
{
    uint i, j, step, n, *hard_constraints, n_hard_constr;
    uint ndim = DSSOPT_DEFAULT_ndim;
    double G, Gnn, **p, **v, **dGdp, *mass, kb = 1, ndof,
        T, T_target, T_end = 0.1, cool_rate, kpur = 0, pur_rate,
        ekin, estart = 0;
    double **K_nj = helper_make_K_nj_alloc(ndim);
    size_t nfixed_bp;
    struct nn_inter *inter;

    n = strlen(vienna);
    inter = nn_inter_xnew(n);
    xvienna_to_pairs(n, vienna, inter->pairs);
    find_interactions(inter);

    hard_constraints = xmalloc(n * sizeof(*hard_constraints));
    n_hard_constr = x_parse_seq_constraints_hard(n, hard_constraints, seq_constraints_hard, inter->pairs);

    /* TODO: or ndof = n * (ndim - 1) - (n_hard_constr * ndim); ? */
    ndof = n * ndim - (n_hard_constr * ndim);
    xmalloc2d_one_chunk(p, n, ndim);
    xmalloc2d_one_chunk(v, n, ndim);
    xmalloc2d_one_chunk(dGdp, n, ndim);
    mass = xmalloc(n * sizeof(*mass));

    dary2d_set(n, ndim, p, 1.0 / ndim);
    dary_set(mass, n, 1.0);
    md_init_velocities_random(n, ndim, v, mass, kb, ndof, T_start);

    if (do_exp_cool)
        cool_rate = pow(T_end / T_start, 1.0 / (nsteps - ncool));
    else
        cool_rate = (T_end - T_start) / (nsteps - ncool);
    /* only linear purification schedule */
    pur_rate = kpur_end / (nsteps - npur);

    if (verbose) {
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
    }

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
                print_for_movie(p, n, ndim, designed_seq);
            } else {
                if (verbose) {
                    printf("%10u  %12.5f    % 12.3f  % 12.3f  % 12.3f  % 12.3f"
                           "  % 12.3f  % 12.3f  % 12.3f  % 12.3f\n",
                           step, step * timestep, Gnn, G, ekin, G + ekin, T,
                           G + ekin - estart, calc_entropy(p, n, ndim), kpur);
                }
            }
        }

        /* check for numerical explosion */
        if (system_is_exploded(p, n, ndim)) {
            printf("END\n\n");
            show_bad_prob(p, n, ndim, true);
            printf("\n");
            printf("vienna = %s\n", vienna);
            pseq_to_str(p, n, ndim, designed_seq);
            printf("seq    = %s\n", designed_seq);
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
    if (verbose) {
        printf("END\n\n");
    }

    pseq_to_str(p, n, ndim, designed_seq);
    show_bad_prob(p, n, ndim, verbose);
    show_bad_bp(designed_seq, inter->pairs, n, verbose);
    if (verbose) {
        printf("before = %s\n", designed_seq);
        printf("fixing bad base pairs\n");
    }
    nfixed_bp = fix_bad_bp(designed_seq, inter->pairs, n);
    if (verbose) {
        printf("nfixed_bp = %zu\n", nfixed_bp);
        printf("\n");

        print_design_score_info_for_seq(inter, designed_seq, n, ndim, K_nj, kpi, kpa,
                                        kpur, kneg, khet, het_window);

        printf("vienna = %s\n", vienna);
    }

    free(K_nj);
    nn_inter_delete(inter);
    free(hard_constraints);
    free(p);
    free(v);
    free(dGdp);
    free(mass);
    return EXIT_SUCCESS;
}

/* steepest descent optimisation */
int
run_sd(const char *vienna, uint maxsteps, uint nprint, double wiggle,
       double kpi, double kpa, double kpur, double kneg, double khet,
       uint het_window, bool do_movie_output, bool verbose,
       char *designed_seq)
{
    uint i, j, step, n;
    uint ndim = DSSOPT_DEFAULT_ndim;
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

        printf("      step     scale       Gnn         G\n");
        printf("     ------   -------   ---------  ---------\n");

        printf("START\n");
    }
    for (step = 0; step <= maxsteps; step++) {
        dary2d_set(n, ndim, dGdp, 0);

        /* calc energy */
        G = Gnn = calc_interactions_pseq(inter, p);
        G += dss_calc_U_pa(p, n, ndim, kpa);
        G += dss_calc_U_pi(p, n, ndim, kpi);
        G += dss_calc_U_pur_cauchy(p, n, ndim, kpur);
        G += dss_calc_U_negdesign_nj(p, n, ndim, kneg, K_nj, inter->pairs);
        G += dss_calc_U_het(p, n, ndim, khet, het_window, inter->pairs);
        if (step == 0) {
            Gstart = G;
            if (! do_movie_output && verbose)
                printf("Gstart = %f\n", Gstart * 0.01);
        }

        /* calc gradient */
        calc_interactions_dGdp_pseq(inter, p, dGdp);
        dss_calc_gradU_pa(p, n, ndim, kpa, dGdp);
        dss_calc_gradU_pi(p, n, ndim, kpi, dGdp);
        dss_calc_gradU_pur_cauchy(p, n, ndim, kpur, dGdp);
        dss_calc_gradU_negdesign_nj(p, n, ndim, kneg, K_nj, inter->pairs, dGdp);
        dss_calc_gradU_het(p, n, ndim, khet, het_window, inter->pairs, dGdp);

        /* move in direction of steepest descent */
        for (i = 0; i < n; i++)
            for (j = 0; j < ndim; j++)
                p[i][j] -= scale * dGdp[i][j];
#if 0
        /* TODO: this doesn't work yet when all p[i][j] for some i a
           re negative.  We need to properly project the p[i][j] or
           the gradient back onto the simplex of permissible values */
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
#endif
        /* calc new energy */
        Gold = G;
        G = Gnn = calc_interactions_pseq(inter, p);
        G += dss_calc_U_pa(p, n, ndim, kpa);
        G += dss_calc_U_pi(p, n, ndim, kpi);
        G += dss_calc_U_pur_cauchy(p, n, ndim, kpur);
        G += dss_calc_U_negdesign_nj(p, n, ndim, kneg, K_nj, inter->pairs);
        delta = G - Gold;

        /* are we moving too far ? */
        if (delta < 0) {
            /* good move */
            scale *= scale_increase;
            if (step % nprint == 0) {
                if (do_movie_output) {
                    print_for_movie(p, n, ndim, designed_seq);
                } else if (verbose) {
                    printf("%10d %10.5f %10.2f %10.2f\n", step, scale, Gnn * 0.01, G * 0.01);
                }
            }
        } else {
            /* bad move */
            if (scale < scale_min)
                break;
            /* move back again */
            for (i = 0; i < n; i++)
                for (j = 0; j < ndim; j++)
                    p[i][j] += scale * dGdp[i][j];
            scale *= scale_decrease;
            if (step % nprint == 0) {
                if (do_movie_output) {
                    print_for_movie(p, n, ndim, designed_seq);
                } else {
                    if (verbose) {
                        printf("%10d %10.5f %10.2f %10.2f\n", step, scale, Gnn * 0.01, Gold * 0.01);
                    }
                }
            }
        }

        /* TODO: probably not needed */
        if (system_is_exploded(p, n, ndim)) {
            printf("END\n\n");
            show_bad_prob(p, n, ndim, true);
            printf("\n");
            printf("vienna = %s\n", vienna);
            pseq_to_str(p, n, ndim, designed_seq);
            printf("seq    = %s\n", designed_seq);
            printf("\nstep = %u\n", step);
            printf("BOOOOOM --- system exploded\n");
            return EXIT_FAILURE;
        }
    }
    if (verbose) {
        printf("END\n\n");
    }

    /* TODO: all this post-optimisation output is the same in opt-md,
       move to common lib function */
    pseq_to_str(p, n, ndim, designed_seq);
    show_bad_prob(p, n, ndim, verbose);
    show_bad_bp(designed_seq, inter->pairs, n, verbose);
    if (verbose) {
        printf("before = %s\n", designed_seq);
        printf("fixing bad base pairs\n");
    }
    nfixed_bp = fix_bad_bp(designed_seq, inter->pairs, n);
    if (verbose) {
        printf("nfixed_bp = %zu\n", nfixed_bp);
        printf("\n");

        print_design_score_info_for_seq(inter, designed_seq, n, ndim, K_nj, kpi, kpa,
                                        kpur, kneg, khet, het_window);
        printf("vienna = %s\n", vienna);
    }

    free(K_nj);
    nn_inter_delete(inter);
    free(p);
    free(dGdp);
    return EXIT_SUCCESS;
}
