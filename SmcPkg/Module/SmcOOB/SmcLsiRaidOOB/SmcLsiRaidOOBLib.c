//****************************************************************************
//****************************************************************************
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
//****************************************************************************
//****************************************************************************
//  File History
//
//  Rev. 1.00
//    Bug Fix:  Initial revision.
//    Reason:
//    Auditor:  Durant Lin
//    Date:     Aug/13/2018
//
//****************************************************************************

#include "Token.h"
#include <AmiLib.h>
#include <AmiDxeLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Protocol/HiiDatabase.h>
#include <Protocol/HiiPackageList.h>
#include <Protocol/HiiString.h>
#include <Protocol/HiiConfigAccess.h>
#include <Protocol/HiiConfigRouting.h>
#include <Protocol/FormBrowser2.h>
#include <Library/HiiLib.h>
#include <Library/PrintLib.h>

#include "SmcLsiRaidOOBSetupProtocol.h"
#include "SmcLsiRaidOOBLib.h"

SMC_LSI_RAID_OOB_SETUP_PROTOCOL*	GetSmcRaidOOBSetupProtocol(){
	
	static SMC_LSI_RAID_OOB_SETUP_PROTOCOL*	pProtocol = NULL;
	if(!!pProtocol) return pProtocol;

	gBS->LocateProtocol(&gSmcLsiRaidOobSetupProtocolGuid,NULL,&pProtocol);

	return pProtocol;
}

EFI_HANDLE	GetSmcRaidCurrDriverHandle(){

	return GetSmcRaidOOBSetupProtocol()->SmcLsiCurrHiiHandleTable->RaidCardDriverHandle;
}

EFI_HII_DATABASE_PROTOCOL*	GetHiiDataBase(){

	static EFI_HII_DATABASE_PROTOCOL* HiiDatabase = NULL;
	if(!!HiiDatabase) return HiiDatabase;

	gBS->LocateProtocol(&gEfiHiiDatabaseProtocolGuid,NULL,&HiiDatabase);
	
	return HiiDatabase;
}

EFI_HII_CONFIG_ROUTING_PROTOCOL* GetHiiConfigRoutingProtocol(){
	static EFI_HII_CONFIG_ROUTING_PROTOCOL* HiiConfig = NULL;
	if(!!HiiConfig) return HiiConfig;

	gBS->LocateProtocol(&gEfiHiiConfigRoutingProtocolGuid,NULL,&HiiConfig);
	return HiiConfig;
}

EFI_HII_STRING_PROTOCOL*	GetHiiStringProtocol(){
	static EFI_HII_STRING_PROTOCOL* HiiString = NULL;
	if(!!HiiString) return HiiString;

	gBS->LocateProtocol(&gEfiHiiStringProtocolGuid,NULL,&HiiString);

	return HiiString;
}

EFI_STRING	GetHiiString(EFI_HII_HANDLE PackageList, EFI_STRING_ID StringId){

	EFI_STATUS			Status	   = EFI_SUCCESS;
	static EFI_STRING	StringTemp = NULL;
	static EFI_STRING	StringLoc  = NULL;
	UINTN  				StringSize = 512;

	if(StringTemp == NULL){
		Status = gBS->AllocatePool(EfiBootServicesData,StringSize * sizeof(CHAR16),&StringTemp);
		if(EFI_ERROR(Status)) return NULL;

		StringLoc = StringTemp;
		//Align bound to prevent assert.
		if(!!((UINTN)StringTemp & 0x01)){
			StringTemp = (EFI_STRING)(((UINT8*)StringTemp) + 1);
		}	
	}
	MemSet(StringLoc,StringSize * sizeof(CHAR16),0x00);

	Status = GetHiiStringProtocol()->GetString(
					GetHiiStringProtocol(),
					"en-US",
					PackageList,
					StringId,
					StringTemp,
					&StringSize,
					NULL
				);
	
	if(EFI_ERROR(Status)) return NULL;

	return StringTemp;
}

EFI_STRING_ID	NewHiiString(EFI_HII_HANDLE PackageList, EFI_STRING	String){
	EFI_STATUS		Status = EFI_SUCCESS;
	EFI_STRING_ID	StringId = 0;

	Status = GetHiiStringProtocol()->NewString(
					GetHiiStringProtocol(),
					PackageList,
					&StringId,
					"en-US",
					NULL,
					String,
					NULL			
				);
	if(EFI_ERROR(Status)) return 0x0;

	return StringId;
}

