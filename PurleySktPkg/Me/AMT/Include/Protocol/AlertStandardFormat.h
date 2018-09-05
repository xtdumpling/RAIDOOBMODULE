/** @file
  Definition of Alert Standard Format (ASF) 2.0

@copyright
 Copyright (c) 2005 - 2016 Intel Corporation. All rights reserved
 This software and associated documentation (if any) is furnished
 under a license and may only be used or copied in accordance
 with the terms of the license. Except as permitted by the
 license, no part of this software or documentation may be
 reproduced, stored in a retrieval system, or transmitted in any
 form or by any means without the express written consent of
 Intel Corporation.
 This file contains an 'Intel Peripheral Driver' and is uniquely
 identified as "Intel Reference Module" and is licensed for Intel
 CPUs and chipsets under the terms of your license agreement with
 Intel or your vendor. This file may be modified by the user, subject
 to additional terms of the license agreement.

@par Specification Reference:
**/
#ifndef __ALERT_STANDARD_FORMAT_H__
#define __ALERT_STANDARD_FORMAT_H__

#include <IndustryStandard/Acpi.h>
#include <IndustryStandard/AlertStandardFormatTable.h>
#include <AlertStandardFormat.h>

///
/// Alert Standard Format Protocol
/// This protocol provides interfaces to get ASF Boot Options and send ASF messages
///
typedef struct _ALERT_STANDARD_FORMAT_PROTOCOL  ALERT_STANDARD_FORMAT_PROTOCOL;
#define DXE_ALERT_SATNDARD_FORMAT_PROTOCOL_REVISION_1  1

#define ASF_ADDR_DEVICE_ARRAY_LENGTH      16

#pragma pack(1)

typedef struct {
  UINT8   SubCommand;
  UINT8   Version;
  UINT32  IanaId;
  UINT8   SpecialCommand;
  UINT16  SpecialCommandParam;
  UINT16  BootOptions;
  UINT16  OemParameters;
} ASF_BOOT_OPTIONS;

typedef struct {
  UINT8 SubCommand;
  UINT8 Version;
} ASF_CLEAR_BOOT_OPTIONS;

/**
  See the DMTF ASF Specification v2.0, section 4.1.2.

  System firmware identifies the system's ASF support and static configuration using the ACPI
  System Description Table architecture. A pointer to the ASF-defined ASF! description table
  appears as one of the entries in the firmware's RSDT (for IA-32 systems) or XSDT (for IA-64
  systems). The information presented in this table is static from the reference of the client's last
  boot.
  This table is formatted as a standard ACPI System Description Table Header followed by one or
  more ASF information records.
**/
typedef struct {
  EFI_ACPI_DESCRIPTION_HEADER Header;
  EFI_ACPI_ASF_INFO           AsfInfo;
  EFI_ACPI_ASF_ALRT           AsfAlert;
  EFI_ACPI_ASF_RCTL           AsfRctl;
  EFI_ACPI_ASF_RMCP           AsfRmcp;
  EFI_ACPI_ASF_ADDR           AsfAddr;
} ACPI_2_0_ASF_DESCRIPTION_TABLE;

#pragma pack()

//
// Special Command Attributes
//
#define NOP               0x00
#define FORCE_PXE         0x01
#define FORCE_HARDDRIVE   0x02
#define FORCE_SAFEMODE    0x03
#define FORCE_DIAGNOSTICS 0x04
#define FORCE_CDDVD       0x05

//
// Boot Options Mask
//
#define LOCK_POWER_BUTTON             0x0002  ///< 0000 0000 0000 0010 - bit 1
#define LOCK_RESET_BUTTON             0x0004  ///< 0000 0000 0000 0200 - bit 2
#define LOCK_KEYBOARD                 0x0020  ///< 0000 0000 0010 0000 - bit 5
#define LOCK_SLEEP_BUTTON             0x0040  ///< 0000 0000 0100 0000 - bit 6
#define USER_PASSWORD_BYPASS          0x0800  ///< 0000 1000 0000 0000 - bit 3
#define FORCE_PROGRESS_EVENTS         0x1000  ///< 0001 0000 0000 0000 - bit 4
#define FIRMWARE_VERBOSITY_DEFAULT    0x0000  ///< 0000 0000 0000 0000 - bit 6:5
#define FIRMWARE_VERBOSITY_QUIET      0x2000  ///< 0010 0000 0000 0000 - bit 6:5
#define FIRMWARE_VERBOSITY_VERBOSE    0x4000  ///< 0100 0000 0000 0000 - bit 6:5
#define FIRMWARE_VERBOSITY_BLANK      0x6000  ///< 0110 0000 0000 0000 - bit 6:5
#define CONFIG_DATA_RESET             0x8000  ///< 1000 0000 0000 0000 - bit 7
#define ASF_BOOT_OPTIONS_PRESENT      0x16
#define ASF_BOOT_OPTIONS_NOT_PRESENT  0x17

