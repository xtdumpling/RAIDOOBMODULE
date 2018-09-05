/**@file

@copyright
 Copyright (c) 2011 - 2016 Intel Corporation. All rights reserved
 This software and associated documentation (if any) is furnished
 under a license and may only be used or copied in accordance
 with the terms of the license. Except as permitted by the
 license, no part of this software or documentation may be
 reproduced, stored in a retrieval system, or transmitted in any
 form or by any means without the express written consent of
 Intel Corporation.
 This file contains a 'Sample Driver' and is licensed as such
 under the terms of your license agreement with Intel or your
 vendor. This file may be modified by the user, subject to
 the additional terms of the license agreement.

@par Specification Reference:
**/

#include <AsfTable.h>

//-----------------------------------------------------------------------------
// ASF Table
//-----------------------------------------------------------------------------
// @todo, The table shound be runtime build like as ASFBSP
//
// ASF Definitions
//

ACPI_2_0_ASF_DESCRIPTION_TABLE_WITH_DATA ASF_TABLE = {
  EFI_ACPI_ASF_DESCRIPTION_TABLE_SIGNATURE,
  sizeof (ACPI_2_0_ASF_DESCRIPTION_TABLE_WITH_DATA),
  EFI_ACPI_2_0_ASF_DESCRIPTION_TABLE_REVISION,
  0,                          // to make sum of entire table == 0

  // OEM identification
  'I', 'N', 'T', 'E', 'L', ' ',

  // OEM table identification
  ((((((((((((('D' << 8) + '8') << 8) + '6') << 8) + '5') << 8) + 'G') << 8) + 'C') << 8) + 'H') << 8) + ' ', // OEM table identification

  1,                          // OEM revision number
  ((((('M' << 8) + 'S') << 8) + 'F') << 8) + 'T',  // ASL compiler vendor ID
  1000000,                    // ASL compiler revision number

  //
  // ASF_INFO
  //
  0x00,                       // Type "ASF_INFO"
  0x00,                       // Reserved
  sizeof (EFI_ACPI_ASF_INFO), // Length
  0xFF,                       // Min Watchdog Reset Value
  0xFF,                       // Min ASF Sensor Inter-poll Wait Time
  0x0001,                     // System ID
  0x57010000,                 // IANA Manufacture ID for Intel
  0x00,                       // Feature Flag
  0x00, 0x00, 0x00,           // Reserved

  //
  // ASF_ALRT
  //
  0x01,                              // Type "ASF_ALRT"
  0x00,                              // Reserved
  sizeof (EFI_ACPI_ASF_ALRT) +
  3*sizeof(EFI_ACPI_ASF_ALERTDATA),  // Length
  0x00,                              // Assertion Event Bit Mask
  0x00,                              // Deassertion Event Bit Mask
  0x03,                              // Number Of Alerts
  0x0C,                              // Array Element Length
  //
  // ICH Slave SMBUS Read devices
  //
  0x89, 0x04, 0x01, 0x01, 0x05, 0x6F, 0x00, 0x68, 0x08, 0x88, 0x17, 0x00,  // Device 0
  0x89, 0x04, 0x04, 0x04, 0x07, 0x6F, 0x00, 0x68, 0x20, 0x88, 0x03, 0x00,  // Device 1
  0x89, 0x05, 0x01, 0x01, 0x19, 0x6F, 0x00, 0x68, 0x20, 0x88, 0x22, 0x00,  // Device 2

  //
  // ASF_RCTL
  //
  0x02,                               // Type "ASF_RCTL"
  0x00,                               // Reserved
  sizeof (EFI_ACPI_ASF_ALRT) +
  4*sizeof(EFI_ACPI_ASF_CONTROLDATA), // Length
  0x04,                               // Number of Controls
  0x04,                               // Array Element Length
  0x0000,                             // Reserved
  //
  // ICH Slave SMBUS Write cmds
  //
  0x00, 0x88, 0x00, 0x03,             // Control 0 --> Reset system
  0x01, 0x88, 0x00, 0x02,             // Control 1 --> Power Off system
  0x02, 0x88, 0x00, 0x01,             // Control 2 --> Power On system
  0x03, 0x88, 0x00, 0x04,             // Control 3 --> Power Cycle Reset (off then on)

  //
  // ASF_RMCP
  //
  0x03,                       // Type "ASF_RMCP"
  0x00,                       // Reserved
  sizeof (EFI_ACPI_ASF_RMCP), // Length

  // Remote Control Capabilities supported Bit Masks
  0x20,                       // System Firmware Capabilities Bit Mask (two MSBytes are Rsvd,
  0x18,                       // Two LSBytes support Lock KYBD, Power Button Lock, FW Verb Blank Screen,
  0x00,                       // Cfg Data Reset, FW Verb Quiet, FW Verb Verbose, FW Verb Forced Progress
  0x00,
  0x00,                       // Special Cmds Bit Mask (Rsvd MSByte, LSByte Does not supports
  0x13,                       // CD/DVD Boot, HD Boot, PXE Boot.
  0xF0,                       // System Capabilities Bit Mask (Supports Reset,Power-Up,
                              // Power-Down, Power-Cycle Reset for compat and secure port.
  0x00,                       // Boot Option Complete Code
  0x57010000,                 // IANA ID for Intel Manufacturer
  0x00,                       // Special Command
  0x00, 0x00,                 // Special Command Parameter
  0x00, 0x01,                 // Boot Options
  0x00, 0x00,                 // OEM Parameters

  //
  // ASF_ADDR
  //
  0x84,                          // Type "ASF_ADDR", last record
  0x00,                          // Reserved
  sizeof (EFI_ACPI_ASF_ADDR) +
  ASF_ADDR_DEVICE_ARRAY_LENGTH,  // Length
  0x00,                          // SEEPROM Address
  ASF_ADDR_DEVICE_ARRAY_LENGTH,  // Number Of Devices
  0x5C, 0x68, 0x88, 0xC2, 0xD2,  // Fixed SMBus Address (Heceta 6, 82573E device - static def (also an ARP addr),
  0xDC, 0xA0, 0xA2, 0xA4, 0xA6,  // ICH Slave Device, SMB ARP, CK410, DB400/800, SDRAM SPD devices 
  0xC8                           // TEKOA Slave Device
};

