/*
 * performance_bench.c -- SENTINEL Phase 3 Performance Benchmark Suite
 *
 * Comprehensive performance measurements for the Sovereign Stack Machine Verifier.
 * Measures latency and throughput for all three core verification functions.
 *
 * License: Apache 2.0 + AGPL 3.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <math.h>
#include "src/verifier/sov_verifier.h"

#define ITERATIONS_SMALL 100000
#define ITERATIONS_MEDIUM 10000
#define ITERATIONS_LARGE 1000

typedef struct {
    const char *name;
    double min_us;
    double max_us;
    double avg_us;
    double stddev_us;
} BenchResult;

/* Measure function execution time */
static BenchResult benchmark_function(const char *name,
                                      int (*func)(void),
                                      int iterations)
{
    BenchResult result = {0};
    result.name = name;

    double *times = malloc(iterations * sizeof(double));
    if (!times) {
        printf("Memory allocation failed\n");
        exit(1);
    }

    /* Warmup */
    for (int i = 0; i < 10; i++) {
        func();
    }

    /* Measure */
    for (int i = 0; i < iterations; i++) {
        clock_t start = clock();
        func();
        clock_t end = clock();
        times[i] = ((double)(end - start)) / CLOCKS_PER_SEC * 1000000;  /* Convert to µs */
    }

    /* Analyze */
    result.min_us = times[0];
    result.max_us = times[0];
    result.avg_us = 0;

    for (int i = 0; i < iterations; i++) {
        if (times[i] < result.min_us) result.min_us = times[i];
        if (times[i] > result.max_us) result.max_us = times[i];
        result.avg_us += times[i];
    }
    result.avg_us /= iterations;

    /* Standard deviation */
    double variance = 0;
    for (int i = 0; i < iterations; i++) {
        double diff = times[i] - result.avg_us;
        variance += diff * diff;
    }
    result.stddev_us = sqrt(variance / iterations);

    free(times);
    return result;
}

/*
 * ============================================================================
 * BENCHMARK 1: VerifyInv with varying matrix sizes
 * ============================================================================
 */

static int64_t *create_identity(int n)
{
    int64_t *I = malloc(n * n * sizeof(int64_t));
    if (!I) return NULL;

    for (int i = 0; i < n * n; i++) {
        I[i] = 0;
    }
    for (int i = 0; i < n; i++) {
        I[i * n + i] = 1;
    }
    return I;
}

static int bench_verify_inv_2_wrapper(void)
{
    static int64_t *I = NULL;
    if (!I) I = create_identity(2);
    sov_verify_inv(I, I, 2);
    return 0;
}

static int bench_verify_inv_4_wrapper(void)
{
    static int64_t *I = NULL;
    if (!I) I = create_identity(4);
    sov_verify_inv(I, I, 4);
    return 0;
}

static int bench_verify_inv_8_wrapper(void)
{
    static int64_t *I = NULL;
    if (!I) I = create_identity(8);
    sov_verify_inv(I, I, 8);
    return 0;
}

static int bench_verify_inv_16_wrapper(void)
{
    static int64_t *I = NULL;
    if (!I) I = create_identity(16);
    sov_verify_inv(I, I, 16);
    return 0;
}

/*
 * ============================================================================
 * BENCHMARK 2: VerifySol with varying problem sizes
 * ============================================================================
 */

static int bench_verify_sol_2x2_wrapper(void)
{
    static int64_t A[] = {1, 0, 0, 1};
    static int64_t x[] = {1, 1};
    static int64_t b[] = {1, 1};
    sov_verify_sol(A, x, b, 2, 2);
    return 0;
}

static int bench_verify_sol_8x4_wrapper(void)
{
    static int64_t *A = NULL;
    static int64_t x[] = {1, 1, 1, 1};
    static int64_t b[] = {1, 1, 1, 1, 1, 1, 1, 1};

    if (!A) {
        A = malloc(8 * 4 * sizeof(int64_t));
        for (int i = 0; i < 8 * 4; i++) A[i] = (i % 4 == i / 8) ? 1 : 0;
    }
    sov_verify_sol(A, x, b, 8, 4);
    return 0;
}

