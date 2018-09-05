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
  Flash update routines file.
**/

//----------------------------------------------------------------------
// Includes
#include <Efi.h>
//#include <Library/DebugLib.h>
#include <AmiDxeLib.h>
#include <Library/TimerLib.h>
#include "FlashPart.h"
#include "Token.h"


//----------------------------------------------------------------------
// Local MACRO Definitions
#define UNLOCK              0
#define LOCK                1

//----------------------------------------------------------------------------
// Flash Part Specific Definitions

#define S29GL01GP_MANID     0x01    // offset 0x00 = Manufacturers ID
#define S29GL01GP_DEVID1    0x7e    // offset 0x02 = Device ID byte 1
#define S29GL01GP_DEVID2    0x28    // offset 0x1c = Device ID byte 2
#define S29GL01GP_DEVID3    0x01    // offset 0x1e = Device ID byte 3
#define Spansion29GLxxx     0x7e01
#define S29WSxxx_DEVID      0x227e

//----------------------------------------------------------------------------
// Module level global data
extern UINT8    pFlashDeviceNumber[FLASH_PART_STRING_LENGTH];

//----------------------------------------------------------------------------
// Function Prototypes
static BOOLEAN
SpansionFlashEraseCommand   (
    IN volatile UINT8       *pBlockAddress
    );
static EFI_STATUS
SpansionFlashEraseCommandEx   (
    IN volatile UINT8       *pBlockAddress,
    IN UINT32               Length
    );
static VOID
SpansionFlashReadCommand    (
    IN  volatile UINT8      *pByteAddress,
    OUT UINT8               *Data,
    OUT UINT32              *Length
    );
static BOOLEAN
SpansionFlashProgramCommand (
    IN      volatile UINT8  *pByteAddress,
    IN      UINT8           *Data,
    IN OUT  UINT32          *Length
    );
static BOOLEAN
SpansionFlashIsEraseCompleted   (
    IN  volatile UINT8      *pBlockAddress,
    OUT BOOLEAN             *pError,
    OUT UINTN               *pStatus
    );
static BOOLEAN
SpansionFlashIsProgramCompleted (
    IN  volatile UINT8      *pByteAddress,
    IN  UINT8               *Byte, 
    IN  UINT32              Length,
    OUT BOOLEAN             *pError, 
    OUT UINTN               *pStatus
    );
static VOID 
SpansionFlashBlockWriteEnable   (
    IN volatile UINT8       *pBlockAddress
    );
static VOID
SpansionFlashBlockWriteDisable  (
    IN volatile UINT8       *pBlockAddress
    );
static VOID 
SpansionFlashDeviceWriteEnable  (
    VOID
    );
static VOID 
SpansionFlashDeviceWriteDisable (
    VOID
    );
static VOID 
SpansionFlashVirtualFixup   (
    IN EFI_RUNTIME_SERVICES *pRS
    );

extern FLASH_PART   *FlashAPI;
//----------------------------------------------------------------------------
// Module specific variables
FLASH_PART mStdSpansionFlash =
    {
        SpansionFlashReadCommand,
        SpansionFlashEraseCommand,
        SpansionFlashEraseCommandEx,
        SpansionFlashProgramCommand,
//-        SpansionFlashIsEraseCompleted,
//-        SpansionFlashIsProgramCompleted,
        SpansionFlashBlockWriteEnable,
        SpansionFlashBlockWriteDisable,
        SpansionFlashDeviceWriteEnable,
        SpansionFlashDeviceWriteDisable,
        SpansionFlashVirtualFixup,      // Spansion flash device virtual address 
                                        // fix up routine
        1,                              // Number of bytes to program to the
                                        // Flash part in each program command
        SECTOR_SIZE_128KB,              // Dummy value to hold place - only used in SPI
        NULL                            // Flash Part Number Pointer                   
    };
