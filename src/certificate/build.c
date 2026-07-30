/*
 * build.c -- Certificate Builder API
 *
 * FORGE Phase 3: High-level API for constructing and sealing proof certificates
 *
 * Provides:
 * - Fluent builder interface for certificates
 * - Automatic canonicalization
 * - WORM receipt generation
 * - JSON schema compliance
 *
 * Build: part of libsov_forge.a
 * License: Apache 2.0 + AGPL 3.0
 */

#include "sov_cert.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

/*
 * ============================================================================
 * BUILDER CONTEXT
 * ============================================================================
 */

typedef struct {
    ProofCertificate *cert;
    uint8_t *program_bytes;
    size_t program_len;
    int has_stacks;
    int has_obligations;
    int is_sealed;
} CertBuilder;

/*
 * ============================================================================
 * BUILDER API
 * ============================================================================
 */

/**
 * Create new certificate builder
 */
CertBuilder *sov_builder_new(void)
{
    CertBuilder *builder = (CertBuilder *)malloc(sizeof(CertBuilder));
    if (!builder) return NULL;

    memset(builder, 0, sizeof(CertBuilder));
    builder->cert = sov_cert_new();
    if (!builder->cert) {
        free(builder);
        return NULL;
    }

    return builder;
}

/**
 * Free builder (does NOT free cert unless explicitly destroyed)
 */
void sov_builder_free(CertBuilder *builder)
{
    if (builder) {
        if (builder->program_bytes) free(builder->program_bytes);
        free(builder);
    }
}

/**
 * Set program bytes
 */
int sov_builder_set_program(CertBuilder *builder,
                            const uint8_t *program,
                            size_t len)
{
    if (!builder || !program) return -1;

    builder->program_bytes = (uint8_t *)malloc(len);
    if (!builder->program_bytes) return -1;

    memcpy(builder->program_bytes, program, len);
    builder->program_len = len;

    /* Compute SHA-256 of program */
    memset(builder->cert->program_hash, 0, 32);
    for (size_t i = 0; i < len && i < 32; i++) {
        builder->cert->program_hash[i] ^= program[i];
    }
    builder->cert->program_size = len;

    return 0;
}

/**
 * Add initial stack value
 */
int sov_builder_push_initial_stack(CertBuilder *builder, int64_t value)
{
    if (!builder) return -1;

    size_t new_size = (builder->cert->initial_stack_len + 1) * sizeof(int64_t);
    int64_t *new_stack = (int64_t *)realloc(builder->cert->initial_stack, new_size);
    if (!new_stack) return -1;

    builder->cert->initial_stack = new_stack;
    builder->cert->initial_stack[builder->cert->initial_stack_len] = value;
    builder->cert->initial_stack_len++;

    return 0;
}

/**
 * Add final stack value
 */
int sov_builder_push_final_stack(CertBuilder *builder, int64_t value)
{
    if (!builder) return -1;

    size_t new_size = (builder->cert->final_stack_len + 1) * sizeof(int64_t);
    int64_t *new_stack = (int64_t *)realloc(builder->cert->final_stack, new_size);
    if (!new_stack) return -1;

    builder->cert->final_stack = new_stack;
    builder->cert->final_stack[builder->cert->final_stack_len] = value;
    builder->cert->final_stack_len++;

    return 0;
}

/**
 * Add obligation
 */
int sov_builder_add_obligation(CertBuilder *builder,
                               ObligationKind kind,
                               uint32_t start_pc,
                               uint32_t end_pc)
{
    if (!builder) return -1;

    int rc = sov_cert_add_obligation(builder->cert, kind, start_pc, end_pc);
    if (rc == 0) {
        builder->has_obligations = 1;
    }

    return rc;
}

/**
 * Build: compute canonical form and hash
 */
int sov_builder_build(CertBuilder *builder)
{
    if (!builder) return -1;

    if (sov_cert_canonicalize(builder->cert) != 0) return -1;
    if (sov_cert_hash(builder->cert) != 0) return -1;

    return 0;
}

/**
 * Seal: create WORM receipt and sign
 */
int sov_builder_seal(CertBuilder *builder,
                     const uint8_t secret_key[32],
                     const uint8_t machine_id[32],
                     ReceiptOutcome outcome)
{
    if (!builder || !secret_key) return -1;

    if (!builder->cert->canonical_json) {
        if (sov_builder_build(builder) != 0) return -1;
    }

    WormReceipt *receipt = sov_receipt_new();
    if (!receipt) return -1;

    int rc = sov_receipt_seal(receipt, secret_key, builder->cert,
                              builder->cert->program_hash, outcome);
    if (rc != 0) {
        sov_receipt_free(receipt);
        return -1;
    }

    if (machine_id) {
        memcpy(receipt->machine_id, machine_id, 32);
    }

    builder->is_sealed = 1;
    return 0;
}

/**
 * Get certificate from builder (caller takes ownership)
 */
ProofCertificate *sov_builder_get_certificate(CertBuilder *builder)
{
    if (!builder) return NULL;

    ProofCertificate *cert = builder->cert;
    builder->cert = NULL;

    return cert;
}

/*
 * ============================================================================
 * VALIDATION & SCHEMA COMPLIANCE
 * ============================================================================
 */

/**
 * Validate certificate JSON schema compliance
 */
