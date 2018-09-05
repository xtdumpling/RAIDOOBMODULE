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


extern  UINT32 preciseTCK[MAX_SUP_FREQ];

//
// Local Prototypes
//
void   GetXMPTimings(PSYSHOST host);
void   GetXMPTimingsDDR4(PSYSHOST host);
UINT16 GetCycles (PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, UINT16 mtbOffset, UINT16 ftbOffset, UINT32 tCK);
UINT16 TimeBaseToDCLKtCK (PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, UINT16 mtbTime, UINT16 ftbTime, UINT32 tCK);




UINT32
WaitForConfigureXMP (
  PSYSHOST host
  )
/*++

  Wait for BSP to send AP continue handshake

  @param host  - Pointer to sysHost

  @retval N/A

--*/
{
  UINT8               handShakeByte;

  if ((host->nvram.mem.DataGood != 1) || (host->var.common.bootMode == NormalBoot)) {
    //
    // if MT MRC, slave processor to inform BSP GatherSPDData is complete
    //
    host->var.common.rcWriteRegDump = 0;
    SendPipePackage(host, host->var.common.socketId, &handShakeByte, 1);

    //
    // now wait for the data back from the BSP to indicate AP should continue
    //
    GetPipePackage(host, host->var.common.socketId, &handShakeByte, 1);

  } // S3 or Warm boot

  return SUCCESS;
} // WaitForConfigureXMP

UINT32
ConfigureXMP (
             PSYSHOST host
             )
/*++

  Checks if XMP is supported by all sockets

  @param host  - Pointer to sysHost

  @retval N/A

--*/
{
  UINT8   socket;
  UINT8               handShakeByte;
  UINT8   ch;
  UINT8   dimm;
  UINT8   SPDReg;
  UINT16  XMPId;
  struct dimmNvram (*dimmNvList)[MAX_DIMM];
  struct channelNvram (*channelNvList)[MAX_CH];

  if ((host->nvram.mem.DataGood != 1) || (host->var.common.bootMode == NormalBoot)) {
    //
    // if MT MRC, wait for slave processors to complete GatherSPDData
    //
    if (host->setup.mem.options & MULTI_THREAD_MRC_EN) {
      for (socket = 0; socket < MAX_SOCKET; socket++) {
        //
        // Continue if this socket is not present
        //
        if ((host->var.common.socketPresentBitMap & (BIT0 << socket)) == 0) continue;

        if (socket == host->var.common.socketId) continue;

        //MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
        //              "Wait for slave GatherSPDData complete (S%d)\n", socket));

        GetPipePackage(host, socket, &handShakeByte, 1);
      } // socket loop
    }

    // Loop for each CPU socket
    for (socket = 0; socket < MAX_SOCKET; socket++) {
      if (host->nvram.mem.socket[socket].enabled == 0 || (host->nvram.mem.socket[socket].maxDimmPop == 0)) continue;

      host->nvram.mem.XMPProfilesSup = 3;
      host->nvram.mem.XMPProfilesRevision = 0xFF;

      channelNvList = GetChannelNvList(host, socket);

      // Detect DIMMs on each channel
      for (ch = 0; ch < MAX_CH; ch++) {
        if ((*channelNvList)[ch].enabled == 0) continue;

        dimmNvList  = GetDimmNvList(host, socket, ch);

        // Detect DIMMs in each slot
        for (dimm = 0; dimm < host->var.mem.socket[socket].channelList[ch].numDimmSlots; dimm++) {

          OutputExtendedCheckpoint((host, STS_CLOCK_INIT, SUB_XMP_INIT, (UINT16)((socket << 8)|(ch << 4)|dimm)));

          if (host->nvram.mem.dramType == SPD_TYPE_DDR3) {
            //
            // Read SPD device to detect DIMM presence and check for XMP support
            //
            if (ReadSpd (host, socket, ch, dimm, SPD_XMP_ID, &SPDReg) == SUCCESS) {

              XMPId = (UINT16) (SPDReg << 8);

              ReadSpd (host, socket, ch, dimm, SPD_XMP_ID + 1, &SPDReg);
              XMPId |= (UINT16) SPDReg;

              //
              // Clear out the profiles supported if this DIMM does not support XMP
              //

              if (XMPId != XMP_ID) {
                host->nvram.mem.XMPProfilesSup = 0;
              } else {

                ReadSpd (host, socket, ch, dimm, SPD_XMP_REV, &(*dimmNvList)[dimm].XMPRev);
                if ( host->nvram.mem.XMPProfilesRevision > (*dimmNvList)[dimm].XMPRev ) {
                  host->nvram.mem.XMPProfilesRevision = (*dimmNvList)[dimm].XMPRev;
                  MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                              "XMP Revision: %d.%d\n",
                              ((*dimmNvList)[dimm].XMPRev >> 4) & 0xFF, (*dimmNvList)[dimm].XMPRev & 0xFF));
                }
                //
                // Get the profiles supported
                //
                ReadSpd (host, socket, ch, dimm, SPD_XMP_ORG, &(*dimmNvList)[dimm].XMPOrg);

                //
                // AND with the profiles supported by the other DIMMs
                //
                host->nvram.mem.XMPProfilesSup &= ((*dimmNvList)[dimm].XMPOrg & 3);
              }
            } else {
              host->nvram.mem.XMPProfilesSup = 0;
            }
          } else { //DDR4
            //
            // Read SPD device to detect DIMM presence and check for XMP support
            //
            if (ReadSpd (host, socket, ch, dimm, SPD_XMP_ID_DDR4, &SPDReg) == SUCCESS) {

              XMPId = (UINT16) (SPDReg << 8);

              ReadSpd (host, socket, ch, dimm, SPD_XMP_ID_DDR4 + 1, &SPDReg);
              XMPId |= (UINT16) SPDReg;

              //
              // Clear out the profiles supported if this DIMM does not support XMP
              //

              if (XMPId != XMP_ID) {
                host->nvram.mem.XMPProfilesSup = 0;
              } else {

                ReadSpd (host, socket, ch, dimm, SPD_XMP_REV_DDR4, &(*dimmNvList)[dimm].XMPRev);
                if ( host->nvram.mem.XMPProfilesRevision > (*dimmNvList)[dimm].XMPRev ) {
                  host->nvram.mem.XMPProfilesRevision = (*dimmNvList)[dimm].XMPRev;
                  MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                                 "XMP Revision: %d.%d\n",
                                 ((*dimmNvList)[dimm].XMPRev >> 4) & 0xFF, (*dimmNvList)[dimm].XMPRev & 0xFF));
                }
                //
                // Get the profiles supported
                //
                ReadSpd (host, socket, ch, dimm, SPD_XMP_ORG_DDR4, &(*dimmNvList)[dimm].XMPOrg);

                //
                // AND with the profiles supported by the other DIMMs
                //
                host->nvram.mem.XMPProfilesSup &= ((*dimmNvList)[dimm].XMPOrg & 3);
              }
            } else {
              host->nvram.mem.XMPProfilesSup = 0;
            }
          }
        } // dimm loop
      } // ch loop
    } // socket loop

#ifdef SERIAL_DBG_MSG
    if (checkMsgLevel(host, SDBG_MINMAX)) {
      if (host->nvram.mem.XMPProfilesSup) {
        rcPrintf ((host, "Supported XMP Profile(s):  "));
        if (host->nvram.mem.XMPProfilesSup & BIT0) {
          rcPrintf ((host, "1"));
        }

        if (host->nvram.mem.XMPProfilesSup & BIT1) {
          rcPrintf ((host, ", 2"));
        }

        rcPrintf ((host, "\n"));

        rcPrintf (( host, "Supported XMP Revision: %d.%d\n", (host->nvram.mem.XMPProfilesRevision & 0xF0) >> 4, host->nvram.mem.XMPProfilesRevision & 0x0F ));
      }
    }
