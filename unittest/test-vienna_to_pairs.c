#include "na.h"

#include "unity.h"
#include "empty-setUp-tearDown.h"
#include <string.h>

#define UP NA_UNPAIRED

void test_xvienna_to_pairs(void) {
    char *vienna = "(((...)))";
    uint pairs[] = {-1, -1, -1, -1, -1, -1, -1, -1, -1};
    uint expected_pairs[] = {8, 7, 6, UP, UP, UP, 2, 1, 0};
    uint n = strlen(vienna);
    xvienna_to_pairs(n, vienna, pairs);
    TEST_ASSERT_EQUAL_UINT_ARRAY(expected_pairs, pairs, n);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_xvienna_to_pairs);
    return UNITY_END();
}
