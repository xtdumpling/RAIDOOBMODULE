//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file KbcUhci.h
    Header file for UCHI controller for Emulation

**/

typedef enum {
  UNSUPPORTED = -1,
  USB1_1 = 0,
  USB1_2,
  USB1_3,
  USB1_4,
  USB2_1,
  NUMBER_USB_CONTROLLERS
} SUPPORTED_USB_CONTROLLERS;

typedef struct {
    UINT16      BusDevFunc;
    UINT8       InterfaceType;
    UINT8       SubClassCode;
    UINT8       BaseClassCode;
    UINT8       LegacyRegOffset;
} UHCI_EMUL_DEVICE;

typedef struct _FULL_USB_DEVICE_PATH
{
  ACPI_HID_DEVICE_PATH acpi;
  PCI_DEVICE_PATH pci;
  EFI_DEVICE_PATH_PROTOCOL end;
} FULL_USB_DEVICE_PATH;

#define ACPI_PATH_MACRO \
    {{ACPI_DEVICE_PATH,ACPI_DP,ACPI_DEVICE_PATH_LENGTH}, EISA_PNP_ID(0x0A03),0}

#define PCI_PATH_MACRO(Device,Function) \
    {{HARDWARE_DEVICE_PATH, HW_PCI_DP, HW_PCI_DEVICE_PATH_LENGTH}, (Function),(Device)}

#define END_PATH_MACRO \
    {END_DEVICE_PATH,END_ENTIRE_SUBTYPE,END_DEVICE_PATH_LENGTH}

#define USB1_1_DEVICE_PATH   \
    { ACPI_PATH_MACRO, PCI_PATH_MACRO(0x1d, 0), END_PATH_MACRO }

//
// Trap Dispatcher
//

typedef struct _TRAPDISPATCH {
    UINT16 status_bit;
    VOID (* trap_function)();
} TRAPDISPATCH;

#define UHCI_SMIBYENDPS     (UINT16)(1U << 15)
#define UHCI_USBPIRQEN      (UINT16)(1U << 13)
#define UHCI_SMIBYUSB       (UINT16)(1U << 12)
#define UHCI_TRAPBY64W      (UINT16)(1U << 11)
#define UHCI_TRAPBY64R      (UINT16)(1U << 10)
#define UHCI_TRAPBY60W      (UINT16)(1U << 9)
#define UHCI_TRAPBY60R      (UINT16)(1U << 8)
#define UHCI_SMIATENDPS     (UINT16)(1U << 7)
#define UHCI_PSTATE         (UINT16)(1U << 6)
#define UHCI_A20PASSEN      (UINT16)(1U << 5)
#define UHCI_USBSMIEN       (UINT16)(1U << 4)
#define UHCI_64WEN          (UINT16)(1U << 3)
#define UHCI_64REN          (UINT16)(1U << 2)
#define UHCI_60WEN          (UINT16)(1U << 1)
#define UHCI_60REN          (UINT16)(1U )

#define PCIBUS(x)           ( x << 16)
#define PCIDEV(x)           ( x << 11)
#define PCIFUNC(x)          ( x << 8)


#define UHCI_TRAPBY_MASK_WITHOUTKBC        (UHCI_TRAPBY64W | UHCI_TRAPBY64R | UHCI_TRAPBY60W | UHCI_TRAPBY60R | UHCI_SMIBYENDPS)
#define UHCI_TRAPBY_MASK_WITHKBC           (UHCI_TRAPBY64W | UHCI_TRAPBY60W | UHCI_TRAPBY60R | UHCI_SMIBYENDPS)

//enable bits for traps
#if defined UHCI_A20PASSEN_SUPPORT && UHCI_A20PASSEN_SUPPORT == 1
#define UHCI_TRAPEN_MASK_WITHOUTKBC        (UHCI_64WEN | UHCI_60WEN  | UHCI_64REN | UHCI_60REN | UHCI_A20PASSEN)
#define UHCI_TRAPEN_MASK_WITHKBC           (UHCI_64WEN | UHCI_60WEN  | UHCI_60REN | UHCI_A20PASSEN)
#else
#define UHCI_TRAPEN_MASK_WITHOUTKBC        (UHCI_64WEN | UHCI_60WEN  | UHCI_64REN | UHCI_60REN)
#define UHCI_TRAPEN_MASK_WITHKBC           (UHCI_64WEN | UHCI_60WEN  | UHCI_60REN )
#endif

BOOLEAN     GetRMhStatus();
BOOLEAN     Uhci_HasTrapStatus();
BOOLEAN     Uhci_TrapEnable(BOOLEAN);
BOOLEAN     NonSmmUhci_TrapEnable(BOOLEAN);
BOOLEAN     NonSmmUhci_HasTrapStatus();
void        NonSmmUhci_SetTrapEnMask();

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

