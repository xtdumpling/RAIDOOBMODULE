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

/** @file Elib.c
    AMI USB MEM/IO/PCI access routines

**/

#include <Library/IoLib.h>
#include "AmiDef.h"
#include "UsbDef.h"
#if !USB_RT_DXE_DRIVER
#include <Library/AmiBufferValidationLib.h>
#endif

extern  USB_GLOBAL_DATA         *gUsbData;

/**
    This routine delays for specified number of micro seconds

    @param Usec      Amount of delay (count in 1 microsec)

    @retval VOID

**/

VOID 
FixedDelay(
    UINTN           Usec                           
 )
{
#if !USB_RT_DXE_DRIVER
    UINTN   Counter, i;
    UINT32  Data32, PrevData;

    Counter = Usec * 3;
    Counter += Usec / 2;
    Counter += (Usec * 2) / 25;

    //
    // Call WaitForTick for Counter + 1 ticks to try to guarantee Counter tick
    // periods, thus attempting to ensure Microseconds of stall time.
    //
    if (Counter != 0) {

        PrevData = IoRead32(PM_BASE_ADDRESS + 8);
        for (i=0; i < Counter; ) {
            Data32 = IoRead32(PM_BASE_ADDRESS + 8);    
            if (Data32 < PrevData) {        // Reset if there is a overlap
                PrevData=Data32;
                continue;
            }
            i += (Data32 - PrevData);        
            PrevData = Data32;
        }
    }
#else
	pBS->Stall(Usec);
#endif
    return;
}

EFI_STATUS
UsbHcStrucValidation(
    HC_STRUC* HcStruc
)
{
    UINTN       Index;

    if (HcStruc == NULL) {
        return EFI_ACCESS_DENIED;
    }
    
    for (Index = 0; Index < gUsbData->HcTableCount; Index++) {
        if (HcStruc == gUsbData->HcTable[Index]) {
            break;
        }
    }

    if (Index == gUsbData->HcTableCount) {
        return EFI_ACCESS_DENIED;
    }

    if (!(HcStruc->dHCFlag & HC_STATE_USED)) {
        return EFI_ACCESS_DENIED;
    }

    return EFI_SUCCESS;
}

EFI_STATUS
UsbDevInfoValidation(
    DEV_INFO* DevInfo
)
{
    UINTN       Index;
    UINT8       *MemBlockEnd = gUsbData->fpMemBlockStart + (gUsbData->MemPages << 12);

    if (DevInfo == NULL) {
        return EFI_ACCESS_DENIED;
    }
    
    for (Index = 0; Index < MAX_DEVICES; Index++) {
        if (DevInfo == &gUsbData->aDevInfoTable[Index]) {
            break;
        }
    }

    if (Index == MAX_DEVICES) {
        return EFI_ACCESS_DENIED;
    }

#if USB_RUNTIME_DRIVER_IN_SMM
    if (DevInfo->fpPollEDPtr) {
        if ((DevInfo->fpPollEDPtr < gUsbData->fpMemBlockStart) ||
            ((DevInfo->fpPollEDPtr + sizeof(MEM_BLK)) > MemBlockEnd)) {
            return EFI_ACCESS_DENIED;
        }
    }

    if (DevInfo->fpPollTDPtr) {
        if ((DevInfo->fpPollTDPtr < gUsbData->fpMemBlockStart) ||
            ((DevInfo->fpPollTDPtr + sizeof(MEM_BLK)) > MemBlockEnd)) {
            return EFI_ACCESS_DENIED;
        }
    }

    if (DevInfo->fpPollDataBuffer) {
        if ((DevInfo->fpPollDataBuffer < gUsbData->fpMemBlockStart) ||
            ((DevInfo->fpPollDataBuffer + DevInfo->PollingLength) > MemBlockEnd)) {
            return EFI_ACCESS_DENIED;
        }
    }
#endif

    return EFI_SUCCESS;
}

/**
    This routine reads a DWORD from the specified Memory Address

    @param 
        BaseAddr   - Memory address
        Offset     - Offset of BaseAddr

    @retval Value read

**/

