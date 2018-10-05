//****************************************************************************
//****************************************************************************
//**            (C)Copyright 1993-2018 Supermicro Computer, Inc.            **
//****************************************************************************
//****************************************************************************
//  File History
//
//  Rev. 1.01
//    Bug Fix:  Add a function to common get pNext in Struct.
//    Reason:
//    Auditor:  Durant Lin
//    Date:     Oct/01/2018
//
//  Rev. 1.00
//    Bug Fix:  Initial revision.
//    Reason:
//    Auditor:  Durant Lin
//    Date:     Sep/28/2018
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

#include "SmcLsiRaidOOB.h"
#include "SmcLsiRaidOOBSetupProtocol.h"
#include "SmcLsiRaidOOBLib_SupGray.h"

static HII_OPCODE_FUNCTIONS SuppressifAndGrayOutFuncTable[] = {
	{ EFI_IFR_EQ_ID_VAL_OP		, OpCodeFunc_p(EQIDVAL) 				},
	{ EFI_IFR_EQ_ID_ID_OP		, OpCodeFunc_p(EQIDID)					},
	{ EFI_IFR_EQ_ID_VAL_LIST_OP	, OpCodeFunc_p(EQIDVALLIST)				},
	{ EFI_IFR_AND_OP			, OpCodeFunc_p(AND_OR)					},
	{ EFI_IFR_OR_OP				, OpCodeFunc_p(AND_OR)					},
	{ EFI_IFR_NOT_OP			, OpCodeFunc_p(NOT)						},
	{ EFI_IFR_EQUAL_OP			, OpCodeFunc_p(EQ_SERIALIZE)			},
	{ EFI_IFR_NOT_EQUAL_OP		, OpCodeFunc_p(EQ_SERIALIZE)			},
	{ EFI_IFR_GREATER_THAN_OP	, OpCodeFunc_p(EQ_SERIALIZE)			},
	{ EFI_IFR_GREATER_EQUAL_OP	, OpCodeFunc_p(EQ_SERIALIZE)			},
	{ EFI_IFR_LESS_THAN_OP		, OpCodeFunc_p(EQ_SERIALIZE)			},
	{ EFI_IFR_LESS_EQUAL_OP		, OpCodeFunc_p(EQ_SERIALIZE)			},
	{ EFI_IFR_BITWISE_AND_OP	, OpCodeFunc_p(BITWISE_AND_OR)			},
	{ EFI_IFR_BITWISE_OR_OP		, OpCodeFunc_p(BITWISE_AND_OR)			},
	{ EFI_IFR_BITWISE_NOT_OP	, OpCodeFunc_p(BITWISE_NOT)				},
	{ EFI_IFR_SHIFT_LEFT_OP		, OpCodeFunc_p(SHIFT_LEFT_RIGHT)		},
	{ EFI_IFR_SHIFT_RIGHT_OP	, OpCodeFunc_p(SHIFT_LEFT_RIGHT)		},
	{ EFI_IFR_ADD_OP			, OpCodeFunc_p(ADD_SUB_MULTI_DIV_MOD)	},
	{ EFI_IFR_SUBTRACT_OP		, OpCodeFunc_p(ADD_SUB_MULTI_DIV_MOD)	},
	{ EFI_IFR_MULTIPLY_OP		, OpCodeFunc_p(ADD_SUB_MULTI_DIV_MOD)	},
	{ EFI_IFR_DIVIDE_OP			, OpCodeFunc_p(ADD_SUB_MULTI_DIV_MOD)	},
	{ EFI_IFR_MODULO_OP			, OpCodeFunc_p(ADD_SUB_MULTI_DIV_MOD)	},
	{ EFI_IFR_QUESTION_REF1_OP	, OpCodeFunc_p(QUESTION_REF1)			},
	{ EFI_IFR_QUESTION_REF2_OP	, OpCodeFunc_p(QUESTION_REF2)			},
	{ EFI_IFR_UINT8_OP			, OpCodeFunc_p(U8_U16_U32_U64)			},
	{ EFI_IFR_UINT16_OP			, OpCodeFunc_p(U8_U16_U32_U64)			},
	{ EFI_IFR_UINT32_OP			, OpCodeFunc_p(U8_U16_U32_U64)			},
	{ EFI_IFR_UINT64_OP			, OpCodeFunc_p(U8_U16_U32_U64)			},
	{ EFI_IFR_TRUE_OP			, OpCodeFunc_p(TRUE_FALSE)				},
	{ EFI_IFR_FALSE_OP			, OpCodeFunc_p(TRUE_FALSE)				}
};

