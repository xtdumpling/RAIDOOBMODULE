//****************************************************************************
//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//****************************************************************************
//  File History
//
//  Rev. 1.00
//      Bug Fixed:  Fix compile redefine issue. (after label 44 beta)
//      Reason:     
//      Auditor:    Kasber Chen
//      Date:       Jul/28/2016
//
//*****************************************************************************
//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/** @file
  This library class provides Mrc Oem Hooks Ppi wrapper.

  Copyright (c) 2011 - 2016, Intel Corporation. All rights reserved.
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/

#ifndef MINIBIOS_BUILD
#include <Uefi.h>
#include <Library/OemProcMemInit.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include "Ppi/MrcOemHooksPpi.h"
#include "Guid/SocketPciResourceData.h"
#include "Ppi/ReadOnlyVariable2.h"

#define MRC_OEM_HOOKS_PPI_FUNC_CALL(Host, FunctionName) {                                               \
  if (IsMrcOemHooksPpiReady (Host)) {                                                                   \
    Host->MrcOemHooksPpi->FunctionName (Host);                                                          \
  }                                                                                                     \
}

#define MRC_OEM_HOOKS_PPI_FUNC_CALL_RET(Host, FunctionName, ReturnName) {                               \
  if (IsMrcOemHooksPpiReady (Host)) {                                                                   \
    ReturnName = Host->MrcOemHooksPpi->FunctionName (Host);                                             \
  }                                                                                                     \
}

#define MRC_OEM_HOOKS_PPI_FUNC_CALL_PARA1(Host, FunctionName, Parameter1) {                             \
  if (IsMrcOemHooksPpiReady (Host)) {                                                                   \
    Host->MrcOemHooksPpi->FunctionName (Host, Parameter1);                                              \
  }                                                                                                     \
}

#define MRC_OEM_HOOKS_PPI_FUNC_CALL_PARA1_RET(Host, FunctionName, Parameter1, ReturnName) {             \
  if (IsMrcOemHooksPpiReady (Host)) {                                                                   \
    ReturnName = Host->MrcOemHooksPpi->FunctionName (Host, Parameter1);                                 \
  }                                                                                                     \
}

#define MRC_OEM_HOOKS_PPI_FUNC_CALL_PARA3(Host, FunctionName, Parameter1, Parameter2, Parameter3) {     \
  if (IsMrcOemHooksPpiReady (Host)) {                                                                   \
    Host->MrcOemHooksPpi->FunctionName (Host, Parameter1, Parameter2, Parameter3);                      \
  }                                                                                                     \
}

extern EFI_GUID gEfiPrevBootNGNDimmCfgVariableGuid;
extern EFI_GUID gEfiSocketPciResourceDataGuid;
#endif // MINIBIOS_BUILD
#if SMCPKG_SUPPORT == 0
// Define intrinsic functions to satisfy the .NET 2008 compiler with size optimization /O1
// compiler automatically inserts memcpy/memset fn calls in certain cases
void *memcpy(void *dst, void *src, size_t cnt);
void *memset(void *dst, char value, size_t cnt);
#ifndef __GNUC__
#pragma function(memcpy,memset)
#endif // __GNUC__

void *memcpy(void *dst, void *src, size_t cnt)
{
  MemCopy((UINT8 *)dst, (UINT8 *)src, (UINT32) cnt);
  return dst;
}

void *memset(void *dst, char value, size_t cnt)
{
  MemSetLocal((UINT8 *)dst, (UINT32) value, (UINT32) cnt);
  return dst;
}
#endif	//SMCPKG_SUPPORT == 0
#ifndef MINIBIOS_BUILD
/**

  MRC wrapper code.
  Check Whether Mrc Oem Hooks Ppi initialized or not

    @param Host = pointer to sysHost structure

**/
BOOLEAN
IsMrcOemHooksPpiReady (
    PSYSHOST Host
  )
{
  BOOLEAN Result = FALSE;

  if((Host != NULL) && (Host->MrcOemHooksPpi != NULL)) {
    Result = TRUE;
  } else {
    DebugPrintRc (Host, SDBG_MIN, "MrcOemHooksPpi used before available!\n");
  }
  return Result;
}

/**

    Routine Description:

      Get and Install Mrc Oem Hooks Ppi pointer into SysHost .

    Arguments:

      Host    - Pointer to SysHost structure.

    Returns:

      EFI_STATUS

**/

