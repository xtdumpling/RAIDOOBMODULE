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


EFI_STATUS	SmcStackPush(SMC_RAID_STACK* pSmcStack, UINT64	Input, UINT8 InDataType){

	EFI_STATUS			Status = EFI_SUCCESS;
	SMC_RAID_STACK_SET*	NewPush = NULL;

	if(!(!!pSmcStack)) return EFI_INVALID_PARAMETER;
	Status = gBS->AllocatePool(EfiBootServicesData,sizeof(SMC_RAID_STACK_SET),&NewPush);
	MemSet(NewPush,sizeof(SMC_RAID_STACK_SET),0x00);

	if(!EFI_ERROR(Status)){
		NewPush->StackBody.data		= Input;
		NewPush->StackBody.dataType	= InDataType;
		NewPush->pNext 				= pSmcStack->SmcStackPoint;
		pSmcStack->SmcStackPoint 	= NewPush;
	}

	return Status;
}

EFI_STATUS	SmcStackPop(SMC_RAID_STACK* pSmcStack, SMC_RAID_STACK_BODY* OutPut){

	EFI_STATUS			Status = EFI_SUCCESS;
	SMC_RAID_STACK_SET*	OldPop = NULL;
	
	if(!(!!OutPut)) return EFI_INVALID_PARAMETER;
	if(!(!!pSmcStack)) return EFI_INVALID_PARAMETER;

	if(!(!!pSmcStack->SmcStackPoint)) return EFI_OUT_OF_RESOURCES;

	OldPop 						= pSmcStack->SmcStackPoint;
	pSmcStack->SmcStackPoint 	= OldPop->pNext;

	MemCpy(OutPut,&OldPop->StackBody,sizeof(SMC_RAID_STACK_BODY));
	Status = gBS->FreePool(OldPop);

	return Status;
}

EFI_STATUS InitialSmcStack(SMC_RAID_STACK* pSmcStack){

	SMC_RAID_STACK_SET*	OldPop = NULL;

	if(!(!!pSmcStack)) return EFI_INVALID_PARAMETER;

	while(!!pSmcStack->SmcStackPoint){
		OldPop = pSmcStack->SmcStackPoint->pNext;
		gBS->FreePool(pSmcStack->SmcStackPoint);
		pSmcStack->SmcStackPoint = OldPop;
	}
	pSmcStack->SmcStackPoint = NULL;

	return EFI_SUCCESS;
}