#endif
    //
    // Collect the XMP timings if at least one XMP profile is supported
    //
    if (host->nvram.mem.XMPProfilesSup) {
      if (host->nvram.mem.dramType == SPD_TYPE_DDR3){
        GetXMPTimings (host);
      } else {
        GetXMPTimingsDDR4(host);
      }
    }

    if (host->setup.mem.options & MULTI_THREAD_MRC_EN) {
      //
      // In MT MRC mode, tell slave processors to continue
      //
      for (socket = 0; socket < MAX_SOCKET; socket++) {
        //
        // Continue if this socket is not present
        //
        if ((host->var.common.socketPresentBitMap & (BIT0 << socket)) == 0) continue;

        if (socket == host->var.common.socketId) continue;

        host->var.common.rcWriteRegDump = 0;
        SendPipePackage(host, socket, &handShakeByte, 1);
      } // socket loop
    }

  } // S3 or Warm boot

  return SUCCESS;
} // ConfigureXMP

/*++

  Gathers XMP timings for DDR3

  @param host  - Pointer to sysHost

  @retval N/A

--*/
void
GetXMPTimings (
              PSYSHOST host
              )
{
  UINT8               socket;
  UINT8               ch;
  UINT8               dimm;
  UINT8               profile;
  UINT8               SPDReg;
  UINT16              cycles;
  UINT8               xmpOffset;
  UINT8               minTCK;
  UINT32              casSup = 0;
  UINT16              tRC;
  UINT16              tRAS;
  UINT16              tRFC;
  UINT16              tFAW;
  UINT16              tempFreq;
  UINT16              tREFI;
  UINT32              checkSum[MAX_SOCKET][MAX_CH][MAX_DIMM];
  UINT32              tCK;
  struct dimmNvram    (*dimmNvList)[MAX_DIMM];
  struct channelNvram (*channelNvList)[MAX_CH];

  //
  // Check for two possible profiles
  //
  for (profile = 0; profile < 2; profile++) {
    //
    // Continue if this profile is not supported
    //
    if ((host->nvram.mem.XMPProfilesSup & (1 << profile)) == 0) continue;

    //
    // Initialize to large values
    //
    host->nvram.mem.profileMemTime[profile].ddrFreq = 0xFFFF;
    host->nvram.mem.profileMemTime[profile].casSup  = 0x0000FFFF;
    host->nvram.mem.profileMemTime[profile].tREFI   = 0x7FFF;

    //
    // Offset into SPD
    //
    xmpOffset     = profile * 35;

    // Loop for each CPU socket
    for (socket = 0; socket < MAX_SOCKET; socket++) {
      if (host->nvram.mem.socket[socket].enabled == 0) continue;

      channelNvList = GetChannelNvList(host, socket);

      //
      // Loop for each channel
      //
      for (ch = 0; ch < MAX_CH; ch++) {
        if ((*channelNvList)[ch].enabled == 0) continue;

        dimmNvList = GetDimmNvList(host, socket, ch);

        //
        // Loop for each DIMM
        //
        for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {
          if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

          checkSum[socket][ch][dimm]  = 0;

          if (!profile) {
            //
            // Get Divisor
            //
            ReadSpd (host, socket, ch, dimm, SPD_XMP_MTB_DIVISOR1, &SPDReg);
            (*dimmNvList)[dimm].mtbDiv  = SPDReg;
            checkSum[socket][ch][dimm]    = checkSum[socket][ch][dimm] + (UINT16) SPDReg;

            //
            // Get Dividend
            //
            ReadSpd (host, socket, ch, dimm, SPD_XMP_MTB_DIVEND1, &SPDReg);
            (*dimmNvList)[dimm].mtbDividend = SPDReg;
            checkSum[socket][ch][dimm]        = checkSum[socket][ch][dimm] + (UINT16) SPDReg;
          } else {
            if ( host->nvram.mem.XMPProfilesRevision > 0x10 ) {
              // XMP Revision 1.1 support only.

              //
              // Get Divisor
              //
              ReadSpd (host, socket, ch, dimm, SPD_XMP_MTB_DIVISOR2, &SPDReg);
              (*dimmNvList)[dimm].mtbDiv  = SPDReg;
              checkSum[socket][ch][dimm]    = checkSum[socket][ch][dimm] + (UINT16) SPDReg;
              //
              // Get Dividend
              //
              ReadSpd (host, socket, ch, dimm, SPD_XMP_MTB_DIVEND2, &SPDReg);
              (*dimmNvList)[dimm].mtbDividend = SPDReg;
              checkSum[socket][ch][dimm]        = checkSum[socket][ch][dimm] + (UINT16) SPDReg;
            }
          }
          //
          // Get Min TCK
          //
          ReadSpd (host, socket, ch, dimm, SPD_XMP_TCK + xmpOffset, &minTCK);
          checkSum[socket][ch][dimm] = checkSum[socket][ch][dimm] + (UINT16) minTCK;

          //
          // Calculate transfer rate
          //
          tempFreq = (UINT16) ((2000 * (*dimmNvList)[dimm].mtbDiv) / (minTCK * (*dimmNvList)[dimm].mtbDividend));

          //
          // Round up if needed
          //
          if (((((2000 * (*dimmNvList)[dimm].mtbDiv) * 10) / (minTCK * (*dimmNvList)[dimm].mtbDividend)) -
               (UINT32) (tempFreq * 10)) >= 5) {
            tempFreq++;
          }
          //
          // Save the frequency supported by all DIMMs
          //
          if (host->nvram.mem.profileMemTime[profile].ddrFreq > tempFreq) {
            host->nvram.mem.profileMemTime[profile].ddrFreq = tempFreq;
          }
        } // dimm loop
      } // ch loop
    } // socket loop

    //
    // Determine DCLK Ratio
    //
    if (host->nvram.mem.profileMemTime[profile].ddrFreq >= 4400) {
      host->nvram.mem.profileMemTime[profile].ddrFreqLimit = DDR_4400 + 1;
    } else if (host->nvram.mem.profileMemTime[profile].ddrFreq >= 4266) {
      host->nvram.mem.profileMemTime[profile].ddrFreqLimit = DDR_4266 + 1;
    } else if (host->nvram.mem.profileMemTime[profile].ddrFreq >= 4200) {
      host->nvram.mem.profileMemTime[profile].ddrFreqLimit = DDR_4200 + 1;
    } else if (host->nvram.mem.profileMemTime[profile].ddrFreq >= 4000) {
      host->nvram.mem.profileMemTime[profile].ddrFreqLimit = DDR_4000 + 1;
    } else if (host->nvram.mem.profileMemTime[profile].ddrFreq >= 3800) {
      host->nvram.mem.profileMemTime[profile].ddrFreqLimit = DDR_3800 + 1;
    } else if (host->nvram.mem.profileMemTime[profile].ddrFreq >= 3733) {
      host->nvram.mem.profileMemTime[profile].ddrFreqLimit = DDR_3733 + 1;
    } else if (host->nvram.mem.profileMemTime[profile].ddrFreq >= 3600) {
      host->nvram.mem.profileMemTime[profile].ddrFreqLimit = DDR_3600 + 1;
    } else if (host->nvram.mem.profileMemTime[profile].ddrFreq >= 3466) {
      host->nvram.mem.profileMemTime[profile].ddrFreqLimit = DDR_3466 + 1;
    } else if (host->nvram.mem.profileMemTime[profile].ddrFreq >= 3400) {
      host->nvram.mem.profileMemTime[profile].ddrFreqLimit = DDR_3400 + 1;
    } else if (host->nvram.mem.profileMemTime[profile].ddrFreq >= 3200) {
      host->nvram.mem.profileMemTime[profile].ddrFreqLimit = DDR_3200 + 1;
    } else if (host->nvram.mem.profileMemTime[profile].ddrFreq >= 3000) {
      host->nvram.mem.profileMemTime[profile].ddrFreqLimit = DDR_3000 + 1;
    } else if (host->nvram.mem.profileMemTime[profile].ddrFreq >= 2933) {
      host->nvram.mem.profileMemTime[profile].ddrFreqLimit = DDR_2933 + 1;
    } else if (host->nvram.mem.profileMemTime[profile].ddrFreq >= 2800) {
      host->nvram.mem.profileMemTime[profile].ddrFreqLimit = DDR_2800 + 1;
    } else if (host->nvram.mem.profileMemTime[profile].ddrFreq >= 2666) {
      host->nvram.mem.profileMemTime[profile].ddrFreqLimit = DDR_2666 + 1;
    } else if (host->nvram.mem.profileMemTime[profile].ddrFreq >= 2600) {
      host->nvram.mem.profileMemTime[profile].ddrFreqLimit = DDR_2600 + 1;
    } else if (host->nvram.mem.profileMemTime[profile].ddrFreq >= 2400) {
      host->nvram.mem.profileMemTime[profile].ddrFreqLimit = DDR_2400 + 1;
    } else if (host->nvram.mem.profileMemTime[profile].ddrFreq >= 2200) {
      host->nvram.mem.profileMemTime[profile].ddrFreqLimit = DDR_2200 + 1;
    } else if (host->nvram.mem.profileMemTime[profile].ddrFreq >= 2133) {
      host->nvram.mem.profileMemTime[profile].ddrFreqLimit = DDR_2133 + 1;
    } else if (host->nvram.mem.profileMemTime[profile].ddrFreq >= 2000) {
      host->nvram.mem.profileMemTime[profile].ddrFreqLimit = DDR_2000 + 1;
    } else if (host->nvram.mem.profileMemTime[profile].ddrFreq >= 1866) {
      host->nvram.mem.profileMemTime[profile].ddrFreqLimit = DDR_1866 + 1;
    } else if (host->nvram.mem.profileMemTime[profile].ddrFreq >= 1800) {
      host->nvram.mem.profileMemTime[profile].ddrFreqLimit = DDR_1800 + 1;
    } else if (host->nvram.mem.profileMemTime[profile].ddrFreq >= 1600) {
      host->nvram.mem.profileMemTime[profile].ddrFreqLimit = DDR_1600 + 1;
    } else if (host->nvram.mem.profileMemTime[profile].ddrFreq >= 1400) {
      host->nvram.mem.profileMemTime[profile].ddrFreqLimit = DDR_1400 + 1;
    } else if (host->nvram.mem.profileMemTime[profile].ddrFreq >= 1333) {
      host->nvram.mem.profileMemTime[profile].ddrFreqLimit = DDR_1333 + 1;
    } else if (host->nvram.mem.profileMemTime[profile].ddrFreq >= 1200) {
      host->nvram.mem.profileMemTime[profile].ddrFreqLimit = DDR_1200 + 1;
    } else if (host->nvram.mem.profileMemTime[profile].ddrFreq >= 1066) {
      host->nvram.mem.profileMemTime[profile].ddrFreqLimit = DDR_1066 + 1;
    } else if (host->nvram.mem.profileMemTime[profile].ddrFreq >= 1000) {
      host->nvram.mem.profileMemTime[profile].ddrFreqLimit = DDR_1000 + 1;
    } else {
      host->nvram.mem.profileMemTime[profile].ddrFreqLimit = DDR_800 + 1;
    }

    tCK = preciseTCK[host->nvram.mem.profileMemTime[profile].ddrFreqLimit - 1];

    // Loop for each CPU socket
    for (socket = 0; socket < MAX_SOCKET; socket++) {
      if (host->nvram.mem.socket[socket].enabled == 0) continue;

      channelNvList = GetChannelNvList(host, socket);

      //
      // Check strap addresses on each channel moving near to far
      //
      for (ch = 0; ch < MAX_CH; ch++) {
        if ((*channelNvList)[ch].enabled == 0) continue;

        dimmNvList = GetDimmNvList(host, socket, ch);

        for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {

          if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

          //
          // Get tFAW
          //
          ReadSpd (host, socket, ch, dimm, SPD_XMP_TFAW + xmpOffset, &SPDReg);
          tFAW = (UINT16) SPDReg;

          ReadSpd (host, socket, ch, dimm, SPD_XMP_UN_TFAW + xmpOffset, &SPDReg);
          tFAW = tFAW | ((UINT16) SPDReg << 8);
          //
          // Get lower byte of tFAW
          //
          cycles = TimeBaseToDCLKtCK(host, socket, ch, dimm, tFAW, 0, tCK);
          if (host->nvram.mem.profileMemTime[profile].nFAW < cycles) {
            host->nvram.mem.profileMemTime[profile].nFAW = (UINT8)cycles;
          }

          checkSum[socket][ch][dimm] = checkSum[socket][ch][dimm] + tFAW;

          //
          // Get tRRD
          //
          cycles = GetCycles(host, socket, ch, dimm, SPD_XMP_TRRD + xmpOffset, 0, tCK);
          if (host->nvram.mem.profileMemTime[profile].nRRD < cycles) {
            host->nvram.mem.profileMemTime[profile].nRRD = (UINT8)cycles;
          }

          checkSum[socket][ch][dimm] = checkSum[socket][ch][dimm] + (UINT16) SPDReg;

          //
          // Get tWTR
          //
          cycles = GetCycles(host, socket, ch, dimm, SPD_XMP_TWTR + xmpOffset, 0, tCK);
          if (host->nvram.mem.profileMemTime[profile].nWTR < cycles) {
            host->nvram.mem.profileMemTime[profile].nWTR = (UINT8)cycles;
          }

          checkSum[socket][ch][dimm] = checkSum[socket][ch][dimm] + (UINT16) SPDReg;

          //
          // Get tRP
          //
          cycles = GetCycles(host, socket, ch, dimm, SPD_XMP_TRP + xmpOffset, SPD_XMP_FTB_TRP + xmpOffset, tCK);
          if (host->nvram.mem.profileMemTime[profile].nRP < cycles) {
            host->nvram.mem.profileMemTime[profile].nRP = (UINT8)cycles;
          }

          checkSum[socket][ch][dimm] = checkSum[socket][ch][dimm] + (UINT16) SPDReg;

          //
          // Get tRC and tRAS
          //
          ReadSpd (host, socket, ch, dimm, SPD_XMP_TRC_TRAS + xmpOffset, &SPDReg);
          tRC = (UINT16) (SPDReg & 0xF0) << 4;
          //
          // Get upper nibble of tRC
          //
          tRAS = (UINT16) (SPDReg & 0x0F) << 8;
          //
          // Get upper nibble of tRAS
          //
          ReadSpd (host, socket, ch, dimm, SPD_XMP_TRAS + xmpOffset, &SPDReg);
          tRAS = tRAS | (UINT16) SPDReg;
          //
          // Get lower byte of tRAS
          //
          cycles = TimeBaseToDCLKtCK(host, socket, ch, dimm, tRAS, 0, tCK);
          if (host->nvram.mem.profileMemTime[profile].nRAS < cycles) {
            host->nvram.mem.profileMemTime[profile].nRAS = (UINT8)cycles;
          }

          checkSum[socket][ch][dimm] = checkSum[socket][ch][dimm] + tRAS;

          ReadSpd (host, socket, ch, dimm, SPD_XMP_TRC + xmpOffset, &SPDReg);
          tRC = tRC | (UINT16) SPDReg;
          //
          // Get lower byte of tRC
          //
          cycles = TimeBaseToDCLKtCK(host, socket, ch, dimm, tRC, 0, tCK);
          if (host->nvram.mem.profileMemTime[profile].nRC < cycles) {
            host->nvram.mem.profileMemTime[profile].nRC = (UINT8)cycles;
          }

          checkSum[socket][ch][dimm] = checkSum[socket][ch][dimm] + tRC;

          //
          // Get tRCD
          //
          cycles = GetCycles(host, socket, ch, dimm, SPD_XMP_TRCD + xmpOffset, SPD_XMP_FTB_TRCD + xmpOffset, tCK);
          if (host->nvram.mem.profileMemTime[profile].nRCD < cycles) {
            host->nvram.mem.profileMemTime[profile].nRCD = (UINT8)cycles;
          }

          checkSum[socket][ch][dimm] = checkSum[socket][ch][dimm] + (UINT16) SPDReg;

          //
          // Get tRTP
          //
          cycles = GetCycles(host, socket, ch, dimm, SPD_XMP_TRTP + xmpOffset, 0, tCK);
          if (host->nvram.mem.profileMemTime[profile].nRTP < cycles) {
            host->nvram.mem.profileMemTime[profile].nRTP = (UINT8)cycles;
          }

          checkSum[socket][ch][dimm] = checkSum[socket][ch][dimm] + (UINT16) SPDReg;

          //
          // Get tRFC
          //
          ReadSpd (host, socket, ch, dimm, SPD_XMP_TRFC_LSB + xmpOffset, &SPDReg);
          tRFC = (UINT16) SPDReg;

          ReadSpd (host, socket, ch, dimm, SPD_XMP_TRFC_MSB + xmpOffset, &SPDReg);
          tRFC = tRFC | ((UINT16) SPDReg << 8);
          //
          // Get lower byte of tRFC
          //
          cycles = TimeBaseToDCLKtCK(host, socket, ch, dimm, tRFC, 0, tCK);
          if (host->nvram.mem.profileMemTime[profile].nRFC < cycles) {
            host->nvram.mem.profileMemTime[profile].nRFC = cycles;
          }

          checkSum[socket][ch][dimm] = checkSum[socket][ch][dimm] + tRFC;

          //
          // Get tWR
          //
          ReadSpd (host, socket, ch, dimm, SPD_XMP_TWR + xmpOffset, &SPDReg);
          cycles = TimeBaseToDCLKtCK(host, socket, ch, dimm, (UINT16)SPDReg, 0, tCK);
          if (host->nvram.mem.profileMemTime[profile].nWR < cycles) {
            host->nvram.mem.profileMemTime[profile].nWR = (UINT8)cycles;
          }

          checkSum[socket][ch][dimm] = checkSum[socket][ch][dimm] + (UINT16) SPDReg;

          //
          // Get tAA
          //
          cycles = GetCycles(host, socket, ch, dimm, SPD_XMP_TAA + xmpOffset, SPD_XMP_FTB_TAA + xmpOffset, tCK);
          if (host->nvram.mem.profileMemTime[profile].nCL < cycles) {
            host->nvram.mem.profileMemTime[profile].nCL = (UINT8)cycles;
          }

          checkSum[socket][ch][dimm] = checkSum[socket][ch][dimm] + (UINT16) SPDReg;

          //
          // Get supported CAS settings
          //
          ReadSpd (host, socket, ch, dimm, SPD_XMP_CAS_SUP_MSB + xmpOffset, &SPDReg);
          casSup = (UINT32) (SPDReg << 8);

          ReadSpd (host, socket, ch, dimm, SPD_XMP_CAS_SUP_LSB + xmpOffset, &SPDReg);

          casSup = casSup | (UINT32) SPDReg;
          host->nvram.mem.profileMemTime[profile].casSup &= casSup;
          //
          // Mask off the unsupported CLs
          //
          checkSum[socket][ch][dimm] = checkSum[socket][ch][dimm] + casSup;

          //
          // Get tCWL
          //
          cycles = GetCycles(host, socket, ch, dimm, SPD_XMP_TCWL + xmpOffset, 0, tCK);
          if (host->nvram.mem.profileMemTime[profile].nCWL < cycles) {
            host->nvram.mem.profileMemTime[profile].nCWL = (UINT8)cycles;
          }

          checkSum[socket][ch][dimm] = checkSum[socket][ch][dimm] + (UINT16) SPDReg;

          //
          // Get command timing
          //
          cycles = GetCycles(host, socket, ch, dimm, SPD_XMP_CMD + xmpOffset, 0, tCK);
          if (host->nvram.mem.profileMemTime[profile].nCMDRate < cycles) {
            host->nvram.mem.profileMemTime[profile].nCMDRate = (UINT8)cycles;
          }

          checkSum[socket][ch][dimm] = checkSum[socket][ch][dimm] + (UINT16) SPDReg;

          //
          // Get refresh rate
          //
          ReadSpd (host, socket, ch, dimm, SPD_XMP_TREFI_MSB + xmpOffset, &SPDReg);
          tREFI = (UINT16) SPDReg << 8;

          ReadSpd (host, socket, ch, dimm, SPD_XMP_TREFI_LSB + xmpOffset, &SPDReg);
          tREFI |= (UINT16) SPDReg;
          tREFI = (tREFI * 1000 * (*dimmNvList)[dimm].mtbDividend) / (*dimmNvList)[dimm].mtbDiv;
          if (host->nvram.mem.profileMemTime[profile].tREFI > tREFI) {
            host->nvram.mem.profileMemTime[profile].tREFI = tREFI;
          }

          checkSum[socket][ch][dimm] = checkSum[socket][ch][dimm] + tREFI;

          //
          // MC Vdd
          //
          ReadSpd (host, socket, ch, dimm, SPD_XMP_MC_VOLT + xmpOffset, &SPDReg);
          if (host->nvram.mem.profileMemTime[profile].ucVolt < SPDReg) {
            host->nvram.mem.profileMemTime[profile].ucVolt = SPDReg;
          }

          checkSum[socket][ch][dimm] = checkSum[socket][ch][dimm] + (UINT16) SPDReg;

          //
          // Vdd
          //
          ReadSpd (host, socket, ch, dimm, SPD_XMP_VDD + xmpOffset, &SPDReg);
          if (host->nvram.mem.profileMemTime[profile].vdd < SPDReg) {
            host->nvram.mem.profileMemTime[profile].vdd = SPDReg;
          }

          checkSum[socket][ch][dimm] = checkSum[socket][ch][dimm] + (UINT16) SPDReg;

          if (host->setup.mem.options & MEM_OVERRIDE_EN) {
            if ((*dimmNvList)[dimm].XMPChecksum != checkSum[socket][ch][dimm]) {
              //
              // If timing overrides are enabled but the DIMM configuration has changed disable memory overrides
              // and save the new checksum
              //
#ifdef SERIAL_DBG_MSG
              if (checkMsgLevel(host, SDBG_MINMAX)) {
                getPrintFControl (host);
                rcPrintf ((host, "DIMM configuration changed!\n"));
                rcPrintf ((host, "Old checksum = 0x%x\n", (*dimmNvList)[dimm].XMPChecksum));
                rcPrintf ((host, "New checksum = 0x%x\n", checkSum[socket][ch][dimm]));
                releasePrintFControl (host);
              }
#endif
              //debug...
              //OutputWarning (host, WARN_MEM_CONFIG_CHANGED, WARN_MEM_OVERRIDE_DISABLED, socket, ch, dimm, 0xFF);

              //
              // Disable overrides
              // host->setup.mem.options &= ~MEM_OVERRIDE_EN;
              // Save new checksum
              //
              (*dimmNvList)[dimm].XMPChecksum = checkSum[socket][ch][dimm];

              //
              // Force frequency to auto
              //
              host->var.mem.ddrFreqLimit = 0;
            }
          } else {
            //
            // Save the checksum
            //
            (*dimmNvList)[dimm].XMPChecksum = checkSum[socket][ch][dimm];
          }
        } // dimm loop
      } // ch loop
    } // socket loop

    if (host->nvram.mem.profileMemTime[profile].vdd & BIT5) {
      host->nvram.mem.profileMemTime[profile].vdd = (host->nvram.mem.profileMemTime[profile].vdd & 0x1F) + 100;
    } else if (host->nvram.mem.profileMemTime[profile].vdd & BIT6) {
      host->nvram.mem.profileMemTime[profile].vdd = (host->nvram.mem.profileMemTime[profile].vdd & 0x1F) + 200;
    }

    //
    // Make sure tCWL is valid
    //
    if ((host->nvram.mem.profileMemTime[profile].nCWL > host->nvram.mem.profileMemTime[profile].nCL) ||
        (host->nvram.mem.profileMemTime[profile].nCWL < (host->nvram.mem.profileMemTime[profile].nCL - 4))) {

      //
      // Set to a safe value
      //
      host->nvram.mem.profileMemTime[profile].nCWL = 8;
    }

#ifdef   SERIAL_DBG_MSG
    if (checkMsgLevel(host, SDBG_MAX)) {
      getPrintFControl(host);
      rcPrintf ((host, "XMP Profile %d\n", profile + 1));
      rcPrintf ((host, "nCL = %d\n", host->nvram.mem.profileMemTime[profile].nCL));
      rcPrintf ((host, "nRP = %d\n", host->nvram.mem.profileMemTime[profile].nRP));
      rcPrintf ((host, "nRCD = %d\n", host->nvram.mem.profileMemTime[profile].nRCD));
      rcPrintf ((host, "nRRD = %d\n", host->nvram.mem.profileMemTime[profile].nRRD));
      rcPrintf ((host, "nWTR = %d\n", host->nvram.mem.profileMemTime[profile].nWTR));
      rcPrintf ((host, "nRAS = %d\n", host->nvram.mem.profileMemTime[profile].nRAS));
      rcPrintf ((host, "nRTP = %d\n", host->nvram.mem.profileMemTime[profile].nRTP));
      rcPrintf ((host, "nWR = %d\n", host->nvram.mem.profileMemTime[profile].nWR));
      rcPrintf ((host, "nFAW = %d\n", host->nvram.mem.profileMemTime[profile].nFAW));
      rcPrintf ((host, "nCWL = %d\n", host->nvram.mem.profileMemTime[profile].nCWL));
      rcPrintf ((host, "nRC = %d\n", host->nvram.mem.profileMemTime[profile].nRC));
      rcPrintf ((host, "nRFC = %d\n", host->nvram.mem.profileMemTime[profile].nRFC));
      rcPrintf ((host, "nCMDRate = %d\n", host->nvram.mem.profileMemTime[profile].nCMDRate));
      rcPrintf ((host, "ddrFreqLimit = %d\n", host->nvram.mem.profileMemTime[profile].ddrFreqLimit));
      rcPrintf ((host, "vdd = 0x%x\n", host->nvram.mem.profileMemTime[profile].vdd));
      rcPrintf ((host, "casSup = 0x%x\n", host->nvram.mem.profileMemTime[profile].casSup));
      rcPrintf ((host, "tREFI = %dns\n", host->nvram.mem.profileMemTime[profile].tREFI));
      rcPrintf ((host, "ddrFreq = %d\n", host->nvram.mem.profileMemTime[profile].ddrFreq));
      releasePrintFControl(host);
    }
#endif
  } // profile loop
} // GetXMPTimings

