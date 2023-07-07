#include "ctools/random.h"
#include "na.h"
#include "nn_param.h"
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <ctype.h>

#define DEBUG printf

const uint NA_NBASES = 4;
const char NA_BASE_NAMES[] = { 'A', 'C', 'G', 'U' };
const uint NA_NBASEPAIRS = 6;
const char NA_BASEPAIR_NAMES[][3] = { "CG", "GC", "GU", "UG", "AU", "UA" };

/* TODO: build these arrays at runtime from the above arrays */
const uint NA_2BASES_TO_PAIRTYPE[4][4] = {
    /*                A          C          G          U  */
    /* A */  { NA_UNDEF,  NA_UNDEF,  NA_UNDEF,         4, },
    /* C */  { NA_UNDEF,  NA_UNDEF,         0,  NA_UNDEF, },
    /* G */  { NA_UNDEF,         1,  NA_UNDEF,         2, },
    /* U */  {        5,  NA_UNDEF,         3,  NA_UNDEF, },
};
const uint NA_BASEPAIR_TO_BASES[][2] = {
    {1, 2}, /* CG */
    {2, 1}, /* GC */
    {2, 3}, /* GU */
    {3, 2}, /* UG */
    {0, 3}, /* AU */
    {3, 0}, /* UA */
};


/* TODO: these two arrays should be spit out by vienna_parse_param.py */
/* TODO: the next extra define is only necessary because the name
   NN_G_NON_GC_PENALTY is too long */
#define NA_NONGC NN_G_NON_GC_PENALTY
const int NN_NON_GC_PENALTY_FOR_BP[] = {
    0, 0, NA_NONGC, NA_NONGC, NA_NONGC, NA_NONGC
};
const int NN_NON_GC_PENALTY_FOR_BASES[][4] = {
    /*                  A           C           G           U  */
    /* A */  {          0,          0,          0,   NA_NONGC, },
    /* C */  {          0,          0,          0,          0, },
    /* G */  {          0,          0,          0,   NA_NONGC, },
    /* U */  {   NA_NONGC,          0,   NA_NONGC,          0, },
};




/*
  representations for nucleic acid secondary structure
  ----------------------------------------------------
                    0 1 2 3 4 5 6 7 8
  vienna string:    ( ( ( . . . ) ) )
  pairlist:         8 7 6 . . . 2 1 0   -- here the dot stands for NA_UNPAIRED
*/

/* transform a structure given in vienna bracket notation to a
   pairlist */
void
xvienna_to_pairs(uint n, const char *vienna, uint *pairs)
{
    uint i, j;
    uint *stack, idx_stack = 0; /* idx_stack is the next free slot */
    stack = xmalloc(n * sizeof(*stack));

    for (i = 0; i < n; i++) {
        switch (vienna[i]) {
        case '(':
            stack[idx_stack++] = i;
            break;
        case ')':
            if (idx_stack > 0) {
                j = stack[--idx_stack];
                pairs[i] = j;
                pairs[j] = i;
            } else {
                printf("ERROR: too many closing parentheses in vienna"
                       " string %s\n", vienna);
                exit(EXIT_FAILURE);
            }
            break;
        case '.':
            pairs[i] = NA_UNPAIRED;
            break;
        default:
            printf("ERROR: illegal character '%c' in vienna string '%s'\n",
                   vienna[i], vienna);
            exit(EXIT_FAILURE);
            break;
        }
    }
    if (idx_stack != 0) {
        printf("ERROR: not enough closing parentheses in vienna string %s\n",
               vienna);
        exit(EXIT_FAILURE);
    }

    free(stack);
}

/* pairlist is assumed to be valid and to correspond to a
   non-pseudoknotted secondary structure */
char *
xpairs_to_vienna(uint n, const uint *pairs, char *vienna)
{
    uint i;
    for (i = 0; i < n; i++) {
        if (pairs[i] == NA_UNPAIRED)
            vienna[i] = '.';
        else if (i < pairs[i])
            vienna[i] = '(';
        else if (i > pairs[i])
            vienna[i] = ')';
        else {
            printf("ERROR: illegal entry in pairlist\n");
            exit(EXIT_FAILURE);
        }
    }
    vienna[n] = '\0';
    return vienna;
}

/* translate an ASCII character to its internal representation */
static uint
xencode_char(char c)
{
    uint j;
    c = toupper(c);
    for (j = 0; j < sizeof(NA_BASE_NAMES); j++)
        if (NA_BASE_NAMES[j] == c)
            break;
    if (j >= sizeof(NA_BASE_NAMES)) {
        printf("ERROR: illegal character %c in sequence\n", c);
        exit(EXIT_FAILURE);
    }
    return j;
}

/* translate from a string representation of a sequence to its internal
   representation */
void
xstr_to_seq(uint n, const char *str, uint *seq)
{
    uint i;
    for (i = 0; i < n; i++)
        seq[i] = xencode_char(str[i]);
}

/* translate a continuous sequence representation into a string by
   taking the most likely character at each position */
void
pseq_to_str(double **p, uint n, uint ndim, char *str)
{
    uint i, j, j_max;
    for (i = 0; i < n; i++) {
        j_max = 0;
        for (j = 1; j < ndim; j++)
            if (p[i][j] > p[i][j_max])
                j_max = j;
        str[i] = NA_BASE_NAMES[j_max];
    }
    str[n] = '\0';
}

/* translate from a string representation of a sequence to a
   probability representation */
void
xstr_to_pseq(uint n, uint ndim, const char *str, double **p)
{
    uint i, j;
    /* ASSERT: ndim == length of NA_BASE_NAMES */
    for (i = 0; i < n; i++) {
        for (j = 0; j < ndim; j++)
            p[i][j] = 0;
        p[i][xencode_char(str[i])] = 1;
    }
}

void
nn_multiloop_xalloc(struct nn_multiloop *ml, uint nstems, uint ndangle5,
                    uint ndangle3)
{
    ml->nstems = nstems;
    ml->stems = xmalloc(nstems * sizeof(*ml->stems));
    ml->ndangle5 = ndangle5;
    ml->dangle5 = xmalloc(ndangle5 * sizeof(*ml->dangle5));
    ml->ndangle3 = ndangle3;
    ml->dangle3 = xmalloc(ndangle3 * sizeof(*ml->dangle3));
}

void
nn_multiloop_free(struct nn_multiloop *ml)
{
    free(ml->stems);
    free(ml->dangle5);
    free(ml->dangle3);
}

struct nn_inter *
nn_inter_xnew(uint n)
{
    /* TODO: memory waste here, but works ok for now */
    uint i;
    struct nn_inter *inter;
    inter = xmalloc(sizeof(*inter));
    inter->n = n;
    inter->pairs = xmalloc(n * sizeof(*inter->pairs));
    inter->nstack = 0;
    inter->stack = xmalloc(n * sizeof(*inter->stack));
    inter->nhairpin = 0;
    inter->hairpin = xmalloc(n * sizeof(*inter->hairpin));
    inter->nbulge = 0;
    inter->bulge = xmalloc(n * sizeof(*inter->bulge));
    inter->nintloop = 0;
    inter->intloop = xmalloc(n * sizeof(*inter->intloop));
    inter->nmultiloop = 0;
    inter->multiloop = xmalloc(n * sizeof(*inter->multiloop));
    /* TODO: temporary hack until nn_multiloop_xfind is fixed */
    inter->extloop.stems = xmalloc(n * sizeof(*inter->extloop.stems));
    inter->extloop.dangle5 = xmalloc(n * sizeof(*inter->extloop.dangle5));
    inter->extloop.dangle3 = xmalloc(n * sizeof(*inter->extloop.dangle3));
    /* TODO: temporary hack until nn_multiloop_xfind is fixed */
    for (i = 0; i < n; i++) {
        inter->multiloop[i].stems =
            xmalloc(n * sizeof(*inter->multiloop[i].stems));
        inter->multiloop[i].dangle5 =
            xmalloc(n * sizeof(*inter->multiloop[i].dangle5));
        inter->multiloop[i].dangle3 =
            xmalloc(n * sizeof(*inter->multiloop[i].dangle3));
    }
    return inter;
}

void
nn_inter_delete(struct nn_inter *inter)
{
    uint i;
    free(inter->pairs);
    free(inter->stack);
    free(inter->hairpin);
    free(inter->bulge);
    free(inter->intloop);
    nn_multiloop_free(&inter->extloop);
    /* TODO: currently assume that n multiloops were allocated */
    for (i = 0; i < inter->n; i++)
        nn_multiloop_free(&inter->multiloop[i]);
    free(inter->multiloop);
    free(inter);
}

