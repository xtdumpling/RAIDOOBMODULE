//****************************************************************************
//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//****************************************************************************
//  File History
//
//  Rev. 1.04
//   Bug Fixed:  Corrected memory location display incorrect
//   Reason:     
//   Auditor:    Stephen Chen
//   Date:       Aug/03/2017
//
//  Rev. 1.03
//   Bug Fixed:  Force to show WARN_USER_DIMM_DISABLE warning on screen when last runtime UECC happened 
//   Reason:     Customer Nutanix report one DIMM was mapped out due to runtime UCE, but there is no
//               warning message shown during the following warm reboot POST time.(SEL log was correct)
//   Auditor:    Max Mu
//   Date:       Jul/06/2017
//
//  Rev. 1.02
//   Bug Fixed:  Remove redundant code 
//   Reason:     
//   Auditor:    Chen Lin
//   Date:       NOV/11/2016
//
//  Rev. 1.01
//   Bug Fixed:  Fixe Memory Map out feature could not work. 
//   Reason:     
//   Auditor:    Chen Lin
//   Date:       Sep/26/2016
//
//  Rev. 1.00
//      Bug Fixed:  Support SMC Memory map-out function.
//      Reason:     
//      Auditor:    Ivern Yeh
//      Date:       Jul/7/2016
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
 *      IMC and DDR modules compliant with JEDEC specification.
 *
 ************************************************************************/

#include "SysFunc.h"
#include "Pipe.h"
#include "PlatformFuncCommon.h"
#include "Token.h"	//SMCPKG_SUPPORT
#if SMCPKG_SUPPORT && MemoryMapOut_SUPPORT
#include "SspTokens.h"
#include <SmcLibCommon.h>
#include <Library/PeiServicesLib.h>
#include <ppi\ReadOnlyVariable2.h>
#include <Guid/SetupVariable.h>
extern void ReportErrorDIMMToScreen(
  PSYSHOST host,
  UINT8    majorCode,
  UINT8    minorCode,
  UINT8    logData,
  UINT8	   pipe
  );
#endif

extern UINT8  tWL_DDR3[MAX_SUP_FREQ];
extern UINT8  tWL_DDR4[MAX_SUP_FREQ];
//
// DCLK ratios
//
const UINT8   DCLKs[MAX_SUP_FREQ] = {6, 10, 8, 12, 10, 14, 12, 18, 14, 20, 16, 22, 18, 26, 20, 28, 22, 30, 24, 34, 26, 27, 28, 38, 30, 42, 32, 33};
//
// BCLK frequency
//
const UINT8   BCLKs[MAX_SUP_FREQ] = {133, 100, 133, 100, 133, 100, 133, 100, 133, 100, 133, 100, 133, 100, 133, 100, 133, 100, 133, 100, 133, 133, 133, 100, 133, 100, 133, 100};
#ifdef _MSC_VER
#pragma warning(disable : 4740)
#endif

#define NO_POST_CODE  0xFF
#define NOT_SUP     0xFF

#define FREQ_COUNT 9
UINT8 supportedFrequencies[FREQ_COUNT] = {DDR_800, DDR_1066, DDR_1333, DDR_1600, DDR_1866, DDR_2133, DDR_2400, DDR_2666, DDR_2933};
UINT8 bclkfreq[2] = {133,100};

//
// Local Prototypes
//
void   SetRankIndexes(PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, UINT8 rank);
UINT32 SetDDRFreq(PSYSHOST host, UINT8 socket);
void LogAdvMemtestError (PSYSHOST host, UINT8 socket, UINT8 channel, UINT8 dimm, UINT8 rank, UINT8 testType, CPGC_ERROR_STATUS_MATS errorInfo);


// -------------------------------------
// Code section
// -------------------------------------

UINT32
CheckVdd (
  PSYSHOST                  host
  )
/*++

  This function checks if Vdd needs to be set

  @param host       - Pointer to sysHost

  @retval SUCCESS:  Memory initialization completed successfully.

--*/
{
  UINT8   socket = 0;

  for (socket = 0; socket < MAX_SOCKET; socket++) {
    //
    // Continue if this socket is not present
    //
    if ((host->nvram.mem.socket[socket].enabled == 0)) continue;

    {
      CHIP_FUNC_CALL(host, SetVdd (host, socket));
    }
  } // socket loop

  return SUCCESS;
} // CheckVdd

#ifdef SERIAL_DBG_MSG
void
CheckMappedOut (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     dimm
  )
/*++

  Displays information about the DIMMs present

  @param host  - Pointer to sysHost

  @retval N/A

--*/
{
  UINT8 ch;

  for (ch = 0; ch < MAX_CH; ch++) {

    EmptyBlock(host, 20);
  } // ch loop
} // CheckMappedOut
#endif //SERIAL_DBG_MSG

UINT32
InitDdrClocks (
              PSYSHOST host
              )
/*++

  Determines what DDR frequency to run at and programs it

  @param host  - Pointer to sysHost

  @retval SUCCESS - Clock is set properly
  @retval FAILURE - A reset is required to set the clock

--*/
{
  UINT8                   socket;
  UINT8                   ch;
  UINT8                   ddrFreq[MAX_SOCKET];
  UINT8                   tmpFreq;
  UINT8                   commonFreq;
  INT32                   commonTck;
  UINT8                   maxDdrFreq;
  UINT8                   commonDramVdd;
  struct channelNvram     (*channelNvList)[MAX_CH];

  commonDramVdd = SPD_VDD_135; // why 1.35?

  //
  // Bypass for the S3 resume path
  //
  if (host->var.common.bootMode == S3Resume || host->var.mem.subBootMode == WarmBootFast || host->var.mem.subBootMode == ColdBootFast) {
    //
    // On S3 resume, just make sure to set the DDR frequency and exit.
    //
    for (socket = 0; socket < MAX_SOCKET; socket++) {
      //
      // Loop for each CPU socket
      //
      if ((host->nvram.mem.socket[socket].enabled == 0) || (host->nvram.mem.socket[socket].maxDimmPop == 0)) continue;
      SetDDRFreq (host, socket);
    } // socket loop

    return SUCCESS;
  }

  if (host->nvram.mem.dramType == SPD_TYPE_DDR3){
    //
    // If any socket does not support v1.35, force all sockets to v1.5
    //
    for (socket = 0; socket < MAX_SOCKET; socket++) {
      //
      // Loop for each CPU socket
      //
      if ((host->nvram.mem.socket[socket].enabled == 0) || (host->nvram.mem.socket[socket].maxDimmPop == 0)) continue;

      //
      // If Vdd is not 1.5V and we aren't trying to force a Vdd check to see if the configuration supports it
      //
      if (host->nvram.mem.socket[socket].ddrVoltage && !host->setup.mem.socket[socket].ddrVddLimit) {

        channelNvList = GetChannelNvList(host, socket);

        for (ch = 0; ch < MAX_CH; ch++) {
          if ((*channelNvList)[ch].enabled == 0) continue;

            //
            // Check if the desired Vdd is supported by this DIMM configuration
            //
            if (CHIP_FUNC_CALL(host, CheckPORDDRVdd(host, socket, ch, host->nvram.mem.socket[socket].ddrVoltage))){
              // Force to Vdd 1.5V if the desired voltage is not supported.
              host->nvram.mem.socket[socket].ddrVoltage = SPD_VDD_150;
              break;
            }
          } // ch loop
        }

      commonDramVdd &= host->nvram.mem.socket[socket].ddrVoltage;
    } // socket loop
  }else {
    // DDR4 only supports 1.2V
    commonDramVdd = SPD_VDD_120;
  }

  for (socket = 0; socket < MAX_SOCKET; socket++) {
    //
    // Loop for each CPU socket
    //
    host->nvram.mem.socket[socket].ddrVoltage = commonDramVdd;
  } // socket loop

  commonFreq = 0xFF; //Init to highest value. Actual value gets set in the below loop.

  for (socket = 0; socket < MAX_SOCKET; socket++) {
    //
    // Loop for each CPU socket
    //
    if ((host->nvram.mem.socket[socket].enabled == 0) || (host->nvram.mem.socket[socket].maxDimmPop == 0)) continue;

    ddrFreq[socket] = 0;

    //
    // Search enabled IMCs
    //
    channelNvList = GetChannelNvList(host, socket);

    if (!(host->setup.mem.ddrFreqLimit == 0) && (host->setup.mem.options & MEM_OVERRIDE_EN)) {
      //
      // The DDR frequency has been provided by an input parameter
      //
      ddrFreq[socket] = host->setup.mem.ddrFreqLimit - 1;
      commonFreq = ddrFreq[socket];
    } else {

      // Determine common tCK for host controller. This needs to be reset for each socket.
      //
      commonTck = host->nvram.mem.socket[socket].minTCK;

      if (commonTck <= DDR_4400_TCK_MIN) {
        ddrFreq[socket] = DDR_4400;
      } else if (commonTck <= DDR_4266_TCK_MIN) {
        ddrFreq[socket] = DDR_4266;
      } else if (commonTck <= DDR_4200_TCK_MIN) {
        ddrFreq[socket] = DDR_4200;
      } else if (commonTck <= DDR_4000_TCK_MIN) {
        ddrFreq[socket] = DDR_4000;
      } else if (commonTck <= DDR_3800_TCK_MIN) {
        ddrFreq[socket] = DDR_3800;
      } else if (commonTck <= DDR_3733_TCK_MIN) {
        ddrFreq[socket] = DDR_3733;
      } else if (commonTck <= DDR_3600_TCK_MIN) {
        ddrFreq[socket] = DDR_3600;
      } else if (commonTck <= DDR_3466_TCK_MIN) {
        ddrFreq[socket] = DDR_3466;
      } else if (commonTck <= DDR_3400_TCK_MIN) {
        ddrFreq[socket] = DDR_3400;
      } else if (commonTck <= DDR_3200_TCK_MIN) {
        ddrFreq[socket] = DDR_3200;
      } else if (commonTck <= DDR_2933_TCK_MIN) {
        ddrFreq[socket] = DDR_2933;
      } else if (commonTck <= DDR_2800_TCK_MIN) {
        ddrFreq[socket] = DDR_2800;
      } else if (commonTck <= DDR_2666_TCK_MIN) {
        ddrFreq[socket] = DDR_2666;
      } else if (commonTck <= DDR_2600_TCK_MIN) {
        ddrFreq[socket] = DDR_2600;
      } else if (commonTck <= DDR_2400_TCK_MIN) {
        ddrFreq[socket] = DDR_2400;
      } else if (commonTck <= DDR_2133_TCK_MIN) {
        ddrFreq[socket] = DDR_2133;
      } else if (commonTck <= DDR_1866_TCK_MIN) {
        ddrFreq[socket] = DDR_1866;
      } else if (commonTck <= DDR_1600_TCK_MIN) {
        ddrFreq[socket] = DDR_1600;
      } else if (commonTck <= DDR_1333_TCK_MIN) {
        ddrFreq[socket] = DDR_1333;
      } else if (commonTck <= DDR_1066_TCK_MIN) {
        ddrFreq[socket] = DDR_1066;
      } else {
        ddrFreq[socket] = DDR_800;
      }

      //
      // Check the DDR frequency supported by each channel and use the fastest common frequency
      //
      if (host->setup.mem.enforcePOR != ENFORCE_POR_DIS) {
        for (ch = 0; ch < MAX_CH; ch++) {
          if ((*channelNvList)[ch].enabled == 0) continue;

          //
          // Returns the frequency supported by the memory configuration on this channel
          //
          tmpFreq = CHIP_FUNC_CALL(host, GetPORDDRFreq (host, socket, ch, commonDramVdd));

          //
          // Map out this channel if the configuration is not supported
          //
          if(tmpFreq == NOT_SUP) {
            DisableChannel(host, socket, ch);
            OutputWarning (host, WARN_DIMM_COMPAT, WARN_CHANNEL_CONFIG_FREQ_NOT_SUPPORTED, socket, ch, 0xFF, 0xFF);
          } else {
            ddrFreq[socket] = (UINT8)MIN(tmpFreq, ddrFreq[socket]);
          }
        } // ch loop
      }
    } // if (!(host->ddrFreqLimit == 0) && (host->setup.mem.options & MEM_OVERRIDE_EN))

    maxDdrFreq = CHIP_FUNC_CALL(host, GetMaxFreq(host, socket));

    //
    // Check if the DDR frequency has been specified by an input parameter
    //
    if (host->setup.mem.ddrFreqLimit) {
      //
      // Make sure this frequency is supported
      //
      if (ddrFreq[socket] > (host->setup.mem.ddrFreqLimit - 1)) {
        ddrFreq[socket] = (host->setup.mem.ddrFreqLimit - 1);
      }

      //
      // Check if we want to ignore POR restrictions
      //
      if (host->setup.mem.enforcePOR == ENFORCE_POR_DIS) {
        ddrFreq[socket] = host->setup.mem.ddrFreqLimit - 1;
      }
    }

    //
    // If overclocking is not enabled and the supported ratio is lower than the desired ratio then cap the desired ratio
    //
    if (ddrFreq[socket] > maxDdrFreq) {
      ddrFreq[socket] = maxDdrFreq;
      MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                    "The requested memory speed is faster than this processor supports. Set to maxDdrFreq = %d\n", maxDdrFreq));
    }

    //
    // Check if the frequency needs to be overridden
    //
    CHIP_FUNC_CALL(host, ChipOverrideFreq(host, socket, ddrFreq));

    if ((ddrFreq[socket] < commonFreq)) commonFreq = ddrFreq[socket];
  } // socket loop

  // Use common frequency across sockets if mixing is not supported
  for (socket = 0; socket < MAX_SOCKET; socket++) {
    ddrFreq[socket] = commonFreq;
  } // socket loop

  if (host->var.mem.subBootMode == WarmBoot || host->var.mem.subBootMode == WarmBootFast || host->var.mem.subBootMode == ColdBootFast) {
    for (socket = 0; socket < MAX_SOCKET; socket++) {
      if ((host->nvram.mem.socket[socket].enabled == 0) || (host->nvram.mem.socket[socket].maxDimmPop == 0)) continue;

      if (host->nvram.mem.socket[socket].ddrFreq != ddrFreq[socket]) {
        host->nvram.mem.socket[socket].ddrFreq = ddrFreq[socket];

        //
        // Disable the warm boot path because the DDR frequency has changed
        //
        host->var.mem.subBootMode = ColdBoot;
        //
        // Also, make sure you don't skip the memory init
        //
        host->var.mem.skipMemoryInit = 0;
      }
    } // socket loop
  } else {
    for (socket = 0; socket < MAX_SOCKET; socket++) {

      host->nvram.mem.socket[socket].ddrFreq = ddrFreq[socket];
    } // socket loop
  }

  for (socket = 0; socket < MAX_SOCKET; socket++) {
    //
    // Loop for each CPU socket
    //
    if ((host->nvram.mem.socket[socket].enabled == 0) || (host->nvram.mem.socket[socket].maxDimmPop == 0)) continue;

    SetDDRFreq (host, socket);
  } // socket loop

  return SUCCESS;
} // InitDdrClocks


UINT32
SetDDRFreq (
  PSYSHOST  host,
  UINT8     socket
  )
