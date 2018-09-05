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

#include "SmcLsiRaidOOB.h"
#include "SmcOobVersion.h"

const CHAR8	CopyRightString[] = "SuperMicro RAID OOB Module, Durant.Lin";

SMC_LSI_RAID_VAR*			LsiVarHashTableName	[VAR_HASH_NUM];
SMC_LSI_RAID_VAR*			LsiVarHashTableVarId[VAR_HASH_NUM];

BOOLEAN						mDetailedDebugMessage = FALSE;

SMC_LSI_INSIDE_DOWN_FUNC		mSmcLsiInsideDownFunc[]		= {
	{ L"InitialLsiVarHashTable"				, InitialLsiVarHashTable 				},
	{ L"SmcLsiRaidOOB_InitialFunc"			, SmcLsiRaidOOB_InitialFunc				},
	{ L"SmcLsiRaidOOB_CollectInformation"	, SmcLsiRaidOOB_CollectInformation		},
	{ L"SmcLsiHookBrowser2Protocol"			, SmcLsiHookBrowser2Protocol			},
	{ L"SmcLsiCallbackAccessMenu"			, SmcLsiCallbackAccessMenu				},
	{ L"SmcLsiRaidOOB_CollectItems"			, SmcLsiRaidOOB_CollectInformation		},
	{ L"SmcLsiSetSmcLsiVariableTable"		, SmcLsiSetSmcLsiVariableTable			},
	{ L"SmcLsiHookBrowser2Protocol"			, SmcLsiHookBrowser2Protocol			},
	{ L""									, NULL									}
};

SMC_LSI_INSIDE_LOAD_FUNC		mSmcLsiInsideLoadFunc[]		= {
	{ L"InsertRaidSetupVariable"			, InsertRaidSetupVariable				},
	{ L"InsertRaidSetupFormGoto"			, InsertRaidSetupFormGoto				},
	{ L"InsertRaidSetupFormItems"			, InsertRaidSetupFormItems				},
	{ L""									, NULL									}
};

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
VOID FreeLsiVarHashTable(){
	
	UINTN	TableIndex = 0;
	SMC_LSI_RAID_VAR*	pTemp = NULL;
	SMC_LSI_RAID_VAR*	pNext = NULL;

	for(TableIndex = 0; TableIndex < VAR_HASH_NUM;++TableIndex){
		pTemp = LsiVarHashTableName[TableIndex];
		while(!!pTemp){
			pNext = pTemp->Lsi_pNameNext;
			gBS->FreePool(pTemp->Lsi_Buffer);
			gBS->FreePool(pTemp);
			pTemp = pNext;
		}
	}
}

EFI_STATUS InitialLsiVarHashTable(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol){
	
	UINTN	TableIndex = 0;
	if(pProtocol == NULL) return EFI_LOAD_ERROR;
	FreeLsiVarHashTable();

	for(TableIndex = 0; TableIndex < VAR_HASH_NUM;++TableIndex){
		LsiVarHashTableName[TableIndex] = NULL;
		LsiVarHashTableVarId[TableIndex] = NULL;
	}
	return EFI_SUCCESS;
}

SMC_LSI_RAID_VAR* AddLsiVarByName(SMC_LSI_RAID_VAR* Var){

	SMC_LSI_RAID_VAR*	pTemp 			= NULL;
	UINTN				VarNameIndex 	= 0;

	VarNameIndex = VarHashByName(Var->Lsi_Name);

	pTemp = LsiVarHashTableName[VarNameIndex];

	if(!(!!pTemp)){
		LsiVarHashTableName[VarNameIndex] = Var;
	}else{
		do {
			if(!(!!pTemp->Lsi_pNameNext)){
				pTemp->Lsi_pNameNext = Var;
				break;
			}
		}while(!!(pTemp = pTemp->Lsi_pNameNext));
	}
	pTemp = Var;

	return pTemp;
}
SMC_LSI_RAID_VAR* AddLsiVarByVId(SMC_LSI_RAID_VAR* Var){

	SMC_LSI_RAID_VAR*	pTemp 			= NULL;
	UINTN				VarIdIndex 		= 0;

	VarIdIndex = VarHashById(Var->Lsi_VarId);

	pTemp = LsiVarHashTableVarId[VarIdIndex];

	if(!(!!pTemp)){
		LsiVarHashTableVarId[VarIdIndex] = Var;
	}else{
		do {
			if(!(!!pTemp->Lsi_pIdNext)){
				pTemp->Lsi_pIdNext = Var;
				break;
			}
		}while(!!(pTemp = pTemp->Lsi_pIdNext));
	}
	pTemp = Var;

	return pTemp;
}
SMC_LSI_RAID_VAR* AddLsiVarToTable(SMC_LSI_RAID_VAR* Var){

	SMC_LSI_RAID_VAR*	pTemp 			= NULL;

	pTemp = AddLsiVarByName(Var);
	pTemp = AddLsiVarByVId(Var);

	return pTemp;
}

