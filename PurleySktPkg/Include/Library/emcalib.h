/** @file
  This is an implementation of the eMCA.

  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement

  Copyright (c) 2009-2015 Intel Corporation.  All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license.  Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/



#ifndef _EMCA_LIB_H_
#define _EMCA_LIB_H_

//
// Includes
//
//
// Consumed protocols
//
#include <Uefi.h>
#include "emca.h"
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Protocol/IioUds.h>
#include <PiDxe.h>
#include <Protocol/SmmSwDispatch2.h>
#include <IndustryStandard/WheaDefs.h>

//Get L1 Directory pointer and skip the header to get the BASE to get Elog directory entry pointers base. 
#define EMCA_GET_L1_BASE(A) \
    ((UINT8 *)((UINT64)(A) + (UINT64)((EMCA_L1_HEADER *)(A))->HdrLen))

//A - L1 directory pointer
//B - ApicId
//C - McBank Number
// OFFSET byte base = apicid * (Number of L1 Entries per logical processor * 8) + McBankNum
#define EMCA_GET_L1_OFFSET(A,B,C) \
    ( (UINT64)(B) * ( (((EMCA_L1_HEADER *)(A))->NumL1EntryLp) * sizeof(UINT64) ) + ((C) * sizeof(UINT64)) )

//A - Elog Directory Entry Pointers Base
//B - Offset for elog entry
#define EMCA_GET_ELOG_ENTRY(A,B) \
    (GENERIC_ERROR_STATUS *)(*((UINT64 *)((UINT64)(A) + (UINT64)(B))))


#define MODE_UMC 0
#define MODE_EMCAGEN1 1
#define MODE_EMCA 2

typedef struct {
  UINT8              Mode;
  UINT64             McBankBits;
} GET_SMI_SRC_ARGS_STRUCT;


/**

    Return True if OS has set the opt in flag in the L1 Directory header or False if it is not set 

    @param None

    @retval optin - TRUE if optin flag is set, False if cleared 

**/
BOOLEAN
isEmcaOptInFlagSet(
VOID
);

/**

    Perform eMCA L1 directory boot time initialization

    @param L1Dir   - A pointer to a pointer to the allocated L1 directory buffer if the call succeeds; undefined otherwise. 
    @param ElogDir - A pointer to a pointer to the allocated Elog directory buffer if the call succeeds; undefined otherwise.

    @retval EFI_SUCCESS - The Allocation and Initialization was successful 
    @retval EFI_UNSUPPORTED - EMCA Logging is not supported
    @retval EFI_OUT_OF_RESOURCES - The directories could not be allocated.
    @retval EFI_INVALID_PARAMETER - At least one of the pointers is NULL or invalid      

**/
EFI_STATUS
InitEMCA( 
IN OUT VOID                         **L1Dir,
IN OUT VOID                         **ElogDir
);

/**

    It receives the generic error status record and stores it in the appropriate elog entry. 

    @param McBankNum - The MC Bank Number which registered the error. It starts from 0 until MCG_CAP.Count. 
    @param McSig     - A pointer to Machine Check Signature Error Record. See Related definitions and EMCA SAS. 
    @param Log       - A pointer to an ACPI Generic Error Status Block completely filled along with its UEFI error section. This function just stores the log. See ACPI Specification for details of the record format. 

    @retval EFI_INVALID_PARAMETER - At least one of the pointers is invalid or McBank number points to an unavailable bank.   
    @retval EFI_OUT_OF_RESOURCES  - The corresponding Elog Entry pointer for the bank and apicid is invalid. There is no memory to store the log. 
    @retval Status                - The log was successfully stored in the elog directory.  

**/
EFI_STATUS
LogEmcaRecord(
  IN UINT32                                 McBankNum,
  IN CONST EMCA_MC_SIGNATURE_ERR_RECORD     *McSig,
  IN CONST GENERIC_ERROR_STATUS             *Log
);

/**
  Get the uncore machine check banks.

  Returns a bitmap indicating uncore machine check banks.

  @retval McBankBitField

**/
UINT64
ReadUncoreMcBankBitField(
  UINT8 Mode
);

/**
  Get the core machine check banks.

  Returns a bitmap indicating core machine check banks.

  @retval CoreMcBankBitField

**/
UINT64
ReadCoreMcBankBitField (
  UINT8  Mode
  );


/**
  Return a bitmap of any error sources set in Uncore McBanks.

  @param[out]  McBankBitField   - Bitmap of sources in UNCORE_SMI_ERR_SRC that indicate error conditions

**/
VOID
GetUncoreMcBankErrors ( 
      OUT GET_SMI_SRC_ARGS_STRUCT  *Params
);

/**
  Return a bitmap of any memory error sources set in McBanks.

  @param[in]  MemBankBits   - Bitmap of memory sources in UNCORE_SMI_ERR_SRC that indicate error conditions

  @retval TRUE if memory bank reported errors, otherwise false
**/
BOOLEAN
GetMcBankMemErrors (
  IN       UINT64   McBankBits
  );

/**
  Clears core error sources.

  @param[in] McBankBitField   Bits to clear

  @retval None

**/
VOID
ClearCoreMcBankBitField ( 
  IN      UINT64    McBankBitField,
  IN      UINT8 Mode
  );

/**
  Clears uncore error sources.

  @param[in] McBankBitField   Bits to clear

  @retval None

**/
VOID
ClearUncoreMcBankBitField ( 
  IN      UINT64    McBankBitField,
  IN      UINT8 Mode
);

/**
  Signal a MCE to the OS

  This function will signal an MCE to the OS

  @param[in] LogicalCpu  Logical Cpu Number
  @param[in] SaveStateType  Msr or Mmio smm save state area
  @param[in] Broadcast  Whether MCE should be broadcast
  @param[in] Lmce  if Lmce was signaled by this thread 

  @retval Status.

**/
EFI_STATUS
SignalMceToOs ( 
  IN      UINTN  LogicalCpu,
  IN      UINT32  SaveStateType,
  IN      BOOLEAN Broadcast,
  IN      BOOLEAN Lmce
);


/**
  Signal a CMCI to the OS

  This function will signal an CMCI to the OS

  @param[in] BankIndex  Bank Index of the CPU.

  @param[in] ApicId     APIC ID of the CPU.

**/
VOID
SignalCmciToOs (
  IN      UINT32  BankIndex,
  IN      UINT32  ApicId
);

/**
  Checks an EMCA Smi source.

  @param[out] ErrorFound  - Highest severity error detected

  @retval ErrorFound      - TRUE if error, FALSE if no error

**/
VOID
EmcaSmiSrc (
      OUT   VOID            *ErrorFound
);


#endif  //_EMCA_LIB_H_

