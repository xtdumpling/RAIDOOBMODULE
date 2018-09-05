//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file UsbBbs.c

**/

#include <AmiDxeLib.h>
#include <Protocol/AmiUsbController.h>

/**
    Create BBS table for each USB mass storage device.

**/

VOID
CollectUsbBbsDevices (
    VOID
)
{
    EFI_STATUS  Status;
    EFI_USB_PROTOCOL        *AmiUsb;

    Status = pBS->LocateProtocol(&gEfiUsbProtocolGuid, NULL, &AmiUsb);
    if (EFI_ERROR(Status)) {
        return;
    }

    AmiUsb->InstallUsbLegacyBootDevices();
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
