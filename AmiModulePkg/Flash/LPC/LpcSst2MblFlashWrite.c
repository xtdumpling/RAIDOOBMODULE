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
#define SST_MANUFACTURER_ID         0xbf    // SST manufacturer  ID
#define SST49LF160C_DEVICE_ID       0x4c    // 2MB LPC
#define SST49LF160C                 0x4cbf  // SST 49LF160C
#define SST49LF016C			        0x5cbf
#define STM50LPW080                 0x2f20  // ST M50LPW080


#define READ_ARRAY_CMD              0xff
#define RD_STATUS_CMD               0x70
#define CLR_STATUS_CMD              0x50
#define ERASE_SET_CMD               0x20
#define ERASE_CNF_CMD               0xd0
#define PRG_SETUP_CMD               0x40
#define SECTOR_ERASE_SET_CMD        0x30 // SST 49LF160C only

#define RD_ID_CODE                  0x90

// SST49LF160C Status Register Bits

#define VPP_LOW                     0x08
#define PROGRAM_FAIL                0x10
#define ERASE_FAIL                  0x20
#define WSM_BUSY                    0x80

// Intel Lock Commands
#define UNLOCK                      0
#define WRITE_LOCK                  1

//-#define SECTOR_SIZE_4KB             0x1000  // Common 4kBytes sector size
//-#define SECTOR_SIZE_64KB            0x10000  // Common 64kBytes sector size

//----------------------------------------------------------------------------
// Module level global data
extern FLASH_PART   *FlashAPI;
extern UINT8        pFlashDeviceNumber[FLASH_PART_STRING_LENGTH];

//----------------------------------------------------------------------------
// Local Variable definitions

static EFI_STATUS
InitSst2MbCheckId   (
    IN volatile UINT8*      pBlockAddress
    );
static BOOLEAN
SstLpcIsEraseCompleted  (
    IN  volatile UINT8      *pBlockAddress,
    OUT BOOLEAN             *pError,
    OUT UINTN               *pStatus
    );
static BOOLEAN
SstLpcIsProgramCompleted    (
    IN  volatile UINT8      *pByteAddress,
    IN  UINT8               *Byte,
    IN  UINT32              Length,
    OUT BOOLEAN             *pError,
    OUT UINTN               *pStatus
    );

static UINT8 gbEraseSetCmd = ERASE_SET_CMD;

//----------------------------------------------------------------------------
// Function Definitions
/**
 * The function delays for flash operation.
 *
 */
VOID
Sst2mLpcCmdDelay (VOID)
{
//-    IoWrite8 ( 0xeb, 0x55 );
//-    IoWrite8 ( 0xeb, 0xaa );
	MicroSecondDelay(10);
}


/**
 * This function resets the flash part.
 *
 * @param pAddress Location to check the device status
 *                      
 */
