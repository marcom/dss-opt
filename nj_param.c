#include "nj_param.h"

/* parameters were generated by averaging over the stacking free
   energies in the mfold 3.0 parameters (vienna17.par in Vienna
   RNA) */
/* free energies in 0.01 * kcal/mol */
const double K_NJ[4][4] = {
    /*          A    C    G    U */
    /* A */ {   0,   0,   0, 144 },
    /* C */ {   0,   0, 239,   0 },
    /* G */ {   0, 239,   0,  93 },
    /* U */ { 144,   0,  93,   0 }
};

#if 0
/* asymmetric version (taking 5'-3' direction into account */
/* TODO: need to make sure the code takes this into account */
const double K_NJ[4][4] = {
    /*          A    C    G    U */
    /* A */ {   0,   0,   0, 138 },
    /* C */ {   0,   0, 223,   0 },
    /* G */ {   0, 255,   0, 108 },
    /* U */ { 150,   0,  78,   0 }
};
#endif
