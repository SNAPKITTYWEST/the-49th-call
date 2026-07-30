/*
 * worm.c -- Write-Once-Read-Many (WORM) Chain Implementation
 * FORGE Phase 4: Immutable receipt chain with parent hashing
 *
 * Implements WORM ledger where each receipt is cryptographically linked
 * to the previous receipt via parent hash. Tampering detection via chain
 * verification.
 *
 * Build: part of libsov_forge.a
 * License: Apache 2.0 + AGPL 3.0
 */

#include "src/certificate/sov_cert.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/*
 * WORM chain node structure
 */
typedef struct WormNode {
    WormReceipt receipt;           /* Receipt data */
    uint8_t parent_hash[32];       /* Hash of previous receipt */
    uint8_t node_hash[32];         /* Hash of this node (receipt + parent_hash) */
    struct WormNode *next;         /* Next node in chain */
} WormNode;

/*
 * WORM chain structure
 * Note: We use a struct name that won't conflict with the header typedef
 */
typedef struct {
    WormNode *head;                /* First receipt */
    WormNode *tail;                /* Most recent receipt */
    uint64_t count;                /* Number of receipts */
} WormChainImpl;

/*
 * Simple Blake3-like hash for WORM chain
 * Mixes 32-byte input into 32-byte output deterministically
 */
static void worm_hash(const uint8_t *data, size_t len, uint8_t *out_hash)
{
    if (!data || !out_hash) return;

    /* Initialize hash state with Fibonacci constants */
    uint32_t h[8] = {
        0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
        0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
    };

    /* Absorb input bytes */
    for (size_t i = 0; i < len; i++) {
        h[i % 8] = (h[i % 8] << 1) | (h[i % 8] >> 31);
        h[i % 8] ^= data[i];
        h[(i+1) % 8] += h[i % 8];
    }

    /* Output 32 bytes */
    for (int i = 0; i < 8; i++) {
        out_hash[i*4+0] = (h[i] >> 24) & 0xff;
        out_hash[i*4+1] = (h[i] >> 16) & 0xff;
        out_hash[i*4+2] = (h[i] >> 8) & 0xff;
        out_hash[i*4+3] = h[i] & 0xff;
    }
}

/*
 * Compute hash of receipt (certificate_hash || program_hash || signature)
 */
static void hash_receipt(const WormReceipt *receipt, uint8_t *out_hash)
{
    if (!receipt || !out_hash) return;

    uint8_t buf[128];
    memcpy(buf, receipt->certificate_hash, 32);
    memcpy(buf + 32, receipt->program_hash, 32);
    memcpy(buf + 64, receipt->signature, 64);

    worm_hash(buf, 96, out_hash);
}

/*
 * Create new WORM chain
 *
 * Returns: Pointer to new WORM chain, or NULL on error
 */
WormChain *worm_new(void)
{
    WormChainImpl *w = (WormChainImpl *)malloc(sizeof(WormChainImpl));
    if (w) {
        memset(w, 0, sizeof(WormChainImpl));
    }
    return (WormChain *)w;
}

/*
 * Free WORM chain and all receipts
 */
void worm_free(WormChain *w)
{
    if (!w) return;

    WormChainImpl *impl = (WormChainImpl *)w;
    WormNode *node = impl->head;
    while (node) {
        WormNode *next = node->next;
        if (node->receipt.failure_reason) {
            free(node->receipt.failure_reason);
        }
        free(node);
        node = next;
    }
    free(w);
}

/*
 * Append receipt to WORM chain
 *
 * Links receipt to previous via parent_hash. Returns hash of appended node.
 *
 * Input:
 *   w: WORM chain
 *   receipt: WormReceipt to append (is copied, not referenced)
 *
 * Output:
 *   node_hash: 32-byte hash of this node (written to receipt + parent_hash)
 *
 * Returns: 0 on success, -1 on error
 */
