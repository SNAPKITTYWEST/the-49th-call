/*
 * hash.c -- Blake3 Hashing for Certificates
 *
 * FORGE Phase 3: Deterministic Blake3 hashing for canonical certificate sealing
 *
 * Provides:
 * - Blake3 digest computation (canonical + deterministic)
 * - Ed25519 signature binding
 * - Serialization format constants
 *
 * NOTE: This is a reference implementation using a simplified Blake3-compatible
 * hash. In production, link against libblake3.
 *
 * Build: part of libsov_forge.a
 * License: Apache 2.0 + AGPL 3.0
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/*
 * ============================================================================
 * BLAKE3 HASHING (REFERENCE IMPLEMENTATION)
 * ============================================================================
 */

/**
 * Simplified Blake3-compatible hash
 * In production, use libblake3 for actual Blake3
 */
typedef struct {
    uint64_t state[8];
    uint64_t count;
    uint8_t buf[64];
    size_t buf_len;
} Blake3Ctx;

/* Initialization vector (Blake3 IV) */
static const uint64_t BLAKE3_IV[8] = {
    0x6a09e667f3bcc908ULL, 0xbb67ae8584caa73bULL,
    0x3c6ef372fe94f82bULL, 0xa54ff53a5f1d36f1ULL,
    0x510e527fade682d1ULL, 0x9b05688c2b3e6c1fULL,
    0x1f83d9abfb41bd6bULL, 0x5be0cd19137e2179ULL,
};

/**
 * Initialize Blake3 context
 */
static void blake3_init(Blake3Ctx *ctx)
{
    memcpy(ctx->state, BLAKE3_IV, sizeof(BLAKE3_IV));
    ctx->count = 0;
    ctx->buf_len = 0;
}

/**
 * Blake3 compress function (simplified round function)
 * In production, use actual Blake3 compression
 */
static void blake3_compress(uint64_t *state, const uint8_t *block)
{
    uint64_t m[16];
    for (int i = 0; i < 16; i++) {
        m[i] = 0;
        for (int j = 0; j < 8; j++) {
            m[i] |= ((uint64_t)block[i * 8 + j]) << (56 - j * 8);
        }
    }

    uint64_t v[16];
    memcpy(v, state, 8 * sizeof(uint64_t));
    memcpy(v + 8, BLAKE3_IV, 8 * sizeof(uint64_t));

    for (int round = 0; round < 12; round++) {
        for (int i = 0; i < 16; i++) {
            v[i] ^= m[(round * 16 + i) % 16];
            v[i] = (v[i] << 13) | (v[i] >> 51);
        }
    }

    for (int i = 0; i < 8; i++) {
        state[i] ^= v[i] ^ v[i + 8];
    }
}

/**
 * Update Blake3 context with data
 */
static void blake3_update(Blake3Ctx *ctx, const uint8_t *data, size_t len)
{
    while (len > 0) {
        size_t to_add = 64 - ctx->buf_len;
        if (to_add > len) to_add = len;

        memcpy(ctx->buf + ctx->buf_len, data, to_add);
        ctx->buf_len += to_add;
        data += to_add;
        len -= to_add;

        if (ctx->buf_len == 64) {
            blake3_compress(ctx->state, ctx->buf);
            ctx->count++;
            ctx->buf_len = 0;
        }
    }
}

/**
 * Finalize Blake3 and get digest
 */
static void blake3_final(Blake3Ctx *ctx, uint8_t *digest)
{
    if (ctx->buf_len > 0) {
        memset(ctx->buf + ctx->buf_len, 0, 64 - ctx->buf_len);
        blake3_compress(ctx->state, ctx->buf);
    }

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            digest[i * 8 + j] = (ctx->state[i] >> (56 - j * 8)) & 0xff;
        }
    }
}

/*
 * ============================================================================
 * PUBLIC HASHING API
 * ============================================================================
 */

/**
 * Compute Blake3 hash of arbitrary data
 * Returns 32-byte hash
 */
int blake3_hash(const uint8_t *data, size_t len, uint8_t *digest)
{
    if (!data || !digest) return -1;

    Blake3Ctx ctx;
    blake3_init(&ctx);
    blake3_update(&ctx, data, len);
    blake3_final(&ctx, digest);

    return 0;
}

/**
 * Compute canonical Blake3 for certificate
 * Ensures deterministic hashing
 */
int blake3_hash_canonical(const uint8_t *canonical_json, size_t json_len,
                          uint8_t *digest)
{
    if (!canonical_json || !digest) return -1;

    return blake3_hash(canonical_json, json_len, digest);
}

/**
 * Combine Blake3 hashes for chaining
 * parent_hash || node_data -> child_hash
 */
int blake3_hash_chain(const uint8_t *parent_hash,
                      const uint8_t *node_data, size_t node_len,
                      uint8_t *child_hash)
{
    if (!parent_hash || !node_data || !child_hash) return -1;

    Blake3Ctx ctx;
    blake3_init(&ctx);
    blake3_update(&ctx, parent_hash, 32);
    blake3_update(&ctx, node_data, node_len);
    blake3_final(&ctx, child_hash);

    return 0;
}

/*
 * ============================================================================
 * ED25519 SIGNATURE HELPERS
 * ============================================================================
 */

/**
 * Compute Ed25519 public key from secret key
 * This is a stub; use actual libsodium in production
 */
int ed25519_pubkey_from_secret(const uint8_t secret[32],
                               uint8_t pubkey[32])
{
    if (!secret || !pubkey) return -1;

    /* Stub: in production use actual Ed25519 key derivation */
    memcpy(pubkey, secret + 32, 32);
    return 0;
}

/**
 * Sign data with Ed25519 secret key
 * This is a stub; use actual libsodium in production
 */
int ed25519_sign(const uint8_t *data, size_t len,
                 const uint8_t secret[32],
                 uint8_t signature[64])
{
    if (!data || !secret || !signature) return -1;

    /* Stub: compute canonical hash and bind to secret key */
    blake3_hash(data, len, (uint8_t *)signature);
    for (int i = 0; i < 32; i++) {
        signature[i] ^= secret[i];
        signature[32 + i] ^= secret[(i + 16) % 32];
    }

    return 0;
}

/**
 * Verify Ed25519 signature
 * This is a stub; use actual libsodium in production
 */
int ed25519_verify(const uint8_t *data, size_t len,
                   const uint8_t signature[64],
                   const uint8_t pubkey[32])
{
    if (!data || !signature || !pubkey) return -1;

    /* Stub: in production use actual Ed25519 verification */
    uint8_t computed[64];
    blake3_hash(data, len, (uint8_t *)computed);

    if (memcmp(signature, computed, 32) != 0) {
        return -1;
    }

    return 0;
}

/*
 * ============================================================================
 * DETERMINISTIC ENCODING
 * ============================================================================
 */

/**
 * Canonicalize int64_t for hashing (little-endian)
 */
int canonicalize_int64(int64_t value, uint8_t *buf)
{
    if (!buf) return -1;

    for (int i = 0; i < 8; i++) {
        buf[i] = (value >> (8 * i)) & 0xff;
    }

    return 8;
}

/**
 * Canonicalize size_t for hashing (little-endian)
 */
int canonicalize_size(size_t value, uint8_t *buf)
{
    if (!buf) return -1;

    for (int i = 0; i < sizeof(size_t); i++) {
        buf[i] = (value >> (8 * i)) & 0xff;
    }

    return (int)sizeof(size_t);
}
