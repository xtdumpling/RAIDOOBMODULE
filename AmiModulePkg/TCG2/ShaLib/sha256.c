/*
 * SHA-256 hash implementation and interface functions
 * Copyright (c) 2003-2012, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "sha256.h"

#if 0
/**
 * hmac_sha256_vector - HMAC-SHA256 over data vector (RFC 2104)
 * @key: Key for HMAC operations
 * @key_len: Length of the key in bytes
 * @num_elem: Number of elements in the data vector
 * @addr: Pointers to the data areas
 * @len: Lengths of the data blocks
 * @mac: Buffer for the hash (32 bytes)
 */
void hmac_sha256_vector(const u8 *key, size_t key_len, size_t num_elem,
                        const u8 *addr[], const size_t *len, u8 *mac)
{
    unsigned char k_pad[64]; /* padding - key XORd with ipad/opad */
    unsigned char tk[32];
    const u8 *_addr[6];
    size_t _len[6], i;

    if (num_elem > 5)
    {
        /*
         * Fixed limit on the number of fragments to avoid having to
         * allocate memory (which could fail).
         */
        return;
    }

    /* if key is longer than 64 bytes reset it to key = SHA256(key) */
    if (key_len > 64)
    {
        sha256_vector(1, &key, &key_len, tk);
        key = tk;
        key_len = 32;
    }

    /* the HMAC_SHA256 transform looks like:
     *
     * SHA256(K XOR opad, SHA256(K XOR ipad, text))
     *
     * where K is an n byte key
     * ipad is the byte 0x36 repeated 64 times
     * opad is the byte 0x5c repeated 64 times
     * and text is the data being protected */

    /* start out by storing key in ipad */
    os_memset(k_pad, 0, sizeof(k_pad));
    os_memcpy(k_pad, key, key_len);
    /* XOR key with ipad values */
    for (i = 0; i < 64; i++)
        k_pad[i] ^= 0x36;

    /* perform inner SHA256 */
    _addr[0] = k_pad;
    _len[0] = 64;
    for (i = 0; i < num_elem; i++)
    {
        _addr[i + 1] = addr[i];
        _len[i + 1] = len[i];
    }
    sha256_vector(1 + num_elem, _addr, _len, mac);

    os_memset(k_pad, 0, sizeof(k_pad));
    os_memcpy(k_pad, key, key_len);
    /* XOR key with opad values */
    for (i = 0; i < 64; i++)
        k_pad[i] ^= 0x5c;

    /* perform outer SHA256 */
    _addr[0] = k_pad;
    _len[0] = 64;
    _addr[1] = mac;
    _len[1] = SHA256_MAC_LEN;
    sha256_vector(2, _addr, _len, mac);
}


/**
 * hmac_sha256 - HMAC-SHA256 over data buffer (RFC 2104)
 * @key: Key for HMAC operations
 * @key_len: Length of the key in bytes
 * @data: Pointers to the data area
 * @data_len: Length of the data area
 * @mac: Buffer for the hash (20 bytes)
 */
void hmac_sha256(const u8 *key, size_t key_len, const u8 *data,
                 size_t data_len, u8 *mac)
{
    hmac_sha256_vector(key, key_len, 1, &data, &data_len, mac);
}


/**
 * sha256_prf - SHA256-based Pseudo-Random Function (IEEE 802.11r, 8.5.1.5.2)
 * @key: Key for PRF
 * @key_len: Length of the key in bytes
 * @label: A unique label for each purpose of the PRF
 * @data: Extra data to bind into the key
 * @data_len: Length of the data
 * @buf: Buffer for the generated pseudo-random key
 * @buf_len: Number of bytes of key to generate
 *
 * This function is used to derive new, cryptographically separate keys from a
 * given key.
 */