/*++

  Set the DDR clock frequency

  @param host  - Pointer to sysHost

  @retval SUCCESS - Clock is set properly
  @retval FAILURE - A reset is required to set the clock

--*/
{
  UINT8                         ch;
  UINT8                         dimm;
  UINT8                         bclk;
  UINT8                         DCLKRatio;
  UINT8                         req_type;
  UINT8                         req_data;
  struct dimmNvram              (*dimmNvList)[MAX_DIMM];
  struct channelNvram           (*channelNvList)[MAX_CH];

  switch (host->nvram.mem.socket[socket].ddrFreq) {
  //
  // DDR 800
  //
  case DDR_800:
    host->nvram.mem.socket[socket].ddrFreqMHz = 400;
    break;

  //
  // DDR 1000
  //
  case DDR_1000:
    host->nvram.mem.socket[socket].ddrFreqMHz = 500;
    break;

  //
  // DDR 1066
  //
  case DDR_1066:
    host->nvram.mem.socket[socket].ddrFreqMHz = 533;
    break;

  //
  // DDR 1200
  //
  case DDR_1200:
    host->nvram.mem.socket[socket].ddrFreqMHz = 600;
    break;

  //
  // DDR 1333
  //
  case DDR_1333:
    host->nvram.mem.socket[socket].ddrFreqMHz = 667;
    break;

  //
  // DDR 1400
  //
  case DDR_1400:
    host->nvram.mem.socket[socket].ddrFreqMHz = 700;
    break;

  //
  // DDR 1600
  //
  case DDR_1600:
    host->nvram.mem.socket[socket].ddrFreqMHz = 800;
    break;

  //
  // DDR 1800
  //
  case DDR_1800:
    host->nvram.mem.socket[socket].ddrFreqMHz = 900;
    break;

  //
  // DDR 1866
  //
  case DDR_1866:
    host->nvram.mem.socket[socket].ddrFreqMHz = 933;
    break;

  //
  // DDR 2000
  //
  case DDR_2000:
    host->nvram.mem.socket[socket].ddrFreqMHz = 1000;
    break;

  //
  // DDR 2133
  //
  case DDR_2133:
    host->nvram.mem.socket[socket].ddrFreqMHz = 1066;
    break;

  //
  // DDR 2200
  //
  case DDR_2200:
    host->nvram.mem.socket[socket].ddrFreqMHz = 1100;
    break;

  //
  // DDR 2400
  //
  case DDR_2400:
    host->nvram.mem.socket[socket].ddrFreqMHz = 1200;
    break;

  //
  // DDR 2600
  //
  case DDR_2600:
    host->nvram.mem.socket[socket].ddrFreqMHz = 1300;
    break;

  //
  // DDR 2666
  //
  case DDR_2666:
    host->nvram.mem.socket[socket].ddrFreqMHz = 1333;
    break;

  //
  // DDR 2800
  //
  case DDR_2800:
    host->nvram.mem.socket[socket].ddrFreqMHz = 1400;
    break;

  //
  // DDR 2933
  //
  case DDR_2933:
    host->nvram.mem.socket[socket].ddrFreqMHz = 1467;
    break;

  //
  // DDR 3000
  //
  case DDR_3000:
    host->nvram.mem.socket[socket].ddrFreqMHz = 1500;
    break;

  //
  // DDR 3200
  //
  case DDR_3200:
    host->nvram.mem.socket[socket].ddrFreqMHz = 1600;
    break;

  //
  // DDR 3400
  //
  case DDR_3400:
    host->nvram.mem.socket[socket].ddrFreqMHz = 1700;
    break;

  //
  // DDR 3466
  //
  case DDR_3466:
    host->nvram.mem.socket[socket].ddrFreqMHz = 1733;
    break;

  //
  // DDR 3600
  //
  case DDR_3600:
    host->nvram.mem.socket[socket].ddrFreqMHz = 1800;
    break;

  //
  // DDR 3733
  //
  case DDR_3733:
    host->nvram.mem.socket[socket].ddrFreqMHz = 1866;
    break;

  //
  // DDR 3800
  //
  case DDR_3800:
    host->nvram.mem.socket[socket].ddrFreqMHz = 1900;
    break;

  //
  // DDR 4000
  //
  case DDR_4000:
    host->nvram.mem.socket[socket].ddrFreqMHz = 2000;
    break;

  //
  // DDR 4200
  //
  case DDR_4200:
    host->nvram.mem.socket[socket].ddrFreqMHz = 2100;
    break;

  //
  // DDR 4266
  //
  case DDR_4266:
    host->nvram.mem.socket[socket].ddrFreqMHz = 2133;
    break;

  //
  // DDR 4400
  //
  case DDR_4400:
    host->nvram.mem.socket[socket].ddrFreqMHz = 2200;
    break;

  //
  // Default DDR 1066 for DDR3, DDR 1600 for DDR4
  //
  default:
    if (host->nvram.mem.dramType == SPD_TYPE_DDR3){
      host->nvram.mem.socket[socket].ddrFreqMHz = 533;
    } else {
      host->nvram.mem.socket[socket].ddrFreqMHz = 800;
    }
    break;
  }

  //
  // QCLK Period in pico seconds
  //
  host->nvram.mem.socket[socket].QCLKps = (UINT16) (1000000/(host->nvram.mem.socket[socket].ddrFreqMHz * 2));

  //
  // Save ratio table index for later use
  //
  host->nvram.mem.ratioIndex = host->nvram.mem.socket[socket].ddrFreq;

  MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                  "ratioIndex = %d\n", host->nvram.mem.ratioIndex));

  channelNvList = GetChannelNvList(host, socket);

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

    if (host->nvram.mem.dramType == SPD_TYPE_DDR3) {
      (*channelNvList)[ch].common.nWL = tWL_DDR3[host->nvram.mem.ratioIndex];
    } else {
      (*channelNvList)[ch].common.nWL = tWL_DDR4[host->nvram.mem.ratioIndex];

      // Initialize ddrtEnabled variable
      (*channelNvList)[ch].ddrtEnabled = 0;
      dimmNvList = GetDimmNvList(host, socket, ch);
      for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {
        if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
        if ((*dimmNvList)[dimm].aepDimmPresent == 0) continue;
        (*channelNvList)[ch].ddrtEnabled = 1;
      }

      //Enforce tWL minimum for SKX NVMDIMM_IF logic
      if (((*channelNvList)[ch].ddrtEnabled) && ((*channelNvList)[ch].common.nWL < 11)) {
        (*channelNvList)[ch].common.nWL = 11;
      }
    }
    (*channelNvList)[ch].timingMode = TIMING_1N; // Default timing mode.
    //
    // Set correct timing mode
    // Enable 2N timing for UDIMMs/SODIMMs if more than 1 UDIMM/SODIMM is installed on this channel
    //
    if ((host->nvram.mem.dimmTypePresent != RDIMM) && ((*channelNvList)[ch].maxDimm > 1)) {
      (*channelNvList)[ch].timingMode = TIMING_2N;
    }

    //
    // All UDIMMs 2133 and above run with 2N timings
    //
    if ((host->nvram.mem.socket[socket].ddrFreq >= DDR_2133) && (host->nvram.mem.dimmTypePresent != RDIMM)) {
      (*channelNvList)[ch].timingMode = TIMING_2N;
    }

    if ((host->setup.mem.options & MEM_OVERRIDE_EN) && host->setup.mem.inputMemTime.nCMDRate) {
      (*channelNvList)[ch].timingMode = host->setup.mem.inputMemTime.nCMDRate;
      if ((*channelNvList)[ch].timingMode == 1) {
        (*channelNvList)[ch].timingMode--;
      }
    }
  } // ch loop

  //
  // Initialize DCLK ratio if desired ratio != DDRFRQ
  // Get desired DCLK ratio
  //
  if (host->setup.mem.ddrFreqLimit == 0) {
    bclk = BCLK_133;
  } else {
    if ((host->setup.mem.imcBclk == IMC_BCLK_AUTO) || (host->nvram.mem.socket[socket].ddrFreqMHz % 100)) {
      bclk = BCLKs[host->nvram.mem.ratioIndex];
    } else if (host->setup.mem.imcBclk == IMC_BCLK_100) {
      bclk = 100;
    } else {
      bclk = 133;
    }
  }
  //
  // Added call to GetDCLKs to support KNL's change of using QCLKs_100 and QCLKs_133 with DCLKs equal to the appropriate QCLK divided by 2
  //
  DCLKRatio = CHIP_FUNC_CALL(host, GetDCLKs(host->nvram.mem.ratioIndex, bclk));

  //
  // Save BCLK speed in case it's needed later
  //
  host->var.mem.memBCLK = bclk;

  //
  // Get current DCLK ratio
  //
  CHIP_FUNC_CALL(host, GetDclkRatio(host, socket, &req_type, &req_data));

  if (bclk == BCLK_133){
    host->var.mem.QCLKPeriod = 1000000 / (DCLKRatio * 133 + (DCLKRatio / 3));
  } else {
    // BCLK = 100Mhz
    host->var.mem.QCLKPeriod = 1000000 / (DCLKRatio * 100);
  }

  if (host->var.common.resetRequired) {
    MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                  "Reset requested: non-MRC\n"));
  }

  //
  // Bubble Generator Setup
  //
  CHIP_FUNC_CALL(host, ProgramBgfTable (host, socket));

  // before warm reset B0 put in fix here.

  //
  // Compare current DCLK ratio to desired DCLK ratio
  //
  if ((DCLKRatio != req_data) || (bclk != bclkfreq[req_type])) {
    //
    // Skip system reset for default ratio
    // Indicate a reset is required
    //

    //
    //Set BCLK
    //
    if (bclk == BCLK_133) {
      req_type = 0;
    } else {
      req_type = 1;
    }

    OutputExtendedCheckpoint((host, STS_CLOCK_INIT, SUB_SET_FREQ, socket));

    // Only called if !SIM_BUILD or RC_SIM

    CHIP_FUNC_CALL(host, SetDclkRatio(host, socket, req_type, DCLKRatio));
    host->var.common.resetRequired |= POST_RESET_WARM;

  } // if frequency

  //
  // Exit with failure if the MRC requires a reset or if a reset was requested before the MRC was called
  // Exiting with failure will force the MRC to return execution to the caller
  //
  if (host->var.common.resetRequired) {
    return FAILURE;
  }

#ifdef SERIAL_DBG_MSG
  if (host->nvram.mem.socket[socket].maxDimmPop != 0) {
    if (checkMsgLevel(host, SDBG_MINMAX)) {
      getPrintFControl(host);
      rcPrintf ((host, "Memory behind processor %d running at ", socket));
      DisplayDDRFreq (host, socket);
      releasePrintFControl(host);
    } // MsgLevel
  }
#endif
  return SUCCESS;
} // SetDDRFreq

UINT8
GetClosestFreq (
               PSYSHOST  host,
               UINT8     socket
               )
/*++

  Initialize the DDRIO interface

  @param host  - Pointer to sysHost

  @retval SUCCESS

--*/
{
  UINT8 freqLoop;
  UINT8 freq = host->nvram.mem.socket[socket].ddrFreq;
  //
  // Make sure the frequency is part of the table
  //
  if(host->nvram.mem.socket[socket].ddrFreq > supportedFrequencies[FREQ_COUNT - 1]) {
    freq = supportedFrequencies[FREQ_COUNT - 1];
    return freq;
  }
  for(freqLoop = 0; freqLoop < FREQ_COUNT; freqLoop++) {
    if(host->nvram.mem.socket[socket].ddrFreq == supportedFrequencies[freqLoop]) {
      freq = host->nvram.mem.socket[socket].ddrFreq;
      //
      // Frequency found
      //
      break;
    }

    if (freqLoop < (FREQ_COUNT - 1)) {
      if((host->nvram.mem.socket[socket].ddrFreq > supportedFrequencies[freqLoop]) && (host->nvram.mem.socket[socket].ddrFreq < supportedFrequencies[freqLoop + 1])) {
        freq = supportedFrequencies[freqLoop + 1];
        break;
      }
    }
  }

  return freq;
}


UINT32
GetChBitmask (
             PSYSHOST  host,
             UINT8     socket,
             UINT8     dimm,
             UINT8     rank
             )
/*++

  Returns a bitmask of channels that have the current dimm and rank enabled

  @param host  - Pointer to sysHost
  @param dimm  - DIMM to check
  @param rank  - rank to check

  @retval chBitmask - Bitmask of channels present for the current rank

--*/
{
  UINT8               ch;
  UINT32              chBitmask = 0;
  struct channelNvram (*channelNvList)[MAX_CH];

  channelNvList = GetChannelNvList(host, socket);

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

    if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

    chBitmask |= (1 << ch);
  } // ch loop

  return  chBitmask;
} // GetChBitmask

UINT32
GetChBitmaskLrdimm (
             PSYSHOST  host,
             UINT8     socket,
             UINT8     dimm,
             UINT8     rank
             )
/*++

  Returns a bitmask of channels that have the current lrdimm and rank enabled

  @param host  - Pointer to sysHost
  @param dimm  - DIMM to check
  @param rank  - rank to check

  @retval chBitmask - Bitmask of channels present for the current rank

--*/
{
  UINT8               ch;
  UINT32              chBitmask = 0;
  struct channelNvram (*channelNvList)[MAX_CH];

  channelNvList = GetChannelNvList(host, socket);

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

    if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

    if (CHIP_FUNC_CALL(host, IsLrdimmPresent(host, socket, ch, dimm)) == 0) continue;

    chBitmask |= (1 << ch);
  } // ch loop

  return  chBitmask;
} // GetChBitmaskLrdimm

UINT32
GetChBitmaskAll (
                PSYSHOST  host,
                UINT8     socket
                )
/*++

  Returns a bitmask of channels that are enabled

  @param host  - Pointer to sysHost
  @param socket  - Socket number

  @retval chBitmask - Bitmask of enabled channels

--*/
{
  UINT8               ch;
  UINT32              chBitmask = 0;
  struct channelNvram (*channelNvList)[MAX_CH];

  channelNvList = GetChannelNvList(host, socket);

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

    chBitmask |= (1 << ch);
  } // ch loop

  return  chBitmask;
} // GetChBitmaskAll

#ifdef  ADV_MEMTEST_SUPPORT

/**

  Runs Advanced Memory Tests based on the legacy Scan algorithm with DDR scrambler enabled

  @param host                - Pointer to sysHost
  @param socket              - Socket Id
  @param ddrChEnabled        - List of channels to be tested
  @param testType            - Memory test type
  @param testSize            - Memory test size
  @param shiftRate           - Rate to shift the WDB content

  @retval status             - SUCCESS/FAILURE

**/
UINT32
MemTestScram (
        PSYSHOST host,
        UINT8    socket,
        UINT8    ddrChEnabled,
        UINT8    testType,
        UINT8    testSize,
        UINT8    shiftRate,
        UINT32   repeatTest
        )
{
  UINT64_STRUCT    patternQW[MT_PATTERN_DEPTH];
  UINT64_STRUCT    shiftedQW[MT_PATTERN_DEPTH];
  UINT64_STRUCT    basePatternQW[MT_PATTERN_DEPTH];
  UINT8  uiShl[MT_PATTERN_DEPTH];
  UINT8  seqDataInv[MT_MAX_SUBSEQ];
  UINT8  numCl;
  UINT32 status = SUCCESS;

  MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "MemTestScram Starts\n"));

  basePatternQW[0].hi = 0x55AA55AA;
  basePatternQW[0].lo = 0x55AA55AA;
  numCl = 1;

  // Add long-term pattern uniqueness
  basePatternQW[0] = AddUINT64x32(basePatternQW[0], repeatTest >> 6);

  // Rotate pattern based on iteration
  shiftedQW[0] = RotateLeftUINT64 (basePatternQW[0], repeatTest % 64);

  // Invert pattern for odd iterations
  if (repeatTest % 2 == 0) {
    patternQW[0].hi = shiftedQW[0].hi;
    patternQW[0].lo = shiftedQW[0].lo;
  } else {
    patternQW[0].hi = ~shiftedQW[0].hi;
    patternQW[0].lo = ~shiftedQW[0].lo;
  }
  uiShl[0] = shiftRate;
  uiShl[1] = shiftRate;
  seqDataInv[0] = 0;
  seqDataInv[1] = 0;

  MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
    "patternQW[0] = 0x%08x%08x\n",
    patternQW[0].hi, patternQW[0].lo));

  {
    UINT64_STRUCT             startTsc  = { 0, 0 };
    UINT64_STRUCT             endTsc = { 0, 0 };

    //1. Write pattern to all of memory
    if (status == SUCCESS) {
      OutputCheckpoint(host, STS_MEMBIST, SUB_MT_STEP1, 0);
      ReadTsc64 (&startTsc);
      status = CHIP_FUNC_CALL(host, CpgcMemTestMATS (host, socket, ddrChEnabled, testType, MT_CPGC_WRITE, seqDataInv, patternQW, uiShl, numCl, 1, MT_ADDR_DIR_UP));
      ReadTsc64 (&endTsc);
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
        "Step1 latency = %d ms\n", TimeDiff (host, startTsc, endTsc, TDIFF_UNIT_MS)));
    }
    //2. Read and compare data pattern
    if (status == SUCCESS) {
      OutputCheckpoint(host, STS_MEMBIST, SUB_MT_STEP2, 0);
      ReadTsc64 (&startTsc);
      status = CHIP_FUNC_CALL(host, CpgcMemTestMATS (host, socket, ddrChEnabled, testType, MT_CPGC_READ, seqDataInv, patternQW, uiShl, numCl, 1, MT_ADDR_DIR_UP));
      ReadTsc64 (&endTsc);
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
        "Step2 latency = %d ms\n", TimeDiff (host, startTsc, endTsc, TDIFF_UNIT_MS)));
    }
    if (status == SUCCESS) {
      OutputCheckpoint(host, STS_MEMBIST, SUB_MT_STEP3, 0);
    } else {
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
        "Test Failed!\n"));
    }
  }
  MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "MemTestScram Ends\n"));
  return status;
} // MemTestScram


/**

  Runs optimized Memory Init = 0 with DDR scrambler enabled

  @param host                - Pointer to sysHost
  @param socket              - Socket Id
  @param ddrChEnabled        - List of channels to be tested

  @retval status             - SUCCESS/FAILURE

**/
UINT32
MemInitOpt (
        PSYSHOST host,
        UINT8    socket,
        UINT32   ddrChEnabled
        )
{
  UINT64_STRUCT    basePatternQW[MT_PATTERN_DEPTH];
  UINT8  uiShl[MT_PATTERN_DEPTH];
  UINT8  seqDataInv[MT_MAX_SUBSEQ];
  UINT8  numCl;
  UINT8  testType = ADV_MT_TYPE_LINIT_SCRAM;
  UINT32 status = SUCCESS;

  MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "MemInitOpt Starts\n"));

  basePatternQW[0].hi = 0;
  basePatternQW[0].lo = 0;
  numCl = 1;
  uiShl[0] = 0;
  uiShl[1] = 0;
  seqDataInv[0] = 0;
  seqDataInv[1] = 0;

  MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
    "patternQW[0] = 0x%08x%08x\n",
    basePatternQW[0].hi, basePatternQW[0].lo));

  //1. Write pattern to all of memory
  OutputCheckpoint(host, STS_MEMBIST, SUB_MT_STEP1, 0);
  status = CHIP_FUNC_CALL(host, CpgcMemTestMATS (host, socket, ddrChEnabled, testType, MT_CPGC_WRITE, seqDataInv, basePatternQW, uiShl, numCl, 1, MT_ADDR_DIR_UP));
  MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "MemInitOpt Ends\n"));
  return status;
} // MemInitOpt


