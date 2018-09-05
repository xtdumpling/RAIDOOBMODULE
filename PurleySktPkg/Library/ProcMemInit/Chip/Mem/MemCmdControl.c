//****************************************************************************
//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//****************************************************************************
//  File History
//
//  Rev. 1.01
//      Bug Fixed:  Fix and Enhance the warning message log by correct DIMM slot location.
//      Reason:     Memory Training Margin Small not shown on screen and log to SEL
//      Auditor:    Max Mu
//      Date:       Jun/30/2017
//
//  Rev. 1.00
//      Bug Fixed:  Fixed DIMM error location incorrect when in memory margin too small case.
//      Reason:     
//      Auditor:    Jacker Yeh
//      Date:       Mar/17/2017
//
//*****************************************************************************

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
 * Copyright 2006 - 2016 Intel Corporation All Rights Reserved.
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

#include "SysHostChip.h"
#include "SysFunc.h"
#include "CmdCtlDelay.h"
#include "RcRegs.h"
#include "FnvAccess.h"
#include "MemApiSkx.h"


/* New SKX CMD/CTL definitions

  Signal  PI group            CSR Name                Changed
 ------------------------------------------------------------
  Unused  6a(cmdctla)         DDRCRCMDPICODING3_CMDN  *
  CS3
  Unused  5a(cmdctla)         DDRCRCMDPICODING2_CMDN  *
  CS2
  ODT1    4a(cmdctla)         DDRCRCMDPICODING2_CMDN  *
  CS1
  ODT0    3a(cmdctla)         DDRCRCMDPICODING2_CMDN  *
  CS0
  CS6/C0  2a(cmdctla)         DDRCRCMDPICODING_CMDN   *
  CS7/C1
  C2      1a(cmdctla)         DDRCRCMDPICODING_CMDN
  MA17
  ODT3    0a(cmdctla)         DDRCRCMDPICODING_CMDN   *
  CS5
------------------------------------------------------------
  ODT2    4b(cmdctlb)         DDRCRCMDPICODING2_CKE   *
  CS4
  MA13    3b(cmdctlb-north)   DDRCRCMDPICODING2_CKE
  MA15
  MA14    2b(cmdctlb-north)   DDRCRCMDPICODING_CKE
  MA16
  BA1     1b(cmdctlb-north)   DDRCRCMDPICODING_CKE
  MA10
  BA0     0b(cmdctlb- North)  DDRCRCMDPICODING_CKE    *
  MA0
------------------------------------------------------------
 Add 7 to these Pi group numbers:
  Unused  0b'(cmdctlb-south)  DDRCRCMDPICODING_CTL    *
  PAR
  MA3     1b'(cmdctlb-south)  DDRCRCMDPICODING_CTL    *
  MA1
  MA4     2b'(cmdctlb-south)  DDRCRCMDPICODING_CTL    *
  MA2
  MA6     3b'(cmdctlb-south)  DDRCRCMDPICODING2_CTL   *
  MA5
  MA7     4b'(cmdctlb-south)  DDRCRCMDPICODING2_CTL   *
  MA8
------------------------------------------------------------
 Add 7 to these Pi group numbers:
  MA12    0a'(cmctla-south)   DDRCRCMDPICODING_CMDS   *
  MA9
  MA11    1a'(cmdctla-south)  DDRCRCMDPICODING_CMDS   *
  BG1
  BG0     2a'(cmdctla-south)  DDRCRCMDPICODING_CMDS   *
  ACT_N
  ALERT_N 3a'(cmdctla-south)  DDRCRCMDPICODING2_CMDS  *
  CKE3
  Unused  4a'(cmdctla-south)  DDRCRCMDPICODING2_CMDS  *
  CKE2
  Unused  5a'(cmdctla-south)  DDRCRCMDPICODING2_CMDS  *
        CKE0
        Unused  6a'(cmdctla-south)  DDRCRCMDPICODING3_CMDS  *
  CKE1
 -----------------------------------------------------------
*/

//
// Internal data types
//

struct signalIOGroup signals[] = {
  { RAS_N,  CmdGrp0, {2, SIDE_B} },
  { CAS_N,  CmdGrp0, {3, SIDE_B} },
  { WE_N,   CmdGrp0, {2, SIDE_B} },
  { BA0,    CmdGrp0, {0, SIDE_B} },
  { BA1,    CmdGrp0, {1, SIDE_B} },
  { A0,     CmdGrp0, {0, SIDE_B} },
  { A1,     CmdGrp0, {8, SIDE_B} },
  { A2,     CmdGrp1, {9, SIDE_B} },
  { A3,     CmdGrp0, {8, SIDE_B} },
  { A4,     CmdGrp1, {9, SIDE_B} },
  { A5,     CmdGrp1, {10, SIDE_B} },
  { A6,     CmdGrp1, {10, SIDE_B} },
  { A7,     CmdGrp1, {11, SIDE_B} },
  { A8,     CmdGrp1, {11, SIDE_B} },
  { A9,     CmdGrp1, {7, SIDE_A} },
  { A10,    CmdGrp0, {1, SIDE_B} },
  { A11,    CmdGrp2, {8, SIDE_A} },
  { A12,    CmdGrp1, {7, SIDE_A} },
  { A13,    CmdGrp0, {3, SIDE_B} },
  { A14,    CmdGrp0, {2, SIDE_B} },
  { A15,    CmdGrp0, {3, SIDE_B} },
  { A16,    CmdGrp0, {2, SIDE_B} },
  { A17,    CmdGrp2, {1, SIDE_A} },
  { PAR,    CmdGrp0, {7, SIDE_B} },
  { BG0,    CmdGrp2, {9, SIDE_A} },
  { BG1,    CmdGrp2, {8, SIDE_A} },
  { ACT_N,  CmdGrp2, {9, SIDE_A} },
  { CS0_N,  CtlGrp0, {3, SIDE_A} },
  { CS1_N,  CtlGrp1, {4, SIDE_A} },
  { CS2_N,  CtlGrp0, {5, SIDE_A} },
  { CS3_N,  CtlGrp0, {6, SIDE_A} },
  { CS4_N,  CtlGrp2, {4, SIDE_B} },
  { CS5_N,  CtlGrp3, {0, SIDE_A} },
  { CS6_N,  CtlGrp2, {2, SIDE_A} },
  { CS7_N,  CtlGrp2, {2, SIDE_A} },
  { CKE0,   CtlGrp0, {12, SIDE_A} },
  { CKE1,   CtlGrp1, {13, SIDE_A} },
  { CKE2,   CtlGrp2, {11, SIDE_A} },
  { CKE3,   CtlGrp3, {10, SIDE_A} },
  { ODT0,   CtlGrp0, {3, SIDE_A} },
  { ODT1,   CtlGrp1, {4, SIDE_A} },
  { ODT2,   CtlGrp2, {4, SIDE_B} },
  { ODT3,   CtlGrp3, {0, SIDE_A} },
  { C0,     CmdGrp2, {2, SIDE_A} },
  { C1,     CmdGrp2, {2, SIDE_A} },
  { C2,     CmdGrp2, {1, SIDE_A} }
};

#define MAX_SIGNALS   (sizeof signals / sizeof (struct signalIOGroup))


// Map Platform CMD Groups to IO CMD Groups

//MA17/C2, MA15/MA13, MA16/MA14, MA10/BA1, MA0/BA0, PAR
struct ioGroup platformCmdGroup0[MAX_CMD_GROUP0] = {{1, SIDE_A},{3, SIDE_B},{2, SIDE_B},{1, SIDE_B},{0, SIDE_B},{7, SIDE_B}};

//MA1/MA3, MA2/MA4, MA5/MA6
struct ioGroup platformCmdGroup1[MAX_CMD_GROUP1] = {{8, SIDE_B},{9, SIDE_B},{10, SIDE_B}};

//MA7/MA8, MA9/MA12, BG1/MA11, ACT_N/BG0
struct ioGroup platformCmdGroup2[MAX_CMD_GROUP2] = {{11, SIDE_B},{7, SIDE_A},{8, SIDE_A},{9, SIDE_A}};


struct ioGroup platformCmdGroupAll[MAX_CMD_GROUPALL] = {
  {1, SIDE_A},{3, SIDE_B},{2, SIDE_B},{1, SIDE_B},{0, SIDE_B},{7, SIDE_B},
  {8, SIDE_B},{9, SIDE_B},{10, SIDE_B},
  {11, SIDE_B},{7, SIDE_A},{8, SIDE_A},{9, SIDE_A}
};


// Map Platform CTL Groups to IO CTL Groups
//ODT0, CKE0, CS0, CS2, CS3
struct ioGroup platformCtlGroup0[MAX_CTL_GROUP0] = {{3, SIDE_A},{12, SIDE_A},{5, SIDE_A},{6, SIDE_A}};
//ODT1, CKE1, CS1
struct ioGroup platformCtlGroup1[MAX_CTL_GROUP1] = {{4, SIDE_A},{13, SIDE_A}};
//ODT2, CKE2, CS4, CS6, CS7
struct ioGroup platformCtlGroup2[MAX_CTL_GROUP2] = {{4, SIDE_B},{11, SIDE_A},{2, SIDE_A}};
//ODT3, CKE3, CS5
struct ioGroup platformCtlGroup3[MAX_CTL_GROUP3] = {{0, SIDE_A},{10, SIDE_A}};

struct ioGroup platformCtlGroupAll[MAX_CTL_GROUPALL] = {
  {3, SIDE_A},{12, SIDE_A},{5, SIDE_A},{6, SIDE_A},
  {4, SIDE_A},{13, SIDE_A},
  {4, SIDE_B},{11, SIDE_A},{2, SIDE_A},
  {0, SIDE_A},{10, SIDE_A}
};

struct ioGroup platformCmdCtlGroupAll[MAX_CMDCTL_GROUPALL] = {
  {1, SIDE_A},{3, SIDE_B},{2, SIDE_B},{1, SIDE_B},{0, SIDE_B},{7, SIDE_B},
  {8, SIDE_B},{9, SIDE_B},{10, SIDE_B},
  {11, SIDE_B},{7, SIDE_A},{8, SIDE_A},{9, SIDE_A},
  {3, SIDE_A},{12, SIDE_A},{5, SIDE_A},{6, SIDE_A},
  {4, SIDE_A},{13, SIDE_A},
  {4, SIDE_B},{11, SIDE_A},{2, SIDE_A},
  {0, SIDE_A},{10, SIDE_A}
};


//-----------------------------------------------------------------------------------
//
// This const array use as DDRIO convert table. Each index will be convert to real address.
//


// Define maximums for CMD, CTL and CK for both processors
#define MAX_CMD_DELAY   (3*64+63)   // Logic value of 3 and Pi value of 63
#define MAX_CTL_DELAY   (3*64+63)   // Logic value of 3 and Pi value of 63
#define MAX_CK_DELAY    128
#define MAX_CMD_DELAY_FNV   (1*64+63)   // Logic value of 1 and Pi value of 63
//-----------------------------------------------------------------------------------

//
// Local Prototypes
//
static void       EvaluateCMDMargins(PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 ckEnabled[MAX_CLK], UINT8 ctlIndex[MAX_CLK],
  INT16 cmdLeft[][MAX_CLK], INT16 cmdRight[][MAX_CLK], GSM_GT cmdGroup[][MAX_CLK], GSM_CSN cmdSignal[][MAX_CLK], INT16 cmdOffset[]);
static MRC_STATUS SignalToPiGroup(PSYSHOST host, GSM_CSN sig, struct signalIOGroup *signalGroup);
static MRC_STATUS GetSetCmdVrefHostSide (PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 mode, INT16 *value);
static MRC_STATUS GetSetCmdVrefFnv (PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, UINT8 mode, INT16 *value);
static MRC_STATUS GetSetCmdVrefFnvBackside(PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, UINT8 mode, INT16 *value);
static UINT8      ClkSwapFix(PSYSHOST host, UINT8 socket, UINT8 clk);
void       Resync(PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 clk);
void       RelockPll(PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 clk);
/**
  Multi-use function to either get or set signal delays based on the provided group number

  @param host:     Pointer to SYSHOST
  @param socket:   Socket number
  @param ch:       Channel number
  @param level:    IO level to access
  @param sig:      Enumerated signal name
  @param mode:     GSM_READ_CSR - Read the data from hardware and not cache
                   GSM_READ_ONLY - Do not write
                   GSM_WRITE_OFFSET - Write offset and not value
                   GSM_FORCE_WRITE - Force the write
  @param value:    Absolute value or offset selected by GSM_WRITE_OFFSET

  @retval MRC_STATUS

**/
MRC_STATUS
GetSetSignal (
             PSYSHOST  host,
             UINT8     socket,
             UINT8     ch,
             GSM_LT    level,
             GSM_CSN   sig,
             UINT8     mode,
             INT16     *value
             )
{
  MRC_STATUS            status;
#ifdef SERIAL_DBG_MSG
  //char                  strBuf0[128];
  //char                  strBuf1[128];
#endif  // SERIAL_DBG_MSG
  struct signalIOGroup  signalGroup;

  status = MRC_STATUS_SUCCESS;
  //
  // Get the silicon pi group this signal belongs to
  //
  status = SignalToPiGroup (host, sig, &signalGroup);

  //MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
  //              "signal %s, platform group %s, num %d, side %d\n",
  //              GetSignalStr(signalGroup.sig, (char *)&strBuf0),
  //              GetPlatformGroupStr(host, signalGroup.platformGroup, (char *)&strBuf1),
  //              signalGroup.group.num, signalGroup.group.side));

  if (status == MRC_STATUS_SUCCESS) {
    status = GetSetCmdDelay (host, socket, ch, signalGroup.group.num, signalGroup.group.side, mode, value);
  }

  return status;
} // GetSetSignal

/**
  Gets the silicon pi group from the signal name

  @param host:     Pointer to SYSHOST
  @param sig:      Enumerated signal name

  @retval MRC_STATUS

**/
static MRC_STATUS
SignalToPiGroup (
                PSYSHOST              host,
                GSM_CSN               sig,
                struct signalIOGroup  *signalGroup
                )
{
  UINT8       sigLoop;
  MRC_STATUS  status;
#ifdef SERIAL_DBG_MSG
  char        strBuf0[128];
#endif  // SERIAL_DBG_MSG

  for (sigLoop = 0; sigLoop < MAX_SIGNALS; sigLoop++) {
    if (signals[sigLoop].sig == sig) {
      *signalGroup = signals[sigLoop];
      break;
    }
  } // sigLoop loop

  if (sigLoop == MAX_SIGNALS) {
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                   "Unable to find signal %s\n", GetSignalStr(sig, (char *)&strBuf0)));
    status = MRC_STATUS_SIGNAL_NOT_SUPPORTED;
  } else {
    status = MRC_STATUS_SUCCESS;
  }

  return status;
} // SignalToPiGroup

void UpdateMinMax(
                 UINT16  val,
                 UINT16  *minVal,
                 UINT16  *maxVal
                 )
/**
  Updates provided min and max values based on an input

  @param val:    New value to evaluate
  @param minVal: Current minimum value
  @param maxVal: Current maximum value

  @retval minVal and maxVal

**/
{
  if (val < *minVal) *minVal = val;
  if (val > *maxVal) *maxVal = val;
} // UpdateMinMax

