/*++

Copyright (c) 2008 - 2015 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  CpuRas.h

Abstract:

--*/

#ifndef _CPU_RAS__H
#define _CPU_RAS__H

#include <Protocol/RasMpLinkProtocol.h>
#include <Protocol/CpuRasProtocol.h>
#include <Protocol/MemRasProtocol.h>
#include <Protocol/CpuCsrAccess.h>
#include <Protocol/SmmCpuService.h>
#include <Protocol/GlobalNvsArea.h>
#include <Protocol/CpuHotAddData.h>
#include <Include/CpuHotPlugData.h>
#include <Protocol/HpIoxAccess.h>
#include <Protocol/IioUds.h>
#include <Protocol/SpsRasNotifyProtocol.h>
#include <Setup/IioUniversalData.h>
#include <Library/TimerLib.h>
#include <Library/PciLib.h>
#include <Library/UefiLib.h>
#include <Library/HobLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/LocalApicLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/CpuPpmLib.h>
#include <Ras/RasStateMachine.h>
#include "SysFunc.h"
#include "SysHostChip.h"
#include <Guid/AcpiVariable.h>
#include "PlatformHooks.h"
#include "Library/PciExpressLib.h"
#include "Iio/IioRegs.h"
#include <Library/PlatformHooksLib.h>
#include <Library/MmPciBaseLib.h>
#include <Library/IoLib.h>
#include <Include/SysRegs.h>



//
// This define is to disable the PPM enable code
//  Need to be removed when the PPM code is unit tested on C0
//
#define CPU_RAS_PMM 1

#define ONLY_REGISTER_OFFSET(x)  (x & 0xFFF)

#define MAX_CPU_APIC_ID        256

#ifndef EFI_DEADLOOP
 #define EFI_DEADLOOP()    { volatile int __iii; __iii = 1; while (__iii); }
#endif


#define RAS_CPU_LOGICAL_ONOFFLINE_REDIRECT   8010
#define RAS_CPU_QPI_TOPOLOGY_DISCOVRY_ERROR  8011
#define RAS_CPU_QPI_IPI_UPDATE_ERROR         8012
#define RAS_CPU_PBSP_WAKEUP_ERROR            8013

#define INVALID_APIC_ID 0xFFFFFFFFFFFFFFFF

// PM_SYNCH Defines
#define PCI_DEVICE_NUMBER_PCH_PM             31
#define PCI_FUNCTION_NUMBER_PCH_PM           0
#define R_PCH_PM_SYNC_MODE                   0x33D4
#define GPIO_B_PM_SYNC_EN                   (1 << 29)
#define GPIO_B_C0_TX_EN                     (1 << 13)
#define REG_10_C0_TX_EN                     (1 << 9)

//#define RAS_DBG_LEVEL(x)                BIT##x
#define RAS_DBG_LEVEL(x)              EFI_D_ERROR
#ifdef EFI_DEBUG
#define CPU_RAS_DEBUG(x)              if (mCPURasDebug) { EfiDebugPrint x ; }
#else
#define CPU_RAS_DEBUG(x)
#endif

EFI_STATUS
GenerateRasSci (
    VOID
);

#endif
