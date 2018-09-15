#include "SmcLsiRaidOOB.h"
#include "SmcOobVersion.h"

SMC_LSI_INSIDE_CHANGE_FUNC		mSmcLsiInsideChangeFunc[]	= {
					
	{ L"InitialLsiVarHashTable"				, InitialLsiVarHashTable				},
	{ L"CheckChangeableItemsInChangedVar"	, CheckChangeableItemsInChangedVar		},
	{ L"ChangedVarToOOBVarBuffer"			, ChangedVarToOOBVarBuffer				},

	{ L"SmcLsiHookBrowser2Protocol"			, SmcLsiHookBrowser2Protocol			},
	{ L"AccessRAIDRecordtoChangeSetting"	, AccessRAIDRecordtoChangeSetting		},
	{ L"SmcLsiHookBrowser2Protocol"			, SmcLsiHookBrowser2Protocol			},

	{ L"ParseRaidCfgCmdString"				, ParseRaidCfgCmdString					},
	{ L""									, NULL									}
};

static	SMC_RAID_SIZE_TYPE_MAP	SmcRaidSizeTypeMap[] = {
		{ SIZE_TB_TYPE	, "TB" },
		{ SIZE_GB_TYPE	, "GB" },
		{ SIZE_NON_TYPE	, ""   }
};

static	SMC_RAID_CMD_SPECIE_MAP	SpecieMap[] = {
		{ 'B' , SMC_CMD_SPECIE_BUILD 	},
		{ 'D' , SMC_CMD_SPECIE_DELETE 	},
		{ 'O' , SMC_CMD_SPECIE_OTHER	},
		{ 'X' , SMC_CMD_SPECIE_NON		}
};

extern	BOOLEAN						mDetailedDebugMessage;

VOID debug_for_Cmd_Set(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol){

	SMC_RAID_CMD_SET*		pSmcRaidCmdSet	= NULL;

	DEBUG((-1,"SmcRaidCmdSet :: \n"));
	DEBUG((-1,"  CardType[%x], CardIndex[%x] - \n",
			  pProtocol->SmcLsiCurrHiiHandleTable->RaidCardType,
			  pProtocol->SmcLsiCurrHiiHandleTable->RaidCardIndex));

	pSmcRaidCmdSet = pProtocol->SmcLsiCurrHiiHandleTable->RaidCmdSet;

	for(;!!pSmcRaidCmdSet;pSmcRaidCmdSet = pSmcRaidCmdSet->pRaidCmdSetNext){
		SMC_RAID_CMD_SECTION*	pPSection	= NULL;
		for(pPSection = pSmcRaidCmdSet->RaidCmdSection;!!pPSection;pPSection = pPSection->pRaidCmdSectionNext){
			switch(pPSection->RaidCmdType){
				case SMC_CMD_RAID_GROUP:
				{
					SMC_RAID_CMD_GROUP* pGroup = NULL;
					UINTN				index  = 0;

					pGroup = pPSection->RaidCmdBody;
					DEBUG((-1,"RAID CMD Group RaidHddNum["));
					for(index=0;pGroup->RaidHddNum[index] != SMC_RAID_CMD_GROUP_TYPE_END;++index){
						DEBUG((-1," %02d",pGroup->RaidHddNum[index]));
					}
					DEBUG((-1,"]\n"));
				}
					break;
				case SMC_CMD_RAID_RAIDTYPE:
				{
					SMC_RAID_CMD_RAIDTYPE* pRaidType = NULL;

					pRaidType = pPSection->RaidCmdBody;
					DEBUG((-1,"RAID CMD RaidType[%02d]\n",pRaidType->RaidType));
				}
					break;
				case SMC_CMD_RAID_SIZE:
				{
					SMC_RAID_CMD_RAIDSIZE*	pRaidSize = NULL;
					UINTN					index	  = 0;

					pRaidSize = pPSection->RaidCmdBody;
					for(index=0;
						SmcRaidSizeTypeMap[index].RaidSizeType != pRaidSize->RaidSizeType;
						++index);
						
					DEBUG((-1,"RAID CMD RaidSize Size[%s], Type[%a]\n",pRaidSize->RaidSizeContext,SmcRaidSizeTypeMap[index].RaidSizeStr));
				}
					break;
				case SMC_CMD_RAID_COMMAND:
				{
					SMC_RAID_CMD_SPECIE* pSpecie = NULL;
					UINTN				 index	 = 0;

					pSpecie = pPSection->RaidCmdBody;
					for(index=0;
						SpecieMap[index].SpecieType != pSpecie->RaidCmdSpecie;
						++index);

					DEBUG((-1,"RAID CMD Specie[%c]\n",SpecieMap[index].SpecieCode));
				}
					break;
				default:
					break;
	
			}
		}
	}
}
VOID Debug_for_RaidChangedVar(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol){

#if defined(DEBUG_MODE) && (DEBUG_MODE == 1)
	SMC_CHANGED_VAR_SET* pRaidChangedVar = pProtocol->SmcLsiCurrHiiHandleTable->RaidChangedVarSet;

	DEBUG((-1,"RaidChangedVarSet :: \n"));
	DEBUG((-1,"  CardType[%x], CardIndex[%x] - \n",
			  pProtocol->SmcLsiCurrHiiHandleTable->RaidCardType,
			  pProtocol->SmcLsiCurrHiiHandleTable->RaidCardIndex));

	for(;pRaidChangedVar != NULL;pRaidChangedVar = pRaidChangedVar->pChangedVarNext){

		DEBUG((-1,"    ChangedVarName[%a], ChangedVarGuid[%g], ChangedVarLength[%x], ChangedVarOriName[%a], ChangedVarType[%x], BeUpdated[%x], ChangedVarBuffer[%a]\n",
				  pRaidChangedVar->ChVarHeader.ChangedVarName,
				  pRaidChangedVar->ChVarHeader.ChangedVarGuid,
				  pRaidChangedVar->ChVarHeader.ChangedVarLength,
				  pRaidChangedVar->ChVarBody.ChangedVarOriName,
				  pRaidChangedVar->ChVarBody.ChangedVarType,
				  pRaidChangedVar->ChVarBody.BeUpdated,
				  (!!pRaidChangedVar->ChangedVarBuffer) ? "HAVE" : "NON"
			 ));
	}
#endif
}

