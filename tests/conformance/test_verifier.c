/*
 * test_verifier.c -- Conformance Tests for Sovereign Stack Machine Verifier
 *
 * FORGE Phase 2 Conformance
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

/* Forward declaration for negative tests (Phase 1, Step 3) */
extern int run_negative_tests(void);

static int test_verify_inv_identity_2x2(void)
{
    int64_t I[] = {1, 0, 0, 1};
    VerifyResult res = sov_verify_inv(I, 4, I, 4, 2, NULL);
    ASSERT(res == VER_OK, "test_verify_inv_identity_2x2", sov_verify_result_to_string(res));
    TEST_PASS("test_verify_inv_identity_2x2");
    return 0;
}

static int test_verify_inv_inverse_2x2(void)
{
    int64_t A[] = {1, 1, 0, 1};
    int64_t A_inv[] = {1, -1, 0, 1};
    VerifyResult res = sov_verify_inv(A, 4, A_inv, 4, 2, NULL);
    ASSERT(res == VER_OK, "test_verify_inv_inverse_2x2", sov_verify_result_to_string(res));
    TEST_PASS("test_verify_inv_inverse_2x2");
    return 0;
}

static int test_verify_inv_fail_not_inverse(void)
{
    int64_t A[] = {1, 2, 3, 4};
    int64_t B[] = {5, 6, 7, 8};
    VerifyResult res = sov_verify_inv(A, 4, B, 4, 2, NULL);
    ASSERT(res == VER_FAIL, "test_verify_inv_fail_not_inverse", sov_verify_result_to_string(res));
    TEST_PASS("test_verify_inv_fail_not_inverse");
    return 0;
}

static int test_verify_sol_2x2_system(void)
{
    int64_t A[] = {2, 0, 0, 2};
    int64_t x[] = {2, 3};
    int64_t b[] = {4, 6};
    VerifyResult res = sov_verify_sol(A, 4, x, 2, b, 2, 2, 2, NULL);
    ASSERT(res == VER_OK, "test_verify_sol_2x2_system", sov_verify_result_to_string(res));
    TEST_PASS("test_verify_sol_2x2_system");
    return 0;
}

static int test_verify_sol_overdetermined(void)
{
    int64_t A[] = {1, 0, 0, 1, 1, 1};
    int64_t x[] = {1, 1};
    int64_t b[] = {1, 1, 2};
    VerifyResult res = sov_verify_sol(A, 6, x, 2, b, 3, 3, 2, NULL);
    ASSERT(res == VER_OK, "test_verify_sol_overdetermined", sov_verify_result_to_string(res));
    TEST_PASS("test_verify_sol_overdetermined");
    return 0;
}

static int test_verify_sol_fail_wrong_solution(void)
{
    int64_t A[] = {2, 0, 0, 2};
    int64_t x[] = {1, 1};
    int64_t b[] = {4, 6};
    VerifyResult res = sov_verify_sol(A, 4, x, 2, b, 2, 2, 2, NULL);
    ASSERT(res == VER_FAIL, "test_verify_sol_fail_wrong_solution", sov_verify_result_to_string(res));
    TEST_PASS("test_verify_sol_fail_wrong_solution");
    return 0;
}

static int test_verify_lstsq_perfect_system(void)
{
    int64_t A[] = {1, 0, 0, 1};
    int64_t x[] = {1, 2};
    int64_t b[] = {1, 2};
    VerifyResult res = sov_verify_lstsq(A, 4, x, 2, b, 2, 2, 2, NULL);
    ASSERT(res == VER_OK, "test_verify_lstsq_perfect_system", sov_verify_result_to_string(res));
    TEST_PASS("test_verify_lstsq_perfect_system");
    return 0;
}

static int test_verify_lstsq_overdetermined_exact(void)
{
    int64_t A[] = {1, 0, 0, 1, 1, 1};
    int64_t x[] = {1, 1};
    int64_t b[] = {1, 1, 2};
    VerifyResult res = sov_verify_lstsq(A, 6, x, 2, b, 3, 3, 2, NULL);
    ASSERT(res == VER_OK, "test_verify_lstsq_overdetermined_exact", sov_verify_result_to_string(res));
    TEST_PASS("test_verify_lstsq_overdetermined_exact");
    return 0;
}

