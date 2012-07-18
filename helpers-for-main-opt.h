#ifndef GUARD_MAIN_HELPERS_OPT_H
#define GUARD_MAIN_HELPERS_OPT_H

#include "na.h" /* struct nn_inter */
#include "ctools/libctools.h" /* uint */

void x_ensure_positive(char *progname, char *optname, double val);

void set_dss_force_constants_defaults(double *kpi, double *kpa, double *kneg,
                                      double *kpur, double *khet, uint *het_window);

void print_design_score_info_for_seq(struct nn_inter *inter, char *seq,
                                     uint n, uint ndim, double **K_nj, double kpi,
                                     double kpa, double kpur, double kneg,
                                     double khet, uint het_window);

#endif /* GUARD_MAIN_HELPERS_OPT_H */
