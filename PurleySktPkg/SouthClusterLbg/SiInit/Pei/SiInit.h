/** @file
  Header file for Silicon Init PEIM.

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
#ifndef _SI_INIT_MODULE_H_
#define _SI_INIT_MODULE_H_

#include <Library/HobLib.h>
#include <Library/DebugLib.h>
#include <Library/PeiServicesLib.h>
#include <Ppi/EndOfPeiPhase.h>
#include <IncludePrivate/Library/PchInitLib.h>

//
// GUID Definitions
//
extern EFI_GUID gSiPolicyHobGuid;


#endif // _SI_INIT_MODULE_H_
