/**

Copyright (c) 1999 - 2012, Intel Corporation. All rights reserved.<BR>
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file PchPeimLib.h
  

**/

#ifndef __PCH_PEIM_LIB__
#define __PCH_PEIM_LIB__

#include <Base.h>
#include <PchRegs.h>
#include <SioRegs.h>
#include <Platform.h>
#include <GpioInitData.h>
#include <Library/PeiServicesLib.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/PciLib.h>
#include <Library/PchInfoLib.h>
#include <Library/PlatformHooksLib.h>
#include <Guid/SetupVariable.h>
#include <Ppi/SystemBoard.h>
#include <Ppi/PchPlatformPolicy.h>
#include <Ppi/SpiSoftStraps.h>
#include <Library/HobLib.h>
#include <Guid/PlatformInfo.h>


#define LEAVE_POWER_STATE_UNCHANGED 0xFF

//
// Reset Generator I/O Port
//
#define RESET_GENERATOR_PORT  0xCF9

//-----------------------------------------------------------------------;
// PCH: Chipset Configuration Register Equates
//-----------------------------------------------------------------------;
#define ICH_RCRB_IRQ0                       0
#define ICH_RCRB_IRQA                       1
#define ICH_RCRB_IRQB                       2
#define ICH_RCRB_IRQC                       3
#define ICH_RCRB_IRQD                       4
#define ICH_RCRB_PIRQA                      0
#define ICH_RCRB_PIRQB                      1
#define ICH_RCRB_PIRQC                      2
#define ICH_RCRB_PIRQD                      3
#define ICH_RCRB_PIRQE                      4
#define ICH_RCRB_PIRQF                      5
#define ICH_RCRB_PIRQG                      6
#define ICH_RCRB_PIRQH                      7


VOID
InitPchUsbConfig (
  IN PCH_USB_CONFIG                     *PchUsbConfig
  );

EFI_STATUS
SystemPchInit (
  IN CONST EFI_PEI_SERVICES          **PeiServices,
  IN SYSTEM_BOARD_PPI                *SystemBoard,
  IN PCH_PLATFORM_POLICY_PPI *PchPlatformPolicyPpi
  );

VOID
PchLpcInit (
  IN SYSTEM_CONFIGURATION        *SystemConfiguration
  );

VOID
PchRcrbInit (
  IN SYSTEM_CONFIGURATION        *SystemConfiguration
  );

VOID
PchSataInitPcdSet (
  IN SYSTEM_BOARD_PPI       *SystemBoard
  );
EFI_STATUS

/**

  GC_TODO: Add function description

  @param PeiServices - GC_TODO: add argument description

  @retval GC_TODO: add return values

**/
EFIAPI
PchReset (
  IN CONST EFI_PEI_SERVICES           **PeiServices
  )
;


#endif
