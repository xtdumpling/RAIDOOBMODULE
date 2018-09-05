/**
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c) 2007-2015 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

  @file HeciInit.h

  HECI Intializion DXE Driver

**/
#ifndef _DXE_HECIINIT_H_
#define _DXE_HECIINIT_H_

#include <PiDxe.h>

#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/MemoryAllocationLib.h>
#include "Protocol/HeciProtocol.h"
#if defined(AMT_SUPPORT) && AMT_SUPPORT
#include "Protocol/MeInfo.h"
#endif // AMT_SUPPORT

#include "Library/HeciCoreLib.h"
#include "MeState.h"

#define ONE_SECOND_TIMEOUT  1000000
#define FWU_TIMEOUT         90

#define CMOS_ADDR_PORT          0x70
#define CMOS_DATA_PORT          0x71
#define UPPER_CMOS_ADDR_PORT    0x72
#define UPPER_CMOS_DATA_PORT    0x73
#define CMOS_BAD_REG            0x0E

#ifdef SPS_CMOS_WRITE 
#define SPSSETUP_SSP_FC_RECEIVED_REG   0x00
#endif

//
// HECI bus function version
//
#define HBM_MINOR_VERSION 0
#define HBM_MAJOR_VERSION 1

//
// HECI private data structure
//
typedef struct {
  UINT16                  DeviceInfo;
  UINT32                  RevisionInfo;
  HECI_PROTOCOL           HeciCtlr;
  VOLATILE UINT32         *HpetTimer;
#if defined(AMT_SUPPORT) && AMT_SUPPORT
  EFI_ME_INFO_PROTOCOL    MeInfo;
  UINT8                   MeFwImageType;
  // events
  EFI_EVENT               ReadyToBootEvent;
  EFI_EVENT               EfiShellEvent;
  EFI_EVENT               EfiPmEvent;
  EFI_EVENT               LegacyBootEvent;
  EFI_EVENT               MpbClearEvent;
#endif // AMT_SUPPORT
  EFI_EVENT               SetupEnterEvent;
  EFI_HANDLE              HeciDrvImageHandle;
} HECI_INSTANCE;

#if defined(AMT_SUPPORT) && AMT_SUPPORT
/**
  Create the FWSTS info to the SMBIOS table using the SMBIOS protocol
  Invoke this routine to add the table entry when all the FWSTS data is finalized.

  - @pre
    - EFI_SMBIOS_PROTOCOL
  @param[in]  Event               - A pointer to the Event that triggered the callback.
                                    If the Event is not NULL, invoke DisableAllMeDevices () before exit
                                    because it is the event registered when failed to install HeciProtocol
                                    and locate SmbiosProtocol early
  @param[in]  Context             - A pointer to private data registered with the callback

  @retval EFI_SUCCESS             - if the data is successfully reported.
  @retval EFI_OUT_OF_RESOURCES    - if not able to get resources.
  @retval EFI_UNSUPPORTED         - if required DataHub or SMBIOS protocol is not available.
**/
VOID
EFIAPI
AddFwStsSmbiosEntry (
  IN  EFI_EVENT                   Event,
  IN  VOID                        *Context
  )
;
#endif // defined(AMT_SUPPORT) && AMT_SUPPORT

#endif // _DXE_HECIINIT_H_

