//****************************************************************************
//****************************************************************************
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//****************************************************************************
//****************************************************************************
//  File History
//
//  Rev. 1.01
//    Bug Fix:  
//    Reason:   Add HttpBoot OOB function support.
//    Auditor:  Durant Lin
//    Date:     Apr/03/2018
//
//  Rev. 1.00
//    Bug Fix:  
//    Reason:   For platform handle special nvram callback.
//    Auditor:  Durant Lin
//    Date:     Jan/17/2018
//
//****************************************************************************

#include "PlatformSpecificFuncLib.h"
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/PrintLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiLib.h>
#include <Library/HiiLib.h>
#include <Library/BaseMemoryLib.h>
#include <NetworkStackSetup.h>

#if defined(MEHLOW_SGX_EXPOSE_SUPPORT) && (MEHLOW_SGX_EXPOSE_SUPPORT == 1)
EFI_GUID SetupGuid = SETUP_GUID;

#define SGX_EPOCH_0 0x553DFD8D5FA48F27
#define SGX_EPOCH_1 0xD76767B9BE4BFDC1

EFI_STATUS MehlowSgxOOBCallbackFunc(
    IN CHAR16 	*VariableName,
    IN EFI_GUID	*VariableGuid,
    IN UINTN 	VariableSize,
    IN UINT8 	*VariableBuffer
){

	SETUP_DATA *Temp = (SETUP_DATA*)VariableBuffer;
  	BOOLEAN                 RngSuccess;
  	UINT64                  RandomEpoch;
	if(Wcscmp(VariableName,L"Setup")  == 0 && MemCmp(VariableGuid,&SetupGuid,sizeof(EFI_GUID)) == 0){
		if (Temp->EpochUpdate == 0x1) {

			RngSuccess    = FALSE;
			RandomEpoch   = 0;
			RngSuccess = GetRandomNumber64 (&RandomEpoch);
        	if (RngSuccess) {
          		Temp->SgxEpoch0 = RandomEpoch;
         		DEBUG ((DEBUG_ERROR, "SgxEpoch0: %016llx\n", Temp->SgxEpoch0));
        	} 
			RngSuccess = GetRandomNumber64 (&RandomEpoch);
        	if (RngSuccess) {
          		Temp->SgxEpoch1 = RandomEpoch;
          		DEBUG ((DEBUG_ERROR, "SgxEpoch1: %016llx\n", Temp->SgxEpoch1));
        	}
			Temp->EpochUpdate = 0x0;
		}
        if (Temp->EpochToFactory == 0x1) {
			Temp->SgxEpoch0 = SGX_EPOCH_0;
			Temp->SgxEpoch1 = SGX_EPOCH_1;
			Temp->EpochToFactory = 0x0;
		}
	}

	return EFI_SUCCESS;
}
#endif

#if defined(SmcHttpBoot_SUPPORT) && (SmcHttpBoot_SUPPORT == 1)
EFI_STATUS
HttpBootCheckUriScheme (
  IN      CHAR8                  *Uri
  )
{
  UINTN                Index;
  EFI_STATUS           Status;

  Status = EFI_SUCCESS;

  for (Index = 0; Index < AsciiStrLen (Uri); Index++) {
    if (Uri[Index] == ':') {
      break;
    }
    if (Uri[Index] >= 'A' && Uri[Index] <= 'Z') {
      Uri[Index] -= (CHAR8)('A' - 'a');
    }
  }

  if ((AsciiStrnCmp (Uri, "http://", 7) != 0) && (AsciiStrnCmp (Uri, "https://", 8) != 0)) {
    DEBUG ((-1, "HttpBootCheckUriScheme: Invalid Uri.\n"));
    return EFI_INVALID_PARAMETER;
  }
  
  return Status;
}

EFI_STATUS SmcHttpBootCallback(
    IN CHAR16 	*VariableName,
    IN EFI_GUID	*VariableGuid,
    IN UINTN 	VariableSize,
    IN UINT8 	*VariableBuffer
){
	EFI_STATUS			Status = EFI_SUCCESS;
	SETUP_DATA 			*Temp = NULL;
	EFI_GUID 			hSetupGuid = SETUP_GUID;
	CHAR8*				Uris = NULL;
  	NETWORK_STACK		NetworkStackVar;
	UINTN				NetworkStackSize;
	EFI_GUID			NetworkStackGuid = NETWORK_STACK_GUID;
	UINT32				NetworkAttr = 0;

	if(Wcscmp(VariableName,L"Setup")  == 0 && MemCmp(VariableGuid,&hSetupGuid,sizeof(EFI_GUID)) == 0){
		Temp = (SETUP_DATA*)VariableBuffer;
		gBS->AllocatePool(EfiBootServicesData,((Wcslen(Temp->HttpUri)+2)*sizeof(CHAR8)),(VOID**)&Uris);
		MemSet(Uris,((Wcslen(Temp->HttpUri)+2)*sizeof(CHAR8)),0x00);
		UnicodeStrToAsciiStrS(Temp->HttpUri,Uris,(Wcslen(Temp->HttpUri)+1));
		Status = HttpBootCheckUriScheme(Uris);
		if(EFI_ERROR(Status)){
			MemSet(Temp->HttpUri,Wcslen((Temp->HttpUri)+1)*sizeof(CHAR16),0x00);
		}else{
		NetworkStackSize = sizeof(NETWORK_STACK);
		MemSet(&NetworkStackVar,sizeof(NETWORK_STACK),0x00);
		gRT->GetVariable(
                 L"NetworkStackVar",
                 &NetworkStackGuid,
                 &NetworkAttr,
                 &NetworkStackSize,
                 (UINT8*)(&NetworkStackVar)
				 );
		Temp->OnboardLANOpRomType = 1;
		NetworkStackVar.Ipv4Http = 1;
		NetworkStackVar.Enable = 1;
		gRT->SetVariable(
				L"NetworkStackVar",
				&NetworkStackGuid,
                NetworkAttr,
				NetworkStackSize,
				(UINT8*)(&NetworkStackVar)
				);
		}
		gBS->FreePool(Uris);
	}
	return EFI_SUCCESS;
}
#endif