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
#include <efi.h>
#include <AmiDxeLib.h>  //QUANTA_CHANGES
#include "FlashPart.h"
#include "Token.h"


//----------------------------------------------------------------------
// Local MACRO Definitions
#define UNLOCK 0
#define LOCK 1

//----------------------------------------------------------------------------
// Flash Part Specific Definitions

#define VENDOR_ID           0xbf // Intel Manufacturers ID
#define DEVICE_ID0          0x60 // 512K Firmware Hub
#define DEVICE_ID1          0x5a // 1 Meg Firmware Hub

#define READ_ARRAY_CMD      0xff
#define RD_STATUS_CMD       0x70
#define CLR_STATUS_CMD      0x50
#define ERASE_SET_CMD       0x20
#define ERASE_CNF_CMD       0xd0
#define PRG_SETUP_CMD       0x40

// Intel Status Register Bits
#define VPP_LOW             0x08
#define PROGRAM_FAIL        0x10
#define ERASE_FAIL          0x20
#define WSM_BUSY            0x80

// Intel Lock Commands
#define UNLOCK              0
#define WRITE_LOCK          1

//----------------------------------------------------------------------------
// Module level global data
extern FLASH_PART   *FlashAPI;
extern UINT8    pFlashDeviceNumber[FLASH_PART_STRING_LENGTH];

//----------------------------------------------------------------------------
// Function Prototypes
static BOOLEAN 
SstFlashEraseCommand    (
    IN volatile UINT8   *pBlockAddress
);
static EFI_STATUS 
SstFlashEraseCommandEx    (
    IN volatile UINT8   *pBlockAddress,
    IN UINT32           Length
);
static VOID 
SstFlashReadCommand (
    IN      volatile UINT8  *pByteAddress,
    IN OUT  UINT8           *Data, 
    IN OUT  UINT32          *Length
);
static BOOLEAN 
SstFlashProgramCommand  (
    IN      volatile UINT8  *pByteAddress,
    IN      UINT8           *Data, 
    IN OUT  UINT32          *Length
);
static BOOLEAN 
SstFlashIsEraseCompleted    (
    IN  volatile UINT8      *pBlockAddress,
    OUT BOOLEAN             *pError, 
    OUT UINTN               *pStatus
);
static BOOLEAN 
SstFlashIsProgramCompleted  (
    IN  volatile UINT8      *pByteAddress,
    IN  UINT8               *Byte, 
    IN  UINT32              Length,
    OUT BOOLEAN             *pError, 
    OUT UINTN               *pStatus
);
static VOID 
SstFlashBlockWriteEnable    (
    IN UINT8*               pBlockAddress
);
static VOID 
SstFlashBlockWriteDisable   (
    IN UINT8*               pBlockAddress
);
static VOID 
SstFlashDeviceWriteEnable   (
    VOID
);
static VOID 
SstFlashDeviceWriteDisable  (
    VOID
);
static VOID 
SstFlashVirtualFixup    (
    IN EFI_RUNTIME_SERVICES *pRS
);

