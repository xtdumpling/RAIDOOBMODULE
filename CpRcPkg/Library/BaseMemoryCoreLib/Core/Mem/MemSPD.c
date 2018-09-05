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
 * Copyright 2006 - 2016, Intel Corporation All Rights Reserved.
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
#include "MemFunc.h"

//                                       |   512Mb   |    1Gb    |    2Gb    |    4Gb    |     8Gb   |4Gb2xRM|4Gb4xRM|
const   UINT8 primaryWidth[MAX_TECH]   = {  4,  8, 16,  4,  8, 16,  4,  8, 16,  4,  8, 16,  4,  8, 16,  4,  8,  4,  8};   // Primary SDRAM device width
const   UINT8 rowBits[MAX_TECH]        = { 13, 13, 12, 14, 14, 13, 15, 15, 14, 16, 16, 15, 16, 16, 16, 17, 17, 18, 18};   // Number of row address bits
const   UINT8 columnBits[MAX_TECH]     = { 11, 10, 10, 11, 10, 10, 11, 10, 10, 11, 10, 10, 12, 11, 10, 11, 10, 11, 10};   // Number of column address bits
const   UINT8 internalBanks[MAX_TECH]  = {  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8};   // Number of internal SDRAM banks
const   UINT16 rankSize[MAX_TECH]      = { 16,  8,  4, 32, 16,  8, 64, 32, 16,128, 64, 32,256,128, 64,256,128,512,256};   // Rank size in 64 MB units

//                                          |      2Gb     |      4Gb     |      8Gb     |     16Gb     |
const   UINT8 primaryWidthDDR4[MAX_TECH]  = {   4,   8,  16,   4,   8,  16,   4,   8,  16,   4,   8,  16};   // Primary SDRAM device width
const   UINT8 rowBitsDDR4[MAX_TECH]       = {  15,  14,  14,  16,  15,  15,  17,  16,  16,  18,  17,  17};   // Number of row address bits
const   UINT8 columnBitsDDR4[MAX_TECH]    = {  10,  10,  10,  10,  10,  10,  10,  10,  10,  10,  10,  10};   // Number of column address bits
const   UINT8 bankGroupsDDR4[MAX_TECH]    = {   4,   4,   2,   4,   4,   2,   4,   4,   2,   4,   4,   2};   // Number of internal SDRAM banks
const   UINT16 rankSizeDDR4[MAX_TECH]     = {  64,  32,  16, 128,  64,  32, 256, 128,  64, 512, 256, 128};   // Rank size in 64 MB units

const   UINT16 rankSizeAEP[MAX_TECH_AEP] = {128, 256,  512, 1024, 2048,4096,8192,16384}; // Rank Size in 64 MB Units

//
// Timing tables are indexed by these frequencies:
// 800  1000  1066  1200  1333  1400  1600  1800  1866  2000  2133  2200  2400  2600  2666  2800  2933  3000  3200
//

// Table for how the Module Delay bytes are adjusted per frequency in MTP units
// The general equation for the delay adjustment value is ROUNDDOWN(((tCK - 2.5ns) * 3/4) / tMTB)
// 800:    (2.5ns   - 2.5ns) * 3/4 / 0.125 = 0     = 0
// 1066:   (1.875ns - 2.5ns) * 3/4 / 0.125 = 3.75  = 3
// 1333:   (1.5ns   - 2.5ns) * 3/4 / 0.125 = 6     = 6
// 1600:   (1.25ns  - 2.5ns) * 3/4 / 0.125 = 7.5   = 7
// 1876:   (1.066ns - 2.5ns) * 3/4 / 0.125 = 8.6   = 8
// 2133:   (0.938ns - 2.5ns) * 3/4 / 0.125 = 9.37  = 9
// 2400:   (0.833ns - 2.5ns) * 3/4 / 0.125 = 9.996 = 10
// 2666:   (0.75ns - 2.5ns) * 3/4 / 0.125 = 10.5 = 10
// 2933:   (0.682ns - 2.5ns) * 3/4 / 0.125 = 10.908 = 11
// 3200:   (0.625ns - 2.5ns) * 3/4 / 0.125 = 11.25 = 11

UINT8 lrDimmModuleDelayAdjust[MAX_SUP_FREQ] = { 0, 2, 3, 5, 6, 7, 7, 8, 8, 9, 9, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 11};

UINT32
DetectDIMMConfig (
                 PSYSHOST host
                 )