EFI_STATUS
CacheMrcOemHooksPpi (
  PSYSHOST Host
  )
{
  EFI_PEI_SERVICES  **PeiServices;
  MRC_OEM_HOOKS_PPI *MrcOemHooksPpi = NULL;
  EFI_STATUS        Status = EFI_NOT_FOUND;

  //
  // Initialize the OEM hooks PPI pointer in the Host structure.
  //
  if((Host != NULL) && (Host->MrcOemHooksPpi == NULL)) {
    //
    // Locate Mrc Oem Hooks Ppi
    //
    PeiServices = (EFI_PEI_SERVICES**) Host->var.common.oemVariable;
    Status = (*PeiServices)->LocatePpi (
      PeiServices,
      &gMrcOemHooksPpiGuid,
      0,
      NULL,
      (VOID **) &MrcOemHooksPpi
      );

    if (EFI_ERROR (Status)) {
      MrcOemHooksPpi = NULL;
      Status = EFI_NOT_FOUND;
    } else {
      Host->MrcOemHooksPpi = MrcOemHooksPpi;
      DebugPrintRc (Host, SDBG_MAX, "CacheMrcOemHooksPpi in HOST: Host->MrcOemHooksPpi = %08x\n", Host->MrcOemHooksPpi);
      Status = EFI_SUCCESS;
    }
  }

  return Status;
}
#endif // MINIBIOS_BUILD

/**

  RC wrapper code.
  Initialize Host structure with OEM specific setup data

    @param Host = pointer to sysHost structure

**/
VOID
OemInitializePlatformData (
    struct sysHost             *Host
  )
{
#ifndef MINIBIOS_BUILD
  EFI_STATUS Status = CacheMrcOemHooksPpi (Host);
  DebugPrintRc (Host, SDBG_MIN, "CacheMrcOemHooksPpi() status = %08x\n", Status);
  MRC_OEM_HOOKS_PPI_FUNC_CALL (Host, OemInitializePlatformData);
#endif // MINIBIOS_BUILD
}

/**

  OemPreMemoryInit  - OEM hook pre memory init

  @param Host  - Pointer to sysHost

**/

UINT32
OemPreMemoryInit (
  PSYSHOST Host
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
#ifndef MINIBIOS_BUILD
  MRC_OEM_HOOKS_PPI_FUNC_CALL_RET (Host, OemPreMemoryInit, Status);
#endif // MINIBIOS_BUILD
  return Status;
}

VOID
OemIssueReset(
  struct sysHost *Host,
  UINT8          ResetType
)
{
#ifndef MINIBIOS_BUILD
  MRC_OEM_HOOKS_PPI_FUNC_CALL_PARA1 (Host, OemIssueReset, ResetType);
#endif // MINIBIOS_BUILD
}

#ifdef SERIAL_DBG_MSG
/**

  Oem Init Serial Debug

    @param Host = pointer to sysHost structure
    KtiStatus - Return status from Rc code execution

    @retval VOID

**/
VOID
OemInitSerialDebug (
    PSYSHOST Host
    )
{
#ifndef MINIBIOS_BUILD
  MRC_OEM_HOOKS_PPI_FUNC_CALL (Host, OemInitSerialDebug);
#endif // MINIBIOS_BUILD
}
#endif  //  SERIAL_DBG_MSG

/**

  Rc wrapper code.
  Copy Host structure to system memory buffer after Rc when memory becomes available

    @param Host = pointer to sysHost structure
    RcStatus - Return status from Rc code execution

@retval VOID

**/
VOID
OemPostMemoryInit (
    struct sysHost   *Host,
    UINT32           RcStatus
  )

{
#ifndef MINIBIOS_BUILD
  MRC_OEM_HOOKS_PPI_FUNC_CALL_PARA1 (Host, OemPostMemoryInit, RcStatus);
#endif // MINIBIOS_BUILD
}

/**

  Proc wrapper code.

    @param Host = pointer to sysHost structure

    @retval VOID

**/
VOID
OemPreProcInit (
    struct sysHost   *Host
  )
{
#ifndef MINIBIOS_BUILD
  MRC_OEM_HOOKS_PPI_FUNC_CALL (Host, OemPreProcInit);
#endif // MINIBIOS_BUILD
}

/**

  Proc wrapper code.

    @param Host = pointer to sysHost structure
    ProcStatus - Return status from Proc RC code execution

    @retval VOID

**/
VOID
OemPostProcInit (
    struct sysHost   *Host,
    UINT32           ProcStatus
  )
{
#ifndef MINIBIOS_BUILD
  MRC_OEM_HOOKS_PPI_FUNC_CALL_PARA1 (Host, OemPostProcInit, ProcStatus);
#endif // MINIBIOS_BUILD
}

/**

  Executed by System BSP only.
  OEM hook before checking and triggering the proper type of reset.

  @param Host - pointer to sysHost structure on stack

  @retval VOID

**/
VOID
OemCheckAndHandleResetRequests (
  struct sysHost             *Host
  )
{
#ifndef MINIBIOS_BUILD
  MRC_OEM_HOOKS_PPI_FUNC_CALL (Host, OemCheckAndHandleResetRequests);
#endif // MINIBIOS_BUILD
}

