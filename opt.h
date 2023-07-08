#ifndef NA_GUARD_OPT_H
#define NA_GUARD_OPT_H

#include "ctools/libctools.h"
#include <stdbool.h>

#define DEFAULT_DSSOPT_ndim 4u

#define DEFAULT_DSSOPT_kpi 50000.0
#define DEFAULT_DSSOPT_kpa 50000.0
#define DEFAULT_DSSOPT_kneg 1.0
#define DEFAULT_DSSOPT_kpur 0.0
#define DEFAULT_DSSOPT_khet 10.0
#define DEFAULT_DSSOPT_het_window 3u

uint x_parse_seq_constraints_hard(uint n, uint *hard, const char *constraint_str,
                                  const uint *pairs);

/* run sequence optimisation by dynamics in sequence space (dynamical
   simulated annealing) */
int run_md(const char *vienna, const char *seq_constraints_hard,
           uint nsteps, uint nprint, uint ncool, uint npur,
           double timestep, double T_start, double kpi, double kpa, double kneg,
           double khet, uint het_window, double kpur_end,
           bool do_exp_cool, bool do_movie_output, bool verbose,
           char ** const designed_seq);

/* run sequence optimisation by steepest descent */
int run_sd(const char *vienna, uint maxsteps, uint nprint, double wiggle,
           double kpi, double kpa, double kpur, double kneg, double khet,
           uint het_window, bool do_movie_output, bool verbose,
           char ** const designed_seq);

#endif /* NA_GUARD_OPT_H */
