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

#include "SmcLsiRaidOOB.h"
#include "SmcOobVersion.h"
#include "SmcLsiRaidOOBGlbTable.h"


EFI_STATUS	SettingErrorStatus(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol, UINT8	ErrorStatus,EFI_STATUS Status){
	
	if(pProtocol == NULL) return EFI_LOAD_ERROR;

	pProtocol->SmcLsiDetailedStatus = (pProtocol->SmcLsiDetailedStatus << 8) | (ErrorStatus & 0xFF);

	return Status;
}

EFI_STATUS THROUGH_DEBUG_MESSAGE(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol, CHAR16*	FuncName, EFI_STATUS Status){

	static	CHAR16	UnknownFuncName[] = L"Unknown Function";
	if(pProtocol == NULL){
		DEBUG((-1,"Fatal Error! SMC_LSI_RAID_OOB_SETUP_PROTOCOL is NULL!\n"));
		return EFI_LOAD_ERROR;
	}
	if(FuncName == NULL) FuncName = UnknownFuncName;	

	if(Status == SMC_LSI_THROUGH_FUNC_EXEC){	
		Status = EFI_SUCCESS;
		DEBUG((-1,"\n==== <%s> Function execute <START>, Status[%r], Detailed Error Code[%x] ==== \n",FuncName,Status,pProtocol->SmcLsiDetailedStatus));
	}else {	
		DEBUG((-1,"\n==== <%s> Function execute < END >, Status[%r], Detailed Error Code[%x] ==== \n",FuncName,Status,pProtocol->SmcLsiDetailedStatus));
	}

	if(!EFI_ERROR(Status)){
		pProtocol->SmcLsiDetailedStatus = 0x0;
	}
	pProtocol->SmcSetupStatus = Status;
	return Status;
}

// ======== LSI VAR START ========

EFI_STATUS FreeLsiVarHashTable(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol){
	
	UINTN	TableIndex = 0;
	SMC_RAID_VAR_HASH*	pTemp = NULL;
	SMC_RAID_VAR_HASH*	pNext = NULL;

	for(TableIndex = 0; TableIndex < VAR_HASH_NUM;++TableIndex){
		pTemp = LsiVarHashTableName[TableIndex];
		while(!!pTemp){
			pNext = pTemp->pRaidpNameNext;
			gBS->FreePool(pTemp->RaidVar.RaidVarBuffer);
			gBS->FreePool(pTemp);
			pTemp = pNext;
		}
	}
	for(TableIndex = 0; TableIndex < VAR_HASH_NUM;++TableIndex){
		LsiVarHashTableName[TableIndex] = NULL;
		LsiVarHashTableVarId[TableIndex] = NULL;
	}
	return EFI_SUCCESS;
}

EFI_STATUS InitialLsiVarHashTable(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol){
	
	UINTN	TableIndex = 0;
	if(pProtocol == NULL) return EFI_LOAD_ERROR;
	if(pProtocol->SmcLsiCurrHiiHandleTable == NULL) return EFI_LOAD_ERROR;
	LsiVarHashTableName 	= pProtocol->SmcLsiCurrHiiHandleTable->RaidLsiVarHashTableName;
	LsiVarHashTableVarId 	= pProtocol->SmcLsiCurrHiiHandleTable->RaidLsiVarHashTableVarId;

	return EFI_SUCCESS;
}

SMC_RAID_VAR_HASH* AddLsiVarByName(SMC_RAID_VAR_HASH* Var){

	SMC_RAID_VAR_HASH*	pTemp 			= NULL;
	UINTN				VarNameIndex 	= 0;

	VarNameIndex = VarHashByName(Var->RaidVar.RaidVarName);

	pTemp = LsiVarHashTableName[VarNameIndex];

	if(!(!!pTemp)){
		LsiVarHashTableName[VarNameIndex] = Var;
	}else{
		do {
			if(!(!!pTemp->pRaidpNameNext)){
				pTemp->pRaidpNameNext = Var;
				break;
			}
		}while(!!(pTemp = pTemp->pRaidpNameNext));
	}
	pTemp = Var;

	return pTemp;
}
SMC_RAID_VAR_HASH* AddLsiVarByVId(SMC_RAID_VAR_HASH* Var){

	SMC_RAID_VAR_HASH*	pTemp 			= NULL;
	UINTN				VarIdIndex 		= 0;

	VarIdIndex = VarHashById(Var->RaidVar.RaidVarId);

	pTemp = LsiVarHashTableVarId[VarIdIndex];

	if(!(!!pTemp)){
		LsiVarHashTableVarId[VarIdIndex] = Var;
	}else{
		do {
			if(!(!!pTemp->pRaidpIdNext)){
				pTemp->pRaidpIdNext = Var;
				break;
			}
		}while(!!(pTemp = pTemp->pRaidpIdNext));
	}
	pTemp = Var;

	return pTemp;
}

SMC_RAID_VAR_HASH* AddLsiVarToTable(SMC_RAID_VAR_HASH* Var){

	SMC_RAID_VAR_HASH*	pTemp 			= NULL;

	pTemp = AddLsiVarByName(Var);
	pTemp = AddLsiVarByVId(Var);

	return pTemp;
}

SMC_RAID_VAR* 	SetLsiVarBuffer_byString(SMC_RAID_VAR* pVar,EFI_STRING DataString){

	EFI_STATUS	Status		  = EFI_SUCCESS;
	UINTN		VarDataSize	  = (pVar->RaidVarSize + VAR_UNUSED_SPACE);
	EFI_STRING	TempProgress  = NULL;

	if(pVar == NULL) return NULL;
	if(DataString == NULL) return NULL;

	if(pVar->RaidVarBuffer == NULL)
		gBS->AllocatePool(EfiBootServicesData,VarDataSize,&pVar->RaidVarBuffer);

	MemSet(pVar->RaidVarBuffer,VarDataSize,0x00);

	Status = GetHiiConfigRoutingProtocol()->ConfigToBlock(
					GetHiiConfigRoutingProtocol(),
					DataString,
					pVar->RaidVarBuffer,
					&VarDataSize,
					&TempProgress			
				);
	SMC_RAID_DETAIL_DEBUG((-1,"    SetLsiVarBuffer_byString ConfigToBlock Status[%r]\n",Status));
	if(EFI_ERROR(Status)){
		SMC_RAID_DETAIL_DEBUG((-1,"    TempProgress = \n"));
		SMC_RAID_DETAIL_DEBUG((-1,"        %s\n",TempProgress));
		MemSet(pVar->RaidVarBuffer,VarDataSize,0x00);
		return NULL;
	}

	return pVar;
}
SMC_RAID_VAR*	SetLsiVarBuffer(SMC_RAID_VAR *pVar, UINT8*	Buffer){
	EFI_STATUS	Status	= EFI_SUCCESS;
	
	if(pVar == NULL) return NULL;
	if(Buffer == NULL) return NULL;

	if(pVar->RaidVarBuffer == NULL){
		gBS->AllocatePool(EfiBootServicesData,pVar->RaidVarSize,&pVar->RaidVarBuffer);
		MemSet(pVar->RaidVarBuffer,pVar->RaidVarSize,0x00);
	}
	//Value is same.
	if(!(!!MemCmp(pVar->RaidVarBuffer,Buffer,pVar->RaidVarSize))) return pVar;

	MemCpy(pVar->RaidVarBuffer,Buffer,pVar->RaidVarSize);
	//We may examine buffer ?
	if(!(!!MemCmp(pVar->RaidVarBuffer,Buffer,pVar->RaidVarSize))) return pVar;

	return NULL;

}
UINT8*	GetLsiVarBuffer(SMC_RAID_VAR *pVar){

	if(pVar == NULL) return NULL;
	if(pVar->RaidVarBuffer == NULL) return NULL;

	return pVar->RaidVarBuffer;
}

UINT16	VarHashByName(CHAR8*	Name){
	UINT16	hashval = 0;
	UINTN	index = 0;

	for(index=0;index < Strlen(Name);++index){
		hashval += (UINT8)Name[index];
	}
	return (hashval % VAR_HASH_NUM);
}

UINT16	VarHashById(UINT16	VarId){

	return (VarId % VAR_HASH_NUM);
}

VOID Debug_for_LsiVarTable(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol){
#if defined(DEBUG_MODE) && (DEBUG_MODE == 1)
	UINTN	VarIndex = 0;
	SMC_RAID_VAR_HASH	*pTemp 	= NULL;
	SMC_RAID_VAR		*pVar	= NULL;

	DEBUG((-1,"LsiVarHashTableName :: \n"));
	for(VarIndex = 0;VarIndex < VAR_HASH_NUM; ++VarIndex){
		DEBUG((-1,"  Index[%02x] - \n",VarIndex));
		pTemp = LsiVarHashTableName[VarIndex];
		while(!!pTemp){
			pVar = &pTemp->RaidVar;
			pTemp = pTemp->pRaidpNameNext;
			DEBUG((-1,"      Name[%a], Guid[%g], Vid[%x], Size[%x]\n",pVar->RaidVarName,pVar->RaidVarGuid,pVar->RaidVarId,pVar->RaidVarSize));
		}
	}
	DEBUG((-1,"LsiVarHashTableVarId :: \n"));
	for(VarIndex = 0;VarIndex < VAR_HASH_NUM; ++VarIndex){
		DEBUG((-1,"  Index[%02x] - \n",VarIndex));
		pTemp = LsiVarHashTableVarId[VarIndex];
		while(!!pTemp){
			pVar = &pTemp->RaidVar;
			pTemp = pTemp->pRaidpIdNext;
			DEBUG((-1,"      Name[%a], Guid[%g], Vid[%x], Size[%x]\n",pVar->RaidVarName,pVar->RaidVarGuid,pVar->RaidVarId,pVar->RaidVarSize));
		}	
	}
#endif
}

SMC_RAID_VAR*	SearchLsiVarByName(CHAR8*	Name,EFI_GUID*	Guid){
	SMC_RAID_VAR_HASH*	pTemp 		= NULL;
	SMC_RAID_VAR*		pVar		= NULL;

	UINTN				VarIndex 	= 0;
	BOOLEAN				SearchFind  = FALSE;

	VarIndex = VarHashByName(Name);
	pTemp = LsiVarHashTableName[VarIndex];

	while(!!pTemp){
		pVar  = &pTemp->RaidVar;
		pTemp = pTemp->pRaidpNameNext;
		if(Strcmp(pVar->RaidVarName,Name) == 0  && ( Guid == NULL || MemCmp(&pVar->RaidVarGuid,Guid,sizeof(EFI_GUID)) == 0)){
			SearchFind = TRUE;
			break;
		}
	}

	return (!!SearchFind) ? pVar : NULL;
}
SMC_RAID_VAR*	SearchLsiVarById(UINT16 VarId){
	SMC_RAID_VAR_HASH*	pTemp 		= NULL;
	SMC_RAID_VAR*		pVar		= NULL;

	UINTN				VarIndex 	= 0;
	BOOLEAN				SearchFind  = FALSE;

	VarIndex = VarHashById(VarId);
	pTemp = LsiVarHashTableVarId[VarIndex];
	while(!!pTemp){
		pVar  = &pTemp->RaidVar;
		pTemp = pTemp->pRaidpIdNext;

		if(pVar->RaidVarId == VarId){
			SearchFind = TRUE;
			break;
		}
	}
	return (!!SearchFind) ? pVar : NULL;
}

VOID VarInitial(SMC_RAID_VAR_HASH* pTemp,CHAR8*	Name,EFI_GUID*	Guid,EFI_VARSTORE_ID VarId,UINT16 VarSize){

	Strcpy(pTemp->RaidVar.RaidVarName,Name);
	MemCpy(&pTemp->RaidVar.RaidVarGuid,Guid,sizeof(EFI_GUID));
	pTemp->RaidVar.RaidVarSize		= VarSize;
	pTemp->RaidVar.RaidVarId		= VarId;
	pTemp->pRaidpNameNext			= NULL;
	pTemp->pRaidpIdNext				= NULL;

	return;
}
 
SMC_RAID_VAR*	AddLsiVarByBuffer(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol,CHAR8*	Name,EFI_GUID*	Guid,EFI_VARSTORE_ID VarId,UINT16 VarSize,UINT8* pBuffer){

	SMC_RAID_VAR_HASH*	pTemp 		= NULL;
	SMC_RAID_VAR*		pVar		= NULL;
	EFI_STATUS			Status 		= EFI_SUCCESS;
	UINTN				VarIndex	= 0;

	pVar = SearchLsiVarByName(Name,Guid);
	//Maybe we should overwride for memory reused?
	if(pVar == NULL){
		Status = gBS->AllocatePool(EfiBootServicesData,sizeof(SMC_RAID_VAR_HASH),(VOID**)(&pVar));
		if(EFI_ERROR(Status)) {
			SettingErrorStatus(pProtocol,0x30,Status);
			return NULL;
		}
		MemSet(pVar,sizeof(SMC_RAID_VAR_HASH),0x00);
	}
	pTemp = (SMC_RAID_VAR_HASH*)pVar;

	VarInitial(pTemp,Name,Guid,VarId,VarSize);

	if(EFI_ERROR(SetLsiVarBuffer(&pTemp->RaidVar,pBuffer))) {
		SettingErrorStatus(pProtocol,0x32,Status);
		return NULL;
	}
	AddLsiVarToTable(pTemp);
	
	return &pTemp->RaidVar;
}

SMC_RAID_VAR*	AddLsiVarByBuffer2(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol,CHAR8*	Name,EFI_GUID*	Guid,EFI_VARSTORE_ID VarId,UINT16 VarSize,UINT8* pBuffer){

	SMC_RAID_VAR_HASH*	pTemp 		= NULL;
	SMC_RAID_VAR*		pVar		= NULL;
	EFI_STATUS			Status 		= EFI_SUCCESS;
	UINTN				VarIndex	= 0;

	pVar = SearchLsiVarByName(Name,Guid);
	//Maybe we should overwride for memory reused?
	if(pVar == NULL){
		Status = gBS->AllocatePool(EfiBootServicesData,sizeof(SMC_RAID_VAR_HASH),(VOID**)(&pVar));
		if(EFI_ERROR(Status)) {
			SettingErrorStatus(pProtocol,0x30,Status);
			return NULL;
		}
		MemSet(pVar,sizeof(SMC_RAID_VAR_HASH),0x00);
	}
	pTemp = (SMC_RAID_VAR_HASH*)pVar;

	VarInitial(pTemp,Name,Guid,VarId,VarSize);
	pTemp->RaidVar.RaidVarBuffer		= pBuffer;

	AddLsiVarToTable(pTemp);

	return &pTemp->RaidVar;
}

SMC_RAID_VAR*	AddLsiVar(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol,CHAR8*	Name,EFI_GUID*	Guid,EFI_VARSTORE_ID VarId,UINT16 VarSize){

	SMC_RAID_VAR_HASH*	pTemp 		= NULL;
	SMC_RAID_VAR*		pVar		= NULL;
	EFI_STATUS			Status = EFI_SUCCESS;
  	EFI_STRING			ConfigHdrTemp = NULL;
  	EFI_STRING			ExTractConfigHdr = NULL;

  	EFI_STRING			TempProgress = NULL;
  	EFI_STRING			TempResult = NULL;
	
	static CHAR16		UVarName[NAME_LENGTH];

	MemSet(UVarName,NAME_LENGTH * sizeof(CHAR16),0x00);
	DEBUG((-1,"Name[%a], Guid[%g], VarId[%x], VarSize[%x] :: ",Name,*Guid,VarId,VarSize));

	pVar = SearchLsiVarByName(Name,Guid);
	//Maybe we should overwride for memory reused?
	if(pVar == NULL){
		Status = gBS->AllocatePool(EfiBootServicesData,sizeof(SMC_RAID_VAR_HASH),(VOID**)(&pVar));
		if(EFI_ERROR(Status)) {
			SettingErrorStatus(pProtocol,0x30,Status);
			return NULL;
		}
		MemSet(pVar,sizeof(SMC_RAID_VAR_HASH),0x00);
	}
	pTemp = (SMC_RAID_VAR_HASH*)pVar;

	VarInitial(pTemp,Name,Guid,VarId,VarSize);
	AsciiStrToUnicodeStr(Name,UVarName);
	
	ConfigHdrTemp = HiiConstructConfigHdr(Guid,UVarName,GetSmcRaidCurrDriverHandle());
	ExTractConfigHdr = AppendOffsetWidth(ConfigHdrTemp,0,VarSize);

//	DEBUG((-1,"ExTractConfigHdr = %s\n",ExTractConfigHdr));

	Status = pProtocol->SmcLsiCurrHiiHandleTable->SmcLsiCurrConfigAccess->ExtractConfig(
			pProtocol->SmcLsiCurrHiiHandleTable->SmcLsiCurrConfigAccess,
			ExTractConfigHdr,&TempProgress,&TempResult);

	DEBUG((-1,"ExtractConfig Status[%r]\n",Status));
	if(EFI_ERROR(Status)) {
		SettingErrorStatus(pProtocol,0x31,Status);
		return NULL;
	}
//	DEBUG((-1,"TempResult = %s\n",TempResult));

	if(SetLsiVarBuffer_byString(pVar,TempResult) == NULL) {
		SettingErrorStatus(pProtocol,0x32,Status);
		return NULL;
	}
	AddLsiVarToTable(pTemp);

	return &pTemp->RaidVar;
}

SMC_RAID_VAR*	SearchInSmcSetupVarById(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol,UINT16 SmcVarId){

	SMC_LSI_HII_HANDLE*		pSmcHiiHandle	= NULL;
	SMC_RAID_VAR_HASH**		pOriVarIdTable  = NULL;
	SMC_RAID_VAR*			pRaidVar		= NULL;

	pOriVarIdTable = LsiVarHashTableVarId;

	for(pSmcHiiHandle = pProtocol->SmcLsiHiiHandleTable;
		pSmcHiiHandle != NULL && pSmcHiiHandle->RaidCardType != RAID_NULL;
		pSmcHiiHandle = pSmcHiiHandle->pNext){
	
		LsiVarHashTableVarId = pSmcHiiHandle->RaidLsiVarHashTableVarId;
		pRaidVar = SearchLsiVarById(SmcVarId);
		if(!!pRaidVar) break;
	}

	LsiVarHashTableVarId = pOriVarIdTable;
	return pRaidVar;
}

SMC_RAID_VAR*	SearchInSmcSetupVarByName(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol,CHAR8* SmcVarName, EFI_GUID* pGuid){

	SMC_LSI_HII_HANDLE*		pSmcHiiHandle		= NULL;
	SMC_RAID_VAR_HASH**		pOriVarNameTable 	= NULL;
	SMC_RAID_VAR*			pRaidVar			= NULL;

	pOriVarNameTable = LsiVarHashTableName;

	for(pSmcHiiHandle = pProtocol->SmcLsiHiiHandleTable;
		pSmcHiiHandle != NULL && pSmcHiiHandle->RaidCardType != RAID_NULL;
		pSmcHiiHandle = pSmcHiiHandle->pNext){
	
		LsiVarHashTableName = pSmcHiiHandle->RaidLsiVarHashTableName;
		pRaidVar = SearchLsiVarByName(SmcVarName,pGuid);
		if(!!pRaidVar) break;
	}

	LsiVarHashTableName = pOriVarNameTable;
	return pRaidVar;
}

