# Sovereign Forge Developer Guide

Complete technical guide for Sovereign Forge contributors.

## Architecture Deep Dive - Five Verification Layers

Layer 1: Verifier (src/verifier/sov_verifier.c)
- Performs exact int64_t arithmetic with overflow detection
- Executes matrix stack machine operations
- Validates memory safety via safe_alloc_matrix()
- No type checking, no obligation generation
- Return codes: VER_OK, VER_OVERFLOW, VER_UNDERFLOW, VER_BUFFER_OVERFLOW, VER_NULL_INPUT

Layer 2: Type System (src/typecheck/sov_types.c)
- Maintains type environment Gamma: variable names to (type, shape)
- Implements stack push/pop/peek with type tracking
- Infers types for matrix operations
- Tracks shape constraints: (rows, cols) pairs
- Detects type mismatches before execution

Layer 3: Obligation Generation (src/obligations/sov_obligations.c)
- Generates obligations from type inference failures
- Links each failed constraint to source location
- Creates obligation lists for external verification
- Obligation types: overflow_checks, underflow_checks, shape_matches, dimension_bounds

Layer 4: Certificate (src/certificate/sov_cert.c)
- Wraps execution history: initial stack, final stack, type environment
- Records program hash (SHA-256), instruction count, execution duration
- Encodes obligations in CBOR (RFC 8949) format
- Produces canonical JSON for round-trip verification
- Includes Blake3 + Ed25519 signatures for tamper detection

Layer 5: Receipt Chain (src/receipts/)
- Appends certificates to immutable WORM (write-once read-many) ledger
- Tracks certificate lineage: previous_hash to current_hash
- Stores on-chain metadata: timestamp, source authority, verification status
- Enables external verification without access to original program

## Formal Semantics

Type judgments: Gamma |- i v tau, O, Gamma-prime

Where:
- Gamma = type environment (input)
- |- = yields or entails
- i = instruction code
- v = produces or executes to
- tau = type of result (MatrixI64, VectorI64, ScalarI64)
- O = obligations (constraints to verify)
- Gamma-prime = type environment (output)

Example 1: Gamma |- push(42) v ScalarI64, {}, Gamma
Example 2: Gamma |- mul v MatrixI64, {shape_match(A.cols, B.rows)}, Gamma

### How Obligations Are Generated

1. During type inference: sov_infer_instruction() checks compatibility
2. On mismatch: obligation record appended to obligation list
3. Obligation fields: type, pc, source_loc, constraint_text

## Contributing to the Verifier Core

### Exact Arithmetic Invariants

Critical invariants:
1. No silent overflow: check for int64_t overflow
2. Exact semantics: exact integer math only
3. Bounded dimensions: max 65536
4. Safe allocation: malloc() paired with overflow checks

### Safe Arithmetic Functions

Use exclusively:
- __builtin_add_overflow(a, b, &result)
- __builtin_sub_overflow(a, b, &result)
- __builtin_mul_overflow(a, b, &result)

### Testing: Write Conformance Test

1. Define test in tests/conformance/test_verifier.c
2. Register test in registry
3. Add adversarial case in tests/adversarial/
4. Run: make test-asan && make check-sanitizers

### Sanitizer: Run before Submitting

- make test-asan (Address Sanitizer + UBSan)
- make run-fuzzer (libFuzzer, 10 seconds)
- make clean && make all && make run-tests

Address Sanitizer detects: overflows, use-after-free, double-free, leaks
UBSan detects: integer overflow, division by zero, nullptr, out-of-bounds

### Overflow: Use __builtin_*_overflow

CORRECT: if (__builtin_add_overflow(a, b, &result)) return VER_OVERFLOW;
WRONG: int64_t result = a + b;

## Adding New Verification Operations

### Step 1: spec/instruction-semantics.md

Add formal stack type transition.

### Step 2: spec/type-rules.md

Document preconditions, postconditions, obligations.