SMC_LSI_RAID_VAR* 	SetLsiVarBuffer_byString(SMC_LSI_RAID_VAR* Var,EFI_STRING DataString){

	EFI_STATUS	Status		  = EFI_SUCCESS;
	UINTN		VarDataSize	  = (Var->Lsi_Size + VAR_UNUSED_SPACE);
	EFI_STRING	TempProgress  = NULL;

	if(Var == NULL) return NULL;
	if(DataString == NULL) return NULL;

	if(Var->Lsi_Buffer == NULL)
		gBS->AllocatePool(EfiBootServicesData,VarDataSize,&Var->Lsi_Buffer);

	MemSet(Var->Lsi_Buffer,VarDataSize,0x00);

	Status = GetHiiConfigRoutingProtocol()->ConfigToBlock(
					GetHiiConfigRoutingProtocol(),
					DataString,
					Var->Lsi_Buffer,
					&VarDataSize,
					&TempProgress			
				);
	SMC_RAID_DETAIL_DEBUG((-1,"SetLsiVarBuffer_byString ConfigToBlock Status[%r]\n",Status));
	if(EFI_ERROR(Status)){
		SMC_RAID_DETAIL_DEBUG((-1,"    TempProgress = \n"));
		SMC_RAID_DETAIL_DEBUG((-1,"        %s\n",TempProgress));
		MemSet(Var->Lsi_Buffer,VarDataSize,0x00);
		return NULL;
	}

	return Var;
}
EFI_STATUS	SetLsiVarBuffer(SMC_LSI_RAID_VAR *Var, UINT8*	Buffer){
	EFI_STATUS	Status	= EFI_SUCCESS;
	
	if(Var == NULL) return EFI_INVALID_PARAMETER;
	if(Buffer == NULL) return EFI_INVALID_PARAMETER;

	if(Var->Lsi_Buffer == NULL){
		gBS->AllocatePool(EfiBootServicesData,Var->Lsi_Size,&Var->Lsi_Buffer);
		MemSet(Var->Lsi_Buffer,Var->Lsi_Size,0x00);
	}
	//Value is same.
	if(MemCmp(Var->Lsi_Buffer,Buffer,Var->Lsi_Size) == 0) return EFI_SUCCESS;

	MemCpy(Var->Lsi_Buffer,Buffer,Var->Lsi_Size);
	//We may examine buffer ?
	if(MemCmp(Var->Lsi_Buffer,Buffer,Var->Lsi_Size) == 0) return EFI_SUCCESS;

	return EFI_LOAD_ERROR;

}
UINT8*	GetLsiVarBuffer(SMC_LSI_RAID_VAR *Var){

	if(Var == NULL) return NULL;
	if(Var->Lsi_Buffer == NULL) return NULL;

	return Var->Lsi_Buffer;
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
	SMC_LSI_RAID_VAR	*pTemp = NULL;

	DEBUG((-1,"LsiVarHashTableName :: \n"));
	for(VarIndex = 0;VarIndex < VAR_HASH_NUM; ++VarIndex){
		DEBUG((-1,"  Index[%02x] - \n",VarIndex));
		pTemp = LsiVarHashTableName[VarIndex];
		while(!!pTemp){
			DEBUG((-1,"      Name[%a], Guid[%g], Vid[%x], Size[%x]\n",pTemp->Lsi_Name,pTemp->Lsi_Guid,pTemp->Lsi_VarId,pTemp->Lsi_Size));
			pTemp = pTemp->Lsi_pNameNext;
		}
	}
	DEBUG((-1,"LsiVarHashTableVarId :: \n"));
	for(VarIndex = 0;VarIndex < VAR_HASH_NUM; ++VarIndex){
		DEBUG((-1,"  Index[%02x] - \n",VarIndex));
		pTemp = LsiVarHashTableVarId[VarIndex];
		while(!!pTemp){
			DEBUG((-1,"      Name[%a], Guid[%g], Vid[%x], Size[%x]\n",pTemp->Lsi_Name,pTemp->Lsi_Guid,pTemp->Lsi_VarId,pTemp->Lsi_Size));
			pTemp = pTemp->Lsi_pIdNext;
		}	
	}
#endif
}
SMC_LSI_RAID_VAR*	SearchLsiVarByName(CHAR8*	Name,EFI_GUID*	Guid){
	SMC_LSI_RAID_VAR*	pTemp 		= NULL;
	UINTN				VarIndex 	= 0;
	BOOLEAN				SearchFind  = FALSE;

	VarIndex = VarHashByName(Name);
	pTemp = LsiVarHashTableName[VarIndex];
	while(!!pTemp){
		if(Strcmp(pTemp->Lsi_Name,Name) == 0  && ( Guid == NULL || MemCmp(&pTemp->Lsi_Guid,Guid,sizeof(EFI_GUID)) == 0)){
			SearchFind = TRUE;
			break;
		}
		pTemp = pTemp->Lsi_pNameNext;
	}
	return (!!SearchFind) ? pTemp : NULL;
}
SMC_LSI_RAID_VAR*	SearchLsiVarById(UINT16 VarId){
	SMC_LSI_RAID_VAR*	pTemp 		= NULL;
	UINTN				VarIndex 	= 0;
	BOOLEAN				SearchFind  = FALSE;

	VarIndex = VarHashById(VarId);
	pTemp = LsiVarHashTableVarId[VarIndex];
	while(!!pTemp){
		if(pTemp->Lsi_VarId == VarId){
			SearchFind = TRUE;
			break;
		}
		pTemp = pTemp->Lsi_pIdNext;
	}
	return (!!SearchFind) ? pTemp : NULL;
}
//Must have 
SMC_LSI_RAID_VAR*	AddLsiVarByBuffer(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol,CHAR8*	Name,EFI_GUID*	Guid,EFI_VARSTORE_ID VarId,UINT16 VarSize,UINT8* pBuffer){

	SMC_LSI_RAID_VAR*	pTemp 		= NULL;
	EFI_STATUS			Status 		= EFI_SUCCESS;
	UINTN				VarIndex	= 0;

	pTemp = SearchLsiVarByName(Name,Guid);
	//Maybe we should overwride for memory reused?
	if(pTemp == NULL){
		Status = gBS->AllocatePool(EfiBootServicesData,sizeof(SMC_LSI_RAID_VAR),(VOID**)(&pTemp));
		if(EFI_ERROR(Status)) {
			SettingErrorStatus(pProtocol,0x30,Status);
			return NULL;
		}
		MemSet(pTemp,sizeof(SMC_LSI_RAID_VAR),0x00);
	}

	Strcpy(pTemp->Lsi_Name,Name);
	MemCpy(&pTemp->Lsi_Guid,Guid,sizeof(EFI_GUID));
	pTemp->Lsi_Size 		= VarSize;
	pTemp->Lsi_VarId 		= VarId;
	pTemp->Lsi_pNameNext	= NULL;
	pTemp->Lsi_pIdNext		= NULL;
	pTemp->Lsi_DrvHandle	= pProtocol->SmcLsiCurrDriverHandle;
	pTemp->BeUsed			= FALSE;
	pTemp->SmcLsiVId		= 0;

	if(EFI_ERROR(SetLsiVarBuffer(pTemp,pBuffer))) {
		SettingErrorStatus(pProtocol,0x32,Status);
		return NULL;
	}
	AddLsiVarToTable(pTemp);

	return pTemp;
}	

SMC_LSI_RAID_VAR*	AddLsiVar(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol,CHAR8*	Name,EFI_GUID*	Guid,EFI_VARSTORE_ID VarId,UINT16 VarSize){

	SMC_LSI_RAID_VAR*	pTemp = NULL;
	EFI_STATUS			Status = EFI_SUCCESS;
  	EFI_STRING			ConfigHdrTemp = NULL;
  	EFI_STRING			ExTractConfigHdr = NULL;

  	EFI_STRING			TempProgress = NULL;
  	EFI_STRING			TempResult = NULL;
	
	UINTN				VarIndex			= 0;

	static CHAR16		UVarName[NAME_LENGTH];

	MemSet(UVarName,NAME_LENGTH * sizeof(CHAR16),0x00);
	DEBUG((-1,"Name[%a], Guid[%g], VarId[%x], VarSize[%x] :: ",Name,*Guid,VarId,VarSize));

	pTemp = SearchLsiVarByName(Name,Guid);
	//Maybe we should overwride for memory reused?
	if(pTemp == NULL){
		Status = gBS->AllocatePool(EfiBootServicesData,sizeof(SMC_LSI_RAID_VAR),(VOID**)(&pTemp));
		if(EFI_ERROR(Status)) {
			SettingErrorStatus(pProtocol,0x30,Status);
			return NULL;
		}
		MemSet(pTemp,sizeof(SMC_LSI_RAID_VAR),0x00);
	}

	Strcpy(pTemp->Lsi_Name,Name);
	MemCpy(&pTemp->Lsi_Guid,Guid,sizeof(EFI_GUID));
	pTemp->Lsi_Size 		= VarSize;
	pTemp->Lsi_VarId 		= VarId;
	pTemp->Lsi_pNameNext	= NULL;
	pTemp->Lsi_pIdNext		= NULL;
	pTemp->Lsi_DrvHandle	= pProtocol->SmcLsiCurrDriverHandle;
	pTemp->BeUsed			= FALSE;
	pTemp->SmcLsiVId		= 0;

	AsciiStrToUnicodeStr(Name,UVarName);
	
	ConfigHdrTemp = HiiConstructConfigHdr(Guid,UVarName,pTemp->Lsi_DrvHandle);
	ExTractConfigHdr = AppendOffsetWidth(ConfigHdrTemp,0,VarSize);

//	DEBUG((-1,"ExTractConfigHdr = %s\n",ExTractConfigHdr));

	Status = pProtocol->SmcLsiCurrConfigAccess->ExtractConfig(pProtocol->SmcLsiCurrConfigAccess,ExTractConfigHdr,&TempProgress,&TempResult);

	DEBUG((-1,"ExtractConfig Status[%r]\n",Status));
	if(EFI_ERROR(Status)) {
		SettingErrorStatus(pProtocol,0x31,Status);
		return NULL;
	}
//	DEBUG((-1,"TempResult = %s\n",TempResult));

	if(SetLsiVarBuffer_byString(pTemp,TempResult) == NULL) {
		SettingErrorStatus(pProtocol,0x32,Status);
		return NULL;
	}
	AddLsiVarToTable(pTemp);

	return pTemp;
}	
// ======== LSI VAR END   ========

EFI_STATUS	RaidDownTheLastCleanFunc(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol){

	UINTN					index = 0;
	UINTN					DefaultIndex = 0;
	SMC_LSI_RAID_FORM*		LocSmcLsiRaidFormRefSearchTable	= NULL;

	LocSmcLsiRaidFormRefSearchTable = pProtocol->SmcLsiRaidFormRefSearchTable;
	for(DefaultIndex=0;
		LocSmcLsiRaidFormRefSearchTable[DefaultIndex].Lsi_RaidTypeIndex != RAID_NULL && 
		LocSmcLsiRaidFormRefSearchTable[DefaultIndex].Lsi_OpCode != 0xFF;
		DefaultIndex++);

	for(index=0;
		LocSmcLsiRaidFormRefSearchTable[index].Lsi_RaidTypeIndex != RAID_NULL && 
		LocSmcLsiRaidFormRefSearchTable[index].Lsi_OpCode != 0xFF;
		index++){
		
		if(pProtocol->SmcLsiCurrHiiHandleTable->RaidCardType == LocSmcLsiRaidFormRefSearchTable[index].Lsi_RaidTypeIndex){
			LocSmcLsiRaidFormRefSearchTable[index].Lsi_FormId 	= LocSmcLsiRaidFormRefSearchTable[DefaultIndex].Lsi_FormId;
			LocSmcLsiRaidFormRefSearchTable[index].Lsi_QId  	= LocSmcLsiRaidFormRefSearchTable[DefaultIndex].Lsi_QId;
			LocSmcLsiRaidFormRefSearchTable[index].Lsi_VId  	= LocSmcLsiRaidFormRefSearchTable[DefaultIndex].Lsi_VId;
			LocSmcLsiRaidFormRefSearchTable[index].Lsi_Voff 	= LocSmcLsiRaidFormRefSearchTable[DefaultIndex].Lsi_Voff;
			LocSmcLsiRaidFormRefSearchTable[index].Lsi_Flags	= LocSmcLsiRaidFormRefSearchTable[DefaultIndex].Lsi_Flags;
			LocSmcLsiRaidFormRefSearchTable[index].BeUsed		= LocSmcLsiRaidFormRefSearchTable[DefaultIndex].BeUsed;	
		}
	}

	return EFI_SUCCESS;
}

