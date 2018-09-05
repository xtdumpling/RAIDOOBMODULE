//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
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
  Internal header of the Setup Component
*/

#ifndef __AMI_PCI_COMPATIBILITY_PROTOCOL__H__
#define __AMI_PCI_COMPATIBILITY_PROTOCOL__H__
#ifdef __cplusplus
extern "C" {
#endif

#include <Efi.h>
#include <PciPort.h>


#define AMI_PCI_PORT_COMPATIBILITY_PROTOCOL_GUID \
		{ 0xea4b0675, 0x1f36, 0x4abe, 0xbb, 0x3a, 0x6d, 0x60, 0x76, 0xa, 0x2, 0xa2 }
 
extern EFI_GUID gAmiPciPortCompatibilityProtocolGuid;

typedef EFI_STATUS (EFIAPI *OEM_PCI_PROGRAM_DEVICE_FUNCTION_PTR)(VOID *PciIoProtocol);
typedef EFI_STATUS (EFIAPI *OEM_PCI_DEVICE_ATTRIBUTE_FUNCTION_PTR)(VOID *PciIoProtocol, UINT64 *Attr, UINT64 Capab, BOOLEAN Set);
typedef EFI_STATUS (EFIAPI *OEM_PCI_DEVICE_SET_ASPM_FUNCTION_PTR)(VOID *PciIoProtocol, VOID *AspmMode);
typedef EFI_STATUS (EFIAPI *OEM_PCI_DEVICE_SET_LNK_SPEED_FUNCTION_PTR)(VOID *PciIoProtocol, UINT8 *LnkSpeed, UINT8 SuppSpeeds);
typedef EFI_STATUS (EFIAPI *OEM_PCI_DEVICE_GET_OPT_ROM_FUNCTION_PTR)(VOID *PciIoProtocol, VOID **OptRom, UINT64 *OptRomSize);
typedef EFI_STATUS (EFIAPI *OEM_PCI_DEVICE_OUT_OF_RESOURCES_FUNCTION_PTR)(VOID *PciIoProtocol, UINTN Count, UINTN ResType);
//typedef EFI_STATUS (EFIAPI *OEM_PCI_PADDING_FUNCTION_PTR)(VOID *HpcData, VOID *HpcLocationData, UINT64 PciAddress);

typedef enum {
	lpAptioV=0,
	lpAptio4x,				//1
	lpMaxPriority			//2
} LAUNCH_PRIORITY;

typedef struct _AMI_PCI_PORT_COMPATIBILITY_PROTOCOL {
	OEM_PCI_PROGRAM_DEVICE_FUNCTION_PTR			PciPortSkipThisDevice;
	OEM_PCI_PROGRAM_DEVICE_FUNCTION_PTR			PciPortOemProgDevice;
	OEM_PCI_PROGRAM_DEVICE_FUNCTION_PTR			PciPortCheckBadPcieDevice;
	OEM_PCI_DEVICE_ATTRIBUTE_FUNCTION_PTR		PciPortOemAttributes; //
	OEM_PCI_DEVICE_SET_ASPM_FUNCTION_PTR		PciPortOemSetAspm;
	OEM_PCI_DEVICE_SET_LNK_SPEED_FUNCTION_PTR	PciPortOemSetLnkSpeed;
	OEM_PCI_DEVICE_GET_OPT_ROM_FUNCTION_PTR		PciPortOemGetOptRom;
	OEM_PCI_DEVICE_OUT_OF_RESOURCES_FUNCTION_PTR PciPortOutOfResourcesRemoveDevice;
    OEM_PCI_PROGRAM_DEVICE_FUNCTION_PTR         PciPortQueryPcieDevice;

//	OEM_PCI_PADDING_FUNCTION_PTR				PciPortSetOemPadding;
} AMI_PCI_PORT_COMPATIBILITY_PROTOCOL;





/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
