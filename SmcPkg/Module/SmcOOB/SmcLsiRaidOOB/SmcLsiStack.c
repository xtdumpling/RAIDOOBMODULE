//****************************************************************************
//****************************************************************************
//**            (C)Copyright 1993-2018 Supermicro Computer, Inc.            **
//****************************************************************************
//****************************************************************************
//  File History
//
//  Rev. 1.00
//    Bug Fix:  Initial revision.
//    Reason:
//    Auditor:  Durant Lin
//    Date:     Sep/28/2018
//
//****************************************************************************
#include "SmcLsiStack.h"
#include <AmiLib.h>
#include <AmiDxeLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Uefi.h>
#include <Library/UefiLib.h>

static SMC_RAID_STACK*	SmcStackPoint = NULL;

EFI_STATUS	SmcStackPush(UINT64	Input){

	EFI_STATUS		Status = EFI_SUCCESS;
	SMC_RAID_STACK*	NewPush = NULL;

	Status = gBS->AllocatePool(EfiBootServicesData,sizeof(SMC_RAID_STACK),&NewPush);
	
	if(!EFI_ERROR(Status)){
		NewPush->data = Input;
		NewPush->pNext = SmcStackPoint;
		SmcStackPoint  = NewPush;
	}

	return Status;
}

EFI_STATUS	SmcStackPop(UINT64* OutPut){

	EFI_STATUS		Status = EFI_SUCCESS;
	SMC_RAID_STACK*	OldPop = NULL;
	
	if(!(!!OutPut)) return EFI_INVALID_PARAMETER;

	OldPop = SmcStackPoint;
	SmcStackPoint = SmcStackPoint->pNext;

	*OutPut = OldPop->data;
	Status = gBS->FreePool(OldPop);

	return Status;
}
