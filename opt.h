#ifndef NA_GUARD_OPT_H
#define NA_GUARD_OPT_H

#include "ctools/libctools.h"
#include <stdbool.h>

static const uint ndim = 4;

uint x_parse_seq_constraints_hard(uint n, uint *hard, char *constraint_str,
                                  uint *pairs);

int run_md(char *vienna, char *seq_constraints_hard,
           uint nsteps, uint nprint, uint ncool, uint npur,
           double timestep, double T_start, double kpi, double kpa, double kneg,
           double khet, uint het_window, double kpur_end,
           bool do_exp_cool, bool do_movie_output, bool verbose,
           char **designed_seq);

int run_sd(char *vienna, uint maxsteps, uint nprint, double wiggle,
           double kpi, double kpa, double kpur, double kneg, double khet,
           uint het_window, bool do_movie_output, bool verbose,
           char **designed_seq);

#endif /* NA_GUARD_OPT_H */
