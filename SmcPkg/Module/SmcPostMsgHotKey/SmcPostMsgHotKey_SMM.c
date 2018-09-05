//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2015 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//****************************************************************************
//  File History
//
//     Rev. 1.00
//       Reason:	Rewrite SmcPostMsgHotKey
//       Auditor:       Leon Xu 
//       Date:          Dec/19/2014
//
//****************************************************************************
//****************************************************************************
#include <Token.h>
#include <Setup.h>
#include <AmiDxeLib.h>
#include <AmiCspLib.h>
#include <Protocol\SmmBase2.h>
#include <Protocol\SmmSwDispatch2.h>
#include <Protocol\SmmCpu.h>

#define SMC_SIGNATURE 0x25534d43	// $SMC

EFI_STATUS InSmmFunction (
    IN EFI_HANDLE           ImageHandle,
    IN EFI_SYSTEM_TABLE     *SystemTable )
{
	EFI_STATUS	Status = EFI_SUCCESS;
	Status = InitAmiSmmLib (ImageHandle, SystemTable);
	if (EFI_ERROR(Status))
		return Status;    
	
	TRACE((TRACE_ALWAYS, "[LEON]InSmmFunction.\n"));

	return Status;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   SmcPostMsgHotKey_SMM_Init
//
// Description: Installs CRB SMM Child Dispatcher Handler.
//
// Input:       ImageHandle - Image handle
//              *SystemTable - Pointer to the system table
//
// Output:      EFI_STATUS
//              EFI_NOT_FOUND - The SMM Base protocol is not found.
//              EFI_SUCCESS   - Installs CRB SMM Child Dispatcher Handler
//                              successfully.
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS SmcPostMsgHotKey_SMM_Init (
    IN EFI_HANDLE           ImageHandle,
    IN EFI_SYSTEM_TABLE     *SystemTable )
{
	InitAmiLib(ImageHandle, SystemTable);
	TRACE((TRACE_ALWAYS, "SmcPostMsgHotKey_SMM_Init\n"));
	return InitSmmHandler(ImageHandle, SystemTable, InSmmFunction, NULL);
}

