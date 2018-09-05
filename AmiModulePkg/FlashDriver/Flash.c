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
  Common file for both the DXE and the SMM drivers. Contains the code for the
  flash protocol functions. The flash protocol functions are wrappers
  around the corresponding AmiFlashLib library functions plus a couple of extras describe below.
      
  1. In order to support fault tolerant FV update, the driver attempts to detect
  FV update transactions by monitoring a sequence of flash protocol calls.
  The driver corrupts FV signature at the beginning of the flash update
  and restores it back once FV  update is completed.
  The RomLayout PEIM checks FV signature prior to publishing an FV.
  If FV signature is corrupted, firmware recovery is initiated.
  2. Flash device write enable/disable operations are expensive on some platforms 
  (for example, when the operation involves communication with embedded controller).
  In order to reduce a number of flash write enable/disable operations, and in order to support overlapping flash transactions
  (for example a SetVariable call in the middle of the FV_MAIN update), the driver maintains a counter that tracks
  DeviceWriteEnable/DeviceWriteDisable calls. The counter is used to skip unnecessary hardware manipulations performed by
  the library AmiFlashDeviceWriteEnable/AmiFlashDeviceWriteDisable functions.
  When Flash->DeviceWriteEnable is called and write enable counter is not zero, AmiFlashDeviceWriteEnable is not called 
  because flash writes are already enabled.
  When Flash->DeviceWriteDisale is called and write enable counter is not zero, AmiFlashDeviceWriteDisable is not called 
  because there are pending flash transactions.
  There are two instances of the Flash driver (DXE instance and SMM instance). Each instance maintains its own write enable counter.
  This may lead to a situation when one of the instances disables flash writes while transaction in another instance is in progress.
  This case is handled by the error recovery code included into FlashDriverErase and FlashDriverWriate functions.
  The function call AmiFlashDeviceWriteEnable to re-enable flash writes if first attempt to modify flash part content fails.
  We are not implementing synchronized counters or a single shared counter because
  such implementation would require a communication channel between DXE and SMM instances of the flash driver, which would make
  SMM instance susceptible to attacks from the OS environment.
**/

#include "FlashCommon.h"
#include <Library/AmiFlashLib.h>
#include <Token.h>

#ifndef NO_MMIO_FLASH_ACCESS_DURING_UPDATE
#define NO_MMIO_FLASH_ACCESS_DURING_UPDATE 0
#endif
static UINTN FlashEmpty = (UINTN)(-FLASH_ERASE_POLARITY); //Flash.sdl
static BOOLEAN FlashNotMemoryMapped = NO_MMIO_FLASH_ACCESS_DURING_UPDATE; //Nvram.sdl
static UINTN FlashDeviceBase = 0xFFFFFFFF - FLASH_SIZE + 1; //Configuration.sdl
static UINT32 FlashBlockSize = FLASH_BLOCK_SIZE; //Configuration.sdl


EFI_STATUS EFIAPI FlashDriverReadExt(
    VOID* FlashAddress, UINTN Size, VOID* DataBuffer
);
EFI_STATUS EFIAPI FlashDriverEraseExt(
    VOID* FlashAddress, UINTN Size
);
EFI_STATUS EFIAPI FlashDriverWriteExt(
    VOID* FlashAddress, UINTN Size, VOID* DataBuffer
);
EFI_STATUS EFIAPI FlashDriverUpdateExt(
    VOID* FlashAddress, UINTN Size, VOID* DataBuffer
);
EFI_STATUS EFIAPI FlashDriverDeviceWriteEnableExt(VOID);
EFI_STATUS EFIAPI FlashDriverDeviceWriteDisableExt(VOID);

#define BEGIN_CRITICAL_SECTION(Cs) \
    { EFI_STATUS Status = BeginCriticalSection(Cs);\
      ASSERT(Status==EFI_SUCCESS || Status==EFI_ACCESS_DENIED);\
      if (EFI_ERROR(Status)) return Status;\
    }
#define END_CRITICAL_SECTION(Cs) \
	{ EFI_STATUS Status = EndCriticalSection(Cs);\
	  ASSERT_EFI_ERROR(Status);\
	}

