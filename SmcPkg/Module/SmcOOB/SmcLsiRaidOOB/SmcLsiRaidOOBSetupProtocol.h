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

#ifndef	_H_SmcLsiRaidOOB_Setup_Protocol_
#define	_H_SmcLsiRaidOOB_Setup_Protocol_

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Protocol/HiiDatabase.h>
#include <Protocol/HiiConfigAccess.h>

#define NAME_LENGTH	      				0x48
#define VAR_UNUSED_SPACE  				0x100
#define VAR_HASH_NUM	  				0x1F
#define EFI_IFR_EXTEND_OP_LABEL       	0x0
#define	ERROR_CODE_SIZE					32

#define EFI_IFR_TIANO_GUID \
  { 0xf0b1735, 0x87a0, 0x4193, 0xb2, 0x66, 0x53, 0x8c, 0x38, 0xaf, 0x48, 0xce }

#define SMC_LSI_DUMMY_GUID \
  { 0xFFFFFFFF, 0xFFFF, 0xFFFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }

#pragma pack(1)

typedef struct 	_SMC_LSI_RAID_OOB_SETUP_PROTOCOL_ 		SMC_LSI_RAID_OOB_SETUP_PROTOCOL;

typedef struct  _SMC_LSI_RAID_NAME_					SMC_LSI_RAID_NANE;
typedef struct 	_SMC_LSI_VAR_						SMC_LSI_VAR;
typedef struct  _SMC_LSI_VAR_SET_					SMC_LSI_VAR_SET;
typedef struct	_SMC_LSI_HII_HANDLE_				SMC_LSI_HII_HANDLE;
typedef struct 	_SMC_LSI_RAID_FORM_ 				SMC_LSI_RAID_FORM;
typedef struct  _SMC_LSI_RAID_ITEM_SET_				SMC_LSI_RAID_ITEM_SET;
typedef struct 	_SMC_LSI_ITEMS_						SMC_LSI_ITEMS;
typedef struct	_SMC_LSI_AFTER_DOWN_FUNC_ 			SMC_LSI_AFTER_DOWN_FUNC;
typedef struct	_SMC_LSI_AFTER_LOAD_FUNC_			SMC_LSI_AFTER_LOAD_FUNC;

typedef enum	_SMC_LSI_RAID_TYPE_					SMC_LSI_RAID_TYPE;
typedef enum	_SMC_LSI_RAID_CARD_INDEX_			SMC_LSI_RAID_CARD_INDEX;

struct _SMC_LSI_RAID_NAME_	{
	SMC_LSI_RAID_TYPE			LsiRaidTypeIndex;
	CHAR16						FormRaidName[NAME_LENGTH];
};

struct _SMC_LSI_RAID_ITEM_SET_ {
	SMC_LSI_RAID_CARD_INDEX		LsiRaidCardIndex;
	UINT16						SmcLsiVarId;
	EFI_IFR_OP_HEADER*			pLsiItemOp;
	SMC_LSI_RAID_ITEM_SET*		pNext;
};

struct _SMC_LSI_ITEMS_ {
	SMC_LSI_RAID_TYPE			LsiRaidTypeIndex;
	CHAR16						LsiItemForm[NAME_LENGTH];
	CHAR16						LsiItemName[NAME_LENGTH];
	SMC_LSI_RAID_ITEM_SET*		pLsiItemSet;
};

struct _SMC_LSI_RAID_FORM_ {
	SMC_LSI_RAID_TYPE			Lsi_RaidTypeIndex;

	CHAR16				Lsi_Name[NAME_LENGTH];
	UINT8				Lsi_OpCode;
	EFI_FORM_ID			Lsi_FormId;
	EFI_QUESTION_ID		Lsi_QId;
	EFI_VARSTORE_ID		Lsi_VId;
	UINT16				Lsi_Voff;
	UINT8				Lsi_Flags;
	BOOLEAN				BeUsed;
	BOOLEAN				HDDForm;
};

struct _SMC_LSI_VAR_SET_ {
	SMC_LSI_RAID_CARD_INDEX		LsiRaidCardIndex;
	CHAR8						SmcLsiVarName[NAME_LENGTH];
	EFI_GUID					SmcLsiVarGuid;
	UINT16						SmcLsiVarId;
	UINT16						SmcLsiVarSize;
	UINT8*						SmcLsiVarBuffer;
	SMC_LSI_VAR_SET*			pNext;
};

struct _SMC_LSI_VAR_ {
	SMC_LSI_RAID_TYPE			LsiRaidTypeIndex;
	SMC_LSI_VAR_SET*			pVarSet;
};

struct _SMC_LSI_HII_HANDLE_ {
	SMC_LSI_RAID_TYPE			RaidCardType;
	SMC_LSI_RAID_CARD_INDEX		RaidCardIndex;
	EFI_HII_HANDLE				RaidCardHiiHandle;

	UINT16						SmcFormId;
	EFI_STRING_ID				SmcFormTitleId;
	SMC_LSI_HII_HANDLE*	pNext;
};

