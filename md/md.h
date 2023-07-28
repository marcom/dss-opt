#ifndef DSSOPT_HEADER_MD_H
#define DSSOPT_HEADER_MD_H

/* TODO: only need the types defined in the header */
#include "ctools/libctools.h"


double
md_calc_temperature(uint n, uint ndim, double **v, double *mass,
                    double kb, double ndof);

void
md_init_velocities_random(uint n, uint ndim, double **v, double *mass,
                          double kb, double ndf, double T);
void
md_init_velocities_maxwell_boltzmann(uint n, uint ndim, double **v,
                                     double *mass, double kb, double ndof,
                                     double T);

void
md_remove_net_momentum_no_rescale(uint n, uint ndim, double **v, double *mass);

void
md_rescale_temperature(uint n, uint ndim, double **v, double *mass,
                       double kb, double ndf, double T_desired);

void
md_rescale_temperature_berendsen(uint n, uint ndim, double **v, double *mass,
                                 double kb, double ndof, double T_desired,
                                 double timestep, double tau);

void
md_integrate_step_leapfrog(uint n, uint ndim, double **r, double **v,
                           double **gradU, double *mass, double timestep);


#endif /* DSSOPT_HEADER_MD_H */
