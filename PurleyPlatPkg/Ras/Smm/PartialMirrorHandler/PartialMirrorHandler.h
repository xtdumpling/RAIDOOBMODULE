/*++

Copyright (c)  1999 - 2015 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  PartialMirrorHandler.h

Abstract:

  Header file for CPU/Memory/IO socket PartialMirror Driver.

Revision History:

--*/

#ifndef _PARTIALMIRROR_HANDLER_H
#define _PartialMIRROR_HANDLER_H

//
// Include files
//
#include <Library/SmmServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiLib.h>
#include <Protocol/SmmBase2.h>
#include <Protocol/GlobalNvsArea.h>
#include <Protocol/SmmSwDispatch2.h>
#include <Guid/AcpiVariable.h>
#include <Ras/RasStateMachine.h>
#include <Protocol/SmmPeriodicTimerDispatch2.h>
#include "SysRegs.h"
#include "IchRegs.h"
#include <Library/PciLib.h>
#include <Library/HobLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Protocol/MemRasProtocol.h>
#include <Protocol/SmmVariable.h>

#ifndef EFI_DEADLOOP 
 #define EFI_DEADLOOP()    { volatile int __iii; __iii = 1; while (__iii); }
#endif

//
// Prototypes
//
EFI_STATUS
EFIAPI
InitPartialMirrorHandler (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  );

#endif