SMC_RAID_CHRECORD_SET* SearchSmcRaidAccessInChangedRecord(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol,SMC_RAID_ITEMS_SET* pItemsSet){

	SMC_RAID_CHRECORD_SET*	pRaidChRecord	= NULL;
	pRaidChRecord	= pProtocol->SmcLsiCurrHiiHandleTable->RaidCardChRecordTable;

	if(!(!!pRaidChRecord)) return NULL;
	
	do {
		
		SMC_RAID_DETAIL_DEBUG((-1,"  -- ChRecord Form[%s], Target[%s], QId[%x], VId[%x]\n",
						pRaidChRecord->ChRecordHeader.ChRFrom,
						pRaidChRecord->ChRecordHeader.ChRTarget,
						pRaidChRecord->ChRecordBody.TargetQId,
						pRaidChRecord->ChRecordBody.TargetVId
					));

		if(pRaidChRecord->ChRecordBody.TargetOpCode == 0 ||
		   pRaidChRecord->ChRecordBody.TargetQId    == 0){
			continue;	
		}
		if( !!StrCmp(pRaidChRecord->ChRecordHeader.ChRFrom,pItemsSet->ItemsHeader.LsiItemForm)) continue;
		SMC_RAID_DETAIL_DEBUG((-1,"  >> [FIND]\n"));

		break;

	}while(!!(pRaidChRecord = pRaidChRecord->ChRecordNext));


	return pRaidChRecord;
}

SMC_CHANGED_VAR_SET* SearchSmcRaidVarInChangedVar(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol,SMC_RAID_VAR* pRaidVar){

	SMC_CHANGED_VAR_SET*	pRaidChangedVar	= NULL;
	pRaidChangedVar	= pProtocol->SmcLsiCurrHiiHandleTable->RaidChangedVarSet;

	if(!(!!pRaidChangedVar)) return NULL;
	
	do {
		//if(pRaidChangedVar->ChVarBody.ChangedVarType != SearchType) continue;
		if(!!Strcmp(pRaidChangedVar->ChVarHeader.ChangedVarName,pRaidVar->RaidVarName)) continue;
		if(!!MemCmp(&pRaidChangedVar->ChVarHeader.ChangedVarGuid,&pRaidVar->RaidVarGuid,sizeof(EFI_GUID))) continue;
		if(pRaidChangedVar->ChVarHeader.ChangedVarLength != pRaidVar->RaidVarSize) continue;

		break;

	}while(!!(pRaidChangedVar = pRaidChangedVar->pChangedVarNext));
	return pRaidChangedVar;
}


