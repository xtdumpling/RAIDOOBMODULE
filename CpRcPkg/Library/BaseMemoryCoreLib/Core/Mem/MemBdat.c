/*++
  This file contains an 'Intel Pre-EFI Module' and is licensed
  for Intel CPUs and Chipsets under the terms of your license
  agreement with Intel or your vendor.  This file may be
  modified by the user, subject to additional terms of the
  license agreement
--*/
 /*************************************************************************
 *
 * Memory Reference Code
 *
 * ESS - Enterprise Silicon Software
 *
 * INTEL CONFIDENTIAL
 *
   Copyright (c) 2006 - 2016, Intel Corporation.

   This source code and any documentation accompanying it ("Material") is furnished
   under license and may only be used or copied in accordance with the terms of that
   license.  No license, express or implied, by estoppel or otherwise, to any
   intellectual property rights is granted to you by disclosure or delivery of these
   Materials.  The Materials are subject to change without notice and should not be
   construed as a commitment by Intel Corporation to market, license, sell or support
   any product or technology.  Unless otherwise provided for in the license under which
   this Material is provided, the Material is provided AS IS, with no warranties of
   any kind, express or implied, including without limitation the implied warranties
   of fitness, merchantability, or non-infringement.  Except as expressly permitted by
   the license for the Material, neither Intel Corporation nor its suppliers assumes
   any responsibility for any errors or inaccuracies that may appear herein.  Except
   as expressly permitted by the license for the Material, no part of the Material
   may be reproduced, stored in a retrieval system, transmitted in any form, or
   distributed by any means without the express written consent of Intel Corporation.
 ************************************************************************
 *
 *  PURPOSE:
 *
 *      This file contains memory detection and initialization for
 *      IMC and DDR modules compliant with JEDEC specification.
 *
 ************************************************************************/

#include "SysFunc.h"

#ifdef  BDAT_SUPPORT

#include "bdat.h"

UINT32
FillBDATStructure (
  PSYSHOST host
  )