UINT64	GetQIdNumericValue(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol,ITEMS_QID_VAL* ItemsQIdVal){

	VOID*	QIdVal = NULL;
	QIdVal = GetQIdValue(pProtocol,ItemsQIdVal->IQVQId);
	
	if(!(!!QIdVal)) return ((UINT64)-1);

	switch(ItemsQIdVal->IQVFlags){
		case EFI_IFR_TYPE_NUM_SIZE_8:
			return *((UINT8*)QIdVal);
		case EFI_IFR_TYPE_NUM_SIZE_16:
			return *((UINT16*)QIdVal);
		case EFI_IFR_TYPE_NUM_SIZE_32:
			return *((UINT32*)QIdVal);
		case EFI_IFR_TYPE_NUM_SIZE_64:
			return *((UINT64*)QIdVal);
		default:
			break;
	}
	return ((UINT64)-1);
}

#define POP_CHECK_VALUE(_y,_x)\
	do{\
		Status = SmcStackPop(_y,_x);\
		if(EFI_ERROR(Status)) return Status;\
	}while(FALSE)
	
OpCodeFunc_impl(EQIDVAL){

	EFI_IFR_EQ_ID_VAL*	EqIdVal 	= (EFI_IFR_EQ_ID_VAL*)InOpHeader;	
	ITEMS_QID_VAL*		Items_QId 	= NULL;	
	UINT16* 			QIdVal		= NULL;

	Items_QId = SearchItemsQIdValByQId(pProtocol,EqIdVal->QuestionId);
	if(!(!!Items_QId)) return EFI_NOT_FOUND;

	if(Items_QId->IQVFlags > EFI_IFR_NUMERIC_SIZE){
		SmcStackPush(pExpreStack,FALSE,EFI_IFR_TYPE_UNDEFINED);
		return EFI_SUCCESS;
	} 
	QIdVal = (UINT16*)GetQIdValue(pProtocol,EqIdVal->QuestionId);
	if(!(!!QIdVal)) return EFI_NOT_FOUND;

	SmcStackPush(pExpreStack,(!!(EqIdVal->Value == *QIdVal)),EFI_IFR_TYPE_BOOLEAN);
	return EFI_SUCCESS;
}

OpCodeFunc_impl(EQIDID){
	
	EFI_IFR_EQ_ID_ID*	EqIdId 	 	= (EFI_IFR_EQ_ID_ID*)InOpHeader;
	ITEMS_QID_VAL*		Items_QId_1 = NULL;
	ITEMS_QID_VAL*		Items_QId_2 = NULL;

	Items_QId_1 = SearchItemsQIdValByQId(pProtocol,EqIdId->QuestionId1);
	Items_QId_2 = SearchItemsQIdValByQId(pProtocol,EqIdId->QuestionId2);

	if(!(!!Items_QId_1) || !(!!Items_QId_2)){
		return EFI_NOT_FOUND;
	}
	do {
		if((Items_QId_1->IQVFlags > EFI_IFR_NUMERIC_SIZE) || (Items_QId_2->IQVFlags > EFI_IFR_NUMERIC_SIZE)){
			if(Items_QId_1->IQVFlags == Items_QId_2->IQVFlags)
				break;
			
			SmcStackPush(pExpreStack,FALSE,EFI_IFR_TYPE_UNDEFINED);
			return EFI_SUCCESS;
		}
	}while(FALSE);

	if(Items_QId_1->IQVFlags <= EFI_IFR_NUMERIC_SIZE){
		UINT64	QIdVal_1 = GetQIdNumericValue(pProtocol,Items_QId_1);
		UINT64	QIdVal_2 = GetQIdNumericValue(pProtocol,Items_QId_2);

		if(QIdVal_1 == ((UINT64)-1) || QIdVal_2 == ((UINT64)-1)) 
			return EFI_NOT_FOUND;

		SmcStackPush(pExpreStack,(QIdVal_1 == QIdVal_2), EFI_IFR_TYPE_BOOLEAN);
	}else {
		//Not Handle yet.
	}
	return EFI_SUCCESS;
}

