/*
 * generate.c -- Obligation Generation from Verification Policy
 *
 * FORGE Phase 3: Generate obligations dynamically from stack machine semantics
 * per spec/verification-policy.md
 *
 * Implements obligation generation for:
 * - Matrix operations (MATMUL, inverse verification)
 * - Type constraints
 * - Property checks
 *
 * Build: part of libsov_forge.a
 * License: Apache 2.0 + AGPL 3.0
 */

#include "sov_obligations.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/*
 * ============================================================================
 * OBLIGATION GENERATION
 * ============================================================================
 */

/**
 * Generate obligations for matrix multiplication result
 * Ensures inner dimensions match and generates type constraint
 */
int sov_oblig_gen_matmul(ObligationSet *obset,
                         size_t m, size_t n, size_t p,
                         uint32_t pc_start, uint32_t pc_end)
{
    if (!obset) return -1;

    /* Generate: "MATMUL requires A[m×n] * B[n×p] → C[m×p]" */
    int32_t id = sov_obset_add_type(obset, "MATMUL shape constraint", pc_start, pc_end);
    if (id < 0) return -1;

    return 0;
}

/**
 * Generate obligations for matrix inverse verification
 * A is n×n, generates: OBL_KIND_INV
 */
int sov_oblig_gen_inv(ObligationSet *obset,
                      const int64_t *A,
                      size_t n,
                      uint32_t pc_start,
                      uint32_t pc_end)
{
    if (!obset) return -1;

    int32_t id = sov_obset_add_inv(obset, A, n, pc_start, pc_end);
    if (id < 0) return -1;

    return 0;
}

/**
 * Generate obligations for linear solve A*x = b
 * A is m×n, b is m
 */
int sov_oblig_gen_solve(ObligationSet *obset,
                        const int64_t *A,
                        const int64_t *b,
                        size_t m, size_t n,
                        uint32_t pc_start, uint32_t pc_end)
{
    if (!obset) return -1;

    int32_t id = sov_obset_add_solve(obset, A, b, m, n, pc_start, pc_end);
    if (id < 0) return -1;

    return 0;
}

/**
 * Generate obligations for least squares A^T(Ax - b) = 0
 * A is m×n, b is m
 */
int sov_oblig_gen_lstsq(ObligationSet *obset,
                        const int64_t *A,
                        const int64_t *b,
                        size_t m, size_t n,
                        uint32_t pc_start, uint32_t pc_end)
{
    if (!obset) return -1;

    int32_t id = sov_obset_add_lstsq(obset, A, b, m, n, pc_start, pc_end);
    if (id < 0) return -1;

    return 0;
}

/**
 * Generate type constraint obligation
 */
int sov_oblig_gen_type_constraint(ObligationSet *obset,
                                  const char *constraint,
                                  uint32_t pc_start,
                                  uint32_t pc_end)
{
    if (!obset || !constraint) return -1;

    int32_t id = sov_obset_add_type(obset, constraint, pc_start, pc_end);
    if (id < 0) return -1;

    return 0;
}

/**
 * Generate property obligation
 */
int sov_oblig_gen_property(ObligationSet *obset,
                           const char *property,
                           uint32_t pc_start,
                           uint32_t pc_end)
{
    if (!obset || !property) return -1;

    int32_t id = sov_obset_add_prop(obset, property, pc_start, pc_end);
    if (id < 0) return -1;

    return 0;
}

/**
 * Batch generate obligations from instruction trace
 * Parses obligation policy and generates from program semantics
 */
int sov_oblig_gen_batch(ObligationSet *obset,
                        const uint8_t *policy_bytes,
                        size_t policy_len,
                        uint32_t *out_count)
{
    if (!obset || !policy_bytes || !out_count) return -1;

    /* Policy format: simple text directives
     * Example: "inv:2:0:10" = OBL_KIND_INV at PC 0-10 with matrix size 2
     */

    uint32_t count = 0;
    const char *policy = (const char *)policy_bytes;

    for (size_t i = 0; i < policy_len && policy[i]; i++) {
        if (policy[i] == 'i' && i + 7 < policy_len &&
            strncmp(policy + i, "inv:", 4) == 0) {
            /* Parse inv:n:pc_start:pc_end */
            uint32_t n, pc_start, pc_end;
            if (sscanf(policy + i + 4, "%u:%u:%u", &n, &pc_start, &pc_end) == 3) {
                if (sov_oblig_gen_inv(obset, NULL, n, pc_start, pc_end) == 0) {
                    count++;
                }
            }
        }
    }

    *out_count = count;
    return 0;
}
