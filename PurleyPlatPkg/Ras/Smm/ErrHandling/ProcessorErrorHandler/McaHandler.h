/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//

Copyright (c) 2009 - 2012 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  McaHandler.h

Abstract:
  This file will contain all definitions related to MCA handler 

------------------------------------------------------------------*/
	
#ifndef _SMI_MCA_ERROR_HANDLER_H
#define _SMI_MCA_ERROR_HANDLER_H

#include <Protocol/PlatformRasPolicyProtocol.h>
#include <Protocol/PlatformErrorHandlingProtocol.h>
#include <Library/SmmMemLib.h>
#include <Protocol/SmmBase2.h>

typedef struct {
  UINT32                    ApicId;
  EFI_CPU_PHYSICAL_LOCATION Loc;
  UINTN                     CpuIndex;
  EFI_EXCEPTION_TYPE        InterruptType;
  EFI_SYSTEM_CONTEXT        SystemContext;
} SMM_MCA_CPU_INFO;

/**
  MCA handler for SMM use.

**/
VOID
EFIAPI
SmiMcaHandler (
  IN EFI_EXCEPTION_TYPE   InterruptType,
  IN EFI_SYSTEM_CONTEXT   SystemContext
);
 
VOID
EFIAPI
McePrint (
  IN  CONST CHAR8  *Format,
  ...
)
;

extern  SMM_ERROR_MP_SYNC_DATA                *mSmmErrorMpSyncData;
extern  EFI_RAS_SYSTEM_TOPOLOGY               *mRasTopology;
extern  SYSTEM_RAS_CAPABILITY                 *mRasCapability;
extern  EFI_PLATFORM_ERROR_HANDLING_PROTOCOL  *mPlatformErrorHandlingProtocol;
 
#endif


