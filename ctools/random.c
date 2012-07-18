#include "random.h"
#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>

/* TODO: convert to srand48(), drand48()

         problem: need to enable -std=gnu99 for it to compile, which
         in turn then defines j1, y0 (Bessel functions) named
         functions that clash with my variable names and cause
         warnings with -Wshadow...
*/

/* seed random number generator */
void
random_seed(uint seed)
{
    srand(seed);
}

/* generate a seed value from the current time of day */
uint
random_get_seedval_from_current_time()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_usec;
}

/* seed random number generator with current time */
void
random_seed_with_current_time()
{
    random_seed(random_get_seedval_from_current_time());
}

/* return uniformly distributed random uint in the interval [a, b]
   general idea taken from Numerical Recipes in C (2nd ed.), p. 277 to
   use the high-order bits of the underlying integer random number
   generator */
uint
random_uint(uint a, uint b)
{
    uint r;
    assert(a <= b);
    /* NOTE: (b - a + 1) is used instead of (b - a) because
       random_double_uniform() gives a number in the half-open
       interval [0,1) */
    /* TODO: does this always work correctly?
       (seems to work ok if a, b are small) */
    r = a + (uint) ((b - a + 1) * random_double_uniform());
    assert(a <= r && r <= b);
    return r;
}

/* return uniformly distributed floating-point number in the half-open
   interval [0,1) */
/* TODO: this function actually generates the interval [0,1] i think */
double
random_double_uniform()
{
    return ((double) rand()) / ((double) RAND_MAX);
}

/* Return normally distributed random number with mean mu and standard
   deviation sigma using the Marsaglia polar method.
   Ref: Knuth, TAOCP, Vol. 2, 3rd ed, p.122 */
/* TODO: use Box-Muller, Ziggurat, or other ? */
double
random_double_normal(double mu, double sigma)
{
    /* TODO: what do we do with the other random number? it would be better to
       store it, but this would have to be in thread-local storage */
    double v1, v2, s;

    do {
        v1 = 2.0 * random_double_uniform() - 1.0;
        v2 = 2.0 * random_double_uniform() - 1.0;
        s = v1 * v1 + v2 * v2;
    } while (s >= 1.0 || s == 0.0);
    return mu + sigma * v1 * sqrt(-2.0 * log(s) / s);
}

/* generate unit vector with random direction */
void
random_dvec_uniform_direction(uint ndim, double *v)
{
    uint i;
    double len2, scale, tmp;

    while (1) {
        /* choose random vector in [-1, 1]^n */
        len2 = 0;
        for (i = 0; i < ndim; i++) {
            tmp = 2 * random_double_uniform() - 1;
            v[i] = tmp;
            len2 += tmp * tmp;
        }
        /* we reject all vectors that lie outside of the unit sphere
           if the vector lies inside, we only need to rescale the length to
           unity and we are done */
        if (len2 <= 1 && len2 != 0) {
            scale = 1 / sqrt(len2);
            for (i = 0; i < ndim; i++)
                v[i] *= scale;
            break;
        }
    }
}