SMC_RAID_VAR*	SearchInTargetHandleSmcSetupVarByName(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol, SMC_LSI_HII_HANDLE* pSmcHiiHandle, CHAR8* SmcVarName, EFI_GUID* pGuid){

	SMC_RAID_VAR_HASH**		pOriVarNameTable 	= NULL;
	SMC_RAID_VAR*			pRaidVar			= NULL;

	if(!(!!pSmcHiiHandle)) return NULL;

	pOriVarNameTable = LsiVarHashTableName;
	LsiVarHashTableName = pSmcHiiHandle->RaidLsiVarHashTableName;

	pRaidVar = SearchLsiVarByName(SmcVarName,pGuid);

	LsiVarHashTableName = pOriVarNameTable;
	
	return pRaidVar;
}

SMC_LSI_HII_HANDLE*	SearchHiiHandleTableByVarName(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol,CHAR8* SmcVarName, EFI_GUID* pGuid){

	SMC_LSI_HII_HANDLE*		pSmcHiiHandle		= NULL;
	SMC_RAID_VAR_HASH**		pOriVarNameTable 	= NULL;
	SMC_RAID_VAR*			pRaidVar			= NULL;

	pOriVarNameTable = LsiVarHashTableName;

	for(pSmcHiiHandle = pProtocol->SmcLsiHiiHandleTable;
		pSmcHiiHandle != NULL && pSmcHiiHandle->RaidCardType != RAID_NULL;
		pSmcHiiHandle = pSmcHiiHandle->pNext){
	
		LsiVarHashTableName = pSmcHiiHandle->RaidLsiVarHashTableName;
		pRaidVar = SearchLsiVarByName(SmcVarName,pGuid);
		if(!!pRaidVar) break;
	}

	LsiVarHashTableName = pOriVarNameTable;
	return pSmcHiiHandle;
}

// ======== LSI VAR END   ========

EFI_STATUS	RaidDownTheLastCleanFunc(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol){

	return EFI_SUCCESS;
}

EFI_STATUS SetLsiRaidFormInformTable(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol,EFI_IFR_REF*	LsiIfrRef){
	UINTN					index = 0;
	EFI_STRING				FormString = NULL;
	EFI_HII_HANDLE			LsiHiiHandle = NULL;
	SMC_LSI_RAID_FORM_SET*	LocRefSearchTable = NULL;	

	LsiHiiHandle = pProtocol->SmcLsiCurrHiiHandleTable->RaidCardHiiHandle;

	FormString = GetHiiString(LsiHiiHandle,LsiIfrRef->Question.Header.Prompt);
	if(! (!!FormString)){
		return EFI_OUT_OF_RESOURCES;
	}

	for(LocRefSearchTable = pProtocol->SmcLsiCurrHiiHandleTable->RaidCardAccessForms;
		LocRefSearchTable != NULL; 
		LocRefSearchTable = LocRefSearchTable->pFormNext){

		if(StrCmp(FormString,LocRefSearchTable->FormHeader.Lsi_FormName) == 0 && !(!!LocRefSearchTable->FormBody.BeUsed)){	
			DEBUG((-1,"  Update :: RaidCardType[%x], RaidCardIndex[%x], FormName[%s], FormId[%x], Qid[%x], Vid[%x], VOff[%x], Flags[%x]\n",
									pProtocol->SmcLsiCurrHiiHandleTable->RaidCardType,
									pProtocol->SmcLsiCurrHiiHandleTable->RaidCardIndex,
									FormString,
									LsiIfrRef->FormId,LsiIfrRef->Question.QuestionId,
									LsiIfrRef->Question.VarStoreId,LsiIfrRef->Question.VarStoreInfo.VarOffset,
									LsiIfrRef->Question.Flags));

			LocRefSearchTable->FormBody.Lsi_FormId 	= LsiIfrRef->FormId;
			LocRefSearchTable->FormBody.Lsi_QId  	= LsiIfrRef->Question.QuestionId;
			LocRefSearchTable->FormBody.Lsi_VId  	= LsiIfrRef->Question.VarStoreId;
			LocRefSearchTable->FormBody.Lsi_Voff 	= LsiIfrRef->Question.VarStoreInfo.VarOffset;
			LocRefSearchTable->FormBody.Lsi_Flags	= LsiIfrRef->Question.Flags;
			LocRefSearchTable->FormBody.BeUsed		= TRUE;
		}
	}
	return EFI_SUCCESS;
}

EFI_STATUS SmcLsiRaidOOB_InitialFunc(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol){

	EFI_STATUS			Status = EFI_SUCCESS;
	EFI_HII_HANDLE		LsiHiiHandle	= NULL;

	if(pProtocol == NULL) return EFI_LOAD_ERROR;

	LsiHiiHandle = pProtocol->SmcLsiCurrHiiHandleTable->RaidCardHiiHandle;

	if(LsiHiiHandle == NULL) return SettingErrorStatus(pProtocol,0x01,Status);

	pProtocol->SmcLsiCurrHiiHandleTable->RaidCardDriverHandle = NULL;
  	Status = GetHiiDataBase()->GetPackageListHandle(GetHiiDataBase(),LsiHiiHandle,&pProtocol->SmcLsiCurrHiiHandleTable->RaidCardDriverHandle);
	if(EFI_ERROR(Status)) return SettingErrorStatus(pProtocol,0x02,Status);

	pProtocol->SmcLsiCurrHiiHandleTable->SmcLsiCurrConfigAccess = NULL;
  	Status = gBS->HandleProtocol(pProtocol->SmcLsiCurrHiiHandleTable->RaidCardDriverHandle,
			                     &gEfiHiiConfigAccessProtocolGuid,
								 &pProtocol->SmcLsiCurrHiiHandleTable->SmcLsiCurrConfigAccess);

	if(EFI_ERROR(Status)) return SettingErrorStatus(pProtocol,0x03,Status);

	pProtocol->SmcLsiCurrHiiHandleTable->HiiFlags = 0;

	return EFI_SUCCESS;
}

VOID SmcLsiRaidOOB_SendFormCallBack(){

}

VOID Debug_for_RaidItems(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol){

#if defined(DEBUG_MODE) && (DEBUG_MODE == 1)

	SMC_RAID_ITEMS_SET*	pItemSet = pProtocol->SmcLsiCurrHiiHandleTable->RaidCardInfItems;

	DEBUG((-1,"RaidCardInfItemsTable :: \n"));
	DEBUG((-1,"  CardType[%x], CardIndex[%x] - \n",
			  pProtocol->SmcLsiCurrHiiHandleTable->RaidCardType,
			  pProtocol->SmcLsiCurrHiiHandleTable->RaidCardIndex));

	for(;pItemSet != NULL;pItemSet = pItemSet->pItemsNext){
		SMC_RAID_ITEMS_BODY*	pItemsBody	= NULL;
		pItemsBody = pItemSet->ItemsBody;

		DEBUG((-1,"    FormName[%s], ItemName[%s], ItemId[%x], FIND[%s]\n",
				   pItemSet->ItemsHeader.LsiItemForm,
				   pItemSet->ItemsHeader.LsiItemName,
				   pItemSet->ItemsHeader.LsiItemId,
				   pItemsBody != NULL ? L"YES" : L"NO"));

		while(!!pItemsBody){
			DEBUG((-1,"      SmcVarId[%x], HDDNum[%x], HII[%s]\n",
				       pItemsBody->SmcLsiVarId,
				       pItemsBody->HdNum,
				       pItemsBody->pLsiItemOp != NULL ? L"HAVE" : L"NON"
			 	 ));
			pItemsBody = pItemsBody->pItemsBodyNext;
		}
	}
#endif
}

VOID Debug_for_RaidChRecords(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol){

#if defined(DEBUG_MODE) && (DEBUG_MODE == 1)
	SMC_RAID_CHRECORD_SET* pLocSmcChRSet = pProtocol->SmcLsiCurrHiiHandleTable->RaidCardChRecordTable;

	DEBUG((-1,"RaidCardChRecordTable :: \n"));
	DEBUG((-1,"  CardType[%x], CardIndex[%x] - \n",
			  pProtocol->SmcLsiCurrHiiHandleTable->RaidCardType,
			  pProtocol->SmcLsiCurrHiiHandleTable->RaidCardIndex));

	for(;pLocSmcChRSet != NULL;pLocSmcChRSet = pLocSmcChRSet->ChRecordNext){

		DEBUG((-1,"    ChRFrom[%s], ChRTarget[%s], TargetOpCode[%x], TargetQId[%x], TargetVId[%x], BeAccessed[%x]\n",
				   pLocSmcChRSet->ChRecordHeader.ChRFrom,
				   pLocSmcChRSet->ChRecordHeader.ChRTarget,
				   pLocSmcChRSet->ChRecordBody.TargetOpCode,
				   pLocSmcChRSet->ChRecordBody.TargetQId,
				   pLocSmcChRSet->ChRecordBody.TargetVId,
				   pLocSmcChRSet->ChRecordBody.BeAccessed
			 ));
	}
#endif
}

HII_PACKAGE_LIST_FROM_SET* SmcLsiRaidOOB_GetCurrentPackageForm(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol){
	
	static	HII_PACKAGE_LIST_FROM_SET sListFormSet = {NULL, NULL};

	EFI_HII_HANDLE				 LsiHiiHandle			= NULL;
	EFI_HII_PACKAGE_HEADER 		*LsiIfrPackagePtr 		= NULL;
	EFI_HII_PACKAGE_LIST_HEADER *LsiHiiPackageList 		= NULL;

	sListFormSet.PackFormHeader = NULL;
	sListFormSet.PackListHeader = NULL;
	LsiHiiHandle = pProtocol->SmcLsiCurrHiiHandleTable->RaidCardHiiHandle;
	
	LsiHiiPackageList = GetHiiPackageList(LsiHiiHandle);
	if(! (!!LsiHiiPackageList)) return NULL;

	LsiIfrPackagePtr = (EFI_HII_PACKAGE_HEADER*)(LsiHiiPackageList+1);
	while((UINT32)LsiIfrPackagePtr < (UINT32)((UINT8*)LsiHiiPackageList + LsiHiiPackageList->PackageLength)){
	  	if(LsiIfrPackagePtr->Type == EFI_HII_PACKAGE_FORMS) break;
		LsiIfrPackagePtr = (EFI_HII_PACKAGE_HEADER*)((UINT8*)LsiIfrPackagePtr + LsiIfrPackagePtr->Length);
	}
	
	if((UINT32)LsiIfrPackagePtr >= (UINT32)((UINT8*)LsiHiiPackageList + LsiHiiPackageList->PackageLength))
		LsiIfrPackagePtr = NULL;

	sListFormSet.PackListHeader = LsiHiiPackageList;
	sListFormSet.PackFormHeader = LsiIfrPackagePtr;

	return &sListFormSet;
}

EFI_STATUS SmcLsiRaidOOB_CollectInformation_Form(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol){

	EFI_STATUS		   			Status 					= EFI_SUCCESS;
	EFI_HII_PACKAGE_HEADER*		LsiIfrPackagePtr		= NULL;
	EFI_IFR_OP_HEADER* 			LsipEFI_IFR_OP_HEADER 	= NULL;

	HII_PACKAGE_LIST_FROM_SET*	sListFormSet			= NULL;

	sListFormSet = SmcLsiRaidOOB_GetCurrentPackageForm(pProtocol);
	if(! (!!sListFormSet)) return SettingErrorStatus(pProtocol,0x85,EFI_NOT_FOUND);

	LsiIfrPackagePtr = sListFormSet->PackFormHeader;
	if(! (!!LsiIfrPackagePtr)) return SettingErrorStatus(pProtocol,0x87,EFI_NOT_FOUND);

	LsipEFI_IFR_OP_HEADER = (EFI_IFR_OP_HEADER*)(LsiIfrPackagePtr + 1);

	while((UINT32)LsipEFI_IFR_OP_HEADER < (UINT32)((UINT8*)LsiIfrPackagePtr + LsiIfrPackagePtr->Length)){
		switch(LsipEFI_IFR_OP_HEADER->OpCode){
			case EFI_IFR_REF_OP:
			{
				Status = SetLsiRaidFormInformTable(pProtocol,(EFI_IFR_REF*)LsipEFI_IFR_OP_HEADER);
				if(EFI_ERROR(Status)) return SettingErrorStatus(pProtocol,0x10,Status);
			}
				break;
			case EFI_IFR_VARSTORE_OP:
			{
				EFI_IFR_VARSTORE*	LsiIfrVar = (EFI_IFR_VARSTORE*)LsipEFI_IFR_OP_HEADER;
				SMC_RAID_VAR*		TempVar   = NULL;
				TempVar = AddLsiVar(pProtocol,LsiIfrVar->Name,&LsiIfrVar->Guid,LsiIfrVar->VarStoreId,LsiIfrVar->Size);
				if(TempVar == NULL) return SettingErrorStatus(pProtocol,0x20,Status);
			}
				break;
			default:
				break;
		}
		LsipEFI_IFR_OP_HEADER = (EFI_IFR_OP_HEADER*)((UINT8*)LsipEFI_IFR_OP_HEADER + LsipEFI_IFR_OP_HEADER->Length);
	}

	Debug_for_LsiVarTable(pProtocol);
	//Below line may cause debug assert?
	if(!!sListFormSet->PackListHeader)
		gBS->FreePool(sListFormSet->PackListHeader);
	return Status;
}

EFI_STATUS SmcLsiRaidOOB_CollectInformation_Items(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol){


	EFI_STATUS		   			Status 					= EFI_SUCCESS;
	EFI_HII_PACKAGE_HEADER*		LsiIfrPackagePtr		= NULL;
	EFI_IFR_OP_HEADER* 			LsipEFI_IFR_OP_HEADER 	= NULL;
	SMC_RAID_ITEMS_SET*	   		LocSmcLsiRaidItemsTable	= NULL;
	SMC_RAID_ITEMS_SET*			pItemsSet				= NULL;
	EFI_HII_HANDLE	   			LsiHiiHandle			= NULL;
	HII_PACKAGE_LIST_FROM_SET*	sListFormSet			= NULL;
	
	CHAR16			   			CurrentFormString[TEMP_FORM_STRING];

	UINT16			   			HddNum     		= 0;
	UINT16			   			NonHddNum    	= 0xFFFF;
	UINT16*			   			pHddNum			= NULL;

	LsiHiiHandle 			=	pProtocol->SmcLsiCurrHiiHandleTable->RaidCardHiiHandle;
	LocSmcLsiRaidItemsTable = 	pProtocol->SmcLsiCurrHiiHandleTable->RaidCardInfItems;

	sListFormSet = SmcLsiRaidOOB_GetCurrentPackageForm(pProtocol);
	if(! (!!sListFormSet)) return SettingErrorStatus(pProtocol,0x85,EFI_NOT_FOUND);

	LsiIfrPackagePtr = sListFormSet->PackFormHeader;
	if(! (!!LsiIfrPackagePtr)) return SettingErrorStatus(pProtocol,0x87,EFI_NOT_FOUND);

	LsipEFI_IFR_OP_HEADER = (EFI_IFR_OP_HEADER*)(LsiIfrPackagePtr + 1);

	while((UINT32)LsipEFI_IFR_OP_HEADER < (UINT32)((UINT8*)LsiIfrPackagePtr + LsiIfrPackagePtr->Length)){
		switch(LsipEFI_IFR_OP_HEADER->OpCode){
			case EFI_IFR_FORM_OP:
			{
				EFI_IFR_FORM*	pFormOp = (EFI_IFR_FORM*)LsipEFI_IFR_OP_HEADER;
				EFI_STRING		TempString = NULL;

				MemSet(CurrentFormString,TEMP_FORM_STRING * sizeof(CHAR16),0x00);
				TempString = GetHiiString(LsiHiiHandle,pFormOp->FormTitle);
				StrCpy(CurrentFormString,TempString);

				SMC_RAID_DETAIL_DEBUG((-1,"Current Form :: [%s]\n",CurrentFormString));
			}
				break;
			case EFI_IFR_STRING_OP:
			case EFI_IFR_ONE_OF_OP:
			case EFI_IFR_NUMERIC_OP:
			case EFI_IFR_CHECKBOX_OP:
			case EFI_IFR_REF_OP:
			{
				CHAR16*	IfrString = NULL;
				SMC_LSI_ITEMS_COMMON_HEADER*	ItemsComnHead = NULL;

				ItemsComnHead = (SMC_LSI_ITEMS_COMMON_HEADER*)LsipEFI_IFR_OP_HEADER;
				IfrString = GetHiiString(LsiHiiHandle,ItemsComnHead->Question.Header.Prompt);
				if(! (!!IfrString)){
					SMC_RAID_DETAIL_DEBUG((-1,"StringId[%x], OP[%x], QId[%x] cannot get string\n",
												ItemsComnHead->Question.Header.Prompt,
												ItemsComnHead->Header.OpCode,
												ItemsComnHead->Question.QuestionId));
					break;
				}

				pHddNum = &NonHddNum;
				for(pItemsSet = LocSmcLsiRaidItemsTable;pItemsSet != NULL;pItemsSet = pItemsSet->pItemsNext){
					if( !!StrCmp(pItemsSet->ItemsHeader.LsiItemForm,CurrentFormString)) continue;
					if( ItemsComnHead->Question.QuestionId >= pItemsSet->ItemsHeader.LsiItemId ||  
						!(!!StrCmp(pItemsSet->ItemsHeader.LsiItemName,IfrString))){
						//If pItemsSet not HDG items table, continue.
						//If pItemsSet is HDG items table, it already pass the LsiItemId.
						if(!(!!StrCmp(pItemsSet->ItemsHeader.LsiItemName,pProtocol->SmcLsiGetHdgName(pProtocol))) || 
						   !(!!StrCmp(pItemsSet->ItemsHeader.LsiItemName,pProtocol->SmcLsiGetRdgName(pProtocol))) ||
						   !(!!StrCmp(pItemsSet->ItemsHeader.LsiItemName,pProtocol->SmcLsiGetJBODName(pProtocol)))
						  ){
							//In Here, it means it's HDD items. 
							pHddNum = &HddNum;
							++HddNum;
						}
						break;
					}
				}
				if(pItemsSet == NULL) break;

				SMC_RAID_DETAIL_DEBUG((-1,"IfrString[%s], VarStoreId[%x] OP[%x] QId[%x] >> \n",
							IfrString,ItemsComnHead->Question.VarStoreId,LsipEFI_IFR_OP_HEADER->OpCode,
							ItemsComnHead->Question.QuestionId));

				{
					SMC_RAID_ITEMS_BODY*	pItemsBody = NULL;
					gBS->AllocatePool(EfiBootServicesData,sizeof(SMC_RAID_ITEMS_BODY),&pItemsBody);
					MemSet(pItemsBody,sizeof(SMC_RAID_ITEMS_BODY),0x00);
						
					pItemsBody->pLsiItemOp 		= LsipEFI_IFR_OP_HEADER;
					pItemsBody->SmcLsiVarId 	= 0x0;
					pItemsBody->HdNum			= *pHddNum;
					pItemsBody->pItemsBodyNext	= NULL;

					if(!(!!pItemsSet->ItemsBody)){
						pItemsSet->ItemsBody = pItemsBody;
					}else{
						SMC_RAID_ITEMS_BODY*	pTempItemsBody = NULL;
						pTempItemsBody = GetPNextStartAddr(pItemsSet->ItemsBody,STRUCT_OFFSET(SMC_RAID_ITEMS_BODY,pItemsBodyNext));
						pTempItemsBody->pItemsBodyNext = pItemsBody;
					}
				}
			}
				break;
			default:
				break;
		}
		LsipEFI_IFR_OP_HEADER = (EFI_IFR_OP_HEADER*)((UINT8*)LsipEFI_IFR_OP_HEADER + LsipEFI_IFR_OP_HEADER->Length);
	}

	Debug_for_RaidItems(pProtocol);	
	return Status;

}

