/**
Copyright (c) 2007 - 2015 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file LastBootErrorLog.h

---------------------------------------------------------------------*/

#ifndef _LAST_BOOT_ERROR_LOG_H_
#define _LAST_BOOT_ERROR_LOG_H_

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/PciExpressLib.h>
#include <IndustryStandard/Pci22.h>
#include <Cpu/CpuBaseLib.h>
#include <Library/ProcMemErrReporting.h>
#include <WheaPlatformDefs.h>
#include <Protocol/WheaBootProtocol.h>
#include <Library/HobLib.h>
#include <Guid/HobList.h>
#include <Protocol/MemRasProtocol.h>
#include <Protocol/Smbios.h>
#include <Protocol/IioUds.h>
#include <Protocol/IioSystem.h>
#include <Protocol/CpuCsrAccess.h>
#include "Library/WheaSiliconHooksLib/WheaSiliconHooksLib.h"
// APTIOV_SERVER_OVERRIDE_RC_START : Added to skip LastBoot Error Handler for cold boot.
#include <Guid/MemoryMapData.h>
// APTIOV_SERVER_OVERRIDE_RC_END : Added to skip LastBoot Error Handler for cold boot.

#define PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_8  7
#define PCH_PCIE_RROT_PORT_AER_UES   0x104

#define UEFI_PROC_UNKNOWN_ERR_TYPE    00
#define UEFI_PROC_CACHE_ERR_TYPE		  01
#define UEFI_PROC_TLB_ERR_TYPE        02
#define UEFI_PROC_BUS_ERR_TYPE        04
#define UEFI_PROC_MICRO_ARCH_ERR_TYPE 0x8

#ifndef PCI_HEADER_TYPE_OFFSET
  #define PCI_HEADER_TYPE_OFFSET      0x0E
#endif

#define ONLY_REGISTER_OFFSET(x)  (x & 0xFFF)


#endif