OpCodeFunc_impl(EQIDVALLIST){

	EFI_IFR_EQ_ID_VAL_LIST* EqIdVarList = (EFI_IFR_EQ_ID_VAL_LIST*)OpHeader;
	UINT16*					QIdVal		= NULL;
	UINT16					ValIndex	= 0;
	BOOLEAN					SearchFind	= FALSE;

	QIdVal = (UINT16*)GetQIdValue(pProtocol,EqIdVarList->QuestionId);
	if(!(!!QIdVal)) {
		return EFI_NOT_FOUND;
	}
	for(ValIndex=0;!(!!SearchFind) && ValIndex < EqIdVarList->ListLength;++ValIndex){
		SearchFind = !!(*QIdVal == EqIdVarList->ValueList[ValIndex]);
	}
	SmcStackPush(ExpreStack,SearchFind,EFI_IFR_TYPE_BOOLEAN);
	return EFI_SUCCESS;
}

OpCodeFunc_impl(AND_OR){
	SMC_RAID_STACK_BODY	AND_1;
	SMC_RAID_STACK_BODY	AND_2;
	BOOLEAN	TempResult = FALSE;

	POP_CHECK_VALUE(pExpreStack,&AND_1);
	POP_CHECK_VALUE(pExpreStack,&AND_2);
	
	if(AND_1.dataType != EFI_IFR_TYPE_BOOLEAN || AND_1.dataType != EFI_IFR_TYPE_BOOLEAN){
		SmcStackPush(pExpreStack,FALSE,EFI_IFR_TYPE_UNDEFINED);
		return EFI_SUCCESS;
	}
	if(InOpHeader->OpCode == EFI_IFR_AND_OP)
		TempResult = (!!(AND_1.data) && !!(AND_2.data));
	else if(InOpHeader->OpCode == EFI_IFR_OR_OP)
		TempResult = (!!(AND_1.data) || !!(AND_2.data));

	SmcStackPush(pExpreStack,TempResult,EFI_IFR_TYPE_BOOLEAN);
	return EFI_SUCCESS;
}

OpCodeFunc_impl(NOT){

	SMC_RAID_STACK_BODY	AND_1;
	POP_CHECK_VALUE(pExpreStack,&AND_1);

	if(AND_1.dataType != EFI_IFR_TYPE_BOOLEAN){
		SmcStackPush(pExpreStack,FALSE,EFI_IFR_TYPE_UNDEFINED);
	}

	SmcStackPush(pExpreStack,!(!!AND_1.data),EFI_IFR_TYPE_BOOLEAN);
	return EFI_SUCCESS;
}

OpCodeFunc_impl(EQ_SERIALIZE){

	SMC_RAID_STACK_BODY rightVal;
	SMC_RAID_STACK_BODY leftVal;

	POP_CHECK_VALUE(pExpreStack,&rightVal);
	POP_CHECK_VALUE(pExpreStack,&leftVal);
				
	//Check Data Type
	if((rightVal.dataType != EFI_IFR_TYPE_BOOLEAN && rightVal.dataType != EFI_IFR_TYPE_STRING && rightVal.dataType > EFI_IFR_NUMERIC_SIZE) ||
	   (leftVal.dataType  != EFI_IFR_TYPE_BOOLEAN && leftVal.dataType  != EFI_IFR_TYPE_STRING && leftVal.dataType  > EFI_IFR_NUMERIC_SIZE)){
		SmcStackPush(pExpreStack,FALSE,EFI_IFR_TYPE_UNDEFINED);
		return EFI_SUCCESS;
	}

	//Check Data Type match each.
	if(rightVal.dataType != leftVal.dataType){
		SmcStackPush(pExpreStack,FALSE,EFI_IFR_TYPE_UNDEFINED);
		return EFI_SUCCESS;
	}
	if(InOpHeader->OpCode == EFI_IFR_EQUAL_OP){
		SmcStackPush(pExpreStack,!!(leftVal.data == rightVal.data),EFI_IFR_TYPE_BOOLEAN);
	}else if(InOpHeader->OpCode == EFI_IFR_NOT_EQUAL_OP){
		SmcStackPush(pExpreStack,!!(leftVal.data != rightVal.data),EFI_IFR_TYPE_BOOLEAN);
	}else if(InOpHeader->OpCode == EFI_IFR_GREATER_THAN_OP){
		SmcStackPush(pExpreStack,!!(leftVal.data > rightVal.data),EFI_IFR_TYPE_BOOLEAN);
	}else if(InOpHeader->OpCode == EFI_IFR_GREATER_EQUAL_OP){
		SmcStackPush(pExpreStack,!!(leftVal.data >= rightVal.data),EFI_IFR_TYPE_BOOLEAN);
	}else if(InOpHeader->OpCode == EFI_IFR_LESS_THAN_OP){
		SmcStackPush(pExpreStack,!!(leftVal.data < rightVal.data),EFI_IFR_TYPE_BOOLEAN);
	}else if(InOpHeader->OpCode == EFI_IFR_LESS_EQUAL_OP){
		SmcStackPush(pExpreStack,!!(leftVal.data <= rightVal.data),EFI_IFR_TYPE_BOOLEAN);
	}

	return EFI_SUCCESS;
}

