//
// This file contains a 'Sample Driver' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may be modified by the user, subject to
// the additional terms of the license agreement
//
/** @file
  Header file for MemorySubClass Driver.

  Copyright (c) 1999 - 2015 Intel Corporation.  All rights reserved.
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/

#ifndef _MEMORY_SUBCLASS_DRIVER_H
#define _MEMORY_SUBCLASS_DRIVER_H

#include <IndustryStandard/SmBios.h>
#include <Protocol/Smbios.h>

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/PrintLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiLib.h>
#include <Library/HiiLib.h>
#include <Library/HobLib.h>
#include <Library/BaseMemoryLib.h>

#include <Guid/MemoryMapData.h>

#define MAX_DIMM_SIZE            256 // in GB
#define MAX_APACHEPASS_DIMM_SIZE 512 // in GB
#define MAX_APACHEPASS_DIMM_NUM  6

// Currently the only voltage supported by DDR4
#define SPD_VDD_120 3 // Module operable and endurant 1.20V

//
// This is the generated header file which includes whatever needs to be exported (strings + IFR)
//

#define EFI_MEMORY_SUBCLASS_DRIVER_GUID \
  { \
    0xef17cee7, 0x267d, 0x4bfd, 0xa2, 0x57, 0x4a, 0x6a, 0xb3, 0xee, 0x85, 0x91 \
  }

extern UINT8 MemorySubClassStrings[];

#endif

