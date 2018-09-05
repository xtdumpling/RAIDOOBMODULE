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

#include <Base.h>
#include <Uefi.h>
#include <Library/DebugLib.h>
#include <Library/TraceHubLib.h>
#include <Library/MemoryAllocationLib.h>

#include "sventx.h"

/* SVEN "driver" dummy output routines */
static void sth_write_d32mts_dummy(psven_handle_t svenh, sven_u32_t v);
static void sth_write_d32ts_dummy(psven_handle_t svenh, sven_u32_t v);
static void sth_write_d8_dummy(psven_handle_t svenh, sven_u8_t v);
static void sth_write_d16_dummy(psven_handle_t svenh, sven_u16_t v);
static void sth_write_d32_dummy(psven_handle_t svenh, sven_u32_t v);
#if defined(SVEN_PCFG_ENABLE_64BIT_IO)
static void sth_write_d64_dummy(psven_handle_t svenh, sven_u64_t vp);
#endif
static void sth_write_flag_dummy(psven_handle_t svenh);

/* SVEN "driver" output routines */
static void sth_write_d32mts(psven_handle_t svenh, sven_u32_t v);
static void sth_write_d32ts(psven_handle_t svenh, sven_u32_t v);
static void sth_write_d8(psven_handle_t svenh, sven_u8_t v);
static void sth_write_d16(psven_handle_t svenh, sven_u16_t v);
static void sth_write_d32(psven_handle_t svenh, sven_u32_t v);
#if defined(SVEN_PCFG_ENABLE_64BIT_IO)
static void sth_write_d64(psven_handle_t svenh, sven_u64_t vp);
#endif
static void sth_write_flag(psven_handle_t svenh);

VOID *
mTraceHubMmioTraceAddress (
  IN  UINT32    Master,
  IN  UINT32    Channel
  )
{
  RETURN_STATUS Status;
  UINT32 TraceAddress;

  Status = TraceHubMmioTraceAddress ((UINT16)Master, (UINT16)Channel, &TraceAddress);
  if (RETURN_ERROR (Status)) {
    TraceAddress = 0;
  }

  return (VOID *) (UINTN)TraceAddress;
}

VOID *
EFIAPI
TraceHubAllocatePool (
  UINTN s
  )
{
  VOID *ptr = NULL;

#ifdef TRACE_HUB_SVENTX_LIB_IA32
  //
  // PEI
  //
  ptr = AllocatePool (s);
  ASSERT (ptr != NULL);
#else
  //
  // DXE SMM
  //
  ptr = AllocateRuntimePool (s);
  ASSERT (ptr != NULL);
#endif

  return ptr;
}

VOID
EFIAPI
TraceHubFreePool (
  VOID *p
  )
{
  FreePool (p);
}

/**
 * Platform specific SVEN handle initialization hook function
 *
 * @param svenh pointer to the new SVEN handle structure
 * @param platform_data user defined data for the init function.
 */
static void platform_HandleInit(psven_handle_t svenh, const void * platform_data)
{
    /* Initialize platform specific data in global SVEN state
     * This example just stores the platform_data, a real implementation
     * would put data into the handle structure that enables the output
     * routines to execute efficiently (pointer to STH memory etc).
     */
#if defined(SVEN_PCFG_ENABLE_PLATFORM_HANDLE_DATA)
    svenh->svh_platform.sph_init_data = platform_data;
#endif
    svenh->svh_platform.TraceHubPlatformData.Master = ((TRACE_HUB_PLATFORM_DATA *) platform_data)->Master;
    svenh->svh_platform.TraceHubPlatformData.Channel = ((TRACE_HUB_PLATFORM_DATA *) platform_data)->Channel;
}

/**
 * Platform specific SVEN handle initialization hook function
 *
 * @param svenh pointer to the new SVEN handle structure
 */
static void platform_HandleRelease(psven_handle_t svenh)
{
    /* Release any handle specific data or resources here.*/
}