UINT32
DwordReadMem(
    UINTN   BaseAddr, 
    UINT16  Offset
)
{
    return MmioRead32((UINTN)(BaseAddr + Offset));
}


/**
    This routine writes a dword to a specified Memory Address

    @param 
        BaseAddr   - Memory address
        Offset     - Offset of BaseAddr
        Value      - Data to write

**/

VOID
DwordWriteMem(
    UINTN   BaseAddr, 
    UINT16  Offset, 
    UINT32  Value
)
{
    MmioWrite32((UINTN)(BaseAddr + Offset), Value);
}


/**
    This routine resets the specified bits at specified memory address

    @param 
        BaseAddr   - Memory address
        Offset     - Offset of BaseAddr
        Value      - Data to reset

    @retval VOID

**/

VOID
DwordResetMem(
    UINTN   BaseAddr, 
    UINT16  Offset, 
    UINT32  Value
)
{
    UINT32 Data = DwordReadMem(BaseAddr, Offset);
    
    Data &= ~Value;
    DwordWriteMem(BaseAddr, Offset, Data);
}


/**
    This routine sets the specified bits at specified memory address

    @param 
        BaseAddr   - Memory address
        Offset     - Offset of BaseAddr
        Value      - Data to set

    @retval VOID

**/

VOID
DwordSetMem(
    UINTN   BaseAddr, 
    UINT16  Offset, 
    UINT32  Value
)
{
    UINT32 Data = DwordReadMem(BaseAddr, Offset);
    
    Data |= Value;
    DwordWriteMem(BaseAddr, Offset, Data);
}


/**
    This routine reads a byte from the specified IO address

    @param IoAddr     I/O address to read

    @retval Value read

**/

UINT8
ByteReadIO(
    UINT16 IoAddr
)
{
    return IoRead8(IoAddr);
}


/**
    This routine writes a byte to the specified IO address

    @param
        IoAddr     I/O address to write
        Value      Byte value to write

    @retval VOID

**/

VOID
ByteWriteIO(
    UINT16  IoAddr,
    UINT8   Value
)
{
    IoWrite8(IoAddr, Value);
}


/**
    This routine reads a word from the specified IO address

    @param IoAddr     I/O address to read

    @retval Value read

**/

UINT16
WordReadIO(
    UINT16 IoAddr
)
{
    return IoRead16(IoAddr);
}


/**
    This routine writes a word to the specified IO address

    @param
        IoAddr     I/O address to write
        Value      Word value to write

    @retval VOID

**/

VOID
WordWriteIO(
    UINT16 IoAddr,
    UINT16 Value
)
{
    IoWrite16(IoAddr, Value);
}


/**
    This routine reads a dword from the specified IO address

    @param IoAddr     I/O address to read

    @retval Value read

**/

UINT32
DwordReadIO(
    UINT16 IoAddr
)
{
    return IoRead32(IoAddr);
}


/**
    This routine writes a double word to the specified IO address

    @param
        IoAddr     I/O address to write
        Value      Double word value to write

    @retval VOID

**/

VOID
DwordWriteIO(
    UINT16 IoAddr, 
    UINT32 Value
)
{
    IoWrite32(IoAddr, Value);
}


/**
    This routine reads from the PCI configuration space register
    the value can be typecasted to 8bits - 32bits

    @param
        BusDevFunc - Bus, device & function number of the PCI device
        Register   - Register offset to read

    @retval Value read

**/

UINT32
ReadPCIConfig(
    UINT16  BusDevFunc,
    UINT8   Register
)
{
    UINT32  Data;
    UINTN   Address = (gUsbData->PciExpressBaseAddress + (UINTN)(BusDevFunc << 12 | (Register & 0xFC)));

    Data = *(volatile UINT32*)(Address);
   
    return (Data >> ((Register & 3) << 3));
}

/**
    This routine writes a byte value to the PCI configuration
    register space

    @param
        BusDevFunc - Bus, device & function number of the PCI device
        Register   - Register offset to write
        Value      - Value to write

**/

VOID
ByteWritePCIConfig(
    UINT16  BusDevFunc,
    UINT8   Register,
    UINT8   Value
)
{
    UINTN   Address = (gUsbData->PciExpressBaseAddress + (UINTN)(BusDevFunc << 12 | Register));

    *(volatile UINT8*)(Address) = Value;
}