//If value not be the same (changed), return TRUE, otherwise return FALSE.
BOOLEAN	CheckTheItemAlreadyBeChagned(
		SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol, 
		SMC_RAID_ITEMS_BODY* 			 pItemsBody,
		SMC_RAID_VAR* 					 pRaidVar, 
		SMC_CHANGED_VAR_SET* 			 pRaidChangedVar){

	BOOLEAN							Status 			= FALSE;
	EFI_IFR_OP_HEADER*				pOpHeader		= (EFI_IFR_OP_HEADER*)pItemsBody->pLsiItemOp;

	UINT8*							pRaidVarBuff	= pRaidVar->RaidVarBuffer;
	UINT8*							pRaidChVarBuff	= pRaidChangedVar->ChangedVarBuffer;

	SMC_RAID_DETAIL_DEBUG((-1,"  -- ItemOp[%x] ",pOpHeader->OpCode));
				
	switch(pOpHeader->OpCode){
		case EFI_IFR_STRING_OP:
		{
			EFI_IFR_STRING*	pString	= (EFI_IFR_STRING*)pOpHeader;
			CHAR16*			OriVal	= NULL;
			CHAR16*			ChVal	= NULL;

			OriVal = (CHAR16*)&pRaidVarBuff[pString->Question.VarStoreInfo.VarOffset];
			ChVal  = (CHAR16*)&pRaidChVarBuff[pString->Question.VarStoreInfo.VarOffset];
			Status = (!!StrCmp(OriVal,ChVal));
			SMC_RAID_DETAIL_DEBUG((-1,"Status[%x], OriVal[%s], ChVal[%s]",Status,OriVal,ChVal));
		}
			break;
		//These two types have the same structure.
		case EFI_IFR_ONE_OF_OP:
		case EFI_IFR_NUMERIC_OP:
		{
			EFI_IFR_NUMERIC* pNumer = (EFI_IFR_NUMERIC*)pOpHeader;
			UINT64			 OriVal	= 0;
			UINT64			 ChVal  = 0;

			switch(pNumer->Flags & EFI_IFR_NUMERIC_SIZE){
				case EFI_IFR_NUMERIC_SIZE_1:
					OriVal = *((UINT8*)(&pRaidVarBuff[pNumer->Question.VarStoreInfo.VarOffset]));
					ChVal  = *((UINT8*)(&pRaidChVarBuff[pNumer->Question.VarStoreInfo.VarOffset]));
					break;
				case EFI_IFR_NUMERIC_SIZE_2:
					OriVal = *((UINT16*)(&pRaidVarBuff[pNumer->Question.VarStoreInfo.VarOffset]));
					ChVal  = *((UINT16*)(&pRaidChVarBuff[pNumer->Question.VarStoreInfo.VarOffset]));
					break;
				case EFI_IFR_NUMERIC_SIZE_4:
					OriVal = *((UINT32*)(&pRaidVarBuff[pNumer->Question.VarStoreInfo.VarOffset]));
					ChVal  = *((UINT32*)(&pRaidChVarBuff[pNumer->Question.VarStoreInfo.VarOffset]));
					break;
				case EFI_IFR_NUMERIC_SIZE_8:
					OriVal = *((UINT64*)(&pRaidVarBuff[pNumer->Question.VarStoreInfo.VarOffset]));
					ChVal  = *((UINT64*)(&pRaidChVarBuff[pNumer->Question.VarStoreInfo.VarOffset]));
					break;
				default:
					break;
			}
			Status = !!(OriVal != ChVal);
			SMC_RAID_DETAIL_DEBUG((-1,"Status[%x], OriVal[%x], ChVal[%x]",Status,OriVal,ChVal));
		}
			break;
		case EFI_IFR_CHECKBOX_OP:
		{
			EFI_IFR_CHECKBOX* pCheckBox = (EFI_IFR_CHECKBOX*)pOpHeader;
			UINT8			 OriVal	= 0;
			UINT8			 ChVal  = 0;

			OriVal = pRaidVarBuff[pCheckBox->Question.VarStoreInfo.VarOffset];
			ChVal  = pRaidChVarBuff[pCheckBox->Question.VarStoreInfo.VarOffset];
			Status = !!(OriVal != ChVal);
			SMC_RAID_DETAIL_DEBUG((-1,"Status[%x], OriVal[%x], ChVal[%x]",Status,OriVal,ChVal));
		}
			break;
		default:
			break;
	}
	SMC_RAID_DETAIL_DEBUG((-1,"\n"));
	return Status;
}

EFI_STATUS	CheckChangeableItemsInChangedVar(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol){
	
	SMC_RAID_ITEMS_SET*		RaidItemsSet 	= NULL;

	RaidItemsSet 	= pProtocol->SmcLsiCurrHiiHandleTable->RaidCardInfItems;
	Debug_for_RaidChangedVar(pProtocol);

	for(; !!RaidItemsSet; RaidItemsSet = RaidItemsSet->pItemsNext){
		
		SMC_RAID_ITEMS_BODY* pItemsBody = RaidItemsSet->ItemsBody;	
		for(;!!pItemsBody;pItemsBody = pItemsBody->pItemsBodyNext){
			SMC_RAID_VAR*			pRaidVar 		= NULL;
			SMC_CHANGED_VAR_SET*	pRaidChangedVar	= NULL;
		
			SMC_RAID_DETAIL_DEBUG((-1,"Check for item :: From[%s], Name[%s], RaidItemsType[%x], SmcLsiVarId[%x]\n",
							RaidItemsSet->ItemsHeader.LsiItemForm,
							RaidItemsSet->ItemsHeader.LsiItemName,
							RaidItemsSet->ItemsHeader.RaidItemsType,
							pItemsBody->SmcLsiVarId
						));
			
			pRaidVar = SearchLsiVarById(pItemsBody->SmcLsiVarId);
			if(!(!!pRaidVar)) continue;
			
			if(RaidItemsSet->ItemsHeader.RaidItemsType == RAID_CHANGEABLE_TYPE ||
			   RaidItemsSet->ItemsHeader.RaidItemsType == RAID_SMCCMD_TYPE){
				pRaidChangedVar = SearchSmcRaidVarInChangedVar(pProtocol,pRaidVar);
			}
			SMC_RAID_DETAIL_DEBUG((-1,"  -- [%s] Search Raid Var in Changed Var\n",(pRaidChangedVar == NULL) ? L"NOT FIND" : L"FIND"));
			if(!(!!pRaidChangedVar)) continue;
			
			if(!CheckTheItemAlreadyBeChagned(pProtocol,pItemsBody,pRaidVar,pRaidChangedVar)) continue;
			pRaidChangedVar->ChVarBody.BeUpdated = TRUE;

			if(RaidItemsSet->ItemsHeader.RaidItemsType == RAID_CHANGEABLE_TYPE){
				SMC_RAID_CHRECORD_SET*	pRaidChRecord	= NULL;

				pRaidChRecord = SearchSmcRaidAccessInChangedRecord(pProtocol,RaidItemsSet);
				if(!(!!pRaidChRecord)) continue;
				pRaidChRecord->ChRecordBody.BeAccessed = TRUE;
			}
			//RAID_SMCCMD_TYPE will be parse later.
			SMC_RAID_DETAIL_DEBUG((-1,"  ---- This Item will be Updated!\n"));
		}
	}
	Debug_for_RaidChRecords(pProtocol);
	Debug_for_RaidChangedVar(pProtocol);
	return EFI_SUCCESS;
}