#if defined(ME_SUPPORT_FLAG) && ME_SUPPORT_FLAG
/**

  MEBIOS hook before checking and triggering the proper type of reset.

  @param Host - pointer to sysHost structure on stack

  @retval VOID

**/
VOID
MEBIOSCheckAndHandleResetRequests(
  struct sysHost             *Host
  )
{
#ifndef MINIBIOS_BUILD
  MRC_OEM_HOOKS_PPI_FUNC_CALL (Host, MEBIOSCheckAndHandleResetRequests);
#endif // MINIBIOS_BUILD
}
#endif // ME_SUPPORT_FLAG
/**

  OEM hook to do any platform specifc init

  @param Host - pointer to sysHost structure on stack
  @param Node - memory controller number ( 0 based)


  @retval UINT8

**/
VOID
OemPostCpuInit (
  struct sysHost             *Host,
  UINT8                      socket
  )
{
#ifndef MINIBIOS_BUILD
  MRC_OEM_HOOKS_PPI_FUNC_CALL_PARA1 (Host, OemPostCpuInit, socket);
#endif // MINIBIOS_BUILD
}

/**

  Puts the Host structure variables for POST

  @param Host - pointer to sysHost structure on stack

@retval VOID

**/
VOID
OemPublishDataForPost (
  struct sysHost             *Host
  )
{
#ifndef MINIBIOS_BUILD
  MRC_OEM_HOOKS_PPI_FUNC_CALL (Host, OemPublishDataForPost);
#endif // MINIBIOS_BUILD
}

/**

    GC_TODO: add routine description

    @param Host        - GC_TODO: add arg description
    @param majorCode   - GC_TODO: add arg description
    @param minorCode   - GC_TODO: add arg description
    @param haltOnError - GC_TODO: add arg description

    @retval None

**/
void
OemPlatformFatalError (
  PSYSHOST Host,
  UINT8    majorCode,
  UINT8    minorCode,
  UINT8    *haltOnError
  )
{
#ifndef MINIBIOS_BUILD
  MRC_OEM_HOOKS_PPI_FUNC_CALL_PARA3(Host, OemPlatformFatalError, majorCode, minorCode, haltOnError);
#endif // MINIBIOS_BUILD
}

/**

  Determine if VRs should be turned off on HEDT

  @param Host - pointer to sysHost structure on stack

@retval VOID

**/
VOID
OemTurnOffVrsForHedt (
  struct sysHost             *Host
  )
{
#ifndef MINIBIOS_BUILD
  MRC_OEM_HOOKS_PPI_FUNC_CALL (Host, OemTurnOffVrsForHedt);
#endif // MINIBIOS_BUILD
}

BOOLEAN
OemReadKtiNvram (
  struct sysHost             *Host
  )
{
  BOOLEAN Status = FALSE;
#ifndef MINIBIOS_BUILD
  MRC_OEM_HOOKS_PPI_FUNC_CALL_RET (Host, OemReadKtiNvram, Status);
#endif // MINIBIOS_BUILD
  return Status;
}

/**

  KTI specific print to serial output

  @param Host      - Pointer to the system Host (root) structure
  @param DbgLevel  - KTI specific debug level
  @param Format    - string format

  @retval VOID

**/
VOID
OemDebugPrintKti (
  struct sysHost *Host,
  UINT32 DbgLevel,
  char* Format,
  ...
  )
{

#ifndef MINIBIOS_BUILD
  #ifdef SERIAL_DBG_MSG
  if (Host->setup.kti.DebugPrintLevel & DbgLevel) {
    va_list  Marker;
    va_start (Marker, Format);
    rcVprintf (Host, Format, Marker);
    va_end (Marker);

  }
  #endif // SERIAL_DBG_MSG
#endif // MINIBIOS_BUILD
}
/*++

Routine Description:

  OEM hook meant to be used by customers where they can use it to write their own jumper detection code
  and have it return FALSE when no jumper is present, thereby, blocking calling of the BSSA loader; else have it
  return TRUE if the concerned jumper is physically present. This check ensures that someone will have to be
  physically present by the platform to enable the BSSA Loader.

Arguments:
  Host              - pointer to sysHost

Returns:
  This function returns TRUE by default as OEMs are meant to implement their own jumper detection code(DetectPhysicalPresence).

--*/
#ifdef SSA_FLAG
BOOLEAN
OemDetectPhysicalPresenceSSA (
  PSYSHOST         Host
  )
{
  BOOLEAN Status = TRUE;
#ifndef MINIBIOS_BUILD
  MRC_OEM_HOOKS_PPI_FUNC_CALL_RET (Host, OemDetectPhysicalPresenceSSA, Status);
#endif // MINIBIOS_BUILD
  return Status;
}
#endif

/**

  OEM hook for reacting to Board VS CPU conflict.
  Refcode will halt execution. OEMs might prefer to allow system to boot to
  video and display error code on screen.

  @param Host - pointer to sysHost

**/
VOID
OemCheckForBoardVsCpuConflicts (
  PSYSHOST Host
  )
{
#ifndef MINIBIOS_BUILD
  MRC_OEM_HOOKS_PPI_FUNC_CALL (Host, OemCheckForBoardVsCpuConflicts);
#endif // MINIBIOS_BUILD
}