EFI_STATUS SmcLsiRaidOOB_CollectInformation_ChRec(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol){

	EFI_STATUS		   			Status 					= EFI_SUCCESS;
	EFI_HII_PACKAGE_HEADER*		LsiIfrPackagePtr		= NULL;
	EFI_IFR_OP_HEADER* 			LsipEFI_IFR_OP_HEADER 	= NULL;
	EFI_HII_HANDLE	   			LsiHiiHandle			= NULL;

	HII_PACKAGE_LIST_FROM_SET*	sListFormSet			= NULL;
	CHAR16			   			CurrentFormString[TEMP_FORM_STRING];

	sListFormSet = SmcLsiRaidOOB_GetCurrentPackageForm(pProtocol);
	if(!(!!sListFormSet)) return SettingErrorStatus(pProtocol,0x85,EFI_NOT_FOUND);

	LsiIfrPackagePtr = sListFormSet->PackFormHeader;
	if(!(!!LsiIfrPackagePtr)) return SettingErrorStatus(pProtocol,0x87,EFI_NOT_FOUND);

	LsiHiiHandle 			=	pProtocol->SmcLsiCurrHiiHandleTable->RaidCardHiiHandle;

	LsipEFI_IFR_OP_HEADER = (EFI_IFR_OP_HEADER*)(LsiIfrPackagePtr + 1);

	while((UINT32)LsipEFI_IFR_OP_HEADER < (UINT32)((UINT8*)LsiIfrPackagePtr + LsiIfrPackagePtr->Length)){
		switch(LsipEFI_IFR_OP_HEADER->OpCode){
			case EFI_IFR_FORM_OP:
			{
				EFI_IFR_FORM*	pFormOp = (EFI_IFR_FORM*)LsipEFI_IFR_OP_HEADER;
				EFI_STRING		TempString = NULL;

				MemSet(CurrentFormString,TEMP_FORM_STRING * sizeof(CHAR16),0x00);
				TempString = GetHiiString(LsiHiiHandle,pFormOp->FormTitle);
				StrCpy(CurrentFormString,TempString);

				SMC_RAID_DETAIL_DEBUG((-1,"Current Form :: [%s]\n",CurrentFormString));
			}
				break;
			case EFI_IFR_REF_OP:
			{
				CHAR16*							IfrString 		= NULL;
				SMC_LSI_ITEMS_COMMON_HEADER*	ItemsComnHead 	= NULL;
				SMC_RAID_CHRECORD_SET*			pLocSmcChRSet	= NULL;

				ItemsComnHead = (SMC_LSI_ITEMS_COMMON_HEADER*)LsipEFI_IFR_OP_HEADER;
				IfrString = GetHiiString(LsiHiiHandle,ItemsComnHead->Question.Header.Prompt);
				if(! (!!IfrString)){
					SMC_RAID_DETAIL_DEBUG((-1,"StringId[%x], OP[%x], QId[%x] cannot get string\n",
												ItemsComnHead->Question.Header.Prompt,
												ItemsComnHead->Header.OpCode,
												ItemsComnHead->Question.QuestionId));
					break;
				}
				SMC_RAID_DETAIL_DEBUG((-1,"IfrString[%s], VarStoreId[%x] OP[%x] QId[%x] >> \n",
							IfrString,ItemsComnHead->Question.VarStoreId,LsipEFI_IFR_OP_HEADER->OpCode,
							ItemsComnHead->Question.QuestionId));

				pLocSmcChRSet			= 	pProtocol->SmcLsiCurrHiiHandleTable->RaidCardChRecordTable;
				while(!!pLocSmcChRSet){
					
					if((!(!!StrCmp(pLocSmcChRSet->ChRecordHeader.ChRFrom,CurrentFormString))) && 
					   (!(!!StrCmp(pLocSmcChRSet->ChRecordHeader.ChRTarget,IfrString)))){

						SMC_RAID_DETAIL_DEBUG((-1,"  -- ChRFrom[%s], ChRTarget[%s]\n",pLocSmcChRSet->ChRecordHeader.ChRFrom,pLocSmcChRSet->ChRecordHeader.ChRTarget));

						pLocSmcChRSet->ChRecordBody.TargetOpCode = ItemsComnHead->Header.OpCode;
						pLocSmcChRSet->ChRecordBody.TargetQId	 = ItemsComnHead->Question.QuestionId;
						pLocSmcChRSet->ChRecordBody.TargetVId	 = ItemsComnHead->Question.VarStoreId;
						pLocSmcChRSet->ChRecordBody.BeAccessed	 = FALSE;
						break;
					}
					pLocSmcChRSet = pLocSmcChRSet->ChRecordNext;
				}
			}
				break;
			default:
				break;
		}
		LsipEFI_IFR_OP_HEADER = (EFI_IFR_OP_HEADER*)((UINT8*)LsipEFI_IFR_OP_HEADER + LsipEFI_IFR_OP_HEADER->Length);
	}

	Debug_for_RaidChRecords(pProtocol);
	//Below line may cause debug assert?
	if(!!sListFormSet->PackListHeader)
		gBS->FreePool(sListFormSet->PackListHeader);
	return EFI_SUCCESS;
}

VOID Debug_for_SmcLsiVarTable(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol){

#if defined(DEBUG_MODE) && (DEBUG_MODE == 1)

	UINTN						VarIndex				= 0;
	SMC_SETUP_RAID_VAR*			pSetupRaidVar			= NULL;

	pSetupRaidVar = pProtocol->SmcLsiCurrHiiHandleTable->RaidSetupVarSet;

	DEBUG((-1,"RaidSetupVarSet CardType[%x], CardIndex[%x] :: \n",
				pProtocol->SmcLsiCurrHiiHandleTable->RaidCardType,
				pProtocol->SmcLsiCurrHiiHandleTable->RaidCardIndex));

	for(;pSetupRaidVar != NULL; pSetupRaidVar = pSetupRaidVar->pSetupRaidVarNext){
		SMC_RAID_VAR*	pVar = NULL;
		pVar = &pSetupRaidVar->SetupRaidVar;
		DEBUG((-1,"  Name[%a], Guid[%g], VarId[%x], VarSize[%x]\n",
					pVar->RaidVarName,pVar->RaidVarGuid,pVar->RaidVarId,pVar->RaidVarSize));
		}
#endif
}


EFI_STATUS	SmcLsiSetSmcLsiVariableTable(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol){

	SMC_SETUP_RAID_VAR*				pSetupRaidVar			= NULL;
	SMC_RAID_ITEMS_SET*				pItemsSet				= NULL;
	SMC_LSI_ITEMS_COMMON_HEADER*	pCmnHeader				= NULL;
	CHAR8							cVarName[NAME_LENGTH];
	
	pItemsSet 		= pProtocol->SmcLsiCurrHiiHandleTable->RaidCardInfItems;
	pSetupRaidVar	= pProtocol->SmcLsiCurrHiiHandleTable->RaidSetupVarSet;

	for(;pItemsSet != NULL;pItemsSet = pItemsSet->pItemsNext){
		SMC_RAID_ITEMS_BODY*	pItemsBody = NULL;
		
		for(pItemsBody = pItemsSet->ItemsBody;pItemsBody != NULL; pItemsBody = pItemsBody->pItemsBodyNext){
			SMC_RAID_VAR*	rVar 		= NULL;
			SMC_RAID_VAR*	cVar 		= NULL;

			pCmnHeader = (SMC_LSI_ITEMS_COMMON_HEADER*)pItemsBody->pLsiItemOp;
			if(pCmnHeader->Header.OpCode == EFI_IFR_REF_OP) continue;

			MemSet(cVarName,NAME_LENGTH * sizeof(CHAR8),0x00);

			rVar = SearchLsiVarById(pCmnHeader->Question.VarStoreId);
			if(rVar == NULL) return SettingErrorStatus(pProtocol,0x77,EFI_NOT_FOUND);

			Sprintf(cVarName,"%s_%s_%x_%x",
					 pProtocol->SmcLsiGetRaidVarName(pProtocol),
					 rVar->RaidVarName,
					 pProtocol->SmcLsiCurrHiiHandleTable->RaidCardType,
					 pProtocol->SmcLsiCurrHiiHandleTable->RaidCardIndex);
			
			cVar = SearchLsiVarByName(cVarName,pProtocol->SmcLsiGetVarGuid(pProtocol));

			if(!(!!cVar)){
				SMC_SETUP_RAID_VAR* pTempSetupRaidVar = NULL;

				cVar = AddLsiVarByBuffer2(
						pProtocol,
						cVarName,
						pProtocol->SmcLsiGetVarGuid(pProtocol),
						pProtocol->SmcLsiGetVIdStart(pProtocol),
						rVar->RaidVarSize,
						rVar->RaidVarBuffer
						);

				gBS->AllocatePool(EfiBootServicesData,sizeof(SMC_SETUP_RAID_VAR),&pTempSetupRaidVar);
				MemSet(pTempSetupRaidVar,sizeof(SMC_SETUP_RAID_VAR),0x00);
				MemCpy(&pTempSetupRaidVar->SetupRaidVar,cVar,sizeof(SMC_RAID_VAR));

				if(pSetupRaidVar == NULL){
					pSetupRaidVar = pProtocol->SmcLsiCurrHiiHandleTable->RaidSetupVarSet = pTempSetupRaidVar;
				}else{
					pSetupRaidVar->pSetupRaidVarNext = pTempSetupRaidVar;
					pSetupRaidVar = pSetupRaidVar->pSetupRaidVarNext;
				}

				if(!!pProtocol->DetailedDebugMessage){
					DEBUG((-1,"Smc Raid Var Name :: [%a]\n",pSetupRaidVar->SetupRaidVar.RaidVarName));
					DEBUG_PRINT_BUFFER(pSetupRaidVar->SetupRaidVar.RaidVarBuffer,pSetupRaidVar->SetupRaidVar.RaidVarSize);
				}
			}
			pItemsBody->SmcLsiVarId	= cVar->RaidVarId;
		}
	}

	Debug_for_SmcLsiVarTable(pProtocol);
	Debug_for_LsiVarTable(pProtocol);
	Debug_for_RaidItems(pProtocol);
	return EFI_SUCCESS;

}

EFI_STATUS	SmcLsiCreateSmcRaidVarAndItems(SMC_LSI_RAID_OOB_SETUP_PROTOCOL*	pProtocol){

	CHAR8					cVarName[NAME_LENGTH];
	UINT8*					TempBuffer 			= NULL;
	SMC_RAID_VAR*			cVar				= NULL;
	SMC_SETUP_RAID_VAR*		pSetupRaidVar		= NULL;
	UINT32*					pSmcRaidCmdOffset	= NULL;
	SMC_RAID_ITEMS_SET*		pItemsSet			= NULL;
	SMC_RAID_ITEMS_BODY*	TempItemBody		= NULL;
	UINT8					CmdNumber			= 0;

	if(!(!!pProtocol->SmcLsiCurrHiiHandleTable->RaidCardInfItems)) return SettingErrorStatus(pProtocol,0x65,EFI_SUCCESS);
	if(!(!!pProtocol->SmcLsiCurrHiiHandleTable->RaidSetupVarSet))  return SettingErrorStatus(pProtocol,0x67,EFI_SUCCESS);

	gBS->AllocatePool(EfiBootServicesData,pProtocol->SmcLsiGetSmcRaidVarSize(pProtocol),&TempBuffer);
	MemSet(TempBuffer,pProtocol->SmcLsiGetSmcRaidVarSize(pProtocol),0x00);

	// NAME_X_Y
	MemSet(cVarName,NAME_LENGTH * sizeof(CHAR8),0x00);
	Sprintf(cVarName,"%s_%x_%x",
			 pProtocol->SmcLsiGetSmcItemsVarName(pProtocol),
			 pProtocol->SmcLsiCurrHiiHandleTable->RaidCardType,
			 pProtocol->SmcLsiCurrHiiHandleTable->RaidCardIndex);

	cVar = AddLsiVarByBuffer2(
			pProtocol,
			cVarName,
			pProtocol->SmcLsiGetVarGuid(pProtocol),
			pProtocol->SmcLsiGetVIdStart(pProtocol),
			pProtocol->SmcLsiGetSmcRaidVarSize(pProtocol),
			TempBuffer
			);
	if(!(!!cVar)) return SettingErrorStatus(pProtocol,0x69,EFI_SUCCESS);
	SMC_RAID_DETAIL_DEBUG((-1,"  SMC RAID VAR Name[%a], Guid[%g], VarId[%x], VarSize[%x]\n",cVar->RaidVarName,cVar->RaidVarGuid,cVar->RaidVarId,cVar->RaidVarSize));

	pSetupRaidVar = GetPNextStartAddr(pProtocol->SmcLsiCurrHiiHandleTable->RaidSetupVarSet,STRUCT_OFFSET(SMC_SETUP_RAID_VAR,pSetupRaidVarNext));

	gBS->AllocatePool(EfiBootServicesData,sizeof(SMC_SETUP_RAID_VAR),&(pSetupRaidVar->pSetupRaidVarNext));
	MemSet(pSetupRaidVar->pSetupRaidVarNext,sizeof(SMC_SETUP_RAID_VAR),0x00);
	MemCpy(&pSetupRaidVar->pSetupRaidVarNext->SetupRaidVar,cVar,sizeof(SMC_RAID_VAR));


	for(pItemsSet = pProtocol->SmcLsiCurrHiiHandleTable->RaidCardInfItems;
	 	pItemsSet->pItemsNext != NULL;
		pItemsSet = pItemsSet->pItemsNext);

	pSmcRaidCmdOffset 	= pProtocol->SmcLsiGetSmcRaidCmdOffset(pProtocol);
	if(!(!!pSmcRaidCmdOffset)) return SettingErrorStatus(pProtocol,0x71,EFI_SUCCESS);

	gBS->AllocatePool(EfiBootServicesData,sizeof(SMC_RAID_ITEMS_SET),&pItemsSet->pItemsNext);
	MemSet(pItemsSet->pItemsNext,sizeof(SMC_RAID_ITEMS_SET),0x00);
	pItemsSet = pItemsSet->pItemsNext;

	pItemsSet->ItemsHeader.LsiRaidTypeIndex = pProtocol->SmcLsiCurrHiiHandleTable->RaidCardType;
	StrCpy(pItemsSet->ItemsHeader.LsiItemForm,SMC_RAID_CMD_FORM_STRING);
	StrCpy(pItemsSet->ItemsHeader.LsiItemName,SMC_RAID_CMD_FORM_STRING);
	pItemsSet->ItemsHeader.RaidItemsType	= RAID_SMCCMD_TYPE;
	pItemsSet->ItemsHeader.LsiItemId		= 0;

	TempItemBody = pItemsSet->ItemsBody		= NULL;	

	while((*pSmcRaidCmdOffset) != (UINT32)(-1) && (*pSmcRaidCmdOffset) < pProtocol->SmcLsiGetSmcRaidVarSize(pProtocol)){
		EFI_IFR_STRING*			SmcCmdString 	= NULL;
		SMC_RAID_ITEMS_BODY*	SmcCmdItemBody	= NULL;
		CHAR16*					CMDNAME			= NULL;

		gBS->AllocatePool(EfiBootServicesData,StrSize(SMC_RAID_CMD_NAME_STRING),&CMDNAME);
		Swprintf(CMDNAME,L"CMD%02d",++CmdNumber);

		gBS->AllocatePool(EfiBootServicesData,sizeof(EFI_IFR_STRING),&SmcCmdString);
		MemSet(SmcCmdString,sizeof(EFI_IFR_STRING),0x00);
		
		gBS->AllocatePool(EfiBootServicesData,sizeof(SMC_RAID_ITEMS_BODY),&SmcCmdItemBody);
		MemSet(SmcCmdItemBody,sizeof(SMC_RAID_ITEMS_BODY),0x00);
		
		SmcCmdItemBody->HdNum 				= 0;
		SmcCmdItemBody->SmcLsiVarId 		= cVar->RaidVarId;
		SmcCmdItemBody->pLsiItemOp			= (EFI_IFR_OP_HEADER*)SmcCmdString;
		SmcCmdItemBody->pItemsBodyNext		= NULL;
		SmcCmdItemBody->ItemsName			= CMDNAME;
		SmcCmdItemBody->ItemsHelp			= CMDNAME;

		SmcCmdString->Header.OpCode = EFI_IFR_STRING_OP;
		SmcCmdString->Header.Length = sizeof(EFI_IFR_STRING);
		SmcCmdString->Header.Scope	= 0;

		SmcCmdString->Question.Flags = 0;
		//Initial After.
		SmcCmdString->Question.Header.Prompt = 0;
		SmcCmdString->Question.Header.Help	 = 0;

		SmcCmdString->Question.QuestionId	 = pProtocol->SmcLsiGetQIdStart(pProtocol);
		SmcCmdString->Question.VarStoreId	 = cVar->RaidVarId;	
		SmcCmdString->Question.VarStoreInfo.VarOffset = (UINT16)(*pSmcRaidCmdOffset);

		SmcCmdString->MinSize	= 0;
		SmcCmdString->MaxSize	= SMC_ITEM_CMD_STRING_SIZE;
		SmcCmdString->Flags		= 0;

		if(!(!!TempItemBody)){
			TempItemBody = pItemsSet->ItemsBody = SmcCmdItemBody;
		}else{
			TempItemBody->pItemsBodyNext = SmcCmdItemBody;
			TempItemBody = TempItemBody->pItemsBodyNext;
		}
		
		StrCpy((CHAR16*)(&(cVar->RaidVarBuffer[*pSmcRaidCmdOffset])),L"{}");
		++pSmcRaidCmdOffset;
	}
	Debug_for_SmcLsiVarTable(pProtocol);
	Debug_for_LsiVarTable(pProtocol);
	Debug_for_RaidItems(pProtocol);

	return EFI_SUCCESS;
}