//
// Function implementations
//
/**
  Entry point for the ACPI ASF Table Driver.
  Building Asf Table when Ready To Boot event is signaled.

  @param[in] ImageHandle   - Pointer to the loaded image protocol for this driver
  @param[in] SystemTable   - Pointer to the EFI System Table

  @retval EFI_SUCCESS      - Building ASF Table successfully.
**/
EFI_STATUS
EFIAPI
AsfTableEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_EVENT   AsfEvent;
  EFI_STATUS  Status;

#if defined(SPS_SUPPORT) && SPS_SUPPORT
  if (!MeTypeIsAmt()) {
    return EFI_UNSUPPORTED;
  }
#endif //SPS_SUPPORT

  Status = EfiCreateEventReadyToBootEx (
             TPL_NOTIFY,
             BuildAsfTable,
             (VOID *)&ImageHandle,
             &AsfEvent
             );
  ASSERT_EFI_ERROR (Status);

  return Status;
}

/**
  Fix ASF Remote Control Capabilities (RMCP).

  @retval EFI_SUCCESS      - The operation completed successfully.
  @retval EFI_UNSUPPORTED  - The operation is unsupported.
**/
EFI_STATUS
AsfFixRMCP (
  VOID
  )
{
  ALERT_STANDARD_FORMAT_PROTOCOL      *Asf;
  ASF_BOOT_OPTIONS                    *BootOption;
  EFI_STATUS                          Status;

  //
  // Get Protocol for ASF
  //
  Status = gBS->LocateProtocol (
                  &gAlertStandardFormatProtocolGuid,
                  NULL,
                  &Asf
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Info : Error gettings ASF protocol -> %r\n", Status));
    return EFI_UNSUPPORTED;
  }

  gBS->SetMem (&BootOption, sizeof(BootOption), 0);
  Status = Asf->GetBootOptions (Asf, &BootOption);
  if (!EFI_ERROR (Status)) {
    if (BootOption->SpecialCommand != NOP) {
      ASF_TABLE.AsfRmcp.RMCPCompletionCode = ASF_RMCP_BOOT_OPTION_SUCCESS;
      gBS->CopyMem (&ASF_TABLE.AsfRmcp.RMCPIANA,
                    &BootOption->IanaId,
                    sizeof(BootOption->IanaId) +
                    sizeof(BootOption->SpecialCommand) +
                    sizeof(BootOption->SpecialCommandParam) +
                    sizeof(BootOption->BootOptions) +
                    sizeof(BootOption->OemParameters)
                    );
    }
  }

  return Status;
}

/**
  Install Alert Standard Format (ASF) Table.

  @param[in] Event         - A pointer to the Event that triggered the callback.
  @param[in] Context       - A pointer to private data registered with the callback function.

  @retval EFI_SUCCESS      - The operation completed successfully.
  @retval EFI_UNSUPPORTED  - The operation is unsupported.
**/
EFI_STATUS
BuildAsfTable (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  )
{
  EFI_STATUS                Status;
  UINTN                     AsfTableHandle;
  EFI_ACPI_TABLE_PROTOCOL   *AcpiTable;
  VOID                      *AsfStruc;

  AsfTableHandle = 0;

  gBS->CloseEvent(Event);

  AsfStruc = (VOID *)&ASF_TABLE;
  AsfFixRMCP();

  Status = gBS->LocateProtocol (&gEfiAcpiTableProtocolGuid, NULL, &AcpiTable);
  Status = AcpiTable->InstallAcpiTable (
                        AcpiTable,
                        AsfStruc,
                        sizeof (ACPI_2_0_ASF_DESCRIPTION_TABLE_WITH_DATA),
                        &AsfTableHandle
                        );
  return Status;
}
