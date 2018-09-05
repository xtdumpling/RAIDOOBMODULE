
//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/** @file
  This library class provides Mrc Oem Hooks Ppi wrapper.

  Copyright (c) 2011 - 2016, Intel Corporation. All rights reserved.
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/

#ifndef MINIBIOS_BUILD
#include <Uefi.h>
#include <SysFunc.h>
#include <Library/OemIioInit.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include "Ppi/MrcOemHooksPpi.h"

/**

  MRC wrapper code.
  Check Whether Mrc Oem Hooks Ppi initialized or not

  @param IioGlobalData - Pointer to IIO global data structure
  @param Host          - Pointer to sysHost structure

  @ret FALSE There was an issue, Host pointer not updated
  @ret TRUE  Host updated and MrcOemHooksPpi has been updated in the Host structure

**/
BOOLEAN 
IsMrcOemHooksPpiReadyIio (
  IN     IIO_GLOBALS *IioGlobalData,
  IN OUT SYSHOST **Host
  )
{
  BOOLEAN Result = FALSE;
  if (IioGlobalData) {
    *Host = (SYSHOST*) IioGlobalData->IioVar.IioOutData.PtrAddress.Address32bit.Value;
    if ((*Host) && (*Host)->MrcOemHooksPpi) {
      Result = TRUE;
    } else {
      DebugPrintRc ((*Host), SDBG_MIN, "MrcOemHooksPpi used before available!\n");
    }
  }
  return Result;
}
#endif // MINIBIOS_BUILD

VOID
OemGetIioPlatformInfo (
  IIO_GLOBALS *IioGlobalData
  )
{
#ifndef MINIBIOS_BUILD
  SYSHOST *Host;
  if (IsMrcOemHooksPpiReadyIio (IioGlobalData, &Host)) {
    Host->MrcOemHooksPpi->OemGetIioPlatformInfo (IioGlobalData);
  }
#endif // MINIBIOS_BUILD
}

VOID
OemIioUplinkPortDetails (
  IIO_GLOBALS   *IioGlobalData,
  UINT8         IioIndex,
  UINT8         *PortIndex,
  UINT8         *Bus,
  UINT8         *Dev,
  UINT8         *Func
  )
{
#ifndef MINIBIOS_BUILD
  SYSHOST *Host;
  if (IsMrcOemHooksPpiReadyIio (IioGlobalData, &Host)) {
    Host->MrcOemHooksPpi->OemIioUplinkPortDetails (IioGlobalData, IioIndex, PortIndex, Bus, Dev, Func);
  }
#endif // MINIBIOS_BUILD
}

VOID
OemIioEarlyWorkAround (
    IIO_GLOBALS                     *IioGlobalData,
    UINT8                           IioIndex,
    IIO_INIT_ENUMERATION            Phase
  )
{
#ifndef MINIBIOS_BUILD
  SYSHOST *Host;
  if (IsMrcOemHooksPpiReadyIio (IioGlobalData, &Host)) {
    Host->MrcOemHooksPpi->OemIioEarlyWorkAround (IioGlobalData, IioIndex, Phase);
  }
#endif // MINIBIOS_BUILD
}

VOID
OemOverrideIioRxRecipeSettings (
    IIO_GLOBALS                     *IioGlobalData,
    UINT8                           IioIndex
  )
{
#ifndef MINIBIOS_BUILD
  SYSHOST *Host;
  if (IsMrcOemHooksPpiReadyIio (IioGlobalData, &Host)) {
    Host->MrcOemHooksPpi->OemOverrideIioRxRecipeSettings (IioGlobalData, IioIndex);
  }
#endif // MINIBIOS_BUILD
}

VOID
OemDefaultIioPortBifurationInfo (
  IN  IIO_GLOBALS  *IioGlobalData
  )
{
#ifndef MINIBIOS_BUILD
  SYSHOST *Host;
  if (IsMrcOemHooksPpiReadyIio (IioGlobalData, &Host)) {
    Host->MrcOemHooksPpi->OemDefaultIioPortBifurationInfo (IioGlobalData);
  }
#endif // MINIBIOS_BUILD
}

VOID
OemInitIioPortBifurcationInfo (
  IN  IIO_GLOBALS  *IioGlobalData
  )
{
#ifndef MINIBIOS_BUILD
  SYSHOST *Host;
  if (IsMrcOemHooksPpiReadyIio (IioGlobalData, &Host)) {
    Host->MrcOemHooksPpi->OemInitIioPortBifurcationInfo (IioGlobalData);
  }
#endif // MINIBIOS_BUILD
}

VOID
OemDmiPreInit (
  IN IIO_GLOBALS    *IioGlobalData,
  IN UINT8          IioIndex,
  IN UINT8          PortIndex
  )
{  
#ifndef MINIBIOS_BUILD
  SYSHOST *Host;
  if (IsMrcOemHooksPpiReadyIio (IioGlobalData, &Host)) {
    Host->MrcOemHooksPpi->OemDmiPreInit (IioGlobalData, IioIndex, PortIndex);
  }
#endif // MINIBIOS_BUILD
}

VOID
OemDmiDeviceInit (
  IN IIO_GLOBALS    *IioGlobalData,
  IN UINT8          IioIndex,
  IN UINT8          PortIndex
  )
{
#ifndef MINIBIOS_BUILD
  SYSHOST *Host;
  if (IsMrcOemHooksPpiReadyIio (IioGlobalData, &Host)) {
    Host->MrcOemHooksPpi->OemDmiDeviceInit (IioGlobalData, IioIndex, PortIndex);
  }
#endif // MINIBIOS_BUILD
}

VOID
OemDmiResourceAssignedInit (
  IN IIO_GLOBALS    *IioGlobalData,
  IN UINT8          IioIndex,
  IN UINT8          PortIndex
  )
{
#ifndef MINIBIOS_BUILD
  SYSHOST *Host;
  if (IsMrcOemHooksPpiReadyIio (IioGlobalData, &Host)) {
    Host->MrcOemHooksPpi->OemDmiResourceAssignedInit (IioGlobalData, IioIndex, PortIndex);
  }
#endif // MINIBIOS_BUILD
}

VOID
OemSetIioDefaultValues (
  IN  IIO_GLOBALS  *IioGlobalData
  )
{
#ifndef MINIBIOS_BUILD
  SYSHOST *Host;
  if (IsMrcOemHooksPpiReadyIio (IioGlobalData, &Host)) {
    Host->MrcOemHooksPpi->OemSetIioDefaultValues (IioGlobalData);
  }
#endif // MINIBIOS_BUILD
}

VOID
OemSetPlatformDataValues (
  IN  IIO_GLOBALS  *IioGlobalData
  )
{
#ifndef MINIBIOS_BUILD
  SYSHOST *Host;
  if (IsMrcOemHooksPpiReadyIio (IioGlobalData, &Host)) {
    Host->MrcOemHooksPpi->OemSetPlatformDataValues (IioGlobalData);
  }
#endif // MINIBIOS_BUILD
}

VOID
OemPublishOpaSocketMapHob (
  IN  IIO_GLOBALS  *IioGlobalData
 )
{
#ifndef MINIBIOS_BUILD
  SYSHOST *Host;
  if (IsMrcOemHooksPpiReadyIio (IioGlobalData, &Host)) {
    Host->MrcOemHooksPpi->OemPublishOpaSocketMapHob (IioGlobalData);
  }
#endif // MINIBIOS_BUILD
}
