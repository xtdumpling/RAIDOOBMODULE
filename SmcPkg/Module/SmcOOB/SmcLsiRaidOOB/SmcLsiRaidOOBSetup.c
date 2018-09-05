#include "SmcLsiRaidOOBSetup.h"


SMC_LSI_HII_HANDLE*	mSmcLsiHiiHandleTable = NULL;

//Since this module doesn't report to TSE, use dummy functions.
//EFI_HII_CONFIG_ACCESS_PROTOCOL	SmcLsiOOBConfigAccess = { NULL, NULL, NULL };
EFI_GUID	gSmcLsiOOBSetupFormSetGuid = SMC_LSI_OOB_FORM_SET_GUID;

CALLBACK_INFO SmcLsiOOBSetupCallBack[] = {
        { &gSmcLsiOOBSetupFormSetGuid, NULL, 0, 0, 0 }
};

EFI_HII_HANDLE	GetHiiHandle(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol){
		
	EFI_HII_HANDLE	pHandle;

	SMC_LSI_RAID_OOB_SETUP_PRIVATE*	pPrivate = NULL;
	pPrivate = STRUCT_START(SMC_LSI_RAID_OOB_SETUP_PRIVATE,SmcLsiRaidOOBSetupProtocol,pProtocol);

	pHandle = pPrivate->HIIHandle;

	return pHandle;
}

EFI_GUID*	GetFormSetGuid(SMC_LSI_RAID_OOB_SETUP_PROTOCOL*	pProtocol){

	SMC_LSI_RAID_OOB_SETUP_PRIVATE*	pPrivate = NULL;
	pPrivate = STRUCT_START(SMC_LSI_RAID_OOB_SETUP_PRIVATE,SmcLsiRaidOOBSetupProtocol,pProtocol);

	return &pPrivate->FormSetGuid;
}
EFI_STRING_ID	GetFormSetNameID(SMC_LSI_RAID_OOB_SETUP_PROTOCOL*	pProtocol){

	SMC_LSI_RAID_OOB_SETUP_PRIVATE*	pPrivate = NULL;
	pPrivate = STRUCT_START(SMC_LSI_RAID_OOB_SETUP_PRIVATE,SmcLsiRaidOOBSetupProtocol,pProtocol);

	return pPrivate->FormSetNameID;
}
UINT16	GetFormID(SMC_LSI_RAID_OOB_SETUP_PROTOCOL*	pProtocol){

	SMC_LSI_RAID_OOB_SETUP_PRIVATE*	pPrivate = NULL;
	pPrivate = STRUCT_START(SMC_LSI_RAID_OOB_SETUP_PRIVATE,SmcLsiRaidOOBSetupProtocol,pProtocol);

	return pPrivate->FormID;
}
UINT16	GetFormIdStart(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol){

	SMC_LSI_RAID_OOB_SETUP_PRIVATE*	pPrivate = NULL;
	pPrivate = STRUCT_START(SMC_LSI_RAID_OOB_SETUP_PRIVATE,SmcLsiRaidOOBSetupProtocol,pProtocol);

	return ++pPrivate->FormIdStart;
}
UINT16	GetFormIdNow(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol){

	SMC_LSI_RAID_OOB_SETUP_PRIVATE*	pPrivate = NULL;
	pPrivate = STRUCT_START(SMC_LSI_RAID_OOB_SETUP_PRIVATE,SmcLsiRaidOOBSetupProtocol,pProtocol);

	return pPrivate->FormIdStart;
}
UINT16	GetQIdStart(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol){

	SMC_LSI_RAID_OOB_SETUP_PRIVATE*	pPrivate = NULL;
	pPrivate = STRUCT_START(SMC_LSI_RAID_OOB_SETUP_PRIVATE,SmcLsiRaidOOBSetupProtocol,pProtocol);

	return ++pPrivate->QIdStart;
}
UINT16	GetQIdNow(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol){

	SMC_LSI_RAID_OOB_SETUP_PRIVATE*	pPrivate = NULL;
	pPrivate = STRUCT_START(SMC_LSI_RAID_OOB_SETUP_PRIVATE,SmcLsiRaidOOBSetupProtocol,pProtocol);

	return pPrivate->QIdStart;
}
UINT16	GetVIdStart(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol){

	SMC_LSI_RAID_OOB_SETUP_PRIVATE*	pPrivate = NULL;
	pPrivate = STRUCT_START(SMC_LSI_RAID_OOB_SETUP_PRIVATE,SmcLsiRaidOOBSetupProtocol,pProtocol);

	return ++pPrivate->VarIdStart;
}
UINT16	GetVIdNow(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol){

	SMC_LSI_RAID_OOB_SETUP_PRIVATE*	pPrivate = NULL;
	pPrivate = STRUCT_START(SMC_LSI_RAID_OOB_SETUP_PRIVATE,SmcLsiRaidOOBSetupProtocol,pProtocol);

	return pPrivate->VarIdStart;
}
EFI_GUID*	GetVarGuid(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol){

	SMC_LSI_RAID_OOB_SETUP_PRIVATE*	pPrivate = NULL;
	pPrivate = STRUCT_START(SMC_LSI_RAID_OOB_SETUP_PRIVATE,SmcLsiRaidOOBSetupProtocol,pProtocol);

	return &pPrivate->VarGuid;
}
UINT16	GetFormGoToLabel(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol){

	SMC_LSI_RAID_OOB_SETUP_PRIVATE*	pPrivate = NULL;
	pPrivate = STRUCT_START(SMC_LSI_RAID_OOB_SETUP_PRIVATE,SmcLsiRaidOOBSetupProtocol,pProtocol);

	return pPrivate->FormGoToLabel;
}