static int bench_verify_sol_16x8_wrapper(void)
{
    static int64_t *A = NULL;
    static int64_t x[] = {1, 1, 1, 1, 1, 1, 1, 1};
    static int64_t b[16];

    if (!A) {
        A = malloc(16 * 8 * sizeof(int64_t));
        for (int i = 0; i < 16 * 8; i++) A[i] = (i % 8 == i / 16) ? 1 : 0;
        for (int i = 0; i < 16; i++) b[i] = 1;
    }
    sov_verify_sol(A, x, b, 16, 8);
    return 0;
}

/*
 * ============================================================================
 * BENCHMARK 3: VerifyLstsq with overdetermined systems
 * ============================================================================
 */

static int bench_verify_lstsq_2x2_wrapper(void)
{
    static int64_t A[] = {1, 0, 0, 1};
    static int64_t x[] = {1, 1};
    static int64_t b[] = {1, 1};
    sov_verify_lstsq(A, x, b, 2, 2);
    return 0;
}

static int bench_verify_lstsq_8x4_wrapper(void)
{
    static int64_t *A = NULL;
    static int64_t x[] = {1, 1, 1, 1};
    static int64_t b[] = {1, 1, 1, 1, 1, 1, 1, 1};

    if (!A) {
        A = malloc(8 * 4 * sizeof(int64_t));
        for (int i = 0; i < 8 * 4; i++) A[i] = (i % 4 == i / 8) ? 1 : 0;
    }
    sov_verify_lstsq(A, x, b, 8, 4);
    return 0;
}

static int bench_verify_lstsq_64x16_wrapper(void)
{
    static int64_t *A = NULL;
    static int64_t x[16];
    static int64_t b[64];

    if (!A) {
        A = malloc(64 * 16 * sizeof(int64_t));
        for (int i = 0; i < 64 * 16; i++) A[i] = (i % 16 == i / 64) ? 1 : 0;
        for (int i = 0; i < 16; i++) x[i] = 1;
        for (int i = 0; i < 64; i++) b[i] = 1;
    }
    sov_verify_lstsq(A, x, b, 64, 16);
    return 0;
}

/*
 * ============================================================================
 * MAIN BENCHMARK RUNNER
 * ============================================================================
 */

