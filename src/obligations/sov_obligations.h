/*
 * sov_obligations.h -- Obligation Generator
 *
 * Generates verification obligations as type inference encounters them.
 * Tracks obligation IDs, kinds, and locations in the program.
 *
 * FORGE Phase 2: Obligation Generator
 *
 * Build: part of libsov_forge.a
 * License: Apache 2.0 + AGPL 3.0
 */

#ifndef SOV_OBLIGATIONS_H
#define SOV_OBLIGATIONS_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * ============================================================================
 * OBLIGATION STRUCTURES
 * ============================================================================
 */

typedef enum {
    OBL_KIND_INV,       /* Invariant: A*X = I */
    OBL_KIND_SOLVE,     /* Linear solve: A*x = b */
    OBL_KIND_LSTSQ,     /* Least squares: A^T(Ax-b) = 0 */
    OBL_KIND_TYPE,      /* Type constraint */
    OBL_KIND_PROP,      /* Property holds */
} ObligationKind;

/* Single obligation */
typedef struct {
    uint32_t id;                /* Unique ID (0, 1, 2, ...) */
    ObligationKind kind;        /* Type of obligation */
    uint32_t start_pc;          /* Program counter where generated */
    uint32_t end_pc;            /* End PC of effect */

    /* Human-readable description */
    char *description;
    size_t desc_len;

    /* Parameters (interpretation depends on kind) */
    void **params;              /* Parameter pointers */
    size_t num_params;
} Obligation;

/* Obligation set (growable) */
typedef struct {
    Obligation *items;          /* Array of obligations */
    size_t count;               /* Current count */
    size_t capacity;            /* Allocated capacity */
    uint32_t next_id;           /* Next obligation ID to assign */
} ObligationSet;

/*
 * ============================================================================
 * OBLIGATION API
 * ============================================================================
 */

/* Create new obligation set */
ObligationSet *sov_obset_new(void);

/* Free obligation set and all contents */
void sov_obset_free(ObligationSet *obset);

/* Add an invariant obligation
 * Creates: OBL_KIND_INV with matrix A (n x n)
 * Returns: obligation ID on success, -1 on failure
 */
int32_t sov_obset_add_inv(ObligationSet *obset,
                          const int64_t *A,
                          size_t n,
                          uint32_t pc_start,
                          uint32_t pc_end);

/* Add a linear solve obligation
 * Creates: OBL_KIND_SOLVE with A (m x n), b (m)
 * Returns: obligation ID, -1 on failure
 */
int32_t sov_obset_add_solve(ObligationSet *obset,
                            const int64_t *A,
                            const int64_t *b,
                            size_t m,
                            size_t n,
                            uint32_t pc_start,
                            uint32_t pc_end);

/* Add a least squares obligation
 * Creates: OBL_KIND_LSTSQ with A (m x n), b (m)
 * Returns: obligation ID, -1 on failure
 */
int32_t sov_obset_add_lstsq(ObligationSet *obset,
                            const int64_t *A,
                            const int64_t *b,
                            size_t m,
                            size_t n,
                            uint32_t pc_start,
                            uint32_t pc_end);

/* Add a type constraint obligation
 * Creates: OBL_KIND_TYPE
 * Returns: obligation ID, -1 on failure
 */
int32_t sov_obset_add_type(ObligationSet *obset,
                           const char *description,
                           uint32_t pc_start,
                           uint32_t pc_end);

/* Add a property obligation
 * Creates: OBL_KIND_PROP
 * Returns: obligation ID, -1 on failure
 */
int32_t sov_obset_add_prop(ObligationSet *obset,
                           const char *property,
                           uint32_t pc_start,
                           uint32_t pc_end);

/* Get obligation by ID */
Obligation *sov_obset_get(ObligationSet *obset, uint32_t id);

/* Iteration: get obligation at index */
Obligation *sov_obset_at(ObligationSet *obset, size_t index);

/* Set witness data for an obligation */
int sov_obset_set_witness(ObligationSet *obset,
                          uint32_t id,
                          const void *witness,
                          size_t witness_len);

/* Get witness from obligation */
const void *sov_obset_get_witness(ObligationSet *obset,
                                   uint32_t id,
                                   size_t *out_len);

/* Serialize obligations to JSON */
int sov_obset_to_json(ObligationSet *obset,
                      uint8_t **out_json,
                      size_t *out_len);

/* Deserialize from JSON */
ObligationSet *sov_obset_from_json(const uint8_t *json_bytes,
                                    size_t len);

#ifdef __cplusplus
}
#endif

#endif /* SOV_OBLIGATIONS_H */
