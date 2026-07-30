/*
 * test_negative_cases.c -- Negative Test Cases for Sovereign Stack Machine Verifier
 *
 * FORGE Phase 1, Step 3: Sanitizer Testing
 *
 * Tests overflow detection, boundary conditions, and error handling.
 * These tests verify that error conditions are caught correctly and don't cause
 * crashes, hangs, or silent failures.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>
#include "src/verifier/sov_verifier.h"

#define TEST_PASS(name) printf("[PASS] %s\n", name)
#define TEST_FAIL(name, reason) printf("[FAIL] %s: %s\n", name, reason)
#define ASSERT(cond, name, reason) do { if (!(cond)) { TEST_FAIL(name, reason); return 1; } } while(0)
#define ASSERT_RESULT(result, expected, name) do { \
    if ((result) != (expected)) { \
        printf("[FAIL] %s: got %s, expected %s\n", (name), \
            sov_verify_result_to_string(result), \
            sov_verify_result_to_string(expected)); \
        return 1; \
    } \
} while(0)

/*
 * Test 1: safe_alloc overflow on dimension multiply
 */
static int test_safe_alloc_overflow_dimension_multiply(void)
{
    size_t max_safe = SIZE_MAX / sizeof(int64_t);
    size_t n = (max_safe / 2) + 1;

    SafeMatrix *m = safe_alloc_matrix(n, n);

    if (m != NULL) {
        safe_free_matrix(m);
        TEST_FAIL("test_safe_alloc_overflow_dimension_multiply",
                  "Expected NULL allocation for overflow");
        return 1;
    }

    TEST_PASS("test_safe_alloc_overflow_dimension_multiply");
    return 0;
}

/*
 * Test 2: safe_alloc overflow on size multiply
 */
static int test_safe_alloc_overflow_size_multiply(void)
{
    size_t max_safe = SIZE_MAX / sizeof(int64_t);
    size_t n = max_safe - 100;
    size_t m = 2;

    SafeMatrix *sm = safe_alloc_matrix(n, m);

    if (sm != NULL) {
        safe_free_matrix(sm);
        TEST_FAIL("test_safe_alloc_overflow_size_multiply",
                  "Expected NULL allocation for size overflow");
        return 1;
    }

    TEST_PASS("test_safe_alloc_overflow_size_multiply");
    return 0;
}

/*
 * Test 3: zero matrix rows
 */
static int test_safe_alloc_zero_matrix_rows(void)
{
    SafeMatrix *m = safe_alloc_matrix(0, 100);

    ASSERT(m != NULL, "test_safe_alloc_zero_matrix_rows", "SafeMatrix struct should be allocated");
    ASSERT(m->rows == 0, "test_safe_alloc_zero_matrix_rows", "rows should be 0");
    ASSERT(m->cols == 100, "test_safe_alloc_zero_matrix_rows", "cols should be 100");
    ASSERT(m->capacity == 0, "test_safe_alloc_zero_matrix_rows", "capacity should be 0");
    ASSERT(m->data == NULL, "test_safe_alloc_zero_matrix_rows", "data should be NULL");

    safe_free_matrix(m);
    TEST_PASS("test_safe_alloc_zero_matrix_rows");
    return 0;
}

/*
 * Test 4: zero matrix cols
 */
static int test_safe_alloc_zero_matrix_cols(void)
{
    SafeMatrix *m = safe_alloc_matrix(100, 0);

    ASSERT(m != NULL, "test_safe_alloc_zero_matrix_cols", "SafeMatrix struct should be allocated");
    ASSERT(m->rows == 100, "test_safe_alloc_zero_matrix_cols", "rows should be 100");
    ASSERT(m->cols == 0, "test_safe_alloc_zero_matrix_cols", "cols should be 0");
    ASSERT(m->capacity == 0, "test_safe_alloc_zero_matrix_cols", "capacity should be 0");
    ASSERT(m->data == NULL, "test_safe_alloc_zero_matrix_cols", "data should be NULL");

    safe_free_matrix(m);
    TEST_PASS("test_safe_alloc_zero_matrix_cols");
    return 0;
}

/*
 * Test 5: verify_inv with very large dimension
 */