EFI_STATUS SetLsiRaidFormInformTable(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol,EFI_IFR_REF*	LsiIfrRef){
	UINTN				index = 0;
	EFI_STRING			FormString = NULL;
	EFI_HII_HANDLE		LsiHiiHandle = NULL;
	SMC_LSI_RAID_FORM*	LocSmcLsiRaidFormRefSearchTable = NULL;	

	LsiHiiHandle = pProtocol->SmcLsiCurrHiiHandleTable->RaidCardHiiHandle;

	FormString = GetHiiString(LsiHiiHandle,LsiIfrRef->Question.Header.Prompt);
	if(! (!!FormString)){
		return EFI_OUT_OF_RESOURCES;
	}

	LocSmcLsiRaidFormRefSearchTable = pProtocol->SmcLsiRaidFormRefSearchTable;

	for(index=0;LocSmcLsiRaidFormRefSearchTable[index].Lsi_RaidTypeIndex != RAID_NULL; ++index){

		if(pProtocol->SmcLsiCurrHiiHandleTable->RaidCardType == LocSmcLsiRaidFormRefSearchTable[index].Lsi_RaidTypeIndex &&
		   StrCmp(FormString,LocSmcLsiRaidFormRefSearchTable[index].Lsi_Name) == 0 &&
		   ! (!!LocSmcLsiRaidFormRefSearchTable[index].BeUsed)
		  ){	
			//if(LocSmcLsiRaidFormRefSearchTable[index].Lsi_FormId != 0xFFFF) break;
			DEBUG((-1,"  Update :: RaidType[%x], FormName[%s], FormId[%x], Qid[%x], Vid[%x], VOff[%x], Flags[%x]\n",
										LocSmcLsiRaidFormRefSearchTable[index].Lsi_RaidTypeIndex,FormString,
										LsiIfrRef->FormId,LsiIfrRef->Question.QuestionId,
										LsiIfrRef->Question.VarStoreId,LsiIfrRef->Question.VarStoreInfo.VarOffset,
										LsiIfrRef->Question.Flags));

			LocSmcLsiRaidFormRefSearchTable[index].Lsi_FormId 	= LsiIfrRef->FormId;
			LocSmcLsiRaidFormRefSearchTable[index].Lsi_QId  	= LsiIfrRef->Question.QuestionId;
			LocSmcLsiRaidFormRefSearchTable[index].Lsi_VId  	= LsiIfrRef->Question.VarStoreId;
			LocSmcLsiRaidFormRefSearchTable[index].Lsi_Voff 	= LsiIfrRef->Question.VarStoreInfo.VarOffset;
			LocSmcLsiRaidFormRefSearchTable[index].Lsi_Flags	= LsiIfrRef->Question.Flags;
			LocSmcLsiRaidFormRefSearchTable[index].BeUsed		= TRUE;
		}
	}
	return EFI_SUCCESS;
}

EFI_STATUS SmcLsiRaidOOB_InitialFunc(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol){

	EFI_STATUS			Status = EFI_SUCCESS;
	EFI_HII_HANDLE		LsiHiiHandle	= NULL;

	if(pProtocol == NULL) return EFI_LOAD_ERROR;
	mDetailedDebugMessage = pProtocol->DetailedDebugMessage;

	LsiHiiHandle = pProtocol->SmcLsiCurrHiiHandleTable->RaidCardHiiHandle;

	if(LsiHiiHandle == NULL) return SettingErrorStatus(pProtocol,0x01,Status);

	pProtocol->SmcLsiCurrDriverHandle = NULL;
  	Status = GetHiiDataBase()->GetPackageListHandle(GetHiiDataBase(),LsiHiiHandle,&pProtocol->SmcLsiCurrDriverHandle);
	if(EFI_ERROR(Status)) return SettingErrorStatus(pProtocol,0x02,Status);

	pProtocol->SmcLsiCurrConfigAccess = NULL;
  	Status = gBS->HandleProtocol(pProtocol->SmcLsiCurrDriverHandle,&gEfiHiiConfigAccessProtocolGuid,&pProtocol->SmcLsiCurrConfigAccess);
	if(EFI_ERROR(Status)) return SettingErrorStatus(pProtocol,0x03,Status);

	pProtocol->HiiFlags	= 0;

	return EFI_SUCCESS;
}

VOID SmcLsiRaidOOB_SendFormCallBack(){

}

VOID Debug_for_RaidItems(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol){

#if defined(DEBUG_MODE) && (DEBUG_MODE == 1)
	UINTN				ItemIndex = 0;
	SMC_LSI_ITEMS*	   	LocSmcLsiRaidItemsTable	= NULL;

	LocSmcLsiRaidItemsTable = pProtocol->SmcLsiRaidItemsTable;

	DEBUG((-1,"SmcLsiRaidItemsTable :: \n"));
	for(ItemIndex = 0;LocSmcLsiRaidItemsTable[ItemIndex].LsiRaidTypeIndex != RAID_NULL; ++ItemIndex){
		SMC_LSI_RAID_ITEM_SET*	pItemSet = NULL;

		DEBUG((-1,"  ItemIndex[%x]Type[%x]Form[%s]ItemName[%s]Find[%s] - \n",
				   ItemIndex,
				   LocSmcLsiRaidItemsTable[ItemIndex].LsiRaidTypeIndex,
				   LocSmcLsiRaidItemsTable[ItemIndex].LsiItemForm,
				   LocSmcLsiRaidItemsTable[ItemIndex].LsiItemName,
				   LocSmcLsiRaidItemsTable[ItemIndex].pLsiItemSet != NULL ? L"TRUE" : L"FALSE"
			 ));

		for(pItemSet = LocSmcLsiRaidItemsTable[ItemIndex].pLsiItemSet;pItemSet != NULL;pItemSet = pItemSet->pNext){
			DEBUG((-1,"    CardIndex[%x], VarId[%x], HII[%s]\n",
				 	   pItemSet->LsiRaidCardIndex,
					   pItemSet->SmcLsiVarId,
					   pItemSet->pLsiItemOp != NULL ? L"HAVE" : L"NON"
				 ));
		}
	}
#endif
}