/*++

  Fills the Compatible BIOS Data Structure

  @param host:         Pointer to sysHost

  @retval SUCCESS

--*/
{
  UINT8                         socket;
  UINT8                         ch;
  UINT8                         dimm;
  UINT8                         rank;
  UINT8                         logRank;
  UINT8                         strobe;
  UINT8                         i;
#ifdef SERIAL_DBG_MSG
  UINT16                        spd;
#endif  // SERIAL_DBG_MSG
  char                          cg;
  UINT16                        ctlMinVal;
  UINT16                        ctlMaxVal;
  UINT16                        cmdMinVal = 127;
  UINT16                        cmdMaxVal = 0;
  INT16                         piDelay = 0;
  INT16                         clkDelay = 0;
#ifdef  LRDIMM_SUPPORT
  UINT8                         sb, vb;
#endif  //  LRDIMM_SUPPORT
  BDAT_CHANNEL_STRUCTURE        (*channelBDAT)[MAX_CH];
  BDAT_DIMM_STRUCTURE           (*dimmBDAT)[MAX_DIMM];
  BDAT_RANK_STRUCTURE           (*rankBDAT)[MAX_RANK_DIMM];
  BDAT_RMT_CHANNEL_STRUCTURE    (*channelBDATRMT)[MAX_CH];
  BDAT_RMT_DIMM_STRUCTURE       (*dimmBDATRMT)[MAX_DIMM];
  BDAT_RMT_RANK_STRUCTURE       (*rankBDATRMT)[MAX_RANK_DIMM];
  struct channelNvram           (*channelNvList)[MAX_CH];
  struct dimmNvram              (*dimmNvList)[MAX_DIMM];
  struct ddrRank                (*rankList)[MAX_RANK_DIMM];
  UINT32                        csrReg;
  EFI_GUID                      bdatMemorySchemaGuid = BDAT_MEMORY_DATA_4B_GUID;
  EFI_GUID                      bdatRMTSchemaGuid    = RMT_SCHEMA_5_GUID;
  UINT32                        RcRevision = 0;

  if (!host->setup.common.bdatEn) return SUCCESS;

  for (socket = 0; socket < MAX_SOCKET; socket++) {
    if ((host->nvram.mem.socket[socket].enabled == 0) || (host->nvram.mem.socket[socket].maxDimmPop == 0)) continue;

    channelNvList = GetChannelNvList(host, socket);
    channelBDAT = &host->bdat.socketList[socket].channelList;
    channelBDATRMT = &host->bdatRmt.socketList[socket].channelList;

    //
    // Indicate this socket is enabled on both schema structs
    //
    host->bdat.socketList[socket].imcEnabled = 1;
    host->bdatRmt.socketList[socket].imcEnabled = 1;
    //
    // DDR frequency in MT/s
    //
    host->bdat.socketList[socket].ddrFreq = host->nvram.mem.socket[socket].ddrFreqMHz * 2;

    //
    // DDR voltage in mV
    //
    host->bdat.socketList[socket].ddrVoltage = host->nvram.mem.socket[socket].ddrVoltage;

    //
    // IMC device ID
    //
    host->bdat.socketList[socket].imcDid = (UINT16)host->nvram.mem.socket[socket].imcDid;

    //
    // IMC revision ID
    //
    host->bdat.socketList[socket].imcRid = host->var.common.stepping;

    //
    //                        (1/64)* QClock * 2048      (1/64)* QClock * 2048       2048
    //   Indicate piStep  =  ----------------------   =  ----------------------  = -------- = 16
    //                                 tCK                       2*QClock             128
    //
    host->bdat.socketList[socket].piStep  = (UINT8)16;

    //
    //                            step * 100       7.81 * 100
    //   Indicate rxVrefStep  =  ------------ =  ------------ = 520
    //                               Vdd             1.5
    //
    host->bdat.socketList[socket].rxVrefStep  = (UINT16) 520;

    //
    //                            step * 100       8 * 100
    //   Indicate txVrefStep  =  ------------ =  ------------ = 533
    //                               Vdd             1.5
    //
    host->bdat.socketList[socket].txVrefStep  = (UINT16) 533;

    //
    //                            step * 100       8 * 100
    //   Indicate caVrefStep  =  ------------ =  ------------ = 533         ???
    //                               Vdd             1.5
    //
    //   caVrefStep  =  caVrefStep * Vdd / 100
    //
//    host->bdat.socketList[socket].caVrefStep  = (UINT16) 533;
    //
    //                         recvenStep * tCK       recvenStep * 2*QClock
    //   Indicate recvenStep  =  ------------   =        ------------           =  ???
    //                               2048                    2048
    //  recvenStep;   // Step unit = recvenStep * tCK / 2048
//    host->bdat.socketList[socket].recvenStep  = (UINT8) 33;
    //
    //                         wrLevStep * tCK       wrLevStep * 2*QClock
    //   Indicate wrLevStep  =  ------------   =        ------------           =  ???
    //                               2048                    2048
    //  recvenStep;   // Step unit = recvenStep * tCK / 2048
 //   host->bdat.socketList[socket].wrLevStep  = (UINT8) 33;

    for (ch = 0; ch < MAX_CH; ch++) {
      //
      // Skip if channel is disabled
      //
      if ((*channelNvList)[ch].enabled == 0) continue;

      //
      // Indicate this channel is enabled on both schema structs
      //
      (*channelBDAT)[ch].chEnabled = 1;
      (*channelBDATRMT)[ch].chEnabled = 1;

      //
      // Indicate the number of DIMM slots available on this channel
      //
      (*channelBDAT)[ch].numDimmSlot = host->var.mem.socket[socket].channelList[ch].numDimmSlots;

      dimmNvList  = GetDimmNvList(host, socket, ch);
      dimmBDAT = &host->bdat.socketList[socket].channelList[ch].dimmList;
      dimmBDATRMT= &host->bdatRmt.socketList[socket].channelList[ch].dimmList;

      for (dimm = 0; dimm < MAX_DIMM; dimm++) {
        //
        // Skip if no DIMM present
        //
        if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

        //
        // Indicate this DIMM is enabled on both schema structs
        //
        (*dimmBDAT)[dimm].dimmEnabled = 1;
        (*dimmBDATRMT)[dimm].dimmEnabled = 1;

        //
        // SPD bytes
        //
        (*dimmBDAT)[dimm].spdBytes.valid[SPD_KEY_BYTE/8] |= 1 << (SPD_KEY_BYTE % 8);
        (*dimmBDAT)[dimm].spdBytes.spdData[SPD_KEY_BYTE] = (*dimmNvList)[dimm].keyByte;
        (*dimmBDAT)[dimm].spdBytes.valid[SPD_KEY_BYTE2/8] |= 1 << (SPD_KEY_BYTE2 % 8);
        (*dimmBDAT)[dimm].spdBytes.spdData[SPD_KEY_BYTE2] = (*dimmNvList)[dimm].actKeyByte2;
        (*dimmBDAT)[dimm].spdBytes.valid[SPD_SDRAM_BANKS/8] |= 1 << (SPD_SDRAM_BANKS % 8);
        (*dimmBDAT)[dimm].spdBytes.spdData[SPD_SDRAM_BANKS] = (*dimmNvList)[dimm].actSPDSDRAMBanks;
        (*dimmBDAT)[dimm].spdBytes.valid[SPD_SDRAM_ADDR/8] |= 1 << (SPD_SDRAM_ADDR % 8);
        (*dimmBDAT)[dimm].spdBytes.spdData[SPD_SDRAM_ADDR] = (*dimmNvList)[dimm].actSPDSdramAddr;
        (*dimmBDAT)[dimm].spdBytes.valid[SPD_MODULE_ORG/8] |= 1 << (SPD_MODULE_ORG % 8);
        (*dimmBDAT)[dimm].spdBytes.spdData[SPD_MODULE_ORG] = (*dimmNvList)[dimm].actSPDModuleOrg;
        (*dimmBDAT)[dimm].spdBytes.valid[SPD_MEM_BUS_WID/8] |= 1 << (SPD_MEM_BUS_WID % 8);
        (*dimmBDAT)[dimm].spdBytes.spdData[SPD_MEM_BUS_WID] = (*dimmNvList)[dimm].SPDMemBusWidth;
        (*dimmBDAT)[dimm].spdBytes.valid[SPD_MTB_DIVISOR/8] |= 1 << (SPD_MTB_DIVISOR % 8);
        (*dimmBDAT)[dimm].spdBytes.spdData[SPD_MTB_DIVISOR] = (*dimmNvList)[dimm].mtbDiv;
        (*dimmBDAT)[dimm].spdBytes.valid[SPD_MIN_TCK/8] |= 1 << (SPD_MIN_TCK % 8);
        //(*dimmBDAT)[dimm].spdBytes.spdData[SPD_MIN_TCK] = (*dimmNvList)[dimm].minTCK;
        (*dimmBDAT)[dimm].spdBytes.valid[SPD_RFSH_OPT/8] |= 1 << (SPD_RFSH_OPT % 8);
        (*dimmBDAT)[dimm].spdBytes.spdData[SPD_RFSH_OPT] = (*dimmNvList)[dimm].SPDThermRefsh;
        (*dimmBDAT)[dimm].spdBytes.valid[SPD_DIMM_TS/8] |= 1 << (SPD_DIMM_TS % 8);
        (*dimmBDAT)[dimm].spdBytes.spdData[SPD_DIMM_TS] = (*dimmNvList)[dimm].dimmTs;
        (*dimmBDAT)[dimm].spdBytes.valid[SPD_SDRAM_TYPE/8] |= 1 << (SPD_SDRAM_TYPE % 8);
        (*dimmBDAT)[dimm].spdBytes.spdData[SPD_SDRAM_TYPE] = (*dimmNvList)[dimm].sdramType;
        (*dimmBDAT)[dimm].spdBytes.valid[SPD_REF_RAW_CARD/8] |= 1 << (SPD_REF_RAW_CARD % 8);
        (*dimmBDAT)[dimm].spdBytes.spdData[SPD_REF_RAW_CARD] = (*dimmNvList)[dimm].SPDRawCard;
        (*dimmBDAT)[dimm].spdBytes.valid[SPD_MMID_LSB/8] |= 1 << (SPD_MMID_LSB % 8);
        (*dimmBDAT)[dimm].spdBytes.spdData[SPD_MMID_LSB] = (UINT8)((*dimmNvList)[dimm].SPDDramMfgId & 0xFF);
        (*dimmBDAT)[dimm].spdBytes.valid[SPD_MMID_MSB/8] |= 1 << (SPD_MMID_MSB % 8);
        (*dimmBDAT)[dimm].spdBytes.spdData[SPD_MMID_MSB] = (UINT8)((*dimmNvList)[dimm].SPDDramMfgId >> 8);

        //
        // Module part number
        //
        for (i = 0; i < SPD_MODULE_PART; i++) {
          (*dimmBDAT)[dimm].spdBytes.valid[(SPD_MODULE_PN + i)/8] |= 1 << ((SPD_MODULE_PN + i) % 8);
          (*dimmBDAT)[dimm].spdBytes.spdData[(SPD_MODULE_PN + i)] = (UINT8)((*dimmNvList)[dimm].SPDModPart[i]);
        }

        //
        // Get DIMM serial number from SPD 122 - 125.
        //
        for (i = 0; i < 4; i++) {
          (*dimmBDAT)[dimm].spdBytes.valid[(SPD_MODULE_SN + i)/8] |= 1 << ((SPD_MODULE_SN + i) % 8);
          (*dimmBDAT)[dimm].spdBytes.spdData[(SPD_MODULE_SN + i)] = (UINT8)((*dimmNvList)[dimm].SPDModSN[i]);
        }

        //
        // RDIMM specific bytes
        //
        if (host->nvram.mem.dimmTypePresent == RDIMM) {
#ifdef  LRDIMM_SUPPORT
          //
          // LRDIMM specific bytes
          //
          if (IsLrdimmPresent(host, socket, ch, dimm)) {
            (*dimmBDAT)[dimm].spdBytes.valid[SPD_LRDIMM_ATTR/8] |= 1 << (SPD_LRDIMM_ATTR % 8);
            (*dimmBDAT)[dimm].spdBytes.spdData[SPD_LRDIMM_ATTR] = (*dimmNvList)[dimm].dimmAttrib;
            (*dimmBDAT)[dimm].spdBytes.valid[SPD_LRBUF_VEN_MSB/8] |= 1 << (SPD_LRBUF_VEN_MSB % 8);
            (*dimmBDAT)[dimm].spdBytes.spdData[SPD_LRBUF_VEN_MSB] = (UINT8)((*dimmNvList)[dimm].SPDRegVen >> 8);
            (*dimmBDAT)[dimm].spdBytes.valid[SPD_LRBUF_VEN_LSB/8] |= 1 << (SPD_LRBUF_VEN_LSB % 8);
            (*dimmBDAT)[dimm].spdBytes.spdData[SPD_LRBUF_VEN_LSB] = (UINT8)((*dimmNvList)[dimm].SPDRegVen & ~BIT7);
            (*dimmBDAT)[dimm].spdBytes.valid[SPD_LRBUF_REV/8] |= 1 << (SPD_LRBUF_REV % 8);
            (*dimmBDAT)[dimm].spdBytes.spdData[SPD_LRBUF_REV] = (*dimmNvList)[dimm].SPDRegRev;
            (*dimmBDAT)[dimm].spdBytes.valid[SPD_LR_F0_RC2_3/8] |= 1 << (SPD_LR_F0_RC2_3 % 8);
            (*dimmBDAT)[dimm].spdBytes.spdData[SPD_LR_F0_RC2_3] = (*dimmNvList)[dimm].spdLrBuf_F0_RC2_3;
            (*dimmBDAT)[dimm].spdBytes.valid[SPD_LR_F0_RC4_5/8] |= 1 << (SPD_LR_F0_RC4_5 % 8);
            (*dimmBDAT)[dimm].spdBytes.spdData[SPD_LR_F0_RC4_5] = (*dimmNvList)[dimm].spdLrBuf_F0_RC4_5;
            (*dimmBDAT)[dimm].spdBytes.valid[SPD_LR_F1_RC8_11/8] |= 1 << (SPD_LR_F1_RC8_11 % 8);
            (*dimmBDAT)[dimm].spdBytes.spdData[SPD_LR_F1_RC8_11] = (*dimmNvList)[dimm].spdLrBuf_F1_RC8_11;
            (*dimmBDAT)[dimm].spdBytes.valid[SPD_LR_F1_RC12_13/8] |= 1 << (SPD_LR_F1_RC12_13 % 8);
            (*dimmBDAT)[dimm].spdBytes.spdData[SPD_LR_F1_RC12_13] = (*dimmNvList)[dimm].spdLrBuf_F1_RC12_13;
            (*dimmBDAT)[dimm].spdBytes.valid[SPD_LR_F1_RC14_15/8] |= 1 << (SPD_LR_F1_RC14_15 % 8);
            (*dimmBDAT)[dimm].spdBytes.spdData[SPD_LR_F1_RC14_15] = (*dimmNvList)[dimm].spdLrBuf_F1_RC14_15;
            // Index into set of bytes based on actual DDR frequency
            // set of 6 bytes
            sb = (host->nvram.mem.socket[socket].ddrFreq >> 1);
            if (sb > 2) sb = 2;
            sb = sb * 6;
            (*dimmBDAT)[dimm].spdBytes.valid[(SPD_LR_SB0_MDQ_DS_ODT + sb)/8] |= 1 << ((SPD_LR_SB0_MDQ_DS_ODT + sb) % 8);
            (*dimmBDAT)[dimm].spdBytes.spdData[(SPD_LR_SB0_MDQ_DS_ODT + sb)] = (*dimmNvList)[dimm].spdLrBuf_MDQ_DS_ODT;
            (*dimmBDAT)[dimm].spdBytes.valid[(SPD_LR_SB0_DR01_QODT_ACT + sb)/8] |= 1 << ((SPD_LR_SB0_DR01_QODT_ACT + sb) % 8);
            (*dimmBDAT)[dimm].spdBytes.spdData[(SPD_LR_SB0_DR01_QODT_ACT + sb)] = (*dimmNvList)[dimm].spdLrBuf_DRAM_QODT_ACT[0];
            (*dimmBDAT)[dimm].spdBytes.valid[(SPD_LR_SB0_DR23_QODT_ACT + sb)/8] |= 1 << ((SPD_LR_SB0_DR23_QODT_ACT + sb) % 8);
            (*dimmBDAT)[dimm].spdBytes.spdData[(SPD_LR_SB0_DR23_QODT_ACT + sb)] = (*dimmNvList)[dimm].spdLrBuf_DRAM_QODT_ACT[1];
            (*dimmBDAT)[dimm].spdBytes.valid[(SPD_LR_SB0_DR45_QODT_ACT + sb)/8] |= 1 << ((SPD_LR_SB0_DR45_QODT_ACT + sb) % 8);
            (*dimmBDAT)[dimm].spdBytes.spdData[(SPD_LR_SB0_DR45_QODT_ACT + sb)] = (*dimmNvList)[dimm].spdLrBuf_DRAM_QODT_ACT[2];
            (*dimmBDAT)[dimm].spdBytes.valid[(SPD_LR_SB0_DR67_QODT_ACT + sb)/8] |= 1 << ((SPD_LR_SB0_DR67_QODT_ACT + sb) % 8);
            (*dimmBDAT)[dimm].spdBytes.spdData[(SPD_LR_SB0_DR67_QODT_ACT + sb)] = (*dimmNvList)[dimm].spdLrBuf_DRAM_QODT_ACT[3];
            (*dimmBDAT)[dimm].spdBytes.valid[(SPD_LR_SB0_MR1_2_RTT + sb)/8] |= 1 << ((SPD_LR_SB0_MR1_2_RTT + sb) % 8);
            (*dimmBDAT)[dimm].spdBytes.spdData[(SPD_LR_SB0_MR1_2_RTT + sb)] = (*dimmNvList)[dimm].spdLrBuf_MR1_2_RTT;
            // Index into next set of bytes based on DDR Voltage
            // set of 2 bytes
            vb = host->nvram.mem.socket[socket].ddrVoltage >> 1;
            if (vb > 2) vb = 2;
            vb = vb * 2;
            (*dimmBDAT)[dimm].spdBytes.valid[(SPD_LR_150_MIN_MOD_DELAY + vb)/8] |= 1 << ((SPD_LR_150_MIN_MOD_DELAY + vb) % 8);
            (*dimmBDAT)[dimm].spdBytes.spdData[(SPD_LR_150_MIN_MOD_DELAY + vb)] = (*dimmNvList)[dimm].spdLrBuf_150_MIN_MOD_DELAY;
            (*dimmBDAT)[dimm].spdBytes.valid[(SPD_LR_150_MAX_MOD_DELAY + vb)/8] |= 1 << ((SPD_LR_150_MAX_MOD_DELAY + vb) % 8);
            (*dimmBDAT)[dimm].spdBytes.spdData[(SPD_LR_150_MAX_MOD_DELAY + vb)] = (*dimmNvList)[dimm].spdLrBuf_150_MAX_MOD_DELAY;
          } else
#endif  //  LRDIMM_SUPPORT
          {
            (*dimmBDAT)[dimm].spdBytes.valid[SPD_RDIMM_ATTR/8] |= 1 << (SPD_RDIMM_ATTR % 8);
            (*dimmBDAT)[dimm].spdBytes.spdData[SPD_RDIMM_ATTR] = (*dimmNvList)[dimm].dimmAttrib;
            (*dimmBDAT)[dimm].spdBytes.valid[SPD_DIMM_HS/8] |= 1 << (SPD_DIMM_HS % 8);
            (*dimmBDAT)[dimm].spdBytes.spdData[SPD_DIMM_HS] = (*dimmNvList)[dimm].dimmHs;
            (*dimmBDAT)[dimm].spdBytes.valid[SPD_REG_VEN_MSB/8] |= 1 << (SPD_REG_VEN_MSB % 8);
            (*dimmBDAT)[dimm].spdBytes.spdData[SPD_REG_VEN_MSB] = (UINT8)((*dimmNvList)[dimm].SPDRegVen >> 8);
            (*dimmBDAT)[dimm].spdBytes.valid[SPD_REG_VEN_LSB/8] |= 1 << (SPD_REG_VEN_LSB % 8);
            (*dimmBDAT)[dimm].spdBytes.spdData[SPD_REG_VEN_LSB] = (UINT8)(*dimmNvList)[dimm].SPDRegVen;
            (*dimmBDAT)[dimm].spdBytes.valid[SPD_REG_REV/8] |= 1 << (SPD_REG_REV % 8);
            (*dimmBDAT)[dimm].spdBytes.spdData[SPD_REG_REV] = (UINT8)(*dimmNvList)[dimm].SPDRegRev;
          }
        } else {
          (*dimmBDAT)[dimm].spdBytes.valid[SPD_ADDR_MAP_FECTD/8] |= 1 << (SPD_ADDR_MAP_FECTD % 8);
          (*dimmBDAT)[dimm].spdBytes.spdData[SPD_ADDR_MAP_FECTD] = (*dimmNvList)[dimm].SPDAddrMapp;
        }

        //Adding new SPD_BYTES for 256 to 512 as per version 4.0

        if (host->nvram.mem.dramType == SPD_TYPE_DDR4) {
          // Module Manufacturer ID Code
          (*dimmBDAT)[dimm].spdBytes.valid[SPD_MMID_LSB_DDR4/8] |= 1 << (SPD_MMID_LSB_DDR4 % 8);
          (*dimmBDAT)[dimm].spdBytes.spdData[SPD_MMID_LSB_DDR4] = (UINT8)((*dimmNvList)[dimm].SPDMMfgId & 0x00FF);
          (*dimmBDAT)[dimm].spdBytes.valid[SPD_MMID_MSB_DDR4/8] |= 1 << (SPD_MMID_MSB_DDR4 % 8);
          (*dimmBDAT)[dimm].spdBytes.spdData[SPD_MMID_MSB_DDR4] = (UINT8)((*dimmNvList)[dimm].SPDMMfgId >> 8);

           // Module Manufacturing Location
          (*dimmBDAT)[dimm].spdBytes.valid[SPD_MM_LOC_DDR4/8] |= 1 << (SPD_MM_LOC_DDR4 % 8);
          (*dimmBDAT)[dimm].spdBytes.spdData[SPD_MM_LOC_DDR4] = (*dimmNvList)[dimm].SPDMMfLoc;

          // Get module module date code from SPD 323-324.
          (*dimmBDAT)[dimm].spdBytes.valid[SPD_MM_DATE_YR_DDR4/8] |= 1 << (SPD_MM_DATE_YR_DDR4 % 8);
          (*dimmBDAT)[dimm].spdBytes.spdData[SPD_MM_DATE_YR_DDR4] = (UINT8)((*dimmNvList)[dimm].SPDModDate & 0x00FF);
          (*dimmBDAT)[dimm].spdBytes.valid[(SPD_MM_DATE_WK_DDR4)/8] |= 1 << ((SPD_MM_DATE_WK_DDR4) % 8);
          (*dimmBDAT)[dimm].spdBytes.spdData[(SPD_MM_DATE_WK_DDR4)] = (UINT8)((*dimmNvList)[dimm].SPDModDate >> 8);

          //Get module Serial Number from SPD 325-328
          for (i = 0; i < 4; i++) {
            (*dimmBDAT)[dimm].spdBytes.valid[(SPD_MODULE_SN_DDR4 + i)/8] |= 1 << ((SPD_MODULE_SN_DDR4 + i) % 8);
            (*dimmBDAT)[dimm].spdBytes.spdData[(SPD_MODULE_SN_DDR4 + i)] = (UINT8)((*dimmNvList)[dimm].SPDModSN[i]);
          }

          // Get module part number from SPD 329-348.
          for (i = 0; i < SPD_MODULE_PART_DDR4; i++) {
            (*dimmBDAT)[dimm].spdBytes.valid[(SPD_MODULE_PN_DDR4 + i)/8] |= 1 << ((SPD_MODULE_PN_DDR4 + i) % 8);
            (*dimmBDAT)[dimm].spdBytes.spdData[(SPD_MODULE_PN_DDR4 + i)] = (UINT8)((*dimmNvList)[dimm].SPDModPartDDR4[i]);
          }

          // Module Revision Code
          (*dimmBDAT)[dimm].spdBytes.valid[SPD_MODULE_RC_DDR4/8] |= 1 << (SPD_MODULE_RC_DDR4 % 8);
          (*dimmBDAT)[dimm].spdBytes.spdData[SPD_MODULE_RC_DDR4] = (*dimmNvList)[dimm].SPDModRevCode;

          // Get DRAM mfg id from SPD 350-351.
          (*dimmBDAT)[dimm].spdBytes.valid[SPD_DRAM_MIDC_LSB_DDR4/8] |= 1 << (SPD_DRAM_MIDC_LSB_DDR4 % 8);
          (*dimmBDAT)[dimm].spdBytes.spdData[SPD_DRAM_MIDC_LSB_DDR4] = (UINT8)((*dimmNvList)[dimm].SPDDramMfgId & 0x00FF);
          (*dimmBDAT)[dimm].spdBytes.valid[(SPD_DRAM_MIDC_MSB_DDR4 + 1)/8] |= 1 << ((SPD_DRAM_MIDC_MSB_DDR4 + 1) % 8);
          (*dimmBDAT)[dimm].spdBytes.spdData[(SPD_DRAM_MIDC_MSB_DDR4 + 1)] = (UINT8)((*dimmNvList)[dimm].SPDDramMfgId >> 8);

          // Get DRAM revision id from SPD 352.
          (*dimmBDAT)[dimm].spdBytes.valid[SPD_DRAM_REV_DDR4/8] |= 1 << (SPD_DRAM_REV_DDR4 % 8);
          (*dimmBDAT)[dimm].spdBytes.spdData[SPD_DRAM_REV_DDR4] = (*dimmNvList)[dimm].SPDDramRev;

          // Get MSB/LSB of 16-bit CRC from SPD 382-383
          (*dimmBDAT)[dimm].spdBytes.valid[SPD_CRC_LSB_DDR4/8] |= 1 << (SPD_CRC_LSB_DDR4 % 8);
          (*dimmBDAT)[dimm].spdBytes.spdData[SPD_CRC_LSB_DDR4] = (UINT8)((*dimmNvList)[dimm].SPDCrcDDR4 & 0x00FF);
          (*dimmBDAT)[dimm].spdBytes.valid[(SPD_CRC_MSB_DDR4 + 1)/8] |= 1 << ((SPD_CRC_MSB_DDR4 + 1) % 8);
          (*dimmBDAT)[dimm].spdBytes.spdData[(SPD_CRC_MSB_DDR4 + 1)] = (UINT8)((*dimmNvList)[dimm].SPDCrcDDR4 >> 8);
        } //if SPD_TYPE_DDR4

        rankList    = GetRankNvList(host, socket, ch, dimm);
        rankBDAT = &host->bdat.socketList[socket].channelList[ch].dimmList[dimm].rankList;
        rankBDATRMT = &host->bdatRmt.socketList[socket].channelList[ch].dimmList[dimm].rankList;

        for (rank = 0; rank < host->var.mem.socket[socket].maxRankDimm; rank++) {
          //
          // Skip if no rank or if it is mapped out
          //
          if (CheckRank(host, socket, ch, dimm, rank, CHECK_MAPOUT)) continue;

          //
          // Get the logical rank #
          //
          logRank = GetLogicalRank(host, socket, ch, dimm, rank);

          //
          // Indicate this DIMM is enabled on both structs
          //
          (*rankBDAT)[rank].rankEnabled = 1;
          (*rankBDATRMT)[rank].rankEnabled = 1;
          (*rankBDATRMT)[rank].rankMarginEnabled = 1;  //Enabled by default
          (*rankBDATRMT)[rank].dqMarginEnabled = 1;    //Enabled by default

          //
          // Get rank margins
          //
          (*rankBDATRMT)[rank].rankMargin.rxDqLeft = (*rankList)[rank].rxDQLeft;
          (*rankBDATRMT)[rank].rankMargin.rxDqRight = (*rankList)[rank].rxDQRight;
          (*rankBDATRMT)[rank].rankMargin.txDqLeft = (*rankList)[rank].txDQLeft;
          (*rankBDATRMT)[rank].rankMargin.txDqRight = (*rankList)[rank].txDQRight;
          (*rankBDATRMT)[rank].rankMargin.cmdLeft = (*rankList)[rank].cmdLeft;
          (*rankBDATRMT)[rank].rankMargin.cmdRight = (*rankList)[rank].cmdRight;
          (*rankBDATRMT)[rank].rankMargin.rxVrefLow = (*rankList)[rank].rxVrefLow;
          (*rankBDATRMT)[rank].rankMargin.rxVrefHigh = (*rankList)[rank].rxVrefHigh;
          (*rankBDATRMT)[rank].rankMargin.txVrefLow = (*rankList)[rank].txVrefLow;
          (*rankBDATRMT)[rank].rankMargin.txVrefHigh = (*rankList)[rank].txVrefHigh;
          (*rankBDATRMT)[rank].rankMargin.cmdVrefLow = 0;   //Not part of ddrRank as of now
          (*rankBDATRMT)[rank].rankMargin.cmdVrefHigh = 0;  //Not part of ddrRank as of now
          (*rankBDATRMT)[rank].rankMargin.ctlLeft = ~((*rankList)[rank].ctlLeft) + 1;  //absolute value needed in BDAT
          (*rankBDATRMT)[rank].rankMargin.ctlRight = (*rankList)[rank].ctlRight;

          //
          // Get training results
          //
          CHIP_FUNC_CALL(host, GetSetClkDelay (host, socket, ch, (*rankList)[rank].ckIndex, GSM_READ_ONLY, &clkDelay));
          (*rankBDAT)[rank].rankTraining.clkDelay = (UINT16)clkDelay;

          ctlMinVal = 256;
          ctlMaxVal = 0;
          CHIP_FUNC_CALL(host, GetSetCtlGroupDelay (host, socket, ch, (*rankList)[rank].ctlIndex + CtlGrp0, GSM_READ_ONLY, &piDelay, &ctlMinVal, &ctlMaxVal));
          (*rankBDAT)[rank].rankTraining.ctlDelay = ctlMinVal;

          for (cg = CmdGrp2; cg >= CmdGrp0; cg--) {
            cmdMinVal = 256;
            CHIP_FUNC_CALL(host, GetSetCmdGroupDelay (host, socket, ch, cg, GSM_READ_ONLY, &piDelay, &cmdMinVal, &cmdMaxVal));
            (*rankBDAT)[rank].rankTraining.cmdDelay[cg - CmdGrp0] = cmdMinVal;
          } // cg loop

          (*rankBDAT)[rank].rankTraining.ioLatency = CHIP_FUNC_CALL(host, GetIOLatency(host, socket, ch, logRank));
          (*rankBDAT)[rank].rankTraining.roundtrip = CHIP_FUNC_CALL(host, GetRoundTrip(host, socket, ch, logRank));

          for (strobe = 0; strobe < MAX_STROBE; strobe++) {
            //
            // Skip if this is an ECC strobe when ECC is disabled
            //
            if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;

            CHIP_FUNC_CALL(host, GetSetDataGroup (host, socket, ch, dimm, rank, strobe, 0, DdrLevel, RecEnDelay, GSM_READ_ONLY,
                             (INT16 *)&(*rankBDAT)[rank].rankTraining.recEnDelay[strobe]));
            CHIP_FUNC_CALL(host, GetSetDataGroup (host, socket, ch, dimm, rank, strobe, 0, DdrLevel, TxDqsDelay, GSM_READ_ONLY,
                             (INT16 *)&(*rankBDAT)[rank].rankTraining.wlDelay[strobe]));
            CHIP_FUNC_CALL(host, GetSetDataGroup (host, socket, ch, dimm, rank, strobe, 0, DdrLevel, RxDqsDelay, GSM_READ_ONLY,
                             (INT16 *)&(*rankBDAT)[rank].rankTraining.rxDqDelay[strobe]));
            CHIP_FUNC_CALL(host, GetSetDataGroup (host, socket, ch, dimm, rank, strobe, 0, DdrLevel, TxDqDelay, GSM_READ_ONLY,
                             (INT16 *)&(*rankBDAT)[rank].rankTraining.txDqDelay[strobe]));
            CHIP_FUNC_CALL(host, GetSetDataGroup (host, socket, ch, dimm, rank, strobe, 0, DdrLevel, TxVref, GSM_READ_ONLY,
                             (INT16 *)&(*rankBDAT)[rank].rankTraining.txvref[strobe]));
          } // strobe loop
        } // rank loop
      } // dimm loop
    } // ch loop

    RcRevision = host->var.common.rcVersion;
    //Memory Schema 4b
    host->bdat.refCodeRevision = RcRevision;  //Save reference code revision
    host->bdat.maxNode     = MAX_SOCKET;
    host->bdat.maxCh       = MAX_CH;
    host->bdat.maxDimm     = MAX_DIMM;
    host->bdat.maxRankDimm = MAX_RANK_DIMM;
    host->bdat.maxStrobe   = MAX_STROBE;
    //BDAT_SCHEMA_HEADER_STRUCTURE
    host->bdat.schemaHeader.SchemaId = bdatMemorySchemaGuid;
    host->bdat.schemaHeader.DataSize = sizeof(BDAT_MEMORY_DATA_STRUCTURE);
    host->bdat.schemaHeader.Crc16 = 0;
    host->bdat.schemaHeader.Crc16 = Crc16 ((char *)&host->bdat, sizeof (BDAT_MEMORY_DATA_STRUCTURE));

    //RMT Schema5
    host->bdatRmt.refCodeRevision  = RcRevision;  //Save reference code revision
    host->bdatRmt.maxNode          = MAX_SOCKET;
    host->bdatRmt.maxCh            = MAX_CH;
    host->bdatRmt.maxDimm          = MAX_DIMM;
    host->bdatRmt.maxRankDimm      = MAX_RANK_DIMM;
    host->bdatRmt.maxDq            = MAX_BITS;
    host->bdatRmt.marginLoopCount  = host->setup.mem.rmtPatternLength;
    //BDAT_SCHEMA_HEADER_STRUCTURE
    host->bdatRmt.schemaHeader.SchemaId = bdatRMTSchemaGuid;
    host->bdatRmt.schemaHeader.DataSize = sizeof(BDAT_RMT_STRUCTURE);
    host->bdatRmt.schemaHeader.Crc16 = 0;
    host->bdatRmt.schemaHeader.Crc16 = Crc16 ((char *)&host->bdat, sizeof (BDAT_RMT_STRUCTURE));

  //
  // Save BDAT to Scratchpad (Memory Schema 4b followed by RMT Schema 5)
  //
  csrReg = (UINT32)&host->bdat;
  WriteCpuPciCfgEx (host, 0, 0, SR_BDAT_STRUCT_PTR_CSR_ADDR(host), csrReg);

#ifdef SERIAL_DBG_MSG
    if (checkMsgLevel(host, SDBG_MAX)) {
      //
      // Print Compatible BIOS Memory Data Schema Structure
      //
      rcPrintf ((host, "\n Bdat Start:\n"));
      rcPrintf ((host, "\n struct host.bdat {\n"));
      rcPrintf ((host, " refCodeRevision          %02x.%02x.%02x\n", (host->bdat.refCodeRevision >> 24) & 0xff,
               (host->bdat.refCodeRevision >> 16) & 0xff, (host->bdat.refCodeRevision >> 8) & 0xff));

      rcPrintf ((host, " struct socketList[%d] {\n", socket));
      rcPrintf ((host, "  imcEnabled             %d\n", host->bdat.socketList[socket].imcEnabled));
      rcPrintf ((host, "  ddrFreq                %dMT/s\n", host->bdat.socketList[socket].ddrFreq));
      rcPrintf ((host, "  ddrVoltage             0x%x\n", host->bdat.socketList[socket].ddrVoltage));
      rcPrintf ((host, "  imcDid                 0x%x\n", host->bdat.socketList[socket].imcDid));
      rcPrintf ((host, "  imcRid                 %d\n", host->bdat.socketList[socket].imcRid));

      channelNvList = GetChannelNvList(host, socket);
      channelBDAT = &host->bdat.socketList[socket].channelList;
      for (ch = 0; ch < MAX_CH; ch++) {
        if ((*channelNvList)[ch].enabled == 0) continue;
        rcPrintf ((host, "    struct channelList[%d] {\n", ch));
        rcPrintf ((host, "      chEnabled            %d\n", (*channelBDAT)[ch].chEnabled));
        rcPrintf ((host, "      numDimmSlot          %d\n", (*channelBDAT)[ch].numDimmSlot));

        dimmNvList = GetDimmNvList(host, socket, ch);
        dimmBDAT = &host->bdat.socketList[socket].channelList[ch].dimmList;
        for (dimm = 0; dimm < MAX_DIMM; dimm++) {
          if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
          rcPrintf ((host, "      struct dimmList[%d] {\n", dimm));
          rcPrintf ((host, "      dimmEnabled          %d\n", (*dimmBDAT)[dimm].dimmEnabled));

          rankList = GetRankNvList(host, socket, ch, dimm);
          rankBDAT = &host->bdat.socketList[socket].channelList[ch].dimmList[dimm].rankList;
          for (rank = 0; rank < host->var.mem.socket[socket].maxRankDimm; rank++) {
            if (CheckRank(host, socket, ch, dimm, rank, CHECK_MAPOUT)) continue;

            rcPrintf ((host, "        struct rankList[%d] {\n", rank));
            rcPrintf ((host, "        rankEnabled        %d\n", (*rankBDAT)[rank].rankEnabled));

            rcPrintf ((host, "          struct rankTraining {\n"));
            for (strobe = 0; strobe < MAX_STROBE; strobe++) {
              //
              // Skip if this is an ECC strobe when ECC is disabled
              //
              if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;
              rcPrintf ((host, "            recEnDelay[%d]  %d\n", strobe, (*rankBDAT)[rank].rankTraining.recEnDelay[strobe]));
            } // strobe loop
            for (strobe = 0; strobe < MAX_STROBE; strobe++) {
              //
              // Skip if this is an ECC strobe when ECC is disabled
              //
              if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;
              rcPrintf ((host, "            wlDelay[%d]     %d\n", strobe, (*rankBDAT)[rank].rankTraining.wlDelay[strobe]));
            } // strobe loop
            for (strobe = 0; strobe < MAX_STROBE; strobe++) {
              //
              // Skip if this is an ECC strobe when ECC is disabled
              //
              if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;
              rcPrintf ((host, "            rxDqDelay[%d]   %d\n", strobe, (*rankBDAT)[rank].rankTraining.rxDqDelay[strobe]));
            } // strobe loop
            for (strobe = 0; strobe < MAX_STROBE; strobe++) {
              //
              // Skip if this is an ECC strobe when ECC is disabled
              //
              if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;
              rcPrintf ((host, "            txDqDelay[%d]   %d\n", strobe, (*rankBDAT)[rank].rankTraining.txDqDelay[strobe]));
            } // strobe loop
            for (strobe = 0; strobe < MAX_STROBE; strobe++) {
              //
              // Skip if this is an ECC strobe when ECC is disabled
              //
              if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;
              rcPrintf ((host, "            txvref ValuePerStrobe[%d]   %d\n", strobe, (*rankBDAT)[rank].rankTraining.txvref[strobe]));
            } // strobe loop

            rcPrintf ((host, "            clkDelay       %d\n", (*rankBDAT)[rank].rankTraining.clkDelay));
            rcPrintf ((host, "            ctlDelay       %d\n", (*rankBDAT)[rank].rankTraining.ctlDelay));
            rcPrintf ((host, "            cmdDelay[0]    %d\n", (*rankBDAT)[rank].rankTraining.cmdDelay[0]));
            rcPrintf ((host, "            cmdDelay[1]    %d\n", (*rankBDAT)[rank].rankTraining.cmdDelay[1]));
            rcPrintf ((host, "            cmdDelay[2]    %d\n", (*rankBDAT)[rank].rankTraining.cmdDelay[2]));
            rcPrintf ((host, "            ioLatency      %d\n", (*rankBDAT)[rank].rankTraining.ioLatency));
            rcPrintf ((host, "            roundtrip      %d\n", (*rankBDAT)[rank].rankTraining.roundtrip));
            rcPrintf ((host, "          } // rankTraining\n"));
            rcPrintf ((host, "          }\n"));
            rcPrintf ((host, "        } // rankList\n"));
          } // rank loop
          rcPrintf ((host, "        struct spdBytes {\n"));
          for (spd = 0; spd < MAX_SPD_BYTE; spd++) {
            if ((*dimmBDAT)[dimm].spdBytes.valid[spd/8] & (1 << (spd % 8))) {
              rcPrintf ((host, "          spdData[%d]        %d\n", spd, (*dimmBDAT)[dimm].spdBytes.spdData[spd]));
            }
          } // spd loop
          rcPrintf ((host, "        } // spdBytes\n"));
          rcPrintf ((host, "      } // dimmList\n"));
        } // dimm loop
        rcPrintf ((host, "    } // channelList\n"));
      } // ch loop

      //
      // Print Compatible BIOS RMT Schema Data Structure
      //
      rcPrintf ((host, "\nstruct host.bdatRmt {\n"));
      rcPrintf ((host, " refCodeRevision          %02x.%02x.%02x\n", (host->bdatRmt.refCodeRevision >> 24) & 0xff,
               (host->bdatRmt.refCodeRevision >> 16) & 0xff, (host->bdatRmt.refCodeRevision >> 8) & 0xff));
      rcPrintf ((host, " marginLoopCount          %d\n", host->bdatRmt.marginLoopCount));

      rcPrintf ((host, " struct socketList[%d] {\n", socket));
      rcPrintf ((host, "  imcEnabled             %d\n", host->bdatRmt.socketList[socket].imcEnabled));

      channelNvList = GetChannelNvList(host, socket);
      channelBDATRMT = &host->bdatRmt.socketList[socket].channelList;
      for (ch = 0; ch < MAX_CH; ch++) {
        if ((*channelNvList)[ch].enabled == 0) continue;
        rcPrintf ((host, "    struct channelList[%d] {\n", ch));
        rcPrintf ((host, "      chEnabled            %d\n", (*channelBDATRMT)[ch].chEnabled));
        rcPrintf ((host, "      numDimmSlot          %d\n", (*channelBDATRMT)[ch].numDimmSlot));

        dimmNvList = GetDimmNvList(host, socket, ch);
        dimmBDATRMT = &host->bdatRmt.socketList[socket].channelList[ch].dimmList;
        for (dimm = 0; dimm < MAX_DIMM; dimm++) {
          if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
          rcPrintf ((host, "      struct dimmList[%d] {\n", dimm));
          rcPrintf ((host, "      dimmEnabled          %d\n", (*dimmBDATRMT)[dimm].dimmEnabled));

          rankList = GetRankNvList(host, socket, ch, dimm);
          rankBDATRMT = &host->bdatRmt.socketList[socket].channelList[ch].dimmList[dimm].rankList;
          for (rank = 0; rank < host->var.mem.socket[socket].maxRankDimm; rank++) {
            if (CheckRank(host, socket, ch, dimm, rank, CHECK_MAPOUT)) continue;

            rcPrintf ((host, "        struct rankList[%d] {\n", rank));
            rcPrintf ((host, "        rankEnabled        %d\n", (*rankBDATRMT)[rank].rankEnabled));
            rcPrintf ((host, "          struct rankMargin {\n"));
            rcPrintf ((host, "            rxDqLeft       %d\n", (*rankBDATRMT)[rank].rankMargin.rxDqLeft));
            rcPrintf ((host, "            rxDqRight      %d\n", (*rankBDATRMT)[rank].rankMargin.rxDqRight));
            rcPrintf ((host, "            txDqLeft       %d\n", (*rankBDATRMT)[rank].rankMargin.txDqLeft));
            rcPrintf ((host, "            txDqRight      %d\n", (*rankBDATRMT)[rank].rankMargin.txDqRight));
            rcPrintf ((host, "            cmdLeft        %d\n", (*rankBDATRMT)[rank].rankMargin.cmdLeft));
            rcPrintf ((host, "            cmdRight       %d\n", (*rankBDATRMT)[rank].rankMargin.cmdRight));
            rcPrintf ((host, "            rxVrefLow      %d\n", (*rankBDATRMT)[rank].rankMargin.rxVrefLow));
            rcPrintf ((host, "            rxVrefHigh     %d\n", (*rankBDATRMT)[rank].rankMargin.rxVrefHigh));
            rcPrintf ((host, "            txVrefLow      %d\n", (*rankBDATRMT)[rank].rankMargin.txVrefLow));
            rcPrintf ((host, "            txVrefHigh     %d\n", (*rankBDATRMT)[rank].rankMargin.txVrefHigh));
            rcPrintf ((host, "          } // rankMargin\n"));
            rcPrintf ((host, "          struct rankTraining {\n"));
            rcPrintf ((host, "          }\n"));
            rcPrintf ((host, "        } // rankList\n"));
          } // rank loop
          rcPrintf ((host, "      } // dimmList\n"));
        } // dimm loop
        rcPrintf ((host, "    } // channelList\n"));
      } // ch loop

      rcPrintf ((host, "  } // socket\n"));

      rcPrintf ((host, "\n Bdat End\n"));
    } //checkMsgLevel

#endif  // SERIAL_DBG_MSG

  } // socket loop

  return SUCCESS;
} // FillBDATStructure

