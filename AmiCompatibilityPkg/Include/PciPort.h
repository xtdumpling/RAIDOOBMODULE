//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2013, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

//**********************************************************************
// $Header: $
//
// $Revision:  $
//
// $Date:  $
//**********************************************************************

#ifndef PCI_PORT_H_
#define PCI_PORT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <Efi.h>


//Define OEM_PCI_DEVICE_PROGRAM_CALLBACK
typedef EFI_STATUS (OEM_PCI_PROGRAM_DEVICE_FUNCTION)(VOID *PciIoProtocol);
typedef EFI_STATUS (OEM_PCI_DEVICE_ATTRIBUTE_FUNCTION)(VOID *PciIoProtocol, UINT64 *Attr, UINT64 Capab, BOOLEAN Set);
typedef EFI_STATUS (OEM_PCI_DEVICE_SET_ASPM_FUNCTION)(VOID *PciIoProtocol, VOID *AspmMode);
typedef EFI_STATUS (OEM_PCI_DEVICE_SET_LNK_SPEED_FUNCTION)(VOID *PciIoProtocol, UINT8 *LnkSpeed, UINT8 SuppSpeeds);
typedef EFI_STATUS (OEM_PCI_DEVICE_GET_OPT_ROM_FUNCTION)(VOID *PciIoProtocol, VOID **OptRom, UINT64 *OptRomSize);
typedef EFI_STATUS (OEM_PCI_DEVICE_OUT_OF_RESOURCES_FUNCTION)(VOID *PciIoProtocol, UINTN Count, UINTN ResType);
//typedef EFI_STATUS (OEM_PCI_PADDING_FUNCTION)(VOID *HpcData, VOID *HpcLocationData, UINT64 PciAddress);

//Data structure of CallBack delivered trough eLink
typedef struct{
	UINT16  VendorId;
    UINT16  DeviceId;
	VOID    *Callback;
} OEM_PCI_DEVICE_PROGRAM_CALLBACK;



//PCI Bus Porting Function Prototypes
/*
EFI_STATUS PciPortSkipThisDevice(VOID *Device);

EFI_STATUS PciPortCheckBadPcieDevice(VOID *Device);

EFI_STATUS PciPortOemProgDevice(VOID *Device);

EFI_STATUS PciPortOemAttributes(VOID *Device, UINT64 *Attr, UINT64 Capab, BOOLEAN Set);

EFI_STATUS PciPortOemSetAspm(PCI_DEV_INFO *Device, VOID *AspmMode);

EFI_STATUS PciPortOemSetLnkSpeed(PCI_DEV_INFO *Device, UINT8 *LnkSpeed, UINT8 SuppSpeeds);

EFI_STATUS PciPortSetOemPadding(VOID *HpcData, VOID *HpcLocationData, UINT64 PciAddress);

EFI_STATUS PciPortOemGetOptRom(PCI_DEV_INFO *Device, VOID **OptRom, UINT64 *OptRomSize);

EFI_STATUS PciPortOutOfResourcesRemoveDevice(PCI_DEV_INFO *Device, UINTN Count, UINTN LowResType);
*/

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif	//PCI_BOARD_H_


//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2013, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

