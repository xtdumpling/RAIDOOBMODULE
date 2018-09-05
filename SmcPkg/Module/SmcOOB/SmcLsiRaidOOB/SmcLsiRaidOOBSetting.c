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
#include "SmcLsiRaidOOBSetupProtocol.h"
#include "SmcLsiRaidOOBSetupVfr.h"
#include "SmcLsiRaidOOBSetup.h"

SMC_LSI_RAID_NANE	RaidName[] = {
	{ RAID_3108, L"AVAGO MegaRAID <AVAGO 3108 MegaRAID>" },
	{ RAID_9260, L"AVAGO MegaRAID <AVAGO 9260 MegaRAID>" },
	{ RAID_NULL, L"" }
};
//Use the last line to restore FormId, QId, VId, VOff, Flag default. 
SMC_LSI_RAID_FORM		mSmcLsiRaidFormRefSearchTable[] = {
	{RAID_3108, L"Main Menu"						, EFI_IFR_REF_OP, 0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFF, FALSE},
	{RAID_3108, L"Configuration Management"			, EFI_IFR_REF_OP, 0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFF, FALSE},
	{RAID_3108, L"Create Virtual Drive"				, EFI_IFR_REF_OP, 0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFF, FALSE},
	{RAID_3108, L"Select Drives"					, EFI_IFR_REF_OP, 0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFF, FALSE},
	{RAID_3108, L"Controller Management"			, EFI_IFR_REF_OP, 0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFF, FALSE},
	{RAID_3108, L"Advanced Controller Properties"	, EFI_IFR_REF_OP, 0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFF, FALSE},
	{RAID_3108, L"Drive Management"					, EFI_IFR_REF_OP, 0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFF, FALSE},

	{RAID_NULL, L"SMC_LSI_RAID_END"					,			0xFF, 0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFF, FALSE}
};

SMC_LSI_ITEMS	mSmcLsiItemsTable[] = {
	// 3108
	{RAID_3108, L"Controller Management", L"Product Name", 			NULL },
	{RAID_3108, L"Controller Management", L"Serial Number",			NULL },
	{RAID_3108, L"Controller Management", L"Controller Status",		NULL },
	{RAID_3108, L"Controller Management", L"PCI ID",				NULL },
	{RAID_3108, L"Controller Management", L"PCI Slot Number",		NULL },
	{RAID_3108, L"Controller Management", L"Package Version",		NULL },
	{RAID_3108, L"Controller Management", L"Firmware Version",		NULL },
	{RAID_3108, L"Controller Management", L"Connector Count",		NULL },
	{RAID_3108, L"Controller Management", L"Drive Count",			NULL },
	{RAID_3108, L"Controller Management", L"Virtual Drive Count",	NULL },
	{RAID_NULL, L"", L"", NULL }
};

SMC_LSI_VAR	mSmcLsiVarTable[] = {
	{RAID_3108, NULL},

	//The last var line.
	{RAID_NULL,	NULL}
};

SMC_LSI_AFTER_DOWN_FUNC		mSmcLsiAfterDwonFuncTable[] = {
	{ L"SmcLsiSetupDownDummyFunc"	, SmcLsiSetupDownDummyFunc	},
	{ L""							, NULL						}
};

SMC_LSI_AFTER_LOAD_FUNC		mSmcLsiAfterLoadFuncTable[] = {
	{ L"SmcLsiSetupLoadDummyFunc"	, SmcLsiSetupDownDummyFunc	},
	{ L""							, NULL						}
};
