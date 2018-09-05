//****************************************************************************
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//****************************************************************************
//****************************************************************************
//  File History
//
//  Rev. 1.05
//    Bug Fix:  Fix compile issue when using SmcBDFTransfer() in SmcAOCPei module.
//    Reason:   
//    Auditor:  Jason Hsu
//    Date:     Aug/10/2017
//
//  Rev. 1.04
//    Bug Fix:  Support FPGA CPU.
//    Reason:   FPGA IOU3 change to IOU1 and pcie reverse.
//    Auditor:  Kasber Chen
//    Date:     Jun/08/2017
//
//  Rev. 1.03
//    Bug Fix:  Add GetOnBoardLanBus function.
//    Reason:     
//    Auditor:  William Hsu
//    Date:     Jan/07/2017
//
//  Rev. 1.02
//    Bug Fix:  Fix Bus dynamic change issue.
//    Reason:     
//    Auditor:  Kasber Chen
//    Date:     Sep/06/2016
//
//  Rev. 1.01
//    Bug Fix:  Add IPMI video ram data transfer library.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Jul/27/2016
//
//  Rev. 1.00
//    Bug Fix:  Initialize revision.
//    Reason:   Get base code from Grantley
//    Auditor:  Kasber Chen
//    Date:     May/12/2016
//**************************************************************************//
//****************************************************************************
#include "Token.h"
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/MmPciBaseLib.h>
#include <Library/BaseLib.h>

UINTN	CPU_IIO_Bus_Tbl[] = {
		PcdToken(PcdSmcCPU0IIOBus),
		PcdToken(PcdSmcCPU1IIOBus),
		PcdToken(PcdSmcCPU2IIOBus),
		PcdToken(PcdSmcCPU3IIOBus)};

UINT8	CPU_IIO_Bus_Size = sizeof(CPU_IIO_Bus_Tbl)/sizeof(UINTN);

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure   : CheckTPMPresent
//
// Description : Check TPM present or not.
//
// Parameters  :
//
// Returns     :  1: TPM present  0: TPM not present
//
//----------------------------------------------------------------------------
//<AMI_PHDR_START>
UINT8
CheckTPMPresent(VOID)
{
    UINT8	TPMFlag = 0;

    // Check TPM's VendorID & DeviceID
    if((*(UINT16*)(UINTN)(PORT_TPM_IOMEMBASE + 0xF00) != 0xFFFF) ||
            (*(UINT16*)(UINTN)(PORT_TPM_IOMEMBASE + 0xF02) != 0xFFFF)) {
        TPMFlag = 1;
    }

    return TPMFlag;
}
//<AMI_PHDR_START>
//---------------------------------------------------------------------------
//
// Procedure: GetCmosTokensValue
//
// Description:
//      This function get value from CMOS by analyze tokenID.
//      (now only for cmos 0x70/0x71)
//
// Input:
//      UINT32 tokenID
//                  -- Please see Notes:
//
// Output:
//      UINT8 Data
//                  -- The data get from CMOS
//
// Notes:
//      Based on AMISSP2 style 2 output
//      Style two uses the 16-bit token value to encode the CMOS item's
//      location and size within CMOS according to:
//      Token[2:0]   = Bit offset within a byte of item's start address in CMOS
//      Token[11:3]  = Byte offset of item's start address in CMOS
//      Token[15:12] = Size of item (in units of bytes for string and password
//                     items, in units of bits otherwise).
//
//---------------------------------------------------------------------------
//<AMI_PHDR_END>
UINT8
GetCmosTokensValue(
    IN OUT	UINT32	tokenID
)
{
    UINT8	Index, Data, maskbit;

    maskbit = (UINT8)(0xff >> (8 - (tokenID >> 12)));
    Index = (UINT8)((tokenID >> 3) & 0xff);

    // get value from CMOS
    IoWrite8(CMOS_ADDR_PORT, (Index | 0x80));
    Data = IoRead8(CMOS_DATA_PORT);

    Data = Data >> (tokenID & 0x7);
    Data &= maskbit;

    return Data;
}

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
//
// Procedure: SetCmosTokensValue
//
// Description:
//      This function write the data to CMOS by analyze tokenID.
//
// Input:
//      UINT32 tokenID
//                  -- Please see Notes:
//
//      UINT8 value
//                  -- The data will be write to CMOS
//
// Output:
//
// Notes:
//      Based on AMISSP2 style 2 output
//      Style two uses the 16-bit token value to encode the CMOS item's
//      location and size within CMOS according to:
//      Token[2:0]   = Bit offset within a byte of item's start address in CMOS
//      Token[11:3]  = Byte offset of item's start address in CMOS
//      Token[15:12] = Size of item (in units of bytes for string and password
//                     items, in units of bits otherwise).
//
//---------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID
SetCmosTokensValue(
    IN	UINT32	tokenID,
    IN	UINT8	value
)
{
    UINT8 Index, Data, temp, maskbit;

    temp = value;
    maskbit = (UINT8)(0xff >> (8 - (tokenID >> 12)));
    maskbit = maskbit << (tokenID & 0x7);
    Index = (UINT8)((tokenID >> 3) & 0xff);

    temp <<= (tokenID & 0x7);
    temp &= maskbit;

    // get value from CMOS
    IoWrite8(CMOS_ADDR_PORT, Index | 0x80);
    Data = IoRead8(CMOS_DATA_PORT);

    Data &= ~maskbit;
    Data |= temp;
    IoWrite8( CMOS_DATA_PORT, Data );
}

