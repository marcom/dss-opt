#include "ctools/random.h"
#include "na.h"

#include "unity.h"
#include "empty-setUp-tearDown.h"
#include <string.h>

#define UP NA_UNPAIRED

# define errexit(msg) do { printf(msg); exit(EXIT_FAILURE); } while(0)

void test_calc_interactions_useq(const char *seq, const char *dbn, int expected_energy) {
    bool verbose = true;
    uint n = strlen(seq);
    if (n != strlen(dbn)) {
        /* printf("ERROR: strlen(seq) != strlen(dbn)"); */
        /* exit(EXIT_FAILURE); */
        errexit("ERROR: strlen(seq) != strlen(dbn)");
    }
    uint *useq = calloc(n, sizeof(*useq));
    xstr_to_useq(n, seq, useq);
    struct nn_inter *inter = nn_inter_xnew(n);
    int ret = vienna_to_pairs(n, dbn, verbose, inter->pairs);
    if (ret != EXIT_SUCCESS) {
        errexit("vienna_to_pairs failed");
    }
    find_interactions(inter);
    int energy = calc_interactions_useq(inter, useq);

    free(useq);
    nn_inter_delete(inter);

    TEST_ASSERT_EQUAL_INT(expected_energy, energy);
}

void all_test_calc_interactions_useq(void) {
    // hairpins
    test_calc_interactions_useq("GAAAC",
                                "(...)", 570);
    test_calc_interactions_useq("GAAAAC",
                                "(....)", 450);
    test_calc_interactions_useq("GAAAAAAC",
                                "(......)", 430);
    // special hairpins (tetraloops)
    test_calc_interactions_useq("GGAAAC",
                                "(....)", 20);
    // stacks
    test_calc_interactions_useq("GGGAAACCC",
                                "(((...)))", -90);
    test_calc_interactions_useq("CGGAAACCG",
                                "(((...)))",   0);
    test_calc_interactions_useq("GGGGUUUUCCCC",
                                "((((....))))", -580);
    test_calc_interactions_useq("GGCGUUAUCGCC",
                                "((((....))))", -500);
    test_calc_interactions_useq("GACGUUAUCGUC",
                                "((((....))))", -290);
    // bulges
    test_calc_interactions_useq("GGGGAAAAAUCCC",
                                "(((.(....))))",   10);
    test_calc_interactions_useq("GGGGAAAACACCC",
                                "((((....).)))", -160);
    // internal loops
    test_calc_interactions_useq("GGAGGAAAACACCC",
                                "((.((....).)))",  220);
    test_calc_interactions_useq("GGAGGAAAACCACC",
                                "((.((....)).))", -170);
    // multiway loops
    test_calc_interactions_useq("GGAAGGGAAAACCCGGGAAACCCACC",
                                "((..(((....)))(((...))).))", -720);
    test_calc_interactions_useq("GGAAACGAAAACGUGGGAAACCCAUC",
                                "((..(((....)))(((...))).))",  -70);
    test_calc_interactions_useq("GGAAACGAAAACGUAGGGAAACCCAUC",
                                "((..(((....))).(((...))).))", -90);
    test_calc_interactions_useq("GGAGGAAAACCAACGAAAACGUAGUGAAACGCAUC",
                                "((.((....)).(((....))).(((...))).))", 150);
}

void all_test_calc_interactions_useq_empty(void) {
    // empty structures
    test_calc_interactions_useq("A",      ".",      0);
    test_calc_interactions_useq("CA",     "..",     0);
    test_calc_interactions_useq("CAA",    "...",    0);
    test_calc_interactions_useq("UGAC",   "....",   0);
    test_calc_interactions_useq("GGACC",  ".....",  0);
    test_calc_interactions_useq("AGGACC", "......", 0);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(all_test_calc_interactions_useq);
    RUN_TEST(all_test_calc_interactions_useq_empty);
    return UNITY_END();
}
