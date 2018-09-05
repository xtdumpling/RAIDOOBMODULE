//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/**
Copyright 1996 - 20016, Intel Corporation.

This source code and any documentation accompanying it ("Material") is furnished
under license and may only be used or copied in accordance with the terms of that
license.  No license, express or implied, by estoppel or otherwise, to any
intellectual property rights is granted to you by disclosure or delivery of these
Materials.  The Materials are subject to change without notice and should not be
construed as a commitment by Intel Corporation to market, license, sell or support
any product or technology.  Unless otherwise provided for in the license under which
this Material is provided, the Material is provided AS IS, with no warranties of
any kind, express or implied, including without limitation the implied warranties
of fitness, merchantability, or non-infringement.  Except as expressly permitted by
the license for the Material, neither Intel Corporation nor its suppliers assumes
any responsibility for any errors or inaccuracies that may appear herein.  Except
as expressly permitted by the license for the Material, no part of the Material
may be reproduced, stored in a retrieval system, transmitted in any form, or
distributed by any means without the express written consent of Intel Corporation.


  @file AcpiPlatformHooks.h
  
--*/

#ifndef _ACPI_PLATFORM_HOOKS_H_
#define _ACPI_PLATFORM_HOOKS_H_

//
// Statements that include other header files
//
#include <PiDxe.h>
#include <Protocol/CpuIo.h>
#include <Library/CpuConfigLib.h>
#include <Library/SetupLib.h>
#include <Protocol/SvSmmProtocol.h>
#include <Library/UsraAccessApi.h>

EFI_STATUS
PlatformHookInit (
  VOID
  );


VOID
DisableAriForwarding (
  VOID
  );

EFI_STATUS
AllocateRasfSharedMemory (
  VOID
);

UINT8 
EFIAPI 
DetectHwpFeature(
  VOID
  );

VOID
InstallXhciAcpiTable (
  VOID
  );

#endif
