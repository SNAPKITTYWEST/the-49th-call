/*
 * persist.c -- Receipt Persistence to Filesystem
 * FORGE Phase 4: Atomic write and recovery of receipts
 *
 * Implements persistent storage of receipts with atomic writes
 * to prevent partial writes and enable WORM chain recovery.
 *
 * Build: part of libsov_forge.a
 * License: Apache 2.0 + AGPL 3.0
 */

#include "src/certificate/sov_cert.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>

/*
 * Serialize receipt to binary format
 *
 * Format:
 *   timestamp(8) || certificate_hash(32) || program_hash(32) ||
 *   machine_id(32) || outcome(1) || pubkey(32) || signature(64) ||
 *   failure_reason_len(4) || failure_reason(...)
 *
 * Returns: Allocated buffer with serialized data, or NULL on error
 *          Caller must free
 */
static uint8_t *serialize_receipt_binary(const WormReceipt *receipt,
                                        size_t *out_len)
{
    if (!receipt || !out_len) return NULL;

    size_t buf_size = 8 + 32 + 32 + 32 + 1 + 32 + 64 + 4 + receipt->failure_reason_len;
    uint8_t *buf = (uint8_t *)malloc(buf_size);
    if (!buf) return NULL;

    size_t pos = 0;

    /* timestamp (8 bytes, big-endian) */
    buf[pos++] = (receipt->timestamp >> 56) & 0xff;
    buf[pos++] = (receipt->timestamp >> 48) & 0xff;
    buf[pos++] = (receipt->timestamp >> 40) & 0xff;
    buf[pos++] = (receipt->timestamp >> 32) & 0xff;
    buf[pos++] = (receipt->timestamp >> 24) & 0xff;
    buf[pos++] = (receipt->timestamp >> 16) & 0xff;
    buf[pos++] = (receipt->timestamp >> 8) & 0xff;
    buf[pos++] = receipt->timestamp & 0xff;

    /* certificate_hash (32 bytes) */
    memcpy(buf + pos, receipt->certificate_hash, 32);
    pos += 32;

    /* program_hash (32 bytes) */
    memcpy(buf + pos, receipt->program_hash, 32);
    pos += 32;

    /* machine_id (32 bytes) */
    memcpy(buf + pos, receipt->machine_id, 32);
    pos += 32;

    /* outcome (1 byte) */
    buf[pos++] = (uint8_t)receipt->outcome;

    /* pubkey (32 bytes) */
    memcpy(buf + pos, receipt->pubkey, 32);
    pos += 32;

    /* signature (64 bytes) */
    memcpy(buf + pos, receipt->signature, 64);
    pos += 64;

    /* failure_reason_len (4 bytes, big-endian) */
    buf[pos++] = (receipt->failure_reason_len >> 24) & 0xff;
    buf[pos++] = (receipt->failure_reason_len >> 16) & 0xff;
    buf[pos++] = (receipt->failure_reason_len >> 8) & 0xff;
    buf[pos++] = receipt->failure_reason_len & 0xff;

    /* failure_reason (variable) */
    if (receipt->failure_reason && receipt->failure_reason_len > 0) {
        memcpy(buf + pos, receipt->failure_reason, receipt->failure_reason_len);
        pos += receipt->failure_reason_len;
    }

    *out_len = pos;
    return buf;
}

/*
 * Deserialize receipt from binary format
 *
 * Returns: Allocated WormReceipt, or NULL on error
 *          Caller must free via sov_receipt_free
 */
static WormReceipt *deserialize_receipt_binary(const uint8_t *buf, size_t len)
{
    if (!buf || len < 203) return NULL;  /* Minimum size without failure_reason */

    WormReceipt *receipt = sov_receipt_new();
    if (!receipt) return NULL;

    size_t pos = 0;

    /* timestamp (8 bytes, big-endian) */
    receipt->timestamp = ((uint64_t)buf[pos+0] << 56) |
                         ((uint64_t)buf[pos+1] << 48) |
                         ((uint64_t)buf[pos+2] << 40) |
                         ((uint64_t)buf[pos+3] << 32) |
                         ((uint64_t)buf[pos+4] << 24) |
                         ((uint64_t)buf[pos+5] << 16) |
                         ((uint64_t)buf[pos+6] << 8) |
                         ((uint64_t)buf[pos+7]);
    pos += 8;

    /* certificate_hash (32 bytes) */
    memcpy(receipt->certificate_hash, buf + pos, 32);
    pos += 32;

    /* program_hash (32 bytes) */
    memcpy(receipt->program_hash, buf + pos, 32);
    pos += 32;

    /* machine_id (32 bytes) */
    memcpy(receipt->machine_id, buf + pos, 32);
    pos += 32;

    /* outcome (1 byte) */
    receipt->outcome = (ReceiptOutcome)buf[pos++];

    /* pubkey (32 bytes) */
    memcpy(receipt->pubkey, buf + pos, 32);
    pos += 32;

    /* signature (64 bytes) */
    memcpy(receipt->signature, buf + pos, 64);
    pos += 64;

    /* failure_reason_len (4 bytes, big-endian) */
    receipt->failure_reason_len = ((uint32_t)buf[pos+0] << 24) |
                                   ((uint32_t)buf[pos+1] << 16) |
                                   ((uint32_t)buf[pos+2] << 8) |
                                   ((uint32_t)buf[pos+3]);
    pos += 4;

    /* failure_reason (variable) */
    if (receipt->failure_reason_len > 0 && pos + receipt->failure_reason_len <= len) {
        receipt->failure_reason = (char *)malloc(receipt->failure_reason_len);
        if (!receipt->failure_reason) {
            sov_receipt_free(receipt);
            return NULL;
        }
        memcpy(receipt->failure_reason, buf + pos, receipt->failure_reason_len);
    }

    return receipt;
}