//----------------------------------------------------------------------------
// Module specific variables
FLASH_PART mSstFirmwareHub =
    {
        SstFlashReadCommand,
        SstFlashEraseCommand,
        SstFlashEraseCommandEx,
        SstFlashProgramCommand,
//-        SstFlashIsEraseCompleted,
//-        SstFlashIsProgramCompleted,
        SstFlashBlockWriteEnable,
        SstFlashBlockWriteDisable,
        SstFlashDeviceWriteEnable,
        SstFlashDeviceWriteDisable,
        SstFlashVirtualFixup,   // SST flash device virtual address 
                                // fix up routine
        1,                      // Number of bytes to program to the
                                // Flash part in each program command
        SECTOR_SIZE_4KB,        // Dummy value to hold place - only used in SPI
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
SstFwhResetFlash    ( 
    IN volatile UINT8*      pAddress 
)
{
    pAddress[0x5555] = 0xaa;
    pAddress[0x2aaa] = 0x55;
    pAddress[0x5555] = 0xf0;
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
SstFlashReadCommand (
    IN      volatile UINT8* pByteAddress, 
    IN OUT  UINT8           *Byte, 
    IN OUT  UINT32          *Length
)
{
    UINT32              dNumBytes = 0;
    
    // Changes for SMIFlash module label "4.6.3.6_SMIFLASH_12" or later.
    for ( dNumBytes = 0; dNumBytes < *Length ; dNumBytes++ )
        *( Byte + dNumBytes ) = *(UINT8*)((UINT32)pByteAddress + dNumBytes );
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
 */
static 
EFI_STATUS 
SstFwhWaitForOperationCompleted (
    IN volatile UINT8*      pAddress
)
{
    UINT8                   bData1, bData2;
    UINT32                  dTimeout = FLASH_RETRIES * 0x10000;

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
SstFlashEraseCommandEx    (
    IN volatile UINT8*      pBlockAddress,
    IN UINT32               Length
)
{
    UINT8               bFlashRetry;
    UINT16              i;
    EFI_STATUS          Status;

    if (Length % FlashAPI->FlashSectorSize) return EFI_INVALID_PARAMETER;
    for (i = 0; i < (Length / FlashAPI->FlashSectorSize); i++) {
        volatile UINT8 *pSectorAddress = pBlockAddress + (i * FlashAPI->FlashSectorSize);
        SstFwhResetFlash ( pSectorAddress );
        for ( bFlashRetry = 0; bFlashRetry < FLASH_RETRIES; bFlashRetry++ ) {
            pSectorAddress[0x5555] = 0xaa;
            pSectorAddress[0x2aaa] = 0x55;
            pSectorAddress[0x5555] = 0x80;
            pSectorAddress[0x5555] = 0xaa;
            pSectorAddress[0x2aaa] = 0x55;
            *pSectorAddress        = 0x50;
            if (EFI_ERROR(SstFwhWaitForOperationCompleted (pSectorAddress))) {
                SstFwhResetFlash (pSectorAddress);
                continue;
            }    
            SstFwhResetFlash (pSectorAddress);
            SstFlashIsEraseCompleted (pSectorAddress, NULL, &Status);
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
SstFlashEraseCommand    (
    IN volatile UINT8*      pBlockAddress
)
{
    return (EFI_ERROR(SstFlashEraseCommandEx(pBlockAddress, FlashBlockSize))) ? FALSE : TRUE;
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
SstFlashIsEraseCompleted    (
    IN  volatile UINT8      *pBlockAddress,
    OUT BOOLEAN             *pError,
    OUT UINTN               *pStatus)
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
SstFlashProgramCommand  (
    IN      volatile UINT8* pByteAddress,
    IN      UINT8           *Byte,
    IN OUT  UINT32          *Length
)
{
    volatile UINT8* pBlockAddress = (volatile UINT8*)BLOCK(pByteAddress);
    UINT8           bFlashRetry;

    if ( *Byte != *pByteAddress ) {
        SstFwhResetFlash ( pBlockAddress );
        for ( bFlashRetry = 0; bFlashRetry < FLASH_RETRIES; bFlashRetry++ ) {
            // Program Command Sequence
            pBlockAddress[0x5555] = 0xaa;
            pBlockAddress[0x2aaa] = 0x55;
            pBlockAddress[0x5555] = 0xa0;
            *pByteAddress = *Byte;
            if (EFI_ERROR(SstFwhWaitForOperationCompleted (pByteAddress))) {
                SstFwhResetFlash ( pBlockAddress );
                continue;
            }
            SstFwhResetFlash ( pBlockAddress );
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
SstFlashIsProgramCompleted  (
    IN  volatile UINT8*     pByteAddress,
    IN  UINT8               *Byte, 
    IN  UINT32              Length,
    OUT BOOLEAN             *pError,
    OUT UINTN               *pStatus
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
 * This function use to set the Lock register for specified block
 *
 * @param pBlockAddress Location where the block register to be set
 * @param LockState Value to use to set the Lock register for the block
 *                  defined by pBlockAddress
 *                      
 */
static 
void 
SetBlockLock    (
    IN volatile UINT8*  pBlockAddress,  
    IN UINT8            LockState
)
{
    // Update the block lock register
    ((UINT8*)((UINTN)pBlockAddress - FlashDeviceBase + \
                                        FwhFeatureSpaceBase))[2] = LockState;
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
SstFlashBlockWriteEnable    (
    IN UINT8*               pBlockAddress
)
{
    SetBlockLock(pBlockAddress, UNLOCK);
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
SstFlashBlockWriteDisable   (
    IN UINT8*               pBlockAddress
)
{
    SetBlockLock(pBlockAddress, WRITE_LOCK);
}


/**
 * This function contains any flash specific code need to enable flash write
 *
 */
static 
VOID 
SstFlashDeviceWriteEnable (VOID)
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
SstFlashDeviceWriteDisable (VOID)
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
SstFlashVirtualFixup(
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
mSstFwhIdentify (
    IN  volatile UINT8*     pBlockAddress,
    OUT FLASH_PART          **Struct
)
{
    UINT8   VID;
    UINT8   DID;

    SstFwhResetFlash( pBlockAddress );

    pBlockAddress[0x5555] = 0xaa;
    pBlockAddress[0x2aaa] = 0x55;
    pBlockAddress[0x5555] = 0x90;

    VID = *pBlockAddress;
    DID = *(pBlockAddress + 1);

    SstFwhResetFlash( pBlockAddress );

    // check for valid ID
    if ((VID == VENDOR_ID) && ((DID == DEVICE_ID0) || (DID == DEVICE_ID1)))
    {
        MemCpy ( pFlashDeviceNumber, "SST 49LF004/008", 15 );
        mSstFirmwareHub.FlashPartNumber = pFlashDeviceNumber;
        *Struct = &mSstFirmwareHub;
        return TRUE;
    }
    else
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