static int test_verify_lstsq_fail_not_solution(void)
{
    int64_t A[] = {1, 0, 0, 1};
    int64_t x[] = {2, 3};
    int64_t b[] = {1, 2};
    VerifyResult res = sov_verify_lstsq(A, 4, x, 2, b, 2, 2, 2, NULL);
    ASSERT(res == VER_FAIL, "test_verify_lstsq_fail_not_solution", sov_verify_result_to_string(res));
    TEST_PASS("test_verify_lstsq_fail_not_solution");
    return 0;
}

static int test_overflow_detection(void)
{
    int64_t A[] = {INT64_MAX, 0, 0, 1};
    int64_t x[] = {2, 1};
    int64_t result[2];
    VerifyResult res = sov_matrix_vec_mult(A, x, result, 2, 2);
    ASSERT(res == VER_OVERFLOW, "test_overflow_detection", sov_verify_result_to_string(res));
    TEST_PASS("test_overflow_detection");
    return 0;
}

static int test_null_input_handling(void)
{
    int64_t A[] = {1, 0, 0, 1};
    VerifyResult res = sov_verify_inv(NULL, 4, A, 4, 2, NULL);
    ASSERT(res == VER_NULL_INPUT, "test_null_input_handling", sov_verify_result_to_string(res));
    TEST_PASS("test_null_input_handling");
    return 0;
}

/* ============================================================================
 * MEMORY SAFETY TESTS (Phase 1, Step 1)
 * ============================================================================
 */

static int test_safe_alloc_matrix_valid(void)
{
    SafeMatrix *m = safe_alloc_matrix(5, 10);
    ASSERT(m != NULL, "test_safe_alloc_matrix_valid", "Allocation failed");
    ASSERT(m->rows == 5, "test_safe_alloc_matrix_valid", "Rows mismatch");
    ASSERT(m->cols == 10, "test_safe_alloc_matrix_valid", "Cols mismatch");
    ASSERT(m->capacity == 50, "test_safe_alloc_matrix_valid", "Capacity mismatch");
    ASSERT(m->data != NULL, "test_safe_alloc_matrix_valid", "Data pointer null");
    safe_free_matrix(m);
    TEST_PASS("test_safe_alloc_matrix_valid");
    return 0;
}

static int test_safe_alloc_matrix_zero_size(void)
{
    SafeMatrix *m = safe_alloc_matrix(0, 10);
    ASSERT(m != NULL, "test_safe_alloc_matrix_zero_size", "Allocation failed");
    ASSERT(m->capacity == 0, "test_safe_alloc_matrix_zero_size", "Capacity should be 0");
    safe_free_matrix(m);
    TEST_PASS("test_safe_alloc_matrix_zero_size");
    return 0;
}

static int test_safe_alloc_matrix_overflow_rows_cols(void)
{
    /* Test overflow in rows * cols */
    SafeMatrix *m = safe_alloc_matrix(SIZE_MAX / 2 + 1, 2);
    ASSERT(m == NULL, "test_safe_alloc_matrix_overflow_rows_cols", "Should reject overflow");
    TEST_PASS("test_safe_alloc_matrix_overflow_rows_cols");
    return 0;
}

static int test_safe_alloc_matrix_overflow_size(void)
{
    /* Test overflow in nelems * sizeof(int64_t) */
    /* SIZE_MAX / sizeof(int64_t) is the max elements that fit */
    size_t max_safe = SIZE_MAX / sizeof(int64_t);
    SafeMatrix *m = safe_alloc_matrix(max_safe + 1, 1);
    ASSERT(m == NULL, "test_safe_alloc_matrix_overflow_size", "Should reject size overflow");
    TEST_PASS("test_safe_alloc_matrix_overflow_size");
    return 0;
}

