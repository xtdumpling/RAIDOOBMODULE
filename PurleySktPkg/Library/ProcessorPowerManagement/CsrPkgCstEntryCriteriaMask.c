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

  Module Name:  CsrPkgCstEntryCriteriaMask.c

**/

#include "CpuPpmIncludes.h"


/**

  GC_TODO: add routine description

  @param ppm - GC_TODO: add arg description

  @retval None

**/
VOID
ProgramCsrPkgCstEntryCriteriaMask(
  EFI_CPU_PM_STRUCT   *ppm
  )
{
  UINT32   data;
  UINT8    SocketNumber;
  UINT64   RegAddr = 0; 
  UINT8    Size = 4;
  EFI_CPU_CSR_ACCESS_PROTOCOL *CpuCsrAccess;

  SocketNumber = ppm->Info->SocketNumber;
  CpuCsrAccess = ppm->CpuCsrAccess;

  data = CpuCsrAccess->ReadCpuCsr(SocketNumber, 0, PKG_CST_ENTRY_CRITERIA_MASK_CFG2_PCU_FUN2_REG);

  if(ppm->Setup->AdvPwrMgtCtlFlags & PCD_CPU_PKG_CST_ENTRY_VAL_CTL) { 

    data &= ~SET_KTI_INPKGCENTRY;

    //
    // Get BIOS Knobs and set KTI C-state entry criteria mask
    //
    data |= ppm->Setup->Cst.PkgCstEntryCriteriaMaskKti[SocketNumber];

    // Program CSR_PKG_CST_ENTRY_CRITERIA_MASK
    CpuCsrAccess->WriteCpuCsr(SocketNumber, 0, PKG_CST_ENTRY_CRITERIA_MASK_CFG2_PCU_FUN2_REG , data);
    RegAddr = CpuCsrAccess->GetCpuCsrAddress(SocketNumber, 0, PKG_CST_ENTRY_CRITERIA_MASK_CFG2_PCU_FUN2_REG, &Size);
    S3BootScriptSaveMemWrite (S3BootScriptWidthUint32, RegAddr, 1, &data );
  }
}


/* 
  This function porgram PCIE_IN_Lx fields in PKG_CST_CRITERIA_MASK CSR 
*/
/**

    GC_TODO: add routine description

    @param ppm - GC_TODO: add arg description

    @retval None

**/
VOID
EFIAPI
SetupPCIEPkgCstEntryCriteria (
  EFI_CPU_PM_STRUCT   *ppm
  )
{
  UINT64  RegAddr = 0;
  UINT8   SocketNumber, Size;
  EFI_CPU_CSR_ACCESS_PROTOCOL *CpuCsrAccess;
  UINT32  data;

  SocketNumber = ppm->Info->SocketNumber;
  CpuCsrAccess = ppm->CpuCsrAccess;
  data = CpuCsrAccess->ReadCpuCsr(SocketNumber, 0, PKG_CST_ENTRY_CRITERIA_MASK_PCU_FUN2_REG);

  if (ppm->Setup->AdvPwrMgtCtlFlags & PCD_CPU_PKG_CST_ENTRY_VAL_CTL) {

    data &= MASK_PCIE_IN_L1_BITS;  //clear bit23:0 PCIE_Lx

    //
    // Get BIOS Knobs and set Pcie C-state entry criteria mask
    //
    data |= ppm->Setup->Cst.PkgCstEntryCriteriaMaskPcie[SocketNumber];

    // Program PKG_CST_ENTRY_CRITERIA_MASK_PCU_FUN2_REG
    CpuCsrAccess->WriteCpuCsr(SocketNumber, 0, PKG_CST_ENTRY_CRITERIA_MASK_PCU_FUN2_REG , data);
    Size = 4;
    RegAddr = CpuCsrAccess->GetCpuCsrAddress(SocketNumber, 0, PKG_CST_ENTRY_CRITERIA_MASK_PCU_FUN2_REG, &Size);
    S3BootScriptSaveMemWrite (S3BootScriptWidthUint32, RegAddr, 1, &data );
  }

}

/**

  This routine is called to program PCU_MISC_CONFIG. It will be call multiple time passing any the SocketNumber to be programmed.

  @param PPMPolicy Pointer to PPM Policy protocol instance

  @retval EFI_SUCCESS

**/
VOID
ProgramB2PDynamicL1 (
    EFI_CPU_PM_STRUCT *ppm
  )
{
  UINT32 PM_MBX_CMD = 0;
  UINT32 PM_MBX_DATA = 0;
  UINT32 data32 = 0;
  UINT32 Tempdata = 0;
  UINT8  SocketNumber;
  EFI_CPU_CSR_ACCESS_PROTOCOL *CpuCsrAccess;

  SocketNumber = ppm->Info->SocketNumber;
  CpuCsrAccess = ppm->CpuCsrAccess;
  data32 = CpuCsrAccess->ReadCpuCsr(SocketNumber, 0, BIOS_MAILBOX_DATA_PCU_FUN1_REG);
 
  if (ppm->Setup->AdvPwrMgtCtlFlags & DYNAMIC_L1_DISABLE) {

    PM_MBX_DATA = 0; 
    PM_MBX_CMD = (UINT32) MAILBOX_BIOS_CMD_READ_PCU_MISC_CONFIG;
    Tempdata = ppm->CpuCsrAccess->Bios2PcodeMailBoxWrite(SocketNumber, PM_MBX_CMD, PM_MBX_DATA);
    if ((Tempdata & 0x000000ff) != PPM_MAILBOX_BIOS_ERROR_CODE_INVALID_COMMAND) {
      data32 = CpuCsrAccess->ReadCpuCsr(SocketNumber, 0, BIOS_MAILBOX_DATA_PCU_FUN1_REG);
      data32 |= PCU_MISC_CONFIG_DYNAMIC_L1;

      PM_MBX_CMD = (UINT32) MAILBOX_BIOS_CMD_WRITE_PCU_MISC_CONFIG;
      PM_MBX_DATA |= data32; 

      Tempdata = ppm->CpuCsrAccess->Bios2PcodeMailBoxWrite(SocketNumber, PM_MBX_CMD, PM_MBX_DATA);
    }
  }
}