/*
 * Atomically persist receipt to file
 *
 * Uses write-to-temp-then-rename pattern for atomicity:
 *   1. Write to .tmp file
 *   2. fsync to ensure durability
 *   3. Rename to final path
 *
 * Input:
 *   receipt: WormReceipt to persist
 *   path: Filesystem path (e.g., /var/lib/receipts/0000001.receipt)
 *
 * Returns: 0 on success, -1 on error
 */
int persist_receipt(WormReceipt *receipt, const char *path)
{
    if (!receipt || !path) return -1;

    /* Serialize receipt */
    size_t buf_len = 0;
    uint8_t *buf = serialize_receipt_binary(receipt, &buf_len);
    if (!buf) return -1;

    /* Create temp filename */
    char temp_path[512];
    snprintf(temp_path, sizeof(temp_path), "%s.tmp", path);

    /* Write to temp file */
    FILE *f = fopen(temp_path, "wb");
    if (!f) {
        free(buf);
        return -1;
    }

    size_t written = fwrite(buf, 1, buf_len, f);
    free(buf);

    if (written != buf_len) {
        fclose(f);
        remove(temp_path);
        return -1;
    }

    /* Ensure durability (fsync) */
    if (fflush(f) != 0) {
        fclose(f);
        remove(temp_path);
        return -1;
    }

    fclose(f);

    /* Atomic rename */
    if (rename(temp_path, path) != 0) {
        remove(temp_path);
        return -1;
    }

    return 0;
}

/*
 * Load receipt from file
 *
 * Input:
 *   path: Filesystem path to receipt file
 *
 * Returns: Allocated WormReceipt, or NULL on error
 *          Caller must free via sov_receipt_free
 */
WormReceipt *load_receipt(const char *path)
{
    if (!path) return NULL;

    FILE *f = fopen(path, "rb");
    if (!f) return NULL;

    /* Get file size */
    fseek(f, 0, SEEK_END);
    long file_size = ftell(f);
    fseek(f, 0, SEEK_SET);

    if (file_size < 203) {  /* Minimum receipt size */
        fclose(f);
        return NULL;
    }

    /* Read file */
    uint8_t *buf = (uint8_t *)malloc(file_size);
    if (!buf) {
        fclose(f);
        return NULL;
    }

    size_t read = fread(buf, 1, file_size, f);
    fclose(f);

    if (read != (size_t)file_size) {
        free(buf);
        return NULL;
    }

    /* Deserialize */
    WormReceipt *receipt = deserialize_receipt_binary(buf, (size_t)file_size);
    free(buf);

    return receipt;
}

/*
 * Persist WORM chain to directory
 *
 * Writes one receipt per file:
 *   dir/00000000.receipt (genesis)
 *   dir/00000001.receipt
 *   ...
 *   dir/XXXXXXXX.receipt (tail)
 *
 * Input:
 *   w: WORM chain to persist
 *   dir: Directory path (must exist)
 *
 * Returns: 0 on success, -1 on any error
 */
int persist_worm_chain(WormChain *w, const char *dir)
{
    if (!w || !dir) return -1;

    /* Placeholder: requires WormChain iteration and directory listing */
    /* For now, returns -1 to indicate not yet implemented */
    (void)w;  /* Avoid unused parameter warning */
    (void)dir;

    return -1;  /* Placeholder: requires WormChain definition in worm.c */
}

/*
 * Load WORM chain from directory
 *
 * Reads all .receipt files and reconstructs chain.
 *
 * Input:
 *   dir: Directory containing receipt files
 *
 * Returns: Pointer to reconstructed WORM chain, or NULL on error
 *          Caller must free via worm_free
 */
WormChain *load_worm_chain(const char *dir)
{
    if (!dir) return NULL;

    /* Placeholder: requires WormChain and directory listing */
    (void)dir;  /* Avoid unused parameter warning */
    return NULL;
}