static int test_validate_matrix_buffer_valid(void)
{
    int64_t buf[20];
    VerifyResult res = validate_matrix_buffer(buf, 4, 5, 20);
    ASSERT(res == VER_OK, "test_validate_matrix_buffer_valid", sov_verify_result_to_string(res));
    TEST_PASS("test_validate_matrix_buffer_valid");
    return 0;
}

static int test_validate_matrix_buffer_null(void)
{
    VerifyResult res = validate_matrix_buffer(NULL, 4, 5, 20);
    ASSERT(res == VER_NULL_INPUT, "test_validate_matrix_buffer_null", sov_verify_result_to_string(res));
    TEST_PASS("test_validate_matrix_buffer_null");
    return 0;
}

static int test_validate_matrix_buffer_undersized(void)
{
    int64_t buf[19];
    /* Need 4*5=20 elements, only have 19 */
    VerifyResult res = validate_matrix_buffer(buf, 4, 5, 19);
    ASSERT(res == VER_BUFFER_OVERFLOW, "test_validate_matrix_buffer_undersized", sov_verify_result_to_string(res));
    TEST_PASS("test_validate_matrix_buffer_undersized");
    return 0;
}

static int test_validate_matrix_buffer_overflow_dims(void)
{
    int64_t buf[100];
    /* SIZE_MAX / 2 * 2 will overflow */
    VerifyResult res = validate_matrix_buffer(buf, SIZE_MAX / 2 + 1, 2, 100);
    ASSERT(res == VER_OVERFLOW, "test_validate_matrix_buffer_overflow_dims", sov_verify_result_to_string(res));
    TEST_PASS("test_validate_matrix_buffer_overflow_dims");
    return 0;
}

static int test_verify_inv_buffer_validation(void)
{
    int64_t I[] = {1, 0, 0, 1};
    /* Undersized buffer: claiming 3x3 but only have 4 elements */
    VerifyResult res = sov_verify_inv(I, 4, I, 4, 3, NULL);
    ASSERT(res == VER_BUFFER_OVERFLOW, "test_verify_inv_buffer_validation", sov_verify_result_to_string(res));
    TEST_PASS("test_verify_inv_buffer_validation");
    return 0;
}

static int test_verify_sol_buffer_validation(void)
{
    int64_t A[] = {2, 0, 0, 2};
    int64_t x[] = {2, 3};
    int64_t b[] = {4, 6};
    /* A buffer undersized: claiming 3x3 but only have 4 */
    VerifyResult res = sov_verify_sol(A, 4, x, 3, b, 2, 3, 3, NULL);
    ASSERT(res == VER_BUFFER_OVERFLOW, "test_verify_sol_buffer_validation", sov_verify_result_to_string(res));
    TEST_PASS("test_verify_sol_buffer_validation");
    return 0;
}

static int test_verify_lstsq_buffer_validation(void)
{
    int64_t A[] = {1, 0, 0, 1};
    int64_t x[] = {1, 1};
    int64_t b[] = {1, 2};
    /* x buffer undersized: claiming 3 but only have 2 */
    VerifyResult res = sov_verify_lstsq(A, 4, x, 2, b, 2, 2, 3, NULL);
    ASSERT(res == VER_BUFFER_OVERFLOW, "test_verify_lstsq_buffer_validation", sov_verify_result_to_string(res));
    TEST_PASS("test_verify_lstsq_buffer_validation");
    return 0;
}

static int test_safe_free_matrix_null(void)
{
    /* Should not crash */
    safe_free_matrix(NULL);
    TEST_PASS("test_safe_free_matrix_null");
    return 0;
}

/* ============================================================================
 * RESOURCE LIMITS TESTS (Phase 1, Step 2)
 * ============================================================================
 */

