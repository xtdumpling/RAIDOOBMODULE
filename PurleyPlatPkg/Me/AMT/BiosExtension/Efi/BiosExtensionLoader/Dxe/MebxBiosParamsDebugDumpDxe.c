/** @file
  Dump whole MEBX_BPF and serial out.

@copyright
 Copyright (c) 2013 - 2016 Intel Corporation. All rights reserved
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

@par Specification Reference:
**/

#include <PiDxe.h>
#include <Library/DebugLib.h>
#include "BiosExtensionLoader.h"

/**
  Dump MEBx BIOS Params

  @param[in] MebxBiosParams       MEBx BIOS params

**/
VOID
DxeMebxBiosParamsDebugDump (
  IN MEBX_BPF                     *MebxBiosParams
  )
{
  DEBUG ((DEBUG_INFO, "\n------------------------ MebxBiosParams Dump Begin -----------------\n"));
  DEBUG ((DEBUG_INFO, " BpfVersion : 0x%x\n", MebxBiosParams->BpfVersion));
  DEBUG ((DEBUG_INFO, " CpuReplacementTimeout : 0x%x\n", MebxBiosParams->CpuReplacementTimeout));
  DEBUG ((DEBUG_INFO, " ActiveRemoteAssistanceProcess : 0x%x\n", MebxBiosParams->ActiveRemoteAssistanceProcess));
  DEBUG ((DEBUG_INFO, " CiraTimeout : 0x%x\n", MebxBiosParams->CiraTimeout));
  DEBUG ((DEBUG_INFO, " OemFlags : 0x%x\n", MebxBiosParams->OemFlags));
  DEBUG ((DEBUG_INFO, "MebxDebugFlags ---\n"));
  DEBUG ((DEBUG_INFO, " Port80 : 0x%x\n", MebxBiosParams->MebxDebugFlags.Port80));
  DEBUG ((DEBUG_INFO, " MeBiosSyncDataPtr : 0x%x\n", MebxBiosParams->MeBiosSyncDataPtr));
  DEBUG ((DEBUG_INFO, " UsbKeyDataStructurePtr : 0x%x\n", MebxBiosParams->UsbKeyDataStructurePtr));
  DEBUG ((DEBUG_INFO, "OemResolutionSettings ---\n"));
  DEBUG ((DEBUG_INFO, " MebxNonUiTextMode : 0x%x\n", MebxBiosParams->OemResolutionSettings.MebxNonUiTextMode));
  DEBUG ((DEBUG_INFO, " MebxUiTextMode : 0x%x\n", MebxBiosParams->OemResolutionSettings.MebxUiTextMode));
  DEBUG ((DEBUG_INFO, " MebxUiTextMode : 0x%x\n", MebxBiosParams->OemResolutionSettings.MebxGraphicsMode));
  DEBUG ((DEBUG_INFO, "\n------------------------ MebxBiosParams Dump End -------------------\n"));
}