/**
  Muli-use function to either get or set control delays based on the provided group number

  @param host:     Pointer to SYSHOST
  @param socket:   Socket number
  @param ch:       Channel number
  @param group:    Group number
  @param mode:     GSM_READ_CSR - Read the data from hardware and not cache
                   GSM_READ_ONLY - Do not write
                   GSM_WRITE_OFFSET - Write offset and not value
                   GSM_FORCE_WRITE - Force the write
  @param value:    Value to program
  @param *minVal:  Current minimum control delay
  @param *maxVal:  Current maximum control delay


  @retval minVal, maxVal and MRC_STATUS

**/
MRC_STATUS
GetSetCtlGroupDelay (
                    PSYSHOST  host,
                    UINT8     socket,
                    UINT8     ch,
                    GSM_GT    group,
                    UINT8     mode,
                    INT16     *value,
                    UINT16    *minVal,
                    UINT16    *maxVal
                    )
{
  UINT8           maxPlatformGroup;
  UINT8           maxIoGroup;
  UINT8           iog;
  UINT8           iogNum;
  UINT8           iogSide;
  struct ioGroup  *iogPtr;
  MRC_STATUS      status = MRC_STATUS_SUCCESS;

  maxPlatformGroup = (UINT8)group + 1;

  // Associate CTL groups for RDIMM
  if (host->nvram.mem.dimmTypePresent == RDIMM) {    // or LRDIMM
    if ((group == CtlGrp0) || (group == CtlGrp2)) maxPlatformGroup++;
  }

  for ( ; group < maxPlatformGroup; group++) {

    switch (group) {
    case CtlGrp0:
      maxIoGroup = MAX_CTL_GROUP0;
      iogPtr = platformCtlGroup0;
      break;
    case CtlGrp1:
      maxIoGroup = MAX_CTL_GROUP1;
      iogPtr = platformCtlGroup1;
      break;
    case CtlGrp2:
      maxIoGroup = MAX_CTL_GROUP2;
      iogPtr = platformCtlGroup2;
      break;
    case CtlGrp3:
      maxIoGroup = MAX_CTL_GROUP3;
      iogPtr = platformCtlGroup3;
      break;
    case CtlAll:
      maxIoGroup = MAX_CTL_GROUPALL;
      iogPtr = platformCtlGroupAll;
      break;
    default:
      // Assert error
      RC_ASSERT(FALSE, ERR_RC_INTERNAL, RC_ASSERT_MINOR_CODE_37);
      maxIoGroup = MAX_CTL_GROUPALL;
      iogPtr = platformCtlGroupAll;
    }

    if (status == MRC_STATUS_SUCCESS) {
      for (iog = 0; iog < maxIoGroup; iog++) {

        // Get IO Group number and side
        iogNum = (iogPtr + iog)->num;
        iogSide = (iogPtr + iog)->side;

        // Program the IO delay offset
        status = GetSetCmdDelay (host, socket, ch, iogNum, iogSide, mode, value);

        // Update min/max values
        UpdateMinMax(*value, minVal, maxVal);
      } // iog loop
    } // status
  }

  return status;
} // GetSetCtlGroupDelay

/**
  Muli-use function to either get or set control delays based on the provided group number

  @param host:     Pointer to SYSHOST
  @param socket:   Socket number
  @param ch:       Channel number
  @param dimm:     DIMM number
  @param ioGroup:  Group number
  @param mode:     GSM_READ_CSR - Read the data from hardware and not cache
                   GSM_READ_ONLY - Do not write
                   GSM_WRITE_OFFSET - Write offset and not value
                   GSM_FORCE_WRITE - Force the write
  @param value:    Value to program or offset
  @param *minVal:  Current minimum control delay
  @param *maxVal:  Current maximum control delay

  @retval Pi delay value

**/
MRC_STATUS
GetSetCtlDelayFnv (
               PSYSHOST host,
               UINT8    socket,
               UINT8    ch,
               UINT8    dimm,
               UINT8    mode,
               INT16    *value,
               UINT16   *minDelay,
               UINT16   *maxDelay
               )
{
  UINT8                                       logicVal;
  UINT8                                       piVal;
  UINT16                                      curVal;
  UINT16                                      piDelay;
  UINT16                                      maxVal;
  INT16                                       tmp;
  struct socketNvram                          *socketNvram;
  DDRCRCMDPICODING_SA_FNV_DDRIO_DAT7_CH_STRUCT ddrCrCmdPiCodingFnv;


  socketNvram = &host->nvram.mem.socket[socket];

  //
  // Maximum CMD delay
  //
  maxVal = MAX_CMD_DELAY_FNV;

  //
  // Get register data
  //
  if (mode & GSM_READ_CSR) {
    GetSetFnvIO(host, socket, ch, dimm, DDRCRCMDPICODING_SA_FNV_DDRIO_DAT7_CH_REG, &ddrCrCmdPiCodingFnv.Data, 0, FNV_IO_READ, 0xF);
  } else {
    ddrCrCmdPiCodingFnv.Data = socketNvram->channelList[ch].ddrCrCmdPiCodingFnv;
  }

  logicVal = (UINT8)ddrCrCmdPiCodingFnv.Bits.cmdpilogicdelay0;
  piVal = (UINT8)ddrCrCmdPiCodingFnv.Bits.cmdpicode0;

  // Combine into delay
  curVal = (logicVal * 64) + piVal;

  // Read only?
  if (mode & GSM_READ_ONLY) {
    *value = (INT16)curVal;
  }
  // Handle writes
  else {

    // Adjust the current CMD delay value by value
    if (mode & GSM_WRITE_OFFSET) {
      tmp = curVal + *value;
    } else {
      //
      // Absolute value
      //
      tmp = *value;
    }

    // Check for boundaries
    // value is INT16 and can be a negative value
    // Value and curVal are UINT8
    // Do not allow negative wraparound (curVal = 3 and offset = -4)
    if (tmp > 0) {
      piDelay = tmp;
    } else {
      piDelay = 0;
    }
    // Ensure we do not exceed maximums (caller should enforce this)
    if (piDelay > maxVal) {
      piDelay = maxVal;
    }

    // Program the IO delay
    logicVal = (UINT8) (piDelay / 64);
    piVal    = (UINT8) (piDelay % 64);

    // BCOM[1:0]
    ddrCrCmdPiCodingFnv.Bits.cmdpilogicdelay0 = logicVal;
    ddrCrCmdPiCodingFnv.Bits.cmdpicode0 = piVal;

    //
    // Write it back if the current data does not equal the cache value or if in force write mode
    //
    if ((socketNvram->channelList[ch].ddrCrCmdPiCodingFnv != ddrCrCmdPiCodingFnv.Data) || (mode & GSM_FORCE_WRITE)) {
      GetSetFnvIO(host, socket, ch, dimm, DDRCRCMDPICODING_SA_FNV_DDRIO_DAT7_CH_REG, &ddrCrCmdPiCodingFnv.Data, 0, FNV_IO_WRITE, 0xF);
    }

  }

  //
  // Save to cache
  //
  if (mode & GSM_UPDATE_CACHE) {
    socketNvram->channelList[ch].ddrCrCmdPiCodingFnv = ddrCrCmdPiCodingFnv.Data;
  }

  // Hack for conformity
  *minDelay = *value;
  *maxDelay = *value;

  return MRC_STATUS_SUCCESS;
} // GetSetCtlDelayFnv

/**
  Muli-use function to either get or set command vref

  @param host:     Pointer to SYSHOST
  @param socket:   Socket number
  @param ch:       Channel number
  @param dimm:     Dimm slot
  @param group:    Group number
  @param mode:     GSM_READ_CSR - Read the data from hardware and not cache
                   GSM_READ_ONLY - Do not write
                   GSM_WRITE_OFFSET - Write offset and not value
                   GSM_FORCE_WRITE - Force the write
  @param value:    Value to program
  @param *minVal:  Current minimum command delay
  @param *maxVal:  Current maximum command delay


  @retval minVal and maxVal

**/
MRC_STATUS
GetSetCmdVref (
                PSYSHOST host,
                UINT8    socket,
                UINT8    ch,
                UINT8    dimm,
                GSM_LT   level,
                UINT8 mode,
                INT16 *value
)
{
  MRC_STATUS status;
  struct dimmNvram (*dimmNvList)[MAX_DIMM];

  dimmNvList = GetDimmNvList(host, socket, ch);

  if ((*dimmNvList)[dimm].aepDimmPresent) {
#ifdef LRDIMM_SUPPORT
    if (level == LrbufLevel) {
      status = GetSetCmdVrefFnvBackside(host, socket, ch, dimm, mode, value);
    } else 
#endif //LRDIMM_SUPPORT
    {
      status = GetSetCmdVrefFnv(host, socket, ch, dimm, mode, value);
    }
  } else {
    status = GetSetCmdVrefHostSide (host, socket, ch, mode, value);
  }

  return status;
}
/**
  Muli-use function to either get or set command delays based on the provided group number

  @param host:     Pointer to SYSHOST
  @param socket:   Socket number
  @param ch:       Channel number
  @param group:    Group number
  @param mode:     GSM_READ_CSR - Read the data from hardware and not cache
                   GSM_READ_ONLY - Do not write
                   GSM_WRITE_OFFSET - Write offset and not value
                   GSM_FORCE_WRITE - Force the write
  @param value:    Value to program
  @param *minVal:  Current minimum command delay
  @param *maxVal:  Current maximum command delay


  @retval minVal and maxVal

**/
MRC_STATUS
GetSetCmdGroupDelay (
                    PSYSHOST  host,
                    UINT8     socket,
                    UINT8     ch,
                    GSM_GT    group,
                    UINT8     mode,
                    INT16     *value,
                    UINT16    *minVal,
                    UINT16    *maxVal
                    )
{
  UINT8           maxIoGroup;
  UINT8           iog;
  UINT8           iogNum;
  UINT8           iogSide;
  MRC_STATUS      status = MRC_STATUS_SUCCESS;
  struct ioGroup  *iogPtr;

  switch (group) {
    case CmdGrp0:
      maxIoGroup = MAX_CMD_GROUP0;
      iogPtr = platformCmdGroup0;
      break;
    case CmdGrp1:
      maxIoGroup = MAX_CMD_GROUP1;
      iogPtr = platformCmdGroup1;
      break;
    case CmdGrp2:
      maxIoGroup = MAX_CMD_GROUP2;
      iogPtr = platformCmdGroup2;
      break;
    case CmdAll:
      maxIoGroup = MAX_CMD_GROUPALL;
      iogPtr = platformCmdGroupAll;
      break;
    default:
      maxIoGroup = MAX_CMD_GROUPALL;
      iogPtr = platformCmdGroupAll;
      status = MRC_STATUS_GROUP_NOT_SUPPORTED;
      break;
    }

  if (status == MRC_STATUS_SUCCESS) {
    for (iog = 0; iog < maxIoGroup; iog++) {

      // Get IO Group number and side
      iogNum = (iogPtr + iog)->num;
      iogSide = (iogPtr + iog)->side;

      status = GetSetCmdDelay (host, socket, ch, iogNum, iogSide, mode, value);

      // Update min/max values
      UpdateMinMax(*value, minVal, maxVal);
    }
  } // status
  return status;
} // GetSetCmdGroupDelay

/**
  Muli-use function to either get or set command delays based on the provided group number

  @param host:     Pointer to SYSHOST
  @param socket:   Socket number
  @param ch:       Channel number
  @param dimm:     DIMM number
  @param group:    Group number
  @param mode:     GSM_READ_CSR - Read the data from hardware and not cache
                   GSM_READ_ONLY - Do not write
                   GSM_WRITE_OFFSET - Write offset and not value
                   GSM_FORCE_WRITE - Force the write
  @param value:    Value to program
  @param *minVal:  Current minimum command delay
  @param *maxVal:  Current maximum command delay


  @retval minVal and maxVal

**/
MRC_STATUS
GetSetCmdGroupDelayFnv (
                    PSYSHOST  host,
                    UINT8     socket,
                    UINT8     ch,
                    UINT8     dimm,
                    GSM_GT    group,
                    UINT8     mode,
                    INT16     *value,
                    UINT16    *minVal,
                    UINT16    *maxVal
                    )
{
  GSM_GT groups[2];
  UINT8  groupIdx;
  UINT8  numGroups = 0;
  MRC_STATUS      status = MRC_STATUS_SUCCESS;

  switch (group) {
    case CmdGrp3:
    case CmdGrp4:
      groups[0] = group;
      numGroups = 1;
      break;
    case CmdAll:
      groups[0] = CmdGrp3;
      groups[1] = CmdGrp4;
      numGroups = 2;
      break;
    default:
      groups[0] = CmdGrp3;
      groups[1] = CmdGrp4;
      numGroups = 2;
      status = MRC_STATUS_GROUP_NOT_SUPPORTED;
      break;
    }

  if (status == MRC_STATUS_SUCCESS) {
    for (groupIdx = 0; groupIdx < numGroups; groupIdx++) {

      status = GetSetCmdDelayFnv (host, socket, ch, dimm, groups[groupIdx], mode, value);

      // Update min/max values
      UpdateMinMax(*value, minVal, maxVal);
    }
  } // status
  return status;
} // GetSetCmdGroupDelayFnv

UINT32 GetCmdGroupAddress (
                          PSYSHOST host,
                          UINT8    ioGroup,
                          UINT8    side,
                          UINT8    *cmdIndex
                          )
{
  UINT32 address = 0;

  switch (ioGroup) {
  case 0:
  case 1:
  case 2:
    if (side == SIDE_A) {
      //DDRCRCMDPICODING_CMDN
      address = DDRCRCMDPICODING_CMDN_MCIO_DDRIOEXT_REG;
      *cmdIndex = 0;

    } else {
      //DDRCRCMDPICODING_CKE
      address = DDRCRCMDPICODING_CKE_MCIO_DDRIOEXT_REG;
      *cmdIndex = 3;
    }
    break;
  case 3:
  case 4:
  case 5:
    if (side == SIDE_A) {
      //DDRCRCMDPICODING2_CMDN
      address = DDRCRCMDPICODING2_CMDN_MCIO_DDRIOEXT_REG;
      *cmdIndex = 1;
    } else {
      //DDRCRCMDPICODING2_CKE
      address = DDRCRCMDPICODING2_CKE_MCIO_DDRIOEXT_REG;
      *cmdIndex = 4;
    }
    break;
  case 6:
    if (side == SIDE_A) {
      //DDRCRCMDPICODING3_CMDN
      address = DDRCRCMDPICODING3_CMDN_MCIO_DDRIOEXT_REG;
      *cmdIndex = 2;
    } else {
      //address = DDRCRCMDPICODING3_CKE_MCIO_DDRIOEXT_REG;
      //*cmdIndex = 5;
      //Assert on error
      RC_ASSERT(FALSE, ERR_RC_INTERNAL, RC_ASSERT_MINOR_CODE_38);
    }
    break;
  case 7:
  case 8:
  case 9:
    if (side == SIDE_B) {
      //DDRCRCMDPICODING_CTL
      address = DDRCRCMDPICODING_CTL_MCIO_DDRIOEXT_REG;
      *cmdIndex = 6;
    } else {
      //DDRCRCMDPICODING_CMDS
      address = DDRCRCMDPICODING_CMDS_MCIO_DDRIOEXT_REG;
      *cmdIndex = 9;
    }
    break;
  case 10:
  case 11:
  case 12:
    if (side == SIDE_B) {
      //DDRCRCMDPICODING2_CTL
      address = DDRCRCMDPICODING2_CTL_MCIO_DDRIOEXT_REG;
      *cmdIndex = 7;
    } else {
      //DDRCRCMDPICODING2_CMDS
      address = DDRCRCMDPICODING2_CMDS_MCIO_DDRIOEXT_REG;
      *cmdIndex = 10;
    }
    break;
  case 13:
    if (side == SIDE_B) {
      //address = DDRCRCMDPICODING3_CTL_MCIO_DDRIOEXT_REG;
      //*cmdIndex = 8;
      //Assert on error
      RC_ASSERT(FALSE, ERR_RC_INTERNAL, RC_ASSERT_MINOR_CODE_39);

    } else {
      //DDRCRCMDPICODING2_CMDS
      address = DDRCRCMDPICODING3_CMDS_MCIO_DDRIOEXT_REG;
      *cmdIndex = 11;
    }
    break;
  default:
    RC_ASSERT(FALSE, ERR_RC_INTERNAL, RC_ASSERT_MINOR_CODE_40);
    break;
  }
  return address;
}