/**

  Detect DIMM population

  @param host  - Point to sysHost

  @retval SUCCESS

**/
{
  UINT8               socket;
  UINT8               ch;
  UINT8               dimm;
  UINT8               SPDReg;
  struct dimmNvram    (*dimmNvList)[MAX_DIMM];
  struct channelNvram (*channelNvList)[MAX_CH];
  struct smbDevice    spd;

  socket = host->var.mem.currentSocket;
  channelNvList = GetChannelNvList(host, socket);
  
  InitSpd (host, socket);
  CHIP_FUNC_CALL(host, UpdateAccessMode(host, socket));

  //
  // Initialize common parts of the smbDevice structure for all SPD devices
  //
  spd.compId = SPD;
  spd.address.controller = PLATFORM_SMBUS_CONTROLLER_PROCESSOR;
  spd.address.deviceType = DTI_EEPROM;

  if ((host->var.common.bootMode == NormalBoot) && (host->var.mem.subBootMode == ColdBoot)) {

#ifdef SERIAL_DBG_MSG
    if (checkMsgLevel(host, SDBG_MAX)) {
      getPrintFControl(host);
    }
#endif // SERIAL_DBG_MSG
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                   "Socket | Channel | DIMM |  Bus Segment | SMBUS Address\n"));
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                   "-------|---------|------|--------------|--------------\n"));
#ifdef SERIAL_DBG_MSG
    if (checkMsgLevel(host, SDBG_MAX)) {
      releasePrintFControl(host);
    }
#endif // SERIAL_DBG_MSG
    //
    // Detect DIMMs on each channel
    //
    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled == 0) continue;

        //
        // Detect DIMMs in each slot
        //
      dimmNvList = GetDimmNvList(host, socket, ch);

      for (dimm = 0; dimm < host->var.mem.socket[socket].channelList[ch].numDimmSlots; dimm++) {

        OutputExtendedCheckpoint((host, STS_DIMM_DETECT, SUB_INIT_SMB, (UINT16)((socket << 8)|(ch << 4)|(dimm))));
        //
        // Init to DIMM not present
        //
        (*dimmNvList)[dimm].dimmPresent = 0;

        CHIP_FUNC_CALL(host, GetSmbAddress(host, socket, ch, dimm, &spd));

        //
        // Read SPD device (type 0x0B) to detect DIMM presence
        //
#ifdef SERIAL_DBG_MSG
        if (checkMsgLevel(host, SDBG_MAX)) {
          getPrintFControl(host);
        }
#endif
        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       "   %d   |    %2d   |   %d  |      %d       |       %d      - ", socket, ch, dimm,
                       spd.address.busSegment, spd.address.strapAddress));
        if (ReadSpd (host, socket, ch, dimm, SPD_KEY_BYTE, &SPDReg) == SUCCESS) {
#ifdef MEM_NVDIMM_EN
          (*dimmNvList)[dimm].nvDimmType = CoreDetectNVDIMM(host, socket, ch, dimm, spd);
          if ((*dimmNvList)[dimm].nvDimmType) {
            MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
              "Present[NVDIMM-N]\n"));
          } else {
            MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
              "Present\n"));
        }
#else
          MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
            "Present\n"));
#endif
#ifdef SERIAL_DBG_MSG
          if (checkMsgLevel(host, SDBG_MAX)) {
            releasePrintFControl(host);
          }
#endif
          (*dimmNvList)[dimm].keyByte = SPDReg;
          //
          // We know a DIMM is present if we arrive here
          //
          (*dimmNvList)[dimm].dimmPresent = 1;

          //
          // Increment the number of DIMMs on this channel
          //
          (*channelNvList)[ch].maxDimm++;
          //
          // Get TimeBases
          //

          CHIP_FUNC_CALL(host, DetectSpdTypeDIMMConfig(host, socket, ch, dimm, SPDReg));


        } // DIMM present end
#ifdef SERIAL_DBG_MSG
        else {
          if (checkMsgLevel(host, SDBG_MAX)) {
            MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                          "Not Present\n"));
            releasePrintFControl(host);
          }
        }
#endif
      } // dimm loop
    } // ch loop
    // Wait for NVMCTLR to indicate that Mailbox interface is ready
    OutputCheckpoint (host, STS_DIMM_DETECT, SUB_MAILBOX_READY, 0);
    CHIP_FUNC_CALL(host, WaitForMailboxReady (host, socket));

    CHIP_FUNC_CALL(host, PostDimmDetectChipHook (host, socket));

    //HSD 5331430: Display FalconValley firmware revision information in serial port
    CHIP_FUNC_CALL(host, DisplayFnvInfo(host));
  } // if cold boot

  return SUCCESS;
}


/**

  Init SPD page select and cached values

  @param host       - Pointer to sysHost
  @param socket     - Socket ID

  @retval none

**/
VOID
InitSpd (
  PSYSHOST         host,
  UINT8            socket
  )
{
  UINT8             seg;

  // Call chip hook to establish DDR4 or DDR3 SPD state
  if (CHIP_FUNC_CALL(host, GetDdr4SpdPageEn (host, socket))) {
    host->nvram.mem.socket[socket].ddr4SpdPageEn = 1;

    //
    // Bypass page SPA cache for first read. This is necessary after SPD device power cycle.
    //
    for (seg = 0; seg < MAX_IMC; seg++) {
      host->nvram.mem.socket[socket].imc[seg].spdPageAddr = 2;
    }
  }
} // InitSpd