CHAR8*	UpperAsciiString(CHAR8*	UpperString){

	while(!!UpperString && !!(*UpperString)){
		if((*UpperString) >= 'a' && (*UpperString) <= 'z'){
			*UpperString = (CHAR8)((UINT8)(*UpperString) - 0x20);
		}
	}
	return UpperString;
}

SMC_RAID_CMD_SECTION*	ParseCmdRaidSize(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol, CHAR8* aCmdString){

	CHAR8*				paCmdCfgPos		= aCmdString;
	CHAR16				RaidSizeContext[80];
	CHAR16*				pRaidSizeContext;
	BOOLEAN				aPoint			= FALSE;
	UINTN				SizeTypeIndex	= 0;
	
	SMC_RAID_CMD_RAIDSIZE*	pRaidType	= NULL;
	SMC_RAID_CMD_SECTION*	pSection	= NULL;

	if(!(!!paCmdCfgPos) || !(*paCmdCfgPos)) return NULL;

	MemSet(RaidSizeContext,80 * sizeof(CHAR16),0x00);
	pRaidSizeContext = RaidSizeContext;

	paCmdCfgPos = Strstr(paCmdCfgPos,"S:[");
	if(!(*paCmdCfgPos)) return NULL;
	paCmdCfgPos += 3;

	// 1. The leading char must be a number.
	if(*paCmdCfgPos < '0' || *paCmdCfgPos > '9') return NULL;
		
	// 2. Copy the totoal Numeric String.
	while((*paCmdCfgPos >= '0' && *paCmdCfgPos <= '9') || (*paCmdCfgPos == '.')){
		if(*paCmdCfgPos == '.'){
			if(!!aPoint) return NULL;
			aPoint = TRUE;
		}
		*pRaidSizeContext++ = (CHAR16)(*paCmdCfgPos++);
	}
	// 3. Check The Size Type.
	for(SizeTypeIndex=0;SmcRaidSizeTypeMap[SizeTypeIndex].RaidSizeType != SIZE_NON_TYPE; ++SizeTypeIndex){
		if(!(!!MemCmp(paCmdCfgPos,SmcRaidSizeTypeMap[SizeTypeIndex].RaidSizeStr,Strlen(SmcRaidSizeTypeMap[SizeTypeIndex].RaidSizeStr)*sizeof(CHAR8)))){
			break;
		}
	}
	if(SmcRaidSizeTypeMap[SizeTypeIndex].RaidSizeType == SIZE_NON_TYPE) return NULL;
	if(*(++paCmdCfgPos)) return NULL;
	

	gBS->AllocatePool(EfiBootServicesData,sizeof(SMC_RAID_CMD_RAIDSIZE),&pRaidType);
	pRaidType->RaidSizeType = SmcRaidSizeTypeMap[SizeTypeIndex].RaidSizeType;
	MemCpy(pRaidType->RaidSizeContext,RaidSizeContext,80 * sizeof(CHAR16));

	gBS->AllocatePool(EfiBootServicesData,sizeof(SMC_RAID_CMD_SECTION),&pSection);
	pSection->RaidCmdType			= SMC_CMD_RAID_SIZE;
	pSection->RaidCmdBody			= pRaidType;
	pSection->pRaidCmdSectionNext	= NULL;

	return pSection;
}