void sha256_prf(const u8 *key, size_t key_len, const char *label,
                const u8 *data, size_t data_len, u8 *buf, size_t buf_len)
{
    u16 counter = 1;
    size_t pos, plen;
    u8 hash[SHA256_MAC_LEN];
    const u8 *addr[4];
    size_t len[4];
    u8 counter_le[2], length_le[2];

    addr[0] = counter_le;
    len[0] = 2;
    addr[1] = (u8 *) label;
    len[1] = os_strlen(label);
    addr[2] = data;
    len[2] = data_len;
    addr[3] = length_le;
    len[3] = sizeof(length_le);

    WPA_PUT_LE16(length_le, buf_len * 8);
    pos = 0;
    while (pos < buf_len)
    {
        plen = buf_len - pos;
        WPA_PUT_LE16(counter_le, counter);
        if (plen >= SHA256_MAC_LEN)
        {
            hmac_sha256_vector(key, key_len, 4, addr, len,
                               &buf[pos]);
            pos += SHA256_MAC_LEN;
        }
        else
        {
            hmac_sha256_vector(key, key_len, 4, addr, len, hash);
            os_memcpy(&buf[pos], hash, plen);
            break;
        }
        counter++;
    }
}
#endif // no hmac

#ifdef INTERNAL_SHA256

struct sha256_state
{
    u64 length;
    u32 state[8], curlen;
    u8 buf[64];
};

/*static */void sha256_init(struct sha256_state *md);
/*static */int sha256_process(struct sha256_state *md, const unsigned char *in,
                              unsigned long inlen);
/*static */int sha256_done(struct sha256_state *md, unsigned char *out);


/**
 * sha256_vector - SHA256 hash for data vector
 * @num_elem: Number of elements in the data vector
 * @addr: Pointers to the data areas
 * @len: Lengths of the data blocks
 * @mac: Buffer for the hash
*/
void sha256_vector(size_t num_elem, const u8 *addr[], const size_t *len,
                   u8 *mac)
{
    struct sha256_state ctx;
    size_t i;

    sha256_init(&ctx);
    for (i = 0; i < num_elem; i++)
        sha256_process(&ctx, addr[i], (unsigned long)len[i]);
    sha256_done(&ctx, mac);

    os_memset( &ctx, 0, sizeof( struct sha256_state ) );
}

/* ===== start - public domain SHA256 implementation ===== */

/* This is based on SHA256 implementation in LibTomCrypt that was released into
 * public domain by Tom St Denis. */

/* Hash small code define generates ~2.1 k smaller code at a cost of slower hash */
#ifdef HASH_SMALL_CODE

/* the K array */
static const unsigned long K[64] =
{
    0x428a2f98UL, 0x71374491UL, 0xb5c0fbcfUL, 0xe9b5dba5UL, 0x3956c25bUL,
    0x59f111f1UL, 0x923f82a4UL, 0xab1c5ed5UL, 0xd807aa98UL, 0x12835b01UL,
    0x243185beUL, 0x550c7dc3UL, 0x72be5d74UL, 0x80deb1feUL, 0x9bdc06a7UL,
    0xc19bf174UL, 0xe49b69c1UL, 0xefbe4786UL, 0x0fc19dc6UL, 0x240ca1ccUL,
    0x2de92c6fUL, 0x4a7484aaUL, 0x5cb0a9dcUL, 0x76f988daUL, 0x983e5152UL,
    0xa831c66dUL, 0xb00327c8UL, 0xbf597fc7UL, 0xc6e00bf3UL, 0xd5a79147UL,
    0x06ca6351UL, 0x14292967UL, 0x27b70a85UL, 0x2e1b2138UL, 0x4d2c6dfcUL,
    0x53380d13UL, 0x650a7354UL, 0x766a0abbUL, 0x81c2c92eUL, 0x92722c85UL,
    0xa2bfe8a1UL, 0xa81a664bUL, 0xc24b8b70UL, 0xc76c51a3UL, 0xd192e819UL,
    0xd6990624UL, 0xf40e3585UL, 0x106aa070UL, 0x19a4c116UL, 0x1e376c08UL,
    0x2748774cUL, 0x34b0bcb5UL, 0x391c0cb3UL, 0x4ed8aa4aUL, 0x5b9cca4fUL,
    0x682e6ff3UL, 0x748f82eeUL, 0x78a5636fUL, 0x84c87814UL, 0x8cc70208UL,
    0x90befffaUL, 0xa4506cebUL, 0xbef9a3f7UL, 0xc67178f2UL
};
#endif

