/*
 * cbor.c -- RFC 8949 CBOR Serialization
 *
 * FORGE Phase 3: CBOR encoder/decoder for proof certificates
 *
 * Implements minimal RFC 8949 CBOR for:
 * - Integers (major type 0)
 * - Arrays (major type 4)
 * - Maps (major type 5)
 * - Byte strings (major type 2)
 * - Text strings (major type 3)
 *
 * Build: part of libsov_forge.a
 * License: Apache 2.0 + AGPL 3.0
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/*
 * ============================================================================
 * CBOR ENCODING
 * ============================================================================
 */

/**
 * Encode CBOR major type + value
 * Returns number of bytes written to buf
 */
static size_t cbor_encode_uint(uint8_t *buf, size_t buflen,
                               uint8_t major_type, uint64_t value)
{
    if (!buf || buflen == 0) return 0;

    if (value < 24) {
        if (buflen < 1) return 0;
        buf[0] = (major_type << 5) | (uint8_t)value;
        return 1;
    }
    if (value < 256) {
        if (buflen < 2) return 0;
        buf[0] = (major_type << 5) | 24;
        buf[1] = (uint8_t)value;
        return 2;
    }
    if (value < 65536) {
        if (buflen < 3) return 0;
        buf[0] = (major_type << 5) | 25;
        buf[1] = (uint8_t)(value >> 8);
        buf[2] = (uint8_t)value;
        return 3;
    }
    if (value < 4294967296ULL) {
        if (buflen < 5) return 0;
        buf[0] = (major_type << 5) | 26;
        buf[1] = (uint8_t)(value >> 24);
        buf[2] = (uint8_t)(value >> 16);
        buf[3] = (uint8_t)(value >> 8);
        buf[4] = (uint8_t)value;
        return 5;
    }
    if (buflen < 9) return 0;
    buf[0] = (major_type << 5) | 27;
    for (int i = 0; i < 8; i++) {
        buf[8 - i] = (uint8_t)(value >> (8 * i));
    }
    return 9;
}

/**
 * Encode CBOR integer (major type 0: unsigned)
 */
size_t cbor_encode_int(uint8_t *buf, size_t buflen, int64_t value)
{
    if (value >= 0) {
        return cbor_encode_uint(buf, buflen, 0, (uint64_t)value);
    } else {
        return cbor_encode_uint(buf, buflen, 1, (uint64_t)(-1 - value));
    }
}

/**
 * Encode CBOR byte string (major type 2)
 */
size_t cbor_encode_bytes(uint8_t *buf, size_t buflen,
                         const uint8_t *data, size_t data_len)
{
    if (!buf || !data) return 0;

    size_t header_len = cbor_encode_uint(buf, buflen, 2, data_len);
    if (header_len == 0) return 0;

    if (buflen < header_len + data_len) return 0;

    memcpy(buf + header_len, data, data_len);
    return header_len + data_len;
}

/**
 * Encode CBOR text string (major type 3)
 */
size_t cbor_encode_text(uint8_t *buf, size_t buflen,
                        const char *text)
{
    if (!buf || !text) return 0;

    size_t text_len = strlen(text);
    size_t header_len = cbor_encode_uint(buf, buflen, 3, text_len);
    if (header_len == 0) return 0;

    if (buflen < header_len + text_len) return 0;

    memcpy(buf + header_len, text, text_len);
    return header_len + text_len;
}

/**
 * Encode CBOR array (major type 4)
 */
size_t cbor_encode_array_start(uint8_t *buf, size_t buflen,
                               size_t num_elements)
{
    return cbor_encode_uint(buf, buflen, 4, num_elements);
}

/**
 * Encode CBOR map (major type 5)
 */
size_t cbor_encode_map_start(uint8_t *buf, size_t buflen,
                             size_t num_pairs)
{
    return cbor_encode_uint(buf, buflen, 5, num_pairs);
}

/*
 * ============================================================================
 * CBOR DECODING
 * ============================================================================
 */

/**
 * Decode CBOR header: major type and value
 * Returns number of bytes consumed, or 0 on error
 * Sets *out_major and *out_value
 */
static size_t cbor_decode_uint(const uint8_t *buf, size_t buflen,
                               uint8_t *out_major, uint64_t *out_value)
{
    if (!buf || buflen == 0 || !out_major || !out_value) return 0;

    uint8_t first = buf[0];
    *out_major = first >> 5;
    uint8_t info = first & 0x1f;

    if (info < 24) {
        *out_value = info;
        return 1;
    }
    if (info == 24) {
        if (buflen < 2) return 0;
        *out_value = buf[1];
        return 2;
    }
    if (info == 25) {
        if (buflen < 3) return 0;
        *out_value = ((uint64_t)buf[1] << 8) | buf[2];
        return 3;
    }
    if (info == 26) {
        if (buflen < 5) return 0;
        *out_value = ((uint64_t)buf[1] << 24) |
                     ((uint64_t)buf[2] << 16) |
                     ((uint64_t)buf[3] << 8) |
                     buf[4];
        return 5;
    }
    if (info == 27) {
        if (buflen < 9) return 0;
        *out_value = 0;
        for (int i = 0; i < 8; i++) {
            *out_value = (*out_value << 8) | buf[1 + i];
        }
        return 9;
    }

    return 0;
}

/**
 * Decode CBOR integer
 */
size_t cbor_decode_int(const uint8_t *buf, size_t buflen,
                       int64_t *out_value)
{
    uint8_t major;
    uint64_t value;
    size_t len = cbor_decode_uint(buf, buflen, &major, &value);
    if (len == 0) return 0;

    if (major == 0) {
        *out_value = (int64_t)value;
    } else if (major == 1) {
        *out_value = -1 - (int64_t)value;
    } else {
        return 0;
    }

    return len;
}

/**
 * Decode CBOR byte string
 */
size_t cbor_decode_bytes(const uint8_t *buf, size_t buflen,
                         uint8_t **out_data, size_t *out_len)
{
    uint8_t major;
    uint64_t len;
    size_t header_len = cbor_decode_uint(buf, buflen, &major, &len);
    if (header_len == 0 || major != 2) return 0;

    if (buflen < header_len + len) return 0;

    *out_data = (uint8_t *)(buf + header_len);
    *out_len = (size_t)len;
    return header_len + len;
}

/**
 * Decode CBOR text string
 */
size_t cbor_decode_text(const uint8_t *buf, size_t buflen,
                        char **out_text, size_t *out_len)
{
    uint8_t major;
    uint64_t len;
    size_t header_len = cbor_decode_uint(buf, buflen, &major, &len);
    if (header_len == 0 || major != 3) return 0;

    if (buflen < header_len + len) return 0;

    *out_text = (char *)(buf + header_len);
    *out_len = (size_t)len;
    return header_len + len;
}

/**
 * Decode CBOR array header
 */
size_t cbor_decode_array_start(const uint8_t *buf, size_t buflen,
                               size_t *out_count)
{
    uint8_t major;
    uint64_t count;
    size_t len = cbor_decode_uint(buf, buflen, &major, &count);
    if (len == 0 || major != 4) return 0;

    *out_count = (size_t)count;
    return len;
}

/**
 * Decode CBOR map header
 */
size_t cbor_decode_map_start(const uint8_t *buf, size_t buflen,
                             size_t *out_pairs)
{
    uint8_t major;
    uint64_t pairs;
    size_t len = cbor_decode_uint(buf, buflen, &major, &pairs);
    if (len == 0 || major != 5) return 0;

    *out_pairs = (size_t)pairs;
    return len;
}