static
VOID
Sst2mLpcResetFlash  (
    IN volatile UINT8*      pAddress
)
{
    *pAddress = READ_ARRAY_CMD;// Return to read mode
    Sst2mLpcCmdDelay ();
    *pAddress = CLR_STATUS_CMD;// clear status
    Sst2mLpcCmdDelay ();
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
Sst2mLpcOperationCompleted  (
    IN volatile UINT8*      pAddress
)
{
    UINT8                   bLpcStatus;
    UINT32                  dTimeout = FLASH_RETRIES * 0x10000;

    do {
        *pAddress = RD_STATUS_CMD;          // read status.
        Sst2mLpcCmdDelay ();
        bLpcStatus = *pAddress;
        if ( bLpcStatus & WSM_BUSY ) {
            if ( bLpcStatus & ( VPP_LOW + PROGRAM_FAIL + ERASE_FAIL ) )
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
SstLpcEraseCommandEx  (
    IN volatile UINT8*      pBlockAddress,
    IN UINT32               Length
)
{
    EFI_STATUS          Status;
    UINT8               bFlashRetry;
    UINTN               nSectorAddr;
    UINT16              wNumSectors, wSectorCount;

    if (Length % FlashAPI->FlashSectorSize) return EFI_INVALID_PARAMETER;
    Sst2mLpcResetFlash( pBlockAddress );
    wNumSectors = ( Length / FlashAPI->FlashSectorSize );
    for ( wSectorCount = 0; wSectorCount < wNumSectors; wSectorCount++ ) {
        nSectorAddr = (UINTN)( wSectorCount * FlashAPI->FlashSectorSize );
        for ( bFlashRetry = 0; bFlashRetry < FLASH_RETRIES; bFlashRetry++ ) {
            *(UINT8*)( (UINTN)pBlockAddress + nSectorAddr ) = gbEraseSetCmd;
            Sst2mLpcCmdDelay ();
            *(UINT8*)( (UINTN)pBlockAddress + nSectorAddr ) = ERASE_CNF_CMD;
            Sst2mLpcCmdDelay ();
            Status = Sst2mLpcOperationCompleted ( \
                            (UINT8*)((UINTN)pBlockAddress + nSectorAddr) );
            Sst2mLpcResetFlash( (UINT8*)((UINTN)pBlockAddress + nSectorAddr) );
            if (EFI_ERROR(Status)) continue;
            SstLpcIsEraseCompleted (pBlockAddress + nSectorAddr, NULL, &Status);
            if (!EFI_ERROR(Status)) break;
        }
        if (EFI_ERROR(Status)) return Status;
    }
    return Status;
}

static
BOOLEAN
SstLpcEraseCommand  (
    IN volatile UINT8*      pBlockAddress
)
{
    return (EFI_ERROR(SstLpcEraseCommandEx(pBlockAddress, FlashBlockSize))) ? FALSE : TRUE;
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
SstLpcIsEraseCompleted  (
    IN  volatile UINT8*     pBlockAddress,
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
 * This function read data from the specified location
 *
 * @param pByteAddress Location where the data to be read
 * @param Byte Data buffer to be read
 * @param Length Number of bytes to read
 *                      
 */
static
VOID
SstLpcReadCommand   (
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
 * This function programs a byte data to the specified location
 *
 * @param pByteAddress Location where the data to be written
 * @param Byte Byte to be written
 * @param Length Number of bytes to write
 *                      
 */
static
BOOLEAN
SstLpcProgramCommand    (
    IN  volatile UINT8*     pByteAddress,
    OUT UINT8               *Byte,
    OUT UINT32              *Length
)
{
    UINT8               bFlashRetry;
    EFI_STATUS          Status;

    if ( *pByteAddress != *Byte ) {
        Sst2mLpcResetFlash( pByteAddress );
        for ( bFlashRetry = 0; bFlashRetry < FLASH_RETRIES; bFlashRetry++ ) {
            *pByteAddress = PRG_SETUP_CMD; // Issue program command
            Sst2mLpcCmdDelay ();
            *pByteAddress = *Byte; // Program a byte
            Sst2mLpcCmdDelay ();
            // Check for completion of the program operation
            if (EFI_ERROR(Sst2mLpcOperationCompleted(pByteAddress))) {
                Sst2mLpcResetFlash (pByteAddress);
                continue;
            }    
            Status = (*pByteAddress == *Byte) ? EFI_SUCCESS : EFI_DEVICE_ERROR;
            if (!EFI_ERROR(Status)) break;
        }
    }
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
SstLpcIsProgramCompleted    (
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
SstLpcBlockWriteEnable  (
    IN UINT8*               pBlockAddress
)
{
    SetBlockLock(pBlockAddress, UNLOCK);
//-    return EFI_SUCCESS;
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
SstLpcBlockWriteDisable (
    IN UINT8*               pBlockAddress
)
{
    SetBlockLock(pBlockAddress, WRITE_LOCK);
//-    return EFI_SUCCESS;
}


/**
 * This function contains any flash specific code need to enable flash write
 *
 */
static
VOID
SstLpcDeviceWriteEnable (VOID)
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
SstLpcDeviceWriteDisable (VOID)
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
SstLpcVirtualFixup  (
    IN EFI_RUNTIME_SERVICES *pRS
)
{
//  // Following is an example code for virtual address conversion
//  pRS->ConvertPointer(0, (VOID**)&FlashDeviceBase);

    return;
}



// Flash Part Data structure for SST 49LF160C

FLASH_PART mSstLpc2Mb =
    {
        SstLpcReadCommand,
        SstLpcEraseCommand,
        SstLpcEraseCommandEx,
        SstLpcProgramCommand,
//-        SstLpcIsEraseCompleted,
//-        SstLpcIsProgramCompleted,
        SstLpcBlockWriteEnable,
        SstLpcBlockWriteDisable,
        SstLpcDeviceWriteEnable,
        SstLpcDeviceWriteDisable,
        SstLpcVirtualFixup,
        1,                              // Number of bytes to program to the
        SECTOR_SIZE_64KB,               // Dummy value to hold place -
        NULL                            // Flash Part Number Pointer                           
    };                                  // Flash part in each program command


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
mSstLpcIdentify (
    IN volatile UINT8*      pBlockAddress,
    IN FLASH_PART           **Struct
)
{

    if (EFI_ERROR(InitSst2MbCheckId(pBlockAddress))) return FALSE;
    mSstLpc2Mb.FlashPartNumber = pFlashDeviceNumber;
    *Struct = &mSstLpc2Mb;
    return TRUE;
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
InitSst2MbCheckId   (
    IN volatile UINT8*      pBlockAddress
)
{
    UINT8           VID, DID;
    EFI_STATUS      Status = EFI_NOT_FOUND;

    *pBlockAddress = READ_ARRAY_CMD;// Return to read mode
    Sst2mLpcCmdDelay ();

    *pBlockAddress = RD_ID_CODE;// Set to read ID code mode
    Sst2mLpcCmdDelay ();

    VID = *pBlockAddress;
    DID = *(pBlockAddress + 1);

    *pBlockAddress = READ_ARRAY_CMD;// Return to read mode

    switch ( ( DID << 8 ) + VID ) {
        case SST49LF160C :
        case SST49LF016C :
            gbEraseSetCmd = SECTOR_ERASE_SET_CMD;
            mSstLpc2Mb.FlashSectorSize = SECTOR_SIZE_4KB;
            MemCpy ( pFlashDeviceNumber, "SST 49LF016/160C", 16 );
            Status = EFI_SUCCESS;
            break;
        case STM50LPW080 :
            gbEraseSetCmd = ERASE_SET_CMD;
            mSstLpc2Mb.FlashSectorSize = SECTOR_SIZE_64KB;
            MemCpy ( pFlashDeviceNumber, "ST M50LPW080", 12 );
            Status = EFI_SUCCESS;
            break;
        default :
            Status = EFI_NOT_FOUND;
    }
    return Status;
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