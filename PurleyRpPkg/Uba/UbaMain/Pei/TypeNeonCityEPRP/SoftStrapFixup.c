//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/** @file
    Soft Strap update.

  Copyright (c) 2014, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
**/

#include "UbaMainPei.h"

#include <Library/UbaSoftStrapUpdateLib.h>


//
// From WBG Soft Straps WIP.xlsx
//
#define WBG_DOWNSKU_STRAP_DSKU      0x80046000
#define WBG_DOWNSKU_STRAP_BSKU      0x8004E003
#define WBG_DOWNSKU_STRAP_TSKU      0x00044000


PLATFORM_SOFTSTRAP_FIXUP_ENTRY  TypeNeonCityEPRPSoftStrapTable[] = 
{
// BoardID, strap_num, low_bit, bit_length, value

//
// Bifurcation of PCIE ports
//
  {TypeNeonCityEPRP,        9,  0,  2, 0x00}, //PCIEPCS1
  {TypeNeonCityEPRP,        9,  2,  2, 0x03}, //PCIEPCS2
  {TypeNeonCityEPRP,        9,  4,  2, 0x00}, //PCIELR2/PCIELR1
  {TypeNeonCityEPRP,        9,  6,  1, 0x00}, //DMILR
  {TypeNeonCityEPRP,        9,  8,  3, 0x06}, //DMIPCIEPORTSEL
  {TypeNeonCityEPRP,        9,  11, 1, 0x00}, //GB_PCIE_EN
  {TypeNeonCityEPRP,        9,  18, 2, 0x01}, //PCIEP1MODE
  {TypeNeonCityEPRP,        9,  20, 2, 0x01}, //PCIEP2MODE

//
// GSX bus used for serial I/O expander?
//
  {TypeNeonCityEPRP,        9,  17,  1, 0x00}, //SERIAL_GPIO_EXPANDER 

//
// END OF LIST
//
  {TypePlatformUnknown,     0,   0, 0,  0                     }  // END OF LIST
};

PLATFORM_SOFT_STRAP_UPDATE_TABLE  TypeNeonCityEPRPSoftStrapUpdate =
{
  PLATFORM_SOFT_STRAP_UPDATE_SIGNATURE,
  PLATFORM_SOFT_STRAP_UPDATE_VERSION,
  TypeNeonCityEPRPSoftStrapTable
};

EFI_STATUS
TypeNeonCityEPRPInstallSoftStrapData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
)
{
  EFI_STATUS                            Status;

  Status = UbaConfigPpi->AddData (
                                 UbaConfigPpi, 
                                 &gPlatformSoftStrapConfigDataGuid, 
                                 &TypeNeonCityEPRPSoftStrapUpdate, 
                                 sizeof(TypeNeonCityEPRPSoftStrapUpdate)
                                 );

  return Status;
}

