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

SMC_RAID_CMD_JBOD_MAP JbodMap[] = {
		{'M' , SMC_CMD_JBOD_MAKE	 },
		{'U' , SMC_CMD_JBOD_UNCONFIG },
		{'X' , SMC_CMD_JBOD_NON		 }
};

RAID_CMD_PROCESSING_MAP		RaidCmdProcessMapTable_UnConfigJbod[] = {
	{P_ENUM_ENTERFORM_SET		, L"Configuration Management" 	, L"Make Unconfigured Good" 	, 0xFFFF 	, EFI_IFR_REF_OP 	  , 0 },
	{P_ENUM_CHOICEHDD_SET		, L"Make Unconfigured Good"		, L"Choice Hdd"	  				, 0xAB00 	, EFI_IFR_CHECKBOX_OP , 0 },
	{P_ENUM_ENTERFORM_SET		, L"Make Unconfigured Good" 	, L"OK" 						, 0xFFFF 	, EFI_IFR_REF_OP 	  , 0 },
	{P_ENUM_CONFIRM_SET	  		, L"Warning"	   			    , L"Confirm"	 		  		, 0xFFFF	, EFI_IFR_CHECKBOX_OP , 0 },
	{P_ENUM_PRESSACTION_SET		, L"Warning"	   			    , L"Yes"	 			  		, 0xFFFF	, EFI_IFR_ACTION_OP	  , 0 },
	{P_ENUM_NONACTION_SET		, L""							, L""					  		, 0xFFFF	, 0xFF				  , 0 }
};

RAID_CMD_PROCESSING_MAP		RaidCmdProcessMapTable_MakeJbod[] = {
	{P_ENUM_ENTERFORM_SET		, L"Configuration Management" 	, L"Make JBOD" 					, 0xFFFF 	, EFI_IFR_REF_OP 	  , 0 },
	{P_ENUM_CHOICEHDD_SET		, L"Make JBOD"					, L"Choice Hdd"	  				, 0xAD00 	, EFI_IFR_CHECKBOX_OP , 0 },
	{P_ENUM_ENTERFORM_SET		, L"Make JBOD" 					, L"OK" 						, 0xFFFF 	, EFI_IFR_REF_OP 	  , 0 },
	{P_ENUM_CONFIRM_SET	  		, L"Warning"	   			    , L"Confirm"	 		  		, 0xFFFF	, EFI_IFR_CHECKBOX_OP , 0 },
	{P_ENUM_PRESSACTION_SET		, L"Warning"	   			    , L"Yes"	 			  		, 0xFFFF	, EFI_IFR_ACTION_OP	  , 0 },
	{P_ENUM_NONACTION_SET		, L""							, L""					  		, 0xFFFF	, 0xFF				  , 0 }
};

RAID_CMD_PROCESSING_MAP		RaidCmdProcessMapTable_D_3108[] = {
	{P_ENUM_ENTERFORM_SET			, L"Main Menu" 					, L"Virtual Drive Management" 	, 0xFFFF 	, EFI_IFR_REF_OP 	, 0 },
	//Choice RaidDrive it will handle last sequence by below table RaidCmdProcessMapTable_D_3108_RaidRef.
	{P_ENUM_CHOICEHDD_SET_DELRAID	, L"Virtual Drive Management"	, L"Choice Raid Drive"	  		, 0xA000 	, EFI_IFR_REF_OP 	, 0 },
	{P_ENUM_NONACTION_SET			, L""							, L""					  		, 0xFFFF	, 0xFF				, 0 }

};
//Since there are many GO REFs, we may use QId to ensure. 
RAID_CMD_PROCESSING_MAP 	RaidCmdProcessMapTable_D_3108_RaidRef[] = {
 	{P_ENUM_ENTERFORM_SET				, L"Virtual Drive Management"	, L"Raid Drive Form Name" 		, 0xFFFF 	, EFI_IFR_REF_OP 	 	, 0 },
	{P_ENUM_CHANGESETTING_SET_DELRAID	, L"Raid Drive Form Name" 		, L"Operation" 					, 0xFFFF 	, EFI_IFR_ONE_OF_OP 	, 0 },
	{P_ENUM_ENTERFORM_SET				, L"Raid Drive Form Name" 		, L"Use QId find Go Form"		, 0xFFFF 	, EFI_IFR_REF_OP   	  	, 0 },
	{P_ENUM_CONFIRM_SET	  				, L"Warning"	   			    , L"Confirm"	 		  		, 0xFFFF	, EFI_IFR_CHECKBOX_OP 	, 0 },
	{P_ENUM_PRESSACTION_SET				, L"Warning"	   			    , L"Yes"	 			  		, 0xFFFF	, EFI_IFR_ACTION_OP 	, 0 },
	{P_ENUM_NONACTION_SET				, L""							, L""					  		, 0xFFFF	, 0xFF					, 0 }
};