UINT8   gIsWordMode = TRUE;
//----------------------------------------------------------------------------
// Function definitions
/**
 * This function resets the flash part.
 *
 * @param pAddress Location to check the device status
 *                      
 */
static 
VOID 
SpansionResetFlash  ( 
    IN volatile UINT8*      pAddress 
)
{
    *pAddress = 0xf0;
}


/**
 * This function read data from the specified location
 *
 * @param pByteAddress Location where the data to be read
 * @param Byte Data buffer to be read
 * @param Length Number of bytes to read
 *                      
 */
static 
VOID 
SpansionFlashReadCommand    (
    IN  volatile UINT8*     pByteAddress, 
    OUT UINT8               *Byte, 
    OUT UINT32              *Length
)
{
    UINT32              dNumBytes = 0;
    
    // Changes for SMIFlash module label "4.6.3.6_SMIFLASH_12" or later.
    for ( dNumBytes = 0; dNumBytes < *Length ; dNumBytes++ )
        *( Byte + dNumBytes ) = *(UINT8*)((UINTN)pByteAddress + dNumBytes );
    *Length = 0;
    return ;
}


/**
 * This function verifies whether the command sent to the flash part has 
 * completed and returns the status of the command
 *
 * @param pAddress Location to check the device status
 *                      
 * @retval EFI_SUCCESS Operation Completed
 * @retval EFI_TIMEOUT Operation Timout
 * @retval EFI_DEVICE_ERROR Operation Error
 */
static 
EFI_STATUS 
WaitForPollingCompleted ( 
    IN volatile UINT8*      pAddress, 
    IN UINT8                bData 
)
{
    UINT8                   bStatus, bStatus2;
    UINT32                  dTimeout = FLASH_RETRIES * 0x10000;

    for ( ; dTimeout > 0; dTimeout-- ) {
        // Read DQ0~7
        bStatus = *(volatile UINT8*)pAddress;
        // Check DQ7 for Polling completed.
        if ((bStatus & BIT07) == (bData & BIT07)) {
            // Check DQ6 for Toggle bit completed.
            bStatus = *(volatile UINT8*)pAddress;
            bStatus2 = *(volatile UINT8*)pAddress;
            if((bStatus & BIT06) != (bStatus2 & BIT06)) continue;
            if((bStatus & BIT02) == (bStatus2 & BIT02)) return EFI_SUCCESS;
        }
        // Check DQ5(Write Timeout),DQ3(Erase Timeout) and DQ1(Abort) for
        // error occured.
        if (bStatus & BIT05) {
            // Check DQ6 for Toggle bit completed.
            bStatus = *(volatile UINT8*)pAddress;
            bStatus2 = *(volatile UINT8*)pAddress;
            if((bStatus & BIT06) != (bStatus2 & BIT06)) return EFI_TIMEOUT;
        }
    }
    return EFI_DEVICE_ERROR;
}


/**
 * This API function erases a block in the flash. Flash model specific code
 * will branch out from this routine
 *
 * @param pBlockAddress Block that need to be erased
 * @param Length Number of bytes that need to be erased
 *
 * @retval EFI_SUCCESS erase complete
 * @retval EFI_DEVICE_ERROR erase not completed
 */