SMC_RAID_CMD_SECTION*	ParseCmdRaidType(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol, CHAR8* aCmdString){

	CHAR8*				paCmdCfgPos		= aCmdString;

	SMC_RAID_CMD_RAIDTYPE*	pRaidType	= NULL;
	SMC_RAID_CMD_SECTION*	pSection	= NULL;
	UINT16					TempVal		= 0xFFFF;
	UINT8					RaidType	= SMC_RAID_CMD_GROUP_TYPE_END;

	if(!(!!paCmdCfgPos) || !(*paCmdCfgPos)) return NULL;

	paCmdCfgPos = Strstr(paCmdCfgPos,"R:[");
	if(!(*paCmdCfgPos)) return NULL;
	paCmdCfgPos += 3;
		
	while(*paCmdCfgPos >= '0' && *paCmdCfgPos <= '9'){
		if(TempVal == 0xFFFF) TempVal = 0x0;
		TempVal = (TempVal * 10) + ((UINT8)*paCmdCfgPos++ - (UINT8)'0');
		if(TempVal >= 0x0FF) return NULL;
	}
	if((*paCmdCfgPos) != ']') return NULL;

	if(TempVal != 0xFFFF) RaidType = (UINT8)TempVal;

	gBS->AllocatePool(EfiBootServicesData,sizeof(SMC_RAID_CMD_RAIDTYPE),&pRaidType);
	pRaidType->RaidType	= RaidType;

	gBS->AllocatePool(EfiBootServicesData,sizeof(SMC_RAID_CMD_SECTION),&pSection);
	pSection->RaidCmdType = SMC_CMD_RAID_GROUP;
	pSection->RaidCmdBody = pRaidType;
	pSection->pRaidCmdSectionNext = NULL;

	return pSection;
}

SMC_RAID_CMD_SECTION*	ParseCmdGroup(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol, CHAR8* aCmdString){

	CHAR8*					paCmdCfgPos		= aCmdString;

	SMC_RAID_CMD_GROUP* 	pGroup 			= NULL;
	SMC_RAID_CMD_SECTION*	pSection		= NULL;
	UINT8					RaidHddNum[128];
	UINT8					RaidHddIndex	= 0;
	UINT16					TempVal			= 0;

	if(!(!!paCmdCfgPos) || !(*paCmdCfgPos)) return NULL;

	MemSet(RaidHddNum,sizeof(UINT8) * 128, SMC_RAID_CMD_GROUP_TYPE_END);

	paCmdCfgPos = Strstr(paCmdCfgPos,"G:[");
	if(!(*paCmdCfgPos)) return NULL;
	paCmdCfgPos += 3;

	do {
		TempVal = 0xFFFF; 
		
		while(*paCmdCfgPos >= '0' && *paCmdCfgPos <= '9'){
			if(TempVal == 0xFFFF) TempVal = 0x0;
			TempVal = (TempVal * 10) + ((UINT8)*paCmdCfgPos++ - (UINT8)'0');
			if(TempVal >= 0x0FF) return NULL;
		}
		
		switch(*paCmdCfgPos){
			case ',':
				++paCmdCfgPos;
				break;
			case ']':
				break;
			default:
				return NULL;
		}
		if(TempVal != 0xFFFF) RaidHddNum[RaidHddIndex++] = (UINT8)TempVal;
	}while(*paCmdCfgPos != ']');

	if(RaidHddIndex > 0){
		gBS->AllocatePool(EfiBootServicesData,sizeof(SMC_RAID_CMD_GROUP),&pGroup);
		MemCpy(pGroup->RaidHddNum,RaidHddNum,sizeof(UINT8) * 128);

		gBS->AllocatePool(EfiBootServicesData,sizeof(SMC_RAID_CMD_SECTION),&pSection);
		pSection->RaidCmdType = SMC_CMD_RAID_GROUP;
		pSection->RaidCmdBody = pGroup;
		pSection->pRaidCmdSectionNext = NULL;
	}
	return pSection;
}


SMC_RAID_CMD_SECTION*	ParseCmdSpecie(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol, CHAR8* aCmdString){

	CHAR8*				paCmdCfgPos		= aCmdString;
	UINT8				SpecieIndex		= 0;

	SMC_RAID_CMD_SPECIE*	pSpecie		= NULL;
	SMC_RAID_CMD_SECTION*	pSection	= NULL;
	
	if(!(!!paCmdCfgPos) || !(*paCmdCfgPos)) return NULL;

	paCmdCfgPos = Strstr(paCmdCfgPos,"C:[");
	if(!(*paCmdCfgPos)) return NULL;
	paCmdCfgPos += 3;

	for(SpecieIndex = 0;SpecieMap[SpecieIndex].SpecieType != SMC_CMD_SPECIE_NON;++SpecieIndex){
		if(*paCmdCfgPos == SpecieMap[SpecieIndex].SpecieCode){
			break;
		}
	}
	if(SpecieMap[SpecieIndex].SpecieType == SMC_CMD_SPECIE_NON) return NULL;

	if((*(++paCmdCfgPos)) != ']') return NULL;
	
	gBS->AllocatePool(EfiBootServicesData,sizeof(SMC_RAID_CMD_SPECIE),&pSpecie);
	pSpecie->RaidCmdSpecie = SpecieMap[SpecieIndex].SpecieType;
	
	gBS->AllocatePool(EfiBootServicesData,sizeof(SMC_RAID_CMD_SECTION),&pSection);
	pSection->RaidCmdType 			= SMC_CMD_RAID_COMMAND;
	pSection->RaidCmdBody 			= pSpecie;
	pSection->pRaidCmdSectionNext	= NULL;

	return pSection;
}

