/*
 * wpa_supplicant/hostapd / common helper functions, etc.
 * Copyright (c) 2002-2007, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include <Uefi.h>
#include <Library/BaseMemoryLib.h>

#define INTERNAL_SHA256
#define INTERNAL_SHA1

#ifndef __size_t__
#define __size_t__
//typedef unsigned int            size_t;
typedef UINTN            size_t;
#endif // __size_t__

#define SHA256_MAC_LEN 32

typedef unsigned char  u8;
typedef unsigned int   u32;
typedef unsigned short u16;
typedef unsigned long long int  u64;

#ifndef os_memcpy
#define os_memcpy(d, s, n) CopyMem((d), (s), (n))
#endif
#ifndef os_memmove
#define os_memmove(d, s, n) CopyMem((d), (s), (n))
#endif
#ifndef os_memset
#define os_memset(s, c, n) SetMem(s, n, c)
#endif
#ifndef os_memcmp
#define os_memcmp(s1, s2, n) CompareMem((s1), (s2), (n))
#endif


#ifndef __must_check
#if __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4)
#define __must_check __attribute__((__warn_unused_result__))
#else
#define __must_check
#endif /* __GNUC__ */
#endif /* __must_check */

#define WPA_GET_BE16(a) ((u16) (((a)[0] << 8) | (a)[1]))
#define WPA_PUT_BE16(a, val)            \
    do {                    \
        (a)[0] = ((u16) (val)) >> 8;    \
        (a)[1] = ((u16) (val)) & 0xff;  \
    } while (0)

#define WPA_GET_LE16(a) ((u16) (((a)[1] << 8) | (a)[0]))
#define WPA_PUT_LE16(a, val)            \
    do {                    \
        (a)[1] = ((u16) (val)) >> 8;    \
        (a)[0] = ((u16) (val)) & 0xff;  \
    } while (0)

#define WPA_GET_BE24(a) ((((u32) (a)[0]) << 16) | (((u32) (a)[1]) << 8) | \
             ((u32) (a)[2]))
#define WPA_PUT_BE24(a, val)                    \
    do {                            \
        (a)[0] = (u8) ((((u32) (val)) >> 16) & 0xff);   \
        (a)[1] = (u8) ((((u32) (val)) >> 8) & 0xff);    \
        (a)[2] = (u8) (((u32) (val)) & 0xff);       \
    } while (0)

#define WPA_GET_BE32(a) ((((u32) (a)[0]) << 24) | (((u32) (a)[1]) << 16) | \
             (((u32) (a)[2]) << 8) | ((u32) (a)[3]))
#define WPA_PUT_BE32(a, val)                    \
    do {                            \
        (a)[0] = (u8) ((((u32) (val)) >> 24) & 0xff);   \
        (a)[1] = (u8) ((((u32) (val)) >> 16) & 0xff);   \
        (a)[2] = (u8) ((((u32) (val)) >> 8) & 0xff);    \
        (a)[3] = (u8) (((u32) (val)) & 0xff);       \
    } while (0)

#define WPA_GET_LE32(a) ((((u32) (a)[3]) << 24) | (((u32) (a)[2]) << 16) | \
             (((u32) (a)[1]) << 8) | ((u32) (a)[0]))
#define WPA_PUT_LE32(a, val)                    \
    do {                            \
        (a)[3] = (u8) ((((u32) (val)) >> 24) & 0xff);   \
        (a)[2] = (u8) ((((u32) (val)) >> 16) & 0xff);   \
        (a)[1] = (u8) ((((u32) (val)) >> 8) & 0xff);    \
        (a)[0] = (u8) (((u32) (val)) & 0xff);       \
    } while (0)

#define WPA_GET_BE64(a) ((((u64) (a)[0]) << 56) | (((u64) (a)[1]) << 48) | \
             (((u64) (a)[2]) << 40) | (((u64) (a)[3]) << 32) | \
             (((u64) (a)[4]) << 24) | (((u64) (a)[5]) << 16) | \
             (((u64) (a)[6]) << 8) | ((u64) (a)[7]))
#define WPA_PUT_BE64(a, val)                \
    do {                        \
        (a)[0] = (u8) (((u64) (val)) >> 56);    \
        (a)[1] = (u8) (((u64) (val)) >> 48);    \
        (a)[2] = (u8) (((u64) (val)) >> 40);    \
        (a)[3] = (u8) (((u64) (val)) >> 32);    \
        (a)[4] = (u8) (((u64) (val)) >> 24);    \
        (a)[5] = (u8) (((u64) (val)) >> 16);    \
        (a)[6] = (u8) (((u64) (val)) >> 8); \
        (a)[7] = (u8) (((u64) (val)) & 0xff);   \
    } while (0)

#define WPA_GET_LE64(a) ((((u64) (a)[7]) << 56) | (((u64) (a)[6]) << 48) | \
             (((u64) (a)[5]) << 40) | (((u64) (a)[4]) << 32) | \
             (((u64) (a)[3]) << 24) | (((u64) (a)[2]) << 16) | \
             (((u64) (a)[1]) << 8) | ((u64) (a)[0]))


