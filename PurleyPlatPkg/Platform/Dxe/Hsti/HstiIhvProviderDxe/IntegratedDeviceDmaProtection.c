/** @file
  This file contains the tests for the IntegratedDeviceDMAProtection bit

@copyright
  INTEL CONFIDENTIAL
  Copyright 2015 - 2017 Intel Corporation.

  The source code contained or described herein and all documents related to the
  source code ("Material") are owned by Intel Corporation or its suppliers or
  licensors. Title to the Material remains with Intel Corporation or its suppliers
  and licensors. The Material may contain trade secrets and proprietary and
  confidential information of Intel Corporation and its suppliers and licensors,
  and is protected by worldwide copyright and trade secret laws and treaty
  provisions. No part of the Material may be used, copied, reproduced, modified,
  published, uploaded, posted, transmitted, distributed, or disclosed in any way
  without Intel's prior express written permission.

  No license under any patent, copyright, trade secret or other intellectual
  property right is granted to or conferred upon you by disclosure or delivery
  of the Materials, either expressly, by implication, inducement, estoppel or
  otherwise. Any license under such intellectual property rights must be
  express and approved by Intel in writing.

  Unless otherwise agreed by Intel in writing, you may not remove or alter
  this notice or any other notice embedded in Materials by Intel or
  Intel's suppliers or licensors in any way.

  This file contains an 'Intel Peripheral Driver' and is uniquely identified as
  "Intel Reference Module" and is licensed for Intel CPUs and chipsets under
  the terms of your license agreement with Intel or your vendor. This file may
  be modified by the user, subject to additional terms of the license agreement.

@par Specification
**/

#include "HstiIhvProviderDxe.h"

extern IIO_GLOBALS        *IioGlobalData;

/**
  Run tests for IntegratedDeviceDMAProtection bit
**/
VOID
CheckIntegratedDeviceDmaProtection (
  VOID
  )
{
  EFI_STATUS      Status;
  BOOLEAN         Result;
  UINT32          Vtd1;
  UINT32          VtdCtrl;
  UINT32          VtdMmioCap;
  CHAR16          *HstiErrorString;
  UINT8           Stack;
  UINT8           IioIndex = 0;
  EFI_CPU_CSR_ACCESS_PROTOCOL   *CpuCsrAccess;

  if ((mFeatureImplemented[0] & HSTI_BYTE0_INTEGRATED_DEVICE_DMA_PROTECTION) == 0) {
    return;
  }

  Result = TRUE;

  CpuCsrAccess = (EFI_CPU_CSR_ACCESS_PROTOCOL *)IioGlobalData->IioVar.IioOutData.PtrAddress.Address64bit;

  DEBUG ((DEBUG_INFO, "  VTd check\n"));

  DEBUG ((DEBUG_INFO, "    1. VTd supported\n"));

  for (IioIndex = 0; IioIndex < MaxIIO; IioIndex++) {
    if(!IioGlobalData->IioVar.IioVData.SocketPresent[IioIndex]){
      continue;
    }

    for (Stack = 0; Stack < MAX_IIO_STACK; Stack++) {
      // Check for a valid stack 
      if (!(IioGlobalData->IioVar.IioVData.StackPresentBitmap[IioIndex] & (1 << Stack))){
        continue;
      }
      // VTD is not enabled in MCP0(Stack3) Stack for SKX-P FPGA
      if ((IioGlobalData->IioVar.IioVData.FpgaActive[IioIndex]) && ( Stack == IIO_PSTACK3)) {
        continue;
      }

      DEBUG ((DEBUG_INFO, "    2. VTd enabled for Socket %d, Stack %d\n", IioIndex, Stack));

      Vtd1 = CpuCsrAccess->ReadCpuCsr(IioIndex, Stack, VTBAR_IIO_VTD_REG);
      if (((Vtd1 & BIT0) == 0) ||
          ((Vtd1 & 0xfffffffe) == 0)) {
          DEBUG ((DEBUG_INFO, "Fail: VTd not enabled\n"));


          HstiErrorString = BuildHstiErrorString (HSTI_BYTE0_INTEGRATED_DEVICE_DMA_PROTECTION_ERROR_CODE_1 ,HSTI_INTEGRATED_DEVICE_DMA_PROTECTION, HSTI_BYTE0_INTEGRATED_DEVICE_DMA_PROTECTION_ERROR_STRING_1);
          Status = HstiLibAppendErrorString (
                     PLATFORM_SECURITY_ROLE_PLATFORM_REFERENCE,
                     NULL,
                     HstiErrorString
                     );
          ASSERT_EFI_ERROR (Status);
          Result = FALSE;
          FreePool (HstiErrorString);
          break;
      }

      if ((Vtd1 & 0xfffffffe) != 0) {
        DEBUG ((DEBUG_INFO, "    3. VTd locked\n"));

        VtdCtrl = CpuCsrAccess->ReadCpuCsr(IioIndex, Stack, VTGENCTRL_IIO_VTD_REG);
        if ((VtdCtrl & BIT15) == 0) {
          DEBUG ((DEBUG_INFO, "Fail: VTd not locked\n"));

          HstiErrorString = BuildHstiErrorString (HSTI_BYTE0_INTEGRATED_DEVICE_DMA_PROTECTION_ERROR_CODE_1 ,HSTI_INTEGRATED_DEVICE_DMA_PROTECTION, HSTI_BYTE0_INTEGRATED_DEVICE_DMA_PROTECTION_ERROR_STRING_1);
          Status = HstiLibAppendErrorString (
                     PLATFORM_SECURITY_ROLE_PLATFORM_REFERENCE,
                     NULL,
                     HstiErrorString
                     );
          ASSERT_EFI_ERROR (Status);
          Result = FALSE;
          FreePool (HstiErrorString);
        }

        DEBUG ((DEBUG_INFO, "    4. VTd configured correctly\n"));
        VtdMmioCap = MmioRead32(((Vtd1 & ~BIT0) + R_VTD_CAP_LOW));

        if (((VtdMmioCap & BIT23) != 0) && ((VtdMmioCap & BIT22) == 0)) {
          DEBUG ((DEBUG_INFO, "Fail: VTd not configured correctly\n"));

          HstiErrorString = BuildHstiErrorString (HSTI_BYTE0_INTEGRATED_DEVICE_DMA_PROTECTION_ERROR_CODE_1 ,HSTI_INTEGRATED_DEVICE_DMA_PROTECTION, HSTI_BYTE0_INTEGRATED_DEVICE_DMA_PROTECTION_ERROR_STRING_1);
          Status = HstiLibAppendErrorString (
                     PLATFORM_SECURITY_ROLE_PLATFORM_REFERENCE,
                     NULL,
                     HstiErrorString
                     );
          ASSERT_EFI_ERROR (Status);
          Result = FALSE;
          FreePool (HstiErrorString);
        }
      }
    }
  }

  //
  // ALL PASS
  //
  if (Result) {
    Status = HstiLibSetFeaturesVerified (
               PLATFORM_SECURITY_ROLE_PLATFORM_REFERENCE,
               NULL,
               0,
               HSTI_BYTE0_INTEGRATED_DEVICE_DMA_PROTECTION
               );
    ASSERT_EFI_ERROR (Status);
  }

  return;
}