/**
  Muli-use function to either get or set command delays based on the provided group number

  @param host:     Pointer to SYSHOST
  @param socket:   Socket number
  @param ch:       Channel number
  @param ioGroup:  Group number
  @param side:     Side number
  @param mode:     GSM_READ_CSR - Read the data from hardware and not cache
                   GSM_READ_ONLY - Do not write
                   GSM_WRITE_OFFSET - Write offset and not value
                   GSM_FORCE_WRITE - Force the write
  @param value:    Value to program or offset

  @retval Pi delay value

**/
MRC_STATUS
GetSetCmdDelay (
               PSYSHOST host,
               UINT8    socket,
               UINT8    ch,
               UINT8    ioGroup,
               UINT8    side,
               UINT8    mode,
               INT16    *value
               )
{
  UINT32                                      csrReg = 0;
  UINT32                                      address;
  UINT8                                       regIndex;
  UINT8                                       logicVal;
  UINT8                                       piVal;
  UINT16                                      curVal;
  UINT16                                      piDelay;
  UINT16                                      maxVal = 0;
  INT16                                       tmp;
  struct socketNvram                          *socketNvram;
  DDRCRCMDPICODING_CMDN_MCIO_DDRIOEXT_STRUCT  cmdCsr;
  DDRCRCMDPICODING2_CMDN_MCIO_DDRIOEXT_STRUCT cmdCsr2;
  DDRCRCMDPICODING3_CMDN_MCIO_DDRIOEXT_STRUCT cmdCsr3;


  //MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
  //               "GetSetCmdDelay ioGroup=%d, side=%d\n", ioGroup, side));

  socketNvram = &host->nvram.mem.socket[socket];

  //
  // Maximum CMD delay
  //
  maxVal = MAX_CMD_DELAY;

  //
  // Point to the correct group
  //
  address = GetCmdGroupAddress(host, ioGroup, side, &regIndex);

  cmdCsr.Data = 0;
  cmdCsr2.Data = 0;
  cmdCsr3.Data = 0;

  //
  // Get register data
  //
  if (mode & GSM_READ_CSR) {
    csrReg =  MemReadPciCfgEp (host, socket, ch, address);
    switch (ioGroup % 7) {
    case 0:
    case 1:
    case 2:
      cmdCsr.Data = csrReg;
      break;
    case 3:
    case 4:
    case 5:
      cmdCsr2.Data = csrReg;
      break;
    case 6:
      cmdCsr3.Data = csrReg;
      break;
    default:
      RC_ASSERT(FALSE, ERR_RC_INTERNAL, RC_ASSERT_MINOR_CODE_41);
    }
  } else {
    switch (regIndex % 3) {
    case 0:
      cmdCsr.Data = socketNvram->channelList[ch].cmdCsr[regIndex];
      break;
    case 1:
      cmdCsr2.Data = socketNvram->channelList[ch].cmdCsr[regIndex];
      break;
    case 2:
      cmdCsr3.Data = socketNvram->channelList[ch].cmdCsr[regIndex];
      break;
    }
  }

  switch (ioGroup % 7) {
  case 0:
    logicVal = (UINT8)cmdCsr.Bits.cmdpilogicdelay0;
    piVal = (UINT8)cmdCsr.Bits.cmdpicode0;
    break;

  case 1:
    logicVal = (UINT8)cmdCsr.Bits.cmdpilogicdelay1;
    piVal = (UINT8)cmdCsr.Bits.cmdpicode1;
    break;

  case 2:
    logicVal = (UINT8)cmdCsr.Bits.cmdpilogicdelay2;
    piVal = (UINT8)cmdCsr.Bits.cmdpicode2;
    break;

  case 3:
    logicVal = (UINT8)cmdCsr2.Bits.cmdpilogicdelay3;
    piVal = (UINT8)cmdCsr2.Bits.cmdpicode3;
    break;

  case 4:
    logicVal = (UINT8)cmdCsr2.Bits.cmdpilogicdelay4;
    piVal = (UINT8)cmdCsr2.Bits.cmdpicode4;
    break;

  case 5:
    logicVal = (UINT8)cmdCsr2.Bits.cmdpilogicdelay5;
    piVal = (UINT8)cmdCsr2.Bits.cmdpicode5;
    break;

  case 6:
    logicVal = (UINT8)cmdCsr3.Bits.cmdpilogicdelay6;
    piVal = (UINT8)cmdCsr3.Bits.cmdpicode6;
    break;

  default:
    logicVal = 0;
    piVal = 0;
    // Assert error
  } // switch ioGroup

  // Combine into delay
  curVal = (logicVal * 64) + piVal;

  // Read only?
  if (mode & GSM_READ_ONLY) {
    *value = (INT16)curVal;
  }
  // Handle writes
  else {

    // Adjust the current CMD delay value by value
    if (mode & GSM_WRITE_OFFSET) {
      tmp = curVal + *value;
    } else {
      //
      // Absolute value
      //
      tmp = *value;
    }

    // Check for boundaries
    // value is INT16 and can be a negative value
    // Value and curVal are UINT8
    // Do not allow negative wraparound (curVal = 3 and offset = -4)
    if (tmp > 0) {
      piDelay = tmp;
    } else {
      piDelay = 0;
    }
    // Ensure we do not exceed maximums (caller should enforce this)
    if (piDelay > maxVal) {
      piDelay = maxVal;
    }

    // Program the IO delay
    logicVal = (UINT8) (piDelay / 64);
    piVal    = (UINT8) (piDelay % 64);

    switch (ioGroup % 7) {
    case 0:
      cmdCsr.Bits.cmdpilogicdelay0 = logicVal;
      cmdCsr.Bits.cmdpicode0 = piVal;
      break;

    case 1:
      cmdCsr.Bits.cmdpilogicdelay1 = logicVal;
      cmdCsr.Bits.cmdpicode1 = piVal;
      break;

    case 2:
      cmdCsr.Bits.cmdpilogicdelay2 = logicVal;
      cmdCsr.Bits.cmdpicode2 = piVal;
      break;

    case 3:
      cmdCsr2.Bits.cmdpilogicdelay3 = logicVal;
      cmdCsr2.Bits.cmdpicode3 = piVal;
      break;

    case 4:
      cmdCsr2.Bits.cmdpilogicdelay4 = logicVal;
      cmdCsr2.Bits.cmdpicode4 = piVal;
      break;

    case 5:
      cmdCsr2.Bits.cmdpilogicdelay5 = logicVal;
      cmdCsr2.Bits.cmdpicode5 = piVal;
      break;

    case 6:
      cmdCsr3.Bits.cmdpilogicdelay6 = logicVal;
      cmdCsr3.Bits.cmdpicode6 = piVal;
      break;
    } // switch ioGroup

    switch (regIndex % 3) {
    case 0:
      csrReg = cmdCsr.Data;
      break;
    case 1:
      csrReg = cmdCsr2.Data;
      break;
    case 2:
      csrReg = cmdCsr3.Data;
      break;
    }
    //
    // Write it back if the current data does not equal the cache value or if in force write mode
    //
    if ((socketNvram->channelList[ch].cmdCsr[regIndex] != csrReg) || (mode & GSM_FORCE_WRITE)) {
      MemWritePciCfgEp (host, socket, ch, address, csrReg);
    }

  }

  //
  // Save to cache
  //
  if (mode & GSM_UPDATE_CACHE) {
    switch (regIndex % 3) {
    case 0:
      socketNvram->channelList[ch].cmdCsr[regIndex] = cmdCsr.Data;
      break;
    case 1:
      socketNvram->channelList[ch].cmdCsr[regIndex] = cmdCsr2.Data;
      break;
    case 2:
      socketNvram->channelList[ch].cmdCsr[regIndex] = cmdCsr3.Data;
      break;
    }
  }

  return MRC_STATUS_SUCCESS;
} // GetSetCmdDelay

/**
  Muli-use function to either get or set command delays based on the provided group number

  @param host:     Pointer to SYSHOST
  @param socket:   Socket number
  @param ch:       Channel number
  @param dimm:     DIMM number
  @param ioGroup:  Group number
  @param mode:     GSM_READ_CSR - Read the data from hardware and not cache
                   GSM_READ_ONLY - Do not write
                   GSM_WRITE_OFFSET - Write offset and not value
                   GSM_FORCE_WRITE - Force the write
  @param value:    Value to program or offset
  @param *minVal:  Current minimum control delay
  @param *maxVal:  Current maximum control delay

  @retval Pi delay value

**/
MRC_STATUS
GetSetCmdDelayFnv (
               PSYSHOST host,
               UINT8    socket,
               UINT8    ch,
               UINT8    dimm,
               GSM_GT   group,
               UINT8    mode,
               INT16    *value
               )
{
  UINT8                                       logicVal;
  UINT8                                       piVal;
  UINT16                                      curVal;
  UINT16                                      piDelay;
  UINT16                                      maxVal = 0;
  INT16                                       tmp;
  struct socketNvram                          *socketNvram;
  DDRCRCMDPICODING2_SA_FNV_DDRIO_DAT7_CH_STRUCT ddrCrCmdPiCoding2;


  socketNvram = &host->nvram.mem.socket[socket];

  //
  // Maximum CMD delay
  //
  maxVal = MAX_CMD_DELAY_FNV;

  //
  // Get register data
  //
  if (mode & GSM_READ_CSR) {
    GetSetFnvIO(host, socket, ch, dimm, DDRCRCMDPICODING2_SA_FNV_DDRIO_DAT7_CH_REG, &ddrCrCmdPiCoding2.Data, 0, FNV_IO_READ, 0xF);
  } else {
    ddrCrCmdPiCoding2.Data = socketNvram->channelList[ch].ddrCrCmdPiCodingFnv2;
  }

  switch (group) {
  case CmdGrp3: // BCOM[1:0]
    logicVal = (UINT8)ddrCrCmdPiCoding2.Bits.cmdpilogicdelay3;
    piVal = (UINT8)ddrCrCmdPiCoding2.Bits.cmdpicode3;
    break;
  case CmdGrp4: // BCOM[3:2]
    logicVal = (UINT8)ddrCrCmdPiCoding2.Bits.cmdpilogicdelay4;
    piVal = (UINT8)ddrCrCmdPiCoding2.Bits.cmdpicode4;
    break;
  default:
    logicVal = 0;
    piVal = 0;
    break;
    // Assert error
  }

  // Combine into delay
  curVal = (logicVal * 64) + piVal;

  // Read only?
  if (mode & GSM_READ_ONLY) {
    *value = (INT16)curVal;
  }
  // Handle writes
  else {

    // Adjust the current CMD delay value by value
    if (mode & GSM_WRITE_OFFSET) {
      tmp = curVal + *value;
    } else {
      //
      // Absolute value
      //
      tmp = *value;
    }

    // Check for boundaries
    // value is INT16 and can be a negative value
    // Value and curVal are UINT8
    // Do not allow negative wraparound (curVal = 3 and offset = -4)
    if (tmp > 0) {
      piDelay = tmp;
    } else {
      piDelay = 0;
    }
    // Ensure we do not exceed maximums (caller should enforce this)
    if (piDelay > maxVal) {
      piDelay = maxVal;
    }

    // Program the IO delay
    logicVal = (UINT8) (piDelay / 64);
    piVal    = (UINT8) (piDelay % 64);

    switch (group) {
    case CmdGrp3: // BCOM[1:0]
      ddrCrCmdPiCoding2.Bits.cmdpilogicdelay3 = logicVal;
      ddrCrCmdPiCoding2.Bits.cmdpicode3 = piVal;
      break;
    case CmdGrp4: // BCOM[3:2]
      ddrCrCmdPiCoding2.Bits.cmdpilogicdelay4 = logicVal;
      ddrCrCmdPiCoding2.Bits.cmdpicode4 = piVal;
      break;
    default:
      break;
    }

    //
    // Write it back if the current data does not equal the cache value or if in force write mode
    //
    if ((socketNvram->channelList[ch].ddrCrCmdPiCodingFnv2 != ddrCrCmdPiCoding2.Data) || (mode & GSM_FORCE_WRITE)) {
      GetSetFnvIO(host, socket, ch, dimm, DDRCRCMDPICODING2_SA_FNV_DDRIO_DAT7_CH_REG, &ddrCrCmdPiCoding2.Data, 0, FNV_IO_WRITE, 0xF);
    }

  }

  //
  // Save to cache
  //
  if (mode & GSM_UPDATE_CACHE) {
    socketNvram->channelList[ch].ddrCrCmdPiCodingFnv2 = ddrCrCmdPiCoding2.Data;
  }

  return MRC_STATUS_SUCCESS;
} // GetSetCmdDelayFnv

/**
  Muli-use function to either get or set command vref

  @param host:     Pointer to SYSHOST
  @param socket:   Socket number
  @param ch:       Channel number
  @param mode:     GSM_READ_CSR - Read the data from hardware and not cache
                   GSM_READ_ONLY - Do not write
                   GSM_WRITE_OFFSET - Write offset and not value
                   GSM_FORCE_WRITE - Force the write
  @param value:    Value to program or offset

  @retval Pi delay value

**/
static MRC_STATUS
GetSetCmdVrefFnv (
              PSYSHOST  host,
              UINT8     socket,
              UINT8     ch,
              UINT8     dimm,
              UINT8     mode,
              INT16     *value
              )
{
  INT16                                         curVal;
  INT16                                         vref;
  UINT16                                        maxVal = 0;
  INT16                                         tmp;
  struct socketNvram                            *socketNvram;
  DDRCRCMDCONTROLS_SA_FNV_DDRIO_DAT7_CH_STRUCT  ddrCrDimmVrefControl1;

  socketNvram = &host->nvram.mem.socket[socket];

  //
  // Maximum CMD delay
  //
  maxVal = MEM_CHIP_POLICY_VALUE(host, maxCmdVref);

  //
  // Get register data
  //
  if (mode & GSM_READ_CSR) {
    // Read only 1 FUB to save time. All 3 should be programmed to be the same
    GetSetFnvIO (host, socket, ch, dimm, DDRCRCMDCONTROLS_NA_FNV_DDRIO_DAT7_CH_REG, &ddrCrDimmVrefControl1.Data, 0, FNV_IO_READ, 0xF);
  } else {
    ddrCrDimmVrefControl1.Data = socketNvram->channelList[ch].dimmVrefControlFnv1;
  }

  //
  // Get the CA Vref current setting
  //
  curVal = (INT16)(ddrCrDimmVrefControl1.Bits.rxvref);

  // Read only?
  if (mode & GSM_READ_ONLY) {
    *value = (INT16)curVal;
  }
  // Handle writes
  else {

    // Adjust the current CMD delay value by value
    if (mode & GSM_WRITE_OFFSET) {
      tmp = curVal + *value;
    } else {
      //
      // Absolute value
      //
      tmp = *value;
    }

    // Check for boundaries
    // value is INT16 and can be a negative value
    // Value and curVal are UINT8
    // Do not allow negative wraparound (curVal = 3 and offset = -4)
    if (tmp > 0) {
      vref = tmp;
    } else {
      vref = 0;
    }
    // Ensure we do not exceed maximums
    if (vref > maxVal) {
      vref = maxVal;
    }

    //
    // Program the new value
    //
    ddrCrDimmVrefControl1.Bits.rxvref = vref;

    //
    // Write it back if the current data does not equal the cache value or if in force write mode
    //
    if ((socketNvram->channelList[ch].dimmVrefControlFnv1 != ddrCrDimmVrefControl1.Data) || (mode & GSM_FORCE_WRITE)) {
      //SKX change
      GetSetFnvIO(host, socket, ch, dimm, DDRCRCMDCONTROLS_NA_FNV_DDRIO_DAT7_CH_REG, &ddrCrDimmVrefControl1.Data, 0, FNV_IO_WRITE, 0xF);
      GetSetFnvIO(host, socket, ch, dimm, DDRCRCMDCONTROLS_NB_FNV_DDRIO_DAT7_CH_REG, &ddrCrDimmVrefControl1.Data, 0, FNV_IO_WRITE, 0xF);
      GetSetFnvIO(host, socket, ch, dimm, DDRCRCMDCONTROLS_SB_FNV_DDRIO_DAT7_CH_REG, &ddrCrDimmVrefControl1.Data, 0, FNV_IO_WRITE, 0xF);
    }
  }

  //
  // Save to cache
  //
  if (mode & GSM_UPDATE_CACHE) {
    socketNvram->channelList[ch].dimmVrefControlFnv1 = ddrCrDimmVrefControl1.Data;
  }

  return MRC_STATUS_SUCCESS;
} // GetSetCmdVrefFnv