static 
EFI_STATUS 
SpansionFlashEraseCommandEx   (
    IN volatile UINT8*      pBlockAddress,
    IN UINT32               Length
)
{
    UINT8               bFlashRetry;
    UINT16              i, wNumSectors;
    EFI_STATUS      	Status;

    if (Length % FlashAPI->FlashSectorSize) return EFI_INVALID_PARAMETER;
    wNumSectors = ( Length / FlashAPI->FlashSectorSize );    
    for (i = 0; i < wNumSectors; i++) {
        volatile UINT8 *pSectorAddress = pBlockAddress + (i * FlashAPI->FlashSectorSize);
        for ( bFlashRetry = 0; bFlashRetry < FLASH_RETRIES; bFlashRetry++ ) {
            SpansionResetFlash ( pSectorAddress );
            if (gIsWordMode == TRUE) {
                // x16 Word Mode - Erase
                *((UINT16 *)pSectorAddress + 0x555) = 0xaa;
                *((UINT16 *)pSectorAddress + 0x2aa) = 0x55;
                *((UINT16 *)pSectorAddress + 0x555) = 0x80;
                *((UINT16 *)pSectorAddress + 0x555) = 0xaa;
                *((UINT16 *)pSectorAddress + 0x2aa) = 0x55;
                *(UINT16 *)pSectorAddress = 0x30;
            } else {
                // x8 Byte Mode - Erase
                *(pSectorAddress + 0xaaa) = 0xaa;
                *(pSectorAddress + 0x555) = 0x55;
                *(pSectorAddress + 0xaaa) = 0x80;
                *(pSectorAddress + 0xaaa) = 0xaa;
                *(pSectorAddress + 0x555) = 0x55;
                *pSectorAddress = 0x30;
            }   
            Status = WaitForPollingCompleted ( pSectorAddress, 0xff );
            if (EFI_ERROR(Status)) SpansionResetFlash (pSectorAddress);
            SpansionFlashIsEraseCompleted (pSectorAddress, NULL, &Status);
            if (!EFI_ERROR(Status)) break;
        }
        if (EFI_ERROR(Status)) return Status;
    }
    return Status;
}


/**
 * This API function erases a block in the flash. Flash model specific code
 * will branch out from this routine
 *
 * @param pBlockAddress Block that need to be erased
 *                      
 */
static 
BOOLEAN 
SpansionFlashEraseCommand   (
    IN volatile UINT8*      pBlockAddress
)
{
    return (EFI_ERROR(SpansionFlashEraseCommandEx(pBlockAddress, FlashBlockSize))) ? FALSE : TRUE;
}


/**
 * This function verifies whether the block erase command is completed and 
 * returns the status of the command
 *
 * @param pBlockAddress Address to access flash part
 * @param pError True on error and false on success
 * @param pStatus Error status code (0 - Success)
 *                      
 * @retval TRUE erase complete
 * @retval FALSE erase not completed
 */
static 
BOOLEAN 
SpansionFlashIsEraseCompleted   (
    IN  volatile UINT8      *pBlockAddress,
    OUT BOOLEAN             *pError,
    OUT UINTN               *pStatus
)
{
    UINT32                      dNumBytes;

    for ( dNumBytes = 0; dNumBytes < FlashAPI->FlashSectorSize; dNumBytes++ ) {
        if ( *(volatile UINT8*)( pBlockAddress + dNumBytes ) != 0xFF ) {
            if ( pError ) *pError = TRUE;
            if ( pStatus ) *pStatus = EFI_DEVICE_ERROR;
            return FALSE;
        }
    }
    if ( pError ) *pError = FALSE;
    if ( pStatus ) *pStatus = EFI_SUCCESS;
    return TRUE;
}


/**
 * This function programs a byte data to the specified location
 *
 * @param pByteAddress Location where the data to be written
 * @param Byte Byte to be written
 * @param Length Number of bytes to write
 *                      
 */