### Step 3: src/verifier/sov_verifier.c

1. Add operation code to .h
2. Implement verification function
3. Register in main dispatch

### Step 4: Add Tests

Conformance and adversarial test cases.

### Step 5: spec/verification-policy.md

Document obligation type and discharge.

## Formal Verification Workflow

### Step 1: Write Lean 4 Proof

Create theorem in proofs/lean4/.

### Step 2: Extract to C

lean extract sov_custom_op.lean --output sov_custom_op_extracted.c

### Step 3: Verify C Matches

Property tests in tests/property/.

### Step 4: Publish Together

Link theorem to implementation.

## Build System

### Makefile.sov Overview

Main build: Makefile.sov (repo root)
Variables: CC, CFLAGS, VERIFIER_SRCS, CERT_SRCS, OBL_SRCS, TYPE_SRCS
Tests: CONFORMANCE_TESTS, ADVERSARIAL_TESTS

### Available Targets

all - Build libsov_forge.a + tests
libsov_forge.a - Static library
test_verifier - Conformance (38 tests)
test_infer - Type inference
run-tests - Execute all
test-asan - With ASan + UBSan
check-sanitizers - Validate 46 tests
run-fuzzer - libFuzzer (10s)
clean - Remove artifacts

### Adding New Test

1. Create tests/conformance/test_new_feature.c
2. Update Makefile.sov
3. Register in runner
4. Build: make clean && make test-asan

## Code Style

### C99, K&R Braces, 80-Column Comments

Function:
  VerifyResult sov_verify_operation(SafeMatrix *a, SafeMatrix *b)
  {
      if (!a || !b) {
          return VER_NULL_INPUT;
      }
      return VER_OK;
  }

K&R placement:
- Opening brace on same line as function/if/loop
- Closing brace on own line

### No C++ Features

ALLOWED: stdlib.h, string.h, stdint.h, limits.h
FORBIDDEN: vector, iostream, class

### Explicit Error Returns

CORRECT: return VER_OVERFLOW
WRONG: silent wrap or undefined behavior

### Public Functions: extern in .h, definition in .c

Header: extern declaration
Implementation: function body

## Security Review Checklist

### Before Code Review

- No unbounded loops: max 65536
- No string formatting: use memcpy
- No floats: int64_t only
- All mallocs: overflow check
- All buffer accesses: bounds check

### Overflow Checking

Addition: __builtin_add_overflow(a, b, &res)
Subtraction: __builtin_sub_overflow(a, b, &res)
Multiplication: __builtin_mul_overflow(a, b, &res)
Size: manual check before malloc

### Memory Safety

make clean
make CC=gcc CFLAGS="-fsanitize=address -g" test-asan
valgrind --leak-check=full ./tests/conformance/test_verifier
make CC=gcc CFLAGS="-fsanitize=undefined -g" test-asan

## Release Process

### Version Format

Development: vX.Y.Z-rcN
Stable: vX.Y.Z

### Signed Tags

git tag -s -m "Release v1.0.0" v1.0.0
git tag -v v1.0.0
git push origin v1.0.0

### GitHub Releases

gh release create v1.0.0 --title "Sovereign Forge v1.0.0"
make clean && make all
gpg --armor --detach-sign libsov_forge.a
sha256sum libsov_forge.a > libsov_forge.a.sha256
gh release upload v1.0.0 libsov_forge.a libsov_forge.a.asc libsov_forge.a.sha256

### Announce Release

GitHub Discussions
Academic mailing lists
Security communities

### Release Checklist

- Version bumped in src/verifier.h
- CHANGELOG.md updated
- All tests pass: make check-sanitizers
- Fuzzer runs 10s without crash: make run-fuzzer
- README.md current
- Signed tag created
- Artifacts built on clean checkout
- Reproducible build hash computed
- GitHub release created
- Announcement posted
- Tagged commit pushed

## Questions & Support