EFI_STATUS EFIAPI SmcLsiHookBrower2Callback(
  IN CONST  EFI_FORM_BROWSER2_PROTOCOL *This,
  IN OUT    UINTN                     *ResultsDataSize,
  IN OUT    EFI_STRING                ResultsData,
  IN CONST  BOOLEAN                   RetrieveData,
  IN CONST  EFI_GUID                  *VariableGuid, OPTIONAL
  IN CONST  CHAR16                    *VariableName OPTIONAL
){
	
	static CHAR8				aVariableName[NAME_LENGTH];

	SMC_RAID_VAR*		LsiVar 				= NULL;
	EFI_STRING			ConfigHdrTemp 		= NULL;
	EFI_STRING			ConfigRequestTemp	= NULL;
	EFI_STRING			ConfigBufferString	= NULL;
	EFI_STRING			mConfig 			= NULL;
	EFI_STRING			mProgress			= NULL;

	SMC_RAID_DETAIL_DEBUG((-1,"    SmcLsiHookBrower2Callback :: \n"));
	SMC_RAID_DETAIL_DEBUG((-1,"    VariableName[%s], VariableGuid[%g], RetrieveData[%x]\n",
				VariableName,*VariableGuid,RetrieveData));
	if(ResultsDataSize != NULL)
		SMC_RAID_DETAIL_DEBUG((-1,"    ResultsDataSize[%x]\n",*ResultsDataSize));
/*
	if(! (!!RetrieveData))
		DEBUG((-1,"From Driver Handle ResultsData = \n %s\n",ResultsData));
*/
	if(VariableGuid == NULL || VariableName == NULL) return EFI_INVALID_PARAMETER;

	if(ResultsDataSize == NULL || (*ResultsDataSize && ResultsData == NULL))
		return EFI_INVALID_PARAMETER;

	MemSet(aVariableName,NAME_LENGTH * sizeof(CHAR8),0x00);
	UnicodeStrToAsciiStr(VariableName,aVariableName);
	LsiVar = SearchLsiVarByName(aVariableName,VariableGuid);

	if(LsiVar == NULL) {
		DEBUG((-1,"Cannot find VarName[%a]\n",aVariableName));
		return EFI_NOT_FOUND;
	}
	SMC_RAID_DETAIL_DEBUG((-1,"    Lsi_Name[%a], Lsi_VarId[%x], Lsi_Guid[%g], Lsi_Size[%x]\n",
			   	LsiVar->RaidVarName,LsiVar->RaidVarId,LsiVar->RaidVarGuid,LsiVar->RaidVarSize));

/*
 * <ConfigResp> 	::= <ConfigHdr><ConfigBody>
 * <ConfigRequest> 	::= <ConfigHdr><RequestElement>
 *
 * <ConfigHdr> 		::= GUID=xxxx & NAME=xxxx & PATH=xxxx
 * <ConfigBody>		::= & OFFSET=xxxx & WIDTH=xxxx & VALUE=xxxx | ...
 * <RequestElement>	::= & OFFSET=xxxx & WIDTH=xxxx
*/
	//<ConfigHdr>
	ConfigHdrTemp = HiiConstructConfigHdr(&LsiVar->RaidVarGuid,VariableName,GetSmcRaidCurrDriverHandle());
	if(!(!!ConfigHdrTemp)) return EFI_OUT_OF_RESOURCES;

	if(RetrieveData == 0){

		gBS->AllocatePool(EfiBootServicesData,(StrLen(ConfigHdrTemp) + 1 + StrLen(ResultsData) + 2) * sizeof(CHAR16),&ConfigBufferString);
		if(!(!!ConfigBufferString)) return EFI_OUT_OF_RESOURCES;
		
		//<ConfigResp>
		StrCpy(ConfigBufferString,ConfigHdrTemp);
		StrCat(ConfigBufferString,L"&");
		StrCat(ConfigBufferString,ResultsData);

//		DEBUG((-1,"Set Data to Var Buffer = \n %s\n",ConfigBufferString));

		if(!(!!SetLsiVarBuffer_byString(LsiVar,ConfigBufferString))){
			DEBUG((-1,"	    SmcLsiHookBrower2Callback Get data from LSI to set into LSI VAR Buffer Error!\n"));
		}
		gBS->FreePool(ConfigHdrTemp);
		gBS->FreePool(ConfigBufferString);

	}else{
		EFI_STRING	mTempData		= NULL;
		EFI_STATUS	Status			= EFI_SUCCESS;
		UINTN		TempSize		= 0;

		if( !!(*ResultsDataSize) && !!StrStr(ResultsData,L"OFFSET=")){
			SMC_RAID_DETAIL_DEBUG((-1,"    Got ResultsData = %s\n",ResultsData));
		}

		//<ConfigRequest>
		ConfigRequestTemp = AppendOffsetWidth(ConfigHdrTemp,0,LsiVar->RaidVarSize);
		if(ConfigRequestTemp == NULL) return EFI_OUT_OF_RESOURCES;

		//<ConfigResp>
		Status = GetHiiConfigRoutingProtocol()->BlockToConfig(
							GetHiiConfigRoutingProtocol(),
							ConfigRequestTemp,
							LsiVar->RaidVarBuffer,
							LsiVar->RaidVarSize,
							&mConfig,
							&mProgress
						);
		SMC_RAID_DETAIL_DEBUG((-1,"    HookBrowserCallback BlockToConfig Status[%r]\n",Status));
		if(EFI_ERROR(Status)){
			DEBUG((-1,"	   SmcLsiHookBrower2Callback BlockToConfig Error! Status[%r]\n",Status));
//			DEBUG((-1,"    mProgress = %s\n",mProgress));
			return Status;
		}
		// <ConfigBody> without &
		mTempData = mConfig + (StrLen(ConfigHdrTemp) + 1);
		TempSize = (StrLen(mTempData) + 1) * sizeof(CHAR16);

//		SMC_RAID_DETAIL_DEBUG((-1,"    ConfigHdrTemp      %s\n",ConfigHdrTemp));
//		SMC_RAID_DETAIL_DEBUG((-1,"    ConfigRequestTemp  %s\n",ConfigRequestTemp));
//		SMC_RAID_DETAIL_DEBUG((-1,"    mConfig            %s\n",mConfig));
//		SMC_RAID_DETAIL_DEBUG((-1,"    mTempData          %s\n",mTempData));

//		DEBUG_PRINT_CONFIG(mTempData);

		SMC_RAID_DETAIL_DEBUG((-1,"    TempSize[%x]\n",TempSize));

		if(*ResultsDataSize < TempSize){
			*ResultsDataSize = TempSize;
			return EFI_BUFFER_TOO_SMALL;
		}

		StrCpy(ResultsData,mTempData);
//		SMC_RAID_DETAIL_DEBUG((-1,"    ResultsData        %s\n",ResultsData));
		gBS->FreePool(ConfigHdrTemp);
		gBS->FreePool(ConfigRequestTemp);
		gBS->FreePool(mConfig);
	}

	return EFI_SUCCESS;
}

EFI_STATUS	SmcLsiCallbackAccessMenu(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol){
	EFI_STATUS					Status 		= EFI_SUCCESS;
	UINTN						FormIndex	= 0;
	EFI_IFR_TYPE_VALUE			TypeValue;
	EFI_BROWSER_ACTION_REQUEST	ActionReq 	= 0;
	UINT8						IfrType		= 0;

	SMC_LSI_RAID_FORM_SET*			LocRefSearchTable = NULL;

	LocRefSearchTable = pProtocol->SmcLsiCurrHiiHandleTable->RaidCardAccessForms;

	for(;LocRefSearchTable != NULL;LocRefSearchTable = LocRefSearchTable->pFormNext){
		
		MemSet(&TypeValue,sizeof(EFI_IFR_TYPE_VALUE),0x00);

		switch(LocRefSearchTable->FormHeader.Lsi_OpCode){
			case EFI_IFR_REF_OP:
				IfrType	= EFI_IFR_TYPE_REF;
				break;
			default:
				break;
		}
		
		DEBUG((-1,"Request CallBack RaidType[%x], RaidCardIndex[%x], Name[%s], OpCode[%x], FormId[%x], QId[%x], VId[%x], Voff[%x], Flags[%x]\n",
					pProtocol->SmcLsiCurrHiiHandleTable->RaidCardType,
					pProtocol->SmcLsiCurrHiiHandleTable->RaidCardIndex,
					LocRefSearchTable->FormHeader.Lsi_FormName,
					LocRefSearchTable->FormHeader.Lsi_OpCode,
					LocRefSearchTable->FormBody.Lsi_FormId,
					LocRefSearchTable->FormBody.Lsi_QId,
					LocRefSearchTable->FormBody.Lsi_VId,
					LocRefSearchTable->FormBody.Lsi_Voff,
					LocRefSearchTable->FormBody.Lsi_Flags
				));

		Status = pProtocol->SmcLsiCurrHiiHandleTable->SmcLsiCurrConfigAccess->Callback(
					pProtocol->SmcLsiCurrHiiHandleTable->SmcLsiCurrConfigAccess,
					EFI_BROWSER_ACTION_CHANGING,
					LocRefSearchTable->FormBody.Lsi_QId,
					IfrType,
					&TypeValue,
					&ActionReq
				);
		DEBUG((-1,"Requesnt Callback Form Status[%r], IfrType[%x], TypeValue[%x], ActionReq[%x]\n",Status,IfrType,TypeValue,ActionReq));
		//gBS->Stall(100); //Hold for Driver.
	}
	return EFI_SUCCESS;	
}

EFI_STATUS	SmcLsiHookBrowser2Protocol(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol){

	static EFI_FORM_BROWSER2_PROTOCOL*	Browser2 					= NULL;
	static EFI_BROWSER_CALLBACK2		OriBrowserCallback			= NULL;
	static EFI_BROWSER_CALLBACK2		HookBrowserCallback			= SmcLsiHookBrower2Callback;

	EFI_STATUS	Status = EFI_SUCCESS;

	if(!(!!Browser2)){
		Status = gBS->LocateProtocol(&gEfiFormBrowser2ProtocolGuid,NULL,&Browser2);
		if(EFI_ERROR(Status)) {
			Browser2 = NULL;
			return SettingErrorStatus(pProtocol,0x01,Status);
		}
	}	

	OriBrowserCallback = Browser2->BrowserCallback;
	Browser2->BrowserCallback = HookBrowserCallback;
	HookBrowserCallback = OriBrowserCallback;

	return EFI_SUCCESS;
}

EFI_STATUS	SmcLsiRaidLib_DownLoad(SMC_LSI_RAID_OOB_SETUP_PROTOCOL*	pProtocol){

	EFI_STATUS	Status		= EFI_SUCCESS;
	UINTN		RaidIndex	= 0;
	
	DEBUG((-1, "\n... SmcLsiRaidLib_DownLoad ... START ...\n"));
	
	if(pProtocol == NULL) return EFI_LOAD_ERROR;
	mDetailedDebugMessage = pProtocol->DetailedDebugMessage;

	for(pProtocol->SmcLsiCurrHiiHandleTable = pProtocol->SmcLsiHiiHandleTable;
		pProtocol->SmcLsiCurrHiiHandleTable != NULL && pProtocol->SmcLsiCurrHiiHandleTable->RaidCardType != RAID_NULL;
		pProtocol->SmcLsiCurrHiiHandleTable = pProtocol->SmcLsiCurrHiiHandleTable->pNext){
		
		DEBUG((-1,"    SmcLsiCurrHiiHandleTable RaidCardType[%x], RaidCardIndex[%x], RaidCardHiiHandle[%08x]\n",
				   pProtocol->SmcLsiCurrHiiHandleTable->RaidCardType,
				   pProtocol->SmcLsiCurrHiiHandleTable->RaidCardIndex,
				   (UINT32)pProtocol->SmcLsiCurrHiiHandleTable->RaidCardHiiHandle
			));

		for(RaidIndex=0;mSmcLsiInsideDownFunc[RaidIndex].Func != NULL;RaidIndex++){
			THROUGH_DEBUG_MESSAGE(pProtocol,mSmcLsiInsideDownFunc[RaidIndex].FuncName,SMC_LSI_THROUGH_FUNC_EXEC);
			Status = mSmcLsiInsideDownFunc[RaidIndex].Func(pProtocol);
			Status = THROUGH_DEBUG_MESSAGE(pProtocol,mSmcLsiInsideDownFunc[RaidIndex].FuncName,Status);
			if(EFI_ERROR(Status)) break;
		}
		if(EFI_ERROR(Status)) break;

		for(RaidIndex=0;pProtocol->SmcLsiAfterDownFuncTable[RaidIndex].Func != NULL;RaidIndex++){
			THROUGH_DEBUG_MESSAGE(pProtocol,pProtocol->SmcLsiAfterDownFuncTable[RaidIndex].FuncName,SMC_LSI_THROUGH_FUNC_EXEC);
			Status = pProtocol->SmcLsiAfterDownFuncTable[RaidIndex].Func(pProtocol);
			Status = THROUGH_DEBUG_MESSAGE(pProtocol,pProtocol->SmcLsiAfterDownFuncTable[RaidIndex].FuncName,Status);
			if(EFI_ERROR(Status)) break;
		}
		if(EFI_ERROR(Status)) break;

		THROUGH_DEBUG_MESSAGE(pProtocol,L"RaidDownTheLastCleanFunc",SMC_LSI_THROUGH_FUNC_EXEC);
		Status = RaidDownTheLastCleanFunc(pProtocol);
		THROUGH_DEBUG_MESSAGE(pProtocol,L"RaidDownTheLastCleanFunc",Status);
	}
	DEBUG((-1, "... SmcLsiRaidLib_DownLoad ... END ...\n"));
	return Status;
}

//===========================================================

SMC_LSI_ITEMS_MEM* CopyAndExtMem(SMC_LSI_ITEMS_MEM*	pItems){
	
	if(!(!!pItems)){
		SMC_LSI_ITEMS_MEM*	pTemp	= NULL;
		gBS->AllocatePool(EfiBootServicesData,sizeof(SMC_LSI_ITEMS_MEM),&pTemp);
		
		pTemp->ItemsBufferAlSize 	= SMC_LSI_ADD_ITEMS_MEM_SIZE;
		pTemp->ItemsBufferCurSize	= 0;
		gBS->AllocatePool(EfiBootServicesData,pTemp->ItemsBufferAlSize,&pTemp->ItemsBuffer);
		MemSet(pTemp->ItemsBuffer,pTemp->ItemsBufferAlSize,0x00);

		pTemp->ItemsCopyBufferSize		= 0;
		pTemp->ItemsCopyBuffer			= NULL;
		
		return pTemp;
	}
	
	if(pItems->ItemsBufferCurSize + pItems->ItemsCopyBufferSize >= pItems->ItemsBufferAlSize && 
	   pItems->ItemsBufferCurSize + pItems->ItemsCopyBufferSize <  (pItems->ItemsBufferAlSize + SMC_LSI_ADD_ITEMS_MEM_SIZE)){
		UINT8*		NewBuffer = NULL;

		pItems->ItemsBufferAlSize = pItems->ItemsBufferAlSize + SMC_LSI_ADD_ITEMS_MEM_SIZE;
		gBS->AllocatePool(EfiBootServicesData,pItems->ItemsBufferAlSize,&NewBuffer);
		MemSet(NewBuffer,pItems->ItemsBufferAlSize,0x00);

		MemCpy(NewBuffer,pItems->ItemsBuffer,pItems->ItemsBufferCurSize);
		gBS->FreePool(pItems->ItemsBuffer);
		pItems->ItemsBuffer = NewBuffer;
	}

	MemCpy(pItems->ItemsBuffer + pItems->ItemsBufferCurSize,pItems->ItemsCopyBuffer,pItems->ItemsCopyBufferSize);
	pItems->ItemsBufferCurSize += pItems->ItemsCopyBufferSize;

	return 	pItems;
}


EFI_IFR_OP_HEADER*	SearchOpCodeInFormData(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol, UINT16 OpCode, UINT16 Label OPTIONAL){

	EFI_STATUS					 Status				= EFI_SUCCESS;
	UINT8*						 SetupData			= NULL;
	BOOLEAN						 SearchFind			= FALSE;
	
	EFI_HII_PACKAGE_HEADER 		*SmcLsiSetupIfrPackagePtr 		= NULL;
  	EFI_IFR_OP_HEADER 			*pEFI_IFR_OP_HEADER 			= NULL;
	
	if(pProtocol->SmcLsiCurrRAIDSetupData == NULL){ 
		SettingErrorStatus(pProtocol,0x01,EFI_LOAD_ERROR);
		return NULL;
	}

	SmcLsiSetupIfrPackagePtr = (EFI_HII_PACKAGE_HEADER*)pProtocol->SmcLsiCurrRAIDSetupData;

	for(pEFI_IFR_OP_HEADER = (EFI_IFR_OP_HEADER*)(SmcLsiSetupIfrPackagePtr+1);
		((UINT32)pEFI_IFR_OP_HEADER) < (UINT32)((UINT8*)SmcLsiSetupIfrPackagePtr + SmcLsiSetupIfrPackagePtr->Length);
		pEFI_IFR_OP_HEADER = (EFI_IFR_OP_HEADER*)((UINT8*)pEFI_IFR_OP_HEADER + pEFI_IFR_OP_HEADER->Length)){

		if(pEFI_IFR_OP_HEADER->OpCode == OpCode){
			switch(pEFI_IFR_OP_HEADER->OpCode){
				case EFI_IFR_GUID_OP:
					{
						EFI_GUID		LabelGuid	= EFI_IFR_TIANO_GUID; 
						EFI_IFR_GUID* 	GuidOp 		= (EFI_IFR_GUID*)pEFI_IFR_OP_HEADER;

						if(MemCmp(&GuidOp->Guid,&LabelGuid,sizeof(EFI_GUID)) == 0){
							EFI_IFR_GUID_LABEL*	pIfrGuidLabel = (EFI_IFR_GUID_LABEL*)GuidOp;
							if(pIfrGuidLabel->ExtendOpCode == EFI_IFR_EXTEND_OP_LABEL && pIfrGuidLabel->Number == Label){
								SearchFind = TRUE;
							}
						}
					}	
					break;
				case EFI_IFR_FORM_OP:
				{
					EFI_IFR_FORM*	pForm = NULL;
					pForm = (EFI_IFR_FORM*)pEFI_IFR_OP_HEADER;

					if(pForm->FormId == Label)
						SearchFind = TRUE;
				}
					break;
				default:
					SearchFind = TRUE;
					break;
			}
		}
		if(!!SearchFind)
			break;
	}
 	
	return (!!SearchFind) ? pEFI_IFR_OP_HEADER : NULL;

}

