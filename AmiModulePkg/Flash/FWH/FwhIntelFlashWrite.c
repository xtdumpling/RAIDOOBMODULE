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
#include <FlashPart.h>
#include <AmiDxeLib.h>
#include <Library/TimerLib.h>
#include "Token.h"


//----------------------------------------------------------------------
// define local MACROS

//Flash Part Specific Tokens
#define VENDOR_ID               0x89    // Intel Manufacturers ID
#define DEVICE_ID0              0xad    // 512K Firmware Hub
#define DEVICE_ID1              0xac    // 1 Meg Firmware Hub
#define Intel_82802AB           0xad89  // Intel 4Mb Firmware Hub
#define Intel_82802AC           0xac89  // Intel 8Mb Firmware Hub

#define VENDOR_ID2              0x1f    // Atmel Manufacturers ID
#define DEVICE_ID3              0xe1    // AT49LW080
#define ATMEL_49LW080           0xe11f  // ATMEL 8Mb Firmware Hub

#define VENDOR_ID_ST            0x20    // ST Manufacturers ID
#define DEVICE_ID_ST0           0x2d    // M50FW080
#define DEVICE_ID_ST1           0x2e    // M50FW016 - 2MB
#define STM_50FW080             0x2d20  // STM 8Mb Firmware Hub
#define STM_50FW016             0x2e20  // STM 16Mb Firmware Hub

#define SST_49LF016             0x5cbf  // SST 16Mb Firmware Hub

#define READ_ARRAY_CMD          0xff
#define RD_STATUS_CMD           0x70
#define CLR_STATUS_CMD          0x50
#define ERASE_SET_CMD           0x20
#define ERASE_CNF_CMD           0xd0
#define PRG_SETUP_CMD           0x40
#define SECTOR_ERASE_SET_CMD    0x30 // SST 49LF016 only

#define RD_ID_CODE              0x90

// Intel Status Register Bits
#define VPP_LOW                 0x08
#define PROGRAM_FAIL            0x10
#define ERASE_FAIL              0x20
#define WSM_BUSY                0x80

// Intel Lock Commands
#define UNLOCK                  0
#define WRITE_LOCK              1

#define SECTOR_SIZE_4KB         0x1000  // Common 4kBytes sector size
#define SECTOR_SIZE_64KB        0x10000  // Common 64kBytes sector size

//----------------------------------------------------------------------------
// Module level global data
extern FLASH_PART   *FlashAPI;
extern  UINT8       pFlashDeviceNumber[FLASH_PART_STRING_LENGTH];

//----------------------------------------------------------------------------
// Function Prototypes
static BOOLEAN
IntelFlashEraseCommand  (
    IN volatile UINT8   *pBlockAddress
    );
static EFI_STATUS
IntelFlashEraseCommandEx  (
    IN volatile UINT8   *pBlockAddress,
    IN UINT32           Length
    );
static VOID
IntelFlashReadCommand   (
    IN     volatile UINT8   *pByteAddress,
    IN OUT UINT8            *Data,
    IN OUT UINT32           *Length
    );
static BOOLEAN
IntelFlashProgramCommand    (
    IN     volatile UINT8   *pByteAddress,
    IN     UINT8            *Data,
    IN OUT UINT32           *Length
    );
static BOOLEAN
IntelFlashIsEraseCompleted  (
    IN     volatile UINT8   *pBlockAddress,
    OUT    BOOLEAN          *pError,
    OUT    UINTN            *pStatus
    );
static BOOLEAN
IntelFlashIsProgramCompleted    (
    IN  volatile UINT8      *pByteAddress,
    IN  UINT8               *Byte,
    IN  UINT32              Length,
    OUT BOOLEAN             *pError,
    OUT UINTN               *pStatus
    );
static VOID
IntelFlashBlockWriteEnable  (
    IN UINT8*       pBlockAddress
    );
static VOID
IntelFlashBlockWriteDisable (
    IN UINT8*       pBlockAddress
    );
static VOID
IntelFlashDeviceWriteEnable (
    VOID
    );
static VOID
IntelFlashDeviceWriteDisable    (
    VOID
    );
static VOID
IntelFlashVirtualFixup(IN EFI_RUNTIME_SERVICES *pRS);


