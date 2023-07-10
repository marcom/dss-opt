#include "opt.h"
#include "na.h"

#include "unity.h"
#include "empty-setUp-tearDown.h"
#include <string.h>

#define UP NA_UNPAIRED
#define N NA_BASE_N
#define A NA_BASE_A
#define C NA_BASE_C
#define G NA_BASE_G
#define U NA_BASE_U

size_t count_char(const char *str, char c) {
    size_t n = 0;
    for (; *str; str++)
        if (*str == c)
            n++;
    return n;
}

void test_xparse_seq_constraints_hard_allN(void) {
    char *constraint_str = "NNNNNNNNN";
    uint pairs[] = {8, 7, 6, UP, UP, UP, 2, 1, 0};
    uint hard[] = {-42, -42, -42, -42, -42, -42, -42, -42, -42};
    uint expected_hard[] = {N, N, N, N, N, N, N, N, N};
    uint n = strlen(constraint_str);
    uint expected_n_constraint = n - count_char(constraint_str, 'N');
    uint n_constraint = x_parse_seq_constraints_hard(n, hard, constraint_str, pairs);
    TEST_ASSERT_EQUAL_UINT_ARRAY(expected_hard, hard, n);
    TEST_ASSERT_EQUAL_INT(expected_n_constraint, n_constraint);
}

void test_xparse_seq_constraints_hard_unpaired(void) {
    char *constraint_str = "NNNANGNNN";
    //                      (((...)))
    uint pairs[] = {8, 7, 6, UP, UP, UP, 2, 1, 0};
    uint hard[] = {-42, -42, -42, -42, -42, -42, -42, -42, -42};
    uint expected_hard[] = {N, N, N, A, N, G, N, N, N};
    uint n = strlen(constraint_str);
    uint expected_n_constraint = n - count_char(constraint_str, 'N');
    uint n_constraint = x_parse_seq_constraints_hard(n, hard, constraint_str, pairs);
    TEST_ASSERT_EQUAL_UINT_ARRAY(expected_hard, hard, n);
    TEST_ASSERT_EQUAL_INT(expected_n_constraint, n_constraint);
}

void test_xparse_seq_constraints_hard_basepaired(void) {
    char *constraint_str = "NNGNNNCNN";
    //                      (((...)))
    uint pairs[] = {8, 7, 6, UP, UP, UP, 2, 1, 0};
    uint hard[] = {-42, -42, -42, -42, -42, -42, -42, -42, -42};
    uint expected_hard[] = {N, N, G, N, N, N, C, N, N};
    uint n = strlen(constraint_str);
    uint expected_n_constraint = n - count_char(constraint_str, 'N');
    uint n_constraint = x_parse_seq_constraints_hard(n, hard, constraint_str, pairs);
    TEST_ASSERT_EQUAL_UINT_ARRAY(expected_hard, hard, n);
    TEST_ASSERT_EQUAL_INT(expected_n_constraint, n_constraint);
}

void test_xparse_seq_constraints_hard_mixed(void) {
    char *constraint_str = "GANNUNCUN";
    //                      (((...)))
    uint pairs[] = {8, 7, 6, UP, UP, UP, 2, 1, 0};
    uint hard[] = {-42, -42, -42, -42, -42, -42, -42, -42, -42};
    uint expected_hard[] = {G, A, N, N, U, N, C, U, N};
    uint n = strlen(constraint_str);
    uint expected_n_constraint = n - count_char(constraint_str, 'N');
    uint n_constraint = x_parse_seq_constraints_hard(n, hard, constraint_str, pairs);
    TEST_ASSERT_EQUAL_UINT_ARRAY(expected_hard, hard, n);
    TEST_ASSERT_EQUAL_INT(expected_n_constraint, n_constraint);
}

void test_parse_seq_constraints_hard_allN(void) {
    bool verbose = true;
    //                      (((...)))
    char *constraint_str = "NNNNNNNNN";
    uint pairs[] = {8, 7, 6, UP, UP, UP, 2, 1, 0};
    uint hard[] = {-42, -42, -42, -42, -42, -42, -42, -42, -42};
    uint expected_hard[] = {N, N, N, N, N, N, N, N, N};
    uint n = strlen(constraint_str);
    uint expected_n_hard = n - count_char(constraint_str, 'N');
    uint n_hard = -42 * 5;
    uint retcode = parse_seq_constraints_hard(n, hard, &n_hard,
                                              constraint_str, verbose, pairs);
    TEST_ASSERT_EQUAL_UINT_ARRAY(expected_hard, hard, n);
    TEST_ASSERT_EQUAL_UINT(expected_n_hard, n_hard);
    TEST_ASSERT_EQUAL_INT(EXIT_SUCCESS, retcode);
}