/*++

  Gathers XMP timings for DDR4

  @param host  - Pointer to sysHost

  @retval N/A

--*/
void
GetXMPTimingsDDR4(
                 PSYSHOST host
                 )
{
  UINT8               socket;
  UINT8               ch;
  UINT8               dimm;
  UINT8               profile;
  UINT8               SPDReg;
  UINT16              cycles;
  UINT8               xmpOffset;
  UINT8               minTCK;
  UINT8               ftbVal;
  UINT16              tempFreq;
  UINT32              casSup;
  UINT16              tRC;
  UINT16              tRAS;
  UINT16              tRFC;
  UINT16              tFAW;
  UINT32              checkSum[MAX_SOCKET][MAX_CH][MAX_DIMM];
  UINT32              tCK;
  INT32               tempTCK;
  struct dimmNvram    (*dimmNvList)[MAX_DIMM];
  struct channelNvram (*channelNvList)[MAX_CH];

  //
  // Check for two possible profiles
  //
  for (profile = 0; profile < 2; profile++) {
    //
    // Continue if this profile is not supported
    //
    if ((host->nvram.mem.XMPProfilesSup & (1 << profile)) == 0) continue;

    //
    // Initialize to large values
    //
    host->nvram.mem.profileMemTime[profile].ddrFreq = 0xFFFF;
    host->nvram.mem.profileMemTime[profile].casSup  = 0xFFFFFFFF;
    host->nvram.mem.profileMemTime[profile].tREFI   = 0x7FFF;

    //
    // Offset into SPD
    //
    xmpOffset = profile * 47;

    // Loop for each CPU socket
    for (socket = 0; socket < MAX_SOCKET; socket++) {
      if (host->nvram.mem.socket[socket].enabled == 0) continue;

      channelNvList = GetChannelNvList(host, socket);

      //
      // Loop for each channel
      //
      for (ch = 0; ch < MAX_CH; ch++) {
        if ((*channelNvList)[ch].enabled == 0) continue;

        dimmNvList = GetDimmNvList(host, socket, ch);

        //
        // Loop for each DIMM
        //
        for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {
          if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

          checkSum[socket][ch][dimm]  = 0;

          if (!profile) {
            //
            // Get TimeBases
            //
            ReadSpd (host, socket, ch, dimm, SPD_XMP_TB_PROFILE_1, &SPDReg);
            checkSum[socket][ch][dimm] = checkSum[socket][ch][dimm] + (UINT16) SPDReg;

            (*dimmNvList)[dimm].SPDftb = 1;      // 1 picosecond
            (*dimmNvList)[dimm].SPDmtb = 125;    //125 picoseconds
          }

          //
          // Get Min TCK
          //
          ReadSpd (host, socket, ch, dimm, SPD_XMP_TCK_DDR4 + xmpOffset, &minTCK);
          checkSum[socket][ch][dimm] = checkSum[socket][ch][dimm] + (UINT16) minTCK;

          // Get FTB Min TCK
          ReadSpd (host, socket, ch, dimm, SPD_XMP_FTB_TCK_DDR4 + xmpOffset, &ftbVal);
          checkSum[socket][ch][dimm] = checkSum[socket][ch][dimm] + (UINT16) ftbVal;

          if (ftbVal & 0x80) {
            tempTCK = (UINT32)((minTCK * 1000) * (*dimmNvList)[dimm].SPDmtb) + (((ftbVal - 256) * 1000) *  (*dimmNvList)[dimm].SPDftb);
          } else {
            tempTCK = (UINT32)((minTCK * 1000) * (*dimmNvList)[dimm].SPDmtb) + ((ftbVal * 1000) *(*dimmNvList)[dimm].SPDftb);
          }

          //
          // Determine transfer rate
          //
          if (tempTCK <= DDR_4400_TCK_MIN) {
            tempFreq = 4400;
          } else if (tempTCK <= DDR_4266_TCK_MIN) {
            tempFreq = 4266;
          } else if (tempTCK <= DDR_4200_TCK_MIN) {
            tempFreq = 4200;
          } else if (tempTCK <= DDR_4000_TCK_MIN) {
            tempFreq = 4000;
          } else if (tempTCK <= DDR_3800_TCK_MIN) {
            tempFreq = 3800;
          } else if (tempTCK <= DDR_3733_TCK_MIN) {
            tempFreq = 3733;
          } else if (tempTCK <= DDR_3600_TCK_MIN) {
            tempFreq = 3600;
          } else if (tempTCK <= DDR_3466_TCK_MIN) {
            tempFreq = 3466;
          } else if (tempTCK <= DDR_3400_TCK_MIN) {
            tempFreq = 3400;
          } else if (tempTCK <= DDR_3200_TCK_MIN) {
            tempFreq = 3200;
          } else if (tempTCK <= DDR_3000_TCK_MIN) {
            tempFreq = 3000;
          } else if (tempTCK <= DDR_2933_TCK_MIN) {
            tempFreq = 2933;
          } else if (tempTCK <= DDR_2800_TCK_MIN) {
            tempFreq = 2800;
          } else if (tempTCK <= DDR_2666_TCK_MIN) {
            tempFreq = 2666;
          } else if (tempTCK <= DDR_2600_TCK_MIN) {
            tempFreq = 2600;
          } else if (tempTCK <= DDR_2400_TCK_MIN) {
            tempFreq = 2400;
          } else if (tempTCK <= DDR_2200_TCK_MIN) {
            tempFreq = 2200;
          } else if (tempTCK <= DDR_2133_TCK_MIN) {
            tempFreq = 2133;
          } else if (tempTCK <= DDR_2000_TCK_MIN) {
            tempFreq = 2000;
          } else if (tempTCK <= DDR_1866_TCK_MIN) {
            tempFreq = 2133;
          } else if (tempTCK <= DDR_1800_TCK_MIN) {
            tempFreq = 1800;
          } else if (tempTCK <= DDR_1600_TCK_MIN) {
            tempFreq = 1600;
          } else if (tempTCK <= DDR_1400_TCK_MIN) {
            tempFreq = 1400;
          } else {
            tempFreq = 1333;
          }
          //
          // Save the frequency supported by all DIMMs
          //
          if (host->nvram.mem.profileMemTime[profile].ddrFreq > tempFreq) {
            host->nvram.mem.profileMemTime[profile].ddrFreq = tempFreq;
          }
        } // dimm loop
      } // ch loop
    } // socket loop

    //
    // Determine DCLK Ratio
    //
    if (host->nvram.mem.profileMemTime[profile].ddrFreq >= 4400) {
      host->nvram.mem.profileMemTime[profile].ddrFreqLimit = DDR_4400 + 1;
    } else if (host->nvram.mem.profileMemTime[profile].ddrFreq >= 4266) {
      host->nvram.mem.profileMemTime[profile].ddrFreqLimit = DDR_4266 + 1;
    } else if (host->nvram.mem.profileMemTime[profile].ddrFreq >= 4200) {
      host->nvram.mem.profileMemTime[profile].ddrFreqLimit = DDR_4200 + 1;
    } else if (host->nvram.mem.profileMemTime[profile].ddrFreq >= 4000) {
      host->nvram.mem.profileMemTime[profile].ddrFreqLimit = DDR_4000 + 1;
    } else if (host->nvram.mem.profileMemTime[profile].ddrFreq >= 3800) {
      host->nvram.mem.profileMemTime[profile].ddrFreqLimit = DDR_3800 + 1;
    } else if (host->nvram.mem.profileMemTime[profile].ddrFreq >= 3733) {
      host->nvram.mem.profileMemTime[profile].ddrFreqLimit = DDR_3733 + 1;
    } else if (host->nvram.mem.profileMemTime[profile].ddrFreq >= 3600) {
      host->nvram.mem.profileMemTime[profile].ddrFreqLimit = DDR_3600 + 1;
    } else if (host->nvram.mem.profileMemTime[profile].ddrFreq >= 3466) {
      host->nvram.mem.profileMemTime[profile].ddrFreqLimit = DDR_3466 + 1;
    } else if (host->nvram.mem.profileMemTime[profile].ddrFreq >= 3400) {
      host->nvram.mem.profileMemTime[profile].ddrFreqLimit = DDR_3400 + 1;
    } else if (host->nvram.mem.profileMemTime[profile].ddrFreq >= 3200) {
      host->nvram.mem.profileMemTime[profile].ddrFreqLimit = DDR_3200 + 1;
    } else if (host->nvram.mem.profileMemTime[profile].ddrFreq >= 3000) {
      host->nvram.mem.profileMemTime[profile].ddrFreqLimit = DDR_3000 + 1;
    } else if (host->nvram.mem.profileMemTime[profile].ddrFreq >= 2933) {
      host->nvram.mem.profileMemTime[profile].ddrFreqLimit = DDR_2933 + 1;
    } else if (host->nvram.mem.profileMemTime[profile].ddrFreq >= 2800) {
      host->nvram.mem.profileMemTime[profile].ddrFreqLimit = DDR_2800 + 1;
    } else if (host->nvram.mem.profileMemTime[profile].ddrFreq >= 2666) {
      host->nvram.mem.profileMemTime[profile].ddrFreqLimit = DDR_2666 + 1;
    } else if (host->nvram.mem.profileMemTime[profile].ddrFreq >= 2600) {
      host->nvram.mem.profileMemTime[profile].ddrFreqLimit = DDR_2600 + 1;
    } else if (host->nvram.mem.profileMemTime[profile].ddrFreq >= 2400) {
      host->nvram.mem.profileMemTime[profile].ddrFreqLimit = DDR_2400 + 1;
    } else if (host->nvram.mem.profileMemTime[profile].ddrFreq >= 2200) {
      host->nvram.mem.profileMemTime[profile].ddrFreqLimit = DDR_2200 + 1;
    } else if (host->nvram.mem.profileMemTime[profile].ddrFreq >= 2133) {
      host->nvram.mem.profileMemTime[profile].ddrFreqLimit = DDR_2133 + 1;
    } else if (host->nvram.mem.profileMemTime[profile].ddrFreq >= 2000) {
      host->nvram.mem.profileMemTime[profile].ddrFreqLimit = DDR_2000 + 1;
    } else if (host->nvram.mem.profileMemTime[profile].ddrFreq >= 1866) {
      host->nvram.mem.profileMemTime[profile].ddrFreqLimit = DDR_1866 + 1;
    } else if (host->nvram.mem.profileMemTime[profile].ddrFreq >= 1800) {
      host->nvram.mem.profileMemTime[profile].ddrFreqLimit = DDR_1800 + 1;
    } else if (host->nvram.mem.profileMemTime[profile].ddrFreq >= 1600) {
      host->nvram.mem.profileMemTime[profile].ddrFreqLimit = DDR_1600 + 1;
    } else if (host->nvram.mem.profileMemTime[profile].ddrFreq >= 1400) {
      host->nvram.mem.profileMemTime[profile].ddrFreqLimit = DDR_1400 + 1;
    } else if (host->nvram.mem.profileMemTime[profile].ddrFreq >= 1333) {
      host->nvram.mem.profileMemTime[profile].ddrFreqLimit = DDR_1333 + 1;
    } else if (host->nvram.mem.profileMemTime[profile].ddrFreq >= 1200) {
      host->nvram.mem.profileMemTime[profile].ddrFreqLimit = DDR_1200 + 1;
    } else if (host->nvram.mem.profileMemTime[profile].ddrFreq >= 1066) {
      host->nvram.mem.profileMemTime[profile].ddrFreqLimit = DDR_1066 + 1;
    } else if (host->nvram.mem.profileMemTime[profile].ddrFreq >= 1000) {
      host->nvram.mem.profileMemTime[profile].ddrFreqLimit = DDR_1000 + 1;
    } else {
      host->nvram.mem.profileMemTime[profile].ddrFreqLimit = DDR_800 + 1;
    }

    tCK = preciseTCK[host->nvram.mem.profileMemTime[profile].ddrFreqLimit - 1];

    // Loop for each CPU socket
    for (socket = 0; socket < MAX_SOCKET; socket++) {
      if (host->nvram.mem.socket[socket].enabled == 0) continue;

      channelNvList = GetChannelNvList(host, socket);

      //
      // Check strap addresses on each channel moving near to far
      //
      for (ch = 0; ch < MAX_CH; ch++) {
        if ((*channelNvList)[ch].enabled == 0) continue;

        dimmNvList = GetDimmNvList(host, socket, ch);

        for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {

          if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

          //
          // Get tFAW
          //
          ReadSpd (host, socket, ch, dimm, SPD_XMP_TFAW_DDR4 + xmpOffset, &SPDReg);
          tFAW = (UINT16) SPDReg;

          ReadSpd (host, socket, ch, dimm, SPD_XMP_UN_TFAW_DDR4 + xmpOffset, &SPDReg);
          tFAW = tFAW | ((UINT16) SPDReg << 8);
          //
          // Get lower byte of tFAW
          //
          cycles = TimeBaseToDCLKtCK(host, socket, ch, dimm, tFAW, 0, tCK);
          if (host->nvram.mem.profileMemTime[profile].nFAW < cycles) {
            host->nvram.mem.profileMemTime[profile].nFAW = (UINT8)cycles;
          }

          checkSum[socket][ch][dimm] = checkSum[socket][ch][dimm] + tFAW;

          //
          // Get tRRD_S
          //
          cycles = GetCycles(host, socket, ch, dimm, SPD_XMP_TRRDS_DDR4 + xmpOffset, SPD_XMP_FTB_TRRDS + xmpOffset, tCK);
          if (host->nvram.mem.profileMemTime[profile].nRRD < cycles) {
            host->nvram.mem.profileMemTime[profile].nRRD = (UINT8)cycles;
          }

          checkSum[socket][ch][dimm] = checkSum[socket][ch][dimm] + (UINT16) SPDReg;

          //
          // Get tRRD_L
          //
          cycles = GetCycles(host, socket, ch, dimm, SPD_XMP_TRRDL_DDR4 + xmpOffset, SPD_XMP_FTB_TRRDL + xmpOffset, tCK);
          if (host->nvram.mem.profileMemTime[profile].nRRD_L < cycles) {
            host->nvram.mem.profileMemTime[profile].nRRD_L = (UINT8)cycles;
          }

          checkSum[socket][ch][dimm] = checkSum[socket][ch][dimm] + (UINT16) SPDReg;

          //
          // Get tRP
          //
          cycles = GetCycles(host, socket, ch, dimm, SPD_XMP_TRP_DDR4 + xmpOffset, SPD_XMP_FTB_TRP_DDR4 + xmpOffset, tCK);
          if (host->nvram.mem.profileMemTime[profile].nRP < cycles) {
            host->nvram.mem.profileMemTime[profile].nRP = (UINT8)cycles;
          }

          checkSum[socket][ch][dimm] = checkSum[socket][ch][dimm] + (UINT16) SPDReg;

          //
          // Get tRC and tRAS
          //
          ReadSpd (host, socket, ch, dimm, SPD_XMP_TRC_TRAS_DDR4 + xmpOffset, &SPDReg);
          tRC = (UINT16) (SPDReg & 0xF0) << 4;
          //
          // Get upper nibble of tRC
          //
          tRAS = (UINT16) (SPDReg & 0x0F) << 8;
          //
          // Get upper nibble of tRAS
          //
          ReadSpd (host, socket, ch, dimm, SPD_XMP_TRAS_DDR4 + xmpOffset, &SPDReg);
          tRAS = tRAS | (UINT16) SPDReg;
          //
          // Get lower byte of tRAS
          //
          cycles = TimeBaseToDCLKtCK(host, socket, ch, dimm, tRAS, 0, tCK);
          if (host->nvram.mem.profileMemTime[profile].nRAS < cycles) {
            host->nvram.mem.profileMemTime[profile].nRAS = (UINT8)cycles;
          }

          checkSum[socket][ch][dimm] = checkSum[socket][ch][dimm] + tRAS;

          ReadSpd (host, socket, ch, dimm, SPD_XMP_TRC_DDR4 + xmpOffset, &SPDReg);
          tRC = tRC | (UINT16) SPDReg;
          //
          // Get lower byte of tRC
          //
          cycles = TimeBaseToDCLKtCK(host, socket, ch, dimm, tRC, 0, tCK);
          if (host->nvram.mem.profileMemTime[profile].nRC < cycles) {
            host->nvram.mem.profileMemTime[profile].nRC = (UINT8)cycles;
          }

          checkSum[socket][ch][dimm] = checkSum[socket][ch][dimm] + tRC;

          //
          // Get tRCD
          //
          cycles = GetCycles(host, socket, ch, dimm, SPD_XMP_TRCD_DDR4 + xmpOffset, SPD_XMP_FTB_TRCD_DDR4 + xmpOffset, tCK);

          //
          // Odd latencies greater than 16 are not supported
          //
          if ((cycles > 16) && (cycles & BIT0)) {
            cycles++;
          }

          if (host->nvram.mem.profileMemTime[profile].nRCD < cycles) {
            host->nvram.mem.profileMemTime[profile].nRCD = (UINT8)cycles;
          }

          checkSum[socket][ch][dimm] = checkSum[socket][ch][dimm] + (UINT16) SPDReg;

          //
          // Get tRFC
          //
          ReadSpd (host, socket, ch, dimm, SPD_XMP_TRFC1_LSB + xmpOffset, &SPDReg);
          tRFC = (UINT16) SPDReg;

          ReadSpd (host, socket, ch, dimm, SPD_XMP_TRFC1_MSB + xmpOffset, &SPDReg);
          tRFC = tRFC | ((UINT16) SPDReg << 8);
          //
          // Get lower byte of tRFC
          //
          cycles = TimeBaseToDCLKtCK(host, socket, ch, dimm, tRFC, 0, tCK);
          if (host->nvram.mem.profileMemTime[profile].nRFC < cycles) {
            host->nvram.mem.profileMemTime[profile].nRFC = cycles;
          }

          checkSum[socket][ch][dimm] = checkSum[socket][ch][dimm] + tRFC;

          //
          // Get tAA
          //
          cycles = GetCycles(host, socket, ch, dimm, SPD_XMP_TAA_DDR4 + xmpOffset, SPD_XMP_FTB_TAA_DDR4 + xmpOffset, tCK);

          if (host->nvram.mem.profileMemTime[profile].nCL < cycles) {
            host->nvram.mem.profileMemTime[profile].nCL = (UINT8)cycles;
          }

          checkSum[socket][ch][dimm] = checkSum[socket][ch][dimm] + (UINT16) SPDReg;

          //
          // Get supported CAS settings
          //
          ReadSpd (host, socket, ch, dimm, SPD_XMP_CAS_SUP_4 + xmpOffset, &SPDReg);
          casSup = (UINT32) (SPDReg << 24);

          ReadSpd (host, socket, ch, dimm, SPD_XMP_CAS_SUP_3 + xmpOffset, &SPDReg);
          casSup = casSup | (UINT32) (SPDReg << 16);

          ReadSpd (host, socket, ch, dimm, SPD_XMP_CAS_SUP_2+ xmpOffset, &SPDReg);
          casSup = (UINT32) (SPDReg << 8);

          ReadSpd (host, socket, ch, dimm, SPD_XMP_CAS_SUP_1+ xmpOffset, &SPDReg);
          casSup = casSup | (UINT32) SPDReg;

          host->nvram.mem.profileMemTime[profile].casSup &= casSup;
          //
          // Mask off the unsupported CLs
          //
          checkSum[socket][ch][dimm] = checkSum[socket][ch][dimm] + casSup;

          //
          // Vdd
          //
          ReadSpd (host, socket, ch, dimm, SPD_XMP_VDD_DDR4 + xmpOffset, &SPDReg);
          if (host->nvram.mem.profileMemTime[profile].vdd < SPDReg) {
            host->nvram.mem.profileMemTime[profile].vdd = SPDReg;
          }

          checkSum[socket][ch][dimm] = checkSum[socket][ch][dimm] + (UINT16) SPDReg;

          if (host->setup.mem.options & MEM_OVERRIDE_EN) {
            if ((*dimmNvList)[dimm].XMPChecksum != checkSum[socket][ch][dimm]) {
              //
              // If timing overrides are enabled but the DIMM configuration has changed disable memory overrides
              // and save the new checksum
              //
#ifdef SERIAL_DBG_MSG
              if (checkMsgLevel(host, SDBG_MINMAX)) {
                getPrintFControl (host);
                rcPrintf ((host, "DIMM configuration changed!\n"));
                rcPrintf ((host, "Old checksum = 0x%x\n", (*dimmNvList)[dimm].XMPChecksum));
                rcPrintf ((host, "New checksum = 0x%x\n", checkSum[socket][ch][dimm]));
                releasePrintFControl (host);
              }
#endif
              //debug...
              //OutputWarning (host, WARN_MEM_CONFIG_CHANGED, WARN_MEM_OVERRIDE_DISABLED, socket, ch, dimm, 0xFF);

              //
              // Disable overrides
              // host->setup.mem.options &= ~MEM_OVERRIDE_EN;
              // Save new checksum
              //
              (*dimmNvList)[dimm].XMPChecksum = checkSum[socket][ch][dimm];

              //
              // Force frequency to auto
              //
              host->var.mem.ddrFreqLimit = 0;
            }
          } else {
            //
            // Save the checksum
            //
            (*dimmNvList)[dimm].XMPChecksum = checkSum[socket][ch][dimm];
          }
        } // dimm loop
      } // ch loop
    } // socket loop

    if (host->nvram.mem.profileMemTime[profile].vdd & BIT7) {
      host->nvram.mem.profileMemTime[profile].vdd = (host->nvram.mem.profileMemTime[profile].vdd & ~BIT7) + 100;
    }
#ifdef   SERIAL_DBG_MSG
    if (checkMsgLevel(host, SDBG_MAX)) {
      getPrintFControl(host);
      rcPrintf ((host, "XMP Profile %d\n", profile + 1));
      rcPrintf ((host, "nCL = %d\n", host->nvram.mem.profileMemTime[profile].nCL));
      rcPrintf ((host, "nRP = %d\n", host->nvram.mem.profileMemTime[profile].nRP));
      rcPrintf ((host, "nRCD = %d\n", host->nvram.mem.profileMemTime[profile].nRCD));
      rcPrintf ((host, "nRRD = %d\n", host->nvram.mem.profileMemTime[profile].nRRD));
      rcPrintf ((host, "nRRD_L = %d\n", host->nvram.mem.profileMemTime[profile].nRRD_L));
      rcPrintf ((host, "nRAS = %d\n", host->nvram.mem.profileMemTime[profile].nRAS));
      rcPrintf ((host, "nFAW = %d\n", host->nvram.mem.profileMemTime[profile].nFAW));
      rcPrintf ((host, "nCWL = %d\n", host->nvram.mem.profileMemTime[profile].nCWL));
      rcPrintf ((host, "nRC = %d\n", host->nvram.mem.profileMemTime[profile].nRC));
      rcPrintf ((host, "nRFC = %d\n", host->nvram.mem.profileMemTime[profile].nRFC));
      rcPrintf ((host, "ddrFreqLimit = %d\n", host->nvram.mem.profileMemTime[profile].ddrFreqLimit));
      rcPrintf ((host, "vdd = 0x%x\n", host->nvram.mem.profileMemTime[profile].vdd));
      rcPrintf ((host, "casSup = 0x%x\n", host->nvram.mem.profileMemTime[profile].casSup));
      rcPrintf ((host, "ddrFreq = %d\n", host->nvram.mem.profileMemTime[profile].ddrFreq));
      releasePrintFControl(host);
    }
#endif
  } // profile loop
} // GetXMPTimingsDDR4

