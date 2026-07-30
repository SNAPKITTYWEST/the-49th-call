/*
 * fuzz_verifier.c -- libFuzzer harness for Sovereign Stack Machine Verifier
 *
 * FORGE Phase 1, Step 3: Sanitizer Testing with libFuzzer
 *
 * libFuzzer input format:
 *   Byte 0:        opcode (0=VerifyInv, 1=VerifySol, 2=VerifyLstsq)
 *   Bytes 1-4:     n (big-endian uint32_t)
 *   Bytes 5-8:     m (big-endian uint32_t)
 *   Bytes 9+:      Matrix/vector data (int64_t values)
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "src/verifier/sov_verifier.h"

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    if (size < 10) return 0;

    uint8_t opcode = data[0];
    uint32_t n = (data[1] << 24) | (data[2] << 16) | (data[3] << 8) | data[4];
    uint32_t m = (data[5] << 24) | (data[6] << 16) | (data[7] << 8) | data[8];

    /* Cap dimensions to prevent legitimate memory exhaustion */
    if (n > 1000) n = 1000;
    if (m > 1000) m = 1000;

    size_t matrix_size = (size_t)n * (size_t)m;
    if (matrix_size * sizeof(int64_t) > size - 9) {
        return 0;
    }

    const int64_t *A = (const int64_t *)(data + 9);
    const int64_t *B = A + matrix_size;
    const int64_t *C = B + matrix_size;


    switch (opcode) {
        case 0:
            /* VerifyInv */
            if (matrix_size > 0 && n == m && n <= 32) {
                sov_verify_inv(A, n * n * sizeof(int64_t),
                               B, n * n * sizeof(int64_t),
                               n, NULL);
            }
            break;

        case 1:
            /* VerifySol */
            if (n <= m && n > 0 && m > 0 && n <= 32 && m <= 32) {
                sov_verify_sol(A, m * n * sizeof(int64_t),
                               B, n * sizeof(int64_t),
                               C, m * sizeof(int64_t),
                               m, n, NULL);
            }
            break;

        case 2:
            /* VerifyLstsq */
            if (n <= m && n > 0 && m > 0 && n <= 32 && m <= 32) {
                sov_verify_lstsq(A, m * n * sizeof(int64_t),
                                 B, n * sizeof(int64_t),
                                 C, m * sizeof(int64_t),
                                 m, n, NULL);
            }
            break;

        default:
            return 0;
    }

    return 0;
}