//----------------------------------------------------------------------------
// Local Variable definitions
UINT8   gAtmel = 0;
UINT8   gbEraseSetCmd = ERASE_SET_CMD;
// Flash Part Data structure fo the intel 82802ACC
FLASH_PART mIntelFirmwareHub =
    {
        IntelFlashReadCommand,
        IntelFlashEraseCommand,
        IntelFlashEraseCommandEx,
        IntelFlashProgramCommand,
//-        IntelFlashIsEraseCompleted,
//-        IntelFlashIsProgramCompleted,
        IntelFlashBlockWriteEnable,
        IntelFlashBlockWriteDisable,
        IntelFlashDeviceWriteEnable,
        IntelFlashDeviceWriteDisable,
        IntelFlashVirtualFixup,
        1,                  // Number of bytes to program to the
                            // Flash part in each program command
        SECTOR_SIZE_64KB,   // Dummy value to hold place - only used in SPI
        NULL                // Flash Part Number Pointer
    };

//----------------------------------------------------------------------------
// Function Definitions
/**
 * The function delays for flash operation.
 *
 */
static VOID IntelFwhCmdDelay (VOID)
{
//-    IoWrite8 ( 0xeb, 0x55 );
//-    IoWrite8 ( 0xeb, 0xaa );
    MicroSecondDelay(10);
}


/**
 * This function resets the Intel flash part.
 *
 * @param pAddress Location to check the device status
 *                      
 */
