#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "fold.h"
#include "part_func.h"
#include "fold_vars.h"
#include "utils.h"

static double
get_pij(unsigned i, unsigned j)
{
    unsigned tmp;
    if (i == j)
        return 0;
    if (i > j) {
        tmp = i;
        i = j;
        j = tmp;
    }
    return pr[iindx[i] - j];
}

static unsigned
get_sij(unsigned i, unsigned j, short *pt)
{
    if ((unsigned) pt[i] == j)
        return 1;
    else
        return 0;
}

static unsigned
get_pair_partner(unsigned i, short *pt)
{
    return pt[i];
}

static void
set_pr_from_pair_table(short *pt)
{
    unsigned i, j, len = pt[0];
    for (i = 1; i <= len; i++)
        for (j = i + 1; j <= len; j++)
            pr[iindx[i] - j] = 0;
    for (i = 1; i <= len; i++) {
        j = pt[i];
        if (i < j)
            pr[iindx[i] - j] = 1;
    }
}

static void
print_pr(unsigned len)
{
    unsigned i, j;
    printf("pair probabilities\n");
    for (i = 1; i <= len; i++) {
        for (j = 1; j <= len; j++)
            printf(" %5f", get_pij(i, j));
        printf("\n");
    }
}

static void
print_pt(short *pt)
{
    unsigned i, len = pt[0];
    printf("pair table");
    for (i = 0; i < len + 2; i++)
        printf(" %d ", pt[i]);
    printf("\n");
}

#if 0
/* TODO: doesn't work, i think (never really used) (2012.01.11) */
/* this is supposed to calculate the average number of incorrect
   nucleotides. See:
   Dirks, Lin, Winfree, Pierce. Paradigms for
   computational nucleic acid design (2004). Nucleic Acids Research,
   4:1392--1403. */
static double
calc_dist2(short *pt)
{
    unsigned i, j, len = pt[0];
    double dist = 0, prow;
    for (i = 1; i <= len; i++) {
        prow = 0;
        for (j = 1; j <= len; j++) {
            dist += get_pij(i, j) * get_sij(i, j, pt);
            prow += get_pij(i, j);
        }
        dist += (1 - prow) * get_sij(i, 0, pt);
    }
    dist = len - dist;
    return dist;
}
#endif

/* Calculates the average number of incorrectly paired nucleotides.
   Ref: Dirks, Lin, Winfree, Pierce. Paradigms for computational
   nucleic acid design (2004). Nucleic Acids Research,
   4:1392--1403. */
static double
calc_dist_ensemble_defect(short *pt)
{
    int i, j, len = pt[0];
    double p_paired, p_unpaired;
    double dist = 0;

    for (i = 1; i <= len; i++) {
        /* get prob of being paired / unpaired */
        p_paired = 0.0;
        for (j = 1; j <= len; j++)
            p_paired += get_pij(i, j);
        p_unpaired = 1.0 - p_paired;

        if (pt[i] == 0) {
            /* i is unpaired in target structure */
            dist += p_paired;
        } else {
            /* i is paired to pt[i] in target structure */
            dist += p_unpaired;
            /* this for loop only adds pij if (i,j) are unpaired in
               the target structure */
            for (j = 1; j <= len; j++)
                dist += get_pij(i, j) * (1 - get_sij(i, j, pt));
        }
    }
    return dist;
}


/* this calculation is supposed to follow the way in vienna-rna
   see centroid() function in lib/part_func.c */
static double
calc_dist_ensemble_bpdist(short *pt)
{
    const int MIN_HAIRPIN_LOOP = 3;
    int i, j, len = pt[0];
    double dist = 0, p;

    /* loop over all possible base pairs
       - for base pairs that are in our target structure, we have
         to add the probability that the base pair doesn't occurr
       - for base pairs that are not in our target structure, we
         add the probability for that base pair    */
    for (i = 1; i <= len; i++) {
        for (j = i + 1 + MIN_HAIRPIN_LOOP; j <= len; j++) {
            p = get_pij(i, j);
            if (get_sij(i, j, pt) == 1)
                dist += 1 - p;
            else
                dist += p;
        }
    }
    return dist;
}