FLASH_PROTOCOL_PRIVATE FlashData =
{
    {
        FlashDriverReadExt, FlashDriverEraseExt, FlashDriverWriteExt, FlashDriverUpdateExt,
        FlashDriverDeviceWriteEnableExt, FlashDriverDeviceWriteDisableExt
    },
    0
};

#define MAX_NUMBER_OF_UPDATED_AREAS 10
UPDATED_AREA_DESCRIPTOR UpdatedArea[MAX_NUMBER_OF_UPDATED_AREAS];
INT32 NumberOfUpdatedAreas=0;
CRITICAL_SECTION Cs;

/** @internal
  Enable writing to the flash device.
  This is an internal function. External consumers use FlashDriverDeviceWriteEnableExt.

  @return EFI_STATUS
  @retval EFI_SUCCESS Writing to the flash part has been enabled
**/
EFI_STATUS EFIAPI FlashDriverDeviceWriteEnable()
{
    FlashData.WriteEnableCounter++;
    // Skip flash write enable operation if flash writes have already been enabled
    if (FlashData.WriteEnableCounter==1)
    {
        AmiFlashDeviceWriteEnable();
    }
    
    return EFI_SUCCESS;
}

/** @internal
  Disable writing to the flash part
  This is an internal function. External consumers use FlashDriverDeviceWriteDisableExt.

  @return EFI_STATUS
  @retval EFI_SUCCESS Writing to the flash part was disabled
**/
EFI_STATUS EFIAPI FlashDriverDeviceWriteDisable()
{
    // Skip flash write disable operation if there are pending flash update transactions
    if (FlashData.WriteEnableCounter!=0)
    {
        FlashData.WriteEnableCounter--;
        
        if (FlashData.WriteEnableCounter==0)
        {
            AmiFlashDeviceWriteDisable();
        }
    }
    
    return EFI_SUCCESS;
}

/** @internal
  Read flash part content.

  When flash writes are enabled, call the flash library's AmiFlashRead function.  Otherwise,
  read the flash area directly.
  This is an internal function. External consumers use FlashDriverReadExt.

  @param FlashAddress The address of the flash part to read
  @param Size The size of the data to read
  @param DataBuffer The buffer to store the data as it is read

  @return EFI_STATUS
  @retval EFI_SUCCESS The data was read from the flash part and put into DataBuffer
  @retval EFI_DEVICE_ERROR The data could not be read from the flash part
**/
EFI_STATUS EFIAPI FlashDriverRead(
    VOID* FlashAddress, UINTN Size, VOID* DataBuffer
)
{

    if (FlashData.WriteEnableCounter!=0)
    {
        BOOLEAN Status;
        Status = AmiFlashRead(FlashAddress, DataBuffer, (UINT32)Size);
        return (Status) ? EFI_SUCCESS : EFI_DEVICE_ERROR;
    }
    else
    {
        CopyMem(DataBuffer,FlashAddress,Size);
        return EFI_SUCCESS;
    }
    
}

#define INT_SIZE sizeof(INTN)
#define FLASH_EMPTY_BYTE ((UINT8)FlashEmpty)

/** @internal
  Loop through the passed buffer and determine if it is empty (matches the FlashEmpty value).

  @param Address Pointer to the buffer to check
  @param Size Size of the buffer

  @return BOOLEAN
  @retval TRUE The buffer is empty (matches the FlashEmpty value)
  @retval FALSE The buffer is not empty
**/
BOOLEAN IsClean(IN UINT8 *Address, IN UINTN Size)
{
    // loops through the buffer and check if it's empty
    if (!( (UINTN)Address & (INT_SIZE-1) ))
    {
        for ( ; Size >= INT_SIZE; Size -= INT_SIZE, Address += INT_SIZE)
        {
            if (FlashNotMemoryMapped)
            {
                UINTN nData=0;	
		        FlashDriverRead (Address, INT_SIZE, (UINT8*)&nData );
		        if (nData != FlashEmpty) return FALSE;
            }
            else
                if (*(UINTN*)Address != FlashEmpty) return FALSE;
	    }

    }
    
    // the address may not be INT_SIZE aligned
    // check the remaining part of the buffer
    for ( ; Size > 0; Size--, Address++)
    {
        if (FlashNotMemoryMapped)
        {
            UINT8 nData=0;	
		    FlashDriverRead (Address, 1, &nData );
		    if (nData != FLASH_EMPTY_BYTE) return FALSE;
        }
        else
            if (*Address != FLASH_EMPTY_BYTE) return FALSE;

	}

    return TRUE;
}