/* TODO: this routine assumes that the multiloop has been fully allocated
         and the arrays contained inside have enough room. this clearly
         is not very good -- should first collect the data, then allocate
         sufficient space and move the data over. the name should also be
         changed to something_alloc to reflect this */
/* TODO: simplify this (storing of the outer base pair manually, but
   what about extloops ?) */
/* run through the multiloop enclosed by the bases (i, j) and store
   information on the location of stems, 5' and 3' dangles and the
   number of unpaired bases.
   Note: the enclosing base pair is *not* stored, this needs to be
   done manually afterwards */
void
nn_multiloop_xfind(struct nn_multiloop *ml, uint n, uint *pairs, uint i, uint j)
{
    ml->unpaired = ml->nstems = ml->ndangle5 = ml->ndangle3 = 0;
    while (i <= j) {
        if (pairs[i] == NA_UNPAIRED) {
            ml->unpaired++;
        } else if (i < pairs[i]) {
            /* add stem */
            ml->stems[ml->nstems][0] = i;
            ml->stems[ml->nstems][1] = pairs[i];
            ml->nstems++;
            if (i > 0) {
                /* add dangle5 */
                ml->dangle5[ml->ndangle5][0] = i;
                ml->dangle5[ml->ndangle5][1] = pairs[i];
                ml->dangle5[ml->ndangle5][2] = i - 1;
                ml->ndangle5++;
            }
            if (pairs[i] < n - 1) {
                /* add dangle3 */
                ml->dangle3[ml->ndangle3][0] = i;
                ml->dangle3[ml->ndangle3][1] = pairs[i];
                ml->dangle3[ml->ndangle3][2] = pairs[i] + 1;
                ml->ndangle3++;
            }
            i = pairs[i];
        } else {
            printf("ERROR: nn_inter_set_multiloop: this should not be\n");
            exit(EXIT_FAILURE);
        }
        i += 1;
    }
}

/* find interactions for a pairlist given as part of a
   struct nn_inter */
void
find_interactions(struct nn_inter *inter)
{
    uint i, p, q, size1, size2, i1, j1, i2, j2;
    uint n = inter->n, *pairs = inter->pairs;
    struct nn_multiloop *ml;

    /* exterior loop */
    nn_multiloop_xfind(&inter->extloop, inter->n, inter->pairs,
                       0, inter->n - 1);
    /* hairpins, interior loops (including stacking basepairs) and multiloops */
    i = 0;
    while (i < n) {
        /* continue until we have found an opening basepairing
           i.e. we're at i of basepair (i, pairs[i]) */
        if (pairs[i] == NA_UNPAIRED || i > pairs[i]) {
            i++;
            continue;
        }
        /* Now search inwards from basepair (i, pairs[i]) for the next
           two base pairs.
           Remember that we are dealing with balanced brackets, where
           the number number of opening and closing brackets must be
           balanced for the whole sequence and any subsequence
           enclosed by brackets.
           The possible cases therefore are:
             - hairpin:   p and q run past each other, there are no more
                          base pairs inside
             - intloop:   we have found the base pair (p,q),
                          i.e. pairs[p] == q
             - multiloop: we have found base pairs (p,pairs[p]) and
                          (pairs[q],q) with pairs[p] != q
        */
        p = i + 1;
        q = pairs[i] - 1;
        while (pairs[p] == NA_UNPAIRED && p < pairs[i])
            p++;
        while (pairs[q] == NA_UNPAIRED && q > i)
            q--;
        /* now we can tell what loop type we have */
        if (q < p) {
            /* hairpin loop - pairs have run past each other */
            struct nn_hairpin hp = { .i = i, .j = pairs[i],
                                     .size = pairs[i] - i - 1 };
            inter->hairpin[inter->nhairpin++] = hp;
        } else {
            /* p and q have found a basepair inside. we now must
               discern if they belong to the same or separate pairs;
               this will tell us if we are dealing with an internal
               loop or a multiloop */
            if (pairs[p] == q) {
                /* stacking pair, bulge loop or internal loop - pairs are
                   the same */
                size1 = p - i - 1;
                size2 = pairs[i] - q - 1;
                i1 = i;
                j1 = pairs[i];
                i2 = p;
                j2 = pairs[p];
                if (size1 == 0 && size2 == 0) {
                    /* stacking pair of basepairs */
                    struct nn_stack st = { .i1 = i1, .j1 = j1,
                                           .i2 = i2, .j2 = j2 };
                    inter->stack[inter->nstack++] = st;
                } else if (size1 == 0 || size2 == 0) {
                    /* bulge loop */
                    struct nn_bulge bg = { .i1 = i1, .j1 = j1,
                                           .i2 = i2, .j2 = j2,
                                           .size = MAX(size1, size2) };
                    inter->bulge[inter->nbulge++] = bg;
                } else {
                    /* generic internal loop */
                    struct nn_intloop in = { .i1 = i1, .j1 = j1,
                                             .i2 = i2, .j2 = j2,
                                             .size1 = size1, .size2 = size2};
                    inter->intloop[inter->nintloop++] = in;
                }
            } else {
                /* multiloop - pairs are different */
                ml = &inter->multiloop[inter->nmultiloop];
                nn_multiloop_xfind(ml, inter->n, inter->pairs, i + 1, pairs[i] - 1);

                /* TODO: perhaps move this into nn_multiloop_xfind
                         assumes there is enough space in arrays */
                /* add the basepair initiating the multiloop to the stems */
                ml = &inter->multiloop[inter->nmultiloop];
                ml->stems[ml->nstems][0] = i;
                ml->stems[ml->nstems][1] = pairs[i];
                ml->nstems++;
                /* Add extra dangles for outer basepair of multiloop.
                   Note the reversed order of i and pairs[i], this is
                   needed as the opening base pair of the multiloop is
                   really the other way around if we want to have the
                   dangling base on the inside of the loop */
                ml->dangle5[ml->ndangle5][0] = pairs[i];
                ml->dangle5[ml->ndangle5][1] = i;
                ml->dangle5[ml->ndangle5][2] = pairs[i] - 1;
                ml->ndangle5++;
                ml->dangle3[ml->ndangle3][0] = pairs[i];
                ml->dangle3[ml->ndangle3][1] = i;
                ml->dangle3[ml->ndangle3][2] = i + 1;
                ml->ndangle3++;

                inter->nmultiloop++;
            }
        }
        /* move to next paired base */
        i = p;
    }
}

void
print_multiloop(const struct nn_multiloop *ml, const char *indent)
{
    uint i;
    printf("%sunpaired: %u\n", indent, ml->unpaired);
    printf("%sstems:   ", indent);
    for (i = 0; i < ml->nstems; i++)
        printf(" (%u, %u) ", ml->stems[i][0], ml->stems[i][1]);
    printf("\n");
    printf("%sdangle5:  ", indent);
    for (i = 0; i < ml->ndangle5; i++)
        printf(" ((%u, %u), %u) ", ml->dangle5[i][0], ml->dangle5[i][1],
               ml->dangle5[i][2]);
    printf("\n");
    printf("%sdangle3:  ", indent);
    for (i = 0; i < ml->ndangle3; i++)
        printf(" ((%u, %u), %u) ", ml->dangle3[i][0], ml->dangle3[i][1],
               ml->dangle3[i][2]);
    printf("\n");
}

void
print_interactions(const struct nn_inter *inter)
{
    uint i;
    printf("interactions\n"
           "------------\n");
    printf("exterior loop\n");
    print_multiloop(&inter->extloop, "    ");
    printf("stacking basepairs\n");
    for (i = 0; i < inter->nstack; i++)
        printf("    (%u, %u) - (%u, %u)\n",
               inter->stack[i].i1, inter->stack[i].j1,
               inter->stack[i].i2, inter->stack[i].j2);
    printf("bulge loops\n");
    for (i = 0; i < inter->nbulge; i++)
        printf("    (%u, %u) - (%u, %u), size = %u\n",
               inter->bulge[i].i1, inter->bulge[i].j1,
               inter->bulge[i].i2, inter->bulge[i].j2, inter->bulge[i].size);
    printf("internal loops\n");
    for (i = 0; i < inter->nintloop; i++)
        printf("    (%u, %u) - (%u, %u), size1 = %u, size2 = %u\n",
               inter->intloop[i].i1, inter->intloop[i].j1,
               inter->intloop[i].i2, inter->intloop[i].j2,
               inter->intloop[i].size1, inter->intloop[i].size2);
    printf("hairpins\n");
    for (i = 0; i < inter->nhairpin; i++)
        printf("    (%u, %u), size = %u\n",
               inter->hairpin[i].i, inter->hairpin[i].j,
               inter->hairpin[i].size);
    printf("multiloops\n");
    for (i = 0; i < inter->nmultiloop; i++)
        print_multiloop(&inter->multiloop[i], "    ");
}

