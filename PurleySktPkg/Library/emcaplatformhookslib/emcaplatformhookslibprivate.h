/** @file
  This is eMCA platform hooks library private definitions

  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement

  Copyright (c) 2009 - 2016 Intel Corporation.  All rights
  reserved This software and associated documentation (if any)
  is furnished under a license and may only be used or copied in
  accordance with the terms of the license.  Except as permitted
  by such license, no part of this software or documentation may
  be reproduced, stored in a retrieval system, or transmitted in
  any form or by any means without the express written consent
  of Intel Corporation.

**/

#ifndef _EMCA_PLATFORM_HOOKS_LIB_PRIVATE_H_
#define _EMCA_PLATFORM_HOOKS_LIB_PRIVATE_H_

//
// Includes
//
#include <Protocol/CpuCsrAccess.h>
#include <Protocol/SmmBase2.h>
#include <Protocol/MemRasProtocol.h>
#include <Protocol/IioUds.h>

//#include "IvxJck.h"
#include "SysFunc.h" // RAS_SUPPORT_BUGBUG - move to different location
#include "SysRegs.h"
#include "RcRegs.h"
#include <Library/LocalApicLib.h>
#include <Library/BaseMemoryLib.h>
#include <IndustryStandard/WheaDefs.h>
#include "UncoreCommonIncludes.h"
#include <Library/SmmServicesTableLib.h>
#include <Library/IoLib.h>
#include <Register/LocalApic.h>


//
// Defines
//

#define CPUID_EXTENDED_TOPOLOGY_LEVEL_TYPE_INVALID 0x0
#define CPUID_EXTENDED_TOPOLOGY_LEVEL_TYPE_SMT     0x1
#define CPUID_EXTENDED_TOPOLOGY_LEVEL_TYPE_CORE    0x2

#define MCA_PASCRUB_MCACOD 0x80
#define MCA_PASCRUB_MCACOD_MSK 0xfff0
#define MCA_PASCRUB_MSCOD 0x10
#define MCA_PASCRUB_MSCOD_MSK 0xffff

#define EVENTS_CTRL_OFFSET                  0xff04
#define UCODE_CR_SMRAM_EVENT_CTL_HLT_IO     0xc1f
#define CMCI_LVT_OFFSET                     0x2f0

#define UBOX_SMI_THRESHOLD_DEFAULT        0x0FFF



typedef struct {
  EFI_MEM_RAS_PROTOCOL   *MemRas;
  EFI_SMM_BASE2_PROTOCOL *SmmBase;
  EFI_SMM_SYSTEM_TABLE2  *Smst;
  EFI_CPU_CSR_ACCESS_PROTOCOL *CpuCsrAccess;
  IIO_UDS *IioUds;
} EMCA_PHLIB_PRIV; 

typedef struct {
  UINT32 McBankNum;
  UINT8 Instance;
} M2MEM_LOOKUP;


//
// Functions prototype declarations 
//

EFI_STATUS
InitMcBankList(
  VOID
  );

EFI_STATUS
EFIAPI 
McaiMcCsrInfoHook 
(
  IN MCA_BANK_HOOKS *This,
  IN UINT32 McBankIdx,
  IN UINT8 *Skt,
  IN EMCA_MC_SIGNATURE_ERR_RECORD *McSig,
  IN OUT VOID *Section 
);

EFI_STATUS
EFIAPI 
ReadMcBankSignature(
  IN UINT32 McBankIdx,
  IN UINT8 skt,    
  OUT EMCA_MC_SIGNATURE_ERR_RECORD *McSig
);


EFI_STATUS
EFIAPI 
ReadM2MemMcBankSignature(
  IN UINT32 McBankIdx,
  IN UINT8 skt,    
  OUT EMCA_MC_SIGNATURE_ERR_RECORD * McSig
);

EFI_STATUS
EFIAPI 
ReadNullSignature(
  IN UINT32 McBankIdx,
  IN UINT8 skt,    
  OUT EMCA_MC_SIGNATURE_ERR_RECORD *McSig
);

BOOLEAN
RecoveryPatrolScrubDummy(
IN UINT32 McBankIdx,
IN OUT EMCA_MC_SIGNATURE_ERR_RECORD *McSig
);

UINT32 EFIAPI
ReadLocalApicRegrep ( 
  IN      UINTN  MmioOffset
  );

#endif  // _EMCA_PLATFORM_HOOKS_LIB_PRIVATE_H_