#define MACRO_CAULBUFFER_ARGU(_x)\
	UINT8*	UpperBuffer	 = NULL;\
	UINT8*	ButtonBuffer = NULL;\
	UINT8*	PackageStart = NULL;\
	UINT8*	InsertStart	 = NULL;\
	UINT8*	PackageEnd	 = NULL;\
	UINT8*	TempCopyAddr = NULL;\
	UINT32	PtILeng	= 0;\
	UINT32	ItELeng = 0

#define MACRO_CAULBUFFER_INITIAL(_x)\
	do {\
		PackageStart 		= pProtocol->SmcLsiCurrRAIDSetupData;\
		PackageEnd 			= (PackageStart + ((EFI_HII_PACKAGE_HEADER*)PackageStart)->Length);\
		\
		PtILeng				= (UINT32)((UINT32)InsertStart - (UINT32)PackageStart);\
		ItELeng				= (UINT32)((UINT32)PackageEnd  - (UINT32)InsertStart);\
		\
		gBS->AllocatePool(EfiBootServicesData,PtILeng,&UpperBuffer);\
		MemCpy(UpperBuffer,PackageStart,PtILeng);\
		\
		gBS->AllocatePool(EfiBootServicesData,ItELeng,&ButtonBuffer);\
		MemCpy(ButtonBuffer,InsertStart,ItELeng);\
		\
	}while(FALSE)

#define	MACRO_CAULBUFFER_REPLACE(pItems_x)\
	do {\
		TempCopyAddr = pProtocol->SmcLsiCurrRAIDSetupData;\
		MemSet(TempCopyAddr,MAX_HPK_LIST_SIZE,0x00);\
		\
		MemCpy(TempCopyAddr,UpperBuffer,PtILeng);\
		TempCopyAddr = (UINT8*)(TempCopyAddr + PtILeng);\
		\
		MemCpy(TempCopyAddr,pItems_x->ItemsBuffer,pItems_x->ItemsBufferCurSize);\
		TempCopyAddr = (UINT8*)(TempCopyAddr + pItems_x->ItemsBufferCurSize);\
		\
		MemCpy(TempCopyAddr,ButtonBuffer,ItELeng);\
		\
		((EFI_HII_PACKAGE_HEADER*)PackageStart)->Length += (pItems_x->ItemsBufferCurSize & 0x00FFFFFF);\
		pProtocol->SmcLsiCurrRAIDSetupDataAddedSize += pItems_x->ItemsBufferCurSize;\
		\
		gBS->FreePool(pItems_x->ItemsBuffer);\
		gBS->FreePool(pItems_x);\
		gBS->FreePool(UpperBuffer);\
		gBS->FreePool(ButtonBuffer);\
	}while(FALSE)

#define MARCO_CTE_BUFFER(pItems_x,tHiiItem)\
	do{\
		pItems_x->ItemsCopyBuffer 		= (UINT8*)tHiiItem;\
		pItems_x->ItemsCopyBufferSize	= ((EFI_IFR_OP_HEADER*)tHiiItem)->Length;\
		pItems_x = CopyAndExtMem(pItems_x);\
	}while(FALSE)

VOID Cte_Buffer(SMC_LSI_ITEMS_MEM* pItems_x, VOID* tHiiItem){
	do{
		pItems_x->ItemsCopyBuffer 		= (UINT8*)tHiiItem;
		pItems_x->ItemsCopyBufferSize	= ((EFI_IFR_OP_HEADER*)tHiiItem)->Length;
		pItems_x = CopyAndExtMem(pItems_x);
	}while(FALSE);
}

EFI_IFR_GUID_LABEL*	CreateGuidLabel(UINT16 LabelId){
	static EFI_IFR_GUID_LABEL* GuidLabel = NULL;
	EFI_GUID			LabelGuid = EFI_IFR_TIANO_GUID;

	if(!(!!GuidLabel))
		gBS->AllocatePool(EfiBootServicesData,sizeof(EFI_IFR_GUID_LABEL),&GuidLabel);

	MemSet(GuidLabel,sizeof(EFI_IFR_GUID_LABEL),0x00);

	GuidLabel->Header.OpCode = EFI_IFR_GUID_OP;
	GuidLabel->Header.Length = sizeof(EFI_IFR_GUID_LABEL);
	GuidLabel->Header.Scope  = 0;
	MemCpy(&GuidLabel->Guid,&LabelGuid,sizeof(EFI_GUID));
	GuidLabel->ExtendOpCode 		= EFI_IFR_EXTEND_OP_LABEL;
	GuidLabel->Number		  		= LabelId;

	return GuidLabel;
}

EFI_IFR_SUBTITLE*	CreateSubTitle(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol, CHAR16*	SubTitleString){
	static EFI_IFR_SUBTITLE*	CopySubtitle = NULL;

	if(!(!!CopySubtitle))
		gBS->AllocatePool(EfiBootServicesData,sizeof(EFI_IFR_SUBTITLE),&CopySubtitle);

	MemSet(CopySubtitle,sizeof(EFI_IFR_SUBTITLE),0x00);

	CopySubtitle->Header.OpCode = EFI_IFR_SUBTITLE_OP;
	CopySubtitle->Header.Length = sizeof(EFI_IFR_SUBTITLE);
	CopySubtitle->Header.Scope  = 0;

	CopySubtitle->Statement.Prompt 	= NewHiiString(pProtocol->SmcLsiGetHiiHandle(pProtocol),SubTitleString);
	CopySubtitle->Statement.Help 	= CopySubtitle->Statement.Prompt;
	CopySubtitle->Flags				= EFI_IFR_FLAGS_HORIZONTAL;

	return CopySubtitle;
}

EFI_IFR_ONE_OF* CopyOneOfItems(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol, SMC_RAID_ITEMS_SET* RaidItemsTable, VOID* pOpHeader, UINT16 VarId){
	
	static EFI_IFR_ONE_OF*	CopyOneOf 	= NULL;
	EFI_IFR_ONE_OF*			OriOneOf	= (EFI_IFR_ONE_OF*)pOpHeader;

	if(!(!!CopyOneOf))
		gBS->AllocatePool(EfiBootServicesData,sizeof(EFI_IFR_ONE_OF),&CopyOneOf);

	MemSet(CopyOneOf,sizeof(EFI_IFR_ONE_OF),0x00);

	MemCpy(&CopyOneOf->Header,&OriOneOf->Header,sizeof(EFI_IFR_OP_HEADER));
	CopyOneOf->Question.Header.Prompt 			= NewHiiString(pProtocol->SmcLsiGetHiiHandle(pProtocol),RaidItemsTable->ItemsHeader.LsiItemName);
	CopyOneOf->Question.Header.Help   			= CopyOneOf->Question.Header.Prompt;
	CopyOneOf->Question.QuestionId 				= pProtocol->SmcLsiGetQIdStart(pProtocol);
	CopyOneOf->Question.VarStoreId	 			= VarId;
	CopyOneOf->Question.VarStoreInfo.VarOffset	= OriOneOf->Question.VarStoreInfo.VarOffset;
	CopyOneOf->Question.Flags		 			= OriOneOf->Question.Flags;
	CopyOneOf->Flags					 		= OriOneOf->Flags;

	MemCpy(&CopyOneOf->data,&OriOneOf->data,sizeof(MINMAXSTEP_DATA));
	switch((CopyOneOf->Flags & EFI_IFR_NUMERIC_SIZE)){
		case EFI_IFR_NUMERIC_SIZE_1:
			CopyOneOf->data.u8.Step = 1;
			break;
		case EFI_IFR_NUMERIC_SIZE_2:
			CopyOneOf->data.u16.Step = 1;
			break;
		case EFI_IFR_NUMERIC_SIZE_4:
			CopyOneOf->data.u32.Step = 1;
			break;
		case EFI_IFR_NUMERIC_SIZE_8:
			CopyOneOf->data.u64.Step = 1;
			break;
	}
	return CopyOneOf;
}



EFI_IFR_END* CreateEndItem(){
	static EFI_IFR_END	EndOp = {{EFI_IFR_END_OP,sizeof(EFI_IFR_END),0}};

	return &EndOp;
}

EFI_IFR_ONE_OF_OPTION*	CopyOneOfOptionItems(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol, SMC_RAID_ITEMS_SET* RaidItemsTable, VOID* pOpHeader, UINT8 defaultFlag){

	static EFI_IFR_ONE_OF_OPTION*	CopyOneOfOption 	= NULL;
	EFI_IFR_ONE_OF_OPTION*			OriOneOfOption		= (EFI_IFR_ONE_OF_OPTION*)pOpHeader;
	CHAR16*							OptionString 		= NULL;
	if(!(!!CopyOneOfOption))
		gBS->AllocatePool(EfiBootServicesData,sizeof(EFI_IFR_ONE_OF_OPTION),&CopyOneOfOption);

	MemSet(CopyOneOfOption,sizeof(EFI_IFR_ONE_OF_OPTION),0x00);

	MemCpy(&CopyOneOfOption->Header,&OriOneOfOption->Header,sizeof(EFI_IFR_OP_HEADER));
	OptionString = 	GetHiiString(pProtocol->SmcLsiCurrHiiHandleTable->RaidCardHiiHandle,OriOneOfOption->Option);
	CopyOneOfOption->Option	= NewHiiString(pProtocol->SmcLsiGetHiiHandle(pProtocol),OptionString);

	CopyOneOfOption->Flags = (OriOneOfOption->Flags | defaultFlag);
	CopyOneOfOption->Type  = OriOneOfOption->Type;
	MemCpy(&CopyOneOfOption->Value,&OriOneOfOption->Value,sizeof(EFI_IFR_TYPE_VALUE));
	
	return CopyOneOfOption;
}

VOID RecurseOneOfOption(
	SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol, 
	SMC_LSI_ITEMS_MEM* ItemsBuffer, 
	SMC_RAID_ITEMS_SET* RaidItemsTable, 
	VOID* pOpHeader, 
	UINT8 defaultFlag, 
	UINT8 OneOfScope){

	if(!(!!OneOfScope)) return ;
	do {
		EFI_IFR_OP_HEADER*	OpHeader = pOpHeader;
		OpHeader =  (EFI_IFR_OP_HEADER*)((UINT8*)OpHeader + OpHeader->Length);
		switch(OpHeader->OpCode){
			case EFI_IFR_ONE_OF_OPTION_OP:
			{
				Cte_Buffer(ItemsBuffer,CopyOneOfOptionItems(pProtocol,RaidItemsTable,OpHeader,defaultFlag));
				defaultFlag = 0;
			}
				break;
			case EFI_IFR_END_OP:
				--OneOfScope;
				break;
			default:	//Temporary, we ignore other OpCode.
				if(!!OpHeader->Scope) ++OneOfScope;
				break;
		}
		RecurseOneOfOption(pProtocol,ItemsBuffer,RaidItemsTable,OpHeader,defaultFlag,OneOfScope);
	}while(FALSE);
}

EFI_STATUS InsertRaidSetupSmcCmdsAndItems(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol){

	MACRO_CAULBUFFER_ARGU(PACKAGE_BUFFER);	
	EFI_IFR_GUID* 			GuidOp 					= NULL;
	SMC_LSI_ITEMS_MEM*		SmcCmdsItemsBuffer		= NULL;
	SMC_RAID_ITEMS_SET*		RaidItemsTable			= NULL;

	if(pProtocol->SmcLsiCurrHiiHandleTable->SmcFormId 		== 0x0 ||
	   pProtocol->SmcLsiCurrHiiHandleTable->SmcFormTitleId 	== 0x0){
		return SettingErrorStatus(pProtocol,0x01,EFI_NOT_FOUND);
	}

	GuidOp = (EFI_IFR_GUID*)SearchOpCodeInFormData(pProtocol,EFI_IFR_GUID_OP,pProtocol->SmcLsiCurrHiiHandleTable->BuildCfgLabel);
	if(GuidOp == NULL) return SettingErrorStatus(pProtocol,0x12,EFI_NOT_FOUND);

	InsertStart	= (UINT8*)((UINT8*)GuidOp + GuidOp->Header.Length);
	RaidItemsTable		= pProtocol->SmcLsiCurrHiiHandleTable->RaidCardInfItems;;
	MACRO_CAULBUFFER_INITIAL(PACKAGE_BUFFER);

	SmcCmdsItemsBuffer = CopyAndExtMem(NULL);
	Cte_Buffer(SmcCmdsItemsBuffer,CreateSubTitle(pProtocol,L" ==== RAID Cfg Commands ==== "));

	for(;RaidItemsTable != NULL;RaidItemsTable = RaidItemsTable->pItemsNext){
		SMC_RAID_ITEMS_BODY*	pItemsBody = NULL;
//		if(!!StrCmp(RaidItemsTable->ItemsHeader.LsiItemForm,SMC_RAID_CMD_FORM_STRING)) continue;
		if(RaidItemsTable->ItemsHeader.RaidItemsType != RAID_SMCCMD_TYPE) continue;

		for(pItemsBody = RaidItemsTable->ItemsBody;pItemsBody != NULL; pItemsBody = pItemsBody->pItemsBodyNext){
			SMC_LSI_ITEMS_COMMON_HEADER*	OpCmnHeader = NULL;
			OpCmnHeader = (SMC_LSI_ITEMS_COMMON_HEADER*)pItemsBody->pLsiItemOp;

			OpCmnHeader->Question.Header.Prompt = NewHiiString(pProtocol->SmcLsiGetHiiHandle(pProtocol),pItemsBody->ItemsName);
			OpCmnHeader->Question.Header.Help	= OpCmnHeader->Question.Header.Prompt;
			
			Cte_Buffer(SmcCmdsItemsBuffer,OpCmnHeader);
		}
	}

	MACRO_CAULBUFFER_REPLACE(SmcCmdsItemsBuffer);
	return EFI_SUCCESS;
}

EFI_STATUS CreateRaid_Hdg_Rdg_Jbod_TypeItems(
		SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol,
		SMC_RAID_ITEMS_SET* pRaidItems,
		SMC_LSI_ITEMS_MEM*	pItemsBuffer,
		SMC_RAID_ITEMS_TYPE ItemsType,
		CHAR16*				SubName){

	EFI_STATUS	Status = EFI_SUCCESS;

	for(;!!pRaidItems; pRaidItems = pRaidItems->pItemsNext){
		SMC_RAID_ITEMS_BODY*	pItemsBody = NULL;
		CHAR16					SubNameBuf[NAME_LENGTH];

		if(pRaidItems->ItemsHeader.RaidItemsType != ItemsType) continue;
	
		MemSet(SubNameBuf,NAME_LENGTH * sizeof(CHAR16),0x00);
		Swprintf(SubNameBuf,L" ---- %s Groups :: ",SubName);
		Cte_Buffer(pItemsBuffer,CreateSubTitle(pProtocol,SubNameBuf));

		for(pItemsBody = pRaidItems->ItemsBody;!!pItemsBody; pItemsBody = pItemsBody->pItemsBodyNext){
			EFI_IFR_OP_HEADER*	OpHeader = NULL;

			OpHeader = pItemsBody->pLsiItemOp;
			switch(OpHeader->OpCode){
				case EFI_IFR_CHECKBOX_OP:
				case EFI_IFR_REF_OP:
				{
					if(ItemsType == RAID_HDG_TYPE || ItemsType == RAID_RDG_TYPE || ItemsType == RAID_JBOD_TYPE){
						SMC_LSI_ITEMS_COMMON_HEADER*	OriOpCmnH = NULL;
						EFI_IFR_TEXT					CopyText;
						CHAR16*							HddString = NULL;
						CHAR16							HddString2[NAME_LENGTH];

						OriOpCmnH = (SMC_LSI_ITEMS_COMMON_HEADER*)OpHeader;
						HddString = GetHiiString(pProtocol->SmcLsiCurrHiiHandleTable->RaidCardHiiHandle,OriOpCmnH->Question.Header.Prompt);
						CopyText.Header.OpCode = EFI_IFR_TEXT_OP;
						CopyText.Header.Length = sizeof(EFI_IFR_TEXT);
						CopyText.Header.Scope  = 0;

						MemSet(HddString2,NAME_LENGTH * sizeof(CHAR16),0x00);
						Swprintf(HddString2,L"Num[%d] :: %s",pItemsBody->HdNum,HddString);
						CopyText.Statement.Prompt = NewHiiString(pProtocol->SmcLsiGetHiiHandle(pProtocol),HddString2);

						CopyText.Statement.Help	  = NewHiiString(pProtocol->SmcLsiGetHiiHandle(pProtocol),L"");
						CopyText.TextTwo		  =	CopyText.Statement.Help;

						Cte_Buffer(pItemsBuffer,&CopyText);
					}
				}
					break;
				default:
					break;
			}
		}
	}

	return Status;
}


EFI_STATUS InsertRaidSetupHDGItems(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol){

	MACRO_CAULBUFFER_ARGU(PACKAGE_BUFFER);
	
	EFI_IFR_GUID* 			GuidOp 					= NULL;
	SMC_LSI_ITEMS_MEM*		HDGItemsBuffer			= NULL;
	SMC_RAID_ITEMS_SET*		RaidItemsTable			= NULL;
	EFI_IFR_GUID_LABEL*		GuidLabel				= NULL;

	SMC_RAID_ITEMS_TYPE		BuildOrder[] 		= {RAID_HDG_TYPE, RAID_JBOD_TYPE, RAID_RDG_TYPE, RAID_NULL_TYPE};
	CHAR16*					BuildOrderSub[]		= {L"HDD Drives", L"JBOD Drives", L"RAID Drives", NULL };
	UINT8					BuildOrdIndex	= 0;

	if(pProtocol->SmcLsiCurrHiiHandleTable->SmcFormId 		== 0x0 ||
	   pProtocol->SmcLsiCurrHiiHandleTable->SmcFormTitleId 	== 0x0){
		return SettingErrorStatus(pProtocol,0x01,EFI_NOT_FOUND);
	}

	GuidOp = (EFI_IFR_GUID*)SearchOpCodeInFormData(pProtocol,EFI_IFR_GUID_OP,pProtocol->SmcLsiCurrHiiHandleTable->HDGLabel);
	if(GuidOp == NULL) return SettingErrorStatus(pProtocol,0x12,EFI_NOT_FOUND);

	InsertStart	= (UINT8*)((UINT8*)GuidOp + GuidOp->Header.Length);
	MACRO_CAULBUFFER_INITIAL(PACKAGE_BUFFER);

	HDGItemsBuffer = CopyAndExtMem(NULL);

	Cte_Buffer(HDGItemsBuffer,CreateSubTitle(pProtocol,L" ==== RAID CARD Hard Drives Information ==== "));

	BuildOrdIndex = 0;
	while(BuildOrder[BuildOrdIndex] != RAID_NULL_TYPE){
		RaidItemsTable		= pProtocol->SmcLsiCurrHiiHandleTable->RaidCardInfItems;;
		SMC_RAID_DETAIL_DEBUG((-1,"Create Hard Drive Type[%x]\n",BuildOrder[BuildOrdIndex]));
		if(EFI_ERROR(CreateRaid_Hdg_Rdg_Jbod_TypeItems(pProtocol,RaidItemsTable,HDGItemsBuffer,BuildOrder[BuildOrdIndex],BuildOrderSub[BuildOrdIndex++])))
			return SettingErrorStatus(pProtocol,0x33,EFI_NOT_FOUND);

	}

	GuidLabel = CreateGuidLabel(pProtocol->SmcLsiGetOLabelStart(pProtocol));
	pProtocol->SmcLsiCurrHiiHandleTable->BuildCfgLabel = pProtocol->SmcLsiGetOLabelNow(pProtocol);
	Cte_Buffer(HDGItemsBuffer,GuidLabel);

	//Combine Upper + VarBuffer + button
	MACRO_CAULBUFFER_REPLACE(HDGItemsBuffer);
	return EFI_SUCCESS;
}