/* TODO: statistics are wrong, don't use when unbiased random
         secondary structures are needed. Also the stuctures turn out
         to be very sparse */
/* generate random structures with minimum size of hairpins of hpmin */
static void helper_random_pairs_iter(uint *pairs, uint i, uint j, uint hpmin);
void
random_pairs(uint n, uint *pairs, uint hpmin)
{
    uint i;
    for (i = 0; i < n; i++)
        pairs[i] = NA_UNPAIRED;
    helper_random_pairs_iter(pairs, 0, n - 1, hpmin);
}
static void
helper_random_pairs_iter(uint *pairs, uint i, uint j, uint hpmin)
{
    uint p, q;
    if (i > j - hpmin - 1)
        return;
    p = random_uint(i, j - hpmin - 1);
    q = random_uint(p + hpmin + 1, j);
    if (p != i - 1) {
        pairs[p] = q;
        pairs[q] = p;
        helper_random_pairs_iter(pairs, p + 1, q - 1, hpmin);
        helper_random_pairs_iter(pairs, q + 1, j, hpmin);
    }
}

/* generate a random sequence compatible with a given secondary
   structure */
void
random_useq(uint n, const uint *pairs, uint *useq)
{
    uint i, bp;
    for (i = 0; i < n; i++) {
        if (pairs[i] == NA_UNPAIRED) {
            useq[i] = random_uint(0, NA_NBASES - 1);
        } else if (i < pairs[i]) {
            /* we found i of the pair (i, pairs[i]) */
            bp = random_uint(0, NA_NBASEPAIRS - 1);
            useq[i] = NA_BASEPAIR_TO_BASES[bp][0];
            useq[pairs[i]] = NA_BASEPAIR_TO_BASES[bp][1];
        }
    }
}

/* random pure sequence composition compatible with a given set of
   base pairs */
void
random_pseq(uint n, uint ndim, const uint *pairs, double **p)
{
    uint i, j, bp;
    for (i = 0; i < n; i++) {
        if (pairs[i] == NA_UNPAIRED) {
            for (j = 0; j < ndim; j++)
                p[i][j] = 0;
            p[i][random_uint(0, NA_NBASES - 1)] = 1;
        } else if (i < pairs[i]) {
            /* we found i of the pair (i, pairs[i]) */
            for (j = 0; j < ndim; j++)
                p[i][j] = 0;
            for (j = 0; j < ndim; j++)
                p[pairs[i]][j] = 0;
            bp = random_uint(0, NA_NBASEPAIRS - 1);
            p[i][NA_BASEPAIR_TO_BASES[bp][0]] = 1;
            p[pairs[i]][NA_BASEPAIR_TO_BASES[bp][1]] = 1;
        }
    }
}

int
calc_interactions(const struct nn_inter *inter, const uint *useq)
{
    uint i;
    int G = 0, Ge = 0, Gs = 0, Gb = 0, Gi = 0, Gh = 0, Gm = 0;
    /* external loop */
    Ge = G_extloop_multiloop(useq, inter->extloop.unpaired,
                             inter->extloop.nstems, inter->extloop.stems,
                             inter->extloop.ndangle5,
                             inter->extloop.dangle5,
                             inter->extloop.ndangle3,
                             inter->extloop.dangle3,
                             false);
    /* stacking pair of basepairs */
    for (i = 0; i < inter->nstack; i++)
        Gs += G_stack(useq, inter->stack[i].i1, inter->stack[i].j1,
                      inter->stack[i].i2, inter->stack[i].j2);
    /* bulge loops */
    for (i = 0; i < inter->nbulge; i++)
        Gb += G_bulgeloop(useq, inter->bulge[i].size,
                          inter->bulge[i].i1, inter->bulge[i].j1,
                          inter->bulge[i].i2, inter->bulge[i].j2);
    /* internal loops */
    for (i = 0; i < inter->nintloop; i++)
        Gi += G_intloop(useq,
                        inter->intloop[i].size1, inter->intloop[i].size2,
                        inter->intloop[i].i1, inter->intloop[i].j1,
                        inter->intloop[i].i2, inter->intloop[i].j2);
    /* hairpins */
    for (i = 0; i < inter->nhairpin; i++)
        Gh += G_hairpin(useq, inter->hairpin[i].size,
                        inter->hairpin[i].i, inter->hairpin[i].j);
    /* multiloops */
    for (i = 0; i < inter->nmultiloop; i++)
        Gm += G_extloop_multiloop(useq, inter->multiloop[i].unpaired,
                                  inter->multiloop[i].nstems,
                                  inter->multiloop[i].stems,
                                  inter->multiloop[i].ndangle5,
                                  inter->multiloop[i].dangle5,
                                  inter->multiloop[i].ndangle3,
                                  inter->multiloop[i].dangle3,
                                  true);
#if 0
    printf("nintloop = %u\n", inter->nintloop);
    printf("extloop   = %d\n"
           "stack     = %d\n"
           "bulge     = %d\n"
           "intloop   = %d\n"
           "hairpin   = %d\n"
           "multiloop = %d\n",
           Ge, Gs, Gb, Gi, Gh, Gm);
#endif
    G = Ge + Gs + Gb + Gi + Gh + Gm;
    return G;
}

double
calc_interactions_pseq(const struct nn_inter *inter, double **p)
{
    uint i;
    double G = 0, Ge = 0, Gs = 0, Gb = 0, Gi = 0, Gh = 0, Gm = 0;
    /* external loop */
    Ge = G_extloop_multiloop_pseq(p, inter->extloop.unpaired,
                                  inter->extloop.nstems, inter->extloop.stems,
                                  inter->extloop.ndangle5,
                                  inter->extloop.dangle5,
                                  inter->extloop.ndangle3,
                                  inter->extloop.dangle3,
                                  false);
    /* stacking pair of basepairs */
    for (i = 0; i < inter->nstack; i++)
        Gs += G_stack_pseq(p, inter->stack[i].i1, inter->stack[i].j1,
                           inter->stack[i].i2, inter->stack[i].j2);
    /* bulge loops */
    for (i = 0; i < inter->nbulge; i++)
        Gb += G_bulgeloop_pseq(p, inter->bulge[i].size,
                               inter->bulge[i].i1, inter->bulge[i].j1,
                               inter->bulge[i].i2, inter->bulge[i].j2);
    /* internal loops */
    for (i = 0; i < inter->nintloop; i++)
        Gi += G_intloop_pseq(p,
                             inter->intloop[i].size1, inter->intloop[i].size2,
                             inter->intloop[i].i1, inter->intloop[i].j1,
                             inter->intloop[i].i2, inter->intloop[i].j2);
    /* hairpins */
    for (i = 0; i < inter->nhairpin; i++)
        Gh += G_hairpin_pseq(p, inter->hairpin[i].size,
                             inter->hairpin[i].i, inter->hairpin[i].j);
    /* multiloops */
    for (i = 0; i < inter->nmultiloop; i++)
        Gm += G_extloop_multiloop_pseq(p, inter->multiloop[i].unpaired,
                                       inter->multiloop[i].nstems,
                                       inter->multiloop[i].stems,
                                       inter->multiloop[i].ndangle5,
                                       inter->multiloop[i].dangle5,
                                       inter->multiloop[i].ndangle3,
                                       inter->multiloop[i].dangle3,
                                       true);
#if 0
    printf("nintloop = %u\n", inter->nintloop);
    printf("extloop   = %f\n"
           "stack     = %f\n"
           "bulge     = %f\n"
           "intloop   = %f\n"
           "hairpin   = %f\n"
           "multiloop = %f\n",
           Ge, Gs, Gb, Gi, Gh, Gm);
#endif
    G = Ge + Gs + Gb + Gi + Gh + Gm;
    return G;
}

