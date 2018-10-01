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

#ifndef	_H_SmcLsiRaidOOB_Setup_Protocol_
#define	_H_SmcLsiRaidOOB_Setup_Protocol_

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Protocol/HiiDatabase.h>
#include <Protocol/HiiConfigAccess.h>

#define NAME_LENGTH	      				0x80
#define VAR_UNUSED_SPACE  				0x100
#define VAR_HASH_NUM	  				0x1F
#define EFI_IFR_EXTEND_OP_LABEL       	0x0
#define	ERROR_CODE_SIZE					32

#define HARD_DRIVES_GROUP				L"HARD DRIVES GROUP"
#define RAID_DRIVES_GROUP				L"RAID DRIVES GROUP"
#define JBOD_DRIVES_GROUP				L"JBOD DRIVES GROUP"

//Format will be NAME_X_Y, X ::= CARD TYPE, Y ::= CARD INDEX
#define FOR_SMC_ITEMS_VAR_NAME			"SMCW_ITEMS_VAR"

//Format will be NAME_S_X_Y, S ::= RAID VAR Original Name, X ::= CARD TYPE, Y ::= CARD INDEX
#define FOR_RAID_ITEMS_VAR_NAME			"SMCR"

#define EFI_IFR_TIANO_GUID \
  { 0xf0b1735, 0x87a0, 0x4193, 0xb2, 0x66, 0x53, 0x8c, 0x38, 0xaf, 0x48, 0xce }

#define SMC_LSI_DUMMY_GUID \
  { 0xFFFFFFFF, 0xFFFF, 0xFFFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }

#pragma pack(1)

typedef struct 	_SMC_LSI_RAID_OOB_SETUP_PROTOCOL_ 		SMC_LSI_RAID_OOB_SETUP_PROTOCOL;

typedef struct  _SMC_LSI_RAID_NAME_					SMC_LSI_RAID_NANE;
typedef struct  _SMC_RAID_VAR_						SMC_RAID_VAR;
typedef struct  _SMC_SETUP_RAID_VAR_				SMC_SETUP_RAID_VAR;
typedef struct  _SMC_RAID_VAR_HASH_					SMC_RAID_VAR_HASH;
typedef struct	_SMC_LSI_HII_HANDLE_				SMC_LSI_HII_HANDLE;

typedef struct 	_SMC_LSI_RAID_FORM_HEADER_ 			SMC_LSI_RAID_FORM_HEADER;
typedef struct 	_SMC_LSI_RAID_FORM_BODY_			SMC_LSI_RAID_FORM_BODY;
typedef struct 	_SMC_LSI_RAID_FORM_SET_				SMC_LSI_RAID_FORM_SET;

typedef struct  _SMC_RAID_ITEMS_HEADER_				SMC_RAID_ITEMS_HEADER;
typedef struct 	_SMC_RAID_ITEMS_BODY_				SMC_RAID_ITEMS_BODY;
typedef struct 	_SMC_RAID_ITEMS_SET_				SMC_RAID_ITEMS_SET;

typedef struct  _SMC_CHANGED_VAR_HEADER_			SMC_CHANGED_VAR_HEADER;
typedef struct  _SMC_CHANGED_VAR_BODY_				SMC_CHANGED_VAR_BODY;
typedef struct 	_SMC_CHANGED_VAR_SET_				SMC_CHANGED_VAR_SET;

typedef struct 	_SMC_RAID_CHRECORD_HEADER_			SMC_RAID_CHRECORD_HEADER;
typedef struct 	_SMC_RAID_CHRECORD_BODY_			SMC_RAID_CHRECORD_BODY;
typedef struct 	_SMC_RAID_CHRECORD_SET_				SMC_RAID_CHRECORD_SET;

typedef struct	_TABLE_COMMON_HEADER_  				TABLE_COMMON_HEADER;

typedef struct  _SMC_ITEMS_VAR_DATA_ 				SMC_ITEMS_VAR_DATA;

typedef struct	_SMC_LSI_AFTER_DOWN_FUNC_ 			SMC_LSI_AFTER_DOWN_FUNC;
typedef struct	_SMC_LSI_AFTER_LOAD_FUNC_			SMC_LSI_AFTER_LOAD_FUNC;