int main(void)
{
    printf("╔══════════════════════════════════════════════════════════════════╗\n");
    printf("║  PHASE 3 SENTINEL PERFORMANCE BENCHMARK                          ║\n");
    printf("║  Date: 2026-07-29                                               ║\n");
    printf("╚══════════════════════════════════════════════════════════════════╝\n");

    printf("\n");
    printf("╔══════════════════════════════════════════════════════════════════╗\n");
    printf("║  VerifyInv Performance (A*X = I, varying matrix size)             ║\n");
    printf("╚══════════════════════════════════════════════════════════════════╝\n");

    BenchResult r1 = benchmark_function("VerifyInv(n=2)", bench_verify_inv_2_wrapper, ITERATIONS_SMALL);
    printf("  n=2:   %8.2f µs (min: %.2f, max: %.2f, σ: %.2f)\n",
           r1.avg_us, r1.min_us, r1.max_us, r1.stddev_us);

    BenchResult r2 = benchmark_function("VerifyInv(n=4)", bench_verify_inv_4_wrapper, ITERATIONS_SMALL);
    printf("  n=4:   %8.2f µs (min: %.2f, max: %.2f, σ: %.2f)\n",
           r2.avg_us, r2.min_us, r2.max_us, r2.stddev_us);

    BenchResult r3 = benchmark_function("VerifyInv(n=8)", bench_verify_inv_8_wrapper, ITERATIONS_MEDIUM);
    printf("  n=8:   %8.2f µs (min: %.2f, max: %.2f, σ: %.2f)\n",
           r3.avg_us, r3.min_us, r3.max_us, r3.stddev_us);

    BenchResult r4 = benchmark_function("VerifyInv(n=16)", bench_verify_inv_16_wrapper, ITERATIONS_MEDIUM);
    printf("  n=16:  %8.2f µs (min: %.2f, max: %.2f, σ: %.2f) [TARGET: <100µs]\n",
           r4.avg_us, r4.min_us, r4.max_us, r4.stddev_us);

    printf("\n");
    printf("╔══════════════════════════════════════════════════════════════════╗\n");
    printf("║  VerifySol Performance (A*x = b, varying problem size)            ║\n");
    printf("╚══════════════════════════════════════════════════════════════════╝\n");

    BenchResult s1 = benchmark_function("VerifySol(2x2)", bench_verify_sol_2x2_wrapper, ITERATIONS_SMALL);
    printf("  2x2:   %8.2f µs (min: %.2f, max: %.2f, σ: %.2f)\n",
           s1.avg_us, s1.min_us, s1.max_us, s1.stddev_us);

    BenchResult s2 = benchmark_function("VerifySol(8x4)", bench_verify_sol_8x4_wrapper, ITERATIONS_SMALL);
    printf("  8x4:   %8.2f µs (min: %.2f, max: %.2f, σ: %.2f)\n",
           s2.avg_us, s2.min_us, s2.max_us, s2.stddev_us);

    BenchResult s3 = benchmark_function("VerifySol(16x8)", bench_verify_sol_16x8_wrapper, ITERATIONS_MEDIUM);
    printf("  16x8:  %8.2f µs (min: %.2f, max: %.2f, σ: %.2f)\n",
           s3.avg_us, s3.min_us, s3.max_us, s3.stddev_us);

    printf("\n");
    printf("╔══════════════════════════════════════════════════════════════════╗\n");
    printf("║  VerifyLstsq Performance (A^T(Ax-b)=0, varying problem size)      ║\n");
    printf("╚══════════════════════════════════════════════════════════════════╝\n");

    BenchResult l1 = benchmark_function("VerifyLstsq(2x2)", bench_verify_lstsq_2x2_wrapper, ITERATIONS_SMALL);
    printf("  2x2:   %8.2f µs (min: %.2f, max: %.2f, σ: %.2f)\n",
           l1.avg_us, l1.min_us, l1.max_us, l1.stddev_us);

    BenchResult l2 = benchmark_function("VerifyLstsq(8x4)", bench_verify_lstsq_8x4_wrapper, ITERATIONS_SMALL);
    printf("  8x4:   %8.2f µs (min: %.2f, max: %.2f, σ: %.2f)\n",
           l2.avg_us, l2.min_us, l2.max_us, l2.stddev_us);

    BenchResult l3 = benchmark_function("VerifyLstsq(64x16)", bench_verify_lstsq_64x16_wrapper, ITERATIONS_MEDIUM);
    printf("  64x16: %8.2f µs (min: %.2f, max: %.2f, σ: %.2f)\n",
           l3.avg_us, l3.min_us, l3.max_us, l3.stddev_us);

    printf("\n");
    printf("╔══════════════════════════════════════════════════════════════════╗\n");
    printf("║  PERFORMANCE SUMMARY                                             ║\n");
    printf("╚══════════════════════════════════════════════════════════════════╝\n");

    printf("\nVerifyInv Scaling:\n");
    printf("  2x2  -> 4x4  : %.1fx\n", r2.avg_us / r1.avg_us);
    printf("  4x4  -> 8x8  : %.1fx\n", r3.avg_us / r2.avg_us);
    printf("  8x8  -> 16x16: %.1fx\n", r4.avg_us / r3.avg_us);
    printf("  Expected: ~4x (since O(n³) for matrix mult)\n");

    printf("\nPerformance Targets:\n");
    if (r4.avg_us < 100) {
        printf("  ✓ VerifyInv(n≤16) meets <100µs target (%.2f µs)\n", r4.avg_us);
    } else {
        printf("  ✗ VerifyInv(n≤16) EXCEEDS <100µs target (%.2f µs)\n", r4.avg_us);
    }

    printf("\nThroughput (ops/second):\n");
    printf("  VerifyInv(n=16):     %.0f ops/sec\n", 1000000 / r4.avg_us);
    printf("  VerifySol(16x8):     %.0f ops/sec\n", 1000000 / s3.avg_us);
    printf("  VerifyLstsq(64x16):  %.0f ops/sec\n", 1000000 / l3.avg_us);

    return 0;
}