static int test_verify_inv_negative_dimension(void)
{
    int64_t A[] = {1, 0, 0, 1};
    int64_t X[] = {1, 0, 0, 1};

    VerifyResult res = sov_verify_inv(A, 4, X, 4, (size_t)-1, NULL);

    ASSERT(res == VER_DIMS_EXCEEDED || res == VER_OVERFLOW || res == VER_DIMENSION_MISMATCH,
           "test_verify_inv_negative_dimension",
           "Expected dimension error");

    TEST_PASS("test_verify_inv_negative_dimension");
    return 0;
}

/*
 * Test 6: verify_sol with INT64_MAX boundary values
 */
static int test_verify_sol_int64_max_accumulation(void)
{
    int64_t A[] = {
        INT64_MAX / 2, INT64_MAX / 2,
        INT64_MAX / 2, INT64_MAX / 2
    };
    int64_t x[] = {2, 2};
    int64_t b[] = {INT64_MAX, INT64_MAX};

    VerifyResult res = sov_verify_sol(A, 4, x, 2, b, 2, 2, 2, NULL);

    ASSERT(res == VER_OVERFLOW || res == VER_FAIL,
           "test_verify_sol_int64_max_accumulation",
           "Expected overflow or verification failure");

    TEST_PASS("test_verify_sol_int64_max_accumulation");
    return 0;
}

/*
 * Test 7: verify_lstsq with residual overflow
 */
static int test_verify_lstsq_residual_overflow(void)
{
    int64_t A[] = {
        INT64_MAX / 3, INT64_MAX / 3,
        INT64_MAX / 3, INT64_MAX / 3,
        INT64_MAX / 3, INT64_MAX / 3
    };
    int64_t x[] = {3, 3};
    int64_t b[] = {0, 0, 0};

    VerifyResult res = sov_verify_lstsq(A, 6, x, 2, b, 3, 3, 2, NULL);

    ASSERT(res == VER_OVERFLOW || res == VER_FAIL,
           "test_verify_lstsq_residual_overflow",
           "Expected overflow or verification failure");

    TEST_PASS("test_verify_lstsq_residual_overflow");
    return 0;
}

/*
 * Test 8: resource_check operation overflow
 */
static int test_resource_check_operation_overflow(void)
{
    SovResourceBudget budget;
    VerifyResult res = sov_init_resource_budget(&budget);
    ASSERT(res == VER_OK, "test_resource_check_operation_overflow", "Budget init failed");

    budget.max_operations = 100;
    budget.operation_count = 50;

    res = sov_add_operation_cost(&budget, 100);

    ASSERT_RESULT(res, VER_OPS_EXCEEDED, "test_resource_check_operation_overflow");

    TEST_PASS("test_resource_check_operation_overflow");
    return 0;
}

/*
 * Main test runner for negative tests
 */
int run_negative_tests(void)
{
    int failed = 0;

    printf("\n");
    printf("========================================\n");
    printf("=== NEGATIVE TEST SUITE (Phase 1, Step 3)\n");
    printf("========================================\n");
    printf("\n");

    printf("[TEST GROUP] Overflow Detection\n");
    failed += test_safe_alloc_overflow_dimension_multiply();
    failed += test_safe_alloc_overflow_size_multiply();
    printf("\n");

    printf("[TEST GROUP] Zero Dimensions\n");
    failed += test_safe_alloc_zero_matrix_rows();
    failed += test_safe_alloc_zero_matrix_cols();
    printf("\n");

    printf("[TEST GROUP] Invalid Inputs\n");
    failed += test_verify_inv_negative_dimension();
    printf("\n");

    printf("[TEST GROUP] Boundary Values & Accumulation\n");
    failed += test_verify_sol_int64_max_accumulation();
    failed += test_verify_lstsq_residual_overflow();
    printf("\n");

    printf("[TEST GROUP] Resource Limits\n");
    failed += test_resource_check_operation_overflow();
    printf("\n");

    if (failed == 0) {
        printf("=== ALL NEGATIVE TESTS PASSED (0/8 failures) ===\n");
    } else {
        printf("=== %d NEGATIVE TESTS FAILED ===\n", failed);
    }

    return failed;
}