1. Read spec: spec/instruction-semantics.md, spec/type-rules.md
2. Check tests: tests/conformance/, tests/adversarial/
3. File issue: Include reproduction steps, sanitizer output
4. Submit PR: Reference issue, include tests, pass all checks

---

Last updated: 2026-07-29
Maintainers: Sovereign Forge Team
License: Apache 2.0 + AGPL 3.0

## Detailed Implementation Guide

### Safe Memory Management Deep Dive

#### Allocation with Overflow Prevention

Safe matrix allocation requires checking for multiplication overflow:

Step 1: Check rows * cols overflow size_t
  if (rows > 0 && cols > SIZE_MAX / rows) return NULL;

Step 2: Compute safe element count
  size_t nelems = rows * cols;

Step 3: Check nelems * sizeof(int64_t) overflow
  if (nelems > 0 && nelems > SIZE_MAX / sizeof(int64_t)) return NULL;

Step 4: Compute safe byte size
  size_t data_size = nelems * sizeof(int64_t);

Step 5: Allocate struct
  SafeMatrix *m = (SafeMatrix *)malloc(sizeof(SafeMatrix));
  if (!m) return NULL;

Step 6: Allocate data buffer
  if (nelems > 0) {
    m->data = (int64_t *)malloc(data_size);
    if (!m->data) { free(m); return NULL; }
  }

Step 7: Initialize and return
  m->rows = rows; m->cols = cols; m->capacity = nelems;
  return m;

#### Buffer Validation

Every buffer access must be validated first:

VerifyResult validate_matrix_buffer(
  const int64_t *data, size_t rows, size_t cols, size_t capacity) {
  if (!data) return VER_NULL_INPUT;
  if (rows > 0 && cols > SIZE_MAX / rows) return VER_OVERFLOW;
  size_t required = rows * cols;
  if (required > capacity) return VER_BUFFER_OVERFLOW;
  return VER_OK;
}

### Type System Implementation

#### Type Environment Management

Type environment stores bindings from variable names to types:

typedef struct {
  char **var_names;
  ValType *var_types;
  Shape *var_shapes;
  uint32_t num_vars;
} TypeEnv;

Operations:
- TypeEnv *sov_tyenv_new() creates new empty environment
- int sov_tyenv_add_var() adds binding (name, type, shape)
- int sov_tyenv_lookup_var() retrieves binding for name
- void sov_tyenv_free() deallocates environment

Capacity: 64 variables maximum to prevent unbounded allocation

#### Stack Operations with Type Tracking

Stack value structure includes ownership tracking:

typedef struct {
  ValType type;
  size_t rows;
  size_t cols;
  void *data;
  int is_owned;
} StackValue;

Stack operations:
- Stack *sov_stack_new() creates stack
- int sov_stack_push() pushes value, check depth < 256
- int sov_stack_pop() pops value, check depth > 0
- StackValue *sov_stack_peek() access top without pop
- void sov_stack_free() deallocates stack

#### Type Inference Engine

Main inference function processes each instruction:

TypeCheckResult sov_infer_instruction(
  TypeEnv *env, Stack *stack, uint8_t opcode, uint8_t *operands)

For each instruction:
1. Check preconditions (stack depth, type constraints)
2. Pop input values from stack
3. Infer output type and shape
4. Generate obligations if constraints violated
5. Push result onto stack
6. Update type environment if necessary

### Obligation Generation Details

#### Obligation Record Structure

typedef struct {
  uint8_t type;
  uint32_t pc;
  uint32_t source_line;
  char constraint_text[256];
  uint8_t severity;
} Obligation;

Types: OBLIG_STACK_UNDERFLOW, OBLIG_STACK_OVERFLOW,
OBLIG_OVERFLOW, OBLIG_DIMENSION_BOUNDS, OBLIG_SHAPE_MISMATCH,
OBLIG_TYPE_MISMATCH, OBLIG_NULL_POINTER, OBLIG_MEMORY_BOUNDS

#### Obligation List Management

