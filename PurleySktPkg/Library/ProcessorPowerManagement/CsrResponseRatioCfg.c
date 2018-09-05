//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/** @file

  Code File for CPU Power Management

  Copyright (c) 2006 - 2016, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  Module Name:  CsrResponseRatioCfg.c

**/

#include "CpuPpmIncludes.h"

/**

  This routine is called to program PROCHOT_RESPONSE_RATIO_CFG. It will be call multiple time passing any the SocketNumber to be programmed.

  @param PPMPolicy Pointer to PPM Policy protocol instance

  @retval EFI_SUCCESS

**/
VOID
ProgramCsrResponseRatioCfg (
    EFI_CPU_PM_STRUCT   *ppm
  )
{
  UINT32 data;
  UINT32 MaxRatio;
  UINT32 MinRatio;
  UINT64 RegAddr = 0; 
  UINT8  Size = 4;
  UINT8  SocketNumber;
  EFI_CPU_CSR_ACCESS_PROTOCOL *CpuCsrAccess;

  SocketNumber = ppm->Info->SocketNumber;
  CpuCsrAccess = ppm->CpuCsrAccess;

  MaxRatio = (CpuCsrAccess->ReadCpuCsr(SocketNumber, 0, PLATFORM_INFO_N0_PCU_FUN0_REG) >> 8) & 0x000000ff;        //Bit[15:8]
  MinRatio = (CpuCsrAccess->ReadCpuCsr(SocketNumber, 0, PLATFORM_INFO_N1_PCU_FUN0_REG) >> 8) & 0x000000ff;        //Bit[15:8]

  // Read CSR
  data = CpuCsrAccess->ReadCpuCsr(SocketNumber, 0, RESPONSE_RATIO_CFG_PCU_FUN2_REG);

  //
  // If setup value non-zero, set prochot ratio using setup value
  //
  if (ppm->Setup->ProchotRatio) {
    data &= ~0x0FF;
    if (ppm->Setup->ProchotRatio < MinRatio) {
      data |= MinRatio;
    } else if (ppm->Setup->ProchotRatio > MaxRatio) {
      data |= MaxRatio;
    } else {
      data |= (UINT32) ppm->Setup->ProchotRatio;
    }
  //
  // Otherwise, adjust current value within acceptable range if needed (Pn is reset value)
  //
  } else {
    data &= 0x0FF;  //PROCHOT_RATIO Bit[7:0]

    if (data < MinRatio) {
      data &= ~0x0FF;
      data |= MinRatio;
    } else if (data > MaxRatio) {
      data &= ~0x0FF;
      data |= MaxRatio;
    }
  }

  CpuCsrAccess->WriteCpuCsr(SocketNumber, 0, RESPONSE_RATIO_CFG_PCU_FUN2_REG, data);
  RegAddr = CpuCsrAccess->GetCpuCsrAddress(SocketNumber, 0, RESPONSE_RATIO_CFG_PCU_FUN2_REG, &Size);
  S3BootScriptSaveMemWrite (S3BootScriptWidthUint32, RegAddr, 1, &data );
}