/* TODO: assumes dGdp array has been initialised to 0 */
void
calc_interactions_dGdp_pseq(const struct nn_inter *inter, double **p,
                            double **dGdp)
{
    uint i;
    /* external loop */
    dGdp_extloop_multiloop_pseq(p,
                                inter->extloop.nstems, inter->extloop.stems,
                                inter->extloop.ndangle5, inter->extloop.dangle5,
                                inter->extloop.ndangle3, inter->extloop.dangle3,
                                dGdp);
    /* stacking pair of basepairs */
    for (i = 0; i < inter->nstack; i++)
        dGdp_stack_pseq(p, inter->stack[i].i1, inter->stack[i].j1,
                        inter->stack[i].i2, inter->stack[i].j2, dGdp);
    /* bulge loops */
    for (i = 0; i < inter->nbulge; i++)
        dGdp_bulgeloop_pseq(p, inter->bulge[i].size,
                            inter->bulge[i].i1, inter->bulge[i].j1,
                            inter->bulge[i].i2, inter->bulge[i].j2, dGdp);
    /* internal loops */
    for (i = 0; i < inter->nintloop; i++)
        dGdp_intloop_pseq(p,
                          inter->intloop[i].size1, inter->intloop[i].size2,
                          inter->intloop[i].i1, inter->intloop[i].j1,
                          inter->intloop[i].i2, inter->intloop[i].j2, dGdp);
    /* hairpins */
    for (i = 0; i < inter->nhairpin; i++)
        dGdp_hairpin_pseq(p, inter->hairpin[i].size,
                          inter->hairpin[i].i, inter->hairpin[i].j, dGdp);
    /* multiloops */
    for (i = 0; i < inter->nmultiloop; i++)
        dGdp_extloop_multiloop_pseq(p,
                                    inter->multiloop[i].nstems,
                                    inter->multiloop[i].stems,
                                    inter->multiloop[i].ndangle5,
                                    inter->multiloop[i].dangle5,
                                    inter->multiloop[i].ndangle3,
                                    inter->multiloop[i].dangle3,
                                    dGdp);
}



/*
  functions for the free energy contributions of different loop types
*/

/* helper functions */

uint
idx_to_pairtype(const uint *seq, uint i, uint j)
{
    return NA_2BASES_TO_PAIRTYPE[seq[i]][seq[j]]; /* TODO: array hardcoded */
}

int
G_non_gc_penalty(uint pairtype)
{
    return NN_NON_GC_PENALTY_FOR_BP[pairtype];
}

double
G_non_gc_penalty_pseq(double **p, uint i1, uint j1)
{
    uint i, j;
    double G = 0;
    for (i = 0; i < NA_NBASES; i++)
        for (j = 0; j < NA_NBASES; j++)
            G += p[i1][i] * p[j1][j] * NN_NON_GC_PENALTY_FOR_BASES[i][j];
    return G;
}

void
dGdp_non_gc_penalty_pseq(double **p, uint i1, uint j1, double **dGdp)
{
    uint i, j;
    for (i = 0; i < NA_NBASES; i++) {
        for (j = 0; j < NA_NBASES; j++) {
            dGdp[i1][i] += p[j1][j] * NN_NON_GC_PENALTY_FOR_BASES[i][j];
            dGdp[j1][j] += p[i1][i] * NN_NON_GC_PENALTY_FOR_BASES[i][j];
        }
    }
}

int
G_dangle5(const uint *seq, uint i1, uint j1, uint b)
{
    uint pair_type = idx_to_pairtype(seq, i1, j1);
    uint base_type = seq[b];
    return nn_G_dangle5[pair_type][base_type];
}

double
G_dangle5_pseq(double **p, uint i1, uint j1, uint b)
{
    uint i, j, bt_i1, bt_j1;
    double G = 0;
    for (i = 0; i < NA_NBASEPAIRS; i++) {
        for (j = 0; j < NA_NBASES; j++) {
            bt_i1 = NA_BASEPAIR_TO_BASES[i][0];
            bt_j1 = NA_BASEPAIR_TO_BASES[i][1];
            G += p[i1][bt_i1] * p[j1][bt_j1] * p[b][j] * nn_G_dangle5[i][j];
        }
    }
    return G;
}

void
dGdp_dangle5_pseq(double **p, uint i1, uint j1, uint b, double **dGdp)
{
    uint i, j, bt_i1, bt_j1;
    for (i = 0; i < NA_NBASEPAIRS; i++) {
        for (j = 0; j < NA_NBASES; j++) {
            bt_i1 = NA_BASEPAIR_TO_BASES[i][0];
            bt_j1 = NA_BASEPAIR_TO_BASES[i][1];
            dGdp[i1][bt_i1] += p[j1][bt_j1] * p[b][j] * nn_G_dangle5[i][j];
            dGdp[j1][bt_j1] += p[i1][bt_i1] * p[b][j] * nn_G_dangle5[i][j];
            dGdp[b][j]      += p[i1][bt_i1] * p[j1][bt_j1] * nn_G_dangle5[i][j];
        }
    }
}

int
G_dangle3(const uint *seq, uint i1, uint j1, uint b)
{
    uint pair_type = idx_to_pairtype(seq, i1, j1);
    uint base_type = seq[b];
    return nn_G_dangle3[pair_type][base_type];
}

double
G_dangle3_pseq(double **p, uint i1, uint j1, uint b)
{
    uint i, j, bt_i1, bt_j1;
    double G = 0;
    for (i = 0; i < NA_NBASEPAIRS; i++) {
        for (j = 0; j < NA_NBASES; j++) {
            bt_i1 = NA_BASEPAIR_TO_BASES[i][0];
            bt_j1 = NA_BASEPAIR_TO_BASES[i][1];
            G += p[i1][bt_i1] * p[j1][bt_j1] * p[b][j] * nn_G_dangle3[i][j];
        }
    }
    return G;
}

void
dGdp_dangle3_pseq(double **p, uint i1, uint j1, uint b, double **dGdp)
{
    uint i, j, bt_i1, bt_j1;
    for (i = 0; i < NA_NBASEPAIRS; i++) {
        for (j = 0; j < NA_NBASES; j++) {
            bt_i1 = NA_BASEPAIR_TO_BASES[i][0];
            bt_j1 = NA_BASEPAIR_TO_BASES[i][1];
            dGdp[i1][bt_i1] += p[j1][bt_j1] * p[b][j] * nn_G_dangle3[i][j];
            dGdp[j1][bt_j1] += p[i1][bt_i1] * p[b][j] * nn_G_dangle3[i][j];
            dGdp[b][j]      += p[i1][bt_i1] * p[j1][bt_j1] * nn_G_dangle3[i][j];
        }
    }
}



/* loop free energy calculations */

int
G_stack(const uint *seq, uint i1, uint j1, uint i2, uint j2)
{
    uint pt1, pt2;
    pt1 = idx_to_pairtype(seq, i1, j1);
    pt2 = idx_to_pairtype(seq, j2, i2); /* note order of i2,j2 swapped */
    return nn_G_stack[pt1][pt2];
}

double
G_stack_pseq(double **p, uint i1, uint j1, uint i2, uint j2)
{
    uint i, j, bt_i1, bt_j1, bt_i2, bt_j2;
    double G = 0;
    for (i = 0; i < NA_NBASEPAIRS; i++) {
        for (j = 0; j < NA_NBASEPAIRS; j++) {
            bt_i1 = NA_BASEPAIR_TO_BASES[i][0];
            bt_j1 = NA_BASEPAIR_TO_BASES[i][1];
            bt_i2 = NA_BASEPAIR_TO_BASES[j][1];/* note order of i2,j2 swapped */
            bt_j2 = NA_BASEPAIR_TO_BASES[j][0];
            G +=  p[i1][bt_i1] * p[j1][bt_j1]
                * p[i2][bt_i2] * p[j2][bt_j2]
                * nn_G_stack[i][j];
        }
    }
    return G;
}

void
dGdp_stack_pseq(double **p, uint i1, uint j1, uint i2, uint j2,
                double **dGdp)
{
    uint i, j, bt_i1, bt_j1, bt_i2, bt_j2;
    for (i = 0; i < NA_NBASEPAIRS; i++) {
        for (j = 0; j < NA_NBASEPAIRS; j++) {
            bt_i1 = NA_BASEPAIR_TO_BASES[i][0];
            bt_j1 = NA_BASEPAIR_TO_BASES[i][1];
            bt_i2 = NA_BASEPAIR_TO_BASES[j][1];/* note order of i2,j2 swapped */
            bt_j2 = NA_BASEPAIR_TO_BASES[j][0];
            dGdp[i1][bt_i1] += p[j1][bt_j1] * p[i2][bt_i2] * p[j2][bt_j2]
                * nn_G_stack[i][j];
            dGdp[j1][bt_j1] += p[i1][bt_i1] * p[i2][bt_i2] * p[j2][bt_j2]
                * nn_G_stack[i][j];
            dGdp[i2][bt_i2] += p[i1][bt_i1] * p[j1][bt_j1] * p[j2][bt_j2]
                * nn_G_stack[i][j];
            dGdp[j2][bt_j2] += p[i1][bt_i1] * p[j1][bt_j1] * p[i2][bt_i2]
                * nn_G_stack[i][j];
        }
    }
}

