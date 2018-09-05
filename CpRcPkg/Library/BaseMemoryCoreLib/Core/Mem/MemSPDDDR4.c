/**
  This file contains an 'Intel Pre-EFI Module' and is licensed
  for Intel CPUs and Chipsets under the terms of your license
  agreement with Intel or your vendor.  This file may be
  modified by the user, subject to additional terms of the
  license agreement
**/
/*************************************************************************
 *
 * Memory Reference Code
 *
 * ESS - Enterprise Silicon Software
 *
 * INTEL CONFIDENTIAL
 *
 * Copyright 2006 - 2015 Intel Corporation All Rights Reserved.
 *
 * The source code contained or described herein and all documents
 * related to the source code ("Material") are owned by Intel
 * Corporation or its suppliers or licensors. Title to the Material
 * remains with Intel Corporation or its suppliers and licensors.
 * The Material contains trade secrets and proprietary and confidential
 * information of Intel or its suppliers and licensors. The Material
 * is protected by worldwide copyright and trade secret laws and treaty
 * provisions.  No part of the Material may be used, copied, reproduced,
 * modified, published, uploaded, posted, transmitted, distributed, or
 * disclosed in any way without Intel's prior express written permission.
 *
 * No license under any patent, copyright, trade secret or other
 * intellectual property right is granted to or conferred upon you
 * by disclosure or delivery of the Materials, either expressly,
 * by implication, inducement, estoppel or otherwise. Any license
 * under such intellectual property rights must be express and
 * approved by Intel in writing.
 ************************************************************************
 *
 *  PURPOSE:
 *
 *      This file contains memory detection and initialization for
 *      IMC and DDR3 modules compliant with JEDEC specification.
 *
 ************************************************************************/
#include "SysFunc.h"

extern  UINT8 primaryWidth[MAX_TECH]; // Primary SDRAM device width
extern  UINT8 rowBits[MAX_TECH];      // Number of row address bits
extern  UINT8 columnBits[MAX_TECH];   // Number of column address bits
extern  UINT8 internalBanks[MAX_TECH];// Number of internal SDRAM banks
extern  UINT8 primaryWidthDDR4[MAX_TECH];   // Primary SDRAM device width
extern  UINT8 rowBitsDDR4[MAX_TECH];        // Number of row address bits
extern  UINT8 columnBitsDDR4[MAX_TECH];     // Number of column address bits
extern  UINT8 bankGroupsDDR4[MAX_TECH];     // Number of internal SDRAM banks

//
// Local Prototypes
//
void   GetDimmTimeBaseDDR4(PSYSHOST host,UINT8 spdmtb,UINT8 spdftb,INT32 * mtb,INT32 * ftb);