void
CopyPerBitMargins (
  PSYSHOST          host,
  UINT8             socket,
  struct bitMargin  *resultsBit,
  GSM_GT            group
  )
/*++

  copy per bit margin results

  @param host  - Pointer to sysHost

  @retval N/A

--*/
{
  UINT8                        ch;
  UINT8                        dimm;
  UINT8                        rank;
  UINT8                        bit;
  BDAT_RMT_RANK_STRUCTURE      (*rankBDATRMT)[MAX_RANK_DIMM];
  struct channelNvram          (*channelNvList)[MAX_CH];
  struct dimmNvram             (*dimmNvList)[MAX_DIMM];
  struct ddrRank               (*rankList)[MAX_RANK_DIMM];

  channelNvList = GetChannelNvList(host, socket);

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

    for (dimm = 0; dimm < MAX_DIMM; dimm++) {

      dimmNvList = GetDimmNvList(host, socket, ch);
      if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

      rankBDATRMT = &host->bdatRmt.socketList[socket].channelList[ch].dimmList[dimm].rankList;
      rankList = GetRankNvList(host, socket, ch, dimm);

      for (rank = 0; rank < host->var.mem.socket[socket].maxRankDimm; rank++) {
        if (CheckRank(host, socket, ch, dimm, rank, CHECK_MAPOUT)) continue;

        (*rankBDATRMT)[rank].dqMarginEnabled          = 0;

        if (group == RxDqsDelay) {
          for (bit = 0; bit < MAX_BITS; bit++) {
            (*rankBDATRMT)[rank].dqMargin[bit].rxDqLeft  = 0;
            (*rankBDATRMT)[rank].dqMargin[bit].rxDqRight = 0;

            if ((host->nvram.mem.eccEn == 0) && (bit > 63)) continue;
            (*rankBDATRMT)[rank].dqMargin[bit].rxDqLeft   = (UINT8)resultsBit->bits[ch][(*rankList)[rank].rankIndex][bit].n;
            (*rankBDATRMT)[rank].dqMargin[bit].rxDqRight  = (UINT8)resultsBit->bits[ch][(*rankList)[rank].rankIndex][bit].p;
          }
        } else if (group == TxDqsDelay) {
          for (bit = 0; bit < MAX_BITS; bit++) {
            (*rankBDATRMT)[rank].dqMargin[bit].txDqLeft   = 0;
            (*rankBDATRMT)[rank].dqMargin[bit].txDqRight  = 0;

            if ((host->nvram.mem.eccEn == 0) && (bit > 63)) continue;
            (*rankBDATRMT)[rank].dqMargin[bit].txDqLeft   = (UINT8)resultsBit->bits[ch][(*rankList)[rank].rankIndex][bit].n;
            (*rankBDATRMT)[rank].dqMargin[bit].txDqRight  = (UINT8)resultsBit->bits[ch][(*rankList)[rank].rankIndex][bit].p;
          }
        } else if (group == RxVref) {
          for (bit = 0; bit < MAX_BITS; bit++) {
            (*rankBDATRMT)[rank].dqMargin[bit].rxVrefLow  = 0;
            (*rankBDATRMT)[rank].dqMargin[bit].rxVrefHigh = 0;

            if ((host->nvram.mem.eccEn == 0) && (bit > 63)) continue;
            (*rankBDATRMT)[rank].dqMargin[bit].rxVrefLow  = (UINT8)resultsBit->bits[ch][(*rankList)[rank].rankIndex][bit].n;
            (*rankBDATRMT)[rank].dqMargin[bit].rxVrefHigh = (UINT8)resultsBit->bits[ch][(*rankList)[rank].rankIndex][bit].p;
          }
        } else if (group == TxVref) {
          for (bit = 0; bit < MAX_BITS; bit++) {
            (*rankBDATRMT)[rank].dqMargin[bit].txVrefLow  = 0;
            (*rankBDATRMT)[rank].dqMargin[bit].txVrefHigh = 0;

            if ((host->nvram.mem.eccEn == 0) && (bit > 63)) continue;
            (*rankBDATRMT)[rank].dqMargin[bit].txVrefLow  = (UINT8)resultsBit->bits[ch][(*rankList)[rank].rankIndex][bit].n;
            (*rankBDATRMT)[rank].dqMargin[bit].txVrefHigh = (UINT8)resultsBit->bits[ch][(*rankList)[rank].rankIndex][bit].p;
          }
        } //end if Group
      } // rank loop
    } // dimm loop
  } // ch loop

}//End CopyPerBitMargins


