#ifndef _H_SMC_LSI_RAID_OOB_GLOBAL_TABLE_H_
#define _H_SMC_LSI_RAID_OOB_GLOBAL_TABLE_H_

extern SMC_RAID_VAR_HASH**			LsiVarHashTableName;
extern SMC_RAID_VAR_HASH**			LsiVarHashTableVarId;
extern BOOLEAN						mDetailedDebugMessage;

extern SMC_LSI_INSIDE_DOWN_FUNC		mSmcLsiInsideDownFunc[];
extern SMC_LSI_INSIDE_LOAD_FUNC		mSmcLsiInsideLoadFunc[];
extern SMC_LSI_INSIDE_CHANGE_FUNC	mSmcLsiInsideChangeFunc[];

extern SMC_RAID_SIZE_TYPE_MAP	SmcRaidSizeTypeMap[];
extern SMC_RAID_CMD_SPECIE_MAP	SpecieMap[];

extern RAID_CMD_PROCESSING_MAP		RaidCmdProcessMapTable[];

extern RAID_CMD_PROCESSING_MAP 	RaidCmdProcessClearMapTable[];

extern SMCLSI_RAIDCMDFUNCTION_MAP	SmcLsiRaidFunctionMapBuildRaidTable[];

#endif
