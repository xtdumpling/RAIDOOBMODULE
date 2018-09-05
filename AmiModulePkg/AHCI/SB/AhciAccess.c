//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file AhciAccess.c
    Provides Index Data Port Access to AHCI Controller

**/
//**********************************************************************
//#include <AmiDxeLib.h>

#define LBAR_REGISTER           0x20
#define LBAR_ADDRESS_MASK       0xFFFFFFE0
#define INDEX_OFFSET_FROM_LBAR  0x10
#define DATA_OFFSET_FROM_LBAR   0x14

#include "AmiDxeLib.h"
#include "Protocol/PciIo.h"
#include "Protocol/BlockIo.h"
#include "Protocol/AmiAhciBus.h"
#include "Token.h"

#define AHCI_LBAR_INDEX      4
#define AHCI_ABAR_INDEX      5

#define     MmAddress( BaseAddr, Register ) \
            ((UINT64)(BaseAddr) + \
            (UINTN)(Register) \
             )
#define     Mm32Ptr( BaseAddr, Register ) \
            ((volatile UINT32 *)MmAddress (BaseAddr, Register ))

#define     Mm16Ptr( BaseAddr, Register ) \
            ((volatile UINT16 *)MmAddress (BaseAddr, Register ))

#define     Mm8Ptr( BaseAddr, Register ) \
            ((volatile UINT8 *)MmAddress (BaseAddr, Register ))

UINT16 IndexPort, DataPort;

/**
    This is chip set porting routine that returns index/data ports
    to access memory-mapped registers.

    @param PciIo

    @retval EFI_SUCCESS Access information is collected
    @retval EFI_ACCESS_DENIED No Access information available

**/

EFI_STATUS
InitilizeIndexDataPortAddress (
    IN EFI_PCI_IO_PROTOCOL *PciIo
)
{
    EFI_STATUS Status;
    UINT32 lbar;

    Status = PciIo->Pci.Read(PciIo, EfiPciIoWidthUint32, LBAR_REGISTER, 1, &lbar);
    ASSERT_EFI_ERROR(Status);

    lbar &= LBAR_ADDRESS_MASK;  // Legacy Bus Master Base Address

    IndexPort = (UINT16)lbar + INDEX_OFFSET_FROM_LBAR;
    DataPort = (UINT16)lbar + DATA_OFFSET_FROM_LBAR;

    return EFI_SUCCESS;
}

/**
    Read the Dword Data

    @param    BaseAddress - BaseAddress of AHCI Controller
    @param    Index       - Index/Offset address to read           

    @retval Value Read

**/
UINT32
SmmReadDataDword(
    IN  UINTN   BaseAddress,
    IN  UINTN   Index
)
{
#if INDEX_DATA_PORT_ACCESS
    IoWrite32(IndexPort, (UINT32)Index);
    return IoRead32(DataPort);
#else
    return (*Mm32Ptr ((BaseAddress), (Index)));
#endif
}
/**
    WriteRead the Dword Data

    @param    BaseAddress - BaseAddress of AHCI Controller
    @param    Index       - Index/Offset address to Write
    @param    Data        - Data to be written        

    @retval    Nothing

**/
VOID
SmmWriteDataDword(
    IN  UINTN   BaseAddress,
    IN  UINTN   Index, 
    IN  UINTN   Data
)
{
#if INDEX_DATA_PORT_ACCESS
    IoWrite32(IndexPort, (UINT32)Index);
    IoWrite32(DataPort, (UINT32)Data);
#else
    (*Mm32Ptr ((BaseAddress), (Index))) = ((UINT32) (Data));
#endif
}