UINT8*	GetSetupData(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol){

	SMC_LSI_RAID_OOB_SETUP_PRIVATE*	pPrivate = NULL;
	pPrivate = STRUCT_START(SMC_LSI_RAID_OOB_SETUP_PRIVATE,SmcLsiRaidOOBSetupProtocol,pProtocol);

	return pPrivate->RaidSetupVfr;
}

UINT8*	GetSetupString(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol){

	SMC_LSI_RAID_OOB_SETUP_PRIVATE*	pPrivate = NULL;
	pPrivate = STRUCT_START(SMC_LSI_RAID_OOB_SETUP_PRIVATE,SmcLsiRaidOOBSetupProtocol,pProtocol);

	return pPrivate->RaidSetupString;
}

EFI_STATUS SmcLsiSetupDownDummyFunc(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol){
	return EFI_SUCCESS;
}

UINT8	GetRaidIndex(SMC_LSI_RAID_TYPE	RaidType){
	static	SMC_LSI_RAID_CARD_INDEX	Raid3108Index = 0;
	static	SMC_LSI_RAID_CARD_INDEX	Raid9260Index = 0;

	SMC_LSI_RAID_CARD_INDEX	ReturnIndex = 0;

	switch(RaidType){
		case RAID_3108:
			ReturnIndex = Raid3108Index++;
			break;
		case RAID_9260:
			ReturnIndex = Raid9260Index++;
			break;
		default:
			break;
	}
	return ReturnIndex;
}

VOID	AddSmcLsiHiiHandle(SMC_LSI_RAID_TYPE RaidType,EFI_HII_HANDLE RaidHiiHandle){
	static 	SMC_LSI_HII_HANDLE*			pLocal 	= NULL;
	SMC_LSI_HII_HANDLE*					pTemp 	= NULL;

	gBS->AllocatePool(EfiBootServicesData,sizeof(SMC_LSI_HII_HANDLE),&pTemp);
	pTemp->RaidCardType			= RaidType;
	pTemp->RaidCardIndex		= GetRaidIndex(RaidType);
	pTemp->RaidCardHiiHandle	= RaidHiiHandle;
	pTemp->SmcFormId			= 0x0;
	pTemp->SmcFormTitleId		= 0x0;
	pTemp->pNext				= NULL;
	
	if(! (!!pLocal)){
		pLocal = mSmcLsiHiiHandleTable = pTemp;
	}else{
		pLocal->pNext 	= pTemp;
		pLocal 			= pLocal->pNext;
	}

	DEBUG((-1,"pLocal RaidCardType[%x], RaidCardIndex[%x], RaidCardHiiHandle[%08x]\n",pLocal->RaidCardType,pLocal->RaidCardIndex,(UINT32)pLocal->RaidCardHiiHandle));
	return;
}