/**

  Runs Advanced Memory Tests based on the MATS+ algorithm

  @param host                - Pointer to sysHost
  @param socket              - Socket Id
  @param ddrChEnabled        - List of channels to be tested
  @param testType            - Memory test type
  @param testSize            - Memory test size
  @param shiftRate           - Rate to shift the WDB content

  @retval status             - SUCCESS/FAILURE

**/
UINT32
MemTestMATSN (
        PSYSHOST host,
        UINT8    socket,
        UINT8    ddrChEnabled,
        UINT8    testType,
        UINT8    testSize,
        UINT8    shiftRate
        )
{
  UINT64_STRUCT    patternQW[MT_PATTERN_DEPTH];
  UINT64_STRUCT    inverseQW[MT_PATTERN_DEPTH];
  UINT64_STRUCT    shiftedQW[MT_PATTERN_DEPTH];
  UINT64_STRUCT    basePatternQW[MT_PATTERN_DEPTH];
  UINT8  uiShl[MT_PATTERN_DEPTH];
  UINT8  seqDataInv[MT_MAX_SUBSEQ];
  UINT8  numCl;
  UINT32  i, j;
  UINT32 status = SUCCESS;

  MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "\nMemTestMATS%d\n", testSize));
  /*
  // Patterns
  8-bit Patterns          16-bit Patterns         32-bit Patterns         64-bit Patterns

  0101010101010101h       0001000100010001h       0000000100000001h       0000000000000001h
  0202020202020202h       0002000200020002h       0000000200000002h       0000000000000002h
  0404040404040404h       0004000400040004h       0000000400000004h       0000000000000004h
  0808080808080808h       0008000800080008h       0000000800000008h       0000000000000008h
  1010101010101010h         ...                     ...                     ...
  2020202020202020h
  4040404040404040h       4000400040004000h       4000000040000000h       4000000000000000h
  8080808080808080h       8000800080008000h       8000000080000000h       8000000000000000h
  0101010101010101h       0001000100010001h       0000000100000001h       0000000000000001h
  ...                     ...                     ...                     ...

  // XMATS algorithm
  (1)
    FOR each 64-bit memory location (increasing sequentially through memory)
      Write sliding data pattern to all of memory
    END FOR
  (2)
    FOR each 64-bit memory location (increasing sequentially through memory)
      read and compare pattern
      IF compare fails
        EXIT with failure message
      write inverse pattern
    END FOR
  (3)
    FOR each 64-bit memory location (decreasing sequentially through memory)
      read and compare inverse pattern
      IF compare fails
        EXIT with failure message
      write original pattern
    END FOR
  (4)
    FOR each 64-bit memory location (increasing sequentially through memory)
      write inverse pattern
    END FOR
  (5)
    FOR each 64-bit memory location (increasing sequentially through memory)
      read and compare inverse pattern
      IF compare fails
        EXIT with failure message
      write original pattern
    END FOR
  (6)
    FOR each 64-bit memory location (decreasing sequentially through memory)
      read and compare original pattern
      IF compare fails
        EXIT with failure message
      Write complement pattern
    END FOR
  */

  switch (testSize) {
  case 8:
    basePatternQW[0].hi = 0x01010101;
    basePatternQW[0].lo = 0x01010101;
    numCl = 1;
    break;
  case 16:
    basePatternQW[0].hi = 0x00010001;
    basePatternQW[0].lo = 0x00010001;
    numCl = 2;
    break;
  case 32:
    basePatternQW[0].hi = 0x00000001;
    basePatternQW[0].lo = 0x00000001;
    numCl = 4;
    break;
  case 64:
    basePatternQW[0].hi = 0x00000000;
    basePatternQW[0].lo = 0x00000001;
    numCl = 8;
    break;
  default:
    numCl = 0;
    RC_ASSERT(FALSE, ERR_RC_INTERNAL, RC_ASSERT_MINOR_CODE_66);
  }

  // SW loop for testSize larger than numCl
  for (j = 0; j < testSize; j = j + (numCl * 8)) {

    // Test each bit position per UI
    for (i = 0; i < (UINT32)(numCl * 8); i++) {

      shiftedQW[0] = LShiftUINT64 (basePatternQW[0], i);

      patternQW[0].hi = shiftedQW[0].hi;
      patternQW[0].lo = shiftedQW[0].lo;
      inverseQW[0].hi = ~shiftedQW[0].hi;
      inverseQW[0].lo = ~shiftedQW[0].lo;
      uiShl[0] = shiftRate;
      uiShl[1] = shiftRate;
      seqDataInv[0] = 0;
      seqDataInv[1] = 1;

      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                    "patternQW[0] = 0x%08x%08x, inverseQW[0] = 0x%08x%08x\n",
                     patternQW[0].hi, patternQW[0].lo, inverseQW[0].hi, inverseQW[0].lo));

      {
        UINT64_STRUCT             startTsc  = { 0, 0 };
        UINT64_STRUCT             endTsc = { 0, 0 };

        //1. Write sliding data pattern to all of memory
        if (status == SUCCESS) {
          OutputCheckpoint(host, STS_MEMBIST, SUB_MT_STEP1, (UINT16)(i + j));
          ReadTsc64 (&startTsc);
          status = CHIP_FUNC_CALL(host, CpgcMemTestMATS (host, socket, ddrChEnabled, testType, MT_CPGC_WRITE, seqDataInv, patternQW, uiShl, numCl, 1, MT_ADDR_DIR_UP));
          ReadTsc64 (&endTsc);
          MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                        "Step1 latency = %d ms\n", TimeDiff (host, startTsc, endTsc, TDIFF_UNIT_MS)));
        }
        //2. Read and compare data pattern
        //   Write inverse pattern
        if (status == SUCCESS) {
          OutputCheckpoint(host, STS_MEMBIST, SUB_MT_STEP2, (UINT16)(i + j));
          ReadTsc64 (&startTsc);
          status = CHIP_FUNC_CALL(host, CpgcMemTestMATS (host, socket, ddrChEnabled, testType, MT_CPGC_READ_WRITE, seqDataInv, patternQW, uiShl, numCl, 1, MT_ADDR_DIR_UP));
          ReadTsc64 (&endTsc);
          MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                        "Step2 latency = %d ms\n", TimeDiff (host, startTsc, endTsc, TDIFF_UNIT_MS)));
        }
        //3. Read and compare inverse pattern
        //   Write original pattern
        if (status == SUCCESS) {
          OutputCheckpoint(host, STS_MEMBIST, SUB_MT_STEP3, (UINT16)(i + j));
          ReadTsc64 (&startTsc);
          status = CHIP_FUNC_CALL(host, CpgcMemTestMATS (host, socket, ddrChEnabled, testType, MT_CPGC_READ_WRITE, seqDataInv, inverseQW, uiShl, numCl, 1, MT_ADDR_DIR_DN));
          ReadTsc64 (&endTsc);
          MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                        "Step3 latency = %d ms\n", TimeDiff (host, startTsc, endTsc, TDIFF_UNIT_MS)));
        }
        //4. Write inverse pattern to all of memory
        if (status == SUCCESS) {
          OutputCheckpoint(host, STS_MEMBIST, SUB_MT_STEP4, (UINT16)(i + j));
          ReadTsc64 (&startTsc);
          status = CHIP_FUNC_CALL(host, CpgcMemTestMATS (host, socket, ddrChEnabled, testType, MT_CPGC_WRITE, seqDataInv, inverseQW, uiShl, numCl, 1, MT_ADDR_DIR_UP));
          ReadTsc64 (&endTsc);
          MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                        "Step4 latency = %d ms\n", TimeDiff (host, startTsc, endTsc, TDIFF_UNIT_MS)));
        }
        //5. Read and compare inverse pattern
        //   Write original pattern
        if (status == SUCCESS) {
          OutputCheckpoint(host, STS_MEMBIST, SUB_MT_STEP5, (UINT16)(i + j));
          ReadTsc64 (&startTsc);
          status = CHIP_FUNC_CALL(host, CpgcMemTestMATS (host, socket, ddrChEnabled, testType, MT_CPGC_READ_WRITE, seqDataInv, inverseQW, uiShl, numCl, 1, MT_ADDR_DIR_UP));
          ReadTsc64 (&endTsc);
          MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                        "Step5 latency = %d ms\n", TimeDiff (host, startTsc, endTsc, TDIFF_UNIT_MS)));
        }
        //6. Read and compare original pattern
        //   Write inverse pattern
        if (status == SUCCESS) {
          OutputCheckpoint(host, STS_MEMBIST, SUB_MT_STEP6, (UINT16)(i + j));
          ReadTsc64 (&startTsc);
          status = CHIP_FUNC_CALL(host, CpgcMemTestMATS (host, socket, ddrChEnabled, testType, MT_CPGC_READ_WRITE, seqDataInv, patternQW, uiShl, numCl, 1, MT_ADDR_DIR_DN));
          ReadTsc64 (&endTsc);
          MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                        "Step6 latency = %d ms\n", TimeDiff (host, startTsc, endTsc, TDIFF_UNIT_MS)));
        }
        if (status == SUCCESS) {
          OutputCheckpoint(host, STS_MEMBIST, SUB_MT_STEP7, (UINT16)(i + j));
        } else {
          MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                        "Test Failed!\n"));
          return status;
        }
      }
    }
  }
  return status;
} // MemTestMATSN


/**

  Runs Advanced Memory Tests based on the MCH algorithm

  @param host                - Pointer to sysHost
  @param socket              - Socket Id
  @param ddrChEnabled        - List of channels to be tested
  @param testType            - Memory test type
  @param testSize            - Memory test size
  @param mchRepeat           - Number of times to repeat each pattern

  @retval status             - SUCCESS/FAILURE

**/
UINT32
MemTestMCH (
        PSYSHOST host,
        UINT8    socket,
        UINT8    ddrChEnabled,
        UINT8    testType,
        UINT8    testSize,
        UINT8    mchRepeat
        )
{
  UINT64_STRUCT    patternQW[MT_PATTERN_DEPTH];
  UINT64_STRUCT    inverseQW[MT_PATTERN_DEPTH];
  UINT64_STRUCT    shiftedQW[MT_PATTERN_DEPTH];
  UINT64_STRUCT    basePatternQW[MT_PATTERN_DEPTH];
  UINT8  uiShl[MT_PATTERN_DEPTH];
  UINT8  seqDataInv[MT_MAX_SUBSEQ];
  UINT8  numCl;
  UINT8  repeat;
  UINT32  i;
  UINT32 status = SUCCESS;

  MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "\nMemTestMCH%d\n", testSize));
  /*
  initialize pattern to 0101010101010101h

  (1)
    FOR each 64-bit memory location (increasing sequentially through memory)
      write pattern
    END FOR
  (2)
    FOR each 64-bit memory location (increasing sequentially through memory)
      read and compare to pattern
      IF compare fails
        EXIT with failure message
      write pattern (purposely same value)
    END FOR
  (3)
    FOR each 64-bit memory location (increasing sequentially through memory)
      read and compare to pattern
      IF compare fails
        EXIT with failure message
    END FOR
  (4)
    FOR each 64-bit memory location (increasing sequentially through memory)
      write pattern
    END FOR
  (5)
    FOR each 64-bit memory location (increasing sequentially through memory)
      write inverse pattern
    END FOR
  (6)
    FOR each 64-bit memory location (increasing sequentially through memory)
      read and compare to inverse pattern
      IF compare fails
        EXIT with failure message
    END FOR
  (7)
    FOR each 64-bit memory location (increasing sequentially through memory)
      write pattern
    END FOR
  (8)
    FOR each 64-bit memory location (decreasing sequentially through memory)
      write inverse pattern
    END FOR
  (9)
    FOR each 64-bit memory location (increasing sequentially through memory)
      read and compare to inverse pattern
      IF compare fails
        EXIT with failure message
    END FOR

  Run the above twice and then rotate the pattern LEFT. Stop after the pattern
  has been rotated 8-bits (16 iterations of the loop).
  */

  switch (testSize) {
  case 8:
    basePatternQW[0].hi = 0x01010101;
    basePatternQW[0].lo = 0x01010101;
    numCl = 1;
    break;
  case 16:
    basePatternQW[0].hi = 0x00010001;
    basePatternQW[0].lo = 0x00010001;
    numCl = 2;
    break;
  case 32:
    basePatternQW[0].hi = 0x00000001;
    basePatternQW[0].lo = 0x00000001;
    numCl = 4;
    break;
  case 64:
    basePatternQW[0].hi = 0x00000000;
    basePatternQW[0].lo = 0x00000001;
    numCl = 8;
    break;
  default:
    numCl = 0;
    basePatternQW[0].hi = 0x00000000;
    basePatternQW[0].lo = 0x00000000;
    RC_ASSERT(FALSE, ERR_RC_INTERNAL, RC_ASSERT_MINOR_CODE_67);
  }

  for (i = 0; i < testSize; i++) {

    shiftedQW[0] = LShiftUINT64 (basePatternQW[0], i);

    patternQW[0].hi = shiftedQW[0].hi;
    patternQW[0].lo = shiftedQW[0].lo;
    inverseQW[0].hi = ~shiftedQW[0].hi;
    inverseQW[0].lo = ~shiftedQW[0].lo;
    uiShl[0] = 0;
    uiShl[1] = 0;
    seqDataInv[0] = 0;
    seqDataInv[1] = 0;

    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                  "patternQW[0] = 0x%08x%08x, inverseQW[0] = 0x%08x%08x\n",
                   patternQW[0].hi, patternQW[0].lo, inverseQW[0].hi, inverseQW[0].lo));

    for (repeat = 0; repeat < mchRepeat; repeat++) {

      UINT64_STRUCT             startTsc  = { 0, 0 };
      UINT64_STRUCT             endTsc = { 0, 0 };

      //1. Write data pattern to all of memory
      if (status == SUCCESS) {
        OutputCheckpoint(host, STS_MEMBIST, SUB_MT_STEP1, (UINT16)i);
        ReadTsc64 (&startTsc);
        status = CHIP_FUNC_CALL(host, CpgcMemTestMATS (host, socket, ddrChEnabled, testType, MT_CPGC_WRITE, seqDataInv, patternQW, uiShl, numCl, 1, MT_ADDR_DIR_UP));
        ReadTsc64 (&endTsc);
        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                      "Step1 latency = %d ms\n", TimeDiff (host, startTsc, endTsc, TDIFF_UNIT_MS)));
      }
      //2. read and compare data pattern
      //write same data pattern
      if (status == SUCCESS) {
        OutputCheckpoint(host, STS_MEMBIST, SUB_MT_STEP2, (UINT16)i);
        ReadTsc64 (&startTsc);
        status = CHIP_FUNC_CALL(host, CpgcMemTestMATS (host, socket, ddrChEnabled, testType, MT_CPGC_READ_WRITE, seqDataInv, patternQW, uiShl, numCl, 1, MT_ADDR_DIR_UP));
        ReadTsc64 (&endTsc);
        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                      "Step2 latency = %d ms\n", TimeDiff (host, startTsc, endTsc, TDIFF_UNIT_MS)));
      }
      //3. Read and compare data pattern
      if (status == SUCCESS) {
        OutputCheckpoint(host, STS_MEMBIST, SUB_MT_STEP3, (UINT16)i);
        ReadTsc64 (&startTsc);
        status = CHIP_FUNC_CALL(host, CpgcMemTestMATS (host, socket, ddrChEnabled, testType, MT_CPGC_READ, seqDataInv, patternQW, uiShl, numCl, 1, MT_ADDR_DIR_UP));
        ReadTsc64 (&endTsc);
        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                      "Step3 latency = %d ms\n", TimeDiff (host, startTsc, endTsc, TDIFF_UNIT_MS)));
      }
      //4. Write data pattern to all of memory
      if (status == SUCCESS) {
        OutputCheckpoint(host, STS_MEMBIST, SUB_MT_STEP4, (UINT16)i);
        ReadTsc64 (&startTsc);
        status = CHIP_FUNC_CALL(host, CpgcMemTestMATS (host, socket, ddrChEnabled, testType, MT_CPGC_WRITE, seqDataInv, patternQW, uiShl, numCl, 1, MT_ADDR_DIR_UP));
        ReadTsc64 (&endTsc);
        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                      "Step4 latency = %d ms\n", TimeDiff (host, startTsc, endTsc, TDIFF_UNIT_MS)));
      }
      //5. Write inverse pattern to all of memory
      if (status == SUCCESS) {
        OutputCheckpoint(host, STS_MEMBIST, SUB_MT_STEP5, (UINT16)i);
        ReadTsc64 (&startTsc);
        status = CHIP_FUNC_CALL(host, CpgcMemTestMATS (host, socket, ddrChEnabled, testType, MT_CPGC_WRITE, seqDataInv, inverseQW, uiShl, numCl, 1, MT_ADDR_DIR_UP));
        ReadTsc64 (&endTsc);
        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                      "Step5 latency = %d ms\n", TimeDiff (host, startTsc, endTsc, TDIFF_UNIT_MS)));
      }
      //6. Read and compare inverse pattern
      if (status == SUCCESS) {
        OutputCheckpoint(host, STS_MEMBIST, SUB_MT_STEP6, (UINT16)i);
        ReadTsc64 (&startTsc);
        status = CHIP_FUNC_CALL(host, CpgcMemTestMATS (host, socket, ddrChEnabled, testType, MT_CPGC_READ, seqDataInv, inverseQW, uiShl, numCl, 1, MT_ADDR_DIR_UP));
        ReadTsc64 (&endTsc);
        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                      "Step6 latency = %d ms\n", TimeDiff (host, startTsc, endTsc, TDIFF_UNIT_MS)));
      }
      //7. Write data pattern to all of memory
      if (status == SUCCESS) {
        OutputCheckpoint(host, STS_MEMBIST, SUB_MT_STEP7, (UINT16)i);
        ReadTsc64 (&startTsc);
        status = CHIP_FUNC_CALL(host, CpgcMemTestMATS (host, socket, ddrChEnabled, testType, MT_CPGC_WRITE, seqDataInv, patternQW, uiShl, numCl, 1, MT_ADDR_DIR_UP));
        ReadTsc64 (&endTsc);
        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                      "Step7 latency = %d ms\n", TimeDiff (host, startTsc, endTsc, TDIFF_UNIT_MS)));
      }
      //8. Write inverse pattern to all of memory
      if (status == SUCCESS) {
        OutputCheckpoint(host, STS_MEMBIST, SUB_MT_STEP8, (UINT16)i);
        ReadTsc64 (&startTsc);
        status = CHIP_FUNC_CALL(host, CpgcMemTestMATS (host, socket, ddrChEnabled, testType, MT_CPGC_WRITE, seqDataInv, inverseQW, uiShl, numCl, 1, MT_ADDR_DIR_DN));
        ReadTsc64 (&endTsc);
        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                      "Step8 latency = %d ms\n", TimeDiff (host, startTsc, endTsc, TDIFF_UNIT_MS)));
      }
      //9. Read and compare inverse pattern
      if (status == SUCCESS) {
        OutputCheckpoint(host, STS_MEMBIST, SUB_MT_STEP9, (UINT16)i);
        ReadTsc64 (&startTsc);
        status = CHIP_FUNC_CALL(host, CpgcMemTestMATS (host, socket, ddrChEnabled, testType, MT_CPGC_READ, seqDataInv, inverseQW, uiShl, numCl, 1, MT_ADDR_DIR_UP));
        ReadTsc64 (&endTsc);
        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                      "Step9 latency = %d ms\n", TimeDiff (host, startTsc, endTsc, TDIFF_UNIT_MS)));
      }
      if (status == SUCCESS) {
        OutputCheckpoint(host, STS_MEMBIST, SUB_MT_STEP10, (UINT16)i);
      } else {
        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                      "Test Failed!\n"));
        return status;
      }
    }
  }
  return status;
}  //MemTestMCH

