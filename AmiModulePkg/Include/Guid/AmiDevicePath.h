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
// Name: AmiDevicePath.h
//
// Description:	Defines AMI specific GUIDed device path nodes
//
//----------------------------------------------------------------------
//<AMI_FHDR_END>

#ifndef __AMI_DEVICE_PATH_H_
#define __AMI_DEVICE_PATH_H_

#include <Protocol/DevicePath.h>
    
// {BA7C46D1-9C5E-4fc8-943D-1A491F23FE01}
#define AMI_ISO9660_MEDIA_GUID \
    { 0xba7c46d1, 0x9c5e, 0x4fc8, 0x94, 0x3d, 0x1a, 0x49, 0x1f, 0x23, 0xfe, 0x1 }

extern EFI_GUID gAmiIso9660MediaGuid;

// {7465BA75-88A7-4b61-9A7E-6D4EDF6804DC}
#define AMI_UDF_MEDIA_GUID \
    { 0x7465ba75, 0x88a7, 0x4b61, 0x9a, 0x7e, 0x6d, 0x4e, 0xdf, 0x68, 0x4, 0xdc }

extern EFI_GUID gAmiUdfMediaGuid;

#define VOLUME_LABEL_SIZE 32
#define ROOT_DIRECTORY_RECORD_SIZE 34
#define ROOT_DIRECTORY_RECORD_OFFSET 156
#define VOLUME_LABEL_OFFSET 40

#define UDF_ANCHOR_LBA 256
#define UDF_ANCHOR_EXTENT_OFFSET 16
#define UDF_ANCHOR_EXTENT_SIZE 16
#define UDF_ANCHOR_DESCRIPTOR_LBA 256
#define UDF_ANCHOR_DESCRIPTOR_TAG_ID 2

#pragma pack(1)
typedef struct
{
    VENDOR_DEVICE_PATH Dp;
    UINT64             VolumeSize;
    UINT8	           Root[ROOT_DIRECTORY_RECORD_SIZE];
    CHAR8              VolumeLabel[VOLUME_LABEL_SIZE];
} AMI_ISO9660_DEVICE_PATH;

typedef struct {
    VENDOR_DEVICE_PATH Dp;
    UINT8        	   Extent[UDF_ANCHOR_EXTENT_SIZE];  //EXTENT_DESC Main & Reserved
} AMI_UDF_DEVICE_PATH;
#pragma pack()

#endif
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
