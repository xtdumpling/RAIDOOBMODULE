/**
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/

/**

Copyright (c) 2010 - 2015 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

@file:

  MeUma.h

@brief:

  Framework PEIM to MeUma

**/
#ifndef _ME_UMA_H_
#define _ME_UMA_H_

#define ME_UMA_PPI_GUID \
  { \
    0x8c376010, 0x2400, 0x4d7d, 0xb4, 0x7b, 0x9d, 0x85, 0x1d, 0xf3, 0xc9, 0xd1 \
  }

extern EFI_GUID gMeUmaPpiGuid;

///
/// PCI Register Definition(s)
///
#define R_MESEG_BASE  0xF0

///
/// Maximum UMA size
///
#define ME_UMA_SIZE_UPPER_LIMIT 0x40

///
/// ME FW communication timeout value definitions
///
#define DID_TIMEOUT_MULTIPLIER    0x1388
#define MUSZV_TIMEOUT_MULTIPLIER  0x1388
#define CPURV_TIMEOUT_MULTIPLIER  0x32
#define STALL_1_MILLISECOND       1000
#define STALL_100_MICROSECONDS    100

///
/// Revision
///
#define ME_UMA_PPI_REVISION 1

typedef
UINT32
(EFIAPI *ME_SEND_UMA_SIZE) (
  IN EFI_FFS_FILE_HEADER * FfsHeader
  );

typedef
EFI_STATUS
(EFIAPI *CPU_REPLACEMENT_CHECK) (
  IN EFI_FFS_FILE_HEADER * FfsHeader,
  UINT8                  *ForceFullTraining,
  UINT8                  *nextStep
  );

typedef
EFI_STATUS
(EFIAPI *ME_CONFIG_DID_REG) (
  IN EFI_FFS_FILE_HEADER * FfsHeader,
  UINT8                  InitStat,
  UINT8                  *nextStep
  );

typedef
EFI_STATUS
(EFIAPI *HANDLE_ME_BIOS_ACTION) (
  UINT8               BiosAction
  );

typedef
BOOLEAN
(EFIAPI *ME_IS_UMA_ENABLED) (
  VOID
  );

typedef
EFI_STATUS
(EFIAPI *HOST_RESET_WARNING) (
  OUT UINT8 *pAction
  );

#if TESTMENU_FLAG || ME_TESTMENU_FLAG
typedef
EFI_STATUS
(EFIAPI *TEST_UMA_LOCATION) (
  IN UINT32 MeNcMemLowBaseAddr,
  IN UINT32 MeNcMemHighBaseAddr,
  IN UINT32 MeNcMemLowLimit,
  IN UINT32 MeNcMemHighLimit
  );
#endif // TESTMENU_FLAG || ME_TESTMENU_FLAG

///
/// PPI definition
///
typedef struct ME_UMA_PPI {
  ME_SEND_UMA_SIZE      MeSendUmaSize;
  CPU_REPLACEMENT_CHECK CpuReplacementCheck;
  ME_CONFIG_DID_REG     MeConfigDidReg;
  HANDLE_ME_BIOS_ACTION HandleMeBiosAction;
  ME_IS_UMA_ENABLED     isMeUmaEnabled;
  HOST_RESET_WARNING    HostResetWarning;
#if TESTMENU_FLAG || ME_TESTMENU_FLAG
  TEST_UMA_LOCATION     TestUmaLocation;
#endif // TESTMENU_FLAG || ME_TESTMENU_FLAG  
} ME_UMA_PPI;

// EFI GUID for notification
// that MemoryInitDone has been executed
extern EFI_GUID gMemoryInitDoneSentGuid;

#endif // _ME_UMA_H_
