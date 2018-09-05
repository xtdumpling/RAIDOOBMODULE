//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
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


    @file OemIioInit.h

    Oem Hooks file for UncoreInit Module

**/
#ifndef _IIO_OEM_IIO_INIT_H_
#define _IIO_OEM_IIO_INIT_H_

VOID
OemDefaultIioPortBifurationInfo(
    IN  IIO_GLOBALS  *IioGlobalData
  );

VOID
OemInitIioPortBifurcationInfo (
  IN  IIO_GLOBALS  *IioGlobalData
  );

VOID
OemSetIioDefaultValues(
  IN OUT IIO_GLOBALS *IioGlobalData
  );

VOID
OemSetPlatformDataValues(
  IN OUT IIO_GLOBALS *IioGlobalData
  );

VOID
OemDmiPreInit (  
  IIO_GLOBALS  *IioGlobalData,
  UINT8         IioIndex,
  UINT8         PortIndex
  );

VOID
OemDmiDeviceInit (  
  IIO_GLOBALS  *IioGlobalData,
  UINT8         IioIndex,
  UINT8         PortIndex
  );

VOID
OemDmiResourceAssignedInit (
     IIO_GLOBALS   *IioGlobalData,
     UINT8         IioIndex,
     UINT8         PortIndex
    );

VOID
OemIioUplinkPortDetails(
    IIO_GLOBALS   *IioGlobalData, 
    UINT8   IioIndex,
    UINT8   *PortIndex,
    UINT8   *Bus,
    UINT8   *Dev,
    UINT8   *Func
);

VOID
OemIioEarlyWorkAround (
    IIO_GLOBALS                     *IioGlobalData,
    UINT8                           IioIndex,
    IIO_INIT_ENUMERATION            Phase
  );

VOID
OemOverrideIioRxRecipeSettings (
    IIO_GLOBALS                     *IioGlobalData,
    UINT8                           IioIndex
  );

VOID
OemPublishOpaSocketMapHob (
  IN IIO_GLOBALS *IioGlobalData
 );

#ifndef MINIBIOS_BUILD
VOID
OemGetIioPlatformInfo(
  IN  IIO_GLOBALS *IioGlobalData
  );

//
// 4926908: SKX not support Vcp
//

UINT8
CheckSoftSimulationPlatform(
  VOID
  );

VOID
OemUpdateSrisConfig(
  IN OUT IIO_GLOBALS   *IioGlobalData
  );
#endif // MINIBIOS_BUILD

#ifndef IA32
VOID
OemUpdateHidePCIePortMenu (
  IN  IIO_GLOBALS  *IioGlobalData
  );


VOID
OemCheckForOtherSupport (
  IIO_GLOBALS  *IioGlobalData
  );

#endif // IA32
#endif //_IIO_OEM_IIO_INIT_H_
