//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/** @file
    ACPI table pcds update.

  Copyright (c) 2015 - 2016, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
**/


#include "UbaMainPei.h"
#include <Library/PcdLib.h>

EFI_STATUS
TypeLightningRidgeEXRPPlatformUpdateAcpiTablePcds (
  VOID
)
{
  CHAR8     AcpiName[]        = "PLATEXRP";     // USED for identify ACPI table in systmeboard dxe driver
  CHAR8     OemTableIdXhci[]    = "xh_nccrb";

  UINTN     Size;
  EFI_STATUS Status;
  
  //#
  //#ACPI items
  //#
  Size = AsciiStrSize (AcpiName);
  Status = PcdSetPtrS (PcdOemSkuAcpiName , &Size, AcpiName);
  ASSERT_EFI_ERROR(Status);

  Size = AsciiStrSize (OemTableIdXhci);
  Status = PcdSetPtrS (PcdOemTableIdXhci , &Size, OemTableIdXhci);
  ASSERT_EFI_ERROR(Status);
   
  return Status;
}

