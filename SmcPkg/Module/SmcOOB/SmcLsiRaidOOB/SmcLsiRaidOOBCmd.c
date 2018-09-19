#include "SmcLsiRaidOOB.h"
#include "SmcOobVersion.h"
#include "SmcLsiRaidOOBGlbTable.h"

VOID debug_for_Cmd_Set(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol){
#if defined(DEBUG_MODE) && (DEBUG_MODE == 1)

	SMC_RAID_CMD_SET*		pSmcRaidCmdSet	= NULL;
	UINTN					CmdIndex		= 1;

	DEBUG((-1,"SmcRaidCmdSet :: \n"));
	DEBUG((-1,"  CardType[%x], CardIndex[%x] - \n",
			  pProtocol->SmcLsiCurrHiiHandleTable->RaidCardType,
			  pProtocol->SmcLsiCurrHiiHandleTable->RaidCardIndex));

	pSmcRaidCmdSet = pProtocol->SmcLsiCurrHiiHandleTable->RaidCmdSet;

	for(;!!pSmcRaidCmdSet;pSmcRaidCmdSet = pSmcRaidCmdSet->pRaidCmdSetNext){
		SMC_RAID_CMD_SECTION*	pPSection	= NULL;
		DEBUG((-1,"     CMD[%d] - CmdEnable[%x] :: \n",CmdIndex++,pSmcRaidCmdSet->CmdEnable));
		for(pPSection = pSmcRaidCmdSet->RaidCmdSection;!!pPSection;pPSection = pPSection->pRaidCmdSectionNext){
			switch(pPSection->RaidCmdType){
				case SMC_CMD_RAID_GROUP:
				{
					SMC_RAID_CMD_GROUP* 	pGroup 		= NULL;
					SMC_RAID_CMD_GROUP_HDD*	pGroupHdd	= NULL;

					pGroup = pPSection->RaidCmdBody;
					pGroupHdd = pGroup->GroupHddSet;

					DEBUG((-1,"      Group :: \n"));
					for(;!!pGroupHdd;pGroupHdd = pGroupHdd->pHddNext){
						DEBUG((-1,"        Num[%02d], Name",pGroupHdd->HdNum));
						if(!!pGroupHdd->HdHame){
							DEBUG((-1,"[%s]",pGroupHdd->HdHame));
						}
						DEBUG((-1,"\n"));
					}
				}
					break;
				case SMC_CMD_RAID_RAIDTYPE:
				{
					SMC_RAID_CMD_RAIDTYPE* pRaidType = NULL;

					pRaidType = pPSection->RaidCmdBody;
					DEBUG((-1,"      Type[%02d]\n",pRaidType->RaidType));
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
						
					DEBUG((-1,"      Size[%s], Type[%a]\n",pRaidSize->RaidSizeContext,SmcRaidSizeTypeMap[index].RaidSizeStr));
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

					DEBUG((-1,"      Specie[%c]\n",SpecieMap[index].SpecieCode));
				}
					break;
				default:
					break;
	
			}
		}
	}
#endif
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
	
			if(RaidItemsSet->ItemsHeader.RaidItemsType != RAID_CHANGEABLE_TYPE &&
			   RaidItemsSet->ItemsHeader.RaidItemsType != RAID_SMCCMD_TYPE){
				continue;
			}

			SMC_RAID_DETAIL_DEBUG((-1,"Check for item :: From[%s], Name[%s], RaidItemsType[%x], SmcLsiVarId[%x]\n",
							RaidItemsSet->ItemsHeader.LsiItemForm,
							RaidItemsSet->ItemsHeader.LsiItemName,
							RaidItemsSet->ItemsHeader.RaidItemsType,
							pItemsBody->SmcLsiVarId
						));
			
			pRaidVar = SearchLsiVarById(pItemsBody->SmcLsiVarId);
			if(!(!!pRaidVar)) continue;

			pRaidChangedVar = SearchSmcRaidVarInChangedVar(pProtocol,pRaidVar);	
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
		++UpperString;
	}
	return UpperString;
}