/**

  Runs Advanced Memory Tests based on the GNDB algorithm

  @param host                - Pointer to sysHost
  @param socket              - Socket Id
  @param ddrChEnabled        - List of channels to be tested
  @param testType            - Memory test type
  @param testSize            - Memory test size

  @retval status             - SUCCESS/FAILURE

**/
UINT32
MemTestGNDB (
        PSYSHOST host,
        UINT8    socket,
        UINT8    ddrChEnabled,
        UINT8    testType,
        UINT8    testSize
        )
{
  UINT64_STRUCT    patternQW[MT_PATTERN_DEPTH];
  UINT64_STRUCT    shiftedQW[MT_PATTERN_DEPTH];
  UINT64_STRUCT    basePatternQW[MT_PATTERN_DEPTH];
  UINT8  uiShl[MT_PATTERN_DEPTH];
  UINT8  seqDataInv[MT_MAX_SUBSEQ];
  UINT8  numCl;
  UINT32  i;
  UINT8 pass;
  UINT32 status = SUCCESS;

  MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "\nMemTestGNDB%d\n", testSize));
  /*
  pattern set 1   00000000 00000000               ;Full bus-width all low
                  00000000 00000001               ;Bit 0 high
                  00000000 00000000               ;Full bus-width all low
                  00000000 00000002               ;Bit 1 high
                  ...
                  00000000 80000000               ;Bit 31 high
                  00000000 00000000
                  00000001 00000000               ;Bit 32 high
                  00000000 00000000
                  00000002 00000000               ;Bit 33 high
                  ...

  pattern set 2   FFFFFFFF FFFFFFFF               ;Full bus-width all high
                  00000000 00000001               ;Bit 0 high
                  FFFFFFFF FFFFFFFF               ;Full bus-width all high
                  00000000 00000002               ;Bit 1 high
                  ...
                  00000000 80000000               ;Bit 31 high
                  FFFFFFFF FFFFFFFF
                  00000001 00000000               ;Bit 32 high
                  FFFFFFFF FFFFFFFF
                  00000002 00000000               ;Bit 33 high
                  ...

  pattern set 3   00000000 00000000               ;Full bus-width all low
                  FFFFFFFF FFFFFFFE               ;Bit 0 low
                  00000000 00000000               ;Full bus-width all low
                  FFFFFFFF FFFFFFFD               ;Bit 1 low
                  ...
                  FFFFFFFF 7FFFFFFF               ;Bit 31 low
                  00000000 00000000
                  FFFFFFFE FFFFFFFF               ;Bit 32 low
                  00000000 00000000
                  FFFFFFFD FFFFFFFF               ;Bit 33 low
                  ...

  pattern set 4   FFFFFFFF FFFFFFFF               ;Full bus-width all high
                  FFFFFFFF FFFFFFFE               ;Bit 0 low
                  FFFFFFFF FFFFFFFF               ;Full bus-width all high
                  FFFFFFFF FFFFFFFD               ;Bit 1 low
                  ...
                  FFFFFFFF 7FFFFFFF               ;Bit 31 low
                  FFFFFFFF FFFFFFFF
                  FFFFFFFE FFFFFFFF               ;Bit 32 low
                  FFFFFFFF FFFFFFFF
                  FFFFFFFD FFFFFFFF               ;Bit 33 low
                  ...

  Algorithm :

  Run the following loops with 4 sets of patters
    Pass 1 - 64-bit background pattern to 0000000000000000h
       64-bit foreground pattern to 0000000000000001h

    Pass 2 - 64-bit background pattern to FFFFFFFFFFFFFFFFh
       64-bit foreground pattern to 0000000000000001h

    Pass 3 - 64-bit background pattern to 0000000000000000h
       64-bit foreground pattern to FFFFFFFFFFFFFFFEh

    Pass 4 - 64-bit background pattern to FFFFFFFFFFFFFFFFh
       64-bit foreground pattern to FFFFFFFFFFFFFFFEh

  (1)
    FOR each 128-bit memory location
      write background pattern to first 64 bits
      write foreground pattern to second 64 bits
      rotate foreground pattern left one bit position
    END FOR

    re-initialize 64-bit foreground pattern to starting pattern
  (2)
    FOR each 128-bit memory location
      read first 64 bits and compare to background pattern
      IF compare fails
        EXIT with failure message
      read second 64 bits and compare to foreground pattern
      IF compare fails
        EXIT with failure message
      rotate foreground pattern left one bit position
    END FOR
  */

  switch (testSize) {
  case 8:
    basePatternQW[0].hi = 0x01010101;
    basePatternQW[0].lo = 0x01010101;
    numCl = 2;
    break;
  case 16:
    basePatternQW[0].hi = 0x00010001;
    basePatternQW[0].lo = 0x00010001;
    numCl = 4;
    break;
  case 32:
    basePatternQW[0].hi = 0x00000001;
    basePatternQW[0].lo = 0x00000001;
    numCl = 8;
    break;
  case 64:
    basePatternQW[0].hi = 0x00000000;
    basePatternQW[0].lo = 0x00000001;
    numCl = 16;
    break;
  default:
    numCl = 0;
    basePatternQW[0].hi = 0x00000000;
    basePatternQW[0].lo = 0x00000000;
    RC_ASSERT(FALSE, ERR_RC_INTERNAL, RC_ASSERT_MINOR_CODE_68);
  }

  for (i = 0; i < testSize; i = i + (numCl * 8)) {

    shiftedQW[0] = LShiftUINT64 (basePatternQW[0], i);

    for (pass = 0; pass < 4; pass++) {

      switch (pass) {
      case 0:
        patternQW[0].hi = 0;
        patternQW[0].lo = 0;
        patternQW[1].hi = shiftedQW[0].hi;
        patternQW[1].lo = shiftedQW[0].lo;
        break;
      case 1:
        patternQW[0].hi = 0xffffffff;
        patternQW[0].lo = 0xffffffff;
        patternQW[1].hi = shiftedQW[0].hi;
        patternQW[1].lo = shiftedQW[0].lo;
        break;
      case 2:
        patternQW[0].hi = 0;
        patternQW[0].lo = 0;
        patternQW[1].hi = ~shiftedQW[0].hi;
        patternQW[1].lo = ~shiftedQW[0].lo;
        break;
      case 3:
        patternQW[0].hi = 0xffffffff;
        patternQW[0].lo = 0xffffffff;
        patternQW[1].hi = ~shiftedQW[0].hi;
        patternQW[1].lo = ~shiftedQW[0].lo;
        break;

      }
      uiShl[0] = 0;
      uiShl[1] = 1;
      seqDataInv[0] = 0;
      seqDataInv[1] = 0;

      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                    "patternQW[0] = 0x%08x%08x, patternQW[1] = 0x%08x%08x\n",
                     patternQW[0].hi, patternQW[0].lo, patternQW[1].hi, patternQW[1].lo));

      {
        UINT64_STRUCT             startTsc  = { 0, 0 };
        UINT64_STRUCT             endTsc = { 0, 0 };

        if (status == SUCCESS) {
          //1. Write sliding data pattern to all of memory
          OutputCheckpoint(host, STS_MEMBIST, SUB_MT_STEP1, (UINT16)(i + pass));
          ReadTsc64 (&startTsc);
          status = CHIP_FUNC_CALL(host, CpgcMemTestMATS (host, socket, ddrChEnabled, testType, MT_CPGC_WRITE, seqDataInv, patternQW, uiShl, numCl, 2, MT_ADDR_DIR_UP));
          ReadTsc64 (&endTsc);
          MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                        "Step1 latency = %d ms\n", TimeDiff (host, startTsc, endTsc, TDIFF_UNIT_MS)));
        }
        if (status == SUCCESS) {
          //2. read and compare data pattern
          OutputCheckpoint(host, STS_MEMBIST, SUB_MT_STEP2, (UINT16) (i + pass));
          ReadTsc64 (&startTsc);
          status = CHIP_FUNC_CALL(host, CpgcMemTestMATS (host, socket, ddrChEnabled, testType, MT_CPGC_READ, seqDataInv, patternQW, uiShl, numCl, 2, MT_ADDR_DIR_UP));
          ReadTsc64 (&endTsc);
          MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                        "Step2 latency = %d ms\n", TimeDiff (host, startTsc, endTsc, TDIFF_UNIT_MS)));
        }
        if (status == SUCCESS) {
          OutputCheckpoint(host, STS_MEMBIST, SUB_MT_STEP3, (UINT16) (i + pass));
        } else {
          MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                        "Test Failed!\n"));
          return status;
        }
      }
    }
  }
  return status;
} // MemTestGNDB


/**

  Runs Advanced Memory Tests based on the MarchC- algorithm

  @param host                - Pointer to sysHost
  @param socket              - Socket Id
  @param ddrChEnabled        - List of channels to be tested
  @param testType            - Memory test type
  @param testSize            - Memory test size

  @retval status             - SUCCESS/FAILURE

**/
UINT32
MemTestMARCHCM (
        PSYSHOST host,
        UINT8    socket,
        UINT8    ddrChEnabled,
        UINT8    testType,
        UINT8    testSize
        )
{
  UINT64_STRUCT    patternQW[MT_PATTERN_DEPTH];
  UINT64_STRUCT    inverseQW[MT_PATTERN_DEPTH];
  UINT8  uiShl[MT_PATTERN_DEPTH];
  UINT8  seqDataInv[MT_MAX_SUBSEQ];
  UINT8  numCl;
  UINT32  i, n;
  UINT32 status = SUCCESS;

  MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "\nMemTestMARCHM%d\n", testSize));
  /*
  Run the following loops for each pattern
    Pass 1 - AAAAAAAAAAAAAAAAh
    Pass 2 - CCCCCCCCCCCCCCCCh
    Pass 3 - F0F0F0F0F0F0F0F0h
    Pass 4 - FF00FF00FF00FF00h
    Pass 5 - FFFF0000FFFF0000h
    Pass 6 - FFFFFFFF00000000h
    Pass 7 - 0000000000000000h

  (1)
    FOR each ascending 64-bit memory location
      write pattern
    END FOR
  (2)
    FOR each ascending 64-bit memory location
      read and compare to pattern
      IF compare fails
        EXIT with failure message
      write inverse pattern
    END FOR
  (3)
    FOR each ascending 64-bit memory location
      read and compare to inverse pattern
      IF compare fails
        EXIT with failure message
      write pattern
    END FOR
  (4)
    FOR each descending 64-bit memory location
      read and compare to pattern
      IF compare fails
        EXIT with failure message
      write inverse pattern
    END FOR
  (5)
    FOR each descending 64-bit memory location
      read and compare to inverse pattern
      IF compare fails
        EXIT with failure message
      write pattern
    END FOR
  (6)
    FOR each ascending 64-bit memory location
      read and compare to pattern
      IF compare fails
        EXIT with failure message
    END FOR
  */

  switch (testSize) {
  case 8:
    n = 3;
    break;
  case 16:
    n = 4;
    break;
  case 32:
    n = 5;
    break;
  case 64:
    n = 6;
    break;
  default:
    n = 0;
    RC_ASSERT(FALSE, ERR_RC_INTERNAL, RC_ASSERT_MINOR_CODE_69);
  }

  for (i = 0; i <= n; i++) {

    switch (i) {
    case 0:
      patternQW[0].hi = 0xaaaaaaaa;
      patternQW[0].lo = 0xaaaaaaaa;
      break;
    case 1:
      patternQW[0].hi = 0xcccccccc;
      patternQW[0].lo = 0xcccccccc;
      break;
    case 2:
      patternQW[0].hi = 0xf0f0f0f0;
      patternQW[0].lo = 0xf0f0f0f0;
      break;
    case 3:
      patternQW[0].hi = 0xff00ff00;
      patternQW[0].lo = 0xff00ff00;
      break;
    case 4:
      patternQW[0].hi = 0xffff0000;
      patternQW[0].lo = 0xffff0000;
      break;
    case 5:
      patternQW[0].hi = 0xffffffff;
      patternQW[0].lo = 0x00000000;
      break;
    case 6:
      patternQW[0].hi = 0x0;
      patternQW[0].lo = 0x0;
      break;
    }

    inverseQW[0].hi = ~patternQW[0].hi;
    inverseQW[0].lo = ~patternQW[0].lo;

    numCl = 1;
    uiShl[0] = 0;
    uiShl[1] = 0;
    seqDataInv[0] = 0;
    seqDataInv[1] = 1;

    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                  "patternQW[0] = 0x%08x%08x, inverseQW[0] = 0x%08x%08x\n",
                   patternQW[0].hi, patternQW[0].lo, inverseQW[0].hi, inverseQW[0].lo));

    {
      UINT64_STRUCT             startTsc  = { 0, 0 };
      UINT64_STRUCT             endTsc = { 0, 0 };

      if (status == SUCCESS) {
      //1. Write data pattern to all of memory
      OutputCheckpoint(host, STS_MEMBIST, SUB_MT_STEP1, (UINT16)i);
      ReadTsc64 (&startTsc);
      status = CHIP_FUNC_CALL(host, CpgcMemTestMATS (host, socket, ddrChEnabled, testType, MT_CPGC_WRITE, seqDataInv, patternQW, uiShl, numCl, 1, MT_ADDR_DIR_UP));
      ReadTsc64 (&endTsc);
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                    "Step1 latency = %d ms\n", TimeDiff (host, startTsc, endTsc, TDIFF_UNIT_MS)));
      }
      if (status == SUCCESS) {
      //2. read and compare data pattern
      //   write inverse pattern
      OutputCheckpoint(host, STS_MEMBIST, SUB_MT_STEP2, (UINT16)i);
      ReadTsc64 (&startTsc);
      status = CHIP_FUNC_CALL(host, CpgcMemTestMATS (host, socket, ddrChEnabled, testType, MT_CPGC_READ_WRITE, seqDataInv, patternQW, uiShl, numCl, 1, MT_ADDR_DIR_UP));
      ReadTsc64 (&endTsc);
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                    "Step2 latency = %d ms\n", TimeDiff (host, startTsc, endTsc, TDIFF_UNIT_MS)));
      }
      if (status == SUCCESS) {
      //3. read and compare inverse pattern
      //   write original pattern
      OutputCheckpoint(host, STS_MEMBIST, SUB_MT_STEP3, (UINT16)i);
      ReadTsc64 (&startTsc);
      status = CHIP_FUNC_CALL(host, CpgcMemTestMATS (host, socket, ddrChEnabled, testType, MT_CPGC_READ_WRITE, seqDataInv, inverseQW, uiShl, numCl, 1, MT_ADDR_DIR_UP));
      ReadTsc64 (&endTsc);
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                    "Step3 latency = %d ms\n", TimeDiff (host, startTsc, endTsc, TDIFF_UNIT_MS)));
      }
      if (status == SUCCESS) {
      //4. read and compare original pattern
      //   write inverse pattern
      OutputCheckpoint(host, STS_MEMBIST, SUB_MT_STEP4, (UINT16)i);
      ReadTsc64 (&startTsc);
      status = CHIP_FUNC_CALL(host, CpgcMemTestMATS (host, socket, ddrChEnabled, testType, MT_CPGC_READ_WRITE, seqDataInv, patternQW, uiShl, numCl, 1, MT_ADDR_DIR_DN));
      ReadTsc64 (&endTsc);
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                    "Step4 latency = %d ms\n", TimeDiff (host, startTsc, endTsc, TDIFF_UNIT_MS)));
      }
      if (status == SUCCESS) {
      //5. read and compare inverse pattern
      //   write original pattern
      OutputCheckpoint(host, STS_MEMBIST, SUB_MT_STEP5, (UINT16)i);
      ReadTsc64 (&startTsc);
      status = CHIP_FUNC_CALL(host, CpgcMemTestMATS (host, socket, ddrChEnabled, testType, MT_CPGC_READ_WRITE, seqDataInv, inverseQW, uiShl, numCl, 1, MT_ADDR_DIR_DN));
      ReadTsc64 (&endTsc);
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                    "Step5 latency = %d ms\n", TimeDiff (host, startTsc, endTsc, TDIFF_UNIT_MS)));
      }
      if (status == SUCCESS) {
      //6. read and compare original pattern
      OutputCheckpoint(host, STS_MEMBIST, SUB_MT_STEP6, (UINT16)i);
      ReadTsc64 (&startTsc);
      status = CHIP_FUNC_CALL(host, CpgcMemTestMATS (host, socket, ddrChEnabled, testType, MT_CPGC_READ, seqDataInv, patternQW, uiShl, numCl, 1, MT_ADDR_DIR_UP));
      ReadTsc64 (&endTsc);
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                    "Step6 latency = %d ms\n", TimeDiff (host, startTsc, endTsc, TDIFF_UNIT_MS)));
      }
      if (status == SUCCESS) {
      OutputCheckpoint(host, STS_MEMBIST, SUB_MT_STEP7, (UINT16)i);
      } else {
        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                      "Test Failed!\n"));
        return status;
      }
    }
  }
  return status;
} // MemTestMARCHM


