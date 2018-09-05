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

#ifndef _ASF_TABLE_H_
#define _ASF_TABLE_H_

#include <IndustryStandard/AlertStandardFormatTable.h>
#include <Protocol/AcpiTable.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <CommonIncludes.h>
#include "Library/MeTypeLib.h"
#include <Protocol/AlertStandardFormat.h>


#define ASF_RMCP_BOOT_OPTION_SUCCESS       0x00
#define ASF_RMCP_NO_BOOT_OPTION            0x01

typedef struct {
  EFI_ACPI_DESCRIPTION_HEADER Header;
  EFI_ACPI_ASF_INFO           AsfInfo;
  EFI_ACPI_ASF_ALRT           AsfAlert;
  EFI_ACPI_ASF_ALERTDATA      AsfAlertData[3];
  EFI_ACPI_ASF_RCTL           AsfRctl;
  EFI_ACPI_ASF_CONTROLDATA    AsfControlData[4];
  EFI_ACPI_ASF_RMCP           AsfRmcp;
  EFI_ACPI_ASF_ADDR           AsfAddr;
  UINT8                       FixedSmbusAddresses[ASF_ADDR_DEVICE_ARRAY_LENGTH];
} ACPI_2_0_ASF_DESCRIPTION_TABLE_WITH_DATA;

/**
  Building Alert Standard Format (ASF) Table.

  @param[in] Event         - A pointer to the Event that triggered the callback.
  @param[in] Context       - A pointer to private data registered with the callback function.

  @retval EFI_SUCCESS      - The operation completed successfully.
  @retval EFI_UNSUPPORTED  - The operation is unsupported.
**/
EFI_STATUS
BuildAsfTable (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  );

#endif
