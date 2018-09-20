#include "SmcLsiRaidOOB.h"
#include "SmcOobVersion.h"

const CHAR8	CopyRightString[] = "SuperMicro RAID OOB Module, Durant.Lin";

SMC_RAID_VAR_HASH**			LsiVarHashTableName  = NULL;
SMC_RAID_VAR_HASH**			LsiVarHashTableVarId = NULL;

BOOLEAN						mDetailedDebugMessage = FALSE;

SMC_LSI_INSIDE_DOWN_FUNC		mSmcLsiInsideDownFunc[]		= {
	{ L"InitialLsiVarHashTable"					, InitialLsiVarHashTable 					},
	{ L"FreeLsiVarHashTable"					, FreeLsiVarHashTable						},
	{ L"SmcLsiRaidOOB_InitialFunc"				, SmcLsiRaidOOB_InitialFunc					},
	{ L"SmcLsiRaidOOB_CollectInformation_Form"	, SmcLsiRaidOOB_CollectInformation_Form		},
	{ L"SmcLsiHookBrowser2Protocol"				, SmcLsiHookBrowser2Protocol				},
	{ L"SmcLsiCallbackAccessMenu"				, SmcLsiCallbackAccessMenu					},
	{ L"SmcLsiRaidOOB_CollectInformation_Items"	, SmcLsiRaidOOB_CollectInformation_Items	},
	{ L"SmcLsiRaidOOB_CollectInformation_ChRec" , SmcLsiRaidOOB_CollectInformation_ChRec	},
	{ L"SmcLsiSetSmcLsiVariableTable"			, SmcLsiSetSmcLsiVariableTable				},
	{ L"SmcLsiCreateSmcRaidVarAndItems"			, SmcLsiCreateSmcRaidVarAndItems			},
	{ L"SmcLsiHookBrowser2Protocol"				, SmcLsiHookBrowser2Protocol				},
	{ L""										, NULL										}
};

SMC_LSI_INSIDE_LOAD_FUNC		mSmcLsiInsideLoadFunc[]		= {
	{ L"InsertRaidSetupVariable"			, InsertRaidSetupVariable				},
	{ L"InsertRaidSetupFormGoto"			, InsertRaidSetupFormGoto				},
	{ L"InsertRaidSetupFormItems"			, InsertRaidSetupFormItems				},
	{ L"InsertRaidSetupChangeItems"			, InsertRaidSetupChangeItems			},
	{ L"InsertRaidSetupHDGItems"			, InsertRaidSetupHDGItems				},
	{ L"InsertRaidSetupSmcCmdsAndItems"		, InsertRaidSetupSmcCmdsAndItems		},
	{ L""									, NULL									}
};

SMC_LSI_INSIDE_CHANGE_FUNC		mSmcLsiInsideChangeFunc[]	= {
					
	{ L"InitialLsiVarHashTable"				, InitialLsiVarHashTable				},
	{ L"CheckChangeableItemsInChangedVar"	, CheckChangeableItemsInChangedVar		},
	{ L"ChangedVarToOOBVarBuffer"			, ChangedVarToOOBVarBuffer				},

	{ L"SmcLsiHookBrowser2Protocol"			, SmcLsiHookBrowser2Protocol			},
	{ L"AccessRAIDRecordtoChangeSetting"	, AccessRAIDRecordtoChangeSetting		},
	{ L"SmcLsiHookBrowser2Protocol"			, SmcLsiHookBrowser2Protocol			},

	{ L"ParseRaidCfgCmdString"				, ParseRaidCfgCmdString					},
	{ L"CollectCfgCmdData"					, CollectCfgCmdData						},
	{ L"ExamineBasicForCmdRequire"			, ExamineBasicForCmdRequire				},
	{ L"SmcLsiHookBrowser2Protocol"			, SmcLsiHookBrowser2Protocol			},
	{ L"HandleRaidCfgCmdString"				, HandleRaidCfgCmdString				},
	{ L"SmcLsiHookBrowser2Protocol"			, SmcLsiHookBrowser2Protocol			},
	{ L""									, NULL									}
};

