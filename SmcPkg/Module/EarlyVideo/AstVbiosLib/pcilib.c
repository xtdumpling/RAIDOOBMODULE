//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//  Rev. 1.00
//    Bug Fix: Turn on Early-Video in earlier PEI.
//    Reason : Check the VGA device only to avoid hanging.
//    Auditor: Yenhsin Cho
//    Date   : May/27/2016
//  
//****************************************************************************
#include "typedef.h"
#include <AmiPeiLib.h>
#include <Token.h>

//
// Basic API
//

VOID SetPCIReg (ULONG ulPCIAddr, ULONG ulMask, ULONG ulData)
{
	ULONG Temp = 0;

    IoWrite32(0xcf8, ulPCIAddr);
	Temp = IoRead32(0xcfc);
    IoWrite32(0xcfc, (Temp & ulMask) | ulData);
	
}


ULONG GetPCIReg (ULONG ulPCIAddr, ULONG ulMask)
{
    
    IoWrite32(0xcf8, ulPCIAddr);
    return (IoRead32(0xcfc) & ulMask);
	
}

ULONG FindAstVgaDevice (USHORT usVendorID, USHORT usDeviceID)
{
    ULONG i;
    
    i = ((0) & 0xFF) | (((0) & 0x07) << 8) | (((0) & 0x1F) << 11) | (((Aspeed_VGA_BUS) & 0xFF) << 16) | (1 << 31);
    
//    for (i=0x80000000; i<0x80000000+256*32*0x800; i+=0x800)
//    {
    	if (((USHORT) GetPCIReg(i, 0x0000FFFF) == usVendorID) && (((USHORT) (GetPCIReg(i, 0xFFFF0000) >> 16)) == usDeviceID))
    	    return i; 
//    }	
 
    return (0);
}
//
// Extended API
//