static
VOID
IntelFwhResetFlash  (
    IN volatile UINT8*      pAddress
)
{
    *pAddress = READ_ARRAY_CMD;// Return to read mode
    IntelFwhCmdDelay ();
    *pAddress = CLR_STATUS_CMD;// clear status
    IntelFwhCmdDelay ();
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
IntelFwhOperationCompleted  (
    IN volatile UINT8*      pAddress
)
{
    UINT8                   bFwhStatus;
    UINT32                  dTimeout = FLASH_RETRIES * 0x10000;

    do {
        *pAddress = RD_STATUS_CMD;          // read status.
        IntelFwhCmdDelay ();
        bFwhStatus = *pAddress;
        if ( bFwhStatus & WSM_BUSY ) {
            if ( bFwhStatus & ( VPP_LOW + PROGRAM_FAIL + ERASE_FAIL ) )
                return EFI_DEVICE_ERROR;
            else return EFI_SUCCESS;
        }
        dTimeout--;
    } while ( dTimeout != 0 );
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
IntelFlashEraseCommandEx  (
    IN volatile UINT8*      pBlockAddress,
    IN UINT32               Length
)
{
    EFI_STATUS          Status;
    UINT8               bFlashRetry;
    UINTN               nSectorAddr;
    UINT16              wNumSectors, wSectorCount;

    if (Length % FlashAPI->FlashSectorSize) return EFI_INVALID_PARAMETER;
    IntelFwhResetFlash( pBlockAddress );
    wNumSectors = ( Length / FlashAPI->FlashSectorSize );
    for ( wSectorCount = 0; wSectorCount < wNumSectors; wSectorCount++ ) {
        nSectorAddr = (UINTN)( wSectorCount * FlashAPI->FlashSectorSize );
        for ( bFlashRetry = 0; bFlashRetry < FLASH_RETRIES; bFlashRetry++ ) {
            *(UINT8*)( (UINTN)pBlockAddress + nSectorAddr ) = gbEraseSetCmd;
            IntelFwhCmdDelay ();
            *(UINT8*)( (UINTN)pBlockAddress + nSectorAddr ) = ERASE_CNF_CMD;
            IntelFwhCmdDelay ();
            Status = IntelFwhOperationCompleted ( \
                                (UINT8*)((UINTN)pBlockAddress + nSectorAddr) );
            IntelFwhResetFlash( (UINT8*)((UINTN)pBlockAddress + nSectorAddr) );
            if (EFI_ERROR(Status)) continue;
            IntelFlashIsEraseCompleted (pBlockAddress + nSectorAddr, NULL, &Status);
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
IntelFlashEraseCommand  (
    IN volatile UINT8*      pBlockAddress
)
{
    return (EFI_ERROR(IntelFlashEraseCommandEx(pBlockAddress, FlashBlockSize))) ? FALSE : TRUE;
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
IntelFlashIsEraseCompleted(
    IN  volatile UINT8*     pBlockAddress,
    OUT BOOLEAN             *pError,
    OUT UINTN               *pStatus)
{
    UINT32                      dNumBytes;

    for ( dNumBytes = 0; dNumBytes < FlashAPI->FlashSectorSize; dNumBytes++ ) {
        if ( *(volatile UINT8*)( pBlockAddress + dNumBytes ) != 0xFF ) {
            if ( pError )  *pError = TRUE;
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
IntelFlashProgramCommand    (
    IN      volatile UINT8* pByteAddress,
    IN      UINT8           *Byte,
    IN OUT  UINT32          *Length
)
{
    UINT8               bFlashRetry;

    if (*pByteAddress != *Byte) {
        IntelFwhResetFlash( pByteAddress );
        for ( bFlashRetry = 0; bFlashRetry < FLASH_RETRIES; bFlashRetry++ ) {
            *pByteAddress = PRG_SETUP_CMD; // Issue program command
            IntelFwhCmdDelay ();
            *pByteAddress = *Byte; // Program a byte
            IntelFwhCmdDelay ();
            // Check for completion of the program operation
            if (EFI_ERROR(IntelFwhOperationCompleted(pByteAddress))) continue;
            *pByteAddress = READ_ARRAY_CMD; // read mode
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
IntelFlashIsProgramCompleted    (
    IN  volatile UINT8*     pByteAddress,
    IN  UINT8               *Byte,
    IN  UINT32              Length,
    OUT BOOLEAN             *pError,
    OUT UINTN               *pStatus)
{
    UINT32                      dNumBytes;
    UINT8                       bByte;

    for ( dNumBytes = 0; dNumBytes < Length; dNumBytes++ ) {
        bByte = * ( Byte + dNumBytes );
        if ( bByte != *(volatile UINT8*)( pByteAddress + dNumBytes ) ) {
            if ( pError )  *pError = TRUE;
            if ( pStatus ) *pStatus = EFI_DEVICE_ERROR;
            return FALSE;
        }
    }
    if ( pError ) *pError = FALSE;
    if ( pStatus ) *pStatus = EFI_SUCCESS;
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
static
VOID
IntelFlashReadCommand   (
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
    ((UINT8 *)((UINTN)pBlockAddress - FlashDeviceBase + \
                            FwhFeatureSpaceBase))[2] = LockState;
    ((UINT8 *)((UINTN)pBlockAddress - FlashDeviceBase + \
                            FwhFeatureSpaceBase + 0x8000))[2] = LockState;
    ((UINT8 *)((UINTN)pBlockAddress - FlashDeviceBase + \
                            FwhFeatureSpaceBase + 0xA000))[2] = LockState;
    ((UINT8 *)((UINTN)pBlockAddress - FlashDeviceBase + \
                            FwhFeatureSpaceBase + 0xC000))[2] = LockState;
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
IntelFlashBlockWriteEnable  (
    IN UINT8*           pBlockAddress
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
IntelFlashBlockWriteDisable (
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
IntelFlashDeviceWriteEnable (VOID)
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
IntelFlashDeviceWriteDisable (VOID)
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
VOID IntelFlashVirtualFixup  (IN EFI_RUNTIME_SERVICES *pRS)
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
mIntelIdentify  (
    IN  volatile UINT8*     pBlockAddress,
    OUT FLASH_PART          **Struct
)
{
    UINT8 VID, DID;

    SetBlockLock(pBlockAddress, UNLOCK);

    *pBlockAddress = READ_ARRAY_CMD;// Return to read mode
    IntelFwhCmdDelay ();

    *pBlockAddress = RD_ID_CODE;// Set to read ID code mode
    IntelFwhCmdDelay ();
    VID = *pBlockAddress;
    DID = *(pBlockAddress + 1);

    *pBlockAddress = READ_ARRAY_CMD;// Return to read mode

    switch ( (DID << 8) + VID ) {

        case ATMEL_49LW080 :
            MemCpy ( pFlashDeviceNumber, "ATMEL 49LW080", 13 );
            gAtmel = 1;
            break;
        case Intel_82802AB :
        case Intel_82802AC :
            MemCpy ( pFlashDeviceNumber, "Intel N82802AB/C", 16 );
            break;
        case STM_50FW080 :
        case STM_50FW016 :
            MemCpy ( pFlashDeviceNumber, "ST M25FW080/016", 15 );
            break;
        case SST_49LF016 :
            gbEraseSetCmd = SECTOR_ERASE_SET_CMD;
            mIntelFirmwareHub.FlashSectorSize = SECTOR_SIZE_4KB;
            MemCpy ( pFlashDeviceNumber, "SST 49LF016C", 12 );
            break;
        default:
            return FALSE;
    }
    mIntelFirmwareHub.FlashPartNumber = pFlashDeviceNumber;
    *Struct = &mIntelFirmwareHub;
    return TRUE;
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