/**
  Muli-use function to either get or set command vref

  @param host:     Pointer to SYSHOST
  @param socket:   Socket number
  @param ch:       Channel number
  @param mode:     GSM_READ_CSR - Read the data from hardware and not cache
                   GSM_READ_ONLY - Do not write
                   GSM_WRITE_OFFSET - Write offset and not value
                   GSM_FORCE_WRITE - Force the write
  @param value:    Value to program or offset

  @retval Pi delay value

**/
MRC_STATUS
GetSetCmdVrefFnvBackside(
              PSYSHOST  host,
              UINT8     socket,
              UINT8     ch,
              UINT8     dimm,
              UINT8     mode,
              INT16     *value
              )
{
  INT16                                         curVal;
  INT16                                         vref;
  UINT16                                        maxVal = 0;
  INT16                                         tmp;
  struct socketNvram                            *socketNvram;
  DDRCRDIMMVREFCONTROL1_CH_FNV_DDRIO_COMP_STRUCT ddrCrDimmVrefControl1;

  socketNvram = &host->nvram.mem.socket[socket];

  //
  // Maximum CMD delay
  //
  maxVal = MEM_CHIP_POLICY_VALUE(host, maxCmdVref);

  //
  // Get register data
  //
  if (mode & GSM_READ_CSR) {
    GetSetFnvIO(host, socket, ch, dimm, DDRCRDIMMVREFCONTROL1_CH_FNV_DDRIO_COMP_REG, &ddrCrDimmVrefControl1.Data, 0, FNV_IO_READ, 0xF);
  } else {
    ddrCrDimmVrefControl1.Data = socketNvram->channelList[ch].dimmVrefControlFnv1Sa;
  }

  //
  // Get the CA Vref current setting
  //
  curVal = (INT16)(ddrCrDimmVrefControl1.Bits.ch0cavrefctl >> 1);

  // Read only?
  if (mode & GSM_READ_ONLY) {
    *value = (INT16)curVal;
  }
  // Handle writes
  else {

    // Adjust the current CMD delay value by value
    if (mode & GSM_WRITE_OFFSET) {
      tmp = curVal + *value;
    } else {
      //
      // Absolute value
      //
      tmp = *value;
    }

    // Check for boundaries
    // value is INT16 and can be a negative value
    // Value and curVal are UINT8
    // Do not allow negative wraparound (curVal = 3 and offset = -4)
    if (tmp > 0) {
      vref = tmp;
    } else {
      vref = 0;
    }
    // Ensure we do not exceed maximums
    if (vref > maxVal) {
      vref = maxVal;
    }

    //
    // Program the new value
    //
    ddrCrDimmVrefControl1.Bits.ch0cavrefctl = vref << 1;

    //
    // Write it back if the current data does not equal the cache value or if in force write mode
    //
    if ((socketNvram->channelList[ch].dimmVrefControlFnv1Sa != ddrCrDimmVrefControl1.Data) || (mode & GSM_FORCE_WRITE)) {
      GetSetFnvIO(host, socket, ch, dimm, DDRCRDIMMVREFCONTROL1_CH_FNV_DDRIO_COMP_REG, &ddrCrDimmVrefControl1.Data, 0, FNV_IO_WRITE, 0xF);
    }
  }

  //
  // Save to cache
  //
  if (mode & GSM_UPDATE_CACHE) {
    socketNvram->channelList[ch].dimmVrefControlFnv1Sa = ddrCrDimmVrefControl1.Data;
  }

  return MRC_STATUS_SUCCESS;
} // GetSetCmdVrefFnvBackside

/**
  Muli-use function to either get or set command vref

  @param host:     Pointer to SYSHOST
  @param socket:   Socket number
  @param ch:       Channel number
  @param mode:     GSM_READ_CSR - Read the data from hardware and not cache
                   GSM_READ_ONLY - Do not write
                   GSM_WRITE_OFFSET - Write offset and not value
                   GSM_FORCE_WRITE - Force the write
  @param value:    Value to program or offset

  @retval Pi delay value

**/
static MRC_STATUS
GetSetCmdVrefHostSide (
              PSYSHOST  host,
              UINT8     socket,
              UINT8     ch,
              UINT8     mode,
              INT16     *value
              )
{
  INT16                                         curVal;
  INT16                                         vref;
  UINT16                                        maxVal = 0;
  INT16                                         tmp;
  struct socketNvram                            *socketNvram;
  DDRCRDIMMVREFCONTROL1_MCIO_DDRIOEXT_STRUCT    ddrCrDimmVrefControl1;

  socketNvram = &host->nvram.mem.socket[socket];

  //
  // Maximum CMD delay
  //
  maxVal = MEM_CHIP_POLICY_VALUE(host, maxCmdVref);

  //
  // Get register data
  //
  if (mode & GSM_READ_CSR) {
    //SKX change
    ddrCrDimmVrefControl1.Data = MemReadPciCfgEp (host, socket, DDRIOEXT2_CH(GetMCID(host, socket, ch)), DDRCRDIMMVREFCONTROL1_MCIO_DDRIOEXT_REG);
  } else {
    ddrCrDimmVrefControl1.Data = socketNvram->imc[GetMCID(host, socket, ch)].dimmVrefControl1;
  }

  //
  // Get the CA Vref current setting
  //
  switch (ch % 3) {
    case 0:
      curVal = (INT16)(ddrCrDimmVrefControl1.Bits.ch0cavrefctl >> 1);
      break;
    case 1:
      curVal = (INT16)(ddrCrDimmVrefControl1.Bits.ch1cavrefctl >> 1);
      break;
    case 2:
    default:
      curVal = (INT16)(ddrCrDimmVrefControl1.Bits.ch2cavrefctl >> 1);
      break;
  }

  // Read only?
  if (mode & GSM_READ_ONLY) {
    *value = (INT16)curVal;
  }
  // Handle writes
  else {

    // Adjust the current CMD delay value by value
    if (mode & GSM_WRITE_OFFSET) {
      tmp = curVal + *value;
    } else {
      //
      // Absolute value
      //
      tmp = *value;
    }

    // Check for boundaries
    // value is INT16 and can be a negative value
    // Value and curVal are UINT8
    // Do not allow negative wraparound (curVal = 3 and offset = -4)
    if (tmp > 0) {
      vref = tmp;
    } else {
      vref = 0;
    }
    // Ensure we do not exceed maximums
    if (vref > maxVal) {
      vref = maxVal;
    }

    //
    // Program the new value
    //
    switch (ch % 3) {
      case 0:
        ddrCrDimmVrefControl1.Bits.ch0cavrefctl = vref << 1;
        break;
      case 1:
        ddrCrDimmVrefControl1.Bits.ch1cavrefctl = vref << 1;
        break;
      case 2:
        ddrCrDimmVrefControl1.Bits.ch2cavrefctl = vref << 1;
        break;
    }

    //
    // Write it back if the current data does not equal the cache value or if in force write mode
    //
    if ((socketNvram->imc[GetMCID(host, socket, ch)].dimmVrefControl1 != ddrCrDimmVrefControl1.Data) || (mode & GSM_FORCE_WRITE)) {
      //SKX change
      MemWritePciCfgEp (host, socket, DDRIOEXT2_CH(GetMCID(host, socket, ch)), DDRCRDIMMVREFCONTROL1_MCIO_DDRIOEXT_REG, ddrCrDimmVrefControl1.Data);
    }
  }

  //
  // Save to cache
  //
  if (mode & GSM_UPDATE_CACHE) {
    socketNvram->imc[GetMCID(host, socket, ch)].dimmVrefControl1 = ddrCrDimmVrefControl1.Data;
  }

  return MRC_STATUS_SUCCESS;
} // GetSetCmdVref

/**
  Multi-use function to either get or set ERID vref

  @param host:     Pointer to SYSHOST
  @param socket:   Socket number
  @param ch:       Channel number
  @param mode:     GSM_READ_CSR - Read the data from hardware and not cache
                   GSM_READ_ONLY - Do not write
                   GSM_WRITE_OFFSET - Write offset and not value
                   GSM_FORCE_WRITE - Force the write
  @param value:    Value to program or offset

  @retval Pi delay value

**/
MRC_STATUS
GetSetEridVref(
              PSYSHOST  host,
              UINT8     socket,
              UINT8     ch,
              UINT8     mode,
              INT16     *value
)
{
  INT16                                   curVal;
  INT16                                   vref;
  INT16                                   tmp;
  UINT16                                  maxLimit;
  UINT16                                  minLimit;
  UINT16                                  usDelay;
  struct channelNvram                     (*channelNvList)[MAX_CH];
  DDRCRCLKCONTROLS_MCIO_DDRIOEXT_STRUCT   ddrCRClkControls;

  channelNvList = GetChannelNvList(host, socket);

  //
  // Get minimum and maximum value
  //
  GetDataGroupLimits(host, DdrLevel, EridVref, &minLimit, &maxLimit, &usDelay);

  //
  // Get register data
  //
  if (mode & GSM_READ_CSR) {
    ddrCRClkControls.Data = MemReadPciCfgEp(host, socket, ch, DDRCRCLKCONTROLS_MCIO_DDRIOEXT_REG);
  } else {
    ddrCRClkControls.Data = (*channelNvList)[ch].ddrCRClkControls;
  }

  //
  // Get the CA Vref current setting
  //
  curVal = (INT16)ddrCRClkControls.Bits.rxvref;

  // Read only?
  if (mode & GSM_READ_ONLY) {
    *value = curVal;
  } else { // Handle writes
    //
    // Write
    //
    // Adjust the current CMD delay value by offset
    if (mode & GSM_WRITE_OFFSET) {
      //
      // Add the offset to the current value
      //
      tmp = curVal + *value;

      //
      // Make sure we do not exeed the limits
      //
      if (tmp >= minLimit) {
        vref = tmp;
      } else {
        vref = minLimit;
      }
      // Ensure we do not exceed maximums
      if (vref > maxLimit) {
        vref = maxLimit;
      }
    } else {
      //
      // Write absolute value
      //
      vref = *value;
    }

    //
    // Program the new value
    //
    ddrCRClkControls.Bits.rxvref = vref;

    //
    // Write it back if the current data does not equal the cache value or if in force write mode
    //
    if ((ddrCRClkControls.Data != (*channelNvList)[ch].ddrCRClkControls) || (mode & GSM_FORCE_WRITE)) {
      MemWritePciCfgEp(host, socket, ch, DDRCRCLKCONTROLS_MCIO_DDRIOEXT_REG, ddrCRClkControls.Data);
    }

    //
    // Wait for the new value to settle
    //
    FixedDelay(host, usDelay);
  }

  //
  // Save to cache
  //
  if (mode & GSM_UPDATE_CACHE) {
    (*channelNvList)[ch].ddrCRClkControls = ddrCRClkControls.Data;
  }

  return MRC_STATUS_SUCCESS;
} // GetSetEridVref

/**
  Muli-use function to either get or set control delays based on the provided group number

  @param host:     Pointer to SYSHOST
  @param socket:   Socket number
  @param ch:       Channel number
  @param clk:      Clock number
  @param mode:     GSM_READ_CSR - Read the data from hardware and not cache
                   GSM_READ_ONLY - Do not write
                   GSM_WRITE_OFFSET - Write offset and not value
                   GSM_FORCE_WRITE - Force the write
  @param value:    Value to program

  @retval Pi delay value

**/
MRC_STATUS
GetSetClkDelay (
               PSYSHOST host,
               UINT8    socket,
               UINT8    ch,
               UINT8    clk,
               UINT8    mode,
               INT16    *value
               )
{
  UINT8                                 logicVal;
  UINT8                                 piVal;
  UINT16                                curVal;
  INT16                                 tmp;
  UINT16                                piDelay;
  struct socketNvram                    *socketNvram;
  DDRCRCLKPICODE_MCIO_DDRIOEXT_STRUCT   clkCsr;

  socketNvram = &host->nvram.mem.socket[socket];

  // HSD 4929953 (swap CLK1 and CLK2 control)
  clk = ClkSwapFix(host, socket, clk);

  //
  // Get register data
  //
  if (mode & GSM_READ_CSR) {
    clkCsr.Data = MemReadPciCfgEp (host, socket, ch, DDRCRCLKPICODE_MCIO_DDRIOEXT_REG);
  } else {
    clkCsr.Data = socketNvram->channelList[ch].clkCsr;
  }

  switch (clk) {
  case 0:
    logicVal = (UINT8)clkCsr.Bits.pilogicdelayrank0;
    piVal = (UINT8)clkCsr.Bits.pisettingrank0;
    break;

  case 1:
    logicVal = (UINT8)clkCsr.Bits.pilogicdelayrank1;
    piVal = (UINT8)clkCsr.Bits.pisettingrank1;
    break;

  case 2:
    logicVal = (UINT8)clkCsr.Bits.pilogicdelayrank2;
    piVal = (UINT8)clkCsr.Bits.pisettingrank2;
    break;

  case 3:
    logicVal = (UINT8)clkCsr.Bits.pilogicdelayrank3;
    piVal = (UINT8)clkCsr.Bits.pisettingrank3;
    break;

  default:
    logicVal = 0;
    piVal = 0;
    // Assert error
  }

  // Combine into delay
  curVal = (logicVal * 64) + piVal;

  // Read only?
  if (mode & GSM_READ_ONLY) {
    *value = (INT16)curVal;
  }
  // Handle writes
  else {

    // Adjust the current CMD delay value by offset
    if (mode & GSM_WRITE_OFFSET) {
      // Get the new offset (note: can be negative)
      tmp = curVal + *value;
    } else {
      // FORCE_WRITE
      tmp = *value;
    }
    // Check if we are below 0
    if (tmp >= 0) {
      piDelay = (UINT8) tmp;
    } else {
      piDelay = 128 + tmp;
    }
    if (piDelay > MAX_CK_DELAY) {
      piDelay = tmp - MAX_CK_DELAY;
    }

    // Program the IO delay
    logicVal = (UINT8) (piDelay / 64);
    piVal    = (UINT8) (piDelay % 64);

    switch (clk) {
    case 0:
      clkCsr.Bits.pilogicdelayrank0 = logicVal;
      clkCsr.Bits.pisettingrank0 = piVal;
      break;

    case 1:
      clkCsr.Bits.pilogicdelayrank1 = logicVal;
      clkCsr.Bits.pisettingrank1 = piVal;
      break;

    case 2:
      clkCsr.Bits.pilogicdelayrank2 = logicVal;
      clkCsr.Bits.pisettingrank2 = piVal;
      break;

    case 3:
      clkCsr.Bits.pilogicdelayrank3 = logicVal;
      clkCsr.Bits.pisettingrank3 = piVal;
      break;
    }

    //
    // Write it back if the current data does not equal the cache value or if in force write mode
    //
    if (host->var.mem.ioInitdone[socket] == 1) {
      Resync(host, socket, ch, clk);
    }

    if ((socketNvram->channelList[ch].clkCsr != clkCsr.Data) || (mode & GSM_FORCE_WRITE)) {
      MemWritePciCfgEp (host, socket, ch, DDRCRCLKPICODE_MCIO_DDRIOEXT_REG, clkCsr.Data);
    }

    if (host->var.mem.ioInitdone[socket] == 1) {
      RelockPll(host, socket, ch, clk);
    }
  }

  //
  // Save to cache
  //
  if (mode & GSM_UPDATE_CACHE) {
    socketNvram->channelList[ch].clkCsr = clkCsr.Data;
  }

  return MRC_STATUS_SUCCESS;
} // GetSetClkDelay