UINT16
GetCycles(
         PSYSHOST  host,
         UINT8     socket,
         UINT8     ch,
         UINT8     dimm,
         UINT16    mtbOffset,
         UINT16    ftbOffset,
         UINT32    tCK
         )
/*++

  Gathers XMP timings for DDR4

  @param host      - Pointer to sysHost
  @param socket    - Socket Id
  @param ch        - Channel
  @param dimm      - DIMM
  @param mtbOffset - Time in MTB (Medium Time Base)
  @param ftbOffset - Time in FTB (Fine Time Base)
  @param tCK       - tCK

  @retval Time in DCLKs

--*/
{
  UINT8             mtbVal;
  UINT8             ftbVal = 0;

  ReadSpd (host, socket, ch, dimm, mtbOffset, &mtbVal);
  if (ftbOffset) {
    ReadSpd (host, socket, ch, dimm, ftbOffset, &ftbVal);
  }

  return TimeBaseToDCLKtCK(host, socket, ch, dimm, (UINT16)mtbVal, (UINT16)ftbVal, tCK);
} // GetCycles

UINT16
TimeBaseToDCLKtCK (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     ch,
  UINT8     dimm,
  UINT16    mtbTime,
  UINT16    ftbTime,
  UINT32    tCK
  )
/*++

  Converts MTB/FTB to DCLK

  @param host      - Pointer to sysHost
  @param socket    - Socket Id
  @param ch        - Channel
  @param dimm      - DIMM
  @param mtbTime   - Time in MTB (Medium Time Base)
  @param ftbTime   - Time in FTB (Fine Time Base)
  @param tCK       - tCK

  @retval Time in DCLKs

--*/
{
  UINT16              mtb;
  UINT16              ftb;
  UINT32              tempVal;
  struct dimmNvram    (*dimmNvList)[MAX_DIMM];

  dimmNvList = GetDimmNvList(host, socket, ch);

  if (host->nvram.mem.dramType == SPD_TYPE_DDR4) {
    mtb = 1250;
    ftb = 10;
  } else {
    mtb = 10000 / (*dimmNvList)[dimm].mtbDiv;
    if ((*dimmNvList)[dimm].SPDftb) {
      ftb = ((((*dimmNvList)[dimm].SPDftb >> 4) & 0xF) * 10) / ((*dimmNvList)[dimm].SPDftb & 0xF);
    } else {
      ftb = 0;
    }
  }

  if (ftbTime & BIT7) {
    tempVal = (UINT32)((mtbTime * mtb) + ((ftbTime - 256) * ftb));
  } else {
    tempVal = (UINT32)((mtbTime * mtb) + ((ftbTime & 0x3F) * ftb));
  }

  //
  // Round up to next cycle
  //
  tempVal = (tempVal + tCK - 1) / tCK;

  return (UINT16) tempVal;
} // TimeBaseToDCLKtCK