int
G_bulgeloop(const uint *seq, uint size, uint i1, uint j1, uint i2, uint j2)
{
    uint pt1, pt2;
    int G = 0;

    pt1 = idx_to_pairtype(seq, i1, j1);
    pt2 = idx_to_pairtype(seq, j2, i2); /* note order of i2,j2 swapped */
    if (size < NN_N_BULGELOOP) {
        G += nn_G_bulge_loop[size];
    } else {
        G += nn_G_bulge_loop[NN_N_BULGELOOP - 1]
            + (int) (NN_LXC37 * log((double) size / (NN_N_BULGELOOP - 1)));
    }
    if (size == 1) {
        G += G_stack(seq, i1, j1, i2, j2);
    } else {
        /* bulge loops larger than 1 get penalty term for non-gc closing
           basepairs */
        G += G_non_gc_penalty(pt1);
        G += G_non_gc_penalty(pt2);
    }
    return G;
}

double
G_bulgeloop_pseq(double **p, uint size, uint i1, uint j1, uint i2, uint j2)
{
    double G = 0;
    if (size < NN_N_BULGELOOP) {
        G += nn_G_bulge_loop[size];
    } else {
        G += nn_G_bulge_loop[NN_N_BULGELOOP - 1]
            + (int) (NN_LXC37 * log((double) size / (NN_N_BULGELOOP - 1)));
    }
    if (size == 1) {
        G += G_stack_pseq(p, i1, j1, i2, j2);
    } else {
        /* bulge loops larger than 1 get penalty term for non-gc closing
           basepairs */
        G += G_non_gc_penalty_pseq(p, i1, j1);
        G += G_non_gc_penalty_pseq(p, j2, i2); /* note order of i2,j2 swapped */
    }
    return G;
}

void
dGdp_bulgeloop_pseq(double **p, uint size, uint i1, uint j1, uint i2, uint j2,
                    double **dGdp)
{
    if (size == 1) {
        dGdp_stack_pseq(p, i1, j1, i2, j2, dGdp);
    } else {
        /* bulge loops larger than 1 get penalty term for non-gc closing
           basepairs */
        dGdp_non_gc_penalty_pseq(p, i1, j1, dGdp);
        dGdp_non_gc_penalty_pseq(p, j2, i2, dGdp); /* note i2,j2 swapped */
    }
}

int
G_intloop(const uint *seq, uint size1, uint size2,
          uint i1, uint j1, uint i2, uint j2)
{
    uint pt1, pt2, b1, b2, b3, b4, size;
    int G = 0;

    pt1 = idx_to_pairtype(seq, i1, j1);
    pt2 = idx_to_pairtype(seq, j2, i2); /* note the reversed order of i2,j2 */
    b1 = seq[i1 + 1];
    b2 = seq[i2 - 1];
    b3 = seq[j2 + 1];
    b4 = seq[j1 - 1];
    if (size1 == 1 && size2 == 1) {
        /* 1x1 internal loop */
        return nn_G_int11[pt1][pt2][b1][b3];
    } else if (size1 == 1 && size2 == 2) {
        /* 1x2 internal loop */
        return nn_G_int21[pt1][pt2][b1][b3][b4];
    } else if (size1 == 2 && size2 == 1) {
        /* 2x1 internal loop */
        /* note switched order of pt1 and pt2 compared to 1x2 loop */
        return nn_G_int21[pt2][pt1][b3][b1][b2];
    } else if (size1 == 2 && size2 == 2) {
        /* 2x2 internal loop */
        return nn_G_int22[pt1][pt2][b1][b2][b3][b4];
    } else {
        /* generic internal loop */
        size = size1 + size2;
        if (size < NN_N_INTERNALLOOP)
            G += nn_G_internal_loop[size];
        else
            G += nn_G_internal_loop[NN_N_INTERNALLOOP - 1]
              + (int) (NN_LXC37 * log((double) size / (NN_N_INTERNALLOOP - 1)));
        /* loop asymmetry contribution */
        G += MIN(NN_NINIO_MAX, abs(size1 - size2) * NN_NINIO_M);
        /* mismatch contribution */
        G += nn_G_mismatch_interior[pt1][b1][b4];
        G += nn_G_mismatch_interior[pt2][b3][b2];
        return G;
    }
}

double
G_intloop_pseq(double **p, uint size1, uint size2,
               uint i1, uint j1, uint i2, uint j2)
{
    uint i, j, k, k1, k2, k3, k4, b1, b2, b3, b4, size,
        bt_i1, bt_j1, bt_i2, bt_j2, bt_i, bt_j;
    double G = 0;
    b1 = i1 + 1;
    b2 = i2 - 1;
    b3 = j2 + 1;
    b4 = j1 - 1;
    if (size1 == 1 && size2 == 1) {
        /* 1x1 internal loop */
        for (i = 0; i < NA_NBASEPAIRS; i++)
            for (j = 0; j < NA_NBASEPAIRS; j++)
                for (k1 = 0; k1 < NA_NBASES; k1++)
                    for (k3 = 0; k3 < NA_NBASES; k3++) {
                        bt_i1 = NA_BASEPAIR_TO_BASES[i][0];
                        bt_j1 = NA_BASEPAIR_TO_BASES[i][1];
                        /* note order of i2,j2 swapped */
                        bt_i2 = NA_BASEPAIR_TO_BASES[j][1];
                        bt_j2 = NA_BASEPAIR_TO_BASES[j][0];
                        G +=  p[i1][bt_i1] * p[j1][bt_j1]
                            * p[i2][bt_i2] * p[j2][bt_j2]
                            * p[b1][k1] * p[b3][k3]
                            * nn_G_int11[i][j][k1][k3];
                    }
        return G;
    } else if (size1 == 1 && size2 == 2) {
        /* 1x2 internal loop */
        for (i = 0; i < NA_NBASEPAIRS; i++)
            for (j = 0; j < NA_NBASEPAIRS; j++)
                for (k1 = 0; k1 < NA_NBASES; k1++)
                    for (k3 = 0; k3 < NA_NBASES; k3++)
                        for (k4 = 0; k4 < NA_NBASES; k4++) {
                            bt_i1 = NA_BASEPAIR_TO_BASES[i][0];
                            bt_j1 = NA_BASEPAIR_TO_BASES[i][1];
                            /* note order of i2,j2 swapped */
                            bt_i2 = NA_BASEPAIR_TO_BASES[j][1];
                            bt_j2 = NA_BASEPAIR_TO_BASES[j][0];
                            G +=  p[i1][bt_i1] * p[j1][bt_j1]
                                * p[i2][bt_i2] * p[j2][bt_j2]
                                * p[b1][k1] * p[b3][k3] * p[b4][k4]
                                * nn_G_int21[i][j][k1][k3][k4];
                        }
        return G;
    } else if (size1 == 2 && size2 == 1) {
        /* 2x1 internal loop */
        for (i = 0; i < NA_NBASEPAIRS; i++)
            for (j = 0; j < NA_NBASEPAIRS; j++)
                for (k3 = 0; k3 < NA_NBASES; k3++)
                    for (k1 = 0; k1 < NA_NBASES; k1++)
                        for (k2 = 0; k2 < NA_NBASES; k2++) {
                            bt_i1 = NA_BASEPAIR_TO_BASES[i][0];
                            bt_j1 = NA_BASEPAIR_TO_BASES[i][1];
                            /* note order of i2,j2 swapped */
                            bt_i2 = NA_BASEPAIR_TO_BASES[j][1];
                            bt_j2 = NA_BASEPAIR_TO_BASES[j][0];
                            /* note switched order of i and j compared to
                               1x2 loop */
                            G +=  p[i1][bt_i1] * p[j1][bt_j1]
                                * p[i2][bt_i2] * p[j2][bt_j2]
                                * p[b3][k3] * p[b1][k1] * p[b2][k2]
                                * nn_G_int21[j][i][k3][k1][k2];
                        }
        return G;
    } else if (size1 == 2 && size2 == 2) {
        /* 2x2 internal loop */
        for (i = 0; i < NA_NBASEPAIRS; i++)
            for (j = 0; j < NA_NBASEPAIRS; j++)
                for (k1 = 0; k1 < NA_NBASES; k1++)
                    for (k2 = 0; k2 < NA_NBASES; k2++)
                        for (k3 = 0; k3 < NA_NBASES; k3++)
                            for (k4 = 0; k4 < NA_NBASES; k4++) {
                                bt_i1 = NA_BASEPAIR_TO_BASES[i][0];
                                bt_j1 = NA_BASEPAIR_TO_BASES[i][1];
                                /* note order of i2,j2 swapped */
                                bt_i2 = NA_BASEPAIR_TO_BASES[j][1];
                                bt_j2 = NA_BASEPAIR_TO_BASES[j][0];
                                G +=  p[i1][bt_i1] * p[j1][bt_j1]
                                    * p[i2][bt_i2] * p[j2][bt_j2]
                                    * p[b1][k1] * p[b2][k2]
                                    * p[b3][k3] * p[b4][k4]
                                    * nn_G_int22[i][j][k1][k2][k3][k4];
                            }
        return G;
    } else {
        /* generic internal loop */
        size = size1 + size2;
        if (size < NN_N_INTERNALLOOP)
            G += nn_G_internal_loop[size];
        else
            G += nn_G_internal_loop[NN_N_INTERNALLOOP - 1]
              + (int) (NN_LXC37 * log((double) size / (NN_N_INTERNALLOOP - 1)));
        /* loop asymmetry contribution */
        G += MIN(NN_NINIO_MAX, abs(size1 - size2) * NN_NINIO_M);
        /* mismatch contribution */
        for (i = 0; i < NA_NBASEPAIRS; i++)
            for (j = 0; j < NA_NBASES; j++)
                for (k = 0; k < NA_NBASES; k++) {
                    bt_i = NA_BASEPAIR_TO_BASES[i][0];
                    bt_j = NA_BASEPAIR_TO_BASES[i][1];
                    G +=  p[i1][bt_i] * p[j1][bt_j]
                        * p[b1][j] * p[b4][k]
                        * nn_G_mismatch_interior[i][j][k];
                    G +=  p[i2][bt_i] * p[j2][bt_j]
                        * p[b3][j] * p[b2][k]
                        * nn_G_mismatch_interior[i][j][k];
                }
        return G;
    }
}

