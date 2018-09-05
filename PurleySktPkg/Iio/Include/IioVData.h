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


  @file IioVData.h

  This file defines everything that's needed for IIO VData struct.

**/

#ifndef _IIOVDATA_H_
#define _IIOVDATA_H_

VOID
IioPreLinkDataInitPerSocket(
  IN OUT IIO_GLOBALS     *IioGlobalData,
  IN UINT8                Iio
  );

BOOLEAN
IsPciePortAvailable (
  IN  PCI_ROOT_BRIDGE_PCI_ADDRESS       PciAddress,
  IN  IIO_GLOBALS                       *IioGlobalData,
  IN  UINT8                              IioIndex
  );

BOOLEAN
IsPciePortDLActive (
  IN  IIO_GLOBALS    *IioGlobalData,
  IN  UINT8          IioIndex,
  IN  UINT8          PortIndex
  );

BOOLEAN
IsPciePortTPHCapable (
  IN  IIO_GLOBALS        *IioGlobalData,
  IN  UINT8              IioIndex,
  IN  UINT8              PortIndex
  );

BOOLEAN
IsPchPciePortsPopulated (
  IN  IIO_GLOBALS                         *IioGlobalData,
  IN  UINT8                               IioIndex
  );

BOOLEAN
IsIioPciePortsPopulated (
  IN  IIO_GLOBALS           *IioGlobalData,
  IN  UINT8                 IioIndex
);

BOOLEAN
IsIioPortPopulated (
  IN  IIO_GLOBALS         *IioGlobalData,
  IN  UINT8               IioIndex,
  IN  UINT8               PortIndex
  );

VOID
IssuePortRetrainLink (
  IN  IIO_GLOBALS                                     *IioGlobalData,
  IN  UINT8                                           IioIndex,
  IN  UINT8                                           PortIndex
  );

VOID
IioVData_DisablePciePortConfigure (
    IIO_GLOBALS                     *IioGlobalData,
    UINT8                           Iio,
    UINT8                           PortIndex
    );

BOOLEAN
IioVData_ConfigurePciePort (
  IN  IIO_GLOBALS                     *IioGlobalData,
  IN  UINT8                           Iio,
  IN  UINT8                           PortIndex
  );

VOID
IioVDataPostLinkInit (
  IN  IIO_GLOBALS                       *IioGlobalData,
  IN  UINT8                             Iio
  );

UINT8
CheckBifurcationMapPort(
    IN IIO_GLOBALS    *IioGlobalData,
    IN  UINT8         IioIndex,
    UINT8             PortIndex
    );

BOOLEAN
IsThisUplinkPort (
  IN  IIO_GLOBALS                       *IioGlobalData,
  IN  UINT8                             Iio,
  IN  UINT8                             PortIndex
  );

BOOLEAN
CheckPciePortEnable (
  IN  IIO_GLOBALS                       *IioGlobalData,
  IN  UINT8                             Iio,
  IN  UINT8                             PortIndex
  );

UINT8
CheckVarPortEnable (
  IN  IIO_GLOBALS                       *IioGlobalData,
  IN  UINT8                             Iio,
  IN  UINT8                             PortIndex
  );

BOOLEAN
SetPchDmiLinkSpeed (
  IN  IIO_GLOBALS                      *IioGlobalData,
  IN  UINT8                            LinkSpeed
  );

UINT8
CheckVarPortLinkSpeed (
  IN  IIO_GLOBALS                      *IioGlobalData,
  IN  UINT8                            Iio,
  IN UINT8                             PortIndex
  );


BOOLEAN
ChangeVarPortLinkSpeed (
  IN  IIO_GLOBALS                       *IioGlobalData,
  IN  UINT8                             Iio,
  IN  UINT8                             PortIndex
  );

VOID
SetPcieLinkWidthInit(
    IIO_GLOBALS  *IioGlobalData,
    UINT8        IioIndex,
    UINT8        PortIndex
    );

UINT8
CheckVarPortASPM (
  IN  IIO_GLOBALS                       *IioGlobalData,
  IN  UINT8                             Iio,
  IN  UINT8                             PortIndex
  );

UINT8
GetAssignedPortAddressWidth (
  IN  IIO_GLOBALS                       *IioGlobalData,
  IN  PCI_ROOT_BRIDGE_PCI_ADDRESS       PciAddress
  );

UINT8
GetAssignedPortIndexWidth (
  IN  IIO_GLOBALS                       *IioGlobalData,
  IN  UINT8                             Iio,
  IN  UINT8                             PortIndex
  );

VOID
EnableDisablePCIePort (
  IN  IIO_GLOBALS                        *IioGlobalData,
  IN  UINT8                              IioIndex,
  IN  UINT8                              PortIndex,
  IN  BOOLEAN                            PortEn
  );

UINT8
GetMaxPortPerStack (
  IN  UINT8              Stack
  );

BOOLEAN
IdentifyIioPort (
  IN  IIO_GLOBALS   *IioGlobalData,
  IN  UINT8         IioIndex,
  IN  UINT8         PortIndex
  );

BOOLEAN
IdentifyDmiPort (
  IN  IIO_GLOBALS   *IioGlobalData,
  IN  UINT8         IioIndex,
  IN  UINT8         PortIndex
  );

UINT8
GetNtbPortPerPortIndex (
  IN  UINT8        PortIndex
  );

BOOLEAN
CheckNtbPortConfSetting (
  IN  IIO_GLOBALS        *IioGlobalData,
  IN  UINT8              IioIndex,
  IN  UINT8              PortIndex
  );


BOOLEAN
CheckVMDEnablePerStack(
  IN  IIO_GLOBALS        *IioGlobalData,
  IN  UINT8              IioIndex,
  IN  UINT8              StackIndex
  );

BOOLEAN
CheckFPGAPort (
  IN IIO_GLOBALS   *IioGlobalData,
  IN UINT8         IioIndex,
  IN UINT8         PortIndex
  );
#endif //_IIOVDATA_H_