EFI_STATUS SmcLsiRaidOOB_CollectInformation_Form(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol,EFI_HII_PACKAGE_HEADER*	LsiIfrPackagePtr){
#define TEMP_FORM_STRING	512

	EFI_STATUS		   Status = EFI_SUCCESS;
	EFI_IFR_OP_HEADER* LsipEFI_IFR_OP_HEADER 	= NULL;
	UINT32			   ItemIndex			 	= 0;
	SMC_LSI_ITEMS*	   LocSmcLsiRaidItemsTable	= NULL;
	EFI_HII_HANDLE	   LsiHiiHandle				= NULL;
	CHAR16			   CurrentFormString[TEMP_FORM_STRING];

	LsiHiiHandle =	pProtocol->SmcLsiCurrHiiHandleTable->RaidCardHiiHandle;
	LocSmcLsiRaidItemsTable = pProtocol->SmcLsiRaidItemsTable;
		
	for(LsipEFI_IFR_OP_HEADER = (EFI_IFR_OP_HEADER*)(LsiIfrPackagePtr+1);
		LsipEFI_IFR_OP_HEADER < (EFI_IFR_OP_HEADER*)((UINT8*)LsiIfrPackagePtr + LsiIfrPackagePtr->Length);
		LsipEFI_IFR_OP_HEADER = (EFI_IFR_OP_HEADER*)((UINT8*)LsipEFI_IFR_OP_HEADER + LsipEFI_IFR_OP_HEADER->Length)){
	
		if(pProtocol->HiiFlags == 0){
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
					SMC_LSI_RAID_VAR*	TempVar   = NULL;
					TempVar = AddLsiVar(pProtocol,LsiIfrVar->Name,&LsiIfrVar->Guid,LsiIfrVar->VarStoreId,LsiIfrVar->Size);
					if(TempVar == NULL) return SettingErrorStatus(pProtocol,0x20,Status);
				}
					break;
				default:
					break;
			}
		}else {
			switch(LsipEFI_IFR_OP_HEADER->OpCode){
				case EFI_IFR_FORM_OP:
				{
					EFI_IFR_FORM*	pFormOp = (EFI_IFR_FORM*)LsipEFI_IFR_OP_HEADER;
					EFI_STRING		TempString = NULL;

					MemSet(CurrentFormString,TEMP_FORM_STRING * sizeof(CHAR16),0x00);
					TempString = GetHiiString(LsiHiiHandle,pFormOp->FormTitle);
					StrCpy(CurrentFormString,TempString);
				}
					break;
				case EFI_IFR_STRING_OP:
				case EFI_IFR_ONE_OF_OP:
				case EFI_IFR_NUMERIC_OP:
				{
					CHAR16*	IfrString = NULL;
					SMC_LSI_ITEMS_COMMON_HEADER*	ItemsComnHead = NULL;

					ItemsComnHead = (SMC_LSI_ITEMS_COMMON_HEADER*)LsipEFI_IFR_OP_HEADER;
					IfrString = GetHiiString(LsiHiiHandle,ItemsComnHead->Question.Header.Prompt);
					if(! (!!IfrString)){
						DEBUG((-1,"StringId[%x], cannot get string\n",ItemsComnHead->Question.Header.Prompt));
						break;
					}
					SMC_RAID_DETAIL_DEBUG((-1,"IfrString[%s], VarStoreId[%x] OP[%x] >> ",IfrString,ItemsComnHead->Question.VarStoreId,LsipEFI_IFR_OP_HEADER->OpCode));

					for(ItemIndex=0;LocSmcLsiRaidItemsTable[ItemIndex].LsiRaidTypeIndex != RAID_NULL;++ItemIndex){
						SMC_LSI_RAID_ITEM_SET*	pItemSet = NULL;
					
						if(LocSmcLsiRaidItemsTable[ItemIndex].LsiRaidTypeIndex != pProtocol->SmcLsiCurrHiiHandleTable->RaidCardType) continue;
						if( !!StrCmp(LocSmcLsiRaidItemsTable[ItemIndex].LsiItemForm,CurrentFormString)) continue;
						if( !!StrCmp(LocSmcLsiRaidItemsTable[ItemIndex].LsiItemName,IfrString)) continue;
						
						gBS->AllocatePool(EfiBootServicesData,sizeof(SMC_LSI_RAID_ITEM_SET),&pItemSet);
						MemSet(pItemSet,sizeof(SMC_LSI_RAID_ITEM_SET),0x00);
						
						pItemSet->LsiRaidCardIndex = pProtocol->SmcLsiCurrHiiHandleTable->RaidCardIndex;
						pItemSet->pLsiItemOp 	   = LsipEFI_IFR_OP_HEADER;
						pItemSet->SmcLsiVarId	   = 0x0;
						pItemSet->pNext		 	   = NULL;
						
						if(! (!!LocSmcLsiRaidItemsTable[ItemIndex].pLsiItemSet)){
							LocSmcLsiRaidItemsTable[ItemIndex].pLsiItemSet = pItemSet;
						}else{
							SMC_LSI_RAID_ITEM_SET*	pTempSet = NULL;
							for(pTempSet = LocSmcLsiRaidItemsTable[ItemIndex].pLsiItemSet;
								pTempSet->pNext != NULL;
								pTempSet = pTempSet->pNext);
							pTempSet = pItemSet;
						}
					}
					SMC_RAID_DETAIL_DEBUG((-1,"\n"));
				}
					break;
				default:
					break;
			}
		}
	}
	
	return Status;
}

EFI_STATUS SmcLsiRaidOOB_CollectInformation(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol){
	EFI_STATUS		Status			= EFI_SUCCESS;
  	UINTN			LsiBufferSize 	= 0;
	EFI_HII_HANDLE	LsiHiiHandle	= NULL;

	EFI_HII_PACKAGE_HEADER 		*LsiIfrPackagePtr 		= NULL;
	EFI_HII_PACKAGE_LIST_HEADER *LsiHiiPackageList 		= NULL;

	LsiHiiHandle = pProtocol->SmcLsiCurrHiiHandleTable->RaidCardHiiHandle;
	
	LsiHiiPackageList = GetHiiPackageList(LsiHiiHandle);
	if(! (!!LsiHiiPackageList)) return SettingErrorStatus(pProtocol,0x01,EFI_NOT_FOUND);

  	for(LsiIfrPackagePtr = (EFI_HII_PACKAGE_HEADER*)(LsiHiiPackageList+1);
	  	LsiIfrPackagePtr < (EFI_HII_PACKAGE_HEADER*)(((UINT8*)LsiHiiPackageList) + LsiHiiPackageList->PackageLength);
	 	LsiIfrPackagePtr = (EFI_HII_PACKAGE_HEADER*)((UINT8*)LsiIfrPackagePtr + LsiIfrPackagePtr->Length)){
 
	  	if(LsiIfrPackagePtr->Type == EFI_HII_PACKAGE_FORMS){
			Status = SmcLsiRaidOOB_CollectInformation_Form(pProtocol,LsiIfrPackagePtr);
  			if(EFI_ERROR(Status)) return SettingErrorStatus(pProtocol,0x02,Status);
		}
  	}
	
	if(pProtocol->HiiFlags == 0){
		Debug_for_LsiVarTable(pProtocol);
		//Below line may cause debug assert?
		gBS->FreePool(LsiHiiPackageList);
	}else if(pProtocol->HiiFlags == 1){
		Debug_for_RaidItems(pProtocol);	
	}
	pProtocol->HiiFlags++;
	return EFI_SUCCESS;
}
VOID Debug_for_SmcLsiVarTable(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol){

#if defined(DEBUG_MODE) && (DEBUG_MODE == 1)
	UINTN						VarIndex			= 0;
	SMC_LSI_VAR*				LocSmcLsiVarTable	= NULL;

	LocSmcLsiVarTable = pProtocol->SmcLsiVarTable;

	DEBUG((-1,"SmcLsiVarTable :: \n"));
	for(VarIndex = 0;LocSmcLsiVarTable[VarIndex].LsiRaidTypeIndex != RAID_NULL; ++VarIndex){
		SMC_LSI_VAR_SET* pVarSet = NULL;
		DEBUG((-1,"  VarIndex[%x]Type[%x] - \n",VarIndex,LocSmcLsiVarTable[VarIndex].LsiRaidTypeIndex));

		for(pVarSet = LocSmcLsiVarTable[VarIndex].pVarSet;pVarSet != NULL;pVarSet = pVarSet->pNext){
			DEBUG((-1,"    CardIndex[%x], Name[%a], Guid[%g], VarId[%x], VarSize[%x]\n",
					  pVarSet->LsiRaidCardIndex,
					  pVarSet->SmcLsiVarName,
					  pVarSet->SmcLsiVarGuid,
					  pVarSet->SmcLsiVarId,
					  pVarSet->SmcLsiVarSize
				  ));
		}
	}
#endif
}

