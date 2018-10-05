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
#ifndef	_H_SmcLsiRaidOOB_LIB_
#define	_H_SmcLsiRaidOOB_LIB_

#include "SmcLsiStack.h"

#define MAX_HPK_LIST_SIZE 				0x100000

#define STRUCT_OFFSET(Type,val) ((UINT32)(((UINT32)(&(((Type*)0)->val))) - ((UINT32)0)))
#define STRUCT_START(Type,val,item) ((Type*)(((UINT32)item) - STRUCT_OFFSET(Type,val)))

EFI_HII_DATABASE_PROTOCOL*			GetHiiDataBase();
EFI_HII_CONFIG_ROUTING_PROTOCOL* 	GetHiiConfigRoutingProtocol();
EFI_HII_STRING_PROTOCOL*			GetHiiStringProtocol();
EFI_HII_STRING_PROTOCOL*			GetHiiStringProtocol();
EFI_STRING							GetHiiString(EFI_HII_HANDLE , EFI_STRING_ID );
EFI_STRING_ID						NewHiiString(EFI_HII_HANDLE , EFI_STRING );

EFI_HANDLE							GetSmcRaidCurrDriverHandle();
SMC_LSI_RAID_OOB_SETUP_PROTOCOL*	GetSmcRaidOOBSetupProtocol();

VOID 								DEBUG_PRINT_CONFIG(CHAR16* );

EFI_STATUS 							GetListPackageListsHandle(UINTN* , EFI_HII_HANDLE**);
EFI_HII_HANDLE 						SearchForFormSet(EFI_GUID* , CHAR16* );
EFI_HII_HANDLE 						SearchForLfiFormSetByHiiOrder(EFI_GUID* , CHAR16** , OUT UINT8* );
EFI_HII_PACKAGE_LIST_HEADER*		GetHiiPackageList(EFI_HII_HANDLE );
CHAR8*								UpperAsciiString(CHAR8*	);
VOID* 								GetPNextStartAddr(VOID* , UINT32 );

EFI_STRING 							AppendOffsetWidth(EFI_STRING , UINTN , UINTN );

EFI_STATUS							SmcLsiRaidLib_DownLoad(SMC_LSI_RAID_OOB_SETUP_PROTOCOL*);
EFI_STATUS							SmcLsiRaidLib_AddDynamicItems(SMC_LSI_RAID_OOB_SETUP_PROTOCOL*);
EFI_STATUS 							SmcLsiRaidLib_ParseNvData(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* ,BOOLEAN ,CHAR16** ,EFI_GUID** ,UINTN** ,UINT8**	);
EFI_STATUS 							SmcLsiRaidLib_CollectData(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* ,CHAR16* ,EFI_GUID* ,UINTN ,UINT8* );
EFI_STATUS 							SmcLsiRaidLib_HandleData(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* );

VOID								DEBUG_PRINT_BUFFER(VOID* ,UINTN );

EFI_STATUS							SmcStackPush(SMC_RAID_STACK* , UINT64 , UINT8 );
EFI_STATUS							SmcStackPop(SMC_RAID_STACK* , SMC_RAID_STACK_BODY* );
EFI_STATUS 							InitialSmcStack(SMC_RAID_STACK* );


#endif