void test_parse_seq_constraints_hard_unpaired(void) {
    bool verbose = true;
    //                      (((...)))
    char *constraint_str = "NNNANGNNN";
    uint pairs[] = {8, 7, 6, UP, UP, UP, 2, 1, 0};
    uint hard[] = {-42, -42, -42, -42, -42, -42, -42, -42, -42};
    uint expected_hard[] = {N, N, N, A, N, G, N, N, N};
    uint n = strlen(constraint_str);
    uint expected_n_hard = n - count_char(constraint_str, 'N');
    uint n_hard = -42 * 5;
    uint retcode = parse_seq_constraints_hard(n, hard, &n_hard,
                                              constraint_str, verbose, pairs);
    TEST_ASSERT_EQUAL_UINT_ARRAY(expected_hard, hard, n);
    TEST_ASSERT_EQUAL_UINT(expected_n_hard, n_hard);
    TEST_ASSERT_EQUAL_INT(EXIT_SUCCESS, retcode);
}

void test_parse_seq_constraints_hard_basepaired(void) {
    bool verbose = true;
    //                      (((...)))
    char *constraint_str = "NNGNNNCNN";
    uint pairs[] = {8, 7, 6, UP, UP, UP, 2, 1, 0};
    uint hard[] = {-42, -42, -42, -42, -42, -42, -42, -42, -42};
    uint expected_hard[] = {N, N, G, N, N, N, C, N, N};
    uint n = strlen(constraint_str);
    uint expected_n_hard = n - count_char(constraint_str, 'N');
    uint n_hard = -42 * 5;
    uint retcode = parse_seq_constraints_hard(n, hard, &n_hard,
                                              constraint_str, verbose, pairs);
    TEST_ASSERT_EQUAL_UINT_ARRAY(expected_hard, hard, n);
    TEST_ASSERT_EQUAL_UINT(expected_n_hard, n_hard);
    TEST_ASSERT_EQUAL_INT(EXIT_SUCCESS, retcode);
}

void test_parse_seq_constraints_hard_mixed(void) {
    bool verbose = true;
    char *constraint_str = "GANNUNCUN";
    //                      (((...)))
    uint pairs[] = {8, 7, 6, UP, UP, UP, 2, 1, 0};
    uint hard[] = {-42, -42, -42, -42, -42, -42, -42, -42, -42};
    uint expected_hard[] = {G, A, N, N, U, N, C, U, N};
    uint n = strlen(constraint_str);
    uint expected_n_hard = n - count_char(constraint_str, 'N');
    uint n_hard = -42 * 5;
    uint retcode = parse_seq_constraints_hard(n, hard, &n_hard,
                                              constraint_str, verbose, pairs);
    TEST_ASSERT_EQUAL_UINT_ARRAY(expected_hard, hard, n);
    TEST_ASSERT_EQUAL_UINT(expected_n_hard, n_hard);
    TEST_ASSERT_EQUAL_INT(EXIT_SUCCESS, retcode);
}

void test_parse_seq_constraints_hard_fail_illegal_char(void) {
    bool verbose = true;
    char *constraint_str = "NNNNXNNNN";
    //                      (((...)))
    uint pairs[] = {8, 7, 6, UP, UP, UP, 2, 1, 0};
    uint hard[] = {-42, -42, -42, -42, -42, -42, -42, -42, -42};
    uint n = strlen(constraint_str);
    uint n_hard = -42 * 5;
    uint retcode = parse_seq_constraints_hard(n, hard, &n_hard,
                                              constraint_str, verbose, pairs);
    TEST_ASSERT_EQUAL_INT(EXIT_FAILURE, retcode);
}

void test_parse_seq_constraints_hard_fail_impossible_basepair(void) {
    bool verbose = true;
    char *constraint_str = "NNGNNNGNN";
    //                      (((...)))
    uint pairs[] = {8, 7, 6, UP, UP, UP, 2, 1, 0};
    uint hard[] = {-42, -42, -42, -42, -42, -42, -42, -42, -42};
    uint n = strlen(constraint_str);
    uint n_hard = -42 * 5;
    uint retcode = parse_seq_constraints_hard(n, hard, &n_hard,
                                              constraint_str, verbose, pairs);
    TEST_ASSERT_EQUAL_INT(EXIT_FAILURE, retcode);
}


int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_xparse_seq_constraints_hard_allN);
    RUN_TEST(test_xparse_seq_constraints_hard_unpaired);
    RUN_TEST(test_xparse_seq_constraints_hard_basepaired);
    RUN_TEST(test_xparse_seq_constraints_hard_mixed);

    RUN_TEST(test_parse_seq_constraints_hard_allN);
    RUN_TEST(test_parse_seq_constraints_hard_unpaired);
    RUN_TEST(test_parse_seq_constraints_hard_basepaired);
    RUN_TEST(test_parse_seq_constraints_hard_mixed);
    RUN_TEST(test_parse_seq_constraints_hard_fail_illegal_char);
    RUN_TEST(test_parse_seq_constraints_hard_fail_impossible_basepair);

    return UNITY_END();
}