EFI_STATUS	SmcLsiSetSmcLsiVariableTable(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol){
	
	EFI_STATUS						Status 					= EFI_SUCCESS;
	SMC_LSI_VAR*					LocSmcLsiVarTable		= NULL;
	SMC_LSI_ITEMS*					LocSmcLsiRaidItemsTable	= NULL;
	UINTN							VarIndex				= 0;
	UINTN							ItemIndex				= 0;
	SMC_LSI_RAID_ITEM_SET*			pItemSet				= NULL;
	SMC_LSI_RAID_VAR*				pVar					= NULL;
	SMC_LSI_VAR_SET*				pVarSet					= NULL;
	SMC_LSI_ITEMS_COMMON_HEADER*	pCmnHeader				= NULL;

	LocSmcLsiVarTable 		= pProtocol->SmcLsiVarTable;
	LocSmcLsiRaidItemsTable	= pProtocol->SmcLsiRaidItemsTable;

	for(VarIndex=0;LocSmcLsiVarTable[VarIndex].LsiRaidTypeIndex != RAID_NULL;++VarIndex){
		if(pProtocol->SmcLsiCurrHiiHandleTable->RaidCardType == LocSmcLsiVarTable[VarIndex].LsiRaidTypeIndex){
			break;
		}
	}
	if(LocSmcLsiVarTable[VarIndex].LsiRaidTypeIndex == RAID_NULL){
		SMC_RAID_DETAIL_DEBUG((-1," Raid Type[%x] doesn't define in SmcLisVarTable!\n",pProtocol->SmcLsiCurrHiiHandleTable->RaidCardType));
		return SettingErrorStatus(pProtocol,0x01,EFI_LOAD_ERROR);
	}

	for(ItemIndex;LocSmcLsiRaidItemsTable[ItemIndex].LsiRaidTypeIndex != RAID_NULL; ++ItemIndex){
		if(pProtocol->SmcLsiCurrHiiHandleTable->RaidCardType  != LocSmcLsiRaidItemsTable[ItemIndex].LsiRaidTypeIndex) continue;
		for(pItemSet = LocSmcLsiRaidItemsTable[ItemIndex].pLsiItemSet;pItemSet != NULL;	pItemSet = pItemSet->pNext){

			if(pProtocol->SmcLsiCurrHiiHandleTable->RaidCardIndex != pItemSet->LsiRaidCardIndex) continue;

			pCmnHeader = (SMC_LSI_ITEMS_COMMON_HEADER*)pItemSet->pLsiItemOp;
			pVar = SearchLsiVarById(pCmnHeader->Question.VarStoreId);
			if(! (!!pVar->BeUsed)) {
				Status = gBS->AllocatePool(EfiBootServicesData,sizeof(SMC_LSI_VAR_SET),&pVarSet);
				if(EFI_ERROR(Status)) return SettingErrorStatus(pProtocol,0x02,Status);
				MemSet(pVarSet,sizeof(SMC_LSI_VAR_SET),0x00);

				gBS->AllocatePool(EfiBootServicesData,pVar->Lsi_Size,&pVarSet->SmcLsiVarBuffer);
				if(EFI_ERROR(Status)) return SettingErrorStatus(pProtocol,0x03,Status);
				MemSet(pVarSet->SmcLsiVarBuffer,pVar->Lsi_Size,0x00);
				MemCpy(pVarSet->SmcLsiVarBuffer,pVar->Lsi_Buffer,pVar->Lsi_Size);

				MemCpy(&pVarSet->SmcLsiVarGuid,pProtocol->SmcLsiGetVarGuid(pProtocol),sizeof(EFI_GUID));
				pVarSet->SmcLsiVarSize		= pVar->Lsi_Size;
				pVarSet->LsiRaidCardIndex 	= pItemSet->LsiRaidCardIndex;
				pVarSet->SmcLsiVarId	  	= pProtocol->SmcLsiGetVIdStart(pProtocol);
				pVarSet->pNext				= NULL;
				//Format VariableName_RaidType_RaidCardIndex
				Sprintf(pVarSet->SmcLsiVarName,"SmcR_%s_%x_%x",
						pVar->Lsi_Name,
						LocSmcLsiVarTable[VarIndex].LsiRaidTypeIndex,
						pVarSet->LsiRaidCardIndex 
						);
				if(LocSmcLsiVarTable[VarIndex].pVarSet == NULL){
					LocSmcLsiVarTable[VarIndex].pVarSet = pVarSet;
				}else{
					SMC_LSI_VAR_SET* pTempVarSet = NULL;
					for(pTempVarSet = LocSmcLsiVarTable[VarIndex].pVarSet;
						pTempVarSet->pNext != NULL;
						pTempVarSet = pTempVarSet->pNext);
				
					pTempVarSet->pNext = pVarSet;
				}
				pVar->BeUsed 	= TRUE;
				pVar->SmcLsiVId	= pVarSet->SmcLsiVarId;
				if(!!pProtocol->DetailedDebugMessage){
					DEBUG((-1,"Smc Raid Var Name :: [%a]\n",pVarSet->SmcLsiVarName));
					DEBUG_PRINT_BUFFER(pVarSet->SmcLsiVarBuffer,pVarSet->SmcLsiVarSize);
				}
			}
			pItemSet->SmcLsiVarId		= pVar->SmcLsiVId;
		}
	}
	Debug_for_SmcLsiVarTable(pProtocol);
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

	SMC_LSI_RAID_VAR*	LsiVar 				= NULL;
	EFI_STRING			ConfigHdrTemp 		= NULL;
	EFI_STRING			ConfigBufferString	= NULL;
	EFI_STRING			Progress 			= NULL;

	SMC_RAID_DETAIL_DEBUG((-1,"SmcLsiHookBrower2Callback :: \n"));
	SMC_RAID_DETAIL_DEBUG((-1,"    VariableName[%s], VariableGuid[%g], RetrieveData[%x]\n",
				VariableName,*VariableGuid,RetrieveData));
	if(ResultsDataSize != NULL)
		SMC_RAID_DETAIL_DEBUG((-1,"    ResultsDataSize[%x]\n",*ResultsDataSize));
/*
	if(! (!!RetrieveData))
		DEBUG((-1,"From Driver Handle ResultsData = \n %s\n",ResultsData));
*/
	if(VariableGuid == NULL || VariableName == NULL) return EFI_LOAD_ERROR;

	if(ResultsDataSize == NULL || (*ResultsDataSize && ResultsData == NULL))
		return EFI_INVALID_PARAMETER;

	if(RetrieveData == 0){
		MemSet(aVariableName,NAME_LENGTH * sizeof(CHAR8),0x00);
		UnicodeStrToAsciiStr(VariableName,aVariableName);

		LsiVar = SearchLsiVarByName(aVariableName,VariableGuid);

		if(LsiVar == NULL) return EFI_OUT_OF_RESOURCES;
		SMC_RAID_DETAIL_DEBUG((-1,"    Lsi_Name[%a], Lsi_VarId[%x], Lsi_Guid[%g], Lsi_Size[%x]\n",
			   		LsiVar->Lsi_Name,LsiVar->Lsi_VarId,LsiVar->Lsi_Guid,LsiVar->Lsi_Size));

		//GUID=xxxxx&NAME=xxxxx&PATH=xxxxx & OFFSET=xxxxx&WIDTH=xxxx
		ConfigHdrTemp = HiiConstructConfigHdr(&LsiVar->Lsi_Guid,VariableName,LsiVar->Lsi_DrvHandle);
		if(! (!!ConfigHdrTemp)) return EFI_OUT_OF_RESOURCES;

		gBS->AllocatePool(EfiBootServicesData,(StrLen(ConfigHdrTemp) + 1 + StrLen(ResultsData) + 2) * sizeof(CHAR16),&ConfigBufferString);
		if(! (!!ConfigBufferString)) return EFI_OUT_OF_RESOURCES;
		
		StrCpy(ConfigBufferString,ConfigHdrTemp);
		StrCat(ConfigBufferString,L"&");
		StrCat(ConfigBufferString,ResultsData);

//		DEBUG((-1,"Set Data to Var Buffer = \n %s\n",ConfigBufferString));

		SetLsiVarBuffer_byString(LsiVar,ConfigBufferString);
		gBS->FreePool(ConfigHdrTemp);
		gBS->FreePool(ConfigBufferString);

	}else{
		//Temporary return Out of Resource
		SMC_RAID_DETAIL_DEBUG((-1,"    Sent Buffer Back\n"));
		return EFI_OUT_OF_RESOURCES;
	}

	return EFI_SUCCESS;
}

