//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/** @file
  Whea Platform hook functions and platform data, which needs to be 
  ported. 

  Copyright (c) 2009 - 2015, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
**/

#ifndef _WHEA_PLATFORM_HOOKS_H_
#define _WHEA_PLATFORM_HOOKS_H_

#include <PiDxe.h>

#include <WheaPlatformDefs.h>

#include <Protocol/SmmBase2.h>
#include <Protocol/WheaPlatformSupport.h>

#include <Library/BaseLib.h>

// Data & Tables defined in WheaPlatform hooks

extern SYSTEM_GENERIC_ERROR_SOURCE      SysGenErrSources[];
extern UINTN                            NumSysGenericErrorSources;
extern SYSTEM_NATIVE_ERROR_SOURCE       SysNativeErrSources[];
extern UINTN                            NumNativeErrorSources;


#define MCI_REG_BASE    0x400
#define R_PCH_LPC_ACPI_BASE                       0x40
#define PCI_FUNCTION_NUMBER_PCH_LPC               0
#define PCI_DEVICE_NUMBER_PCH_LPC                 31
#define DEFAULT_PCI_BUS_NUMBER_PCH                0
#define R_PCH_LPC_GPIO_BASE                       0x48
#define R_PCH_GPIO_GPI_INV                        0x2C
#define R_PCH_ACPI_GPE0a_STS                      0x20
#define R_PCH_ALT_GP_SMI_EN                       0x38
#define R_PCH_ACPI_GPE0a_EN                       0x28
#define R_PCH_LPC_GPI_ROUT                        0xB8

 



#define RBOX0_CTL      (MCI_REG_BASE + 0x00)
#define RBOX1_CTL      (MCI_REG_BASE + 0x04)
#define UBOX_CTL       (MCI_REG_BASE + 0x18)
#define WBOX_CTL       (MCI_REG_BASE + 0x1C)
#define MBOX0_CTL      (MCI_REG_BASE + 0x20)
#define MBOX1_CTL      (MCI_REG_BASE + 0x24)
#define BBOX0_CTL      (MCI_REG_BASE + 0x28)
#define BBOX1_CTL      (MCI_REG_BASE + 0x2C)
#define SBOX0_CTL      (MCI_REG_BASE + 0x30)
#define SBOX1_CTL      (MCI_REG_BASE + 0x34)
#define CBOX0_CTL      (MCI_REG_BASE + 0x38)
#define CBOX4_CTL      (MCI_REG_BASE + 0x3C)
#define CBOX2_CTL      (MCI_REG_BASE + 0x40)
#define CBOX6_CTL      (MCI_REG_BASE + 0x44)
#define CBOX1_CTL      (MCI_REG_BASE + 0x48)
#define CBOX5_CTL      (MCI_REG_BASE + 0x4C)
#define CBOX3_CTL      (MCI_REG_BASE + 0x50)
#define CBOX7_CTL      (MCI_REG_BASE + 0x54)

// Machine Check error log registers
#define MCE_RBOX_MASK  0x00000FFF
#define MCE_UBOX_MASK  0x00003FFF
#define MCE_WBOX_MASK  0x00000005
#define MCE_MBOX_MASK  0x003FF7FF
#define MCE_BBOX_MASK  0x7FF7FFFF
#define MCE_SBOX_MASK  0x0003E3FF
#define MCE_CBOX_MASK  0x007FFFFF

//
// Per the IA32 manual, writing all 1s to the Core MCI CTL registers would enable
// MCE error logging. The core would enable only those bits that are supported
// leaving the reserved bits untouched.
//

#define MCE_IFU_MASK   0xFFFFFFFF
#define MCE_DCU_MASK   0xFFFFFFFF
#define MCE_DTLB_MASK  0xFFFFFFFF
#define MCE_MLC_MASK   0xFFFFFFFF

#define MCE_MISC2_REG  0x280



//
// This is copied from Include\Acpi.h
//
#define CREATOR_ID_INTEL 0x4C544E49  //"LTNI""INTL"(Intel)
#define CREATOR_REV_INTEL 0x20090903

//
// Update Platform & OEM ID's in Whea ACPI tables.
//
VOID
UpdateAcpiTableIds(
  EFI_ACPI_DESCRIPTION_HEADER     *TableHeader
  );

//
// Memory device location informaton
//
EFI_STATUS
WheaGetMemoryFruInfo (
  IN MEMORY_DEV_INFO    *MemInfo,
  OUT EFI_GUID          *FruId,
  OUT CHAR8             *FruString,
  OUT UINT16            *Card 
  );

EFI_STATUS
ProgramWheaSignalsHook (
  BOOLEAN                       EnableWhea,
  EFI_SMM_BASE2_PROTOCOL       *SmmBase,
  EFI_SMM_SYSTEM_TABLE2        *Smst
  );

EFI_STATUS
TriggerWheaSci (
  EFI_SMM_BASE2_PROTOCOL         *SmmBase,
  EFI_SMM_SYSTEM_TABLE2         *Smst
  );

EFI_STATUS
TriggerWheaNmi (
  EFI_SMM_BASE2_PROTOCOL         *SmmBase,
  EFI_SMM_SYSTEM_TABLE2          *Smst
  );

EFI_STATUS
InitWheaPlatformHookLib (
  void
  );

#endif  //_WHEA_PLATFORM_HOOKS_H_

