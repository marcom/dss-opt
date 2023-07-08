#ifndef NA_GUARD_NA_H
#define NA_GUARD_NA_H

#include "ctools/libctools.h" /* we only need the typedefs */
#include <limits.h>
#include <stdbool.h>

/* TODO: only needed for NN_NON_GC_PENALTY, move array NN_NON_GC_PENALTY_ARY
         to nn_param.h (i.e. autogenerate from parameter file) */
#include "nn_param.h"

#define NA_UNDEF    (UINT_MAX / 2)
#define NA_UNPAIRED (UINT_MAX / 2)
#define NA_BASE_N   (UINT_MAX / 2)
#define NA_BASE_A   ((uint) 0)
#define NA_BASE_C   ((uint) 1)
#define NA_BASE_G   ((uint) 2)
#define NA_BASE_U   ((uint) 3)

extern const uint NA_2BASES_TO_PAIRTYPE[4][4];

struct nn_stack {
    /* TODO: only i1, j1 needed (can then be called i and j) */
    uint i1, j1, i2, j2;
};

struct nn_hairpin {
    uint i, j, size;
};

struct nn_bulge {
    uint i1, j1, i2, j2, size;
};

struct nn_intloop {
    uint i1, j1, i2, j2, size1, size2;
};

struct nn_multiloop {
    uint unpaired, nstems, ndangle5, ndangle3;
    uint (*stems)[2];
    uint (*dangle5)[3];
    uint (*dangle3)[3];
    /* bool is_multiloop; */
};

struct nn_inter {
    uint n, nstack, nhairpin, nbulge, nintloop, nmultiloop;
    uint *pairs;
    struct nn_stack *stack;
    struct nn_hairpin *hairpin;
    struct nn_bulge *bulge;
    struct nn_intloop *intloop;
    struct nn_multiloop extloop;
    struct nn_multiloop *multiloop;
};



int      vienna_to_pairs(uint n, const char *vienna, bool verbose, uint *pairs);
void     xvienna_to_pairs(uint n, const char *vienna, uint *pairs);
char *   xpairs_to_vienna(uint n, const uint *pairs, char *vienna);
void     xstr_to_seq(uint n, const char *str, uint *seq);
void     xstr_to_pseq(uint n, uint ndim, const char *str, double **p);
void     pseq_to_str(double **p, uint n, uint ndim, char *str);

struct nn_inter *  nn_inter_xnew(uint n);
void               nn_inter_delete(struct nn_inter *inter);
void               find_interactions(struct nn_inter *inter);
void               print_interactions(const struct nn_inter *inter);

void     random_pairs(uint n, uint *pairs, uint hpmin);
void     random_useq(uint n, const uint *pairs, uint *useq);



int      calc_interactions(const struct nn_inter *inter, const uint *useq);
double   calc_interactions_pseq(const struct nn_inter *inter, double **p);
void     calc_interactions_dGdp_pseq(const struct nn_inter *inter, double **p,
                                     double **dGdp);

int      G_stack(const uint *seq, uint i1, uint j1, uint i2, uint j2);
double   G_stack_pseq(double **p, uint i1, uint j1, uint i2, uint j2);
void     dGdp_stack_pseq(double **p, uint i1, uint j1, uint i2, uint j2,
                         double **dGdp);

int      G_bulgeloop(const uint *seq, uint size, uint i1, uint j1,
                     uint i2, uint j2);
double   G_bulgeloop_pseq(double **p, uint size, uint i1, uint j1,
                          uint i2, uint j2);
void     dGdp_bulgeloop_pseq(double **p, uint size, uint i1, uint j1,
                             uint i2, uint j2, double **dGdp);

int      G_intloop(const uint *seq, uint size1, uint size2,
                   uint i1, uint j1, uint i2, uint j2);
double   G_intloop_pseq(double **p, uint size1, uint size2,
                        uint i1, uint j1, uint i2, uint j2);
void     dGdp_intloop_pseq(double **p, uint size1, uint size2,
                           uint i1, uint j1, uint i2, uint j2, double **dGdp);

int      G_hairpin(const uint *seq, uint size, uint i1, uint j1);
double   G_hairpin_pseq(double **p, uint size, uint i1, uint j1);
void     dGdp_hairpin_pseq(double **p, uint size, uint i1, uint j1,
                           double **dGdp);

int      G_extloop_multiloop(const uint *seq, uint unpaired,
                             uint nstems, uint (*stems)[2],
                             uint ndangle5, uint (*dangle5)[3],
                             uint ndangle3, uint (*dangle3)[3],
                             bool is_multiloop);
double   G_extloop_multiloop_pseq(double **p, uint unpaired,
                                  uint nstems, uint (*stems)[2],
                                  uint ndangle5, uint (*dangle5)[3],
                                  uint ndangle3, uint (*dangle3)[3],
                                  bool is_multiloop);
void     dGdp_extloop_multiloop_pseq(double **p,
                                     uint nstems, uint (*stems)[2],
                                     uint ndangle5, uint (*dangle5)[3],
                                     uint ndangle3, uint (*dangle3)[3],
                                     double **dGdp);


#endif /* NA_GUARD_NA_H */
