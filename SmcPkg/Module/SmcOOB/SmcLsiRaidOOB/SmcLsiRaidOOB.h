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

#ifndef	_H_SmcLsiRaidOOB_
#define	_H_SmcLsiRaidOOB_

#include "Token.h"
#include <AmiLib.h>
#include <AmiDxeLib.h>
#include <Guid/SmcOobPlatformPolicy.h>
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
#include <Protocol/SmcOobLibraryProtocol.h>

#include "SmcLsiRaidOOBSetupProtocol.h"
#include "SmcLsiRaidOOBLib.h"


#define SMC_LSI_ADD_ITEMS_MEM_SIZE				0x1000
#define SMC_LSI_THROUGH_FUNC_EXEC				0x112357

#define SMC_RAID_CMD_FORM_STRING				L"CMDFORM"
#define SMC_RAID_CMD_NAME_STRING				L"CMDXXX"
#define TEMP_FORM_STRING	512

#pragma pack(1)


typedef struct _SMC_LSI_ITEMS_PAIR_				SMC_LSI_ITEMS_PAIR;
typedef struct _EFI_IFR_GUID_LABEL_ 			EFI_IFR_GUID_LABEL;
typedef struct _SMC_LSI_ITEMS_COMMON_HEADER_	SMC_LSI_ITEMS_COMMON_HEADER;
typedef struct _SMC_LSI_ITEMS_MEM_ 				SMC_LSI_ITEMS_MEM;
typedef struct _SMC_LSI_INSIDE_DOWN_FUNC_		SMC_LSI_INSIDE_DOWN_FUNC;
typedef struct _SMC_LSI_INSIDE_LOAD_FUNC_		SMC_LSI_INSIDE_LOAD_FUNC;
typedef struct _SMC_LSI_INSIDE_CHANGE_FUNC_		SMC_LSI_INSIDE_CHANGE_FUNC;
typedef struct _HII_PACKAGE_LIST_FORM_SET_		HII_PACKAGE_LIST_FROM_SET;
typedef struct _RAID_CMD_PROCESSING_ITEM_		RAID_CMD_PROCESSING_ITEM;
typedef struct _RAID_CMD_PROCESSING_			RAID_CMD_PROCESSING;
typedef struct _RAID_CMD_PROCESSING_MAP_		RAID_CMD_PROCESSING_MAP;

typedef enum 	_RAID_CMD_PROCESSING_ENUM_				RAID_CMD_PROCESSING_ENUM;
typedef enum    _SMC_RAID_CMD_TYPE_ENUM_SUB_			SMC_RAID_CMD_TYPE_ENUM_SUB;
typedef enum 	_RAID_TYPE_CMD_PROCESSING_SPECIE_MAP_ 	RAID_TYPE_CMD_PROCESSING_SPECIE_MAP;

struct _HII_PACKAGE_LIST_FORM_SET_ {
	EFI_HII_PACKAGE_LIST_HEADER*	PackListHeader;
	EFI_HII_PACKAGE_HEADER*			PackFormHeader;
};


struct _EFI_IFR_GUID_LABEL_ {
  	EFI_IFR_OP_HEADER   Header;
  	EFI_GUID            Guid;
  	UINT8               ExtendOpCode;
  	UINT16              Number;
};

struct _SMC_LSI_ITEMS_COMMON_HEADER_ {
  	EFI_IFR_OP_HEADER        Header;
  	EFI_IFR_QUESTION_HEADER  Question;
};

struct _SMC_LSI_ITEMS_MEM_ {
	UINTN	ItemsBufferAlSize;
	UINT32	ItemsBufferCurSize;
	UINT8*	ItemsBuffer;

	UINT32	ItemsCopyBufferSize;
	UINT8*	ItemsCopyBuffer;
};

typedef EFI_STATUS		(*SMCLSI_INSIDEDOWNFUNC)		(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* );
typedef EFI_STATUS		(*SMCLSI_INSIDELOADFUNC)		(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* );
typedef	EFI_STATUS		(*SMCLSI_INSIDECHANGEFUNC)		(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* );

struct	_SMC_LSI_INSIDE_DOWN_FUNC_ {
	CHAR16					FuncName[NAME_LENGTH];
	SMCLSI_INSIDEDOWNFUNC	Func;
};

