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

/** @file UsbMisc.c
    AMI USB driver miscellaneous routines

**/

#include <Efi.h>

#include "AmiDef.h"
#include "UsbDef.h"
#include "Uhcd.h"

#if USB_RUNTIME_DRIVER_IN_SMM
#include <Protocol/SmmControl2.h>
EFI_SMM_CONTROL2_PROTOCOL *gSmmCtl = NULL;
#endif

BOOLEAN gFirstCall = TRUE;
VOID *gGlobalPointer;
VOID *gStartPointer;
VOID *gEndPointer;

extern USB_GLOBAL_DATA             *gUsbData;
extern EFI_USB_PROTOCOL            *gAmiUsbController;

/**
    Generates SW SMI using global SmmCtl pointer.

**/

VOID
USBGenerateSWSMI (
    UINT8   Data
)
{
#if USB_RUNTIME_DRIVER_IN_SMM
	EFI_STATUS Status;
	UINT8 SwSmiValue = Data;
	UINT8 DataSize = 1;

	if (gSmmCtl == NULL) {
	    Status = gBS->LocateProtocol(&gEfiSmmControl2ProtocolGuid, NULL, &gSmmCtl);
		if (EFI_ERROR(Status)){
			return;
		}
	}
    gSmmCtl->Trigger(gSmmCtl, &SwSmiValue, &DataSize, 0, 0);
#endif
}

/**

**/

VOID
InvokeUsbApi(
	URP_STRUC *Urp
)
{
    EFI_TPL OldTpl;
    UINTN   Temp;

    if (gAmiUsbController->UsbInvokeApi) {   
        OldTpl = gBS->RaiseTPL(TPL_NOTIFY);
		gAmiUsbController->UsbInvokeApi(Urp);
        gBS->RestoreTPL(OldTpl);
	} else {
        Temp = (UINTN)gUsbData->fpURP;
        gUsbData->fpURP = Urp;
        USBGenerateSWSMI(USB_SWSMI);
        gUsbData->fpURP = (URP_STRUC*)Temp;
    }
}


/**
    Returns the aligned address.

**/

UINTN
AlignPhysicalAddress(
    UINTN   PhyAddress,
    UINT16  AlignSize
)
{
    UINTN   AlignAddr;

    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_7, "Un-aligned address : %lX\n", PhyAddress);
    if ((PhyAddress % AlignSize) != 0) {
        AlignAddr = PhyAddress - (PhyAddress % (UINT32)AlignSize) + AlignSize;
    }
    else {
        AlignAddr = PhyAddress;
    }
    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_7, "Aligned address : %lX\n", AlignAddr);

    return AlignAddr;
}


/**
    Allocates memory with the given alignment.

**/

VOID*
AllocAlignedMemory (
    UINT32 AllocSize,
    UINT16 Align
)
{
    UINTN                   Ptr;
    UINT32                  Size;
    EFI_STATUS              Status;
    EFI_PHYSICAL_ADDRESS    MemAddress;

    if (AllocSize == 0) {
        return NULL;
    }
    //
    // If this is the first time the function is called,
    // allocate the USB memory and make the size 4K aligned (VTD).
    //
    if (gFirstCall) {
        gFirstCall = FALSE;                 // Make sure to only allocate once.
        Size = CalculateMemorySize();       // Determine total required size.
        Size = (Size + 0x1000) >> 12;       // Express Size in pages.
        //
        // Allocate and zero memory in pages.
        //
        MemAddress = 0xFFFFFFFF;
        Status = gBS->AllocatePages(AllocateMaxAddress, EfiRuntimeServicesData,
                Size, &MemAddress);
        if (EFI_ERROR(Status)) {
            Status = gBS->AllocatePages(AllocateAnyPages, EfiRuntimeServicesData, Size, &MemAddress);
            ASSERT_EFI_ERROR(Status);
            if (EFI_ERROR(Status)) {
                return NULL;
            }
        }

        gGlobalPointer = (VOID*)(UINTN)MemAddress;
        gBS->SetMem (gGlobalPointer, (Size << 12), 0);
        //
        // Save pointers to beginning and end of region.
        //
        gStartPointer = gGlobalPointer;
        gEndPointer = (VOID *)((UINTN)gGlobalPointer + (Size << 12) - 1);
    }

    //USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_6, "Unaligned : %Fp, %X, %X\n", gGlobalPointer, AllocSize, Align);
    Ptr = AlignPhysicalAddress((UINTN)gGlobalPointer, Align);
    //USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_6, "Aligned : %Fp, %X, %X\n", Ptr, AllocSize, Align);

    gGlobalPointer = (VOID*)(Ptr + AllocSize);

    if (gGlobalPointer < gEndPointer) {
        return (VOID*)Ptr;
    }
    return NULL;
}


