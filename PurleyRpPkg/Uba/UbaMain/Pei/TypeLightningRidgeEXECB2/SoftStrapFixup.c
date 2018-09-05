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


PLATFORM_SOFTSTRAP_FIXUP_ENTRY  TypeLightningRidgeEXECB2SoftStrapTable[] = 
{
// BoardID, strap_num, low_bit, bit_length, value

//
// Bifurcation of PCIE ports
//
  {TypeLightningRidgeEXECB2,        9,  0,  2, 0x00}, //PCIEPCS1
  {TypeLightningRidgeEXECB2,        9,  2,  2, 0x03}, //PCIEPCS2
  {TypeLightningRidgeEXECB2,        9,  4,  2, 0x00}, //PCIELR2/PCIELR1
  {TypeLightningRidgeEXECB2,        9,  6,  1, 0x00}, //DMILR
  {TypeLightningRidgeEXECB2,        9,  8,  3, 0x06}, //DMIPCIEPORTSEL
  {TypeLightningRidgeEXECB2,        9,  11, 1, 0x00}, //GB_PCIE_EN
  {TypeLightningRidgeEXECB2,        9,  18, 2, 0x01}, //PCIEP1MODE
  {TypeLightningRidgeEXECB2,        9,  20, 2, 0x01}, //PCIEP2MODE

//
// GSX bus used for serial I/O expander?
//
  {TypeLightningRidgeEXECB2,        9,  17,  1, 0x00}, //SERIAL_GPIO_EXPANDER 

//
// END OF LIST
//
  {TypePlatformUnknown,     0,   0, 0,  0                     }  // END OF LIST
};

PLATFORM_SOFT_STRAP_UPDATE_TABLE  TypeLightningRidgeEXECB2SoftStrapUpdate =
{
  PLATFORM_SOFT_STRAP_UPDATE_SIGNATURE,
  PLATFORM_SOFT_STRAP_UPDATE_VERSION,
  TypeLightningRidgeEXECB2SoftStrapTable
};

EFI_STATUS
TypeLightningRidgeEXECB2InstallSoftStrapData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
)
{
  EFI_STATUS                            Status;

  Status = UbaConfigPpi->AddData (
                                 UbaConfigPpi, 
                                 &gPlatformSoftStrapConfigDataGuid, 
                                 &TypeLightningRidgeEXECB2SoftStrapUpdate, 
                                 sizeof(TypeLightningRidgeEXECB2SoftStrapUpdate)
                                 );

  return Status;
}

