/** @file
  Header file for Silicon Init Pre Memory module.

@copyright
  Copyright (c) 2014 - 2015 Intel Corporation. All rights reserved
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

@par Specification
**/
#ifndef _SI_INIT_PRE_MEM_MODULE_H_
#define _SI_INIT_PRE_MEM_MODULE_H_

#include <Library/DebugLib.h>
#include <Library/PeiServicesLib.h>
#include <IncludePrivate/Library/PchInitLib.h>

//
// IO/MMIO resource limits
//
#define MIN_IO_SPACE_SIZE   0x10
//
// Set minimum MMIO space to 2MB for PCH XDCI requirement
//
#define MIN_MMIO_SPACE_SIZE 0x200000

//
// Pre Mem Performance GUIDs
//
extern EFI_GUID gPerfPchPrePolicyGuid;
extern EFI_GUID gPerfSiValidateGuid;
extern EFI_GUID gPerfPchValidateGuid;
extern EFI_GUID gPerfAmtValidateGuid;
extern EFI_GUID gPerfCpuValidateGuid;
extern EFI_GUID gPerfMeValidateGuid;
extern EFI_GUID gPerfSaValidateGuid;
extern EFI_GUID gPerfHeciPreMemGuid;
extern EFI_GUID gPerfPchPreMemGuid;
extern EFI_GUID gPerfCpuPreMemGuid;
extern EFI_GUID gPerfMePreMemGuid;
extern EFI_GUID gPerfSaPreMemGuid;
extern EFI_GUID gPerfEvlGuid;
extern EFI_GUID gPerfMemGuid;


#endif // _SI_INIT_PRE_MEM_MODULE_H_