/** @internal
  Erase content of the portion of flash part
  
  The function erases Size bytes of the flash part space starting at address FlashAddress.
  This is an internal function. External consumers use FlashDriverEraseExt.
  
  @param FlashAddress Flash address to erase. Must start at the flash part block boundary.
  @param Size Size of the flash region to erase. Must be multiple of the flash part block size.

  @return EFI_STATUS
  @retval EFI_SUCCESS The desired portion of the flash was erased
  @retval EFI_DEVICE_ERROR An error was encountered while erasing the flash
**/
EFI_STATUS EFIAPI FlashDriverErase(VOID* FlashAddress, UINTN Size)
{
    BOOLEAN Status = TRUE;
    UINT8 *Address = (UINT8*)FlashAddress;
    BOOLEAN FlashWriteEnabled = FALSE;
    
    // Enable write
    FlashDriverDeviceWriteEnable();
    
    for (; Status && Size>0; Address+=FlashBlockSize, Size-=FlashBlockSize)
    {
        // If block is already clean, skip block erase and continue to the next block
        if (IsClean(Address, FlashBlockSize)) continue;
        // Erase the block
        AmiFlashBlockWriteEnable(Address);
        Status=AmiFlashEraseBlock(Address);
        if (!Status && !FlashWriteEnabled){
            // Error recovery code. The flash write may have been disabled due to 
            // overlapping transactions in Runtime and SMM instances of the flash driver.
            // Re-enable the flash writes.See item 2 in the file comments section for additional details.
            AmiFlashDeviceWriteEnable();
            AmiFlashBlockWriteEnable(Address);
            FlashWriteEnabled = TRUE;
            Status=AmiFlashEraseBlock(Address);
        }
        AmiFlashBlockWriteDisable(Address);
    }
    // Disable Write
    FlashDriverDeviceWriteDisable();
    return (Status) ? EFI_SUCCESS : EFI_DEVICE_ERROR;
}

