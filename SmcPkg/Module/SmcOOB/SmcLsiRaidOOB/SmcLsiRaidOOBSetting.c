//****************************************************************************
//****************************************************************************
//**            (C)Copyright 1993-2018 Supermicro Computer, Inc.            **
//****************************************************************************
//****************************************************************************
//  File History
//
//  Rev. 1.01
//    Bug Fix:  
//    Reason:   Add RAID 2208 settings.
//    Auditor:  Durant Lin
//    Date:     Oct/02/2018
//
//  Rev. 1.00
//    Bug Fix:  Initial revision.
//    Reason:
//    Auditor:  Durant Lin
//    Date:     Sep/28/2018
//
//****************************************************************************
#include "SmcLsiRaidOOBSetupProtocol.h"
#include "SmcLsiRaidOOBSetupVfr.h"
#include "SmcLsiRaidOOBSetup.h"



SMC_LSI_RAID_NANE	RaidName[] = {
	{ RAID_3108, L"AVAGO MegaRAID <AVAGO 3108 MegaRAID>" },
	{ RAID_2208, L"AVAGO MegaRAID <LSI 2208 MegaRAID>" },
//	{ RAID_9260, L"AVAGO MegaRAID <AVAGO 9260 MegaRAID>" },
	{ RAID_NULL, L"" }
};
//Use the last line to restore FormId, QId, VId, VOff, Flag default. 
SMC_LSI_RAID_FORM_HEADER		mSmcLsiRaidFormRefSearchTable[] = {
	// 3108 
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
	// 2208
	{RAID_2208, L"Main Menu"						, EFI_IFR_REF_OP },
	{RAID_2208, L"Configuration Management"			, EFI_IFR_REF_OP },
	{RAID_2208, L"Create Virtual Drive"				, EFI_IFR_REF_OP },
	{RAID_2208, L"Select Drives"					, EFI_IFR_REF_OP },
	{RAID_2208, L"Make JBOD"						, EFI_IFR_REF_OP },
	{RAID_2208, L"Make Unconfigured Good"			, EFI_IFR_REF_OP },
	{RAID_2208, L"Controller Management"			, EFI_IFR_REF_OP },
	{RAID_2208, L"Advanced Controller Properties"	, EFI_IFR_REF_OP },
	{RAID_2208, L"Virtual Drive Management"			, EFI_IFR_REF_OP },
	{RAID_2208, L"Drive Management"					, EFI_IFR_REF_OP },

	{RAID_NULL, L"SMC_LSI_RAID_END"					,			0xFF }
};

SMC_RAID_ITEMS_HEADER	mSmcLsiItemsTable[] = {
	// 3108
	{RAID_3108, L"Dashboard View"			, L"Status", 		 	 0xFFFF , RAID_INFORMATION_TYPE},
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
	{RAID_3108, L"Dashboard View"			, L"MegaRAID RAID6"		,0xFFFF , RAID_INFORMATION_TYPE},
	{RAID_3108, L"Dashboard View"			, L"MegaRAID RAID5"		,0xFFFF , RAID_INFORMATION_TYPE},
	{RAID_3108, L"Select Drives"			, HARD_DRIVES_GROUP,	 0xA700 , RAID_HDG_TYPE},
	{RAID_3108, L"Virtual Drive Management"	, RAID_DRIVES_GROUP,     0xA000 , RAID_RDG_TYPE},
	{RAID_3108, L"Make Unconfigured Good"	, JBOD_DRIVES_GROUP,     0xAB00 , RAID_JBOD_TYPE},

	{RAID_3108, L"Advanced Controller Properties", L"Boot Mode",	 	0xFFFF  , RAID_CHANGEABLE_TYPE },
	{RAID_3108, L"Advanced Controller Properties", L"JBOD Mode",	 	0xFFFF	, RAID_CHANGEABLE_TYPE },
	{RAID_3108, L"Advanced Controller Properties", L"Write Verify",	 	0xFFFF	, RAID_CHANGEABLE_TYPE },
	{RAID_3108, L"Advanced Controller Properties", L"Large IO Support",	0xFFFF	, RAID_CHANGEABLE_TYPE },

	// 2208
	{RAID_2208, L"Dashboard View"			, L"Status", 		 	 0xFFFF , RAID_INFORMATION_TYPE},
	{RAID_2208, L"Controller Management"	, L"Product Name", 		 0xFFFF , RAID_INFORMATION_TYPE},
	{RAID_2208, L"Controller Management"	, L"Serial Number",		 0xFFFF , RAID_INFORMATION_TYPE},
	{RAID_2208, L"Controller Management"	, L"Controller Status",	 0xFFFF , RAID_INFORMATION_TYPE},
	{RAID_2208, L"Controller Management"	, L"PCI ID",			 0xFFFF , RAID_INFORMATION_TYPE},
	{RAID_2208, L"Controller Management"	, L"PCI Slot Number",	 0xFFFF , RAID_INFORMATION_TYPE},
	{RAID_2208, L"Controller Management"	, L"Package Version",	 0xFFFF , RAID_INFORMATION_TYPE},
	{RAID_2208, L"Controller Management"	, L"Firmware Version",	 0xFFFF , RAID_INFORMATION_TYPE},
	{RAID_2208, L"Controller Management"	, L"Connector Count",	 0xFFFF , RAID_INFORMATION_TYPE},
	{RAID_2208, L"Controller Management"	, L"Drive Count",		 0xFFFF , RAID_INFORMATION_TYPE},
	{RAID_2208, L"Controller Management"	, L"Virtual Drive Count",0xFFFF , RAID_INFORMATION_TYPE},
	{RAID_2208, L"Dashboard View"			, L"MegaRAID RAID6"		,0xFFFF , RAID_INFORMATION_TYPE},
	{RAID_2208, L"Dashboard View"			, L"MegaRAID RAID5"		,0xFFFF , RAID_INFORMATION_TYPE},
	{RAID_2208, L"Select Drives"			, HARD_DRIVES_GROUP,	 0xA700 , RAID_HDG_TYPE},
	{RAID_2208, L"Virtual Drive Management"	, RAID_DRIVES_GROUP,     0xA000 , RAID_RDG_TYPE},
	{RAID_2208, L"Make Unconfigured Good"	, JBOD_DRIVES_GROUP,     0xAB00 , RAID_JBOD_TYPE},

	{RAID_2208, L"Advanced Controller Properties", L"Boot Mode",	 	0xFFFF  , RAID_CHANGEABLE_TYPE },
	{RAID_2208, L"Advanced Controller Properties", L"JBOD Mode",	 	0xFFFF	, RAID_CHANGEABLE_TYPE },
	{RAID_2208, L"Advanced Controller Properties", L"Write Verify",	 	0xFFFF	, RAID_CHANGEABLE_TYPE },
	{RAID_2208, L"Advanced Controller Properties", L"Large IO Support",	0xFFFF	, RAID_CHANGEABLE_TYPE },

	{RAID_NULL, L"", L"", 0x0 }
};

