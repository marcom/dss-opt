#include "num_utils.h"
#include "libctools.h"
#include <math.h>

double
plogp_sane(double p)
{
    p = MAX(p, 1e-6);
    p = MIN(p, 1);
    return p * log(p);
}