/**
 * Platform specific global SVEN state initialization hook function
 *
 * @param svenh pointer to the new SVEN handle structure
 * @param platform_data user defined data for the init function.
 */
SVEN_EXPORT void SVEN_CALLCONV
sventx_PlatformInit(psven_header_t svenh, const void * platform_data)
{
    /* Set handle init hook that performs per SVEN handle initialization
     * and destruction
     */
#if defined(SVEN_PCFG_ENABLE_PLATFORM_HANDLE_DATA)
    svenh->svh_inith    = platform_HandleInit;
    svenh->svh_releaseh = platform_HandleRelease;
#endif

    /* Initialize platform specific data in global SVEN state
     * This platform example puts its STH low level output function
     * pointers here. A real implementation may have these "inlined"
     * for performance reasons.
     */
#if defined(SVEN_PCFG_ENABLE_PLATFORM_STATE_DATA)
    svenh->svh_platform.sps_some_val = platform_data;

    svenh->svh_platform.write_d32ts  = sth_write_d32ts;
    svenh->svh_platform.write_d32mts = sth_write_d32mts;
    svenh->svh_platform.write_d8     = sth_write_d8;
    svenh->svh_platform.write_d16    = sth_write_d16;
    svenh->svh_platform.write_d32    = sth_write_d32;
#if defined(SVEN_PCFG_ENABLE_64BIT_IO)
    svenh->svh_platform.write_d64    = sth_write_d64;
#endif
    svenh->svh_platform.write_flag   = sth_write_flag;
#endif

    if (mTraceHubMmioTraceAddress (0xFF, 0xFF) == 0x0) {
#if defined(SVEN_PCFG_ENABLE_PLATFORM_STATE_DATA)
      svenh->svh_platform.sps_some_val = platform_data;

      svenh->svh_platform.write_d32ts  = sth_write_d32ts_dummy;
      svenh->svh_platform.write_d32mts = sth_write_d32mts_dummy;
      svenh->svh_platform.write_d8     = sth_write_d8_dummy;
      svenh->svh_platform.write_d16    = sth_write_d16_dummy;
      svenh->svh_platform.write_d32    = sth_write_d32_dummy;
#if defined(SVEN_PCFG_ENABLE_64BIT_IO)
      svenh->svh_platform.write_d64    = sth_write_d64_dummy;
#endif
      svenh->svh_platform.write_flag   = sth_write_flag_dummy;
#endif
    }
}

SVEN_EXPORT void SVEN_CALLCONV
sventx_PlatformDestroy(psven_header_t svenh)
{
}

/**
 * This example platform uses SVENTX as a shared library inside an
 * application. The platform init hook is called during a shared library
 * constructor call.
 */
EFI_STATUS
EFIAPI
shared_library_init (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
    /* Initialize SVEN infrastructure
     * This must be done once at platform startup.
     * The parameters are the platform specific initialization function and
     * the data that gets passed to it.
     */
    SVEN_INIT(sventx_PlatformInit, (void*)(UINTN)42);
    return EFI_SUCCESS;
}

/**
 * This example platform  uses SVENTX as a shared library inside an
 * application. The platform destroy hook is called during a shared library
 * destructor call.
 */
EFI_STATUS
EFIAPI
shared_library_exit (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
    /* run platform shutdown code */
    SVEN_SHUTDOWN(sventx_PlatformDestroy);
    return EFI_SUCCESS;
}

/*  Dummy SVEN driver output routines that just print their operation.
 */
static void sth_write_d32mts_dummy(psven_handle_t svenh, sven_u32_t v)
{
    /* d32mts == Short SVEN header, reset io cnt
     */
#if defined(SVEN_PCFG_ENABLE_PLATFORM_HANDLE_DATA)
    svenh->svh_platform.sph_io_cnt = 0;
#endif
}

static void sth_write_d32ts_dummy(psven_handle_t svenh, sven_u32_t v)
{
    /* d32ts == SVEN header, reset io cnt
     */
#if defined(SVEN_PCFG_ENABLE_PLATFORM_HANDLE_DATA)
    svenh->svh_platform.sph_io_cnt = 0;
#endif
}
static void sth_write_d8_dummy(psven_handle_t svenh, sven_u8_t v)
{
}

