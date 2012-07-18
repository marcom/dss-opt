#ifndef NA_GUARD_DSS_H
#define NA_GUARD_DSS_H

/* TODO: only need uint typedef */
#include "ctools/libctools.h"

double dss_calc_U_pa(double **p, uint n, uint ndim, double k);
void   dss_calc_gradU_pa(double **p, uint n, uint ndim, double k,
                         double **gradU);

double dss_calc_U_pi(double **p, uint n, uint ndim, double k);
void   dss_calc_gradU_pi(double **p, uint n, uint ndim, double k,
                         double **gradU);

double dss_calc_U_pur_cauchy(double **p, uint n, uint ndim, double kpur);
void   dss_calc_gradU_pur_cauchy(double **p, uint n, uint ndim, double kpur,
                                 double **gradU);

double dss_calc_U_negdesign_het(double **p, uint n, uint ndim, double kneg);
void   dss_calc_gradU_negdesign_het(double **p, uint n, uint ndim, double kneg,
                                    double **gradU);

double dss_calc_U_negdesign_nj(double **p, uint n, uint ndim, double kneg,
                               double **K_nj, uint *pairs);
void   dss_calc_gradU_negdesign_nj(double **p, uint n, uint ndim, double kneg,
                                   double **K_nj, uint *pairs, double **gradU);

double dss_calc_U_het(double **p, uint n, uint ndim, double khet,
                      uint het_window, uint *pairs);
void   dss_calc_gradU_het(double **p, uint n, uint ndim, double khet,
                          uint het_window, uint *pairs, double **gradU);


#endif /* NA_GUARD_DSS_H */
