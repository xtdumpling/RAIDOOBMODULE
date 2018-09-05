//****************************************************************************
//****************************************************************************
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//****************************************************************************
//****************************************************************************
//  File History
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
// Name:        SmcSwSmiFlashDxe.c
//
// Description:
//
//<AMI_FHDR_END>
//**********************************************************************

#include "Token.h"
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Protocol/SmmControl2.h>
#include "SmcSwSmiFlashSmm.h"



VOID
EFIAPI
UnregisterSwSmiE9BeforeOsBoot (
    IN EFI_EVENT Event,
    IN VOID *Content
)
{
    EFI_STATUS Status;
    EFI_SMM_CONTROL2_PROTOCOL *gSmmCtl = NULL;
    UINT8 SwSmiValue = SWSMI_SMC_FLASH;
    UINT8 DataPort = SWSMI_E9_SUBCMD_UNREGISTER;

    //
    // Trigger a software SMI to copy FULL SmBios data to SMRAM.
    //
    Status = gBS->LocateProtocol(&gEfiSmmControl2ProtocolGuid, NULL, &gSmmCtl);
    if (!EFI_ERROR(Status)) {
        DataPort = SWSMI_E9_SUBCMD_UNREGISTER;
        gSmmCtl->Trigger(gSmmCtl, &SwSmiValue, &DataPort, 0, 0);
    }

    if (Event != NULL) {
        gBS->CloseEvent (Event);
    }
    return;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Name:  SmcSwSmiFlashDxeEntry
//
// Description:
//
// Steps :
//
// Input:
//      IN EFI_HANDLE ImageHandle
//                  - Image handle
//      IN EFI_SYSTEM_TABLE *SystemTable
//                  - System table pointer
//
// Output:
//      EFI_STATUS (Return Value)
//                  = EFI_SUCCESS if successful
//                  = or other valid EFI error code
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS
SmcSwSmiFlashDxeEntry(
    IN EFI_HANDLE ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable
)
{
    EFI_STATUS Status;
    EFI_HANDLE Handle = NULL;
    EFI_EVENT Event;

    //InitAmiLib(ImageHandle, SystemTable);

    DEBUG((-1, "SmcSwSmiFlashDxeEntry entry.\n"));

    //
    // Create event to unregister SmcInBandSwSmiSpiWriteHandler before OS boot.
    //
    Status = gBS->CreateEventEx (
                 EVT_NOTIFY_SIGNAL,
                 TPL_NOTIFY,
                 UnregisterSwSmiE9BeforeOsBoot,
                 NULL,
                 &gEfiEventExitBootServicesGuid,
                 &Event
             );
    ASSERT_EFI_ERROR (Status);

    Status = gBS->CreateEventEx (
                 EVT_NOTIFY_SIGNAL,
                 TPL_NOTIFY,
                 UnregisterSwSmiE9BeforeOsBoot,
                 NULL,
                 &gEfiEventLegacyBootGuid,
                 &Event
             );
    ASSERT_EFI_ERROR (Status);

    DEBUG((-1, "SmcSwSmiFlashDxeEntry end.\n"));

    return EFI_SUCCESS;
}