SMC_RAID_CMD_RAIDTYPE_TO_VALUE_MAP	mSmcRaidTypeToValueMapTable[] = {
	// 3108
	{RAID_3108, { 
		{ SMC_CMD_RAIDTYPE_R0, 0}, 
		{ SMC_CMD_RAIDTYPE_R1, 1}, 
		{ SMC_CMD_RAIDTYPE_R5, 5}, 
		{ SMC_CMD_RAIDTYPE_R6, 6}, 
		{ SMC_CMD_RAIDTYPE_R00, 0x64}, 
		{ SMC_CMD_RAIDTYPE_R10, 0x0A}, 
		{ SMC_CMD_RAIDTYPE_R50, 0x32}, 
		{ SMC_CMD_RAIDTYPE_R60, 0x3C}, 
		{ SMC_CMD_RAIDTYPE_NON, T_SHORT_END} ,
		{ SMC_CMD_RAIDTYPE_NON, T_SHORT_END} } 
	},

	// 2208
	{RAID_2208, { 
		{ SMC_CMD_RAIDTYPE_R0, 0}, 
		{ SMC_CMD_RAIDTYPE_R1, 1}, 
		{ SMC_CMD_RAIDTYPE_R5, 5}, 
		{ SMC_CMD_RAIDTYPE_R6, 6}, 
		{ SMC_CMD_RAIDTYPE_R00, 0x64}, 
		{ SMC_CMD_RAIDTYPE_R10, 0x0A}, 
		{ SMC_CMD_RAIDTYPE_R50, 0x32}, 
		{ SMC_CMD_RAIDTYPE_R60, 0x3C}, 
		{ SMC_CMD_RAIDTYPE_NON, T_SHORT_END} ,
		{ SMC_CMD_RAIDTYPE_NON, T_SHORT_END} } 
	},

	{RAID_NULL, { 
		{ SMC_CMD_RAIDTYPE_R0, 0}, 
		{ SMC_CMD_RAIDTYPE_R1, 1}, 
		{ SMC_CMD_RAIDTYPE_R5, 5}, 
		{ SMC_CMD_RAIDTYPE_R6, 6}, 
		{ SMC_CMD_RAIDTYPE_R00, 0x64}, 
		{ SMC_CMD_RAIDTYPE_R10, 0x0A}, 
		{ SMC_CMD_RAIDTYPE_R50, 0x32}, 
		{ SMC_CMD_RAIDTYPE_R60, 0x3C}, 
		{ SMC_CMD_RAIDTYPE_NON, T_SHORT_END} ,
		{ SMC_CMD_RAIDTYPE_NON, T_SHORT_END} } 
	}
};

/*
 * Now, we assume each form only have one way to apply changes.
 * If one form has two or more ways to apply changes, we need rewrite the structure and access method.
 */

SMC_RAID_CHRECORD_HEADER	mSmcLsiChRecordsTable[] = {
	{RAID_3108,	L"Advanced Controller Properties", L"Apply Changes" },
	{RAID_2208,	L"Advanced Controller Properties", L"Apply Changes" },

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
