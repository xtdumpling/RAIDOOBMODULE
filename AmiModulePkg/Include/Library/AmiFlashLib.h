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
  Header file that defines the Flash Library interfaces.
**/

#ifndef __AMI_FLASH_LIB__H__
#define __AMI_FLASH_LIB__H__

// Bit definitions for AmiPcdFlashPropertyMask
/// The flash device is not memory mapped when flash writes are enabled
#define FLASH_PROPERTY_NON_MEMORY_MAPPED_ON_WRITE_ENABLE 1
/// The flash device is not memory map under all circumstances
#define FLASH_PROPERTY_NON_MEMORY_MAPPED 3
/// The flash device has a zero flash erase polarity
#define FLASH_PROPERTY_ZERO_ERASE_POLARITY 4

/**
  Function to enable writes to the flash part.
**/
VOID AmiFlashDeviceWriteEnable(VOID);

/**
  Function to disable writes to the flash part.
**/
VOID AmiFlashDeviceWriteDisable(VOID);

/**
  Write the data into the block pointed to by BlockAddress

  @param BlockAddress Pointer to the flash part area to write the data into
  @param Data Pointer to the data buffer to write into provided the BlockAddress

  @return BOOLEAN
  @retval TRUE The flash region was updated
  @retval FALSE The flash region could not be updated
**/
BOOLEAN AmiFlashWriteBlock(VOID* BlockAddress, VOID *Data);

/**
  Erase the block pointed to by the passed BlockAddress.

  @param BlockAddress Pointer to an address contained in the block.

  @return BOOLEAN
  @retval TRUE Erase completed successfully
  @retval FALSE Erase did not complete successfully
**/
BOOLEAN AmiFlashEraseBlock(VOID* BlockAddress);

/**
  Enable writing to the block that contains the BlockAddress

  @param BlockAddress Pointer to an address contained in the block.
**/
VOID AmiFlashBlockWriteEnable(VOID* BlockAddress);

/**
  Disable writing to the block that contains the BlockAddress.

  @param BlockAddress Pointer to an address contained in the block.
**/
VOID AmiFlashBlockWriteDisable(VOID* BlockAddress);

/**
  Program Length bytes of data from Data into the Address.

  @param Address Pointer to the address to write the data
  @param Data The data to write into the address
  @param Length The number of bytes that need to be written

  @return BOOLEAN
  @retval TRUE Programming the data was successful
  @retval FALSE Programming the data was not successful 
**/
BOOLEAN AmiFlashProgram(VOID* Address, VOID *Data, UINT32 Length);

/**
  Read Length bytes from the Address and put them into the Data buffer

  @param Address Pointer to an address to start reading
  @param Data Pointer to a buffer to place the data into
  @param Length The number of bytes to read and place into the buffer

  @return BOOLEAN
  @retval TRUE Erase completed successfully
  @retval FALSE Erase did not complete successfully
**/
BOOLEAN  AmiFlashRead(VOID* Address, VOID *Data, UINT32 Length);

/**
  Converts a physical address to the starting address of the block
**/
VOID *AmiFlashBlockStartAddress(VOID *Address);

/**
  Performs flash device detection and prepares the library to work with the detected flash device.

  NOTE: There is no requirement of calling this function prior to using other library functions.
  This function can be used to force re-initialization of the library (and re-detection of the flash device).
  The re-initialization might be useful when system switches between multiple flash devices at runtime.
  
  @return BOOLEAN
  @retval TRUE Erase completed successfully
  @retval FALSE Erase did not complete successfully
**/
BOOLEAN AmiFlashInit();

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
