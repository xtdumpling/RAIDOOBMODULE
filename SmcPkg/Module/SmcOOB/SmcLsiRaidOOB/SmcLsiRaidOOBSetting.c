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
	{RAID_3108, L"Make JBOD"						, EFI_IFR_REF_OP },
	{RAID_3108, L"Make Unconfigured Good"			, EFI_IFR_REF_OP },
	{RAID_3108, L"Controller Management"			, EFI_IFR_REF_OP },
	{RAID_3108, L"Advanced Controller Properties"	, EFI_IFR_REF_OP },
	{RAID_3108, L"Virtual Drive Management"			, EFI_IFR_REF_OP },
	{RAID_3108, L"Drive Management"					, EFI_IFR_REF_OP },

	{RAID_NULL, L"SMC_LSI_RAID_END"					,			0xFF }
};

SMC_RAID_ITEMS_HEADER	mSmcLsiItemsTable[] = {
	// 3108
	{RAID_3108, L"Controller Management"	, L"Product Name", 		 0xFFFF , RAID_INFORMATION_TYPE},
	{RAID_3108, L"Controller Management"	, L"Serial Number",		 0xFFFF , RAID_INFORMATION_TYPE},
	{RAID_3108, L"Controller Management"	, L"Controller Status",	 0xFFFF , RAID_INFORMATION_TYPE},
	{RAID_3108, L"Controller Management"	, L"PCI ID",			 0xFFFF , RAID_INFORMATION_TYPE},
	{RAID_3108, L"Controller Management"	, L"PCI Slot Number",	 0xFFFF , RAID_INFORMATION_TYPE},
	{RAID_3108, L"Controller Management"	, L"Package Version",	 0xFFFF , RAID_INFORMATION_TYPE},
	{RAID_3108, L"Controller Management"	, L"Firmware Version",	 0xFFFF , RAID_INFORMATION_TYPE},
	{RAID_3108, L"Controller Management"	, L"Connector Count",	 0xFFFF , RAID_INFORMATION_TYPE},
	{RAID_3108, L"Controller Management"	, L"Drive Count",		 0xFFFF , RAID_INFORMATION_TYPE},
	{RAID_3108, L"Controller Management"	, L"Virtual Drive Count",0xFFFF , RAID_INFORMATION_TYPE},
	{RAID_3108, L"Select Drives"			, HARD_DRIVES_GROUP,	 0xA700 , RAID_HDG_TYPE},
	{RAID_3108, L"Virtual Drive Management"	, RAID_DRIVES_GROUP,     0xA000 , RAID_RDG_TYPE},
	{RAID_3108, L"Make Unconfigured Good"	, JBOD_DRIVES_GROUP,     0xAB00 , RAID_JBOD_TYPE},

	{RAID_3108, L"Advanced Controller Properties", L"Boot Mode",	 0xFFFF , RAID_CHANGEABLE_TYPE },
	{RAID_3108, L"Advanced Controller Properties", L"JBOD Mode",	 0xFFFF	, RAID_CHANGEABLE_TYPE },

	{RAID_NULL, L"", L"", 0x0 }
};

SMC_RAID_CMD_RAIDTYPE_TO_VALUE_MAP	mSmcRaidTypeToValueMapTable[] = {
	{RAID_3108, { { 0, 0}, { 1, 1}, { 5, 5}, { 6, 6}, {100, 0x64}, {10, 0x0A}, {50, 0x32}, {60, 0x3C}, {T_SHORT_END, T_SHORT_END} ,{T_SHORT_END, T_SHORT_END} } },



	{RAID_NULL, { { 0, 0}, { 1, 1}, { 5, 5}, { 6, 6}, {100, 0x64}, {10, 0x0A}, {50, 0x32}, {60, 0x3C}, {T_SHORT_END, T_SHORT_END} ,{T_SHORT_END, T_SHORT_END} } }
};

/*
 * Now, we assume each form only have one way to apply changes.
 * If one form has two or more ways to apply changes, we need rewrite the structure and access method.
 */

SMC_RAID_CHRECORD_HEADER	mSmcLsiChRecordsTable[] = {
	{RAID_3108,	L"Advanced Controller Properties", L"Apply Changes" },
	{RAID_NULL, L"", L"" }
};

SMC_LSI_AFTER_DOWN_FUNC		mSmcLsiAfterDwonFuncTable[] = {
	{ L"SmcLsiSetupDownDummyFunc"	, SmcLsiSetupDownDummyFunc	},
	{ L""							, NULL						}
};

SMC_LSI_AFTER_LOAD_FUNC		mSmcLsiAfterLoadFuncTable[] = {
	{ L"SmcLsiSetupLoadDummyFunc"	, SmcLsiSetupDownDummyFunc	},
	{ L""							, NULL						}
};
