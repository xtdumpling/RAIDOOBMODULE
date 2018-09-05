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
  Flash Write file.
**/

//----------------------------------------------------------------------
// Includes
#include <Efi.h>
#include <Token.h>
#include <Flash.h>
#include <FlashPart.h>
//AptioV
#define _DXE_FLASH_LIB_
#include <FlashElink.h>
//AptioV end
#if defined _DXE_FLASH_LIB_
#include <AmiDxeLib.h>
#endif
//----------------------------------------------------------------------
// component MACROs


//----------------------------------------------------------------------
// Module defined global variables
FLASH_PART      *FlashAPI = NULL;

// flash part list creation code must be in this order
typedef BOOLEAN (IDENTIFY)(
    volatile UINT8*     pBlockAddress,
    FLASH_PART          **Struct
    );

extern IDENTIFY FLASH_LIST EndOfInitList;

IDENTIFY* FlashList[] = {FLASH_LIST NULL};

// oem flash write enable/disable list creation code must be in this order
typedef VOID (OEM_FLASH_WRITE) (VOID);
extern OEM_FLASH_WRITE OEM_FLASH_WRITE_ENABLE_LIST EndOfOemFlashList;
extern OEM_FLASH_WRITE OEM_FLASH_WRITE_DISABLE_LIST EndOfOemFlashList;
OEM_FLASH_WRITE* OemFlashWriteEnable[] = {OEM_FLASH_WRITE_ENABLE_LIST NULL};
OEM_FLASH_WRITE* OemFlashWriteDisable[] = {OEM_FLASH_WRITE_DISABLE_LIST NULL};
UINT16 OemFlashValidTable[] = {OEM_FLASH_VALID_TABLE_LIST 0xFFFF};
// END flash part list creation code

//----------------------------------------------------------------------
// Function prototypes
VOID OemFlashDeviceWriteEnable ( VOID );
VOID OemFlashDeviceWriteDisable ( VOID );

//----------------------------------------------------------------------
// Module specific global variable
UINT16  gFlashId = 0;
UINT8   pFlashDeviceNumber[FLASH_PART_STRING_LENGTH];

//----------------------------------------------------------------------
// externally defined variables
//-extern VOID ChipsetFlashDeviceWriteEnable();
//-extern VOID ChipsetFlashDeviceWriteDisable();

//----------------------------------------------------------------------
// Function definitions
/**
 * This function identifies the supported flash parts and returns TRUE. 
 * If flash part is not supported by this module it will return FALSE.
 *
 * @param dFlashId Present Flash Part ID
 *                      
 * @retval TRUE Flash part is Valid.
 * @retval FALSE Flash part is not Valid.
 */
BOOLEAN 
OemIsFlashValid(VOID)
{
    UINTN           i = 0;
    if (OemFlashValidTable[0] == 0xFFFF) return TRUE;
    for (i = 0; OemFlashValidTable[i] != 0xFFFF; i++) {
        if (gFlashId == OemFlashValidTable[i]) return TRUE;
    }
    return FALSE;
}


/**
 * This function goes through the elinked list of identify functions giving 
 * control until one part is identified properly
 *
 * @param pBlockAddress Address to access the flash part
 *                      
 * @retval EFI_SUCCESS
 */
EFI_STATUS
FlashInit   (
    IN volatile UINT8*      pBlockAddress
)
{
    UINTN i;
    BOOLEAN found = FALSE;

    for(i=0; !found && FlashList[i]; i++)
    {
        found=FlashList[i](pBlockAddress, &FlashAPI);
    }
    if (found && !OemIsFlashValid()) FlashAPI = NULL;
    return EFI_SUCCESS;
}


/**
 * This function Chooses the correct flash part to call and then initiates 
 * flash block erase command
 *
 * @param pBlockAddress Address to access flash part
 *                      
 */
BOOLEAN
FlashEraseCommand   (
    IN volatile UINT8*      pBlockAddress
)
{
    if ( FlashAPI == NULL)
        FlashInit(pBlockAddress);

    // if FlashAPI is still NULL return
    if ( FlashAPI == NULL) return FALSE;

    return FlashAPI->FlashEraseCommand(pBlockAddress);

}


/**
 * This function Chooses the correct flash part to call and then initiates 
 * flash block erase command
 *
 * @param pBlockAddress Address to access flash part
 *                      
 */
EFI_STATUS
FlashEraseCommandEx   (
    IN volatile UINT8*      pBlockAddress,
    IN UINT32               Length
)
{
    if (FlashAPI == NULL) FlashInit(pBlockAddress);

    // if FlashAPI is still NULL return
    if ( FlashAPI == NULL) return EFI_UNSUPPORTED;

    return FlashAPI->FlashEraseCommandEx(pBlockAddress, Length);
}


/**
 * This function chooses the correct flash part to call and then checks 
 * current status of the last erase operation
 *
 * @param pBlockAddress Address to access flash part
 * @param pError Boolean that tells if fatal error occured
 * @param pStatus Status of the erase command
 *                      
 * @retval TRUE erase complete
 * @retval FALSE erase not completed
 */