/** @internal
  Write provided data to a flash region
  
  Write the passed data from DataBuffer into the flash part at FlashAddress.
  Unlike FlashDriverUpdate, this function does not erase the flash part content
  and therefore may fail is flash region being written to is not blank.
  This is an internal function. External consumers use FlashDriverWriteExt.

  @param FlashAddress The address in the flash part to write the data to
  @param Size The size of the data to write
  @param DataBuffer Pointer to the buffer of data to write into the flash part

  @return EFI_STATUS
  @retval EFI_SUCCESS The data was written successfully
  @retval EFI_DEVICE_ERROR An error was encountered while writing the data
**/
EFI_STATUS EFIAPI FlashDriverWrite(
    VOID* FlashAddress, UINTN Size, VOID* DataBuffer
)
{
    BOOLEAN Status = TRUE;
    UINT8 *Address = (UINT8*)FlashAddress;
    UINT8 *Data = (UINT8*)DataBuffer;
    UINT8 *BlockAddress = AmiFlashBlockStartAddress(Address); //Align to the block size
    UINTN PartialSize;
    BOOLEAN FlashWriteEnabled = FALSE;
    
    FlashDriverDeviceWriteEnable();
    // If FlashAddress is not aligned on the flash block boundary
    // perform partial programming of the first block
    if (BlockAddress!=Address)
    {
        PartialSize = FlashBlockSize - (Address - BlockAddress);
        
        if (PartialSize > Size) PartialSize = Size;
        // Write the data
        AmiFlashBlockWriteEnable(BlockAddress);
        Status=AmiFlashProgram(Address, Data, (UINT32)PartialSize);
        if (!Status && !FlashWriteEnabled){
            // Error recovery code. The flash write may have been disabled due to 
            // overlapping transactions in Runtime and SMM instances of the flash driver.
            // Re-enable the flash writes.See item 2 in the file comments section for additional details.
            AmiFlashDeviceWriteEnable();
            AmiFlashBlockWriteEnable(BlockAddress);
            FlashWriteEnabled = TRUE;
            Status=AmiFlashProgram(Address, Data, (UINT32)PartialSize);
        }
        AmiFlashBlockWriteDisable(BlockAddress);
        Address = BlockAddress + FlashBlockSize;
        Size -= PartialSize;
        Data += PartialSize; 
    }
    // Full blocks programming
    for (; Status && Size>=FlashBlockSize
            ; Address+=FlashBlockSize, Size-=FlashBlockSize, Data+=FlashBlockSize
        )
    {
        AmiFlashBlockWriteEnable(Address);
        Status=AmiFlashProgram(Address, Data, FlashBlockSize);
        if (!Status && !FlashWriteEnabled){
            // Error recovery code. The flash write may have been disabled due to 
            // overlapping transactions in Runtime and SMM instances of the flash driver.
            // Re-enable the flash writes.See item 2 in the file comments section for additional details.
            AmiFlashDeviceWriteEnable();
            AmiFlashBlockWriteEnable(Address);
            FlashWriteEnabled = TRUE;
            Status=AmiFlashProgram(Address, Data, FlashBlockSize);
        }
        AmiFlashBlockWriteDisable(Address);
    }
    // Partial programming of the last block
    if (Size!=0 && Status)
    {
        AmiFlashBlockWriteEnable(Address);
        Status=AmiFlashProgram(Address, Data, (UINT32)Size);
        if (!Status && !FlashWriteEnabled){
            // Error recovery code. The flash write may have been disabled due to 
            // overlapping transactions in Runtime and SMM instances of the flash driver.
            // Re-enable the flash writes.See item 2 in the file comments section for additional details.
            AmiFlashDeviceWriteEnable();
            AmiFlashBlockWriteEnable(Address);
            FlashWriteEnabled = TRUE;
            Status=AmiFlashProgram(Address, Data, (UINT32)Size);
        }
        AmiFlashBlockWriteDisable(Address);
    }
    
    FlashDriverDeviceWriteDisable();
    return (Status) ? EFI_SUCCESS : EFI_DEVICE_ERROR;
}

/** @internal
  Update the region of the flash part with the provided data
  
  Write the passed data from DataBuffer into the flash part at FlashAddress.
  Unlike FlashDriverWrite, this function will perform flash part erase 
  if the corresponding flash region is not blank.
  This is an internal function. External consumers use FlashDriverUpdateExt.

  @param FlashAddress The address in the flash part to write the data to. Must start at the flash part block boundary.
  @param Size The size of the data to write. Must be multiple of the flash part block size.
  @param DataBuffer Pointer to the buffer of data to write into the flash part

  @return EFI_STATUS
  @retval EFI_SUCCESS The data was written successfully
  @retval EFI_DEVICE_ERROR An error was encountered while writing the data
**/
EFI_STATUS EFIAPI FlashDriverUpdate(
    VOID* FlashAddress, UINTN Size, VOID* DataBuffer
)
{
    BOOLEAN Status = TRUE;
    UINT8 *Address = (UINT8*)FlashAddress;
    UINT8 *Data = (UINT8*)DataBuffer;
    
    FlashDriverDeviceWriteEnable();
    
    for (; Status && Size>=FlashBlockSize
            ; Address+=FlashBlockSize, Size-=FlashBlockSize, Data+=FlashBlockSize
        )
    {
        // Writes a block
        Status = AmiFlashWriteBlock(Address, Data);
    }
    
    FlashDriverDeviceWriteDisable();
    return (Status) ? EFI_SUCCESS : EFI_DEVICE_ERROR;
}