/**

  Read SPD byte from Serial EEPROM

  @param host       - Pointer to sysHost
  @param socket     - Socket ID
  @param ch         - Channel on socket
  @param dimm       - DIMM on channel
  @param byteOffset - Byte Offset to read
  @param data       - Pointer to data

  @retval status

**/
UINT32
ReadSpd (
  PSYSHOST         host,
  UINT8            socket,
  UINT8            ch,
  UINT8            dimm,
  UINT16           byteOffset,
  UINT8            *data
  )
{
  struct smbDevice spd;
  struct smbDevice spa;
  INT16 spdOffset = 0;
  UINT8 temp = 0;
  UINT32 status;

  spd.compId = SPD;
  spd.address.controller = PLATFORM_SMBUS_CONTROLLER_PROCESSOR;
  spd.address.deviceType = DTI_EEPROM;
  CHIP_FUNC_CALL(host, GetSmbAddress(host, socket, ch, dimm, &spd));

  if (host->nvram.mem.socket[socket].ddr4SpdPageEn) {
    if (byteOffset > 255) {

      // If current page does not match desired page
      if (host->nvram.mem.socket[socket].imc[spd.address.busSegment].spdPageAddr != 1) {

        // Update current page
        host->nvram.mem.socket[socket].imc[spd.address.busSegment].spdPageAddr = 1;

        // Set page
        spa = spd;
        spa.address.deviceType = DTI_WP_EEPROM;
        spa.address.strapAddress = SPD_EE_PAGE_SELECT_1;

        // Write to SPA
        CHIP_FUNC_CALL(host, WriteSmb (host, socket, spa, 0, &temp));
      }
      spdOffset = -256;

    } else {

      // If current page does not match desired page
      if (host->nvram.mem.socket[socket].imc[spd.address.busSegment].spdPageAddr != 0) {

        // Update current page
        host->nvram.mem.socket[socket].imc[spd.address.busSegment].spdPageAddr = 0;

        // Set page
        spa = spd;
        spa.address.deviceType = DTI_WP_EEPROM;
        spa.address.strapAddress = SPD_EE_PAGE_SELECT_0;

        // Write to SPA
        CHIP_FUNC_CALL(host, WriteSmb (host, socket, spa, 0, &temp));
      }
    }
  }

  // Read from SPD
  status = CHIP_FUNC_CALL(host, ReadSmb (host, socket, spd, (UINT8)(byteOffset + spdOffset), data));

  return status;

} //ReadSpd


UINT16
TimeBaseToDCLK (
               PSYSHOST  host,
               UINT8     socket,
               UINT8     ch,
               UINT8     dimm,
               UINT16    mtbTime,
               UINT16    ftbTime
               )
/**

  Converts MTB/FTB to DCLK

  @param mtbTime   - Time in MTB (Medium Time Base)
  @param ftbTime   - Time in FTB (Fine Time Base)

  @retval Time in DCLKs

**/
{
  UINT16              mtb;
  UINT16              ftb;
  UINT32              tempVal;

  mtb = 1250;
  ftb = 10;

  if (ftbTime & BIT7) {
    tempVal = (UINT32)((mtbTime * mtb) + ((ftbTime - 256) * ftb));
  } else {
    tempVal = (UINT32)((mtbTime * mtb) + (ftbTime * ftb));
  }

  //
   // Round up to next cycle subtracting the guardband of 0.01 clocks described in the SPD specification
  //
  tempVal = ((tempVal + GettCK(host, socket) - 10) * 100) / GettCK(host, socket);

  //
  // 0.01 clocks guardband
  //
  tempVal -= 1;
  tempVal = tempVal / 100;

  return(UINT16) tempVal;
} // TimeBaseToDCLK

UINT8
MemReadSmb (
  PSYSHOST         host,
  UINT8            socket,
  struct smbDevice dev,
  UINT8            byteOffset,
  UINT8            *data
  )
{
  return (PlatformReadSmb (host, socket, dev, byteOffset, data));
}

UINT8
MemWriteSmb(
    PSYSHOST         host,
    struct smbDevice dev,
    UINT8            byteOffset,
    UINT8            *data
    )
{
  return (PlatformWriteSmb (host, dev, byteOffset, data));
}

struct dimmDevice (*GetChDimmList (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     ch
  ))[MAX_DIMM]
{
  return (&host->var.mem.socket[socket].channelList[ch].dimmList);
}