/**

  Runs Advanced Memory Test algorithms with CPGC and logs any failures

  @param host                - Pointer to sysHost

  @retval - SUCCESS

**/
UINT32
AdvancedMemTestWorker (
        PSYSHOST host,
        UINT32   advMemTestOptions
        )
{
  UINT8               socket;
  UINT8               ch;
  UINT8               ddrChEnabled;
  UINT32              maxRepeatTest;
  UINT32              repeatTest;
  UINT32              scrambleConfigOrg[MAX_CH];
  struct channelNvram (*channelNvList)[MAX_CH];

  if (~host->memFlows & MF_MEMTEST_EN) {
    return SUCCESS;
  }

  MemSetLocal ((UINT8 *)scrambleConfigOrg, 0, sizeof (scrambleConfigOrg));

  host->var.common.rcWriteRegDump = 0;
  if (host->var.common.bootMode == NormalBoot) {
    if (((host->var.mem.subBootMode == WarmBootFast ||
      host->var.mem.subBootMode == ColdBootFast) &&
      host->setup.mem.options & MEMORY_TEST_FAST_BOOT_EN) ||
      (!(host->var.mem.subBootMode == WarmBootFast ||
      host->var.mem.subBootMode == ColdBootFast) &&
      host->setup.mem.options & MEMORY_TEST_EN))
    {

      socket = host->var.mem.currentSocket;

      //
      // Return if this socket is disabled
      //
      if ((host->nvram.mem.socket[socket].enabled == 0) || (host->nvram.mem.socket[socket].maxDimmPop == 0)) return SUCCESS;

      //
      // if we are running memory test, we need to not skip the memory init
      //
      host->var.mem.skipMemoryInit = 0;
      // Create the Channel Mask
      ddrChEnabled = 0;
      channelNvList = GetChannelNvList(host, socket);

      if (host->setup.mem.memTestLoops) {
        maxRepeatTest = (UINT32)host->setup.mem.memTestLoops;
      } else {
        maxRepeatTest = 0xFFFFFFFF;   //used for infinite loop
      }
      for (repeatTest = 0; repeatTest < maxRepeatTest; repeatTest++)
      {
        // Chip hook to disable scrambling and other stuff that interferes with Adv memtest
        for (ch = 0; ch < MAX_CH; ch++) {
          if ((*channelNvList)[ch].enabled == 0) continue;
          ddrChEnabled |= 1 << ch;

          scrambleConfigOrg[ch] = CHIP_FUNC_CALL(host, PreAdvancedMemTest (host, socket, ch));
        }

        // XMATS
        if (advMemTestOptions & ADV_MT_XMATS8) {
          MemTestMATSN (host, socket, ddrChEnabled, ADV_MT_TYPE_XMATS8, 8, 1);
        }
        if (advMemTestOptions & ADV_MT_XMATS16) {
          MemTestMATSN (host, socket, ddrChEnabled, ADV_MT_TYPE_XMATS16, 16, 1);
        }
        if (advMemTestOptions & ADV_MT_XMATS32) {
          MemTestMATSN (host, socket, ddrChEnabled, ADV_MT_TYPE_XMATS32, 32, 1);
        }
        if (advMemTestOptions & ADV_MT_XMATS64) {
          MemTestMATSN (host, socket, ddrChEnabled, ADV_MT_TYPE_XMATS64, 64, 1);
        }

        // Worst-case MATS (no sliding pattern)
        if (advMemTestOptions & ADV_MT_WCMATS8) {
          MemTestMATSN (host, socket, ddrChEnabled, ADV_MT_TYPE_WCMATS8, 8, 0);
        }

        // Worst-case March (repeat 2 times)
        if (advMemTestOptions & ADV_MT_WCMCH8) {
          MemTestMCH (host, socket, ddrChEnabled, ADV_MT_TYPE_WCMCH8, 8, 2);
        }

        // Ground Bounce
        if (advMemTestOptions & ADV_MT_GNDB64) {
          MemTestGNDB (host, socket, ddrChEnabled, ADV_MT_TYPE_GNDB64, 64);
        }

        // March C-
        if (advMemTestOptions & ADV_MT_MARCHCM64) {
          MemTestMARCHCM (host, socket, ddrChEnabled, ADV_MT_TYPE_MARCHCM64, 64);
        }

        // Chip hook to re-enable scrambling
        for (ch = 0; ch < MAX_CH; ch++) {
          if ((*channelNvList)[ch].enabled == 0) continue;
          CHIP_FUNC_CALL(host, PostAdvancedMemTestRestoreScrambler (host, socket, ch, scrambleConfigOrg[ch]));
        }

        // Like legacy Memtest, but faster with tCCD_S
        if (advMemTestOptions & ADV_MT_LTEST_SCRAM) {
          MemTestScram (host, socket, ddrChEnabled, ADV_MT_TYPE_LTEST_SCRAM, 64, 0, repeatTest);
        }

        // Chip hook to re-enable scrambling and other stuff that was modified for Adv memtest
        for (ch = 0; ch < MAX_CH; ch++) {
          if ((*channelNvList)[ch].enabled == 0) continue;
          CHIP_FUNC_CALL(host, PostAdvancedMemTest (host, socket, ch, scrambleConfigOrg[ch]));
        }
        //
        //incorporates infinite loop accordingly
        //
        if (!host->setup.mem.memTestLoops && (repeatTest == 0xFFFFFFFE)) repeatTest = 0;
      }

    } // if MemTest enabled
  } // if NormalBoot
  host->var.common.rcWriteRegDump = 1;
  return SUCCESS;
} // AdvancedMemTestWorker

/**

  Runs Advanced Memory Test algorithms with CPGC and logs any failures

  @param host                - Pointer to sysHost

  @retval - SUCCESS

**/
UINT32
AdvancedMemTest (
        PSYSHOST host
        )
{
  if (host->setup.mem.advMemTestEn == 0) {
    return SUCCESS;
  }
  return AdvancedMemTestWorker(host, host->setup.mem.advMemTestOptions);
} // AdvancedMemTest

/**

  Runs optimized MemTest with tCCD_S

  @param host  - Pointer to sysHost

  @retval SUCCESS

**/
UINT32
MemTest (
        PSYSHOST host
        )
{
  return AdvancedMemTestWorker(host, ADV_MT_LTEST_SCRAM);
} // MemTest


/**

  Checks the results of the preceding memory test

  @param host                - Pointer to sysHost
  @param socket              - Socket Id
  @param chBitmask           - List of channels that were tested
  @param dimmRank[MAX_RANK_CH][MAX_CH]         - DIMM containing logicalRank
  @param logical2Physical[MAX_RANK_CH][MAX_CH] - Geometery for logical rank within channel
  @param logicalRank         - Logical rank
  @param testType            - Memory test type for logging purposes

  @retval status             - SUCCESS/FAILURE

**/
UINT32
CheckTestResultsMATS (
  PSYSHOST                  host,
  UINT8                     socket,
  UINT32                    chBitmask,
  UINT8                     dimmRank[MAX_RANK_CH][MAX_CH],
  UINT8                     logical2Physical[MAX_RANK_CH][MAX_CH],
  UINT8                     logicalRank,
  UINT8                     testType
  )
{
  UINT32              errorStatus0;
  UINT32              errorStatus1;
  UINT32              errorStatus2;
  UINT8               dimm;
  UINT8               ch;
  UINT8               rank;
  CPGC_ERROR_STATUS_MATS   cpgcErrorStatus[MAX_CH];
  UINT32              status = SUCCESS;

  // Collect test results
  CHIP_FUNC_CALL(host, CpgcAdvTrainingErrorStatusMATS (host, socket, chBitmask, cpgcErrorStatus));

  for (ch = 0; ch < MAX_CH; ch++){
    dimm = dimmRank[logicalRank][ch];
    rank = logical2Physical[logicalRank][ch];

    //
    // Was this channel part of the test group?
    //
    if (chBitmask & (1 << ch)) {
      //
      // Read error status
      //
      errorStatus0 = cpgcErrorStatus[ch].cpgcErrDat0S | cpgcErrorStatus[ch].cpgcErrDat2S;;
      errorStatus1 = cpgcErrorStatus[ch].cpgcErrDat1S | cpgcErrorStatus[ch].cpgcErrDat3S;
      if (host->nvram.mem.eccEn) {
        errorStatus2 = (cpgcErrorStatus[ch].cpgcErrEccS & 0xFF) | ((cpgcErrorStatus[ch].cpgcErrEccS >> 8) & 0xFF);
      } else {
        errorStatus2 = 0;
      }
      if (!(host->var.common.emulation & SIMICS_FLAG)) {
        // Check for any bit errors on any lane.
        if ((errorStatus0 | errorStatus1 | errorStatus2) != 0) {
          status = FAILURE;

#ifdef SERIAL_DBG_MSG
          if (checkMsgLevel(host, SDBG_MINMAX)) {
            getPrintFControl (host);
            setColor (host, ANSI_FOREGROUND_RED, ANSI_BACKGROUND_BLACK, ANSI_ATTRIBUTE_OFF);
            MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
                          "Failure detected - Advance Memtest Type %d, Checkpoint 0x%08x\n",
                          testType, host->var.mem.lastCheckpoint[socket]));
            MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                          "Logical Rank = %d, Subrank = %d, Bank = 0x%x, Row = 0x%x, Column = 0x%x, Chunk = 0x%x\n",
                          cpgcErrorStatus[ch].cpgcErrRank, cpgcErrorStatus[ch].cpgcSubrank,
                          cpgcErrorStatus[ch].cpgcErrBank, cpgcErrorStatus[ch].cpgcErrRow,
                          cpgcErrorStatus[ch].cpgcErrColumn, cpgcErrorStatus[ch].cpgcErrChunk));
            MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                          "DQ[71:0] Error on rising strobe:  0x%02x%02x%02x%02x%02x%02x%02x%02x%02x\n",
                          (cpgcErrorStatus[ch].cpgcErrEccS >> 0) & 0xFF,
                          (cpgcErrorStatus[ch].cpgcErrDat1S >> 24) & 0xFF,
                          (cpgcErrorStatus[ch].cpgcErrDat1S >> 16) & 0xFF,
                          (cpgcErrorStatus[ch].cpgcErrDat1S >> 8) & 0xFF,
                          (cpgcErrorStatus[ch].cpgcErrDat1S >> 0) & 0xFF,
                          (cpgcErrorStatus[ch].cpgcErrDat0S >> 24) & 0xFF,
                          (cpgcErrorStatus[ch].cpgcErrDat0S >> 16) & 0xFF,
                          (cpgcErrorStatus[ch].cpgcErrDat0S >> 8) & 0xFF,
                          (cpgcErrorStatus[ch].cpgcErrDat0S >> 0) & 0xFF
                          ));
            MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                          "DQ[71:0] Error on falling strobe: 0x%02x%02x%02x%02x%02x%02x%02x%02x%02x\n",
                          (cpgcErrorStatus[ch].cpgcErrEccS >> 8) & 0xFF,
                          (cpgcErrorStatus[ch].cpgcErrDat3S >> 24) & 0xFF,
                          (cpgcErrorStatus[ch].cpgcErrDat3S >> 16) & 0xFF,
                          (cpgcErrorStatus[ch].cpgcErrDat3S >> 8) & 0xFF,
                          (cpgcErrorStatus[ch].cpgcErrDat3S >> 0) & 0xFF,
                          (cpgcErrorStatus[ch].cpgcErrDat2S >> 24) & 0xFF,
                          (cpgcErrorStatus[ch].cpgcErrDat2S >> 16) & 0xFF,
                          (cpgcErrorStatus[ch].cpgcErrDat2S >> 8) & 0xFF,
                          (cpgcErrorStatus[ch].cpgcErrDat2S >> 0) & 0xFF
                          ));

            setWhite (host);
            releasePrintFControl (host);
          } // checkMsgLevel
#endif
          // Log that the error occurred
          LogAdvMemtestError(host, socket, ch, dimm, rank, testType, cpgcErrorStatus[ch]);

        } // if errorStatus
      } // if emulation
    } // if (chBitmask & (1 << ch))
  } // ch loop

  return status;
} // CheckTestResultsMATS


/**

  Logs AdvancedMemtestError into the host structure

  @param host      - Pointer to the system host (root) structure
  @param host      - Pointer to SysHost
  @param socket    - Current socket
  @param ch        - Current channel
  @param dimm      - Current dimm
  @param rank      - Current rank
  @param errorInfo - Advanced Memtest Error status

  @retval N/A

**/
void
LogAdvMemtestError (
           PSYSHOST host,
           UINT8    socket,
           UINT8    channel,
           UINT8    dimm,
           UINT8    rank,
           UINT8    testType,
           CPGC_ERROR_STATUS_MATS errorInfo
           )
{
  UINT32  i;
#if ENHANCED_WARNING_LOG
  EWL_ENTRY_HEADER *warningHeader;
  EWL_ENTRY_TYPE4  *warning;
  EWL_ADV_MT_STATUS ewlErrorInfo;
  //
  // Search for existing entry in Enhanced Warning Log
  //
  i = 0;
  while (i < host->var.common.ewlPrivateData.status.Header.FreeOffset) {
    //Get next warning from EWL structure
    warningHeader = (EWL_ENTRY_HEADER*) &(host->var.common.ewlPrivateData.status.Buffer[i]);
    //If Warning is Advanced Memtest Type (type4)
    if (warningHeader->Type == EwlType4) {
      //Find matching entry in the log
      warning = (EWL_ENTRY_TYPE4 *)warningHeader;
      if (MemCompare ((UINT8 *) &(warning->AdvMemtestErrorInfo),
                      (UINT8 *) &errorInfo, sizeof(CPGC_ERROR_STATUS_MATS)) == 0) {
        //
        // Increment count in the already logged warning
        //
        warning->Count++;
      }
    }
    i += warningHeader->Size;
  }

  //if there is not a warning logged, create EWL entry
  if (i == host->var.common.ewlPrivateData.status.Header.FreeOffset) {
    ewlErrorInfo.Dat0S = errorInfo.cpgcErrDat0S;
    ewlErrorInfo.Dat1S = errorInfo.cpgcErrDat1S;
    ewlErrorInfo.Dat2S = errorInfo.cpgcErrDat2S;
    ewlErrorInfo.Dat3S = errorInfo.cpgcErrDat3S;
    ewlErrorInfo.EccS = errorInfo.cpgcErrEccS;
    ewlErrorInfo.Chunk = errorInfo.cpgcErrChunk;
    ewlErrorInfo.Column = errorInfo.cpgcErrColumn;
    ewlErrorInfo.Row = errorInfo.cpgcErrRow;
    ewlErrorInfo.Bank = errorInfo.cpgcErrBank;
    ewlErrorInfo.Rank = errorInfo.cpgcErrRank;
    ewlErrorInfo.Subrank = errorInfo.cpgcSubrank;

    EwlOutputType4 (host, ERR_MEM_TEST, ERR_MEM_TEST_MINOR_HARDWARE, socket, channel, dimm, rank, EwlSeverityWarning,
                    testType, ewlErrorInfo, 1);
  }

#else
  //
  // Search for existing entry
  //
  for (i = 0; i < host->var.mem.advMemtestErrors.index && i < MAX_ADV_MT_LOG; i++) {

    if (MemCompare((UINT8 *)&host->var.mem.advMemtestErrors.log[i].advMemtestErrorInfo,
                   (UINT8 *)&errorInfo, sizeof(CPGC_ERROR_STATUS_MATS)) == 0) {

      //
      // Increment count
      //
      host->var.mem.advMemtestErrors.log[i].count++;
    }
  }

  //
  // Append warningCode to the status log
  //
  if (i < MAX_ADV_MT_LOG) {
    host->var.mem.advMemtestErrors.log[i].code = (UINT32) ((ERR_MEM_TEST << 24) | (ERR_MEM_TEST_MINOR_HARDWARE << 16) | (testType << 8));
    host->var.mem.advMemtestErrors.log[i].data = (socket << 24) | (channel << 16) | (dimm << 8) | rank;
    host->var.mem.advMemtestErrors.log[i].checkpoint = host->var.mem.lastCheckpoint[socket];
    host->var.mem.advMemtestErrors.log[i].advMemtestErrorInfo = errorInfo;
    host->var.mem.advMemtestErrors.log[i].count = 1;
    host->var.mem.advMemtestErrors.index++;
  }
#endif
}
#else  //ADV_MEMTEST_SUPPORT

