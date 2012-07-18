#ifndef CT_GUARD_NUM_UTILS_H
#define CT_GUARD_NUM_UTILS_H

/* utility functions for numerical calculations */

/* calculate p * log(p) after forcing p into the range [0.000001, 1]
   this is mainly useful when p is supposed to be a probability */
double plogp_sane(double p);

#endif /* CT_GUARD_NUM_UTILS_H */