struct _SMC_LSI_INSIDE_LOAD_FUNC_ {
	CHAR16					FuncName[NAME_LENGTH];
	SMCLSI_INSIDELOADFUNC	Func;
};

struct _SMC_LSI_INSIDE_CHANGE_FUNC_ {
	CHAR16					FuncName[NAME_LENGTH];
	SMCLSI_INSIDECHANGEFUNC	Func;
};

#pragma pack()

EFI_STATUS							SettingErrorStatus					(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* ,UINT8 	,EFI_STATUS );
EFI_STATUS 							THROUGH_DEBUG_MESSAGE				(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* ,CHAR16*	,EFI_STATUS );


EFI_HII_CONFIG_ACCESS_PROTOCOL*		GetHiiConfigAccessProtocol			();
SMC_RAID_VAR* 						SetLsiVarBuffer_byString			(SMC_RAID_VAR* ,EFI_STRING );
SMC_RAID_VAR*						SetLsiVarBuffer						(SMC_RAID_VAR* ,UINT8* );
UINT16								VarHashByName						(CHAR8*);
UINT16								VarHashById							(UINT16	VarId);
SMC_RAID_VAR*						SearchLsiVarByName					(CHAR8*	,EFI_GUID* );
SMC_RAID_VAR*						SearchLsiVarById					(UINT16 );
SMC_RAID_VAR*						SearchInSmcSetupVarById				(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* ,UINT16 );
SMC_RAID_VAR*						SearchInSmcSetupVarByName			(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* ,CHAR8* , EFI_GUID* );
SMC_LSI_HII_HANDLE*					SearchHiiHandleTableByVarName		(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* ,CHAR8* , EFI_GUID* );
VOID*								GetQIdValue							(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* , UINT16 );
ITEMS_QID_VAL*						SearchItemsQIdValByQId				(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* , UINT16 );

VOID 								Debug_for_RaidChRecords				(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* );
EFI_STATUS							CallbackForTargetProcessing			(EFI_HII_CONFIG_ACCESS_PROTOCOL* ,EFI_BROWSER_ACTION ,EFI_QUESTION_ID ,UINT8 ,VOID*);
SMC_RAID_ITEMS_TYPE					GetHddTypeByCmd						(SMC_RAID_CMD_SECTION* );


HII_PACKAGE_LIST_FROM_SET* 			SmcLsiRaidOOB_GetCurrentPackageForm	(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* );


EFI_STATUS 							InitialLsiVarHashTable					(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* );
EFI_STATUS 							SmcLsiRaidOOB_InitialFunc				(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* );
EFI_STATUS 							SmcLsiRaidOOB_CollectInformation_Form	(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* );
EFI_STATUS 							SmcLsiRaidOOB_CollectInformation_Items	(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* );
EFI_STATUS							SmcLsiRaidOOB_CollectInformation_ChRec	(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* );
EFI_STATUS							SmcLsiHookBrowser2Protocol				(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* );
EFI_STATUS							SmcLsiCallbackAccessMenu				(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* );
EFI_STATUS							SmcLsiSetSmcLsiVariableTable			(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* );
EFI_STATUS 							FreeLsiVarHashTable						(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* );
EFI_STATUS							SmcLsiCreateSmcRaidVarAndItems			(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* );
EFI_STATUS							InitialItemsQIdTable					(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* );
EFI_STATUS							Debug_for_ItemsQIdTable					(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* );

EFI_STATUS							InsertRaidSetupVariable					(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* );
EFI_STATUS 							InsertRaidSetupFormGoto					(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* );
EFI_STATUS 							InsertRaidSetupFormItems				(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* );
EFI_STATUS 							InsertRaidSetupChangeItems				(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* );
EFI_STATUS 							InsertRaidSetupHDGItems					(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* );
EFI_STATUS 							InsertRaidSetupSmcCmdsAndItems			(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* );

EFI_STATUS							CheckChangeableItemsInChangedVar		(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* );
EFI_STATUS							AccessRAIDRecordtoChangeSetting			(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* );
EFI_STATUS							ParseRaidCfgCmdString					(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* );
EFI_STATUS							CollectCfgCmdData						(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* );
EFI_STATUS 							ChangedVarToOOBVarBuffer				(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* );
EFI_STATUS 							ExamineBasicForCmdRequire				(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* );
EFI_STATUS							HandleRaidCfgCmdString					(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* );

