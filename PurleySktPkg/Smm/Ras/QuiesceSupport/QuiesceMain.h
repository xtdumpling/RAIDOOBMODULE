//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//
/**

Copyright (c) 2009 - 2015, Intel Corporation. All rights reserved.
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file QuiesceMain.h

  Main Quiesce service definition


**/

#ifndef _QUIESCE_MAIN_H_
#define _QUIESCE_MAIN_H_

#include "QuiesceSupport.h"
#include <Protocol/QuiesceProtocol.h>
#include <Library/BaseLib.h>
#include <Library/SynchronizationLib.h>
#include <Protocol/MpService.h>
#include <Protocol/SmmCpu.h>
#include <Protocol/SmmCpuService.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Protocol/IioUds.h>
#include <CpuRegs.h>
#include <Protocol/IioSystem.h>

#define QUIESCE_TABLE_SIZE            (64 * 1024)             //64K
#define QUIESCE_CACHE_CODE_SIZE       QUIESCE_TABLE_SIZE      //64K
#define QUIESCE_CACHE_DATA_SIZE       QUIESCE_TABLE_SIZE      //64K
#define PAGE_SIZE_4K                  0x1000
#define QUIESCE_PAGE_DIRECTORY_SIIZE  PAGE_SIZE_4K      //4K for paget directory entry
#define QUIESCE_PAGE_TABLE_SIZE       PAGE_SIZE_4K      //4K for paget table entry

#define PROGRAM_SELF                  0
#define PROGRAM_ALL                   1

#ifndef MSR_QUIESCE_CTL1
#define MSR_QUIESCE_CTL1            0x50
#define B_QUIESCE_CTL1_QUIESCE      0x01  //bit0
#define B_QUIESCE_CTL1_UNQUIESCE    0x02  //bit1
#define B_QUIESCE_CTL1_EVIC_PENDING 0x04  //bit2
#define B_QUIESCE_CTL1_QUIESCE_CAP  0x80  //bit7
#endif

#ifndef MSR_QUIESCE_CTL2
#define MSR_QUIESCE_CTL2            0x51
#define B_QUIESCE_CTL2_DISABLE_PSMI 0x01
#endif

#ifndef MSR_MISC_FEATURE_CONTROL
#define MSR_MISC_FEATURE_CONTROL              0x000001A4
#define   B_EFI_MISC_FEATURE_CONTROL_MLC_STRP   BIT0
#define   B_EFI_MISC_FEATURE_CONTROL_MLC_SPAP   BIT1
#define   B_EFI_MISC_FEATURE_CONTROL_DCU_STRP   BIT2
#define   B_EFI_MISC_FEATURE_CONTROL_DCU_IPP    BIT3
#define   B_EFI_MISC_FEATURE_CONTROL_3STRIKE_DIS  BIT11
#endif

#ifndef MSR_IA32_CR_PAT
#define MSR_IA32_CR_PAT                       0x00000277
#endif
#define PAT_MSR_DEFAULT   (0x0000040600000406ULL)



//BKL_PORTING - Needs porting for Platform
// CSRs
#define PCU_DESIRED_CORES_CSR      ((10 << 15) + (1 << 12) + 0xA4)
#define MT_DISABLE_FLAG            (1 << 30)

#define BIOSSCRATCHPAD2_CSR        ((11 << 15) + (3 << 12) + 0x48)


//BKL_PORTING

#define B_QUIESCE_ON            0x01    //bit0
#define B_RECONFIG_DONE         0x02    //bit1
#define B_UNQUIESCED            0x04    //bit2

// for Monarch AP Status
#define B_MONARCH_AP_READY      0x010    //bit8 for monarch AP ready for config
#define B_MONARCH_AP_DONE       0x020    //bit9 for monarch AP ready for join back to main SMI

#define EFI_SMM_SAVE_STATE_REGISTER_PROCESSOR_ID  514 // It is temporarilly defined here, it should be deleted after this define is moved to core in future(you should see duplicate define). 

extern QUIESCE_COMMUNICATION_DATA_STRUCT mQuiesceCommData;
extern UINT32                             mSocketLevelShift;

VOID
QuiesceApHandler(
  IN EFI_QUIESCE_SUPPORT_PROTOCOL *QuiesceSupport
  );


UINT64
GetNumberOfProcessorsinSmm (
  VOID
  );

  VOID
Save_Misc_Feature_Msr (
  UINT64 *Value
);

VOID
QuiesceMainInit(
  VOID
);

#endif