/**
    Allocates a number of pages with the given alignment.

    @note  The minimum alignment passed to this function is CPU page, 4K

**/

VOID *
AllocateHcMemory (
	IN EFI_PCI_IO_PROTOCOL	*PciIo,
    IN UINTN            	Pages,
    IN UINTN            	Alignment
)
{
    EFI_STATUS            Status;
    VOID                  *Memory;
    UINTN                 AlignedMemory;
    UINTN                 AlignmentMask;
    UINTN                 UnalignedPages;
    UINTN                 RealPages;

    //
    // Alignment must be a power of two or zero.
    //
    ASSERT ((Alignment & (Alignment - 1)) == 0);
 
    if (Pages == 0) {
        return NULL;
    }
    if (Alignment > EFI_PAGE_SIZE) {
        //
        // Caculate the total number of pages since alignment is larger than page size.
        //
        AlignmentMask  = Alignment - 1;
        RealPages      = Pages + EFI_SIZE_TO_PAGES (Alignment);
        //
        // Make sure that Pages plus EFI_SIZE_TO_PAGES (Alignment) does not overflow.
        //
        ASSERT (RealPages > Pages);

 		Memory = (VOID*)0xFFFFFFFF;
        Status = PciIo->AllocateBuffer(PciIo, AllocateMaxAddress, EfiRuntimeServicesData,
                                    RealPages, &Memory, 0);
        if (EFI_ERROR(Status)) {
            Status = PciIo->AllocateBuffer(PciIo, AllocateAnyPages, EfiRuntimeServicesData,
                                    RealPages, &Memory, 0);
            if (EFI_ERROR(Status)) {
                return NULL;
            }
        }
        AlignedMemory  = ((UINTN) Memory + AlignmentMask) & ~AlignmentMask;
        UnalignedPages = EFI_SIZE_TO_PAGES(AlignedMemory - (UINTN) Memory);
        if (UnalignedPages > 0) {
            //
            // Free first unaligned page(s).
            //
            Status = PciIo->FreeBuffer(PciIo, UnalignedPages, Memory);
            ASSERT_EFI_ERROR (Status);
        }
        Memory = (VOID*)(AlignedMemory + EFI_PAGES_TO_SIZE(Pages));
        UnalignedPages = RealPages - Pages - UnalignedPages;
        if (UnalignedPages > 0) {
            //
            // Free last unaligned page(s).
            //
			Status = PciIo->FreeBuffer(PciIo, UnalignedPages, Memory);
            ASSERT_EFI_ERROR (Status);
        }
    } else {
        //
        // Do not over-allocate pages in this case.
        //
        Memory = (VOID*)0xFFFFFFFF;
		Status = PciIo->AllocateBuffer(PciIo, AllocateMaxAddress, EfiRuntimeServicesData,
                                    Pages, &Memory, 0);
        if (EFI_ERROR(Status)) {
    		Status = PciIo->AllocateBuffer(PciIo, AllocateAnyPages, EfiRuntimeServicesData,
                                        Pages, &Memory, 0);
            if (EFI_ERROR(Status)) {
                return NULL;
            }
        }
        AlignedMemory  = (UINTN) Memory;
    }
    return (VOID*) AlignedMemory;
}

/**
    Free the memory allocated by AllocateHcMemory().

**/

VOID
FreeHcMemory(
	IN EFI_PCI_IO_PROTOCOL	*PciIo,
	IN UINTN				Pages,
	IN VOID*				Memory
)
{
	EFI_STATUS            Status;

	Status = PciIo->FreeBuffer(PciIo, Pages, Memory);
	ASSERT_EFI_ERROR(Status);
	return;
}

/**

**/

EFI_STATUS
ReallocateMemory (
  IN UINTN  OldSize,
  IN UINTN  NewSize,
  IN VOID   **OldBuffer
)
{
	EFI_STATUS  Status;
    VOID        *NewBuffer = NULL;
	VOID		*FreeBuffer = NULL;

	if (OldBuffer == NULL) {
		return EFI_INVALID_PARAMETER;
	}

	Status = gBS->AllocatePool (EfiRuntimeServicesData, NewSize, &NewBuffer);
    if (EFI_ERROR(Status)) {
        return Status;
    }

    pBS->SetMem(NewBuffer, NewSize, 0);

	if (OldSize > 0 && *OldBuffer != NULL) {
		gBS->CopyMem(NewBuffer, *OldBuffer, (OldSize < NewSize) ? OldSize : NewSize);
		FreeBuffer = *OldBuffer;
	}

	*OldBuffer = NewBuffer;

    if (FreeBuffer != NULL) {
        gBS->FreePool(FreeBuffer);
    }
    return EFI_SUCCESS;
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
