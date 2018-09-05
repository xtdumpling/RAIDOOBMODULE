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
#include <AmiDxeLib.h>
#include <Library/TimerLib.h>
#include "FlashPart.h"
#include "Token.h"


//----------------------------------------------------------------------
// Local MACRO Definitions
#define UNLOCK              0
#define LOCK                1
#define SECTOR_SIZE_8KB     0x2000          // 8kbytes sector size
#define SECTOR_SIZE_64KB    0x10000        // 64kBytes sector size

//----------------------------------------------------------------------------
// Flash Part Specific Definitions

#define Mx29LV640DB     0xcbc2
#define Mx29LV640DT     0xc9c2
#define Sst39VF1601     0x4bbf
#define Sst39VF1602     0x4abf
#define Sst39VF1681     0xc8bf
#define Sst39VF1682     0xc9bf
#define Sst39VF3201     0x5bbf
#define Sst39VF3202     0x5abf
#define Stm29W160ET     0xc420
#define Stm29W160EB     0x4920

//----------------------------------------------------------------------------
// Module level global data
extern UINT8    pFlashDeviceNumber[FLASH_PART_STRING_LENGTH];

//----------------------------------------------------------------------------
// Function Prototypes
static BOOLEAN
MxicFlashEraseCommand   (
    IN volatile UINT8       *pBlockAddress
    );
static EFI_STATUS
MxicFlashEraseCommandEx   (
    IN volatile UINT8       *pBlockAddress,
    IN UINT32               Length
    );
static VOID
MxicFlashReadCommand    (
    IN  volatile UINT8      *pByteAddress,
    OUT UINT8               *Data,
    OUT UINT32              *Length
    );
static BOOLEAN
MxicFlashProgramCommand (
    IN      volatile UINT8  *pByteAddress,
    IN      UINT8           *Data,
    IN OUT  UINT32          *Length
    );
static BOOLEAN
MxicFlashIsEraseCompleted   (
    IN  volatile UINT8      *pBlockAddress,
    OUT BOOLEAN             *pError,
    OUT UINTN               *pStatus
    );
static BOOLEAN
MxicFlashIsProgramCompleted (
    IN  volatile UINT8      *pByteAddress,
    IN  UINT8               *Byte, 
    IN  UINT32              Length,
    OUT BOOLEAN             *pError, 
    OUT UINTN               *pStatus
    );
static VOID 
MxicFlashBlockWriteEnable   (
    IN volatile  UINT8      *pBlockAddress
    );
static VOID
MxicFlashBlockWriteDisable  (
    IN volatile  UINT8      *pBlockAddress
    );
static VOID 
MxicFlashDeviceWriteEnable  (
    VOID
    );
static VOID 
MxicFlashDeviceWriteDisable (
    VOID
    );
static VOID 
MxicFlashVirtualFixup   (
    IN EFI_RUNTIME_SERVICES *pRS
    );