CHAR8*	DeleteUselessChar(CHAR8* aCmdString){

	CHAR8		aTempString[SMC_ITEM_CMD_STRING_SIZE];
	CHAR8*		apTemp		= NULL;
	BOOLEAN		LeftPar		= FALSE;
	BOOLEAN		RightPar	= FALSE;

	BOOLEAN		LeftBra		= FALSE;
	
	MemSet(aTempString,SMC_ITEM_CMD_STRING_SIZE * sizeof(CHAR8),0x00);
	MemCpy(aTempString,aCmdString,SMC_ITEM_CMD_STRING_SIZE * sizeof(CHAR8));
	MemSet(aCmdString,SMC_ITEM_CMD_STRING_SIZE * sizeof(CHAR8),0x00);
	
	apTemp = aTempString;

	while(!!(*apTemp) && !(!!RightPar)){
		switch(*apTemp){
			case ' ':
			case '\t':
				++apTemp;
				break;
			case '{':
				if(!!LeftPar) return NULL;
				LeftPar = TRUE;
				*aCmdString++ = *apTemp++;
				break;
			case '}':
				if(!(!!LeftPar)) return NULL;
				RightPar = TRUE;
				*aCmdString++ = *apTemp++;
				break;
			default:
				if(!!LeftPar){
					switch(*apTemp){
						case '[':
							if(!!LeftBra) return NULL;
							LeftBra = TRUE;
							break;
						case ']':
							if(!(!!LeftBra)) return NULL;
							LeftBra = FALSE;
							break;
					}
					*aCmdString++ = *apTemp++;
				}
				break;
		}
	}
	
	if(!(!!RightPar) || (!!LeftBra)) return NULL;
	return aCmdString;
}

CHAR8*	ReConstructTheCmdLine(CHAR16*	pOriCmdCfg){

	CHAR8*	aCmdString		= NULL;
	CHAR8*	aNewCmdString	= NULL;

	if(!(!!pOriCmdCfg)) return NULL;
	if(StrLen(pOriCmdCfg)+1 > SMC_ITEM_CMD_STRING_SIZE) return NULL;

	gBS->AllocatePool(EfiBootServicesData,SMC_ITEM_CMD_STRING_SIZE * sizeof(CHAR8),&aCmdString);
	MemSet(aCmdString,SMC_ITEM_CMD_STRING_SIZE * sizeof(CHAR8),0x00);

	UnicodeStrToAsciiStr(pOriCmdCfg,aCmdString);
	UpperAsciiString(aCmdString);
	aCmdString = DeleteUselessChar(aCmdString);
	
	return aCmdString;
}
SMC_RAID_CMD_SET* AnalysisAndParseCmdString(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol, CHAR16*	puCmdCfgString){

	SMC_RAID_CMD_SET* 		pSmcRaidCmdSet	= NULL;
	CHAR8*					aCmdCfgString	= NULL;

	CHAR8*					paCmdCfgPos		= NULL;
	CHAR8*					CmdCfgStart		= NULL;
	CHAR8*					CmdCfgEnd		= NULL;

	EFI_STATUS				ParseStatus				= EFI_SUCCESS;
	SMC_RAID_CMD_SECTION*	pNextSection			= NULL;
	SMC_RAID_CMD_SECTION*	pTempSection			= NULL;

	if(!(!!pProtocol) || !(!!puCmdCfgString)) return NULL;

	aCmdCfgString = ReConstructTheCmdLine(puCmdCfgString);
	if(!(!!aCmdCfgString)) return NULL;
	
	SMC_RAID_DETAIL_DEBUG((-1,"ReConstructTheCmdLine ==> aCmdCfgString[%a]\n",aCmdCfgString));

	// 2. Parse C:[_1]
	pTempSection = ParseCmdSpecie(pProtocol,aCmdCfgString);
	SMC_RAID_DETAIL_DEBUG((-1,"ParseCmdSpecie :: [%s]\n",(!!pTempSection) ? L"Got Cmd Specie" : L"Not Cmd Specie"));
	if(!(!!pTempSection)) return NULL;

	gBS->AllocatePool(EfiBootServicesData,sizeof(SMC_RAID_CMD_SET),&pSmcRaidCmdSet);
	MemSet(pSmcRaidCmdSet,sizeof(SMC_RAID_CMD_SET),0x00);
	pNextSection = pSmcRaidCmdSet->RaidCmdSection = pTempSection;
	pTempSection = NULL;

	//3. Parse G:[_1,_2,_3, ... ]
	pTempSection = ParseCmdGroup(pProtocol,aCmdCfgString);
	SMC_RAID_DETAIL_DEBUG((-1,"ParseCmdGroup :: [%s]\n",(!!pTempSection) ? L"Got Cmd Group" : L"Not Cmd Group"));
	if(!!pTempSection){
		pNextSection->pRaidCmdSectionNext = pTempSection;
		pNextSection = pNextSection->pRaidCmdSectionNext;
	}
	//4. Parse R:[_1]
	pTempSection = ParseCmdRaidSize(pProtocol,aCmdCfgString);
	SMC_RAID_DETAIL_DEBUG((-1,"ParseCmdRaidSize :: [%s]\n",(!!pTempSection) ? L"Got Cmd Raid Size" : L"Not Cmd Raid Size"));
	if(!!pTempSection){
		pNextSection->pRaidCmdSectionNext = pTempSection;
		pNextSection = pNextSection->pRaidCmdSectionNext;
	}

	return pSmcRaidCmdSet;
}

	/*
	 *
	 *  {C:[_1] G:[_1,_2,_3, ... ] R:[_1] S:[_1 TB|GB]}
	 *
	 *  C ::= B,D,O
	 *    B ::= Build  Raid.
	 *    D ::= Delete Raid.
	 *    O ::= Otehr  Cmd.
	 *
	 *    B ::= Must have <G>, Optional <R,S>
	 *    D ::= Must have <G>
	 *    O ::= Undefined.
	 */