static 
BOOLEAN 
SpansionFlashProgramCommand (
    IN volatile UINT8*      pByteAddress,
    IN UINT8                *Byte,
    IN OUT UINT32           *Length
)
{
    volatile UINT8* pBlockAddress = (volatile UINT8*)BLOCK(pByteAddress);
    volatile UINT8* pProgramAddress = pByteAddress;
    UINT8           bFlashRetry, bWordProgrammed = 0;
    UINT16          WordData = 0;
    EFI_STATUS      Status = EFI_SUCCESS;

    if (*Byte != *pByteAddress) {
        SpansionResetFlash ( pBlockAddress );
        for ( bFlashRetry = 0; bFlashRetry < FLASH_RETRIES; bFlashRetry++ ) {
            if (gIsWordMode == TRUE) {
                pProgramAddress = (UINT8*)((UINTN)pByteAddress & ~BIT00);
                // Get Word Data from ROM or input buffer. 
                WordData = bWordProgrammed = 0;
                if((UINTN)pByteAddress & 1) {
                    // Get Last Byte of current address
                    WordData = (*Byte) << 8;
                    WordData += *(pByteAddress - 1);
                } else {
                    if (*Length > 1) {
                        // Get Next Byte from Input Buffer
                        WordData = *(Byte + 1) << 8;
                        bWordProgrammed = 1;
                    } else {
                        // Get Next Byte of current address
                        WordData = *(pByteAddress + 1) << 8;
                    }    
                    WordData += *Byte;
                }
                // x16 Word Mode - Program
                *((UINT16*)pBlockAddress + 0x555) = 0xaa;
                *((UINT16*)pBlockAddress + 0x2aa) = 0x55;
                *((UINT16*)pBlockAddress + 0x555) = 0xa0;
                *((volatile UINT16*)pProgramAddress) = WordData;       
            } else {
                // x8 Byte Mode - Program
                *(pBlockAddress + 0xaaa) = 0xaa;
                *(pBlockAddress + 0x555) = 0x55;
                *(pBlockAddress + 0xaaa) = 0xa0;
                *pProgramAddress = *Byte;
            }    
            Status = WaitForPollingCompleted ( pProgramAddress, (UINT8) WordData);
            if (EFI_ERROR(Status)) SpansionResetFlash (pProgramAddress);
            Status = EFI_DEVICE_ERROR;
            if (gIsWordMode == FALSE) {
                if (*pProgramAddress == *Byte) Status = EFI_SUCCESS;
            } else {
                if (*(UINT16*)pProgramAddress == WordData) Status = EFI_SUCCESS;
            }                                              
            if (!EFI_ERROR(Status)) break;     
        }
    }
    if (bWordProgrammed) *Length = *Length - 1;
    *Length = *Length - 1;
    return (EFI_ERROR(Status)) ? FALSE : TRUE; 
}


/**
 * This function verifies whether the program (page or byte) command is 
 * completed and returns the status of the command
 *
 * @param pByteAddress Location of the program command
 * @param Byte Last data byte written
 * @param Length Number of bytes to write
 * @param pError True on error and false on success
 * @param pStatus Error status code (0 - Success)
 *                      
 * @retval TRUE If operation completed successfully 
 *              *pError = FALSE, *pStatus = 0
 * @retval FALSE If operation failed                    
 *              *pError = TRUE, *pStatus = error status
 */
static 
BOOLEAN 
SpansionFlashIsProgramCompleted (
    IN  volatile UINT8*         pByteAddress,
    IN  UINT8                   *Byte, 
    IN  UINT32                  Length,
    OUT BOOLEAN                 *pError,
    OUT UINTN                   *pStatus
)
{
    UINT32                      dNumBytes;
    UINT8                       bByte;

    for ( dNumBytes = 0; dNumBytes < Length; dNumBytes++ ) {
        bByte = * ( Byte + dNumBytes );
        if ( bByte != *(volatile UINT8*)( pByteAddress + dNumBytes ) ) {
            if ( pError ) *pError = TRUE;
            if ( pStatus ) *pStatus = EFI_DEVICE_ERROR;
            return FALSE;
        }
    }
    if ( pError ) *pError = FALSE;
    if ( pStatus ) *pStatus = EFI_SUCCESS;
    return TRUE;
}


/**
 * This function contains any flash specific code need to enable a 
 * particular flash block write
 *
 * @param pBlockAddress Location where the block register to be set
 *
 */
static 
VOID 
SpansionFlashBlockWriteEnable   (
    IN volatile UINT8*               pBlockAddress
)
{

}


/**
 * This function contains any flash specific code need to disable a 
 * particular flash block write
 *
 * @param pBlockAddress Location where the block register to be set
 *
 */