RAID_CMD_PROCESSING_MAP 	RaidCmdProcessClearMapTable_D_3108[] = {
	{P_ENUM_ENTERFORM_SET		, L"Main Menu"					, L"Virtual Drive Management" , 0xFFFF	, EFI_IFR_REF_OP	},
	{P_ENUM_NONACTION_SET		, L""							, L""						  , 0xFFFF	, 0xFF				}
};

RAID_CMD_PROCESSING_MAP		RaidCmdProcessMapTable_B_3108[] = {
	{P_ENUM_ENTERFORM_SET				, L"Configuration Management" 	, L"Create Virtual Drive" 		, 0xFFFF 	, EFI_IFR_REF_OP 	  	, 0 },
	{P_ENUM_CHANGESETTING_SET_RAIDTYPE	, L"Create Virtual Drive"		, L"Select RAID Level"	  		, 0xFFFF	, EFI_IFR_ONE_OF_OP	 	, 1 },
	{P_ENUM_ENTERFORM_SET				, L"Create Virtual Drive"	   	, L"Select Drives"		  		, 0xFFFF 	, EFI_IFR_REF_OP 	 	, 0 },
	{P_ENUM_CHOICEHDD_SET				, L"Select Drives"		   		, L"Choice Hdd"			  		, 0xA700 	, EFI_IFR_CHECKBOX_OP 	, 0 },
	{P_ENUM_ENTERFORM_SET				, L"Select Drives"		   		, L"Apply Changes"		  		, 0xFFFF 	, EFI_IFR_REF_OP	 	, 0 },
	{P_ENUM_CHANGESETTING_SET_SIZE_UNIT	, L"Create Virtual Drive"		, L"Virtual Drive Size Unit"    , 0xFFFF	, EFI_IFR_ONE_OF_OP  	, 0 },
	{P_ENUM_CHANGESETTING_SET_SIZE_SIZE	, L"Create Virtual Drive"		, L"Virtual Drive Size"   		, 0xFFFF	, EFI_IFR_STRING_OP  	, 0 },
	{P_ENUM_ENTERFORM_SET				, L"Create Virtual Drive"	    , L"Save Configuration"	  		, 0xFFFF	, EFI_IFR_REF_OP	 	, 0 },
	{P_ENUM_CONFIRM_SET	  				, L"Warning"	   			    , L"Confirm"	 		  		, 0xFFFF	, EFI_IFR_CHECKBOX_OP 	, 0 },
	{P_ENUM_PRESSACTION_SET				, L"Warning"	   			    , L"Yes"	 			  		, 0xFFFF	, EFI_IFR_ACTION_OP	 	, 0 },
	{P_ENUM_NONACTION_SET				, L""							, L""					  		, 0xFFFF	, 0xFF				  	, 0 }
};

RAID_CMD_PROCESSING_MAP 	RaidCmdProcessClearMapTable_B_3108[] = {
	{P_ENUM_ENTERFORM_SET		, L"Configuration Management"	, L"Create Virtual Drive" , 0xFFFF	, EFI_IFR_REF_OP	, 0 },
	{P_ENUM_NONACTION_SET		, L""							, L""					  , 0xFFFF	, 0xFF				, 0 }
};

RAID_CMD_PROCESSING_MAP 	RaidCmdProcessDynamicMap[] = {
	{P_ENUM_ENTERFORM_SET		, L"Configuration Management"	, L"Create Virtual Drive" , 0xFFFF	, EFI_IFR_REF_OP	, 0 },
	{P_ENUM_NONACTION_SET		, L""							, L""					  , 0xFFFF	, 0xFF				, 0 }
};

// --- SPAN ---- //
// First enter to select RAID Level

