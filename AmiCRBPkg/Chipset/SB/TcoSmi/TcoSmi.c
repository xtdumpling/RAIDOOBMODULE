//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

/** @file TcoSmi.c
    This file register Tco Smi Handler to IchnDispatch

**/

//----------------------------------------------------------------------------
#include <Token.h>
#include <AmiDxeLib.h>
#include <Protocol/SmmBase2.h>
#include <Protocol/PchTcoSmiDispatch.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
//----------------------------------------------------------------------------

/**
    Handle TcoSmi generated

    @param  DispatchHandle,
**/
VOID
EFIAPI CenturyRollOverSmiHandler (
    IN  EFI_HANDLE                      DispatchHandle)
{
    UINT8   Century;
    UINT8   Value;
    
    //DEBUG((EFI_D_INFO, "### CenturyRollOverSmiHandler ###\n"));
    IoWrite8(CMOS_ADDR_PORT, ACPI_CENTURY_CMOS);
    Century = IoRead8(CMOS_DATA_PORT); // Read register.
    
    Value = BcdToDecimal8(Century);
    Value++;
    Century = DecimalToBcd8(Value);
    
    IoWrite8(CMOS_ADDR_PORT, ACPI_CENTURY_CMOS);
    IoWrite8(CMOS_DATA_PORT, Century);
}

/**
    In SMM Function for TcoSmi SMM driver.

    @param[in] ImageHandle  Image handle of this driver.
    @param[in] SystemTable  A Pointer to the EFI System Table.

    @return EFI_STATUS
    @retval EFI_SUCCESS
**/
EFI_STATUS
InSmmFunction (
    IN EFI_HANDLE           ImageHandle,
    IN EFI_SYSTEM_TABLE     *SystemTable )
{
    EFI_STATUS                              Status;
    PCH_TCO_SMI_DISPATCH_PROTOCOL           *mPchTcoSmiDispatchProtocol;
    EFI_HANDLE                              Handle;

    Status = InitAmiSmmLib (ImageHandle, SystemTable);
    if (EFI_ERROR(Status)) return Status;
    
    mPchTcoSmiDispatchProtocol = NULL;
    Status = gSmst->SmmLocateProtocol (&gPchTcoSmiDispatchProtocolGuid, NULL, (VOID **) &mPchTcoSmiDispatchProtocol);
    if (EFI_ERROR(Status)) return Status;
    
    Handle = NULL;
    Status = mPchTcoSmiDispatchProtocol->NewCenturyRegister(mPchTcoSmiDispatchProtocol, CenturyRollOverSmiHandler, Handle);

    return Status;
}

/**
    Entry Point for TcoSmi SMM driver.

    @param[in] ImageHandle  Image handle of this driver.
    @param[in] SystemTable  A Pointer to the EFI System Table.

    @return EFI_STATUS
    @retval EFI_SUCCESS
**/
EFI_STATUS
EFIAPI
InitializeTcoSmm (
    IN EFI_HANDLE           ImageHandle,
    IN EFI_SYSTEM_TABLE     *SystemTable )
{
    InitAmiLib(ImageHandle, SystemTable);
    return InitSmmHandler(ImageHandle, SystemTable, InSmmFunction, NULL);
}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
