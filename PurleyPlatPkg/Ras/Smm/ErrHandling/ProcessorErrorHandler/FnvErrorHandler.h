/**++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//

Copyright (c) 2009 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file FnvErrorHandler.h

  This file will contain all definitions related to Memory Error Handler
  
----------------------------------------------------------------**/
	
#ifndef _FNV_ERROR_HANDLER_H
#define _FNV_ERROR_HANDLER_H

#include <Protocol/PlatformRasPolicyProtocol.h>
#include <Protocol/PlatformErrorHandlingProtocol.h>
#include <Protocol/SmmBase2.h>
#include <Library/emcaplatformhookslib.h>
#include <IndustryStandard/WheaDefs.h>
#include <Library/ProcMemErrReporting.h>
#include "ErrorRecords.h"
#include "CommonErrorHandlerDefs.h"
#include <WheaPlatformDefs.h>
#include <Protocol/CrystalRidge.h>
#include "FnvAccess.h"

EFI_STATUS
EnableElogFnv (
  VOID
  );

EFI_STATUS
FnvErrorHandler (
  VOID
  );

#endif

