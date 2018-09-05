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
#define VENDOR_ID           0x88 // Intel Manufacturers ID
#define DEVICE_ID64T        0x17 // 64M TB
#define DEVICE_ID128T       0x18 // 128M TB
#define DEVICE_ID256T       0x19 // 256M TB
#define DEVICE_ID64B        0x1A // 64M BB
#define DEVICE_ID128B       0x1B // 128M BB
#define DEVICE_ID256B       0x1C // 256M BB

//Flash Part Specific Tokens
#define READ_ARRAY_CMD      0xff
#define RD_STATUS_CMD       0x70
#define CLR_STATUS_CMD      0x50
#define ERASE_SET_CMD       0x20
#define ERASE_CNF_CMD       0xd0
#define PRG_SETUP_CMD       0x40

#define RD_ID_CODE          0x90

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
extern UINT8    pFlashDeviceNumber[FLASH_PART_STRING_LENGTH];

//----------------------------------------------------------------------------
// Function Prototypes
static BOOLEAN 
IntelStdFlashEraseCommand  (
    IN volatile UINT8       *pBlockAddress
    );
static EFI_STATUS
IntelStdFlashEraseCommandEx   (
    IN volatile UINT8       *pBlockAddress,
    IN UINT32               Length
    );
static VOID 
IntelStdFlashReadCommand   (
    IN volatile UINT8       *pByteAddress,
    IN UINT8                *Data, 
    IN UINT32               *Length
    );
static BOOLEAN 
IntelStdFlashProgramCommand    (
    IN      volatile UINT8  *pByteAddress,
    IN      UINT8           *Data, 
    IN OUT  UINT32          *Length
    );
static BOOLEAN 
IntelStdFlashIsEraseCompleted  (
    IN  volatile UINT8      *pBlockAddress,
    OUT BOOLEAN             *pError, 
    OUT UINTN               *pStatus
    );
static BOOLEAN 
IntelStdFlashIsProgramCompleted    (
    IN  volatile UINT8      *pByteAddress,
    IN  UINT8               *Byte, 
    IN  UINT32              Length,
    OUT BOOLEAN             *pError, 
    OUT UINTN               *pStatus
    );
static VOID 
IntelStdFlashBlockWriteEnable  (
   IN volatile  UINT8       *pBlockAddress
    );
static VOID 
IntelStdFlashBlockWriteDisable (
   IN volatile UINT8        *pBlockAddress
    );
static VOID 
IntelStdFlashDeviceWriteEnable (
    VOID
    );
static VOID 
IntelStdFlashDeviceWriteDisable    (
    VOID
    );
static VOID 
IntelStdFlashVirtualFixup  (
    IN EFI_RUNTIME_SERVICES *pRS
    );

extern FLASH_PART   *FlashAPI;
//========================================================================
// Local Variable definitions

// Flash Part Data structure fo the intel 82802ACC
FLASH_PART mIntelSTD =
    {
        IntelStdFlashReadCommand,
        IntelStdFlashEraseCommand,
        IntelStdFlashEraseCommandEx,
        IntelStdFlashProgramCommand,
//-        IntelStdFlashIsEraseCompleted,
//-        IntelStdFlashIsProgramCompleted,
        IntelStdFlashBlockWriteEnable,
        IntelStdFlashBlockWriteDisable,
        IntelStdFlashDeviceWriteEnable,
        IntelStdFlashDeviceWriteDisable,
        IntelStdFlashVirtualFixup,
        1,                  // Number of bytes to program to the
                            // Flash part in each program command
        SECTOR_SIZE_32KB,   // Dummy value to hold place - only used in SPI
        NULL                // Flash Part Number Pointer                   
    };



//========================================================================
// Function Definitions
/**
 * The function delays for flash operation.
 *
 */