/**

  PlatformSetVdd - OEM hook to change the socket Vddq

  @param Host  - Pointer to sysHost
  @param socket  - processor number

  @retval SUCCESS if the Vddq change was made
  @retval FAILURE if the Vddq change was not made

**/
UINT8
PlatformSetVdd (
  PSYSHOST Host,
  UINT8    socket
  )
{
  UINT8 Status = FAILURE;
#ifndef MINIBIOS_BUILD
  MRC_OEM_HOOKS_PPI_FUNC_CALL_PARA1_RET (Host, PlatformSetVdd, socket, Status);
#endif // MINIBIOS_BUILD
  return Status;
}
/**

  OEM hook to release any platform clamps affecting CKE
  and/or DDR Reset

  @param Host      - pointer to sysHost structure

**/
void
PlatformReleaseADRClamps (
  PSYSHOST Host,
  UINT8 socket
  )
{
#ifndef MINIBIOS_BUILD
  MRC_OEM_HOOKS_PPI_FUNC_CALL_PARA1 (Host, PlatformReleaseADRClamps, socket);
#endif // MINIBIOS_BUILD
}

void
PlatformCheckPORCompat (
  PSYSHOST Host
  )
{
#ifndef MINIBIOS_BUILD
  MRC_OEM_HOOKS_PPI_FUNC_CALL (Host, PlatformCheckPORCompat);
#endif // MINIBIOS_BUILD
}

/**

  PlatformFatalError - OEM hook to provide fatal error output to the
  user interface and to override the halt on error policy.

  @param Host - pointer to sysHost structure
  @param majorCode - major error/warning code
  @param minorCode - minor error/warning code
  @param haltOnError - pointer to variable that selects policy

**/
void
PlatformFatalError (
  PSYSHOST Host,
  UINT8    majorCode,
  UINT8    minorCode,
  UINT8    *haltOnError
  )
{
#ifndef MINIBIOS_BUILD
  MRC_OEM_HOOKS_PPI_FUNC_CALL_PARA3(Host, PlatformFatalError, majorCode, minorCode, haltOnError);
#endif // MINIBIOS_BUILD
}

/**

  PlatformCheckpoint - OEM hook to provide major checkpoint output
  to the user interface. By default a byte is written to port 80h.

  @param Host  - Pointer to the system Host (root) structure

  @retval N/A

**/
void
PlatformCheckpoint (
  PSYSHOST Host
  )
{
#ifndef MINIBIOS_BUILD
  MRC_OEM_HOOKS_PPI_FUNC_CALL (Host, PlatformCheckpoint);
#endif // MINIBIOS_BUILD
}
/**

  OEM Hook to Log warning to the warning log

  @param Host              - pointer to sysHost
  @param warningCode       - Major warning code
  @param minorWarningCode  - Minor warning code
  @param socket              - socket number
  @param ch                - channel number
  @param dimm              - dimm number
  @param rank              - rank number

**/
void
PlatformOutputWarning (
  PSYSHOST Host,
  UINT8    warningCode,
  UINT8    minorWarningCode,
  UINT8    socket,
  UINT8    ch,
  UINT8    dimm,
  UINT8    rank
  )
{
#ifndef MINIBIOS_BUILD
  if (IsMrcOemHooksPpiReady (Host)) {
    Host->MrcOemHooksPpi->PlatformOutputWarning (Host, warningCode, minorWarningCode, socket, ch, dimm, rank);
  }
#endif // MINIBIOS_BUILD
}

/**

  Oem hook to Log Error to the warning log

  @param Host              - pointer to sysHost
  @param ErrorCode         - Major warning code
  @param minorErrorCode    - Minor warning code
  @param socket            - socket number
  @param ch                - channel number
  @param dimm              - dimm number
  @param rank              - rank number

**/
void
PlatformOutputError (
  PSYSHOST Host,
  UINT8    ErrorCode,
  UINT8    minorErrorCode,
  UINT8    socket,
  UINT8    ch,
  UINT8    dimm,
  UINT8    rank
  )
{
#ifndef MINIBIOS_BUILD
  if (IsMrcOemHooksPpiReady (Host)) {
    Host->MrcOemHooksPpi->PlatformOutputError (Host, ErrorCode, minorErrorCode, socket, ch, dimm, rank);
  }
#endif // MINIBIOS_BUILD
}

/**

  PlatformLogWarning - OEM hook to provide common warning output to the
  user interface

  @param Host      - pointer to sysHost structure
  @param majorCode - major error/warning code
  @param minorCode - minor error/warning code
  @param logData   - data log

**/
void
PlatformLogWarning (
  PSYSHOST Host,
  UINT8    majorCode,
  UINT8    minorCode,
  UINT32   logData
  )
{
#ifndef MINIBIOS_BUILD
  MRC_OEM_HOOKS_PPI_FUNC_CALL_PARA3(Host, PlatformLogWarning, majorCode, minorCode, logData);
#endif // MINIBIOS_BUILD
}