EFI_STATUS	ParseRaidCfgCmdString(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol){

	SMC_RAID_CMD_SET*		pSmcRaidCmdSet	= NULL;
	SMC_RAID_CMD_SET*		pTempRaidCmdSet = NULL;
	SMC_RAID_ITEMS_SET*		pRaidItemsSet	= NULL;
	
	
	pRaidItemsSet 	= pProtocol->SmcLsiCurrHiiHandleTable->RaidCardInfItems;
	pSmcRaidCmdSet	= pProtocol->SmcLsiCurrHiiHandleTable->RaidCmdSet;

	for(;!!pRaidItemsSet;pRaidItemsSet = pRaidItemsSet->pItemsNext){
		SMC_RAID_ITEMS_BODY*	pItemsBody	= NULL;
			
		if(pRaidItemsSet->ItemsHeader.RaidItemsType != RAID_SMCCMD_TYPE) continue;
		if(!!StrCmp(pRaidItemsSet->ItemsHeader.LsiItemName,SMC_RAID_CMD_FORM_STRING)) continue;

		for(pItemsBody = pRaidItemsSet->ItemsBody;!!pItemsBody;pItemsBody = pItemsBody->pItemsBodyNext){
			SMC_RAID_VAR*	pSmcRaidVar		= NULL;
			EFI_IFR_STRING*	pIfrString 		= (EFI_IFR_STRING*)pItemsBody->pLsiItemOp;
			CHAR16*			CmdCfgString	= NULL;

			pSmcRaidVar 	= SearchLsiVarById(pIfrString->Question.VarStoreId);
			if(!(!!SearchSmcRaidVarInChangedVar(pProtocol,pSmcRaidVar))) continue;

			CmdCfgString 	= (CHAR16*)(&pSmcRaidVar->RaidVarBuffer[pIfrString->Question.VarStoreInfo.VarOffset]);
			SMC_RAID_DETAIL_DEBUG((-1,"CmdCfgString [%s]\n",CmdCfgString));

			pTempRaidCmdSet	= AnalysisAndParseCmdString(pProtocol,CmdCfgString);
			if(!(!!pTempRaidCmdSet)) continue;
			if(!(!!pSmcRaidCmdSet)){
				pSmcRaidCmdSet = pProtocol->SmcLsiCurrHiiHandleTable->RaidCmdSet = pTempRaidCmdSet;
			}else{
				pSmcRaidCmdSet->pRaidCmdSetNext = pTempRaidCmdSet;
				pSmcRaidCmdSet = pSmcRaidCmdSet->pRaidCmdSetNext;
			}	
		}
	}
	debug_for_Cmd_Set(pProtocol);
	return EFI_SUCCESS;
}

EFI_STATUS	AccessRAIDRecordtoChangeSetting(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol){

	SMC_RAID_CHRECORD_SET*	pRaidChRecord	= NULL;
	/*
	 * 2. Second Step we access each accessed method.
	 */

	pRaidChRecord = pProtocol->SmcLsiCurrHiiHandleTable->RaidCardChRecordTable;

	for(;!!pRaidChRecord;pRaidChRecord = pRaidChRecord->ChRecordNext){
		
		if(!!pRaidChRecord->ChRecordBody.BeAccessed){
			EFI_STATUS					Status		= EFI_SUCCESS;
			EFI_IFR_TYPE_VALUE			TypeValue;
			EFI_BROWSER_ACTION_REQUEST	ActionReq 	= 0;
			UINT8						IfrType		= 0;
			
			MemSet(&TypeValue,sizeof(EFI_IFR_TYPE_VALUE),0x00);

			switch(pRaidChRecord->ChRecordBody.TargetOpCode){
				case EFI_IFR_REF_OP:
					IfrType	= EFI_IFR_TYPE_REF;
					break;
				default:
					break;
			}

			DEBUG((-1,"RaidType[%x], RaidCardIndex[%x] Callback to set RAID CARD Setting, FormName[%s], FormTarget[%s] OpCode[%x], QId[%x], VId[%x] >> START\n",
						pProtocol->SmcLsiCurrHiiHandleTable->RaidCardType,
						pProtocol->SmcLsiCurrHiiHandleTable->RaidCardIndex,
						pRaidChRecord->ChRecordHeader.ChRFrom,
						pRaidChRecord->ChRecordHeader.ChRTarget,
						pRaidChRecord->ChRecordBody.TargetOpCode,
						pRaidChRecord->ChRecordBody.TargetQId,
						pRaidChRecord->ChRecordBody.TargetVId
					));

			Status = pProtocol->SmcLsiCurrHiiHandleTable->SmcLsiCurrConfigAccess->Callback(
						pProtocol->SmcLsiCurrHiiHandleTable->SmcLsiCurrConfigAccess,
						EFI_BROWSER_ACTION_CHANGING,
						pRaidChRecord->ChRecordBody.TargetQId,
						IfrType,
						&TypeValue,
						&ActionReq
					);

			DEBUG((-1,"RaidType[%x], RaidCardIndex[%x] Callback to set RAID CARD Setting, Status[%r], IfrType[%x], ActionReq[%x] >> END \n",
						pProtocol->SmcLsiCurrHiiHandleTable->RaidCardType,
						pProtocol->SmcLsiCurrHiiHandleTable->RaidCardIndex,
						Status,IfrType,ActionReq));
		
			if(EFI_ERROR(Status)) SettingErrorStatus(pProtocol,0x55,EFI_SUCCESS);
		}
	}
	return EFI_SUCCESS;
}

