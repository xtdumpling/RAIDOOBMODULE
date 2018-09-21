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

extern SMCLSI_RAIDCMDFUNCTION_MAP	SmcLsiRaidFunctionMapBuildRaidTable[];
extern SMCLSI_RAIDCMDFUNCTION_MAP	SmcLsiRaidFunctionMapDeleteRaidTable[];

#define D_3108_RAIDREF_RAIDDRIVE_NAME	0x0
#define D_3108_RAIDREF_OPERATION		0x1
#define D_3108_RAIDREF_GO				0x2
#define D_3108_RAIDREF_GO_QID			0x15C

extern RAID_CMD_PROCESSING_MAP 		RaidCmdProcessMapTable_D_3108_RaidRef[];

#endif