/**
  Muli-use function to either get or set control delays based on the provided group number

  @param host:     Pointer to SYSHOST
  @param socket:   Socket number
  @param ch:       Channel number
  @param dimm:     DIMM number
  @param mode:     GSM_READ_CSR - Read the data from hardware and not cache
                   GSM_READ_ONLY - Do not write
                   GSM_WRITE_OFFSET - Write offset and not value
                   GSM_FORCE_WRITE - Force the write
  @param value:    Value to program

  @retval Pi delay value

**/
MRC_STATUS
GetSetClkDelayFnv (
               PSYSHOST host,
               UINT8    socket,
               UINT8    ch,
               UINT8    dimm,
               UINT8    mode,
               INT16    *value
               )
{
  UINT8                                 logicVal;
  UINT8                                 piVal;
  UINT16                                curVal;
  INT16                                 tmp;
  UINT16                                piDelay;
  UINT16                                maxVal;
  struct socketNvram                    *socketNvram;
  DDRCRCMDPICODING_SA_FNV_DDRIO_DAT7_CH_STRUCT   ddrCrCmdPiCodingFnv;

  socketNvram = &host->nvram.mem.socket[socket];

  //
  // Maximum CMD delay
  //
  maxVal = MAX_CMD_DELAY_FNV;

  //
  // Get register data
  //
  if (mode & GSM_READ_CSR) {
    GetSetFnvIO(host, socket, ch, dimm, DDRCRCMDPICODING_SA_FNV_DDRIO_DAT7_CH_REG, &ddrCrCmdPiCodingFnv.Data, 0, FNV_IO_READ, 0xF);
  } else {
    ddrCrCmdPiCodingFnv.Data = socketNvram->channelList[ch].ddrCrCmdPiCodingFnv;
  }

  logicVal = (UINT8)ddrCrCmdPiCodingFnv.Bits.cmdpilogicdelay2;
  piVal = (UINT8)ddrCrCmdPiCodingFnv.Bits.cmdpicode2;

  // Combine into delay
  curVal = (logicVal * 64) + piVal;

  // Read only?
  if (mode & GSM_READ_ONLY) {
    *value = (INT16)curVal;
  }
  // Handle writes
  else {

    // Adjust the current CMD delay value by offset
    if (mode & GSM_WRITE_OFFSET) {
      // Get the new offset (note: can be negative)
      tmp = curVal + *value;
    } else {
      // FORCE_WRITE
      tmp = *value;
    }

    // Check for boundaries
    // value is INT16 and can be a negative value
    // Value and curVal are UINT8
    // Do not allow negative wraparound (curVal = 3 and offset = -4)
    if (tmp > 0) {
      piDelay = tmp;
    } else {
      piDelay = 0;
    }
    // Ensure we do not exceed maximums (caller should enforce this)
    if (piDelay > maxVal) {
      piDelay = maxVal;
    }

    // Program the IO delay
    logicVal = (UINT8) (piDelay / 64);
    piVal    = (UINT8) (piDelay % 64);

    ddrCrCmdPiCodingFnv.Bits.cmdpilogicdelay2 = logicVal;
    ddrCrCmdPiCodingFnv.Bits.cmdpicode2 = piVal;

    //
    // Write it back if the current data does not equal the cache value or if in force write mode
    //
    if ((socketNvram->channelList[ch].ddrCrCmdPiCodingFnv != ddrCrCmdPiCodingFnv.Data) || (mode & GSM_FORCE_WRITE)) {
      GetSetFnvIO(host, socket, ch, dimm, DDRCRCMDPICODING_SA_FNV_DDRIO_DAT7_CH_REG, &ddrCrCmdPiCodingFnv.Data, 0, FNV_IO_WRITE, 0xF);
    }
  }

  //
  // Save to cache
  //
  if (mode & GSM_UPDATE_CACHE) {
    socketNvram->channelList[ch].ddrCrCmdPiCodingFnv = ddrCrCmdPiCodingFnv.Data;
  }

  return MRC_STATUS_SUCCESS;
} // GetSetClkDelayFnv

//-----------------------------------------------------------------------------------
/**

  Disables unused CMD/CTL signals

  @param host  - Pointer to sysHost
  @param socket  - Socket number
  @param ch    - Channel number (0-based)
  @param Value - Value to program

  @retval N/A

**/
static void
SetCtlRankCnfg (
               PSYSHOST host,
               UINT8    socket,
               UINT8    ch,
               UINT16   Value
               )
{
} // SetCtlRankCnfg


//-----------------------------------------------------------------------------------
/**

  Disables unused clocks

  @param host  - Pointer to sysHost
  @param socket  - Socket number
  @param ch    - Channel number (0-based)
  @param Value - Value to program

  @retval N/A

**/
static void
SetRankUsed (
            PSYSHOST host,
            UINT8    socket,
            UINT8    ch,
            UINT8    Value
            )
{
} // SetRankUsed

// Table of # pi ticks per pico second for each supported frequency
// Formula is: ((1/(Frq/2))*1.0E6)/64 - ((1/(1066/2))*1000000)/64 = 29.29
//                                         800 1066 1333 1600 1833 2133 2400
//                                         400 533   667  800  933 1066 1200
// HSD 4987780
static const UINT8 piPsFrqTable[MAX_SUP_FREQ] = { 20, 16, 15, 13, 12, 11, 10, 9, 8, 8, 7, 7, 7, 6, 6, 6, 5, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 4};

/**

  Set starting/initial values for Clock and Control signals
  Initial values come from Analog Design

  @param host    - Pointer to sysHost
  @param socket  - Socket number

  @retval N/A

**/
void
SetStartingCCC (
               PSYSHOST  host,
               UINT8     socket
               )
{
  UINT8               ai;
  UINT8               mode;
  UINT8               piPerPico;
  INT16               delay;
  UINT8               cpuSku;
  UINT8               entries;
  INT16               initOffset;
  struct channelNvram (*channelNvList)[MAX_CH];
  CAPID4_PCU_FUN3_STRUCT  capid4;
  // A-Stepping Cmd/CTL DivBy2AlignCtl Xover Workaround
  UINT8               ch;
  UINT8               clk;
  UINT8               dimm;
  UINT8               chop;
  struct dimmNvram    (*dimmNvList)[MAX_DIMM];
  INT16               aepClk[MAX_CH][MAX_CLK];
  channelNvList = GetChannelNvList(host, socket);
  piPerPico = piPsFrqTable[host->nvram.mem.ratioIndex];

#ifdef SERIAL_DBG_MSG
  getPrintFControl(host);
#endif

  //
  // Initial offset for CMD and CTL
  //
  // A-Stepping Cmd/CTL DivBy2AlignCtl Xover Workaround
  if (CheckSteppingGreaterThan(host, CPU_SKX, A2_REV_SKX)) {
    initOffset = 128; // B-Stepping
  } else {
    initOffset = 64; // A-Stepping with DDR4 and NVMDIMM
  }

  mode = GSM_FORCE_WRITE + GSM_UPDATE_CACHE; // Use the cached value and don't read

  // capid4 contains sku info
  capid4.Data = ReadCpuPciCfgEx(host, socket, 0, CAPID4_PCU_FUN3_REG);
  chop = (UINT8)capid4.Bits.physical_chop;

  switch (chop) {
    case SKX_SKU_LCC:
      cpuSku = 0;
      break;
    case SKX_SKU_HCC:
      cpuSku = 1;
      break;
    case SKX_SKU_XCC:
      cpuSku = 2;
      break;
    default:
      cpuSku = 2;
      break;
  }

  entries = (UINT8) (sizeof (igCtl) / sizeof (struct IoGroupDelayStruct));
  
  OutputCheckpoint(host, STS_CHANNEL_TRAINING, PACKAGE_DELAY_CCC, socket);
  
  MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "SetStartingCCC => CpuSku=%2d, CtlEntries=%3d\n",chop, entries));

  for (ai = 0; ai < entries; ai++) {
    if ((*channelNvList)[igCtl[ai].ch].enabled) {
      if (igCtl[ai].delay[cpuSku] != 0xFF) {
        // A-Stepping Cmd/CTL DivBy2AlignCtl Xover Workaround
        delay = (igCtl[ai].delay[cpuSku] / piPerPico) + initOffset;  // Ensure proper rounding
        //MemDebugPrint((host, SDBG_MAX, socket, igCtl[ai].ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
        //               "CTL Group %d, CTL side %d, piDelay %d\n",igCtl[ai].iog, igCtl[ai].side, delay));
        GetSetCmdDelay (host, socket, igCtl[ai].ch, igCtl[ai].iog, igCtl[ai].side, mode, &delay);
      }
    }
  } // ai loop

  entries = (UINT8) (sizeof (igCmd) / sizeof (struct IoGroupDelayStruct));

  MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "SetStartingCCC => CpuSku=%2d, CmdEntries=%3d\n",
                 chop, entries));

  for (ai = 0; ai < entries; ai++) {
    if ((*channelNvList)[igCmd[ai].ch].enabled) {
      if (igCmd[ai].delay[cpuSku] != 0xFF) {
        // A-Stepping Cmd/CTL DivBy2AlignCtl Xover Workaround
        delay = (igCmd[ai].delay[cpuSku] / piPerPico) + initOffset;  // Ensure proper rounding
        //MemDebugPrint((host, SDBG_MAX, socket, igCmd[ai].ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
        //               "CMD Group %d, CMD side %d, piDelay %d\n",igCmd[ai].iog, igCmd[ai].side, delay));
        GetSetCmdDelay (host, socket, igCmd[ai].ch, igCmd[ai].iog, igCmd[ai].side, mode, &delay);
      }
    }
  } // ai loop

  entries = (UINT8) (sizeof (igClk) / sizeof (struct IoGroupClkDelayStruct));

  MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "SetStartingCCC => CpuSku=%2d, ClkEntries=%3d\n",
                 chop, entries));

  for (ch = 0; ch < MAX_CH; ch++) {
    for (clk = 0; clk < MAX_CLK; clk++) {
      aepClk[ch][clk]  = 0;
    } // clk loop
  }
  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;
    dimmNvList = GetDimmNvList(host, socket, ch);
    for (dimm = 0; dimm < MAX_DIMM; dimm++) {
      // Offset Clk by 64
      if ((*dimmNvList)[dimm].aepDimmPresent) {
          aepClk[ch][dimm]  = -64;
        break;
      }
    }
  }

    for (ai = 0; ai < entries; ai++) {
    if ((*channelNvList)[igClk[ai].ch].enabled) {
      if (igClk[ai].delay[cpuSku] != 0xFF) {
        delay = igClk[ai].delay[cpuSku] / piPerPico - aepClk[igClk[ai].ch][igClk[ai].clk];  // Ensure proper rounding
        GetSetClkDelay (host, socket, igClk[ai].ch, igClk[ai].clk, mode, &delay);
      }
    }
  } // ai loop

#ifdef SERIAL_DBG_MSG
  DisplayCCCResults(host, socket);
  releasePrintFControl(host);
#endif
} // SetStartingCCC

/**

  Set starting/initial values for Clock and Control signals
  Initial values come from Analog Design

  @param host      - Pointer to host structure (outside scope of this spec)
  @param socket    - Processor socket within the system (0-based)
  @param level     - IO level to access
  @param group     - Command, clock or control group to access
  @param *minLimit - Minimum delay value allowed
  @param *maxLimit - Maximum delay value allowed

  @retval MRC_STATUS

**/
MRC_STATUS
GetCmdGroupLimits (
                  PSYSHOST  host,
                  UINT8     socket,
                  GSM_LT    level,
                  GSM_GT    group,
                  UINT16    *minLimit,
                  UINT16    *maxLimit
                  )
{
  *minLimit = 0;

  if (group == CkAll) {
    *maxLimit = 127;
  } else {
    *maxLimit = 255;
  }

  return MRC_STATUS_SUCCESS;
} // GetCmdGroupLimits

#ifdef SERIAL_DBG_MSG
char *GetSignalStr(GSM_CSN sig, char *strBuf) {

  switch (sig) {
  case PAR:
    StrCpyLocal(strBuf, "PAR");
    break;
  case ACT_N:
    StrCpyLocal(strBuf, "ACT_N");
    break;
  case RAS_N:
    StrCpyLocal(strBuf, "RAS_N");
    break;
  case CAS_N:
    StrCpyLocal(strBuf, "CAS_N");
    break;
  case WE_N:
    StrCpyLocal(strBuf, "WE_N");
    break;
  case BA0:
    StrCpyLocal(strBuf, "BA0");
    break;
  case BA1:
    StrCpyLocal(strBuf, "BA1");
    break;
  case BA2:
    StrCpyLocal(strBuf, "BA2");
    break;
  case BG0:
    StrCpyLocal(strBuf, "BG0");
    break;
  case BG1:
    StrCpyLocal(strBuf, "BG1");
    break;
  case A0:
    StrCpyLocal(strBuf, "A0");
    break;
  case A1:
    StrCpyLocal(strBuf, "A1");
    break;
  case A2:
    StrCpyLocal(strBuf, "A2");
    break;
  case A3:
    StrCpyLocal(strBuf, "A3");
    break;
  case A4:
    StrCpyLocal(strBuf, "A4");
    break;
  case A5:
    StrCpyLocal(strBuf, "A5");
    break;
  case A6:
    StrCpyLocal(strBuf, "A6");
    break;
  case A7:
    StrCpyLocal(strBuf, "A7");
    break;
  case A8:
    StrCpyLocal(strBuf, "A8");
    break;
  case A9:
    StrCpyLocal(strBuf, "A9");
    break;
  case A10:
    StrCpyLocal(strBuf, "A10");
    break;
  case A11:
    StrCpyLocal(strBuf, "A11");
    break;
  case A12:
    StrCpyLocal(strBuf, "A12");
    break;
  case A13:
    StrCpyLocal(strBuf, "A13");
    break;
  case A14:
    StrCpyLocal(strBuf, "A14");
    break;
  case A15:
    StrCpyLocal(strBuf, "A15");
    break;
  case A16:
    StrCpyLocal(strBuf, "A16");
    break;
  case A17:
    StrCpyLocal(strBuf, "A17");
    break;
  case CS0_N:
    StrCpyLocal(strBuf, "CS0_N");
    break;
  case CS1_N:
    StrCpyLocal(strBuf, "CS1_N");
    break;
  case CS2_N:
    StrCpyLocal(strBuf, "CS2_N/C0");
    break;
  case CS3_N:
    StrCpyLocal(strBuf, "CS3_N/C1");
    break;
  case CS4_N:
    StrCpyLocal(strBuf, "CS4_N");
    break;
  case CS5_N:
    StrCpyLocal(strBuf, "CS5_N");
    break;
  case CS6_N:
    StrCpyLocal(strBuf, "CS6_N/C0");
    break;
  case CS7_N:
    StrCpyLocal(strBuf, "CS7_N/C1");
    break;
  case CS8_N:
    StrCpyLocal(strBuf, "CS8_N");
    break;
  case CS9_N:
    StrCpyLocal(strBuf, "CS9_N");
    break;
  case CKE0:
    StrCpyLocal(strBuf, "CKE0");
    break;
  case CKE1:
    StrCpyLocal(strBuf, "CKE1");
    break;
  case CKE2:
    StrCpyLocal(strBuf, "CKE2");
    break;
  case CKE3:
    StrCpyLocal(strBuf, "CKE3");
    break;
  case CKE4:
    StrCpyLocal(strBuf, "CKE4");
    break;
  case CKE5:
    StrCpyLocal(strBuf, "CKE5");
    break;
  case ODT0:
    StrCpyLocal(strBuf, "ODT0");
    break;
  case ODT1:
    StrCpyLocal(strBuf, "ODT1");
    break;
  case ODT2:
    StrCpyLocal(strBuf, "ODT2");
    break;
  case ODT3:
    StrCpyLocal(strBuf, "ODT3");
    break;
  case ODT4:
    StrCpyLocal(strBuf, "ODT4");
    break;
  case ODT5:
    StrCpyLocal(strBuf, "ODT5");
    break;
  case CK0:
    StrCpyLocal(strBuf, "CK0");
    break;
  case CK1:
    StrCpyLocal(strBuf, "CK1");
    break;
  case CK2:
    StrCpyLocal(strBuf, "CK2");
    break;
  case CK3:
    StrCpyLocal(strBuf, "CK3");
    break;
  case CK4:
    StrCpyLocal(strBuf, "CK4");
    break;
  case CK5:
    StrCpyLocal(strBuf, "CK5");
    break;
  case C0:
    StrCpyLocal(strBuf, "C0");
    break;
  case C1:
    StrCpyLocal(strBuf, "C1");
    break;
  case C2:
    StrCpyLocal(strBuf, "C2");
    break;
  default:
    StrCpyLocal(strBuf, "Unknown signal");
    break;
  }
  return strBuf;
} // GetSignalStr

