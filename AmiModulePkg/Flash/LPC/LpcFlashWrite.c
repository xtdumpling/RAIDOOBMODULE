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

//----------------------------------------------------------------------------
// Includes
#include <Efi.h>
#include <AmiDxeLib.h>
#include <FlashPart.h>
#include <Token.h>

//----------------------------------------------------------------------------
// Flash part related tokens
#define SST_MANUFACTURER_ID     0xbf
#define SST49LF020A_DEVICE_ID   0x52
#define SST49LF080A_DEVICE_ID   0x5b
#define SST49LF020A             0x52bf
#define SST49LF080A             0x5bbf
#define WINBOND_W39V080A        0xd0da

#define COMMAND_SEQUENCE_1      0xaa
#define COMMAND_SEQUENCE_2      0x55
#define BYTE_PROGRAM_CMD        0xa0
#define BLOCK_ERASE_CMD_1       0x80
#define BLOCK_ERASE_CMD_2       0x30
#define LPC_ID_CMD              0x90
#define LPC_RESET_CMD           0xf0

#define UNLOCK                  0
#define WRITE_LOCK              1

//----------------------------------------------------------------------------
// Module level global data
extern UINT8    pFlashDeviceNumber[FLASH_PART_STRING_LENGTH];
extern FLASH_PART   *FlashAPI;

//----------------------------------------------------------------------------
// Function prototypes
static EFI_STATUS
InitSstAndCheckId   (
    IN volatile UINT8       *pBlockAddress
    );
static BOOLEAN
LpcFlashEraseCommand    (
    IN volatile UINT8       *pBlockAddress
    );
static EFI_STATUS
LpcFlashEraseCommandEx    (
    IN volatile UINT8       *pBlockAddress,
    IN UINT32               Length
    );
static VOID
LpcFlashReadCommand (
    IN      volatile UINT8  *pByteAddress,
    IN OUT  UINT8           *Data,
    IN OUT  UINT32          *Length
    );
static BOOLEAN
LpcFlashProgramCommand  (
    IN      volatile UINT8  *pByteAddress,
    IN      UINT8           *Data,
    IN OUT  UINT32          *Length
    );
static BOOLEAN
LpcFlashIsEraseCompleted    (
    IN  volatile UINT8      *pBlockAddress,
    OUT BOOLEAN             *pError,
    OUT UINTN               *pStatus
    );
static BOOLEAN
LpcFlashIsProgramCompleted  (
    IN  volatile UINT8      *pByteAddress,
    IN  UINT8               *Byte,
    IN  UINT32              Length,
    OUT BOOLEAN             *pError,
    OUT UINTN               *pStatus
    );
static VOID
LpcFlashBlockWriteEnable    (
    IN UINT8                *pBlockAddress
    );
static VOID
LpcFlashBlockWriteDisable   (
    IN UINT8                *pBlockAddress
    );
static VOID
LpcFlashDeviceWriteEnable   (
    VOID
    );
static VOID
LpcFlashDeviceWriteDisable  (
    VOID
    );
static EFI_STATUS
LpcOperationCompleted   (
    IN volatile UINT8       *pAddress
    );//,
static VOID
LpcResetFlash   (
    IN volatile UINT8       *pBlockAddress
    );
static VOID
LpcFlashVirtualFixup    (
    IN EFI_RUNTIME_SERVICES *pRS
    );
UINT8   gbLpcFlashEraseCommand;
//----------------------------------------------------------------------------
// Module specific variables
FLASH_PART mCommonLpcFlash = {
    LpcFlashReadCommand,            // dummy byte read command
    LpcFlashEraseCommand,           // block erase command
    LpcFlashEraseCommandEx,           // block erase command
    LpcFlashProgramCommand,         // byte program command
//-    LpcFlashIsEraseCompleted,       // erase complete identifier command
//-    LpcFlashIsProgramCompleted,     // program complete identifier command
    LpcFlashBlockWriteEnable,       // block write enable command
    LpcFlashBlockWriteDisable,      // block write disable command
    LpcFlashDeviceWriteEnable,      // write enable command
    LpcFlashDeviceWriteDisable,     // write disable command
    LpcFlashVirtualFixup,           // virtual address fix up routine
    1,                              // Number of bytes to program to the
                                    // Flash part in each program command
    SECTOR_SIZE_4KB,                // Dummy value to hold place - only used in SPI
    NULL                            // Flash Part Number Pointer                   
    };