VOID*	SearchForCmdSectionBody(SMC_RAID_CMD_SECTION* pCmdSection, SMC_RAID_CMD_TYPE_ENUM CmdTypeEnum){
	
	for(;!!pCmdSection && pCmdSection->RaidCmdType != CmdTypeEnum;pCmdSection = pCmdSection->pRaidCmdSectionNext);
	return (!!pCmdSection) ? pCmdSection->RaidCmdBody : NULL;
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
	paCmdCfgPos += Strlen(SmcRaidSizeTypeMap[SizeTypeIndex].RaidSizeStr);
	if(*(paCmdCfgPos) != ']') return NULL;
	

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

	if(RaidType != SMC_RAID_CMD_GROUP_TYPE_END){
		gBS->AllocatePool(EfiBootServicesData,sizeof(SMC_RAID_CMD_RAIDTYPE),&pRaidType);
		pRaidType->RaidType	= RaidType;

		gBS->AllocatePool(EfiBootServicesData,sizeof(SMC_RAID_CMD_SECTION),&pSection);
		pSection->RaidCmdType = SMC_CMD_RAID_RAIDTYPE;
		pSection->RaidCmdBody = pRaidType;
		pSection->pRaidCmdSectionNext = NULL;
	}
	return pSection;
}

SMC_RAID_CMD_SECTION*	ParseCmdGroup(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol, CHAR8* aCmdString){

	CHAR8*					paCmdCfgPos		= aCmdString;

	SMC_RAID_CMD_GROUP* 	pGroup 			= NULL;
	SMC_RAID_CMD_SECTION*	pSection		= NULL;
	UINT16					RaidHddNum[128];
	UINT8					RaidHddIndex	= 0;
	UINT8					TempIndex		= 0;

	if(!(!!paCmdCfgPos) || !(*paCmdCfgPos)) return NULL;

	MemSet(RaidHddNum,sizeof(UINT16) * 128, SMC_RAID_CMD_GROUP_TYPE_END);

	paCmdCfgPos = Strstr(paCmdCfgPos,"G:[");
	if(!(*paCmdCfgPos)) return NULL;
	paCmdCfgPos += 3;

	do {
		UINT16 	TempVal	= 0xFFFF;
		
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
		if(TempVal != 0xFFFF) RaidHddNum[RaidHddIndex++] = TempVal;
	}while(*paCmdCfgPos != ']');

	if(RaidHddIndex > 0){
		SMC_RAID_CMD_GROUP_HDD*		pGroupHddSet = NULL;

		gBS->AllocatePool(EfiBootServicesData,sizeof(SMC_RAID_CMD_GROUP),&pGroup);
		MemSet(pGroup,sizeof(SMC_RAID_CMD_GROUP),0x00);

		for(TempIndex=0;TempIndex < RaidHddIndex;++TempIndex){
			SMC_RAID_CMD_GROUP_HDD*	pTempGroupHdd	= NULL;
			gBS->AllocatePool(EfiBootServicesData,sizeof(SMC_RAID_CMD_GROUP_HDD),&pTempGroupHdd);
			pTempGroupHdd->HdNum 	= RaidHddNum[TempIndex];
			pTempGroupHdd->HdHame 	= NULL;
			pTempGroupHdd->pHddNext	= NULL;

			if(! (!!pGroupHddSet)){
				pGroupHddSet = pGroup->GroupHddSet = pTempGroupHdd;
			}else{
				pGroupHddSet->pHddNext = pTempGroupHdd;
				pGroupHddSet = pGroupHddSet->pHddNext;
			}
		}

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
	CHAR8*		apTempCmd	= NULL;
	BOOLEAN		LeftPar		= FALSE;
	BOOLEAN		RightPar	= FALSE;

	BOOLEAN		LeftBra		= FALSE;
	
	MemSet(aTempString,SMC_ITEM_CMD_STRING_SIZE * sizeof(CHAR8),0x00);
	MemCpy(aTempString,aCmdString,SMC_ITEM_CMD_STRING_SIZE * sizeof(CHAR8));
	MemSet(aCmdString,SMC_ITEM_CMD_STRING_SIZE * sizeof(CHAR8),0x00);

	apTemp 		= aTempString;
	apTempCmd 	=  aCmdString;

	while(!!(*apTemp) && !(!!RightPar)){
		switch(*apTemp){
			case ' ':
			case '\t':
				++apTemp;
				break;
			case '{':
				if(!!LeftPar) return NULL;
				LeftPar = TRUE;
				*apTempCmd++ = *apTemp++;
				break;
			case '}':
				if(!(!!LeftPar)) return NULL;
				RightPar = TRUE;
				*apTempCmd++ = *apTemp++;
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
					*apTempCmd++ = *apTemp++;
				}else{
					++apTemp;
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
	SMC_RAID_DETAIL_DEBUG((-1,"After Upper Str [%a]\n",aCmdString));
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

	// 3. Parse G:[_1,_2,_3, ... ]
	pTempSection = ParseCmdGroup(pProtocol,aCmdCfgString);
	SMC_RAID_DETAIL_DEBUG((-1,"ParseCmdGroup :: [%s]\n",(!!pTempSection) ? L"Got Cmd Group" : L"Not Cmd Group"));
	if(!!pTempSection){
		pNextSection->pRaidCmdSectionNext = pTempSection;
		pNextSection = pNextSection->pRaidCmdSectionNext;
	}
	// 4. Parse S:[_1 TB|GB]
	pTempSection = ParseCmdRaidSize(pProtocol,aCmdCfgString);
	SMC_RAID_DETAIL_DEBUG((-1,"ParseCmdRaidSize :: [%s]\n",(!!pTempSection) ? L"Got Cmd Raid Size" : L"Not Cmd Raid Size"));
	if(!!pTempSection){
		pNextSection->pRaidCmdSectionNext = pTempSection;
		pNextSection = pNextSection->pRaidCmdSectionNext;
	}
	// 5. Parse R:[_1]
	pTempSection = ParseCmdRaidType(pProtocol,aCmdCfgString);
	SMC_RAID_DETAIL_DEBUG((-1,"ParseCmdRaidType :: [%s]\n",(!!pTempSection) ? L"Got Cmd Raid Type" : L"Not Cmd Raid Type"));
	if(!!pTempSection){
		pNextSection->pRaidCmdSectionNext = pTempSection;
		pNextSection = pNextSection->pRaidCmdSectionNext;
	}

	return pSmcRaidCmdSet;
}
	/*
	 *
	 * In Lsi Raid Build Raid Processing :: 
	 *    1. Enter Create Virtual Drive form.
	 *    2. Enter Select Drives form.
	 *    3. Choice hdds.
	 *    4. Enter Apply Changes form.
	 *    5. Chnage settings.
	 *    6. Enter Save Configuration form.
	 *    7. Set confirm item to enable.
	 *    8. Press Ifr action Yes.
	 *
	 * 	  In each process, we need to get LSI Card Hii again, since LSI Card will update HII in any processing.
	 */



RAID_CMD_PROCESSING_ITEM* SmcLsiRaidOOB_GetTargetItemOpHeader(
	SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol,
	RAID_CMD_PROCESSING				 CmdProcessing,
	CHAR16*							 InTheFormName,
	CHAR16*							 TargetItemName,
	EFI_QUESTION_ID					 LimitQuestionId,
	UINT8							 TargetOpCode
){
	static HII_PACKAGE_LIST_FROM_SET*	sListFormSet			= NULL;

	EFI_STATUS		   			Status 					= EFI_SUCCESS;
	EFI_HII_PACKAGE_HEADER*		LsiIfrPackagePtr		= NULL;
	EFI_IFR_OP_HEADER* 			IfrOpHeader			 	= NULL;
	EFI_HII_HANDLE	   			LsiHiiHandle			= NULL;

	RAID_CMD_PROCESSING_ITEM*	CmdProcessingItems		= NULL;
	RAID_CMD_PROCESSING_ITEM*	CmdProcessingItemsNext	= NULL;

	CHAR16			   			CurrentFormString[TEMP_FORM_STRING];

	LsiHiiHandle 			=	pProtocol->SmcLsiCurrHiiHandleTable->RaidCardHiiHandle;

	if(!!sListFormSet){
		if(!!sListFormSet->PackListHeader){
			gBS->FreePool(sListFormSet->PackListHeader);
			sListFormSet->PackFormHeader = NULL;
			sListFormSet->PackListHeader = NULL;
		}
	}
	if(CmdProcessing == P_RAID_NON_ACTION) return NULL;

	sListFormSet = SmcLsiRaidOOB_GetCurrentPackageForm(pProtocol);
	if(! (!!sListFormSet)) return NULL;

	LsiIfrPackagePtr = sListFormSet->PackFormHeader;
	if(! (!!LsiIfrPackagePtr)) return NULL;

	for(IfrOpHeader = (EFI_IFR_OP_HEADER*)(LsiIfrPackagePtr + 1);
		(UINT32)IfrOpHeader < (UINT32)((UINT8*)LsiIfrPackagePtr + LsiIfrPackagePtr->Length);
		IfrOpHeader = (EFI_IFR_OP_HEADER*)((UINT8*)IfrOpHeader + IfrOpHeader->Length)){

		if(IfrOpHeader->OpCode == EFI_IFR_FORM_OP){
			EFI_IFR_FORM*	pFormOp = (EFI_IFR_FORM*)IfrOpHeader;
			EFI_STRING		TempString = NULL;

			MemSet(CurrentFormString,TEMP_FORM_STRING * sizeof(CHAR16),0x00);
			TempString = GetHiiString(LsiHiiHandle,pFormOp->FormTitle);
			StrCpy(CurrentFormString,TempString);

		}
		if(IfrOpHeader->OpCode == TargetOpCode){
			CHAR16*	IfrString = NULL;
			SMC_LSI_ITEMS_COMMON_HEADER*	ItemsComnHead = NULL;

			SMC_RAID_DETAIL_DEBUG((-1,"Current Form :: [%s]\n",CurrentFormString));	

			ItemsComnHead = (SMC_LSI_ITEMS_COMMON_HEADER*)IfrOpHeader;
			IfrString = GetHiiString(LsiHiiHandle,ItemsComnHead->Question.Header.Prompt);

			if( !!StrCmp(InTheFormName,CurrentFormString)) continue;
			if(! (!!IfrString)){
				SMC_RAID_DETAIL_DEBUG((-1,"StringId[%x], OP[%x], QId[%x] cannot get string\n",
											ItemsComnHead->Question.Header.Prompt,
											ItemsComnHead->Header.OpCode,
											ItemsComnHead->Question.QuestionId));
				continue;
			}
/*			
			SMC_RAID_DETAIL_DEBUG((-1,"IfrString[%s], VarStoreId[%x] OP[%x] QId[%x] >> \n",
						IfrString,ItemsComnHead->Question.VarStoreId,IfrOpHeader->OpCode,
						ItemsComnHead->Question.QuestionId));
*/
			if((ItemsComnHead->Question.QuestionId >= LimitQuestionId) || !(!!StrCmp(TargetItemName,IfrString))){
				RAID_CMD_PROCESSING_ITEM*	pTempItem	= NULL;
				
				SMC_RAID_DETAIL_DEBUG((-1,"IfrString[%s], VarStoreId[%x] OP[%x] QId[%x] >> ",
							IfrString,ItemsComnHead->Question.VarStoreId,IfrOpHeader->OpCode,
							ItemsComnHead->Question.QuestionId));
	
				SMC_RAID_DETAIL_DEBUG((-1,"Recore This item OpHeader!\n"));
				
				gBS->AllocatePool(EfiBootServicesData,sizeof(RAID_CMD_PROCESSING_ITEM),&pTempItem);
				pTempItem->ItemOpHeader = IfrOpHeader;
				pTempItem->pItemNext	= NULL;
				if(!(!!CmdProcessingItems)){
					CmdProcessingItemsNext = CmdProcessingItems = pTempItem;
				}else{
					CmdProcessingItemsNext->pItemNext = pTempItem;
					CmdProcessingItemsNext = CmdProcessingItemsNext->pItemNext;
				}
			}
		}	
	}
	return CmdProcessingItems;
}

EFI_IFR_OP_HEADER*	SearchHddNameInCmdProcessingItem(EFI_HII_HANDLE LsiHiiHandle, RAID_CMD_PROCESSING_ITEM*	pCmdProcessingItem, CHAR16*	pHddName){

	if(!(!!pCmdProcessingItem) || !(!!pHddName)) return NULL;
	SMC_RAID_DETAIL_DEBUG((-1,"SearchHddNameInCmdProcessingItem\n"));

	for(;!!pCmdProcessingItem;pCmdProcessingItem = pCmdProcessingItem->pItemNext){
		EFI_STRING						HddName		= NULL;
		SMC_LSI_ITEMS_COMMON_HEADER*	pCmnHeader	= NULL;

		pCmnHeader = (SMC_LSI_ITEMS_COMMON_HEADER*)pCmdProcessingItem->ItemOpHeader;
		HddName = GetHiiString(LsiHiiHandle,pCmnHeader->Question.Header.Prompt);

		SMC_RAID_DETAIL_DEBUG((-1," HddName[%s], pHddName[%s]\n",HddName,pHddName));
		
		if(!(!!StrCmp(pHddName,HddName))) return pCmdProcessingItem->ItemOpHeader;
	}
	SMC_RAID_DETAIL_DEBUG((-1," Cannot Find!\n"));
	
	return NULL;
}

EFI_STATUS	HandleBuildRaidCmd_B(
		SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol, 
		SMC_RAID_CMD_SET* pCmdSet, 
		RAID_CMD_PROCESSING_MAP* RaidCmdProcessMapTable
){

	EFI_STATUS							Status				= EFI_SUCCESS;
	
	if(!(!!pProtocol) || !(!!pCmdSet) || !(!!RaidCmdProcessMapTable) || !(!!RaidCmdProcessClearMapTable)) return EFI_INVALID_PARAMETER;

	{
		UINTN								TableIndex 			= 0;

		EFI_HII_CONFIG_ACCESS_PROTOCOL*		pConfigAccess		= pProtocol->SmcLsiCurrHiiHandleTable->SmcLsiCurrConfigAccess;
		RAID_CMD_PROCESSING_ITEM*			pCmdProcessingItem	= NULL;
		EFI_HII_HANDLE						LsiHiiHandle		= pProtocol->SmcLsiCurrHiiHandleTable->RaidCardHiiHandle;

		RAID_CMD_PROCESSING_MAP*			pExecuteCmdProcessingMap = RaidCmdProcessMapTable;
		
		do{
			pCmdProcessingItem = SmcLsiRaidOOB_GetTargetItemOpHeader(
									pProtocol,
									pExecuteCmdProcessingMap[TableIndex].CmdProcess,
									pExecuteCmdProcessingMap[TableIndex].CmdProcessLastForm,
									pExecuteCmdProcessingMap[TableIndex].CmdProcessTargetName,
									pExecuteCmdProcessingMap[TableIndex].CmdProcessLimitQId,
									pExecuteCmdProcessingMap[TableIndex].CmdProcessTargetOpCode);
			
			if(!(!!pCmdProcessingItem)){
				DEBUG((-1,"Cannot Find the item! TableIndex[%x]\n",TableIndex));
				break;
			}

			switch(pExecuteCmdProcessingMap[TableIndex++].CmdProcess){
				case P_RAID_ENTER_FORM:
				{
					EFI_IFR_REF*	pOpRef	= NULL;
					pOpRef = (EFI_IFR_REF*)pCmdProcessingItem->ItemOpHeader;

					Status = CallbackForTargetProcessing(
								pConfigAccess,EFI_BROWSER_ACTION_CHANGING,pOpRef->Question.QuestionId,EFI_IFR_TYPE_REF,NULL
							 );
					DEBUG((-1," P_RAID_ENTER_FORM Status[%r]\n",Status));
				}
					break;
				case P_RAID_PRESS_ACTION:
				{	
					EFI_IFR_ACTION	*pOpAction 	= NULL;
					EFI_STRING_ID	ActionId	= 0;

					pOpAction = (EFI_IFR_ACTION*)pCmdProcessingItem->ItemOpHeader;
					ActionId = pOpAction->QuestionConfig;

					Status = CallbackForTargetProcessing(
								pConfigAccess,EFI_BROWSER_ACTION_CHANGING,pOpAction->Question.QuestionId,EFI_IFR_TYPE_ACTION,&ActionId
							 );
					DEBUG((-1," 1. P_RAID_PRESS_ACTION Status[%r]\n",Status));
					if(!EFI_ERROR(Status)){
						Status = CallbackForTargetProcessing(
									pConfigAccess,EFI_BROWSER_ACTION_CHANGED,pOpAction->Question.QuestionId,EFI_IFR_TYPE_ACTION,&ActionId
							 	 );
						DEBUG((-1," 2. P_RAID_PRESS_ACTION Status[%r]\n",Status));
					}
				}
					break;
				case P_RAID_CONFIRM:
				{	
					EFI_IFR_CHECKBOX*	pOpCheckBox	= NULL;
					SMC_RAID_VAR*		pLsiVar		= NULL;
					pOpCheckBox = (EFI_IFR_CHECKBOX*)pCmdProcessingItem->ItemOpHeader;
					if(!!(pLsiVar = SearchLsiVarById(pOpCheckBox->Question.VarStoreId))){

						SMC_RAID_DETAIL_DEBUG((-1," Target Var Name[%a], Guid[%g], VarId[%x], VarSize[%x]\n",
													pLsiVar->RaidVarName,pLsiVar->RaidVarGuid,pLsiVar->RaidVarId,pLsiVar->RaidVarSize));	

						pLsiVar->RaidVarBuffer[pOpCheckBox->Question.VarStoreInfo.VarOffset] = 0x01;

					}else{
						Status = EFI_NOT_FOUND;
					}
					DEBUG((-1," P_RAID_CONFIRM Status[%r]\n",Status));
				}
					break;
				case P_RAID_CHOICE_HDD:
				{
					SMC_RAID_CMD_GROUP*			pCmdGroup 		= NULL;
					SMC_RAID_CMD_GROUP_HDD* 	pCmdGroupHdd	= NULL;
					if(!(!!(pCmdGroup = SearchForCmdSectionBody(pCmdSet->RaidCmdSection,SMC_CMD_RAID_GROUP)))){
						Status	= EFI_NOT_FOUND;
						break;
					}
					pCmdGroupHdd = pCmdGroup->GroupHddSet;
					
					for(;!EFI_ERROR(Status) && !!pCmdGroupHdd; pCmdGroupHdd = pCmdGroupHdd->pHddNext){
						SMC_RAID_VAR*		pLsiVar		= NULL;
						EFI_IFR_CHECKBOX*	pOpCheckBox	= NULL;
						BOOLEAN				Val			= TRUE;

						if(!(!!pCmdGroupHdd->HdHame)){
							Status = EFI_NOT_FOUND;
							break;
						}
						pOpCheckBox = (EFI_IFR_CHECKBOX*)SearchHddNameInCmdProcessingItem(LsiHiiHandle,pCmdProcessingItem,pCmdGroupHdd->HdHame);
						if(!(!!pOpCheckBox)){
							Status = EFI_NOT_FOUND;
							break;
						}
						Status = CallbackForTargetProcessing(
									pConfigAccess,EFI_BROWSER_ACTION_CHANGING,pOpCheckBox->Question.QuestionId,EFI_IFR_TYPE_BOOLEAN,&Val
								 );
						DEBUG((-1," 1. P_RAID_CHOICE_HDD Status[%r] ACTION[%x]\n",Status,EFI_BROWSER_ACTION_CHANGING));
						if(!EFI_ERROR(Status)){
							
							pLsiVar = SearchLsiVarById(pOpCheckBox->Question.VarStoreId);
							
							DEBUG((-1," Target Var Name[%a], Guid[%g], VarId[%x], VarSize[%x]\n",
										pLsiVar->RaidVarName,pLsiVar->RaidVarGuid,pLsiVar->RaidVarId,pLsiVar->RaidVarSize));	
						
							pLsiVar->RaidVarBuffer[pOpCheckBox->Question.VarStoreInfo.VarOffset] = 0x1;
							Val = TRUE;
							Status = CallbackForTargetProcessing(
										pConfigAccess,EFI_BROWSER_ACTION_CHANGED,pOpCheckBox->Question.QuestionId,EFI_IFR_TYPE_BOOLEAN,&Val
								 	 );
							DEBUG((-1," 2. P_RAID_CHOICE_HDD Status[%r] ACTION[%x]\n",Status,EFI_BROWSER_ACTION_CHANGED));
						}	
					}	
				}
					break;
				default:
					break;
			}
		}while(!(EFI_ERROR(Status)));	
	}

	if(EFI_ERROR(Status)) pCmdSet->CmdEnable = FALSE;
	
	return Status;
}


EFI_STATUS	HandleRaidCfgCmdString(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol){

	SMC_RAID_CMD_SET*	pRaidCmdSet 		= NULL;
	UINTN				Processindex		= 0x0;

	pRaidCmdSet = pProtocol->SmcLsiCurrHiiHandleTable->RaidCmdSet;

	for(;!!pRaidCmdSet;pRaidCmdSet = pRaidCmdSet->pRaidCmdSetNext){
		if(!!pRaidCmdSet->CmdEnable){
			SMC_RAID_CMD_SPECIE*	pCmdSpecie = NULL;
			pCmdSpecie = SearchForCmdSectionBody(pRaidCmdSet->RaidCmdSection,SMC_CMD_RAID_COMMAND);
			if(!(!!pCmdSpecie)) continue;

			switch(pCmdSpecie->RaidCmdSpecie){
				case SMC_CMD_SPECIE_BUILD:
					for(Processindex=0;SmcLsiRaidFunctionMapBuildRaidTable[Processindex].RaidCardType != RAID_NULL;++Processindex){
						if(pProtocol->SmcLsiCurrHiiHandleTable->RaidCardType == SmcLsiRaidFunctionMapBuildRaidTable[Processindex].RaidCardType){
							EFI_STATUS	Status = EFI_SUCCESS;

							Status = SmcLsiRaidFunctionMapBuildRaidTable[Processindex].RaidFunc(
										pProtocol,
										pRaidCmdSet,
										SmcLsiRaidFunctionMapBuildRaidTable[Processindex].RaidCmdProcessingMap
									 );
						}
					}
					break;
				default:
					break;
			}
		}
	}
	return EFI_SUCCESS;
}

EFI_STATUS	CollectCfgCmdData(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol){

	/*
	 *	Collect HDD Name into HDD Group.
	 */

	SMC_RAID_ITEMS_SET*			pItemsSet		= NULL;
	SMC_RAID_CMD_SET*			pCmdSet			= NULL;
	SMC_RAID_CMD_GROUP_HDD*		GroupHddTable[64];
	UINTN						GroupHddIndex	= 0;

	MemSet(GroupHddTable,sizeof(SMC_RAID_CMD_GROUP_HDD*) * 64, 0x00);

	pItemsSet = pProtocol->SmcLsiCurrHiiHandleTable->RaidCardInfItems;
	pCmdSet = pProtocol->SmcLsiCurrHiiHandleTable->RaidCmdSet;

	//No Cmd;
	if(!(!!pCmdSet)) return SettingErrorStatus(pProtocol,0xA0,EFI_SUCCESS);

	for(;!!pItemsSet;pItemsSet = pItemsSet->pItemsNext){
		SMC_RAID_ITEMS_BODY	*pItemsBody	= NULL;

		if(pItemsSet->ItemsHeader.RaidItemsType != RAID_HDG_TYPE) continue;

		pItemsBody = pItemsSet->ItemsBody;
		if(! (!!pItemsBody)) continue;
		do {
			EFI_IFR_OP_HEADER*	pOpHeader = NULL;

			pOpHeader = pItemsBody->pLsiItemOp;
			
			switch(pOpHeader->OpCode){
				case EFI_IFR_CHECKBOX_OP: 
				{
					EFI_IFR_CHECKBOX*			OriCBox	  		= (EFI_IFR_CHECKBOX*)pOpHeader;
					CHAR16*						HddString 		= NULL;
					CHAR16*						CopyHddString	= NULL;
					SMC_RAID_CMD_GROUP_HDD*		pGroupHdd		= NULL;	
						
					gBS->AllocatePool(EfiBootServicesData,sizeof(SMC_RAID_CMD_GROUP_HDD),&pGroupHdd);
					HddString = GetHiiString(pProtocol->SmcLsiCurrHiiHandleTable->RaidCardHiiHandle,OriCBox->Question.Header.Prompt);
					gBS->AllocatePool(EfiBootServicesData,StrSize(HddString),&CopyHddString);
					StrCpy(CopyHddString,HddString);
						
					pGroupHdd->HdNum 	= pItemsBody->HdNum;
					pGroupHdd->HdHame	= CopyHddString;
					pGroupHdd->pHddNext	= NULL;

					GroupHddTable[GroupHddIndex++] = pGroupHdd;
				}
					break;
				default:
					break;
			}
		}while(!!(pItemsBody = pItemsBody->pItemsBodyNext));
	}

	pCmdSet = pProtocol->SmcLsiCurrHiiHandleTable->RaidCmdSet;
	// List all Cmd Cfgs.
	for(;!!pCmdSet;pCmdSet = pCmdSet->pRaidCmdSetNext){
		SMC_RAID_CMD_SECTION*	pCmdSection	= NULL;

		pCmdSection = pCmdSet->RaidCmdSection;
		if(!(!!pCmdSection)) continue;
		
		// List all GROUPs in one Cmd Cfg.
		do{
			if(pCmdSection->RaidCmdType == SMC_CMD_RAID_GROUP){
				SMC_RAID_CMD_GROUP*			pCmdGroup 		= NULL;
				SMC_RAID_CMD_GROUP_HDD*		pCmdGroupHdd	= NULL;

				pCmdGroup 		= pCmdSection->RaidCmdBody;
				pCmdGroupHdd 	= pCmdGroup->GroupHddSet;
				if(!(!!pCmdGroupHdd)) continue;
				//List all Hdds in one GROUP.
				do {
					UINTN	TempIndex = 0;
					for(TempIndex=0;TempIndex < GroupHddIndex && GroupHddTable[TempIndex]->HdNum != pCmdGroupHdd->HdNum;++TempIndex);
					//If find target HDD num, record the HDD Name.
					if(TempIndex < GroupHddIndex){
						pCmdGroupHdd->HdHame = GroupHddTable[TempIndex]->HdHame;
					}
				}while(!!(pCmdGroupHdd = pCmdGroupHdd->pHddNext));
			}
		}while(!!(pCmdSection = pCmdSection->pRaidCmdSectionNext));
	}
	
	{
		UINTN	TempIndex = 0;
		for(;TempIndex < GroupHddIndex;++TempIndex){
			gBS->FreePool(GroupHddTable[TempIndex]);
		}
	}

	debug_for_Cmd_Set(pProtocol);
	return EFI_SUCCESS;
}

EFI_STATUS ExamineBasicForCmdRequire(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol){

	SMC_RAID_CMD_SET*		pRaidCmdSet	= NULL;

	pRaidCmdSet = pProtocol->SmcLsiCurrHiiHandleTable->RaidCmdSet;

	for(;!!pRaidCmdSet;pRaidCmdSet = pRaidCmdSet->pRaidCmdSetNext){
		if(!!pRaidCmdSet->CmdEnable){
			SMC_RAID_CMD_SPECIE*	pCmdSpecie = NULL;
			pCmdSpecie = SearchForCmdSectionBody(pRaidCmdSet->RaidCmdSection,SMC_CMD_RAID_COMMAND);
			
			if(!(!!pCmdSpecie)){
				pRaidCmdSet->CmdEnable = FALSE;
				continue;
			}

			switch(pCmdSpecie->RaidCmdSpecie){
				case SMC_CMD_SPECIE_BUILD:
				case SMC_CMD_SPECIE_DELETE:
				{
					SMC_RAID_CMD_GROUP* 	pCmdGroup = NULL;
					if(!(!!(pCmdGroup = SearchForCmdSectionBody(pRaidCmdSet->RaidCmdSection,SMC_CMD_RAID_GROUP)))){
						pRaidCmdSet->CmdEnable = FALSE;
						break;
					}
				}
					break;
				default:
					break;
			}
		}
	}

	debug_for_Cmd_Set(pProtocol);
	return EFI_SUCCESS;
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

			pTempRaidCmdSet->CmdEnable = TRUE;
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

EFI_STATUS	CallbackForTargetProcessing(
	EFI_HII_CONFIG_ACCESS_PROTOCOL*	 pConfigAccess,
	EFI_BROWSER_ACTION				 Action,
	EFI_QUESTION_ID					 QuestionId,
	UINT8							 IfrType,
	VOID*							 pTypeValue){

	if(!!pConfigAccess){
		EFI_STATUS					Status	  = EFI_SUCCESS;
		EFI_BROWSER_ACTION_REQUEST	ActionReq = 0;	
		EFI_IFR_TYPE_VALUE			TypeValue;

		MemSet(&TypeValue,sizeof(EFI_IFR_TYPE_VALUE),0x00);

		if(!!pTypeValue){
			switch(IfrType){
				case EFI_IFR_TYPE_BOOLEAN:
					TypeValue.b = !!(*((BOOLEAN*)pTypeValue));
					break;
				case EFI_IFR_TYPE_ACTION:
					TypeValue.string = *((EFI_STRING_ID*)pTypeValue);
					break;
				default:
					break;
			}
		}

		Status = pConfigAccess->Callback(
					pConfigAccess,
					Action,
					QuestionId,
					IfrType,
					&TypeValue,
					&ActionReq
				);
		
		return Status;
	}
	return EFI_INVALID_PARAMETER;
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