BOOLEAN SearchForLsiFormSetByHiiOrder(){

	EFI_HII_HANDLE*				HiiHandleBuffer = NULL;
	UINTN						BufferSize = 0;
	UINTN						Index = 0;
	UINTN						NameIndex = 0;
	BOOLEAN						SearchFind = FALSE;

	EFI_HII_PACKAGE_LIST_HEADER *HiiPackageList = NULL;
    EFI_HII_PACKAGE_HEADER 		*IfrPackagePtr;
    EFI_IFR_OP_HEADER* 			pEFI_IFR_OP_HEADER = NULL;
	EFI_IFR_FORM_SET*  			pEFI_IFR_FORM_SET = NULL;
	CHAR16*			   			TempS = NULL;
	EFI_STATUS					PackStatus	= EFI_SUCCESS;

	PackStatus = GetListPackageListsHandle(&BufferSize,&HiiHandleBuffer);

	for(Index = 0; !EFI_ERROR(PackStatus) ;Index++){

		if(HiiHandleBuffer[Index] == NULL || Index >= BufferSize/sizeof(EFI_HII_HANDLE)) break;
		
		HiiPackageList = GetHiiPackageList(HiiHandleBuffer[Index]);
		if(! (!!HiiPackageList)) break;
    	for (IfrPackagePtr = (EFI_HII_PACKAGE_HEADER*)(HiiPackageList+1);
        	 IfrPackagePtr < (EFI_HII_PACKAGE_HEADER*)((UINT8*)HiiPackageList+HiiPackageList->PackageLength);
        	 IfrPackagePtr = (EFI_HII_PACKAGE_HEADER*)((UINT8*)IfrPackagePtr+IfrPackagePtr->Length)){

			if(IfrPackagePtr->Type == EFI_HII_PACKAGE_FORMS){
				pEFI_IFR_OP_HEADER = (EFI_IFR_OP_HEADER*)((UINT8*)IfrPackagePtr + sizeof (EFI_HII_PACKAGE_HEADER));	
				if (pEFI_IFR_OP_HEADER->OpCode == EFI_IFR_FORM_SET_OP) {
					pEFI_IFR_FORM_SET = (EFI_IFR_FORM_SET*)pEFI_IFR_OP_HEADER;
					TempS = GetHiiString(HiiHandleBuffer[Index],pEFI_IFR_FORM_SET->FormSetTitle);
					DEBUG((-1," FormSetTitle :: %s\n",TempS));
					DEBUG((-1," Guid :: %g\n",pEFI_IFR_FORM_SET->Guid));
						
					for(NameIndex=0;RaidName[NameIndex].LsiRaidTypeIndex != RAID_NULL;++NameIndex){
						if(MemCmp(TempS,RaidName[NameIndex].FormRaidName,StrLen(RaidName[NameIndex].FormRaidName) * sizeof(CHAR16)) == 0){
							DEBUG((-1,"Find RaidName[%d] :: <%s>\n",NameIndex,RaidName[NameIndex].FormRaidName));
							AddSmcLsiHiiHandle(RaidName[NameIndex].LsiRaidTypeIndex,HiiHandleBuffer[Index]);
							SearchFind = TRUE;
						}
					}	
				}
			}
		}
	}
	return SearchFind;
}