/**

  Used to add warnings to the promote warning exception list

  @param Host  - pointer to sysHost

  @retval N/A

**/
UINT32
PlatformMemInitWarning (
  PSYSHOST Host
  )
{
  UINT32 Status = SUCCESS;
#ifndef MINIBIOS_BUILD
  MRC_OEM_HOOKS_PPI_FUNC_CALL_RET (Host, PlatformMemInitWarning, Status);
#endif // MINIBIOS_BUILD
  return Status;
}

/**

  OEM hook to allow a secondary ADR dection method other than
  PCH_PM_STS

  @param Host      - pointer to sysHost structure

  @retval 0         - Hook not implemented or no ADR recovery
  @retval 1         - Hook implemented and platform should recover from ADR

**/
UINT32
PlatformDetectADR (
  PSYSHOST Host
  )
{
  UINT32 Status = SUCCESS;
#ifndef MINIBIOS_BUILD
  MRC_OEM_HOOKS_PPI_FUNC_CALL_RET (Host, PlatformDetectADR, Status);
#endif // MINIBIOS_BUILD
  return Status;
}

/**

    Wait for PBSP to check in

    @param Host     - Pointer to the system Host (root) structure
    @param WaitTime - time in milliseconds

    @retval None

**/
VOID
OemWaitTimeForPSBP (
    struct sysHost *Host,
    UINT32 *WaitTime
  )
{
#ifndef MINIBIOS_BUILD
  MRC_OEM_HOOKS_PPI_FUNC_CALL_PARA1 (Host, OemWaitTimeForPSBP, WaitTime);
#endif // MINIBIOS_BUILD
}

/**
  OEM hook before topology discovery in KTIRC.

  @param Host - pointer to syshost
**/
VOID
OemHookPreTopologyDiscovery (
  struct sysHost             *Host
  )
{
#ifndef MINIBIOS_BUILD
  MRC_OEM_HOOKS_PPI_FUNC_CALL (Host, OemHookPreTopologyDiscovery);
#endif // MINIBIOS_BUILD
}

/**
  OEM hook at the end of topology discovery in KTIRC to be used for error reporting.

  @param Host - pointer to syshost
**/
VOID
OemHookPostTopologyDiscovery (
  struct sysHost             *Host
  )
{
#ifndef MINIBIOS_BUILD
  MRC_OEM_HOOKS_PPI_FUNC_CALL (Host, OemHookPostTopologyDiscovery);
#endif // MINIBIOS_BUILD
}

/**

  Get the board type bitmask.
    Bits[3:0]   - Socket0
    Bits[7:4]   - Socket1
    Bits[11:8]  - Socket2
    Bits[15:12] - Socket3
    Bits[19:16] - Socket4
    Bits[23:20] - Socket5
    Bits[27:24] - Socket6
    Bits[31:28] - Socket7

  Within each Socket-specific field, bits mean:
    Bit0 = CPU_TYPE_STD support; always 1 on Socket0
    Bit1 = CPU_TYPE_F support
    Bit2 = CPU_TYPE_P support
    Bit3 = reserved

  @param Host - pointer to syshost

  @retval board type bitmask

**/
UINT32
OemGetBoardTypeBitmask (
  struct sysHost *Host
  )
{
  UINT32 Status = SUCCESS;
#ifndef MINIBIOS_BUILD
  MRC_OEM_HOOKS_PPI_FUNC_CALL_RET (Host, OemGetBoardTypeBitmask, Status);
#endif // MINIBIOS_BUILD
  return Status;
}

/**

  OEM hook immediately before initialization of memory throttling early in the Rc

  @param Host  - Point to sysHost

  @retval UINT32

**/
UINT32
OemInitThrottlingEarly (
  PSYSHOST Host
  )
{
  UINT32 Status = SUCCESS;
#ifndef MINIBIOS_BUILD
  MRC_OEM_HOOKS_PPI_FUNC_CALL_RET (Host, OemInitThrottlingEarly, Status);
#endif // MINIBIOS_BUILD
  return Status;
}

/**

  OEM hook immediately before DIMM detection

  @param Host  - Point to sysHost

  @retval UINT32

**/
UINT32
OemDetectDIMMConfig (
  PSYSHOST Host
  )
{
  UINT32 Status = SUCCESS;
#ifndef MINIBIOS_BUILD
  MRC_OEM_HOOKS_PPI_FUNC_CALL_RET (Host, OemDetectDIMMConfig, Status);
#endif // MINIBIOS_BUILD
  return Status;
}

/**

  OEM hook immediately before DIMM detection

  @param Host  - Point to sysHost

  @retval UINT32

**/
UINT32
OemInitDdrClocks (
  PSYSHOST Host
  )
{
  UINT32 Status = SUCCESS;
#ifndef MINIBIOS_BUILD
  MRC_OEM_HOOKS_PPI_FUNC_CALL_RET (Host, OemInitDdrClocks, Status);
#endif // MINIBIOS_BUILD
  return Status;
}

/**

  OEM hook immediately before DIMM detection

  @param Host  - Point to sysHost

  @retval UINT32

**/
UINT32
OemSetDDRFreq (
  PSYSHOST Host
  )
{
  UINT32 Status = SUCCESS;
#ifndef MINIBIOS_BUILD
  MRC_OEM_HOOKS_PPI_FUNC_CALL_RET (Host, OemSetDDRFreq, Status);
#endif // MINIBIOS_BUILD
  return Status;
}

