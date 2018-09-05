//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/** @file

  Code File for CPU Power Management

  Copyright (c) 2006 - 2017, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  Module Name:  CsrPmaxConfig.c

**/

#include "CpuPpmIncludes.h"

//
// This table is used for PMax Offset programming based on Package TDP
/*
TDP (W) BIOS Register
55      00001
65      00001
75      00001
85      00001
90      00001
105     00010
120     00010
135     00010
140     00010
145     00011
150     00011
*/
const UINT16        mPMAXOffsetvsTDPTable[][2] = {
        // TDP         MSR Value
       {   55        ,     1 },  
       {   65        ,     1 },
       {   75        ,     1 },
       {   85        ,     1 },
       {   90        ,     1 },
       {  105        ,     2 },
       {  120        ,     2 },
       {  135        ,     2 },
       {  140        ,     2 },
       {  145        ,     3 },
       {  150        ,     3 },
       {END_OF_TABLE,END_OF_TABLE}
};

/**
  Get Die Power Limit from B2P MAILBOX_BIOS_CMD_XEON_POWER_LIMIT

  This function retuns of Xeon CPU die power limit in Watts

  @param  EFI_CPU_PM_STRUCT
**/
UINT32
ReadB2PForDiePowerLimit (
    EFI_CPU_PM_STRUCT *ppm
  )
{
  UINT32    PmMailBoxCommand = 0;
  UINT32    PmMailBoxData = 0;
  UINT32    Tempdata = 0;
  UINT8     SocketNumber;
  EFI_CPU_CSR_ACCESS_PROTOCOL  *CpuCsrAccess;

  SocketNumber = ppm->Info->SocketNumber;
  CpuCsrAccess = ppm->CpuCsrAccess;

  PmMailBoxCommand = (UINT32)MAILBOX_BIOS_CMD_XEON_POWER_LIMIT;

  Tempdata = ppm->CpuCsrAccess->Bios2PcodeMailBoxWrite(SocketNumber, PmMailBoxCommand, PmMailBoxData);
  if (Tempdata == MAILBOX_BIOS_ERROR_CODE_INVALID_WORKAROUND) {
    DEBUG ((EFI_D_ERROR, " \n\n :: !!! Failed to get XEON_POWER_LIMIT(0x%x) for FPGA Sku, in current socket (%d) !!!  Error code: %d \n", PmMailBoxCommand, SocketNumber, Tempdata));
    DEBUG ((EFI_D_ERROR, " :: !!! Setting to reset safe TDP value: 118W \n\n"));
    PmMailBoxData = 0x3B0;
  } else {
    // B2P mailbox data to read vccp voltage value
    PmMailBoxData = CpuCsrAccess->ReadCpuCsr(SocketNumber, 0, BIOS_MAILBOX_DATA_PCU_FUN1_REG);
    DEBUG ( (EFI_D_INFO, " \n:PM: Read B2P XEON_POWER_LIMIT(0x%x) for FPGA Sku, Value = %x in S#%d\n", PmMailBoxCommand, PmMailBoxData, SocketNumber));
  }

  //
  // Calculate value to be in Watts from 12.3 format
  //
  Tempdata = (UINT16) DivU64x32 ((UINT64)(PmMailBoxData & (UINT32)MAILBOX_BIOS_DIE_POWER_LIMIT_MASK), ppm->Info->ProcessorPowerUnit[SocketNumber]);

  return (Tempdata);
}