UINT32
MemTest (
        PSYSHOST host
        )
/*++

  Runs MemTest

  @param host  - Pointer to sysHost

  @retval SUCCESS

--*/
{
  UINT8               socket;
  UINT8               ch;
  UINT32              ddrChEnabled;
  UINT16              maxRepeatTest;
  UINT16              repeatTest;
  struct channelNvram (*channelNvList)[MAX_CH];

  if (~host->memFlows & MF_MEMTEST_EN) {
    return SUCCESS;
  }

  host->var.common.rcWriteRegDump = 0;
  if (host->var.common.bootMode == NormalBoot) {
    if (((host->var.mem.subBootMode == WarmBootFast ||
          host->var.mem.subBootMode == ColdBootFast) &&
        host->setup.mem.options & MEMORY_TEST_FAST_BOOT_EN) ||
        (!(host->var.mem.subBootMode == WarmBootFast ||
            host->var.mem.subBootMode == ColdBootFast) &&
            host->setup.mem.options & MEMORY_TEST_EN))
    {

      socket = host->var.mem.currentSocket;

      //
      // Return if this socket is disabled
      //
      if ((host->nvram.mem.socket[socket].enabled == 0) || (host->nvram.mem.socket[socket].maxDimmPop == 0)) return SUCCESS;

      //
      // if we are running memory test, we need to not skip the memory init
      //
      host->var.mem.skipMemoryInit = 0;
      // Create the Channel Mask
      ddrChEnabled = 0;
      channelNvList = GetChannelNvList(host, socket);

      for (ch = 0; ch < MAX_CH; ch++) {
        if ((*channelNvList)[ch].enabled == 0) continue;
        ddrChEnabled |= 1 << ch;
      }

      if (host->setup.mem.memTestLoops)
        maxRepeatTest = host->setup.mem.memTestLoops;
      else
        maxRepeatTest=3;   //used for infinite loop
      for (repeatTest = 0; repeatTest < maxRepeatTest; repeatTest++) {
        //
        // Step 1: Write all A5's pattern
        //
        CHIP_FUNC_CALL(host, CpgcMemTest (host, socket, ddrChEnabled, DDR_CPGC_MEM_TEST_WRITE | DDR_CPGC_MEM_TEST_A5));

        //
        // Step 2: Read all A5's pattern
        //
        CHIP_FUNC_CALL(host, CpgcMemTest (host, socket, ddrChEnabled, DDR_CPGC_MEM_TEST_READ | DDR_CPGC_MEM_TEST_A5));

        //
        //incorporates infinite loop accordingly
        //
        if (!host->setup.mem.memTestLoops) repeatTest=0;
      }

    } // if MemTest enabled
  } // if NormalBoot
  host->var.common.rcWriteRegDump = 1;
  return SUCCESS;
} // MemTest

#endif  //ADV_MEMTEST_SUPPORT

UINT32
MemInit (
        PSYSHOST host
        )
/*++

  Runs MemInit

  @param host  - Pointer to sysHost

  @retval SUCCESS

--*/
{
  UINT8               socket;

  if (~host->memFlows & MF_MEMINIT_EN) {
    return SUCCESS;
  }

  //
  // Skip MemInit if the skip meminit flag is set and memory does not need to be wiped
  //
  if ((host->var.mem.skipMemoryInit) && (host->var.mem.wipeMemory == 0)){
    return SUCCESS;
  }

  //
  // Execute MemInit only when ECC is enabled or memory needs to be wiped
  //
  if ((host->nvram.mem.eccEn == 0) && (host->var.mem.wipeMemory == 0)) {
    return SUCCESS;
  }

  //
  // Skip if not a normal boot
  //
  if ((host->var.common.bootMode != NormalBoot)) {
    return SUCCESS;
  }

  //
  // Return if this socket is disabled
  //
  socket = host->var.mem.currentSocket;
  if ((host->nvram.mem.socket[socket].enabled == 0) || (host->nvram.mem.socket[socket].maxDimmPop == 0)) {
    return SUCCESS;
  }

  host->var.common.rcWriteRegDump = 0;

  CHIP_FUNC_CALL(host, MemoryInitDdr (host, socket));

  host->var.common.rcWriteRegDump = 1;

  return SUCCESS;
} // MemInit

BOOLEAN
CheckMemPresentStatus (
                      PSYSHOST host
                      )
/*++

  Checks to make sure there is still memory available

  @param host  - Pointer to sysHost

  @retval N/A

--*/
{
  UINT8               socket;
  UINT8               ch;
  UINT8               dimm;
  UINT8               rank;
  UINT8               memPresent;
#if SMCPKG_SUPPORT && MemoryMapOut_SUPPORT
 UINT8 Data;
  EFI_PEI_SERVICES            **PeiServices;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI       *PeiVariable;
  EFI_STATUS                  Status;
  UINTN Size = sizeof(UINT32);
  UINT32 MapOutRTFlag;
#endif
  struct channelNvram (*channelNvList)[MAX_CH];
  struct dimmNvram    (*dimmNvList)[MAX_DIMM];

  memPresent = 0;

  for (socket = 0; socket < MAX_SOCKET; socket++) {
    if ((host->nvram.mem.socket[socket].enabled == 0) || (host->nvram.mem.socket[socket].maxDimmPop == 0)) continue;

    channelNvList = GetChannelNvList(host, socket);

    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled == 0) continue;

      dimmNvList = GetDimmNvList(host, socket, ch);

      for (dimm = 0; dimm < MAX_DIMM; dimm++){
        if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

        for (rank = 0; rank < host->var.mem.socket[socket].maxRankDimm; rank++) {
          if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

          MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                  "Memory Found!\n"));
          memPresent = 1 << socket;
          break;
        } // rank loop
      } // dimm loop
    } // ch loop

    //
    // Disable this socket if doesn't have any enabled memory
    //
    if ((memPresent & (1 << socket)) == 0) {
      host->nvram.mem.socket[socket].maxDimmPop = 0;
    }
  } // socket loop

#if SMCPKG_SUPPORT && MemoryMapOut_SUPPORT
  //Get cmos
  Data = GetCmosTokensValue (MEM_MAPOUT);
  PeiServices  = (EFI_PEI_SERVICES **) host->var.common.oemVariable;
  // Force to report WARN_USER_DIMM_DISABLE warning when runtime UECC happened
        //
        // Locate Variable PPI
        //
        (**PeiServices).LocatePpi (
                        PeiServices,
                        &gEfiPeiReadOnlyVariable2PpiGuid,
                        0,
                        NULL,
                        &PeiVariable
                        );
                        
        Status = PeiVariable->GetVariable (
                          PeiVariable,
                          L"MemMapout",
                          &gSmcMemMapOutGuid,
                          NULL,
                          &Size,
                          &MapOutRTFlag
                          );
// if ((Data) && (Data != 0xFF))                        
//  ASSERT_EFI_ERROR (Status);

//    if ( (Data & 0x80 ) && ( EFI_ERROR(Status) || MapOutRTFlag !=0xAB ) )
//  	{             
//        OutputWarning (host, WARN_USER_DIMM_DISABLE, 0, ((Data >> 4) & 0x03), ((Data >> 1) & 0x07), (Data & 0x01), 0xFF);
//    }
    if ( (Data & 0x80 )   )
    {
        if(( EFI_ERROR(Status) || MapOutRTFlag !=0xAB )){
            OutputWarning (host, WARN_USER_DIMM_DISABLE, 0, ((Data >> 4) & 0x03), ((Data >> 1) & 0x07), (Data & 0x01), 0xFF);
        }
        else{
            //Force to show WARN_USER_DIMM_DISABLE warning on screen when last runtime UECC happened
            Data = (((Data >> 4) & 0x03) << 5) | (((Data >> 1) & 0x07) << 2) | (Data & 0x01);// Bit7-5:CPU, 4-2: Channel 1-0: DIMM
            ReportErrorDIMMToScreen(host, WARN_USER_DIMM_DISABLE, 0, Data, 0);
        }
    }
#endif	//#if SMCPKG_SUPPORT && MemoryMapOut_SUPPORT
  //
  // Check for no memory error
  //
  if (memPresent == 0) {
    return FALSE;
  } else {
    return TRUE;
  }
} // CheckMemPresentStatus


/**

  Classifies the DQ errors as correctable or uncorrectable based on current mode of operation

  @param host                - Pointer to sysHost
  @param socket              - Socket Id
  @param ch                  - Channel number
  @param dimm                - DIMM number (0-based)
  @param rank                - Rank number (0-based)
  @param errorStatus0        - DQ[31:0] test result
  @param errorStatus1        - DQ[63:32] test result
  @param errorStatus2        - DQ[71:64] test result

  @retval SUCCESS

**/
UINT8
ClassifyDqErrorStatus (
  PSYSHOST                  host,
  UINT8                     socket,
  UINT8                     ch,
  UINT8                     dimm,
  UINT8                     rank,
  UINT32                    errorStatus0,
  UINT32                    errorStatus1,
  UINT32                    errorStatus2
  )
{
  UINT8               errorCount = 0;
  UINT8               errorType = 0;  // 0 = correctable.  1 = uncorrectable.
  UINT8               i;
  UINT8               strobe;
  struct dimmNvram    (*dimmNvList)[MAX_DIMM];
  struct ddrRank      (*rankList)[MAX_RANK_DIMM];

  dimmNvList    = GetDimmNvList(host, socket, ch);
  rankList      = GetRankNvList(host, socket, ch, dimm);


  // Check for any bit errors on any lane.
  if ((errorStatus0 | errorStatus1 | errorStatus2) != 0) {
    //
    // Now check to see if the errors found are correctable or uncorrectable.
    // For failures only fall within x4 aligned lanes, i.e. failed lanes within a single nibble,
    // e.g. lane 15:12 (regardless of running independent mode or lockstep), you can treat it is correctable error.
    // Otherwise, for failures within x8 device aligned lanes, i.e. fail lanes within a double-nibble
    // (e.g. lane 15:8 is but lane 11:4 is not) and running lock-step mode, you can treat the error as correctable error.
    // Otherwise, remaining failures are UC.
    //
    if (!host->nvram.mem.eccEn) {
      errorType = 1;

    } else if (CHIP_FUNC_CALL(host, ChipLockstepEn(host))) {
      //
      // We are dealing with a x4 or x8 DIMM in lockstep.
      // Loop through all 9 bytes to check for errors.
      //
      for (i = 0; i < 4; i++) {
        if (((errorStatus0 >> (i * 8)) & 0xFF) != 0) {
          strobe = 0;
          if ((*dimmNvList)[dimm].x4Present) {
            if (((errorStatus0 >> (i * 8)) & 0x0F) != 0) {
              errorCount++;
              strobe = i;
              (*rankList)[rank].faultyParts[strobe] |= FPT_MEM_BIST_FAILED;
            }
            if (((errorStatus0 >> (i * 8)) & 0xF0) != 0) {
              errorCount++;
              strobe = i + 9;
              (*rankList)[rank].faultyParts[strobe] |= FPT_MEM_BIST_FAILED;
            }
          } else {
            errorCount++;
            strobe = i;
            (*rankList)[rank].faultyParts[strobe] |= FPT_MEM_BIST_FAILED;
          }
          MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
            "FPT strobe = %d!\n", strobe));
        }
      }
      //
      // errorStatus1
      //
      for (i = 0; i < 4; i++) {
        if (((errorStatus1 >> (i * 8)) & 0xFF) != 0) {
          strobe = 4;
          if ((*dimmNvList)[dimm].x4Present) {
            if (((errorStatus1 >> (i * 8)) & 0x0F) != 0) {
              errorCount++;
              strobe = i + 4;
              (*rankList)[rank].faultyParts[strobe] |= FPT_MEM_BIST_FAILED;
            }
            if (((errorStatus1 >> (i * 8)) & 0xF0) != 0) {
              errorCount++;
              strobe = i + 4 + 9;
              (*rankList)[rank].faultyParts[strobe] |= FPT_MEM_BIST_FAILED;
            }
          } else {
            errorCount++;
            strobe = i + 4;
            (*rankList)[rank].faultyParts[strobe] |= FPT_MEM_BIST_FAILED;
          }
          MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
            "FPT strobe = %d!\n", strobe));
        }
      }
      //
      // errorStatus2
      //
      if ((errorStatus2 & 0xFF) != 0) {
        strobe = 8;
        if ((*dimmNvList)[dimm].x4Present) {
          if ((errorStatus2 & 0x0F) != 0) {
            errorCount++;
            strobe =  8;
            (*rankList)[rank].faultyParts[strobe] |= FPT_MEM_BIST_FAILED;
          }
          if ((errorStatus2 & 0xF0) != 0) {
            errorCount++;
            strobe = 17;
            (*rankList)[rank].faultyParts[strobe] |= FPT_MEM_BIST_FAILED;
          }
        } else {
          errorCount++;
          strobe = 8;
          (*rankList)[rank].faultyParts[strobe] |= FPT_MEM_BIST_FAILED;
        }
        MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
          "FPT strobe = %d!\n", strobe));
      }
      //
      // If we detect errors on more than 1 byte, it is uncorrectable.
      //
      if (errorCount > 1) {
        errorType = 1;
      }
    } else {
      //
      // We are dealing with either a x4 or x8 DIMM in independent channel mode.
      // Loop through all 18 nibbles and look for errors.
      //
      for (i = 0; i < 4; i++) {
        if (((errorStatus0 >> (i * 8)) & 0xFF) != 0) {
          strobe = 0;
          if ((*dimmNvList)[dimm].x4Present) {
            if (((errorStatus0 >> (i * 8)) & 0x0F) != 0) {
              errorCount++;
              strobe = i;
              (*rankList)[rank].faultyParts[strobe] |= FPT_MEM_BIST_FAILED;
            }
            if (((errorStatus0 >> (i * 8)) & 0xF0) != 0) {
              errorCount++;
              strobe = i + 9;
              (*rankList)[rank].faultyParts[strobe] |= FPT_MEM_BIST_FAILED;
            }
          } else {
            errorCount++;
            strobe = i;
            (*rankList)[rank].faultyParts[strobe] |= FPT_MEM_BIST_FAILED;
          }
          MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
            "FPT strobe = %d!\n", strobe));
        }
      }
      //
      // errorStatus1
      //
      for (i = 0; i < 4; i++) {
        if (((errorStatus1 >> (i * 8)) & 0xFF) != 0) {
          strobe = 4;
          if ((*dimmNvList)[dimm].x4Present) {
            if (((errorStatus1 >> (i * 8)) & 0x0F) != 0) {
              errorCount++;
              strobe = i + 4;
              (*rankList)[rank].faultyParts[strobe] |= FPT_MEM_BIST_FAILED;
            }
            if (((errorStatus1 >> (i * 8)) & 0xF0) != 0) {
              errorCount++;
              strobe = i + 4 + 9;
              (*rankList)[rank].faultyParts[strobe] |= FPT_MEM_BIST_FAILED;
            }
          } else {
            errorCount++;
            strobe = i + 4;
            (*rankList)[rank].faultyParts[strobe] |= FPT_MEM_BIST_FAILED;
          }
          MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
            "FPT strobe = %d!\n", strobe));
        }
      }
      //
      // errorStatus2
      //
      if ((errorStatus2 & 0xFF) != 0) {
        strobe = 8;
        if ((*dimmNvList)[dimm].x4Present) {
          if ((errorStatus2 & 0x0F) != 0) {
            errorCount++;
            strobe =  8;
            (*rankList)[rank].faultyParts[strobe] |= FPT_MEM_BIST_FAILED;
          }
          if ((errorStatus2 & 0xF0) != 0) {
            errorCount++;
            strobe = 17;
            (*rankList)[rank].faultyParts[strobe] |= FPT_MEM_BIST_FAILED;
          }
        } else {
          errorCount++;
          strobe = 8;
          (*rankList)[rank].faultyParts[strobe] |= FPT_MEM_BIST_FAILED;
        }
        MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
          "FPT strobe = %d!\n", strobe));
      }
      //
      // If we detect errors on more than 1 DRAM chip, it is uncorrectable.
      //
      if (errorCount > 1) {
        errorType = 1;
      }
    }
  }
  return errorType;
}