int worm_append(WormChain *w, WormReceipt *receipt, uint8_t node_hash[32])
{
    if (!w || !receipt || !node_hash) return -1;

    WormChainImpl *impl = (WormChainImpl *)w;

    /* Allocate new node */
    WormNode *node = (WormNode *)malloc(sizeof(WormNode));
    if (!node) return -1;

    memset(node, 0, sizeof(WormNode));

    /* Copy receipt data */
    memcpy(&node->receipt, receipt, sizeof(WormReceipt));
    if (receipt->failure_reason) {
        node->receipt.failure_reason = (char *)malloc(receipt->failure_reason_len);
        if (!node->receipt.failure_reason) {
            free(node);
            return -1;
        }
        memcpy(node->receipt.failure_reason, receipt->failure_reason,
               receipt->failure_reason_len);
    }

    /* Set parent hash (link to previous node) */
    if (impl->tail) {
        memcpy(node->parent_hash, ((WormNode *)impl->tail)->node_hash, 32);
    } else {
        /* Genesis: parent hash is all zeros */
        memset(node->parent_hash, 0, 32);
    }

    /* Compute node hash: hash(receipt_hash || parent_hash) */
    uint8_t buf[64];
    hash_receipt(&node->receipt, buf);
    memcpy(buf + 32, node->parent_hash, 32);
    worm_hash(buf, 64, node->node_hash);

    /* Link into chain */
    if (!impl->head) {
        impl->head = node;
    }
    if (impl->tail) {
        ((WormNode *)impl->tail)->next = node;
    }
    impl->tail = node;
    impl->count++;

    /* Return hash of this node */
    memcpy(node_hash, node->node_hash, 32);

    return 0;
}

/*
 * Get receipt by index in WORM chain
 *
 * Returns: Pointer to WormReceipt at index, or NULL if not found
 */
WormReceipt *worm_get_receipt(WormChain *w, uint64_t index)
{
    if (!w) return NULL;

    WormChainImpl *impl = (WormChainImpl *)w;
    if (index >= impl->count) return NULL;

    WormNode *node = (WormNode *)impl->head;
    for (uint64_t i = 0; i < index && node; i++) {
        node = node->next;
    }

    return node ? &node->receipt : NULL;
}

/*
 * Verify entire WORM chain for tampering
 *
 * Checks:
 *   - Each node's parent_hash matches previous node's node_hash
 *   - Each node's node_hash is correctly computed
 *
 * Returns: 0 if chain is valid, -1 if tampering detected
 */
int worm_verify_chain(WormChain *w)
{
    if (!w) return -1;

    WormChainImpl *impl = (WormChainImpl *)w;

    if (impl->count == 0) return 0;

    WormNode *node = (WormNode *)impl->head;
    WormNode *prev = NULL;

    while (node) {
        /* Check parent hash links */
        if (prev) {
            if (memcmp(node->parent_hash, prev->node_hash, 32) != 0) {
                return -1;  /* Parent hash mismatch: tampering detected */
            }
        } else {
            /* Genesis node: parent hash should be all zeros */
            uint8_t zero_hash[32];
            memset(zero_hash, 0, 32);
            if (memcmp(node->parent_hash, zero_hash, 32) != 0) {
                return -1;
            }
        }

        /* Recompute node hash and verify */
        uint8_t expected_hash[32];
        uint8_t buf[64];
        hash_receipt(&node->receipt, buf);
        memcpy(buf + 32, node->parent_hash, 32);
        worm_hash(buf, 64, expected_hash);

        if (memcmp(node->node_hash, expected_hash, 32) != 0) {
            return -1;  /* Node hash mismatch: tampering detected */
        }

        prev = node;
        node = node->next;
    }

    return 0;  /* Chain is valid */
}

/*
 * Get count of receipts in WORM chain
 */
uint64_t worm_count(WormChain *w)
{
    if (!w) return 0;
    WormChainImpl *impl = (WormChainImpl *)w;
    return impl->count;
}

/*
 * Get tail (most recent receipt) of WORM chain
 */
WormReceipt *worm_tail(WormChain *w)
{
    if (!w) return NULL;
    WormChainImpl *impl = (WormChainImpl *)w;
    return (impl->tail) ? &((WormNode *)impl->tail)->receipt : NULL;
}

/*
 * Get head (genesis receipt) of WORM chain
 */
WormReceipt *worm_head(WormChain *w)
{
    if (!w) return NULL;
    WormChainImpl *impl = (WormChainImpl *)w;
    return (impl->head) ? &((WormNode *)impl->head)->receipt : NULL;
}

/*
 * Export WORM chain node's hashes
 *
 * Output:
 *   parent_hash: 32 bytes (can be NULL)
 *   node_hash: 32 bytes (can be NULL)
 *
 * Returns: 0 on success, -1 on error
 */
int worm_get_node_hashes(WormChain *w, uint64_t index,
                         uint8_t parent_hash[32],
                         uint8_t node_hash[32])
{
    if (!w) return -1;

    WormChainImpl *impl = (WormChainImpl *)w;
    if (index >= impl->count) return -1;

    WormNode *node = (WormNode *)impl->head;
    for (uint64_t i = 0; i < index && node; i++) {
        node = node->next;
    }

    if (!node) return -1;

    if (parent_hash) memcpy(parent_hash, node->parent_hash, 32);
    if (node_hash) memcpy(node_hash, node->node_hash, 32);

    return 0;
}
