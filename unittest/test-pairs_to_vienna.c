#include "na.h"

#include "unity.h"
#include "empty-setUp-tearDown.h"
#include <string.h>

#define UP NA_UNPAIRED

void test_xpairs_to_vienna(void) {
    uint pairs[] = {8, 7, 6, UP, UP, UP, 2, 1, 0};
    uint n = sizeof(pairs) / sizeof(*pairs);
    char *expected_vienna = "(((...)))";
    char vienna[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    char *out_vienna = xpairs_to_vienna(n, pairs, vienna);
    TEST_ASSERT_EQUAL_CHAR_ARRAY(expected_vienna, vienna, n);
    TEST_ASSERT_EQUAL_CHAR_ARRAY(expected_vienna, out_vienna, n);
    TEST_ASSERT_EQUAL_PTR(vienna, out_vienna);
}

void test_pairs_to_vienna(void) {
    bool verbose = true;
    uint pairs[] = {8, 7, 6, UP, UP, UP, 2, 1, 0};
    uint n = sizeof(pairs) / sizeof(*pairs);
    char *expected_vienna = "(((...)))";
    char vienna[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    int retcode = pairs_to_vienna(n, pairs, verbose, vienna);
    TEST_ASSERT_EQUAL_CHAR_ARRAY(expected_vienna, vienna, n);
    TEST_ASSERT_EQUAL_INT(EXIT_SUCCESS, retcode);
}

void test_pairs_to_vienna_fails_self_pairentry(void) {
    bool verbose = true;
    uint pairs[] = {8, 7, 2, UP, UP, UP, 2, 1, 0};
    uint n = sizeof(pairs) / sizeof(*pairs);
    char vienna[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    int retcode = pairs_to_vienna(n, pairs, verbose, vienna);
    TEST_ASSERT_EQUAL_INT(EXIT_FAILURE, retcode);
}

void test_pairs_to_vienna_fails_two_positions_paired_to_same(void) {
    bool verbose = true;
    uint pairs[] = {8, 7, 7, UP, UP, UP, UP, 1, 0};
    uint n = sizeof(pairs) / sizeof(*pairs);
    char vienna[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    int retcode = pairs_to_vienna(n, pairs, verbose, vienna);
    TEST_ASSERT_EQUAL_INT(EXIT_FAILURE, retcode);
}

void test_pairs_to_vienna_fails_paired_to_unpaired(void) {
    bool verbose = true;
    uint pairs[] = {8, 7, 6, UP, UP, UP, UP, 1, 0};
    uint n = sizeof(pairs) / sizeof(*pairs);
    char vienna[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    int retcode = pairs_to_vienna(n, pairs, verbose, vienna);
    TEST_ASSERT_EQUAL_INT(EXIT_FAILURE, retcode);
}

int
main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_xpairs_to_vienna);
    RUN_TEST(test_pairs_to_vienna);
    RUN_TEST(test_pairs_to_vienna_fails_self_pairentry);
    RUN_TEST(test_pairs_to_vienna_fails_two_positions_paired_to_same);
    RUN_TEST(test_pairs_to_vienna_fails_paired_to_unpaired);
    return UNITY_END();
}
