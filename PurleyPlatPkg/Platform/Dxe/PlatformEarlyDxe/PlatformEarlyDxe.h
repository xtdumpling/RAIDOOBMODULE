/**

Copyright (c)  1999 - 2016 Intel Corporation. All rights 
reserved This software and associated documentation (if any) is 
furnished under a license and may only be used or copied in 
accordance with the terms of the license. Except as permitted by 
such license, no part of this software or documentation may be 
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file PlatformEarlyDxe.h

  
**/

#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/PcdLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/S3BootScriptLib.h>
#include <Protocol/MpService.h>
#include <Guid/PlatformTxt.h>
#include <Cpu/CpuRegs.h>
#include <Platform.h>
#include <PchAccess.h>
#include <Library/SetupLib.h>
#include "RcRegs.h"

extern EFI_GUID gEfiDprRegsProgrammedGuid;
extern EFI_GUID gEfiEventExitBootServicesGuid;

//
// Chispet register
//
#define SA_DPR_REG                          0x290 
#define   SA_DPR_LOCK                       0x1
#define   SA_DPR_PRM                        0x4

///#define PcdPciExpressBaseAddress           0x80000000


//
// define PCI address macro for reg > 0xFF
//
#define EFI_PCI_ADDRESS_EXT(bus, dev, func, reg) \
    ((UINT64) ((((UINTN) reg) << 32) + (((UINTN) bus) << 24) + (((UINTN) dev) << 16) + (((UINTN) func) << 8)))
    
//
// LT Configuration Space Register Definitions
//
#define LT_PUBLIC_SPACE_BASE_ADDRESS        0xfed30000
#define LT_DMA_PROTECTED_RANGE_REG_OFF      0x330

EFI_STATUS
EFIAPI
PlatformEarlyDxeEntry ( 
  IN EFI_HANDLE             ImageHandle,
  IN EFI_SYSTEM_TABLE       *SystemTable
);

VOID
ProgramDPRregs (
  VOID
);
