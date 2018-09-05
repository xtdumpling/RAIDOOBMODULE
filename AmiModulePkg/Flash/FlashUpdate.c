//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

/** @file
  Flash Update file.
**/
//----------------------------------------------------------------------
// header includes
#include <Efi.h>
#include <Flash.h>
#include <Token.h>

//----------------------------------------------------------------------
// external variable defines
extern const UINTN FlashEmpty;

//----------------------------------------------------------------------
// global variable for the module
#define INT_SIZE sizeof(INTN)
#define FLASH_EMPTY_BYTE (UINT8)FlashEmpty

//----------------------------------------------------------------------
// Function prototypes
BOOLEAN
FlashEraseCommand (
    volatile UINT8* pBlockAddress
    );
EFI_STATUS
FlashEraseCommandEx (
    volatile UINT8* pBlockAddress,
    UINT32          Length
    );
BOOLEAN
FlashProgramCommand (
    volatile UINT8* pByteAddress,
    UINT8 *Byte, UINT32 Length
    );

VOID
FlashReadCommand (
    volatile UINT8* pByteAddress,
    UINT8 *Byte, UINT32 Length
    );

BOOLEAN
FlashIsEraseCompleted (
    volatile UINT8* pBlockAddress,
    BOOLEAN *pError,
    UINTN *pStatus
    );
BOOLEAN
FlashIsProgramCompleted (
    volatile UINT8* pByteAddress,
    UINT8 *Byte,
    UINT32 Length,
    BOOLEAN *pError,
    UINTN *pStatus
    );


//----------------------------------------------------------------------
// Function definitions
/**
 * Verifies if the device has been erased properly or if the current byte 
 * is the same as the byte to be written at this location.
 *
 * @param pDest starting address of where the data will be written
 * @param pSource starting address of the data that is supposed to be written
 * @param Size length of the data set to check
 * @param pIsClean return value to indicate that the data area that will be written 
 *        to has already been erased
 * @param pIsEqual return value to indicate that the data to be written is the same
 *        as the data that is already there  
 *                      
 */
VOID
IsCleanOrEqual  (
    IN  UINT8           *pDest,
    IN  const UINT8     *pSource,
    IN  UINTN           Size,
    OUT BOOLEAN         *pIsClean,
    OUT BOOLEAN         *pIsEqual
)
{
    *pIsEqual = TRUE;
    *pIsClean = TRUE;

#if defined NO_MMIO_FLASH_ACCESS_DURING_UPDATE && NO_MMIO_FLASH_ACCESS_DURING_UPDATE == 1
    {
        UINT8               bData = 0;
        for( ; Size > 0; Size--) {
            FlashReadCommand(pDest++, &bData, sizeof(UINT8));
            if (bData != FLASH_EMPTY_BYTE) *pIsClean = FALSE;
            if (bData != *pSource++) *pIsEqual = FALSE;
            if (!(*pIsEqual || *pIsClean)) return;
        }
    }
#else
    // loops through the destination looking to see if the data is the same
    //  as the source, or if the Destination has already bee erased
    if (!( (UINTN)pDest & (INT_SIZE-1) || (UINTN)pSource & (INT_SIZE-1) ))
    {
        UINTN *Dest = (UINTN*)pDest, *Source = (UINTN*)pSource;

        for( ; Size >= INT_SIZE; Size -= INT_SIZE)
        {
            if (*Dest != FlashEmpty)
                *pIsClean = FALSE;
            if (*Dest++ != *Source++)
                *pIsEqual = FALSE;
            if ( !(*pIsEqual || *pIsClean) )
                return;
        }
    }

    // since the address may not be INT_SIZE aligned, this checks
    //  the rest of the data
    for( ; Size > 0; Size--)
    {
        if (*pDest != FLASH_EMPTY_BYTE)
            *pIsClean = FALSE;
        if (*pDest++ != *pSource++)
            *pIsEqual = FALSE;
        if ( !(*pIsEqual || *pIsClean) )
            return;
    }
#endif
}