/**

  OEM hook immediately before DIMM detection

  @param Host  - Point to sysHost

  @retval UINT32

**/
UINT32
OemConfigureXMP (
  PSYSHOST Host
  )
{
  UINT32 Status = SUCCESS;
#ifndef MINIBIOS_BUILD
  MRC_OEM_HOOKS_PPI_FUNC_CALL_RET (Host, OemConfigureXMP, Status);
#endif // MINIBIOS_BUILD
  return Status;
}

/**

  OEM hook immediately before DIMM detection

  @param Host  - Point to sysHost

  @retval UINT32

**/
UINT32
OemCheckVdd (
  PSYSHOST Host
  )
{
  UINT32 Status = SUCCESS;
#ifndef MINIBIOS_BUILD
  MRC_OEM_HOOKS_PPI_FUNC_CALL_RET (Host, OemCheckVdd, Status);
#endif // MINIBIOS_BUILD
  return Status;
}

/**

  OEM hook immediately before DIMM detection

  @param Host  - Point to sysHost

  @retval UINT32

**/
UINT32
OemEarlyConfig (
  PSYSHOST Host
  )
{
  UINT32 Status = SUCCESS;
#ifndef MINIBIOS_BUILD
  MRC_OEM_HOOKS_PPI_FUNC_CALL_RET (Host, OemEarlyConfig, Status);
#endif // MINIBIOS_BUILD
  return Status;
}

/**

  OEM hook immediately before DIMM detection

  @param Host  - Point to sysHost

  @retval UINT32

**/
UINT32
OemLateConfig (
  PSYSHOST Host
  )
{
  UINT32 Status = SUCCESS;
#ifndef MINIBIOS_BUILD
  MRC_OEM_HOOKS_PPI_FUNC_CALL_RET (Host, OemLateConfig, Status);
#endif // MINIBIOS_BUILD
  return Status;
}

/**

  OEM hook immediately before initialization of memory throttling

  @param Host  - Point to sysHost

  @retval UINT32

**/
UINT32
OemInitThrottling (
  PSYSHOST Host
  )
{
  UINT32 Status = SUCCESS;
#ifndef MINIBIOS_BUILD
  MRC_OEM_HOOKS_PPI_FUNC_CALL_RET (Host, OemInitThrottling, Status);
#endif // MINIBIOS_BUILD
  return Status;
}

/**

   Locates the platform EPARAMS based on socket/link

   @param Host  - Pointer to the system Host (root) structure
   @param Socket - SocId we are looking up
   @param Link - Link# on socket
   @param Freq - Link frequency
   *LinkSpeedParameter - return Ptr

   @retval Ptr to EPARAM table
   @retval KTI_SUCCESS if entry found
   @retval KTI_FAILURE if no entry found

**/
INT32
OemKtiGetEParams (
  struct sysHost           *Host,
  UINT8                    SocketID,
  UINT8                    Link,
  UINT8                    Freq,
  VOID                     *LinkSpeedParameter
  )
{
  UINT32 Status = SUCCESS;
#ifndef MINIBIOS_BUILD
  if (IsMrcOemHooksPpiReady (Host)) {
    Status = Host->MrcOemHooksPpi->OemKtiGetEParams (Host, SocketID, Link, Freq, LinkSpeedParameter);
  }
#endif // MINIBIOS_BUILD
  return Status;
}

/**
  this routine can be used to update CCUR
  @param Host  - pointer to sysHost structure on stack
**/
UINT32
OemUpdatePlatformConfig(
  struct sysHost            *Host
  )
{
  UINT32 Status = SUCCESS;
#ifndef MINIBIOS_BUILD
  MRC_OEM_HOOKS_PPI_FUNC_CALL_RET (Host, OemUpdatePlatformConfig, Status);
#endif // MINIBIOS_BUILD
  return Status;
}

/**
  RAS hook for validating Current config (CCUR)
  @param Host  - pointer to sysHost structure on stack
**/
UINT32
ValidateCurrentConfigOemHook(
  PSYSHOST Host
  )
{
  UINT32 Status = SUCCESS;
#ifndef MINIBIOS_BUILD
  MRC_OEM_HOOKS_PPI_FUNC_CALL_RET (Host, ValidateCurrentConfigOemHook, Status);
#endif // MINIBIOS_BUILD
  return Status;
}

/**

  VR SVID's for MC on SKX Platform

  @param Host                - Pointer to sysHost
  @param socket              - Socket number
  @param mcId                - Memory Controller Id

  @retval 0 - VR SVID for Memory Controller ID 0
  @retval 2 - VR SVID for Memory Controller ID 1

**/
UINT8
PlatformVRsSVID (
  PSYSHOST  Host,
  UINT8     socket,
  UINT8     mcId
  )
{
  UINT8 Svid = 0;
#ifndef MINIBIOS_BUILD
  if (IsMrcOemHooksPpiReady (Host)) {
    Svid = Host->MrcOemHooksPpi->PlatformVRsSVID (Host, socket, mcId);
  }
#endif // MINIBIOS_BUILD
  return Svid;
}