EFI_STATUS SmcLsiOOBSetupDriverStart(SMC_LSI_RAID_OOB_SETUP_DRIVER*	pDriver){

	EFI_STATUS		Status			= EFI_SUCCESS;
	UINTN			Index			= 0;
	EFI_GUID		lFormSetGuid	= SMC_LSI_OOB_FORM_SET_GUID;
	EFI_GUID		lVarGuid		= SMC_LSI_OOB_VAR_GUID;

	SMC_LSI_RAID_OOB_SETUP_PROTOCOL*		mSmcLsiRaidOOBSetupProtocol = NULL;
	SMC_LSI_RAID_OOB_SETUP_PRIVATE*			pPrivate = NULL;

	DEBUG((-1,"SmcLsiOOBSetupDriverStart Entry\n"));

	pPrivate = STRUCT_START(SMC_LSI_RAID_OOB_SETUP_PRIVATE,SmcLsiRaidOOBSetupDriver,pDriver);

	if(!(!!SearchForLsiFormSetByHiiOrder())) return EFI_LOAD_ERROR;

	gBS->AllocatePool(EfiBootServicesData,sizeof(SMC_LSI_RAID_OOB_SETUP_PROTOCOL),&mSmcLsiRaidOOBSetupProtocol);
	MemSet(mSmcLsiRaidOOBSetupProtocol,sizeof(SMC_LSI_RAID_OOB_SETUP_PROTOCOL),0x00);

	mSmcLsiRaidOOBSetupProtocol->SmcLsiDetailedStatus			= 0x0;
	mSmcLsiRaidOOBSetupProtocol->DetailedDebugMessage			= SMC_RAID_DETAILED_DEBUG_MESSAGE;
	mSmcLsiRaidOOBSetupProtocol->SmcSetupStatus					= EFI_NOT_READY; 
	mSmcLsiRaidOOBSetupProtocol->SmcLsiRaidNameTable			= RaidName; 
	mSmcLsiRaidOOBSetupProtocol->SmcLsiVarTable 		 		= mSmcLsiVarTable; 
	mSmcLsiRaidOOBSetupProtocol->SmcLsiHiiHandleTable 			= mSmcLsiHiiHandleTable;
	mSmcLsiRaidOOBSetupProtocol->SmcLsiRaidFormRefSearchTable	= mSmcLsiRaidFormRefSearchTable;
	mSmcLsiRaidOOBSetupProtocol->SmcLsiRaidItemsTable			= mSmcLsiItemsTable;
	mSmcLsiRaidOOBSetupProtocol->SmcLsiAfterDownFuncTable		= mSmcLsiAfterDwonFuncTable;
	mSmcLsiRaidOOBSetupProtocol->SmcLsiAfterLoadFuncTable		= mSmcLsiAfterLoadFuncTable;

	mSmcLsiRaidOOBSetupProtocol->SmcLsiGetFormId				= GetFormID;
	mSmcLsiRaidOOBSetupProtocol->SmcLsiGetFormSetGuid			= GetFormSetGuid;
	mSmcLsiRaidOOBSetupProtocol->SmcLsiGetFormSetNameId			= GetFormSetNameID;
	mSmcLsiRaidOOBSetupProtocol->SmcLsiGetFormToLabel			= GetFormGoToLabel;
	mSmcLsiRaidOOBSetupProtocol->SmcLsiGetHiiHandle				= GetHiiHandle;
	mSmcLsiRaidOOBSetupProtocol->SmcLsiGetQIdStart				= GetQIdStart;
	mSmcLsiRaidOOBSetupProtocol->SmcLsiGetQidNow				= GetQIdNow;
	mSmcLsiRaidOOBSetupProtocol->SmcLsiGetFormIdStart			= GetFormIdStart;
	mSmcLsiRaidOOBSetupProtocol->SmcLsiGetFormIdNow				= GetFormIdNow;
	mSmcLsiRaidOOBSetupProtocol->SmcLsiGetVIdStart				= GetVIdStart;
	mSmcLsiRaidOOBSetupProtocol->SmcLsiGetVIdNow				= GetVIdNow;
	mSmcLsiRaidOOBSetupProtocol->SmcLsiGetVarGuid				= GetVarGuid;
	mSmcLsiRaidOOBSetupProtocol->SmcLsiGetSetupData				= GetSetupData;
	mSmcLsiRaidOOBSetupProtocol->SmcLsiGetSetupString			= GetSetupString;

	pPrivate->HIIHandle 					= NULL;		//Will update after LoadResources
	pPrivate->FormSetNameID					= STRING_TOKEN(STR_SMC_LSI_OOB_TITLE); 
	pPrivate->FormID						= SMC_LSI_OOB_FORM;
	pPrivate->QIdStart						= SMC_LSI_OOB_Q_ID_START;
	pPrivate->FormIdStart					= SMC_LSI_OOB_FORM_START;	
	pPrivate->FormGoToLabel					= SMC_LSI_OOB_FORM_GOTO_LABEL;
	pPrivate->FormSetGuid					= lFormSetGuid;
	pPrivate->VarIdStart					= SMC_LSI_OOB_VAR_ID_START;
	pPrivate->VarGuid						= lVarGuid;
	pPrivate->HaveRaidResource				= TRUE;

	pPrivate->RaidSetupVfr					= NULL;
	pPrivate->RaidSetupString				= NULL;

	MemCpy(&pPrivate->SmcLsiRaidOOBSetupProtocol,mSmcLsiRaidOOBSetupProtocol,sizeof(SMC_LSI_RAID_OOB_SETUP_PROTOCOL));

	DEBUG((-1,"SmcLsiRaidOOBSetupProtocol Location[%08x]\n",&pPrivate->SmcLsiRaidOOBSetupProtocol));

	Status = gBS->InstallProtocolInterface(
					&pPrivate->DriverHandle,
					&gSmcLsiRaidOobSetupProtocolGuid,
					EFI_NATIVE_INTERFACE,
					&pPrivate->SmcLsiRaidOOBSetupProtocol
				);
	DEBUG((-1,"SmcLsiOOBSetupDriverStart InstallProtocolInterface gSmcLsiRaidOobSetupProtocolGuid Status[%r]\n",Status));
	if(EFI_ERROR(Status)) return Status;

    return EFI_SUCCESS;
}

