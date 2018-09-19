//****************************************************************************
//****************************************************************************
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
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

#ifndef	_H_SmcLsiRaidOOB_LIB_
#define	_H_SmcLsiRaidOOB_LIB_

#define MAX_HPK_LIST_SIZE 				0x100000

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

EFI_STRING 							AppendOffsetWidth(EFI_STRING , UINTN , UINTN );


EFI_STATUS							SmcLsiRaidLib_DownLoad(SMC_LSI_RAID_OOB_SETUP_PROTOCOL*);
EFI_STATUS							SmcLsiRaidLib_AddDynamicItems(SMC_LSI_RAID_OOB_SETUP_PROTOCOL*);
EFI_STATUS 							SmcLsiRaidLib_ParseNvData(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* ,BOOLEAN ,CHAR16** ,EFI_GUID** ,UINTN** ,UINT8**	);
EFI_STATUS 							SmcLsiRaidLib_CollectData(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* ,CHAR16* ,EFI_GUID* ,UINTN ,UINT8* );
EFI_STATUS 							SmcLsiRaidLib_HandleData(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* );

VOID								DEBUG_PRINT_BUFFER(VOID* ,UINTN );




#endif