EFI_STATUS InsertRaidSetupChangeItems(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol){

	MACRO_CAULBUFFER_ARGU(PACKAGE_BUFFER);
	
	EFI_IFR_GUID* 			GuidOp 					= NULL;
	SMC_LSI_ITEMS_MEM*		ChangeItemsBuffer		= NULL;
	EFI_IFR_GUID_LABEL*		GuidLabel				= NULL;
	SMC_RAID_ITEMS_SET*		RaidItemsTable			= NULL;

	if(pProtocol->SmcLsiCurrHiiHandleTable->SmcFormId 		== 0x0 ||
	   pProtocol->SmcLsiCurrHiiHandleTable->SmcFormTitleId 	== 0x0){
		return SettingErrorStatus(pProtocol,0x01,EFI_NOT_FOUND);
	}

	GuidOp = (EFI_IFR_GUID*)SearchOpCodeInFormData(pProtocol,EFI_IFR_GUID_OP,pProtocol->SmcLsiCurrHiiHandleTable->ChangeItemsLabel);
	if(GuidOp == NULL) return SettingErrorStatus(pProtocol,0x12,EFI_NOT_FOUND);

	InsertStart	= (UINT8*)((UINT8*)GuidOp + GuidOp->Header.Length);
	MACRO_CAULBUFFER_INITIAL(PACKAGE_BUFFER);

	RaidItemsTable		= pProtocol->SmcLsiCurrHiiHandleTable->RaidCardInfItems;;
	ChangeItemsBuffer = CopyAndExtMem(NULL);

	Cte_Buffer(ChangeItemsBuffer,CreateSubTitle(pProtocol,L" ==== RAID CARD Changedable Items ==== "));

	/* This part should be insert changedable Items in here. */

	for(;RaidItemsTable != NULL; RaidItemsTable = RaidItemsTable->pItemsNext){
	
		SMC_RAID_ITEMS_BODY*	pItemsBody = NULL;

//		if(!(!!StrCmp(RaidItemsTable->ItemsHeader.LsiItemName,pProtocol->SmcLsiGetHdgName(pProtocol)))) continue;
		if(RaidItemsTable->ItemsHeader.RaidItemsType != RAID_CHANGEABLE_TYPE) continue;

		for(pItemsBody = RaidItemsTable->ItemsBody;pItemsBody != NULL; pItemsBody = pItemsBody->pItemsBodyNext){
			
			EFI_IFR_OP_HEADER*	OpHeader = NULL;
			OpHeader = pItemsBody->pLsiItemOp;
			
			switch(OpHeader->OpCode){
				case EFI_IFR_ONE_OF_OP:
				{	
					EFI_IFR_ONE_OF*		OriOneOf 		= (EFI_IFR_ONE_OF*)OpHeader;
					UINT8				DefaultFlags	= EFI_IFR_OPTION_DEFAULT;

					//First step Create ONE_OF
					Cte_Buffer(ChangeItemsBuffer,CopyOneOfItems(pProtocol,RaidItemsTable,OpHeader,pItemsBody->SmcLsiVarId));
					RecurseOneOfOption(pProtocol,ChangeItemsBuffer,RaidItemsTable,OpHeader,DefaultFlags,OpHeader->Scope);
					Cte_Buffer(ChangeItemsBuffer,CreateEndItem());
				}
					break;
				default:
					break;
			}
		}
	}

	/* This part should be insert changedable Items in here. */

	GuidLabel = CreateGuidLabel(pProtocol->SmcLsiGetOLabelStart(pProtocol));
	pProtocol->SmcLsiCurrHiiHandleTable->HDGLabel = pProtocol->SmcLsiGetOLabelNow(pProtocol);
	Cte_Buffer(ChangeItemsBuffer,GuidLabel);

	//Combine Upper + VarBuffer + button
	MACRO_CAULBUFFER_REPLACE(ChangeItemsBuffer);
	return EFI_SUCCESS;
}

EFI_STATUS InsertRaidSetupFormItems(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol){

	MACRO_CAULBUFFER_ARGU(PACKAGE_BUFFER);

	UINTN				ItemIndex			= 0;

	EFI_IFR_OP_HEADER*		pFormOp				= NULL;
	SMC_LSI_ITEMS_MEM*		FormItemsBuffer		= NULL;
	SMC_RAID_ITEMS_SET*		RaidItemsTable		= NULL;
	EFI_HII_HANDLE			SmcLsiSetupHandle	= NULL;

	EFI_IFR_FORM	FormOp;
	EFI_IFR_END		EndOp;

	UINT8				HddCount = 0;

	pFormOp = (EFI_IFR_OP_HEADER*)SearchOpCodeInFormData(pProtocol,EFI_IFR_FORM_OP,pProtocol->SmcLsiGetFormLabel(pProtocol));	
	if(pFormOp == NULL) return SettingErrorStatus(pProtocol,0x02,EFI_LOAD_ERROR);
	
	//FormOp --- EndOp
	InsertStart 		= (UINT8*)pFormOp;
	MACRO_CAULBUFFER_INITIAL(PACKAGE_BUFFER);

	SmcLsiSetupHandle 	= pProtocol->SmcLsiGetHiiHandle(pProtocol);
	RaidItemsTable		= pProtocol->SmcLsiCurrHiiHandleTable->RaidCardInfItems;;
	FormItemsBuffer		= CopyAndExtMem(NULL);

	if(pProtocol->SmcLsiCurrHiiHandleTable->SmcFormId 		== 0x0 ||
	   pProtocol->SmcLsiCurrHiiHandleTable->SmcFormTitleId 	== 0x0){

		DEBUG((-1," RaidType[%x], RaidCardIndex[%x] doesn't create Form Goto Ref\n",
				   pProtocol->SmcLsiCurrHiiHandleTable->RaidCardType, pProtocol->SmcLsiCurrHiiHandleTable->RaidCardIndex));
		return SettingErrorStatus(pProtocol,0x01,EFI_LOAD_ERROR);
	}

	FormOp.Header.OpCode = EFI_IFR_FORM_OP;
	FormOp.Header.Length = sizeof(EFI_IFR_FORM);
	FormOp.Header.Scope	 = 1;
	FormOp.FormTitle = pProtocol->SmcLsiCurrHiiHandleTable->SmcFormTitleId;
	FormOp.FormId	 = pProtocol->SmcLsiCurrHiiHandleTable->SmcFormId;

	Cte_Buffer(FormItemsBuffer,&FormOp);
	
	Cte_Buffer(FormItemsBuffer,CreateSubTitle(pProtocol,L" ==== RAID CARD Informations ==== "));

	for(;RaidItemsTable != NULL; RaidItemsTable = RaidItemsTable->pItemsNext){
	
		SMC_RAID_ITEMS_BODY*	pItemsBody = NULL;
//		if(!(!!StrCmp(RaidItemsTable->ItemsHeader.LsiItemName,pProtocol->SmcLsiGetHdgName(pProtocol)))) continue;
		if(RaidItemsTable->ItemsHeader.RaidItemsType != RAID_INFORMATION_TYPE) continue;

		for(pItemsBody = RaidItemsTable->ItemsBody;pItemsBody != NULL; pItemsBody = pItemsBody->pItemsBodyNext){
			
			EFI_IFR_OP_HEADER*	OpHeader = NULL;
			OpHeader = pItemsBody->pLsiItemOp;

			switch(OpHeader->OpCode){
				case EFI_IFR_STRING_OP:
				{
					EFI_IFR_STRING		CopyString;
					EFI_IFR_STRING		*OriString;

					OriString = (EFI_IFR_STRING*)OpHeader;
					CopyString.Header.OpCode = EFI_IFR_STRING_OP;
					CopyString.Header.Length = sizeof(EFI_IFR_STRING);
					CopyString.Header.Scope  = 0;
					
					CopyString.Flags	= OriString->Flags;
					CopyString.MaxSize	= OriString->MaxSize;
					CopyString.MinSize	= OriString->MinSize;

					CopyString.Question.Flags = OriString->Question.Flags;
					CopyString.Question.Header.Prompt = NewHiiString(SmcLsiSetupHandle,RaidItemsTable->ItemsHeader.LsiItemName);
					//Temporary use this.
					CopyString.Question.Header.Help   = CopyString.Question.Header.Prompt;

					CopyString.Question.QuestionId	  = pProtocol->SmcLsiGetQIdStart(pProtocol);
					CopyString.Question.VarStoreId	  = pItemsBody->SmcLsiVarId;

					CopyString.Question.VarStoreInfo.VarOffset = OriString->Question.VarStoreInfo.VarOffset;
				
					Cte_Buffer(FormItemsBuffer,&CopyString);

				}
					break;
				case EFI_IFR_ONE_OF_OP:
				{
					EFI_IFR_ONE_OF*		OriOneOf 		= (EFI_IFR_ONE_OF*)OpHeader;
					UINT8				DefaultFlags	= EFI_IFR_OPTION_DEFAULT;

					//First step Create ONE_OF
					Cte_Buffer(FormItemsBuffer,CopyOneOfItems(pProtocol,RaidItemsTable,OpHeader,pItemsBody->SmcLsiVarId));
					RecurseOneOfOption(pProtocol,FormItemsBuffer,RaidItemsTable,OpHeader,DefaultFlags,OpHeader->Scope);
					Cte_Buffer(FormItemsBuffer,CreateEndItem());
				}
					break;
				case EFI_IFR_NUMERIC_OP:
				{
					EFI_IFR_NUMERIC*	pNumer 		= NULL;
					EFI_IFR_NUMERIC		CopyNumer;
					EFI_IFR_DEFAULT		CopyDef;
					EFI_IFR_TYPE_VALUE	MinVal;

					pNumer	= (EFI_IFR_NUMERIC*)OpHeader;

					MemCpy(&CopyNumer.Header,&pNumer->Header,sizeof(EFI_IFR_OP_HEADER));

					CopyNumer.Flags								= pNumer->Flags;
					CopyNumer.Question.Flags					= pNumer->Question.Flags;
					CopyNumer.Question.Header.Prompt 			= NewHiiString(SmcLsiSetupHandle,RaidItemsTable->ItemsHeader.LsiItemName);
					CopyNumer.Question.Header.Help				= CopyNumer.Question.Header.Prompt;
					CopyNumer.Question.QuestionId				= pProtocol->SmcLsiGetQIdStart(pProtocol);
					CopyNumer.Question.VarStoreId				= pItemsBody->SmcLsiVarId;
					CopyNumer.Question.VarStoreInfo.VarOffset	= pNumer->Question.VarStoreInfo.VarOffset;

					MemCpy(&CopyNumer.data,&pNumer->data,sizeof(MINMAXSTEP_DATA));
					switch((CopyNumer.Flags & EFI_IFR_NUMERIC_SIZE)){
						case EFI_IFR_NUMERIC_SIZE_1:
							MinVal.u64 = CopyNumer.data.u8.MinValue;
							CopyNumer.data.u8.Step = 1;
							break;
						case EFI_IFR_NUMERIC_SIZE_2:
							MinVal.u64 = CopyNumer.data.u16.MinValue;
							CopyNumer.data.u16.Step = 1;
							break;
						case EFI_IFR_NUMERIC_SIZE_4:
							MinVal.u64 = CopyNumer.data.u32.MinValue;
							CopyNumer.data.u32.Step = 1;
							break;
						case EFI_IFR_NUMERIC_SIZE_8:
							MinVal.u64 = CopyNumer.data.u64.MinValue;
							CopyNumer.data.u64.Step = 1;
							break;
					}
					Cte_Buffer(FormItemsBuffer,&CopyNumer);

					CopyDef.Header.OpCode					= EFI_IFR_DEFAULT_OP;
					CopyDef.Header.Length					= sizeof(EFI_IFR_DEFAULT);
					CopyDef.Header.Scope					= 0;
					CopyDef.DefaultId						= EFI_HII_DEFAULT_CLASS_STANDARD;
					CopyDef.Type							= (CopyNumer.Flags & EFI_IFR_NUMERIC_SIZE);
					MemCpy(&(CopyDef.Value),&MinVal,sizeof(EFI_IFR_TYPE_VALUE));

					Cte_Buffer(FormItemsBuffer,&CopyDef);

					//Copy related OP? Now just copy the End Op.
					if(!!pNumer->Header.OpCode){
						EFI_IFR_END	nEndOp;

						nEndOp.Header.OpCode 	= EFI_IFR_END_OP;
						nEndOp.Header.Length 	= sizeof(EFI_IFR_END);
						nEndOp.Header.Scope		= 0;
						
						Cte_Buffer(FormItemsBuffer,&nEndOp);
					}
				}
					break;
				default:
					break;
			}
		}
	}
/*
 * Insert a Guid Label in this for next to Insert Change Items
 *
*/
	{
		EFI_IFR_GUID_LABEL*	GuidLabel = NULL;

		GuidLabel = CreateGuidLabel(pProtocol->SmcLsiGetOLabelStart(pProtocol));
		pProtocol->SmcLsiCurrHiiHandleTable->ChangeItemsLabel = pProtocol->SmcLsiGetOLabelNow(pProtocol);
		Cte_Buffer(FormItemsBuffer,GuidLabel);
	}

	EndOp.Header.OpCode = EFI_IFR_END_OP;
	EndOp.Header.Length = sizeof(EFI_IFR_END);
	EndOp.Header.Scope	= 0;

	Cte_Buffer(FormItemsBuffer,&EndOp);

	//Combine Upper + VarBuffer + button
	MACRO_CAULBUFFER_REPLACE(FormItemsBuffer);

	return EFI_SUCCESS;
}

EFI_STATUS InsertRaidSetupFormGoto(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol){

	MACRO_CAULBUFFER_ARGU(PACKAGE_BUFFER);

	UINT8*				GuidOp				= NULL;
	SMC_LSI_ITEMS_MEM*	FormGoToBuffer		= NULL;
	SMC_LSI_RAID_NANE*	RaidNameTable		= NULL;

	EFI_HII_HANDLE		SmcLsiSetupHandle	= NULL;

	CHAR16*			FormGoToPromptHelp	= NULL;
	UINTN			FormIndex			= 0;
	EFI_IFR_REF		FormGoto;

	GuidOp = (UINT8*)SearchOpCodeInFormData(pProtocol,EFI_IFR_GUID_OP,pProtocol->SmcLsiGetFormGoToLabel(pProtocol));	
	if(GuidOp == NULL) return SettingErrorStatus(pProtocol,0x02,EFI_LOAD_ERROR);
	
	InsertStart 		= (UINT8*)GuidOp;

	MACRO_CAULBUFFER_INITIAL(PACKAGE_BUFFER);

	SmcLsiSetupHandle 	= pProtocol->SmcLsiGetHiiHandle(pProtocol);
	RaidNameTable 		= pProtocol->SmcLsiRaidNameTable;

	FormGoToBuffer		= CopyAndExtMem(NULL);

	for(FormIndex=0;
		RaidNameTable[FormIndex].LsiRaidTypeIndex != RAID_NULL && 
		RaidNameTable[FormIndex].LsiRaidTypeIndex != pProtocol->SmcLsiCurrHiiHandleTable->RaidCardType;
		++FormIndex);

	do {
		UINTN		FormName_Length_Byte = 0;
		CHAR16		UnKnownRaidName[] 	= L"UNKNOWN RAID CARD";
		CHAR16*		RaidNameString		= NULL;

		RaidNameString = UnKnownRaidName;
		if(RaidNameTable[FormIndex].LsiRaidTypeIndex ==  pProtocol->SmcLsiCurrHiiHandleTable->RaidCardType){
			RaidNameString = RaidNameTable[FormIndex].FormRaidName;
		}

		FormName_Length_Byte = ((StrLen(RaidNameString) + 10 + 1) * sizeof(CHAR16));
		gBS->AllocatePool(EfiBootServicesData,FormName_Length_Byte,&FormGoToPromptHelp);
		MemSet(FormGoToPromptHelp,FormName_Length_Byte,0x00);
		Swprintf(FormGoToPromptHelp,L"%d. %s",pProtocol->SmcLsiCurrHiiHandleTable->RaidCardIndex,RaidNameString);
	}while(FALSE);

	MemSet(&FormGoto,sizeof(EFI_IFR_REF),0x0);

	FormGoto.Header.OpCode = EFI_IFR_REF_OP;
	FormGoto.Header.Length = sizeof(EFI_IFR_REF);
	FormGoto.Header.Scope  = 0;

	FormGoto.Question.Header.Prompt = NewHiiString(SmcLsiSetupHandle,FormGoToPromptHelp);
	FormGoto.Question.Header.Help	= NewHiiString(SmcLsiSetupHandle,FormGoToPromptHelp);

	FormGoto.Question.QuestionId				= pProtocol->SmcLsiGetQIdStart(pProtocol);
	FormGoto.Question.VarStoreId				= 0;
	FormGoto.Question.VarStoreInfo.VarOffset	= 0xFFFF;

	FormGoto.Question.Flags						= EFI_IFR_FLAG_CALLBACK;
	FormGoto.FormId								= pProtocol->SmcLsiGetFormIdStart(pProtocol);

	//How to pass this FormId to next create Form? Below variable now very good.
	pProtocol->SmcLsiCurrHiiHandleTable->SmcFormId 			= FormGoto.FormId;
	pProtocol->SmcLsiCurrHiiHandleTable->SmcFormTitleId		= FormGoto.Question.Header.Prompt;
	
	Cte_Buffer(FormGoToBuffer,&FormGoto);

	gBS->FreePool(FormGoToPromptHelp);

	//Combine Upper + VarBuffer + button
	MACRO_CAULBUFFER_REPLACE(FormGoToBuffer);

	return EFI_SUCCESS;
}



