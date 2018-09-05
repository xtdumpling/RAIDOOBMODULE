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
/** @file
  AmiImageTypeLib instance.
  It implements AmiGetImageType by analyzing image device path.
**/

#include <Protocol/DevicePath.h>
#include <Protocol/FirmwareVolume2.h>
#include <Protocol/PciIo.h>
#include <Protocol/BlockIo.h>
#include <Protocol/BlockIo2.h>
#include <Protocol/LoadFile.h>
#include <Protocol/DxeSmmReadyToLock.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DevicePathLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/AmiImageTypeLib.h>

#define AMI_MEDIA_DEVICE_PATH_GUID \
    { 0x5023b95c, 0xdb26, 0x429b, 0xa6, 0x48, 0xbd, 0x47, 0x66, 0x4c, 0x80, 0x12 }

typedef UINT32 (*DEVICE_PATH_HANDLER)(
    IN EFI_DEVICE_PATH_PROTOCOL *DevicePath, IN EFI_DEVICE_PATH_PROTOCOL *Node
);

typedef struct {
	UINT8 Type;
	UINT8 SubType;
	DEVICE_PATH_HANDLER Handler;
} DEVICE_PATH_DESCRIPTOR;

static EFI_GUID AmiMediaDevicePathGuid = AMI_MEDIA_DEVICE_PATH_GUID;
static UINTN NumberOfFvHandles = 0;
static EFI_HANDLE *FvHandles = NULL;
static BOOLEAN AfterSmmLockEvent = FALSE;

/**
  Locate Protocol associated with the given device path.
  
  The function calls LocateDevicePath to find the closest handle that supports the protocol
  and then calls HandleProtocol to get protocol interface.
  
 @param[in,out]  DevicePath    In input, device path to match. 
                               On output the pointer is updated by the LocateDevicePath with the address
                               of the first unmatched node in the original device path.
 @param[in]      ProtocolGuid  GUID of the protocol to locate
 @param[out]     HandlePtr     Handle on which ProtocolGuid is installed.

 @return         Interface Pointer
**/
VOID* AmiImageTypeLibLocateInterface(
	EFI_DEVICE_PATH_PROTOCOL **DevicePath, EFI_GUID *ProtocolGuid, EFI_HANDLE *HandlePtr
){
	VOID *Interface;
	EFI_HANDLE Handle;
	EFI_DEVICE_PATH_PROTOCOL *TmpDevicePath = *DevicePath;
	
	if (EFI_ERROR(gBS->LocateDevicePath(ProtocolGuid, &TmpDevicePath, &Handle)))
		return NULL;
	if (EFI_ERROR(gBS->HandleProtocol(Handle, ProtocolGuid, &Interface))) 
		return NULL;
	*DevicePath = TmpDevicePath;
	if (HandlePtr != NULL) *HandlePtr = Handle;
	return Interface;
}

/// The function handles FV Device Path
UINT32 AmiImageTypeLibFvDevicePathHandler(
    IN EFI_DEVICE_PATH_PROTOCOL *DevicePath, IN EFI_DEVICE_PATH_PROTOCOL *Node
){
	EFI_HANDLE Handle;
	UINTN i;
	
	// Make sure FV protocol exists.
	if (AmiImageTypeLibLocateInterface(&DevicePath, &gEfiFirmwareVolume2ProtocolGuid, &Handle)==NULL)
		return AMI_IMAGE_UNKNOWN;
	// Make sure we found the right instance of the FV protocol
	Node=NextDevicePathNode(Node);
	if (DevicePath != Node)
		return AMI_IMAGE_UNKNOWN;

	// Make sure the rest of device path looks fine
	// We're expecting FwFile node...
	if (Node->Type != MEDIA_DEVICE_PATH || Node->SubType != MEDIA_PIWG_FW_FILE_DP)
		return AMI_IMAGE_UNKNOWN;
	// ...followed by the terminating node
	if (!IsDevicePathEnd(NextDevicePathNode(Node)))
		return AMI_IMAGE_UNKNOWN;
	
	// If we are past our trust boundary, make sure we are dealing with the known FV.
	if (!AfterSmmLockEvent) return AMI_IMAGE_FROM_FV;
	// FvHandles array is initialized in the DxeSmmReadyToLock callback (AmiImageTypeLibSmmReadyToLockCallBack).
    for(i=0;i < NumberOfFvHandles; i++){
        if( Handle == FvHandles[i] ) return AMI_IMAGE_FROM_FV;
    }
    return AMI_IMAGE_UNKNOWN;
}

