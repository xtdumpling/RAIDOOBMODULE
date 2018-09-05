//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2008, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

//****************************************************************************
// $Header: /Alaska/SOURCE/CPU/Intel/Nehalem/FwhFvb.c 2     3/04/09 10:52a Markw $
//
// $Revision: 2 $
//
// $Date: 3/04/09 10:52a $
//
//****************************************************************************

//<AMI_FHDR_START>
//----------------------------------------------------------------------------
//
// Name:		FwhFvb.c
//
// Description:	
//  This file contains code for flash update used by the MicrocodeUpdate module.
//
//----------------------------------------------------------------------------
//<AMI_FHDR_END>

#include <Efi.h>
#include "Flash.h"
#include "Token.h"


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:	FwhFvbErase
//
// Description:	This function erases a block of NVRAM.
//
// Input:	    Address - Address of the block to erase
//
// Output:		Return Status based on errors that occurred while erasing.
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS FwhErase(
    IN UINTN      Address,
    IN UINTN      NumberOfBytes
)
{
    UINTN FlashBlockStart = BLOCK(Address);
    UINTN FlashBlockEnd = BLOCK(Address + NumberOfBytes - 1);
    UINTN FlashBlock;
    BOOLEAN bStatus;

    FlashDeviceWriteEnable();

    for (FlashBlock = FlashBlockStart; FlashBlock <= FlashBlockEnd; FlashBlock += FLASH_BLOCK_SIZE) {
        FlashBlockWriteEnable((UINT8*)FlashBlock);
		bStatus = FlashEraseBlock((UINT8*)FlashBlock);
        FlashBlockWriteDisable((UINT8*)FlashBlock);
        if (!bStatus) break;
    }

    FlashDeviceWriteDisable();

    return bStatus ? EFI_SUCCESS : EFI_DEVICE_ERROR;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:	FwhWrite
//
// Description:	This function writes a block of NVRAM.
//
// Input:	    Address       - Address of the block to write.
//              Buffer        - Pointer to a buffer containing data to write.
//              NumberOfBytes - The number of bytes to write
//
// Output:		Return Status based on errors that occurred while writing.
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS FwhWrite(
    IN UINT8      *Buffer, 
    IN UINTN      Address, 
    IN UINTN      NumberOfBytes
)
{
    BOOLEAN  bStatus;
    UINTN FlashBlockStart = BLOCK(Address);
    UINTN FlashBlockEnd = BLOCK(Address + NumberOfBytes - 1);
    UINTN FlashBlock;

    FlashDeviceWriteEnable();

    for (FlashBlock = FlashBlockStart; FlashBlock <= FlashBlockEnd; FlashBlock += FLASH_BLOCK_SIZE) {
        FlashBlockWriteEnable((UINT8*)FlashBlock);
    }


    bStatus = FlashProgram((UINT8*)Address, Buffer, (UINT32)NumberOfBytes);


    for (FlashBlock = FlashBlockStart; FlashBlock <= FlashBlockEnd; FlashBlock += FLASH_BLOCK_SIZE) {
        FlashBlockWriteDisable((UINT8*)FlashBlock);
    }

    FlashDeviceWriteDisable();

    return bStatus ? EFI_SUCCESS : EFI_DEVICE_ERROR;
}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2008, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