/**
    This routine writes a word value to the PCI configuration
    register space

    @param
        BusDevFunc - Bus, device & function number of the PCI device
        Register   - Register offset to write
        Value      - Value to write

**/

VOID
WordWritePCIConfig(
    UINT16  BusDevFunc,
    UINT8   Register,
    UINT16  Value
)
{
    UINTN   Address = (gUsbData->PciExpressBaseAddress + (UINTN)(BusDevFunc << 12 | Register));

    *(volatile UINT16*)(Address) = Value;
}

/**
    This routine writes a Dword value to the PCI configuration
    register space

    @param
        BusDevFunc - Bus, device & function number of the PCI device
        Register   - Register offset to write
        Value      - Value to write

**/

VOID
DwordWritePCIConfig(
    UINT16  BusDevFunc,
    UINT8   Register,
    UINT32  Value
)
{
    UINTN   Address = (gUsbData->PciExpressBaseAddress + (UINTN)(BusDevFunc << 12 | Register));

    *(volatile UINT32*)(Address) = Value;
}

/**
    This routine reads a dword value from the PCI configuration
    register space of the host controller.

    @param
        HcStruc     - HcStruc pointer
        Offset      - Register offset to read

**/

UINT32
HcReadPciReg(
    HC_STRUC	*HcStruc,
    UINT32		Offset
)
{
#if !USB_RT_DXE_DRIVER
    return ReadPCIConfig(HcStruc->wBusDevFuncNum, Offset);
#else
    EFI_STATUS	            Status;
	UINT32                  Data = 0;
    EFI_PCI_IO_PROTOCOL	    *PciIo = HcStruc->PciIo;

    Status = PciIo->Pci.Read(PciIo, EfiPciIoWidthUint32, Offset, 1, &Data);
    ASSERT_EFI_ERROR(Status);
    return Data;
#endif
}

/**
    This routine writes a dword value to the PCI configuration
    register space of the host controller.

    @param
        HcStruc     - HcStruc pointer
        Offset      - Register offset to write
        Data        - Value to write

**/

VOID
HcWritePciReg(
    HC_STRUC    *HcStruc,
    UINT32      Offset,
    UINT32      Data
)
{
#if !USB_RT_DXE_DRIVER
    DwordWritePCIConfig(HcStruc->wBusDevFuncNum, Offset, Data);
    return;
#else
	EFI_STATUS              Status;
    EFI_PCI_IO_PROTOCOL	    *PciIo = HcStruc->PciIo;

    Status = PciIo->Pci.Write(PciIo, EfiPciIoWidthUint32, Offset, 1, &Data);
    ASSERT_EFI_ERROR(Status);
    return;
#endif
}

/**
    This routine writes a word value to the PCI configuration
    register space of the host controller.

    @param
        HcStruc     - HcStruc pointer
        Offset      - Register offset to write
        Data        - Value to write

**/

VOID
HcWordWritePciReg(
    HC_STRUC    *HcStruc,
    UINT32      Offset,
    UINT16      Data
)
{
#if !USB_RT_DXE_DRIVER
    WordWritePCIConfig(HcStruc->wBusDevFuncNum, Offset, Data);
    return;
#else
    EFI_STATUS	            Status;
    EFI_PCI_IO_PROTOCOL	    *PciIo = HcStruc->PciIo;

    Status = PciIo->Pci.Write(PciIo, EfiPciIoWidthUint16, Offset, 1, &Data);
    ASSERT_EFI_ERROR(Status);
    return;
#endif
}

/**
    This routine reads a dword value from the MMIO register 
    of the host controller.

    @param
        HcStruc     - HcStruc pointer
        Offset      - Register offset to read

**/

UINT32
HcReadHcMem(
    HC_STRUC    *HcStruc,
    UINT32      Offset
)
{
#if !USB_RT_DXE_DRIVER
    if (Offset > HcStruc->BaseAddressSize) {
        return 0;
    }
    return DwordReadMem(HcStruc->BaseAddress, Offset);
#else
    EFI_STATUS              Status;
    UINT32                  Data = 0;
    EFI_PCI_IO_PROTOCOL	    *PciIo = HcStruc->PciIo;

	Status = PciIo->Mem.Read(PciIo, EfiPciIoWidthUint32, 0, Offset, 1, &Data);
    ASSERT_EFI_ERROR(Status);
    return Data;
#endif
}