typedef struct {
  Obligation *obligations;
  uint32_t count;
  uint32_t capacity;
} ObligationList;

Operations:
- ObligationList *sov_obligations_new() creates empty
- int sov_obligations_add() appends and expands if needed
- void sov_obligations_free() deallocates all

### Certificate Encoding

#### CBOR Serialization

Canonical CBOR encoding for portability:

Major types: 0=uint, 1=nint, 2=bstr, 3=tstr, 4=array, 5=map

Certificate structure (CBOR map):
0: program_hash (bytes, 32)
1: initial_stack (array of integers)
2: final_stack (array of integers)
3: obligations (array of maps)
4: timestamp (integer, UNIX seconds)
5: program_size (integer)
6: instruction_count (integer)
7: type_env_snapshot (map)

#### Canonical JSON Generation

JSON representation for human readability and verification:
- All object keys sorted alphabetically
- No whitespace (compact form)
- Numbers encoded as decimal strings for exact precision
- Hashes encoded as hex strings (lowercase)

#### Signature Generation

1. Compute Blake3 hash of canonical JSON
2. Sign Blake3 digest with Ed25519 private key
3. Include public key in certificate metadata
4. Signature = 64 bytes (Ed25519)
5. Public key = 32 bytes (Ed25519)

### Receipt Chain Architecture

#### WORM Ledger Structure

Write-once read-many ledger stores immutable receipt history:

Receipt structure:
  index: receipt number (monotonically increasing)
  certificate_hash: SHA-256 of certificate
  previous_hash: SHA-256 of previous receipt
  timestamp: UNIX seconds when appended
  source_authority: identifier of appender
  verification_status: VERIFIED or UNVERIFIED

Ledger file format (JSONL):
- One receipt per line
- Each line is a complete JSON object
- Immutable: never modify or delete lines

#### Hash Chain Verification

To verify receipt N at index I:

1. Read receipt N: has certificate_hash and previous_hash
2. Read receipt N-1: has certificate_hash
3. Compute SHA-256 of receipt N-1 serialization
4. Verify computed hash = previous_hash of N
5. Continue to first receipt (previous_hash should be zero)

### Test Structure

#### Conformance Test Framework

Conformance tests verify correct behavior with known inputs:

void test_operation_case(void) {
  SafeMatrix *input = safe_alloc_matrix(...);
  VerifyResult result = sov_verify_operation(...);
  ASSERT_EQ(result, VER_OK);
  ASSERT_EQ(output->data[i], EXPECTED_VALUE);
  safe_free_matrix(input);
}

Test cases should cover:
- Boundary values (0, 1, MAX)
- Normal/typical inputs
- Edge cases (empty matrices, dimension 1)
- Full rows and columns

#### Adversarial Test Framework

Adversarial tests verify error handling:

void test_operation_failure(void) {
  SafeMatrix *a = safe_alloc_matrix(2, 3);
  SafeMatrix *b = safe_alloc_matrix(5, 4);
  VerifyResult result = sov_verify_operation(a, b, ...);
  ASSERT_EQ(result, VER_SHAPE_MISMATCH);
  safe_free_matrix(a);
  safe_free_matrix(b);
}

Error cases should cover:
- Null pointers (NULL input)
- Shape mismatches (cols != rows)
- Overflow inputs (INT64_MAX)
- Underflow stack depth (empty stack)
- Dimension bounds (>65536)

#### Fuzzing with libFuzzer

libFuzzer generates random test inputs:

extern int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size);

Fuzz target:
1. Creates random matrices from fuzz input bytes
2. Calls verifier with random inputs
3. Checks for crashes, hangs, undefined behavior

Running: make run-fuzzer (runs for 10 seconds)

### Performance Considerations

#### Complexity Analysis

Matrix multiplication: O(m * n * p) for m x n times n x p

For max dimensions (65536 x 65536 x 65536):
- Would require massive memory
- Not practical in real systems
- Bounded by memory allocation

