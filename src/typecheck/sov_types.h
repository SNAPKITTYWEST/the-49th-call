/*
 * sov_types.h -- Sovereign Stack Machine Type System
 *
 * Heterogeneous stack type system with shape tracking.
 * Implements type inference from instruction-semantics.md spec.
 *
 * FORGE Phase 2: Type Inference Engine
 *
 * Build: part of libsov_forge.a
 * License: Apache 2.0 + AGPL 3.0
 */

#ifndef SOV_TYPES_H
#define SOV_TYPES_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * ============================================================================
 * TYPE SYSTEM
 * ============================================================================
 */

/* Base value types */
typedef enum {
    VAL_SCALAR,         /* int64_t scalar */
    VAL_VECTOR,         /* Dynamic vector [int64_t] */
    VAL_MATRIX,         /* 2D matrix int64_t[m][n] */
    VAL_PROOF,          /* Proof object (opaque) */
} ValType;

/* Shape descriptor for matrices and vectors */
typedef struct {
    size_t rows;        /* Number of rows (1 for vectors) */
    size_t cols;        /* Number of columns */
} Shape;

/* Stack value with type and shape info */
typedef struct {
    ValType type;
    Shape shape;
    void *data;         /* Pointer to actual data */
    bool is_owned;      /* If true, we allocated this */
} StackValue;

/* Stack snapshot */
typedef struct {
    StackValue *values;
    size_t depth;
} Stack;

/* Type environment from type-rules.md */
typedef struct {
    uint32_t num_vars;
    char **var_names;
    ValType *var_types;
    Shape *var_shapes;
} TypeEnv;

/* Type inference result */
typedef struct {
    Stack *final_stack;         /* Inferred final stack type */
    uint32_t *obligation_ids;   /* Generated obligation IDs */
    size_t num_obligations;     /* Number of obligations */
    char *error_msg;            /* Error message if inference failed */
} InferResult;

/*
 * ============================================================================
 * TYPE INFERENCE API
 * ============================================================================
 */

/* Create new type environment */
TypeEnv *sov_tyenv_new(void);

/* Free type environment */
void sov_tyenv_free(TypeEnv *env);

/* Add variable to environment */
int sov_tyenv_add_var(TypeEnv *env,
                      const char *name,
                      ValType type,
                      size_t rows,
                      size_t cols);

/* Create empty stack */
Stack *sov_stack_new(void);

/* Free stack */
void sov_stack_free(Stack *stack);

/* Push value onto stack with type */
int sov_stack_push(Stack *stack,
                   ValType type,
                   size_t rows,
                   size_t cols,
                   void *data,
                   bool is_owned);

/* Pop value from stack */
StackValue *sov_stack_pop(Stack *stack);

/* Peek at top value */
StackValue *sov_stack_peek(Stack *stack);

/* Type inference engine: infer(program, initial_stack) -> (final_stack, obligations) */
InferResult *sov_infer_program(const uint8_t *program_bytes,
                               size_t program_len,
                               Stack *initial_stack,
                               TypeEnv *env);

/* Free inference result */
void sov_infer_free(InferResult *result);

/* Shape unification: check if two shapes are compatible */
bool sov_shape_unify(Shape s1, Shape s2);

/* Type display for debugging */
void sov_type_print(ValType t, Shape s);

#ifdef __cplusplus
}
#endif

#endif /* SOV_TYPES_H */