static void sth_write_d16_dummy(psven_handle_t svenh, sven_u16_t v)
{
}

static void sth_write_d32_dummy(psven_handle_t svenh, sven_u32_t v)
{
}

#if defined(SVEN_PCFG_ENABLE_64BIT_IO)
static void sth_write_d64_dummy(psven_handle_t svenh, sven_u64_t v)
{
}
#endif

static void sth_write_flag_dummy(psven_handle_t svenh)
{
}

/*  Actual SVEN driver output routine for UEFI
 */
static void sth_write_d32mts(psven_handle_t svenh, sven_u32_t v)
{
    /* d32mts == Short SVEN header, reset io cnt
     */
  void *ptr;

  ptr = mTraceHubMmioTraceAddress (svenh->svh_platform.TraceHubPlatformData.Master,
                                   svenh->svh_platform.TraceHubPlatformData.Channel);
  if (ptr != ((void *) 0))
  {
    *((sven_u32_t *)((UINT8 *)(ptr) + 0x18)) = v;
  }
}

static void sth_write_d32ts(psven_handle_t svenh, sven_u32_t v)
{
    /* d32ts == SVEN header, reset io cnt
     */
  void *ptr;

  ptr = mTraceHubMmioTraceAddress (svenh->svh_platform.TraceHubPlatformData.Master,
                                   svenh->svh_platform.TraceHubPlatformData.Channel);
  if (ptr != ((void *) 0))
  {
    *((sven_u32_t *)((UINT8 *)(ptr) + 0x10)) = v;
  }
}
static void sth_write_d8(psven_handle_t svenh, sven_u8_t v)
{
  void *ptr;

  ptr = mTraceHubMmioTraceAddress (svenh->svh_platform.TraceHubPlatformData.Master,
                                   svenh->svh_platform.TraceHubPlatformData.Channel);
  if (ptr != ((void *) 0))
  {
    *((sven_u8_t *)((UINT8 *)(ptr) + 0x0)) = v;
  }
}

static void sth_write_d16(psven_handle_t svenh, sven_u16_t v)
{
  void *ptr;

  ptr = mTraceHubMmioTraceAddress (svenh->svh_platform.TraceHubPlatformData.Master,
                                   svenh->svh_platform.TraceHubPlatformData.Channel);
  if (ptr != ((void *) 0))
  {
    *((sven_u16_t *)((UINT8 *)(ptr) + 0x0)) = v;
  }
}

static void sth_write_d32(psven_handle_t svenh, sven_u32_t v)
{
  void *ptr;

  ptr = mTraceHubMmioTraceAddress (svenh->svh_platform.TraceHubPlatformData.Master,
                                   svenh->svh_platform.TraceHubPlatformData.Channel);
  if (ptr != ((void *) 0))
  {
    *((sven_u32_t *)((UINT8 *)(ptr) + 0x0)) = v;
  }
}

#if defined(SVEN_PCFG_ENABLE_64BIT_IO)
static void sth_write_d64(psven_handle_t svenh, sven_u64_t v)
{
  void *ptr;

  ptr = mTraceHubMmioTraceAddress (svenh->svh_platform.TraceHubPlatformData.Master,
                                   svenh->svh_platform.TraceHubPlatformData.Channel);
  if (ptr != ((void *) 0))
  {
    *((sven_u64_t *)((UINT8 *)(ptr) + 0x0)) = v;
  }
}
#endif

static void sth_write_flag(psven_handle_t svenh)
{
  void *ptr;

  ptr = mTraceHubMmioTraceAddress (svenh->svh_platform.TraceHubPlatformData.Master,
                                   svenh->svh_platform.TraceHubPlatformData.Channel);
  if (ptr != ((void *) 0))
  {
    *((sven_u32_t *)((UINT8 *)(ptr) + 0x30)) = 0;
  }
}