/** @internal
  Check if the address that is going to be erased matches a firmware volume address. If it does,
  invalidate the firmware volume signature so that if erasing is interrupted, the system will
  not attempt to use the partially erased firmware volume because it will not be considered valid.

  @param FlashAddress Address of the firmware volume
**/
VOID BeforeErase(VOID* FlashAddress)
{
    AMI_ROM_AREA *Area;
    UINT32 Data;

    if ( NumberOfUpdatedAreas == MAX_NUMBER_OF_UPDATED_AREAS ) return;
    Area = AmiGetRomAreaByAddress((EFI_PHYSICAL_ADDRESS)(UINTN)FlashAddress);
    if (   Area == NULL || Area->Type != AmiRomAreaTypeFv 
    	|| Area->Address != (EFI_PHYSICAL_ADDRESS)(UINTN)FlashAddress
    ) return;
    
    UpdatedArea[NumberOfUpdatedAreas].AreaSize = Area->Size;
    UpdatedArea[NumberOfUpdatedAreas].RomArea = Area;
    UpdatedArea[NumberOfUpdatedAreas++].RestoreSignature = FALSE;
    // Invalidate FV by destroying the signature
    Data = (UINT32)~FlashEmpty;
    FlashDriverWrite(
        &((EFI_FIRMWARE_VOLUME_HEADER*)FlashAddress)->Signature,
        sizeof(UINT32),
        &Data
    );
}

/** @internal
  Check if the address and size that are attempting to be written correspond to a firmware volume. If they do, overwrite
  the firmware volume signature. Do this so that if the system is interrupted while attempting to flash, that on reboot
  the partially written firmware volume will not be seen as valid.

  @param FlashAddress Pointer to the address that is the system is going to write data to.
  @param Size Size of the data the system is going to write.
  @param DataBuffer Pointer to the data buffer containing what is to be written.
**/
VOID BeforeWrite(VOID* FlashAddress, UINTN Size, VOID* DataBuffer)
{
    EFI_FIRMWARE_VOLUME_HEADER* Fv;
    INT32 i;

    for (i=0; i<NumberOfUpdatedAreas; i++)
    {
        // Invalidate FV by destroying the signature (alter data being programmed)
        if (  UpdatedArea[i].RomArea->Address == (EFI_PHYSICAL_ADDRESS)(UINTN)FlashAddress
           )
        {
            Fv = (EFI_FIRMWARE_VOLUME_HEADER*)DataBuffer;
            
            if (   Size < sizeof(EFI_FIRMWARE_VOLUME_HEADER)
                    || Fv->Signature != EFI_FVH_SIGNATURE
               )   //The data being programmed is not FV, don't do anything
            {
                UpdatedArea[i] = UpdatedArea[NumberOfUpdatedAreas-1];
                NumberOfUpdatedAreas--;
            }
            else
            {
                Fv->Signature = (UINT32)FlashEmpty;
                UpdatedArea[i].AreaSize = (UINT32)Fv->FvLength;
                UpdatedArea[i].RestoreSignature = TRUE;
            }
            return;
        }
    }
}

/** @internal
  After the write action was completed, check if the write corresponded to writing an entire firmware
  volume into the flash part. If it was a firmware volume, then perform a write to the flash part with
  the valid firmware volume header signature, and restore the DataBuffer's Firmware volume header signature
  as well

  @param FlashAddress Pointer to the address of the FV in the flash part
  @param Size Size of the data to write
  @param DataBuffer Pointer to the data buffer
**/
VOID AfterWrite(VOID* FlashAddress, UINTN Size, VOID* DataBuffer)
{
    EFI_PHYSICAL_ADDRESS Address;
    INT32 i;
    
    Address = (EFI_PHYSICAL_ADDRESS)(UINTN)FlashAddress;
    for (i=0; i<NumberOfUpdatedAreas; i++)
    {
       
        if (UpdatedArea[i].RomArea->Address==Address)
        {
            //Restore original data
            ((EFI_FIRMWARE_VOLUME_HEADER*)DataBuffer)->Signature=EFI_FVH_SIGNATURE;
        }
        
        if (UpdatedArea[i].RomArea->Address+UpdatedArea[i].AreaSize==Address+Size)
        {
            UINT32 FvSignature = EFI_FVH_SIGNATURE;
            //Restore FV signature
            FlashDriverWrite(
                &((EFI_FIRMWARE_VOLUME_HEADER*)UpdatedArea[i].RomArea->Address)->Signature,
                sizeof(UINT32),
                &FvSignature
            );
            UpdatedArea[i] = UpdatedArea[NumberOfUpdatedAreas-1];
            NumberOfUpdatedAreas--;
        }
    }
}