char *GetPlatformGroupStr(PSYSHOST host, GSM_GT group, char *strBuf) {

  switch (group) {
  case CmdAll:
    StrCpyLocal(strBuf, "CmdAll");
    break;
  case CmdGrp0:
    StrCpyLocal(strBuf, "CmdGrp0");
    break;
  case CmdGrp1:
    StrCpyLocal(strBuf, "CmdGrp1");
    break;
  case CmdGrp2:
    StrCpyLocal(strBuf, "CmdGrp2");
    break;
  case CtlAll:
    StrCpyLocal(strBuf, "CtlAll");
    break;
  case CtlGrp0:
    StrCpyLocal(strBuf, "CtlGrp0");
    break;
  case CtlGrp1:
    StrCpyLocal(strBuf, "CtlGrp1");
    break;
  case CtlGrp2:
    StrCpyLocal(strBuf, "CtlGrp2");
    break;
  case CtlGrp3:
    StrCpyLocal(strBuf, "CtlGrp3");
    break;
  case CtlGrp4:
    StrCpyLocal(strBuf, "CtlGrp4");
    break;
  case CtlGrp5:
    StrCpyLocal(strBuf, "CtlGrp5");
    break;
  case CkAll:
    StrCpyLocal(strBuf, "CkAll");
    break;
  case CmdCtlAll:
    StrCpyLocal(strBuf, "CmdCtlAll");
    break;
  default:
    StrCpyLocal(strBuf, "Unknown platform group");
    break;
  }
  return strBuf;
} // GetPlatformGroupStr

char *GetLevelStr(GSM_LT level , char *strBuf) {
  switch (level) {
  case DdrLevel:
    StrCpyLocal(strBuf, "DdrLevel");
    break;
  case LrbufLevel:
    StrCpyLocal(strBuf, "LrbufLevel");
    break;
  case RegALevel:
    StrCpyLocal(strBuf, "RegALevel");
    break;
  case RegBLevel:
    StrCpyLocal(strBuf, "RegBLevel");
    break;
  case HbmLevel:
    StrCpyLocal(strBuf, "HbmLevel");
    break;
  default:
    StrCpyLocal(strBuf, "Unknown level");
    break;
  }
  return strBuf;
} // GetLevelStr
#endif  // SERIAL_DBG_MSG

/**

  Given an array of command delays relative to current clock and control delays,
  this function will combine shared settings in the DDRIO design and normalize the
  lowest command, clock or control value to their minimum limits.

  @param host                 - Pointer to host structure
  @param socket               - Processor socket within the system (0-based)
  @param ch                   - DDR channel number within the processor socket (0-based)
  @param listSize             - Number of entries in each list
  @param *value[MAX_RANK_CH]  - Array of pointers to listType

  @retval MRC_STATUS

**/
MRC_STATUS
SetCombinedCtlGroup (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     ch,
  UINT16    listSize,
  VOID      *value
  )
{
  UINT8                 dimm;
  UINT8                 rank;
  UINT8                 listIndex;
  UINT8                 ctlPiGroup;
  INT16                 ctlRight[MAX_CMDCTL_GROUPALL];
  INT16                 ctlLeft[MAX_CMDCTL_GROUPALL];
  GSM_CSN               ctlSignal[MAX_CMDCTL_GROUPALL];
  INT16                 ctlValue[MAX_CMDCTL_GROUPALL];
#ifdef SERIAL_DBG_MSG
  char                  strBuf0[128];
#endif  // SERIAL_DBG_MSG
  MRC_STATUS            status;
  GSM_CSEDGE_CTL        (*signalListEdge)[MAX_RANK_CH][NUM_SIGNALS_TO_SWEEP_LB];
  struct signalIOGroup  signalGroup;
  struct ddrRank        (*rankList)[MAX_RANK_DIMM];
  struct dimmNvram      (*dimmNvList)[MAX_DIMM];

  status = MRC_STATUS_SUCCESS;


  //
  // Initialize the edges for each CTL pi group
  //
  for (ctlPiGroup = 0; ctlPiGroup < MAX_CMDCTL_GROUPALL; ctlPiGroup++) {
    ctlLeft[ctlPiGroup] = 0;
    ctlRight[ctlPiGroup] = 511;
    ctlValue[ctlPiGroup] = 0;
  } // ctlPiGroup loop

  signalListEdge = value;

  dimmNvList = GetDimmNvList(host, socket, ch);
  for (dimm = 0; dimm < MAX_DIMM; dimm++) {

    rankList = GetRankNvList(host, socket, ch, dimm);
    for (rank = 0; rank < (*dimmNvList)[dimm].numRanks; rank++) {
      if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

        //
        // Loop through the list to create the composite left and right edges for each CTL group
        //
        for (listIndex = 0; listIndex < listSize; listIndex++) {

          //
          // Get the silicon pi group this signal belongs to
          //
          if ((*signalListEdge)[(*rankList)[rank].rankIndex][listIndex].toSample == 0) continue;
          status = SignalToPiGroup (host, (*signalListEdge)[(*rankList)[rank].rankIndex][listIndex].signal, &signalGroup);

          //
          // Get the index into the CTL silicon pi group
          //
          for (ctlPiGroup = 0; ctlPiGroup < MAX_CMDCTL_GROUPALL; ctlPiGroup++) {
            if ((signalGroup.group.num == platformCmdCtlGroupAll[ctlPiGroup].num) &&
                (signalGroup.group.side == platformCmdCtlGroupAll[ctlPiGroup].side)) {

              //
              // We found the CTL pi group so break out of the loop
              //
              break;
            }
          } // ctlPiGroup loop

          RC_ASSERT(ctlPiGroup != MAX_CMDCTL_GROUPALL, ERR_RC_INTERNAL, RC_ASSERT_MINOR_CODE_42);

          //
          // Update the composite left and right edges for the current CTL pi group relative to the clock
          //
          UpdateEdges((*signalListEdge)[(*rankList)[rank].rankIndex][listIndex].le, (*signalListEdge)[(*rankList)[rank].rankIndex][listIndex].re,
                      &ctlLeft[ctlPiGroup], &ctlRight[ctlPiGroup]);
          ctlValue[ctlPiGroup] = ((ctlLeft[ctlPiGroup] + ctlRight[ctlPiGroup])/2)%256;
          ctlSignal[ctlPiGroup] = (*signalListEdge)[(*rankList)[rank].rankIndex][listIndex].signal;

          MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       "%s:\t CTL Pi Group %d: le = %d re = %d, ctlLeft = %d ctlRight = %d ctlValue = %d\n",
                        GetSignalStr(signalGroup.sig, (char *)&strBuf0), ctlPiGroup,
                        (*signalListEdge)[(*rankList)[rank].rankIndex][listIndex].le, (*signalListEdge)[(*rankList)[rank].rankIndex][listIndex].re,
                        ctlLeft[ctlPiGroup], ctlRight[ctlPiGroup], ctlValue[ctlPiGroup]));
      } // listIndex loop
    } // rank loop
  } // dimm loop

  for (ctlPiGroup = 0; ctlPiGroup < MAX_CMDCTL_GROUPALL; ctlPiGroup++) {
    if (ctlValue[ctlPiGroup] != 0) {
      status = GetSetSignal (host, socket, ch, DdrLevel, ctlSignal[ctlPiGroup], GSM_FORCE_WRITE | GSM_UPDATE_CACHE, &ctlValue[ctlPiGroup]);
    }
  }
  return status;
} // SetCombinedCtlGroup


