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

#ifndef	_H_SmcLsiRaidOOB_SETUP_
#define	_H_SmcLsiRaidOOB_SETUP_

#include "Token.h"
//#include "SMCLSIAutoId.h"
#include "SmcLsiRaidOOBSetupStrDefs.h"
#include "SmcLsiRaidOOBSetupVfr.h"
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

#include <Protocol/SmcLsiRaidOOBSetupDriver.h>
#include "SmcLsiRaidOOBSetupProtocol.h"

#include "SmcLsiRaidOOBLib.h"
#include "SmcLsiRaidOOBSetting.h"

#define STRUCT_OFFSET(Type,val) ((UINT32)(((UINT32)(&(((Type*)0)->val))) - ((UINT32)0)))
#define STRUCT_START(Type,val,item) ((Type*)(((UINT32)item) - STRUCT_OFFSET(Type,val)))

#pragma pack(1)

typedef struct _SMC_LSI_RAID_OOB_SETUP_PRIVATE_	 SMC_LSI_RAID_OOB_SETUP_PRIVATE;

struct _SMC_LSI_RAID_OOB_SETUP_PRIVATE_{
	EFI_HANDLE		DriverHandle;
	EFI_HII_HANDLE	HIIHandle;
	EFI_GUID		FormSetGuid;
	EFI_STRING_ID	FormSetNameID;
	UINT16			FormID;
	UINT16			QIdStart;
	UINT16			FormGoToLabel;
	UINT16			FormLabel;
	UINT16			FormIdStart;
	EFI_GUID		VarGuid;
	UINT16			VarIdStart;
	UINT16			OtherLabelStart;
	CHAR16			HardDriveGroupsName[NAME_LENGTH];

	UINT8*			RaidSetupVfr;
	UINT8*			RaidSetupString;
	BOOLEAN			HaveRaidResource;

	SMC_LSI_RAID_OOB_SETUP_PROTOCOL	SmcLsiRaidOOBSetupProtocol;
	SMC_LSI_RAID_OOB_SETUP_DRIVER	SmcLsiRaidOOBSetupDriver;
};

#pragma pack()

EFI_STATUS 		SmcLsiSetupDownDummyFunc	(SMC_LSI_RAID_OOB_SETUP_PROTOCOL* );

UINT8	GetRaidIndex(SMC_LSI_RAID_TYPE );

#endif