typedef enum	_SMC_LSI_RAID_TYPE_					SMC_LSI_RAID_TYPE;
typedef enum	_SMC_LSI_RAID_CARD_INDEX_			SMC_LSI_RAID_CARD_INDEX;
typedef enum 	_SMC_CHANGED_VAR_TYPE_				SMC_CHANGED_VAR_TYPE;
typedef enum	_SMC_RAID_ITEMS_TYPE_				SMC_RAID_ITEMS_TYPE;
typedef enum 	_SMC_RAID_CMD_SPECIE_ENUM_			SMC_RAID_CMD_SPECIE_ENUM;
typedef enum	_SMC_RAID_CMD_TYPE_ENUM_			SMC_RAID_CMD_TYPE_ENUM;
typedef enum 	_SMC_RAID_CMD_JBOD_ENUM_ 			SMC_RAID_CMD_JBOD_ENUM;
typedef enum 	_SMC_RAID_CMD_RAIDTYPE_ENUM_		SMC_RAID_CMD_RAIDTYPE_ENUM;

struct _SMC_LSI_RAID_NAME_	{
	SMC_LSI_RAID_TYPE			LsiRaidTypeIndex;
	CHAR16						FormRaidName[NAME_LENGTH];
};

struct _SMC_LSI_RAID_FORM_HEADER_ {
	SMC_LSI_RAID_TYPE			LsiRaidTypeIndex;
	CHAR16						Lsi_FormName[NAME_LENGTH];
	UINT8						Lsi_OpCode;
};

struct _SMC_LSI_RAID_FORM_BODY_ {
	EFI_FORM_ID				Lsi_FormId;
	EFI_QUESTION_ID			Lsi_QId;
	EFI_VARSTORE_ID			Lsi_VId;
	UINT16					Lsi_Voff;
	UINT8					Lsi_Flags;
	BOOLEAN					BeUsed;
};

struct _SMC_LSI_RAID_FORM_SET_ {
	SMC_LSI_RAID_FORM_HEADER		FormHeader;		
	SMC_LSI_RAID_FORM_BODY			FormBody;
	SMC_LSI_RAID_FORM_SET*			pFormNext;
};

struct _SMC_RAID_CHRECORD_HEADER_ {
	SMC_LSI_RAID_TYPE			LsiRaidTypeIndex;
	CHAR16						ChRFrom[NAME_LENGTH];
	CHAR16						ChRTarget[NAME_LENGTH];
};

struct _SMC_RAID_CHRECORD_BODY_ {
	UINT8					TargetOpCode;
	EFI_QUESTION_ID			TargetQId;
	EFI_VARSTORE_ID			TargetVId;
	BOOLEAN					BeAccessed;
};

struct _SMC_RAID_CHRECORD_SET_ {
	SMC_RAID_CHRECORD_HEADER	ChRecordHeader;
	SMC_RAID_CHRECORD_BODY		ChRecordBody;
	SMC_RAID_CHRECORD_SET*		ChRecordNext;
};

struct _SMC_RAID_ITEMS_HEADER_ {
	SMC_LSI_RAID_TYPE			LsiRaidTypeIndex;
	CHAR16						LsiItemForm[NAME_LENGTH];
	CHAR16						LsiItemName[NAME_LENGTH];
	UINT16						LsiItemId;					//Temporary use this method to identifier HDD QId.
	SMC_RAID_ITEMS_TYPE			RaidItemsType;				//Means Item come from RAID CARD can be changeable or not.
};

struct _SMC_RAID_ITEMS_BODY_ {
	UINT16						SmcLsiVarId;
	UINT16						HdNum;
	CHAR16*						ItemsName;
	CHAR16*						ItemsHelp;
	EFI_IFR_OP_HEADER*			pLsiItemOp;
	SMC_RAID_ITEMS_BODY*		pItemsBodyNext;
};

struct _SMC_RAID_ITEMS_SET_ {
	SMC_RAID_ITEMS_HEADER		ItemsHeader;
	SMC_RAID_ITEMS_BODY*		ItemsBody;
	SMC_RAID_ITEMS_SET*			pItemsNext;
};

struct	_TABLE_COMMON_HEADER_ {
	SMC_LSI_RAID_TYPE			LsiRaidTypeIndex;
};

//VarHashTable contain Original RAID VAR and SMC RAID VAR.
struct _SMC_RAID_VAR_ {
	CHAR8				RaidVarName[NAME_LENGTH];
	EFI_GUID			RaidVarGuid;
	EFI_VARSTORE_ID		RaidVarId;
	UINT16				RaidVarSize;
	UINT8*				RaidVarBuffer;
};

struct _SMC_SETUP_RAID_VAR_ {
	SMC_RAID_VAR				SetupRaidVar;
	SMC_SETUP_RAID_VAR*			pSetupRaidVarNext;
};

