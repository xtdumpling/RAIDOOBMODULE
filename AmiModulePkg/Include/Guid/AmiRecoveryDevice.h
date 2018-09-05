//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2012, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
// $Header: $
//
// $Revision: $
//
// $Date: $
//**********************************************************************
//<AMI_FHDR_START>
//----------------------------------------------------------------------
//
// Name: RecoveryDevice.h - GUID definitions for RecoverySerialMode
//
// Description:	Defines GUIDs that identify AMI's recovery capsules
//
//----------------------------------------------------------------------
//<AMI_FHDR_END>

#ifndef _AMI_RECOVERY_DEVICE_H_
#define _AMI_RECOVERY_DEVICE_H_

//Recovery from mass storage device with FAT file system
#define BLOCK_DEVICE_RECOVERY_CAPSULE_GUID \
	{ 0x0ba8263c, 0xa8bd, 0x4aad, { 0xb4, 0x02, 0x6a, 0x6a, 0xf2, 0xf7, 0xe7, 0x7d } }
    
extern EFI_GUID gBlockDeviceCapsuleGuid;

//Serial port recovery
#define SERIAL_RECOVERY_CAPSULE_GUID \
	{ 0x699add70, 0x8554, 0x4993, { 0x83, 0xf6, 0xd2, 0xcd, 0xc0, 0x81, 0xdd, 0x85 } }
    
extern EFI_GUID gSerialCapsuleGuid;

// OEM Recovery
#define  OEM_RECOVERY_CAPSULE_GUID \
	{ 0x595a6edc, 0x6d2c, 0x474a, { 0x90, 0x82, 0x3b, 0x99, 0x28, 0x51, 0xdf, 0xfe } }
    
extern EFI_GUID gOemCapsuleGuid;

#endif
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2009, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