/// The function handles Relative Offset Device Path (OpROM images)
UINT32 AmiImageTypeLibRelativeOffsetDevicePathHandler(
    IN EFI_DEVICE_PATH_PROTOCOL *DevicePath,
    IN EFI_DEVICE_PATH_PROTOCOL *Node
){
	EFI_PCI_IO_PROTOCOL *PciIo;
	UINT64 Attributes;
	
	PciIo = AmiImageTypeLibLocateInterface(&DevicePath, &gEfiPciIoProtocolGuid, NULL);
	// Make sure it's a PCI device
	if ( PciIo==NULL ) 
		return AMI_IMAGE_UNKNOWN;
	// Make sure we found the right PCI I/O instance
	if (DevicePath != Node)
		return AMI_IMAGE_UNKNOWN;
	// Make sure relative offset node is the last node in the device path.
	if (!IsDevicePathEnd(NextDevicePathNode(Node)))
		return AMI_IMAGE_UNKNOWN;
	
	// Check if this is device with an embedded OpROM. Embedded OpROM are loaded from one of the firmware FV.
    if (EFI_ERROR(PciIo->Attributes(PciIo, EfiPciIoAttributeOperationGet, 0, &Attributes)))
    	return AMI_IMAGE_UNKNOWN;

    return ( Attributes & EFI_PCI_IO_ATTRIBUTE_EMBEDDED_ROM ) ? AMI_IMAGE_FROM_FV : AMI_IMAGE_FROM_OPTION_ROM;
}

/// The function handles Memory Mapped Device Path
UINT32 AmiImageTypeLibMemMapDevicePathHandler(
    IN EFI_DEVICE_PATH_PROTOCOL *DevicePath, IN EFI_DEVICE_PATH_PROTOCOL *Node
){
	UINT32 ImageType;
	// We handle two breeds of memory-mapped device paths:
	// 1. Device path installed by PciBus driver for embedded OpROM images.
	// 2. Device path installed on handles of FV that do not expose FV name GUID via extended FV header.
	ImageType = AmiImageTypeLibRelativeOffsetDevicePathHandler(DevicePath,Node);
	if (ImageType != AMI_IMAGE_UNKNOWN) return ImageType;
	return AmiImageTypeLibFvDevicePathHandler(DevicePath,Node);
}

/// The function handles File Device Path
UINT32 AmiImageTypeLibFileDevicePathHandler(
    IN EFI_DEVICE_PATH_PROTOCOL *DevicePath,
    IN EFI_DEVICE_PATH_PROTOCOL *Node
){
	EFI_BLOCK_IO_PROTOCOL *BlockIo;
	EFI_BLOCK_IO2_PROTOCOL *BlockIo2;

	// Use BlockIo or BlockIo2 instance to detect the media type.
	// If we are dealing with file system without an associated Block I/O device, return AMI_IMAGE_UNKNOWN.
	BlockIo = AmiImageTypeLibLocateInterface(&DevicePath, &gEfiBlockIoProtocolGuid, NULL);
	if ( BlockIo != NULL ){
		return (BlockIo->Media->RemovableMedia) ? AMI_IMAGE_FROM_REMOVABLE_MEDIA : AMI_IMAGE_FROM_FIXED_MEDIA;
	}
	BlockIo2 = AmiImageTypeLibLocateInterface(&DevicePath, &gEfiBlockIo2ProtocolGuid, NULL);
	if ( BlockIo2 != NULL ){
		return (BlockIo2->Media->RemovableMedia) ? AMI_IMAGE_FROM_REMOVABLE_MEDIA : AMI_IMAGE_FROM_FIXED_MEDIA;
	}
	
	return AMI_IMAGE_UNKNOWN;
}

/// The function handles embedded applications such ash Shell loaded via FwLoadFileInterface published by BDS
UINT32 AmiImageTypeLibEmbeddedApplicationHandler(
    IN EFI_DEVICE_PATH_PROTOCOL *DevicePath, IN EFI_DEVICE_PATH_PROTOCOL *Node
){
	// Make sure we are dealing with the vendor device path installed by BDS
	if (   Node!=DevicePath // Vendor DP Node must the the first node
		|| DevicePathNodeLength(Node) != sizeof(VENDOR_DEVICE_PATH)
		|| !CompareGuid((EFI_GUID*)(Node+1), &AmiMediaDevicePathGuid)
	) return AMI_IMAGE_UNKNOWN;
	// Make sure in instance of LoadFile exists
	if (AmiImageTypeLibLocateInterface(&DevicePath, &gEfiLoadFileProtocolGuid, NULL)==NULL)
		return AMI_IMAGE_UNKNOWN;
	// Make sure we found the right LoadFile instance
	Node=NextDevicePathNode(Node);
	if (DevicePath != Node)
		return AMI_IMAGE_UNKNOWN;
	
	// Vendor DP may be optionally followed by a FwFile node...
	if (Node->Type == MEDIA_DEVICE_PATH && Node->SubType == MEDIA_PIWG_FW_FILE_DP)
		Node=NextDevicePathNode(Node);
	// ...followed by the terminating node
	if (!IsDevicePathEnd(Node))
		return AMI_IMAGE_UNKNOWN;
	
	// At this point we know that we are dealing with the embedded application.
	// Return image type based on the platform policy set forth by the AmiPcdLoadUnsignedEmbeddedApplications PCD.
	return (PcdGetBool(AmiPcdLoadUnsignedEmbeddedApplications)) ? AMI_IMAGE_FROM_FV : AMI_IMAGE_FROM_FIXED_MEDIA;
}

