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
//      Bug Fixed:  Support SMC Memory map-out function.
//      Reason:     
//      Auditor:    Ivern Yeh
//      Date:       Jul/07/2016
//
/** @file
  This file will contain all definitions related to Memory Error Handler.

  Copyright (c) 2009-2014 Intel Corporation.
  All rights reserved. This software and associated documentation
  (if any) is furnished under a license and may only be used or
  copied in accordance with the terms of the license.  Except as
  permitted by such license, no part of this software or
  documentation may be reproduced, stored in a retrieval system, or
  transmitted in any form or by any means without the express
  written consent of Intel Corporation.

**/
	
#ifndef _MEMORY_ERROR_HANDLER_H
#define _MEMORY_ERROR_HANDLER_H

#include <Protocol/PlatformRasPolicyProtocol.h>
#include <Protocol/PlatformErrorHandlingProtocol.h>
#include <Protocol/SmmBase2.h>
#include <Protocol/MemRasProtocol.h>
#include <Library/emcaplatformhookslib.h>
#include <Library/ProcMemErrReporting.h>
#include <IndustryStandard/WheaDefs.h>
#include <Protocol/SmmVariable.h>
#include "ErrorRecords.h"

// These may go away when entire RAS code is ported from Node/Ch to Socket/Ch
#define SKTCH_TO_NODE(Skt, Ch)      ((Skt * MAX_IMC) + (Ch % MAX_MC_CH))
#define SKTCH_TO_NODECH(Ch)         (Ch % MAX_MC_CH)

//
// Memory Error sub-type definitions
//
//
#define MEM_ERROR                 0xf0
#define MEM_LINK_ERROR            0x01
#define MEM_LANE_FAILOVER         0x02
#define MEM_DDR_PARITY_ERROR      0x03
#define MEM_CRC_ERROR             0x04
#define MEM_ECC_ERROR             0x05
#define MEM_MIRROR_FAILOVER       0x06

//
// Corrected error thresholds
//
#define PLATFORM_PER_RANK_THRESHOLD_DURATION_HOURS 24
#define PLATFORM_ERROR_COUNT_THRESHOLD 80

//
// RTC definitions
//
#define RTC_ADDRESS_SECONDS           0   // R/W  Range 0..59
#define RTC_ADDRESS_MINUTES           2   // R/W  Range 0..59
#define RTC_ADDRESS_HOURS             4   // R/W  Range 1..12 or 0..23 Bit 7 is AM/PM
#define RTC_ADDRESS_DAY_OF_THE_MONTH  7   // R/W  Range 1..31
#define RTC_ADDRESS_MONTH             8   // R/W  Range 1..12
#define RTC_ADDRESS_YEAR              9   // R/W  Range 0..99
#define RTC_ADDRESS_REGISTER_A        10  // R/W[0..6]  R0[7]
#define RTC_ADDRESS_REGISTER_B        11  // R/W
#define RTC_ADDRESS_REGISTER_C        12  // RO
#define RTC_ADDRESS_REGISTER_D        13  // RO

#define BCD2DEC(x)  ((x & 0x0F) + ((x >> 4) * 10))

//
// Register A
//
typedef struct {
  UINT8               Rs:4;   // Rate Selection Bits
  UINT8               Dv:3;   // Divisor
  UINT8               Uip:1;  // Update in progress
} RTC_REGISTER_A_BITS;

typedef union {
  RTC_REGISTER_A_BITS Bits;
  UINT8               Data;
} RTC_REGISTER_A;

//
// Register D
//
typedef struct {
  UINT8               Reserved:7; // Read as zero.  Can not be written.
  UINT8               Vrt:1;      // Valid RAM and Time
} RTC_REGISTER_D_BITS;

typedef union {
  RTC_REGISTER_D_BITS Bits;
  UINT8               Data;
} RTC_REGISTER_D;

typedef struct {
  UINT16  McaCod;
  UINT16  McaCodMsk;
  UINT16  MsCod;
  UINT16  MsCodMsk;
} DIMM_ISOLATION_SKIP_DESC;

typedef struct {
  UINT32  Year:7;
  UINT32  Month:4;
  UINT32  Day:5;
  UINT32  Hour:5;
  UINT32  Minute:6;
  UINT32  Second:6;
} TIME_STAMP;

////////////////////////////////// Common Routines //////////////////////////////////

VOID
ClearMemoryErrors(
  VOID
  );

VOID
ClearImcRankCntrsPerNode (
  UINT8 Socket,
  UINT8 Mc
  );

////////////////////////////////// Error Handling Routines //////////////////////////////////




/**
  Executes based on error handling policies.

  @param[in]  MemoryErrRecord      - Pointer to the memory error record.
  
  @retval None
**/
VOID
ElogMcExecuteCorrectableErrorHandlingPolicies (
  IN        UEFI_MEM_ERROR_RECORD   *MemoryErrRecord
  );

static
EFI_STATUS
HandleSparing(
  IN  UINT8         NodeId,
  IN  EVENT_TYPE    EventType,
  IN OUT BOOLEAN    *IsEventHandled
  );