/* Various logical functions */
#define RORc(x, y) \
( ((((unsigned long) (x) & 0xFFFFFFFFUL) >> (unsigned long) ((y) & 31)) | \
   ((unsigned long) (x) << (unsigned long) (32 - ((y) & 31)))) & 0xFFFFFFFFUL)
#define Ch(x,y,z)       (z ^ (x & (y ^ z)))
#define Maj(x,y,z)      (((x | y) & z) | (x & y))
#define S(x, n)         RORc((x), (n))
#define R(x, n)         (((x)&0xFFFFFFFFUL)>>(n))
#define Sigma0(x)       (S(x, 2) ^ S(x, 13) ^ S(x, 22))
#define Sigma1(x)       (S(x, 6) ^ S(x, 11) ^ S(x, 25))
#define Gamma0(x)       (S(x, 7) ^ S(x, 18) ^ R(x, 3))
#define Gamma1(x)       (S(x, 17) ^ S(x, 19) ^ R(x, 10))
#ifndef MIN
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#endif

/* compress 512-bits */
static int sha256_compress(struct sha256_state *md, unsigned char *buf)
{
#ifdef HASH_SMALL_CODE
    u32 S[8], W[64], t0, t1;
    u32 t;
    int i;

    /* copy state into S */
    for (i = 0; i < 8; i++)
    {
        S[i] = md->state[i];
    }

    /* copy the state into 512-bits into W[0..15] */
    for (i = 0; i < 16; i++)
        W[i] = WPA_GET_BE32(buf + (4 * i));

    /* fill W[16..63] */
    for (i = 16; i < 64; i++)
    {
        W[i] = Gamma1(W[i - 2]) + W[i - 7] + Gamma0(W[i - 15]) +
               W[i - 16];
    }

    /* Compress */
#define RND(a,b,c,d,e,f,g,h,i)                          \
    t0 = h + Sigma1(e) + Ch(e, f, g) + K[i] + W[i]; \
    t1 = Sigma0(a) + Maj(a, b, c);          \
    d += t0;                    \
    h  = t0 + t1;

    for (i = 0; i < 64; ++i)
    {
        RND(S[0], S[1], S[2], S[3], S[4], S[5], S[6], S[7], i);
        t = S[7];
        S[7] = S[6];
        S[6] = S[5];
        S[5] = S[4];
        S[4] = S[3];
        S[3] = S[2];
        S[2] = S[1];
        S[1] = S[0];
        S[0] = t;
    }

    /* feedback */
    for (i = 0; i < 8; i++)
    {
        md->state[i] = md->state[i] + S[i];
    }

#else // fast code
    u32 temp1, temp2, W[64];
    u32 A, B, C, D, E, F, G, H;

#define GET_UINT32_BE(n,b,i)                       \
{                                                  \
    (n) = ( (u32) (b)[(i)    ] << 24 )             \
        | ( (u32) (b)[(i) + 1] << 16 )             \
        | ( (u32) (b)[(i) + 2] <<  8 )             \
        | ( (u32) (b)[(i) + 3]       );            \
}

    GET_UINT32_BE( W[ 0], buf,  0 );
    GET_UINT32_BE( W[ 1], buf,  4 );
    GET_UINT32_BE( W[ 2], buf,  8 );
    GET_UINT32_BE( W[ 3], buf, 12 );
    GET_UINT32_BE( W[ 4], buf, 16 );
    GET_UINT32_BE( W[ 5], buf, 20 );
    GET_UINT32_BE( W[ 6], buf, 24 );
    GET_UINT32_BE( W[ 7], buf, 28 );
    GET_UINT32_BE( W[ 8], buf, 32 );
    GET_UINT32_BE( W[ 9], buf, 36 );
    GET_UINT32_BE( W[10], buf, 40 );
    GET_UINT32_BE( W[11], buf, 44 );
    GET_UINT32_BE( W[12], buf, 48 );
    GET_UINT32_BE( W[13], buf, 52 );
    GET_UINT32_BE( W[14], buf, 56 );
    GET_UINT32_BE( W[15], buf, 60 );

#define  SHR(x,n) ((x & 0xFFFFFFFF) >> n)
#define ROTR(x,n) (SHR(x,n) | (x << (32 - n)))

#define S0(x) (ROTR(x, 7) ^ ROTR(x,18) ^  SHR(x, 3))
#define S1(x) (ROTR(x,17) ^ ROTR(x,19) ^  SHR(x,10))

#define S2(x) (ROTR(x, 2) ^ ROTR(x,13) ^ ROTR(x,22))
#define S3(x) (ROTR(x, 6) ^ ROTR(x,11) ^ ROTR(x,25))

#define F0(x,y,z) ((x & y) | (z & (x | y)))
#define F1(x,y,z) (z ^ (x & (y ^ z)))

#define R2(t)                                   \
(                                               \
    W[t] = S1(W[t -  2]) + W[t -  7] +          \
       S0(W[t - 15]) + W[t - 16]            \
)

#define P(a,b,c,d,e,f,g,h,x,K)                  \
{                                               \
    temp1 = h + S3(e) + F1(e,f,g) + K + x;      \
    temp2 = S2(a) + F0(a,b,c);                  \
    d += temp1; h = temp1 + temp2;              \
}

    A = md->state[0];
    B = md->state[1];
    C = md->state[2];
    D = md->state[3];
    E = md->state[4];
    F = md->state[5];
    G = md->state[6];
    H = md->state[7];

    P( A, B, C, D, E, F, G, H, W[ 0], 0x428A2F98 );
    P( H, A, B, C, D, E, F, G, W[ 1], 0x71374491 );
    P( G, H, A, B, C, D, E, F, W[ 2], 0xB5C0FBCF );
    P( F, G, H, A, B, C, D, E, W[ 3], 0xE9B5DBA5 );
    P( E, F, G, H, A, B, C, D, W[ 4], 0x3956C25B );
    P( D, E, F, G, H, A, B, C, W[ 5], 0x59F111F1 );
    P( C, D, E, F, G, H, A, B, W[ 6], 0x923F82A4 );
    P( B, C, D, E, F, G, H, A, W[ 7], 0xAB1C5ED5 );
    P( A, B, C, D, E, F, G, H, W[ 8], 0xD807AA98 );
    P( H, A, B, C, D, E, F, G, W[ 9], 0x12835B01 );
    P( G, H, A, B, C, D, E, F, W[10], 0x243185BE );
    P( F, G, H, A, B, C, D, E, W[11], 0x550C7DC3 );
    P( E, F, G, H, A, B, C, D, W[12], 0x72BE5D74 );
    P( D, E, F, G, H, A, B, C, W[13], 0x80DEB1FE );
    P( C, D, E, F, G, H, A, B, W[14], 0x9BDC06A7 );
    P( B, C, D, E, F, G, H, A, W[15], 0xC19BF174 );
    P( A, B, C, D, E, F, G, H, R2(16), 0xE49B69C1 );
    P( H, A, B, C, D, E, F, G, R2(17), 0xEFBE4786 );
    P( G, H, A, B, C, D, E, F, R2(18), 0x0FC19DC6 );
    P( F, G, H, A, B, C, D, E, R2(19), 0x240CA1CC );
    P( E, F, G, H, A, B, C, D, R2(20), 0x2DE92C6F );
    P( D, E, F, G, H, A, B, C, R2(21), 0x4A7484AA );
    P( C, D, E, F, G, H, A, B, R2(22), 0x5CB0A9DC );
    P( B, C, D, E, F, G, H, A, R2(23), 0x76F988DA );
    P( A, B, C, D, E, F, G, H, R2(24), 0x983E5152 );
    P( H, A, B, C, D, E, F, G, R2(25), 0xA831C66D );
    P( G, H, A, B, C, D, E, F, R2(26), 0xB00327C8 );
    P( F, G, H, A, B, C, D, E, R2(27), 0xBF597FC7 );
    P( E, F, G, H, A, B, C, D, R2(28), 0xC6E00BF3 );
    P( D, E, F, G, H, A, B, C, R2(29), 0xD5A79147 );
    P( C, D, E, F, G, H, A, B, R2(30), 0x06CA6351 );
    P( B, C, D, E, F, G, H, A, R2(31), 0x14292967 );
    P( A, B, C, D, E, F, G, H, R2(32), 0x27B70A85 );
    P( H, A, B, C, D, E, F, G, R2(33), 0x2E1B2138 );
    P( G, H, A, B, C, D, E, F, R2(34), 0x4D2C6DFC );
    P( F, G, H, A, B, C, D, E, R2(35), 0x53380D13 );
    P( E, F, G, H, A, B, C, D, R2(36), 0x650A7354 );
    P( D, E, F, G, H, A, B, C, R2(37), 0x766A0ABB );
    P( C, D, E, F, G, H, A, B, R2(38), 0x81C2C92E );
    P( B, C, D, E, F, G, H, A, R2(39), 0x92722C85 );
    P( A, B, C, D, E, F, G, H, R2(40), 0xA2BFE8A1 );
    P( H, A, B, C, D, E, F, G, R2(41), 0xA81A664B );
    P( G, H, A, B, C, D, E, F, R2(42), 0xC24B8B70 );
    P( F, G, H, A, B, C, D, E, R2(43), 0xC76C51A3 );
    P( E, F, G, H, A, B, C, D, R2(44), 0xD192E819 );
    P( D, E, F, G, H, A, B, C, R2(45), 0xD6990624 );
    P( C, D, E, F, G, H, A, B, R2(46), 0xF40E3585 );
    P( B, C, D, E, F, G, H, A, R2(47), 0x106AA070 );
    P( A, B, C, D, E, F, G, H, R2(48), 0x19A4C116 );
    P( H, A, B, C, D, E, F, G, R2(49), 0x1E376C08 );
    P( G, H, A, B, C, D, E, F, R2(50), 0x2748774C );
    P( F, G, H, A, B, C, D, E, R2(51), 0x34B0BCB5 );
    P( E, F, G, H, A, B, C, D, R2(52), 0x391C0CB3 );
    P( D, E, F, G, H, A, B, C, R2(53), 0x4ED8AA4A );
    P( C, D, E, F, G, H, A, B, R2(54), 0x5B9CCA4F );
    P( B, C, D, E, F, G, H, A, R2(55), 0x682E6FF3 );
    P( A, B, C, D, E, F, G, H, R2(56), 0x748F82EE );
    P( H, A, B, C, D, E, F, G, R2(57), 0x78A5636F );
    P( G, H, A, B, C, D, E, F, R2(58), 0x84C87814 );
    P( F, G, H, A, B, C, D, E, R2(59), 0x8CC70208 );
    P( E, F, G, H, A, B, C, D, R2(60), 0x90BEFFFA );
    P( D, E, F, G, H, A, B, C, R2(61), 0xA4506CEB );
    P( C, D, E, F, G, H, A, B, R2(62), 0xBEF9A3F7 );
    P( B, C, D, E, F, G, H, A, R2(63), 0xC67178F2 );

    md->state[0] += A;
    md->state[1] += B;
    md->state[2] += C;
    md->state[3] += D;
    md->state[4] += E;
    md->state[5] += F;
    md->state[6] += G;
    md->state[7] += H;

#endif

    return 0;
}