/** @internal
  If attempting to read in the middle of a flash update, then attempting to read a firmware volume may
  not return the proper firmware volume header signature. In this case, the AfterRead function will
  patch the returned data with a proper firmware volume header signature.

  @param FlashAddress Address of the flash part to read
  @param Size Size of the data to read from the flash part
  @param DataBuffer The data buffer to store the read data into.
**/
VOID AfterRead(VOID* FlashAddress, UINTN Size, VOID* DataBuffer)
{
    INT32 i;

    if (FlashData.WriteEnableCounter==0) return;
    // If we are in the middle of flash update 
    // (FlashData.WriteEnableCounter is not zero), 
    // it may happen that the FV signature has not been restored yet 
    // (typically happens during partial FV updates).
    // Let's return the proper value. The signature will be restored later.
    for (i=0; i<NumberOfUpdatedAreas; i++)
    {
        EFI_FIRMWARE_VOLUME_HEADER* Fv = (EFI_FIRMWARE_VOLUME_HEADER*)(UpdatedArea[i].RomArea->Address);
        if ( Fv == FlashAddress && Size >= OFFSET_OF(EFI_FIRMWARE_VOLUME_HEADER, Attributes))
        {
            ((EFI_FIRMWARE_VOLUME_HEADER*)DataBuffer)->Signature = EFI_FVH_SIGNATURE;
            return;
        }
    }
}

/**
  Read flash part content.

  When flash writes are enabled, call the flash library's AmiFlashRead function.  Otherwise,
  read the flash area directly.

  @param FlashAddress The address of the flash part to read
  @param Size The size of the data to read
  @param DataBuffer The buffer to store the data as it is read

  @return EFI_STATUS
  @retval EFI_SUCCESS The data was read from the flash part and put into DataBuffer
  @retval EFI_DEVICE_ERROR The data could not be read from the flash part**/
EFI_STATUS EFIAPI FlashDriverReadExt(
    VOID* FlashAddress, UINTN Size, VOID* DataBuffer
){
    EFI_STATUS Status;
    BEGIN_CRITICAL_SECTION(Cs);
    Status = FlashDriverRead(FlashAddress,Size,DataBuffer);
    AfterRead(FlashAddress, Size, DataBuffer);
    END_CRITICAL_SECTION(Cs);
    return Status;
}

/**
  Erase content of the portion of flash part
  
  The function erases Size bytes of the flash part space starting at address FlashAddress.
  
  @param FlashAddress Flash address to erase. Must start at the flash part block boundary.
  @param Size Size of the flash region to erase. Must be multiple of the flash part block size.

  @return EFI_STATUS
  @retval EFI_SUCCESS The desired portion of the flash was erased
  @retval EFI_DEVICE_ERROR An error was encountered while erasing the flash
**/
EFI_STATUS EFIAPI FlashDriverEraseExt(
    VOID* FlashAddress, UINTN Size
){
    EFI_STATUS Status;

    if (Size==0) return EFI_SUCCESS;
    // If Address in not aligned on flash block boundary or Size is not multiple of FlashBlockSize, abort
    if ((UINT8*)AmiFlashBlockStartAddress(FlashAddress)!=FlashAddress || Size%FlashBlockSize!=0)
        return EFI_UNSUPPORTED;

    BEGIN_CRITICAL_SECTION(Cs);
    // Invalidate FV by destroying the signature
    BeforeErase(FlashAddress);
    Status = FlashDriverErase(FlashAddress, Size);
    END_CRITICAL_SECTION(Cs);
    return Status;
}

/**
  Write provided data to a flash region
  
  Write the passed data from DataBuffer into the flash part at FlashAddress.
  Unlike FlashDriverUpdateExt, this function does not erase the flash part content
  and therefore may fail is flash region being written to is not blank.

  @param FlashAddress The address in the flash part to write the data to
  @param Size The size of the data to write
  @param DataBuffer Pointer to the buffer of data to write into the flash part

  @return EFI_STATUS
  @retval EFI_SUCCESS The data was written successfully
  @retval EFI_DEVICE_ERROR An error was encountered while writing the data
**/
EFI_STATUS EFIAPI FlashDriverWriteExt(
    VOID* FlashAddress, UINTN Size, VOID* DataBuffer
){
    EFI_STATUS Status;
    if (Size==0) return EFI_SUCCESS;

    BEGIN_CRITICAL_SECTION(Cs);
    // Invalidate FV by destroying the signature--------------------
    BeforeWrite(FlashAddress, Size, DataBuffer);
    Status = FlashDriverWrite(FlashAddress,Size,DataBuffer);
    // Restore FV signature-------------------------------------------
    AfterWrite(FlashAddress, Size, DataBuffer);
    END_CRITICAL_SECTION(Cs);
    return Status;
}

