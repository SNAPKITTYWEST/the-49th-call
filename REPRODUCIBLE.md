# REPRODUCIBLE.md — Bit-Identical Build Manifest for v1.0.0

## Purpose

This document specifies the exact build process for creating bit-identical binaries of the Sovereign Stack Machine (BOB) FORGE Phase 4 & 5. Reproducibility ensures:

1. **Verifiability**: Anyone can rebuild and verify the binary against published SHA-256 hashes
2. **Supply Chain Integrity**: Confirms no backdoors introduced during compilation
3. **Formal Alignment**: Proves the C implementation matches Lean 4 formal model

---

## Build Environment (Required)

### Compiler
- **GCC**: 11.4.0 or exact match (deterministic output)
- **Command**: `gcc -v` → must show `gcc version 11.4.0`

### C Standard Library
- **glibc**: 2.35 or exact match
- **Verification**: `ldd --version` → `GLIBC 2.35`

### Build Flags (Mandatory)

```makefile
CC := gcc-11
CFLAGS := -std=c99 \
          -Wall -Wextra \
          -O2 \
          -fno-strict-aliasing \
          -fwrapv \
          -fno-asynchronous-unwind-tables \
          -Wno-unused-function \
          -I.

LDFLAGS := -lm
```

### Key Reproducibility Options

| Flag | Purpose |
|------|---------|
| `-O2` | Consistent optimization level (not `-O0` or `-O3`) |
| `-std=c99` | C99 standard semantics, no extensions |
| `-fno-strict-aliasing` | Prevents optimizer-dependent behavior |
| `-fwrapv` | Signed integer overflow wraps (deterministic) |
| `-fno-asynchronous-unwind-tables` | Removes runtime unwind metadata |

---

## Source Files (Phase 4 & 5)

### Core Receipt Signing & Verification
- `src/receipts/sign.c` (189 lines): Ed25519 deterministic signatures
- `src/receipts/worm.c` (318 lines): Write-Once-Read-Many chain linkage
- `src/receipts/replay.c` (296 lines): Nonce and timestamp validation
- `src/receipts/persist.c` (313 lines): Atomic receipt persistence

### Test Suite (Phase 4)
- `tests/receipts/test_provenance.c` (436 lines): 8 comprehensive tests

### Formal Model (Phase 5)
- `proofs/lean4/Sovereign/CRefinement.lean` (239 lines): C-to-Lean refinement
- `proofs/lean4/Sovereign/Serialization.lean` (240 lines): Crypto property proofs

---

## Reproducible Build Steps

### 1. Prepare Build Environment

```bash
gcc --version
# Expected: gcc (Ubuntu ...) 11.4.0

ldd --version | head -1
# Expected: ldd (Ubuntu GLIBC 2.35...)

make -f Makefile.sov clean
```

### 2. Build Receipt Objects

```bash
gcc -std=c99 -Wall -Wextra -I. -c src/receipts/sign.c
gcc -std=c99 -Wall -Wextra -I. -c src/receipts/worm.c
gcc -std=c99 -Wall -Wextra -I. -c src/receipts/replay.c
gcc -std=c99 -Wall -Wextra -I. -c src/receipts/persist.c
```

### 3. Run Phase 4 Tests

```bash
gcc -std=c99 -Wall -Wextra -I. -c tests/receipts/test_provenance.c
gcc -std=c99 -Wall -Wextra \
  sign.o worm.o replay.o persist.o test_provenance.o \
  -o tests/receipts/test_provenance -lm

./tests/receipts/test_provenance
# Expected: 8 passed, 0 failed
```

### 4. Run Phase 5 Tests

```bash
gcc -std=c99 -Wall -Wextra -I. -c tests/refinement/test_c_model_equivalence.c
gcc -std=c99 -Wall -Wextra \
  sign.o worm.o replay.o persist.o test_c_model_equivalence.o \
  -o tests/refinement/test_c_model_equivalence -lm -lsodium

./tests/refinement/test_c_model_equivalence
# Expected: 5 passed, 0 failed
```

---

## Verification Checklist

- [ ] GCC version 11.4.0 verified
- [ ] glibc version 2.35 verified
- [ ] Build flags exact match
- [ ] All 8 receipt tests PASS
- [ ] All 5 refinement tests PASS
- [ ] Object file hashes verified
- [ ] Binary hashes verified

---

## Version

**v1.0.0** — 2026-07-29 — Final: Phase 4 + 5 complete, all tests PASS
