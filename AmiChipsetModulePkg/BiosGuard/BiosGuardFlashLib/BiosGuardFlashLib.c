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
/** @file BiosGuardFlashLib.c
    BiosGuard Flash update routines

**/

#include <Efi.h>
#include <FlashPart.h>
#include "Token.h"

#include <AmiDxeLib.h>
#include <AmiCspLib.h>
#include <Protocol/BiosGuardProtocol.h>
#include <CpuRegs.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Register/PchRegsSpi.h>

#ifndef MSR_SPCL_CHIPSET_USAGE_ADDR
#define  MSR_SPCL_CHIPSET_USAGE_ADDR 0x1FE
#endif

typedef BOOLEAN (IDENTIFY)(
    volatile UINT8*     pBlockAddress,
    FLASH_PART          **Struct
);

FLASH_PART              *FlashInitialized = NULL;
BIOSGUARD_PROTOCOL      *BiosGuardInstance = NULL;
extern IDENTIFY*        FlashList[];
EFI_SMM_SYSTEM_TABLE2   *BiosGuardSmst = NULL;
EFI_SMM_BASE2_PROTOCOL  *BiosGuardSmmBase2 = NULL;

EFI_STATUS
BiosGuardFlashEraseCommandEx   (
    IN volatile UINT8       *pBlockAddress,
    IN UINT32               Length
);

BOOLEAN
BiosGuardFlashEraseCommand   (
    volatile UINT8          *pBlockAddress
);

BOOLEAN
BiosGuardProgramCommand (
    volatile UINT8          *pByteAddress,
    UINT8                   *Byte,
    UINT32                  *Length
);

VOID
BiosGuardReadCommand    (
    volatile UINT8          *pByteAddress,
    UINT8                   *Byte,
    UINT32                  *Length
);

BOOLEAN
BiosGuardIsEraseCompleted   (
    IN  volatile UINT8      *pBlockAddress,
    OUT BOOLEAN             *pError,
    OUT UINTN               *pStatus
);

BOOLEAN
BiosGuardProgramCompleted (
    volatile UINT8          *pByteAddress,
    UINT8                   *Byte,
    UINT32                  Length,
    BOOLEAN                 *pError,
    UINTN                   *pStatus
);

VOID
BiosGuardBlockWriteEnable(
    UINT8                   *pBlockAddress
);

VOID
BiosGuardBlockWriteDisable  (
    UINT8                   *pBlockAddress
);

VOID
BiosGuardDeviceWriteEnable  (
    VOID
);

VOID
BiosGuardDeviceWriteDisable (
    VOID
);

VOID
BiosGuardDeviceVirtualFixup (
    EFI_RUNTIME_SERVICES    *pRS
);

#define SECTOR_SIZE_4KB             0x1000  // Common 4kBytes sector size

FLASH_PART mBiosGuardFlash =
{
    BiosGuardReadCommand,
    BiosGuardFlashEraseCommand,
    BiosGuardFlashEraseCommandEx,
    BiosGuardProgramCommand,
    BiosGuardBlockWriteEnable,
    BiosGuardBlockWriteDisable,
    BiosGuardDeviceWriteEnable,
    BiosGuardDeviceWriteDisable,
    BiosGuardDeviceVirtualFixup,
    1,
    SECTOR_SIZE_4KB,
    NULL
};