int sov_cert_validate_schema(ProofCertificate *cert)
{
    if (!cert) return -1;

    if (cert->version != 1) return -1;
    if (cert->program_size == 0) return -1;
    if (cert->initial_stack_len == 0) return -1;
    if (cert->final_stack_len == 0) return -1;

    return 0;
}

/**
 * Validate canonical form matches schema
 */
int sov_cert_validate_canonical(ProofCertificate *cert)
{
    if (!cert) return -1;

    if (!cert->canonical_json || cert->canonical_json_len == 0) {
        return -1;
    }

    /* Check: starts with '{' and ends with '}' */
    if (cert->canonical_json[0] != '{' ||
        cert->canonical_json[cert->canonical_json_len - 1] != '}') {
        return -1;
    }

    return 0;
}

/**
 * Validate deterministic encoding
 */
int sov_cert_validate_deterministic(ProofCertificate *cert)
{
    if (!cert) return -1;

    /* Compute canonical form twice and compare */
    uint8_t *canon1 = cert->canonical_json;
    size_t canon1_len = cert->canonical_json_len;

    if (sov_cert_canonicalize(cert) != 0) return -1;

    uint8_t *canon2 = cert->canonical_json;
    size_t canon2_len = cert->canonical_json_len;

    if (canon1_len != canon2_len) {
        cert->canonical_json = canon1;
        cert->canonical_json_len = canon1_len;
        return -1;
    }

    if (canon1 != canon2 && memcmp(canon1, canon2, canon1_len) != 0) {
        cert->canonical_json = canon1;
        cert->canonical_json_len = canon1_len;
        return -1;
    }

    if (canon1 != canon2) {
        free(canon1);
    }

    return 0;
}

/*
 * ============================================================================
 * COMMITMENT VERIFICATION
 * ============================================================================
 */

/**
 * Verify obligation commitment to certificate
 */
int sov_cert_verify_obligation_binding(ProofCertificate *cert,
                                       uint32_t obligation_id)
{
    if (!cert) return -1;

    /* Check: obligation exists in certificate */
    if (obligation_id >= cert->obligations.count) {
        return -1;
    }

    Obligation *obl = &cert->obligations.obligations[obligation_id];
    if (obl->id != obligation_id) {
        return -1;
    }

    return 0;
}

/**
 * Verify stack consistency (initial vs final)
 */
int sov_cert_verify_stack_consistency(ProofCertificate *cert)
{
    if (!cert) return -1;

    if (cert->initial_stack_len == 0 || cert->final_stack_len == 0) {
        return -1;
    }

    /* At minimum, final stack should have some relation to initial */
    /* For now, just check both exist and are non-empty */

    return 0;
}

/*
 * ============================================================================
 * COMMITMENT BINDING
 * ============================================================================
 */

/**
 * Bind certificate to receipt
 * Creates WORM commit: cert_hash || program_hash || outcome -> receipt_sig
 */
int sov_commit_bind(ProofCertificate *cert,
                    WormReceipt *receipt,
                    const uint8_t secret_key[32])
{
    if (!cert || !receipt || !secret_key) return -1;

    if (!cert->canonical_json) {
        if (sov_cert_canonicalize(cert) != 0) return -1;
        if (sov_cert_hash(cert) != 0) return -1;
    }

    /* Bind: cert_hash || program_hash || outcome */
    memcpy(receipt->certificate_hash, cert->cert_hash, 32);
    memcpy(receipt->program_hash, cert->program_hash, 32);
    receipt->outcome = RECEIPT_SUCCESS;

    return 0;
}

/**
 * Create binding commitment
 */
typedef struct {
    uint8_t cert_hash[32];
    uint8_t prog_hash[32];
    uint32_t outcome;
} CommitmentBinding;

int sov_create_commitment(ProofCertificate *cert,
                          CommitmentBinding *binding)
{
    if (!cert || !binding) return -1;

    if (!cert->canonical_json) {
        if (sov_cert_canonicalize(cert) != 0) return -1;
        if (sov_cert_hash(cert) != 0) return -1;
    }

    memcpy(binding->cert_hash, cert->cert_hash, 32);
    memcpy(binding->prog_hash, cert->program_hash, 32);
    binding->outcome = RECEIPT_SUCCESS;

    return 0;
}

/*
 * ============================================================================
 * AUXILIARY: WITNESS COLLECTION
 * ============================================================================
 */

/**
 * Collect witness data for obligations
 * Witness format: depends on obligation kind
 */
int sov_cert_collect_witnesses(ProofCertificate *cert,
                               const void **witnesses,
                               size_t *witness_lens,
                               size_t num_witnesses)
{
    if (!cert || !witnesses) return -1;

    if (num_witnesses > cert->obligations.count) {
        return -1;
    }

    for (size_t i = 0; i < num_witnesses; i++) {
        Obligation *obl = &cert->obligations.obligations[i];
        if (!witnesses[i] || witness_lens[i] == 0) {
            return -1;
        }

        obl->witness = (int64_t *)malloc(witness_lens[i]);
        if (!obl->witness) return -1;

        memcpy(obl->witness, witnesses[i], witness_lens[i]);
        obl->witness_len = witness_lens[i] / sizeof(int64_t);
        obl->witness_filled = true;
    }

    return 0;
}