/**
 * Erases the block pointed to by BlockAddress
 *
 * @param BlockAddress Address within the block to be erased
 *                      
 * @retval TRUE Erase completed successfully
 * @retval FALSE Erase did not complete properly
 */
BOOLEAN
FlashEraseBlock (
    IN volatile UINT8*      BlockAddress
)
{
    return FlashEraseCommand(BlockAddress);
}


/**
 * Erases the block pointed to by BlockAddress
 *
 * @param BlockAddress Address within the block to be erased
 * @param Length length of the data to write
 *                      
 * @retval TRUE Erase completed successfully
 * @retval FALSE Erase did not complete properly
 */
BOOLEAN
FlashErase (
    IN volatile UINT8*      BlockAddress,
    IN UINT32               Length
)
{
    return (EFI_ERROR(FlashEraseCommandEx(BlockAddress, Length))) ? FALSE : TRUE;
}


/**
 * Programs the area pointed to by BlockAddress and defined by Length with 
 * the data pointed to by Data
 *
 * @param Address Address where to write the Data
 * @param Data pointer to the data to write
 * @param Length length of the data to write
 *                      
 * @retval TRUE Program completed successfully
 * @retval FALSE Program did not complete properly
 */
BOOLEAN
FlashProgram    (
    IN volatile UINT8*      Address,
    IN UINT8                *Data,
    IN UINT32               Length
)
{
    return FlashProgramCommand(Address,Data,Length);
}


/**
 * This function returns the read data from the location indicated the data 
 * pointed to by Data.
 *
 * @param Address Address where to read the Data
 * @param Data pointer to the data to read
 * @param Length length of the data to read
 *                      
 * @retval TRUE Read completed successfully
 * @retval FALSE Read did not complete properly
 */
BOOLEAN
FlashRead   (
    IN  volatile UINT8*     Address,
    OUT UINT8               *Data,
    IN  UINT32              Length
)
{
    FlashReadCommand(Address,Data,Length);

    return TRUE;
}


/**
 * Programs the Block pointed to by pBlock with the data pointed to by pData
 *
 * @param pBlock Address of the beginning of the block
 * @param pData pointer to the data to write
 *                      
 * @retval TRUE Program completed successfully
 * @retval FALSE Program did not complete properly
 */
BOOLEAN
FlashProgramBlock   (
    IN UINT8*               pBlock,
    IN UINT8*               pData
)
{
    return FlashProgram(pBlock,pData,FlashBlockSize);
}


/**
 * Enables the ability to write to the flash part and programs the Block 
 * pointed to by pBlock with the data pointed to by Data and then sets the
 * block to read only
 *
 * @param pBlock Address of the beginning of the block
 * @param pData pointer to the data to write
 *                      
 * @retval TRUE Program completed successfully
 * @retval FALSE Program did not complete properly
 */
BOOLEAN
FlashWriteBlock (
    IN UINT8*               pBlock,
    IN UINT8*               pData
)
{
    BOOLEAN IsClean;
    BOOLEAN IsEqual;
    BOOLEAN Result;
    // This checks to see if the data in the flash part has already been
    // erased or if it is already the same as what will be programmed there
    IsCleanOrEqual(pBlock, pData, FlashBlockSize, &IsClean, &IsEqual);

    // If it is already the same then exit
    if (IsEqual)
        return TRUE;

    // if not the same enable the block for writing
    FlashBlockWriteEnable(pBlock);

    // This loop checks IsClean to see if the block area that is to be written to
    // has already been erased.  If it hasn't then, the FlashEraseBlock() is called
    // to erase the block.
    // If it is either clean or is erased, then it calls the program function
    if (IsClean || FlashEraseBlock(pBlock))
        Result = FlashProgramBlock(pBlock,pData);
    // If it is not clean and the erase block function does not finish properly
    // then set result to return false
    else
        Result = FALSE;

    // disable the ability to write to the block
    FlashBlockWriteDisable(pBlock);

    return Result;
}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