EFI_STATUS							HandleRaidCmdSub_ChangeSetting(
	SMC_LSI_RAID_OOB_SETUP_PROTOCOL* ,
	SMC_RAID_CMD_SET* , 
	RAID_CMD_PROCESSING_ITEM* , 
	RAID_CMD_PROCESSING* );

EFI_STATUS 							HandleRaidCmdSub_EnterForm(
	SMC_LSI_RAID_OOB_SETUP_PROTOCOL* ,
	SMC_RAID_CMD_SET* , 
	RAID_CMD_PROCESSING_ITEM* ,
	RAID_CMD_PROCESSING* );

EFI_STATUS 							HandleRaidCmdSub_PressAction(
	SMC_LSI_RAID_OOB_SETUP_PROTOCOL* ,
	SMC_RAID_CMD_SET* , 
	RAID_CMD_PROCESSING_ITEM* ,
	RAID_CMD_PROCESSING* );

EFI_STATUS 							HandleRaidCmdSub_Confirm(
	SMC_LSI_RAID_OOB_SETUP_PROTOCOL* ,
	SMC_RAID_CMD_SET* , 
	RAID_CMD_PROCESSING_ITEM* ,
	RAID_CMD_PROCESSING* );

EFI_STATUS 							HandleRaidCmdSub_ChangeRaidType(
	SMC_LSI_RAID_OOB_SETUP_PROTOCOL* ,
	SMC_RAID_CMD_SET* , 
	RAID_CMD_PROCESSING_ITEM* ,
	RAID_CMD_PROCESSING* );

EFI_STATUS 							HandleRaidCmdSub_ChoiceHdds(
	SMC_LSI_RAID_OOB_SETUP_PROTOCOL* ,
	SMC_RAID_CMD_SET* , 
	RAID_CMD_PROCESSING_ITEM* ,
	RAID_CMD_PROCESSING* ,
	SMC_RAID_CMD_GROUP*	);


EFI_STATUS							HandleRaidCmdSub(
	SMC_LSI_RAID_OOB_SETUP_PROTOCOL* , 
	SMC_RAID_CMD_SET* , 
	RAID_CMD_PROCESSING_MAP* , 
	SMC_RAID_CMD_GROUP* );

EFI_STATUS							HandleBuildRaidCmd						(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* , SMC_RAID_CMD_SET* );
EFI_STATUS							HandleBuildRaidCmd_D					(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* , SMC_RAID_CMD_SET* );
EFI_STATUS							HandleOtherRaidCmd						(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* , SMC_RAID_CMD_SET* );


EFI_STATUS EFIAPI SmcLsiHookBrower2Callback(
  IN CONST  EFI_FORM_BROWSER2_PROTOCOL*,
  IN OUT    UINTN*,
  IN OUT    EFI_STRING,
  IN CONST  BOOLEAN,
  IN CONST  EFI_GUID*, 
  IN CONST  CHAR16* 
);

#define P_ENUM_ENTERFORM_SET					{ P_RAID_ENTER_FORM				, SMC_CMD_RAID_NON 			, TES_NON }
#define P_ENUM_ENTERFORM_SET_SPANSRAID			{ P_RAID_ENTER_FORM				, SMC_CMD_RAID_RAIDTYPE 	, TES_BUILD_SPANS_RAID }

#define P_ENUM_CHOICEHDD_SET					{ P_RAID_CHOICE_HDD				, SMC_CMD_RAID_COMMAND 	, TES_NON }
#define P_ENUM_CHOICEHDD_SET_DELRAID			{ P_RAID_CHOICE_HDD				, SMC_CMD_RAID_COMMAND 	, TES_DELETE_RAID_DRIVE }
#define P_ENUM_CONFIRM_SET						{ P_RAID_CONFIRM				, SMC_CMD_RAID_NON 		, TES_NON }
#define P_ENUM_PRESSACTION_SET					{ P_RAID_PRESS_ACTION			, SMC_CMD_RAID_NON 		, TES_NON }
#define P_ENUM_NONACTION_SET					{ P_RAID_NON_ACTION				, SMC_CMD_RAID_NON 		, TES_NON }

