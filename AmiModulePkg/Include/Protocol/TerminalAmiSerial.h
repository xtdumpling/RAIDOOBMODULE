//****************************************************************************
//****************************************************************************
//**                                                                        **
//**             (C)Copyright 1985-2016, American Megatrends, Inc.          **
//**                                                                        **
//**                          All Rights Reserved.                          **
//**                                                                        **
//**             5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093       **
//**                                                                        **
//**                          Phone (770)-246-8600                          **
//**                                                                        **
//****************************************************************************
//****************************************************************************

/** @file TerminalAmiSerial.h
    Description: AMI Serial Protocol Definiton.

**/

#ifndef _AMI_SERIAL_H_
#define _AMI_SERIAL_H_
#ifdef __cplusplus
extern "C" {
#endif
#include <Efi.h>
#include <Protocol/DevicePath.h>

#define AMI_SERIAL_VENDOR_DEVICE_PATH_GUID \
    {0x32d1497b,0x288c,0x440a,0x9f,0xc3,0x65,0x1a,0x14,0xef,0xc7,0xb4}

#define AMI_SERIAL_PROTOCOL_GUID \
    {0x50dc5c90,0x1d33,0x4fd6,0x87,0xe5,0x06,0x3b,0x1d,0xfa,0x21,0x70}

GUID_VARIABLE_DECLARATION(gAmiSerialProtocolGuid,AMI_SERIAL_PROTOCOL_GUID);

typedef struct _AMI_SERIAL_PROTOCOL AMI_SERIAL_PROTOCOL;

typedef VOID (EFIAPI *EFI_SERIAL_GET_BASE_ADDRESS) (
    IN  AMI_SERIAL_PROTOCOL *This,
    OUT UINT64              *BaseAddress
);

typedef VOID (EFIAPI *EFI_CHECK_PCI_MMIO) (
    IN  AMI_SERIAL_PROTOCOL *This,
    OUT BOOLEAN             *Pci,
    OUT BOOLEAN             *Mmio,
    OUT UINT8               *MmioWidth
);

typedef VOID (EFIAPI *EFI_GET_PCI_LOCATION) (
    IN  AMI_SERIAL_PROTOCOL *This,
    OUT UINTN	            *Bus,
    OUT UINTN	            *Dev,
    OUT UINTN           	*Func,
    OUT UINT8               *Port
);

typedef VOID (EFIAPI *EFI_GET_SERIAL_IRQ) (
    IN  AMI_SERIAL_PROTOCOL *This,
    OUT UINT8             *SerialIRQ
);

typedef VOID (EFIAPI *EFI_GET_ACPI_UID) (
    IN  AMI_SERIAL_PROTOCOL *This,
    OUT UINT8               *Uid
);

typedef VOID (EFIAPI *EFI_GET_UART_CLOCK) (
    IN  AMI_SERIAL_PROTOCOL *This,
    OUT UINTN              *Clock
);

struct _AMI_SERIAL_PROTOCOL {
    EFI_SERIAL_GET_BASE_ADDRESS GetBaseAddress;
    EFI_CHECK_PCI_MMIO          CheckPciMmio;
    EFI_GET_PCI_LOCATION        GetPciLocation;
    EFI_GET_SERIAL_IRQ          GetSerialIRQ;
    EFI_GET_ACPI_UID            GetUID;
    EFI_GET_UART_CLOCK          GetUartClock;
};


typedef struct {
    VENDOR_DEVICE_PATH  VendorDevicePath;
    UINT8               Bus;
    UINT8               Device;
    UINT8               Function;
    UINT8               Port;    
}AMI_SERIAL_VENDOR_DEVICE_PATH;

#ifdef __cplusplus
}
#endif
#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
