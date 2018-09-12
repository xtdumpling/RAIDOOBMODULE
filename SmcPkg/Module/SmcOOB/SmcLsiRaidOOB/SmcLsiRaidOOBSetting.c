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
SMC_LSI_RAID_FORM_HEADER		mSmcLsiRaidFormRefSearchTable[] = {
	{RAID_3108, L"Main Menu"						, EFI_IFR_REF_OP },
	{RAID_3108, L"Configuration Management"			, EFI_IFR_REF_OP },
	{RAID_3108, L"Create Virtual Drive"				, EFI_IFR_REF_OP },
	{RAID_3108, L"Select Drives"					, EFI_IFR_REF_OP },
	{RAID_3108, L"Controller Management"			, EFI_IFR_REF_OP },
	{RAID_3108, L"Advanced Controller Properties"	, EFI_IFR_REF_OP },
	{RAID_3108, L"Virtual Drive Management"			, EFI_IFR_REF_OP },
	{RAID_3108, L"Drive Management"					, EFI_IFR_REF_OP },

	{RAID_NULL, L"SMC_LSI_RAID_END"					,			0xFF }
};

SMC_RAID_ITEMS_HEADER	mSmcLsiItemsTable[] = {
	// 3108
	{RAID_3108, L"Controller Management"	, L"Product Name", 			0x0 , FALSE},
	{RAID_3108, L"Controller Management"	, L"Serial Number",			0x0 , FALSE},
	{RAID_3108, L"Controller Management"	, L"Controller Status",		0x0 , FALSE},
	{RAID_3108, L"Controller Management"	, L"PCI ID",				0x0 , FALSE},
	{RAID_3108, L"Controller Management"	, L"PCI Slot Number",		0x0 , FALSE},
	{RAID_3108, L"Controller Management"	, L"Package Version",		0x0 , FALSE},
	{RAID_3108, L"Controller Management"	, L"Firmware Version",		0x0 , FALSE},
	{RAID_3108, L"Controller Management"	, L"Connector Count",		0x0 , FALSE},
	{RAID_3108, L"Controller Management"	, L"Drive Count",			0x0 , FALSE},
	{RAID_3108, L"Controller Management"	, L"Virtual Drive Count",	0x0 , FALSE},
	{RAID_3108, L"Select Drives"			, HARD_DRIVES_GROUP,	 0xA700 , FALSE},
	{RAID_3108, L"Virtual Drive Management"	, RAID_DRIVES_GROUP,     0xFFFF , FALSE},

	{RAID_3108, L"Advanced Controller Properties", L"Boot Mode",		0x0 , TRUE },
	{RAID_3108, L"Advanced Controller Properties", L"JBOD Mode",		0x0	, TRUE },

	{RAID_NULL, L"", L"", 0x0 }
};


SMC_LSI_AFTER_DOWN_FUNC		mSmcLsiAfterDwonFuncTable[] = {
	{ L"SmcLsiSetupDownDummyFunc"	, SmcLsiSetupDownDummyFunc	},
	{ L""							, NULL						}
};

SMC_LSI_AFTER_LOAD_FUNC		mSmcLsiAfterLoadFuncTable[] = {
	{ L"SmcLsiSetupLoadDummyFunc"	, SmcLsiSetupDownDummyFunc	},
	{ L""							, NULL						}
};
