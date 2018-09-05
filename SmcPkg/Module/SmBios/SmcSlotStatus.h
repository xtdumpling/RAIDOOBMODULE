//****************************************************************************
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//****************************************************************************
//  File History
//
//    Rev. 1.00
//    Reason:	Initialize revision.
//    Auditor:	Kasber Chen
//    Date:	Aug/03/2016
//
//****************************************************************************
#ifndef	_H_SmcSlotStatus_
#define	_H_SmcSlotStatus_

typedef enum{
    GPU = 0x01,
    NIC_LAN,
    LSI_AOC,
    NVME,
    IB
}SMC_SLOT_STATUS;

UINT8
SmcSlotStatus(
    IN	UINT8	Bus
);

#endif
