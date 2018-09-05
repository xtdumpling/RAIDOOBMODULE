//***************************************************************************
//**       (C)Copyright 1993-2016 Supermicro Computer, Inc.                **
//***************************************************************************
//
//  File History
//
//  Rev. 1.05
//    Bug Fix:  Fix system hang after insert OA key by afu tool.
//    Reason:
//    Auditor:  Kasber Chen
//    Date:     Feb/09/2017
//
//  Rev. 1.04
//    Bug Fix:  Support SmcRomHole preserve in IPMI.
//    Reason:
//    Auditor:  Kasber Chen
//    Date:     Jan/06/2017
//
//  Rev. 1.03
//    Bug Fix:  Trigger a software SMI to flash ROM part after BIOS lock.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Sep/30/2016
//
//  Rev. 1.02
//    Bug Fix:  Change Flash to SmcFlash for redefine issue.
//    Auditor:  Kasber Chen
//    Date:     Aug/18/2016
//
//  Rev. 1.01
//    Bug Fix:  Fix system hangs at Sum InBand command.
//    Reason:   Because of using Boot Service in SMM. Separate SmcRomHole into two libraries - DXE and SMM.
//    Auditor:  Hartmann Hsieh
//    Date:     Jul/12/2016
//
//  Rev. 1.00
//    Bug Fix:  Renew the SMC ROM Hole.
//    Reason:
//    Auditor:  Kasber Chen
//    Date:     May/04/2016
//
//****************************************************************************
#include <Token.h>
#include "EFI.h"
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Protocol/FlashProtocol.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Protocol/SmmControl2.h>
#include "SmcSwSmiFlashLib.h"
#include <SmcRomHole.h>


FLASH_PROTOCOL *SmcFlash = NULL;
#if DEBUG_MODE == 1
extern CHAR8* SmcRegionStr[];
#endif


EFI_STATUS AllocateBuffer (VOID** Buffer, UINTN BufferSize)
{
    return gBS->AllocatePool(EfiBootServicesData, BufferSize, Buffer);
}

EFI_STATUS FreeBuffer (UINT8* Buffer)
{
    return gBS->FreePool(Buffer);
}

EFI_STATUS
SmcRomHoleWriteRegion(
    IN	UINT8	Operation,
    IN	UINT8	*pBuffer
)
{
    EFI_STATUS Status;
    UINT32 TempSize, Offset;
    UINT8 *TempBuffer;
    EFI_SMM_CONTROL2_PROTOCOL *gSmmCtl = NULL;
    BOOLEAN RunSwSmiFlash = FALSE;

    DEBUG((-1, "SmcRomHole.c - SmcRomHoleWriteRegion Start.\n"));
#if DEBUG_MODE == 1
    DEBUG((-1, "Operation = %s\n", SmcRegionStr[Operation]));
#endif

    if(SmcFlash == NULL) return EFI_NOT_READY;

    //
    // Please use "AllocateBuffer" to alocate memory and use "FreeBuffer" to free memory.
    // Separate SmcRomHole into two libraries - DXE and SMM.
    // In DXE, "AllocateBuffer" uses "gBS->AllocatePool" to allocate memory.
    // In SMM, "AllocateBuffer" uses "pSmst->SmmAllocatePool" to allocate memory.
    // "FreeBuffer" is the same.
    //
    Status = AllocateBuffer (&TempBuffer, SMC_ROM_HOLE_SIZE);
    if(Status)	return Status;

    if(Operation == FULL_REGION) {
        TempSize = SMC_ROM_HOLE_SIZE;
        Offset = 0;
    } else {
        TempSize = SmcRomHoleSize(Operation);
        Offset = SmcRomHoleOffset(Operation);
    }

    Status = SmcFlash->Read((UINT8*)SMC_ROM_HOLE_LOCATION, SMC_ROM_HOLE_SIZE, TempBuffer);
    if(Status) {
        FreeBuffer(TempBuffer);
        return Status;
    }

    CopyMem(TempBuffer + Offset, pBuffer, TempSize);

    //
    // Try to trigger a SW SMI to write ROM part.
    //
    Status = gBS->LocateProtocol(&gSmcSwSmiFlashProtocolGuid, NULL, &gSmmCtl);
    if (!EFI_ERROR(Status)) {
        Status = SmcSwSmiFlashWrite ((UINT8*)SMC_ROM_HOLE_LOCATION, SMC_ROM_HOLE_SIZE, TempBuffer);
        if (!EFI_ERROR(Status)) {
            RunSwSmiFlash = TRUE;
        }
    }

    //
    // If can't trigger SW SMI, write ROM part directly.
    //
    if (RunSwSmiFlash == FALSE) {
        Status = SmcFlash->DeviceWriteEnable();
        if(Status) {
            FreeBuffer(TempBuffer);
            return Status;
        }
        Status = SmcFlash->Erase((UINT8*)SMC_ROM_HOLE_LOCATION, SMC_ROM_HOLE_SIZE);
        if(Status) {
            FreeBuffer(TempBuffer);
            return Status;
        }
        Status = SmcFlash->Write((UINT8*)SMC_ROM_HOLE_LOCATION, SMC_ROM_HOLE_SIZE, TempBuffer);
        if(Status) {
            FreeBuffer(TempBuffer);
            return Status;
        }
        Status = SmcFlash->DeviceWriteDisable();
    }

    FreeBuffer(TempBuffer);

    DEBUG((-1, "SmcRomHole.c - SmcRomHoleWriteRegion End.\n"));

    return Status;
}

EFI_STATUS
EFIAPI
SmcRomHoleInit()
{
    EFI_STATUS Status = EFI_SUCCESS;
    DEBUG((-1, "SmcRomHoleInit Start.\n"));

    Status = gBS->LocateProtocol(&gFlashProtocolGuid, NULL, &SmcFlash);
    DEBUG((-1, "gFlashProtocolGuid Status = %r.\n", Status));
    if (EFI_ERROR(Status)) SmcFlash = NULL;

    DEBUG((-1, "SmcRomHoleInit End.\n"));
    return Status;
}