static int test_resource_init(void)
{
    SovResourceBudget budget;
    VerifyResult res = sov_init_resource_budget(&budget);
    ASSERT(res == VER_OK, "test_resource_init", sov_verify_result_to_string(res));
    ASSERT(budget.max_dimensions == SOV_MAX_MATRIX_DIM, "test_resource_init", "max_dimensions mismatch");
    ASSERT(budget.max_cells == SOV_MAX_MATRIX_CELLS, "test_resource_init", "max_cells mismatch");
    ASSERT(budget.max_operations == SOV_OPERATION_BUDGET_PER_CALL, "test_resource_init", "max_operations mismatch");
    ASSERT(budget.operation_count == 0, "test_resource_init", "operation_count not 0");
    ASSERT(budget.budget_exceeded == false, "test_resource_init", "budget_exceeded not false");
    TEST_PASS("test_resource_init");
    return 0;
}

static int test_check_dimensions_valid(void)
{
    VerifyResult res = sov_check_dimensions(100, 100);
    ASSERT(res == VER_OK, "test_check_dimensions_valid", sov_verify_result_to_string(res));
    TEST_PASS("test_check_dimensions_valid");
    return 0;
}

static int test_check_dimensions_max_valid(void)
{
    /* 65535 should be valid (just under limit) */
    VerifyResult res = sov_check_dimensions(65535, 65535);
    ASSERT(res == VER_OK, "test_check_dimensions_max_valid", sov_verify_result_to_string(res));
    TEST_PASS("test_check_dimensions_max_valid");
    return 0;
}

static int test_check_dimensions_exceeded(void)
{
    /* 65537 exceeds SOV_MAX_MATRIX_DIM (65536) */
    VerifyResult res = sov_check_dimensions(65537, 1);
    ASSERT(res == VER_DIMS_EXCEEDED, "test_check_dimensions_exceeded", sov_verify_result_to_string(res));
    TEST_PASS("test_check_dimensions_exceeded");
    return 0;
}

static int test_check_matrix_cells_valid(void)
{
    /* 10x10 = 100 cells, well under limit */
    VerifyResult res = sov_check_matrix_cells(10, 10);
    ASSERT(res == VER_OK, "test_check_matrix_cells_valid", sov_verify_result_to_string(res));
    TEST_PASS("test_check_matrix_cells_valid");
    return 0;
}

static int test_check_matrix_cells_max_valid(void)
{
    /* 16384 x 16384 = 268435456 (max cells exactly) */
    VerifyResult res = sov_check_matrix_cells(16384, 16384);
    ASSERT(res == VER_OK, "test_check_matrix_cells_max_valid", sov_verify_result_to_string(res));
    TEST_PASS("test_check_matrix_cells_max_valid");
    return 0;
}

static int test_check_matrix_cells_exceeded(void)
{
    /* 65536 x 65536 would exceed limit */
    VerifyResult res = sov_check_matrix_cells(65536, 65536);
    ASSERT(res == VER_CELLS_EXCEEDED, "test_check_matrix_cells_exceeded", sov_verify_result_to_string(res));
    TEST_PASS("test_check_matrix_cells_exceeded");
    return 0;
}

static int test_add_operation_cost_valid(void)
{
    SovResourceBudget budget;
    sov_init_resource_budget(&budget);

    VerifyResult res = sov_add_operation_cost(&budget, 1000);
    ASSERT(res == VER_OK, "test_add_operation_cost_valid", sov_verify_result_to_string(res));
    ASSERT(budget.operation_count == 1000, "test_add_operation_cost_valid", "operation_count mismatch");
    ASSERT(budget.budget_exceeded == false, "test_add_operation_cost_valid", "budget_exceeded should be false");
    TEST_PASS("test_add_operation_cost_valid");
    return 0;
}

static int test_add_operation_cost_exceeded(void)
{
    SovResourceBudget budget;
    sov_init_resource_budget(&budget);

    /* Try to add more than the budget allows */
    VerifyResult res = sov_add_operation_cost(&budget, SOV_OPERATION_BUDGET_PER_CALL + 1);
    ASSERT(res == VER_OPS_EXCEEDED, "test_add_operation_cost_exceeded", sov_verify_result_to_string(res));
    ASSERT(budget.budget_exceeded == true, "test_add_operation_cost_exceeded", "budget_exceeded should be true");
    TEST_PASS("test_add_operation_cost_exceeded");
    return 0;
}

