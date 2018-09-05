/**

Copyright (c) 2008 - 2014 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


   @file SvSmmSupport.c

  SV SMM Driver implementation.

**/

#ifndef _SV_SMM_SUPPORT_H
#define _SV_SMM_SUPPORT_H

#include <Protocol/SvSmmProtocol.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiLib.h>

typedef struct _SVOS_ACPI_DATA {

  UINTN                         AcpiGasAddress;   
  UINT8                         SmiDoorBell;

} SVOS_ACPI_DATA;

typedef struct _SV_SMM_SERVICE_DATA {

  UINTN                         SvSmmExtenderAddress;

} SV_SMM_SERVICE_DATA;

typedef struct _SV_COMMUNICATION_DATA {

  SV_SMM_SERVICE_DATA SmmData;

  SVOS_ACPI_DATA      AcpiData;

} SV_COMMUNICATION_DATA;   


VOID
GetSvosGasAddress (
  OUT UINTN         *Address
  );

VOID
SetSvosGasAddress (
  IN UINTN          Address
  );

VOID
GetSvosSmiDoorBell (
  OUT UINT8         *SmiDoorBell
  );

VOID
SetSvosSmiDoorBell (
  IN UINT8          SmiDoorBell
  );


#endif
