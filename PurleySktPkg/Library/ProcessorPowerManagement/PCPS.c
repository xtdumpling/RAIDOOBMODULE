/**
This file contains an 'Intel Peripheral Driver' and is
licensed for Intel CPUs and chipsets under the terms of your
license agreement with Intel or your vendor. This file may
be modified by the user, subject to additional terms of the
license agreement
**/
/** @file

Code for Enhanced Intel Speedstep Technology

Copyright (c) 2006 - 2016, Intel Corporation. All rights reserved.<BR>
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name: PCPS.c

**/

#include "CpuPpmIncludes.h"

/**

  This routine is called to program PCU_MISC_CONFIG. It will be call multiple time passing any the SocketNumber to be programmed.

  @param PPMPolicy Pointer to PPM Policy protocol instance

  @retval EFI_SUCCESS

**/
VOID
ProgramB2PPcuMiscConfig (
    EFI_CPU_PM_STRUCT *ppm
  )
{
  UINT32 PM_MBX_CMD = 0;
  UINT32 PM_MBX_DATA = 0;
  UINT32 data32 = 0;
  UINT32 Tempdata = 0;
  UINT8   SocketNumber;
  EFI_CPU_CSR_ACCESS_PROTOCOL *CpuCsrAccess;

  SocketNumber = ppm->Info->SocketNumber;
  CpuCsrAccess = ppm->CpuCsrAccess;
  data32 = CpuCsrAccess->ReadCpuCsr(SocketNumber, 0, BIOS_MAILBOX_DATA_PCU_FUN1_REG);

  SocketNumber = ppm->Info->SocketNumber;

  if ( ((ppm->Info->CapId4 & B_PCPS_DISABLE) == 0) && ((ppm->Setup->PcpsCtrl & PCD_CPU_PCPS_SPD) != 0) ) {

    PM_MBX_DATA = 0; 
    PM_MBX_CMD = (UINT32) MAILBOX_BIOS_CMD_READ_PCU_MISC_CONFIG;
    Tempdata = ppm->CpuCsrAccess->Bios2PcodeMailBoxWrite(SocketNumber, PM_MBX_CMD, PM_MBX_DATA);
    if ((Tempdata & 0x000000ff) != PPM_MAILBOX_BIOS_ERROR_CODE_INVALID_COMMAND) {
      data32 = CpuCsrAccess->ReadCpuCsr(SocketNumber, 0, BIOS_MAILBOX_DATA_PCU_FUN1_REG);
      data32 |= PCU_MISC_CONFIG_SPD_ENABLE;

      PM_MBX_CMD = (UINT32) MAILBOX_BIOS_CMD_WRITE_PCU_MISC_CONFIG;
      PM_MBX_DATA |= data32; 

      Tempdata = ppm->CpuCsrAccess->Bios2PcodeMailBoxWrite(SocketNumber, PM_MBX_CMD, PM_MBX_DATA);
    }

    PM_MBX_DATA = 0; 
    PM_MBX_CMD = (UINT32) MAILBOX_BIOS_CMD_READ_PCU_MISC_CONFIG;
    Tempdata = ppm->CpuCsrAccess->Bios2PcodeMailBoxWrite(SocketNumber, PM_MBX_CMD, PM_MBX_DATA);
    if ((Tempdata & 0x000000ff) != PPM_MAILBOX_BIOS_ERROR_CODE_INVALID_COMMAND) {
      data32 = CpuCsrAccess->ReadCpuCsr(SocketNumber, 0, BIOS_MAILBOX_DATA_PCU_FUN1_REG);
    }
  }
}

/**

  This routine is called to program MSR_MISC_PWR_MGMT.

  @param PPMPolicy       - Pointer to PPM Policy protocol instance
  @param ProcessorNumber - Processor index that is executing

  @retval None

**/
VOID
ProgramMsrMiscPwrMgmt (
  EFI_CPU_PM_STRUCT *ppm,
  UINTN             ProcessorNumber
  )

{
  MSR_REGISTER  MsrData;
    
  //
  // Program MSR_MISC_PWR_MGMT
  //
  MsrData.Qword = AsmReadMsr64 (MSR_MISC_PWR_MGMT);
  MsrData.Dwords.Low &= (MsrData.Dwords.Low & ~(B_SINGLE_PCTL_EN)) ;
 
  if (ppm->Setup->PcpsCtrl & PCD_CPU_PCPS_SINGLEPCTL) { 
    MsrData.Dwords.Low |= B_SINGLE_PCTL_EN;
    AsmWriteMsr64 (MSR_MISC_PWR_MGMT, MsrData.Qword);

    //save for S3
    WriteRegisterTable(ProcessorNumber, Msr, MSR_MISC_PWR_MGMT, 0, 64, MsrData.Qword);
  }

  return;
}

