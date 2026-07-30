# Sovereign Forge: Matrix Verification with Exact Arithmetic

**Sovereign Forge (FORGE)** is a production-grade verifier for matrix computations with exact int64_t arithmetic, cryptographic proofs, and tamper-evident receipts. It proves that A*X = I, A*x = b, and A^T(A*x - b) = 0 hold exactly without floating-point approximation or unverifiable claims.

## Quick Start

```bash
git clone https://github.com/SNAPKITTYWEST/sov-kernel-monster
cd sov-kernel-monster
make clean && make all
make test-asan
```

```c
#include "src/verifier/sov_verifier.h"
int main(void) {
    int64_t I[] = {1, 0, 0, 1};
    VerifyResult res = sov_verify_inv(I, 4, I, 4, 2, NULL);
    return (res == VER_OK) ? 0 : 1;
}
```

## Core Verifications

- `sov_verify_inv(A, A_inv, n)` — Prove A*A_inv = I
- `sov_verify_sol(A, x, b, m, n)` — Prove A*x = b
- `sov_verify_lstsq(A, x, b, m, n)` — Prove A^T(A*x - b) = 0

## Five Layers

**Layer 1: Stack Machine** - Exact int64_t arithmetic, overflow detection
**Layer 2: Type System** - Type inference with formal judgments (Γ ⊢ i ↓ τ)
**Layer 3: Obligations** - Generate verification constraints
**Layer 4: Certificates** - RFC 8949 CBOR encoding, SHA-256 + Ed25519
**Layer 5: WORM Receipt** - Immutable ledger, timestamp, authority

## Status

**v1.0.0: Production Ready**
- All 12 core loop invariants proven (Agda, 100% coverage)
- 100+ conformance tests passing
- AddressSanitizer + UBSan validation complete
- libFuzzer fuzzing (10+ seconds)
- Reproducible builds

## License

Dual-licensed: Apache 2.0 + AGPL 3.0

**Design:** Ahmad Ali Parr
**Implementation:** Jessica  
**AI:** Claude (Anthropic)

---

See [USER_GUIDE.md](USER_GUIDE.md) and [DEVELOPER.md](DEVELOPER.md) for complete documentation.