EFI_STATUS SmcLsiOOBSetupDriverDownload(SMC_LSI_RAID_OOB_SETUP_DRIVER*	pDriver){

	EFI_STATUS								Status			= EFI_NOT_READY;
	SMC_LSI_RAID_OOB_SETUP_PRIVATE*			pPrivate 		= NULL;

	DEBUG((-1,"SmcLsiOOBSetupDriverDownload Entry\n"));
	pPrivate = STRUCT_START(SMC_LSI_RAID_OOB_SETUP_PRIVATE,SmcLsiRaidOOBSetupDriver,pDriver);
	
	if(!!pPrivate->HaveRaidResource){
		Status = SmcLsiRaidLib_DownLoad(&pPrivate->SmcLsiRaidOOBSetupProtocol);
		DEBUG((-1,"SmcLsiOOBSetupDriverDownload Status[%r]\n",Status));
	}
	return Status;
}
EFI_STATUS SmcLsiOOBSetupDriverLoadResource(SMC_LSI_RAID_OOB_SETUP_DRIVER*	pDriver){

	EFI_STATUS								Status			= EFI_NOT_READY;
	SMC_LSI_RAID_OOB_SETUP_PRIVATE*			pPrivate 		= NULL;

	DEBUG((-1,"SmcLsiOOBSetupDriverLoadResource Entry\n"));

	pPrivate = STRUCT_START(SMC_LSI_RAID_OOB_SETUP_PRIVATE,SmcLsiRaidOOBSetupDriver,pDriver);

	//Temporary in here to check resoucre and Debug.
	if(!!pPrivate->HaveRaidResource && !EFI_ERROR(pPrivate->SmcLsiRaidOOBSetupProtocol.SmcSetupStatus)){
		
    	Status = LoadResources( pPrivate->DriverHandle,
                            	sizeof(SmcLsiOOBSetupCallBack)/sizeof(CALLBACK_INFO), 
                            	SmcLsiOOBSetupCallBack, 
                            	NULL 
                          		);
		
		DEBUG((-1,"SmcLsiOOBSetupDriverLoadResource LoadResources Status[%r]\n",Status));
		
		if(!EFI_ERROR(Status)) {
			pPrivate->HIIHandle = SmcLsiOOBSetupCallBack[0].HiiHandle;
			Status = SmcLsiRaidLib_AddDynamicItems(&pPrivate->SmcLsiRaidOOBSetupProtocol);

			DEBUG((-1,"SmcLsiOOBSetupDriverLoadResource AddRaidDynamicItems Status[%r]\n",Status));
		}
	}
	
	return Status;
}