struct _SMC_RAID_VAR_HASH_ {
	SMC_RAID_VAR		RaidVar;
	SMC_RAID_VAR_HASH*	pRaidpNameNext;
	SMC_RAID_VAR_HASH*	pRaidpIdNext;
};

struct _SMC_CHANGED_VAR_HEADER_ {
	CHAR8					ChangedVarName[NAME_LENGTH];
	EFI_GUID				ChangedVarGuid;
	UINTN					ChangedVarLength;
};

struct _SMC_CHANGED_VAR_BODY_ {
	SMC_CHANGED_VAR_TYPE	ChangedVarType;
	BOOLEAN					BeUpdated;
	CHAR8					ChangedVarOriName[NAME_LENGTH];
};

struct _SMC_CHANGED_VAR_SET_ {

	SMC_CHANGED_VAR_HEADER	ChVarHeader;
	SMC_CHANGED_VAR_BODY	ChVarBody;

	UINT8*					ChangedVarBuffer;
	SMC_CHANGED_VAR_SET*	pChangedVarNext;
};

typedef enum  _RAID_SIZE_TYPE_ 		RAID_SIZE_TYPE;
enum _RAID_SIZE_TYPE_ {
	SIZE_TB_TYPE	= 0,
	SIZE_GB_TYPE	= 1,

	SIZE_NON_TYPE	= 0xFF
};

typedef struct  _SMC_RAID_SIZE_TYPE_MAP_	SMC_RAID_SIZE_TYPE_MAP;

typedef struct 	_SMC_RAID_CMD_GROUP_ 		SMC_RAID_CMD_GROUP;
typedef struct 	_SMC_RAID_CMD_RAIDTYPE_ 	SMC_RAID_CMD_RAIDTYPE;
typedef struct 	_SMC_RAID_CMD_RAIDSIZE_ 	SMC_RAID_CMD_RAIDSIZE;
typedef struct 	_SMC_RAID_CMD_SECTION_ 		SMC_RAID_CMD_SECTION;
typedef struct 	_SMC_RAID_CMD_SET_ 			SMC_RAID_CMD_SET;
typedef struct  _SMC_RAID_CMD_SPECIE_		SMC_RAID_CMD_SPECIE;
typedef struct  _SMC_RAID_CMD_JBOD_			SMC_RAID_CMD_JBOD;
typedef struct  _SMC_RAID_CMD_JBOD_MAP_ 	SMC_RAID_CMD_JBOD_MAP;
typedef struct	_SMC_RAID_CMD_SPECIE_MAP_	SMC_RAID_CMD_SPECIE_MAP;

struct	_SMC_RAID_CMD_JBOD_MAP_ {
	CHAR8						JBODCode;
	SMC_RAID_CMD_JBOD_ENUM		JBODEnum;
};

struct _SMC_RAID_CMD_JBOD_ {
	SMC_RAID_CMD_JBOD_ENUM		RaidCmdJbodEnum;
};

struct _SMC_RAID_SIZE_TYPE_MAP_ {
	RAID_SIZE_TYPE		RaidSizeType;
	CHAR8				RaidSizeStr[5];
};

struct _SMC_RAID_CMD_SPECIE_ {
	SMC_RAID_CMD_SPECIE_ENUM	RaidCmdSpecie;
};

struct _SMC_RAID_CMD_SPECIE_MAP_ {
	CHAR8						SpecieCode;
	SMC_RAID_CMD_SPECIE_ENUM	SpecieType;
};

#define SMC_RAID_CMD_GROUP_TYPE_END	0xFF
#define T_SHORT_END	SMC_RAID_CMD_GROUP_TYPE_END

typedef struct _SMC_RAID_CMD_GROUP_HDD_ 	SMC_RAID_CMD_GROUP_HDD;

struct _SMC_RAID_CMD_GROUP_HDD_ {
	UINT16						HdNum;
	SMC_RAID_ITEMS_TYPE			HddType;
	CHAR16*						HdHame;
	SMC_RAID_CMD_GROUP_HDD*		pHddNext;
};

struct _SMC_RAID_CMD_GROUP_ {
	SMC_RAID_CMD_GROUP_HDD*		GroupHddSet;
	BOOLEAN						bEnabled;
};

struct _SMC_RAID_CMD_RAIDTYPE_ {
	SMC_RAID_CMD_RAIDTYPE_ENUM		RaidType;
	BOOLEAN							bEnabled;
};