/* Initialize the hash state */
/*static */void sha256_init(struct sha256_state *md)
{
    md->curlen = 0;
    md->length = 0;
    md->state[0] = 0x6A09E667UL;
    md->state[1] = 0xBB67AE85UL;
    md->state[2] = 0x3C6EF372UL;
    md->state[3] = 0xA54FF53AUL;
    md->state[4] = 0x510E527FUL;
    md->state[5] = 0x9B05688CUL;
    md->state[6] = 0x1F83D9ABUL;
    md->state[7] = 0x5BE0CD19UL;
}

/**
   Process a block of memory though the hash
   @param md     The hash state
   @param in     The data to hash
   @param inlen  The length of the data (octets)
   @return CRYPT_OK if successful
*/
/*static */int sha256_process(struct sha256_state *md, const unsigned char *in,
                              unsigned long inlen)
{
    unsigned long n;
#define block_size 64

    if (md->curlen > sizeof(md->buf))
        return -1;

    while (inlen > 0)
    {
        if (md->curlen == 0 && inlen >= block_size)
        {
            if (sha256_compress(md, (unsigned char *) in) < 0)
                return -1;
            md->length += block_size * 8;
            in += block_size;
            inlen -= block_size;
        }
        else
        {
            n = MIN(inlen, (block_size - md->curlen));
            os_memcpy(md->buf + md->curlen, in, n);
            md->curlen += n;
            in += n;
            inlen -= n;
            if (md->curlen == block_size)
            {
                if (sha256_compress(md, md->buf) < 0)
                    return -1;
                md->length += 8 * block_size;
                md->curlen = 0;
            }
        }
    }

    return 0;
}


