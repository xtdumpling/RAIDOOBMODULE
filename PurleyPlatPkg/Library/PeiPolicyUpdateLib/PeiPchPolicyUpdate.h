/** @file

@copyright
 Copyright (c) 2009 - 2016 Intel Corporation. All rights reserved
 This software and associated documentation (if any) is furnished
 under a license and may only be used or copied in accordance
 with the terms of the license. Except as permitted by the
 license, no part of this software or documentation may be
 reproduced, stored in a retrieval system, or transmitted in any
 form or by any means without the express written consent of
 Intel Corporation.
 This file contains a 'Sample Driver' and is licensed as such
 under the terms of your license agreement with Intel or your
 vendor. This file may be modified by the user, subject to
 the additional terms of the license agreement.

@par Specification Reference:
**/
#ifndef _PEI_PCH_POLICY_UPDATE_H_
#define _PEI_PCH_POLICY_UPDATE_H_

//
// External include files do NOT need to be explicitly specified in real EDKII
// environment
//
#include <PiPei.h>

#include <Guid/PlatformInfo.h>
#include "Guid/SetupVariable.h"
#include <Guid/PchRcVariable.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Library/PeiServicesLib.h>
#if AMT_SUPPORT
#include <MeBiosExtensionSetup.h>
#endif // AMT_SUPPORT
#include <PchAccess.h>
#include <Ppi/PchPolicy.h>
#include <Library/MmPciBaseLib.h>

VOID
UpdatePchUsbConfig (
  IN PCH_USB_CONFIG            *PchUsbConfig,
  IN SYSTEM_CONFIGURATION      *SetupVariables,
  IN PCH_RC_CONFIGURATION      *PchRcVariables,
  IN VOID                      *Usb20OverCurrentMappings,
  IN VOID                      *Usb30OverCurrentMappings,
  IN VOID                      *Usb20AfeParams
  )
/*++

Routine Description:

  This function performs PCH USB Platform Policy initialzation

Arguments:
  UsbConfig                          Pointer to PCH_USB_CONFIG data buffer
  SetupVariables                   Pointer to Setup variable
  PchRcVariables                   Pointer to Pch Rc Variables
  Usb20OverCurrentMappings Pointer to Overcurrent mapping table for USB 20 ports
  Usb30OverCurrentMappings Pointer to Overcurrent mapping table for USB 30 ports

Returns:

--*/
;

#endif