static
EFI_STATUS
HandleSpareNodeMigration(
  IN  UINT8         NodeId,
  IN  EVENT_TYPE    EventType,
  IN OUT BOOLEAN    *IsEventHandled
  );

/**
  Calculates and returns physical address from Addr and Misc.

  @param[in]  Addr      - MCA_ADDR
  @param[in]  Misc      - MCA_MISC

  @retval None
**/
UINT64
MCAddress (
  UINT64 Addr,
  UINT64 Misc
  );

/**
  Fills error record information for detected error.

  @param[in]  McBank      - MC Bank the error was detected on
  @param[in]  skt         - The socket the MCE occured on
  @param[in]  Sev         - The error severity
  @param[in]  McErrSig    - MCE error information
  @param[out] MemRecord   - The error record to fill

  @retval EFI_STATUS
    MemRecord - Error record for the detected error
**/
EFI_STATUS
FillPlatformMemorySection (
  IN        UINT8                         McBank,
  IN        UINT8                         skt,
  IN        UINT32                        Sev,
  IN        EMCA_MC_SIGNATURE_ERR_RECORD  *McErrSig,
      OUT   UEFI_MEM_ERROR_RECORD         *MemRecord
  );

/**
  Creates the memory error record based on the inputs.
  
  This function will only handle correctable errors.

  @param[in]  Type      - The error type
  @param[in]  SubType   - The error sub type
  @param[in]  Node      - The node the error was detected on
  @param[in]  Channel   - The channel the error was detected on
  @param[in]  Rank      - The rank the error was detected on
  @param[in]  Dimm      - The dimm the error was detected on
  @param[in]  McaCode   - The MCA code for the error
  
  @retval None
**/
VOID
LogCorrectedMemError (
  IN        UINT8               Type,
  IN        UINT8               SubType,
  IN        UINT8               Node,
  IN        UINT8               Ch,
  IN        UINT8               Rank,
  IN        UINT8               Dimm,
  IN        UINT8               McaCode
  );

/**
  Check if any socket has an MCE.

  @retval None
**/
/*
BOOLEAN
McErrNodeFn (
  VOID
  );
*/
/**
  Handle corrected memory errors.

  @param[in]  Socket              - The socket to check
  @param[in]  ImcEventsProgress   - Events in progress
  @param[in]  ImcNewEvents        - New events
  
  @retval None
**/
VOID
CorrectedMemoryErrorHandler (
  IN        UINT8   Socket,
  IN        UINT32  ImcEventsProgress,
  IN        UINT32  ImcNewEvents
  );
EFI_STATUS
EfiSmmGetTime (
  IN OUT TIME_STAMP *Time
  );

UINT32
EfiSmmGetNoofHoursPast (
  IN OUT TIME_STAMP *PrevTime
  );

////////////////////////////////// Initialization routines //////////////////////////////////

/**
  Enables or disables the memory and HA specifc error reporting based on the SYSTEM_INFO structure.
  
  @retval EFI_SUCCESS if the call is succeed

**/
EFI_STATUS
EnableElogMemory (
  VOID
  );

/**
  This function enables or disables SMI generation of memory and HA corrected memeory errors.

  @param[in]  Node      - Node to init

  @retval None
**/
VOID
EnableSmiForCorrectedMemoryErrors (
  IN        UINT8   Socket,
  IN        UINT8   Mc
  );

/**
  This function enables or disables interrupt generation of memory ecc errors.

  @param[in]  Node    - The node to enable/disable interrupts for.
  
  @retval None
**/
VOID
EnableDisableIntForEccCorrectedMemErrors (
  IN        UINT8               Socket,
  IN        UINT8               Mc
  );

/**
  Init each source of correctable errors: IMC spare, IMC error threshold, HA mirroring

  @param[in]  Node      - Node to init

  @retval None
**/
VOID
InitImcCorrectableError (
  IN        UINT8   Socket,
  IN        UINT8   Mc
  );
extern  EFI_GUID                              gErrRecordNotifyMceGuid;
extern  EFI_GUID                              gErrRecordNotifyCmcGuid;
extern  EFI_GUID                              gErrRecordPlatformMemErrGuid;

extern  EFI_RAS_SYSTEM_TOPOLOGY               *mRasTopology;
extern  SYSTEM_RAS_CAPABILITY                 *mRasCapability;
extern  EFI_MEM_RAS_PROTOCOL                  *mMemRas;
extern  EFI_CPU_CSR_ACCESS_PROTOCOL           *mCpuCsrAccess;
extern  EFI_PLATFORM_ERROR_HANDLING_PROTOCOL  *mPlatformErrorHandlingProtocol;
extern  SYSTEM_RAS_SETUP                      *mRasSetup;
extern  BOOLEAN                               IsEx;
extern  EFI_SMM_VARIABLE_PROTOCOL             *mSmmVariable;
#if SMCPKG_SUPPORT && MemoryMapOut_SUPPORT
extern 
EFI_STATUS
CommonErrorHandling (
  VOID  *ErrStruc
);  
#endif
#endif


 