typedef struct  _SMC_RAID_CMD_RAIDTYPE_TO_VALUE_		SMC_RAID_CMD_RAIDTYPE_TO_VALUE;
typedef struct  _SMC_RAID_CMD_RAIDTYPE_TO_VALUE_MAP_	SMC_RAID_CMD_RAIDTYPE_TO_VALUE_MAP;

struct _SMC_RAID_CMD_RAIDTYPE_TO_VALUE_ {
	SMC_RAID_CMD_RAIDTYPE_ENUM		RaidType;
	UINT8							RaidValue;
};

struct _SMC_RAID_CMD_RAIDTYPE_TO_VALUE_MAP_ {
	SMC_LSI_RAID_TYPE					RaidCardType;
	SMC_RAID_CMD_RAIDTYPE_TO_VALUE		RaidTypeValueTable[10];
};

#define CHANGE_RAIDSIZE_UNIT	0x00
#define CHANGE_RAIDSIZE_SIZE	0x01
#define CHANGE_RAIDSIZE_END		0x02


struct _SMC_RAID_CMD_RAIDSIZE_ {
	RAID_SIZE_TYPE		RaidSizeType;
	CHAR16				RaidSizeContext[80];
	UINT8				ChangeSequence;
	BOOLEAN				bEnabled;
};

struct _SMC_RAID_CMD_SECTION_ {
	SMC_RAID_CMD_TYPE_ENUM		RaidCmdType;
	VOID*						RaidCmdBody;
	SMC_RAID_CMD_SECTION*		pRaidCmdSectionNext;
};

struct _SMC_RAID_CMD_SET_ {
	SMC_RAID_CMD_SECTION*	RaidCmdSection;
	BOOLEAN					CmdEnable;
	SMC_RAID_CMD_SET*		pRaidCmdSetNext;
};

#define SMC_ITEM_CMD_STRING_SIZE 255

struct _SMC_ITEMS_VAR_DATA_ {
	CHAR16	CMD1[SMC_ITEM_CMD_STRING_SIZE];
	CHAR16	CMD2[SMC_ITEM_CMD_STRING_SIZE];
	CHAR16	CMD3[SMC_ITEM_CMD_STRING_SIZE];

	UINT8	Reserve[128];
};

struct _SMC_LSI_HII_HANDLE_ {
	//RaidOOBSetup Initial Below data.
	SMC_LSI_RAID_TYPE			RaidCardType;
	SMC_LSI_RAID_CARD_INDEX		RaidCardIndex;
	EFI_HII_HANDLE				RaidCardHiiHandle;
	SMC_LSI_HII_HANDLE*			pNext;

	//RaidOOBSetup initial Header, RaidOOBLib initial Body.
	SMC_LSI_RAID_FORM_SET*		RaidCardAccessForms;
	SMC_RAID_ITEMS_SET*			RaidCardInfItems;
	SMC_RAID_CHRECORD_SET*		RaidCardChRecordTable;

	//RaidOOBLib initial below items.
	EFI_HANDLE							RaidCardDriverHandle;
	EFI_HII_CONFIG_ACCESS_PROTOCOL*		SmcLsiCurrConfigAccess;
	UINT8								HiiFlags;

	SMC_RAID_VAR_HASH*				RaidLsiVarHashTableName	[VAR_HASH_NUM];
	SMC_RAID_VAR_HASH*				RaidLsiVarHashTableVarId[VAR_HASH_NUM];
	SMC_SETUP_RAID_VAR*				RaidSetupVarSet;
	
	SMC_CHANGED_VAR_SET*			RaidChangedVarSet;
	
	SMC_RAID_CMD_SET*				RaidCmdSet;
	//When Create Form for this Handle, Initial Below data.
	UINT16						SmcFormId;
	EFI_STRING_ID				SmcFormTitleId;
	UINT16						ChangeItemsLabel;
	UINT16						HDGLabel;
	UINT16						BuildCfgLabel;
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
typedef UINT16			(*SMCLSI_GETOLABSTART)		(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* );
typedef UINT16			(*SMCLSI_GETOLABNOW)		(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* );
typedef UINT8*			(*SMCLSI_GETSETUPDATA)		(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* );
typedef UINT8*			(*SMCLSI_GETSETUPSTRING)	(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* );

typedef EFI_GUID*		(*SMCLSI_GETVARGUID)			(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* );
typedef CHAR16*			(*SMCLSI_GETHDGNAME)			(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* );
typedef CHAR16*			(*SMCLSI_GETRDGNAME)			(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* );
typedef CHAR16*			(*SMCLSI_GETJBODNAME)			(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* );
typedef CHAR8*			(*SMCLSI_GETSMCITESMVARNAME)	(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* );
typedef CHAR8*			(*SMCLSI_GETRAIDVARNAME)		(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* );
typedef UINT16			(*SMCLSI_GETSMCRAIDVARSIZE)		(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* );
typedef UINT32*			(*SMCLSI_GETSMCRAIDCMDOFFSET)	(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* );

typedef SMC_RAID_CMD_RAIDTYPE_TO_VALUE_MAP* 	(*SMCLSI_GETSMCRAIDTYPETOVALUEMAP)	(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* );

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
	SMC_LSI_HII_HANDLE*			SmcLsiHiiHandleTable;
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

