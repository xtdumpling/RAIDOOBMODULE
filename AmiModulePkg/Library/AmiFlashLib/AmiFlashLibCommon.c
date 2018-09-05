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
  Common AmiFlashlibrary functions used by both the DXE and Runtime drivers
**/

#include <Token.h>
#include <Flash.h>

/// Global variable used to store the address of the start of the BIOS region in the flash part
UINTN gAmiFlashDeviceBase = (0xFFFFFFFF - FLASH_SIZE + 1);

/**
  Function to enable writes to the flash part.
**/
VOID AmiFlashDeviceWriteEnable(VOID)
{
    FlashDeviceWriteEnable();
}

/**
  Function to disable writes to the flash part.
**/
VOID AmiFlashDeviceWriteDisable(VOID)
{
    FlashDeviceWriteDisable();
}

/**
  Write the data into the block pointed to by BlockAddress

  @param BlockAddress Pointer to the flash part area to write the data into
  @param Data Pointer to the data buffer to write into provided the BlockAddress

  @return BOOLEAN
  @retval TRUE The flash region was updated
  @retval FALSE The flash region could not be updated
**/
BOOLEAN AmiFlashWriteBlock(VOID* BlockAddress, VOID *Data)
{
    return FlashWriteBlock( (UINT8*)BlockAddress, (UINT8*)Data);
}

/**
  Erase the block pointed to by the passed BlockAddress.

  @param BlockAddress Pointer to an address contained in the block.

  @return BOOLEAN
  @retval TRUE Erase completed successfully
  @retval FALSE Erase did not complete successfully
**/
BOOLEAN AmiFlashEraseBlock(VOID* BlockAddress)
{
    return FlashEraseBlock((UINT8*)BlockAddress);
}

/**
  Enable writing to the block that contains the BlockAddress

  @param BlockAddress Pointer to an address contained in the block.
**/
VOID AmiFlashBlockWriteEnable(VOID* BlockAddress)
{
    FlashBlockWriteEnable((UINT8*)BlockAddress);
}

/**
  Disable writing to the block that contains the BlockAddress.

  @param BlockAddress Pointer to an address contained in the block.
**/
VOID AmiFlashBlockWriteDisable(VOID* BlockAddress)
{
    FlashBlockWriteDisable((UINT8*)BlockAddress);
}

/**
  Program Length bytes of data from Data into the Address.

  @param Address Pointer to the address to write the data
  @param Data The data to write into the address
  @param Length The number of bytes that need to be written

  @return BOOLEAN
  @retval TRUE Programming the data was successful
  @retval FALSE Programming the data was not successful 
**/
BOOLEAN AmiFlashProgram(VOID* Address, VOID *Data, UINT32 Length)
{
    return FlashProgram((UINT8*)Address, (UINT8*)Data, Length);

}

/**
  Read Length bytes from the Address and put them into the Data buffer

  @param Address Pointer to an address to start reading
  @param Data Pointer to a buffer to place the data into
  @param Length The number of bytes to read and place into the buffer

  @return BOOLEAN
  @retval TRUE Erase completed successfully
  @retval FALSE Erase did not complete successfully
**/
BOOLEAN  AmiFlashRead(VOID* Address, VOID *Data, UINT32 Length)
{
    return FlashRead((UINT8*)Address, (UINT8*)Data, Length);
}

/**
  Converts a physical address to the starting address of the block
**/
VOID *AmiFlashBlockStartAddress(VOID *Address)
{
    return (VOID*)( (UINTN)Address - (((UINTN)Address - gAmiFlashDeviceBase)%FLASH_BLOCK_SIZE) );
}

/**
  Performs flash device detection and prepares the library to work with the detected flash device.

  NOTE: There is no requirement of calling this function prior to using other library functions.
  This function can be used to force re-initialization of the library (and re-detection of the flash device).
  The re-initialization might be useful when system switches between multiple flash devices at runtime.
  
  @return BOOLEAN
  @retval TRUE Erase completed successfully
  @retval FALSE Erase did not complete successfully
**/
BOOLEAN AmiFlashInit(){
    return !EFI_ERROR(FlashInit((UINT8*)gAmiFlashDeviceBase));
}

/**
 * Library constructor for the DXE AmiFlashLib instance.
 * 
 * @param ImageHandle The ImageHandle of the driver consuming the AmiFlashLib
 * @param SystemTable Pointer to the EFI System Table
 * 
 * @return EFI_SUCCESS The library constructor completed execution
 */
EFI_STATUS EFIAPI DxeAmiFlashLibConstructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
){
	return FlashInit((UINT8*)gAmiFlashDeviceBase);
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