EFI_STATUS	InsertRaidSetupVariable(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol){
	
	MACRO_CAULBUFFER_ARGU(PACKAGE_BUFFER);

	static BOOLEAN			InsertDefStore		= TRUE;	//Temporary Set to TRUE that use VFR auto gen defaultstore.
	EFI_STATUS				Status				= EFI_SUCCESS;
	UINTN					VarIndex			= 0;

	EFI_IFR_FORM_SET*		FormSetOp			= NULL;
	EFI_IFR_VARSTORE*		VarStore 			= NULL;

	SMC_LSI_ITEMS_MEM*		ItemsBuffer			= NULL;
	
	SMC_SETUP_RAID_VAR*		LocSmcLsiVarTable	= NULL;
	EFI_HII_HANDLE			SmcLsiSetupHandle	= NULL;

	FormSetOp = (EFI_IFR_FORM_SET*)SearchOpCodeInFormData(pProtocol,EFI_IFR_FORM_SET_OP,0);	
	if(FormSetOp == NULL) return SettingErrorStatus(pProtocol,0x02,EFI_LOAD_ERROR);

	LocSmcLsiVarTable = pProtocol->SmcLsiCurrHiiHandleTable->RaidSetupVarSet;
	SmcLsiSetupHandle = pProtocol->SmcLsiGetHiiHandle(pProtocol);

	InsertStart 		= (UINT8*)((UINT8*)FormSetOp + FormSetOp->Header.Length);
	MACRO_CAULBUFFER_INITIAL(PACKAGE_BUFFER);

	ItemsBuffer = CopyAndExtMem(NULL);

//DEFAULT Store. //Temporary put here.
	if(!(!!InsertDefStore)){
		EFI_IFR_DEFAULTSTORE DefStore;
		
		DefStore.Header.OpCode 	= EFI_IFR_DEFAULTSTORE_OP;
		DefStore.Header.Length 	= sizeof(EFI_IFR_DEFAULTSTORE);
		DefStore.Header.Scope	= 0;

		DefStore.DefaultName	= NewHiiString(SmcLsiSetupHandle,L"STANDARD");
		DefStore.DefaultId		= EFI_HII_DEFAULT_CLASS_STANDARD;
		
		Cte_Buffer(ItemsBuffer,&DefStore);

		DefStore.DefaultName	= NewHiiString(SmcLsiSetupHandle,L"MANUFACTURING");
		DefStore.DefaultId		= EFI_HII_DEFAULT_CLASS_MANUFACTURING;

		Cte_Buffer(ItemsBuffer,&DefStore);

		InsertDefStore = TRUE;
	}
	
	for(; LocSmcLsiVarTable != NULL;LocSmcLsiVarTable = LocSmcLsiVarTable->pSetupRaidVarNext){
		UINT8	VarStoreLength = (UINT8)(sizeof(EFI_IFR_VARSTORE) + Strlen(LocSmcLsiVarTable->SetupRaidVar.RaidVarName));

		VarStore = NULL;
		Status = gBS->AllocatePool(EfiBootServicesData,VarStoreLength,&VarStore);
		if(EFI_ERROR(Status)) return SettingErrorStatus(pProtocol,0x05,Status);
		MemSet(VarStore,VarStoreLength,0x00);

		VarStore->Header.OpCode = EFI_IFR_VARSTORE_OP;
		VarStore->Header.Length = VarStoreLength;
		VarStore->Header.Scope  = 0;
			
		MemCpy(&VarStore->Guid,&LocSmcLsiVarTable->SetupRaidVar.RaidVarGuid,sizeof(EFI_GUID));
		MemCpy(VarStore->Name,LocSmcLsiVarTable->SetupRaidVar.RaidVarName,Strlen(LocSmcLsiVarTable->SetupRaidVar.RaidVarName));
		VarStore->Name[Strlen(LocSmcLsiVarTable->SetupRaidVar.RaidVarName)] = '\0';
			
		VarStore->Size 		 = LocSmcLsiVarTable->SetupRaidVar.RaidVarSize;
		VarStore->VarStoreId = LocSmcLsiVarTable->SetupRaidVar.RaidVarId;
				
		Cte_Buffer(ItemsBuffer,VarStore);
		gBS->FreePool(VarStore);
	}

	//Combine Upper + VarBuffer + button
	MACRO_CAULBUFFER_REPLACE(ItemsBuffer);
	return EFI_SUCCESS;
}
VOID	DEBUG_ADDDYNAMICITEMS_PACKAGE(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol){

#if defined(DEBUG_MODE) && (DEBUG_MODE == 1)
	EFI_HII_PACKAGE_HEADER*			SmcLsiSetupIfrPackagePtr 		= NULL;
	EFI_IFR_OP_HEADER*				IfrOpHeader						= NULL;
	EFI_HII_HANDLE					SmcSetupHiiHandle				= NULL;
	UINT16							ScopeSpace						= 0;
	CHAR8							ScopeBuff[50];

	SmcLsiSetupIfrPackagePtr	= (EFI_HII_PACKAGE_HEADER*)pProtocol->SmcLsiCurrRAIDSetupData;
	SmcSetupHiiHandle			= pProtocol->SmcLsiGetHiiHandle(pProtocol);

	DEBUG((-1,"DEBUG -- SmcRAID SETUP HII PACKAGE Form :: \n"));
	for(IfrOpHeader = (EFI_IFR_OP_HEADER*)(SmcLsiSetupIfrPackagePtr+1);
		((UINT32)IfrOpHeader) < (UINT32)((UINT8*)SmcLsiSetupIfrPackagePtr + SmcLsiSetupIfrPackagePtr->Length);
		IfrOpHeader = (EFI_IFR_OP_HEADER*)((UINT8*)IfrOpHeader + IfrOpHeader->Length)){
		
		MemSet(ScopeBuff,50*sizeof(CHAR8),0x00);
		MemSet(ScopeBuff,ScopeSpace * 4,' ');

		switch(IfrOpHeader->OpCode){
			case EFI_IFR_FORM_SET_OP:
			{
				EFI_IFR_FORM_SET*	pFormSet = NULL;
				
				pFormSet = (EFI_IFR_FORM_SET*)IfrOpHeader;
				DEBUG((-1,"%aFormSet[%s] :: \n",ScopeBuff,GetHiiString(SmcSetupHiiHandle,pFormSet->FormSetTitle)));
			}
				break;
			case EFI_IFR_DEFAULTSTORE_OP:
			{
				EFI_IFR_DEFAULTSTORE*	DefStore = NULL;
				DefStore = (EFI_IFR_DEFAULTSTORE*)IfrOpHeader;

				DEBUG((-1,"%aDefStore[%s], DefId[%x]\n",ScopeBuff,GetHiiString(SmcSetupHiiHandle,DefStore->DefaultName),DefStore->DefaultId));
			}
				break;
			case EFI_IFR_DEFAULT_OP:
			{
				EFI_IFR_DEFAULT*		DefVal 	= NULL;

				DefVal = (EFI_IFR_DEFAULT*)IfrOpHeader;	
				DEBUG((-1,"%aDefault DefId[%x], Type[%x], Value[%x]\n",ScopeBuff,DefVal->DefaultId,DefVal->Type,DefVal->Value.u64));
			}
				break;
			case EFI_IFR_VARSTORE_OP:
			{
				EFI_IFR_VARSTORE*	pVar = NULL;
				pVar = (EFI_IFR_VARSTORE*)IfrOpHeader;
				DEBUG((-1,"%aVarStore[%a], Size[%x], VId[%x], Guid[%g]\n",ScopeBuff,pVar->Name,pVar->Size,pVar->VarStoreId,pVar->Guid));
			}
				break;
			case EFI_IFR_TEXT_OP:
			{
				EFI_IFR_TEXT*	pText = NULL;
				pText = (EFI_IFR_TEXT*)IfrOpHeader;

				DEBUG((-1,"%aText Prompt[%s]\n",ScopeBuff,GetHiiString(SmcSetupHiiHandle,pText->Statement.Prompt)));
			}
				break;
			case EFI_IFR_FORM_OP:
			{
				EFI_IFR_FORM*	pForm = NULL;
				pForm = (EFI_IFR_FORM*)IfrOpHeader;
				DEBUG((-1,"%aForm[%s], FormId[%x] :: \n",ScopeBuff,GetHiiString(SmcSetupHiiHandle,pForm->FormTitle),pForm->FormId));
			}
				break;
			case EFI_IFR_STRING_OP:
			{
				EFI_IFR_STRING*		pString 	= NULL;
				SMC_RAID_VAR*		pVariable	= NULL;
				UINT8*				pBuffer		= NULL;
				
				pString = (EFI_IFR_STRING*)IfrOpHeader;
				DEBUG((-1,"%aString[%s], QId[%x], VId[%x], VOffset[%x]\n",ScopeBuff,
							GetHiiString(SmcSetupHiiHandle,pString->Question.Header.Prompt),
							pString->Question.QuestionId,
							pString->Question.VarStoreId,
							pString->Question.VarStoreInfo.VarOffset));

				pVariable = SearchInSmcSetupVarById(pProtocol,pString->Question.VarStoreId);
				if(pVariable != NULL){
					pBuffer = pVariable->RaidVarBuffer;
					DEBUG((-1,"%a Val:\"%s\"\n",ScopeBuff,(EFI_STRING)(&pBuffer[pString->Question.VarStoreInfo.VarOffset])));
				}else{
					DEBUG((-1,"Can't find Variable!\n"));
				}
			}
				break;
			case EFI_IFR_REF_OP:
			{	
				EFI_IFR_REF* pRef = NULL;
				pRef = (EFI_IFR_REF*)IfrOpHeader;
				DEBUG((-1,"%aRef[%s], FromId[%x], QId[%x]\n",ScopeBuff,
							GetHiiString(SmcSetupHiiHandle,pRef->Question.Header.Prompt),
							pRef->FormId,
							pRef->Question.QuestionId));
			}
				break;
			case EFI_IFR_GUID_OP:
			{
				EFI_IFR_GUID* 		GuidOp = NULL;
				EFI_IFR_GUID_LABEL*	pIfrGuidLabel = NULL;
				EFI_GUID	  		LabelGuid = EFI_IFR_TIANO_GUID;

				GuidOp = (EFI_IFR_GUID*)IfrOpHeader;
				DEBUG((-1,"%aGuid[%g], ",ScopeBuff,GuidOp->Guid));
				if(MemCmp(&GuidOp->Guid,&LabelGuid,sizeof(EFI_GUID)) == 0){
					pIfrGuidLabel = (EFI_IFR_GUID_LABEL*)GuidOp;
					DEBUG((-1,"Label ExtendOp[%x], Number[%x]",pIfrGuidLabel->ExtendOpCode,pIfrGuidLabel->Number));
				}
				DEBUG((-1,"\n"));
			}
				break;
			case EFI_IFR_NUMERIC_OP:
			case EFI_IFR_ONE_OF_OP:
			{
				//These two have same structure.
				EFI_IFR_NUMERIC*		pNumer			= NULL;
				SMC_RAID_VAR*			pVariable		= NULL;
				UINT8*					pBuffer			= NULL;
				UINT64					DVal			= 0;
				UINT64					MinValue		= 0;
				UINT64					MaxValue		= 0;
				UINT64					Step			= 0;
				CHAR16					NumSize[][5]	= {L"u8", L"u16", L"u32", L"u64"};

				pNumer = (EFI_IFR_NUMERIC*)IfrOpHeader;
				DEBUG((-1,"%a%s",ScopeBuff,(pNumer->Header.OpCode == EFI_IFR_NUMERIC_OP) ? L"Numeric" : L"OneOf"));

				DEBUG((-1,"[%s], Flags[%x], QId[%x], VId[%x], VOffset[%x], DataSize[%s]\n",
							GetHiiString(SmcSetupHiiHandle,pNumer->Question.Header.Prompt),
							pNumer->Flags,
							pNumer->Question.QuestionId,
							pNumer->Question.VarStoreId,
							pNumer->Question.VarStoreInfo.VarOffset,
							NumSize[(pNumer->Flags & EFI_IFR_NUMERIC_SIZE)]));

				pVariable = SearchInSmcSetupVarById(pProtocol,pNumer->Question.VarStoreId);
				if(pVariable != NULL){
					pBuffer = pVariable->RaidVarBuffer;
					switch((pNumer->Flags & EFI_IFR_NUMERIC_SIZE)){
						case EFI_IFR_NUMERIC_SIZE_1:
							MinValue = pNumer->data.u8.MinValue;
							MaxValue = pNumer->data.u8.MaxValue;
							Step	 = pNumer->data.u8.Step;
							DVal = *((UINT8*)(&pBuffer[pNumer->Question.VarStoreInfo.VarOffset]));
							break;
						case EFI_IFR_NUMERIC_SIZE_2:
							MinValue = pNumer->data.u16.MinValue;
							MaxValue = pNumer->data.u16.MaxValue;
							Step	 = pNumer->data.u16.Step;
							DVal = *((UINT16*)(&pBuffer[pNumer->Question.VarStoreInfo.VarOffset]));
							break;
						case EFI_IFR_NUMERIC_SIZE_4:
							MinValue = pNumer->data.u32.MinValue;
							MaxValue = pNumer->data.u32.MaxValue;
							Step	 = pNumer->data.u32.Step;
							DVal = *((UINT32*)(&pBuffer[pNumer->Question.VarStoreInfo.VarOffset]));
							break;
						case EFI_IFR_NUMERIC_SIZE_8:
							MinValue = pNumer->data.u64.MinValue;
							MaxValue = pNumer->data.u64.MaxValue;
							Step	 = pNumer->data.u64.Step;
							DVal = *((UINT64*)(&pBuffer[pNumer->Question.VarStoreInfo.VarOffset]));
							break;
					}
					DEBUG((-1,"%a Val:\"%x\" Range[%x - %x], Step[%x]\n",ScopeBuff,DVal,MinValue,MaxValue,Step));
				}else{
					DEBUG((-1,"Can't find Variable!\n"));
				}
			}
				break;
			case EFI_IFR_ONE_OF_OPTION_OP:
			{
				EFI_IFR_ONE_OF_OPTION* pOneOfOption = NULL;
				UINTN	Val = 0x0;
				pOneOfOption = (EFI_IFR_ONE_OF_OPTION*)IfrOpHeader;
				DEBUG((-1,"%aOption[%s], Flags[%x], Type[%x], Val[%x]\n",ScopeBuff,
							GetHiiString(SmcSetupHiiHandle,pOneOfOption->Option),
							pOneOfOption->Flags,pOneOfOption->Type,pOneOfOption->Value.u64));
			}
				break;
			case EFI_IFR_CHECKBOX_OP:
			{
				EFI_IFR_CHECKBOX*	pCheckBox = NULL;
				pCheckBox = (EFI_IFR_CHECKBOX*)IfrOpHeader;

				DEBUG((-1,"%aCheckBox[%s], QId[%x], VId[%x], VOffset[%x]\n",ScopeBuff,
							GetHiiString(SmcSetupHiiHandle,pCheckBox->Question.Header.Prompt),
							pCheckBox->Question.QuestionId,
							pCheckBox->Question.VarStoreId,
							pCheckBox->Question.VarStoreInfo.VarOffset));
			}
				break;
			case EFI_IFR_SUBTITLE_OP:
			{
				EFI_IFR_SUBTITLE*	pSubTitle = NULL;
				pSubTitle = (EFI_IFR_SUBTITLE*)IfrOpHeader;

				DEBUG((-1,"%aSubtitle[%s]\n",ScopeBuff,GetHiiString(SmcSetupHiiHandle,pSubTitle->Statement.Prompt)));
			}
				break;
			default:
				break;
		}
		if(IfrOpHeader->Scope){
			++ScopeSpace;
		}
		if(IfrOpHeader->OpCode == EFI_IFR_END_OP){
			--ScopeSpace;
		}
	}
	return;
#endif

}

EFI_STATUS	InitialSmcRaidSetupPageData(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol){

	EFI_HII_HANDLE				 SmcLsiSetupHandle				= NULL;
	EFI_STATUS					 Status						 	= EFI_SUCCESS;
	
	EFI_HII_PACKAGE_HEADER 		*SmcLsiSetupIfrPackagePtr 		= NULL;
	EFI_HII_PACKAGE_LIST_HEADER *SmcLsiSetupHiiPackageList 		= NULL;

	SmcLsiSetupHandle = pProtocol->SmcLsiGetHiiHandle(pProtocol);

	if(!(!!SmcLsiSetupHandle)) return SettingErrorStatus(pProtocol,0x01,EFI_INVALID_PARAMETER);

	pProtocol->SmcLsiCurrRAIDSetupData	= NULL;

	SmcLsiSetupHiiPackageList = GetHiiPackageList(SmcLsiSetupHandle);
	if(!(!!SmcLsiSetupHiiPackageList)) return SettingErrorStatus(pProtocol,0x03,EFI_NOT_FOUND);

  	for(SmcLsiSetupIfrPackagePtr = (EFI_HII_PACKAGE_HEADER*)(SmcLsiSetupHiiPackageList+1);
	  	SmcLsiSetupIfrPackagePtr < (EFI_HII_PACKAGE_HEADER*)(((UINT8*)SmcLsiSetupHiiPackageList) + SmcLsiSetupHiiPackageList->PackageLength);
	  	SmcLsiSetupIfrPackagePtr = (EFI_HII_PACKAGE_HEADER*)((UINT8*)SmcLsiSetupIfrPackagePtr + SmcLsiSetupIfrPackagePtr->Length)){
 
		if(SmcLsiSetupIfrPackagePtr->Type == EFI_HII_PACKAGE_FORMS) break;
	}

	if(SmcLsiSetupIfrPackagePtr >= (EFI_HII_PACKAGE_HEADER*)(((UINT8*)SmcLsiSetupHiiPackageList) + SmcLsiSetupHiiPackageList->PackageLength))
		return SettingErrorStatus(pProtocol,0x02,EFI_LOAD_ERROR);

	Status = gBS->AllocatePool(EfiBootServicesData,MAX_HPK_LIST_SIZE,&(pProtocol->SmcLsiCurrRAIDSetupData));
	RETURN_IFERROR();
	MemCpy(pProtocol->SmcLsiCurrRAIDSetupData,SmcLsiSetupIfrPackagePtr,SmcLsiSetupIfrPackagePtr->Length);
	pProtocol->SmcLsiCurrRAIDSetupDataAddedSize		= 0;

	DEBUG_ADDDYNAMICITEMS_PACKAGE(pProtocol);
	return EFI_SUCCESS;
}

