# Sovereign Forge User Guide

**Sovereign Forge** is a cryptographic verification engine for the Sovereign Stack Machine. It provides zero-tolerance matrix verification, proof certificate generation, and WORM receipt signing for deterministic mathematical invariants.

All computations use `int64_t` arithmetic with overflow detection via `__builtin_*_overflow`. **No floating-point tolerances. No unverifiable claims. Exact match verification only.**

---

## Table of Contents

1. [Installation & Setup](#installation--setup)
2. [Quick Start Examples](#quick-start-examples)
3. [Working with Certificates](#working-with-certificates)
4. [Receipt Signing & Provenance](#receipt-signing--provenance)
5. [Error Handling](#error-handling)
6. [Performance Characteristics](#performance-characteristics)
7. [Troubleshooting](#troubleshooting)
8. [Integration Patterns](#integration-patterns)
9. [API Reference](#api-reference)

---

## Installation & Setup

### System Requirements

- **Compiler**: GCC 9+ or Clang 10+ with `__builtin_*_overflow` support
- **OS**: 64-bit Linux, macOS, or WSL (Windows Subsystem for Linux)
- **Build tools**: GNU Make 4.2+
- **Standard library**: POSIX-compliant C library (glibc 2.28+ or equivalent)
- **Optional**: AddressSanitizer/UBSan (GCC/Clang runtime libraries)

### Build from Source

**1. Clone the repository**

```bash
git clone https://github.com/SNAPKITTYWEST/bobs-control-repo.git
cd bobs-control-repo
```

**2. Build the library**

```bash
make clean
make
```

This creates `libsov_forge.a` (static library) and object files in `src/`.

**3. Run tests**

```bash
make test
```

Expected output:
```
[PASS] test_verify_inv_identity_2x2
[PASS] test_verify_inv_inverse_2x2
[PASS] test_verify_sol_2x2_system
[PASS] test_verify_lstsq_perfect_system
...
All conformance tests passed.
```

### Optional: Build with Sanitizers

For memory safety and undefined behavior detection:

```bash
make clean
make SANITIZE=1
```

This enables `-fsanitize=address -fsanitize=undefined` (adds ~30% overhead to verification time).

For fuzzing with libFuzzer:

```bash
make clean
make FUZZ=1
```

Creates `fuzz_verifier` target. Run it with:

```bash
./fuzz_verifier -max_len=1024 -timeout=5 corpus/
```

---

## Quick Start Examples

### Example 1: Verify 2×2 Matrix Inversion

Verify that `A * X = I` (identity matrix).

```c
#include <stdio.h>
#include <stdint.h>
#include "src/verifier/sov_verifier.h"

int main(void) {
    /* Matrix A = [[2, 1], [1, 1]] */
    int64_t A[4] = {2, 1, 1, 1};
    
    /* Expected inverse: X = [[1, -1], [-1, 2]] */
    int64_t X[4] = {1, -1, -1, 2};
    
    /* Verify A*X = I with dimension n=2 */
    VerifyResult result = sov_verify_inv(
        A, 4*sizeof(int64_t),    /* Matrix A, buffer size */
        X, 4*sizeof(int64_t),    /* Matrix X, buffer size */
        2,                        /* Dimension n=2 */
        NULL                      /* Use default resource budget */
    );
    
    if (result == VER_OK) {
        printf("✓ Matrix inversion verified\n");
    } else {
        printf("✗ Verification failed: %s\n", sov_verify_result_to_string(result));
        return 1;
    }
    
    return 0;
}
```

**Compile and run:**

```bash
gcc -std=c99 -Wall -O2 example1.c src/verifier/sov_verifier.c -o example1 -lm
./example1
```

**Output:**
```
✓ Matrix inversion verified
```

---

### Example 2: Verify Linear System Solution

Verify that `A*x = b` (exact solution).

```c
#include <stdio.h>
#include <stdint.h>
#include "src/verifier/sov_verifier.h"

int main(void) {
    /* System: 2*x1 + 0*x2 = 4
     *         0*x1 + 2*x2 = 6
     *
     * Matrix A = [[2, 0], [0, 2]] (2x2)
     * Vector x = [2, 3]
     * Vector b = [4, 6]
     */
    
    int64_t A[4] = {2, 0, 0, 2};
    int64_t x[2] = {2, 3};
    int64_t b[2] = {4, 6};
    
    VerifyResult result = sov_verify_sol(
        A, 4,               /* Matrix A (2x2 = 4 elements), buffer size */
        x, 2,               /* Solution vector x, buffer size */
        b, 2,               /* RHS vector b, buffer size */
        2, 2,               /* Dimensions: m=2 rows, n=2 cols */
        NULL                /* Default budget */
    );
    
    if (result == VER_OK) {
        printf("✓ Linear system solution verified: [%ld, %ld] solves A*x = [%ld, %ld]\n",
               x[0], x[1], b[0], b[1]);
    } else {
        printf("✗ Verification failed: %s\n", sov_verify_result_to_string(result));
        return 1;
    }
    
    return 0;
}
```

---

### Example 3: Verify Least-Squares Solution

Verify that `A^T(A*x - b) = 0` (normal equations hold exactly).

```c
#include <stdio.h>
#include <stdint.h>
#include "src/verifier/sov_verifier.h"

int main(void) {
    /* Overdetermined system: 3 equations, 2 unknowns
     *
     * A = [[1, 0],      b = [1]
     *      [0, 1],           [1]
     *      [1, 1]]           [2]
     *
     * Least-squares solution: x = [1, 1]
     * Residual: A*x - b = [0, 0, 0]
     * Normal: A^T(A*x - b) = [0, 0]
     */
    
    int64_t A[6] = {1, 0, 0, 1, 1, 1};  /* 3x2 matrix (6 elements) */
    int64_t x[2] = {1, 1};
    int64_t b[3] = {1, 1, 2};
    
    VerifyResult result = sov_verify_lstsq(
        A, 6,               /* Matrix A (3x2), buffer size */
        x, 2,               /* Solution vector, buffer size */
        b, 3,               /* RHS vector, buffer size */
        3, 2,               /* Dimensions: m=3 rows, n=2 cols */
        NULL                /* Default budget */
    );
    
    if (result == VER_OK) {
        printf("✓ Least-squares solution verified\n");
        printf("  A^T(A*x - b) = 0 (exact)\n");
    } else {
        printf("✗ Verification failed: %s\n", sov_verify_result_to_string(result));
        return 1;
    }
    
    return 0;
}
```

---

### Example 4: Generate and Sign a Proof Certificate

Create a certificate and sign it with an Ed25519 keypair.

```c
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <string.h>
#include "src/verifier/sov_verifier.h"
#include "src/certificate/sov_cert.h"

int main(void) {
    /* Create certificate */
    ProofCertificate *cert = sov_cert_new();
    if (!cert) {
        printf("Failed to create certificate\n");
        return 1;
    }
    
    /* Example program (could be bytecode from stack machine) */
    uint8_t program_hash[32];
    memset(program_hash, 0xAB, 32);  /* Example hash */
    uint32_t program_size = 512;
    
    sov_cert_set_program(cert, program_hash, program_size);
    
    /* Set initial and final stacks */
    int64_t init_stack[5] = {1, 2, 3, 4, 5};
    int64_t final_stack[3] = {15, 10, 5};
    
    if (sov_cert_set_stacks(cert, init_stack, 5, final_stack, 3) < 0) {
        printf("Failed to set stacks\n");
        sov_cert_free(cert);
        return 1;
    }
    
    /* Add obligations */
    sov_cert_add_obligation(cert, OB_INV_OK, 0, 128);
    sov_cert_add_obligation(cert, OB_SOLVE_OK, 128, 256);
    
    /* Canonicalize and hash */
    if (sov_cert_canonicalize(cert) < 0) {
        printf("Failed to canonicalize certificate\n");
        sov_cert_free(cert);
        return 1;
    }
    
    if (sov_cert_hash(cert) < 0) {
        printf("Failed to hash certificate\n");
        sov_cert_free(cert);
        return 1;
    }
    
    /* Print certificate hash */
    printf("Certificate hash: ");
    for (int i = 0; i < 32; i++) {
        printf("%02x", cert->cert_hash[i]);
    }
    printf("\n");
    
    /* Create WORM receipt */
    WormReceipt *receipt = sov_receipt_new();
    if (!receipt) {
        printf("Failed to create receipt\n");
        sov_cert_free(cert);
        return 1;
    }
    
    /* Generate Ed25519 keypair (32-byte secret key) */
    uint8_t secret_key[32];
    memset(secret_key, 0x12, 32);  /* Example secret (use random in production!) */
    
    /* Sign and seal receipt */
    if (sov_receipt_seal(receipt, secret_key, cert, program_hash, RECEIPT_SUCCESS) < 0) {
        printf("Failed to seal receipt\n");
        sov_receipt_free(receipt);
        sov_cert_free(cert);
        return 1;
    }
    
    printf("✓ Receipt sealed with Ed25519 signature\n");
    printf("  Public key: ");
    for (int i = 0; i < 32; i++) {
        printf("%02x", receipt->pubkey[i]);
    }
    printf("\n");
    
    /* Clean up */
    sov_receipt_free(receipt);
    sov_cert_free(cert);
    
    return 0;
}
```

---

### Example 5: WORM Chain Receipt Linkage

Build a Write-Once-Read-Many chain of receipts.

```c
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <string.h>
#include "src/certificate/sov_cert.h"

int main(void) {
    /* Create WORM chain */
    WormChain *chain = worm_new();
    if (!chain) {
        printf("Failed to create WORM chain\n");
        return 1;
    }
    
    /* Create and append 3 receipts */
    for (int i = 0; i < 3; i++) {
        WormReceipt *receipt = sov_receipt_new();
        if (!receipt) {
            printf("Failed to create receipt %d\n", i);
            worm_free(chain);
            return 1;
        }
        
        /* Populate receipt */
        receipt->timestamp = time(NULL);
        memset(receipt->certificate_hash, i, 32);
        memset(receipt->program_hash, i + 0x10, 32);
        memset(receipt->machine_id, i + 0x20, 32);
        receipt->outcome = RECEIPT_SUCCESS;
        
        /* Append to chain with parent hash linkage */
        uint8_t node_hash[32];
        if (worm_append(chain, receipt, node_hash) < 0) {
            printf("Failed to append receipt %d to chain\n", i);
            sov_receipt_free(receipt);
            worm_free(chain);
            return 1;
        }
        
        printf("Receipt %d appended to chain, node hash: ", i);
        for (int j = 0; j < 8; j++) printf("%02x", node_hash[j]);
        printf("...\n");
        
        sov_receipt_free(receipt);
    }
    
    /* Verify chain integrity */
    if (worm_verify_chain(chain) < 0) {
        printf("✗ WORM chain verification failed (tampering detected)\n");
        worm_free(chain);
        return 1;
    }
    
    printf("✓ WORM chain verified: %llu receipts, no tampering\n", worm_count(chain));
    
    /* Get head and tail */
    WormReceipt *head = worm_head(chain);
    WormReceipt *tail = worm_tail(chain);
    
    printf("  Genesis: timestamp=%lu\n", head->timestamp);
    printf("  Tail: timestamp=%lu\n", tail->timestamp);
    
    /* Verify replay protection */
    NonceRegistry *registry = nonce_registry_new();
    WormReceipt *r = worm_head(chain);
    
    /* This would require adding nonce support to receipts */
    nonce_registry_free(registry);
    
    /* Persist chain to disk */
    if (persist_worm_chain(chain, "/tmp/worm_chain") < 0) {
        printf("Failed to persist WORM chain\n");
        worm_free(chain);
        return 1;
    }
    
    printf("✓ WORM chain persisted to /tmp/worm_chain\n");
    
    /* Clean up */
    worm_free(chain);
    
    return 0;
}
```

---

## Working with Certificates

### Creating a Certificate from a Program

Certificates encapsulate proof of correct computation. They contain:

- **Program identification**: SHA-256 hash of bytecode
- **Stack snapshots**: Initial and final stack states
- **Type environment**: Type inference results
- **Obligations**: Verification requirements
- **Evidence chain**: Audit trail of computations
- **Canonical form**: JSON representation for hashing

**Step-by-step example:**

```c
/* 1. Create empty certificate */
ProofCertificate *cert = sov_cert_new();

/* 2. Compute SHA-256 of program bytecode */
uint8_t program_hash[32];
sha256_compute(bytecode, bytecode_len, program_hash);
sov_cert_set_program(cert, program_hash, bytecode_len);

/* 3. Record initial state (stack before execution) */
int64_t init_stack[10] = {/* ... */};
sov_cert_set_stacks(cert, init_stack, 10, NULL, 0);

/* 4. Execute program... */
/* (run stack machine) */

/* 5. Record final state (stack after execution) */
int64_t final_stack[8] = {/* ... */};
sov_cert_set_stacks(cert, init_stack, 10, final_stack, 8);

/* 6. Add obligations for each verification gate */
sov_cert_add_obligation(cert, OB_INV_OK, 0, 128);      /* PC 0-127: matrix inversion */
sov_cert_add_obligation(cert, OB_SOLVE_OK, 128, 256);  /* PC 128-255: linear solve */
sov_cert_add_obligation(cert, OB_LSTSQ_OK, 256, 384);  /* PC 256-383: least-squares */

/* 7. Canonicalize (deterministic JSON) */
sov_cert_canonicalize(cert);

/* 8. Compute certificate hash */
sov_cert_hash(cert);

/* 9. Serialize to CBOR for transmission */
uint8_t *cbor_bytes;
size_t cbor_len;
sov_cert_serialize_cbor(cert, &cbor_bytes, &cbor_len);
```

### Adding Obligations and Evidence

Obligations represent verification claims. Each obligation has:

- **Kind**: Type of verification (INV, SOLVE, LSTSQ, TYPE, PROP)
- **PC range**: Program counter interval where obligation applies
- **Parameters**: Matrix/vector data needed for verification
- **Witness**: Solution filled by verifier

**Example: Add and fill obligations**

```c
/* Add three verification obligations */
uint32_t ob_inv = sov_cert_add_obligation(cert, OB_INV_OK, 100, 150);
uint32_t ob_sol = sov_cert_add_obligation(cert, OB_SOLVE_OK, 150, 200);
uint32_t ob_lsq = sov_cert_add_obligation(cert, OB_LSTSQ_OK, 200, 250);

/* Populate matrices for each obligation */
int64_t inv_matrix[16] = {/* 4x4 matrix */};
int64_t sol_matrix[12] = {/* 3x4 matrix */};
int64_t sol_b[3] = {/* ... */};

/* Verification engine fills witness fields after verification succeeds */
```

### Serializing to CBOR

CBOR (Concise Binary Object Representation, RFC 8949) provides compact, deterministic encoding.

```c
/* Serialize to CBOR */
uint8_t *cbor_bytes = NULL;
size_t cbor_len = 0;

if (sov_cert_serialize_cbor(cert, &cbor_bytes, &cbor_len) < 0) {
    printf("Serialization failed\n");
    return 1;
}

/* Write to file */
FILE *f = fopen("certificate.cbor", "wb");
fwrite(cbor_bytes, 1, cbor_len, f);
fclose(f);

printf("Serialized to %zu bytes\n", cbor_len);

/* Deserialize later */
uint8_t cbor_buffer[4096];
size_t read_len = fread(cbor_buffer, 1, sizeof(cbor_buffer), fopen("certificate.cbor", "rb"));

ProofCertificate *cert2 = sov_cert_deserialize_cbor(cbor_buffer, read_len);
```

### Computing Canonical Hash

The certificate hash is SHA-256 of the canonical JSON representation.

```c
/* Canonicalize (produces deterministic JSON) */
if (sov_cert_canonicalize(cert) < 0) {
    printf("Canonicalization failed\n");
    return 1;
}

/* Compute SHA-256 of canonical JSON */
if (sov_cert_hash(cert) < 0) {
    printf("Hashing failed\n");
    return 1;
}

/* Print the hash */
printf("Certificate hash: ");
for (int i = 0; i < 32; i++) {
    printf("%02x", cert->cert_hash[i]);
}
printf("\n");
```

**Hash properties:**
- Deterministic: same certificate always produces same hash
- Immutable: changing any field changes the hash
- Unique: collision probability < 2^-256
- WORM-sealed: once generated, can be written once to append-only log

---

## Receipt Signing & Provenance

### Generating Ed25519 Keypair

Generate a keypair for receipt signing.

```c
#include <openssl/rand.h>
#include <openssl/evp.h>

/* Generate 32-byte random secret key */
uint8_t secret_key[32];
if (RAND_bytes(secret_key, 32) != 1) {
    printf("Random generation failed\n");
    return 1;
}

/* Public key is derived from secret key by Ed25519 (handled internally) */
uint8_t public_key[32];
/* (filled by signing routine) */

printf("Secret key (store securely): ");
for (int i = 0; i < 32; i++) printf("%02x", secret_key[i]);
printf("\n");
```

**WARNING**: Never transmit, log, or persist secret keys. Use only in memory.

### Signing a Receipt

Sign a receipt with an Ed25519 keypair.

```c
WormReceipt *receipt = sov_receipt_new();

/* Fill receipt fields */
receipt->timestamp = time(NULL);
memcpy(receipt->certificate_hash, cert->cert_hash, 32);
memcpy(receipt->program_hash, program_hash, 32);
memset(receipt->machine_id, 0x42, 32);
receipt->outcome = RECEIPT_SUCCESS;

/* Sign with secret key (32 bytes) */
uint8_t secret_key[32] = {/* ... */};

if (sov_receipt_seal(receipt, secret_key, cert, program_hash, RECEIPT_SUCCESS) < 0) {
    printf("Signing failed\n");
    sov_receipt_free(receipt);
    return 1;
}

printf("✓ Receipt signed\n");
printf("  Public key: ");
for (int i = 0; i < 32; i++) {
    printf("%02x", receipt->pubkey[i]);
}
printf("\n");
printf("  Signature: ");
for (int i = 0; i < 64; i++) {
    printf("%02x", receipt->signature[i]);
}
printf("\n");
```

### Verifying Signatures

Verify a receipt was signed by the claimed public key.

```c
/* Verify embedded signature */
if (sov_receipt_verify(receipt) < 0) {
    printf("Signature verification failed\n");
    return 1;
}

printf("✓ Signature verified\n");

/* Or verify with explicit public key */
uint8_t public_key[32] = {/* ... */};
if (verify_signature(receipt, public_key, receipt->signature) < 0) {
    printf("Verification failed\n");
    return 1;
}
```

**Signature scheme:**
- **Algorithm**: Ed25519 (RFC 8032)
- **Message**: `certificate_hash || program_hash || machine_id || outcome`
- **Deterministic**: Same inputs always produce same signature (no nonce)
- **Verifiable**: Public key uniquely identifies signer

### WORM Chain Validation

Validate an entire chain of receipts for tampering.

```c
/* Load WORM chain from disk */
WormChain *chain = load_worm_chain("/tmp/worm_chain");

/* Verify chain integrity */
if (worm_verify_chain(chain) < 0) {
    printf("✗ Chain verification failed: tampering detected\n");
    worm_free(chain);
    return 1;
}

printf("✓ WORM chain verified\n");
printf("  Total receipts: %llu\n", worm_count(chain));

/* Validate each receipt */
for (uint64_t i = 0; i < worm_count(chain); i++) {
    WormReceipt *r = worm_get_receipt(chain, i);
    
    if (sov_receipt_verify(r) < 0) {
        printf("  Receipt %llu: ✗ Signature invalid\n", i);
        worm_free(chain);
        return 1;
    }
    
    printf("  Receipt %llu: ✓ Signature valid, timestamp=%lu\n", i, r->timestamp);
}

worm_free(chain);
```

---

## Error Handling

### All VerifyResult Codes

```c
typedef enum {
    VER_OK,                     /* Verification passed */
    VER_FAIL,                   /* Verification failed (invariant not satisfied) */
    VER_OVERFLOW,               /* Arithmetic overflow detected in int64_t */
    VER_DIMENSION_MISMATCH,     /* Matrix dimensions don't match operation */
    VER_NULL_INPUT,             /* Null pointer passed */
    VER_SINGULAR,               /* Matrix is singular (for inversion) */
    VER_ALLOC_FAILURE,          /* Memory allocation failed */
    VER_BUFFER_OVERFLOW,        /* Declared dimensions exceed buffer capacity */
    VER_DIMS_EXCEEDED,          /* Matrix dimension > 65536 */
    VER_CELLS_EXCEEDED,         /* Total cells > 268M (65536^2) */
    VER_OPS_EXCEEDED,           /* Operation budget exceeded (100M ops) */
    VER_RESOURCE_EXCEEDED,      /* Generic resource limit exceeded */
} VerifyResult;
```

### Safe Allocation Patterns

**Always check for NULL and validate buffer sizes:**

```c
/* Pattern 1: Use safe allocation */
SafeMatrix *A = safe_alloc_matrix(1000, 1000);
if (!A) {
    printf("Allocation failed: matrix would require %llu bytes\n",
           1000ULL * 1000 * sizeof(int64_t));
    return 1;
}

/* Pattern 2: Validate caller-provided buffer */
int64_t user_buffer[256];  /* User's buffer, 256 elements */

VerifyResult res = validate_matrix_buffer(
    user_buffer,    /* Data pointer */
    16, 16,         /* Declared 16x16 = 256 elements */
    256             /* Actual buffer capacity */
);

if (res != VER_OK) {
    printf("Buffer validation failed: %s\n", sov_verify_result_to_string(res));
    return 1;
}

/* Pattern 3: Resource budgeting */
SovResourceBudget budget;
sov_init_resource_budget(&budget);
budget.max_dimensions = 1000;     /* Limit matrix size */
budget.max_operations = 50000000;  /* Limit to 50M operations */

VerifyResult res = sov_verify_inv(A, A_size, X, X_size, n, &budget);
if (res == VER_OPS_EXCEEDED) {
    printf("Operation budget exceeded\n");
    return 1;
}
```

### Resource Limits

```c
#define SOV_MAX_MATRIX_DIM 65536              /* Max n for n×n */
#define SOV_MAX_MATRIX_CELLS 268435456        /* Max total cells (65K^2) */
#define SOV_MAX_OPERATIONS 1000000000         /* Max accumulation ops */
#define SOV_OPERATION_BUDGET_PER_CALL 100000000  /* Per-call budget */
```

**Enforced at verification time:**

```c
/* Dimension check */
if (n > SOV_MAX_MATRIX_DIM) {
    /* Result: VER_DIMS_EXCEEDED */
}

/* Cell count check */
if (rows * cols > SOV_MAX_MATRIX_CELLS) {
    /* Result: VER_CELLS_EXCEEDED */
}

/* Operation budget check */
SovResourceBudget budget;
sov_init_resource_budget(&budget);
if (budget.operation_count > budget.max_operations) {
    /* Result: VER_OPS_EXCEEDED */
}
```

---

## Performance Characteristics

### Benchmark Summary

| Operation | Input size | Time | Memory |
| --- | --- | --- | --- |
| `sov_verify_inv` | n=16 (256 elements) | <100µs | ~16KB |
| `sov_verify_inv` | n=128 (16K elements) | ~2ms | ~1.3MB |
| `sov_verify_sol` | m=64, n=16 (1K elements) | <200µs | ~128KB |
| `sov_verify_lstsq` | m=64, n=16 | <200µs | ~256KB |
| `sov_matrix_mult` | A(64×64) × B(64×64) | ~1ms | ~2MB |
| `sov_verify_inv` | n=256 (65K elements) | ~35ms | ~8MB |

### Memory Usage

- **Temporary buffers**: O(n²) for matrix operations
- **Result buffers**: O(m×p) for multiplication output
- **Allocator overhead**: ~10% for malloc/free bookkeeping
- **No heap fragmentation**: All allocations freed immediately after verification

### Overflow Detection Overhead

Using `__builtin_*_overflow` adds <2% time overhead on modern CPUs (GCC/Clang optimize to single CPU overflow flag check).

### Sanitizer Impact

AddressSanitizer (`-fsanitize=address`) adds:
- **Time overhead**: +25-30% (shadow memory checks on each load/store)
- **Memory overhead**: +3x (shadow memory map)
- **Compile time**: +50% (instrumentation)

Disable sanitizers in production builds:

```bash
make clean
make SANITIZE=0
```

---

## Troubleshooting

### "Segmentation fault" during verification

**Cause**: Buffer size mismatch.

**Solution**: Always validate before calling:

```c
/* Validate caller's buffer can hold n×n matrix */
VerifyResult res = validate_matrix_buffer(A, n, n, A_capacity);
if (res != VER_OK) {
    printf("Buffer validation failed: %s\n", sov_verify_result_to_string(res));
    return 1;
}
```

### "VER_OVERFLOW" on apparently valid input

**Cause**: int64_t intermediate product exceeds [-2^63, 2^63-1).

**Example**:
```c
int64_t A[] = {2e18, 2, 3, 4};  /* First element too large */
int64_t X[] = {1, 0, 0, 1};
/* sov_verify_inv detects overflow during A*X computation */
```

**Solution**: Ensure all matrix elements satisfy:
```
-2^63 < element < 2^63 - 1
```

More conservatively:
```
-9e18 < element < 9e18
```

### "VER_DIMS_EXCEEDED" or "VER_CELLS_EXCEEDED"

**Cause**: Matrix too large.

**Solution**: Check before calling:

```c
if (n > SOV_MAX_MATRIX_DIM) {
    printf("Matrix dimension %zu exceeds max %u\n", n, SOV_MAX_MATRIX_DIM);
    return 1;
}

if (sov_check_matrix_cells(n, n) != VER_OK) {
    printf("Too many cells\n");
    return 1;
}
```

### Fuzzer timeout

**Cause**: Dimensions being tested trigger expensive computations.

**Solution**: Limit dimensions in corpus:

```bash
/* Create corpus with bounded inputs */
for i in {1..100}; do
    head -c $((RANDOM % 256)) /dev/urandom > corpus/input_$i
done

./fuzz_verifier -max_len=512 -timeout=5 corpus/
```

### "VER_BUFFER_OVERFLOW" despite valid dimensions

**Cause**: Declared dimensions don't match actual buffer size.

**Example**:
```c
int64_t buffer[16];  /* 16 elements */
int64_t A[] = {1, 2, 3, 4};  /* 4 elements */

/* Declare as 4x4 = 16, but only pass 4 elements */
VerifyResult res = sov_verify_sol(buffer, 4,  /* Wrong: should be 16 */
                                  A, 4, b, 2, 4, 4, NULL);
/* Result: VER_BUFFER_OVERFLOW */
```

**Solution**: Pass correct size:
```c
VerifyResult res = sov_verify_sol(buffer, 16, A, 4, b, 2, 4, 4, NULL);
```

### "VER_ALLOC_FAILURE" on small matrices

**Cause**: Temporary allocation overflow detection.

**Solution**: Check available memory:

```bash
free -h  /* Linux/macOS */
```

Or request smaller resource budget:
```c
SovResourceBudget budget;
sov_init_resource_budget(&budget);
budget.max_dimensions = 512;  /* Smaller max */
```

---

## Integration Patterns

### Stack Machine Verification

Use Sovereign Forge to verify stack machine operations:

```c
/*
 * Stack machine pseudo-code:
 *   1. Push matrix A
 *   2. Push matrix X
 *   3. Emit obligation: verify A*X = I
 *   4. Pop result
 */

typedef struct {
    int64_t *stack[100];
    size_t stack_top;
    ProofCertificate *cert;
} StackMachine;

void stack_emit_inv_obligation(StackMachine *sm, size_t n, uint32_t pc) {
    /* Get matrices from stack */
    int64_t *X = sm->stack[sm->stack_top - 1];
    int64_t *A = sm->stack[sm->stack_top - 2];
    
    /* Add obligation to certificate */
    sov_cert_add_obligation(sm->cert, OB_INV_OK, pc, pc + 128);
    
    /* Verify immediately */
    VerifyResult res = sov_verify_inv(A, n*n*sizeof(int64_t),
                                      X, n*n*sizeof(int64_t),
                                      n, NULL);
    
    if (res != VER_OK) {
        printf("Stack verification failed: %s\n", sov_verify_result_to_string(res));
        /* Handle error: halt execution, record failure in certificate */
    }
    
    /* Push success marker */
    sm->stack[sm->stack_top++] = (int64_t*)(uintptr_t)res;
}
```

### Remote Verification

Serialize certificate and verify on trusted verifier:

```c
/* Producer side: create and serialize certificate */
ProofCertificate *cert = sov_cert_new();
/* ... populate certificate ... */
sov_cert_canonicalize(cert);
sov_cert_hash(cert);

uint8_t *cbor_bytes;
size_t cbor_len;
sov_cert_serialize_cbor(cert, &cbor_bytes, &cbor_len);

/* Send cbor_bytes over network to verifier */

/* --- Network transmission --- */

/* Verifier side: deserialize and verify */
ProofCertificate *cert = sov_cert_deserialize_cbor(received_bytes, received_len);

/* Verify all obligations */
for (size_t i = 0; i < cert->obligations.count; i++) {
    Obligation *ob = &cert->obligations.obligations[i];
    VerifyResult res;
    
    switch (ob->kind) {
        case OB_INV_OK:
            res = sov_verify_inv(ob->params.inv_params.matrix_a,
                                 ob->params.inv_params.n * ob->params.inv_params.n * sizeof(int64_t),
                                 ob->witness,
                                 ob->witness_len * sizeof(int64_t),
                                 ob->params.inv_params.n, NULL);
            break;
        /* ... other obligation types ... */
    }
    
    if (res != VER_OK) {
        printf("Obligation %zu verification failed\n", i);
        return 1;
    }
}

printf("✓ All obligations verified on trusted verifier\n");
```

### Hardware Attestation

Link receipts to machine identity and TPM attestation:

```c
#include <openssl/evp.h>

typedef struct {
    uint8_t machine_id[32];      /* Blake3 hash of machine identity */
    uint32_t tpm_pcr[8];         /* TPM Platform Configuration Registers */
    uint64_t attestation_time;
    uint8_t attestation_sig[256]; /* TPM-signed attestation */
} MachineAttestation;

void create_attested_receipt(WormReceipt *receipt,
                             MachineAttestation *att,
                             const uint8_t *secret_key) {
    /* Fill machine ID */
    memcpy(receipt->machine_id, att->machine_id, 32);
    
    /* Sign receipt (signature implicitly covers machine_id) */
    sov_receipt_seal(receipt, secret_key, NULL, NULL, RECEIPT_SUCCESS);
    
    /* Verifier can now check:
     * 1. Receipt signature is valid
     * 2. Receipt machine_id matches attested identity
     * 3. Attestation time is recent
     * 4. Attestation signature is valid (TPM public key)
     */
}
```

---

## API Reference

### Verification Functions

#### `sov_verify_inv`

Verify matrix inversion: `A * X = I`.

```c
VerifyResult sov_verify_inv(
    const int64_t *A,       /* n×n matrix (row-major) */
    size_t A_len,           /* Buffer size in elements */
    const int64_t *X,       /* n×n matrix (row-major) */
    size_t X_len,           /* Buffer size in elements */
    size_t n,               /* Dimension */
    SovResourceBudget *budget  /* Optional resource limit */
);
```

**Preconditions:**
- `A != NULL && X != NULL`
- `A_len >= n*n && X_len >= n*n`
- `n > 0 && n <= SOV_MAX_MATRIX_DIM`

**Postconditions:**
- On `VER_OK`: `A*X = I` (element-wise, exact)
- No buffer modifications
- Resource budget unchanged or marked exceeded

**Time complexity**: O(n³)  
**Space complexity**: O(n²) temporary

---

#### `sov_verify_sol`

Verify linear system solution: `A*x = b`.

```c
VerifyResult sov_verify_sol(
    const int64_t *A,       /* m×n matrix (row-major) */
    size_t A_len,           /* Buffer size in elements */
    const int64_t *x,       /* n-element vector */
    size_t x_len,           /* Buffer size in elements */
    const int64_t *b,       /* m-element vector */
    size_t b_len,           /* Buffer size in elements */
    size_t m,               /* Rows */
    size_t n,               /* Columns */
    SovResourceBudget *budget
);
```

**Preconditions:**
- All pointers non-null
- `A_len >= m*n`, `x_len >= n`, `b_len >= m`
- `m, n > 0 && m*n <= SOV_MAX_MATRIX_CELLS`

**Postconditions:**
- On `VER_OK`: `A*x = b` (element-wise, exact)

**Time complexity**: O(m×n)  
**Space complexity**: O(m) temporary

---

#### `sov_verify_lstsq`

Verify least-squares solution: `A^T(A*x - b) = 0`.

```c
VerifyResult sov_verify_lstsq(
    const int64_t *A,
    size_t A_len,
    const int64_t *x,
    size_t x_len,
    const int64_t *b,
    size_t b_len,
    size_t m, size_t n,
    SovResourceBudget *budget
);
```

**Preconditions:** Same as `sov_verify_sol`

**Postconditions:**
- On `VER_OK`: For all i in [0, n):
  ```
  sum_j A[j*n + i] * (sum_k A[j*n + k] * x[k] - b[j]) = 0
  ```

**Time complexity**: O(m×n²)  
**Space complexity**: O(m + n) temporary

---

### Resource Budget Functions

#### `sov_init_resource_budget`

Initialize default resource budget.

```c
VerifyResult sov_init_resource_budget(SovResourceBudget *budget);
```

Sets:
- `max_dimensions = SOV_MAX_MATRIX_DIM (65536)`
- `max_cells = SOV_MAX_MATRIX_CELLS (268M)`
- `max_operations = SOV_OPERATION_BUDGET_PER_CALL (100M)`
- `operation_count = 0`
- `budget_exceeded = false`

---

#### `sov_add_operation_cost`

Track operation count against budget.

```c
VerifyResult sov_add_operation_cost(
    SovResourceBudget *budget,
    size_t cost
);
```

Called internally during matrix operations. Adds `cost` to `operation_count`. Returns `VER_OPS_EXCEEDED` if total would exceed `max_operations`.

---

### Safe Memory Functions

#### `safe_alloc_matrix`

Allocate matrix with overflow checking.

```c
SafeMatrix* safe_alloc_matrix(size_t rows, size_t cols);
```

**Returns:**
- Pointer to `SafeMatrix` on success
- `NULL` if rows×cols would overflow or allocation failed

**Must be freed with `safe_free_matrix`.**

---

#### `validate_matrix_buffer`

Validate caller-provided buffer dimensions.

```c
VerifyResult validate_matrix_buffer(
    const int64_t *data,
    size_t declared_rows,
    size_t declared_cols,
    size_t actual_capacity
);
```

**Returns:**
- `VER_OK` if `declared_rows * declared_cols <= actual_capacity`
- `VER_NULL_INPUT` if data is NULL
- `VER_BUFFER_OVERFLOW` if dimensions exceed capacity
- `VER_OVERFLOW` if multiplication overflows

---

### Helper Functions

#### `sov_matrix_vec_mult`

Compute `A*x = result` (m-element result).

```c
VerifyResult sov_matrix_vec_mult(
    const int64_t *A,       /* m×n matrix */
    const int64_t *x,       /* n-element vector */
    int64_t *result,        /* m-element output */
    size_t m, size_t n
);
```

---

#### `sov_matrix_mult`

Compute `A*B = result` (m×p result).

```c
VerifyResult sov_matrix_mult(
    const int64_t *A,       /* m×n */
    const int64_t *B,       /* n×p */
    int64_t *result,        /* m×p output */
    size_t m, size_t n, size_t p
);
```

---

#### `sov_matrix_transpose`

Compute `A^T = result`.

```c
VerifyResult sov_matrix_transpose(
    const int64_t *A,       /* m×n */
    int64_t *result,        /* n×m output */
    size_t m, size_t n
);
```

---

#### `sov_matrix_equal`

Check if two matrices are element-wise equal.

```c
bool sov_matrix_equal(
    const int64_t *A,
    const int64_t *B,
    size_t nelems
);
```

---

#### `sov_verify_result_to_string`

Convert error code to human-readable string.

```c
const char *sov_verify_result_to_string(VerifyResult r);
```

Returns static string like `"VER_OK"`, `"VER_OVERFLOW"`, etc.

---

### Certificate Functions

#### `sov_cert_new` / `sov_cert_free`

Create and destroy certificates.

```c
ProofCertificate *sov_cert_new(void);
void sov_cert_free(ProofCertificate *cert);
```

---

#### `sov_cert_set_program`

Set program identification.

```c
void sov_cert_set_program(
    ProofCertificate *cert,
    const uint8_t *program_hash,  /* 32-byte SHA-256 */
    uint32_t program_size
);
```

---

#### `sov_cert_add_obligation`

Add verification obligation.

```c
int sov_cert_add_obligation(
    ProofCertificate *cert,
    ObligationKind kind,
    uint32_t start_pc,
    uint32_t end_pc
);
```

**Kinds:**
- `OB_INV_OK`: Matrix inversion
- `OB_SOLVE_OK`: Linear system solution
- `OB_LSTSQ_OK`: Least-squares solution
- `OB_TYPE_OK`: Type inference
- `OB_PROP_OK`: Property hold

---

#### `sov_cert_canonicalize` / `sov_cert_hash`

Canonicalize and hash certificate.

```c
int sov_cert_canonicalize(ProofCertificate *cert);
int sov_cert_hash(ProofCertificate *cert);
```

**Must call `canonicalize` before `hash`.**

After successful call, `cert->cert_hash` contains 32-byte SHA-256 digest.

---

#### `sov_cert_serialize_cbor` / `sov_cert_deserialize_cbor`

CBOR serialization.

```c
int sov_cert_serialize_cbor(
    ProofCertificate *cert,
    uint8_t **out_bytes,
    size_t *out_len
);

ProofCertificate *sov_cert_deserialize_cbor(
    const uint8_t *bytes,
    size_t len
);
```

---

### Receipt Functions

#### `sov_receipt_new` / `sov_receipt_free`

Create and destroy receipts.

```c
WormReceipt *sov_receipt_new(void);
void sov_receipt_free(WormReceipt *receipt);
```

---

#### `sov_receipt_seal` / `sov_receipt_verify`

Sign and verify receipts.

```c
int sov_receipt_seal(
    WormReceipt *receipt,
    const uint8_t *secret_key,  /* 32-byte Ed25519 */
    ProofCertificate *cert,
    const uint8_t *program_hash,
    ReceiptOutcome outcome
);

int sov_receipt_verify(const WormReceipt *receipt);
```

---

### WORM Chain Functions

#### `worm_new` / `worm_free`

Create and destroy WORM chains.

```c
WormChain *worm_new(void);
void worm_free(WormChain *w);
```

---

#### `worm_append`

Add receipt to chain with cryptographic linkage.

```c
int worm_append(
    WormChain *w,
    WormReceipt *receipt,
    uint8_t node_hash[32]  /* Output: Blake3 hash of node */
);
```

Each appended receipt is cryptographically linked to previous nodes. Modifying any node invalidates all subsequent hashes.

---

#### `worm_verify_chain`

Verify entire chain for tampering.

```c
int worm_verify_chain(WormChain *w);
```

Returns 0 if chain is valid (no tampering detected), -1 if any hash mismatch.

---

#### `worm_count` / `worm_head` / `worm_tail`

Query chain.

```c
uint64_t worm_count(WormChain *w);
WormReceipt *worm_head(WormChain *w);  /* First receipt */
WormReceipt *worm_tail(WormChain *w);  /* Last receipt */
```

---

### Persistence Functions

#### `persist_receipt` / `load_receipt`

Save and load individual receipts.

```c
int persist_receipt(WormReceipt *receipt, const char *path);
WormReceipt *load_receipt(const char *path);
```

---

#### `persist_worm_chain` / `load_worm_chain`

Save and load entire chains.

```c
int persist_worm_chain(WormChain *w, const char *dir);
WormChain *load_worm_chain(const char *dir);
```

Chains are persisted as a directory of receipt files with hash linkage metadata.

---

## Example: Complete End-to-End Workflow

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "src/verifier/sov_verifier.h"
#include "src/certificate/sov_cert.h"

int main(void) {
    printf("=== Sovereign Forge End-to-End Demo ===\n\n");
    
    /* 1. Create certificate */
    ProofCertificate *cert = sov_cert_new();
    
    /* 2. Set program identification */
    uint8_t program_hash[32];
    memset(program_hash, 0xAA, 32);
    sov_cert_set_program(cert, program_hash, 1024);
    
    /* 3. Verify matrix inversion and record in certificate */
    int64_t A[4] = {2, 1, 1, 1};
    int64_t X[4] = {1, -1, -1, 2};
    
    VerifyResult res = sov_verify_inv(A, 4*sizeof(int64_t),
                                      X, 4*sizeof(int64_t), 2, NULL);
    if (res != VER_OK) {
        printf("Inversion verification failed\n");
        sov_cert_free(cert);
        return 1;
    }
    printf("✓ Inversion verified\n");
    
    /* 4. Add obligation to certificate */
    sov_cert_add_obligation(cert, OB_INV_OK, 0, 128);
    
    /* 5. Record stack state */
    int64_t init_stack[4] = {2, 1, 1, 1};
    int64_t final_stack[4] = {1, -1, -1, 2};
    sov_cert_set_stacks(cert, init_stack, 4, final_stack, 4);
    
    /* 6. Canonicalize and hash certificate */
    if (sov_cert_canonicalize(cert) < 0 || sov_cert_hash(cert) < 0) {
        printf("Certificate preparation failed\n");
        sov_cert_free(cert);
        return 1;
    }
    printf("✓ Certificate generated, hash: ");
    for (int i = 0; i < 8; i++) printf("%02x", cert->cert_hash[i]);
    printf("...\n");
    
    /* 7. Create and sign receipt */
    WormReceipt *receipt = sov_receipt_new();
    uint8_t secret_key[32];
    memset(secret_key, 0x42, 32);
    
    if (sov_receipt_seal(receipt, secret_key, cert, program_hash, RECEIPT_SUCCESS) < 0) {
        printf("Receipt sealing failed\n");
        sov_receipt_free(receipt);
        sov_cert_free(cert);
        return 1;
    }
    printf("✓ Receipt sealed and signed\n");
    
    /* 8. Create WORM chain and append receipt */
    WormChain *chain = worm_new();
    uint8_t node_hash[32];
    
    if (worm_append(chain, receipt, node_hash) < 0) {
        printf("WORM chain append failed\n");
        worm_free(chain);
        sov_receipt_free(receipt);
        sov_cert_free(cert);
        return 1;
    }
    printf("✓ Receipt appended to WORM chain\n");
    
    /* 9. Verify WORM chain */
    if (worm_verify_chain(chain) < 0) {
        printf("WORM chain verification failed\n");
        worm_free(chain);
        sov_receipt_free(receipt);
        sov_cert_free(cert);
        return 1;
    }
    printf("✓ WORM chain verified (%llu receipt(s))\n", worm_count(chain));
    
    /* 10. Persist to disk */
    if (persist_worm_chain(chain, "/tmp/sovereign_forge_demo") < 0) {
        printf("Persistence failed\n");
        worm_free(chain);
        sov_receipt_free(receipt);
        sov_cert_free(cert);
        return 1;
    }
    printf("✓ Chain persisted to /tmp/sovereign_forge_demo\n");
    
    /* 11. Load and verify from disk */
    WormChain *loaded_chain = load_worm_chain("/tmp/sovereign_forge_demo");
    if (!loaded_chain || worm_verify_chain(loaded_chain) < 0) {
        printf("Loaded chain verification failed\n");
        worm_free(chain);
        sov_receipt_free(receipt);
        sov_cert_free(cert);
        return 1;
    }
    printf("✓ Loaded chain verified from disk\n\n");
    
    printf("=== Demo Complete ===\n");
    
    /* Clean up */
    worm_free(loaded_chain);
    worm_free(chain);
    sov_receipt_free(receipt);
    sov_cert_free(cert);
    
    return 0;
}
```

---

## Summary

Sovereign Forge provides:

✓ **Zero-tolerance verification**: Exact match only, no floating-point tolerances  
✓ **Overflow-safe**: All int64_t operations checked with `__builtin_*_overflow`  
✓ **Resource-budgeted**: Operation limits prevent denial of service  
✓ **CBOR-serializable**: Compact, deterministic binary format (RFC 8949)  
✓ **Cryptographically sealed**: Ed25519 signatures and SHA-256 hashing  
✓ **WORM-enabled**: Write-once-read-many chains with tamper detection  
✓ **Production-hardened**: AddressSanitizer, UBSan, and libFuzzer support  

**For production deployment**, always:
1. Build with `-O3` and without sanitizers
2. Validate all input dimensions and buffer sizes
3. Check `VerifyResult` codes before proceeding
4. Persist receipts to append-only storage
5. Verify WORM chains periodically to detect tampering

---

**Last Updated**: 2026-07-29  
**License**: Apache 2.0 + AGPL 3.0