/**

  PlatformInitializeData - OEM hook to initialize Input structure data if required.

  @param Host  - Pointer to sysHost

  @retval N/A

**/
void
PlatformInitializeData (
  // APTIOV_SERVER_OVERRIDE_RC_START
  PSYSHOST Host,
  UINT8 socket
  // APTIOV_SERVER_OVERRIDE_RC_END
  )
{
#ifndef MINIBIOS_BUILD
  // APTIOV_SERVER_OVERRIDE_RC_START
  MRC_OEM_HOOKS_PPI_FUNC_CALL_PARA1 (Host, PlatformInitializeData, socket);
  // APTIOV_SERVER_OVERRIDE_RC_END
#endif // MINIBIOS_BUILD
}

/**

  Platform hook for initializing Tcrit to 105 temp offset & the config register
  which is inside of initialization of memory throttling

  @param Host              - pointer to sysHost
  @param socket            - socket number
  @param smbDevice dev     - SMB Device
  @param byteOffset        - byte Offset
  @param data              - data

**/
void
PlatformHookMst (
  PSYSHOST Host,
  UINT8    socket,
  struct   smbDevice dev,
  UINT8    byteOffset,
  UINT8    *data
  )
{
#ifndef MINIBIOS_BUILD
  if (IsMrcOemHooksPpiReady (Host)) {
    Host->MrcOemHooksPpi->PlatformHookMst (Host, socket, dev, byteOffset, data);
  }
#endif // MINIBIOS_BUILD
}

/**

  Initialize prevBootNGNDimmCfg with data from previous boot

    @param Host = pointer to sysHost structure
    @param prevBootNGNDimmCfg = pointer to prevBootNGNDimmCfg structure

**/
UINT32
UpdatePrevBootNGNDimmCfg(
    struct sysHost             *Host,
    struct prevBootNGNDimmCfg  *prevBootNGNDimmData)
{
  UINT32 Status = SUCCESS;
#ifndef MINIBIOS_BUILD
  MRC_OEM_HOOKS_PPI_FUNC_CALL_PARA1_RET (Host, UpdatePrevBootNGNDimmCfg, prevBootNGNDimmData, Status);
#endif // MINIBIOS_BUILD
  return Status;
}

/**

   Callout to allow OEM to adjust the resource map.
   Used to allow adjustment of IO ranges, bus numbers, mmioh and mmiol resource assignments
   Calling code assumes the data structure is returned in a good format.
   SBSP resources must be assigned first, with rest of sockets assigned in ascending order of NID
   Resources must be assigned in ascending consecutive order with no gaps.
   Notes: This does not allow for changing the mmcfg size of mmioh granularity
          However you can allocate multiple consecutive blocks of mmioh to increase size in a particular stack
          and adjust the mmiohbase


   @param Host         - Pointer to the system Host (root) structure
   @param CpuRes       - Ptr to array of Cpu Resources
   @param SystemParams - structure w/ system parameters needed

**/

VOID
OemGetResourceMapUpdate (
  struct sysHost    *Host,
  KTI_CPU_RESOURCE  CpuRes[MAX_SOCKET],
  KTI_OEMCALLOUT    SystemParams
)
{
#ifndef MINIBIOS_BUILD
  if (IsMrcOemHooksPpiReady (Host)) {
    Host->MrcOemHooksPpi->OemGetResourceMapUpdate (Host, CpuRes, SystemParams);
  }
#endif // MINIBIOS_BUILD
}

/*++

Routine Description:

  OEM hook for overriding the memory POR frequency table

Arguments:

  freqTablePtr    - Pointer to Intel POR memory frequency table
  freqTableLength - Pointer to number of DimmPOREntryStruct entries in POR Frequency Table


Returns:

  freqTablePtr    - Pointer to POR memory frequency table to use
  freqTableLength - Pointer to number of DimmPOREntryStruct entries in POR Frequency Table to use

--*/
void
OemLookupFreqTable (
  PSYSHOST Host,
  struct DimmPOREntryStruct **freqTablePtr,
  UINT16 *freqTableLength
  )
{

#ifndef MINIBIOS_BUILD
  if (IsMrcOemHooksPpiReady (Host)) {
    Host->MrcOemHooksPpi->OemLookupFreqTable (Host, freqTablePtr, freqTableLength);
  }
#endif // MINIBIOS_BUILD
}

