/**+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) 2007 Intel Corporation.  All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license.  Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

    @file SvSmmProtocol.h

    Header file for SV SMM access

---------------------------------------------------------------------------**/

#ifndef _SVOS_SMM_PROTOCOL_H_
#define _SVOS_SMM_PROTOCOL_H_

//
// Includes
//

#define SVOS_SMM_GUID \
  { \
    0xf139bd77, 0x7ff7, 0x49d6, 0x90, 0x86, 0xd5, 0xa, 0xb2, 0x6f, 0x6d, 0xd7 \
  }

//
// Protocol Data Structures
//
typedef 
EFI_STATUS 
(EFIAPI *EFI_SV_EXTENDER) (
  VOID
);

typedef
VOID
(EFIAPI *EFI_SVOS_ACPI_SERVICE_GET_SVOS_GAS_ADDRESS) (
  OUT UINTN                               *Address
  );

typedef
VOID
(EFIAPI *EFI_SVOS_ACPI_SERVICE_SET_SVOS_GAS_ADDRESS) (
  IN UINTN                               Address
  );

typedef
VOID
(EFIAPI *EFI_SVOS_ACPI_SERVICE_GET_SVOS_SMI_DOORBELL) (
  OUT UINT8                               *SmiDoorBell
  );

typedef
VOID
(EFIAPI *EFI_SVOS_ACPI_SERVICE_SET_SVOS_SMI_DOORBELL) (
  IN UINT8                               SmiDoorBell
  );

typedef struct _EFI_SV_SMM_SERVICE_PROTOCOL {

  UINTN                         SvSmmExtenderAddress;
  EFI_SV_EXTENDER               SVExtenderHandler;

} EFI_SV_SMM_SERVICE_PROTOCOL;


typedef struct _EFI_SVOS_ACPI_SERVICE_PROTOCOL {


  EFI_SVOS_ACPI_SERVICE_GET_SVOS_GAS_ADDRESS  GetSvosGasAddress;
  EFI_SVOS_ACPI_SERVICE_SET_SVOS_GAS_ADDRESS  SetSvosGasAddress;
  EFI_SVOS_ACPI_SERVICE_GET_SVOS_SMI_DOORBELL GetSvosSmiDoorBell;
  EFI_SVOS_ACPI_SERVICE_SET_SVOS_SMI_DOORBELL SetSvosSmiDoorBell;

} EFI_SVOS_ACPI_SERVICE_PROTOCOL;

typedef struct _EFI_SV_SERVICE_PROTOCOL {

  EFI_SV_SMM_SERVICE_PROTOCOL     SmmService;

  EFI_SVOS_ACPI_SERVICE_PROTOCOL  AcpiService;

} EFI_SV_SERVICE_PROTOCOL;

extern EFI_GUID gEfiSvSmmProtocolGuid;

#endif
