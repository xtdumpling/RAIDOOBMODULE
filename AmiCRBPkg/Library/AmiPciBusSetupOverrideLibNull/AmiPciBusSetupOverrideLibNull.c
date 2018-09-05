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
    Contains the functions to override the PCI related setup options so
    that it won't be modified or their registers were not programmed
    again.
*/

//---------------------------------------------------------------------------

#include <Token.h>
#include <PciBus.h>
#include <PciSetup.h>
#include <Protocol/AmiBoardInfo2.h>
#include <Setup.h>
#include <AmiDxeLib.h>

//---------------------------------------------------------------------------

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
//
// Name: AmiPciGetPciDevSetupDataOverrideHook
//
// Description: 
//  Sets PCI_SETUP_DONT_TOUCH for all the common PCI Dynamic Setup options
//
// Input:
//  IN PciDev                 - Pointer to the General Pci related setup data buffer
//  IN DevSdlInfo             - Pointer to the Pci device information
//  IN DevIdx                 - Index of the Pci device
//  IN Set                    - TRUE or FALSE
//
// Output:
//     EFI_STATUS
//       EFI_SUCCESS         - Success will be returned.
//
// Modified:    Nothing.
//
// Referrals:   None.
//
// Notes:       None.
//---------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS 
AmiPciGetPciDevSetupDataOverrideHook (
  IN PCI_DEV_INFO               *PciDevInfo,
  IN PCI_DEVICE_SETUP_DATA      *PciDev,
  IN AMI_SDL_PCI_DEV_INFO       *DevSdlInfo,  //OPTIONAL if == NULL get defaults...
  IN UINTN                      DevIdx,
  IN BOOLEAN                    Set
)
{

    return EFI_SUCCESS;

}

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
//
// Name: AmiPciGetCommonSetupDataOverrideHook
//
// Description: 
//  Sets PCI_SETUP_DONT_TOUCH for all the common PCI Dynamic Setup options
//
// Input:
//  IN PciCommon             - Pointer to the Pci Bus common setup data buffer
//
// Output:
//     EFI_STATUS
//       EFI_SUCCESS         - Success will be returned.
//
// Modified:    Nothing.
//
// Referrals:   None.
//
// Notes:       None.
//---------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS
AmiPciGetCommonSetupDataOverrideHook (
  IN PCI_DEV_INFO               *PciDevInfo,
  IN PCI_COMMON_SETUP_DATA      *PciCommon
)
{
    return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
//
// Name: AmiPciGetPcie1SetupDataOverrideHook
//
// Description: 
//  Sets PCI_SETUP_DONT_TOUCH for all the GEN 1 PCI Dynamic Setup options
//
// Input:
//  IN Pcie1                  - Pointer to the GEN 1 Pci related setup data buffer
//  IN DevSdlInfo             - Pointer to the Pci device information
//  IN DevIdx                 - Index of the Pci device
//  IN Set                    - TRUE or FALSE
//
// Output:
//     EFI_STATUS
//       EFI_SUCCESS        - Success will be returned.
//
// Modified:    Nothing.
//
// Referrals:   None.
//
// Notes:       None.
//---------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS
AmiPciGetPcie1SetupDataOverrideHook (
  IN PCI_DEV_INFO               *PciDevInfo,
  IN PCIE1_SETUP_DATA           *Pcie1,
  IN AMI_SDL_PCI_DEV_INFO       *DevSdlInfo,  //OPTIONAL if == NULL get defaults...
  IN UINTN                      DevIdx,      //OPTIONAL if == 0 
  IN BOOLEAN                    Set
)
{

    return EFI_SUCCESS;

}

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
//
// Name: AmiPciGetPcie2SetupDataOverrideHook
//
// Description: 
//  Sets PCI_SETUP_DONT_TOUCH for all the GEN 2 PCI Dynamic Setup options
//
// Input:
//  IN Pcie2                  - Pointer to the GEN 2 Pci related setup data buffer
//  IN DevSdlInfo             - Pointer to the Pci device information
//  IN DevIdx                 - Index of the Pci device
//  IN Set                    - TRUE or FALSE
//
// Output:
//     EFI_STATUS
//       EFI_SUCCESS        - Success will be returned.
//
// Modified:    Nothing.
//
// Referrals:   None.
//
// Notes:       None.
//---------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS
AmiPciGetPcie2SetupDataOverrideHook (
  IN PCI_DEV_INFO               *PciDevInfo,
  IN PCIE2_SETUP_DATA           *Pcie2,
  IN AMI_SDL_PCI_DEV_INFO       *DevSdlInfo,  //OPTIONAL if == NULL get defaults...
  IN UINTN                      DevIdx,      //OPTIONAL if == 0 
  IN BOOLEAN                    Set
)
{

    return EFI_SUCCESS;

}

#if (PCI_SETUP_USE_APTIO_4_STYLE == 1)
//<AMI_PHDR_START>
//---------------------------------------------------------------------------
//
// Name: AmiPciGetSetupDataOverrideHook
//
// Description: 
//  Sets PCI_SETUP_DONT_TOUCH for all the PCI Static Setup options
//
// Input:
//  IN PciSetupData         - Pointer to the Pci Bus specific setup data buffer
//  IN PciCommon            - Pointer to the Pci Bus common setup data buffer
//  IN HpSetup              - Pointer to the Pci Bus Hot plug related setup data buffer
//
// Output:
//     EFI_STATUS
//       EFI_SUCCESS        - Success will be returned.
//
// Modified:    Nothing.
//
// Referrals:   None.
//
// Notes:       None.
//---------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
AmiPciGetSetupDataOverrideHook (
  IN PCI_DEV_INFO               *PciDevInfo,
  IN PCI_SETUP_DATA             *PciSetupData,
  IN PCI_COMMON_SETUP_DATA      *PciCommon,
  IN PCI_HOTPLUG_SETUP_DATA     *HpSetup
)
{

    return EFI_SUCCESS;

}
#else
//<AMI_PHDR_START>
//---------------------------------------------------------------------------
//
// Name: AmiPciGetSetupDataOverrideHook
//
// Description: 
//  Sets PCI_SETUP_DONT_TOUCH for all the PCI Static Setup options
//
// Input:
//  IN PciCommon              - Pointer to the Pci Bus common setup data buffer
//  IN Pci                    - Pointer to the general PCI related setup data buffer
//  IN Pcie1                  - Pointer to the GEN 1 Pci related setup data buffer
//  IN Pcie2                  - Pointer to the GEN 2 Pci related setup data buffer
//  IN DevSdlInfo             - Pointer to the Pci device information
//  IN DevIdx                 - Index of the Pci device
//
// Output:
//     EFI_STATUS
//       EFI_SUCCESS        - Success will be returned.
//
// Modified:    Nothing.
//
// Referrals:   None.
//
// Notes:       None.
//---------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS
AmiPciGetSetupDataOverrideHook (
  PCI_DEV_INFO              *PciDevInfo,
  PCI_COMMON_SETUP_DATA     *PciCommon,
  PCI_DEVICE_SETUP_DATA     *Pci,
  PCIE1_SETUP_DATA          *Pcie1,
  PCIE2_SETUP_DATA          *Pcie2,
  AMI_SDL_PCI_DEV_INFO      *DevSdlInfo,    //OPTIONAL if ==NULL get defaults...
  UINTN                     DevIdx          //OPTIONAL 
)
{
    return EFI_SUCCESS;
}
#endif
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
