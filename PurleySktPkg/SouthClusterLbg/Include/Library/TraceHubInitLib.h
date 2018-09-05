/** @file
  Header file for TraceHub Init Lib.
  All function in this library is available for PEI, DXE, and SMM,
  But do not support UEFI RUNTIME environment call.

@copyright
 Copyright (c) 2014 Intel Corporation. All rights reserved
 This software and associated documentation (if any) is furnished
 under a license and may only be used or copied in accordance
 with the terms of the license. Except as permitted by the
 license, no part of this software or documentation may be
 reproduced, stored in a retrieval system, or transmitted in any
 form or by any means without the express written consent of
 Intel Corporation.
 This file contains an 'Intel Peripheral Driver' and is uniquely
 identified as "Intel Reference Module" and is licensed for Intel
 CPUs and chipsets under the terms of your license agreement with
 Intel or your vendor. This file may be modified by the user, subject
 to additional terms of the license agreement.

@par Specification Reference:
**/
#ifndef _TRACE_HUB_INIT_LIB_H_
#define _TRACE_HUB_INIT_LIB_H_

#include <Uefi/UefiBaseType.h>
#include <PchAccess.h>

///
/// The PCH_TRACE_HUB_ENABLE_MODE describes the desired TraceHub mode of operation
///
typedef enum {
  TraceHubModeDisabled       = 0,   ///< Pch TraceHub Disabled
  TraceHubModeHostDebugger   = 2,   ///< Pch TraceHub External Debugger Present
  TraceHubModeMax
} TRACE_HUB_ENABLE_MODE;

///
/// The PCH_TRACE_HUB_TRACE_DESTINATION block describes the desired TraceHub output 
///
typedef enum {
   TraceHubTraceDestMemoryReg0 = 0,
   TraceHubTraceDestUsb3       = 1,
   TraceHubTraceDestBssb       = 2,
   TraceHubTraceDestMax
} TRACE_HUB_TRACE_DESTINATION;

/**
  This function performs basic initialization for TraceHub
  This routine will consume address range 0xFE0C0000 - 0xFE3FFFFF for BARs usage.
  Although controller allows access to a 64bit address resource, PEI phase is a 32bit env,
  addresses greater than 4G is not allowed by CPU address space.
  So, the addresses must be limited to below 4G and UBARs should be set to 0.
  If this routine is called by platform code, it is expected EnableMode is passed in as PchTraceHubModeDisabled, 
  relying on the Intel TH debugger to enable it through the "cratchpad0 bit [24]".
  By this practice, it gives the validation team the capability to use single debug BIOS 
  to validate the early trace functionality and code path that enable/disable Intel TH using BIOS policy.

  @param[in] EnableMode                 Trace Hub Enable Mode
  @param[in] Destination                Trace Hub Trace Destination

**/
VOID
TraceHubInitialize (
  IN  UINT8                             EnableMode
 );

#endif // _TRACE_HUB_INIT_LIB_H_
