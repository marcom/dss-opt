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
void helper_run_md(ulong seed, const char *vienna,
                   const char *seq_constraints_hard,
                   uint expected_status,
                   const char *expected_designed_seq)
{
    random_seed(seed);
    uint n = strlen(vienna);
    char *designed_seq = calloc(n + 1, sizeof(*designed_seq));
    for (uint i = 0; i < n + 1; i++)
        designed_seq[i] = 5;

    double timestep = 0.0015, T_start = 40.0,
        time_total = 50.0, time_print = 2.5, time_cool = 0.1 * time_total,
        time_pur = 0.8 * time_total;

    uint nsteps = round(time_total / timestep);
    uint nprint = round(time_print / timestep);
    uint ncool  = round(time_cool  / timestep);
    uint npur   = round(time_pur   / timestep);
    double kpi, kpa, kneg, kpur_end, khet;
    uint het_window;
    set_dss_force_constants_defaults(&kpi, &kpa, &kneg, &kpur_end, &khet, &het_window);
    bool do_exp_cool = false;
    bool do_movie_output = false;
    bool verbose = false;

    int status = run_md(vienna, seq_constraints_hard,
                        nsteps, nprint, ncool, npur,
                        timestep, T_start, kpi, kpa, kneg,
                        khet, het_window, kpur_end,
                        do_exp_cool, do_movie_output, verbose,
                        designed_seq);
    printf("designed_seq = %s\n", designed_seq);
    TEST_ASSERT_EQUAL_CHAR_ARRAY(expected_designed_seq, designed_seq, n + 1);
    TEST_ASSERT_EQUAL_INT(expected_status, status);
    if (seq_constraints_hard != NULL) {
        uint *pairs = calloc(n, sizeof(*pairs));
        xvienna_to_pairs(n, vienna, pairs);
        TEST_ASSERT_TRUE(iscompatible_seq_with_constraints(n, designed_seq, seq_constraints_hard, pairs));
        free(pairs);
    }
    free(designed_seq);
}

void test_run_md(void)
{
    helper_run_md(42,
                  "(((...)))", NULL, 0,
                  "GGGAAACCC");
    helper_run_md(12,
                  "((((((((...))).))).(((....)))))", NULL, 0,
                  "GCGCGGCCAAAGGCACGCAGGGGAAACCCGC");
    helper_run_md(42,
                  "((((((((...))).))).(((....)))))", NULL, 0,
                  "GCGGGCUGAAACAGACCCAGCCGAAAGGCGC");
    helper_run_md(42,
                  "((((((((...))).))).(((....)))))",
                  "NNCAANGCNUCGNNNNNGNNNNNNGGNNNNN", 0,
                  "ACCAAGGCAUCGCCAUUGAGGCCAGGGCCGU");
}

int
main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_run_md   );
    return UNITY_END();
}