OpCodeFunc_impl(BITWISE_AND_OR){
	SMC_RAID_STACK_BODY rightVal;
	SMC_RAID_STACK_BODY leftVal;
	UINT64				TempData = 0;

	POP_CHECK_VALUE(ExpreStack,&rightVal);
	POP_CHECK_VALUE(ExpreStack,&leftVal);

	if(rightVal.dataType > EFI_IFR_NUMERIC_SIZE ||  leftVal.dataType > EFI_IFR_NUMERIC_SIZE){
		SmcStackPush(pExpreStack,FALSE,EFI_IFR_TYPE_UNDEFINED);
		return EFI_SUCCESS;
	}
				
	if(InOpHeader->OpCode == EFI_IFR_BITWISE_AND_OP)
		TempData = (leftVal.data & rightVal.data);
	else if(InOpHeader->OpCode == EFI_IFR_BITWISE_OR_OP)
		TempData = (leftVal.data | rightVal.data);

	SmcStackPush(pExpreStack,TempData,EFI_IFR_TYPE_NUM_SIZE_64);

	return EFI_SUCCESS;
}

OpCodeFunc_impl(SHIFT_LEFT_RIGHT){
	SMC_RAID_STACK_BODY rightVal;
	SMC_RAID_STACK_BODY leftVal;
	UINT64				TempData = 0;

	POP_CHECK_VALUE(ExpreStack,&rightVal);
	POP_CHECK_VALUE(ExpreStack,&leftVal);

	if(rightVal.dataType > EFI_IFR_NUMERIC_SIZE ||  leftVal.dataType > EFI_IFR_NUMERIC_SIZE){
		SmcStackPush(pExpreStack,FALSE,EFI_IFR_TYPE_UNDEFINED);
		return EFI_SUCCESS;
	}
				
	if(InOpHeader->OpCode == EFI_IFR_SHIFT_LEFT_OP)
		TempData = (leftVal.data << rightVal.data);
	else if(InOpHeader->OpCode == EFI_IFR_SHIFT_RIGHT_OP)
		TempData = (leftVal.data >> rightVal.data);

	SmcStackPush(pExpreStack,TempData,EFI_IFR_TYPE_NUM_SIZE_64);

	return EFI_SUCCESS;
}

OpCodeFunc_impl(ADD_SUB_MULTI_DIV_MOD){
	SMC_RAID_STACK_BODY rightVal;
	SMC_RAID_STACK_BODY leftVal;
	UINT64				TempData = 0;

	POP_CHECK_VALUE(ExpreStack,&rightVal);
	POP_CHECK_VALUE(ExpreStack,&leftVal);

	if(rightVal.dataType > EFI_IFR_NUMERIC_SIZE ||  leftVal.dataType > EFI_IFR_NUMERIC_SIZE){
		SmcStackPush(pExpreStack,FALSE,EFI_IFR_TYPE_UNDEFINED);
		return EFI_SUCCESS;
	}
				
	if(InOpHeader->OpCode == EFI_IFR_ADD_OP)
		TempData = (leftVal.data + rightVal.data);
	else if(InOpHeader->OpCode == EFI_IFR_SUBTRACT_OP)
		TempData = (leftVal.data - rightVal.data);
	else if(InOpHeader->OpCode == EFI_IFR_MULTIPLY_OP)
		TempData = (rightVal.data * leftVal.data);
	else if(InOpHeader->OpCode == EFI_IFR_DIVIDE_OP)
		TempData = (leftVal.data / rightVal.data);
	else if(InOpHeader->OpCode == EFI_IFR_MODULO_OP)
		TempData = (leftVal.data % rightVal.data);

	SmcStackPush(pExpreStack,TempData,EFI_IFR_TYPE_NUM_SIZE_64);

	return EFI_SUCCESS;
}

