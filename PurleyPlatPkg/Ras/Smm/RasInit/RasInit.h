//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//
/*++

Copyright (c) 2008 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  RasInit.h

Abstract:

--*/

#ifndef _RAS_INIT_H
#define _RAS_INIT_H


#include <Library/SmmServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/HpIoxAccess.h>
#include <Protocol/QuiesceProtocol.h>
#include <Protocol/MemRasProtocol.h>
#include <Protocol/CpuRasProtocol.h>
#include <Protocol/IioRasProtocol.h>
#include <Protocol/CpuHotAddData.h>
#include <Protocol/GlobalNvsArea.h>
#include <Protocol/RasfProtocol.h>
#include <Protocol/PlatformRasPolicyProtocol.h>
#include <Guid/AcpiVariable.h>
#include <Library/SynchronizationLib.h>
#include <Protocol/SpsRasNotifyProtocol.h>

#include <Protocol/SmmGpiDispatch2.h>
#include <Protocol/SmmPeriodicTimerDispatch2.h>

#include <Library/BaseMemoryLib.h>
#include <Library/BaseLib.h>
#include <Protocol/MpService.h>
//#include <Library/CpuConfigLib.h>
#include <Library/LocalApicLib.h>
#include <Library/PciLib.h>
#include <Library/PciExpressLib.h>
#include <Library/DebugLib.h>
#include <Library/PlatformHooksLib.h>
#include <Protocol/SmmCpuService.h>
#include <Ras/RasStateMachine.h>
#include "IchRegs.h"
#include <Register/PchRegsLpc.h>
#include <PchAccess.h>
#include <Library/MmPciBaseLib.h>
#include <Library/IoLib.h>
#include <Register/PchRegsPmc.h>
#include <GpioPinsSklH.h>
#include <Library/GpioLib.h>
#include <IncludePrivate/Library/GpioPrivateLib.h>
#include <Library/PeiDxeSmmGpioLib/GpioLibrary.h>
//#include "CpuHotAdd.h"
#include <Protocol/IioUds.h>
#include <Protocol/PciRootBridgeIo.h>
#include <Protocol/SmmPeriodicTimerDispatch2.h>
#include <Protocol/SmmSwDispatch2.h>
#include <Library/PlatformHooksLib.h>


#define EFI_CPUID_VERSION_INFO                0x1
#define   B_EFI_MSR_IA32_APIC_BASE_G_XAPIC      BIT11
#define   B_EFI_MSR_IA32_APIC_BASE_M_XAPIC      BIT10
#define   B_EFI_MSR_IA32_APIC_BASE_BSP          BIT8

#define EFI_EXT_XAPIC_LOGICAL_APIC_ID         0x00000802

#define APIC_REGISTER_LOCAL_ID_OFFSET         0x00000020

#define DEBUG_ACCESS(x)            *(volatile UINT8 *)(UINTN)(x)

#define PCI_DEVICE_END    0xFF

#ifndef EFI_DEADLOOP 
 #define EFI_DEADLOOP()    { volatile int __iii; __iii = 1; while (__iii); }
#endif

  #define PCH_CPU_HP_SMI_GPP_GROUP  GPIO_SKL_H_GROUP_GPP_E
  #define PCH_CPU_HP_SMI_GPP_PAD    GPIO_SKL_H_GPP_E6

//
// 
//
VOID
InitHpGPI(
  );

EFI_STATUS
EnableGpiChildHandler (
  );

EFI_STATUS
RasInitGpiChildCallback (
  IN EFI_HANDLE     DispatchHandle,
  IN CONST VOID    *mGpiContext,
  IN OUT VOID      *CommBuffer      OPTIONAL,
  IN OUT UINTN     *CommBufferSize  OPTIONAL
  );


VOID
HpGPIHandler(
  );

EFI_STATUS
RasPeriodicChildCallback (
  IN  EFI_HANDLE                  PeriodicTimerHandle,
  IN  CONST VOID     *mPeriodicTimer,
  IN  OUT VOID      *CommBuffer   OPTIONAL,
  IN  OUT UINTN     *CommBufferSize OPTIONAL
  );

VOID
RegisterPeriodicTimer(
  VOID
  );

EFI_STATUS
RasPeriodicTimerSmiHandler(
  VOID
  );

EFI_STATUS
CpuRasHandler(
  VOID
  );

EFI_STATUS
MemRasInit(
  VOID
  );

EFI_STATUS
MemRasHandler(
  VOID
  );
  
EFI_STATUS
IioRasHandler(
  VOID
  );

EFI_STATUS
GetSourceOfGpi(
  VOID
  );

typedef
EFI_STATUS
(EFIAPI *GET_SOURCE_OF_GPI) (
  VOID
  );

typedef
EFI_STATUS
(EFIAPI *CPU_RAS_HANDLER) (
  VOID
  );

typedef
EFI_STATUS
(EFIAPI *MEM_RAS_HANDLER) (
  VOID
  );

typedef
EFI_STATUS
(EFIAPI *IIO_RAS_HANDLER) (
  VOID
  );

typedef struct _RAS_DISPATCHER_TABLE {
  GET_SOURCE_OF_GPI         GetSourceOfGpi;
  CPU_RAS_HANDLER           CpuRasHandler;
  MEM_RAS_HANDLER           MemRasHandler;
  IIO_RAS_HANDLER           IioRasHandler;
  UINTN                     EndOfEntry;
} RAS_DISPATCHER_TABLE;

typedef
EFI_STATUS
(EFIAPI *NEXT_ENTRY_POINTER) (
  VOID
  );

typedef struct _NEXT_STATE_ENTRY {
  NEXT_ENTRY_POINTER     NextEntryPointer;
} NEXT_STATE_ENTRY;

EFI_STATUS
GenerateRasSci (
  VOID
  );
  
VOID
InitMemoryRas (
  VOID
);

VOID
InitIioRas (
  VOID
);


VOID
EnableSwSmiCpuRasHandler (
  VOID
 );

EFI_STATUS
RasInitSwSmiChildCallback (
  IN EFI_HANDLE  DispatchHandle,
  IN CONST VOID     *mGpiContext,
  IN OUT VOID    *CommBuffer      OPTIONAL,
  IN OUT UINTN   *CommBufferSize  OPTIONAL
);

EFI_STATUS
RasSmiHandler(
  VOID
 );

EFI_STATUS
UpdateAcpiMemoryParams (
  IN UINT8         SocketId,
  IN UINT8         BranchId,
  IN BOOLEAN       Ejection
);

VOID
ModifyCacheLines (
   UINTN  StartAddress,
   UINTN  Length,
   UINTN  CacheLineSize
   );

VOID
ModifyCacheLinesWithFlush (
   UINTN  StartAddress,
   UINTN  Length,
   UINTN  CacheLineSize
   );

#endif