void
dGdp_intloop_pseq(double **p, uint size1, uint size2,
                  uint i1, uint j1, uint i2, uint j2, double **dGdp)
{
    uint i, j, k, k1, k2, k3, k4, b1, b2, b3, b4,
        bt_i1, bt_j1, bt_i2, bt_j2, bt_i, bt_j;
    b1 = i1 + 1;
    b2 = i2 - 1;
    b3 = j2 + 1;
    b4 = j1 - 1;
    if (size1 == 1 && size2 == 1) {
        /* 1x1 internal loop */
        for (i = 0; i < NA_NBASEPAIRS; i++)
            for (j = 0; j < NA_NBASEPAIRS; j++)
                for (k1 = 0; k1 < NA_NBASES; k1++)
                    for (k3 = 0; k3 < NA_NBASES; k3++) {
                        bt_i1 = NA_BASEPAIR_TO_BASES[i][0];
                        bt_j1 = NA_BASEPAIR_TO_BASES[i][1];
                        /* note order of i2,j2 swapped */
                        bt_i2 = NA_BASEPAIR_TO_BASES[j][1];
                        bt_j2 = NA_BASEPAIR_TO_BASES[j][0];
                        dGdp[i1][bt_i1] += p[j1][bt_j1] * p[i2][bt_i2]
                            * p[j2][bt_j2] * p[b1][k1] * p[b3][k3]
                            * nn_G_int11[i][j][k1][k3];
                        dGdp[j1][bt_j1] += p[i1][bt_i1] * p[i2][bt_i2]
                            * p[j2][bt_j2] * p[b1][k1] * p[b3][k3]
                            * nn_G_int11[i][j][k1][k3];
                        dGdp[i2][bt_i2] += p[i1][bt_i1] * p[j1][bt_j1]
                            * p[j2][bt_j2] * p[b1][k1] * p[b3][k3]
                            * nn_G_int11[i][j][k1][k3];
                        dGdp[j2][bt_j2] += p[i1][bt_i1] * p[j1][bt_j1]
                            * p[i2][bt_i2] * p[b1][k1] * p[b3][k3]
                            * nn_G_int11[i][j][k1][k3];
                        dGdp[b1][k1] += p[i1][bt_i1] * p[j1][bt_j1]
                            * p[i2][bt_i2] * p[j2][bt_j2] * p[b3][k3]
                            * nn_G_int11[i][j][k1][k3];
                        dGdp[b3][k3] += p[i1][bt_i1] * p[j1][bt_j1]
                            * p[i2][bt_i2] * p[j2][bt_j2] * p[b1][k1]
                            * nn_G_int11[i][j][k1][k3];
                    }
    } else if (size1 == 1 && size2 == 2) {
        /* 1x2 internal loop */
        for (i = 0; i < NA_NBASEPAIRS; i++)
            for (j = 0; j < NA_NBASEPAIRS; j++)
                for (k1 = 0; k1 < NA_NBASES; k1++)
                    for (k3 = 0; k3 < NA_NBASES; k3++)
                        for (k4 = 0; k4 < NA_NBASES; k4++) {
                            bt_i1 = NA_BASEPAIR_TO_BASES[i][0];
                            bt_j1 = NA_BASEPAIR_TO_BASES[i][1];
                            /* note order of i2,j2 swapped */
                            bt_i2 = NA_BASEPAIR_TO_BASES[j][1];
                            bt_j2 = NA_BASEPAIR_TO_BASES[j][0];
                            dGdp[i1][bt_i1] +=
                                p[j1][bt_j1] * p[i2][bt_i2] * p[j2][bt_j2]
                                * p[b1][k1] * p[b3][k3] * p[b4][k4]
                                * nn_G_int21[i][j][k1][k3][k4];
                            dGdp[j1][bt_j1] +=
                                p[i1][bt_i1] * p[i2][bt_i2] * p[j2][bt_j2]
                                * p[b1][k1] * p[b3][k3] * p[b4][k4]
                                * nn_G_int21[i][j][k1][k3][k4];
                            dGdp[i2][bt_i2] +=
                                p[i1][bt_i1] * p[j1][bt_j1] * p[j2][bt_j2]
                                * p[b1][k1] * p[b3][k3] * p[b4][k4]
                                * nn_G_int21[i][j][k1][k3][k4];
                            dGdp[j2][bt_j2] +=
                                p[i1][bt_i1] * p[j1][bt_j1] * p[i2][bt_i2]
                                * p[b1][k1] * p[b3][k3] * p[b4][k4]
                                * nn_G_int21[i][j][k1][k3][k4];
                            dGdp[b1][k1] +=
                                p[i1][bt_i1] * p[j1][bt_j1]
                                * p[i2][bt_i2] * p[j2][bt_j2]
                                * p[b3][k3] * p[b4][k4]
                                * nn_G_int21[i][j][k1][k3][k4];
                            dGdp[b3][k3] +=
                                p[i1][bt_i1] * p[j1][bt_j1]
                                * p[i2][bt_i2] * p[j2][bt_j2]
                                * p[b1][k1] * p[b4][k4]
                                * nn_G_int21[i][j][k1][k3][k4];
                            dGdp[b4][k4] +=
                                p[i1][bt_i1] * p[j1][bt_j1]
                                * p[i2][bt_i2] * p[j2][bt_j2]
                                * p[b1][k1] * p[b3][k3]
                                * nn_G_int21[i][j][k1][k3][k4];
                        }
    } else if (size1 == 2 && size2 == 1) {
       /* 2x1 internal loop */
        for (i = 0; i < NA_NBASEPAIRS; i++)
            for (j = 0; j < NA_NBASEPAIRS; j++)
                for (k3 = 0; k3 < NA_NBASES; k3++)
                    for (k1 = 0; k1 < NA_NBASES; k1++)
                        for (k2 = 0; k2 < NA_NBASES; k2++) {
                            bt_i1 = NA_BASEPAIR_TO_BASES[i][0];
                            bt_j1 = NA_BASEPAIR_TO_BASES[i][1];
                            /* note order of i2,j2 swapped */
                            bt_i2 = NA_BASEPAIR_TO_BASES[j][1];
                            bt_j2 = NA_BASEPAIR_TO_BASES[j][0];
                            /* note switched order of i and j compared to
                               1x2 loop */
                            dGdp[i1][bt_i1] +=
                                p[j1][bt_j1] * p[i2][bt_i2] * p[j2][bt_j2]
                                * p[b3][k3] * p[b1][k1] * p[b2][k2]
                                * nn_G_int21[j][i][k3][k1][k2];
                            dGdp[j1][bt_j1] +=
                                p[i1][bt_i1] * p[i2][bt_i2] * p[j2][bt_j2]
                                * p[b3][k3] * p[b1][k1] * p[b2][k2]
                                * nn_G_int21[j][i][k3][k1][k2];
                            dGdp[i2][bt_i2] +=
                                p[i1][bt_i1] * p[j1][bt_j1] * p[j2][bt_j2]
                                * p[b3][k3] * p[b1][k1] * p[b2][k2]
                                * nn_G_int21[j][i][k3][k1][k2];
                            dGdp[j2][bt_j2] +=
                                p[i1][bt_i1] * p[j1][bt_j1] * p[i2][bt_i2]
                                * p[b3][k3] * p[b1][k1] * p[b2][k2]
                                * nn_G_int21[j][i][k3][k1][k2];
                            dGdp[b3][k3] +=
                                p[i1][bt_i1] * p[j1][bt_j1]
                                * p[i2][bt_i2] * p[j2][bt_j2]
                                * p[b1][k1] * p[b2][k2]
                                * nn_G_int21[j][i][k3][k1][k2];
                            dGdp[b1][k1] +=
                                p[i1][bt_i1] * p[j1][bt_j1]
                                * p[i2][bt_i2] * p[j2][bt_j2]
                                * p[b3][k3] * p[b2][k2]
                                * nn_G_int21[j][i][k3][k1][k2];
                            dGdp[b2][k2] +=
                                p[i1][bt_i1] * p[j1][bt_j1]
                                * p[i2][bt_i2] * p[j2][bt_j2]
                                * p[b3][k3] * p[b1][k1]
                                * nn_G_int21[j][i][k3][k1][k2];
                        }
    } else if (size1 == 2 && size2 == 2) {
        /* 2x2 internal loop */
        for (i = 0; i < NA_NBASEPAIRS; i++)
            for (j = 0; j < NA_NBASEPAIRS; j++)
                for (k1 = 0; k1 < NA_NBASES; k1++)
                    for (k2 = 0; k2 < NA_NBASES; k2++)
                        for (k3 = 0; k3 < NA_NBASES; k3++)
                            for (k4 = 0; k4 < NA_NBASES; k4++) {
                                bt_i1 = NA_BASEPAIR_TO_BASES[i][0];
                                bt_j1 = NA_BASEPAIR_TO_BASES[i][1];
                                /* note order of i2,j2 swapped */
                                bt_i2 = NA_BASEPAIR_TO_BASES[j][1];
                                bt_j2 = NA_BASEPAIR_TO_BASES[j][0];
                                dGdp[i1][bt_i1] +=
                                    p[j1][bt_j1] * p[i2][bt_i2] * p[j2][bt_j2]
                                    * p[b1][k1] * p[b2][k2]
                                    * p[b3][k3] * p[b4][k4]
                                    * nn_G_int22[i][j][k1][k2][k3][k4];
                                dGdp[j1][bt_j1] +=
                                    p[i1][bt_i1] * p[i2][bt_i2] * p[j2][bt_j2]
                                    * p[b1][k1] * p[b2][k2]
                                    * p[b3][k3] * p[b4][k4]
                                    * nn_G_int22[i][j][k1][k2][k3][k4];
                                dGdp[i2][bt_i2] +=
                                    p[i1][bt_i1] * p[j1][bt_j1] * p[j2][bt_j2]
                                    * p[b1][k1] * p[b2][k2]
                                    * p[b3][k3] * p[b4][k4]
                                    * nn_G_int22[i][j][k1][k2][k3][k4];
                                dGdp[j2][bt_j2] +=
                                    p[i1][bt_i1] * p[j1][bt_j1] * p[i2][bt_i2]
                                    * p[b1][k1] * p[b2][k2]
                                    * p[b3][k3] * p[b4][k4]
                                    * nn_G_int22[i][j][k1][k2][k3][k4];
                                dGdp[b1][k1] +=
                                    p[i1][bt_i1] * p[j1][bt_j1]
                                    * p[i2][bt_i2] * p[j2][bt_j2]
                                    * p[b2][k2] * p[b3][k3] * p[b4][k4]
                                    * nn_G_int22[i][j][k1][k2][k3][k4];
                                dGdp[b2][k2] +=
                                    p[i1][bt_i1] * p[j1][bt_j1]
                                    * p[i2][bt_i2] * p[j2][bt_j2]
                                    * p[b1][k1] * p[b3][k3] * p[b4][k4]
                                    * nn_G_int22[i][j][k1][k2][k3][k4];
                                dGdp[b3][k3] +=
                                    p[i1][bt_i1] * p[j1][bt_j1]
                                    * p[i2][bt_i2] * p[j2][bt_j2]
                                    * p[b1][k1] * p[b2][k2] * p[b4][k4]
                                    * nn_G_int22[i][j][k1][k2][k3][k4];
                                dGdp[b4][k4] +=
                                    p[i1][bt_i1] * p[j1][bt_j1]
                                    * p[i2][bt_i2] * p[j2][bt_j2]
                                    * p[b1][k1] * p[b2][k2] * p[b3][k3]
                                    * nn_G_int22[i][j][k1][k2][k3][k4];
                            }
    } else {
        /* generic internal loop */
        /* mismatch contribution */
        for (i = 0; i < NA_NBASEPAIRS; i++)
            for (j = 0; j < NA_NBASES; j++)
                for (k = 0; k < NA_NBASES; k++) {
                    bt_i = NA_BASEPAIR_TO_BASES[i][0];
                    bt_j = NA_BASEPAIR_TO_BASES[i][1];
                    dGdp[i1][bt_i] +=
                        p[j1][bt_j] * p[b1][j] * p[b4][k]
                        * nn_G_mismatch_interior[i][j][k];
                    dGdp[j1][bt_j] +=
                        p[i1][bt_i] * p[b1][j] * p[b4][k]
                        * nn_G_mismatch_interior[i][j][k];
                    dGdp[b1][j] +=
                        p[i1][bt_i] * p[j1][bt_j] * p[b4][k]
                        * nn_G_mismatch_interior[i][j][k];
                    dGdp[b4][k] +=
                        p[i1][bt_i] * p[j1][bt_j] * p[b1][j]
                        * nn_G_mismatch_interior[i][j][k];
                    dGdp[i2][bt_i] +=
                        p[j2][bt_j] * p[b3][j] * p[b2][k]
                        * nn_G_mismatch_interior[i][j][k];
                    dGdp[j2][bt_j] +=
                        p[i2][bt_i] * p[b3][j] * p[b2][k]
                        * nn_G_mismatch_interior[i][j][k];
                    dGdp[b3][j] +=
                        p[i2][bt_i] * p[j2][bt_j] * p[b2][k]
                        * nn_G_mismatch_interior[i][j][k];
                    dGdp[b2][k] +=
                        p[i2][bt_i] * p[j2][bt_j] * p[b3][j]
                        * nn_G_mismatch_interior[i][j][k];
                }
    }
}