BOOLEAN
FlashIsEraseCompleted   (
    IN volatile UINT8     *pBlockAddress,
    OUT         BOOLEAN   *pError,
    OUT         UINTN     *pStatus)
{
//-    BOOLEAN Temp;
//-
//-    if (FlashAPI == NULL)
//-        FlashInit(pBlockAddress);
//-
//-    // if FlashAPI is still NULL return
//-    if (FlashAPI == NULL)
//-        return FALSE;
//-
//-    Temp = FlashAPI->FlashIsEraseCompleted(pBlockAddress, pError, pStatus);
//-
//-    return Temp;
    return TRUE;
}


/**
 * This function chooses the correct flash part to call and 
 * then initiates the program command
 *
 * @param pByteAddress Address to write to the flash part
 * @param Byte pointer to data to write to the flash part
 * @param Length The total amount of data that Byte points to be written
 *                      
 */
BOOLEAN
FlashProgramCommand (
    IN volatile UINT8*      pByteAddress,
    IN UINT8                *Byte,
    IN UINT32               Length
)
{
    volatile UINT8 *a=(volatile UINT8 *) pByteAddress;
    UINT32  TempLength = Length;
    UINT8   *DataPtr = Byte;

    if ( FlashAPI == NULL)
        FlashInit((volatile UINT8*)BLOCK(pByteAddress));

    // if FlashAPI is still NULL return
    if ( FlashAPI == NULL) return FALSE;

    do {
        if (!FlashAPI->FlashProgramCommand(a, DataPtr, &TempLength)) return FALSE;
        a += (Length - TempLength);
        DataPtr += (Length - TempLength);
        Length = TempLength;
    } while (TempLength != 0);
    
    return TRUE;
}


/**
 * This function chooses the correct flash part to call and then initiates 
 * the read command
 *
 * @param pByteAddress Address to read from the flash part
 * @param Byte pointer to data to read to the flash part
 * @param Length The total amount of data that Byte points to be read
 *                      
 */
VOID
FlashReadCommand    (
    IN  volatile UINT8*     pByteAddress,
    OUT UINT8               *Byte,
    IN  UINT32              Length
)
{
    volatile UINT8 *a=(volatile UINT8 *) pByteAddress;
    UINT32  TempLength = Length;
    UINT8   *DataPtr = Byte;

    if ( FlashAPI == NULL)
        FlashInit((volatile UINT8*)BLOCK(pByteAddress));

    // if FlashAPI is still NULL return
    if ( FlashAPI == NULL)
        return;

    do {
        FlashAPI->FlashReadCommand(a, DataPtr, &TempLength);
        a += (Length - TempLength);
        DataPtr += (Length - TempLength);
        Length = TempLength;
    } while (TempLength != 0);
}


/**
 * This function chooses the correct flash part to call and then checks 
 * current status of the last program operation
 *
 * @param pByteAddress Address to access flash part
 * @param Byte values previously written to the Flash Device
 * @param Length The amount of data that needs to be checked
 * @param pError Boolean that tells if fatal error occured
 * @param pStatus Status of the erase command
 *                      
 * @retval program complete
 * @retval program not completed
 */
BOOLEAN
FlashIsProgramCompleted (
    IN  volatile UINT8*     pByteAddress,
    IN  UINT8               *Byte,
    IN  UINT32              Length,
    OUT BOOLEAN             *pError,
    OUT UINTN               *pStatus
)
{
//-    if (FlashAPI == NULL)
//-        FlashInit(pByteAddress);
//-
//-    // if FlashAPI is still NULL return
//-    if (FlashAPI == NULL)
//-        return FALSE;
//-
//-    return FlashAPI->FlashIsProgramCompleted( \
//-                        pByteAddress, Byte, Length, pError, pStatus);
    return TRUE;
}


/**
 * This function chooses the correct flash part to call and then enables 
 * write operations(erase/programming) for a specific block
 *
 * @param pBlockAddress Address to access flash part
 *                      
 */
VOID
FlashBlockWriteEnable   (
    IN UINT8*               pBlockAddress
)
{
    if ( FlashAPI == NULL)
        FlashInit(pBlockAddress);

    // if FlashAPI is still NULL return
    if ( FlashAPI == NULL)
        return;

    FlashAPI->FlashBlockWriteEnable(pBlockAddress);
}


/**
 * This function chooses the correct flash part to call and then disables 
 * write operations(erase/programming) for a specific block
 *
 * @param pBlockAddress Address to access flash part
 *                      
 */
VOID
FlashBlockWriteDisable  (
    IN UINT8*               pBlockAddress
)
{
    if ( FlashAPI == NULL)
        FlashInit(pBlockAddress);

    // if FlashAPI is still NULL return
    if ( FlashAPI == NULL)
        return;

    FlashAPI->FlashBlockWriteDisable(pBlockAddress);

}


/**
 * This function chooses the correct flash part to call and then enables 
 * write operation for a flash device
 *
 */
