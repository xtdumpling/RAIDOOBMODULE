/*

  Copyright(c) 2012-2014 Intel Corporation. All rights reserved.
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:

    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in
      the documentation and/or other materials provided with the
      distribution.
    * Neither the name of Intel Corporation nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

/* Example platform specific extensions
 * This "platform" shows how to use the SVEN platform modules
 * to configure the SVEN feature set and add platform specific
 * customizations.
 */

#ifndef SVEN_PLATFORM_INCLUDED
#define SVEN_PLATFORM_INCLUDED

#include "sventx_platform_config.h"

#if defined(SVEN_PCFG_ENABLE_HEAP_MEMORY)
VOID *
EFIAPI
TraceHubAllocatePool (
  UINTN s
  );

VOID
EFIAPI
TraceHubFreePool (
  VOID *p
  );
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Platform specific SVEN global state extension
 *
 * The contents of this structure can be freely defined to
 * match platform specific data needs. It can later be
 * accessed through the sven_header svh_platform member.
 *
 * This platform example puts STH low-level output function pointers
 * here. Real implementations may have them "inlined" for performance
 * reasons.
 */
typedef struct s_sven_platform_state {
    void (*write_d32ts)(struct s_sven_handle * svenh, sven_u32_t v);
    void (*write_d32mts)(struct s_sven_handle * svenh, sven_u32_t v);
    void (*write_d8)(struct s_sven_handle * svenh, sven_u8_t v);
    void (*write_d16)(struct s_sven_handle * svenh, sven_u16_t v);
    void (*write_d32)(struct s_sven_handle * svenh, sven_u32_t v);
#if defined(SVEN_PCFG_ENABLE_64BIT_IO)
    void (*write_d64)(struct s_sven_handle * svenh, sven_u64_t v);
#endif
    void (*write_flag)(struct s_sven_handle * svenh);

    const void *  sps_some_val;
} sven_platform_state_t, *psven_platform_state_t;

extern SVEN_EXPORT void SVEN_CALLCONV
sventx_PlatformInit(struct s_sven_header *, const void *);
extern SVEN_EXPORT void SVEN_CALLCONV
sventx_PlatformDestroy(struct s_sven_header * svenh);

typedef struct _TRACE_HUB_PLATFORM_DATA
{
  UINT32    Master;
  UINT32    Channel;
} TRACE_HUB_PLATFORM_DATA;

/**
 * Platform specific SVEN handle state extension
 *
 * The contents of this structure can be freely defined to
 * match platform specific data needs. It can later be
 * accessed through the sven_handles's svh_platform member.
 *
 * @see SVEN_PCFG_ENABLE_PLATFORM_HANDLE_DATA sven_handle_t
 */
typedef struct s_sven_platform_handle {
	const void*sph_init_data;/**< data passed from init call               */
    sven_u32_t sph_io_cnt;   /**< hello example, cnt io's for pretty print */
    TRACE_HUB_PLATFORM_DATA TraceHubPlatformData;
} sven_platform_handle_t, *psven_platform_handle_t;


#if defined(SVEN_PCFG_ENABLE_HEAP_MEMORY)
/**
 * Map malloc style heap allocation function to SVENTX macro.
 * This function is used for handle allocations if heap usage
 * is supported by the platform.
 *
 * @param s number of bytes to allocate
 * @see
 */
#define SVEN_HEAP_MALLOC(s) TraceHubAllocatePool(s)
#define SVEN_HEAP_FREE(p)   TraceHubFreePool(p)
#endif

/* STH IO output routine mapping
 * Call the function pointers in the global SVEN state
 */
#if defined(SVEN_PCFG_ENABLE_PLATFORM_STATE_DATA)
#define SVEN_STH_OUT_D32TS(sven_handle, data) \
    (sven_handle)->svh_header->svh_platform.write_d32ts((sven_handle), (data))
#define SVEN_STH_OUT_D32MTS(sven_handle, data) \
    (sven_handle)->svh_header->svh_platform.write_d32mts((sven_handle), (data))
#define SVEN_STH_OUT_D8(sven_handle, data) \
    (sven_handle)->svh_header->svh_platform.write_d8((sven_handle), (data))
#define SVEN_STH_OUT_D16(sven_handle, data) \
    (sven_handle)->svh_header->svh_platform.write_d16((sven_handle), (data))
#define SVEN_STH_OUT_D32(sven_handle, data) \
    (sven_handle)->svh_header->svh_platform.write_d32((sven_handle), (data))
#if defined(SVEN_PCFG_ENABLE_64BIT_IO)
#define SVEN_STH_OUT_D64(sven_handle, data) \
    (sven_handle)->svh_header->svh_platform.write_d64((sven_handle), (data))
#endif
#define SVEN_STH_OUT_FLAG(sven_handle) \
    (sven_handle)->svh_header->svh_platform.write_flag((sven_handle))
#else
#define SVEN_STH_OUT_D32TS(sven_handle, data)
#define SVEN_STH_OUT_D8(sven_handle, data)
#define SVEN_STH_OUT_D16(sven_handle, data)
#define SVEN_STH_OUT_D32(sven_handle, data)
#if defined(SVEN_PCFG_ENABLE_64BIT_IO)
#define SVEN_STH_OUT_D64(sven_handle, data)
#endif
#define SVEN_STH_OUT_FLAG(sven_handle)
#endif // SVEN_PCFG_ENABLE_PLATFORM_STATE_DATA

#endif

#if defined(SVEN_UNIT_TEST)
#define SVEN_UNIT_TEST_EXAMPLE
#endif

#ifdef __cplusplus
} /* extern C */
#endif


