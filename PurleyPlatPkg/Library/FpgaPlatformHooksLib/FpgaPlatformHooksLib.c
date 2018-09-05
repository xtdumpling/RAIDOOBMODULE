/**
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c)  1999 - 2017 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


    @file FpgaPlatformHooksLib.c

    FPGA Platform Hooks file

**/

#include <Base.h>
#include <PiPei.h>
#include <Library/GpioLib.h>
#include <Library/BaseLib.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Library/HobLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MmPciBaseLib.h>
#include <Platform.h>
#include <GpioPinsSklH.h>
#include <PchAccess.h>
#include <Guid/PlatformInfo.h>
#include <Ppi/SystemBoard.h>

/**
    
    Get Blue Bitstream index according to platform default policy  

    @param socket                          socket number
    @param FpgaBitStreamGuidIndex   return Ptr
    
    @retval EFI_SUCCESS       Operation completed successfully.
    @retval Otherwise            Operation failed.

**/


EFI_STATUS
EFIAPI
GetBBSIndexFromPlatformPolicy (
  IN   UINT8             Socket,
  OUT UINT8             *DefaultFpgaBitStreamGuidIndex
  )
{
  EFI_STATUS                  Status = EFI_UNSUPPORTED;  
  EFI_PEI_SERVICES            **PeiServices;  
  SYSTEM_BOARD_PPI            *SystemBoard;

  if (DefaultFpgaBitStreamGuidIndex == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *DefaultFpgaBitStreamGuidIndex = 0xFF;

  //
  // Get system board PPI
  //
  PeiServices = (EFI_PEI_SERVICES **)GetPeiServicesTablePointer ();
    
  Status = (**PeiServices).LocatePpi (
                  PeiServices,
                  &gEfiPeiSystemBoardPpiGuid,
                  0,
                  NULL,
                  &SystemBoard
                  );
  ASSERT_EFI_ERROR (Status);

  *DefaultFpgaBitStreamGuidIndex = SystemBoard->GetDefaultFpgaBbsIndex(Socket);
  return Status;
}

/**
    
    Get Blue Bitstream index according to platform HSSI card design  

    @param socket                          socket number
    @param FpgaBitStreamGuidIndex          return BBS index
    @param FpgaHssiCardID                  return HSSI card ID
    
    @retval EFI_SUCCESS                    Operation completed successfully.
    @retval Otherwise                      Operation failed.

**/
EFI_STATUS
EFIAPI
GetBBSIndexFromHssiCardId (
  IN  UINT8             Socket,
  OUT UINT8             *FpgaBitStreamGuidIndex,
  OUT UINT8             *FpgaHssiCardID
  )
{
  EFI_STATUS                  Status = EFI_UNSUPPORTED;  
  EFI_PEI_SERVICES            **PeiServices;  
  SYSTEM_BOARD_PPI            *SystemBoard;

  if ((FpgaBitStreamGuidIndex == NULL) || (FpgaHssiCardID == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  *FpgaBitStreamGuidIndex = 0xFF;
  *FpgaHssiCardID = 0;

  //
  // Get the Fpga Hssi Card information and its map table according platform design
  //
  PeiServices = (EFI_PEI_SERVICES **)GetPeiServicesTablePointer ();
    
  Status = (**PeiServices).LocatePpi (
                  PeiServices,
                  &gEfiPeiSystemBoardPpiGuid,
                  0,
                  NULL,
                  &SystemBoard
                  );
  ASSERT_EFI_ERROR (Status);

  Status = SystemBoard->GetFpgaHssiCardBbsInfo (Socket, FpgaBitStreamGuidIndex, FpgaHssiCardID);

  return Status;
}


/**
    
    Enables/Disables Clock and Power for FPGA  

    @param socket               socket number
    
    @retval EFI_SUCCESS       Operation completed successfully.
    @retval Otherwise            Operation failed.

**/
EFI_STATUS
EFIAPI
FpgaPoweroff(
  IN  UINT8  Socket
)
{
  EFI_STATUS                       Status = EFI_UNSUPPORTED;
  EFI_PEI_SERVICES                 **PeiServices;  
  SYSTEM_BOARD_PPI                 *SystemBoard;
  //
  // Get the Fpga GPIO number for Power control according platform design
  //

  PeiServices = (EFI_PEI_SERVICES **)GetPeiServicesTablePointer ();
      
  Status = (**PeiServices).LocatePpi (
                  PeiServices,
                  &gEfiPeiSystemBoardPpiGuid,
                  0,
                  NULL,
                  &SystemBoard
                  );
  
  ASSERT_EFI_ERROR (Status);

  Status = SystemBoard->SetFpgaGpioOff(Socket);

  return Status;
}
