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

typedef struct _SMC_RAID_STACK_ 			SMC_RAID_STACK;
typedef struct _SMC_RAID_STACK_BODY_		SMC_RAID_STACK_BODY;
typedef struct _SMC_RAID_STACK_SET_			SMC_RAID_STACK_SET;

struct _SMC_RAID_STACK_BODY_ {
	UINT64				data;
	UINT8				dataType;
};

struct _SMC_RAID_STACK_SET_ {
	SMC_RAID_STACK_BODY		StackBody;
	SMC_RAID_STACK_SET* 	pNext;
};

struct _SMC_RAID_STACK_ {
	SMC_RAID_STACK_SET*	SmcStackPoint;
};

#endif