typedef EFI_HII_HANDLE 	(*SMCLSI_GETHIIHANDLE) 		(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* );
typedef	EFI_GUID*	   	(*SMCLSI_GETFORMSETGUID)	(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* );
typedef EFI_STRING_ID	(*SMCLSI_GETFORMSETNAMEID)	(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* );
typedef UINT16			(*SMCLSI_GETFORMID)			(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* );
typedef UINT16			(*SMCLSI_GETQIDSTART)		(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* );
typedef UINT16			(*SMCLSI_GETQIDNOW)			(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* );
typedef UINT16			(*SMCLSI_GETFORMGOTOLABEL)	(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* );
typedef UINT16			(*SMCLSI_GETFORMLABEL)		(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* );
typedef UINT16			(*SMCLSI_GETFORMIDSTART)	(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* );
typedef UINT16			(*SMCLSI_GETFORMIDNOW)		(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* );
typedef UINT16			(*SMCLSI_GETVIDSTART)		(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* );
typedef UINT16			(*SMCLSI_GETVIDNOW)			(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* );
typedef UINT8*			(*SMCLSI_GETSETUPDATA)		(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* );
typedef UINT8*			(*SMCLSI_GETSETUPSTRING)	(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* );

typedef EFI_GUID*		(*SMCLSI_GETVARGUID)		(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* );

typedef EFI_STATUS		(*SMCLSI_AFTERDOWNFUNC)		(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* );
typedef EFI_STATUS		(*SMCLSI_AFTERLOADFUNC)		(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* );

struct	_SMC_LSI_AFTER_DOWN_FUNC_ {
	CHAR16					FuncName[NAME_LENGTH];
	SMCLSI_AFTERDOWNFUNC	Func;
};

struct	_SMC_LSI_AFTER_LOAD_FUNC_ {
	CHAR16					FuncName[NAME_LENGTH];
	SMCLSI_AFTERLOADFUNC	Func;
};

struct _SMC_LSI_RAID_OOB_SETUP_PROTOCOL_ {

	// SMC LSI RAID OOB SETUP Variables and functions
	// Record 8 error situations in each function.
	// The time sequence is High Bytes to Low Bytes.
	UINT64						SmcLsiDetailedStatus;
	EFI_STATUS					SmcSetupStatus;
	BOOLEAN						DetailedDebugMessage;

	SMC_LSI_RAID_NANE*			SmcLsiRaidNameTable;
	SMC_LSI_VAR*				SmcLsiVarTable;
	SMC_LSI_HII_HANDLE*			SmcLsiHiiHandleTable;
	SMC_LSI_RAID_FORM*			SmcLsiRaidFormRefSearchTable;
	SMC_LSI_ITEMS*				SmcLsiRaidItemsTable;
	SMC_LSI_AFTER_DOWN_FUNC*	SmcLsiAfterDownFuncTable;
	SMC_LSI_AFTER_LOAD_FUNC*	SmcLsiAfterLoadFuncTable;

	SMCLSI_GETHIIHANDLE			SmcLsiGetHiiHandle;
	SMCLSI_GETFORMSETGUID		SmcLsiGetFormSetGuid;
	SMCLSI_GETFORMSETNAMEID		SmcLsiGetFormSetNameId;
	SMCLSI_GETFORMID			SmcLsiGetFormId;
	SMCLSI_GETQIDSTART			SmcLsiGetQIdStart;
	SMCLSI_GETQIDNOW			SmcLsiGetQidNow;
	SMCLSI_GETFORMIDSTART		SmcLsiGetFormIdStart;
	SMCLSI_GETFORMIDNOW			SmcLsiGetFormIdNow;
	SMCLSI_GETVIDSTART			SmcLsiGetVIdStart;
	SMCLSI_GETVIDNOW			SmcLsiGetVIdNow;
	SMCLSI_GETVARGUID			SmcLsiGetVarGuid;
	SMCLSI_GETFORMGOTOLABEL		SmcLsiGetFormGoToLabel;
	SMCLSI_GETFORMLABEL			SmcLsiGetFormLabel;
	SMCLSI_GETSETUPDATA			SmcLsiGetSetupData;
	SMCLSI_GETSETUPSTRING		SmcLsiGetSetupString;

	//SMC LSI RAID OOB LIB Variables and functions
	SMC_LSI_HII_HANDLE*					SmcLsiCurrHiiHandleTable;
	EFI_HANDLE							SmcLsiCurrDriverHandle;	
	EFI_HII_CONFIG_ACCESS_PROTOCOL*		SmcLsiCurrConfigAccess;
	UINT8								HiiFlags;

	UINT8*								SmcLsiCurrRAIDSetupData;
	UINT32								SmcLsiCurrRAIDSetupDataAddedSize;
	//If you need, please add in below.
};

enum _SMC_LSI_RAID_TYPE_ {
	RAID_3108 	= 0,
	RAID_9260 	= 1,
	RAID_INTEL	= 2,
	RAID_NULL = 0xFF
};

enum _SMC_LSI_RAID_CARD_INDEX_ {
	RAIDCARD_FIRST 	= 0,
	RAIDCARD_SECOND	= 1,
	RAIDCARD_THIRD	= 2,
	RAIDCARD_NULL	= 0xFF
};

#pragma pack()

extern EFI_GUID	gSmcLsiRaidOobSetupProtocolGuid;

#endif
