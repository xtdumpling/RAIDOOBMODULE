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
#ifndef	_H_SmcLsiStack_
#define	_H_SmcLsiStack_

typedef struct _SMC_RAID_STACK_ SMC_RAID_STACK;

struct _SMC_RAID_STACK_ {
	UINT64			data;

	SMC_RAID_STACK* pNext;
};

EFI_STATUS	SmcStackPush(UINT64	);
EFI_STATUS	SmcStackPop(UINT64* );

#endif
