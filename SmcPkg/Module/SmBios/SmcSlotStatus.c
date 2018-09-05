//****************************************************************************
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//****************************************************************************
//****************************************************************************
//  File History
//
//    Rev. 1.01
//    Reason:	Fix only one CPU on DP, system deadloop.
//    Auditor:	Kasber Chen
//    Date:	Aug/17/2016  
//
//    Rev. 1.00
//    Reason:	Initialize revision.
//    Auditor:	Kasber Chen
//    Date:	Aug/03/2016      
//
//**************************************************************************//
#include <Library/UefiBootServicesTableLib.h>
#include <Library/PciLib.h>
#include <Library/DebugLib.h>
#include <Library/MmPciBaseLib.h>
#include <PciBus.h>
#include "SmcSlotStatus.h"

//if SSID is 0x0000, if will only detect SVID.
UINT32	GPU_Table[] = {		0x000010DE,	//NV SVID
				0x00001002,	//AMD SVID
				0x225C8086,	//intel G65757-200
				0x22508086,	//intel G65758-200
				0x225D8086,	//intel G65760-100
				0x225E8086	//Intel Xeon Phi 31S1
};

//UINT32	LAN_Table[] = {
//};

UINT32	LSI_AOC_Table[] = {	0x005B1000,     //LSI 2208
                                0x005D1000,     //LSI 3108
                                0x006E1000,     //LSI 2308
                                0x00791000      //LSI 2108
};

UINT32	NVME_Table[] = {	0x871815d9,	//SMC AOC NVMe card
				0x090a15d9,
				0x083915d9
};

UINT8
SmcSlotStatus(
    IN	UINT8	Bus
)
{
    UINT8	i, SlotStatus = 0;
    UINT8	TempBusS, TempBusE, TempDev, TempFun;
    UINT32	SVID, ClassCode;

    DEBUG((-1, "SmcSlotStatus Entry.\n"));

    TempBusS = Bus;

    TempBusE = TempBusS;
    ClassCode = *(UINT32*)MmPciAddress(0, TempBusS, 0, 0, 0x8);
    //
    // Check if PLX bridge switch present at add-on card.
    //
    if((ClassCode & 0xFFFF0000) == 0x06040000)
	TempBusE = *(UINT32*)MmPciAddress(0, TempBusE, 0, 0, 0x1A);

    for( ; (TempBusS <= TempBusE) && (TempBusE != 0xff); TempBusS++){
	for(TempDev = 0; TempDev <= 31; TempDev++){
	    for(TempFun = 0; TempFun <= 7; TempFun++){
		SVID = *(UINT32*)MmPciAddress(0, TempBusS, TempDev, TempFun, 0x0);
		if(SVID == 0xffffffff)	continue;

		ClassCode = *(UINT32*)MmPciAddress(0, TempBusS, TempDev, TempFun, 0x08);
		DEBUG((-1, "SVID:0x%x, Bus:0x%x, Dev:0x%x, Fun:0x%x.\n", SVID, TempBusS, TempDev, TempFun));
		for(i = 0; i < (sizeof(GPU_Table)/sizeof(UINT32)) && !SlotStatus; i++){
		    if(GPU_Table[i] >> 16){	// search SVID and SSID
			if(SVID == GPU_Table[i])	SlotStatus = GPU;
		    } else {	//only search SVID
			if((UINT16)SVID == (UINT16)GPU_Table[i])	SlotStatus = GPU;
		    }
		}
//		for(i = 0; i < (sizeof(LAN_Table)/sizeof(UINT32)) && SlotStatus; i++){
//		    if(SVID == LAN_Table[i])	SlotStatus = NIC_LAN;
//		}
		if((((ClassCode >> 16) & 0xFF00) == 0x0200) && !SlotStatus) {
		    if((((ClassCode >> 16) & 0xFF) == 0x07) || (((ClassCode >> 16) & 0xFF) == 0x80))
			SlotStatus = IB;
		    else
    			SlotStatus = NIC_LAN;
		}
		for(i = 0; i < (sizeof(LSI_AOC_Table)/sizeof(UINT32)) && !SlotStatus; i++){
		    if(SVID == LSI_AOC_Table[i])	SlotStatus = LSI_AOC;
		}
		SVID = *(UINT32*)MmPciAddress(0, TempBusS, TempDev, TempFun, 0xA8);
		for(i = 0; i < (sizeof(NVME_Table)/sizeof(UINT32)) && !SlotStatus; i++){
		    if(SVID == NVME_Table[i])	SlotStatus = NVME;
		}
		if(SlotStatus){
		    DEBUG((-1, "SlotStatus:0x%x\n", SlotStatus));
		    return	(SlotStatus << 4);
		}
	    }
	}
    }
    DEBUG((-1, "SmcSlotStatus End.\n"));
    return	(SlotStatus << 4);
}