Element-wise operations: O(m * n)

#### Benchmarking

Benchmark template:
1. Allocate input matrices
2. Record start time with clock_gettime()
3. Execute operation
4. Record end time
5. Compute elapsed time in nanoseconds
6. Report per-element processing rate

### Integration Testing

#### Integration Test Scenarios

Scenario 1: Multi-operation sequence
- Load matrix A
- Load matrix B
- Multiply A*B = C
- Transpose C = D
- Verify final result

Scenario 2: Type inference with all operations
- Execute sequence of operations
- Verify type environment evolves correctly
- Check all shapes match expectations

Scenario 3: Full verification pipeline
- Execute program bytecode
- Generate type environment
- Generate obligations
- Create certificate
- Sign with Ed25519
- Append to WORM ledger
- Verify round-trip

#### Cross-layer Testing

Test Layer 1 (Verifier) alone: arithmetic, buffers
Test Layer 1-2: type inference, shapes
Test Layer 1-3: obligation generation
Test Layer 1-4: certificate serialization, signatures
Test Layer 1-5: full end-to-end with WORM ledger

### Documentation Standards

#### Function Documentation

Every public function requires complete documentation:

/*
 * sov_verify_matrix_multiply - Matrix multiplication with overflow.
 *
 * Description:
 *   Multiplies matrix A (m x n) by matrix B (n x p) to produce
 *   result (m x p). Performs exact arithmetic with int64_t
 *   overflow detection. Shape constraint: a->cols must equal
 *   b->rows.
 *
 * Arguments:
 *   a: Input matrix (m x n), must not be NULL, capacity >= m*n
 *   b: Input matrix (n x p), must not be NULL, capacity >= n*p
 *   result: Output (m x p), must not be NULL, capacity >= m*p
 *
 * Returns:
 *   VER_OK on success.
 *   VER_NULL_INPUT if argument is NULL.
 *   VER_SHAPE_MISMATCH if a->cols != b->rows.
 *   VER_OVERFLOW if multiply or add overflows.
 *   VER_BUFFER_OVERFLOW if result buffer too small.
 *
 * Side effects:
 *   Modifies result->data in place with product matrix.
 *
 * Thread safety:
 *   Not thread-safe. Caller must synchronize accesses.
 */

#### Test Documentation

Each test file documents its purpose:

/*
 * test_verifier.c - Conformance tests for verifier layer.
 *
 * Tests the arithmetic correctness and safety properties of all
 * verifier operations. Each test covers one operation with
 * specific inputs designed to validate the operation's type
 * signature and overflow behavior.
 *
 * Test coverage:
 *   - 38 conformance tests for all operations
 *   - Known inputs and expected outputs
 *   - Boundary values (0, 1, MAX, overflow)
 */

### Code Review Checklist

When reviewing new operations:

1. Specification validation
   - Stack type judgment in spec/instruction-semantics.md
   - Type rule in spec/type-rules.md
   - Obligation types in spec/verification-policy.md

2. Implementation quality
   - All pointer arguments checked for NULL
   - All arithmetic uses __builtin_*_overflow()
   - All buffer accesses validated with bounds check
   - No unbounded loops (max 65536)
   - Memory allocation checks for overflow

3. Test coverage
   - Conformance test with known values
   - Adversarial test with shape mismatch
   - Adversarial test with overflow
   - Adversarial test with null pointer
   - All tests pass with make test-asan

4. Documentation
   - Function header comment complete
   - Return codes documented
   - Side effects documented
   - Thread safety documented

5. Build system
   - Builds with make all without warnings
   - Tests registered in Makefile.sov
   - make clean removes all artifacts
   - make run-fuzzer completes without crash

6. Security
   - No format strings
   - No unbounded string operations
   - No floating-point arithmetic
   - No cryptographic shortcuts
   - All overflow cases handled

### Continuous Integration

#### CI Pipeline Requirements

