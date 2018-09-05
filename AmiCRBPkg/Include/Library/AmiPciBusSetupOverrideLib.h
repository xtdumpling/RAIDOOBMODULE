//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file
    Contains the function declarations for PCI Express override function.
*/

#ifndef __AmiPciBusSetupOverrideLib__H__
#define __AmiPciBusSetupOverrideLib__H__

//---------------------------------------------------------------------------
#include <Token.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <PciBus.h>
#include <PciSetup.h>
#include <Protocol/AmiBoardInfo2.h>
#include <Setup.h>
#include <AmiDxeLib.h>

//---------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

#if (PCI_SETUP_USE_APTIO_4_STYLE == 1)
EFI_STATUS
AmiPciGetSetupDataOverrideHook (
  IN PCI_DEV_INFO               *PciDevInfo,
  IN PCI_SETUP_DATA             *PciSetupData,
  IN PCI_COMMON_SETUP_DATA      *PciCommon,
  IN PCI_HOTPLUG_SETUP_DATA     *HpSetup
);

#else
EFI_STATUS
AmiPciGetSetupDataOverrideHook (
  PCI_DEV_INFO              *PciDevInfo,
  PCI_COMMON_SETUP_DATA     *PciCommon,
  PCI_DEVICE_SETUP_DATA     *Pci,
  PCIE1_SETUP_DATA          *Pcie1,
  PCIE2_SETUP_DATA          *Pcie2,
  AMI_SDL_PCI_DEV_INFO      *DevSdlInfo,    //OPTIONAL if ==NULL get defaults...
  UINTN                     DevIdx          //OPTIONAL 
);
#endif

EFI_STATUS
AmiPciGetCommonSetupDataOverrideHook (
  IN PCI_DEV_INFO               *PciDevInfo,
  IN PCI_COMMON_SETUP_DATA      *PciCommon
);

EFI_STATUS 
AmiPciGetPciDevSetupDataOverrideHook (
  IN PCI_DEV_INFO               *PciDevInfo,
  IN PCI_DEVICE_SETUP_DATA      *PciDev,
  IN AMI_SDL_PCI_DEV_INFO       *DevSdlInfo,  //OPTIONAL if == NULL get defaults...
  IN UINTN                      DevIdx,
  IN BOOLEAN                    Set
);

EFI_STATUS
AmiPciGetPcie1SetupDataOverrideHook (
  IN PCI_DEV_INFO               *PciDevInfo,
  IN PCIE1_SETUP_DATA           *Pcie1,
  IN AMI_SDL_PCI_DEV_INFO       *DevSdlInfo,  //OPTIONAL if == NULL get defaults...
  IN UINTN                      DevIdx,		 //OPTIONAL if == 0 
  IN BOOLEAN                    Set
);

EFI_STATUS
AmiPciGetPcie2SetupDataOverrideHook (
  IN PCI_DEV_INFO               *PciDevInfo,
  IN PCIE2_SETUP_DATA           *Pcie2,
  IN AMI_SDL_PCI_DEV_INFO       *DevSdlInfo,  //OPTIONAL if == NULL get defaults...
  IN UINTN                      DevIdx,		 //OPTIONAL if == 0 
  IN BOOLEAN                    Set
);

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