EFI_STATUS	SmcLsiCallbackAccessMenu(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol){
	EFI_STATUS					Status 		= EFI_SUCCESS;
	UINTN						FormIndex	= 0;
	EFI_IFR_TYPE_VALUE			TypeValue;
	EFI_BROWSER_ACTION_REQUEST	ActionReq 	= 0;
	UINT8						IfrType		= 0;

	SMC_LSI_RAID_FORM*			LocSmcLsiRaidFormRefSearchTable = NULL;

	LocSmcLsiRaidFormRefSearchTable = pProtocol->SmcLsiRaidFormRefSearchTable;

	for(FormIndex = 0;LocSmcLsiRaidFormRefSearchTable[FormIndex].Lsi_RaidTypeIndex != RAID_NULL;++FormIndex){
		
		if(pProtocol->SmcLsiCurrHiiHandleTable->RaidCardType != LocSmcLsiRaidFormRefSearchTable[FormIndex].Lsi_RaidTypeIndex)
			continue;

		MemSet(&TypeValue,sizeof(EFI_IFR_TYPE_VALUE),0x00);
		IfrType	= EFI_IFR_TYPE_REF;
		
		DEBUG((-1,"Request CallBack FormIndex[%x] :: RaidType[%x], Name[%s], OpCode[%x], FormId[%x], QId[%x], VId[%x], Voff[%x], Flags[%x]\n",
					FormIndex, LocSmcLsiRaidFormRefSearchTable[FormIndex].Lsi_RaidTypeIndex,
					LocSmcLsiRaidFormRefSearchTable[FormIndex].Lsi_Name,
					LocSmcLsiRaidFormRefSearchTable[FormIndex].Lsi_OpCode,
					LocSmcLsiRaidFormRefSearchTable[FormIndex].Lsi_FormId,
					LocSmcLsiRaidFormRefSearchTable[FormIndex].Lsi_QId,
					LocSmcLsiRaidFormRefSearchTable[FormIndex].Lsi_VId,
					LocSmcLsiRaidFormRefSearchTable[FormIndex].Lsi_Voff,
					LocSmcLsiRaidFormRefSearchTable[FormIndex].Lsi_Flags
				));

		Status = pProtocol->SmcLsiCurrConfigAccess->Callback(
					pProtocol->SmcLsiCurrConfigAccess,
					EFI_BROWSER_ACTION_CHANGING,
					LocSmcLsiRaidFormRefSearchTable[FormIndex].Lsi_QId,
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

	if(Browser2 == NULL){
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
	
	if(! (!!pItems)){
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
							if(pIfrGuidLabel->ExtendOpCode == EFI_IFR_EXTEND_OP_LABEL && Label == pIfrGuidLabel->Number){
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

EFI_STATUS InsertRaidSetupFormItems(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol){

	MACRO_CAULBUFFER_ARGU(PACKAGE_BUFFER);

	UINTN				ItemIndex			= 0;

	EFI_IFR_OP_HEADER*	pFormOp				= NULL;
	SMC_LSI_ITEMS_MEM*	FormItemsBuffer		= NULL;
	SMC_LSI_ITEMS*		RaidItemsTable		= NULL;
	EFI_HII_HANDLE		SmcLsiSetupHandle	= NULL;

	EFI_IFR_FORM	FormOp;
	EFI_IFR_END		EndOp;

	pFormOp = (EFI_IFR_OP_HEADER*)SearchOpCodeInFormData(pProtocol,EFI_IFR_FORM_OP,pProtocol->SmcLsiGetFormLabel(pProtocol));	
	if(pFormOp == NULL) return SettingErrorStatus(pProtocol,0x02,EFI_LOAD_ERROR);
	
	//FormOp --- EndOp
	InsertStart 		= (UINT8*)pFormOp;
	MACRO_CAULBUFFER_INITIAL(PACKAGE_BUFFER);

	SmcLsiSetupHandle 	= pProtocol->SmcLsiGetHiiHandle(pProtocol);
	RaidItemsTable		= pProtocol->SmcLsiRaidItemsTable;
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

	FormItemsBuffer->ItemsCopyBuffer = (UINT8*)&FormOp;
	FormItemsBuffer->ItemsCopyBufferSize = FormOp.Header.Length;
	FormItemsBuffer = CopyAndExtMem(FormItemsBuffer);

	for(ItemIndex = 0;RaidItemsTable[ItemIndex].LsiRaidTypeIndex != RAID_NULL;++ItemIndex){
	
		SMC_LSI_RAID_ITEM_SET*	pItemSet = NULL;
		if(RaidItemsTable[ItemIndex].LsiRaidTypeIndex != pProtocol->SmcLsiCurrHiiHandleTable->RaidCardType) continue;
		for(pItemSet = RaidItemsTable[ItemIndex].pLsiItemSet;pItemSet != NULL; pItemSet = pItemSet->pNext){
			
			EFI_IFR_OP_HEADER*	OpHeader = NULL;
			if(pItemSet->LsiRaidCardIndex != pProtocol->SmcLsiCurrHiiHandleTable->RaidCardIndex) continue;
	
			OpHeader = pItemSet->pLsiItemOp;
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
					CopyString.Question.Header.Prompt = NewHiiString(SmcLsiSetupHandle,RaidItemsTable[ItemIndex].LsiItemName);
					//Temporary use this.
					CopyString.Question.Header.Help   = NewHiiString(SmcLsiSetupHandle,RaidItemsTable[ItemIndex].LsiItemName);

					CopyString.Question.QuestionId	  = pProtocol->SmcLsiGetQIdStart(pProtocol);
					CopyString.Question.VarStoreId	  = pItemSet->SmcLsiVarId;

					CopyString.Question.VarStoreInfo.VarOffset = OriString->Question.VarStoreInfo.VarOffset;
					
					FormItemsBuffer->ItemsCopyBuffer 		= (UINT8*)&CopyString;
					FormItemsBuffer->ItemsCopyBufferSize	= CopyString.Header.Length;
					FormItemsBuffer = CopyAndExtMem(FormItemsBuffer);
				}
					break;
				case EFI_IFR_ONE_OF_OP:
				{
					EFI_IFR_ONE_OF*		OriOneOf;
					EFI_IFR_ONE_OF		CopyOneOf;
	
					EFI_IFR_ONE_OF_OPTION* OriOneOfOption;
					EFI_IFR_ONE_OF_OPTION  CopyOneOfOption;

					OriOneOf = (EFI_IFR_ONE_OF*)OpHeader;
					CopyOneOf.Header.OpCode = EFI_IFR_ONE_OF_OP;
					CopyOneOf.Header.Length = sizeof(EFI_IFR_ONE_OF);
					CopyOneOf.Header.Scope  = OriOneOf->Header.Scope;
					
					CopyOneOf.Question.Header.Prompt = NewHiiString(SmcLsiSetupHandle,RaidItemsTable[ItemIndex].LsiItemName);
					CopyOneOf.Question.Header.Help   = NewHiiString(SmcLsiSetupHandle,RaidItemsTable[ItemIndex].LsiItemName);

					CopyOneOf.Question.QuestionId	 = pProtocol->SmcLsiGetQIdStart(pProtocol);
					CopyOneOf.Question.VarStoreId	 = pItemSet->SmcLsiVarId;
					CopyOneOf.Question.VarStoreInfo.VarOffset	= OriOneOf->Question.VarStoreInfo.VarOffset;
					CopyOneOf.Question.Flags		 = OriOneOf->Question.Flags;
					CopyOneOf.Flags					 = OriOneOf->Flags;

					MemCpy(&CopyOneOf.data,&OriOneOf->data,sizeof(MINMAXSTEP_DATA));
					switch((CopyOneOf.Flags & EFI_IFR_NUMERIC_SIZE)){
						case EFI_IFR_NUMERIC_SIZE_1:
							CopyOneOf.data.u8.Step = 1;
							break;
						case EFI_IFR_NUMERIC_SIZE_2:
							CopyOneOf.data.u16.Step = 1;
							break;
						case EFI_IFR_NUMERIC_SIZE_4:
							CopyOneOf.data.u32.Step = 1;
							break;
						case EFI_IFR_NUMERIC_SIZE_8:
							CopyOneOf.data.u64.Step = 1;
							break;
					}

					FormItemsBuffer->ItemsCopyBuffer 		= (UINT8*)&CopyOneOf;
					FormItemsBuffer->ItemsCopyBufferSize	= CopyOneOf.Header.Length;
					FormItemsBuffer = CopyAndExtMem(FormItemsBuffer);

					if(!!OriOneOf->Header.Scope){
						
						for(OriOneOfOption = (EFI_IFR_ONE_OF_OPTION*)((UINT8*)OriOneOf + OriOneOf->Header.Length);
							OriOneOfOption->Header.OpCode != EFI_IFR_END_OP;
							OriOneOfOption = (EFI_IFR_ONE_OF_OPTION*)((UINT8*)OriOneOfOption + OriOneOfOption->Header.Length)){

							CHAR16*	OptionString = NULL;
							MemSet(&CopyOneOfOption,sizeof(EFI_IFR_ONE_OF_OPTION),0x00);
							MemCpy(&CopyOneOfOption.Header,&OriOneOfOption->Header,sizeof(EFI_IFR_OP_HEADER));
						
							OptionString = GetHiiString(pProtocol->SmcLsiCurrHiiHandleTable->RaidCardHiiHandle,OriOneOfOption->Option);
							CopyOneOfOption.Option = NewHiiString(SmcLsiSetupHandle,OptionString);
							
							CopyOneOfOption.Flags = OriOneOfOption->Flags;
							CopyOneOfOption.Type  = OriOneOfOption->Type;
							MemCpy(&CopyOneOfOption.Value,&OriOneOfOption->Value,sizeof(EFI_IFR_TYPE_VALUE));
							
							FormItemsBuffer->ItemsCopyBuffer 		= (UINT8*)&CopyOneOfOption;
							FormItemsBuffer->ItemsCopyBufferSize	= CopyOneOfOption.Header.Length;
							FormItemsBuffer = CopyAndExtMem(FormItemsBuffer);
						}
						//The last should be EFI_IFR_END
						FormItemsBuffer->ItemsCopyBuffer 		= (UINT8*)OriOneOfOption;
						FormItemsBuffer->ItemsCopyBufferSize	= OriOneOfOption->Header.Length;
						FormItemsBuffer = CopyAndExtMem(FormItemsBuffer);
					}
				}
					break;
				case EFI_IFR_NUMERIC_OP:
				{
					EFI_IFR_NUMERIC*	pNumer 		= NULL;
					EFI_IFR_NUMERIC		CopyNumer;

					pNumer	= (EFI_IFR_NUMERIC*)OpHeader;

					MemCpy(&CopyNumer.Header,&pNumer->Header,sizeof(EFI_IFR_OP_HEADER));

					CopyNumer.Flags								= pNumer->Flags;
					CopyNumer.Question.Flags					= pNumer->Question.Flags;
					CopyNumer.Question.Header.Prompt 			= NewHiiString(SmcLsiSetupHandle,RaidItemsTable[ItemIndex].LsiItemName);
					CopyNumer.Question.Header.Help				= CopyNumer.Question.Header.Prompt;
					CopyNumer.Question.QuestionId				= pProtocol->SmcLsiGetQIdStart(pProtocol);
					CopyNumer.Question.VarStoreId				= pItemSet->SmcLsiVarId;
					CopyNumer.Question.VarStoreInfo.VarOffset	= pNumer->Question.VarStoreInfo.VarOffset;

					MemCpy(&CopyNumer.data,&pNumer->data,sizeof(MINMAXSTEP_DATA));
					switch((CopyNumer.Flags & EFI_IFR_NUMERIC_SIZE)){
						case EFI_IFR_NUMERIC_SIZE_1:
							CopyNumer.data.u8.Step = 1;
							break;
						case EFI_IFR_NUMERIC_SIZE_2:
							CopyNumer.data.u16.Step = 1;
							break;
						case EFI_IFR_NUMERIC_SIZE_4:
							CopyNumer.data.u32.Step = 1;
							break;
						case EFI_IFR_NUMERIC_SIZE_8:
							CopyNumer.data.u64.Step = 1;
							break;
					}

					FormItemsBuffer->ItemsCopyBuffer 		= (UINT8*)(&CopyNumer);
					FormItemsBuffer->ItemsCopyBufferSize	= CopyNumer.Header.Length;
					FormItemsBuffer = CopyAndExtMem(FormItemsBuffer);
					//Copy related OP? Now just copy the End Op.
					if(!!pNumer->Header.OpCode){
						EFI_IFR_END	EndOp;

						EndOp.Header.OpCode 	= EFI_IFR_END_OP;
						EndOp.Header.Length 	= sizeof(EFI_IFR_END);
						EndOp.Header.Scope		= 0;

						FormItemsBuffer->ItemsCopyBuffer 		= (UINT8*)(&EndOp);
						FormItemsBuffer->ItemsCopyBufferSize	= EndOp.Header.Length;
						FormItemsBuffer = CopyAndExtMem(FormItemsBuffer);
					}
				}
					break;
				default:
					break;
			}
		}
	}
	
	EndOp.Header.OpCode = EFI_IFR_END_OP;
	EndOp.Header.Length = sizeof(EFI_IFR_END);
	EndOp.Header.Scope	= 0;

	FormItemsBuffer->ItemsCopyBuffer 		= (UINT8*)&EndOp;
	FormItemsBuffer->ItemsCopyBufferSize	= EndOp.Header.Length;
	FormItemsBuffer = CopyAndExtMem(FormItemsBuffer);

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

	{
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
	}

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
		
	FormGoToBuffer->ItemsCopyBuffer		= (UINT8*)&FormGoto;
	FormGoToBuffer->ItemsCopyBufferSize	= FormGoto.Header.Length;
	FormGoToBuffer = CopyAndExtMem(FormGoToBuffer);

	gBS->FreePool(FormGoToPromptHelp);

	//Combine Upper + VarBuffer + button
	MACRO_CAULBUFFER_REPLACE(FormGoToBuffer);

	return EFI_SUCCESS;
}

EFI_STATUS	InsertRaidSetupVariable(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol){
	
	MACRO_CAULBUFFER_ARGU(PACKAGE_BUFFER);

	EFI_STATUS			Status				= EFI_SUCCESS;
	UINTN				VarIndex			= 0;

	EFI_IFR_FORM_SET*	FormSetOp			= NULL;
	EFI_IFR_VARSTORE*	VarStore 			= NULL;

	SMC_LSI_ITEMS_MEM*	ItemsBuffer			= NULL;
	
	SMC_LSI_VAR*		LocSmcLsiVarTable	= NULL;

	FormSetOp = (EFI_IFR_FORM_SET*)SearchOpCodeInFormData(pProtocol,EFI_IFR_FORM_SET_OP,0);	
	if(FormSetOp == NULL) return SettingErrorStatus(pProtocol,0x02,EFI_LOAD_ERROR);

	LocSmcLsiVarTable = pProtocol->SmcLsiVarTable;
	
	InsertStart 		= (UINT8*)((UINT8*)FormSetOp + FormSetOp->Header.Length);
	MACRO_CAULBUFFER_INITIAL(PACKAGE_BUFFER);

	ItemsBuffer = CopyAndExtMem(NULL);
	
	for(VarIndex = 0; LocSmcLsiVarTable[VarIndex].LsiRaidTypeIndex != RAID_NULL;VarIndex++){
		
		SMC_LSI_VAR_SET*	pVarSet = NULL;
		if(pProtocol->SmcLsiCurrHiiHandleTable->RaidCardType != LocSmcLsiVarTable[VarIndex].LsiRaidTypeIndex) continue;

		for(pVarSet=LocSmcLsiVarTable[VarIndex].pVarSet;pVarSet != NULL; pVarSet = pVarSet->pNext){

			if(pVarSet->LsiRaidCardIndex == pProtocol->SmcLsiCurrHiiHandleTable->RaidCardIndex){
				UINT8	VarStoreLength = (UINT8)(sizeof(EFI_IFR_VARSTORE) + Strlen(pVarSet->SmcLsiVarName));

				VarStore = NULL;
				Status = gBS->AllocatePool(EfiBootServicesData,VarStoreLength,&VarStore);
				if(EFI_ERROR(Status)) return SettingErrorStatus(pProtocol,0x05,Status);
				MemSet(VarStore,VarStoreLength,0x00);

				VarStore->Header.OpCode = EFI_IFR_VARSTORE_OP;
				VarStore->Header.Length = VarStoreLength;
				VarStore->Header.Scope  = 0;
			
				MemCpy(&VarStore->Guid,&pVarSet->SmcLsiVarGuid,sizeof(EFI_GUID));
				MemCpy(VarStore->Name,pVarSet->SmcLsiVarName,Strlen(pVarSet->SmcLsiVarName));
				VarStore->Name[Strlen(pVarSet->SmcLsiVarName)] = '\0';
			
				VarStore->Size = pVarSet->SmcLsiVarSize;
				VarStore->VarStoreId = pVarSet->SmcLsiVarId;
			
				ItemsBuffer->ItemsCopyBuffer = (UINT8*)VarStore;
				ItemsBuffer->ItemsCopyBufferSize = VarStoreLength;
				ItemsBuffer = CopyAndExtMem(ItemsBuffer);
				gBS->FreePool(VarStore);
			}
		}
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
			case EFI_IFR_VARSTORE_OP:
			{
				EFI_IFR_VARSTORE*	pVar = NULL;
				pVar = (EFI_IFR_VARSTORE*)IfrOpHeader;
				DEBUG((-1,"%aVarStore[%a], Size[%x], VId[%x], Guid[%g]\n",ScopeBuff,pVar->Name,pVar->Size,pVar->VarStoreId,pVar->Guid));
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
				SMC_LSI_RAID_VAR*	pVariable	= NULL;
				UINT8*				pBuffer		= NULL;
				
				pString = (EFI_IFR_STRING*)IfrOpHeader;
				DEBUG((-1,"%aString[%s], QId[%x], VId[%x], VOffset[%x]\n",ScopeBuff,
							GetHiiString(SmcSetupHiiHandle,pString->Question.Header.Prompt),
							pString->Question.QuestionId,
							pString->Question.VarStoreId,
							pString->Question.VarStoreInfo.VarOffset));

				pVariable = SearchLsiVarById(pString->Question.VarStoreId);
				if(pVariable != NULL){
					pBuffer = pVariable->Lsi_Buffer;
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
					DEBUG((-1,"ExtendOp[%x], Number[%x]",pIfrGuidLabel->ExtendOpCode,pIfrGuidLabel->Number));
				}
				DEBUG((-1,"\n"));
			}
				break;
			case EFI_IFR_NUMERIC_OP:
			case EFI_IFR_ONE_OF_OP:
			{
				//These two have same structure.
				EFI_IFR_NUMERIC*		pNumer			= NULL;
				SMC_LSI_RAID_VAR*		pVariable		= NULL;
				UINT8*					pBuffer			= NULL;
				UINT64					DVal			= 0;
				UINT64					MinValue		= 0;
				UINT64					MaxValue		= 0;
				UINT64					Step			= 0;
				CHAR16					NumSize[][5]	= {L"u8", L"u16", L"u32", L"u64"};

				pNumer = (EFI_IFR_NUMERIC*)IfrOpHeader;
				DEBUG((-1,"%a%s",ScopeBuff,(pNumer->Header.OpCode == EFI_IFR_NUMERIC_OP) ? L"Numeric" : L"OneOf"));

				DEBUG((-1,"[%s], QId[%x], VId[%x], VOffset[%x], DataSize[%s]\n",
							GetHiiString(SmcSetupHiiHandle,pNumer->Question.Header.Prompt),
							pNumer->Question.QuestionId,
							pNumer->Question.VarStoreId,
							pNumer->Question.VarStoreInfo.VarOffset,
							NumSize[(pNumer->Flags & EFI_IFR_NUMERIC_SIZE)]));

				pVariable = SearchLsiVarById(pNumer->Question.VarStoreId);
				if(pVariable != NULL){
					pBuffer = pVariable->Lsi_Buffer;
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
				DEBUG((-1,"%aOption[%s], Type[%x], Val[%x]\n",ScopeBuff,
							GetHiiString(SmcSetupHiiHandle,pOneOfOption->Option),
							pOneOfOption->Type,pOneOfOption->Value.u64));
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
	UINTN						 VarIndex						= 0;
	
	EFI_HII_PACKAGE_HEADER 		*SmcLsiSetupIfrPackagePtr 		= NULL;
	EFI_HII_PACKAGE_LIST_HEADER *SmcLsiSetupHiiPackageList 		= NULL;
	SMC_LSI_VAR					*SmcRaidVarTable				= NULL;

	SmcRaidVarTable	  = pProtocol->SmcLsiVarTable;
	SmcLsiSetupHandle = pProtocol->SmcLsiGetHiiHandle(pProtocol);

	if(! (!!SmcLsiSetupHandle)) return SettingErrorStatus(pProtocol,0x01,EFI_INVALID_PARAMETER);

	pProtocol->SmcLsiCurrRAIDSetupData	= NULL;

	SmcLsiSetupHiiPackageList = GetHiiPackageList(SmcLsiSetupHandle);
	if(! (!!SmcLsiSetupHiiPackageList)) return SettingErrorStatus(pProtocol,0x03,EFI_NOT_FOUND);

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

	if(EFI_ERROR(InitialLsiVarHashTable(pProtocol))) return SettingErrorStatus(pProtocol,0x15,EFI_LOAD_ERROR);

	for(VarIndex=0;SmcRaidVarTable[VarIndex].LsiRaidTypeIndex != RAID_NULL;++VarIndex){
		SMC_LSI_VAR_SET* pVarSet = NULL;
		for(pVarSet=SmcRaidVarTable[VarIndex].pVarSet;pVarSet != NULL;pVarSet = pVarSet->pNext){
			if(AddLsiVarByBuffer(pProtocol,\
								 pVarSet->SmcLsiVarName,\
								 &pVarSet->SmcLsiVarGuid,\
								 pVarSet->SmcLsiVarId,\
								 pVarSet->SmcLsiVarSize,\
								 pVarSet->SmcLsiVarBuffer) == NULL){
				return SettingErrorStatus(pProtocol,0x19,EFI_OUT_OF_RESOURCES);
			}
			if(!!pProtocol->DetailedDebugMessage) {
				DEBUG((-1,"Var %a :: \n",pVarSet->SmcLsiVarName));
				DEBUG_PRINT_BUFFER(pVarSet->SmcLsiVarBuffer,pVarSet->SmcLsiVarSize);
			}
		}
	}
	Debug_for_LsiVarTable(pProtocol);
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
	if(! (!!SmcLsiSetupHiiPackageList)) return SettingErrorStatus(pProtocol,0x01,EFI_NOT_FOUND);

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

		static	BOOLEAN				ReNewVar			= TRUE;
		static	INTN				VarIndex 			= -1;
		static	SMC_LSI_VAR*		SmcLsiVarTable 		= NULL;
		static	SMC_LSI_VAR_SET*	pVarSet				= NULL;
		
		EFI_STATUS					Status				= EFI_SUCCESS;

		if((!!ReNew)) ReNewVar = TRUE;

		if(ReNewVar){
			pVarSet 	= NULL;
			VarIndex 	= -1;
			SmcLsiVarTable = pProtocol->SmcLsiVarTable;
		}
		while(pVarSet == NULL && SmcLsiVarTable[++VarIndex].LsiRaidTypeIndex != RAID_NULL){
			pVarSet = SmcLsiVarTable[VarIndex].pVarSet;
		}
		if(pVarSet == NULL){
			SMC_RAID_DETAIL_DEBUG((-1,"SmcLsiRaidLib_ParseNvData :: Not Find\n"));
			ReNewVar = TRUE;
			return EFI_NO_MEDIA;
		}
		SMC_RAID_DETAIL_DEBUG((-1,"SmcLsiRaidLib_ParseNvData :: VarIndex[%d], ReNewVar[%x], ",VarIndex,ReNewVar));

		*SmcRaidVarName = NULL; *SmcRaidVarGuid = NULL; *SmcRaidVarSize = NULL; *SmcRaidVarBuffer = NULL;
		Status = gBS->AllocatePool(EfiBootServicesData,(Strlen(pVarSet->SmcLsiVarName) + 1) * sizeof(CHAR16),SmcRaidVarName);
		RETURN_IFERROR();
		MemSet(*SmcRaidVarName,(Strlen(pVarSet->SmcLsiVarName) + 1) * sizeof(CHAR16),0x00);
		AsciiStrToUnicodeStr(pVarSet->SmcLsiVarName,*SmcRaidVarName);
		SMC_RAID_DETAIL_DEBUG((-1,"SmcRaidVarName[%s], ",*SmcRaidVarName));

		Status = gBS->AllocatePool(EfiBootServicesData,sizeof(EFI_GUID),SmcRaidVarGuid);
		RETURN_IFERROR();
		MemSet(*SmcRaidVarGuid,sizeof(EFI_GUID),0x00);
		MemCpy(*SmcRaidVarGuid,&pVarSet->SmcLsiVarGuid,sizeof(EFI_GUID));
		SMC_RAID_DETAIL_DEBUG((-1,"SmcRaidVarGuid[%g], ",*SmcRaidVarGuid));

		Status = gBS->AllocatePool(EfiBootServicesData,sizeof(UINTN),SmcRaidVarSize);
		RETURN_IFERROR();
		MemSet(*SmcRaidVarSize,sizeof(UINTN),0x00);
		MemCpy(*SmcRaidVarSize,&pVarSet->SmcLsiVarSize,sizeof(UINT16));
		SMC_RAID_DETAIL_DEBUG((-1,"SmcRaidVarSize[%x]\n",**SmcRaidVarSize));

		Status = gBS->AllocatePool(EfiBootServicesData,pVarSet->SmcLsiVarSize,SmcRaidVarBuffer);
		RETURN_IFERROR();
		MemSet(*SmcRaidVarBuffer,pVarSet->SmcLsiVarSize,0x00);
		MemCpy(*SmcRaidVarBuffer,pVarSet->SmcLsiVarBuffer,pVarSet->SmcLsiVarSize);

		pVarSet 	= pVarSet->pNext;
		ReNewVar 	= FALSE;

		return EFI_SUCCESS;
}

EFI_STATUS SmcLsiRaidOOBLibConstructor(
	EFI_HANDLE ImageHandle,
	EFI_SYSTEM_TABLE *SystemTable
){
		
	return EFI_SUCCESS;
}