EFI_STATUS	UpdateSmcRaidOOBSetupdata(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol){

	EFI_HII_HANDLE				 	SmcLsiSetupHandle				= NULL;
  	UINTN						 	SmcLsiSetupBufferSize 			= 0;
	EFI_HII_PACKAGE_HEADER*			SmcLsiSetupIfrPackagePtr 		= NULL;
	EFI_HII_PACKAGE_LIST_HEADER*	SmcLsiSetupHiiPackageList 		= NULL;
	EFI_HII_PACKAGE_HEADER*			SmcRaidOOBPackHeader			= NULL;
	
	EFI_STATUS						Status							= EFI_SUCCESS;
	UINT8*							ListStart						= NULL;
	UINT8*							ReplaceStart					= NULL;
	UINT8*							ReplaceEnd						= NULL;
	UINT8*							ListEnd							= NULL;
	UINT8*							UpperBuffer						= NULL;
	UINT8*							ButtonBuffer					= NULL;
	UINT8*							CopyBuffer						= NULL;
	UINTN							StRLeng							= 0;
	UINTN							RtELeng							= 0;

	DEBUG_ADDDYNAMICITEMS_PACKAGE(pProtocol);

	SmcLsiSetupHandle = pProtocol->SmcLsiGetHiiHandle(pProtocol);

	SmcRaidOOBPackHeader 		= (EFI_HII_PACKAGE_HEADER*)pProtocol->SmcLsiCurrRAIDSetupData;
	//Why should export again? Because we add new string in functions.
	SmcLsiSetupHiiPackageList	= GetHiiPackageList(SmcLsiSetupHandle);
	if(!(!!SmcLsiSetupHiiPackageList)) return SettingErrorStatus(pProtocol,0x01,EFI_NOT_FOUND);

  	for(SmcLsiSetupIfrPackagePtr = (EFI_HII_PACKAGE_HEADER*)(SmcLsiSetupHiiPackageList+1);
	  	SmcLsiSetupIfrPackagePtr < (EFI_HII_PACKAGE_HEADER*)(((UINT8*)SmcLsiSetupHiiPackageList) + SmcLsiSetupHiiPackageList->PackageLength);
	  	SmcLsiSetupIfrPackagePtr = (EFI_HII_PACKAGE_HEADER*)((UINT8*)SmcLsiSetupIfrPackagePtr + SmcLsiSetupIfrPackagePtr->Length)){
		
		if(SmcLsiSetupIfrPackagePtr->Type == EFI_HII_PACKAGE_FORMS) break;
	}

	if(SmcLsiSetupIfrPackagePtr >= (EFI_HII_PACKAGE_HEADER*)(((UINT8*)SmcLsiSetupHiiPackageList) + SmcLsiSetupHiiPackageList->PackageLength))
		return SettingErrorStatus(pProtocol,0x02,EFI_LOAD_ERROR);

	ListStart 		= (UINT8*)SmcLsiSetupHiiPackageList;
	ReplaceStart 	= (UINT8*)SmcLsiSetupIfrPackagePtr;
	ReplaceEnd		= (UINT8*)(((UINT8*)SmcLsiSetupIfrPackagePtr) + SmcLsiSetupIfrPackagePtr->Length);
	ListEnd 		= (UINT8*)(((UINT8*)SmcLsiSetupHiiPackageList) + SmcLsiSetupHiiPackageList->PackageLength);
	CopyBuffer		= (UINT8*)(SmcLsiSetupHiiPackageList);

	StRLeng			= (UINTN)((UINTN)ReplaceStart - (UINTN)ListStart);
	RtELeng			= (UINTN)((UINTN)ListEnd - (UINTN)ReplaceEnd);

	gBS->AllocatePool(EfiBootServicesData,StRLeng,&UpperBuffer);
	MemCpy(UpperBuffer,ListStart,StRLeng);

	gBS->AllocatePool(EfiBootServicesData,RtELeng,&ButtonBuffer);
	MemCpy(ButtonBuffer,ReplaceEnd,RtELeng);

	MemCpy(CopyBuffer,UpperBuffer,StRLeng);
	CopyBuffer = (UINT8*)(CopyBuffer + StRLeng);

	MemCpy(CopyBuffer,SmcRaidOOBPackHeader,SmcRaidOOBPackHeader->Length);
	CopyBuffer = (UINT8*)(CopyBuffer + SmcRaidOOBPackHeader->Length);

	MemCpy(CopyBuffer,ButtonBuffer,RtELeng);

	SmcLsiSetupHiiPackageList->PackageLength += pProtocol->SmcLsiCurrRAIDSetupDataAddedSize;
	
	Status = GetHiiDataBase()->UpdatePackageList(GetHiiDataBase(),SmcLsiSetupHandle,SmcLsiSetupHiiPackageList);
	if(EFI_ERROR(Status)) return SettingErrorStatus(pProtocol,0x02,Status);

	return Status;
}

EFI_STATUS	SmcLsiRaidLib_AddDynamicItems(SMC_LSI_RAID_OOB_SETUP_PROTOCOL*	pProtocol){

	EFI_STATUS	Status		= EFI_SUCCESS;
	UINTN		RaidIndex	= 0;
	
	if(pProtocol == NULL) return EFI_LOAD_ERROR;

	DEBUG((-1, "\n... SmcLsiRaidLib_AddDynamicItems ... START ...\n"));

	THROUGH_DEBUG_MESSAGE(pProtocol,L"InitialSmcRaidSetupPageData",SMC_LSI_THROUGH_FUNC_EXEC);
	Status = InitialSmcRaidSetupPageData(pProtocol);
	Status = THROUGH_DEBUG_MESSAGE(pProtocol,L"InitialSmcRaidSetupPageData",Status);
	if(EFI_ERROR(Status)) return Status; 

	for(pProtocol->SmcLsiCurrHiiHandleTable = pProtocol->SmcLsiHiiHandleTable;
		pProtocol->SmcLsiCurrHiiHandleTable != NULL && pProtocol->SmcLsiCurrHiiHandleTable->RaidCardType != RAID_NULL;
		pProtocol->SmcLsiCurrHiiHandleTable = pProtocol->SmcLsiCurrHiiHandleTable->pNext){
		
		DEBUG((-1,"    SmcLsiCurrHiiHandleTable RaidCardType[%x], RaidCardIndex[%x], RaidCardHiiHandle[%08x]\n",
				   pProtocol->SmcLsiCurrHiiHandleTable->RaidCardType,
				   pProtocol->SmcLsiCurrHiiHandleTable->RaidCardIndex,
				   (UINT32)pProtocol->SmcLsiCurrHiiHandleTable->RaidCardHiiHandle
			));

		for(RaidIndex=0;mSmcLsiInsideLoadFunc[RaidIndex].Func != NULL;RaidIndex++){
			THROUGH_DEBUG_MESSAGE(pProtocol,mSmcLsiInsideLoadFunc[RaidIndex].FuncName,SMC_LSI_THROUGH_FUNC_EXEC);
			Status = mSmcLsiInsideLoadFunc[RaidIndex].Func(pProtocol);
			Status = THROUGH_DEBUG_MESSAGE(pProtocol,mSmcLsiInsideLoadFunc[RaidIndex].FuncName,Status);
			if(EFI_ERROR(Status)) break;
		}
		if(EFI_ERROR(Status)) break;
		
		for(RaidIndex=0;pProtocol->SmcLsiAfterLoadFuncTable[RaidIndex].Func != NULL;RaidIndex++){
			THROUGH_DEBUG_MESSAGE(pProtocol,pProtocol->SmcLsiAfterLoadFuncTable[RaidIndex].FuncName,SMC_LSI_THROUGH_FUNC_EXEC);
			Status = pProtocol->SmcLsiAfterLoadFuncTable[RaidIndex].Func(pProtocol);
			Status = THROUGH_DEBUG_MESSAGE(pProtocol,pProtocol->SmcLsiAfterLoadFuncTable[RaidIndex].FuncName,Status);
			if(EFI_ERROR(Status)) break;
		}
		if(EFI_ERROR(Status)) break;
	}
	if(!EFI_ERROR(Status)){
		THROUGH_DEBUG_MESSAGE(pProtocol,L"UpdateSmcRaidOOBSetupdata",SMC_LSI_THROUGH_FUNC_EXEC);
		Status = UpdateSmcRaidOOBSetupdata(pProtocol);	
		Status = THROUGH_DEBUG_MESSAGE(pProtocol,L"UpdateSmcRaidOOBSetupdata",Status);
	}
	DEBUG((-1, "... SmcLsiRaidLib_AddDynamicItems ... END ...\n"));
	return Status;
}

EFI_STATUS SmcLsiRaidLib_ParseNvData(
		SMC_LSI_RAID_OOB_SETUP_PROTOCOL* 	 pProtocol,
		BOOLEAN								 ReNew,
		OUT CHAR16**						 SmcRaidVarName,
		OUT EFI_GUID**						 SmcRaidVarGuid,
		OUT UINTN**							 SmcRaidVarSize,
		OUT UINT8**							 SmcRaidVarBuffer)
{

		static	BOOLEAN					ReNewVar			= TRUE;
		static	SMC_SETUP_RAID_VAR*		SmcLsiVarTable 		= NULL;
		EFI_STATUS						Status				= EFI_SUCCESS;

		if((!!ReNew)) ReNewVar = TRUE;

		SMC_RAID_DETAIL_DEBUG((-1,"SmcLsiRaidLib_ParseNvData :: ReNewVar[%x], ",ReNewVar));

		if(ReNewVar){
			pProtocol->SmcLsiCurrHiiHandleTable = pProtocol->SmcLsiHiiHandleTable;
			SmcLsiVarTable = NULL;
		}
		while(pProtocol->SmcLsiCurrHiiHandleTable != NULL && SmcLsiVarTable == NULL){
			 SmcLsiVarTable = pProtocol->SmcLsiCurrHiiHandleTable->RaidSetupVarSet;
			 pProtocol->SmcLsiCurrHiiHandleTable = pProtocol->SmcLsiCurrHiiHandleTable->pNext;
		}
		if(SmcLsiVarTable == NULL){
			SMC_RAID_DETAIL_DEBUG((-1,"SmcLsiRaidLib_ParseNvData :: Not Find\n"));
			ReNewVar = TRUE;
			return EFI_NO_MEDIA;
		}

		*SmcRaidVarName = NULL; *SmcRaidVarGuid = NULL; *SmcRaidVarSize = NULL; *SmcRaidVarBuffer = NULL;
		Status = gBS->AllocatePool(EfiBootServicesData,(Strlen(SmcLsiVarTable->SetupRaidVar.RaidVarName) + 1) * sizeof(CHAR16),SmcRaidVarName);
		RETURN_IFERROR();
		MemSet(*SmcRaidVarName,(Strlen(SmcLsiVarTable->SetupRaidVar.RaidVarName) + 1) * sizeof(CHAR16),0x00);
		AsciiStrToUnicodeStr(SmcLsiVarTable->SetupRaidVar.RaidVarName,*SmcRaidVarName);
		SMC_RAID_DETAIL_DEBUG((-1,"SmcRaidVarName[%s], ",*SmcRaidVarName));

		Status = gBS->AllocatePool(EfiBootServicesData,sizeof(EFI_GUID),SmcRaidVarGuid);
		RETURN_IFERROR();
		MemSet(*SmcRaidVarGuid,sizeof(EFI_GUID),0x00);
		MemCpy(*SmcRaidVarGuid,&SmcLsiVarTable->SetupRaidVar.RaidVarGuid,sizeof(EFI_GUID));
		SMC_RAID_DETAIL_DEBUG((-1,"SmcRaidVarGuid[%g], ",*SmcRaidVarGuid));

		Status = gBS->AllocatePool(EfiBootServicesData,sizeof(UINTN),SmcRaidVarSize);
		RETURN_IFERROR();
		MemSet(*SmcRaidVarSize,sizeof(UINTN),0x00);
		MemCpy(*SmcRaidVarSize,&SmcLsiVarTable->SetupRaidVar.RaidVarSize,sizeof(UINT16));
		SMC_RAID_DETAIL_DEBUG((-1,"SmcRaidVarSize[%x]\n",**SmcRaidVarSize));

		Status = gBS->AllocatePool(EfiBootServicesData,SmcLsiVarTable->SetupRaidVar.RaidVarSize,SmcRaidVarBuffer);
		RETURN_IFERROR();
		MemSet(*SmcRaidVarBuffer,SmcLsiVarTable->SetupRaidVar.RaidVarSize,0x00);
		MemCpy(*SmcRaidVarBuffer,SmcLsiVarTable->SetupRaidVar.RaidVarBuffer,SmcLsiVarTable->SetupRaidVar.RaidVarSize);

		SmcLsiVarTable 	= SmcLsiVarTable->pSetupRaidVarNext;
		ReNewVar 		= FALSE;

		return EFI_SUCCESS;
}

EFI_STATUS SmcLsiRaidLib_CollectData(
	SMC_LSI_RAID_OOB_SETUP_PROTOCOL* 	 pProtocol,
	CHAR16* 	VariableName,
	EFI_GUID* 	VariableGuid,
	UINTN 		VariableSize,
	UINT8* 		VariableBuffer){

	static CHAR16		uRaidItemVarNamePrefix[NAME_LENGTH];
	static CHAR16		uSmcItemVarNamePrefix[NAME_LENGTH];

	CHAR8*		aRaidItemVarNamePrefix = NULL;
	CHAR8*		aSmcItemVarNamePrefix  = NULL;

	SMC_CHANGED_VAR_TYPE		VarType 	= VAR_NON_TYPE;
	SMC_CHANGED_VAR_SET*		pChVarSet 	= NULL;
	SMC_RAID_VAR*				pVar		= NULL;
		
	SMC_LSI_HII_HANDLE*			pHiiHandle	= NULL;
	SMC_RAID_DETAIL_DEBUG((-1,"SmcLsiRaidLib_CollectData :: \n"));
	SMC_RAID_DETAIL_DEBUG((-1,"VariableName[%s], VariableGuid[%g], VariableSize[%x]\n",VariableName,*VariableGuid,VariableSize));
	//PREFIX_NNNN_X_Y; => RAID VAR //PREFIX_X_Y SMC VAR
	aRaidItemVarNamePrefix = pProtocol->SmcLsiGetRaidVarName(pProtocol);	
	aSmcItemVarNamePrefix  = pProtocol->SmcLsiGetSmcItemsVarName(pProtocol);

	MemSet(uRaidItemVarNamePrefix,NAME_LENGTH * sizeof(CHAR16),0x00);
	MemSet(uSmcItemVarNamePrefix,NAME_LENGTH * sizeof(CHAR16),0x00);

	AsciiStrToUnicodeStr(aRaidItemVarNamePrefix,uRaidItemVarNamePrefix);
	AsciiStrToUnicodeStr(aSmcItemVarNamePrefix,uSmcItemVarNamePrefix);
	SMC_RAID_DETAIL_DEBUG((-1,"uRaidItemVarNamePrefix[%s], uSmcItemVarNamePrefix[%s]\n",uRaidItemVarNamePrefix,uSmcItemVarNamePrefix));

	if(!(!!MemCmp(VariableName,uRaidItemVarNamePrefix,StrLen(uRaidItemVarNamePrefix) * sizeof(CHAR16)))){
		VarType = VAR_RAID_TYPE;
	}else if(!(!!MemCmp(VariableName,uSmcItemVarNamePrefix,StrLen(uSmcItemVarNamePrefix) * sizeof(CHAR16)))){
		VarType = VAR_SMC_TYPE;
	}
	SMC_RAID_DETAIL_DEBUG((-1,"VarType[%x]\n",VarType));

	if(VarType == VAR_NON_TYPE) return EFI_UNSUPPORTED;

	gBS->AllocatePool(EfiBootServicesData,sizeof(SMC_CHANGED_VAR_SET),&pChVarSet);
	MemSet(pChVarSet,sizeof(SMC_CHANGED_VAR_SET),0x00);

	UnicodeStrToAsciiStr(VariableName,pChVarSet->ChVarHeader.ChangedVarName);
	MemCpy(&pChVarSet->ChVarHeader.ChangedVarGuid,VariableGuid,sizeof(EFI_GUID));
	pChVarSet->ChVarHeader.ChangedVarLength = VariableSize;
	SMC_RAID_DETAIL_DEBUG((-1,"ChangedVarName[%a], ChangedVarGuid[%g]\n",pChVarSet->ChVarHeader.ChangedVarName,pChVarSet->ChVarHeader.ChangedVarGuid));

	pHiiHandle = SearchHiiHandleTableByVarName(pProtocol,pChVarSet->ChVarHeader.ChangedVarName,&pChVarSet->ChVarHeader.ChangedVarGuid);
	if(!(!!pHiiHandle)){
		SMC_RAID_DETAIL_DEBUG((-1,"Cannot Find the Target HiiHandleTable!\n"));
		gBS->FreePool(pChVarSet);
		return EFI_NOT_FOUND;
	}
	
	pVar = SearchInTargetHandleSmcSetupVarByName(pProtocol,pHiiHandle,pChVarSet->ChVarHeader.ChangedVarName,&pChVarSet->ChVarHeader.ChangedVarGuid);
	if(!(!!pVar) || pVar->RaidVarSize != VariableSize){
		SMC_RAID_DETAIL_DEBUG((-1,"Cannot Find the variable in SmcSetupVarTable!\n"));
		gBS->FreePool(pChVarSet);
		return EFI_NOT_FOUND;
	}


	pChVarSet->ChVarBody.ChangedVarType = VarType;
	if(pChVarSet->ChVarBody.ChangedVarType == VAR_RAID_TYPE){
		CHAR8*	sStart 		= NULL;
		CHAR8*	sEnd	  	= NULL;

		sStart 	= Strstr(pChVarSet->ChVarHeader.ChangedVarName,"_");
		sStart = sStart+1;
		sEnd	= Strstr(sStart,"_");
		
		MemCpy(pChVarSet->ChVarBody.ChangedVarOriName,sStart,(UINT32)sEnd - (UINT32)sStart);
		pChVarSet->ChVarBody.ChangedVarOriName[(UINT32)sEnd - (UINT32)sStart] = '\0';
		SMC_RAID_DETAIL_DEBUG((-1,"ChangedVarOriName[%a]\n",pChVarSet->ChVarBody.ChangedVarOriName));
	}
	
	pChVarSet->ChVarBody.BeUpdated = FALSE;

	gBS->AllocatePool(EfiBootServicesData,pChVarSet->ChVarHeader.ChangedVarLength,&pChVarSet->ChangedVarBuffer);
	MemSet(pChVarSet->ChangedVarBuffer,pChVarSet->ChVarHeader.ChangedVarLength,0x00);
	MemCpy(pChVarSet->ChangedVarBuffer,VariableBuffer,pChVarSet->ChVarHeader.ChangedVarLength);

	if(!(!!pHiiHandle->RaidChangedVarSet)){
		pHiiHandle->RaidChangedVarSet = pChVarSet;
	}else{
		SMC_CHANGED_VAR_SET*		pTempVarSet	= NULL;

		//Should we examine changed var have the same name condition ? Now, we pass the examine.
		
		for(pTempVarSet = pHiiHandle->RaidChangedVarSet;
			pTempVarSet->pChangedVarNext != NULL;
			pTempVarSet = pTempVarSet->pChangedVarNext);
		pTempVarSet->pChangedVarNext = pChVarSet;
	}

	pProtocol->SmcLsiHaveNvData = TRUE;

	return EFI_SUCCESS;
}


EFI_STATUS SmcLsiRaidOOBLibConstructor(
	EFI_HANDLE ImageHandle,
	EFI_SYSTEM_TABLE *SystemTable
){
		
	return EFI_SUCCESS;
}