/**
   Terminate the hash to get the digest
   @param md  The hash state
   @param out [out] The destination of the hash (32 bytes)
   @return CRYPT_OK if successful
*/
/*static */int sha256_done(struct sha256_state *md, unsigned char *out)
{
#ifdef HASH_SMALL_CODE
    int i;
#endif
    if (md->curlen >= sizeof(md->buf))
        return -1;

    /* increase the length of the message */
    md->length += md->curlen * 8;

    /* append the '1' bit */
    md->buf[md->curlen++] = (unsigned char) 0x80;

    /* if the length is currently above 56 bytes we append zeros
     * then compress.  Then we can fall back to padding zeros and length
     * encoding like normal.
     */
    if (md->curlen > 56)
    {
        while (md->curlen < 64)
        {
            md->buf[md->curlen++] = (unsigned char) 0;
        }
        sha256_compress(md, md->buf);
        md->curlen = 0;
    }

    /* pad upto 56 bytes of zeroes */
    while (md->curlen < 56)
    {
        md->buf[md->curlen++] = (unsigned char) 0;
    }

    /* store length */
    WPA_PUT_BE64(md->buf + 56, md->length);
    sha256_compress(md, md->buf);

#ifdef HASH_SMALL_CODE
    /* copy output */
    for (i = 0; i < 8; i++)
        WPA_PUT_BE32(out + (4 * i), md->state[i]);
#else
    WPA_PUT_BE32(out + (0), md->state[0]);
    WPA_PUT_BE32(out + (4), md->state[1]);
    WPA_PUT_BE32(out + (8), md->state[2]);
    WPA_PUT_BE32(out + (12), md->state[3]);
    WPA_PUT_BE32(out + (16), md->state[4]);
    WPA_PUT_BE32(out + (20), md->state[5]);
    WPA_PUT_BE32(out + (24), md->state[6]);
    WPA_PUT_BE32(out + (28), md->state[7]);
#endif
    return 0;
}

/* ===== end - public domain SHA256 implementation ===== */

#endif /* INTERNAL_SHA256 */
