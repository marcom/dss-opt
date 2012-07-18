#include "ctools/random.h"
#include "md.h"
#include <math.h>

/* calculate the temperature: T = 2 * Ekin / (kb * ndof) */
double
md_calc_temperature(uint n, uint ndim, double **v, double *mass,
                    double kb, double ndof)
{
    uint i, j;
    double T = 0;

    for (i = 0; i < n; i++)
        for (j = 0; j < ndim; j++)
            T += mass[i] * v[i][j] * v[i][j];
    return T / (kb * ndof);
}

/* initialize velocities with uniform length and random directions,
   then rescale the velocities to the desired temperature T */
void
md_init_velocities_random(uint n, uint ndim, double **v, double *mass,
                          double kb, double ndof, double T)
{
    uint i;
    /* initialize with random directions */
    for (i = 0; i < n; i++)
        random_dvec_uniform_direction(ndim, v[i]);
    /* remove net momentum and rescale to desired temperature */
    md_remove_net_momentum_no_rescale(n, ndim, v, mass);
    md_rescale_temperature(n, ndim, v, mass, kb, ndof, T);
}

/* initialize velocities with random directions and magnitudes distributed
   according to the maxwell-boltzmann distribution, then rescale to
   desired temperature T */
/* TODO: - reference for Maxwell-Boltzmann distribution
         - is this really the correct form of the distribution if
           the particles have different masses ? */
void
md_init_velocities_maxwell_boltzmann(uint n, uint ndim, double **v,
                                     double *mass, double kb, double ndof,
                                     double T)
{
    uint i, j;

    /* initialize each velocity component with a normally distributed random
       number (mean 0 and standard deviation of 1) */
    for (i = 0; i < n; i++)
        for (j = 0; j < ndim; j++)
            v[i][j] = random_double_normal(0, 1);
    /* remove net momentum and rescale to desired temperature */
    md_remove_net_momentum_no_rescale(n, ndim, v, mass);
    md_rescale_temperature(n, ndim, v, mass, kb, ndof, T);
}

/* remove the net center-of-mass momentum from the system without rescaling
   the velocities, thus possibly decreasing kinetic energy */
/* TODO: is multiplication and division by mass really needed? */
/* TODO: would be nice to avoid the malloc/free of the psum vector, but
         perhaps this is not really critical */
void
md_remove_net_momentum_no_rescale(uint n, uint ndim, double **v, double *mass)
{
    uint i, j;

    /* TODO: move to dary_zero_alloc function */
    double *psum = xmalloc(ndim * sizeof(*psum));
    for (i = 0; i < ndim; i++)
        psum[i] = 0;

    for (i = 0; i < n; i++)
        for (j = 0; j < ndim; j++)
            psum[j] += v[i][j] * mass[i];

    /* TODO: move to dary_div function */
    for (j = 0; j < ndim; j++)
        psum[j] /= n;

    for (i = 0; i < n; i++)
        for (j = 0; j < ndim; j++)
            v[i][j] -= psum[j] / mass[i];

    free(psum);
}

/* rescaling thermostat */
void
md_rescale_temperature(uint n, uint ndim, double **v, double *mass,
                       double kb, double ndof, double T_desired)
{
    uint i, j;
    double T, scale;

    T = md_calc_temperature(n, ndim, v, mass, kb, ndof);
    /* rescale to desired temperature */
    scale = sqrt(T_desired / T);
    /* TODO: move to dary2d_mul function */
    for (i = 0; i < n; i++)
        for (j = 0; j < ndim; j++)
            v[i][j] *= scale;
}

/* berendsen thermostat */
/* TODO: factor out commonalities with
         md_rescale_temperature(), they are very similar */
/* TODO: assert timestep > 0, t_rise > timestep */
void
md_rescale_temperature_berendsen(uint n, uint ndim, double **v, double *mass,
                                 double kb, double ndof, double T_desired,
                                 double timestep, double tau)
{
    uint i, j;
    double T, scale;

    T = md_calc_temperature(n, ndim, v, mass, kb, ndof);
    /* rescale to desired temperature */
    scale = sqrt(1 + (timestep / tau) * (T_desired / T - 1));
    /* TODO: move to dary2d_mul function */
    for (i = 0; i < n; i++)
        for (j = 0; j < ndim; j++)
            v[i][j] *= scale;
}

/* leapfrog integrator
   means s is on timesteps n-1, n, n+1,...
   and velocities are on steps n-3/2, n-1/2, n+1/2, ...
   i.e. velocities are half a timestep behind */
void
md_integrate_step_leapfrog(uint n, uint ndim, double **r, double **v,
                           double **gradU, double *mass, double timestep)
{
    uint i, j;
#if 0
    dss_eval_UF(d);

    /* we calculate the kinetic energy and temperature before performing an
       integration step so that the kinetic energy will be computed at the
       same timestep as the potential energy, which is computed when computing
       the gradient of U */
    dss_calc_kin_energy_and_temperature_leapfrog(d);
#endif

    /* calculate new positions and velocities */
    for (i = 0; i < n; i++)
        for (j = 0; j < ndim; j++) {
            v[i][j] -= timestep * gradU[i][j] / mass[i];
            r[i][j] += timestep * v[i][j];
        }
}

#if 0
/* calculate kinetic energy and temperature
   due to the fact that velocities are half a timestep behind,
   we adjust the velocities (as in the velocity-verlet algorithm)
   to get the velocity at the same timestep as the positions */
/* WARNING: this function assumes that the gradient has been computed
            as we need to move the velocities forward half a step */
/* TODO: rename tmp to vnew or vcorrected */
void
dss_calc_kin_energy_and_temperature_leapfrog(struct dss *d)
{
    uint i, j;
    real tmp;
    d->Ekin = 0;
    for (i = 0; i < d->N; i++) {
        for (j = 0; j < d->Nalpha; j++) {
            tmp = d->v[i][j] - 0.5 * d->timestep * d->gradU[i][j] / d->mass[i];
            d->Ekin += tmp * tmp;   /* TODO: should be: d->Ekin += tmp * tmp * d->mass[i] */
        }
        tmp *= d->mass[i];   /* TODO: this looks wrong!!! */
    }
    d->Ekin *= 0.5;
    d->Tkin = 2 * d->Ekin / (d->kb * d->Ndf);
}
#endif