/**
    Identifies whether BiosGuardFlash library should control access to
    SPI flash part

    @param pBlockAddress Address to access flash part
    @param FlashStruc Generic interface pointer for the flash part

    @retval BOOLEAN

**/
BOOLEAN
IdentifyBiosGuardFlash (
    IN  volatile UINT8 *pBlockAddress,
    OUT FLASH_PART     **FlashStruc
)
{
    UINTN         i;
    BOOLEAN       found = FALSE;

    if ( !((AsmReadMsr64 (EFI_PLATFORM_INFORMATION) & B_MSR_PLATFORM_INFO_BIOSGUARD_AVAIL) &&
            (AsmReadMsr64 (MSR_PLAT_FRMW_PROT_CTRL) & B_MSR_PLAT_FRMW_PROT_CTRL_EN)) ) {
        //
        // BIOS Guard is disabled or not supported
        //
    	DEBUG((EFI_D_INFO, "[BiosGuardFlashLib.c] BIOS Guard is disabled. \n"));
        return FALSE;
    } else {
    	DEBUG((EFI_D_INFO, "[BiosGuardFlashLib.c] BIOS Guard is enabled. \n"));
    }

    if ( FlashInitialized == NULL ) {
        for(i=1; !found && FlashList[i]; i++) {
            found=FlashList[i](pBlockAddress, &FlashInitialized);
        }
    }

    if ( FlashInitialized == NULL ) return FALSE;

    if(found) {
        *FlashStruc = &mBiosGuardFlash;
    }

    return found;
}

/**
    Verifies whether BiosGuard library has been initialized or not

    @param VOID

    @retval BOOLEAN

**/
BOOLEAN
IsBiosGuardFlashInitialized()
{    
    if (BiosGuardSmst == NULL) return FALSE;
    
    if( BiosGuardInstance == NULL ) {         
        BiosGuardSmst->SmmLocateProtocol (&gSmmBiosGuardProtocolGuid, NULL, &BiosGuardInstance);
    }
    
    if( BiosGuardInstance != NULL ) {
        return TRUE;
    }

    BiosGuardInstance = NULL;
    return FALSE;
}