#define USE_KVM                       0x0020  ///< 0000 0000 0010 0000 - bit 5
///
/// ASF Internet Assigned Numbers Authority Manufacturer ID
/// (The firmware sends 0XBE110000 for decimal value 4542)
///
#define INDUSTRY_IANA_SWAP32(x)       ((((x) & 0xff) << 24) | (((x) & 0xff00) << 8) | \
                                        (((x) & 0xff0000) >> 8) | (((x) & 0xff000000) >> 24))
#define ASF_INDUSTRY_IANA             0x000011BE
#define ASF_INDUSTRY_CONVERTED_IANA   INDUSTRY_IANA_SWAP32 (ASF_INDUSTRY_IANA)  ///< 0XBE110000, received from ME FW

/**
  Return the SMBus address used by the ASF driver.
  Not applicable in Intel ME/HECI system, need to return EFI_UNSUPPORTED.

  @retval EFI_SUCCESS             Address returned
  @retval EFI_INVALID_PARAMETER   Invalid SMBus address
**/
typedef
EFI_STATUS
(EFIAPI *ALERT_STANDARD_FORMAT_PROTOCOL_GET_SMBUSADDR) (
  IN  ALERT_STANDARD_FORMAT_PROTOCOL       *This,
  OUT UINTN                                *SmbusDeviceAddress
  )
;

/**
  Set the SMBus address used by the ASF driver. 0 is an invalid address.
  Not applicable in Intel ME/HECI system, need to return EFI_UNSUPPORTED.

  @param[in] SmbusAddr            SMBus address of the controller

  @retval EFI_SUCCESS             Address set
  @retval EFI_INVALID_PARAMETER   Invalid SMBus address
**/
typedef
EFI_STATUS
(EFIAPI *ALERT_STANDARD_FORMAT_PROTOCOL_SET_SMBUSADDR) (
  IN  ALERT_STANDARD_FORMAT_PROTOCOL       *This,
  IN  UINTN                                SmbusDeviceAddress
  )
;

/**
  Return the ASF Boot Options obtained from the controller. If the
  Boot Options parameter is NULL and no boot options have been retrieved,
  Query the ASF controller for its boot options.
  Get ASF Boot Options through HECI.

  @param[in] AsfBootOptions       Pointer to ASF boot options to copy current ASF Boot options

  @retval EFI_SUCCESS             Boot options copied
  @retval EFI_NOT_READY           No boot options
**/
typedef
EFI_STATUS
(EFIAPI *ALERT_STANDARD_FORMAT_PROTOCOL_GET_BOOT_OPTIONS) (
  IN  ALERT_STANDARD_FORMAT_PROTOCOL       *This,
  IN  OUT ASF_BOOT_OPTIONS                 **AsfBootOptions
  )
;

/**
  Send ASF Message.
  Send ASF Message through HECI.

  @param[in] AsfMessage           Pointer to ASF message

  @retval EFI_SUCCESS             Boot options copied
  @retval EFI_INVALID_PARAMETER   Invalid pointer
  @retval EFI_NOT_READY           No controller
**/
typedef
EFI_STATUS
(EFIAPI *ALERT_STANDARD_FORMAT_PROTOCOL_SEND_ASF_MESSAGE) (
  IN  ALERT_STANDARD_FORMAT_PROTOCOL       *This,
  IN  ASF_MESSAGE                          *AsfMessage
  )
;

///
/// Alert Standard Format Protocol
/// This protocol provides interfaces to get ASF Boot Options and send ASF messages
/// HECI protocol is consumed and used to send ASF messages and receive ASF Boot Options
///
struct _ALERT_STANDARD_FORMAT_PROTOCOL {
  UINT8                                           Revision;
  ALERT_STANDARD_FORMAT_PROTOCOL_GET_SMBUSADDR    GetSmbusAddr;
  ALERT_STANDARD_FORMAT_PROTOCOL_SET_SMBUSADDR    SetSmbusAddr;
  ALERT_STANDARD_FORMAT_PROTOCOL_GET_BOOT_OPTIONS GetBootOptions;
  ALERT_STANDARD_FORMAT_PROTOCOL_SEND_ASF_MESSAGE SendAsfMessage;
};

extern EFI_GUID gAlertStandardFormatProtocolGuid;

#endif
