#include "na.h"

#include "unity.h"
#include "empty-setUp-tearDown.h"
#include <string.h>

#define UP NA_UNPAIRED

void test_xpairs_to_vienna(void) {
    uint pairs[] = {8, 7, 6, UP, UP, UP, 2, 1, 0};
    uint n = sizeof(pairs) / sizeof(*pairs);
    char *expected_vienna = "(((...)))";
    char vienna[] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    xpairs_to_vienna(n, pairs, vienna);
    TEST_ASSERT_EQUAL_CHAR_ARRAY(expected_vienna, vienna, n);
}

int
main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_xpairs_to_vienna);
    return UNITY_END();
}
