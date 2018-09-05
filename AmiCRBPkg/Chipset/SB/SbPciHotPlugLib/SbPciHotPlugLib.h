//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2013, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
//*************************************************************************

#ifndef _PCI_HOT_PLUG_H_
#define _PCI_HOT_PLUG_H_

//
// External include files do NOT need to be explicitly specified in real EDKII 
// environment
//

#define PCI_HOT_PLUG_DRIVER_PRIVATE_SIGNATURE EFI_SIGNATURE_32 ('G', 'U', 'L', 'P')

#define ACPI \
  { \
    ACPI_DEVICE_PATH, ACPI_DP, (UINT8) (sizeof (ACPI_HID_DEVICE_PATH)), (UINT8) \
      ((sizeof (ACPI_HID_DEVICE_PATH)) >> 8), EISA_PNP_ID (0x0A03), 0 \
  }

#define PCI(device, function) \
  { \
    HARDWARE_DEVICE_PATH, HW_PCI_DP, (UINT8) (sizeof (PCI_DEVICE_PATH)), (UINT8) ((sizeof (PCI_DEVICE_PATH)) >> 8), \
      (UINTN) function, (UINTN) device \
  }

#define END \
  { \
    END_DEVICE_PATH_TYPE, END_ENTIRE_DEVICE_PATH_SUBTYPE, END_DEVICE_PATH_LENGTH, 0 \
  }

#define LPC(eisaid, function) \
  { \
    ACPI_DEVICE_PATH, ACPI_DP, (UINT8) (sizeof (ACPI_HID_DEVICE_PATH)), (UINT8) \
      ((sizeof (ACPI_HID_DEVICE_PATH)) >> 8), EISA_PNP_ID (eisaid), function \
  }

typedef struct PCIE_HOT_PLUG_DEVICE_PATH {
  ACPI_HID_DEVICE_PATH      PciRootBridgeNode;
  PCI_DEVICE_PATH           PciRootPortNode;
  EFI_DEVICE_PATH_PROTOCOL  EndDeviceNode;
} PCIE_HOT_PLUG_DEVICE_PATH;

typedef struct {
  UINTN                           Signature;
  EFI_HANDLE                      Handle; // Handle for protocol this driver installs on
  EFI_PCI_HOT_PLUG_INIT_PROTOCOL  HotPlugInitProtocol;
} PCI_HOT_PLUG_INSTANCE;

EFI_STATUS
GetRootHpcList (
  IN  EFI_PCI_HOT_PLUG_INIT_PROTOCOL    *This,
  OUT UINTN                             *PhpcCount,
  OUT EFI_HPC_LOCATION                  **PhpcList
  )
/*++

Routine Description:

   This procedure returns a list of Root Hot Plug controllers that require
   initialization during boot process

Arguments:

  This      - The pointer to the instance of the EFI_PCI_HOT_PLUG_INIT protocol.
  PhpcCount - The number of Root HPCs returned.
  PhpcList  - The list of Root HPCs. HpcCount defines the number of elements in this list.
Returns:

  EFI_SUCCESS.

--*/
;

EFI_STATUS
InitializeRootHpc (
  IN  EFI_PCI_HOT_PLUG_INIT_PROTOCOL  *This,
  IN  EFI_DEVICE_PATH_PROTOCOL        *PhpcDevicePath,
  IN  UINT64                          PhpcPciAddress,
  IN  EFI_EVENT                       Event, OPTIONAL
  OUT EFI_HPC_STATE                   *PhpcState
  )
/*++

Routine Description:

  This procedure Initializes one Root Hot Plug Controller
  This process may casue initialization of its subordinate buses

Arguments:

  This            - The pointer to the instance of the EFI_PCI_HOT_PLUG_INIT protocol.
  PhpcDevicePath  - The Device Path to the HPC that is being initialized.
  PhpcPciAddress  - The address of the Hot Plug Controller function on the PCI bus.
  Event           - The event that should be signaled when the Hot Plug Controller initialization is complete. Set to NULL if the caller wants to wait until the entire initialization process is complete. The event must be of the type EFI_EVT_SIGNAL.
  PhpcState       - The state of the Hot Plug Controller hardware. The type EFI_Hpc_STATE is defined in section 3.1.

Returns:

  EFI_SUCCESS.
--*/
;

EFI_STATUS
GetResourcePadding (
  IN  EFI_PCI_HOT_PLUG_INIT_PROTOCOL  *This,
  IN  EFI_DEVICE_PATH_PROTOCOL        *PhpcDevicePath,
  IN  UINT64                          PhpcPciAddress,
  OUT EFI_HPC_STATE                   *PhpcState,
  OUT VOID                            **Padding,
  OUT EFI_HPC_PADDING_ATTRIBUTES      *Attributes
  )
/*++

Routine Description:

  Returns the resource padding required by the PCI bus that is controlled by the specified Hot Plug Controller. 

Arguments:

  This            - The pointer to the instance of the EFI_PCI_HOT_PLUG_INIT protocol. initialized.
  PhpcDevicePath  - The Device Path to the Hot Plug Controller.
  PhpcPciAddress  - The address of the Hot Plug Controller function on the PCI bus.
  PhpcState       - The state of the Hot Plug Controller hardware. The type EFI_HPC_STATE is defined in section 3.1.
  Padding         - This is the amount of resource padding required by the PCI bus under the control of the specified Hpc. Since the caller does not know the size of this buffer, this buffer is allocated by the callee and freed by the caller.
  Attributes      - Describes how padding is accounted for.

Returns:

  EFI_SUCCESS.
--*/
;

#endif
//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2013, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
