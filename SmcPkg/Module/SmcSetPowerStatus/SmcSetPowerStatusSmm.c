//****************************************************************************
//**            (C)Copyright 1993-2015 Supermicro Computer, Inc.            **
//****************************************************************************
//****************************************************************************
//  File History
//
//  Rev. 1.00
//    Bug Fix:  Initialize revision.
//    Reason:   
//    Auditor:  Leon Xu
//    Date:     Dec/28/2014
//**************************************************************************//
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Protocol/SmmPowerButtonDispatch2.h>
#include <Protocol/SmmSxDispatch2.h>
#include <Library/SmmServicesTableLib.h>
#include <Setup.h>
#include "SmcLib.h"
#include "SmcCspLibSb.h"

BOOLEAN  gSmcLastState;

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure: SxSleepHandler
//
// Description:
//  S5 sleep SMI handler. Modify AFTERG3_EN depended on BIOS setting.
//
// Input:
//  DispatchHandle  - Points to the handler
//  Context         - Points to an optional context buffer
//  CommBuffer      - Points to the optional communication buffer
//  CommBufferSize  - Points to the size of the optional communication buffer
//
// Output:      
//  EFI_SUCCESS     - Protocol successfully started and installed
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
SxSleepHandler(
    IN	EFI_HANDLE	DispatchHandle,
    IN	CONST EFI_SMM_SX_REGISTER_CONTEXT	*DispatchContext,
    IN OUT	VOID	*CommonBuffer,
    IN OUT	UINTN	*CommonBufferSize
)
{
    EFI_STATUS	Status = EFI_SUCCESS;

    Status = AfterG3Setting(gSmcLastState);

    switch(DispatchContext->Type){
    case SxS4:
	break;
    case SxS5:
	break;
    default:
	break;
    }

    return Status;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure: PowerButtonHandler
//
// Description:
//  Power button SMI handler. Modify AFTERG3_EN depended on BIOS setting
//
// Input:
//  DispatchHandle  - Points to the handler
//  Context         - Points to an optional context buffer
//  CommBuffer      - Points to the optional communication buffer
//  CommBufferSize  - Points to the size of the optional communication buffer
//
// Output:      
//  EFI_SUCCESS     - Protocol successfully started and installed
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
PowerButtonHandler(
    IN EFI_HANDLE    DispatchHandle,
    IN CONST VOID    *Context         OPTIONAL,
    IN OUT VOID      *CommBuffer      OPTIONAL,
    IN OUT UINTN     *CommBufferSize  OPTIONAL
)
{
    EFI_STATUS	Status = EFI_SUCCESS;
	
    Status = AfterG3Setting(gSmcLastState);
	
    return Status;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure: SmcSetPowerStatusSmmEntry
//
// Description:
//  Register sleep SMI and power button SMI. Set last state flag depend on BIOS
//  setting
//
// Input:
//  ImageHandle     - Handle for the image of this driver
//  SystemTable     - Pointer to the EFI System Table
//
// Output:      
//  EFI_SUCCESS     - Protocol successfully started and installed
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
SmcSetPowerStatusSmmEntry(
    IN	EFI_HANDLE        ImageHandle,
    IN	EFI_SYSTEM_TABLE  *SystemTable
)
{
    EFI_STATUS	Status;
    EFI_HANDLE	DispatchHandle, SxDispatchHandle;
    EFI_SMM_SX_REGISTER_CONTEXT	SxDispatchContext;
    EFI_SMM_SX_DISPATCH2_PROTOCOL	*pEfiSmmSxDispatch2Protocol;
    EFI_SMM_POWER_BUTTON_REGISTER_CONTEXT	Context = {EfiPowerButtonEntry};
    EFI_SMM_POWER_BUTTON_DISPATCH2_PROTOCOL	*pEfiSmmPowerButtonDispatch2Protocol;   
    EFI_GUID	SetupGuid = SETUP_GUID;
    SETUP_DATA	SetupData;
    UINTN	Size;    
    
    Status = gSmst->SmmLocateProtocol(
		    &gEfiSmmPowerButtonDispatch2ProtocolGuid,
		    NULL,
		    &pEfiSmmPowerButtonDispatch2Protocol);

    if(EFI_ERROR(Status))	return EFI_SUCCESS;

    Status = pEfiSmmPowerButtonDispatch2Protocol->Register(
		    pEfiSmmPowerButtonDispatch2Protocol,
		    PowerButtonHandler,
		    &Context,
		    &DispatchHandle);

    if(EFI_ERROR(Status))	return EFI_SUCCESS;

    Status = gSmst->SmmLocateProtocol(
		    &gEfiSmmSxDispatch2ProtocolGuid,
		    NULL,
		    &pEfiSmmSxDispatch2Protocol);

    if(EFI_ERROR(Status))	return EFI_SUCCESS;

    SxDispatchContext.Type = SxS5;
    SxDispatchContext.Phase = SxEntry;
    Status = pEfiSmmSxDispatch2Protocol->Register(
		    pEfiSmmSxDispatch2Protocol,
		    SxSleepHandler,
		    &SxDispatchContext,
		    &SxDispatchHandle);

    if(EFI_ERROR(Status))	return EFI_SUCCESS;    

    SxDispatchContext.Type = SxS4;
    SxDispatchContext.Phase = SxEntry;
    Status = pEfiSmmSxDispatch2Protocol->Register(
		    pEfiSmmSxDispatch2Protocol,
		    SxSleepHandler,
		    &SxDispatchContext,
		    &SxDispatchHandle);

    if(EFI_ERROR(Status))	return EFI_SUCCESS;    

    Size = sizeof(SETUP_DATA);
    
    Status = gRT->GetVariable(
		    L"Setup", 
		    &SetupGuid, 
		    NULL, 
		    &Size, 
		    &SetupData);

    if(EFI_ERROR(Status))	return EFI_SUCCESS;

    gSmcLastState = TRUE;
    
    if(SetupData.SmcAfterG3Save == 0x01)	gSmcLastState = FALSE;

    return EFI_SUCCESS;
}