/**

  Checks the results of the preceding memory test

  @param host                - Pointer to sysHost
  @param socket              - Socket Id
  @param chBitmask           - List of channels that were tested
  @param dimm                - DIMM number (0-based)
  @param rank                - Rank number (0-based)

  @retval SUCCESS

**/
UINT8
CheckTestResults (
  PSYSHOST                  host,
  UINT8                     socket,
  UINT32                    chBitmaskIn,
  UINT32                   *chBitmaskOut,
  UINT8                     dimmRank[MAX_RANK_CH][MAX_CH],
  UINT8                     logical2Physical[MAX_RANK_CH][MAX_CH],
  UINT8                     logicalRank,
  UINT8                     logicalSubRank,
  UINT8                     bank,
  BOOLEAN                   secondPass
  )

{
  UINT32              errorStatus0;
  UINT32              errorStatus1;
  UINT32              errorStatus2;
  UINT32              errorBits[3];
  UINT8               *dqBytes;
  UINT8               errorType; // 0 = correctable.  1 = uncorrectable.
  UINT8               dimm;
  UINT8               ch;
  UINT8               rank;
  CPGC_ERROR_STATUS   cpgcErrorStatus[MAX_CH];

  *chBitmaskOut = chBitmaskIn;

  // Collect test results
  CHIP_FUNC_CALL(host, CpgcAdvTrainingErrorStatus (host, socket, chBitmaskIn, cpgcErrorStatus));
  for (ch = 0; ch < MAX_CH; ch++){
    dimm = dimmRank[logicalRank][ch];
    rank = logical2Physical[logicalRank][ch];

    //
    // Clear error counter
    //
    errorType  = 0;

    //
    // Was this channel part of the test group?
    //
    if (chBitmaskIn & (1 << ch)) {
      //
      // Read error status
      //
      errorStatus0 = cpgcErrorStatus[ch].cpgcErrDat0S | cpgcErrorStatus[ch].cpgcErrDat2S;
      errorStatus1 = cpgcErrorStatus[ch].cpgcErrDat1S | cpgcErrorStatus[ch].cpgcErrDat3S;
      if (host->nvram.mem.eccEn) {
        errorStatus2 = (cpgcErrorStatus[ch].cpgcErrEccS & 0xFF) | ((cpgcErrorStatus[ch].cpgcErrEccS >> 8) & 0xFF);
      } else {
        errorStatus2 = 0;
      }
      errorBits[0] = errorStatus0;
      errorBits[1] = errorStatus1;
      errorBits[2] = errorStatus2;
      dqBytes = (UINT8 *)errorBits;

      if (!(host->var.common.emulation & SIMICS_FLAG)) {

        // Check for any bit errors on any lane.
        if ((errorStatus0 | errorStatus1 | errorStatus2) != 0) {
#ifdef SERIAL_DBG_MSG
          if (checkMsgLevel(host, SDBG_MINMAX)) {
            getPrintFControl (host);
            setColor (host, ANSI_FOREGROUND_RED, ANSI_BACKGROUND_BLACK, ANSI_ATTRIBUTE_OFF);
            MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
                          "MemTest Failure!\n"));
            MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
                          "B0:B3 = 0x%x\n", errorStatus0));
            MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
                          "B4:B7 = 0x%x\n", errorStatus1));
            if (host->nvram.mem.eccEn) {
              MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
                            "ECC = 0x%x\n", errorStatus2));
            }
            setWhite (host);
            releasePrintFControl (host);
          }
#endif
          //
          // Now check to see if the errors found are correctable or uncorrectable.
          // For failures only fall within x4 aligned lanes, i.e. failed lanes within a single nibble,
          // e.g. lane 15:12 (regardless of running independent mode or lockstep), you can treat it is correctable error.
          // Otherwise, for failures within x8 device aligned lanes, i.e. fail lanes within a double-nibble
          // (e.g. lane 15:8 is but lane 11:4 is not) and running lock-step mode, you can treat the error as correctable error.
          // Otherwise, remaining failures are UC.
          //
          errorType = ClassifyDqErrorStatus (host, socket, ch, dimm, rank, errorStatus0, errorStatus1, errorStatus2);

          //
          // If the error is uncorrectable and this was the 2nd pass through memtest, disable the rank.
          //
          if (errorType == 1 && secondPass) {
            //
            // Add this error to the warning log for both correctable and uncorrectable errors.
            //
            DisableRank (host, socket, ch, dimm, rank);
            *chBitmaskOut &= ~(1 << ch);
            EwlOutputType5 (host, WARN_MEMTEST_DIMM_DISABLE, WARN_FPT_MINOR_MEM_TEST, socket, ch, dimm, rank, EwlSeverityWarning, logicalSubRank, bank, dqBytes);
          }
          //
          // Else if the error was uncorrectable and this is the first pass through memtest, run it again on the same rank.
          //
          else if (errorType == 1 && !secondPass) {

            return FAILURE;
          } else if (errorType == 0) {

            //
            // Add this error to the warning log for correctable errors.
            //
            EwlOutputType5 (host, WARN_FPT_CORRECTABLE_ERROR, WARN_FPT_MINOR_MEM_TEST, socket, ch, dimm, rank, EwlSeverityWarning, logicalSubRank, bank, dqBytes);
          }
        } // if error
      } // if emulation
    } // if (chBitmask & (1 << ch))
  } // ch loop

  //
  // This means we do not need to rerun the memtest.  Either it passed or the rank was disabled after
  // 2 failing tests in a row.  Returning SUCCESS means that we are done testing this rank.
  //
  return SUCCESS;
} // CheckTestResults


void
SetPatternBuffers (
                  PSYSHOST  host,
                  UINT8     socket,
                  UINT16    mode,
                  UINT16    loopNum
                  )
/*++

  Sets the patter to be used during test

  @param host    - Pointer to sysHost
  @param socket    - Socket Id
  @param mode    - 0 = MemTest; 1 = MemInit
  @param loopNum - Loop number for MemTest

  @retval N/A

--*/
{
  UINT8                     ch;
  UINT8                     rotateBit;
  UINT16                    i;
  UINT16                    lfsrNum;
  UINT32                    lfsrSeed;
  UINT32                    tempSeed;
  UINT32                    pattern;
  UINT32                    *WdbLinePrt;
  TWdbLine                  WdbLines[MRC_WDB_LINES];
  struct channelNvram       (*channelNvList)[MAX_CH];

  lfsrSeed = 0x73DA65;

  if (mode == MEMTEST) {
    //
    // Determine LFSR seed
    //
    lfsrNum   = loopNum >> 1;
    tempSeed  = lfsrSeed;
    if (lfsrNum) {
      //
      // Rotate the LFSR seed for short term effect
      //
      for (i = 0; i < (lfsrNum % 23); i++) {
        rotateBit = (UINT8) (tempSeed & BIT0);
        tempSeed >>= 1;
        if (rotateBit) {
          tempSeed |= BIT22;
        }
      }
      //
      // Add unique number for long term effect
      //
      tempSeed += lfsrNum / 23;
    }

    // Pattern to use
    pattern = tempSeed;

  } else {
    //
    // MEMINIT
    //
    pattern = 0;
  }

  channelNvList = GetChannelNvList(host, socket);

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

    //
    // Fill the WDB buffer with the write content.
    //
    WdbLinePrt = (UINT32 *) &WdbLines[0].WdbLine[0];
    for (i = 0; i < (MRC_WDB_LINE_SIZE / sizeof (UINT32)); i++) {
      WdbLinePrt[i] = pattern;
    }

    WDBFill (host, socket, ch, WdbLines, MRC_WDB_LINES, 0);
  } // ch loop
} // SetPatternBuffers


void
DisableChannel (
               PSYSHOST  host,
               UINT8     socket,
               UINT8     ch
               )
/*++
  Disables the specified channel

  @param host         - Pointer to sysHost
  @param socket       - Socket
  @param ch           - Channel

  @retval N/A

--*/
{
  UINT8               dimm;
  struct channelNvram (*channelNvList)[MAX_CH];
  struct dimmNvram    (*dimmNvList)[MAX_DIMM];

  MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                           "FPT: DisableChannel()\n"));

  channelNvList = GetChannelNvList(host, socket);
  dimmNvList = GetDimmNvList(host, socket, ch);
  (*channelNvList)[ch].enabled = 0;

  for (dimm = 0; dimm < MAX_DIMM; dimm++) {
    //
    // Skip if no DIMM present
    //
    if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

    DisableDIMM(host, socket, ch, dimm);
  } // dimm loop

  CHIP_FUNC_CALL(host, SetChannelDisable (host, socket, ch));
} // DisableChannel

void
DisableDIMM (
            PSYSHOST  host,
            UINT8     socket,
            UINT8     ch,
            UINT8     dimm
            )
/*++
  Disables the specified DIMM

  @param host         - Pointer to sysHost
  @param socket       - Socket
  @param ch           - Channel
  @param dimm         - DIMM to disable

  @retval N/A

--*/
{
  UINT8                       rank;
  MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                           "FPT: DisableDIMM()\n"));

  //
  // Disable DIMM so no maintenance operations go to it
  //
  CHIP_FUNC_CALL(host, SetDimmPop(host, socket, ch, dimm));

  //
  // Disable each rank on this DIMM
  //
  for (rank = 0; rank < MAX_RANK_DIMM; rank++) {

    // Continue to the next rank if this one is disabled
    if (CheckRank(host, socket, ch, dimm, rank, CHECK_MAPOUT)) continue;

    DisableRank(host, socket, ch, dimm, rank);
  } // rank loop
} // DisableDIMM

void
DisableRank (
            PSYSHOST  host,
            UINT8     socket,
            UINT8     ch,
            UINT8     dimm,
            UINT8     rank
            )
/*++
  Disables the specified rank

  @param host        - Pointer to sysHost
  @param socket      - Socket
  @param ch          - Channel
  @param dimm        - DIMM
  @param rank        - Rank to disable

  @retval N/A

--*/
{
  UINT8                       pairChannel;
  UINT8                       pairDimm;
  UINT8                       pairRank;
  UINT8                       pairRankNumber;
  struct ddrRank              (*rankList)[MAX_RANK_DIMM];
  struct dimmNvram            (*dimmNvList)[MAX_DIMM];

  dimmNvList    = GetDimmNvList(host, socket, ch);
  rankList      = GetRankNvList(host, socket, ch, dimm);

  //
  // Make sure the rank has not already been disabled.  This function can get called recurvsively in Mirror/Lockstep
  //

  if ((*rankList)[rank].enabled != 0){
    MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
                             "FPT: DisableRank()\n\n"));

    //
    // Disable rank in host structure
    //
    (*rankList)[rank].enabled = 0;
    (*dimmNvList)[dimm].mapOut[rank] = 1;

    //
    // Rank is logically disabled at this point using rank mapout
    //
    // Notes about the mapout flow:
    //
    // - Before switching from IOSAV mode to Normal mode:
    // CheckRASSupportAfterMemInit evaluates rank mapout settings
    // If all ranks present on a channel are mapped out, then set SCRUBMASK all_ranks field
    // If only some ranks present on a channel are mapped out, then set AMAP dimmX_pat_rank_disable field

    //
    // - After Memtest and init are complete:
    // CheckRASSupportAfterMemInit initializes the patrol scrub settings

    //
    //If in Lockstep or Mirror, disable the paired Rank
    //
    if (CHIP_FUNC_CALL(host, ChipLockstepEn(host)) || CHIP_FUNC_CALL(host, ChipMirrorEn(host))){
      MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
            "System configured for Lockstep or Mirroring.  Disable paired rank(s)\n"));

      pairRankNumber = 1;

      //
      // Disable all paired ranks
      //
      while (pairRankNumber) {

        CHIP_FUNC_CALL(host, GetPairedRank(host, socket, ch, dimm, rank, &pairRankNumber, &pairChannel, &pairDimm, &pairRank));

        //
        // Break if no more pairs
        //
        if (pairRankNumber == 0) break;

        dimmNvList  = GetDimmNvList(host, socket, pairChannel);
        rankList    = GetRankNvList(host, socket, pairChannel, pairDimm);

        MemDebugPrint((host, SDBG_MAX, socket, pairChannel, pairDimm, pairRank, NO_STROBE, NO_BIT,
                       "FPT: DisableRank() paired rank\n\n"));

        //
        // Disable rank in host structure
        //
        (*rankList)[pairRank].enabled = 0;
        (*dimmNvList)[pairDimm].mapOut[pairRank] = 1;

        //
        // Check for another pair
        //
        pairRankNumber++;
      }
    } //end if lockstep
  }
} // DisableRank

void
GetDimmPresent (
               PSYSHOST  host,
               UINT8     socket,
               UINT8     *dimmMask[MAX_CH]
               )
/*++
  Returns a bit mask of DIMMs present.

  @param host:         Pointer to sysHost
  @param socket:         Processor socket to check
  @param dimmMask:     Pointer to results

  @retval N/A
--*/
{
  UINT8  ch;
  UINT8  dimm;
  struct dimmNvram (*dimmNvList)[MAX_DIMM];
  struct channelNvram (*channelNvList)[MAX_CH];

  channelNvList = GetChannelNvList(host, socket);

  // Update channels
  for (ch = 0; ch < MAX_CH; ch++) {

    // Initialize to 0 for this channel
    *dimmMask[ch] = 0;

    // Go to next channel if this is disabled
    if ((*channelNvList)[ch].enabled == 0) continue;

    dimmNvList = GetDimmNvList(host, socket, ch);

    for (dimm = 0; dimm < MAX_DIMM; dimm++) {

      // Check next DIMM if this one is not present or is mapped out
      //if (((*dimmNvList)[dimm].dimmPresent == 0) || ((*dimmNvList)[dimm].mapOut)) continue;
      if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

      // Indicate this dimm is present
      *dimmMask[ch] |= 1 << dimm;

    } // dimm loop
  } // ch loop
} // GetDimmPresent


void
GetRankPresent (
               PSYSHOST  host,
               UINT8     socket,
               UINT8     *rankMask[MAX_CH]
               )
/*++
  Returns a bit mask of logical ranks present.

  @param host:         Pointer to sysHost
  @param socket:         Processor socket to check
  @param rankMask:     Pointer to results

  @retval N/A
--*/
{
  UINT8  ch;
  UINT8  dimm;
  UINT8  rank;
  struct channelNvram (*channelNvList)[MAX_CH];
  struct dimmNvram (*dimmNvList)[MAX_DIMM];
  struct ddrRank (*rankList)[MAX_RANK_DIMM];

  channelNvList = GetChannelNvList(host, socket);

  // Update channels
  for (ch = 0; ch < MAX_CH; ch++) {

    // Initialize to 0 for this channel
    *rankMask[ch] = 0;

    // Go to next channel if this is disabled
    if ((*channelNvList)[ch].enabled == 0) continue;

    dimmNvList = GetDimmNvList(host, socket, ch);

    for (dimm = 0; dimm < MAX_DIMM; dimm++) {

      // Check next DIMM if this one is not present or is mapped out
      if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

      rankList = GetRankNvList(host, socket, ch, dimm);

      // Loop for each rank
      for (rank = 0; rank < MAX_RANK_DIMM; rank++) {

        // Continue to the next rank if this one is disabled
        if (CheckRank(host, socket, ch, dimm, rank, CHECK_MAPOUT)) continue;

        // Indicate this rank is present
        *rankMask[ch] |= 1 << (*rankList)[rank].logicalRank;
      } // rank loop
    } // dimm loop
  } // ch loop
} // GetRankPresent


void
SetRankIndexes (
               PSYSHOST host,
               UINT8    socket,
               UINT8    ch,
               UINT8    dimm,
               UINT8    rank
               )
