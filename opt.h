#ifndef DSSOPT_HEADER_OPT_H
#define DSSOPT_HEADER_OPT_H

#include "ctools/libctools.h"
#include <stdbool.h>

extern const uint    DSSOPT_DEFAULT_ndim;
extern const double  DSSOPT_DEFAULT_kpi;
extern const double  DSSOPT_DEFAULT_kpa;
extern const double  DSSOPT_DEFAULT_kneg;
extern const double  DSSOPT_DEFAULT_kpur;
extern const double  DSSOPT_DEFAULT_khet;
extern const uint    DSSOPT_DEFAULT_het_window;

extern const double  DSSOPT_DEFAULT_OPT_MD_time_total;
extern const double  DSSOPT_DEFAULT_OPT_MD_time_print;
extern const double  DSSOPT_DEFAULT_OPT_MD_time_cool;
extern const double  DSSOPT_DEFAULT_OPT_MD_time_pur;
extern const double  DSSOPT_DEFAULT_OPT_MD_timestep;
extern const double  DSSOPT_DEFAULT_OPT_MD_T_start;

extern const uint    DSSOPT_DEFAULT_OPT_SD_maxsteps;
extern const uint    DSSOPT_DEFAULT_OPT_SD_nprint;
extern const double  DSSOPT_DEFAULT_OPT_SD_wiggle;

/**
 * @brief Parse a hard sequence constraints string to an integer
 * encoding, exiting on failure.
 *
 * Checks if the constraint string contains only valid nucleotides or
 * wildcards, and that the constraints are compatible with a given
 * secondary structure.
 *
 * @param[in]  n               Length of `constraint_str` and `pairs`
 * @param[out] hard            Hard sequence constraints encoded as array of uint
 * @param[in]  constraint_str  Hard sequence constraints as a string of "N" or "." wildcards and "ACGU" letters
 * @param[in]  pairs           Pair list encoding a secondary structure
 *
 * @return                     Returns number of non-wildcard hard sequence constraints
 */
uint x_parse_seq_constraints_hard(uint n, uint *hard, const char *constraint_str,
                                  const uint *pairs);

/**
 * @brief Parse a hard sequence constraints string to an integer
 * encoding and indicate errors in the constraints.
 *
 * Checks if the constraint string contains only valid nucleotides or
 * wildcards, and that the constraints are compatible with a given
 * secondary structure.
 *
 * @param[in]  n               Length of `constraint_str` and `pairs`
 * @param[out] hard            Hard sequence constraints encoded as array of uint
 * @param[out] n_hard          Number of non-wildcard hard sequence constraints
 * @param[in]  constraint_str  Hard sequence constraints as a string of "N" or "." wildcards and "ACGU" letters
 * @param[in]  verbose         Print details on parsing errors of the sequence constraints to stdout
 * @param[in]  pairs           Pair list encoding a secondary structure
 *
 * @return                     Returns EXIT_SUCCESS if there were no errors parsing the sequence constraints
 */
int parse_seq_constraints_hard(uint n, uint *hard, uint *n_hard,
                               const char *constraint_str, bool verbose,
                               const uint *pairs);

/**
 * @brief Design a sequence for a given secondary structure by
 * dynamical simulated annealing optimization (dynamics in sequence
 * space).
 *
 * Returns the status code EXIT_SUCCESS if the optimization was
 * successful.
 *
 * @param[in]  vienna            Target secondary structure in Vienna format (dot-bracket)
 * @param[in]  seq_constraints_hard  Hard sequence constraints (can be NULL). String of "ACGU" chars and "N" or "." wildcards
 * @param[in]  nsteps            Number of steps for the optimization
 * @param[in]  nprint            Interval for printing the optimization status
 * @param[in]  ncool             Number of steps after which cooling begins
 * @param[in]  npur              Number of steps after which purification begins
 * @param[in]  timestep          Simulation timestep
 * @param[in]  T_start           Starting temperature
 * @param[in]  kpi               Scoring function constant for penalty terms, keeps \f$ x_{ij} \in [0,1] \f$
 * @param[in]  kpa               Scoring function constant for penalty termn, keeps \f$ \sum_j x_{ij} \approx 1 \f$
 * @param[in]  kneg              Scoring function constant for mean-field negative design term
 * @param[in]  khet              Scoring function constant for sequence heterogeneity term
 * @param[in]  het_window        Window size for sequence heterogeneity term
 * @param[in]  kpur_end          Scoring function constant for sequence purification term at the end of opt.
 * @param[in]  do_exp_cool       Flag to indicate if exponential cooling is to be used
 * @param[in]  do_movie_output   Flag to indicate if movie output is to be generated
 * @param[in]  verbose           Flag to indicate if verbose output is enabled
 * @param[out] designed_seq      The output designed sequence
 *
 * @return                       Returns int status code EXIT_SUCCESS on success.
 */
int run_md(const char *vienna, const char *seq_constraints_hard,
           uint nsteps, uint nprint, uint ncool, uint npur,
           double timestep, double T_start, double kpi, double kpa, double kneg,
           double khet, uint het_window, double kpur_end,
           bool do_exp_cool, bool do_movie_output, bool verbose,
           char *designed_seq);

/**
 * @brief Design a sequence for a given secondary structure by
 * steepest descent optimization.
 *
 * Returns the status code EXIT_SUCCESS if the optimization was
 * successful.
 *
 * @param[in]  vienna           Target secondary structure in Vienna format (dot-bracket)
 * @param[in]  maxsteps         Maximum number of steps for the optimization
 * @param[in]  nprint           Interval for printing the optimization status
 * @param[in]  wiggle           Scaling factor of random perturbation to initial starting point
 * @param[in]  kpi              Scoring function constant for penalty terms, keeps \f$ x_{ij} \in [0,1] \f$
 * @param[in]  kpa              Scoring function constant for penalty termn, keeps \f$ \sum_j x_{ij} \approx 1 \f$
 * @param[in]  kpur             Scoring function constant for sequence purification term
 * @param[in]  kneg             Scoring function constant for mean-field negative design term
 * @param[in]  khet             Scoring function constant for sequence heterogeneity term
 * @param[in]  het_window       Window size for sequence heterogeneity term
 * @param[in]  do_movie_output  Flag to indicate if movie output is to be generated
 * @param[in]  verbose          Flag to indicate if verbose output is enabled
 * @param[out] designed_seq     The output designed sequence
 *
 * @return                      Returns int status code EXIT_SUCCESS on success.
 */
int run_sd(const char *vienna, uint maxsteps, uint nprint, double wiggle,
           double kpi, double kpa, double kpur, double kneg, double khet,
           uint het_window, bool do_movie_output, bool verbose,
           char *designed_seq);

#endif /* DSSOPT_HEADER_OPT_H */