/**
  Store the common DDR4 DIMM configuration in the host structure

  @param host   - Point to sysHost
  @param socket - CPU socket ID
  @param ch     - channell to be accessed
  @param dimm   - DIMM to be accessed

  @retval N/A

**/
void
GetCommonDDR4DIMMConfig(
  PSYSHOST         host,
  UINT8            socket,
  UINT8            ch,
  UINT8            dimm
)
{
  UINT8  SPDReg;
  INT32  mediumTimebase;
  INT32  fineTimebase;
  struct dimmDevice   (*chdimmList)[MAX_DIMM];
  struct dimmNvram    (*dimmNvList)[MAX_DIMM];

  chdimmList    = GetChDimmList(host, socket, ch);
  dimmNvList    = GetDimmNvList(host, socket, ch);

  // FTB
  ReadSpd (host, socket, ch, dimm, SPD_TB_DDR4, &SPDReg);
  SPDReg = SPDReg & 0x0F; //mask out reserved bits 7-4
  switch (SPDReg & 3) {
    case 0:
      (*dimmNvList)[dimm].SPDftb = 1;    // 1 picosecond
      break;
  }

  // MTB
  switch ((SPDReg >> 2) & 3) {
    case 0:
      (*dimmNvList)[dimm].SPDmtb = 125;    //125 picoseconds
      break;
  }

  //
  // Get Min TCK
  //
  ReadSpd (host, socket, ch, dimm, SPD_MIN_TCK_DDR4, &SPDReg);
  (*dimmNvList)[dimm].minTCK = SPDReg;

  //
  // Get Max TCK
  //
  ReadSpd (host, socket, ch, dimm, SPD_MAX_TCK_DDR4, &SPDReg);
  (*dimmNvList)[dimm].maxTCK = SPDReg;

  //
  // Get FTB Min TCK
  //
  ReadSpd (host, socket, ch, dimm, SPD_FTB_MIN_TCK_DDR4, &SPDReg);
  (*dimmNvList)[dimm].ftbTCK = SPDReg;

  //
  // Get FTB Max TCK
  //
  ReadSpd (host, socket, ch, dimm, SPD_FTB_MAX_TCK_DDR4, &SPDReg);
  (*dimmNvList)[dimm].maxftbTCK = SPDReg;

  GetDimmTimeBaseDDR4 (host, (*dimmNvList)[dimm].SPDmtb, (*dimmNvList)[dimm].SPDftb, &mediumTimebase, &fineTimebase);

  if ((*dimmNvList)[dimm].ftbTCK & 0x80) {
    (*dimmNvList)[dimm].minTCK = (mediumTimebase * (INT32) (*dimmNvList)[dimm].minTCK) +
      (fineTimebase * ((INT32) (*dimmNvList)[dimm].ftbTCK - 256));
  } else {
    (*dimmNvList)[dimm].minTCK = (mediumTimebase * (INT32) (*dimmNvList)[dimm].minTCK) +
      (fineTimebase * (INT32) (*dimmNvList)[dimm].ftbTCK);
  }
  (*chdimmList)[dimm].minTCK = (*dimmNvList)[dimm].minTCK;

  //
  // Get Vdd DIMM Support Info (Only support 1.2v for DDR4)
  //
  ReadSpd (host, socket, ch, dimm, SPD_VDD_DDR4, &SPDReg);
  SPDReg = SPDReg & 0X03; //mask out reserved bits 7-2
  if (SPDReg & SPD_VDD_120) {
    host->nvram.mem.socket[socket].ddrVoltage = SPD_VDD_120;
  } else {
    host->nvram.mem.socket[socket].channelList[ch].v120NotSupported = 1;
    host->nvram.mem.socket[socket].ddrVoltage = SPDReg;
  }

}