/**
    This routine writes a dword value to the MMIO register 
    of the host controller.

    @param
        HcStruc     - HcStruc pointer
        Offset      - Register offset to write
        Data        - Value to write
        
**/

VOID
HcWriteHcMem(
    HC_STRUC    *HcStruc,
    UINT32      Offset,
    UINT32      Data
)
{
#if !USB_RT_DXE_DRIVER
    if ((Offset + sizeof(UINT32)) > HcStruc->BaseAddressSize) {
        return;
    }
    DwordWriteMem(HcStruc->BaseAddress, Offset, Data);
    return;
#else
    EFI_STATUS	            Status;
    EFI_PCI_IO_PROTOCOL	    *PciIo = HcStruc->PciIo;

    Status = PciIo->Mem.Write(PciIo, EfiPciIoWidthUint32, 0, Offset, 1, &Data);
    ASSERT_EFI_ERROR(Status);
    return;
#endif
}

/**
    This routine sets the specified bits to the MMIO register 
    of the host controller.

    @param
        HcStruc     - HcStruc pointer
        Offset      - Register offset to set
        Data        - Value to set
        
**/

VOID
HcSetHcMem(
    HC_STRUC    *HcStruc,
    UINT32      Offset,
    UINT32      Bit
)
{
	UINT32  Data;

    if (Offset > HcStruc->BaseAddressSize) {
        return;
    }

	Data = HcReadHcMem(HcStruc, Offset) | Bit;
	HcWriteHcMem(HcStruc, Offset, Data);
    return;
}

/**
    This routine clears the specified bits to the MMIO register 
    of the host controller.

    @param
        HcStruc     - HcStruc pointer
        Offset      - Register offset to clear
        Data        - Value to clear
        
**/

VOID
HcClearHcMem(
	HC_STRUC	*HcStruc,
	UINT32		Offset,
	UINT32		Bit
)
{
    UINT32  Data;

    if (Offset > HcStruc->BaseAddressSize) {
        return;
    }

    Data = HcReadHcMem(HcStruc, Offset) & ~Bit;
    HcWriteHcMem(HcStruc, Offset, Data);
    return;
}

/**
    This routine reads a dword value from the operational register 
    of the host controller.

    @param
        HcStruc     - HcStruc pointer
        Offset      - Register offset to read

**/

UINT32
HcReadOpReg(
    HC_STRUC    *HcStruc,
    UINT32      Offset
)
{
    return HcReadHcMem(HcStruc, HcStruc->bOpRegOffset + Offset);
}

/**
    This routine write a dword value to the operational register
    of the host controller.

    @param
        HcStruc     - HcStruc pointer
        Offset      - Register offset to write
        Data        - Value to write
        
**/

VOID
HcWriteOpReg(
    HC_STRUC    *HcStruc,
    UINT32      Offset,
    UINT32      Data
)
{
    HcWriteHcMem(HcStruc, HcStruc->bOpRegOffset + Offset, Data);
    return;
}

/**
    This routine sets the specified bits to the operational register 
    of the host controller.

    @param
        HcStruc     - HcStruc pointer
        Offset      - Register offset to set
        Data        - Value to set
        
**/

VOID
HcSetOpReg(
    HC_STRUC    *HcStruc,
    UINT32      Offset,
    UINT32      Bit
)
{
    UINT32  Data;

    Data = HcReadOpReg(HcStruc, Offset) | Bit;
    HcWriteOpReg(HcStruc, Offset, Data);
    return;
}

/**
    This routine clears the specified bits to the operational register 
    of the host controller.

    @param
        HcStruc     - HcStruc pointer
        Offset      - Register offset to clear
        Data        - Value to clear
        
**/

VOID
HcClearOpReg(
    HC_STRUC    *HcStruc,
    UINT32      Offset,
    UINT32      Bit
)
{
    UINT32  Data;

    Data = HcReadOpReg(HcStruc, Offset) & ~Bit;
    HcWriteOpReg(HcStruc, Offset, Data);
	return;
}

