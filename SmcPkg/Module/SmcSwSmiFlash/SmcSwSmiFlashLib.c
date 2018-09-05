//****************************************************************************
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//****************************************************************************
//****************************************************************************
//  File History
//
//  Rev. 1.01
//    Bug Fix:  Workaround for OOB data didn't be saved to ROM part.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Oct/05/2016
//
//  Rev. 1.00
//    Bug Fix:  Trigger a software SMI to flash ROM part after BIOS lock.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Sep/30/2016
//
//**************************************************************************//
//****************************************************************************
#include "Token.h"
#include "EFI.h"
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include "SspTokens.h"
#include <Protocol/SmmControl2.h>
#include "SmcLib.h"
#include "SmcSwSmiFlashSmm.h"
#include "SmcSwSmiFlashLib.h"



typedef UINT8* TMP_LIST;
#define TMP_INTSIZEOF(n)   	( (sizeof(n) + sizeof(UINT32) - 1) & ~(sizeof(UINT32) - 1) )
#define TMP_START(ap,v)  	( ap = (TMP_LIST)&v + TMP_INTSIZEOF(v) )
VOID TMP_WA(const UINT8 *Format, ...) {
    TMP_LIST Marker;
    TMP_START(Marker, Format);
}

EFI_STATUS SmcSwSmiFlashWrite (
    VOID* FlashAddress,
    UINTN Size,
    VOID* DataBuffer
)
{
    EFI_STATUS Status = EFI_SUCCESS;
    UINT32 Data32;
    SMC_SW_SMI_FLASH_HEADER SmcSwSmiFlashHeader;
    EFI_SMM_CONTROL2_PROTOCOL *gSmmCtl = NULL;
    UINT8 SwSmiValue = SWSMI_SMC_FLASH;
    UINT8 DataPort = SWSMI_E9_SUBCMD_FLASH_WRITE;

    //
    // Trigger a software SMI to write data to ROM.
    //
    Status = gBS->LocateProtocol(&gEfiSmmControl2ProtocolGuid, NULL, &gSmmCtl);
    if (EFI_ERROR(Status)) {
        return Status;
    }

    SmcSwSmiFlashHeader.FlashAddress = FlashAddress;
    SmcSwSmiFlashHeader.Size = Size;
    SmcSwSmiFlashHeader.DataBuffer = DataBuffer;

    //
    // Save the buffer address to CMOS. The address can be passed to SMI by CMOS.
    //
    Data32 = (UINT32)(&SmcSwSmiFlashHeader);
    SetCmosTokensValue (Q_SMC_ADDR_HH, (Data32 >> 24) & 0xff);
    SetCmosTokensValue (Q_SMC_ADDR_H, (Data32 >> 16) & 0xff);
    SetCmosTokensValue (Q_SMC_ADDR_L, (Data32 >> 8) & 0xff);
    SetCmosTokensValue (Q_SMC_ADDR_LL, Data32 & 0xff);
	TMP_WA("%x", Data32);

    //
    // Trigger a software SMI to write data to ROM.
    //
    DataPort = SWSMI_E9_SUBCMD_FLASH_WRITE;
    Status = gSmmCtl->Trigger(gSmmCtl, &SwSmiValue, &DataPort, 0, 0);

    return Status;
}

