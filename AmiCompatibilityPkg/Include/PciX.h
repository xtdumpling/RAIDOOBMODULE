//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2012, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************


//**********************************************************************
// $Header: /Alaska/BIN/Core/Include/PCIX.h 1     11/04/05 6:32p Yakovlevs $Revision: 6 $
//
// $Date: 11/04/05 6:32p $
//**********************************************************************
// Revision History
// ----------------

#ifndef _PCIX_H
#define _PCIX_H

#ifdef __cplusplus
extern "C" {
#endif
/****** DO NOT WRITE ABOVE THIS LINE *******/

//This Header file Suppose to cover PCIX and PCIX2
#include <Efi.h>

#ifndef PCI_CAP_ID_PCIX
#define PCI_CAP_ID_PCIX			0x07
#endif

//PCIX and PCIX2 Register Block located after Capability Header

//=================================================================================
//Here will go Capability registers for Device with Type0 Header(PCIX Devices)

//PCIX Device Command Register 
#define PCIX_DEV_CMD_OFFSET			0x0002

typedef union _PCIX_DEV_CMD_REG { 
	UINT16		DEV_CMD;
	struct	{
		UINT16		UncDataErrRecovery	:	1;	//0
		UINT16		RelaxedOrdering		:	1;	//1
		UINT16 		MaxMemRdByteCnt		:	2;	//2 3 
		UINT16		MaxOutstSplitTrans	:	3;	//4 5 6
		UINT16		Reserved1			:	5;	//7 8 9 10 11
		UINT16		CapabVersion		:	2;	//12 13
		UINT16		Reserved			:	2;	//14 15
	};
} PCIX_DEV_CMD_REG;

//PCIX Device Status register 
#define PCIX_DEV_STA_OFFSET			0x004

typedef union _PCIX_DEV_STA_REG { 
	UINT32		DEV_STA;
	struct	{
		UINT32		FunctionNo			:	3;	//0 1 2 
		UINT32 		DeviceNo			:	5;	//3 4 5 6 7
		UINT32		BusNo				:	8;	//8 ... 15
		UINT32		DevCap64bit			:	1;	//16
		UINT32		DevCap133Mhz		:	1;	//17
		UINT32		SplComplDiscard		:	1;	//18
		UINT32		UnexpSplCompl		:	1;	//19
		UINT32		DevComplexity		:	1;	//20
		UINT32		MaxMemRdBcnt		:	2;	//21 22
		UINT32		MaxOutSplTrans		:	3;	//23 24 25
		UINT32		MaxCmlRdSize		:	3;	//26 27 28
		UINT32		SplComplErrRcv		:	1;	//29
		UINT32		DevCapPcix266		:	1;	//30
		UINT32		DevCapPcix533		:	1;	//31
	};
} PCIX_DEV_STA_REG;


//=================================================================================
//Now will go Capability registers for Device with Type1 Header(PCIX Bridges)

//PCIX Secondary I/F Status register 
#define PCIX_SEC_STA_OFFSET			0x002

//definitions for SecBus Mode and Frequency 4 bits
#define PCIX_MODE_CONVENT			0x0

#define PCIX_MODE1_66MHZ			0x1
#define PCIX_MODE1_100MHZ			0x2
#define PCIX_MODE1_133MHZ			0x3

//next definitions are for PCI-X Mode 2
#define PCIX_266_66MHZ				0x9
#define PCIX_266_100MHZ				0xA	
#define PCIX_266_133MHZ				0xB

#define PCIX_533_66MH				0xD
#define PCIX_533_100MHZ				0xE
#define PCIX_533_133MHZ				0xF


typedef union _PCIX_SEC_STA_REG { 
	UINT16		SEC_STA;
	struct	{
		UINT16		DevCap64bit			:	1;	//16
		UINT16		DevCap133Mhz		:	1;	//17
		UINT16		SplComplDiscard		:	1;	//2
		UINT16		UnexpSplCompl		:	1;	//3
		UINT16		SplComplOverrun		:	1;	//4
		UINT16		SplReqDelayed		:	1;	//5
		UINT16		SecBusModeFreq		:	4;	//6 7 8 9
		UINT16		Reserverd			:	2;	//10 11
		UINT16		CapabVersion		:	2;	//12 13
		UINT16		DevCapPcix266		:	1;	//14
		UINT16		DevCapPcix533		:	1;	//15
	};
} PCIX_SEC_STA_REG;









/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif

#endif
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2012, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