/*++

  This routine determines that the various indexes associated with each rank

  @param host  - Pointer to sysHost
  @param socket  - Socket Id
  @param ch    - Channel number (0-based)
  @param dimm  - DIMM number (0-based)
  @param rank  - Rank number (0-based)

  @retval N/A

--*/
{
  struct ddrRank      (*rankList)[MAX_RANK_DIMM];
  struct RankCh       (*rankPerCh)[MAX_RANK_CH];
  struct dimmNvram  (*dimmNvList)[MAX_DIMM];
  struct channelNvram (*channelNvList)[MAX_CH];

  channelNvList = GetChannelNvList(host, socket);
  dimmNvList    = GetDimmNvList(host, socket, ch);
  rankList      = GetRankNvList(host, socket, ch, dimm);
  rankPerCh     = &(*channelNvList)[ch].rankPerCh;

  //
  // Enforce that rankIndex == logicalRank
  //
  (*rankList)[rank].rankIndex = GetLogicalRank(host, socket, ch, dimm, rank);

  if (((host->nvram.mem.dramType != SPD_TYPE_DDR3) && ((*channelNvList)[ch].encodedCSMode != 1)) ||
      ((host->nvram.mem.dramType == SPD_TYPE_DDR3) && ((*channelNvList)[ch].lrRankMultEnabled == 0))  || (rank < 2)) {
    (*rankPerCh)[(*rankList)[rank].rankIndex].dimm = dimm;
    (*rankPerCh)[(*rankList)[rank].rankIndex].rank = rank;
  }

  //
  // Get physical rank
  //
  (*rankList)[rank].phyRank = (dimm * 4) + rank;

  //
  // Get logical rank
  //
  (*rankList)[rank].logicalRank  = GetLogicalRank(host, socket, ch, dimm, rank);

  //
  // Get ODT Index
  //
  if (((*dimmNvList)[dimm].numRanks > 2) && (host->nvram.mem.dramType == SPD_TYPE_DDR3)) {
    (*rankList)[rank].ODTIndex = (dimm << 1) | (rank >> 1);
  } else {
    (*rankList)[rank].ODTIndex = (dimm << 1) | (rank & 1);
  }

  //
  // Get CKE Index
  //
  (*rankList)[rank].CKEIndex = (*rankList)[rank].logicalRank;

  //
  // Get CK Index
  //
  if (host->nvram.mem.dimmTypePresent == RDIMM) {
    (*rankList)[rank].ckIndex = dimm;
  } else {
    (*rankList)[rank].ckIndex = (rank << 1) | dimm;
  }
  // Get CTL Index
  if (host->nvram.mem.dimmTypePresent == RDIMM) {
    (*rankList)[rank].ctlIndex = (dimm << 1);
  } else {
    (*rankList)[rank].ctlIndex = (dimm << 1) | rank;
  }

  //
  // Get CS Index
  //
  if ((*dimmNvList)[dimm].lrRankMult > 1) {
    (*rankList)[rank].CSIndex = (dimm << 2) | (rank & 1);
  } else {
    (*rankList)[rank].CSIndex = (dimm << 2) | rank;
  }

} // SetRankIndexes


void
FastestCommon (
              UINT8 *time1,
              UINT8 *time2,
              UINT8 *time3
              )
/*++

  Returns the fastest time of the times provided

  @param time1 - 1st time to check
  @param time2 - 2nd time to check
  @param time3 - 3rd time to check (ignored if 0)

  @retval N/A

--*/
{
  UINT8 newTime;

  if (*time1 > *time2) {
    newTime = *time1;
  } else {
    newTime = *time2;
  }

  if ((time3 != 0) && (*time3 > newTime)) {
    newTime = *time3;

  }

  *time1  = newTime;
  *time2  = newTime;
  if (time3 != 0) {
    *time3 = newTime;
  }
} // FastestCommon

void
FastestCommon16 (
                UINT16 *time1,
                UINT16 *time2,
                UINT16 *time3
                )
/*++

  Returns the fastest time of the times provided

  @param time1 - 1st time to check
  @param time2 - 2nd time to check
  @param time3 - 3rd time to check (ignored if 0)

  @retval N/A

--*/
{
  UINT16  newTime;

  if (*time1 > *time2) {
    newTime = *time1;
  } else {
    newTime = *time2;
  }

  if ((time3 != 0) && (*time3 > newTime)) {
    newTime = *time3;

  }

  *time1  = newTime;
  *time2  = newTime;
  if (time3 != 0) {
    *time3 = newTime;
  }
} // FastestCommon16



UINT32
BaseLimitOffset (
                UINT32 value,
                UINT32 base,
                UINT32 limit,
                UINT32 offset
                )
/*++

  Ensure we do not program a value larger than is supported by the field

  @param value   - Value to be programmed
  @param base    - Smallest value that can be programmed
  @param limit   - Largest value that can be programmed
  @param offset  - Offset in the register to be programmed

  @retval regValue  - Value to be programmed

--*/
{
  UINT32  retValue;

  if (value > base) {
    retValue = value - base;
  } else {
    retValue = 0;

  }

  if (retValue > limit) {
    retValue = limit;
  }

  return retValue << offset;
} // BaseLimitOffset


void
FixedQClkDelay (
               PSYSHOST host,
               UINT32   QClk
               )
/*++

  Delay time in QCLK units
    Note: Max delay is 1.2 seconds

  @param host  - Pointer to sysHost
  @param QClk  - Number of QClks to delay

  @retval N/A

--*/
{
  UINT32  delay;

  delay = ((host->var.mem.QCLKPeriod/1000000) * QClk) + 1;
  FixedDelay (host, delay);
} // FixedQClkDelay


UINT8
CheckChDisabled (
                PSYSHOST  host,
                UINT8     socket,
                UINT8     ch
                )
/*++

  Checks if a rank is absent

  @param host    - Pointer to sysHost
  @param socket  - Socket Id
  @param ch      - Channel number

  @retval 1 if rank is disabled or absent
  @retval 0 if enabled

--*/
{

  if (host->nvram.mem.socket[socket].channelList[ch].enabled == 0) {
    return 1;
  } else {
    return 0;
  }

} // CheckRankAbsentDisabled

UINT8
CheckRankAbsent (
                PSYSHOST  host,
                UINT8     socket,
                UINT8     ch,
                UINT8     dimm,
                UINT8     rank
                )
/*++

  Checks if a rank is absent

  @param host    - Pointer to sysHost
  @param socket  - Socket Id
  @param ch      - Channel number
  @param dimm    - DIMM number
  @param rank    - Rank number

  @retval 1 if rank is disabled or absent
  @retval 0 if enabled or present

--*/
{

  if (host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].rankList[rank].enabled == 0) {
    return 1;
  } else {
    return 0;
  }

} // CheckRankAbsentDisabled

UINT8
CheckRankAbsentDisabled (
                        PSYSHOST  host,
                        UINT8     socket,
                        UINT8     ch,
                        UINT8     dimm,
                        UINT8     rank
                        )
/*++

  Checks if a rank is absent or disabled

  @param host    - Pointer to sysHost
  @param socket  - Socket Id
  @param ch      - Channel number
  @param dimm    - DIMM number
  @param rank    - Rank number

  @retval 1 if rank is disabled or absent
  @retval 0 if enabled

--*/
{

  if (host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].rankList[rank].enabled == 0) return 1;
  if (host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].mapOut[rank]) return 1;

  return 0;

} // CheckRankAbsentDisabled


UINT8
CheckRank (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     ch,
  UINT8     dimm,
  UINT8     rank,
  UINT8     flags
  )
/*++

  Returns if this rank is valid

  @param host   - Pointer to sysHost
  @param socket - Socket to check
  @param ch     - Channel to check
  @param dimm   - DIMM to check
  @param rank   - Rank to check
  @param flags  - CHECK_MAPOUT

  @retval SUCCESS

--*/
{
  struct dimmNvram  (*dimmNvList)[MAX_DIMM];
  struct ddrRank    (*rankList)[MAX_RANK_DIMM];

  dimmNvList    = GetDimmNvList(host, socket, ch);
  rankList      = GetRankNvList(host, socket, ch, dimm);

  if ((*rankList)[rank].enabled == 0) return FAILURE;
  if ((flags & CHECK_MAPOUT) && (host->var.mem.checkMappedOutRanks == 0)) {
    if ((*dimmNvList)[dimm].mapOut[rank]) return FAILURE;
  }

  return SUCCESS;
} // CheckRank

UINT32
CheckDimmRanks (
               PSYSHOST host
               )
/*++

  Initialize rank structures.  This is based on the requested
  RAS mode and what RAS modes the configuration supports. This
  routine also evalues the requested RAS modes to ensure they
  are supported by the system configuration.

  @param host  - Pointer to sysHost

  @retval SUCCESS

--*/
{
  UINT8   socket;
  UINT8   ch;
  UINT8   dimm;
  UINT16  size;
  UINT8   i;
  struct ddrRank (*rankList)[MAX_RANK_DIMM];
  struct dimmNvram (*dimmNvList)[MAX_DIMM];
  struct channelNvram (*channelNvList)[MAX_CH];

  //if (!(host->memFlowsExt & MF_EXT_INIT_DIMM_RANK)) return SUCCESS;

  //
  // Check if any ranks have been disabled
  //
  CHIP_FUNC_CALL(host, CheckRankPop(host));

  for (socket = 0; socket < MAX_SOCKET; socket++) {
    // Continue if this socket is not present
    if ((host->nvram.mem.socket[socket].enabled == 0) || (host->nvram.mem.socket[socket].maxDimmPop == 0)) continue;

    channelNvList = GetChannelNvList(host, socket);

    //
    // Look at each channel
    //
    for (ch = 0; ch < MAX_CH; ch++) {

      dimmNvList = GetDimmNvList(host, socket, ch);

      //
      // Look at each DIMM
      //
      for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {
        if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

        rankList = GetRankNvList(host, socket, ch, dimm);

        if ((host->nvram.mem.dramType != SPD_TYPE_DDR3) && IsLrdimmPresent(host, socket, ch, dimm)) {
          i = (*dimmNvList)[dimm].numDramRanks;
        } else {
          i = (*dimmNvList)[dimm].numRanks;
        }

        //
        // Loop for each rank
        //
        for (; i != 0; i--) {

          // Skip ranks that are disabled
          if (CHIP_FUNC_CALL(host, IsRankDisabled(host, socket, ch, dimm, i - 1))) continue;

#ifdef  LRDIMM_SUPPORT
          if (IsLrdimmPresent(host, socket, ch, dimm) && (host->nvram.mem.dramType == SPD_TYPE_DDR3)) {
            if ((*dimmNvList)[dimm].dimmAttrib & BIT5) {

              // Skip All ranks except Rank 0 for 1/2 populated LRDIMMs
              if (i > 1) {
                MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, i - 1, NO_STROBE, NO_BIT,
                               "LRDIMM (1/2 Pop) Skip Rank\n"));
                continue;
              }
            }
          }
#endif
          //
          // Determine rank size
          //
          if ((*dimmNvList)[dimm].mapOut[i - 1]) {
            size = 0;
          } else {
            if (host->nvram.mem.dramType == SPD_TYPE_DDR3) {
              size = rankSize[(*dimmNvList)[dimm].techIndex];
            } else {
              if ((*dimmNvList)[dimm].aepDimmPresent){
                size = rankSizeAEP[(*dimmNvList)[dimm].aepTechIndex];
              } else {
                size = rankSizeDDR4[(*dimmNvList)[dimm].techIndex];
                //
                //Handle non-symmetric dimms
                //
                if (((*dimmNvList)[dimm].SPDSecondaryDeviceType & SPD_HALF_SIZE_SECOND_RANK) && ((i - 1) % 2)) {
                  size = size / 2;
                } else if (((*dimmNvList)[dimm].SPDSecondaryDeviceType & SPD_QUARTER_SIZE_SECOND_RANK) && ((i - 1) % 2)) {
                  size = size / 4;
                }
              }
            }
          }

          if (((*dimmNvList)[dimm].dieCount) && ((*channelNvList)[ch].encodedCSMode == 2)) {
            size *= (*dimmNvList)[dimm].dieCount;
          }

          SetRankIndexes (host, socket, ch, dimm, i - 1);

          //
          // Enable this rank (0-based)
          //
          (*rankList)[i - 1].enabled  = 1;

          if ((host->nvram.mem.dramType != SPD_TYPE_DDR3) && ((*channelNvList)[ch].encodedCSMode == 1)) {
            //
            // for non-3DS encoded mode, the size of ranks 0 and 1 are double and 2 and 3 are size 0.
            //
            if (i < 3) {
              (*rankList)[i - 1].rankSize = size << 1;
              (*rankList)[i - 1].remSize  = size << 1;
            } else {
              (*rankList)[i - 1].rankSize = 0;
              (*rankList)[i - 1].remSize  = 0;
            }
          } else {
            (*rankList)[i - 1].rankSize = size;
            (*rankList)[i - 1].remSize  = size;
          }

#ifdef NVMEM_FEATURE_EN
#ifdef MEM_NVDIMM_EN
          if ((((*dimmNvList)[dimm].nvDimmType && host->setup.mem.ADRDataSaveMode == ADR_NVDIMM) || host->setup.mem.socket[socket].ddrCh[ch].batterybacked) && !(host->nvram.mem.aepDimmPresent))
#else
          if (host->setup.mem.socket[socket].ddrCh[ch].batterybacked && !(host->nvram.mem.aepDimmPresent))
#endif
          {
            MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, i - 1, NO_STROBE, NO_BIT,
              "RankSize"));
            if ((*channelNvList)[ch].encodedCSMode == 1) {
              //
              // For encoded mode, the size of ranks 0 and 1 are double and 2 and 3 are size 0.
              //
              if (i < 3) {
                (*rankList)[i - 1].NVrankSize = size << 1;
                (*rankList)[i - 1].remSize  = size << 1;
              } else {
                (*rankList)[i - 1].NVrankSize = 0;
                (*rankList)[i - 1].remSize  = 0;
              }
            } else {
              (*rankList)[i - 1].NVrankSize = size;
              (*rankList)[i - 1].remSize  = 0;
            }
          }
#endif
        } // rank loop
      } // dimm loop

      CHIP_FUNC_CALL(host, SetRankTerminationMask(host, socket, ch));

    } // ch loop
  } // socket loop

  CHIP_FUNC_CALL(host, RASCheckDimmRanks(host));

  return SUCCESS;
} // CheckDimmRanks

void
ResetSystem (
  PSYSHOST host,
  UINT8    resetType
  )
/*++

  Issues a platform reset

  @param host      - Pointer to sysHost
  @param resetType - Reset type

  @retval N/A

--*/
{

  if (resetType == POST_RESET_WARM) {
    //
    // Reset the system
    //
    CHIP_FUNC_CALL(host, ResetSystemEx (host, 6));
  } else {
    //
    // Reset the system
    //
    CHIP_FUNC_CALL(host, ResetSystemEx (host, resetType));
  }
} // ResetSystem

#ifdef DEASSERT_IO_RESET_SUPPORT
void
DeassertIoReset (
  PSYSHOST host,
  UINT8    socket,
  UINT8    channel
  )
/*++

  This deasserts resets all MC IO modules

  @param host    - Pointer to sysHost
  @param socket  - Socket number

  @retval N/A

--*/
{
  DeassertIoResetEx(host, socket, channel);
}
#endif // DEASSERT_IO_RESET_SUPPORT

//
// Chip/OEM API via Core
//

/**

    Hook to call the OemInitializePlatformData routine

    @param host - Pointer to SysHost

    @retval None

**/
VOID
OemInitializePlatformDataCore (
    PSYSHOST             host
    )
{
  OemInitializePlatformData (host);
} // OemInitializePlatformDataCore

/**

    Hook to call CheckAndHandleResetRequests routine

    @param host - Pointer to SysHost

    @retval None

**/
VOID
OemCheckAndHandleResetRequestsCore(
    PSYSHOST host
    )
{
  OemCheckAndHandleResetRequests(host);
} // OemCheckAndHandleResetRequestsCore

#if defined(ME_SUPPORT_FLAG) && ME_SUPPORT_FLAG
/**

    Hook to call MEBIOSCheckAndHandleResetRequests routine

    @param host - Pointer to SysHost

    @retval None

**/
VOID
MEBIOSCheckAndHandleResetRequestsCore(
    PSYSHOST host
    )
{
  MEBIOSCheckAndHandleResetRequests(host);
} // MEBIOSCheckAndHandleResetRequestsCore
#endif // ME_SUPPORT_FLAG

/**

    Hook to call OemPublishDataForPost routine

    @param host - Pointer to SysHost

    @retval None

**/
VOID
OemPublishDataForPostCore (
  PSYSHOST host
  )
{
  OemPublishDataForPost (host);
} // OemPublishDataForPostCore

VOID
OemPostUncoreInitCore (
  PSYSHOST host,
  UINT32   UncoreStatus
  )
{
} // OemPostUncoreInitCore

//
// Host struct abstraction functions
//
struct imcNvram (*GetImcNvList(
  PSYSHOST host,
  UINT8    socket
  ))[MAX_IMC]
{
  return(&host->nvram.mem.socket[socket].imc);
}

struct channelNvram (*GetChannelNvList (
  PSYSHOST  host,
  UINT8     socket
  ))[MAX_CH]
{
  return(&host->nvram.mem.socket[socket].channelList);
}

struct dimmNvram (*GetDimmNvList (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     ch
  ))[MAX_DIMM]
{
  return(&host->nvram.mem.socket[socket].channelList[ch].dimmList);
}

struct ddrRank (*GetRankNvList (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     ch,
  UINT8     dimm
  ))[MAX_RANK_DIMM]
{
  return(&host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].rankList);
}

struct rankDevice (*GetRankStruct (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     ch,
  UINT8     dimm
  ))[MAX_RANK_DIMM]
{
  return(&host->var.mem.socket[socket].channelList[ch].dimmList[dimm].rankStruct);
}

//
// Host struct abstraction functions
//
 UINT8  GetCpu (
  PSYSHOST  host,
  UINT8     socket
  )
{
  return host->var.common.cpuType;
} // GetCpu


