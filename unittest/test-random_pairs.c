#include "ctools/random.h"
#include "na.h"

#include "unity.h"
#include "empty-setUp-tearDown.h"
#include <string.h>

#define UP NA_UNPAIRED

void test_random_pairs(uint n, uint hpmin, uint seed, const uint *expected_pairs) {
    random_seed(seed);
    uint *pairs = calloc(n, sizeof(*pairs));
    random_pairs(n, pairs, hpmin);
    TEST_ASSERT_EQUAL_UINT_ARRAY(expected_pairs, pairs, n);
}

void all_test_random_pairs(void) {
    // only empty structures possible
    test_random_pairs(1, 0, 0, (uint[]){UP});
    test_random_pairs(1, 3, 0, (uint[]){UP});
    test_random_pairs(2, 3, 0, (uint[]){UP, UP});
    test_random_pairs(3, 3, 0, (uint[]){UP, UP, UP});
    test_random_pairs(4, 3, 0, (uint[]){UP, UP, UP, UP});
    // non-empty structures
    test_random_pairs(3, 0, 0, (uint[]){UP, 2, 1});
    test_random_pairs(9, 3, 0, (uint[]){UP, UP, UP, UP, 8, UP, UP, UP, 4});
    test_random_pairs(9, 3, 42, (uint[]){5, UP, UP, UP, UP, 0, UP, UP, UP});
}

void test_random_pairs_n0(void) {
    uint n = 0;
    uint hpmin = 0;
    uint *pairs = NULL;
    random_pairs(n, pairs, hpmin);
    TEST_ASSERT_EQUAL_PTR(pairs, NULL);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(all_test_random_pairs);
    RUN_TEST(test_random_pairs_n0);
    return UNITY_END();
}