EFI_STATUS OpCodeFuncSub_QUESTION_REF(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol, UINT16 QId, SMC_RAID_STACK* pExpreStack){
	ITEMS_QID_VAL*			ItemsQIdVal	= NULL;
	VOID*					QIdVal		= NULL;

	ItemsQIdVal = SearchItemsQIdValByQId(pProtocol,QId);
	if(!(!!ItemsQIdVal)){
		SmcStackPush(pExpreStack,FALSE,EFI_IFR_TYPE_UNDEFINED);
		return EFI_SUCCESS;
	}

	QIdVal = GetQIdValue(pProtocol,ItemsQIdVal->IQVQId);
	if(!(!!QIdVal)){
		SmcStackPush(pExpreStack,FALSE,EFI_IFR_TYPE_UNDEFINED);
		return EFI_SUCCESS;
	}

	switch(ItemsQIdVal->IQVFlags){
		case EFI_IFR_TYPE_NUM_SIZE_8:
			SmcStackPush(pExpreStack,*((UINT8*)QIdVal),EFI_IFR_TYPE_NUM_SIZE_8);
			break;
		case EFI_IFR_TYPE_NUM_SIZE_16:
			SmcStackPush(pExpreStack,*((UINT16*)QIdVal),EFI_IFR_TYPE_NUM_SIZE_16);
			break;
		case EFI_IFR_TYPE_NUM_SIZE_32:
			SmcStackPush(pExpreStack,*((UINT32*)QIdVal),EFI_IFR_TYPE_NUM_SIZE_32);
			break;
		case EFI_IFR_TYPE_NUM_SIZE_64:
			SmcStackPush(pExpreStack,*((UINT64*)QIdVal),EFI_IFR_TYPE_NUM_SIZE_64);
			break;
		case EFI_IFR_TYPE_STRING:
			SmcStackPush(pExpreStack,((UINT32)QIdVal),EFI_IFR_TYPE_STRING);
			break;
		default:
			SmcStackPush(pExpreStack,FALSE,EFI_IFR_TYPE_UNDEFINED);
			break;
	}

	return EFI_SUCCESS;
}

OpCodeFunc_impl(QUESTION_REF2){

	SMC_RAID_STACK_BODY		PopData;

	POP_CHECK_VALUE(pExpreStack,&PopData);
	
	if(PopData.dataType > EFI_IFR_NUMERIC_SIZE || 
	   PopData.data > 0xFFFF){

		SmcStackPush(pExpreStack,FALSE,EFI_IFR_TYPE_UNDEFINED);
		return EFI_SUCCESS;	
	}
	return OpCodeFuncSub_QUESTION_REF(pProtocol,((UINT16)PopData->data),pExpreStack);
}

OpCodeFunc_impl(QUESTION_REF1){
	EFI_IFR_QUESTION_REF1*	QueRef1 	= (EFI_IFR_QUESTION_REF1*)InOpHeader;

	return OpCodeFuncSub_QUESTION_REF(pProtocol,QueRef1->QuestionId,pExpreStack);
}