/**

  This routine is called to PMax Offset from the table based on each socket TDP value.

  @param PPMPolicy Pointer to PPM Policy protocol instance

  @retval PMax Offset

**/
UINT32
GetFromPMaxOffsetTable (
    EFI_CPU_PM_STRUCT   *ppm
    )
{
  UINT16                                Index;
  UINT32                                data;
  UINT32                                GetValue;
  UINT8                                 SocketNumber;
  BOOLEAN                               FoundinTBL;
  CAPID0_PCU_FUN3_STRUCT                CapId0;
  UINT32                                IccMax;
  EFI_CPU_CSR_ACCESS_PROTOCOL           *CpuCsrAccess;
  CONFIG_TDP_LEVEL1_N0_PCU_FUN3_STRUCT  ConfigTdpLevel1;
  VR_CURRENT_CONFIG_N0_PCU_FUN0_STRUCT  VrCurrentConfig;

  CpuCsrAccess = ppm->CpuCsrAccess;
  SocketNumber = ppm->Info->SocketNumber;

  CapId0.Data = CpuCsrAccess->ReadCpuCsr(SocketNumber, 0, CAPID0_PCU_FUN3_REG);

  //
  // CapId0.segment - 011: EX with FPGA; 010: EP with FPGA
  // If FPGA, read processor TDP from B2P
  //
  if ((CapId0.Bits.segment != 2) && (CapId0.Bits.segment != 3)) {
    //
    // Need to read TDP from TDP Level1 CSR for accurate PMAX offset calculation
    //
    ConfigTdpLevel1.Data = CpuCsrAccess->ReadCpuCsr(SocketNumber, 0, CONFIG_TDP_LEVEL1_N0_PCU_FUN3_REG);
    data = (UINT32)DivU64x32 ((UINT64)ConfigTdpLevel1.Bits.pkg_tdp, ppm->Info->ProcessorPowerUnit[SocketNumber]);
  } else {
    data = ReadB2PForDiePowerLimit(ppm);
  }

  //
  // Need to read Icc Max to use to determine on or off roadmap part.  Units are in 0.125A
  //
  VrCurrentConfig.Data = CpuCsrAccess->ReadCpuCsr(SocketNumber, 0, VR_CURRENT_CONFIG_N0_PCU_FUN0_REG);
  IccMax = (UINT32)DivU64x32 ((UINT64)VrCurrentConfig.Bits.current_limit, 8);

  GetValue = 0;
  FoundinTBL = FALSE;

  for (Index = 0; mPMAXOffsetvsTDPTable[Index][0] != END_OF_TABLE; Index++) {

    //
    // IccMax of 228A indicates off-roadmap part, return 0 offset correction
    //
    if ((data < mPMAXOffsetvsTDPTable[0][0]) || (IccMax == 228)) {
      break;
    }

    if (data == mPMAXOffsetvsTDPTable[Index][0]) {
      FoundinTBL = TRUE;
      GetValue = mPMAXOffsetvsTDPTable[Index][1];
      //DEBUG ( (EFI_D_ERROR, "\nFound exactly value in PMaxOffset table Index%d: %d, %d\n\n", Index, data, GetValue));
      break;
    }

    if ((data > (UINT32) mPMAXOffsetvsTDPTable[Index - 1][0]) && (data < (UINT32) mPMAXOffsetvsTDPTable[Index][0]) && (Index > 0)) {
      FoundinTBL = TRUE;
      GetValue = mPMAXOffsetvsTDPTable[Index - 1][1];
      if ((ppm->Setup->PmaxConfig & USER_PMAX_VALUE_BIT_MASK) > 0) {
        DEBUG ( (EFI_D_ERROR, "\nNOTE: Found TDP [%d] closer value in PMaxOffset table %d: %d, %d on socket%d!!!\n\n", data, Index, mPMAXOffsetvsTDPTable[Index - 1][0], GetValue, SocketNumber) );
      }
      break;
    }
  }

  if ((!FoundinTBL) && ((ppm->Setup->PmaxConfig & USER_PMAX_VALUE_BIT_MASK) > 0)) {
    DEBUG ( (EFI_D_ERROR, " \n!!!Did not find the TDP [%d] in PMaxOffset table, forced Register value as %d on socket%d!!!\n\n", data, GetValue, SocketNumber) );
  }

  DEBUG((EFI_D_INFO, "PMAX Offset: Socket: %d :: Pmax Package TDP value: %dW, IccMax value: %dA, Table Offset Correction: %d\n", SocketNumber, data, IccMax, GetValue));

  return GetValue;
}


/**

  This routine is called to program PERF_P_LIMIT_CONTROL. It will be call multiple time passing any the SocketNumber to be programmed.

  @param PPMPolicy Pointer to PPM Policy protocol instance

  @retval EFI_SUCCESS

**/
VOID
ProgramCsrPmaxConfig (
    EFI_CPU_PM_STRUCT   *ppm
    )
{
  UINT32 data;
  UINT32 Tempdata;
  UINT64 RegAddr = 0; 
  UINT8  Size = 4;
  UINT8  SocketNumber;
  UINT8  Negative;
  EFI_CPU_CSR_ACCESS_PROTOCOL *CpuCsrAccess;

  SocketNumber = ppm->Info->SocketNumber;
  CpuCsrAccess = ppm->CpuCsrAccess;

  data = CpuCsrAccess->ReadCpuCsr(SocketNumber, 0, PMAX_CONFIG_PCU_FUN0_REG);
  data &= ~PCU_CR_PMAX_CFG_OFFSET;
#ifndef SKXD_EN
  if (ppm->Setup->PmaxConfig & USER_PMAX_USE_OFFSET_TABLE) {
    data |= (UINT32) GetFromPMaxOffsetTable (ppm);
  }
#endif

  //
  // If negative bit is set, Tempdata will be negative 2's complement value
  //
  Negative = ppm->Setup->PmaxConfig & USER_PMAX_NEGATIVE_BIT;
  Tempdata = (Negative ? 0 - (ppm->Setup->PmaxConfig & USER_PMAX_VALUE_BIT_MASK) : (ppm->Setup->PmaxConfig & USER_PMAX_VALUE_BIT_MASK));

  //
  // If we are adding positive value, make sure to cap at 15 (0x0F)
  //
  if (((Tempdata + (data & PCU_CR_PMAX_CFG_OFFSET)) > 15) && !Negative) {
    Tempdata = 15;
  } else {
    Tempdata += (data & PCU_CR_PMAX_CFG_OFFSET);
  }

  //
  // PMAX Config Register field is only 5 bits, need to mask
  //
  data = Tempdata & PCU_CR_PMAX_CFG_OFFSET;

  CpuCsrAccess->WriteCpuCsr(SocketNumber, 0, PMAX_CONFIG_PCU_FUN0_REG, data);
  RegAddr = CpuCsrAccess->GetCpuCsrAddress(SocketNumber, 0, PMAX_CONFIG_PCU_FUN0_REG, &Size);
  S3BootScriptSaveMemWrite (S3BootScriptWidthUint32, RegAddr, 1, &data );
}
