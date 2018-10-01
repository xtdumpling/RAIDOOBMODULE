//****************************************************************************
//****************************************************************************
//**            (C)Copyright 1993-2018 Supermicro Computer, Inc.            **
//****************************************************************************
//****************************************************************************
//  File History
//
//  Rev. 1.00
//    Bug Fix:  Initial revision.
//    Reason:
//    Auditor:  Durant Lin
//    Date:     Sep/28/2018
//
//****************************************************************************
#ifndef _H_SMC_LSI_RAID_OOB_SETTING_
#define _H_SMC_LSI_RAID_OOB_SETTING_


extern	SMC_LSI_RAID_NANE			RaidName[];
extern 	SMC_LSI_RAID_FORM_HEADER	mSmcLsiRaidFormRefSearchTable[];
extern  SMC_RAID_ITEMS_HEADER		mSmcLsiItemsTable[];
extern	SMC_RAID_CHRECORD_HEADER	mSmcLsiChRecordsTable[];

extern	SMC_LSI_AFTER_DOWN_FUNC		mSmcLsiAfterDwonFuncTable[];
extern	SMC_LSI_AFTER_LOAD_FUNC		mSmcLsiAfterLoadFuncTable[];

extern 	SMC_RAID_CMD_RAIDTYPE_TO_VALUE_MAP	mSmcRaidTypeToValueMapTable[];

#endif