	SMCLSI_GETHDGNAME			SmcLsiGetHdgName;
	SMCLSI_GETRDGNAME			SmcLsiGetRdgName;
	SMCLSI_GETJBODNAME			SmcLsiGetJBODName;
	SMCLSI_GETSMCITESMVARNAME	SmcLsiGetSmcItemsVarName;
	SMCLSI_GETRAIDVARNAME		SmcLsiGetRaidVarName;
	SMCLSI_GETSMCRAIDVARSIZE	SmcLsiGetSmcRaidVarSize;
	SMCLSI_GETSMCRAIDCMDOFFSET	SmcLsiGetSmcRaidCmdOffset;

	SMCLSI_GETSMCRAIDTYPETOVALUEMAP 	SmcLsiGetSmcRaidTypeToValueMap;

	SMCLSI_GETOLABSTART			SmcLsiGetOLabelStart;
	SMCLSI_GETOLABNOW			SmcLsiGetOLabelNow;
	

	//SMC LSI RAID OOB LIB Variables and functions
	SMC_LSI_HII_HANDLE*					SmcLsiCurrHiiHandleTable;
	UINT8*								SmcLsiCurrRAIDSetupData;
	UINT32								SmcLsiCurrRAIDSetupDataAddedSize;

	BOOLEAN								SmcLsiHaveNvData;
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

enum _SMC_CHANGED_VAR_TYPE_ {
	VAR_NON_TYPE 	= 0,
	VAR_RAID_TYPE	= 1,
	VAR_SMC_TYPE	= 2,
	VAR_NULLTYPE	= 0xFF
};

enum _SMC_RAID_ITEMS_TYPE_ {
	RAID_INFORMATION_TYPE 	= 0,
	RAID_CHANGEABLE_TYPE	= 1,
	RAID_HDG_TYPE			= 2,
	RAID_RDG_TYPE			= 3,
	RAID_JBOD_TYPE			= 4,
	RAID_SMCCMD_TYPE		= 5,
	RAID_NULL_TYPE			= 0xFF
};

enum _SMC_RAID_CMD_TYPE_ENUM_ {
	SMC_CMD_RAID_GROUP 		= 0,
	SMC_CMD_RAID_RAIDTYPE 	= 1,
	SMC_CMD_RAID_SIZE 		= 2,
	SMC_CMD_RAID_COMMAND	= 3,
	SMC_CMD_RAID_JBOD		= 5,
	SMC_CMD_RAID_NON 		= 0xFF
};

enum _SMC_RAID_CMD_SPECIE_ENUM_ {
	SMC_CMD_SPECIE_BUILD	= 0,
	SMC_CMD_SPECIE_DELETE 	= 1,
	SMC_CMD_SPECIE_OTHER	= 2,
	SMC_CMD_SPECIE_NON 		= 0xFF
};

enum _SMC_RAID_CMD_JBOD_ENUM_ {
	SMC_CMD_JBOD_MAKE		= 0,
	SMC_CMD_JBOD_UNCONFIG 	= 1,
	SMC_CMD_JBOD_NON		= 0xFF,
};

enum _SMC_RAID_CMD_RAIDTYPE_ENUM_ {
	SMC_CMD_RAIDTYPE_R0		= 0,
	SMC_CMD_RAIDTYPE_R1 	= 1,
	SMC_CMD_RAIDTYPE_R5		= 5,
	SMC_CMD_RAIDTYPE_R6		= 6,
	SMC_CMD_RAIDTYPE_NOSPAN = 9,
	SMC_CMD_RAIDTYPE_R10	= 10,
	SMC_CMD_RAIDTYPE_R00	= 100,
	SMC_CMD_RAIDTYPE_R50	= 50,
	SMC_CMD_RAIDTYPE_R60	= 60,

	SMC_CMD_RAIDTYPE_NON	= 0xFF
};

#pragma pack()

extern EFI_GUID	gSmcLsiRaidOobSetupProtocolGuid;

#endif