/**
    This routine reads a byte value from the Io register 
    of the host controller.

    @param
        HcStruc     - HcStruc pointer
        Offset      - Register offset to read

**/

UINT8
HcByteReadHcIo(
    HC_STRUC    *HcStruc,
    UINT32      Offset
)
{
#if !USB_RT_DXE_DRIVER
    return ByteReadIO((UINT16)HcStruc->BaseAddress + Offset);
#else
    EFI_STATUS              Status;
    UINT8                   Data = 0;
    EFI_PCI_IO_PROTOCOL	    *PciIo = HcStruc->PciIo;

    Status = PciIo->Io.Read(PciIo, EfiPciIoWidthUint8, 4, Offset, 1, &Data);
    ASSERT_EFI_ERROR(Status);
    return Data;
#endif
}

/**
    This routine writes a byte value to the Io register 
    of the host controller.

    @param
        HcStruc     - HcStruc pointer
        Offset      - Register offset to write
        Data        - Value to write
        
**/

VOID
HcByteWriteHcIo(
    HC_STRUC    *HcStruc,
    UINT32      Offset,
    UINT8       Data
)
{
#if !USB_RT_DXE_DRIVER
    ByteWriteIO((UINT16)HcStruc->BaseAddress + Offset, Data);
    return;
#else
    EFI_STATUS	            Status;
    EFI_PCI_IO_PROTOCOL	    *PciIo = HcStruc->PciIo;

    Status = PciIo->Io.Write(PciIo, EfiPciIoWidthUint8, 4, Offset, 1, &Data);
    ASSERT_EFI_ERROR(Status);
    return;
#endif
}

/**
    This routine reads a word value from the Io register 
    of the host controller.

    @param
        HcStruc     - HcStruc pointer
        Offset      - Register offset to read

**/

UINT16
HcWordReadHcIo(
    HC_STRUC    *HcStruc,
    UINT32      Offset
)
{
#if !USB_RT_DXE_DRIVER
    return WordReadIO((UINT16)HcStruc->BaseAddress + Offset);
#else
    EFI_STATUS              Status;
    UINT16                  Data = 0;
    EFI_PCI_IO_PROTOCOL	    *PciIo = HcStruc->PciIo;

    Status = PciIo->Io.Read(PciIo, EfiPciIoWidthUint16, 4, Offset, 1, &Data);
    ASSERT_EFI_ERROR(Status);
    return Data;
#endif
}

/**
    This routine writes a word value to the Io register 
    of the host controller.

    @param
        HcStruc     - HcStruc pointer
        Offset      - Register offset to write
        Data        - Value to write
        
**/

VOID
HcWordWriteHcIo(
    HC_STRUC    *HcStruc,
    UINT32      Offset,
    UINT16      Data
)
{
#if !USB_RT_DXE_DRIVER
    WordWriteIO((UINT16)HcStruc->BaseAddress + Offset, Data);
    return;
#else
    EFI_STATUS	            Status;
    EFI_PCI_IO_PROTOCOL	    *PciIo = HcStruc->PciIo;

    Status = PciIo->Io.Write(PciIo, EfiPciIoWidthUint16, 4, Offset, 1, &Data);
    ASSERT_EFI_ERROR(Status);
    return;
#endif
}

/**
    This routine reads a dword value from the Io register 
    of the host controller.

    @param
        HcStruc     - HcStruc pointer
        Offset      - Register offset to read

**/

UINT32
HcDwordReadHcIo(
    HC_STRUC    *HcStruc,
    UINT32      Offset
)
{
#if !USB_RT_DXE_DRIVER
    return DwordReadIO((UINT16)HcStruc->BaseAddress + Offset);
#else
    EFI_STATUS              Status;
    UINT32                  Data = 0;
    EFI_PCI_IO_PROTOCOL	    *PciIo = HcStruc->PciIo;

    Status = PciIo->Io.Read(PciIo, EfiPciIoWidthUint32, 4, Offset, 1, &Data);
    ASSERT_EFI_ERROR(Status);
    return Data;
#endif
}

