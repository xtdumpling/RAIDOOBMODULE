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
#ifndef	_H_SmcLsiRaidOOB_LIB_SupGray_
#define	_H_SmcLsiRaidOOB_LIB_SupGray_

#include "SmcLsiStack.h"
#include "SmcLsiRaidOOBLib.h"
#include "SmcLsiRaidOOBSetupProtocol.h"

#define OpCodeFunc_impl(_FuncName)\
	EFI_STATUS OpCodeFunc##_FuncName(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* pProtocol, EFI_IFR_OP_HEADER* InOpHeader, SMC_RAID_STACK* pExpreStack)

#define OpCodeFunc_decl(_FuncName)\
	EFI_STATUS OpCodeFunc##_FuncName(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* , EFI_IFR_OP_HEADER* , SMC_RAID_STACK* );

#define OpCodeFunc_p(_FuncName)\
	OpCodeFunc##_FuncName

typedef struct _HII_OPCODE_FUNCTIONS_ HII_OPCODE_FUNCTIONS;
typedef EFI_STATUS (*HANDLE_HII_OPCODE_FUNCTION)(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* , EFI_IFR_OP_HEADER* , SMC_RAID_STACK* );

struct _HII_OPCODE_FUNCTIONS_ {
	UINT8						OpCode;
	HANDLE_HII_OPCODE_FUNCTION	HandleHiiOpCodeFunc;
};


OpCodeFunc_decl(EQIDVAL)
OpCodeFunc_decl(EQIDID)
OpCodeFunc_decl(EQIDVALLIST)
OpCodeFunc_decl(AND_OR)
OpCodeFunc_decl(NOT)
OpCodeFunc_decl(EQ_SERIALIZE)
OpCodeFunc_decl(BITWISE_NOT)
OpCodeFunc_decl(BITWISE_AND_OR)
OpCodeFunc_decl(ADD_SUB_MULTI_DIV_MOD)
OpCodeFunc_decl(QUESTION_REF1)
OpCodeFunc_decl(QUESTION_REF2)
OpCodeFunc_decl(U8_U16_U32_U64)
#endif
