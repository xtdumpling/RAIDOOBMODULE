/**
  This file contains 'Framework Code' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may not be modified, except as allowed by
  additional terms of your license agreement.
**/
/**
// **************************************************************************
// *                                                                        *
// *      Intel Restricted Secret                                           *
// *                                                                        *
// *      KTI Reference Code                                                *
// *                                                                        *
// *      ESS - Enterprise Silicon Software                                 *
// *                                                                        *
// *      Copyright (c) 2004 - 2016 Intel Corp.                             *
// *                                                                        *
// *      This program has been developed by Intel Corporation.             *
// *      Licensee has Intel's permission to incorporate this source code   *
// *      into their product, royalty free.  This source code may NOT be    *
// *      redistributed to anyone without Intel's written permission.       *
// *                                                                        *
// *      Intel specifically disclaims all warranties, express or           *
// *      implied, and all liability, including consequential and other     *
// *      indirect damages, for the use of this code, including liability   *
// *      for infringement of any proprietary rights, and including the     *
// *      warranties of merchantability and fitness for a particular        *
// *      purpose.  Intel does not assume any responsibility for any        *
// *      errors which may appear in this code nor any responsibility to    *
// *      update it.                                                        *
// *                                                                        *
// **************************************************************************
// **************************************************************************
// *                                                                        *
// *  PURPOSE:                                                              *
// *                                                                        *
// *      Data structures for Electrical parameter Data for platform(s).    *
// *                                                                        *
// **************************************************************************
**/
#include "UbaMainPei.h"
#include "KtiLib.h"
#include "KtiMisc.h"
#include "SysHost.h"
#include "SysFunc.h"
#include "KtiPlatform.h"
#include "KtiSetupDefinitions.h"
#include <Library/UbaKtiLib.h>

extern EFI_GUID gPlatformKtiEparamUpdateDataGuid;
ALL_LANES_EPARAM_LINK_INFO  KtiNeonCityEcbAllLanesEparamTable[] = {
  //
  // SocketID, Freq, Link, TXEQL, CTLEPEAK
  //

  //
  // Socket 0
  //
  {0x0, (1 << SPEED_REC_96GT) | (1 << SPEED_REC_104GT), (1 << KTI_LINK0), 0x2D39333F, ADAPTIVE_CTLE},
  {0x0, (1 << SPEED_REC_96GT) | (1 << SPEED_REC_104GT), (1 << KTI_LINK1), 0x2D35373F, ADAPTIVE_CTLE},

  //
  // Socket 1
  //
  {0x1, (1 << SPEED_REC_96GT) | (1 << SPEED_REC_104GT), (1 << KTI_LINK0), 0x2F39353F, ADAPTIVE_CTLE},
  {0x1, (1 << SPEED_REC_96GT) | (1 << SPEED_REC_104GT), (1 << KTI_LINK1), 0x2B35353F, ADAPTIVE_CTLE}
};

//PER_LANE_EPARAM_LINK_INFO  KtiNeocityEcbPerLaneEparamTable[] = { 0 };
PLATFORM_KTI_EPARAM_UPDATE_TABLE  TypeNeonCityEcbKtiEparamUpdate =
{
  PLATFORM_KTIEP_UPDATE_SIGNATURE,
  PLATFORM_KTIEP_UPDATE_VERSION,
  KtiNeonCityEcbAllLanesEparamTable,
  sizeof (KtiNeonCityEcbAllLanesEparamTable),
  NULL,
  0
};

EFI_STATUS
TypeNeonCityEPECBInstallKtiEparamData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
)
{
  EFI_STATUS                            Status;
  
  Status = UbaConfigPpi->AddData (
                                 UbaConfigPpi, 
                                 &gPlatformKtiEparamUpdateDataGuid, 
                                 &TypeNeonCityEcbKtiEparamUpdate, 
                                 sizeof(TypeNeonCityEcbKtiEparamUpdate)
                                 );

  return Status;
}