/**

  Given an array of command delays relative to current clock and control delays,
  this function will combine shared settings in the DDRIO design and normalize the
  lowest command, clock or control value to their minimum limits.

  @param host                 - Pointer to host structure
  @param socket               - Processor socket within the system (0-based)
  @param ch                   - DDR channel number within the processor socket (0-based)
  @param level                - IO level to access
  @param mode                 - Bit-field of different modes
  @param listType             - Selects type of each list
  @param listSize             - Number of entries in each list
  @param *value[MAX_RANK_CH]  - Array of pointers to listType

  @retval MRC_STATUS

**/
MRC_STATUS
SetCombinedCmdGroup (
                    PSYSHOST  host,
                    UINT8     socket,
                    UINT8     ch,
                    GSM_LT    level,
                    UINT8     mode,
                    UINT8     listType,
                    UINT16    listSize,
                    VOID      *value
                    )
{
  UINT8                 dimm;
  UINT8                 rank;
  UINT8                 clk;
  UINT8                 listIndex;
  UINT8                 cmdPiGroup;
  UINT8                 ckEnabled[MAX_CLK];
  UINT8                 ctlIndex[MAX_CLK];
  INT16                 cmdLeft[MAX_CMDCTL_GROUPALL][MAX_CLK];
  INT16                 cmdRight[MAX_CMDCTL_GROUPALL][MAX_CLK];
  GSM_GT                cmdGroup[MAX_CMDCTL_GROUPALL][MAX_CLK];
  GSM_CSN               cmdSignal[MAX_CMDCTL_GROUPALL][MAX_CLK];
  INT16                 cmdOffset[MAX_CMDCTL_GROUPALL];
  UINT16                cmdPiDelaySum[MAX_CMDCTL_GROUPALL];
  UINT8                 cmdPiDelayCount[MAX_CMDCTL_GROUPALL];
  INT16                 cmdPiDelay[MAX_CMDCTL_GROUPALL];
  UINT8                 maxIoGroup;
  UINT8                 iog;
//#if SMCPKG_SUPPORT
//  UINT8	tempSocket = 0xff;
//  UINT8	tempChannel = 0xff;
//  UINT8	tempDimm = 0xff;
//#endif	
  struct ioGroup        *iogPtr;
#ifdef SERIAL_DBG_MSG
  char                  strBuf0[128];
#endif  // SERIAL_DBG_MSG
  MRC_STATUS            status;
  GSM_CSVAL             (*signalList)[MAX_RANK_CH][NUM_SIGNALS_TO_SWEEP];
  GSM_CSEDGE            (*signalListEdge)[MAX_RANK_CH][NUM_SIGNALS_TO_SWEEP];
  GSM_CGVAL             (*groupList)[MAX_RANK_CH][8];
  GSM_CGEDGE            (*groupListEdge)[MAX_RANK_CH][MAX_CMD];
  struct signalIOGroup  signalGroup;
  struct ddrRank        (*rankList)[MAX_RANK_DIMM];
  struct dimmNvram      (*dimmNvList)[MAX_DIMM];

  status = MRC_STATUS_SUCCESS;

  dimmNvList = GetDimmNvList(host, socket, ch);

  for (clk = 0; clk < MAX_CLK; clk++) {
    ckEnabled[clk] = 0;
    ctlIndex[clk] = 0;
  } // clk loop

  for (dimm = 0; dimm < MAX_DIMM; dimm++) {
    rankList = GetRankNvList(host, socket, ch, dimm);
    for (rank = 0; rank < (*dimmNvList)[dimm].numRanks; rank++) {

      // If rank enabled
      if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

      clk = (*rankList)[rank].ckIndex;
      ctlIndex[clk] = (*rankList)[rank].ctlIndex;
      ckEnabled[clk] = 1;
    } // rank loop
  } // dimm loop

  //
  // Initialize the edges for each CMD pi group
  //
  for (cmdPiGroup = 0; cmdPiGroup < MAX_CMDCTL_GROUPALL; cmdPiGroup++) {
    // Init CK composite edges to 255
    for (clk = 0; clk < MAX_CLK; clk++) {
      cmdLeft[cmdPiGroup][clk] = -UNMARGINED_CMD_EDGE;
      cmdRight[cmdPiGroup][clk] = UNMARGINED_CMD_EDGE;
      //cmdOffsetClk[cmdPiGroup][clk] = 0;
    } // clk loop
    cmdOffset[cmdPiGroup] = 0;
  } // cmdPiGroup loop

  if (listType == CMD_GRP_DELAY) {
    //
    // Combine edges if necessary
    //
    if (mode == GSM_COMBINE_EDGES) {
      groupListEdge = value;

      for (dimm = 0; dimm < MAX_DIMM; dimm++) {

        rankList = GetRankNvList(host, socket, ch, dimm);

        for (rank = 0; rank < (*dimmNvList)[dimm].numRanks; rank++) {
          if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

          clk = (*rankList)[rank].ckIndex;

          //
          // Loop through the list to create the composite left and right edges for each CMD group
          //
          for (listIndex = 0; listIndex < listSize; listIndex++) {

            //if ((*groupListEdge)[(*rankList)[rank].rankIndex][listIndex].group == CmdAll) {
            //  for (cmdPiGroup = 0; cmdPiGroup < MAX_CMD; cmdPiGroup++) {
            //    // Init CK composite edges to 127
            //    cmdLeft[cmdPiGroup][clk] = (*groupListEdge)[(*rankList)[rank].rankIndex][listIndex].le;
            //    cmdRight[cmdPiGroup][clk] = (*groupListEdge)[(*rankList)[rank].rankIndex][listIndex].re;
            //  } // cmdPiGroup loop
            //} else {
            MemDebugPrint((host, SDBG_MAX, NO_SOCKET, ch, dimm, rank, NO_STROBE, NO_BIT,
                           "Platform Group = %s\n", GetPlatformGroupStr(host, (*groupListEdge)[(*rankList)[rank].rankIndex][listIndex].group, (char *)&strBuf0)));
            switch ((*groupListEdge)[(*rankList)[rank].rankIndex][listIndex].group) {
            case CmdGrp0:
              maxIoGroup = MAX_CMD_GROUP0;
              iogPtr = platformCmdGroup0;
              break;
            case CmdGrp1:
              maxIoGroup = MAX_CMD_GROUP1;
              iogPtr = platformCmdGroup1;
              break;
            case CmdGrp2:
              maxIoGroup = MAX_CMD_GROUP2;
              iogPtr = platformCmdGroup2;
              break;
            case CmdAll:
              maxIoGroup = MAX_CMD_GROUPALL;
              iogPtr = platformCmdGroupAll;
              break;
            default:
              maxIoGroup = MAX_CMD_GROUPALL;
              iogPtr = platformCmdGroupAll;
              status = MRC_STATUS_GROUP_NOT_SUPPORTED;
              break;
            }

            for (iog = 0; iog < maxIoGroup; iog++) {

              //
              // Get the index into the CMD silicon pi group
              //
              for (cmdPiGroup = 0; cmdPiGroup < MAX_CMD_GROUPALL; cmdPiGroup++) {
                if (((iogPtr + iog)->num == platformCmdGroupAll[cmdPiGroup].num) &&
                    ((iogPtr + iog)->side == platformCmdGroupAll[cmdPiGroup].side)) {
                  //
                  // We found the CMD pi group so break out of the loop
                  //
                  MemDebugPrint((host, SDBG_MAX, NO_SOCKET, ch, dimm, rank, NO_STROBE, NO_BIT,
                                 "Found CMD Pi group: %d side %d\n", platformCmdGroupAll[cmdPiGroup].num, platformCmdGroupAll[cmdPiGroup].side));
                  break;
                }
              } // cmdPiGroup loop

              cmdLeft[cmdPiGroup][clk] = (*groupListEdge)[(*rankList)[rank].rankIndex][listIndex].le;
              cmdRight[cmdPiGroup][clk] = (*groupListEdge)[(*rankList)[rank].rankIndex][listIndex].re;
              cmdGroup[cmdPiGroup][clk] = (*groupListEdge)[(*rankList)[rank].rankIndex][listIndex].group;
              cmdSignal[cmdPiGroup][clk] = gsmCsnDelim;
              cmdOffset[cmdPiGroup] = 0;
              MemDebugPrint((host, SDBG_MAX, NO_SOCKET, ch, dimm, rank, NO_STROBE, NO_BIT,
                             "cmdLeft[%d][%d] = %d : cmdRight[%d][%d] = %d\n", cmdPiGroup, clk,
                             (*groupListEdge)[(*rankList)[rank].rankIndex][listIndex].le,cmdPiGroup, clk,
                             (*groupListEdge)[(*rankList)[rank].rankIndex][listIndex].re));
//#if SMCPKG_SUPPORT
//              if (((cmdRight[cmdPiGroup][clk] - cmdLeft[cmdPiGroup][clk]) < 40) && ((tempSocket != socket) || (tempChannel != ch) || (tempDimm != dimm))){
//              	tempSocket = socket;
//              	tempChannel = ch;
//              	tempDimm = dimm;
//              	OutputWarning (host, WARN_CMD_CLK_TRAINING, 0, socket, ch, dimm, rank);
//              }
//#endif
            } // iog loop


            //}
          } // listIndex loop
        } // rank loop
      } // dimm loop

      //
      // Evaluate CMD margins and offset CMD, CTL, and CLK appropriately
      //
      EvaluateCMDMargins(host, socket, ch, ckEnabled, ctlIndex, cmdLeft, cmdRight, cmdGroup, cmdSignal, cmdOffset);


    } else {
      //
      // Average center points
      //
      groupList = value;

      //
      // Initialize for each CMD pi group
      //
      for (cmdPiGroup = 0; cmdPiGroup < MAX_CMD_GROUPALL; cmdPiGroup++) {
        cmdPiDelaySum[cmdPiGroup] = 0;
        cmdPiDelayCount[cmdPiGroup] = 0;
      } // cmdPiGroup loop

      for (dimm = 0; dimm < MAX_DIMM; dimm++) {

        rankList = GetRankNvList(host, socket, ch, dimm);

        for (rank = 0; rank < (*dimmNvList)[dimm].numRanks; rank++) {
          if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

          //
          // Loop through the list to create the composite left and right edges for each CMD group
          //
          for (listIndex = 0; listIndex < listSize; listIndex++) {

            switch ((*groupList)[(*rankList)[rank].rankIndex][listIndex].group) {
            case CmdGrp0:
              maxIoGroup = MAX_CMD_GROUP0;
              iogPtr = platformCmdGroup0;
              break;
            case CmdGrp1:
              maxIoGroup = MAX_CMD_GROUP1;
              iogPtr = platformCmdGroup1;
              break;
            case CmdGrp2:
              maxIoGroup = MAX_CMD_GROUP2;
              iogPtr = platformCmdGroup2;
              break;
            case CmdAll:
              maxIoGroup = MAX_CMD_GROUPALL;
              iogPtr = platformCmdGroupAll;
              break;
            default:
              maxIoGroup = MAX_CMD_GROUPALL;
              iogPtr = platformCmdGroupAll;
              status = MRC_STATUS_GROUP_NOT_SUPPORTED;
              break;
            }

            for (iog = 0; iog < maxIoGroup; iog++) {
              //
              // Get the index into the CMD silicon pi group
              //
              for (cmdPiGroup = 0; cmdPiGroup < MAX_CMD_GROUPALL; cmdPiGroup++) {
                if (((iogPtr + iog)->num == platformCmdGroupAll[cmdPiGroup].num) &&
                    ((iogPtr + iog)->side == platformCmdGroupAll[cmdPiGroup].side)) {

                  //
                  // Add this pi delay to the total and increment the count
                  //
                  cmdPiDelaySum[cmdPiGroup] += (*groupList)[(*rankList)[rank].rankIndex][listIndex].value;
                  cmdPiDelayCount[cmdPiGroup]++;

                  //
                  // We found the CMD pi group so break out of the loop
                  //
                  break;
                }
              } // cmdPiGroup loop
            } // iog group
          } // listIndex loop
        } // rank loop
      } // dimm loop

      //
      // Calculate the pi delay for each CMD si group
      //
      for (cmdPiGroup = 0; cmdPiGroup < MAX_CMD_GROUPALL; cmdPiGroup++) {
        if (cmdPiDelayCount[cmdPiGroup] == 0) continue;

        cmdPiDelay[cmdPiGroup] = (INT16)(cmdPiDelaySum[cmdPiGroup] / cmdPiDelayCount[cmdPiGroup]);
      } // cmdPiGroup loop

      //
      // Program the delay
      //
      for (cmdPiGroup = 0; cmdPiGroup < MAX_CMD_GROUPALL; cmdPiGroup++) {
        GetSetCmdDelay (host, socket, ch, platformCmdGroupAll[cmdPiGroup].num, platformCmdGroupAll[cmdPiGroup].side,
                        GSM_WRITE_OFFSET + GSM_UPDATE_CACHE, &cmdPiDelay[cmdPiGroup]);
      } // cmdPiGroup loop
    }

  } else if (listType == CMD_SIGNAL_DELAY) {

    //
    // Combine edges if necessary
    //
    if (mode == GSM_COMBINE_EDGES) {

      signalListEdge = value;

      for (dimm = 0; dimm < MAX_DIMM; dimm++) {

        rankList = GetRankNvList(host, socket, ch, dimm);

        for (rank = 0; rank < (*dimmNvList)[dimm].numRanks; rank++) {
          if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

          //
          // Loop through the list to create the composite left and right edges for each CMD group
          //
          for (listIndex = 0; listIndex < listSize; listIndex++) {

            clk = (*rankList)[rank].ckIndex;

            //
            // Get the silicon pi group this signal belongs to
            //
            status = SignalToPiGroup (host, (*signalListEdge)[(*rankList)[rank].rankIndex][listIndex].signal, &signalGroup);

            //
            // Get the index into the CMD silicon pi group
            //
            for (cmdPiGroup = 0; cmdPiGroup < MAX_CMD_GROUPALL; cmdPiGroup++) {
              if ((signalGroup.group.num == platformCmdGroupAll[cmdPiGroup].num) &&
                  (signalGroup.group.side == platformCmdGroupAll[cmdPiGroup].side)) {
                //
                // We found the CMD pi group so break out of the loop
                //
                break;
              }
            } // cmdPiGroup loop

            RC_ASSERT(cmdPiGroup != MAX_CMD_GROUPALL, ERR_RC_INTERNAL, RC_ASSERT_MINOR_CODE_43);
            //
            // Update the composite left and right edges for the current CMD pi group relative to the clock
            //
            UpdateEdges((*signalListEdge)[(*rankList)[rank].rankIndex][listIndex].le, (*signalListEdge)[(*rankList)[rank].rankIndex][listIndex].re,
                        &cmdLeft[cmdPiGroup][clk], &cmdRight[cmdPiGroup][clk]);
            cmdGroup[cmdPiGroup][clk] = GsmGtDelim;
            cmdSignal[cmdPiGroup][clk] = (*signalListEdge)[(*rankList)[rank].rankIndex][listIndex].signal;
            MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                           "%s:\t CMD Pi Group %d clk %d: le %d re = %d, cmdLeft = %d cmdRight = %d\n",
                           GetSignalStr(signalGroup.sig, (char *)&strBuf0), cmdPiGroup, clk,
                           (*signalListEdge)[(*rankList)[rank].rankIndex][listIndex].le, (*signalListEdge)[(*rankList)[rank].rankIndex][listIndex].re,
                           cmdLeft[cmdPiGroup][clk], cmdRight[cmdPiGroup][clk]));
          } // listIndex loop
        } // rank loop
      } // dimm loop

      //
      // Evaluate CMD margins and offset CMD, CTL, and CLK appropriately
      //
      EvaluateCMDMargins(host, socket, ch, ckEnabled, ctlIndex, cmdLeft, cmdRight, cmdGroup, cmdSignal, cmdOffset);

    } else {
      //
      // Average center points
      //
      signalList = value;

      //
      // Initialize for each CMD pi group
      //
      for (cmdPiGroup = 0; cmdPiGroup < MAX_CMD_GROUPALL; cmdPiGroup++) {
        cmdPiDelaySum[cmdPiGroup] = 0;
        cmdPiDelayCount[cmdPiGroup] = 0;
      } // cmdPiGroup loop

      for (dimm = 0; dimm < MAX_DIMM; dimm++) {

        rankList = GetRankNvList(host, socket, ch, dimm);

        for (rank = 0; rank < (*dimmNvList)[dimm].numRanks; rank++) {
          if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

          //
          // Loop through the list to create the composite left and right edges for each CMD group
          //
          for (listIndex = 0; listIndex < listSize; listIndex++) {

            //
            // Get the silicon pi group this signal belongs to
            //
            status = SignalToPiGroup (host, (*signalList)[(*rankList)[rank].rankIndex][listIndex].signal, &signalGroup);

            //
            // Get the index into the CMD silicon pi group
            //
            for (cmdPiGroup = 0; cmdPiGroup < MAX_CMD_GROUPALL; cmdPiGroup++) {
              if ((signalGroup.group.num == platformCmdGroupAll[cmdPiGroup].num) &&
                  (signalGroup.group.side == platformCmdGroupAll[cmdPiGroup].side)) {

                //
                // Add this pi delay to the total and increment the count
                //
                cmdPiDelaySum[cmdPiGroup] += (*signalList)[(*rankList)[rank].rankIndex][listIndex].value;
                cmdPiDelayCount[cmdPiGroup]++;

                //
                // We found the CMD pi group so break out of the loop
                //
                break;
              }
            } // cmdPiGroup loop
          } // listIndex loop
        } // rank loop
      } // dimm loop

      //
      // Calculate the pi delay for each CMD pi group
      //
      for (cmdPiGroup = 0; cmdPiGroup < MAX_CMD_GROUPALL; cmdPiGroup++) {
        if (cmdPiDelayCount[cmdPiGroup] == 0) continue;

        cmdPiDelay[cmdPiGroup] = (INT16)(cmdPiDelaySum[cmdPiGroup] / cmdPiDelayCount[cmdPiGroup]);
      } // cmdPiGroup loop

      //
      // Program the delay
      //
      for (cmdPiGroup = 0; cmdPiGroup < MAX_CMD_GROUPALL; cmdPiGroup++) {
        GetSetCmdDelay (host, socket, ch, platformCmdGroupAll[cmdPiGroup].num, platformCmdGroupAll[cmdPiGroup].side,
                        GSM_WRITE_OFFSET + GSM_UPDATE_CACHE, &cmdPiDelay[cmdPiGroup]);
      } // cmdPiGroup loop

    }
  } else if (listType == CMDCTL_SIGNAL_DELAY) {

    //
    // Combine edges if necessary
    //
    if (mode == GSM_COMBINE_EDGES) {
      MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                           "Calculate Composite edges per PI Group per DIMM\n"));

      signalListEdge = value;

      for (dimm = 0; dimm < MAX_DIMM; dimm++) {

        rankList = GetRankNvList(host, socket, ch, dimm);

        for (rank = 0; rank < (*dimmNvList)[dimm].numRanks; rank++) {
          if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

          //skip ranks >0 if rdimm
          if ((host->nvram.mem.dimmTypePresent != UDIMM) && (rank > 0)) continue;

          //
          // Loop through the list to create the composite left and right edges for each CMD group
          //
          for (listIndex = 0; listIndex < listSize; listIndex++) {

            clk = (*rankList)[rank].ckIndex;

            //
            // Get the silicon pi group this signal belongs to
            //
            status = SignalToPiGroup (host, (*signalListEdge)[(*rankList)[rank].rankIndex][listIndex].signal, &signalGroup);

            //
            // Get the index into the CMD silicon pi group
            //

            for (cmdPiGroup = 0; cmdPiGroup < MAX_CMDCTL_GROUPALL; cmdPiGroup++) {
              if ((signalGroup.group.num == platformCmdCtlGroupAll[cmdPiGroup].num) &&
                  (signalGroup.group.side == platformCmdCtlGroupAll[cmdPiGroup].side)) {
                //
                // We found the CMD pi group so break out of the loop
                //
                break;
              }
            } // cmdPiGroup loop

            RC_ASSERT(cmdPiGroup != MAX_CMDCTL_GROUPALL, ERR_RC_INTERNAL, RC_ASSERT_MINOR_CODE_44);

            //
            // Validating cmdPiGroup to avoid array index out of boundary (KW issue)
            //

            if(cmdPiGroup != MAX_CMDCTL_GROUPALL) {
              //
              // Update the composite left and right edges for the current CMD pi group relative to the clock
              //
              UpdateEdges((*signalListEdge)[(*rankList)[rank].rankIndex][listIndex].le, (*signalListEdge)[(*rankList)[rank].rankIndex][listIndex].re,
                          &cmdLeft[cmdPiGroup][clk], &cmdRight[cmdPiGroup][clk]);
              cmdGroup[cmdPiGroup][clk] = GsmGtDelim;
              cmdSignal[cmdPiGroup][clk] = (*signalListEdge)[(*rankList)[rank].rankIndex][listIndex].signal;
              MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
                             "%8s: CMD/CTL PI Group %2d, Group Num %2d, Side %2d,  clk %d: le %3d re = %3d, Left = %3d Right = %3d\n",
                             GetSignalStr(signalGroup.sig, (char *)&strBuf0), cmdPiGroup, signalGroup.group.num, signalGroup.group.side, clk,
                             (*signalListEdge)[(*rankList)[rank].rankIndex][listIndex].le, (*signalListEdge)[(*rankList)[rank].rankIndex][listIndex].re,
                             cmdLeft[cmdPiGroup][clk], cmdRight[cmdPiGroup][clk]));
            }
          } // listIndex loop
        } // rank loop
      } // dimm loop

      //
      // Evaluate CMD margins and offset CMD, CTL, and CLK appropriately
      //
      EvaluateCMDMargins(host, socket, ch, ckEnabled, ctlIndex, cmdLeft, cmdRight, cmdGroup, cmdSignal, cmdOffset);

    } else {
      MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                           "ERROR: This SetCombinedCmd mode not supported!\n"));

    }
  } else {
    status = MRC_STATUS_GROUP_NOT_SUPPORTED;
  }

  return status;
} // SetCombinedCmdGroup

#if SMCPKG_SUPPORT == 1
/*
 * Locate the clock to dimm slot
 */
UINT8 clk_to_dimm(PSYSHOST host,UINT8 socket, UINT8 ch, UINT8 clk)
{
  UINT8                 dimm;
  UINT8                 rank;
  struct ddrRank        (*rankList)[MAX_RANK_DIMM];
  struct dimmNvram      (*dimmNvList)[MAX_DIMM];
  UINT8                 dimm_located = FALSE;

  dimmNvList = GetDimmNvList(host, socket, ch);
  
  for (dimm = 0; dimm < MAX_DIMM ; dimm++) {
    if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
    
    rankList = GetRankNvList(host, socket, ch, dimm);
    for (rank = 0; rank < (*dimmNvList)[dimm].numRanks; rank++) {
      if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;
        if ( clk == (*rankList)[rank].ckIndex) {
          dimm_located = TRUE;
          break;
      }
    }
    if(dimm_located) break;
  }

  return dimm_located ? dimm : 0;//default DIMM slot 0
}
#endif


