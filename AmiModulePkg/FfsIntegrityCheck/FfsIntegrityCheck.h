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
    This file contains MACRO definitions and function prototypes for the
    common functions defined in FfsIntegrityCheck.c
    
*/

#ifndef __FfsIntegrityCheck__H__
#define __FfsIntegrityCheck__H__
#ifdef __cplusplus
extern "C" {
#endif

#include <AmiRomLayout.h>


/// MACRO to convert pointers to 64bit
#define CONVERTTOUINT64(Address)\
    ((UINT64)(UINTN)Address)

/// MACRO to calculate the actual of size that is occupied by the file given the alignment
#define GETOCCUPIEDSIZE(ActualSize, Alignment) \
  ((ActualSize)+(((Alignment)-((ActualSize)&((Alignment)-1)))&((Alignment)-1)))

/// Mask to isolate the non reserved bits from the Size member of the FFS File header
#define FFS_FILE_SIZE_MASK  0x00FFFFFF


EFI_FFS_FILE_STATE
AmiGetFileState(
  IN UINT8                ErasePolarity,
  IN EFI_FFS_FILE_HEADER  *FfsHeader
  );

EFI_STATUS AmiFindNextFile(
        UINT8 ErasePolarity, 
        EFI_FIRMWARE_VOLUME_HEADER *FvStartingAddress, 
        EFI_FFS_FILE_HEADER **FileAddress);

BOOLEAN VerifyFvHeaderChecksum (EFI_FIRMWARE_VOLUME_HEADER *FvHeader);

BOOLEAN FileChecksum(EFI_FFS_FILE_HEADER* FfsHeader);





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