/**
  Update the region of the flash part with the provided data
  
  Write the passed data from DataBuffer into the flash part at FlashAddress.
  Unlike FlashDriverWriteExt, this function will perform flash part erase 
  if the corresponding flash region is not blank.

  @param FlashAddress The address in the flash part to write the data to. Must start at the flash part block boundary.
  @param Size The size of the data to write. Must be multiple of the flash part block size.
  @param DataBuffer Pointer to the buffer of data to write into the flash part

  @return EFI_STATUS
  @retval EFI_SUCCESS The data was written successfully
  @retval EFI_DEVICE_ERROR An error was encountered while writing the data
**/
EFI_STATUS EFIAPI FlashDriverUpdateExt(
    VOID* FlashAddress, UINTN Size, VOID* DataBuffer
){
    EFI_STATUS Status;

    if (Size==0) return EFI_SUCCESS;
    // If Address in not aligned on flash block boundary or Size is not multiple of FlashBlockSize, abort
    if ((UINT8*)AmiFlashBlockStartAddress(FlashAddress)!=FlashAddress || Size%FlashBlockSize!=0)
        return EFI_UNSUPPORTED;

    BEGIN_CRITICAL_SECTION(Cs);
    BeforeErase(FlashAddress);
    BeforeWrite(FlashAddress, Size, DataBuffer);
    Status = FlashDriverUpdate(FlashAddress,Size,DataBuffer);
    AfterWrite(FlashAddress, Size, DataBuffer);
    END_CRITICAL_SECTION(Cs);
    return Status;
}

/**
  Enable writing to the flash device.

  @return EFI_STATUS
  @retval EFI_SUCCESS Writing to the flash part has been enabled
**/
EFI_STATUS EFIAPI FlashDriverDeviceWriteEnableExt(VOID){
    EFI_STATUS Status;
    BEGIN_CRITICAL_SECTION(Cs);
    Status = FlashDriverDeviceWriteEnable();
    END_CRITICAL_SECTION(Cs);
    return Status;
}

/**
  Disable writing to the flash part

  @return EFI_STATUS
  @retval EFI_SUCCESS Writing to the flash part was disabled
**/
EFI_STATUS EFIAPI FlashDriverDeviceWriteDisableExt(VOID){
    EFI_STATUS Status;
    UINT32 OldWriteEnableCounter;

    BEGIN_CRITICAL_SECTION(Cs);
    OldWriteEnableCounter = FlashData.WriteEnableCounter;
    Status = FlashDriverDeviceWriteDisable();
    if (OldWriteEnableCounter!=0 && FlashData.WriteEnableCounter==0)
    {
        // Before disabling the flash writes
        // restore the destroyed FV signatures (if any).
        // In case of full FV update, the FV signature is restored 
        // in the AfterWrite function once the last FV block is updated. 
        // In case of partial FV update when last FV block is not updated, 
        // the FV Signature is restored either here.
        INT32 i;
        for (i=0; i<NumberOfUpdatedAreas; i++)
        {
            if (UpdatedArea[i].RestoreSignature){
                EFI_FIRMWARE_VOLUME_HEADER* Fv = (EFI_FIRMWARE_VOLUME_HEADER*)(UpdatedArea[i].RomArea->Address);
                UINT32 FvSignature = EFI_FVH_SIGNATURE;
                //Restore FV signature
                FlashDriverWrite(
                    &Fv->Signature, sizeof(UINT32), &FvSignature
                );
            }
        }
        NumberOfUpdatedAreas = 0;
        AmiFlashDeviceWriteDisable();
    }

    END_CRITICAL_SECTION(Cs);
    return Status;
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