void
CopyBDATPerBitMargins (
  PSYSHOST          host
  )
/*++

  copy per bit margin results to BDAT

  @param host  - Pointer to sysHost

  @retval N/A

--*/
{

  UINT8               socket;
  struct bitMargin    resultsBit;
  struct bitMask      filter;

  for (socket = 0; socket < MAX_SOCKET; socket++) {
    if (host->setup.mem.optionsExt & PER_BIT_MARGINS) {

      GetMargins (host, socket, DdrLevel, RxDqsDelay, MODE_VIC_AGG, SCOPE_BIT, &filter, (void *)&resultsBit,
                host->setup.mem.rmtPatternLength, 0, 0, 0, WDB_BURST_LENGTH);

      CopyPerBitMargins (host, socket, &resultsBit, RxDqsDelay);


      GetMargins (host, socket, DdrLevel, TxDqDelay, MODE_VIC_AGG, SCOPE_BIT, &filter, (void *)&resultsBit,
                host->setup.mem.rmtPatternLength, 0, 0, 0, WDB_BURST_LENGTH);

      CopyPerBitMargins (host, socket, &resultsBit, TxDqDelay);


      GetMargins (host, socket, DdrLevel, RxVref, MODE_VIC_AGG, SCOPE_BIT, &filter, (void *)&resultsBit,
                host->setup.mem.rmtPatternLength, 0, 0, 0, WDB_BURST_LENGTH);

      CopyPerBitMargins (host, socket, &resultsBit, RxVref);


      GetMargins (host, socket, DdrLevel, TxVref, MODE_VIC_AGG, SCOPE_BIT, &filter, (void *)&resultsBit,
                host->setup.mem.rmtPatternLength, 0, 0, 0, WDB_BURST_LENGTH);

      CopyPerBitMargins (host, socket, &resultsBit, TxVref);

    } // if per bit margins
  } //socket loop
} // CopyBDATPerBitMargins

#endif  //  BDAT_SUPPORT