/// The function handles MAC Address Device Path
UINT32 AmiImageTypeLibMacDevicePathHandler(
    IN EFI_DEVICE_PATH_PROTOCOL *DevicePath,
    IN EFI_DEVICE_PATH_PROTOCOL *Node
){
	return AMI_IMAGE_FROM_REMOVABLE_MEDIA;
}

DEVICE_PATH_DESCRIPTOR DevicePathHandler[] = {
    { MEDIA_DEVICE_PATH, MEDIA_PIWG_FW_VOL_DP, AmiImageTypeLibFvDevicePathHandler}, // FV
    { HARDWARE_DEVICE_PATH, HW_MEMMAP_DP, AmiImageTypeLibMemMapDevicePathHandler}, // FV or Embedded OpROM
    { MEDIA_DEVICE_PATH, MEDIA_RELATIVE_OFFSET_RANGE_DP, AmiImageTypeLibRelativeOffsetDevicePathHandler}, // PCI    
    { MEDIA_DEVICE_PATH, MEDIA_FILEPATH_DP, AmiImageTypeLibFileDevicePathHandler}, // File
    { MEDIA_DEVICE_PATH, MEDIA_VENDOR_DP, AmiImageTypeLibEmbeddedApplicationHandler}, // Shell
    { MESSAGING_DEVICE_PATH, MSG_MAC_ADDR_DP, AmiImageTypeLibMacDevicePathHandler}, // Network
    {0,0,NULL}
};

/**
  The function returns image type.
  
  @param[in]  File         A device path the image was loaded from. 
                           This is a device path that was used to call gBS->LoadImage().
  @param[in]  FileBuffer   A pointer to the PE buffer of the image
  @param[in]  FileSize     The size of FileBuffer.
  @param[in]  BootPolicy   A boot policy that was used to call gBS->LoadImage().

  @return Image Type. See @ref AmiImageTypeLibImageTypes "Image Types" for a list of supported values.
**/
UINT32 AmiGetImageType (
	IN CONST EFI_DEVICE_PATH_PROTOCOL   *File,
	IN VOID *FileBuffer, IN UINTN FileSize, IN BOOLEAN BootPolicy
){
	DEVICE_PATH_DESCRIPTOR *Descriptor;
	EFI_DEVICE_PATH_PROTOCOL *Node;
	EFI_DEVICE_PATH_PROTOCOL *DevicePath = (EFI_DEVICE_PATH_PROTOCOL *)File;
	
	if ( DevicePath==NULL ) return AMI_IMAGE_UNKNOWN;

    for( Node = DevicePath; !IsDevicePathEnd(Node); Node=NextDevicePathNode(Node) ){
    	for(Descriptor = DevicePathHandler; Descriptor->Handler!=0; Descriptor++){
    		if (Node->Type != Descriptor->Type || Node->SubType != Descriptor->SubType) continue;
    		return Descriptor->Handler(DevicePath,Node);
    	}
    }
	
	return AMI_IMAGE_UNKNOWN;
}

/// DxeSmmReadyToLock protocol callback
VOID EFIAPI AmiImageTypeLibSmmReadyToLockCallBack(IN EFI_EVENT Event, IN VOID *Context){
	EFI_STATUS Status;
	
	if (AfterSmmLockEvent) return;
	AfterSmmLockEvent = TRUE;
	gBS->CloseEvent(Event);
    
    Status = gBS->LocateHandleBuffer (
        ByProtocol, &gEfiFirmwareVolume2ProtocolGuid,
        NULL,  &NumberOfFvHandles, &FvHandles
    );

    if(EFI_ERROR(Status)){
    	FvHandles = NULL;
    	NumberOfFvHandles = 0;
    }
}

/**
  Library constructor. Called in Driver's entry point.
  @param ImageHandle     Image handle.
  @param SystemTable    Pointer to the EFI system table.

  @retval: EFI_SUCCESS  
**/
EFI_STATUS EFIAPI AmiImageTypeLibConstructor(
    IN  EFI_HANDLE ImageHandle, IN  EFI_SYSTEM_TABLE *SystemTable
){
    EFI_STATUS Status;
    EFI_EVENT Event;
    VOID *Registration;
    
    Status = gBS->CreateEvent(
        EVT_NOTIFY_SIGNAL, TPL_CALLBACK, AmiImageTypeLibSmmReadyToLockCallBack,
        NULL, &Event
    );
    ASSERT_EFI_ERROR (Status);
    Status = gBS->RegisterProtocolNotify(&gEfiDxeSmmReadyToLockProtocolGuid, Event, &Registration);
    ASSERT_EFI_ERROR (Status);
    return EFI_SUCCESS;
}

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
