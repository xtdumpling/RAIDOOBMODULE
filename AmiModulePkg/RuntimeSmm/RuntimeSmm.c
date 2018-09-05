//**********************************************************************
// $Header: $
//
// $Revision: $
//
// $Date: $
//**********************************************************************
//<AMI_FHDR_START>
//
// Name:  RuntimeSmm.c
//
// Description:	Installs SMM copy of the Runtime Services Table
//
//<AMI_FHDR_END>
//**********************************************************************

//=============================================================================
// Includes
#include <Protocol/Runtime.h>
#include <Protocol/LoadedImage.h>
#include <AmiDxeLib.h>

//**********************************************************************
// RT SMM BEGIN
//**********************************************************************
//======================================================================
// SMM externs
extern EFI_GUID SmmRsTableGuid;

//======================================================================
// SMM Function Prototypes

//======================================================================
// SMM Function Definitions
//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:	DummySmmRtFunction
//
// Description:	
//  This function is a Dummy function that is used as a default function
//  for the SMM instance of the Runtime Services Table
//
// Input:   
//  None
//
// Output:
//  EFI_UNSUPPORTED
//
// Notes:  
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS DummySmmRtFunction(){ return EFI_UNSUPPORTED; }

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:	RuntimeSmmInitialize
//
// Description:	
//  This function initializes the SMM version of the Runtime Services Table
//
// Input:   
//  EFI_HANDLE ImageHandle - The firmware allocated handle for this driver
//  EFI_SYSTEM_TABLE *SystemTable - Pointer to the UEFI SystemTable
//
// Output:
//  EFI_STATUS - based on the return values of the SmmInstallConfigurationTable function
//
// Notes:  
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS RuntimeSmmInitialize(
    EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable
)
{
    EFI_RUNTIME_SERVICES 	*SmmRs;
    EFI_STATUS      Status;
    EFI_HANDLE      Handle = NULL;
    VOID **p;

    Status = pSmst->SmmAllocatePool(EfiRuntimeServicesData, sizeof(EFI_RUNTIME_SERVICES), &SmmRs);
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status))
        return Status;
        
    //copy header
    SmmRs->Hdr = pRS->Hdr;
    //Init Runtime Services function pointers with our dummy function
    for(p = (VOID**)((UINT8*)SmmRs + sizeof(SmmRs->Hdr)); p < (VOID**)(SmmRs + 1); p++)
        *p = DummySmmRtFunction;

    // install the SMM version of the Runtime Services Table SMM Configuration Table
    Status = pSmst->SmmInstallConfigurationTable(
        pSmst, &SmmRsTableGuid, SmmRs, sizeof(EFI_RUNTIME_SERVICES) );
    ASSERT_EFI_ERROR(Status);
    
    Status = pBS->InstallMultipleProtocolInterfaces (
        &Handle,
        &SmmRsTableGuid,
        SmmRs,
        NULL
    );

    return Status;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:	RuntimeEntry
//
// Description:	
//  This function is the entry point for the Runtime Driver.  It initializes
//  the AMI Library and then it calls the Initialization functions for both
//  the SMM and NON-SMM versions of the Runtime Driver.  It then calls the 
//  InitParts2 function which calls the functions in the RuntimeSmmInitialize
//  eLink.
//
// Input:   
//  EFI_HANDLE ImageHandle - The firmware allocated handle for this driver
//  EFI_SYSTEM_TABLE *SystemTable - Pointer to the UEFI SystemTable
//
// Output:
//  EFI_STATUS - based on the return values of the InitSmmHandlerEx function
//
// Notes:  
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS EFIAPI RuntimeEntry (
    IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable
)
{
    EFI_STATUS Status;

    // Initialize the AMI Library for this module
	InitAmiLib(ImageHandle, SystemTable);
    
    // Initialize both the SMM and Non-SMM verions of the Runtime Services
	Status = InitSmmHandlerEx(
        ImageHandle, SystemTable, RuntimeSmmInitialize, NULL
    );
    return Status;
}
//**********************************************************************
// RT SMM END
//**********************************************************************

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2012, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
