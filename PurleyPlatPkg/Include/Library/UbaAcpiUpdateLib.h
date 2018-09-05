/*++
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/
/*++

Copyright (c) 2008 - 2013 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  UbaAcpiUpdateLib.h

Abstract:

--*/

#ifndef _PLATFORM_ACPI_UPDATE_LIB_H
#define _PLATFORM_ACPI_UPDATE_LIB_H

#include <Base.h>
#include <Uefi.h>

#define PLATFORM_ACPI_FIX_UPDATE_SIGNATURE  SIGNATURE_32 ('A', 'C', 'P', 'F')
#define PLATFORM_ACPI_FIX_UPDATE_VERSION    01


// {81129EF8-391D-4f63-AE99-58517EC077E3}
#define   PLATFORM_ACPI_FIX_TABLE_GUID \
{ 0x81129ef8, 0x391d, 0x4f63, { 0xae, 0x99, 0x58, 0x51, 0x7e, 0xc0, 0x77, 0xe3 } }

typedef struct {
  UINT32                  Signature;
  UINT32                  Version;
  
  VOID                    *TablePtr;
  
} ACPI_FIX_UPDATE_TABLE;

EFI_STATUS
PlatformGetAcpiFixTableDataPointer (
  IN  VOID                          **TablePtr
);

STATIC  EFI_GUID gPlatformAcpiFixTableGuid = PLATFORM_ACPI_FIX_TABLE_GUID;

#endif //_PLATFORM_ACPI_UPDATE_LIB_H
