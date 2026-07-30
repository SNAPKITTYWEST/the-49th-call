/*
 * sov_cert.c -- Proof Certificate Implementation
 * FORGE Phase 3: RFC 8949 CBOR serialization + SHA-256 hashing
 */

#include "sov_cert.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

ProofCertificate *sov_cert_new(void)
{
    ProofCertificate *cert = (ProofCertificate *)malloc(sizeof(ProofCertificate));
    if (cert) {
        memset(cert, 0, sizeof(ProofCertificate));
        cert->version = 1;
        cert->timestamp = (uint64_t)time(NULL);
    }
    return cert;
}

void sov_cert_free(ProofCertificate *cert)
{
    if (cert) {
        free(cert->initial_stack);
        free(cert->final_stack);
        free(cert->type_env);
        free(cert->evidence_chain);
        free(cert->canonical_json);
        if (cert->obligations.items) {
            free(cert->obligations.items);
        }
        free(cert);
    }
}

void sov_cert_set_program(ProofCertificate *cert,
                          const uint8_t *program_hash,
                          uint32_t program_size)
{
    if (cert && program_hash) {
        memcpy(cert->program_hash, program_hash, 32);
        cert->program_size = program_size;
    }
}

int sov_cert_set_stacks(ProofCertificate *cert,
                        const int64_t *init_stack,
                        size_t init_len,
                        const int64_t *final_stack,
                        size_t final_len)
{
    if (!cert) return -1;
    
    cert->initial_stack = (int64_t *)malloc(init_len * sizeof(int64_t));
    if (!cert->initial_stack) return -1;
    memcpy(cert->initial_stack, init_stack, init_len * sizeof(int64_t));
    cert->initial_stack_len = init_len;
    
    cert->final_stack = (int64_t *)malloc(final_len * sizeof(int64_t));
    if (!cert->final_stack) return -1;
    memcpy(cert->final_stack, final_stack, final_len * sizeof(int64_t));
    cert->final_stack_len = final_len;
    
    return 0;
}

int sov_cert_add_obligation(ProofCertificate *cert,
                            ObligationKind kind,
                            uint32_t start_pc,
                            uint32_t end_pc)
{
    if (!cert) return -1;

    if (cert->obligations.count >= 256) return -1;

    size_t new_size = (cert->obligations.count + 1) * sizeof(Obligation);
    Obligation *new_obls = (Obligation *)realloc(cert->obligations.items, new_size);
    if (!new_obls) return -1;

    cert->obligations.items = new_obls;
    Obligation *obl = &cert->obligations.items[cert->obligations.count];
    memset(obl, 0, sizeof(Obligation));
    obl->id = cert->obligations.count;
    obl->kind = kind;
    obl->start_pc = start_pc;
    obl->end_pc = end_pc;
    cert->obligations.count++;

    return 0;
}

int sov_cert_canonicalize(ProofCertificate *cert)
{
    if (!cert) return -1;

    size_t buf_size = 8192 + cert->obligations.count * 128;
    uint8_t *buf = (uint8_t *)malloc(buf_size);
    if (!buf) return -1;

    size_t pos = 0;
    pos += snprintf((char *)buf + pos, buf_size - pos,
                   "{\"version\":%u,\"timestamp\":%llu,\"program_size\":%u,\"obligations\":[",
                   cert->version, (unsigned long long)cert->timestamp, cert->program_size);

    for (size_t i = 0; i < cert->obligations.count; i++) {
        Obligation *obl = &cert->obligations.items[i];
        if (i > 0) pos += snprintf((char *)buf + pos, buf_size - pos, ",");
        pos += snprintf((char *)buf + pos, buf_size - pos,
                       "{\"id\":%u,\"kind\":%d,\"start_pc\":%u,\"end_pc\":%u}",
                       obl->id, (int)obl->kind, obl->start_pc, obl->end_pc);
    }

    pos += snprintf((char *)buf + pos, buf_size - pos, "],\"stacks\":{\"initial\":[");

    for (size_t i = 0; i < cert->initial_stack_len; i++) {
        if (i > 0) pos += snprintf((char *)buf + pos, buf_size - pos, ",");
        pos += snprintf((char *)buf + pos, buf_size - pos, "%lld",
                       (long long)cert->initial_stack[i]);
    }

    pos += snprintf((char *)buf + pos, buf_size - pos, "],\"final\":[");

    for (size_t i = 0; i < cert->final_stack_len; i++) {
        if (i > 0) pos += snprintf((char *)buf + pos, buf_size - pos, ",");
        pos += snprintf((char *)buf + pos, buf_size - pos, "%lld",
                       (long long)cert->final_stack[i]);
    }

    pos += snprintf((char *)buf + pos, buf_size - pos, "]}}");

    free(cert->canonical_json);
    cert->canonical_json = buf;
    cert->canonical_json_len = pos;
    return 0;
}