/**

  Detect DDR4 configuration

  @param host   - Point to sysHost
  @param socket - CPU socket ID
  @param ch     - channel to be accessed
  @param dimm   - DIMM to be accessed

  @retval SUCCESS

**/
UINT32
DetectDDR4DIMMConfig (
                     PSYSHOST         host,
                     UINT8            socket,
                     UINT8            ch,
                     UINT8            dimm
                  )
{
  UINT8  SPDReg;
  struct dimmNvram    (*dimmNvList)[MAX_DIMM];
  struct channelNvram (*channelNvList)[MAX_CH];

  dimmNvList    = GetDimmNvList(host, socket, ch);
  channelNvList = GetChannelNvList(host, socket);

  host->nvram.mem.dramType = SPD_TYPE_DDR4;

  GetCommonDDR4DIMMConfig (host, socket, ch, dimm);

  //
  // Read SPD Bytes containing DDR module info
  //
  ReadSpd(host, socket, ch, dimm, SPD_KEY_BYTE2, &SPDReg);
  (*dimmNvList)[dimm].keyByte2 = SPDReg & 0xF;
  (*dimmNvList)[dimm].actKeyByte2 = SPDReg & 0xF;

  if (SPDReg == SPD_ECC_SO_UDIMM_DDR4) {
    // Take SODIMM path, Width will be checked for ECC.
    (*dimmNvList)[dimm].keyByte2 = SPD_SODIMM;
    (*dimmNvList)[dimm].actKeyByte2 = SPD_SODIMM;
  }

  if (SPDReg == SPD_ECC_SO_RDIMM_DDR4) {
    // Take SODIMM path, Width will be checked for ECC.
    (*dimmNvList)[dimm].keyByte2 = SPD_RDIMM;
    (*dimmNvList)[dimm].actKeyByte2 = SPD_RDIMM;
  }

#ifdef LRDIMM_SUPPORT
  if ((SPDReg & 0xF) == SPD_LRDIMM_DDR4) {
    (*dimmNvList)[dimm].lrDimmPresent = 1;
    (*channelNvList)[ch].lrDimmPresent = 1;
    host->nvram.mem.socket[socket].lrDimmPresent = 1;

    // Take RDIMM path to minimize changes
    (*dimmNvList)[dimm].keyByte2 = SPD_RDIMM;
  }
#endif

  //
  // Get SDRAM Capacity
  //
  ReadSpd (host, socket, ch, dimm, SPD_SDRAM_BANKS, &SPDReg);
  (*dimmNvList)[dimm].sdramCapacity = SPDReg & 0xF;
  //
  // Get the number of banks
  //
  switch ((SPDReg >> 4) & 3) {
    case 0:
      (*dimmNvList)[dimm].numBanks = 4;
      break;
    case 1:
      (*dimmNvList)[dimm].numBanks = 8;
      break;
  }

  //
  // Get the number of bank groups
  //
  switch ((SPDReg >> 6) & 3) {
    case 0:
      (*dimmNvList)[dimm].numBankGroups = 0;
      break;
    case 1:
      (*dimmNvList)[dimm].numBankGroups = 2;
      break;
    case 2:
     (*dimmNvList)[dimm].numBankGroups = 4;
     break;
  }
  (*dimmNvList)[dimm].SPDSDRAMBanks = SPDReg;
  (*dimmNvList)[dimm].actSPDSDRAMBanks = SPDReg;

  //
  // Get Row and Column address
  //
  ReadSpd (host, socket, ch, dimm, SPD_SDRAM_ADDR, &SPDReg);

  //
  // Get number of row bits
  //
  (*dimmNvList)[dimm].numRowBits = ((SPDReg >> 3) & 7) + 12;

  //
  // Get number of column bits
  //
  (*dimmNvList)[dimm].numColBits = (SPDReg & 7) + 9;
  (*dimmNvList)[dimm].actSPDSdramAddr = SPDReg;

  //
  // Get Number of ranks and device width
  //
  ReadSpd (host, socket, ch, dimm, SPD_MODULE_ORG_DDR4, &SPDReg);
  SPDReg = SPDReg & 0X7F; //mask out reserved bits 7
  (*dimmNvList)[dimm].SPDModuleOrg = SPDReg;
  (*dimmNvList)[dimm].actSPDModuleOrg = SPDReg;


  //
  // Get Primary SDRAM Device Type
  //
  ReadSpd (host, socket, ch, dimm, SPD_SDRAM_TYPE_DDR4, &SPDReg);
  (*dimmNvList)[dimm].SPDDeviceType = SPDReg & 0xF3; //mask out reserved bits 3-2

  //
  // Get Secondary SDRAM Device Type
  //
  ReadSpd (host, socket, ch, dimm, SPD_SECONDARY_SDRAM_TYPE_DDR4, &SPDReg);
  (*dimmNvList)[dimm].SPDSecondaryDeviceType = SPDReg;

  //
  // Get Raw Card Number
  //
  ReadSpd (host, socket, ch, dimm, SPD_REF_RAW_CARD_DDR4, &SPDReg);
  (*dimmNvList)[dimm].SPDRawCard = SPDReg;
  //
  // Get Bus Width
  //
  ReadSpd (host, socket, ch, dimm, SPD_MEM_BUS_WID_DDR4, &SPDReg);
  SPDReg = SPDReg & 0x1F; //mask out reserved bits 7-5
  (*dimmNvList)[dimm].SPDMemBusWidth = SPDReg;

  return SUCCESS;
}

/**
  Calculate the medium and fine timebases, using integer math.

  @param mtb     - Location to store the calculated medium timebase, in femtoseconds.
                   Example: mtb = 125ps. ((125/1,000) * 1,000,000) = 125,000
  @param ftb     - Location to store the calculated fine timebase, in femtoseconds.
                   Example: ftb = 1ps.  (1 * 1,000) = 1,000

  @retval N/A

**/
void
GetDimmTimeBaseDDR4 (
                    PSYSHOST  host,
                    UINT8     spdmtb,
                    UINT8     spdftb,
                    INT32     * mtb,
                    INT32     * ftb
                    )
{
  if (spdftb > 0) {
    *ftb = spdftb * FREQUENCY_FTB_OFFSET;
  } else {
    *ftb = 0;
  }

  if (spdmtb > 0) {
    *mtb = spdmtb * (FREQUENCY_MTB_OFFSET/1000);
  } else {
    *mtb = 0;
  }
} // GetDimmTimeBaseDDR4

void
InitDDRVoltageDDR4(
              PSYSHOST  host,
              UINT8     socket
              )
{
  //
  // Stubbed out as this is not necessary for DDR4
  //
}