/**
    Read the Word Data

    @param    BaseAddress - BaseAddress of AHCI Controller
    @param    Index       - Index/Offset address to read           

    @retval Value Read

**/
UINT16
SmmReadDataWord(
    IN  UINTN   BaseAddress,
    IN  UINTN   Index
)
{
#if INDEX_DATA_PORT_ACCESS
    IoWrite32(IndexPort, (UINT32)Index);
    return (UINT16)IoRead32(DataPort);
#else
    return (*Mm16Ptr ((BaseAddress), (Index)));
#endif
}
/**
    WriteRead the word Data

    @param    BaseAddress - BaseAddress of AHCI Controller
    @param    Index       - Index/Offset address to Write
    @param    Data        - Data to be written        

    @retval    Nothing

**/
VOID
SmmWriteDataWord(
    IN  UINTN   BaseAddress,
    IN  UINTN   Index, 
    IN  UINTN   Data
)
{
#if INDEX_DATA_PORT_ACCESS
    IoWrite32(IndexPort, (UINT32)Index);
    IoWrite32(DataPort, (UINT16)Data);
#else
    (*Mm16Ptr ((BaseAddress), (Index))) = ((UINT16) (Data));
#endif
}
/**
    Read the Byte Data

    @param    BaseAddress - BaseAddress of AHCI Controller
    @param    Index       - Index/Offset address to read           

    @retval    Value Read

**/
UINT8
SmmReadDataByte(
    IN  UINTN   BaseAddress,
    IN  UINTN   Index
)
{
#if INDEX_DATA_PORT_ACCESS
    IoWrite32(IndexPort, (UINT32)Index);
    return (UINT8)IoRead32(DataPort);
#else
    return (*Mm8Ptr ((BaseAddress), (Index)));
#endif
}
/**
    WriteRead the Dword Data

    @param    BaseAddress - BaseAddress of AHCI Controller
    @param    Index       - Index/Offset address to Write
    @param    Data        - Data to be written        

    @retval Nothing

**/
VOID
SmmWriteDataByte(
    IN  UINTN   BaseAddress,
    IN  UINTN   Index,
    IN  UINTN   Data
)
{
#if INDEX_DATA_PORT_ACCESS
    IoWrite32(IndexPort, (UINT32)Index);
    IoWrite8(DataPort, (UINT8)Data);
#else
    (*Mm8Ptr ((BaseAddress), (Index))) = ((UINT8) (Data));
#endif
}

/**
    Read the Dword Data

    @param    AhciBusInterface - Pointer to AMI_AHCI_BUS_PROTOCOL of AHCI Controller
    @param    Index       - Index/Offset address to read           

    @retval Value Read

**/
UINT32
ReadDataDword(
    IN AMI_AHCI_BUS_PROTOCOL   *AhciBusInterface,
    IN  UINTN   Index
)
{
    UINT32      Data = 0;
#if AHCI_USE_PCIIO_FOR_MMIO_AND_IO
    if(AhciBusInterface !=NULL && AhciBusInterface->PciIO != NULL) {
#if INDEX_DATA_PORT_ACCESS
        AhciBusInterface->PciIO->Io.Write (
                     AhciBusInterface->PciIO,
                     EfiPciIoWidthUint32,
                     AHCI_LBAR_INDEX,
                     INDEX_OFFSET_FROM_LBAR,
                     1,
                     &Index);
        AhciBusInterface->PciIO->Io.Read (
                     AhciBusInterface->PciIO,
                     EfiPciIoWidthUint32,
                     AHCI_LBAR_INDEX,
                     DATA_OFFSET_FROM_LBAR,
                     1,
                     &Data);
#else
        AhciBusInterface->PciIO->Mem.Read (
                     AhciBusInterface->PciIO,
                     EfiPciIoWidthUint32,
                     AHCI_ABAR_INDEX,
                     Index,
                     1,
                     &Data);
#endif
    }
#else //AHCI_USE_PCIIO_FOR_MMIO_AND_IO
#if INDEX_DATA_PORT_ACCESS
    IoWrite32(IndexPort, (UINT32)Index);
    Data = IoRead32(DataPort);
#else
    if(AhciBusInterface !=NULL) {
        Data = (*Mm32Ptr ((AhciBusInterface->AhciBaseAddress), (Index)));
    }
#endif
#endif //AHCI_USE_PCIIO_FOR_MMIO_AND_IO
    return Data;
}
/**
    WriteRead the Dword Data

    @param    AhciBusInterface - Pointer to AMI_AHCI_BUS_PROTOCOL of AHCI Controller
    @param    Index       - Index/Offset address to Write
    @param    Data        - Data to be written        

    @retval    Nothing

**/
VOID
WriteDataDword(
    IN AMI_AHCI_BUS_PROTOCOL   *AhciBusInterface,
    IN  UINTN   Index, 
    IN  UINTN   Data
)
{
#if AHCI_USE_PCIIO_FOR_MMIO_AND_IO
    if(AhciBusInterface !=NULL && AhciBusInterface->PciIO != NULL) {
#if INDEX_DATA_PORT_ACCESS
        AhciBusInterface->PciIO->Io.Write (
                     AhciBusInterface->PciIO,
                     EfiPciIoWidthUint32,
                     AHCI_LBAR_INDEX,
                     INDEX_OFFSET_FROM_LBAR,
                     1,
                     &Index);
        AhciBusInterface->PciIO->Io.Write (
                     AhciBusInterface->PciIO,
                     EfiPciIoWidthUint32,
                     AHCI_LBAR_INDEX,
                     DATA_OFFSET_FROM_LBAR,
                     1,
                     &Data);
#else
        AhciBusInterface->PciIO->Mem.Write (
                     AhciBusInterface->PciIO,
                     EfiPciIoWidthUint32,
                     AHCI_ABAR_INDEX,
                     Index,
                     1,
                     &Data);
#endif
    }
#else //AHCI_USE_PCIIO_FOR_MMIO_AND_IO
#if INDEX_DATA_PORT_ACCESS
    IoWrite32(IndexPort, (UINT32)Index);
    IoWrite32(DataPort, (UINT32)Data);
#else
    if(AhciBusInterface !=NULL) {
        (*Mm32Ptr ((AhciBusInterface->AhciBaseAddress), (Index))) = ((UINT32) (Data));
    }
#endif
#endif //AHCI_USE_PCIIO_FOR_MMIO_AND_IO
}

