//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/*++

Copyright (c)  1999 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  IioInitLib.h

Abstract:

  This file provides required function headers publish for IioInitLib

--*/

#ifndef _IIO_INIT_LIB_H_
#define _IIO_INIT_LIB_H_

VOID
InitializeIioGlobalData (
  IIO_GLOBALS          *IioGlobalData
  );

IIO_STATUS
IioEarlyLinkInit(
  struct sysHost             *host
  );

IIO_STATUS
IioEarlyPostLinkInit(
  struct sysHost             *host
  );

VOID
IioOtherVariableInit (
  IN IIO_GLOBALS  *IioGlobalData,
  IN UINT8        IioIndex
  );

VOID
IioUpdatePcieConfigurationData (
  IN  IIO_GLOBALS *IioGlobalData,
  IN  UINT8       IioIndex
  );

VOID
DumpIioPciePortPcieLinkStatus (
  IN  IIO_GLOBALS          *IioGlobalData
  );


VOID
IioPostInit (
  IN  IIO_GLOBALS         *IioGlobalData,
  IN  UINT8               IioIndex
  );

VOID
IioInit (
  IN  IIO_GLOBALS         *IioGlobalData,
  IN  UINT8               IioIndex
  );

VOID
IioVMDEarlySetup (
  IN  IIO_GLOBALS        *IioGlobalData,
  IN  UINT8              IioIndex
  );

VOID
PcieLateCommonLibInit (
   IN IIO_GLOBALS       *IioGlobalData
  );

VOID
IioLatePostLinkTrainingPhase (
  IN  IIO_GLOBALS         *IioGlobalData,
  IN  UINT8               IioIndex
  );
  
VOID
NtbLateInit (
  IN  IIO_GLOBALS          *IioGlobalData,
  IN  UINT8                 IioIndex
  );

VOID
IioIoApicInitBootEvent (
  IN  IIO_GLOBALS         *IioGlobalData,
  IN  UINT8               IioIndex
  );

VOID
IioMiscHide (
  IN  IIO_GLOBALS            *IioGlobalData,
  IN  UINT8                  IioIndex,
  IN  UINT8                  Phase,
  IN OUT  IIO_DEVFUNHIDE_TABLE   *IioDevHides
  );

VOID
IioDisableLinkPorts (
  IN  IIO_GLOBALS        *IioGlobalData,
  IN  UINT8              IioIndex
  );

VOID
UpdateSsids (
  IN  IIO_GLOBALS        *IioGlobalData
  );

IIO_STATUS
IioEarlyInitializeEntry (
  IN  IIO_GLOBALS        *IioGlobalData
  );

IIO_STATUS
IioLateInitializeEntry (
  IN  IIO_GLOBALS        *IioGlobalData
  );

VOID
DmiPostInit (
    IIO_GLOBALS            *IioGlobalData,
    UINT8                  IioIndex
  );

VOID
DmiLinkReTrain (
  IN  IIO_GLOBALS                  *IioGlobalData,
  IN  UINT8                        IioIndex,
  IN  UINT8                        PortIndex
  );
#endif // _IIO_INIT_LIB_H_