EFI_STATUS GetListPackageListsHandle(UINTN *ArgBufferLength, EFI_HII_HANDLE* *ArgHandleBuffer){

    EFI_HII_HANDLE   TempHiiHandleBuffer = NULL;
    EFI_HII_HANDLE 	*HiiHandleBuffer = NULL;
    UINTN 			HandleBufferLength = 0;
	EFI_STATUS				Status;


	Status = GetHiiDataBase()->ListPackageLists(
							GetHiiDataBase(),
							EFI_HII_PACKAGE_TYPE_ALL,
							NULL,
							&HandleBufferLength,
							&TempHiiHandleBuffer
						);

	if(Status != EFI_BUFFER_TOO_SMALL) return EFI_LOAD_ERROR;

	gBS->AllocatePool(EfiBootServicesData,
					  HandleBufferLength + sizeof(EFI_HII_HANDLE),
					  &((UINT8*)HiiHandleBuffer));

	if(!(!!HiiHandleBuffer)) return EFI_LOAD_ERROR;

	MemSet((UINT8*)HiiHandleBuffer,(UINT32)(HandleBufferLength + sizeof(EFI_HII_HANDLE)),0x00);

	Status = GetHiiDataBase()->ListPackageLists(
							GetHiiDataBase(),
							EFI_HII_PACKAGE_TYPE_ALL,
							NULL,
							&HandleBufferLength,
							HiiHandleBuffer
						);

	if(EFI_ERROR(Status)) return EFI_LOAD_ERROR;

	*ArgBufferLength = HandleBufferLength;
	*ArgHandleBuffer = HiiHandleBuffer;
	return EFI_SUCCESS;
}

EFI_HII_PACKAGE_LIST_HEADER*	GetHiiPackageList(EFI_HII_HANDLE TargetHiiHandle){

	UINTN							ListBufferSize 			= 0;
	EFI_HII_PACKAGE_LIST_HEADER 	*HiiPackageList 		= NULL;
	EFI_STATUS						Status					= EFI_SUCCESS;

	if(! (!!TargetHiiHandle)) return NULL;

	gBS->AllocatePool(EfiBootServicesData,MAX_HPK_LIST_SIZE,(VOID**)(&HiiPackageList));
	if(! (!!HiiPackageList)) return NULL;

	MemSet(HiiPackageList,MAX_HPK_LIST_SIZE,0x00);
	ListBufferSize = 0;

	Status = GetHiiDataBase()->ExportPackageLists(
								GetHiiDataBase(),
								TargetHiiHandle,
								&ListBufferSize,
								HiiPackageList					
							);

	if(Status != EFI_BUFFER_TOO_SMALL) return NULL;

	Status = GetHiiDataBase()->ExportPackageLists(
								GetHiiDataBase(),
								TargetHiiHandle,
								&ListBufferSize,
								HiiPackageList					
							);
	if(EFI_ERROR(Status)) return NULL;
	
	return HiiPackageList;
}


