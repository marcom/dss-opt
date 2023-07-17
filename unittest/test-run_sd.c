#include "ctools/random.h"
#include "opt.h"
#include "helpers-for-main-opt.h"

#include "unity.h"
#include "empty-setUp-tearDown.h"
#include <string.h>
#include <stdio.h>


// TODO: move this function to dss-opt lib
// TODO: move function to check if two chars are legal basepair to dss-opt lib, it's hardcoded here
int iscompatible_seq_with_constraints(uint n, const char *seq,
                                      const char *constraints,
                                      const uint *pairs)
{
    bool isok = true;
    for (uint i = 0; i < n; i++) {
        if (constraints[i] != 'N') {
            uint j = pairs[i];
            if (j == NA_UNPAIRED) {
                if (seq[i] != constraints[i]) {
                    return false;
                }
            } else {
                char ci = seq[i];
                char cj = seq[j];
                // TODO: is_legal_basepair(ci, cj)
                if (! ((ci == 'A' && cj == 'U') ||
                       (ci == 'U' && cj == 'A') ||
                       (ci == 'G' && cj == 'C') ||
                       (ci == 'C' && cj == 'G') ||
                       (ci == 'G' && cj == 'U') ||
                       (ci == 'U' && cj == 'G'))
                    ) {
                    return false;
                }
            }
        }
    }
    return isok;
}

// TODO: move this convenience function to dss-opt lib
void helper_run_sd(ulong seed, const char *vienna,
                   uint expected_status,
                   const char *expected_designed_seq)
{
    random_seed(seed);
    uint n = strlen(vienna);
    char *designed_seq = calloc(n + 1, sizeof(*designed_seq));
    for (uint i = 0; i < n + 1; i++)
        designed_seq[i] = 5;

    // TODO: duplicated from main-opt-sd.c
    uint maxsteps = 20000, nprint = 1000;
    double wiggle = 0.1;
    double kpi, kpa, kneg, kpur, khet;
    uint het_window;
    set_dss_force_constants_defaults(&kpi, &kpa, &kneg, &kpur, &khet, &het_window);
    bool do_movie_output = false;
    bool verbose = false;

    int status = run_sd(vienna, maxsteps, nprint, wiggle,
                        kpi, kpa, kpur, kneg, khet, het_window,
                        do_movie_output, verbose, designed_seq);
    printf("designed_seq = %s\n", designed_seq);
    TEST_ASSERT_EQUAL_CHAR_ARRAY(expected_designed_seq, designed_seq, n + 1);
    TEST_ASSERT_EQUAL_INT(expected_status, status);
    free(designed_seq);
}

void test_run_sd(void)
{
    helper_run_sd(42,
                  "(((...)))", 0,
                  "GGCAAAGCC");
    helper_run_sd(11,
                  "(((...)))", 0,
                  "CGCAAAGCG");
    helper_run_sd(12,
                  "((((((((...))).))).(((....)))))", 0,
                  "GCGCUAGCAAAGCUAAGCAGUCGAAAGACGC");
    helper_run_sd(42,
                  "((((((((...))).))).(((....)))))", 0,
                  "GCGCCGGCAAAGCCAGGCAGGCGAAAGCCGC");
}

int
main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_run_sd);
    return UNITY_END();
}
