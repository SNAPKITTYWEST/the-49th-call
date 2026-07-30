/*
 * sov_cert.h -- Sovereign Stack Machine Certificate Structure
 *
 * Implements RFC 8949 CBOR-compatible proof certificates with SHA-256 sealing.
 * NO floating-point tolerances. NO unverifiable claims. All int64_t with overflow detection.
 *
 * FORGE Phase 2: Proof Certificate Generator
 *
 * Build: part of libsov_forge.a
 * License: Apache 2.0 + AGPL 3.0
 */

#ifndef SOV_CERT_H
#define SOV_CERT_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * ============================================================================
 * CERTIFICATE STRUCTURES
 * ============================================================================
 */

/* Import Obligation and ObligationSet from sov_obligations.h */
#include "src/obligations/sov_obligations.h"

/* Map certificate obligation kinds to internal kinds */
typedef enum {
    OB_INV_OK = OBL_KIND_INV,      /* Invariant verification required: A*X = I */
    OB_SOLVE_OK = OBL_KIND_SOLVE,  /* Linear system solution required: A*x = b */
    OB_LSTSQ_OK = OBL_KIND_LSTSQ,  /* Least squares solution required: A^T(Ax-b) = 0 */
    OB_TYPE_OK = OBL_KIND_TYPE,    /* Type inference obligation */
    OB_PROP_OK = OBL_KIND_PROP,    /* Property holds at location */
} CertObligationKind;

/* Proof certificate structure (RFC 8949 CBOR-compatible) */
typedef struct {
    uint32_t version;           /* Schema version (1) */
    uint64_t timestamp;         /* UNIX timestamp when generated */

    /* Program identification */
    uint8_t program_hash[32];   /* SHA-256 of program bytecode */
    uint32_t program_size;      /* Size in bytes */

    /* Stack machine state */
    int64_t *initial_stack;     /* Initial stack values */
    size_t initial_stack_len;

    int64_t *final_stack;       /* Final stack values */
    size_t final_stack_len;

    /* Type environment */
    uint8_t *type_env;          /* Serialized type environment */
    size_t type_env_len;

    /* Obligations */
    ObligationSet obligations;

    /* Evidence chain */
    uint8_t *evidence_chain;    /* CBOR-serialized evidence */
    size_t evidence_chain_len;

    /* Canonical JSON representation (for hashing) */
    uint8_t *canonical_json;    /* Canonical JSON bytes */
    size_t canonical_json_len;

    /* Certificate hash (SHA-256 of canonical_json) */
    uint8_t cert_hash[32];      /* SHA-256 digest */
} ProofCertificate;

/*
 * ============================================================================
 * WORM RECEIPT STRUCTURES
 * ============================================================================
 */

typedef enum {
    RECEIPT_SUCCESS,            /* Verification succeeded */
    RECEIPT_FAILURE,            /* Verification failed */
    RECEIPT_TIMEOUT,            /* Verification timed out */
    RECEIPT_INVALID_INPUT,      /* Malformed input */
} ReceiptOutcome;

typedef struct {
    uint64_t timestamp;         /* UNIX timestamp */
    uint8_t certificate_hash[32];  /* SHA-256 of certificate */
    uint8_t program_hash[32];   /* SHA-256 of program */
    uint8_t machine_id[32];     /* Machine identity (Blake3) */

    ReceiptOutcome outcome;     /* Success/failure */

    /* Ed25519 signature (64 bytes) */
    uint8_t signature[64];      /* Signature of (cert_hash || program_hash || machine_id || outcome) */
    uint8_t pubkey[32];         /* Ed25519 public key */

    /* Optional: reason for failure */
    char *failure_reason;       /* Human-readable failure reason */
    size_t failure_reason_len;
} WormReceipt;

/*
 * ============================================================================
 * CERTIFICATE API
 * ============================================================================
 */

/* Create a new empty certificate */
ProofCertificate *sov_cert_new(void);

/* Free a certificate and all its data */
void sov_cert_free(ProofCertificate *cert);

/* Set program identification */
void sov_cert_set_program(ProofCertificate *cert,
                          const uint8_t *program_hash,
                          uint32_t program_size);

/* Set initial/final stacks */
int sov_cert_set_stacks(ProofCertificate *cert,
                        const int64_t *init_stack,
                        size_t init_len,
                        const int64_t *final_stack,
                        size_t final_len);

/* Add an obligation to the certificate */
int sov_cert_add_obligation(ProofCertificate *cert,
                            ObligationKind kind,
                            uint32_t start_pc,
                            uint32_t end_pc);

/* Compute canonical JSON representation */
int sov_cert_canonicalize(ProofCertificate *cert);

/* Compute SHA-256 certificate hash */
int sov_cert_hash(ProofCertificate *cert);