EFI_HII_HANDLE SearchForFormSet(EFI_GUID* SearchGuid, CHAR16* SearchFormSetName){

	EFI_HII_HANDLE*					HiiHandleBuffer 		= NULL;
	UINTN							BufferSize 				= 0;
	UINTN							Index 					= 0;
	EFI_HII_PACKAGE_LIST_HEADER 	*HiiPackageList 		= NULL;
	UINTN							ListBufferSize 			= 0;
    EFI_HII_PACKAGE_HEADER 			*IfrPackagePtr;
    EFI_IFR_OP_HEADER* 				pEFI_IFR_OP_HEADER 		= NULL;
	EFI_IFR_FORM_SET*  				pEFI_IFR_FORM_SET 		= NULL;
	BOOLEAN			   				SearchFind 				= FALSE;
	UINT8			   				TargetHandle 			= 1;
	CHAR16*			   				TempS 					= NULL;
	

	if(!EFI_ERROR(GetListPackageListsHandle(&BufferSize,&HiiHandleBuffer))){

		for(Index = 0; ;Index++){
			if(HiiHandleBuffer[Index] == NULL || (Index >= BufferSize/sizeof(EFI_HII_HANDLE)))
				break;
			HiiPackageList = GetHiiPackageList(HiiHandleBuffer[Index]);
			
			if(! (!!HiiPackageList)) break;

    		for (
        		IfrPackagePtr = (EFI_HII_PACKAGE_HEADER*)(HiiPackageList+1);
        		IfrPackagePtr < (EFI_HII_PACKAGE_HEADER*)((UINT8*)HiiPackageList+HiiPackageList->PackageLength);
        		IfrPackagePtr = (EFI_HII_PACKAGE_HEADER*)((UINT8*)IfrPackagePtr+IfrPackagePtr->Length)){
				
				if(IfrPackagePtr->Type == EFI_HII_PACKAGE_FORMS){
					pEFI_IFR_OP_HEADER = (EFI_IFR_OP_HEADER*)((UINT8*)IfrPackagePtr + sizeof (EFI_HII_PACKAGE_HEADER));	
					if (pEFI_IFR_OP_HEADER->OpCode == EFI_IFR_FORM_SET_OP) {
						pEFI_IFR_FORM_SET = (EFI_IFR_FORM_SET*)pEFI_IFR_OP_HEADER;
						TempS = GetHiiString(HiiHandleBuffer[Index],pEFI_IFR_FORM_SET->FormSetTitle);
						DEBUG((-1," FormSetTitle :: %s\n",TempS));
						DEBUG((-1," Guid :: %g\n",pEFI_IFR_FORM_SET->Guid));
						if(MemCmp(TempS,SearchFormSetName,StrLen(SearchFormSetName) * sizeof(CHAR16)) == 0){
							DEBUG((-1,"Search Find!, addr = %x\n",SearchFormSetName));
							SearchFind = TRUE;
							break;
						}
					}
				}
			}
			gBS->FreePool(HiiPackageList);
			if(SearchFind) break;
		}
	}
	return (!!SearchFind) ? HiiHandleBuffer[Index] : NULL;
}



EFI_STRING AppendOffsetWidth(EFI_STRING AppString, UINTN AppOffset, UINTN AppWidth){

	UINTN		Size;
	EFI_STRING 	ConfigRequest = NULL;

	Size = (StrLen(AppString) + 32 + 1) * sizeof(CHAR16);
	gBS->AllocatePool(EfiBootServicesData,Size,(VOID*)(&ConfigRequest));
	
	if(ConfigRequest == NULL) return NULL;

	MemSet(ConfigRequest,Size,0x00);

	UnicodeSPrint(ConfigRequest, Size, L"%s&OFFSET=0&WIDTH=%016LX",AppString, AppWidth);
	//gBS->FreePool(AppString);

	return ConfigRequest;
}

VOID	DEBUG_PRINT_BUFFER(VOID* pBuffer,UINTN BufferSize){
#if defined(DEBUG_MODE) && (DEBUG_MODE == 1)
	UINTN	index 		= 0;
	UINT8*	TempBuff	= NULL;
	DEBUG((-1,"\n"));
	for(index=0,TempBuff = (UINT8*)pBuffer;index < BufferSize; ++index){
		if(index % 16 == 0) DEBUG((-1,"0x%04x :: ",index));
		DEBUG((-1,"%02x ",TempBuff[index]));
		if((index+1) % 16 == 0) DEBUG((-1,"\n"));
	}
	DEBUG((-1,"\n"));
#endif
}

VOID 	DEBUG_PRINT_CONFIG(CHAR16* Target){
	CHAR16*		OriS = NULL;
	CHAR16		TempS[101];
	UINTN		LenSize = StrLen(Target);

	OriS = Target;
	do {
		MemSet(TempS,101 * sizeof(CHAR16),0x00);
		if((UINTN)(Target + 100) > (UINTN)(OriS + StrLen(OriS))){
			MemCpy(TempS,Target,StrLen(Target) * sizeof(CHAR16));
			Target += StrLen(Target);
		}else{
			MemCpy(TempS,Target,100 * sizeof(CHAR16));
			Target += 100;
		}
		DEBUG((-1,"%s\n",TempS));		
	}while((UINTN)Target < (UINTN)(OriS + StrLen(OriS)));
}
