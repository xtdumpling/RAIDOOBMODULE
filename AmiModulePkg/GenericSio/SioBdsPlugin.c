//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

#include <IndustryStandard/Pci.h>
#include <Library/UefiBootServicesTableLib.h>

// Defined in Bds.c
EFI_STATUS GetPciHandlesByClass(
    UINT8 Class, UINT8 SubClass, UINTN *NumberOfHandles, EFI_HANDLE **HandleBuffer
);

VOID ConnectSio(){
    EFI_STATUS Status;
    EFI_HANDLE *Handle;
    UINTN Number,i;

    //Get a list of all PCI to ISA Bridges
    Status = GetPciHandlesByClass(
        PCI_CLASS_BRIDGE, PCI_CLASS_BRIDGE_ISA, &Number, &Handle
    );
    if (EFI_ERROR(Status)) return;
    for(i=0; i<Number; i++)
    {
        gBS->ConnectController(Handle[i],NULL,NULL,FALSE);
    }
    gBS->FreePool(Handle);
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