/* Serialize to CBOR bytes */
int sov_cert_serialize_cbor(ProofCertificate *cert,
                            uint8_t **out_bytes,
                            size_t *out_len);

/* Deserialize from CBOR bytes */
ProofCertificate *sov_cert_deserialize_cbor(const uint8_t *bytes,
                                             size_t len);

/*
 * ============================================================================
 * WORM RECEIPT API
 * ============================================================================
 */

/* Create a new WORM receipt */
WormReceipt *sov_receipt_new(void);

/* Free a receipt */
void sov_receipt_free(WormReceipt *receipt);

/* Sign and seal a receipt with Ed25519 */
int sov_receipt_seal(WormReceipt *receipt,
                     const uint8_t *secret_key,  /* 32-byte Ed25519 secret */
                     ProofCertificate *cert,
                     const uint8_t *program_hash,
                     ReceiptOutcome outcome);

/* Verify a receipt signature */
int sov_receipt_verify(const WormReceipt *receipt);

/* Serialize receipt to JSON */
int sov_receipt_to_json(const WormReceipt *receipt,
                        uint8_t **out_json,
                        size_t *out_len);

/*
 * ============================================================================
 * PHASE 4: Ed25519 SIGNING API
 * ============================================================================
 */

/* Sign receipt with Ed25519 deterministic signature */
int sign_receipt(WormReceipt *receipt, const uint8_t sk[32]);

/* Verify Ed25519 signature with public key */
int verify_signature(const WormReceipt *receipt, const uint8_t pk[32],
                    const uint8_t sig[64]);

/* Verify receipt using embedded public key */
int verify_receipt_signature(const WormReceipt *receipt);

/*
 * ============================================================================
 * PHASE 4: WORM CHAIN API
 * ============================================================================
 */

typedef struct WormChain WormChain;  /* Opaque WORM chain type */

/* Create new WORM chain */
WormChain *worm_new(void);

/* Free WORM chain */
void worm_free(WormChain *w);

/* Append receipt to WORM chain with parent hashing */
int worm_append(WormChain *w, WormReceipt *receipt, uint8_t node_hash[32]);

/* Get receipt by index */
WormReceipt *worm_get_receipt(WormChain *w, uint64_t index);

/* Verify entire WORM chain for tampering */
int worm_verify_chain(WormChain *w);

/* Get count of receipts in chain */
uint64_t worm_count(WormChain *w);

/* Get tail (most recent) receipt */
WormReceipt *worm_tail(WormChain *w);

/* Get head (genesis) receipt */
WormReceipt *worm_head(WormChain *w);

/* Export node hashes */
int worm_get_node_hashes(WormChain *w, uint64_t index,
                         uint8_t parent_hash[32],
                         uint8_t node_hash[32]);

/*
 * ============================================================================
 * PHASE 4: REPLAY PROTECTION API
 * ============================================================================
 */

typedef struct NonceRegistry NonceRegistry;  /* Opaque nonce registry */

/* Create new nonce registry */
NonceRegistry *nonce_registry_new(void);

/* Free nonce registry */
void nonce_registry_free(NonceRegistry *reg);

/* Add nonce to receipt */
int receipt_add_nonce(WormReceipt *receipt, uint64_t nonce);

/* Extract nonce from receipt */
uint64_t receipt_get_nonce(const WormReceipt *receipt);

/* Add timestamp to receipt */
int receipt_add_timestamp(WormReceipt *receipt, int64_t unix_ms);

/* Register nonce in registry (reject duplicates) */
int register_nonce(uint64_t nonce, NonceRegistry *registry);

/* Verify nonce and register (replay protection) */
int verify_receipt_nonce(const WormReceipt *receipt, NonceRegistry *registry);

/* Verify timestamp is within bounds */
int verify_receipt_timestamp(const WormReceipt *receipt, int64_t max_age_seconds);

/* Verify no clock skew */
int verify_no_clock_skew(const WormReceipt *receipt, int64_t max_skew_seconds);

/* Complete replay protection validation */
int verify_replay_protection(const WormReceipt *receipt,
                            NonceRegistry *registry,
                            int64_t max_age_seconds,
                            int64_t max_skew_seconds);

/*
 * ============================================================================
 * PHASE 4: PERSISTENCE API
 * ============================================================================
 */

/* Atomically persist receipt to file */
int persist_receipt(WormReceipt *receipt, const char *path);

/* Load receipt from file */
WormReceipt *load_receipt(const char *path);

/* Persist WORM chain to directory */
int persist_worm_chain(WormChain *w, const char *dir);

/* Load WORM chain from directory */
WormChain *load_worm_chain(const char *dir);

#ifdef __cplusplus
}
#endif

#endif /* SOV_CERT_H */