EFI_STATUS	SmcLsiOOBSetupDriverGetNvData(SMC_LSI_RAID_OOB_SETUP_DRIVER*  pDriver, BOOLEAN  ReNew,OUT SMC_LSI_RAID_OOB_SETUP_NVSTRUCT*  pNVStruct){
	
	EFI_STATUS								Status 			= EFI_NOT_READY;
	SMC_LSI_RAID_OOB_SETUP_PRIVATE*			pPrivate 		= NULL;

	if(pNVStruct == NULL) return EFI_BUFFER_TOO_SMALL;
/*
	if(pNVStruct->SmcRaidVarName 	!= NULL || 
	   pNVStruct->SmcRaidVarGuid 	!= NULL || 
	   pNVStruct->SmcRaidVarSize 	!= NULL ||
	   pNVStruct->SmcRaidVarBuffer	!= NULL)
		return EFI_INVALID_PARAMETER; 
*/
	pPrivate = STRUCT_START(SMC_LSI_RAID_OOB_SETUP_PRIVATE,SmcLsiRaidOOBSetupDriver,pDriver);

	if(!!pPrivate->HaveRaidResource && !EFI_ERROR(pPrivate->SmcLsiRaidOOBSetupProtocol.SmcSetupStatus)){

		Status = SmcLsiRaidLib_ParseNvData(&pPrivate->SmcLsiRaidOOBSetupProtocol,
										   (!!ReNew),
										   &pNVStruct->SmcRaidVarName,
										   &pNVStruct->SmcRaidVarGuid,
										   &pNVStruct->SmcRaidVarSize,
										   &pNVStruct->SmcRaidVarBuffer);

		DEBUG((-1,"SmcLsiRaidLib_ParseNvData Status[%r]\n",Status));
		return Status;
	}
	return Status;
}

EFI_STATUS SmcLsiRaidOOBSetupEntry(
	IN EFI_HANDLE ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable
){
  
	EFI_STATUS	Status = EFI_SUCCESS;
	SMC_LSI_RAID_OOB_SETUP_PRIVATE*			SmcLsiRaidPrivateData = NULL;
	SMC_LSI_RAID_OOB_SETUP_DRIVER*			SmcLsiRaidOobSetupDriver = NULL;

	DEBUG((-1,"SmcLsiRaidOOBSetupEntry Entry\n"));

	InitAmiLib(ImageHandle, SystemTable);
	
	gBS->AllocatePool(EfiBootServicesData,sizeof(SMC_LSI_RAID_OOB_SETUP_PRIVATE),&SmcLsiRaidPrivateData);
	gBS->AllocatePool(EfiBootServicesData,sizeof(SMC_LSI_RAID_OOB_SETUP_DRIVER),&SmcLsiRaidOobSetupDriver);

	MemSet(SmcLsiRaidPrivateData,sizeof(SMC_LSI_RAID_OOB_SETUP_PRIVATE), 0x00);

	SmcLsiRaidOobSetupDriver->SmcLsiSetupDriverStart 		= SmcLsiOOBSetupDriverStart;
	SmcLsiRaidOobSetupDriver->SmcLsiSetupDriverDownload		= SmcLsiOOBSetupDriverDownload;
	SmcLsiRaidOobSetupDriver->SmcLsiSetupDriverLoadR		= SmcLsiOOBSetupDriverLoadResource;
	SmcLsiRaidOobSetupDriver->SmcLsiSetupDriverGetNvData	= SmcLsiOOBSetupDriverGetNvData;

	SmcLsiRaidPrivateData->DriverHandle					= ImageHandle;
	SmcLsiRaidPrivateData->HaveRaidResource				= FALSE;
	
	MemCpy(&SmcLsiRaidPrivateData->SmcLsiRaidOOBSetupDriver,SmcLsiRaidOobSetupDriver,sizeof(SMC_LSI_RAID_OOB_SETUP_DRIVER));
	DEBUG((-1,"SmcLsiRaidOOBSetupDriver Location[%08x]\n",&SmcLsiRaidPrivateData->SmcLsiRaidOOBSetupDriver));

	Status = gBS->InstallProtocolInterface(
				&SmcLsiRaidPrivateData->DriverHandle,
				&gSmcLsiRaidOobSetupDriverGuid,
				EFI_NATIVE_INTERFACE,
				&SmcLsiRaidPrivateData->SmcLsiRaidOOBSetupDriver
			);

	DEBUG((-1,"InstallProtocolInterface SmcLsiRaidOOBSetupDriver Status[%r]\n",Status));

    DEBUG((-1, "[SMC_OOB] :: SmcLsiRaidOOBSetupEntry End.\n"));
    return EFI_SUCCESS;
}