int
G_hairpin(const uint *seq, uint size, uint i1, uint j1)
{
    uint i, j, pt, b1, b2;
    int G = 0;

    pt = idx_to_pairtype(seq, i1, j1);
    b1 = seq[i1 + 1];
    b2 = seq[j1 - 1];
    if (size < NN_N_HAIRPINLOOP)
        G += nn_G_hairpin_loop[size];
    else
        G += nn_G_hairpin_loop[NN_N_HAIRPINLOOP - 1]
            + (int) (NN_LXC37 * log((double) size / (NN_N_HAIRPINLOOP - 1)));
    /* mismatch penalty for the mismatch interior to the closing basepair of
       the hairpin. triloops get non-parameterised mismatch penalty */
    if (size == 3)
        G += G_non_gc_penalty(pt);
    else
        G += nn_G_mismatch_hairpin[pt][b1][b2];
    if (size == 4) {
        /* tetraloop bonus */
        for (i = 0; i < NN_NTETRALOOP; i++) {
            if (pt == nn_tetraloop_bp[i]) {
                for (j = 0; j < 4; j++)
                    if (nn_tetraloop_loop[i][j] != seq[i1 + j + 1])
                        break;
                if (j == 4) {
                    /* found the loop */
                    G += nn_G_tetraloop[i];
                    break; /* tetraloop occurrs only once, so we're done */
                }
            }
        }
    }

    return G;
}