VOID 
IntelStdCmdDelay (VOID)
{
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
IntelStdResetFlash  ( 
    IN volatile UINT8*      pAddress 
)
{
    *pAddress = READ_ARRAY_CMD;// Return to read mode
    IntelStdCmdDelay ();
    *pAddress = CLR_STATUS_CMD;// clear status
    IntelStdCmdDelay ();
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
IntelStdOperationCompleted  (
    IN volatile UINT8*      pAddress
)
{
    UINT8                   bSTDStatus;
    UINT32                  dTimeout = FLASH_RETRIES * 0x10000;

    do {
        *pAddress = RD_STATUS_CMD;          // read status.
        IntelStdCmdDelay ();
        bSTDStatus = *pAddress;
        if ( bSTDStatus & WSM_BUSY ) {
            if ( bSTDStatus & ( VPP_LOW + PROGRAM_FAIL + ERASE_FAIL ) )
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
IntelStdFlashEraseCommandEx  (
    IN volatile UINT8*      pBlockAddress,
    IN UINT32               Length
)
{
    EFI_STATUS          Status;
    UINT8               bFlashRetry;
    UINT16              i, wNumSectors;

    if (Length % FlashAPI->FlashSectorSize) return EFI_INVALID_PARAMETER;
    wNumSectors  = Length / FlashAPI->FlashSectorSize;
    for (i = 0; i < wNumSectors; i++ ) {
        volatile UINT8 *pSectorAddress = pBlockAddress + (i * FlashAPI->FlashSectorSize);
        for (bFlashRetry = 0 ; bFlashRetry < FLASH_RETRIES; bFlashRetry++) {
            IntelStdResetFlash (pSectorAddress);
            *pSectorAddress = ERASE_SET_CMD;
            IntelStdCmdDelay ();
            *pSectorAddress = ERASE_CNF_CMD;
            IntelStdCmdDelay ();
            Status = IntelStdOperationCompleted (pSectorAddress);
            IntelStdResetFlash (pSectorAddress);
            if (Status != EFI_SUCCESS) continue;
            IntelStdFlashIsEraseCompleted (pSectorAddress, NULL, &Status);
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
IntelStdFlashEraseCommand  (
    IN volatile UINT8*      pBlockAddress
)
{
    return (EFI_ERROR(IntelStdFlashEraseCommandEx (pBlockAddress, FlashBlockSize))) ? FALSE : TRUE;
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
IntelStdFlashIsEraseCompleted  (
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
 * This function programs a byte data to the specified location
 *
 * @param pByteAddress Location where the data to be written
 * @param Byte Byte to be written
 * @param Length Number of bytes to write
 *                      
 */
static 
BOOLEAN 
IntelStdFlashProgramCommand    (
    IN      volatile UINT8* pByteAddress,
    IN      UINT8           *Byte,
    IN OUT  UINT32          *Length
)
{
    UINT8               bFlashRetry;
    UINT16              Word = 0;
    UINT16*             adjAddress;
    BOOLEAN             Program_Word = FALSE;

    adjAddress =(UINT16*)( (UINTN)pByteAddress & 0xFFFFFFFE);
    if (*pByteAddress != *Byte) {
        // Word Program Only , Adjust Input word data
        if((UINTN)pByteAddress & 1) {
            // Get Last Byte of current address
            Word = (*Byte) << 8;
            Word += *(UINT8*)((UINTN)pByteAddress - 1);
        } else {
            if(*Length > 1) {
                // Get Next Byte from Input Buffer
                Word = *(Byte + 1) << 8;
                Program_Word = TRUE;
            } else {
                // Get Next Byte of current address
                Word = (*(UINT8*)((UINTN)pByteAddress + 1)) << 8;
            }
            Word += *Byte;
        }
        IntelStdResetFlash( (volatile UINT8*)adjAddress );
        for ( bFlashRetry = 0; bFlashRetry < FLASH_RETRIES; bFlashRetry++ ) {
            *(UINT16*)adjAddress = PRG_SETUP_CMD; // Issue program command
            IntelStdCmdDelay ();
            *(UINT16*)adjAddress = Word; // Program a Word
            IntelStdCmdDelay ();
            // Check for completion of the program operation
            if (EFI_ERROR(IntelStdOperationCompleted ((volatile UINT8*)adjAddress))) {
                IntelStdResetFlash( (volatile UINT8*)adjAddress );
                continue;
            }    
            if (*adjAddress == Word) break;
            if (FLASH_RETRIES == (bFlashRetry + 1)) return FALSE;
        }
    }
    if ( !Program_Word ) *Length = *Length - 1;
    else *Length = *Length - 2;
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
IntelStdFlashIsProgramCompleted    (
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
 * This function read data from the specified location
 *
 * @param pByteAddress Location where the data to be read
 * @param Byte Data buffer to be read
 * @param Length Number of bytes to read
 *                      
 */
static 
VOID 
IntelStdFlashReadCommand   (
    IN volatile UINT8*      pByteAddress, 
    IN UINT8                *Byte, 
    IN UINT32               *Length
)
{
    UINT32              dNumBytes = 0;
    
    // Changes for SMIFlash module label "4.6.3.6_SMIFLASH_12" or later.
    for ( dNumBytes = 0; dNumBytes < *Length ; dNumBytes++ )
        *( Byte + dNumBytes ) = *(UINT8*)((UINTN)pByteAddress + dNumBytes );
    *Length = 0;
    return ;
//-    return EFI_SUCCESS;
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
IntelStdFlashBlockWriteEnable  (
    IN volatile UINT8*               pBlockAddress
)
{
//-    SetBlockLock(pBlockAddress, UNLOCK);
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
IntelStdFlashBlockWriteDisable (
    IN volatile UINT8*               pBlockAddress
)
{
//-    SetBlockLock(pBlockAddress, WRITE_LOCK);
}


/**
 * This function contains any flash specific code need to enable flash write
 *
 */
static 
VOID 
IntelStdFlashDeviceWriteEnable (VOID)
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
IntelStdFlashDeviceWriteDisable (VOID)
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
IntelStdFlashVirtualFixup  ( 
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
IntelSTDIdentify    (
    IN  volatile UINT8*     pBlockAddress,
    OUT FLASH_PART          **Struct
)
{
    UINT8 VID, DID;

//-    SetBlockLock(pBlockAddress, UNLOCK);

    *pBlockAddress = RD_ID_CODE;// Set to read ID code mode
    VID = *pBlockAddress;
    DID = *(pBlockAddress + 2);
    *pBlockAddress = READ_ARRAY_CMD;// Return to read mode

    if ((VID == VENDOR_ID) && 
        (DID == DEVICE_ID64T || DID == DEVICE_ID128T || \
         DID == DEVICE_ID256T || DID == DEVICE_ID64B || \
         DID == DEVICE_ID128B || DID == DEVICE_ID256B)) {
        // 1. Please modify your FLASH_BLOCK_SIZE to 128K in flash.sdl.
        // 2. Although most Blocks are uniform 128k, but we have to report its minimum block 32K. 
        MemCpy ( pFlashDeviceNumber, "Numonyx 28F640/128/256P30B/T", 28 );
        mIntelSTD.FlashPartNumber = pFlashDeviceNumber;
        mIntelSTD.FlashSectorSize = SECTOR_SIZE_32KB;
        *Struct = &mIntelSTD;
        return TRUE;
    } else return FALSE;
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
