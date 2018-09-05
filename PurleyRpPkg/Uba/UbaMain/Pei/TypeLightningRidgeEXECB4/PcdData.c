/*++
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/
/*++

Copyright (c) 2013 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  PcdData.c

Abstract:

--*/


#include "UbaMainPei.h"

#include <Library/UbaPcdUpdateLib.h>
#include <Library/PcdLib.h>

#define GPIO_SKL_H_GPP_B20      0x01010014

EFI_STATUS
TypeLightningRidgeEXECB4PlatformPcdUpdateCallback (
  VOID
)
{
  CHAR8     FamilyName[]  = "WildcatPass";

  CHAR8     BoardName[]   = "S2600WT ";

  UINTN     Size;
  UINT32    Data32;
  CHAR16    PlatformName[]   = L"TypeLightningRidgeEXECB4";
  UINTN     PlatformNameSize = 0;
  UINTN     PlatformFeatureFlag = 0;
  EFI_STATUS Status;
   
  //#Integer for BoardID, must match the SKU number and be unique.  
  Status = PcdSet16S (PcdOemSkuBoardID                      , TypeLightningRidgeEXECB4);
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status)) return Status;
  Status = PcdSet16S (PcdOemSkuBoardFamily                  , 0x30);
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status)) return Status;
  
  //# Number of Sockets on Board.
  Status = PcdSet32S (PcdOemSkuBoardSocketCount             , 4);
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status)) return Status;

  // Max channel and max DIMM
  Status = PcdSet32S (PcdOemSkuMaxChannel , 6);
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status)) return Status;
  Status = PcdSet32S (PcdOemSkuMaxDimmPerChannel , 2);
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status)) return Status;
  Status = PcdSetBoolS (PcdOemSkuDimmLayout, TRUE);
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status)) return Status;
  
  //#
  //# Misc.
  //#
  //# V_PCIE_PORT_PXPSLOTCTRL_ATNLED_OFF
  Status = PcdSet16S (PcdOemSkuMrlAttnLed                   , 0xc0);
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status)) return Status;
  
  //SDP Active Flag
  Status = PcdSet8S  (PcdOemSkuSdpActiveFlag                , 0x0);
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status)) return Status;
    
  //# Zero terminated string to ID family
  Size = AsciiStrSize (FamilyName);
  Status = PcdSetPtrS (PcdOemSkuFamilyName             , &Size, FamilyName);
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status)) return Status;
  
  //# Zero terminated string to Board Name
  Size = AsciiStrSize (BoardName);
  Status = PcdSetPtrS (PcdOemSkuBoardName              , &Size, BoardName);
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status)) return Status;
  
  PlatformNameSize = sizeof (PlatformName);
  Status = PcdSet32S (PcdOemSkuPlatformNameSize           , (UINT32)PlatformNameSize);
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status)) return Status;
  Status = PcdSetPtrS (PcdOemSkuPlatformName              , &PlatformNameSize, PlatformName);
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status)) return Status;

  //# FeaturesBasedOnPlatform
  Status = PcdSet32S (PcdOemSkuPlatformFeatureFlag           , (UINT32)PlatformFeatureFlag);
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status)) return Status;

  //# Assert GPIO
  Data32 = 0;
  Status = PcdSet32S (PcdOemSkuAssertPostGPIOValue, Data32);
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status)) return Status;
  Status = PcdSet32S (PcdOemSkuAssertPostGPIO, GPIO_SKL_H_GPP_B20);  
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status)) return Status;
     
  //# UplinkPortIndex
  Status = PcdSet8S (PcdOemSkuUplinkPortIndex, 5);
  if (EFI_ERROR(Status)) return Status;

  DEBUG ((EFI_D_INFO, "Uba Callback: PlatformPcdUpdateCallback is called!\n"));
  Status = TypeLightningRidgeEXECB4PlatformUpdateAcpiTablePcds ();
   
  //# Board Type Bit Mask
  PcdSet32S (PcdBoardTypeBitmask, 0);
  ASSERT_EFI_ERROR(Status);
  return Status;
}

PLATFORM_PCD_UPDATE_TABLE    TypeLightningRidgeEXECB4PcdUpdateTable = 
{
  PLATFORM_PCD_UPDATE_SIGNATURE,
  PLATFORM_PCD_UPDATE_VERSION,
  TypeLightningRidgeEXECB4PlatformPcdUpdateCallback
};

EFI_STATUS
TypeLightningRidgeEXECB4InstallPcdData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
)
{
  EFI_STATUS                            Status;
  
  Status = UbaConfigPpi->AddData (
                                 UbaConfigPpi,
                                 &gPlatformPcdConfigDataGuid, 
                                 &TypeLightningRidgeEXECB4PcdUpdateTable, 
                                 sizeof(TypeLightningRidgeEXECB4PcdUpdateTable)
                                 );

  return Status;
}

