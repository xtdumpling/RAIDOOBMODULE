//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
/** @file
  PEI definitions
*/

#ifndef __PEI__H__
#define __PEI__H__

#include "Efi.h"
#include "Ffs.h"

// Include files from EDKII
// MdePkg
#include <Pi/PiPeiCis.h>
#include <Ppi/CpuIo.h>
#include <Ppi/PciCfg2.h>
#include <Ppi/MemoryDiscovered.h>
#include <Ppi/MasterBootMode.h>
#include <Ppi/BootInRecoveryMode.h>
#include <Ppi/DxeIpl.h>
#include <Ppi/EndOfPeiPhase.h>
// IntelFrameworkPkg
#include <Ppi/PciCfg.h>

#ifdef __cplusplus
extern "C" {
#endif


// Ppi/PciCfg2.h
typedef EFI_PEI_PCI_CFG_PPI_PCI_ADDRESS PEI_PCI_CFG_PPI_PCI_ADDRESS;

#if BACKWARD_COMPATIBLE_MODE
//compatibility definition
//for PI 0.91 Modules that do not use Modify funciton 
typedef EFI_PEI_PCI_CFG2_PPI EFI_PEI_PCI_CFG_PPI;
#endif

// Pi/PiPeiCis.h
typedef EFI_PEIM_ENTRY_POINT2 EFI_PEIM_ENTRY_POINT;

// Ppi/MemoryDiscovered.h
#define EFI_PEI_PERMANENT_MEMORY_INSTALLED_PPI EFI_PEI_PERMANENT_MEMORY_INSTALLED_PPI_GUID

// Ppi/BootInRecoveryMode.h
#define EFI_PEI_BOOT_IN_RECOVERY_MODE_PEIM_PPI EFI_PEI_BOOT_IN_RECOVERY_MODE_PPI
/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