EFI_STATUS ChangedVarToOOBVarBuffer(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* 	 pProtocol){

	SMC_CHANGED_VAR_SET*	pRaidChangedVar = NULL;
	/*
	 * 1. First step we will set var from changed var list.
	 */
	
	pRaidChangedVar = pProtocol->SmcLsiCurrHiiHandleTable->RaidChangedVarSet;
	for(; !!pRaidChangedVar;pRaidChangedVar = pRaidChangedVar->pChangedVarNext){

		if(!!pRaidChangedVar->ChVarBody.BeUpdated){
			SMC_RAID_VAR*	pVar = NULL;
			pVar = SearchLsiVarByName(pRaidChangedVar->ChVarHeader.ChangedVarName,&pRaidChangedVar->ChVarHeader.ChangedVarGuid);
			
			//This situation should not happen, but if occur, we now just continue.
			if(!(!!pVar)) continue;

			pVar = SetLsiVarBuffer(pVar,pRaidChangedVar->ChangedVarBuffer);
			if(!(!!pVar)) continue;		
		}	
	}

	return EFI_SUCCESS;
}

	//
	// Handle Change setting first and then BUILD RAID.
	// Handle each RAID CARD one by one, even if one RAID CARD have problem, other RAID CARD still need to handle.
	//
	/*
	 * 1. Check changed items in changed var is changed or not.
	 * 2. record this changed var and changed form link to beUpdated and beAccessed.
	 * 3. Access handshake list to change raid card setting.
	 * 4. Optional ::= Make sure items already be changed.
	 * 5. Loop to next RAID Card.
	 *
	 */

EFI_STATUS SmcLsiRaidLib_HandleData(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* 	 pProtocol){

	EFI_STATUS	Status		= EFI_SUCCESS;
	UINTN		RaidIndex	= 0;

	DEBUG((-1, "\n... SmcLsiRaidLib_HandleData ... START ...\n"));
	if(pProtocol == NULL) return EFI_LOAD_ERROR;
	if(!(!!pProtocol->SmcLsiHaveNvData)) return EFI_NOT_FOUND;
	
	for(pProtocol->SmcLsiCurrHiiHandleTable = pProtocol->SmcLsiHiiHandleTable;
		pProtocol->SmcLsiCurrHiiHandleTable != NULL && pProtocol->SmcLsiCurrHiiHandleTable->RaidCardType != RAID_NULL;
		pProtocol->SmcLsiCurrHiiHandleTable = pProtocol->SmcLsiCurrHiiHandleTable->pNext){
		
		DEBUG((-1,"    SmcLsiCurrHiiHandleTable RaidCardType[%x], RaidCardIndex[%x], RaidCardHiiHandle[%08x]\n",
				   pProtocol->SmcLsiCurrHiiHandleTable->RaidCardType,
				   pProtocol->SmcLsiCurrHiiHandleTable->RaidCardIndex,
				   (UINT32)pProtocol->SmcLsiCurrHiiHandleTable->RaidCardHiiHandle
			));

		for(RaidIndex=0;mSmcLsiInsideChangeFunc[RaidIndex].Func != NULL;RaidIndex++){
			THROUGH_DEBUG_MESSAGE(pProtocol,mSmcLsiInsideChangeFunc[RaidIndex].FuncName,SMC_LSI_THROUGH_FUNC_EXEC);
			Status = mSmcLsiInsideChangeFunc[RaidIndex].Func(pProtocol);
			Status = THROUGH_DEBUG_MESSAGE(pProtocol,mSmcLsiInsideChangeFunc[RaidIndex].FuncName,Status);
			if(EFI_ERROR(Status)) break;
		}
		if(EFI_ERROR(Status)) break;
	}

	pProtocol->SmcLsiHaveNvData = FALSE;
	DEBUG((-1, "... SmcLsiRaidLib_HandleData ... END ...\n"));
	return Status;
}

