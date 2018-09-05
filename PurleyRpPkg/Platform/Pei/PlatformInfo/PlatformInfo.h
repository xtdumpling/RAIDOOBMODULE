//
// This file contains an 'Intel Pre-EFI Module' and is licensed
// for Intel CPUs and Chipsets under the terms of your license
// agreement with Intel or your vendor.  This file may be
// modified by the user, subject to additional terms of the
// license agreement
//
/**

Copyright (c)  1999 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

  @file PlatformInfo.h

  Platform Info Driver.

**/

#ifndef _PLATFORM_INFO_H_
#define _PLATFORM_INFO_H_

#include <PiPei.h>
#include <Ppi/CpuIo.h>
#include <Ppi/PciCfg.h>
#include <Ppi/Smbus.h>
#include <Ppi/Spi.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/PlatformHooksLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/SetupLib.h>
#include <Guid/SocketVariable.h>
#include <Guid/SetupVariable.h>
#include <Guid/PlatformInfo.h>
#include <IndustryStandard/Pci22.h>
#include <Cpu/CpuRegs.h>
#include <Library/MmPciBaseLib.h>
#include <GpioPinsSklH.h>
#include <Library/GpioLib.h>
#include <Platform.h>
#include <BoardTypes.h>
#include "SioRegs.h"
#include <Register/PchRegsSpi.h>
#include <PchAccess.h>
#include <FlashMap.h>
#include <Register/PchRegsLpc.h>
// APTIOV_SERVER_OVERRIDE_RC_START : Use AMI SPI services
//#include <Platform/SpiFvbServices/SpiFlashDevice.h>
// APTIOV_SERVER_OVERRIDE_RC_END : Use AMI SPI services
#include <Ppi/SpiSoftStraps.h>
#include <Library/ReportStatusCodeLib.h>
#include <Library/PlatformStatusCodes.h>


#define EFI_PLATFORMINFO_DRIVER_PRIVATE_SIGNATURE SIGNATURE_32 ('P', 'I', 'N', 'F')

//
// CPU Model
//
#define  INVALID_MODEL             0x0

#define R_SB_SPI_FDOC         0xB0
#define R_SB_SPI_FDOD         0xB4
//
// APTIOV_SERVER_OVERRIDE_RC_START : Added to resolve redefinition error.
//
#ifndef SPI_OPCODE_READ_INDEX
#define SPI_OPCODE_READ_INDEX            4
#endif
//
// APTIOV_SERVER_OVERRIDE_RC_END : Added to resolve redefinition error.
//
#define PDR_REGION_START_OFFSET		0x0

typedef union BOARD_ID
{
   struct{
      UINT8  BoardID0            :1;
      UINT8  BoardID1            :1;
      UINT8  BoardID2            :1;
      UINT8  BoardID3            :1;
      UINT8  BoardID4            :1;
      UINT8  BoardRev0           :1;
      UINT8  BoardRev1           :1;
      UINT8  Rsvd                :1;
   }BoardID;
}BOARD_ID;

typedef union RISER_ID
{
   struct{
      UINT8  RiserID0            :1;
      UINT8  RiserID1            :1;
      UINT8  RiserID2            :1;
      UINT8  RiserID3            :1;
      UINT8  Rsvd                :4;
   }RiserID;
}RISER_ID;



EFI_STATUS
PdrGetPlatformInfo (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  OUT EFI_PLATFORM_INFO   *PlatformInfoHob
  );

EFI_STATUS
GPIOGetPlatformInfo (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  OUT EFI_PLATFORM_INFO   *PlatformInfoHob
);

#endif
