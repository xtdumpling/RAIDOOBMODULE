//****************************************************************************
//****************************************************************************
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//****************************************************************************
//****************************************************************************
//  File History
//
//  Rev. 1.01
//    Bug Fix:  Re-write SmcOutBand module.
//    Reason:   
//    Auditor:  Hartmann Hsieh
//    Date:     Aug/02/2016
//
//  Rev. 1.00
//    Bug Fix:  Add Full SMBIOS module.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Jul/27/2016
//
//****************************************************************************

//**********************************************************************
//<AMI_FHDR_START>
//
// Name:        OobLibrary.c
//
// Description: OOB library code.
//
//<AMI_FHDR_END>
//**********************************************************************

#include "EFI.h"
#include "Token.h"
#include <AmiDxeLib.h>
#include <AmiCspLib.h>
#include <Library/BaseCryptLib.h>
#include "TimeStamp.h"
#include "OobLibrary.h"
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>

EFI_STATUS
CalculateMd5(
    IN		VOID	*HashData,
    IN		UINTN	DataSize,
    IN OUT	UINT8	*Digest
)
{
    UINTN	CtxSize;
    VOID	*HashCtx;
    BOOLEAN	Status;

    DEBUG((-1,"- MD5:    "));
//
// MD5 Digest Validation
//
    MemSet (Digest, MD5_DIGEST_SIZE, 0);
    CtxSize = Md5GetContextSize();
    pBS->AllocatePool(EfiBootServicesData, CtxSize, &HashCtx);

    DEBUG((-1,"Init...\n"));
    Status = Md5Init(HashCtx);
    if(!Status) {
        DEBUG((-1,"[Fail]\n"));
        return EFI_ABORTED;
    }

    DEBUG((-1,"Update...\n"));
    Status = Md5Update(HashCtx, HashData, DataSize);
    if(!Status) {
        DEBUG((-1,"[Fail]\n"));
        return EFI_ABORTED;
    }

    DEBUG((-1, "Finalize...\n"));
    Status = Md5Final(HashCtx, Digest);
    if(!Status) {
        DEBUG((-1,"[Fail]\n"));
        return EFI_ABORTED;
    }

    pBS->FreePool(HashCtx);
    return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Name:  CompareBIOSInfo
//
// Description:
//  Compare BIOS Info.
//
//  Board Info byte map :
//
//  SVID[0] = (UINT8)CRB1_SSID
//  SVID[1] = (UINT8)(CRB1_SSID >> 8)
//  SVID[2] = (UINT8)(CRB1_SSID >> 16)
//  SVID[3] = (UINT8)(CRB1_SSID >> 24)
//  Date[0] = Day
//  Date[1] = Month
//  Date[2] = (UINT8)Year
//  Date[3] = (UINT8)(Year >> 8)
//  Time[0] = Minute
//  Time[1] = Hour
//
// Input:
//      IN Board_Info BoardInfo - Board information.
//
// Output:
//      TRUE = CurrentBoardInfo and BoardInfo are the same.
//      FALSE = CurrentBoardInfo and BoardInfo are different..
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

//BOOLEAN CompareBIOSInfo(
//    IN SMC_BOARD_INFO* BoardInfo
//)
//{
//    EFI_STATUS Status = EFI_SUCCESS;
//    SMC_BOARD_INFO CurrentBoardInfo;
//
//    OOB_DEBUG((TRACE_ALWAYS, "OOB_DEBUG : CompareBIOSInfo entry.\n"));
//
//    *(UINT32*)CurrentBoardInfo.SVID = SMC_SSID;
//    *(UINT32*)CurrentBoardInfo.Date = TWO_DIGIT_DAY_INT + (TWO_DIGIT_MONTH_INT << 8) + (FOUR_DIGIT_YEAR_INT << 16);
//    *(UINT16*)CurrentBoardInfo.Time = TWO_DIGIT_MINUTE_INT + (TWO_DIGIT_HOUR_INT << 8);
//
//    OOB_DEBUG((TRACE_ALWAYS, "OOB_DEBUG : BoardInfo.SVID = %x.\n", *(UINT32*)(&BoardInfo->SVID)));
//    OOB_DEBUG((TRACE_ALWAYS, "OOB_DEBUG : BoardInfo.Date = %x.\n", *(UINT32*)(&BoardInfo->Date)));
//    OOB_DEBUG((TRACE_ALWAYS, "OOB_DEBUG : BoardInfo.Time = %x.\n", *(UINT16*)(&BoardInfo->Time)));
//
//    OOB_DEBUG((TRACE_ALWAYS, "OOB_DEBUG : CurrentBoardInfo.SVID = %x.\n", *(UINT32*)(&(CurrentBoardInfo.SVID))));
//    OOB_DEBUG((TRACE_ALWAYS, "OOB_DEBUG : CurrentBoardInfo.Date = %x.\n", *(UINT32*)(&(CurrentBoardInfo.Date))));
//    OOB_DEBUG((TRACE_ALWAYS, "OOB_DEBUG : CurrentBoardInfo.Time = %x.\n", *(UINT16*)(&(CurrentBoardInfo.Time))));
//
//    if(!MemCmp(&CurrentBoardInfo, BoardInfo, sizeof(SMC_BOARD_INFO))) {
//        OOB_DEBUG((TRACE_ALWAYS, "OOB_DEBUG : CurrentBoardInfo and BoardInfo are the same.\n"));
//        return	TRUE;
//    }
//    else {
//        OOB_DEBUG((TRACE_ALWAYS, "OOB_DEBUG : CurrentBoardInfo and BoardInfo are different.\n"));
//        return	FALSE;
//    }
//}

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
OobGetCmosTokensValue(
    IN OUT	UINT32	tokenID
)
{
    UINT8	Index, Data, maskbit;

    maskbit = (UINT8)(0xff >> (8 - (tokenID >> 12)));
    Index = (UINT8)((tokenID >> 3) & 0xff);

    // get value from CMOS
    IoWrite8(0x70, (Index | 0x80));
    Data = IoRead8(0x71);

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
OobSetCmosTokensValue(
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