The CI system must verify:

1. Compilation stage
   - Compile with gcc -std=c99 -Wall -Wextra
   - Compile with clang for comparison
   - All warnings are errors

2. Testing stage
   - Run make test-asan (sanitizers)
   - Run make run-fuzzer (10 seconds)
   - Run make check-sanitizers (46 tests)

3. Coverage stage
   - Generate coverage report
   - Fail if coverage < 90%

4. Reproducibility stage
   - Clean checkout
   - Build with make all
   - Compute SHA-256 of all artifacts

5. Security stage
   - Run static analysis tools
   - Check for common vulnerabilities

### Troubleshooting Common Issues

#### Build Failures

Issue: Undefined reference to function
Solution: Ensure function is defined in .c file and declared in .h

Issue: Compilation warning about unused variable
Solution: Remove unused variable or prefix with _ to suppress

Issue: Test fails but works locally
Solution: Check for timezone/locale dependencies, use UTC

#### Sanitizer Reports

Issue: SEGV (segmentation fault)
Solution: Null pointer dereference - add NULL check before use

Issue: Heap buffer overflow
Solution: Validate buffer size before access - check bounds

Issue: Undefined Behavior Sanitizer integer overflow
Solution: Use __builtin_*_overflow instead of raw operators

### Performance Optimization

#### Micro-optimizations

1. Inline simple functions to reduce call overhead

2. Avoid repeated bounds checks in tight loops
   Bad:  for (i) { validate_access(data, i); data[i] = ... }
   Good: validate_once(); for (i) { data[i] = ... }

3. Use const pointers to enable compiler optimizations
   const int64_t *src = a->data;

4. Cache matrix dimensions in local variables
   size_t rows = m->rows; size_t cols = m->cols;

#### Profiling

Profile operations to identify bottlenecks:

perf record -g ./test_verifier
perf report

This shows:
- Time spent in each function
- Cache misses and page faults
- Branch prediction failures

---

Document History:
- 2026-07-29: Complete version with 1200+ lines


## Advanced Topics

### Extending to Higher-Order Tensors

The current architecture supports 2D matrices. To extend:

1. Update Shape structure:
   typedef struct {
     uint8_t ndim;  /* number of dimensions (2-4) */
     size_t dims[4]; /* dimensions for each axis */
   } Shape;

2. Update StackValue to use extended Shape

3. Implement tensor operations:
   - 3D tensor multiplication (batched matrix multiply)
   - Tensor transpose (arbitrary axis permutation)
   - Tensor reshape (change dimensions)
   - Tensor slice (extract subregion)

4. Add type rules for tensor operations

5. Implement conformance and adversarial tests

### Parallelization Strategy

To parallelize matrix operations:

1. Partition matrix by rows (row-wise stripping)
   - Each thread computes subset of result rows
   - No false sharing if chunks large enough

2. Use OpenMP for thread pool:
   #pragma omp parallel for
   for (size_t i = 0; i < result_rows; i++) {
       for (size_t j = 0; j < result_cols; j++) {
           /* compute result[i][j] */
       }
   }

3. Ensure atomicity of overflow checks
   - Overflow is local to each thread
   - Reduce error codes after computation

### GPU Acceleration

To use GPU (CUDA/OpenCL):

1. Create GPU memory management layer
   - Copy SafeMatrix from host to device
   - Ensure device allocations check overflow
   - Copy results back to host

2. Implement GPU kernels for each operation
   - Matrix multiply (highly parallel)
   - Element-wise operations
   - Reductions (transpose, dot product)

3. Benchmark GPU vs CPU
   - GPU beneficial for large matrices (>1000x1000)
   - CPU better for small matrices (overhead)

### Distributed Computing

To support distributed matrices:

1. Partitioning strategy:
   - 2D block partitioning across nodes
   - Each node stores block and computation logic

2. Communication protocol:
   - Send matrix blocks via MPI or gRPC
   - Ensure integrity of transmitted data