double
G_hairpin_pseq(double **p, uint size, uint i1, uint j1)
{
    uint i, j, k, b1, b2, bt_i1, bt_j1;
    double G = 0, p_loop;
    b1 = i1 + 1;
    b2 = j1 - 1;
    if (size < NN_N_HAIRPINLOOP)
        G += nn_G_hairpin_loop[size];
    else
        G += nn_G_hairpin_loop[NN_N_HAIRPINLOOP - 1]
            + (int) (NN_LXC37 * log((double) size / (NN_N_HAIRPINLOOP - 1)));
    /* mismatch penalty for the mismatch interior to the closing basepair of
       the hairpin. triloops get non-parameterised mismatch penalty */
    if (size == 3) {
        G += G_non_gc_penalty_pseq(p, i1, j1);
    } else {
        for (i = 0; i < NA_NBASEPAIRS; i++) {
            for (j = 0; j < NA_NBASES; j++) {
                for (k = 0; k < NA_NBASES; k++) {
                    bt_i1 = NA_BASEPAIR_TO_BASES[i][0];
                    bt_j1 = NA_BASEPAIR_TO_BASES[i][1];
                    G += p[i1][bt_i1] * p[j1][bt_j1] * p[b1][j] * p[b2][k]
                        * nn_G_mismatch_hairpin[i][j][k];
                }
            }
        }
    }
    if (size == 4) {
        /* tetraloop bonus */
        for (i = 0; i < NN_NTETRALOOP; i++) {
            p_loop = 1;
            for (j = 0; j < 4; j++)
                p_loop *= p[i1 + j + 1][nn_tetraloop_loop[i][j]];
            bt_i1 = NA_BASEPAIR_TO_BASES[nn_tetraloop_bp[i]][0];
            bt_j1 = NA_BASEPAIR_TO_BASES[nn_tetraloop_bp[i]][1];
            G += p[i1][bt_i1] * p[j1][bt_j1] * p_loop * nn_G_tetraloop[i];
        }
    }
    return G;
}

void
dGdp_hairpin_pseq(double **p, uint size, uint i1, uint j1, double **dGdp)
{
    uint i, j, k, b1, b2, bt_i1, bt_j1;
    double p_loop, p_loop_no_j;
    b1 = i1 + 1;
    b2 = j1 - 1;
    /* mismatch penalty for the mismatch interior to the closing basepair of
       the hairpin. triloops get non-parameterised mismatch penalty */
    if (size == 3) {
        dGdp_non_gc_penalty_pseq(p, i1, j1, dGdp);
    } else {
        for (i = 0; i < NA_NBASEPAIRS; i++) {
            for (j = 0; j < NA_NBASES; j++) {
                for (k = 0; k < NA_NBASES; k++) {
                    bt_i1 = NA_BASEPAIR_TO_BASES[i][0];
                    bt_j1 = NA_BASEPAIR_TO_BASES[i][1];
                    dGdp[i1][bt_i1] += p[j1][bt_j1] * p[b1][j] * p[b2][k]
                        * nn_G_mismatch_hairpin[i][j][k];
                    dGdp[j1][bt_j1] += p[i1][bt_i1] * p[b1][j] * p[b2][k]
                        * nn_G_mismatch_hairpin[i][j][k];
                    dGdp[b1][j]     += p[i1][bt_i1] * p[j1][bt_j1] * p[b2][k]
                        * nn_G_mismatch_hairpin[i][j][k];
                    dGdp[b2][k]     += p[i1][bt_i1] * p[j1][bt_j1] * p[b1][j]
                        * nn_G_mismatch_hairpin[i][j][k];
                }
            }
        }
    }
    if (size == 4) {
        /* tetraloop bonus */
        for (i = 0; i < NN_NTETRALOOP; i++) {
            bt_i1 = NA_BASEPAIR_TO_BASES[nn_tetraloop_bp[i]][0];
            bt_j1 = NA_BASEPAIR_TO_BASES[nn_tetraloop_bp[i]][1];
            p_loop = 1;
            for (j = 0; j < 4; j++)
                p_loop *= p[i1 + j + 1][nn_tetraloop_loop[i][j]];

            dGdp[i1][bt_i1] += p[j1][bt_j1] * p_loop * nn_G_tetraloop[i];
            dGdp[j1][bt_j1] += p[i1][bt_i1] * p_loop * nn_G_tetraloop[i];
            /* derivatives for the positions in the loop */
            for (j = 0; j < 4; j++) {
                /* p_loop_no_j is the same as p_loop but leaving out base j */
                p_loop_no_j = 1;
                for (k = 0; k < 4; k++)
                    if (k != j)
                        p_loop_no_j *= p[i1 + k + 1][nn_tetraloop_loop[i][k]];
                dGdp[i1 + j + 1][nn_tetraloop_loop[i][j]] +=
                    p[i1][bt_i1] * p[j1][bt_j1] * p_loop_no_j
                    * nn_G_tetraloop[i];
            }
        }
    }
}

int
G_extloop_multiloop(const uint *seq, uint unpaired,
                    uint nstems, uint (*stems)[2],
                    uint ndangle5, uint (*dangle5)[3],
                    uint ndangle3, uint (*dangle3)[3],
                    bool is_multiloop)
{
    uint i;
    int G = 0;
    /* penalty for non gc basepair initiating a stem */
    for (i = 0; i < nstems; i++)
        G += G_non_gc_penalty(idx_to_pairtype(seq, stems[i][0], stems[i][1]));
    /* 5' dangle */
    for (i = 0; i < ndangle5; i++)
        G += G_dangle5(seq, dangle5[i][0], dangle5[i][1], dangle5[i][2]);
    /* 3' dangle */
    for (i = 0; i < ndangle3; i++)
        G += G_dangle3(seq, dangle3[i][0], dangle3[i][1], dangle3[i][2]);
    /* linear multiloop energy */
    if (is_multiloop)
        G += NN_ML_OFFSET + unpaired * NN_ML_UNPAIRED + nstems * NN_ML_STEMS;
    return G;
}

double
G_extloop_multiloop_pseq(double **p, uint unpaired,
                         uint nstems, uint (*stems)[2],
                         uint ndangle5, uint (*dangle5)[3],
                         uint ndangle3, uint (*dangle3)[3],
                         bool is_multiloop)
{
    uint i;
    double G = 0;
    /* penalty for non gc basepair initiating a stem */
    for (i = 0; i < nstems; i++)
        G += G_non_gc_penalty_pseq(p, stems[i][0], stems[i][1]);
    /* 5' dangle */
    for (i = 0; i < ndangle5; i++)
        G += G_dangle5_pseq(p, dangle5[i][0], dangle5[i][1], dangle5[i][2]);
    /* 3' dangle */
    for (i = 0; i < ndangle3; i++)
        G += G_dangle3_pseq(p, dangle3[i][0], dangle3[i][1], dangle3[i][2]);
    /* linear multiloop energy */
    if (is_multiloop)
        G += NN_ML_OFFSET + unpaired * NN_ML_UNPAIRED + nstems * NN_ML_STEMS;
    return G;
}

void
dGdp_extloop_multiloop_pseq(double **p,
                            uint nstems, uint (*stems)[2],
                            uint ndangle5, uint (*dangle5)[3],
                            uint ndangle3, uint (*dangle3)[3],
                            double **dGdp)
{
    uint i;
    /* penalty for non gc basepair initiating a stem */
    for (i = 0; i < nstems; i++)
        dGdp_non_gc_penalty_pseq(p, stems[i][0], stems[i][1], dGdp);
    /* 5' dangle */
    for (i = 0; i < ndangle5; i++)
        dGdp_dangle5_pseq(p, dangle5[i][0], dangle5[i][1], dangle5[i][2], dGdp);
    /* 3' dangle */
    for (i = 0; i < ndangle3; i++)
        dGdp_dangle3_pseq(p, dangle3[i][0], dangle3[i][1], dangle3[i][2], dGdp);
}