/**
    This routine writes a dword value to the Io register 
    of the host controller.

    @param
        HcStruc     - HcStruc pointer
        Offset      - Register offset to write
        Data        - Value to write
        
**/

VOID
HcDwordWriteHcIo(
    HC_STRUC    *HcStruc,
    UINT32      Offset,
    UINT32      Data
)
{
#if !USB_RT_DXE_DRIVER
    DwordWriteIO((UINT16)HcStruc->BaseAddress + Offset, Data);
    return;
#else
    EFI_STATUS              Status;
    EFI_PCI_IO_PROTOCOL	    *PciIo = HcStruc->PciIo;

    Status = PciIo->Io.Write(PciIo, EfiPciIoWidthUint32, 4, Offset, 1, &Data);
    ASSERT_EFI_ERROR(Status);
    return;
#endif
}

/**
    This routine uses Map of the DMA services for DMA operations. 

    @param
        HcStruc     - HcStruc pointer
        Direction   - Data direction 
        BufferAddr  - BufferAddr for DmaMap.
        BufferSize  - BufferSize for DmaMap
        PhyAddr     - Phyaddr to access
        Mapping     - A resulting value to pass to HcDmaUnmap.
**/

UINT8
HcDmaMap(
    HC_STRUC    *HcStruc,
    UINT8       Direction,
    UINT8       *BufferAddr,
    UINT32      BufferSize,
    UINT8       **PhyAddr,
    VOID        **Mapping
)
{
#if !USB_RT_DXE_DRIVER
    *PhyAddr = BufferAddr;
#else
    EFI_PCI_IO_PROTOCOL_OPERATION       Operation;
    EFI_PHYSICAL_ADDRESS                Addr;
    EFI_STATUS                          Status;
    UINTN                               Bytes = BufferSize;
    EFI_PCI_IO_PROTOCOL	                *PciIo = HcStruc->PciIo;

    if (Direction & BIT7) {
        Operation = EfiPciIoOperationBusMasterWrite;
    } else {
        Operation = EfiPciIoOperationBusMasterRead;
    }

	Status = PciIo->Map(PciIo, Operation, BufferAddr, &Bytes, &Addr, Mapping);
    
	if (EFI_ERROR(Status) || Bytes != BufferSize) {
        *PhyAddr = BufferAddr;
		return USB_ERROR;
	}

	*PhyAddr = (UINT8*)Addr;
#endif
	return USB_SUCCESS;
}

/**
    This routine uses UnMap of the DMA services for DMA operations. 

    @param
        HcStruc     - HcStruc pointer
        Mapping     - The mapping value returned from HcDmaMap.
**/

UINT8
HcDmaUnmap(
	HC_STRUC	*HcStruc,
	VOID		*Mapping
)
{
#if USB_RT_DXE_DRIVER

    EFI_STATUS              Status;
    EFI_PCI_IO_PROTOCOL	    *PciIo = HcStruc->PciIo;

    Status = PciIo->Unmap(PciIo, Mapping);
    if (EFI_ERROR(Status)) {
        return USB_ERROR;
    }
#endif
    return USB_SUCCESS;
}

/**
    This routine produces a sound on the internal PC speaker

    @param
        Freq -     Sound frequency
        Duration - Sound duration in 15 microsecond units
        HcStruc - Pointer to HCStruc

    @retval VOID

**/

VOID
SpeakerBeep(
    UINT8       Freq,
    UINT16      Duration, 
    HC_STRUC    *HcStruc
)
{
#if USB_BEEP_ENABLE
    UINT8   Value;
    if (gUsbData->dUSBStateFlag & USB_FLAG_ENABLE_BEEP_MESSAGE) {
        ByteWriteIO((UINT8)0x43, (UINT8)0xB6);
        ByteWriteIO((UINT8)0x42, (UINT8)Freq);
        ByteWriteIO((UINT8)0x42, (UINT8)Freq);
        Value = ByteReadIO((UINT8)0x61);
        ByteWriteIO((UINT8)0x61, (UINT8)(Value | 03));
        FixedDelay((UINTN)Duration * 15);
        ByteWriteIO((UINT8)0x61, (UINT8)(Value));
    }
#endif
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