3. Consistency model:
   - Strong consistency: wait for all writes
   - Eventual consistency: read-repair

### Persistence and Recovery

To support persistence:

1. Checkpointing strategy:
   - Periodic snapshots of all matrices
   - Save to disk with CRC checksums

2. Recovery on failure:
   - Load latest checkpoint
   - Verify checksum
   - Replay subsequent operations

3. Transaction log:
   - Write-ahead log (WAL) for durability
   - Each operation logged before execution
   - Replay log after crash

## API Documentation

### Public Headers

#### src/verifier/sov_verifier.h

Core verification functions:

SafeMatrix* safe_alloc_matrix(size_t rows, size_t cols)
  - Allocate matrix with overflow checking
  - Returns NULL on allocation failure
  - Capacity automatically set

void safe_free_matrix(SafeMatrix *m)
  - Deallocate matrix and its data
  - Safe to call with NULL

VerifyResult validate_matrix_buffer(
  const int64_t *data, size_t rows, size_t cols, size_t capacity)
  - Validate buffer size before access
  - Returns VER_OK or specific error

VerifyResult sov_verify_push_constant(SafeMatrix *stack, int64_t value)
  - Push constant value onto stack
  - Stack must be allocated

VerifyResult sov_verify_pop(SafeMatrix *stack)
  - Pop value from stack
  - Check underflow before pop

#### src/typecheck/sov_types.h

Type inference functions:

TypeEnv *sov_tyenv_new(void)
  - Create new type environment
  - Empty environment with capacity 64

int sov_tyenv_add_var(TypeEnv *env, const char *name,
  ValType type, size_t rows, size_t cols)
  - Add variable binding
  - Check capacity not exceeded

#### src/obligations/sov_obligations.h

Obligation management:

ObligationList *sov_obligations_new(void)
  - Create empty obligation list
  - Initial capacity 32

int sov_obligations_add(ObligationList *list,
  uint8_t type, uint32_t pc, const char *text)
  - Append obligation to list
  - Auto-expand if capacity exceeded

#### src/certificate/sov_cert.h

Certificate functions:

ProofCertificate *sov_cert_new(void)
  - Create new certificate
  - Version 1, timestamp set

int sov_cert_set_stacks(ProofCertificate *cert,
  const int64_t *init, size_t init_len,
  const int64_t *final, size_t final_len)
  - Set initial and final stacks
  - Allocate and copy data

int sov_cert_to_cbor(ProofCertificate *cert,
  uint8_t *output, size_t output_size, size_t *written)
  - Serialize certificate to CBOR
  - Return number of bytes written

#### src/receipts/sov_receipts.h

Receipt ledger functions:

WormLedger *sov_ledger_open(const char *filepath)
  - Open WORM ledger file
  - Create if doesn't exist

int sov_ledger_append(WormLedger *ledger, const Receipt *receipt)
  - Append receipt to ledger
  - Assign index automatically
  - Verify hash chain

Receipt *sov_ledger_read(WormLedger *ledger, uint64_t index)
  - Read receipt at index
  - Verify hash chain integrity

## Version History

### v1.0.0 (2026-07-29)
- Complete 5-layer architecture
- 46 conformance + adversarial tests
- ASan/UBSan sanitizer integration
- CBOR certificate encoding
- Ed25519 signature support
- WORM receipt ledger

### Future v1.1.0
- 3D/4D tensor support
- OpenMP parallelization
- GPU acceleration
- Distributed matrix support

---

Complete Sovereign Forge Developer Guide
Last updated: 2026-07-29
For questions: See Questions & Support section


## Appendices

### Appendix A: Common Error Codes

VER_OK = 0x00
  Operation completed successfully.

VER_NULL_INPUT = 0x01
  One or more required pointer arguments is NULL.
  Caller must provide valid non-NULL pointers.

VER_UNDERFLOW = 0x02
  Stack depth less than required by operation.
  Operation needs N stack values but has M < N.

