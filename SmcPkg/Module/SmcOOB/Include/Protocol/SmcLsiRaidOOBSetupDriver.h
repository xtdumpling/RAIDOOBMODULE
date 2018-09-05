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

#ifndef	_H_SmcLsiRaidOOB_Setup_Driver_
#define	_H_SmcLsiRaidOOB_Setup_Driver_

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Protocol/HiiDatabase.h>

#pragma pack(1)

typedef struct 	_SMC_LSI_RAID_OOB_SETUP_DRIVER_			SMC_LSI_RAID_OOB_SETUP_DRIVER;
typedef struct	_SMC_LSI_RAID_OOB_SETUP_NVSTRUCT_		SMC_LSI_RAID_OOB_SETUP_NVSTRUCT;


typedef EFI_STATUS		(*SMCLSI_SETUPDRIVERSTART)			(SMC_LSI_RAID_OOB_SETUP_DRIVER*	);
typedef EFI_STATUS		(*SMCLSI_SETUPDRIVERDOWNLOAD)		(SMC_LSI_RAID_OOB_SETUP_DRIVER* );
typedef	EFI_STATUS		(*SMCLSI_SETUPDRIVERLOADR)			(SMC_LSI_RAID_OOB_SETUP_DRIVER* );
typedef EFI_STATUS		(*SMCLSI_SETUPDRIVERGETNVDATA)		(SMC_LSI_RAID_OOB_SETUP_DRIVER* , BOOLEAN ,IN OUT SMC_LSI_RAID_OOB_SETUP_NVSTRUCT* );

struct _SMC_LSI_RAID_OOB_SETUP_DRIVER_ {
	SMCLSI_SETUPDRIVERSTART			SmcLsiSetupDriverStart;
	SMCLSI_SETUPDRIVERDOWNLOAD		SmcLsiSetupDriverDownload;
	SMCLSI_SETUPDRIVERLOADR			SmcLsiSetupDriverLoadR;
	SMCLSI_SETUPDRIVERGETNVDATA		SmcLsiSetupDriverGetNvData;
};

struct _SMC_LSI_RAID_OOB_SETUP_NVSTRUCT_ {
	CHAR16*		SmcRaidVarName;
	EFI_GUID*	SmcRaidVarGuid;
	UINTN*		SmcRaidVarSize;
	UINT8*		SmcRaidVarBuffer;
};

#pragma pack()

extern EFI_GUID	gSmcLsiRaidOobSetupDriverGuid;
#endif