SMC_RAID_SIZE_TYPE_MAP	SmcRaidSizeTypeMap[] = {
		{ SIZE_TB_TYPE	, "TB" },
		{ SIZE_GB_TYPE	, "GB" },
		{ SIZE_NON_TYPE	, ""   }
};

SMC_RAID_CMD_SPECIE_MAP	SpecieMap[] = {
		{ 'B' , SMC_CMD_SPECIE_BUILD 	},
		{ 'D' , SMC_CMD_SPECIE_DELETE 	},
		{ 'O' , SMC_CMD_SPECIE_OTHER	},
		{ 'X' , SMC_CMD_SPECIE_NON		}
};

RAID_CMD_PROCESSING_MAP		RaidCmdProcessMapTable[] = {
	{P_RAID_ENTER_FORM		, L"Configuration Management" 	, L"Create Virtual Drive" 		, 0xFFFF 	, EFI_IFR_REF_OP 	  ,	 SMC_CMD_RAID_NON		},
	{P_RAID_CHANGE_RAIDTYPE	, L"Create Virtual Drive"		, L"Select RAID Level"	  		, 0xFFFF	, EFI_IFR_ONE_OF_OP	  ,	 SMC_CMD_RAID_RAIDTYPE	},
	{P_RAID_ENTER_FORM		, L"Create Virtual Drive"	   	, L"Select Drives"		  		, 0xFFFF 	, EFI_IFR_REF_OP 	  ,	 SMC_CMD_RAID_NON		},
	{P_RAID_CHOICE_HDD		, L"Select Drives"		   		, L"Choice Hdd"			  		, 0xA700 	, EFI_IFR_CHECKBOX_OP ,	 SMC_CMD_RAID_NON		},
	{P_RAID_ENTER_FORM		, L"Select Drives"		   		, L"Apply Changes"		  		, 0xFFFF 	, EFI_IFR_REF_OP	  ,  SMC_CMD_RAID_NON		},
	{P_RAID_CHANGE_SETTING	, L"Create Virtual Drive"		, L"Virtual Drive Size Unit"    , 0xFFFF	, EFI_IFR_ONE_OF_OP   ,  SMC_CMD_RAID_SIZE		},
	{P_RAID_CHANGE_SETTING	, L"Create Virtual Drive"		, L"Virtual Drive Size"   		, 0xFFFF	, EFI_IFR_STRING_OP   ,  SMC_CMD_RAID_SIZE		},
	{P_RAID_ENTER_FORM		, L"Create Virtual Drive"	    , L"Save Configuration"	  		, 0xFFFF	, EFI_IFR_REF_OP	  ,	 SMC_CMD_RAID_NON		},
	{P_RAID_CONFIRM	  		, L"Warning"	   			    , L"Confirm"	 		  		, 0xFFFF	, EFI_IFR_CHECKBOX_OP ,	 SMC_CMD_RAID_NON		},
	{P_RAID_PRESS_ACTION	, L"Warning"	   			    , L"Yes"	 			  		, 0xFFFF	, EFI_IFR_ACTION_OP	  ,	 SMC_CMD_RAID_NON		},
	{P_RAID_NON_ACTION		, L""							, L""					  		, 0xFFFF	, 0xFF				  ,  SMC_CMD_RAID_NON		}

};

RAID_CMD_PROCESSING_MAP 	RaidCmdProcessClearMapTable[] = {
	{P_RAID_ENTER_FORM		, L"Configuration Management"	, L"Create Virtual Drive" , 0xFFFF	, EFI_IFR_REF_OP	  ,  SMC_CMD_RAID_NON	},
	{P_RAID_NON_ACTION		, L""							, L""					  , 0xFFFF	, 0xFF				  ,  SMC_CMD_RAID_NON	}
};

SMCLSI_RAIDCMDFUNCTION_MAP	SmcLsiRaidFunctionMapBuildRaidTable[] = {
	{RAID_3108 , HandleBuildRaidCmd_B , RaidCmdProcessMapTable 		},
	{RAID_3108 , HandleBuildRaidCmd_B , RaidCmdProcessClearMapTable },
	{RAID_NULL , NULL				  , NULL						}
};