static int test_verify_inv_with_budget_valid(void)
{
    int64_t I[] = {1, 0, 0, 1};
    SovResourceBudget budget;
    sov_init_resource_budget(&budget);

    VerifyResult res = sov_verify_inv(I, 4, I, 4, 2, &budget);
    ASSERT(res == VER_OK, "test_verify_inv_with_budget_valid", sov_verify_result_to_string(res));
    ASSERT(budget.operation_count > 0, "test_verify_inv_with_budget_valid", "operation_count should be tracked");
    TEST_PASS("test_verify_inv_with_budget_valid");
    return 0;
}

static int test_verify_sol_with_budget_valid(void)
{
    int64_t A[] = {2, 0, 0, 2};
    int64_t x[] = {2, 3};
    int64_t b[] = {4, 6};
    SovResourceBudget budget;
    sov_init_resource_budget(&budget);

    VerifyResult res = sov_verify_sol(A, 4, x, 2, b, 2, 2, 2, &budget);
    ASSERT(res == VER_OK, "test_verify_sol_with_budget_valid", sov_verify_result_to_string(res));
    ASSERT(budget.operation_count > 0, "test_verify_sol_with_budget_valid", "operation_count should be tracked");
    TEST_PASS("test_verify_sol_with_budget_valid");
    return 0;
}

int main(void)
{
    int failed = 0;

    printf("=== Sovereign Stack Machine Verifier Conformance Tests ===\n\n");

    printf("[TEST GROUP] Matrix Inversion (A*X = I)\n");
    failed += test_verify_inv_identity_2x2();
    failed += test_verify_inv_inverse_2x2();
    failed += test_verify_inv_fail_not_inverse();
    printf("\n");

    printf("[TEST GROUP] Linear System Solution (A*x = b)\n");
    failed += test_verify_sol_2x2_system();
    failed += test_verify_sol_overdetermined();
    failed += test_verify_sol_fail_wrong_solution();
    printf("\n");

    printf("[TEST GROUP] Least Squares (A^T(Ax-b) = 0)\n");
    failed += test_verify_lstsq_perfect_system();
    failed += test_verify_lstsq_overdetermined_exact();
    failed += test_verify_lstsq_fail_not_solution();
    printf("\n");

    printf("[TEST GROUP] Error Handling\n");
    failed += test_overflow_detection();
    failed += test_null_input_handling();
    printf("\n");

    printf("[TEST GROUP] Memory Safety (Phase 1, Step 1)\n");
    failed += test_safe_alloc_matrix_valid();
    failed += test_safe_alloc_matrix_zero_size();
    failed += test_safe_alloc_matrix_overflow_rows_cols();
    failed += test_safe_alloc_matrix_overflow_size();
    failed += test_validate_matrix_buffer_valid();
    failed += test_validate_matrix_buffer_null();
    failed += test_validate_matrix_buffer_undersized();
    failed += test_validate_matrix_buffer_overflow_dims();
    failed += test_verify_inv_buffer_validation();
    failed += test_verify_sol_buffer_validation();
    failed += test_verify_lstsq_buffer_validation();
    failed += test_safe_free_matrix_null();
    printf("\n");

    printf("[TEST GROUP] Resource Limits (Phase 1, Step 2)\n");
    failed += test_resource_init();
    failed += test_check_dimensions_valid();
    failed += test_check_dimensions_max_valid();
    failed += test_check_dimensions_exceeded();
    failed += test_check_matrix_cells_valid();
    failed += test_check_matrix_cells_max_valid();
    failed += test_check_matrix_cells_exceeded();
    failed += test_add_operation_cost_valid();
    failed += test_add_operation_cost_exceeded();
    failed += test_verify_inv_with_budget_valid();
    failed += test_verify_sol_with_budget_valid();
    printf("\n");

    printf("[TEST GROUP] Negative Tests (Phase 1, Step 3)\n");
    failed += run_negative_tests();

    if (failed == 0) {
        printf("=== ALL TESTS PASSED ===\n");
    } else {
        printf("=== %d TESTS FAILED ===\n", failed);
    }

    return failed;
}