OpCodeFunc_impl(U8_U16_U32_U64){
	VOID*			OpValue 	= NULL;
	EFI_STATUS		Status		= EFI_SUCCESS;

	OpValue = ((UINT8*)InOpHeader + sizeof(EFI_IFR_OP_HEADER));

	switch(InOpHeader->OpCode){
		case EFI_IFR_UINT8_OP:	
			Status = SmcStackPush(pExpreStack,*((UINT8*)OpValue),EFI_IFR_TYPE_NUM_SIZE_8);
			break;
		case EFI_IFR_UINT16_OP:
			Status = SmcStackPush(pExpreStack,*((UINT16*)OpValue),EFI_IFR_TYPE_NUM_SIZE_16);
			break;
		case EFI_IFR_UINT32_OP:
			Status = SmcStackPush(pExpreStack,*((UINT32*)OpValue),EFI_IFR_TYPE_NUM_SIZE_32);
			break;
		case EFI_IFR_UINT64_OP:
			Status = SmcStackPush(pExpreStack,*((UINT64*)OpValue),EFI_IFR_TYPE_NUM_SIZE_64);
			break;
	}

	return Status;
}

OpCodeFunc_impl(BITWISE_NOT){
	
	SMC_RAID_STACK_BODY rightVal;

	POP_CHECK_VALUE(pExpreStack,&rightVal);

	if(rightVal.dataType > EFI_IFR_NUMERIC_SIZE){
		SmcStackPush(pExpreStack,FALSE,EFI_IFR_TYPE_UNDEFINED);
		return EFI_SUCCESS;
	}
	SmcStackPush(pExpreStack,~rightVal.data,EFI_IFR_TYPE_NUM_SIZE_64);
	return EFI_SUCCESS;
}

OpCodeFunc_impl(TRUE_FALSE){
	
	BOOLEAN		PushData = (InOpHeader->OpCode == EFI_IFR_TRUE_OP ? TRUE : FALSE);
	
	return SmcStackPush(pExpreStack,PushData,EFI_IFR_TYPE_BOOLEAN);
}

HANDLE_HII_OPCODE_FUNCTION SearchHiiOpCodeFunc(UINT8 OpCode, UINT8 TableStartIndex, UINT8 TableEndIndex){
	
	UINT8	TableSearchIndex = ((TableStartIndex + TableEndIndex) >> 1);

	if(SuppressifAndGrayOutFuncTable[TableSearchIndex].OpCode == OpCode)
		return SuppressifAndGrayOutFuncTable[TableSearchIndex].HandleHiiOpCodeFunc;

	if(TableSearchIndex == TableEndIndex || TableSearchIndex == TableStartIndex) return NULL;

	if(SuppressifAndGrayOutFuncTable[TableSearchIndex].OpCode > OpCode)
		return SearchHiiOpCodeFunc(OpCode,TableSearchIndex+1,TableEndIndex);

	if(SuppressifAndGrayOutFuncTable[TableSearchIndex].OpCode < OpCode)
		return SearchHiiOpCodeFunc(OpCode,TableStartIndex,TableSearchIndex);
}

BOOLEAN HandleForSuppressifAndGrayoutifBoolean(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol, EFI_IFR_OP_HEADER* 	InOpHeader){

	EFI_IFR_OP_HEADER*	OpHeader 	= InOpHeader;
	EFI_STATUS			Status	 	= EFI_SUCCESS;
	SMC_RAID_STACK*		ExpreStack	= NULL;
	UINTN				TableIndex  = 0;

	HANDLE_HII_OPCODE_FUNCTION	pHandleHiiOpCodeFunc = NULL;

	if(!(!!pProtocol)) return FALSE;
	if(!(!!OpHeader) || OpHeader->OpCode != EFI_IFR_SUPPRESS_IF_OP || OpHeader->OpCode != EFI_IFR_GRAY_OUT_IF_OP) return FALSE;
	
	gBS->AllocatePool(EfiBootServicesData,sizeof(SMC_RAID_STACK),&ExpreStack);
	InitialSmcStack(ExpreStack);

	do {
		OpHeader = (EFI_IFR_OP_HEADER*)((UINT8*)OpHeader + OpHeader->Length);
		pHandleHiiOpCodeFunc = SearchHiiOpCodeFunc(OpHeader->OpCode,0,(sizeof(SuppressifAndGrayOutFuncTable)/sizeof(HII_OPCODE_FUNCTIONS)) - 1);
		if(!!pHandleHiiOpCodeFunc){ 
			pHandleHiiOpCodeFunc(pProtocol,OpHeader,ExpreStack);
		}else{
		
		}
	}while(OpHeader->OpCode != EFI_IFR_END_OP);
	return TRUE;
}