/**
    Erases SPI via BiosGuard library

    @param pBlockAddress Address to access flash part

    @retval VOID

**/
EFI_STATUS
BiosGuardFlashEraseCommandEx   (
    IN volatile UINT8*      pBlockAddress,
    IN UINT32               Length
)
{
    EFI_STATUS Status;

    if(IsBiosGuardFlashInitialized()) {
#if BIOS_GUARD_DEBUG_MODE	    
    	DEBUG((EFI_D_INFO, "[BiosGuardFlashLib.c] BiosGuardFlashEraseCommand = %lX \n", (UINTN)(pBlockAddress - BIOS_GUARD_FLASH_START_ADDRESS)));
#endif        
        BiosGuardInstance->Erase( BiosGuardInstance, (UINTN)(pBlockAddress - BIOS_GUARD_FLASH_START_ADDRESS) );

        Status = BiosGuardInstance->Execute( BiosGuardInstance, FALSE );
        if( EFI_ERROR(Status) ) {
#if BIOS_GUARD_DEBUG_MODE
            DEBUG((EFI_D_INFO, "[BiosGuardFlashLib.c] BiosGuardFlashEraseCommand RETRY. \n"));
#endif
            BiosGuardInstance->Erase( BiosGuardInstance, (UINTN)(pBlockAddress - BIOS_GUARD_FLASH_START_ADDRESS) );

            Status = BiosGuardInstance->Execute( BiosGuardInstance, FALSE );
            ASSERT_EFI_ERROR (Status);
        }
    } else {
#if BIOS_GUARD_DEBUG_MODE
    	DEBUG((EFI_D_INFO, "[BiosGuardFlashLib.c] Non-BiosGuardFlashEraseCommand. \n"));
#endif        
        if ( FlashInitialized == NULL) return EFI_NOT_READY;
        Status = FlashInitialized->FlashEraseCommandEx(pBlockAddress, Length);
    }
    return Status;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------
// Procedure:   BiosGuardFlashEraseCommand
//
// Description: Erases SPI via BiosGuard library
//
// Input:   IN volatile UINT8*      pBlockAddress
//
// Output:  VOID
//
// Returns: VOID
//
//----------------------------------------------------------------------
//<AMI_PHDR_END>
BOOLEAN
BiosGuardFlashEraseCommand   (
    IN volatile UINT8*      pBlockAddress
)
{
    return (EFI_ERROR(BiosGuardFlashEraseCommandEx(\
                        pBlockAddress, FlashBlockSize))) ? FALSE : TRUE;
}


/**
    Checks whether BiosGuard erase command was completed

    @param pBlockAddress Address to access flash part
    @param pError 
    @param pStatus 

    @retval BOOLEAN

**/
BOOLEAN
BiosGuardIsEraseCompleted (
    IN  volatile UINT8          *pBlockAddress,
    OUT BOOLEAN                 *pError,
    OUT UINTN                   *pStatus
)
{
    UINT32                      dNumBytes;
    
#if BIOS_GUARD_DEBUG_MODE
    DEBUG((EFI_D_INFO, "[BiosGuardFlashLib.c] BiosGuardIsEraseCompleted. \n"));
#endif

    for ( dNumBytes = 0; dNumBytes < SECTOR_SIZE_4KB; dNumBytes++ ) {
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
    Programs SPI part via BiosGuard library

    @param pByteAddress
        Byte
        Length

    @retval VOID

**/
BOOLEAN
BiosGuardProgramCommand (
    IN volatile UINT8*      pByteAddress,
    IN UINT8                *Byte,
    IN UINT32               *Length
)
{
    EFI_STATUS Status;

    if(IsBiosGuardFlashInitialized()) {
#if BIOS_GUARD_DEBUG_MODE	    
    	DEBUG((EFI_D_INFO, "[BiosGuardFlashLib.c] BiosGuardProgramCommand = %lX \n", (UINTN)(pByteAddress - BIOS_GUARD_FLASH_START_ADDRESS)));
#endif        
        BiosGuardInstance->Write(BiosGuardInstance,
                                 (UINTN)(pByteAddress - BIOS_GUARD_FLASH_START_ADDRESS),
                                 *Length, Byte);

        Status = BiosGuardInstance->Execute(BiosGuardInstance, FALSE);
        if( EFI_ERROR(Status) ) {
#if BIOS_GUARD_DEBUG_MODE        	
            DEBUG((EFI_D_INFO, "[BiosGuardFlashLib.c] BiosGuardProgramCommand RETRY. \n"));
#endif            
            BiosGuardInstance->Write(BiosGuardInstance,
                                     (UINTN)(pByteAddress - BIOS_GUARD_FLASH_START_ADDRESS),
                                     *Length, Byte);

            Status = BiosGuardInstance->Execute(BiosGuardInstance, FALSE);
            ASSERT_EFI_ERROR (Status);
            if (EFI_ERROR(Status)) return FALSE;
        }

        *Length = 0;
    } else {
#if BIOS_GUARD_DEBUG_MODE	    
    	DEBUG((EFI_D_INFO, "[BiosGuardFlashLib.c] Non-BiosGuardProgramCommand. \n"));
#endif        
        if ( FlashInitialized == NULL) return FALSE;
        return FlashInitialized->FlashProgramCommand(pByteAddress, Byte, Length);
    }
    return TRUE;
}

/**
    Reads the SPI flash part via BiosGuard library

    @param pBlockAddress Address to access flash part
    @param Byte pointer to data to write to the flash part
    @param Length The total amount of data that Byte points to

    @retval VOID

**/
VOID
BiosGuardReadCommand    (
    volatile UINT8          *pByteAddress,
    UINT8                   *Byte,
    UINT32                  *Length
)
{
    UINT32              dNumBytes = 0;

    for ( dNumBytes = 0; dNumBytes < *Length ; dNumBytes++ )
        *( Byte + dNumBytes ) = *(UINT8*)((UINTN)pByteAddress + dNumBytes );

    *Length = 0;
    return ;
}

/**
    Checks whether BiosGuard Flash program was completed


    @param pBlockAddress Address to access flash part
    @param Byte values previously written to the Flash Device
    @param Length The amount of data that needs to be checked
    @param pError Boolean that tells if fatal error occured
    @param pStatus Status of the erase command

    @retval TRUE Program completed, check pError for fatal error
    @retval FALSE programming in progress

**/
BOOLEAN
BiosGuardProgramCompleted (
    IN  volatile UINT8*     pByteAddress,
    IN  UINT8               *Byte,
    IN  UINT32              Length,
    OUT BOOLEAN             *pError,
    OUT UINTN               *pStatus
)
{
    UINT32                      dNumBytes;
    UINT8                       bByte;

#if BIOS_GUARD_DEBUG_MODE
    DEBUG((EFI_D_INFO, "[BiosGuardFlashLib.c] BiosGuardProgramCompleted. \n"));
#endif    

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
    This function chooses the correct flash part to call and
    then enables write operations(erase/programming) for a specific block

    @param pBlockAddress Address to access flash part

    @retval VOID

**/
VOID
BiosGuardBlockWriteEnable(
    UINT8      *pBlockAddress
)
{
    if (IsBiosGuardFlashInitialized())return;
    if ( FlashInitialized == NULL)return;
    FlashInitialized->FlashBlockWriteEnable(pBlockAddress);
}

/**
    This function chooses the correct flash part to call and
    then disables write operations(erase/programming) for a specific
    block

    @param pBlockAddress Address to access flash part

    @retval VOID

**/
VOID
BiosGuardBlockWriteDisable (
    IN  UINT8          *pBlockAddress
)
{
    if (IsBiosGuardFlashInitialized())return;
    if ( FlashInitialized == NULL)return;
    FlashInitialized->FlashBlockWriteDisable(pBlockAddress);
}

/**
    This function chooses the correct flash part to call and
    then enables write operation for a flash device

    @param VOID

    @retval VOID

**/
VOID
BiosGuardDeviceWriteEnable (VOID)
{
    if (IsBiosGuardFlashInitialized())return;
    if ( FlashInitialized == NULL)return;
    FlashInitialized->FlashDeviceWriteEnable();
}

/**
    This function chooses the correct flash part to call and
    then disables write operation for a flash device

    @param VOID

    @retval VOID

    @retval VOID

**/
VOID
BiosGuardDeviceWriteDisable(VOID)
{
    if (IsBiosGuardFlashInitialized())return;
    if ( FlashInitialized == NULL)return;
    FlashInitialized->FlashDeviceWriteDisable();
}

/**
    Fixup global data for for a virtual address space.
    This routine must be called by the library consumer in the
    EVT_SIGNAL_VIRTUAL_ADDRESS_CHANGE event handler

    @param pRS pointer to the Runtime Services Table

    @retval VOID

    @retval VOID

**/
VOID
BiosGuardDeviceVirtualFixup   (
    IN EFI_RUNTIME_SERVICES *pRS
)
{
    return;	
}


EFI_STATUS
EFIAPI
BiosGuardFlashLibConstructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
    EFI_STATUS         Status;

#if BIOS_GUARD_DEBUG_MODE    
        DEBUG((EFI_D_INFO, "[BiosGuardFlashLib.c] BiosGuardFlashLibConstructor Start\n"));
#endif        
        
    if ( !((AsmReadMsr64 (EFI_PLATFORM_INFORMATION) & B_MSR_PLATFORM_INFO_BIOSGUARD_AVAIL) &&
           (AsmReadMsr64 (MSR_PLAT_FRMW_PROT_CTRL) & B_MSR_PLAT_FRMW_PROT_CTRL_EN)) ) {
        //
        // BIOS Guard is disabled or not supported
        //
#if BIOS_GUARD_DEBUG_MODE	    
        DEBUG((EFI_D_INFO, "[BiosGuardFlashLib.c - BiosGuardFlashLibConstructor] BIOS Guard  is disabled. \n"));
#endif        
        return EFI_SUCCESS;
    }
    
    if ( BiosGuardSmst == NULL ) {
        if( BiosGuardSmmBase2 == NULL ) {
            Status = gBS->LocateProtocol( &gEfiSmmBase2ProtocolGuid, NULL, &BiosGuardSmmBase2 );
            if ( EFI_ERROR(Status) ) return EFI_SUCCESS;            
        }
    
        BiosGuardSmmBase2->GetSmstLocation (BiosGuardSmmBase2, &BiosGuardSmst);
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