RAID_CMD_PROCESSING_MAP		RaidCmdProcessMapTable_B_Span_3108_First[] = {
	{P_ENUM_ENTERFORM_SET				, L"Configuration Management" 	, L"Create Virtual Drive" 		, 0xFFFF 	, EFI_IFR_REF_OP 		, 0 },
	{P_ENUM_CHANGESETTING_SET_RAIDTYPE	, L"Create Virtual Drive"		, L"Select RAID Level"	  		, 0xFFFF	, EFI_IFR_ONE_OF_OP		, 0 },
	{P_ENUM_NONACTION_SET				, L""							, L""					  		, 0xFFFF	, 0xFF					, 0 }
};
// Second Press Add more Spans for multi times follow Hdd Groups.
RAID_CMD_PROCESSING_MAP		RaidCmdProcessMapTable_B_Span_3108_Second[] = {
	{P_ENUM_PRESSACTION_SET				, L"Create Virtual Drive"		, L"Add More Spans"	  			, 0xFFFF	, EFI_IFR_ACTION_OP		, 0 },
	{P_ENUM_NONACTION_SET				, L""							, L""					  		, 0xFFFF	, 0xFF					, 0 }
};
// Third Enter each Select Drives form.
RAID_CMD_PROCESSING_MAP		RaidCmdProcessMapTable_B_Span_3108_Third[] = {
	{P_ENUM_ENTERFORM_SET				, L"Create Virtual Drive"		, L"Select Drives"	  			, 0xFFFF	, EFI_IFR_REF_OP		, 0 },
	{P_ENUM_NONACTION_SET				, L""							, L""					  		, 0xFFFF	, 0xFF					, 0 }
};
// Forth Choice HDDs
RAID_CMD_PROCESSING_MAP		RaidCmdProcessMapTable_B_Span_3108_Forth[] = {
	{P_ENUM_CHOICEHDD_SET				, L"Select Drives"		   		, L"Choice Hdd"			  		, 0xA700 	, EFI_IFR_CHECKBOX_OP 	, 0 },
	{P_ENUM_ENTERFORM_SET				, L"Select Drives"		   		, L"Apply Changes"		  		, 0xFFFF 	, EFI_IFR_REF_OP	 	, 0 },
	//Use to make sure Select Drives successfully.
	{P_ENUM_TEST_FORMITEMS				, L"Success"					, L"OK"							, 0xFFFF	, EFI_IFR_REF_OP		, 0 },
	{P_ENUM_NONACTION_SET				, L""							, L""					  		, 0xFFFF	, 0xFF					, 0 }
};
// Fifth Build Raid Drive.
RAID_CMD_PROCESSING_MAP		RaidCmdProcessMapTable_B_Span_3108_Fifth[] = {
	{P_ENUM_CHANGESETTING_SET_SIZE_UNIT	, L"Create Virtual Drive"		, L"Virtual Drive Size Unit"    , 0xFFFF	, EFI_IFR_ONE_OF_OP  	, 0 },
	{P_ENUM_CHANGESETTING_SET_SIZE_SIZE	, L"Create Virtual Drive"		, L"Virtual Drive Size"   		, 0xFFFF	, EFI_IFR_STRING_OP  	, 0 },
	{P_ENUM_ENTERFORM_SET				, L"Create Virtual Drive"	    , L"Save Configuration"	  		, 0xFFFF	, EFI_IFR_REF_OP	 	, 0 },
	{P_ENUM_CONFIRM_SET	  				, L"Warning"	   			    , L"Confirm"	 		  		, 0xFFFF	, EFI_IFR_CHECKBOX_OP 	, 0 },
	{P_ENUM_PRESSACTION_SET				, L"Warning"	   			    , L"Yes"	 			  		, 0xFFFF	, EFI_IFR_ACTION_OP	 	, 0 },
	{P_ENUM_NONACTION_SET				, L""							, L""					  		, 0xFFFF	, 0xFF				  	, 0 }
};

SMCLSI_RAIDCMDFUNCTION_MAP	SmcLsiRaidFunctionMapBuildRaidTable[] = {
	{RAID_3108 , HandleBuildRaidCmd },
	{RAID_NULL , NULL				}
};

SMCLSI_RAIDCMDFUNCTION_MAP	SmcLsiRaidFunctionMapDeleteRaidTable[] = {
	{RAID_3108 , HandleBuildRaidCmd_D },
	{RAID_NULL , NULL				  }
};

SMCLSI_RAIDCMDFUNCTION_MAP	SmcLsiRaidFunctionMapOtherRaidTable[] = {
	{RAID_3108 , HandleOtherRaidCmd },
	{RAID_NULL , NULL				}
};