UINT16
SmcBDFTransfer(
    IN	UINT32	OldBDF
)
{
    UINT16	NewBDF = 0, Bus;
    UINT8	CPU, IOU, Flag, Bifurcate;

    DEBUG((-1, "SmcBDFTransfer Entry.\n"));

    CPU = ((UINT8)(OldBDF >> 20) & 0x0f);
    IOU = ((UINT8)(OldBDF >> 16) & 0x0f);

    if((CPU > 4) || (CPU < 1))	return (UINT16)OldBDF;	//CPU max 4
    if(IOU > 5)	return (UINT16)OldBDF;			//IOU max 6

    Flag = (UINT8)RShiftU64(LibPcdGet64(CPU_IIO_Bus_Tbl[CPU - 1]), 56);
    Bifurcate = (UINT8)RShiftU64(LibPcdGet64(CPU_IIO_Bus_Tbl[CPU - 1]), 48);
    if((Flag & BIT0) && (IOU == 0x01)){         //if FPGA CPU and IOU1
        DEBUG((-1, "It is FPGA CPU, copy IOU3 to IOU1.\n"));
        Bus = (UINT16)RShiftU64(LibPcdGet64(CPU_IIO_Bus_Tbl[CPU - 1]),(3 * 8));
        if(Bifurcate < 4){     //x4x4x4x4, x4x4xxx8, xxx8x4x4, xxx8xxx8
            if((UINT16)OldBDF == 0x0000)        OldBDF = 0x0010;
            else if((UINT16)OldBDF == 0x0008)   OldBDF = 0x0018;
            else if((UINT16)OldBDF == 0x0010)   OldBDF = 0x0000;
            else if((UINT16)OldBDF == 0x0018)   OldBDF = 0x0008;
        }
    } else {
        Bus = (UINT16)RShiftU64(LibPcdGet64(CPU_IIO_Bus_Tbl[CPU - 1]),(IOU * 8));
    }

    NewBDF = (Bus << 8) + (UINT16)OldBDF;
    DEBUG((-1, "SmcBDFTransfer, OldBDF = %x, NewBDF = %x.\n", OldBDF, NewBDF));
    return NewBDF;
}


UINT8
GetOnBoardLanBus(
    IN	UINT32	TempBDF
)
{	   
	UINT8	Sec_Bus, Sub_Bus, i, BaseClassCode;
	
	DEBUG((-1, "GetOnBoardLanBus Entry.\n"));
	Sec_Bus = *(UINT8*)MmPciAddress(0, TempBDF >> 8, (TempBDF >> 3) & 0x1F, TempBDF & 0x07, 0x19);
	Sub_Bus = *(UINT8*)MmPciAddress(0, TempBDF >> 8, (TempBDF >> 3) & 0x1F, TempBDF & 0x07, 0x1A);
	if (Sec_Bus == 0xff && Sub_Bus == 0xff)
		return 0xff;
	for ( i = Sec_Bus ; i <= Sub_Bus; i++ ){
		BaseClassCode = *(UINT8*)MmPciAddress(0, i , 0, 0, 0x0B); 
		if (BaseClassCode == 0x02)
			break;
	}
	DEBUG((-1, "GetOnBoardLanBus, On Board LAN Bus = %x \n", i));
	return i;		
}


//****************************************************************************
