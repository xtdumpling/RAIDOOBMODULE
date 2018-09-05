/**

Copyright (c) 2008 - 2014, Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


   @file SvSmmSupport.c

  SV SMM Driver implementation.

**/

#include "SvSmmSupport.h"

EFI_SV_SERVICE_PROTOCOL mSvSmmService = {
  {
    0,
    NULL,
  },
  {
    GetSvosGasAddress,
    SetSvosGasAddress,
    GetSvosSmiDoorBell,
    SetSvosSmiDoorBell,
  }
};

SV_COMMUNICATION_DATA mSvCommData;

//
//Intel_IP_Start
//
/**

    Smm support driver entry point

    @param ImageHandle   EFI_HANDLE: A handle for the image that is initializing this driver
    @param SystemTable   EFI_SYSTEM_TABLE: A pointer to the EFI system table

    @retval EFI_SUCCESS:              Driver initialized successfully
    @retval EFI_UNSUPPORTED:           Failed to Install interface

**/
EFI_STATUS
EFIAPI
InitSvSmmSupport (
  IN      EFI_HANDLE                ImageHandle,
  IN      EFI_SYSTEM_TABLE          *SystemTable
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  EFI_HANDLE mHandle = NULL;

  Status = gBS->InstallMultipleProtocolInterfaces (&mHandle, &gEfiSvSmmProtocolGuid, &mSvSmmService, NULL);
  if(EFI_ERROR(Status)) {
    DEBUG ((EFI_D_ERROR, "Error, failed to install SvSmmProtocol: %x\n", Status));
    return EFI_UNSUPPORTED;
  }
  return Status;
}

/**

    Give access to the acpi gas address

    @param Address - Pointer to address that will be populated

    @retval None

**/
VOID
GetSvosGasAddress (
  OUT UINTN                               *Address
  )
{

  *Address = mSvCommData.AcpiData.AcpiGasAddress;
}

/**

    Set ACPI gas address

    @param Address - Pointer to address that will be populated

    @retval None

**/
VOID
SetSvosGasAddress (
  IN UINTN                               Address
  )
{
  mSvCommData.AcpiData.AcpiGasAddress = Address;
}

/**

    Set acpi smidoorbell

    @param SmiDoorBell - Pointer to address that is used to set acpi smidoorbell

    @retval None

**/
VOID
GetSvosSmiDoorBell (
  OUT UINT8                               *SmiDoorBell
  )
{
  *SmiDoorBell = mSvCommData.AcpiData.SmiDoorBell;
}

/**

    Set acpi smidoorbell

    @param SmiDoorBell - Pointer to address that is used to set acpi smidoorbell

    @retval None

**/
VOID
SetSvosSmiDoorBell (
  IN UINT8                               SmiDoorBell
  )
{
  mSvCommData.AcpiData.SmiDoorBell = SmiDoorBell;
}

//
//Intel_IP_End
// 