/**

  Evaluates the CMD margins and determines the appropriate offset

  @param host                 - Pointer to host structure
  @param socket               - Processor socket within the system (0-based)

  @retval MRC_STATUS

**/
static void
EvaluateCMDMargins (
                   PSYSHOST  host,
                   UINT8     socket,
                   UINT8     ch,
                   UINT8     ckEnabled[MAX_CLK],
                   UINT8     ctlIndex[MAX_CLK],
                   INT16     cmdLeft[MAX_CMDCTL_GROUPALL][MAX_CLK],
                   INT16     cmdRight[MAX_CMDCTL_GROUPALL][MAX_CLK],
                   GSM_GT    group[MAX_CMDCTL_GROUPALL][MAX_CLK],
                   GSM_CSN   cmdSignal[MAX_CMDCTL_GROUPALL][MAX_CLK],
                   INT16     cmdOffset[MAX_CMDCTL_GROUPALL]
                   )
{
  UINT8                 dimm;
  UINT8                 rank;
  UINT8 clk;
  UINT8 cmdPiGroup;
  INT16                 minCmdOffset;
  INT16                 maxCmdOffset;
  INT16                 cmdOffsetClk[MAX_CMDCTL_GROUPALL][MAX_CLK];
  INT16                 maxLeftOffset;
  UINT16                ctlMinVal;
  UINT16                ctlMaxVal;
  INT16                 minRightOffset;
  INT16                 ckOffset[MAX_CLK];
  UINT8                 earlyCmdClkDisable = 0;
  UINT8                 parityWorkaround = 0;
  struct ddrRank        (*rankList)[MAX_RANK_DIMM];
  struct dimmNvram      (*dimmNvList)[MAX_DIMM];
#if SMCPKG_SUPPORT == 1
  UINT8                 dimm_report = 0;
#endif

  //
  // Determine if early CMD/CLK training has been disabled
  //
  dimmNvList = GetDimmNvList(host, socket, ch);

  for (dimm = 0; dimm < MAX_DIMM; dimm++) {
    parityWorkaround = CheckParityWorkaround(host, socket, ch, dimm);

    if (parityWorkaround) break;
  } // dimm loop

  //
  // Determine if early CMD/CLK training has been disabled
  //
  earlyCmdClkDisable = ((~host->memFlows & MF_E_CMDCLK_EN) || parityWorkaround);

  MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       "Calculate offset per PI group per DIMM \n"));

  for (clk = 0; clk < MAX_CLK; clk++) {
    if (ckEnabled[clk] == 0) continue;
    //
    // Initialize these values so the maximum CMD delay can be determined
    //
    minCmdOffset = 255;
    maxCmdOffset = -255;

    //
    // Determine the offset for each CMD pi group and the max offset
    //
    for (cmdPiGroup = 0; cmdPiGroup < MAX_CMDCTL_GROUPALL; cmdPiGroup++) {

      //
      // Make sure the eye width is large enough
      //
      if ((cmdRight[cmdPiGroup][clk] - cmdLeft[cmdPiGroup][clk]) < 40) {
        MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
          "Eye width is too small: %d\n",
          cmdRight[cmdPiGroup][clk] - cmdLeft[cmdPiGroup][clk]));
#if SMCPKG_SUPPORT == 1
        //locate which dimm this signal belong to
        dimm = clk_to_dimm(host,socket,ch,clk);

        if(!(dimm_report & (1 << dimm))){//per bit map for DIMM, avoid to multi-time report on same dimm.
          dimm_report |= (1 << dimm);
          
          //warning report for dimm slot
          EwlOutputType3(host, WARN_CMD_CLK_TRAINING, WARN_CMD_PI_GROUP_SMALL_EYE, socket, ch, dimm, NO_RANK, EwlSeverityWarning, group[cmdPiGroup][clk], DdrLevel, cmdSignal[cmdPiGroup][clk], (UINT8)(cmdRight[cmdPiGroup][clk] - cmdLeft[cmdPiGroup][clk]));
        }
#else
        EwlOutputType3(host, WARN_CMD_CLK_TRAINING, WARN_CMD_PI_GROUP_SMALL_EYE, socket, ch, NO_DIMM, NO_RANK, EwlSeverityWarning, group[cmdPiGroup][clk], DdrLevel, cmdSignal[cmdPiGroup][clk], (UINT8)(cmdRight[cmdPiGroup][clk] - cmdLeft[cmdPiGroup][clk]));
#endif
        DisableChannel(host, socket, ch);
      }
      if ((cmdLeft[cmdPiGroup][clk] != -UNMARGINED_CMD_EDGE) && (cmdRight[cmdPiGroup][clk] != UNMARGINED_CMD_EDGE)){
        cmdOffsetClk[cmdPiGroup][clk] = (cmdLeft[cmdPiGroup][clk] + cmdRight[cmdPiGroup][clk]) / 2;

      UpdateMinMaxInt(cmdOffsetClk[cmdPiGroup][clk], &minCmdOffset, &maxCmdOffset);

        MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       "CMD Pi Group %2d clk %d cmdOffset %d\n",
                       cmdPiGroup, clk, cmdOffsetClk[cmdPiGroup][clk]));
      }
    } // cmdPiGroup loop

  } // clk offset

  //
  // Calculate the final offset for each CMD pi group
  //
  MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       "Calculate Final offset per PI group per CH \n"));

  for (cmdPiGroup = 0; cmdPiGroup < MAX_CMDCTL_GROUPALL; cmdPiGroup++) {
    // Track minCgOffset, maxCgOffset across all CK groups
    minCmdOffset = 255;
    maxCmdOffset = -255;

    for (clk = 0; clk < MAX_CLK; clk++) {
      if (ckEnabled[clk]) {
        if ((cmdLeft[cmdPiGroup][clk] != -UNMARGINED_CMD_EDGE) && (cmdRight[cmdPiGroup][clk] != UNMARGINED_CMD_EDGE)){
          //if(moveClk) {
          //  UpdateMinMaxInt(cmdPiShift[cmdPiGroup][clk], &minCmdOffset, &maxCmdOffset);
          //} else {
          UpdateMinMaxInt(cmdOffsetClk[cmdPiGroup][clk], &minCmdOffset, &maxCmdOffset);
        }
      }
    } // clk loop

    // Save common CMD offset
    cmdOffset[cmdPiGroup] = (maxCmdOffset + minCmdOffset) / 2;

    //
    // Write the CMD pi Group
    //
    if ((maxCmdOffset != -255) && (minCmdOffset != 255)){
      GetSetCmdDelay (host, socket, ch, platformCmdCtlGroupAll[cmdPiGroup].num, platformCmdCtlGroupAll[cmdPiGroup].side,
                    GSM_WRITE_OFFSET + GSM_UPDATE_CACHE, &cmdOffset[cmdPiGroup]);

      MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                   "CMD Pi Group %2d: maxOffset = %d, minOffset = %d, cmdOffset = %d\n",
                   cmdPiGroup, maxCmdOffset, minCmdOffset, cmdOffset[cmdPiGroup]));
    }
  } // cmdPiGroup loop


  //
  // Determine the clock offset
  //
  MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       "Calculate Clk offset \n"));
  for (cmdPiGroup = 0; cmdPiGroup < MAX_CMDCTL_GROUPALL; cmdPiGroup++) {
    for (clk = 0; clk < MAX_CLK; clk++) {
      if (ckEnabled[clk] == 0) continue;
      if ((cmdLeft[cmdPiGroup][clk] != -UNMARGINED_CMD_EDGE) && (cmdRight[cmdPiGroup][clk] != UNMARGINED_CMD_EDGE)){
        cmdLeft[cmdPiGroup][clk] = cmdLeft[cmdPiGroup][clk] - cmdOffset[cmdPiGroup];
        cmdRight[cmdPiGroup][clk] = cmdRight[cmdPiGroup][clk] - cmdOffset[cmdPiGroup];

        MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       "<--CMD Pi Group %2d clk %d: cmdLeft %d - cmdRight %d\n",
                       cmdPiGroup, clk, cmdLeft[cmdPiGroup][clk], cmdRight[cmdPiGroup][clk]));
      }
    } // clk offset
  } // cmdPiGroup loop

  for (clk = 0; clk < MAX_CLK; clk++) {
    if (ckEnabled[clk] == 0) continue;

    minRightOffset = 255;
    maxLeftOffset = -255;

    for (cmdPiGroup = 0; cmdPiGroup < MAX_CMDCTL_GROUPALL; cmdPiGroup++) {
      if ((cmdLeft[cmdPiGroup][clk] != -UNMARGINED_CMD_EDGE) && (cmdRight[cmdPiGroup][clk] != UNMARGINED_CMD_EDGE)){
        UpdateMinMaxInt(cmdLeft[cmdPiGroup][clk], &minCmdOffset, &maxLeftOffset);
        UpdateMinMaxInt(cmdRight[cmdPiGroup][clk], &minRightOffset, &maxCmdOffset);
      }
    } // cmdPiGroup loop

    ckOffset[clk] = -(minRightOffset + maxLeftOffset) / 2;

    //
    // Write the CTL delay
    //
    ctlMinVal = 255;
    ctlMaxVal = 0;
    GetSetCtlGroupDelay (host, socket, ch, ctlIndex[clk] + CtlGrp0, GSM_WRITE_OFFSET + GSM_UPDATE_CACHE, &ckOffset[clk], &ctlMinVal, &ctlMaxVal);

    //
    // Write the CLK delay
    //
    GetSetClkDelay (host, socket, ch, clk, GSM_WRITE_OFFSET + GSM_UPDATE_CACHE, &ckOffset[clk]);
    //
    // Only move IO Delays if in late CMD CLK (early CMD CLK disabled OR early CMD CLK executed already)
    //
    if (earlyCmdClkDisable || (host->var.mem.earlyCmdClkExecuted == 1)) {
      for (dimm = 0; dimm < MAX_DIMM; dimm++) {
        rankList = GetRankNvList(host, socket, ch, dimm);
        for (rank = 0; rank < (*dimmNvList)[dimm].numRanks; rank++) {
          if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;
          if ( clk == (*rankList)[rank].ckIndex) {
            GetSetDataGroup (host, socket, ch, dimm, rank, ALL_STROBES, 0, DdrLevel, RecEnDelay, GSM_WRITE_OFFSET + GSM_UPDATE_CACHE,
                             (INT16 *)&ckOffset[clk]);
            GetSetDataGroup (host, socket, ch, dimm, rank, ALL_STROBES, 0, DdrLevel, TxDqDelay, GSM_WRITE_OFFSET + GSM_UPDATE_CACHE,
                             (INT16 *)&ckOffset[clk]);
            GetSetDataGroup (host, socket, ch, dimm, rank, ALL_STROBES, 0, DdrLevel, TxDqsDelay, GSM_WRITE_OFFSET + GSM_UPDATE_CACHE,
                             (INT16 *)&ckOffset[clk]);
          }
        } // rank loop
      } // dimm loop
    }

    MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                   "<----clk %d ckOffset %d: -(maxLeftOffset:%d + minRightOffset:%d) / 2\n",
                   clk, ckOffset[clk], maxLeftOffset, minRightOffset));

  } // clk loop
} // EvaluateCMDMargins

/**

  Swap CLKs 1 and 2 for mapping issue

  @param host                 - Pointer to host structure
  @param socket               - Processor socket within the system (0-based)

  @retval MRC_STATUS

**/
static UINT8
ClkSwapFix(
    PSYSHOST  host,
    UINT8     socket,
    UINT8     clk
)
{

  // HSD 4929953 (swap CLK1 and CLK2 control)
  if (host->var.mem.socket[socket].clkSwapFixDis == 0) {  // original CLK mapping, NVMDIMM
    switch(clk) {
      case 1:
        clk = 2;
        break;
      case 2:
        clk = 1;
        break;
      default:
        break;
    }
  }
  return clk;
}

void
Resync(
        PSYSHOST host,
        UINT8    socket,
        UINT8    ch,
        UINT8    clk
)
{
  UINT8                                           dimm;
  struct dimmNvram                                (*dimmNvList)[MAX_DIMM];
  MSC_PLL_CTRL_FNV_DFX_MISC_0_STRUCT              pllCtrlMisc;
  MSC_FNVIO_INIT_CONTROL_1_FNV_DFX_MISC_0_STRUCT  fnvioInitCtl;

  dimmNvList = GetDimmNvList(host, socket, ch);

  for (dimm = 0; dimm < MAX_DIMM; dimm++) {
    if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
    if ((*dimmNvList)[dimm].aepDimmPresent == 0) continue;
    if ((clk == 1) || (clk == 3)) continue;

    pllCtrlMisc.Data = ReadFnvCfg(host, socket, ch, dimm, MSC_PLL_CTRL_FNV_DFX_MISC_0_REG);
    fnvioInitCtl.Data = ReadFnvCfg(host, socket, ch, dimm, MSC_FNVIO_INIT_CONTROL_1_FNV_DFX_MISC_0_REG);

    //
    // 1. Set ddrtio and daunit mask
    //
    pllCtrlMisc.Bits.daunit_pmreset_mask = 0;
    pllCtrlMisc.Bits.ddrtio_pmreset_mask = 0;
    WriteFnvCfg(host, socket, ch, dimm, MSC_PLL_CTRL_FNV_DFX_MISC_0_REG, pllCtrlMisc.Data);

    //
    // 2. Wait 2 usec
    //
    FixedDelay (host, 2);

    //
    // 3. Set ddrt clk idle mask
    //
    pllCtrlMisc.Bits.ddrtio_ckidle_mask = 1;
    WriteFnvCfg(host, socket, ch, dimm, MSC_PLL_CTRL_FNV_DFX_MISC_0_REG, pllCtrlMisc.Data);

    //
    // 4. Kick off Ddrt Resync FSM
    //
    fnvioInitCtl.Bits.ddrt_resync = 1;
    WriteFnvCfg(host, socket, ch, dimm, MSC_FNVIO_INIT_CONTROL_1_FNV_DFX_MISC_0_REG, fnvioInitCtl.Data);

    //
    // 5. Wait 2 us
    //
    FixedDelay (host, 2);

    //
    // 6. Clear DDRT Resync Bit
    //
    fnvioInitCtl.Bits.ddrt_resync = 0;
    WriteFnvCfg(host, socket, ch, dimm, MSC_FNVIO_INIT_CONTROL_1_FNV_DFX_MISC_0_REG, fnvioInitCtl.Data);

  } // dimm loop
}

void
RelockPll (
           PSYSHOST host,
           UINT8    socket,
           UINT8    ch,
           UINT8    clk
)
{
  UINT8                                           dimm;
  struct dimmNvram                                (*dimmNvList)[MAX_DIMM];
  MSC_PLL_CTRL_FNV_DFX_MISC_0_STRUCT              pllCtrlMisc;

  dimmNvList = GetDimmNvList(host, socket, ch);

  for (dimm = 0; dimm < MAX_DIMM; dimm++) {
    if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
    if ((*dimmNvList)[dimm].aepDimmPresent == 0) continue;
    if ((clk == 1) || (clk == 3)) continue;

    pllCtrlMisc.Data = ReadFnvCfg(host, socket, ch, dimm, MSC_PLL_CTRL_FNV_DFX_MISC_0_REG);

    //
    // 7. Clear ddrt clk idle mask
    //

    pllCtrlMisc.Bits.daunit_pmreset_mask = 1;
    pllCtrlMisc.Bits.ddrtio_pmreset_mask = 1;
    WriteFnvCfg(host, socket, ch, dimm, MSC_PLL_CTRL_FNV_DFX_MISC_0_REG, pllCtrlMisc.Data);

    //
    // 8. Wait 10usec
    //

    FixedDelay (host, 10);

    //
    // 9. Clear ddrt clk idle mask
    //

    pllCtrlMisc.Bits.ddrtio_ckidle_mask = 0;
    WriteFnvCfg(host, socket, ch, dimm, MSC_PLL_CTRL_FNV_DFX_MISC_0_REG, pllCtrlMisc.Data);

  } // dimm loop
}
