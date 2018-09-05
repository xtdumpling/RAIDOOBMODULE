/** @file

@copyright
 Copyright (c) 2012 - 2016 Intel Corporation. All rights reserved
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

//
// EDK and EDKII have different GUID formats
//
#include <Uefi/UefiBaseType.h>
#include <Ppi/PchPolicy.h>
#include <PlatformInfo.h>
#include "PlatformHost.h"
#include <SetupVariable.h>
#include <Guid/PchRcVariable.h>
#include <Library/PchInfoLib.h>


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
  PchUsbConfig                    Pointer to PCH_USB_CONFIG data buffer
  SetupVariables                  Pointer to Setup variable
  PlatformType                    PlatformType specified
  PlatformFlavor                  PlatformFlavor specified
  BoardType                       BoardType specified

Returns:

--*/
{
  UINTN  PortIndex;
#ifdef TESTMENU_FLAG
  UINT8  Index;
#endif

  PchUsbConfig->UsbPrecondition = PchRcVariables->UsbPrecondition;

    for (PortIndex = 0; PortIndex < GetPchXhciMaxUsb2PortNum (); PortIndex++) {
      if (PchRcVariables->PchUsbHsPort[PortIndex] == 1) {
      PchUsbConfig->PortUsb20[PortIndex].Enable = TRUE;
    } else {
        PchUsbConfig->PortUsb20[PortIndex].Enable = FALSE;
      }
    }
    for (PortIndex = 0; PortIndex < GetPchXhciMaxUsb3PortNum (); PortIndex++) {
      if (PchRcVariables->PchUsbSsPort[PortIndex] == 1) {
      PchUsbConfig->PortUsb30[PortIndex].Enable = TRUE;
    } else {
        PchUsbConfig->PortUsb30[PortIndex].Enable = FALSE;
    }
  }

  for (PortIndex = 0; PortIndex < PCH_H_XHCI_MAX_USB2_PHYSICAL_PORTS; PortIndex++) {
    PchUsbConfig->PortUsb20[PortIndex].Afe.Petxiset  = (UINT8)(((PCH_USB20_AFE *)Usb20AfeParams)[PortIndex].Petxiset);
    PchUsbConfig->PortUsb20[PortIndex].Afe.Txiset    = (UINT8)(((PCH_USB20_AFE *)Usb20AfeParams)[PortIndex].Txiset);
    PchUsbConfig->PortUsb20[PortIndex].Afe.Predeemp  = (UINT8)(((PCH_USB20_AFE *)Usb20AfeParams)[PortIndex].Predeemp);
   	PchUsbConfig->PortUsb20[PortIndex].Afe.Pehalfbit = (UINT8)(((PCH_USB20_AFE *)Usb20AfeParams)[PortIndex].Pehalfbit);
  }

  //
  // xDCI (USB device) related settings from setup variable
  //
  if(PchRcVariables->PchXdciSupport == 1) {
    PchUsbConfig->XdciConfig.Enable= TRUE;
  } else {
    PchUsbConfig->XdciConfig.Enable= FALSE;
  }

#ifdef TESTMENU_FLAG
  //
  // Need to clear UsbEPTypeLockPolicy[8] and UsbEPTypeLockPolicy[16] since this is not assign in the setup
  //
  PchRcVariables->UsbEPTypeLockPolicy[8] = 0;
  SetupPchRcVariablesVariables->UsbEPTypeLockPolicy[16] = 0;

  //
  // Update USB EP Type Lock Policy Settings
  //
  for (Index = 0; Index < 24; Index++) {
     PchUsbConfig->Usb30EpTypeLockPolicySettings.EPTypeLockPolicy |= (UINT32) (PchRcVariables->UsbEPTypeLockPolicy[Index] << Index);
  }

  for (Index = 0; Index < 16; Index++) {
     PchUsbConfig->Usb30EpTypeLockPolicySettings.EPTypeLockPolicyPortControl1  |= (UINT32) (PchRcVariables->RootPortPolicyControl[Index] << Index * 2);
  }

  for (Index = 16; Index < 20; Index++) {
     PchUsbConfig->Usb30EpTypeLockPolicySettings.EPTypeLockPolicyPortControl2  |= (UINT32) (PchRcVariables->RootPortPolicyControl[Index] << (Index - 16) * 2);
  }

  //
  // Remark: Can be disabled only for debugging process!!!
  //
  PchUsbConfig->TstMnuControllerEnabled  = PchRcVariables->XhciEnabled;
  //
  // SSIC debug mode
  //
  PchUsbConfig->TstMnuSsicHalt  = PchRcVariables->XhciSsicHalt;
#endif
  //
  // XHCI USB Over Current Pins disabled, update it based on setup option.
  //
  PchUsbConfig->XhciOcMapEnabled = PchRcVariables->XhciOcMapEnabled;

  //
  // XHCI Wake On USB configured based on user input through setup option
  //
  PchUsbConfig->XhciWakeOnUsb = SetupVariables->XhciWakeOnUsbEnabled;
  //
  // XHCI option to disable MSIs
  //
  PchUsbConfig->XhciDisMSICapability = PchRcVariables->XhciDisMSICapability;

  //
  // Platform Board programming per the layout of each port.
  //
  // OC Map for USB2 Ports
  for (PortIndex=0;PortIndex<PCH_MAX_USB2_PORTS;PortIndex++) {
    PchUsbConfig->PortUsb20[PortIndex].OverCurrentPin = (UINT8)((PCH_USB_OVERCURRENT_PIN *)Usb20OverCurrentMappings)[PortIndex];
  }

  // OC Map for USB3 Ports
  for (PortIndex=0;PortIndex<PCH_MAX_USB3_PORTS;PortIndex++) {
    PchUsbConfig->PortUsb30[PortIndex].OverCurrentPin = (UINT8)((PCH_USB_OVERCURRENT_PIN *)Usb30OverCurrentMappings)[PortIndex];
  }

}
