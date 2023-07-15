#include "ctools/random.h"
#include "opt.h"
#include "helpers-for-main-opt.h"

#include "unity.h"
#include "empty-setUp-tearDown.h"
#include <string.h>

void test_run_md(void) {
    ulong seed = 42;
    random_seed(seed);
    char *vienna = "(((...)))";
    int n = strlen(vienna);
    char *seq_constraints_hard = NULL;
    char designed_seq[] = {5, 5, 5, 5, 5, 5, 5, 5, 5, 5};
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

    char *expected_designed_seq = "GGGAAACCC";
    int expected_status = 0;
    TEST_ASSERT_EQUAL_CHAR_ARRAY(expected_designed_seq, designed_seq, n + 1);
    TEST_ASSERT_EQUAL_INT(expected_status, status);
}

int
main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_run_md);
    return UNITY_END();
}
