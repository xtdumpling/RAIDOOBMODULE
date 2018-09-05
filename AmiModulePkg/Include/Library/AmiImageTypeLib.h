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
  Header file that defines AmiImageTypeLib interface.

  AmiImageTypeLib is a policy library used by SecurityStub driver,
  which calls AmiGetImageType to identify type of the device the image originated from.
  Image type returned by AmiImageTypeLib is used to apply appropriate image authentication policy.
  The library is used by the SecurityStub dirver indirectly, 
  via the security handler installed by the SecureBoot module.
**/

#ifndef __AMI_IMAGE_TYPE_LIB__H__
#define __AMI_IMAGE_TYPE_LIB__H__

/// @name Image Types
/// @anchor AmiImageTypeLibImageTypes
/// The image type macros define values returned by the AmiGetImageType function.
/// Each value defines image type in terms of the device(or a class of devices) the image was loaded from.
///@{

/// Image origin is unknown
#define AMI_IMAGE_UNKNOWN               0
/// Image was loaded from FV
#define AMI_IMAGE_FROM_FV               1
/// Image was loaded from PCI OpROM
#define AMI_IMAGE_FROM_OPTION_ROM       2
/// Image was loaded from removable media
#define AMI_IMAGE_FROM_REMOVABLE_MEDIA	3
/// Image was loaded from fixed media
#define AMI_IMAGE_FROM_FIXED_MEDIA      4

///@}

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
);

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
