/**

Copyright (c)  1999 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file SvSmmHandler.h

  Header file for SV SMM Handler Driver.

Revision History:

**/

#ifndef _SV_SMM_DRIVER_H
#define _SV_SMM_DRIVER_H

#include <Protocol/SvSmmProtocol.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiLib.h>
#include <Library/GpioLib.h>
#include <Protocol/SmmBase2.h>
#include <Protocol/QuiesceProtocol.h>
#include <Protocol/GlobalNvsArea.h>
#include <Protocol/SmmSwDispatch2.h>
#include <Protocol/MemRasProtocol.h>
#include <Protocol/CpuCsrAccess.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/PciLib.h>
#include <GpioPinsSklH.h>
#include <Ras/RasStateMachine.h>
//#include "QuiesceSupport.h"
//#include "QuiesceMain.h"
#include "MemRas.h"

#define SVOS_SMI_MAX_PARAMETER 511
#define QUIESCE_TEST_DATA1 0xa5a5a5a5
#define QUIESCE_TEST_DATA2 0xa5


#pragma pack(1)
//
// Related data structures definition
//
typedef struct _SV_SMM_PARAMETER {
  UINT64  Command;
  UINT64  Parameter[SVOS_SMI_MAX_PARAMETER];
} SV_SMM_PARAMETER;
#pragma pack()

EFI_STATUS
EFIAPI
SVExtenderHandler(
  VOID);

#define PCI_FUNCTION_NUMBER_PCH_LPC               0
#define PCI_DEVICE_NUMBER_PCH_LPC                 31
#define DEFAULT_PCI_BUS_NUMBER_PCH                0
#define R_PCH_LPC_GPIO_BASE                       0x48

#endif