/**

  OEM hook to read data from a device on the SMBbus.

  @param Host - pointer to sysHost structure
  @param socket - socket number
  @param dev - SMB device structure
  @param byteOffset - offset of data to read
  data - Pointer to data to be returned

  @retval data - Pointer to data to be returned
  @retval 0 - Hook not implemented (default)
  @retval 1 - Hook implemented and read was successful
  @retval 2 - Hook implemented and read failed / slave device not present
  @retval Return 1 or 2 if this routine should replace ReadSmb

**/
UINT8
PlatformReadSmb (
  PSYSHOST         Host,
  UINT8            socket,
  struct smbDevice dev,
  UINT8            byteOffset,
  UINT8            *data
  )
{
  UINT8 Ret = 0;
#ifndef MINIBIOS_BUILD
  if (IsMrcOemHooksPpiReady (Host)) {
    Ret = Host->MrcOemHooksPpi->PlatformReadSmb (Host, socket, dev, byteOffset, data);
  }
#endif // MINIBIOS_BUILD
  return Ret;
}

/**

  OEM hook to write data to a device on the SMBbus.

  @param Host - pointer to sysHost structure
  @param dev - SMB device structure
  @param byteOffset - offset of data to write
  @param data - Pointer to data to write

**/
UINT8
PlatformWriteSmb (
  PSYSHOST         Host,
  struct smbDevice dev,
  UINT8            byteOffset,
  UINT8            *data
  )
{
  UINT8 Ret = 0;
#ifndef MINIBIOS_BUILD
  if (IsMrcOemHooksPpiReady (Host)) {
    Ret = Host->MrcOemHooksPpi->PlatformWriteSmb (Host, dev, byteOffset, data);
  }
#endif // MINIBIOS_BUILD
  return Ret;
}

/**

  OEM hook for overriding the DDR4 ODT Value table

  @param ddr4OdtValueStructPtr   - Pointer to Intel DDR4 ODT Value array
  @param ddr4OdtValueStructSize  - Pointer to size of Intel DDR4 ODT Value array in number of bytes


  @retval ddr4OdtValueStructPtr   - Pointer to DDR4 ODT Value array to use
  @retval ddr4OdtValueStructSize  - Pointer to size of DDR4 ODT Value array in number of bytes to use

**/
void
OemLookupDdr4OdtTable (
  PSYSHOST Host,
  struct ddr4OdtValueStruct **ddr4OdtValueStructPtr,
  UINT16 *ddr4OdtValueStructSize
  )
{
#ifndef MINIBIOS_BUILD
  if (IsMrcOemHooksPpiReady (Host)) {
    Host->MrcOemHooksPpi->OemLookupDdr4OdtTable (Host, ddr4OdtValueStructPtr, ddr4OdtValueStructSize);
  }
#endif // MINIBIOS_BUILD
}

#if ENHANCED_WARNING_LOG
/**

Platform function for initializing the enhanced warning log.
It is called from core prior any calls to PlatformEwlLogEntry.
This function will initialize the enhanced warning log public
and private data structures.

@param Host        - Pointer to the system Host (root) structure
@retval – SUCCESS

**/
UINT32
PlatformEwlInit(
  PSYSHOST Host
  )
{
  UINT32 Status = SUCCESS;
#ifndef MINIBIOS_BUILD
  MRC_OEM_HOOKS_PPI_FUNC_CALL_RET (Host, PlatformEwlInit, Status);
#endif // MINIBIOS_BUILD
  return Status;
}

/**

Platform hook that is called to report a new entry is available for reporting to
the Enhanced Warning Log.

This function will add it to the log or return an error if there is insufficient
space remaining for the entry.

@param  Host        - Pointer to the system Host (root) structure
@param  EwlEntry    - Pointer to new EWL entry buffer
@retval – SUCCESS if entry is added; FAILURE if entry is not added

**/
MRC_STATUS
PlatformEwlLogEntry(
PSYSHOST          Host,         // Pointer to the system Host (root) structure
EWL_ENTRY_HEADER  *EwlEntry     // Pointer to the EWL entry header
)
{
  UINT32 Status = SUCCESS;
#ifndef MINIBIOS_BUILD
  MRC_OEM_HOOKS_PPI_FUNC_CALL_PARA1_RET (Host, PlatformEwlLogEntry, EwlEntry, Status);
#endif // MINIBIOS_BUILD
  return Status;
}
#endif //ENHANCED_WARNING_LOG

UINT32
OemSendCompressedPacket (
  PSYSHOST Host,
  UINT8    *packetPtr,
  UINT32   packetBytes
  )
{
  UINT32 Ret = SUCCESS;
#ifndef MINIBIOS_BUILD
  if (IsMrcOemHooksPpiReady (Host)) {
    Ret = Host->MrcOemHooksPpi->OemSendCompressedPacket (Host, packetPtr, packetBytes);
  }
#endif // MINIBIOS_BUILD
  return Ret;
}

/**

  OemAfterAddressMapConfigured - OEM hook after address map is configured.

  @param Host  - Pointer to sysHost

  @retval N/A

**/
VOID
OemAfterAddressMapConfigured (
  PSYSHOST Host
  )
{
#ifndef MINIBIOS_BUILD
  MRC_OEM_HOOKS_PPI_FUNC_CALL (Host, OemAfterAddressMapConfigured);
#endif // MINIBIOS_BUILD
}
