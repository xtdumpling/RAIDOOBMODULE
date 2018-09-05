//****************************************************************************
//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2015 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//****************************************************************************
//
//  File History
//  Rev. 1.00
//       Bug fixed: Add function to check Top Swap
//       Reason:	Add check Top Swap bit setting. If Top Swap enable will always enter Recovery mode.
//       Auditor:	Salmon Chen
//       Date:		Jul-20-2015
//****************************************************************************
//****************************************************************************


//============================================================================
// Includes

#include <Token.h>
#include <Efi.h>
#include <Pei.h>
//#include <AmiPeiLib.h>
//#include <AmiDxeLib.h>
#include <IoLib.h>
#include <Protocol/PciIo.h>
#include <Protocol/PciRootBridgeIo.h>

#define PCH_PCR_BASE_ADDRESS		0xFD000000
#define PCH_PCR_ADDRESS(Pid, Offset) (PCH_PCR_BASE_ADDRESS | ((UINT8)(Pid) << 16) | (UINT16)(Offset))
#define PID_RT						0xC3
#define R_PCH_PCR_RTC_BUC			0x3414               ///< Backed Up Control

BOOLEAN SMCCheckRecovery(EFI_PEI_SERVICES **PeiServices){
	UINT32	RegVal;
	volatile UINT8* Address8;

	RegVal = MmioRead32(0xFDAE04C8);
	RegVal = 0x82040102;
	MmioWrite32(0xFDAE04C8, RegVal);
	
	RegVal = MmioRead32(0xFDAE04C8);
	if(!(RegVal & BIT01))
		return TRUE;

	Address8 = (UINT8 *)(PCH_PCR_ADDRESS (PID_RT, R_PCH_PCR_RTC_BUC));
	return ((*Address8) & 1) ? TRUE : FALSE; 
}