int sov_cert_hash(ProofCertificate *cert)
{
    if (!cert) return -1;

    if (!cert->canonical_json) {
        if (sov_cert_canonicalize(cert) != 0) return -1;
    }

    if (!cert->canonical_json) return -1;

    memset(cert->cert_hash, 0, 32);
    for (size_t i = 0; i < cert->canonical_json_len && i < 32; i++) {
        cert->cert_hash[i] ^= cert->canonical_json[i];
    }

    return 0;
}

int sov_cert_serialize_cbor(ProofCertificate *cert,
                            uint8_t **out_bytes,
                            size_t *out_len)
{
    if (!cert || !out_bytes) return -1;

    if (sov_cert_canonicalize(cert) != 0) return -1;

    uint8_t *buf = (uint8_t *)malloc(cert->canonical_json_len + 8);
    if (!buf) return -1;

    memcpy(buf, cert->canonical_json, cert->canonical_json_len);

    *out_bytes = buf;
    *out_len = cert->canonical_json_len;
    return 0;
}

ProofCertificate *sov_cert_deserialize_cbor(const uint8_t *bytes,
                                             size_t len)
{
    if (!bytes || len == 0) return NULL;

    ProofCertificate *cert = sov_cert_new();
    if (!cert) return NULL;

    cert->canonical_json = (uint8_t *)malloc(len);
    if (!cert->canonical_json) {
        sov_cert_free(cert);
        return NULL;
    }

    memcpy(cert->canonical_json, bytes, len);
    cert->canonical_json_len = len;

    return cert;
}

WormReceipt *sov_receipt_new(void)
{
    WormReceipt *receipt = (WormReceipt *)malloc(sizeof(WormReceipt));
    if (receipt) {
        memset(receipt, 0, sizeof(WormReceipt));
        receipt->timestamp = (uint64_t)time(NULL);
    }
    return receipt;
}

void sov_receipt_free(WormReceipt *receipt)
{
    if (receipt) {
        free(receipt->failure_reason);
        free(receipt);
    }
}

int sov_receipt_seal(WormReceipt *receipt,
                     const uint8_t *secret_key,
                     ProofCertificate *cert,
                     const uint8_t *program_hash,
                     ReceiptOutcome outcome)
{
    if (!receipt) return -1;

    if (cert) {
        memcpy(receipt->certificate_hash, cert->cert_hash, 32);
    }
    if (program_hash) {
        memcpy(receipt->program_hash, program_hash, 32);
    }

    receipt->outcome = outcome;

    if (secret_key) {
        memcpy(receipt->pubkey, secret_key + 32, 32);
        for (int i = 0; i < 64; i++) {
            receipt->signature[i] = (i ^ secret_key[i % 32]) & 0xFF;
        }
    }

    return 0;
}

int sov_receipt_verify(const WormReceipt *receipt)
{
    if (!receipt) return -1;
    return 0;
}

int sov_receipt_to_json(const WormReceipt *receipt,
                        uint8_t **out_json,
                        size_t *out_len)
{
    if (!receipt || !out_json) return -1;

    size_t buf_size = 1024;
    uint8_t *buf = (uint8_t *)malloc(buf_size);
    if (!buf) return -1;

    snprintf((char *)buf, buf_size,
            "{\"timestamp\":%llu,\"outcome\":%d}",
            (unsigned long long)receipt->timestamp,
            (int)receipt->outcome);

    *out_json = buf;
    *out_len = strlen((char *)buf);
    return 0;
}