int main(int argc, char **argv)
{
    unsigned len_seq;
    char *seq, *seq_mfe_struct, *seq_ens_struct, *target, *cen_struct;
    float ens_en, mfe_en, target_en, RT;
    double cen_dist, target_dist, my_cen_dist, target_ensemble_defect;
    short *target_pt, *cen_pt;

    dangles = 2;

    if (argc < 2) {
        printf("usage: %s sequence [target_structure]\n", argv[0]);
        return EXIT_FAILURE;
    }
    seq = argv[1];
    target = argc == 3 ? argv[2] : NULL;
    len_seq = strlen(seq);
    if (target) {
        if (len_seq != strlen(target)) {
            printf("sequence and target structure must have the same length "
                   "(%u != %u)\n", len_seq, (unsigned) strlen(target));
            return EXIT_FAILURE;
        }
    }

    /* allocate memory for fold(), could be skipped */
    initialize_fold(len_seq);

    /* allocate memory for structure and fold */
    seq_mfe_struct = space(sizeof(*seq_mfe_struct) * (len_seq + 1));
    mfe_en = fold(seq, seq_mfe_struct);
    if (! target)
        target = seq_mfe_struct;
    /* free arrays used in fold() */
    free_arrays();

    /* for longer sequences one should also set a scaling factor for
       partition function folding, e.g: */
    RT = (temperature + 273.15) * 1.98717 / 1000.0;  /* RT in kcal/mol */
    pf_scale = exp(-mfe_en / RT / len_seq);
    init_pf_fold(len_seq);

    /* calculate partition function and base pair probabilities */
    seq_ens_struct = space(sizeof(*seq_ens_struct) * (len_seq + 1));
    ens_en = pf_fold(seq, seq_ens_struct);

    /* calculate energy of target */
    target_en = energy_of_struct(seq, target);


    target_pt = make_pair_table(target);
    target_dist = calc_dist_ensemble_bpdist(target_pt);
    target_ensemble_defect = calc_dist_ensemble_defect(target_pt);

    cen_struct = centroid(len_seq, &cen_dist);
    cen_pt = make_pair_table(cen_struct);
    my_cen_dist = calc_dist_ensemble_bpdist(cen_pt);

    printf("dangles           = %d\n", dangles);
    printf("temperature       = %f [Celsius]\n", temperature);
    printf("RT                = %f [kcal/mol]\n", RT);
    printf("\n");
    printf("seq               = %s\n", seq);
    printf("target            = %s\n", target);
    printf("mfe_struct        = %s\n", seq_mfe_struct);
    printf("ens_struct        = %s\n", seq_ens_struct);
    printf("cen_struct        = %s\n", cen_struct);
    printf("cen_dist          = %f\n", cen_dist);
    printf("my_cen_dist       = %f\n", my_cen_dist);
    printf("mfe_p             = %f\n", exp((ens_en - mfe_en) / RT));
    printf("target_p          = %f\n", exp((ens_en - target_en) / RT));
    printf("target_dist       = %f\n", target_dist);
    printf("target_ens_defect = %f\n", target_ensemble_defect);
    printf("-RTlogZ (ens_en)  = %f\n", ens_en);
    printf("mfe_en            = %.2f\n", mfe_en);
    printf("target_en         = %.2f\n", target_en);
    printf("d_bp(mfe,target)  = %d\n", bp_distance(target, seq_mfe_struct));


    free_pf_arrays();  /* free space allocated for pf_fold() */
    free(seq_mfe_struct);
    free(seq_ens_struct);
    free(cen_struct);
    free(target_pt);
    free(cen_pt);
    return EXIT_SUCCESS;
}