//----------------------------------------------------------------------------
// Function Definitions

/**
 * This function identifies the supported flash parts and returns appropriate 
 * flash device API pointer. If flash part is not supported by this module it 
 * will return FALSE.
 *
 * @param pBlockAddress Block address of the flash part. Can be used to 
 *                      send ID command
 * @param FlashApi Pointer to hold the returned flash API
 *
 * @retval TRUE If flash part is supported, FlashApi contains routines to 
 *              handle the flash requests
 * @retval FALSE Flash part is not supported
 */
BOOLEAN
mLpcFlashIdentify   (
    IN  volatile UINT8      *pBlockAddress,
    OUT FLASH_PART          **FlashApi
)
{
    EFI_STATUS Status;

    Status = InitSstAndCheckId( pBlockAddress );
    if ( Status != EFI_SUCCESS  ) return FALSE;
    else {
        mCommonLpcFlash.FlashPartNumber = pFlashDeviceNumber;
        *FlashApi = &mCommonLpcFlash;
    }    
    return TRUE;
}


/**
 * This function read data from the specified location
 *
 * @param pByteAddress Location where the data to be read
 * @param Byte Data buffer to be read
 * @param Length Number of bytes to read
 *                      
 */
VOID
LpcFlashReadCommand (
    IN      volatile UINT8  *pByteAddress,
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
 * This API function erases a block in the flash. Flash model specific code
 * will branch out from this routine
 *
 * @param pBlockAddress Block that need to be erased
 * @param Length Number of bytes that need to be erased
 *
 * @retval EFI_SUCCESS erase complete
 * @retval EFI_DEVICE_ERROR erase not completed
 */
EFI_STATUS
LpcFlashEraseCommandEx    (
    IN volatile UINT8       *pBlockAddress,
    IN UINT32               Length

)
{
    EFI_STATUS          Status;
    UINT8               bFlashRetry;
    UINT16              i, wNumSectors;

    if (Length % FlashAPI->FlashSectorSize) return EFI_INVALID_PARAMETER;
    wNumSectors = Length / FlashAPI->FlashSectorSize;    
    for (i = 0; i < wNumSectors; i++) {
        volatile UINT8 *pSectorAddress = pBlockAddress + (i * FlashAPI->FlashSectorSize);
        LpcResetFlash ( pSectorAddress );
        for ( bFlashRetry = 0; bFlashRetry < FLASH_RETRIES; bFlashRetry++ ) {
            pSectorAddress[0x5555] = COMMAND_SEQUENCE_1;
            pSectorAddress[0x2aaa] = COMMAND_SEQUENCE_2;
            pSectorAddress[0x5555] = BLOCK_ERASE_CMD_1;
            pSectorAddress[0x5555] = COMMAND_SEQUENCE_1;
            pSectorAddress[0x2aaa] = COMMAND_SEQUENCE_2;
            *pSectorAddress = gbLpcFlashEraseCommand;
            Status = LpcOperationCompleted( pSectorAddress );
            LpcResetFlash ( pSectorAddress );
            LpcFlashIsEraseCompleted (pSectorAddress, NULL, &Status);
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
BOOLEAN
LpcFlashEraseCommand    (
    IN volatile UINT8       *pBlockAddress
)
{
    return (EFI_ERROR(LpcFlashEraseCommandEx(pBlockAddress, FlashBlockSize))) ? FALSE : TRUE;
}

/**
 * This function verifies whether the command sent to the flash part has 
 * completed and returns the status of the command
 *
 * @param pAddress Location to check the device status
 *                      
 * @retval EFI_SUCCESS Operation Completed
 * @retval EFI_TIMEOUT Operation Timout
 */
EFI_STATUS
LpcOperationCompleted   (
    IN volatile UINT8       *pAddress
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
 * This function programs a byte data to the specified location
 *
 * @param pByteAddress Location where the data to be written
 * @param Data Byte to be written
 * @param Length Number of bytes to write
 *                      
 */
BOOLEAN
LpcFlashProgramCommand  (
    IN      volatile UINT8  *pByteAddress,
    IN      UINT8           *Data,
    IN OUT  UINT32          *Length
)
{
    volatile UINT8* pBlockAddress = (volatile UINT8 *)BLOCK(pByteAddress);
    UINT8           bFlashRetry;
    EFI_STATUS      Status;

    if (*Data != *pByteAddress)
    {
        LpcResetFlash ( pBlockAddress );
        for ( bFlashRetry = 0; bFlashRetry < FLASH_RETRIES; bFlashRetry++ ) {
            // Program Command Sequence
            pBlockAddress[0x5555] = COMMAND_SEQUENCE_1;
            pBlockAddress[0x2aaa] = COMMAND_SEQUENCE_2;
            pBlockAddress[0x5555] = BYTE_PROGRAM_CMD;
            *pByteAddress=*Data;
            if (EFI_ERROR(LpcOperationCompleted (pByteAddress))) {
                LpcResetFlash ( pBlockAddress );
                continue;
            }    
            Status = (*pByteAddress != *Data) ? EFI_DEVICE_ERROR : EFI_SUCCESS;
            if (!EFI_ERROR(Status)) break;
        }
        LpcResetFlash ( pBlockAddress );
    }
    *Length = *Length - 1;
    return (EFI_ERROR(Status)) ? FALSE : TRUE;
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
BOOLEAN
LpcFlashIsEraseCompleted    (
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
BOOLEAN
LpcFlashIsProgramCompleted  (
    IN  volatile UINT8      *pByteAddress,
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
    IN volatile UINT8*      pBlockAddress,
    IN UINT8                LockState
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
VOID
LpcFlashBlockWriteEnable    (
    IN UINT8                *pBlockAddress
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
VOID
LpcFlashBlockWriteDisable   (
    IN UINT8                *pBlockAddress
)
{
    SetBlockLock(pBlockAddress, WRITE_LOCK);
}


/**
 * This function contains any flash specific code need to enable flash write
 *
 */
VOID
LpcFlashDeviceWriteEnable (VOID)
{
    // Flash device does not support this feature
}


/**
 * This function contains any flash specific code need to disable flash write
 *
 */
VOID
LpcFlashDeviceWriteDisable (VOID)
{
    // Flash device does not support this feature
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
LpcFlashVirtualFixup    (
    IN EFI_RUNTIME_SERVICES *pRS
)
{
//  // Following is an example code for virtual address conversion
//  pRS->ConvertPointer(0, (VOID**)&FlashDeviceBase);

    return;
}


/**
 * This function resets the flash part.
 *
 * @param pAddress Location to check the device status
 *                      
 */
static
VOID
LpcResetFlash   (
    IN volatile UINT8*      pBlockAddress)
{
    pBlockAddress[0x5555] = COMMAND_SEQUENCE_1;
    pBlockAddress[0x2aaa] = COMMAND_SEQUENCE_2;
    pBlockAddress[0x5555] = LPC_RESET_CMD;
}


/**
 * This function identifies the supported flash parts.
 *
 * @param pBlockAddress Block address of the flash part. Can be used to 
 *                      send ID command
 *
 * @retval EFI_SUCCESS If flash part is supported
 * @retval EFI_NOT_FOUND Flash part is not supported
 */
EFI_STATUS
InitSstAndCheckId   (
    IN volatile UINT8*      pBlockAddress
)
{
    UINT8               ManfactureID, DeviceID;

    LpcResetFlash(pBlockAddress);
    pBlockAddress[0x5555] = COMMAND_SEQUENCE_1;
    pBlockAddress[0x2aaa] = COMMAND_SEQUENCE_2;
    pBlockAddress[0x5555] = LPC_ID_CMD;
    ManfactureID = *pBlockAddress;
    DeviceID = *(pBlockAddress + 1);
    LpcResetFlash(pBlockAddress);
    switch ( ( DeviceID << 8 ) + ManfactureID ) {
        case SST49LF020A :
        case SST49LF080A :
            gbLpcFlashEraseCommand = 0x50;
            MemCpy ( pFlashDeviceNumber, "SST 49LF020/080A", 16 );
            break;
        case WINBOND_W39V080A :
            gbLpcFlashEraseCommand = 0x30;
            MemCpy ( pFlashDeviceNumber, "Winbond W39V080A", 16 );
            break;
        default :
            return EFI_NOT_FOUND;
    }
    return EFI_SUCCESS;
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