static 
VOID 
SpansionFlashBlockWriteDisable  (
    IN volatile UINT8*               pBlockAddress
)
{

}


/**
 * This function contains any flash specific code need to enable flash write
 *
 */
static 
VOID 
SpansionFlashDeviceWriteEnable (VOID)
{
    //We don't have to do anything here because
    //Flash Device is write enabled by the South Bridge driver
}


/**
 * This function contains any flash specific code need to disable flash write
 *
 */
static 
VOID 
SpansionFlashDeviceWriteDisable (VOID)
{
    //We don't have to do anything here because
    //we always keep flash device in the write enabled state
}


/**
 * This function will be invoked by the core to convert runtime pointers 
 * to virtual address
 *
 * @param pRS Pointer to runtime services
 *                      
 */
static
VOID
SpansionFlashVirtualFixup(
    IN EFI_RUNTIME_SERVICES *pRS
)
{

//  // Following is an example code for virtual address conversion
//  pRS->ConvertPointer(0, (VOID**)&FlashDeviceBase);

    return;
}


/**
 * This function identifies the supported flash parts and returns appropriate 
 * flash device API pointer. If flash part is not supported by this module it 
 * will return FALSE.
 *
 * @param pBlockAddress Block address of the flash part. Can be used to 
 *                      send ID command
 * @param Struct Pointer to hold the returned flash API
 *
 * @retval TRUE If flash part is supported, FlashApi contains routines to 
 *              handle the flash requests
 * @retval FALSE Flash part is not supported
 */
BOOLEAN 
mStdSpansionIdentify (
    IN  volatile UINT8*     pBlockAddress,
    OUT FLASH_PART          **Struct
)
{
    UINT16  MID, DID;
    volatile UINT16* pWordAddress = (volatile UINT16*)pBlockAddress;

    // Check x16 Word mode first..
    SpansionResetFlash( pBlockAddress );
    
    pWordAddress[0x555] = 0xaa;
    pWordAddress[0x2aa] = 0x55;
    pWordAddress[0x555] = 0x90;

    // Delay before read flash ID.
    MicroSecondDelay(10);

    MID = *pWordAddress;
    DID = *(pWordAddress + 1);
//-    DEBUG((EFI_D_ERROR, "In Flash: MID:%x, DID:%x, DID2:%x\n", MID, DID));

    SpansionResetFlash( pBlockAddress );

    // Check MID & DID1 for S29GL01G/512/256/128P support.
    if ((MID == S29GL01GP_MANID) && ((UINT8)DID == S29GL01GP_DEVID1)) {
        MemCpy ( pFlashDeviceNumber, "Spansion S29GL/WS128/256/512/01GP", 33 );
        mStdSpansionFlash.FlashPartNumber = pFlashDeviceNumber; 
        *Struct = &mStdSpansionFlash;
        return TRUE;
    }

    // Here check x8 Byte Mode..
    pBlockAddress[0xaaa] = 0xaa;
    pBlockAddress[0x555] = 0x55;
    pBlockAddress[0xaaa] = 0x90;

    // Delay before read flash ID.
    MicroSecondDelay(10);
    
    MID = (UINT16)*pBlockAddress;
    DID = (UINT16)*(pBlockAddress + 0x02);
//-    DEBUG((EFI_D_ERROR, "In Flash: MID:%x, DID:%x, DID2:%x\n", MID, DID));

    SpansionResetFlash( pBlockAddress );

    // Check MID & DID1 for S29GL01G/512/256/128P support.
    if (((UINT8)MID == S29GL01GP_MANID) && ((UINT8)DID == S29GL01GP_DEVID1)) {
        MemCpy ( pFlashDeviceNumber, "Spansion S29GL/WS128/256/512/01GP", 33 );
        mStdSpansionFlash.FlashPartNumber = pFlashDeviceNumber; 
        *Struct = &mStdSpansionFlash;
        gIsWordMode = FALSE;
        return TRUE;
    }

    return FALSE;
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