VER_OVERFLOW = 0x03
  Integer arithmetic overflow detected in computation.
  Result exceeds int64_t range [INT64_MIN, INT64_MAX].

VER_BUFFER_OVERFLOW = 0x04
  Buffer access exceeds allocated capacity.
  Attempt to access index >= capacity.

VER_SHAPE_MISMATCH = 0x05
  Matrix dimensions incompatible for operation.
  Example: multiply requires cols[A] == rows[B].

VER_DIMENSION_OVERFLOW = 0x06
  Result matrix dimensions exceed maximum (65536).
  Example: multiply 65536x65536 * 65536x65536.

VER_TYPE_MISMATCH = 0x07
  Type system constraint violated.
  Stack value type does not match expected type.

VER_MEMORY_ERROR = 0x08
  Memory allocation failed (malloc returned NULL).
  System out of memory or allocation too large.

### Appendix B: Operation Reference

All operations with stack transitions:

PUSH <value>
  Stack: [...] -> [..., <value>]
  Type: any scalar value

POP
  Stack: [..., X] -> [...]
  Type: X can be any type

ADD
  Stack: [..., A, B] -> [..., A+B]
  Type: scalar, scalar -> scalar

MUL (matrix multiply)
  Stack: [..., A, B] -> [..., A*B]
  Type: matrix m*n, matrix n*p -> matrix m*p

TRANSPOSE
  Stack: [..., A] -> [..., A^T]
  Type: matrix m*n -> matrix n*m

SCALE
  Stack: [..., A, s] -> [..., s*A]
  Type: matrix, scalar -> matrix

DOT
  Stack: [..., A, B] -> [..., sum(A*B)]
  Type: vector, vector -> scalar

### Appendix C: Build Commands Reference

make all
  Build library and all tests. Default target.

make libsov_forge.a
  Build static library only (no tests).

make test_verifier
  Build and link verifier conformance tests.

make run-tests
  Compile and run all tests with output.

make test-asan
  Build with ASan/UBSan and run conformance+adversarial.

make check-sanitizers
  Alias for test-asan (same functionality).

make run-fuzzer
  Build libFuzzer and run for 10 seconds.

make clean
  Remove all compiled objects, binaries, archives.

### Appendix D: Environment Setup

On Linux (Ubuntu/Debian):

sudo apt update
sudo apt install build-essential gcc clang valgrind

On macOS (Homebrew):

brew install gcc clang valgrind

On Windows (MSYS2):

pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-toolchain

### Appendix E: Git Workflow

1. Clone repository:
   git clone https://github.com/USER/sovereign-forge.git
   cd sovereign-forge

2. Create feature branch:
   git checkout -b feature/new-operation

3. Make changes:
   vim src/verifier/sov_verifier.c
   vim tests/conformance/test_verifier.c

4. Build and test:
   make clean && make test-asan

5. Commit changes:
   git add src/verifier/sov_verifier.c tests/conformance/test_verifier.c
   git commit -m "feat: add new operation"

6. Push to GitHub:
   git push origin feature/new-operation

7. Create pull request on GitHub

8. After review, merge to main:
   git checkout main
   git pull origin main
   git merge --no-ff feature/new-operation
   git push origin main

### Appendix F: Debugging Tips

#### Using GDB

gdb ./tests/conformance/test_verifier
(gdb) break sov_verify_multiply
(gdb) run
(gdb) print result->data[0]
(gdb) continue

#### Using Valgrind

valgrind --leak-check=full ./tests/conformance/test_verifier

#### Sanitizer Output Interpretation

SUMMARY: AddressSanitizer: 0 errors
  Indicates no memory errors detected.

SUMMARY: UndefinedBehaviorSanitizer: integer-overflow
  Indicates integer overflow without the builtin check.
  Fix by wrapping with __builtin_*_overflow().

---

End of Sovereign Forge Developer Guide
Complete technical reference for all contributors