extern FLASH_PART   *FlashAPI;
//----------------------------------------------------------------------------
// Module specific variables
FLASH_PART mStdMxicFlash =
    {
        MxicFlashReadCommand,
        MxicFlashEraseCommand,
        MxicFlashEraseCommandEx,
        MxicFlashProgramCommand,
//-        MxicFlashIsEraseCompleted,
//-        MxicFlashIsProgramCompleted,
        MxicFlashBlockWriteEnable,
        MxicFlashBlockWriteDisable,
        MxicFlashDeviceWriteEnable,
        MxicFlashDeviceWriteDisable,
        MxicFlashVirtualFixup,  // Mxic flash device virtual address 
                                // fix up routine
        1,                      // Number of bytes to program to the
                                // Flash part in each program command
        SECTOR_SIZE_64KB,       // Dummy value to hold place - only used in SPI
        NULL                    // Flash Part Number Pointer                   
    };

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
MxicWriteBufferAbortReset   (
    IN volatile UINT8*      pAddress 
)
{
    pAddress[0xaaa] = 0xaa;
    pAddress[0x555] = 0x55;
    pAddress[0xaaa] = 0xf0;
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
MxicFlashReadCommand    (
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
 * This function verifies whether the command sent to the FWH part has 
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
MxicStdWaitForOperationCompleted (
    IN volatile UINT8*      pAddress
)
{
    UINT8                   bData1, bData2;
    UINT32                  dTimeout = FLASH_RETRIES * 0x1000000;

    do {
        bData1 = (*pAddress) & 0x40;
        bData2 = (*pAddress) & 0x40;
        if ( bData1 == bData2 ) return EFI_SUCCESS;
        dTimeout--;
    } while ( dTimeout != 0 );                 // Check for Bit 6 Toggle

    return EFI_TIMEOUT;
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
MxicFlashEraseCommandEx   (
    IN volatile UINT8*      pBlockAddress,
    IN UINT32               Length
)
{
    EFI_STATUS          Status;
    UINT8               bFlashRetry;
    UINT16              wNumSectors, wSector;
    UINT32              i;
    volatile UINT8*     pSectorAddr;

    if (Length % FlashAPI->FlashSectorSize) return EFI_INVALID_PARAMETER;
    MxicWriteBufferAbortReset ( pBlockAddress );
    wNumSectors = ( Length / FlashAPI->FlashSectorSize );    
    for ( pSectorAddr = NULL, wSector = 0; wSector < wNumSectors; wSector++ ) {
        pSectorAddr = pBlockAddress + wSector * FlashAPI->FlashSectorSize;
        for ( i = 0; i < FlashAPI->FlashSectorSize; i++ ) {
            if ( *(pSectorAddr + i) != 0xFF ) break;
        }
        if ( i == FlashAPI->FlashSectorSize ) break;
        for ( bFlashRetry = 0; bFlashRetry < FLASH_RETRIES; bFlashRetry++ ) {
            pSectorAddr[0xaaa] = 0xaa;
            pSectorAddr[0x555] = 0x55;
            pSectorAddr[0xaaa] = 0x80;
            pSectorAddr[0xaaa] = 0xaa;
            pSectorAddr[0x555] = 0x55;
            *pSectorAddr = 0x30;
            Status = MxicStdWaitForOperationCompleted ( pSectorAddr );
            MxicWriteBufferAbortReset ( pSectorAddr );
            if (!EFI_ERROR(Status)) continue;
            MxicFlashIsEraseCompleted(pSectorAddr, NULL, &Status);
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
MxicFlashEraseCommand   (
    IN volatile UINT8*      pBlockAddress
)
{
    return (EFI_ERROR(MxicFlashEraseCommandEx (pBlockAddress, FlashBlockSize))) ? FALSE : TRUE;
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
MxicFlashIsEraseCompleted   (
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
MxicFlashProgramCommand (
    IN volatile UINT8*      pByteAddress,
    IN UINT8                *Byte,
    IN OUT UINT32           *Length
)
{
    volatile UINT8* pBlockAddress = (volatile UINT8*)BLOCK(pByteAddress);
    UINT8           bFlashRetry;


    if ( *Byte != *pByteAddress ) {
        MxicWriteBufferAbortReset ( pBlockAddress );
        for ( bFlashRetry = 0; bFlashRetry < FLASH_RETRIES; bFlashRetry++ ) {
            // Program Command Sequence
            pBlockAddress[0xaaa] = 0xaa;
            pBlockAddress[0x555] = 0x55;
            pBlockAddress[0xaaa] = 0xa0;
            *pByteAddress = *Byte;
            if (EFI_ERROR(MxicStdWaitForOperationCompleted (pBlockAddress))) continue;
            if (*pByteAddress == *Byte) break;
            if (FLASH_RETRIES == (bFlashRetry + 1)) return FALSE;    
        }
    }
    *Length = *Length - 1;
    return TRUE;
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
MxicFlashIsProgramCompleted (
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
MxicFlashBlockWriteEnable   (
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
MxicFlashBlockWriteDisable  (
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
MxicFlashDeviceWriteEnable (VOID)
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
MxicFlashDeviceWriteDisable (VOID)
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
MxicFlashVirtualFixup(
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
mStdMxicIdentify (
    IN  volatile UINT8*     pBlockAddress,
    OUT FLASH_PART          **Struct
)
{
    UINT8   bVenID, bDevID_1, bDevID_2;

    MxicWriteBufferAbortReset( pBlockAddress );

    pBlockAddress[0xaaa] = 0xaa;
    pBlockAddress[0x555] = 0x55;
    pBlockAddress[0xaaa] = 0x90;

    // Delay before read flash ID.
    MicroSecondDelay(10);
    
    bVenID = *pBlockAddress;
    bDevID_1 = *(pBlockAddress + 0x01);
    bDevID_2 = *(pBlockAddress + 0x02);

    MxicWriteBufferAbortReset( pBlockAddress );

    // Check for SST39VF1681/1682 support.
    switch ((bDevID_1 << 8) + bVenID) {
        case Sst39VF1601:
        case Sst39VF1602:
        case Sst39VF1681:
        case Sst39VF1682:
        case Sst39VF3201:
        case Sst39VF3202:
            MemCpy ( pFlashDeviceNumber, "SST 39VF1681/1682/3201/3202", 27 );
            mStdMxicFlash.FlashPartNumber = pFlashDeviceNumber; 
            *Struct = &mStdMxicFlash;
            return TRUE;             
        case Stm29W160ET:
        case Stm29W160EB:
            MemCpy ( pFlashDeviceNumber, "ST M29W160B/T", 13 );
            mStdMxicFlash.FlashSectorSize = SECTOR_SIZE_8KB;
            mStdMxicFlash.FlashPartNumber = pFlashDeviceNumber; 
            *Struct = &mStdMxicFlash;
            return TRUE;             
    }
    // Check for Mxic 29LV640DB/T support.
    switch ((bDevID_2 << 8) + bVenID) {
        case Mx29LV640DB:
        case Mx29LV640DT:
            MemCpy ( pFlashDeviceNumber, "MXIC 29LV640B/T", 15 );
            mStdMxicFlash.FlashPartNumber = pFlashDeviceNumber; 
            *Struct = &mStdMxicFlash;
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
