//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/** @file

  Code File for CPU Power Management

  Copyright (c) 2006 - 2015, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  Module Name:  CsrPerfPLimit.c

**/

#include "CpuPpmIncludes.h"

/**

  This routine is called to program PERF_P_LIMIT_CONTROL. It will be call multiple time passing any the SocketNumber to be programmed.

  @param PPMPolicy Pointer to PPM Policy protocol instance

  @retval EFI_SUCCESS

**/
VOID
ProgramCsrPerfPlimitControl (
  EFI_CPU_PM_STRUCT   *ppm
  )
{
  UINT32 data;
  UINT64 RegAddr = 0; 
  UINT8  Size = 4;
  UINT8   SocketNumber;
  EFI_CPU_CSR_ACCESS_PROTOCOL *CpuCsrAccess;

  SocketNumber = ppm->Info->SocketNumber;
  CpuCsrAccess = ppm->CpuCsrAccess;

  // DEBUG ( (EFI_D_ERROR, ":: Doing PERF_P_LIMIT_CONTROL...\n") );
  // Read CSR
  data = CpuCsrAccess->ReadCpuCsr(SocketNumber, 0, PERF_P_LIMIT_CONTROL_PCU_FUN2_REG);
  
  data &= ~PERF_P_LIMIT_CTRL_MASK;

  data |= ppm->Setup->PerPLimitCtl;

  CpuCsrAccess->WriteCpuCsr(SocketNumber, 0, PERF_P_LIMIT_CONTROL_PCU_FUN2_REG, data);
  RegAddr = CpuCsrAccess->GetCpuCsrAddress(SocketNumber, 0, PERF_P_LIMIT_CONTROL_PCU_FUN2_REG, &Size);
  S3BootScriptSaveMemWrite (S3BootScriptWidthUint32, RegAddr, 1, &data );
}
