//****************************************************************************
//****************************************************************************
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//****************************************************************************
//****************************************************************************
//  File History
//
//  Rev. 1.02
//    Bug Fix:  Fix system hang after insert OA key by afu tool.
//    Reason:
//    Auditor:  Kasber Chen
//    Date:     Feb/09/2017
//
//  Rev. 1.01
//    Bug Fix:  Fix system hanging when enabling debug mode.
//    Reason:   
//    Auditor:  Hartmann Hsieh
//    Date:     Oct/13/2016
//
//  Rev. 1.00
//    Bug Fix:  Trigger a software SMI to flash ROM part after BIOS lock.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Sep/30/2016
//
//****************************************************************************

//**********************************************************************
//<AMI_FHDR_START>
//
// Name:        SmcSwSmiFlash.c
//
// Description:
//
//<AMI_FHDR_END>
//**********************************************************************

#include "Token.h"
#include <AmiDxeLib.h>
#include <AmiCspLib.h>
#include <AmiSmm.h>
#include <Protocol\SmmSwDispatch2.h>
#include <Protocol\FlashProtocol.h>
#include "SspTokens.h"
#include "SmcLib.h"
#include "SmcSwSmiFlashSmm.h"


BOOLEAN DisableSmcSwSmiFlash = FALSE;

EFI_STATUS
SmcSwSmiFlashCallBack (
    IN EFI_HANDLE DispatchHandle,
    IN CONST VOID *Context OPTIONAL,
    IN OUT VOID *CommBuffer OPTIONAL,
    IN OUT UINTN *CommBufferSize OPTIONAL
)
{
    EFI_STATUS Status = EFI_SUCCESS;
    FLASH_PROTOCOL *SmcRomHoleFlash = NULL;
    UINT8 DataPort = 0xff;
    UINT8* pBuffer = NULL;
    UINT32 Data32 = 0;
    UINT32 Temp32 = 0;
    SMC_SW_SMI_FLASH_HEADER* pSmcSwSmiFlashHeader = NULL;

    TRACE ((TRACE_ALWAYS, "SmcSwSmiFlashSmm.c SmcInBandSwSmiSpiWriteHandler entry.\n"));

    if (DisableSmcSwSmiFlash) return Status;

    DataPort = ((EFI_SMM_SW_CONTEXT*)CommBuffer)->DataPort;

    switch(DataPort) {
    case SWSMI_E9_SUBCMD_FLASH_WRITE:

        Status = pSmst->SmmLocateProtocol(&gAmiSmmFlashProtocolGuid, NULL, &SmcRomHoleFlash);
        if (!EFI_ERROR (Status)) {
            //
            // Get parameters from CMOS.
            //
            Data32 = GetCmosTokensValue (Q_SMC_ADDR_HH);
            Temp32 |= Data32 << 24;
            Data32 = GetCmosTokensValue (Q_SMC_ADDR_H);
            Temp32 |= Data32 << 16;
            Data32 = GetCmosTokensValue (Q_SMC_ADDR_L);
            Temp32 |= Data32 << 8;
            Data32 = GetCmosTokensValue (Q_SMC_ADDR_LL);
            Temp32 |= Data32;
            pBuffer = (UINT8*)Temp32;

            pSmcSwSmiFlashHeader = (SMC_SW_SMI_FLASH_HEADER*)pBuffer;

            SmcRomHoleFlash->DeviceWriteEnable();
            SmcRomHoleFlash->Erase(pSmcSwSmiFlashHeader->FlashAddress, pSmcSwSmiFlashHeader->Size);
            SmcRomHoleFlash->Write(pSmcSwSmiFlashHeader->FlashAddress, pSmcSwSmiFlashHeader->Size, pSmcSwSmiFlashHeader->DataBuffer);
            SmcRomHoleFlash->DeviceWriteDisable();

        }
        else {
            TRACE ((TRACE_ALWAYS, "Error : Can't locate SMM protocol - 'gAmiSmmFlashProtocolGuid'\n"));
        }

        break;
    case SWSMI_E9_SUBCMD_UNREGISTER:
        DisableSmcSwSmiFlash = TRUE;
        break;
    }

    TRACE ((TRACE_ALWAYS, "SmcSwSmiFlashSmm.c SmcSwSmiFlashCallBack end.\n"));

    return Status;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Name:  InSmmFunction
//
// Description:
//
// Input:
//      IN EFI_HANDLE ImageHandle
//                  - Image handle
//      IN EFI_SYSTEM_TABLE *SystemTable
//                  - System table pointer
//
// Output:
//      EFI_STATUS
//          EFI_SUCCESS - Success
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS InSmmFunction(
    IN EFI_HANDLE       ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable
)
{
    EFI_STATUS Status;
    EFI_SMM_SW_DISPATCH2_PROTOCOL *pSwDispatch2;
    EFI_SMM_SW_REGISTER_CONTEXT SwSmiSmcFlashContext = {SWSMI_SMC_FLASH};
    EFI_HANDLE SwSmiSmcFlashHandle = NULL, Handle = NULL;

    TRACE ((TRACE_ALWAYS, "SmcSwSmiFlashSmm.c - InSmmFunction entry.\n"));

    //
    // Register Sw Smi for writing flash part.
    // Because that write flash part must be in SMM after BIOS lock.
    //
    Status = pSmst->SmmLocateProtocol(&gEfiSmmSwDispatch2ProtocolGuid, NULL, &pSwDispatch2);

    if(!EFI_ERROR(Status)) {
        Status = pSwDispatch2->Register(
                     pSwDispatch2,
                     SmcSwSmiFlashCallBack,
                     &SwSmiSmcFlashContext,
                     &SwSmiSmcFlashHandle);

        if(!Status)
            pBS->InstallProtocolInterface(
                            &Handle,
                            &gSmcSwSmiFlashProtocolGuid,
                            EFI_NATIVE_INTERFACE,
                            NULL);
    }
    else {
        TRACE ((TRACE_ALWAYS, "Error : Can't locate SMM protocol - 'gEfiSmmSwDispatch2ProtocolGuid'\n"));
    }

    TRACE ((TRACE_ALWAYS, "SmcSwSmiFlashSmm.c - InSmmFunction end.\n"));

    return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Name:  InitSmcSwSmiFlashSmm
//
// Description:
//
// Input:
//      IN EFI_HANDLE ImageHandle
//                  - Image handle
//      IN EFI_SYSTEM_TABLE *SystemTable
//                  - System table pointer
//
// Output:
//      EFI_STATUS
//          EFI_SUCCESS - Success
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS InitSmcSwSmiFlashSmm(
    IN EFI_HANDLE	ImageHandle,
    IN EFI_SYSTEM_TABLE	*SystemTable
)
{
    EFI_STATUS Status = EFI_SUCCESS;

    InitAmiLib(ImageHandle, SystemTable);

    TRACE ((TRACE_ALWAYS, "InitSmcSwSmiFlashSmm entry.\n"));

    return InitSmmHandler(ImageHandle, SystemTable, InSmmFunction, NULL);
}