/**
    Read the Word Data

    @param    AhciBusInterface - Pointer to AMI_AHCI_BUS_PROTOCOL of AHCI Controller
    @param    Index       - Index/Offset address to read           

    @retval Value Read

**/
UINT16
ReadDataWord(
    IN AMI_AHCI_BUS_PROTOCOL   *AhciBusInterface,
    IN  UINTN   Index
)
{
    UINT16      Data = 0;
#if AHCI_USE_PCIIO_FOR_MMIO_AND_IO
    if(AhciBusInterface !=NULL && AhciBusInterface->PciIO != NULL) {
#if INDEX_DATA_PORT_ACCESS
        UINT32      Temp = 0;
        AhciBusInterface->PciIO->Io.Write (
                     AhciBusInterface->PciIO,
                     EfiPciIoWidthUint32,
                     AHCI_LBAR_INDEX,
                     INDEX_OFFSET_FROM_LBAR,
                     1,
                     &Index);
        AhciBusInterface->PciIO->Io.Read (
                     AhciBusInterface->PciIO,
                     EfiPciIoWidthUint32,
                     AHCI_LBAR_INDEX,
                     DATA_OFFSET_FROM_LBAR,
                     1,
                     &Temp);
        Data = (UINT16)Temp;
#else
        AhciBusInterface->PciIO->Mem.Read (
                     AhciBusInterface->PciIO,
                     EfiPciIoWidthUint16,
                     AHCI_ABAR_INDEX,
                     Index,
                     1,
                     &Data);
#endif
    }
#else //AHCI_USE_PCIIO_FOR_MMIO_AND_IO
#if INDEX_DATA_PORT_ACCESS
    IoWrite32(IndexPort, (UINT32)Index);
    Data = (UINT16)IoRead32(DataPort);
#else
    if(AhciBusInterface !=NULL) {
        Data = (*Mm16Ptr ((AhciBusInterface->AhciBaseAddress), (Index)));
    }
#endif
#endif //AHCI_USE_PCIIO_FOR_MMIO_AND_IO
    return Data;
}
/**
    WriteRead the word Data

    @param    AhciBusInterface - Pointer to AMI_AHCI_BUS_PROTOCOL of AHCI Controller
    @param    Index       - Index/Offset address to Write
    @param    Data        - Data to be written        

    @retval    Nothing

**/
VOID
WriteDataWord(
    IN AMI_AHCI_BUS_PROTOCOL   *AhciBusInterface,
    IN  UINTN   Index, 
    IN  UINTN   Data
)
{
#if AHCI_USE_PCIIO_FOR_MMIO_AND_IO
    if(AhciBusInterface !=NULL && AhciBusInterface->PciIO != NULL) {
#if INDEX_DATA_PORT_ACCESS
        AhciBusInterface->PciIO->Io.Write (
                     AhciBusInterface->PciIO,
                     EfiPciIoWidthUint32,
                     AHCI_LBAR_INDEX,
                     INDEX_OFFSET_FROM_LBAR,
                     1,
                     &Index);
        AhciBusInterface->PciIO->Io.Write (
                     AhciBusInterface->PciIO,
                     EfiPciIoWidthUint32,
                     AHCI_LBAR_INDEX,
                     DATA_OFFSET_FROM_LBAR,
                     1,
                     &Data);
#else
        AhciBusInterface->PciIO->Mem.Write (
                     AhciBusInterface->PciIO,
                     EfiPciIoWidthUint16,
                     AHCI_ABAR_INDEX,
                     Index,
                     1,
                     &Data);
#endif
    }
#else //AHCI_USE_PCIIO_FOR_MMIO_AND_IO
#if INDEX_DATA_PORT_ACCESS
    IoWrite32(IndexPort, (UINT32)Index);
    IoWrite32(DataPort, (UINT16)Data);
#else
    if(AhciBusInterface !=NULL) {
        (*Mm16Ptr ((AhciBusInterface->AhciBaseAddress), (Index))) = ((UINT16) (Data));
    }
#endif
#endif //AHCI_USE_PCIIO_FOR_MMIO_AND_IO
}
/**
    Read the Byte Data

    @param    AhciBusInterface - Pointer to AMI_AHCI_BUS_PROTOCOL of AHCI Controller
    @param    Index       - Index/Offset address to read           

    @retval    Value Read

**/
UINT8
ReadDataByte(
    IN AMI_AHCI_BUS_PROTOCOL   *AhciBusInterface,
    IN  UINTN   Index
)
{
    UINT8       Data = 0;
#if AHCI_USE_PCIIO_FOR_MMIO_AND_IO
    if(AhciBusInterface !=NULL && AhciBusInterface->PciIO != NULL) {
#if INDEX_DATA_PORT_ACCESS
        UINT32      Temp = 0;
        AhciBusInterface->PciIO->Io.Write (
                     AhciBusInterface->PciIO,
                     EfiPciIoWidthUint32,
                     AHCI_LBAR_INDEX,
                     INDEX_OFFSET_FROM_LBAR,
                     1,
                     &Index);
        AhciBusInterface->PciIO->Io.Read (
                     AhciBusInterface->PciIO,
                     EfiPciIoWidthUint32,
                     AHCI_LBAR_INDEX,
                     DATA_OFFSET_FROM_LBAR,
                     1,
                     &Temp);
        Data = (UINT8)Temp;
#else
        AhciBusInterface->PciIO->Mem.Read (
                     AhciBusInterface->PciIO,
                     EfiPciIoWidthUint8,
                     AHCI_ABAR_INDEX,
                     Index,
                     1,
                     &Data);
#endif
    }
#else //AHCI_USE_PCIIO_FOR_MMIO_AND_IO
#if INDEX_DATA_PORT_ACCESS
    IoWrite32(IndexPort, (UINT32)Index);
    Data = (UINT8)IoRead32(DataPort);
#else
    if(AhciBusInterface !=NULL) {
        Data = (*Mm8Ptr ((AhciBusInterface->AhciBaseAddress), (Index)));
    }
#endif
#endif //AHCI_USE_PCIIO_FOR_MMIO_AND_IO
    return Data;
}
/**
    WriteRead the Byte Data

    @param    AhciBusInterface - Pointer to AMI_AHCI_BUS_PROTOCOL of AHCI Controller
    @param    Index       - Index/Offset address to Write
    @param    Data        - Data to be written        

    @retval Nothing

**/
VOID
WriteDataByte(
    IN AMI_AHCI_BUS_PROTOCOL   *AhciBusInterface,
    IN  UINTN   Index,
    IN  UINTN   Data
)
{
#if AHCI_USE_PCIIO_FOR_MMIO_AND_IO
    if(AhciBusInterface !=NULL && AhciBusInterface->PciIO != NULL) {
#if INDEX_DATA_PORT_ACCESS
        AhciBusInterface->PciIO->Io.Write (
                     AhciBusInterface->PciIO,
                     EfiPciIoWidthUint32,
                     AHCI_LBAR_INDEX,
                     INDEX_OFFSET_FROM_LBAR,
                     1,
                     &Index);
        AhciBusInterface->PciIO->Io.Write (
                     AhciBusInterface->PciIO,
                     EfiPciIoWidthUint8,
                     AHCI_LBAR_INDEX,
                     DATA_OFFSET_FROM_LBAR,
                     1,
                     &Data);
#else
        AhciBusInterface->PciIO->Mem.Write (
                     AhciBusInterface->PciIO,
                     EfiPciIoWidthUint8,
                     AHCI_ABAR_INDEX,
                     Index,
                     1,
                     &Data);
#endif
    }
#else //AHCI_USE_PCIIO_FOR_MMIO_AND_IO
#if INDEX_DATA_PORT_ACCESS
    IoWrite32(IndexPort, (UINT32)Index);
    IoWrite8(DataPort, (UINT8)Data);
#else
    if(AhciBusInterface !=NULL) {
        (*Mm8Ptr ((AhciBusInterface->AhciBaseAddress), (Index))) = ((UINT8) (Data));
    }
#endif
#endif //AHCI_USE_PCIIO_FOR_MMIO_AND_IO
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
