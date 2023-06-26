#ifndef CT_GUARD_RANDOM_H
#define CT_GUARD_RANDOM_H

#include "libctools.h"

/* functions for generating random numbers and vectors */

/* seed random number generator */
void random_seed(uint seed);

/* generate a seed value from the current time of day */
uint random_get_seedval_from_current_time(void);

/* seed random number generator with current time */
void random_seed_with_current_time(void);

/* return uniformly distributed random uint in the interval [a, b] */
uint random_uint(uint a, uint b);

/* return uniformly distributed random number in the half-open
   interval [0,1) */
double random_double_uniform(void);

/* normally distributed random number */
double random_double_normal(double mu, double sigma);

/* vector of length 1 with uniformly distributed direction
   storage must be allocated by the caller
   WARNING: the way this function is implemented at the moment, there is a
   very small but nonzero probability that this function will not terminate */
void random_dvec_uniform_direction(uint ndim, double *v);

#endif /* CT_GUARD_RANDOM_H */