#define P_ENUM_CHANGESETTING_SET_SIZE_UNIT		{ P_RAID_CHANGE_SETTING 		, SMC_CMD_RAID_SIZE 	, TES_CHANGE_RAIDSIZE_UNIT }
#define P_ENUM_CHANGESETTING_SET_SIZE_SIZE		{ P_RAID_CHANGE_SETTING 		, SMC_CMD_RAID_SIZE 	, TES_CHANGE_RAIDSIZE_SIZE }
#define P_ENUM_CHANGESETTING_SET_RAIDTYPE		{ P_RAID_CHANGE_SETTING 		, SMC_CMD_RAID_RAIDTYPE , TES_NON }
#define P_ENUM_CHANGESETTING_SET_DELRAID		{ P_RAID_CHANGE_SETTING 		, SMC_CMD_RAID_COMMAND 	, TES_DELETE_RAID_DRIVE }

#define P_ENUM_TEST_FORMITEMS					{ P_RAID_GET_TEST_FORM_ITEMS	, SMC_CMD_RAID_NON 			, TES_NON }

enum _RAID_CMD_PROCESSING_ENUM_ {
	P_RAID_ENTER_FORM 			= 0,
	P_RAID_CHOICE_HDD			= 1,
	P_RAID_CHANGE_SETTING		= 2,
	P_RAID_CONFIRM				= 3,
	P_RAID_PRESS_ACTION			= 4,
	P_RAID_GET_TEST_FORM_ITEMS	= 5,
	P_RAID_NON_ACTION			= 0xFF
};

enum _SMC_RAID_CMD_TYPE_ENUM_SUB_ {
	TES_CHANGE_RAIDSIZE_UNIT		= 0,
	TES_CHANGE_RAIDSIZE_SIZE		= 1,
	TES_CHANGE_RAIDSIZE_END			= 2,
	TES_DELETE_RAID_DRIVE			= 3,
	TES_BUILD_SPANS_RAID			= 4,
	TES_NON							= 0xFF
};

enum _RAID_TYPE_CMD_PROCESSING_SPECIE_MAP_ {
	SINGLE_BUILDRAID_MAP_TABLE,
	SPANS_BUILDRAID_MAP_TABLE_FIRST,
	SPANS_BUILDRAID_MAP_TABLE_SECOND,
	SPANS_BUILDRAID_MAP_TABLE_THIRD,
	SPANS_BUILDRAID_MAP_TABLE_Forth,
	SPANS_BUILDRAID_MAP_TABLE_Fifth,
	DELETERAID_MAP_TABLE_RAIDREF,
	MAKEJBOD_MAP_TABLE,
	UNCONFIGUREDJBOD_MAP_TABLE
};

struct _RAID_CMD_PROCESSING_ITEM_ {
	EFI_IFR_OP_HEADER*			ItemOpHeader;
	RAID_CMD_PROCESSING_ITEM*	pItemNext;
};

struct _RAID_CMD_PROCESSING_ {
	RAID_CMD_PROCESSING_ENUM	CmdProcess;
	SMC_RAID_CMD_TYPE_ENUM		TypeEnum;
	SMC_RAID_CMD_TYPE_ENUM_SUB	TypeEnumSub;
};

struct _RAID_CMD_PROCESSING_MAP_ {
	RAID_CMD_PROCESSING			CmdProcessSet;
	CHAR16						CmdProcessLastForm[NAME_LENGTH];
	UINT16						LastFormId;
	CHAR16						CmdProcessTargetName[NAME_LENGTH];
	EFI_QUESTION_ID				CmdProcessLimitQId;
	UINT8						CmdProcessTargetOpCode;
	UINT8						TableItemIndex;
};

typedef EFI_STATUS	(*SMCLSI_RAIDCMDFUNC)	(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* , SMC_RAID_CMD_SET*);

typedef struct 	_SMCLSI_RAIDCMDFUNCTION_MAP_ 	SMCLSI_RAIDCMDFUNCTION_MAP;
struct _SMCLSI_RAIDCMDFUNCTION_MAP_ {
	SMC_LSI_RAID_TYPE			RaidCardType;
	SMCLSI_RAIDCMDFUNC			RaidFunc;
};



#if defined(DEBUG_MODE) && (DEBUG_MODE == 1)
	#define SMC_RAID_DETAIL_DEBUG(x)\
		do {\
			if(!!mDetailedDebugMessage){\
				DEBUG(x);\
			}\
		}while(FALSE)
#else
	#define SMC_RAID_DETAIL_DEBUG(x) 
#endif

#define RETURN_IFERROR() do { if(EFI_ERROR(Status)) return Status; }while(FALSE)

#endif