VOID
FlashDeviceWriteEnable  (VOID)
{
    OemFlashDeviceWriteEnable ();

    // ChipsetFlashDeviceWriteEnable();

    if ( FlashAPI == NULL)
        FlashInit((UINT8 *)FlashDeviceBase);

    // if FlashAPI is still NULL return
    if ( FlashAPI == NULL)
        return;

    FlashAPI->FlashDeviceWriteEnable();

}


/**
 * This function chooses the correct flash part to call and then disables
 * write operation for a flash device
 *
 */
VOID
FlashDeviceWriteDisable (VOID)
{
    // ChipsetFlashDeviceWriteDisable();

    if ( FlashAPI == NULL)
        FlashInit((UINT8 *)FlashDeviceBase);

    // if FlashAPI is still NULL return
    if ( FlashAPI == NULL)
        return;

    FlashAPI->FlashDeviceWriteDisable();

    OemFlashDeviceWriteDisable ();
}


/**
 * This function goes through the elinked list of oem flash write enable 
 * functions giving control.
 *
 */
VOID
OemFlashDeviceWriteEnable (VOID)
{
    UINT8   i;

        for(i = 0; OemFlashWriteEnable[i] != NULL; i++) 
            OemFlashWriteEnable[i]();
}


/**
 * This function goes through the elinked list of oem flash write disable
 * functions giving control.
 *
 */
VOID
OemFlashDeviceWriteDisable (VOID)
{
    UINT8   i;

        for(i = 0; OemFlashWriteDisable[i] != NULL; i++) 
            OemFlashWriteDisable[i]();
}


/**
 * Fixup global data for for a virtual address space. This routine must be 
 * called by the library consumer in the EVT_SIGNAL_VIRTUAL_ADDRESS_CHANGE 
 * event handler
 *
 * @param pRS pointer to the Runtime Services Table
 *                      
 */
VOID
FlashVirtualFixup   (
    IN EFI_RUNTIME_SERVICES *pRS
)
{
    static BOOLEAN  bFlashVirtual = FALSE;
    VOID            **p;
    UINT8           i;

    //if FlashAPI is NULL, nothing to fix up
    //Notes: Current implementation of Identify routines
    //will not work in virtual mode since absolute addresses are used
    //That's Ok for now since current flash library consumers
    //call flash routines at boot time, so we should have FlashAPI
    //initialized at this point
    if (!FlashAPI) return;
    
    // This function gets called from Nvramdxe.c and flash.c, do anything when 
    // the function is called second time.
    if (bFlashVirtual == TRUE) return;
    else bFlashVirtual = TRUE;

    //Do device specific fixups
    FlashAPI->FlashVirtualFixup(pRS);

    //Fixup FlashAPI member functions
    for(p = (VOID**)FlashAPI; p < (VOID**)(FlashAPI + 1); p++)
        pRS->ConvertPointer(0, p);

    //Fixup FlashAPI pointer
    pRS->ConvertPointer(0, (VOID**)&FlashAPI);

    //convert FeatureSpaceBase address
    pRS->ConvertPointer(0, (VOID**)&FwhFeatureSpaceBase);

    //convert FlashDeviceBase address
    pRS->ConvertPointer(0, (VOID**)&FlashDeviceBase);

    //convert FlashBlockSize address
    pRS->ConvertPointer(0, (VOID**)&FlashBlockSize);

    //Fixup OemFlashWriteEnable list
    for(i = 0; OemFlashWriteEnable[i] != NULL; i++)
        pRS->ConvertPointer(0, (VOID**)&OemFlashWriteEnable[i]);

    //Fixup OemFlashWriteDisable list
    for(i = 0; OemFlashWriteDisable[i] != NULL; i++)
        pRS->ConvertPointer(0, (VOID**)&OemFlashWriteDisable[i]);
}


#if defined _DXE_FLASH_LIB_
/**
 * This function chooses the correct flash part to call and then fill its model name.
 *
 * @param pBlockAddress Address to access flash part
 * @param Buffer Buffer to fill the flash model name.
 *                      
 */
VOID
GetFlashPartInfomation  (
    IN  UINT8*          pBlockAddress,
    IN  UINT8*          Buffer
)
{
    UINT8           bFlashStringSign[4] = {'$','F','P','S'};
    
    if ( FlashAPI == NULL) {
        MemSet( &pFlashDeviceNumber, FLASH_PART_STRING_LENGTH, 0 );
        FlashInit(pBlockAddress);
    }        
    if ((FlashAPI != NULL) && ( FlashAPI->FlashPartNumber != NULL )) {
        MemCpy ( Buffer, bFlashStringSign, 4 );
        Buffer += sizeof (UINT32);
        WriteUnaligned32((UINT32*)Buffer, FLASH_PART_STRING_LENGTH);
        Buffer += sizeof (UINT32);
        MemCpy ( Buffer, FlashAPI->FlashPartNumber, FLASH_PART_STRING_LENGTH );
    }
}    
#endif  // #ifdef _DXE_FLASHLIB_

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
