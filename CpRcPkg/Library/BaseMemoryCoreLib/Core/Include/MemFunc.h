//
// This file contains an 'Intel Pre-EFI Module' and is licensed
// for Intel CPUs and Chipsets under the terms of your license
// agreement with Intel or your vendor.  This file may be
// modified by the user, subject to additional terms of the
// license agreement
//
/**

  Memory Reference Code

  ESS - Enterprise Silicon Software

  INTEL CONFIDENTIAL

@copyright
  Copyright 2006 - 2017 Intel Corporation All Rights Reserved.

  The source code contained or described herein and all documents
  related to the source code ("Material") are owned by Intel
  Corporation or its suppliers or licensors. Title to the Material
  remains with Intel Corporation or its suppliers and licensors.
  The Material contains trade secrets and proprietary and confidential
  information of Intel or its suppliers and licensors. The Material
  is protected by worldwide copyright and trade secret laws and treaty
  provisions.  No part of the Material may be used, copied, reproduced,
  modified, published, uploaded, posted, transmitted, distributed, or
  disclosed in any way without Intel's prior express written permission.

  No license under any patent, copyright, trade secret or other
  intellectual property right is granted to or conferred upon you
  by disclosure or delivery of the Materials, either expressly,
  by implication, inducement, estoppel or otherwise. Any license
  under such intellectual property rights must be express and
  approved by Intel in writing.

@file
  MemFunc.h

@brief
       This file contains memory detection and initialization for
       IMC and DDR3 modules compliant with JEDEC specification.

**/

#ifndef _memfunc_h
#define _memfunc_h
#include "DataTypes.h"
#include "SysHost.h"
#include "Cpgc.h"
#include "MemFuncChipCommon.h"
#include "PlatformFuncCommon.h"
#include "CoreApi.h"
#include "ChipApi.h"
#include "PlatformApi.h"


// Main flow

// MemMain.c
/**

  This function returns the reset status

  @param host - Pointer to sysHost, the system host (root) structure

  @retval SUCCESS

**/
UINT32 CheckStatus(PSYSHOST host);    // CALLTABLE

// InitMem.c
/**

  This function checks if Vdd needs to be set

  @param host - Pointer to sysHost, the system host (root) structure

  @retval SUCCESS

**/
UINT32 CheckVdd(PSYSHOST host);                   // CALLTABLE

/**

  Determines what DDR frequency to run at and programs it

  @param host  - Pointer to sysHost, the system host (root) structure

  @retval SUCCESS - Clock is set properly
  @retval FAILURE - A reset is required to set the clock

**/
UINT32 InitDdrClocks(PSYSHOST host);              // CALLTABLE

/**

  Initialize rank structures.  This is based on the requested
  RAS mode and what RAS modes the configuration supports. This
  routine also evalues the requested RAS modes to ensure they
  are supported by the system configuration.

  @param host  - Pointer to sysHost, the system host (root) structure

  @retval SUCCESS

**/
UINT32 CheckDimmRanks(PSYSHOST host);             // CALLTABLE

/**

  This deasserts resets all MC IO modules

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket number
  @param channel - Channel number

  @retval None

**/
void   DeassertIoReset ( PSYSHOST host, UINT8 socket, UINT8 channel);     // OEM

#ifdef SSA_FLAG
/**

  Initialize the SSA API

  @param host - Pointer to sysHost, the system host (root) structure

  @retval SUCCESS

**/
UINT32 SSAInit(PSYSHOST host);                                            //Calling the BSSA API
#endif  //SSA_FLAG

// MrcSsaServices.c
#ifdef SSA_FLAG
/**

  Initialize the SsaBiosServices data structure.

  @param[in] MrcData  - The MRC global data area.

  @retval None

**/
extern void SsaBiosInitialize(PSYSHOST host);                            //Initialize the SsaBiosServices data structures
#endif  //SSA_FLAG

// MemPpr.c
/**



  @param host - Pointer to sysHost, the system host (root) structure

  @retval SUCCESS
  @retval FAILURE

**/
UINT32 PostPackageRepairMain (PSYSHOST  host);        // CALLTABLE

/**

  Routine that runs Post Package Repair for all valid entries in error address list

  @param host      - Pointer to sysHost, the system host (root) structure
  @param chBitMask - Bit mask of channels

  @retval SUCCESS
  @retval FAILURE

**/
UINT32 PostPackageRepair (PSYSHOST  host, UINT32 chBitMask);

// MemJedec.c
#define A15_PARITY     BIT2
#define A17_PARITY     BIT3
#define A_SIDE         0
#define B_SIDE         1
#define REF_STAG_0     0
#define REF_STAG_20    1
#define REF_STAG_30    2
#define REF_STAG_40    3
#define REF_STAG_60    4
#define REF_STAG_80    5
#define REF_STAG_100   6
#define REF_STAG_120   7

/**

  Muli-use function to either get or set control delays based on the provided group number

  @param host     - Pointer to sysHost, the system host (root) structure
  @param socket   - Socket number
  @param ch       - Channel number
  @param dimm     - Dimm number
  @param DdrLevel - Level of the bus
  @param clk      - Clock number
  @param mode     - GSM_READ_CSR - Read the data from hardware and not cache
                    GSM_READ_ONLY - Do not write
                    GSM_WRITE_OFFSET - Write offset and not value
                    GSM_FORCE_WRITE - Force the write
  @param value    - Value to program

  @retval MRC_STATUS_SUCCESS
  @retval Pi Delay

**/
MRC_STATUS GetSetClkDelayCore (PSYSHOST  host, UINT8 socket, UINT8 ch, UINT8 dimm, GSM_LT DdrLevel, UINT8 clk, UINT8 mode, INT16 *value);

/**

  Muli-use function to either get or set control delays based on the provided group number

  @param host     - Pointer to sysHost, the system host (root) structure
  @param socket   - Socket number
  @param ch       - Channel number
  @param dimm     - Dimm number
  @param level    - Level of the bus
  @param group    - Group number
  @param mode     - GSM_READ_CSR - Read the data from hardware and not cache
                    GSM_READ_ONLY - Do not write
                    GSM_WRITE_OFFSET - Write offset and not value
                    GSM_FORCE_WRITE - Force the write
  @param value    - Value to program
  @param *minVal  - Current minimum control delay
  @param *maxVal  - Current maximum control delay

  @retval status

**/
MRC_STATUS GetSetCtlGroupDelayCore (PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, GSM_LT level, GSM_GT group, UINT8 mode, INT16 *value, UINT16 *minVal, UINT16 *maxVal);

/**

  Muli-use function to either get or set command delays based on the provided group number

  @param host     - Pointer to sysHost, the system host (root) structure
  @param socket   - Socket number
  @param ch       - Channel number
  @param dimm     - Dimm number
  @param level    - Level of the bus
  @param group    - Group number
  @param mode     - GSM_READ_CSR - Read the data from hardware and not cache
                    GSM_READ_ONLY - Do not write
                    GSM_WRITE_OFFSET - Write offset and not value
                    GSM_FORCE_WRITE - Force the write
  @param value    - Value to program
  @param *minVal  - Current minimum command delay
  @param *maxVal  - Current maximum command delay

  @retval status

**/
MRC_STATUS GetSetCmdGroupDelayCore (PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, GSM_LT level, GSM_GT group, UINT8 mode, INT16 *value, UINT16 *minVal, UINT16 *maxVal);

/**

  Muli-use function to either get or set command vref

  @param host     - Pointer to sysHost, the system host (root) structure
  @param socket   - Socket number
  @param ch       - Channel number
  @param dimm     - Dimm number
  @param level    - Level of the bus
  @param mode     - GSM_READ_CSR - Read the data from hardware and not cache
                    GSM_READ_ONLY - Do not write
                    GSM_WRITE_OFFSET - Write offset and not value
                    GSM_FORCE_WRITE - Force the write
  @param value    - Value to program or offset

  @retval status

**/
MRC_STATUS GetSetCmdVrefCore (PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, GSM_LT level, UINT8 mode, INT16 *value);

/**

  Determine if Backside CMD Margin is enabled

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket number

  @retval TRUE - Backside CMD Margin is enabled
  @retval FALSE - Backside CMD Margin is not enabled

**/
UINT8 IsBacksideCmdMarginEnabled (PSYSHOST host, UINT8 socket);

/**

  Determine if the DDR4 register is Rev 2

  @param host   - Pointer to sysHost, the system host (root) structure
  @param socket - Socket number
  @param ch     - Channel number
  @param dimm   - Dimm number

  @retval TRUE - Register is Rev 2
  @retval FALSE - Register is not Rev 2

**/
UINT8 IsDdr4RegisterRev2 (PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm);

/**

  Writes a nibble or byte to DDR4 Register Control using the indirect method

  @param host             - Pointer to sysHost, the system host (root) structure
  @param socket           - Socket Id
  @param ch               - Channel number (0-based)
  @param dimm             - Dimm number
  @param rank             - Rank number (0-based)
  @param controlWordData  - Control Word data (a nibble or byte)
  @param controlWordFunc  - Control Word Function
  @param controlWordAddr  - Control Word Address

  @retval None

**/
void IndirectRCWrite(PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, UINT8 rank, UINT8 controlWordData, UINT8 controlWordFunc, UINT8 controlWordAddr);

/**

  Function to shift backside Cmd, Ctl, Ck by +/- 31/64 clock

  @param host       - Pointer to sysHost, the system host (root) structure
  @param socket     - Socket Id
  @param direction  - +/- direction to move timings

  @retval SUCCESS

**/
UINT32 BacksideShift(PSYSHOST host, UINT8 socket, INT8 direction);
void DumpDimmStateDdr4 (PSYSHOST host, UINT8 socket);
//
// MemTraining.c
//


/**

  Re-centers using a more robust test

  @param host - Pointer to sysHost, the system host (root) structure

  @retval SUCCESS

**/
UINT32 TxLRDIMMDqCentering(PSYSHOST host);

//
// MemPowerTraining.c
//

/**

  Train CTLE

  @param host - Pointer to sysHost, the system host (root) structure

  @retval SUCCESS

**/
UINT32 TrainCTLE (PSYSHOST host);

/**

  Train TCO COMP

  @param host - Pointer to sysHost, the system host (root) structure

  @retval SUCCESS

**/
UINT32 TrainTcoComp (PSYSHOST host);

/**

  Train McRON

  @param host - Pointer to sysHost, the system host (root) structure

  @retval SUCCESS

**/
UINT32 TrainMcRon (PSYSHOST host);

/**

  Train McODT

  @param host - Pointer to sysHost, the system host (root) structure

  @retval SUCCESS

**/
UINT32 TrainMcOdt (PSYSHOST host);

/**

  Train DRAM RON

  @param host - Pointer to sysHost, the system host (root) structure

  @retval SUCCESS

**/
UINT32 TrainDramRon (PSYSHOST host);

#define LINEAR_SWEEP {{0},{1},{2}.{3}.{4},{5},{6},{7},{8},{9},{10},{11},{12},{13},{14},{15},{16},{17},{18},{19}}    // Power Training definitions
#define LINEAR_ARRAY {0,100,200,300,400,500,600,700,800,900,1000,1100,1200,1300,1400,1500,1600,1700,1800,1900}

/**

  This function implements a generic 1-D parameter optimization

  @param host           - Pointer to sysHost, the system host (root) structure
  @param socket         - Socket Id
  @param mode           - Test mode
  @param patternLength  - Pattern length
  @param group          - Array of Groups
  @param numGroups      - Number of Groups in the group array
  @param marginType     - Type of margining to perform
  @param settingList    - Array of settings
  @param powerLevel     - Array for power level (LINEAR_SWEEP or LINEAR_ARRAY)
  @param numSettings    - Number of settings in the settingList Array
  @param param          - Array of parameters to pass
  @param paramType      - Array of parameter types
  @param numParams      - Number of entries in param and ParamType arrays

  @retval None

**/
void TrainDDROptParam (PSYSHOST host,UINT8 socket,UINT16 mode,UINT32 patternLength,GSM_GT group[MAX_GROUPS],UINT8 numGroups,UINT8 marginType,INT16 settingList[MAX_ODT_SETTINGS][MAX_PARAMS],UINT16 powerLevel[MAX_SETTINGS],UINT8 numSettings,GSM_GT param[MAX_PARAMS],UINT8 paramType,UINT8 numParams);

/**

  Calculate the power trend

  @param host             - Pointer to sysHost, the system host (root) structure
  @param socket           - Socket Id
  @param averageMargins   - Average margins
  @param powerLevel       - Array for power level (LINEAR_SWEEP or LINEAR_ARRAY)
  @param slopeFactor      - The tradeoff between power and margin. (Set to 0 to configure the algorithm to only look for margin gain)
  @param powerTrendLine   - Calculated power trend for each setting
  @param numSettings      - Number of settings
  @param numGroups        - Number of groups

  @retval None

**/
void CalcPowerTrend (PSYSHOST host, UINT8 socket, INT16 averageMargins[MAX_GROUPS][MAX_ODT_SETTINGS], UINT16 powerLevel[MAX_SETTINGS], UINT8 slopeFactor, INT16 powerTrendLine[MAX_SETTINGS], UINT8 numSettings, UINT8 numGroups);

/**

  Find the optimal trade off between margins and power

  @param host            - Pointer to sysHost, the system host (root) structure
  @param socket          - Socket Id
  @param averageMargins  - Average margins
  @param powerTrendLine  - Calculated power trend for each setting
  @param numSettings     - Number of settings
  @param numGroups       - Number of groups

  @retval INT16

**/
INT16 FindOptimalTradeOff(PSYSHOST host,UINT8 socket,INT16 averageMargins[MAX_GROUPS][MAX_ODT_SETTINGS],INT16 powerTrendLine[MAX_SETTINGS],UINT8 numSettings,UINT8 numGroups);


// MemRecEnable.c

// MemODT.c
// ODT programming
#define  RD_TARGET      0
#define  WR_TARGET      1

#ifdef  LRDIMM_SUPPORT
// MemDdr4Lrdimm.c
// DDR4 LRDIMM Backside Training (RX)

/**

  Perform DDR4 LRDIMM Backside Read Training (RX)

  @param host - Pointer to sysHost, the system host (root) structure

  @retval SUCCESS

**/
UINT32 Ddr4LrdimmBacksideRxPhase (PSYSHOST host);   // CALL TABLE

/**

  Perform DDR4 LRDIMM Backside Read Training (RX)

  @param host - Pointer to sysHost, the system host (root) structure

  @retval SUCCESS

**/
UINT32 Ddr4LrdimmBacksideRxCycle (PSYSHOST host);   // CALL TABLE

/**

  Perform DDR4 LRDIMM Backside Read Training (RX)

  @param host - Pointer to sysHost, the system host (root) structure

  @retval SUCCESS

**/
UINT32 Ddr4LrdimmBacksideRxDelay (PSYSHOST host);   // CALL TABLE

// DDR4 LRDIMM Backside Training (TX)

/**

  Perform LRDIMM Backside Training (TX)

  @param host - Pointer to sysHost, the system host (root) structure

  @retval SUCCESS

**/
UINT32 Ddr4LrdimmBacksideTxFineWL (PSYSHOST host);  // CALL TABLE

/**

  Perform LRDIMM Backside Training (TX)

  @param host - Pointer to sysHost, the system host (root) structure

  @retval SUCCESS

**/
UINT32 Ddr4LrdimmBacksideTxCoarseWL (PSYSHOST host);  // CALL TABLE

/**

  Perform LRDIMM Backside Training (TX)

  @param host - Pointer to sysHost, the system host (root) structure

  @retval SUCCESS

**/
UINT32 Ddr4LrdimmBacksideTxDelay (PSYSHOST host);   // CALL TABLE

/**

  Inphi A1 WA

  @param host    - Pointer to sysHost, the system host (root) structure struct
  @param socket  - Socket Id
  @param ch      - Channel number (0-based)
  @param dimm    - Current dimm

  @retval None

**/
void InphiPIWA(PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm);
/**

  Re-centers using a more robust test

  @param host - Pointer to sysHost, the system host (root) structure

  @retval SUCCESS

**/
UINT32 WrLRDIMMVrefCentering(PSYSHOST  host);

/**

  Re-centers using a more robust test

  @param host - Pointer to sysHost, the system host (root) structure

  @retval SUCCESS

**/
UINT32 RdLRDIMMVrefCentering(PSYSHOST host);

/**

  Re-centers using a more robust test

  @param host - Pointer to sysHost, the system host (root) structure

  @retval SUCCESS

**/
UINT32 RxLRDIMMDqCentering(PSYSHOST host);
#endif // LRDIMM_SUPPORT


// PeLoader.c

/**

  Modify the image base field of the PE Header of the output file.

  @param host     - Pointer to sysHost, the system host (root) structure struct
  @param baseAddr - Base Address for the file

  @retval None

**/
void   PlacePeImage(PSYSHOST host, UINT8 *baseAddr);    // DEBUG

#ifdef SERIAL_DBG_MSG
// PerfTracker.c
UINT32 PrintAllStats(PSYSHOST host);                                                        // CALL TABLE
#endif // SERIAL_DBG_MSG


// MemXMP.c

// MemDisplay.c
#define NO_SOCKET       0xFF
#define NO_CH           0xFF
#define NO_DIMM         0xFF
#define NO_RANK         0xFF
#define NO_STROBE       0xFF
#define NO_BIT          0xFF
#define ALL_BANKS       0xFF
#ifdef SERIAL_DBG_MSG
#define SINGLE_LINE     0
#define DOUBLE_LINE     1
#define NOCRLF_FLAG     0
#define CRLF_FLAG       1

/**

  Displays information about the DIMMs present

  @param host - Pointer to sysHost, the system host (root) structure struct

  @retval SUCCESS

**/
UINT32 DIMMInfo (PSYSHOST host);                                                                            // CALLTABLE

/**

  This routine prints out memory configuration of each socket at channel level.

  @param host - Pointer to sysHost, the system host (root) structure struct

  @retval None

**/
void PrintDimmConfig(PSYSHOST host);                                                                       // CALLTABLE

/**

  Early video initialization

  @retval None

**/
void   EarlyVideoInit(void);                                                                                // CHIP

/**

  Displays the training results for all parameters

  @param host - Pointer to sysHost, the system host (root) structure struct

  @retval SUCCESS

**/
UINT32 DisplayTrainResults (PSYSHOST host);                                                                 // CALLTAB

/**

  Function to offset the final training results before entering the OS that would facilitate customer debug.

  @param host - Pointer to sysHost, the system host (root) structure struct

  @retval SUCCESS

**/
UINT32 TrainingResultOffsetCfg (PSYSHOST host);

#endif // SERIAL_DBG_MSG

#ifdef  BDAT_SUPPORT
// MemBdata.c

/**

  Fills the Compatible BIOS Data Structure

  @param host - Pointer to sysHost, the system host (root) structure struct

  @retval SUCCESS

**/
UINT32 FillBDATStructure(PSYSHOST host);                      // CHIP / CORE
#endif  //  BDAT_SUPPORT

// MemSpdSimInit.c

/**

  Check if DIMM is present in socket, ch, slot

  @param socket         - Socket
  @param ch             - Channel
  @param dimm           - DIMM Slot
  @param spdbin         - Pointer to the SPD Data
  @param size           - Length of spdbin

  @retval TRUE
  @retval FALSE

**/
UINT32 MemSpdbinRead (UINT8 socket, UINT8 ch, UINT8 dimm, UINT8 *spdbin, UINT32 *size);         // CHIP

//
// MemRdDqDqs.c
//

/**

  Perform main loop of READ DqDqs algorithm.

  @param host - Pointer to sysHost, the system host (root) structure struct

  @retval SUCCESS

**/
UINT32 ReadDqDqsPerBit (PSYSHOST host);                     // CALLTABLE

// MemCmdClk.c

/**

  DeNormalize all PI's so CMD/CTL's can be margined

  @param host       - Pointer to sysHost, the system host (root) structure struct
  @param socket     - current socket
  @param direction  - DENORMALIZE (-1) or RENORMALIZE (1)

  @retval SUCCESS

**/
UINT32 DeNormalizeCCC(PSYSHOST host, UINT8 socket, INT8 direction);

// Timing
// MemTiming.c

/**

  Adjust DQ/DQS-to-CMD offset based on CMD timing mode.

  @param host            - Pointer to sysHost, the system host (root) structure struct
  @param socket          - Socket Id
  @param ch              - Channel number (0-based)
  @param prevTimingMode  - Previous timing mode
  @param newTimingMode   - New timing mode

  @retval None

**/
void   TxRoundtripAdjust(PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 prevTimingMode, UINT8 newTimingMode); // CHIP
/**

  Update the t_rcwr register and cached value based on current parameters.
  This timing parameter can change if the command timing changes or if
  2tCK (long) write preamble is enabled.

  @param host    - Pointer to sysHost, the system host (root) structure struct
  @param socket  - Socket Id
  @param ch      - Channel number (0-based)

  @retval None

**/
void   UpdateTrcwr (PSYSHOST host, UINT8 socket, UINT8 ch);

// CPGC
// MemCpgc.c

//
// ***********************       CORE to CHIP API Prototypes      ******************
//

//
// Functions which handle register accesses and other project-specific details but have the
// same arguments/return values across all platforms.
//
// InitMemChip.c

/**

  Returns the physical rank number

  @param dimm - Current dimm
  @param rank - Rank number (0-based)

  @retval UINT8

**/
UINT8  GetCpu (PSYSHOST host, UINT8 socket);
struct imcNvram (*GetImcNvList (PSYSHOST host, UINT8 socket))[MAX_IMC];                                 // CORE / CHIP
struct channelNvram (*GetChannelNvList (PSYSHOST host, UINT8 socket))[MAX_CH];                          // CORE / CHIP
struct dimmNvram (*GetDimmNvList (PSYSHOST host, UINT8 socket, UINT8 ch))[MAX_DIMM];                    // CORE / CHIP
struct ddrRank (*GetRankNvList (PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm))[MAX_RANK_DIMM];     // CORE / CHIP
struct rankDevice (*GetRankStruct (PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm))[MAX_RANK_DIMM];  // CORE / CHIP
//
// MemStartChip.c
//

// MemSPD.c
struct dimmDevice (*GetChDimmList(PSYSHOST host, UINT8 socket, UINT8 ch))[MAX_DIMM];                    // CORE

// Memory Commands
// MemIOControl.c

//
// MemTrainingChip.c
//
extern const INT16 RCSimSenseAmp [MAX_STROBE][4];                               // CORE / CHIP

UINT16 CalctDdr4toDdrtRRDD(PSYSHOST host, UINT8 socket, UINT8 ch, TT_VARS *ttVars);                 // CHIP
UINT8 CalctDdrttoDdr4RRDD(PSYSHOST host, UINT8 socket, UINT8 ch, TT_VARS *ttVars);                 // CHIP
UINT8 CalctDdr4toDdrtWWDD(PSYSHOST host, UINT8 socket, UINT8 ch, TT_VARS *ttVars);                 // CHIP
UINT8 CalctDdrttoDdr4WWDD(PSYSHOST host, UINT8 socket, UINT8 ch, TT_VARS *ttVars);                 // CHIP

// MemCpg.c

// MemPciAccess.c
#define PARAM_UNUSED  0xFF // Used in place of ChIdOrBoxInst in MemRead/WritePciCfg for MC functional blocks
                           // that have only 1 instance per MC

//
// MemProjectSpecfic.c
//



/**

  Get DDR CR DDRT

  @param host    - Pointer to sysHost, the system host (root) structure struct
  @param socket  - Socket Id
  @param ch      - Channel number (0-based)

  @retval DDR CR DDRT result

**/
UINT16 GetDdrCrDdrt (PSYSHOST host, UINT8 socket, UINT8 ch);

/**

  Set ERID enable/disable

  @param host    - Pointer to sysHost, the system host (root) structure struct
  @param socket  - Socket Id
  @param ch      - Channel number (0-based)
  @param data    - Enable/Disable

  @retval None

**/
void   SetEridEnable (PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 data);



//MemTrainingChip.c have helper functions landed here

UINT8  IsDramType (PSYSHOST host, UINT8 dramType);

#ifdef SERIAL_DBG_MSG

void   DisplayRxVrefResults (PSYSHOST host, UINT8 socket);

/**

  Displays the Power/Margin Training results

  @param host       -  Pointer to sysHost, the system host (root) structure struct
  @param socket     -  Socket Id
  @param group      -  Signal group

  @retval None

**/
void   DisplayPowerTraingResults(PSYSHOST host, UINT8 socket, GSM_GT group);

/**

  Displays the CMD Vref training results

  @param host   - Pointer to sysHost, the system host (root) structure struct
  @param socket - Socket Id

  @retval None

**/
void   DisplayCMDVrefResults(PSYSHOST host, UINT8 socket);

#endif // SERIAL_DBG_MSG

/**

  This deasserts resets all MC IO modules

  @param host     - Pointer to sysHost, the system host (root) structure struct
  @param socket   - Socket Id
  @param channel  - Channel number (0-based)

  @retval None

**/
void   DeassertIoResetEx ( PSYSHOST host, UINT8 socket, UINT8 channel);

//
// ***********************     CORE only Prototypes     ******************
//

// Main flow

// MemMain.c
#ifndef ASM_INC
/**

  Main function used to initialize the memory controler on each processor.

  @param host              - Pointer to sysHost, the system host (root) structure struct
  @param ctptrOrg          - Pointer to the CallTable to be used
  @param callTableEntries  - Size of the CallTable

  @retval SUCCESS - Memory initialization completed successfully.
  @retval FAILURE -  A reset is required

**/
UINT32 MemMain(PSYSHOST host, CallTableEntryType *ctptrOrg, UINT8 callTableEntries); // CORE
#endif // !ASM_INC

// InitMem.c

/**

  Sets the patter to be used during test

  @param host     - Pointer to sysHost, the system host (root) structure struct
  @param socket   - Socket Id
  @param mode     - 0 = MemTest 1 = MemInit
  @param loopNum  - Loop number for MemTest

  @retval None

**/
void   SetPatternBuffers(PSYSHOST host, UINT8 socket, UINT16 mode, UINT16 loopNum); // CORE
//
/**

  Returns a bitmask of channels that have the current lrdimm and rank enabled

  @param host   - Pointer to sysHost, the system host (root) structure struct
  @param socket - Socket Id
  @param dimm   - Current dimm
  @param rank   - Rank number (0-based)

  @retval Bitmask of channels present for the current rank

**/
UINT32 GetChBitmaskLrdimm(PSYSHOST host, UINT8 socket, UINT8 dimm, UINT8 rank);     // CORE

/**

  Checks DQ swizzling and programs registers to account for it

  @param host    - Pointer to sysHost, the system host (root) structure struct
  @param socket  - Socket Id

  @retval None

**/
void CheckAndProgramDqSwizzle(PSYSHOST host, UINT8 socket);

/**

Checks DQ swizzling and programs registers to account for it

@param host    - Pointer to sysHost, the system host (root) structure struct
@param socket  - Socket Id

@retval None

**/
void CheckAndProgramDqSwizzleAep(PSYSHOST host, UINT8 socket);

// Memory timing routines

/**

  Disables the specified DIMM

  @param host    - Pointer to sysHost, the system host (root) structure struct
  @param socket  - Socket Id
  @param ch      - Channel number (0-based)
  @param dimm    - Current dimm

  @retval None

**/
void   DisableDIMM(PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm);              // CORE Only
//
/**

  Disables the specified rank

  @param host        - Pointer to sysHost, the system host (root) structure struct
  @param socket      - Socket Id
  @param ch          - Channel number (0-based)
  @param dimm        - Current dimm
  @param rank        - Rank to disable

  @retval None

**/
void   DisableRank(PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, UINT8 rank);  // CORE
//
/**

  Displays information about the DIMMs present

  @param host    - Pointer to sysHost, the system host (root) structure struct
  @param socket  - Socket Id
  @param dimm    - Current dimm

  @retval None

**/
void   CheckMappedOut (PSYSHOST  host, UINT8 socket, UINT8 dimm);                   // CORE

// MemSPDDDR4.c

/**

  Store the common DDR4 DIMM configuration in the host structure

  @param host    - Pointer to sysHost, the system host (root) structure struct
  @param socket  - Socket Id
  @param ch      - Channel number (0-based)
  @param dimm    - Current dimm

  @retval None

**/
void   GetCommonDDR4DIMMConfig(PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm);    // CORE

// MemJedec.c
UINT32 FnvPollingBootStatusRegister(PSYSHOST host, UINT8 socket, UINT32 bitmask);
UINT32 FnvPollingBootStatusRegisterCh(PSYSHOST host, UINT8 socket, UINT8 ch, UINT32 bitmask);
/**

  This routine does the twr scaling for the DIMMS capable of the same in the system

  @param host - Pointer to sysHost, the system host (root) structure struct

  @retval SUCCESS

**/
UINT32 TwrScaling (PSYSHOST host);                                                                 // CORE

/**

  Enables or Disables the MPR pattern in all DIMMs/Channels/Ranks

  @param host    - Pointer to sysHost, the system host (root) structure struct
  @param socket  - Socket Id
  @param mprMode - Either 0 for Disable or MR3_MPR to Enable

  @retval None

**/
void   SetAllRankMPR (PSYSHOST host, UINT8 socket, UINT8 mprMode);                                                         // CORE
//
/**

  Enables or Disables the MPR pattern in all DIMMs/Channels/Ranks

  @param host     - Pointer to sysHost, the system host (root) structure struct
  @param socket   - Socket Id
  @param ch       - Channel number (0-based)
  @param dimm     - Current dimm
  @param rank     - Rank number (0-based)
  @param mprMode  - Either 0 for Disable or MR3_MPR to Enable
  @param pattern  - Pattern to use for testing

  @retval None

**/
void   SetRankMPRPattern (PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, UINT8 rank, UINT16 mprMode, UINT16 pattern);  // CORE

/**

  Enables or Disables the MPR pattern in all DIMMs/Channels/Ranks

  @param host      - Pointer to sysHost, the system host (root) structure struct
  @param socket    - Socket Id
  @param ch        - Channel number (0-based)
  @param dimm      - Current dimm
  @param rank      - Rank number (0-based)
  @param mprMode   - Either 0 for Disable or MR3_MPR to Enable
  @param pattern   - Pattern to use for testing

  @retval None

**/
void   SetRankMPRPatternDA (PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, UINT8 rank, UINT16 mprMode, UINT16 pattern);  // CORE

/**

  Perform a DLL reset

  @param host   - Pointer to sysHost, the system host (root) structure struct
  @param socket - Socket Id

  @retval None

**/
void   IssueDllReset(PSYSHOST host, UINT8 socket);                                                  // CORE

// MemMargins.c
#define SET_ALL   0xFF

// MemRecEnable.c

/**

 For each data module, a 128 bit vector of zeros and ones holding the tests results is
 evaluated which is a concatenation of the two reads from the feedback CRs.
 BIOS locates the middle of the longest consecutive stream of ones (when the vector is evaluated as cyclic,
 this location in the vector indicates the PI setting and logic delay for that data module).

  @param host              - Pointer to sysHost, the system host (root) structure struct
  @param socket            - Current Socket
  @param ch                - Channel number
  @param dimm              - Dimm number on ch
  @param rank              - Rank on dimm
  @param strobe            - Strobe number
  @param risingEdge        - Pointer to save the rising edge
  @param centerPoint       - Pointer to save the center point
  @param fallingEdge       - Pointer to save the falling edge
  @param pulseWidth        - Pointer to save the pulse width
  @param step              - The PI Step to work on:
                               FPT_PI_READ_TYPE FPT_PI_WRITE_TYPE
                               FPT_PI_LRDIMM_READ_TYPE
                               FPT_PI_LRDIMM_WRITE_TYPE
                               FPT_PI_LRDIMM_RD_MRD_TYPE
                               FPT_PI_LRDIMM_WR_MRD_TYPE

  @retval None

**/
void   EvaluatePiSettingResults(PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, UINT8 rank, UINT8 strobe,
                              UINT16 risingEdge[MAX_CH][MAX_STROBE], UINT16 centerPoint[MAX_CH][MAX_STROBE],
                              UINT16 fallingEdge[MAX_CH][MAX_STROBE], UINT16 pulseWidth[MAX_CH][MAX_STROBE], UINT8 step); // CORE
//
/**

  This function gets the test engine ready to run tests for Receive Enable Fine training

  @param host       - Pointer to sysHost, the system host (root) structure struct
  @param socket     - Current Socket
  @param chBitmask  - Bitmask of channels to test

  @retval None

**/
void   SetupRecEnFineTest (PSYSHOST host, UINT8 socket, UINT32 chBitmask);                                                // CORE
#ifdef   SERIAL_DBG_MSG
/**

  This function print the sample array

  @param host      - Pointer to sysHost, the system host (root) structure struct
  @param socket    - Socket number
  @param ch        - Channel number
  @param dimm      - DIMM number
  @param ReadMode  - 5 = LRDIMM Write MWD Delay
                     4 = LRDIMM Read DQ DQS
                     3 = LRDIMM Read Recieve Enable Phase
                     2 = LRDIMM Write DWL
                     1 = Read DqDqs
                     0 = Write DqDqs

  @retval None

**/
void   PrintSampleArray (PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, UINT8 ReadMode);                              // CORE

/**

  Prints the edges found

  @param host          - Pointer to sysHost, the system host (root) structure struct
  @param socket        - Current Socket
  @param ch            - Channel number
  @param pulseWidth    - Pointer to save the pulse width
  @param fallingEdge   - Pointer to save the falling edge
  @param risingEdge    - Pointer to save the rising edge
  @param centerPoint   - Pointer to save the center point
  @param maxStrobe     - Maximum number of strobes

  @retval None

**/
void   DisplayEdges (PSYSHOST host, UINT8 socket, UINT8 ch, UINT16 pulseWidth[MAX_STROBE], UINT16 fallingEdge[MAX_STROBE],
                   UINT16 risingEdge[MAX_STROBE], UINT16 centerPoint[MAX_STROBE], UINT8 maxStrobe);                       // CORE

/**

  Prints the edges found for LRDIMM

  @param host          - Pointer to sysHost, the system host (root) structure struct
  @param socket        - Current Socket
  @param ch            - Channel number
  @param pulseWidth    - Pointer to save the pulse width
  @param fallingEdge   - Pointer to save the falling edge
  @param risingEdge    - Pointer to save the rising edge
  @param centerPoint   - Pointer to save the center point
  @param maxStrobe     - Maximum number of strobes

  @retval None

**/
void   DisplayLrdimmMrdEdges (PSYSHOST host, UINT8 socket, UINT8 ch, UINT16 pulseWidth[MAX_STROBE], UINT16 fallingEdge[MAX_STROBE],
                   UINT16 risingEdge[MAX_STROBE], UINT16 centerPoint[MAX_STROBE], UINT8 maxStrobe);                       // CORE
#endif // SERIAL_DBG_MSG

// MemWrLeveling.c

/**

  Changes Rtt Nom to Rtt Wr

  @param host    - Pointer to sysHost, the system host (root) structure struct
  @param socket  - Socket number
  @param dimm    - DIMM number
  @param rank    - Rank number

  @retval None

**/
void   OverrideRttNom(PSYSHOST host, UINT8 socket, UINT8 dimm, UINT8 rank);     // CORE
//
/**

  Restore Rtt Nom

  @param host    - Pointer to sysHost, the system host (root) structure struct
  @param socket  - Socket number
  @param dimm    - DIMM number
  @param rank    - Rank number

  @retval None

**/
void   RestoreRttNom(PSYSHOST host, UINT8 socket, UINT8 dimm, UINT8 rank);      // CORE

// MemODT.c
// ODT programming

/**

  Get the odtActStruct

  @param host            - Pointer to sysHost, the system host (root) structure struct
  @param socket          - Socket number
  @param ch              - Channel number
  @param dimm            - DIMM number
  @param rank            - Rank number

  @retval Pointer to the structure for odtActStruct

**/
struct odtActStruct *LookupOdtAct(PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, UINT8 rank); // CORE

#ifdef  DEBUG_PERFORMANCE_STATS
#ifdef SERIAL_DBG_MSG
// PerfTracker.c

/**

  Sets the current memory phase

  @param host             - Pointer to sysHost, the system host (root) structure struct
  @param majorCheckPoint  - Major Check Point to set
  @param minorCheckPoint  - Minor Check Point to set

  @retval None

**/
void   SetMemPhaseCP(PSYSHOST host, UINT32 majorCheckPoint, UINT32 minorCheckPoint);        // CORE
//
/**

  Clears the memory phase

  @param host             - Pointer to sysHost, the system host (root) structure struct
  @param majorCheckPoint  - Major Check Point to clear
  @param minorCheckPoint  - Minor Check Point to clear

  @retval None

**/
void   ClearMemPhaseCP(PSYSHOST host, UINT32 majorCheckPoint, UINT32 minorCheckPoint);      // CORE
#endif // SERIAL_DBG_MSG
#endif // DEBUG_PERFORMANCE_STATS

// MemDisplay.c
#ifdef SERIAL_DBG_MSG

/**

  Displays information about the device being refered to

  @param host    - Pointer to sysHost, the system host (root) structure struct
  @param socket  - Socket number

  @retval None

**/
void   DisplayDDRFreq (PSYSHOST host, UINT8 socket);                                          // CORE
#endif // SERIAL_DBG_MSG
#ifdef SERIAL_DBG_MSG

/**

  @param host          - Pointer to sysHost
  @param socket        - Processor socket to check
  @param level         - DDR or LRDIMM Host side, LRDIMM Backside, etc...
  @param group         - Parameter to be margined
                            RxDqsDelay    : Margin Rx DqDqs
                            TxDqDelay     : Margin Tx DqDqs
                            RxVref         : Margin Rx Vref
                            TxVref         : Margin Tx Vref
                            RecEnDelay    : Margin Receive Enable
                            WrLvlDelay    : Margin Write Level
                            CmdGrp0        : Margin CMD group 0
                            CmdGrp1        : Margin CMD group 1
                            CmdGrp2        : Margin CMD group 2
                            CmdAll         : Margin all CMD groups
  @param mode          - Test mode to use
                            MODE_XTALK            BIT0: Enable crosstalk (placeholder)
                            MODE_VIC_AGG          BIT1: Enable victim/aggressor
                            MODE_START_ZERO       BIT2: Start at margin 0
                            MODE_POS_ONLY         BIT3: Margin only the positive side
                            MODE_NEG_ONLY         BIT4: Margin only the negative side
                            MODE_DATA_MPR         BIT5: Enable MPR Data pattern
                            MODE_DATA_FIXED       BIT6: Enable Fixed Data pattern
                            MODE_DATA_LFSR        BIT7: Enable Data LFSR
                            MODE_ADDR_LFSR        BIT8: Enable Address LFSR
                            MODE_ADDR_CMD0_SETUP  BIT9
                            MODE_CHECK_PARITY     BIT10: Enable parity checking
                            MODE_DESELECT         BIT11: Enable deselect patterns
                            MODE_VA_DESELECT      BIT12: Enable Victim - Aggressor deselect patterns
  @param scope         - Margin granularity
                           SCOPE_SOCKET      0: Margin per processor socket
                           SCOPE_CH        1: Margin per channel
                           SCOPE_DIMM      2: Margin per DIMM
                           SCOPE_RANK      3: Margin per rank
                           SCOPE_STROBE    4: Margin per strobe group
                           SCOPE_BIT       5: Margin per bit
  @param patternLength - RankMarginTest Pattern length
  @param chMask        - Bit Mask of channels to not be used
  @param rankMask      - Bit Mask of ranks to not be used
  @param dataMask      - Bit Mask for data

  @retval None

**/
void   EyeDiagram(PSYSHOST host, UINT8 socket, GSM_LT level, GSM_GT group, UINT16 mode, UINT8 scope, UINT32 patternLength,
                UINT8 chMask, UINT8 rankMask, UINT8 dataMask[MAX_STROBE/2]);                              // CORE

/**

  Displays the bitwise error status

  @param host    - Pointer to sysHost, the system host (root) structure struct
  @param socket  - Socket Id
  @param ch      - Channel number (0-based)
  @param dimm    - Current dimm
  @param rank    - Rank number (0-based)
  @param bwSerr  - BitWise error status
  @param margin  - Margin

  @retval None

**/
void   DisplayBwSerr(PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, UINT8 rank, UINT32 bwSerr[3], INT16 margin);  // CORE

/**

  Displays the training results

  @param host    - Pointer to sysHost, the system host (root) structure struct
  @param socket  - Socket Id
  @param group   - GSM_GT group

  @retval None

**/
void   DisplayResults(PSYSHOST host, UINT8 socket, GSM_GT group);                                         // CORE
#ifdef LRDIMM_SUPPORT

/**

  Displays the LRDIMM backside training results

  @param host    - Pointer to sysHost, the system host (root) structure struct
  @param socket  - Socket Id
  @param group   - GSM_GT group

  @retval None

**/
void   DisplayDdr4LrdimmTrainingResults(PSYSHOST host, UINT8 socket, GSM_GT group);
#endif
#endif // SERIAL_DBG_MSG

/**

  Function converts a BCD to a decimal value.

  @param BcdValue - An 8 bit BCD value

  @retval The decimal value of the BcdValue

**/
UINT8  BCDtoDecimal (UINT8 BcdValue);                                                                     // CORE


#ifdef  BDAT_SUPPORT
// MemBdata.c
void CopyBDATPerBitMargins(PSYSHOST host);
void CopyPerBitMargins(PSYSHOST host, UINT8 socket, struct bitMargin *resultsBit, GSM_GT group);
#endif  //  BDAT_SUPPORT

// MemWDB.c

/**

  Write 1 cacheline worth of data to the WDB

  @param host             - Pointer to sysHost, the system host (root) structure struct
  @param socket           - Memory Controller
  @param temporalPattern  - Array of bytes.  Each bytes represents 8 chunks of the cachelines for 1 lane
                            Each entry in Patterns represents a different cacheline for a different lane
  @param numCachelines    - Array of len Spread uint8.  Maps the patterns to the actual DQ lanes
                            DQ[0] = Patterns[PMask[0]], ... DQ[Spread-1] = Patterns[PMask[Spread-1]]
                            DQ[Spread] = DQ[0], ... DQ[2*Spread-1] = DQ[Spread-1]

  @retval None

**/
void   WriteTemporalPattern( PSYSHOST host, UINT8 socket, UINT32 temporalPattern, UINT8 numCachelines);                         // CORE


//
// MemRdDqDqs.c
//
#ifdef   SERIAL_DBG_MSG

/**

  Display a summary of the results

  @param host            - Pointer to sysHost, the system host (root) structure struct
  @param socket          - Socket number
  @param ch              - Channel number
  @param dimm            - DIMM number
  @param rank            - Rank number
  @param ErrorResult     - Array that include that data to display
  @param PiMaxNumber     - Array size. It can be 128 or 72 pi
  @param ReadMode        - if ReadMode is:
                             0 = "Read DQ/DQS"
                             1 = "Read Vref"
                             2 = "Write DQ/DQS"
                             3 = "Write Vref"
  @param AggressiveMode  - If true that caller is aggressive training

  @retval None

**/
void   DqDqsDebugDataSummarization(PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, UINT8 rank,
  TErrorResult *ErrorResult, UINT8 PiMaxNumber, UINT8 ReadMode, UINT8 AggressiveMode);   // CORE only

/**

  Display a summary of the results

  @param host        - Pointer to sysHost, the system host (root) structure struct
  @param socket      - Socket number
  @param ch          - Channel number
  @param piPosition  - PI Position
  @param errorStart  - Error start
  @param errorEnd    - Error end

  @retval None

**/
void   DisplayErrorStartEnd(PSYSHOST host, UINT8 socket, UINT8 ch, UINT16 piPosition[MAX_STROBE],
  UINT16 errorStart[MAX_STROBE], UINT16 errorEnd[MAX_STROBE]);  // CORE
#endif // SERIAL_DBG_MSG

// MemCmdClk.c


//#ifndef SIM_BUILD
//
// Decompress.c
//
//#endif //SIM_BUILD

//
// MemTraining.c
//

/**

  Evaluates (based on ODT table) if there are any detected issues
  with strobes and if so whether the set of issues form correctable
  or non-correctable error

  @param host    - Pointer to sysHost, the system host (root) structure struct
  @param socket  - Socket
  @param ch      - Channel

  @retval FPT_NO_ERROR
  @retval FPT_CORRECTABLE_ERROR
  @retval FPT_NO_CORRECTABLE_ERROR

**/
UINT8  EvaluateFaultyParts(PSYSHOST host, UINT8 socket, UINT8 ch);                                        // CORE
//
/**

  Clears the results of the previous test

  @param host    - Pointer to sysHost, the system host (root) structure struct
  @param socket  - Socket
  @param ch      - Channel

  @retval None

**/
void   ClearCmdAddrParityError(PSYSHOST host, UINT8 socket, UINT8 ch);                                    // CORE

UINT8
CheckBacksideSwizzle(
PSYSHOST host,
UINT8    socket,
UINT8    ch,
UINT8    dimm,
UINT8    strobe
);

/**

  Collects the results of the previous test

  @param host    - Pointer to sysHost, the system host (root) structure struct
  @param socket  - Socket
  @param ch      - Channel

  @retval UINT32

**/
UINT32 CheckCmdAddrParityError(PSYSHOST host, UINT8 socket, UINT8 ch);                                    // CORE

/**

  Calculates margins at multiple Vref points

  @param host         - Pointer to sysHost, the system host (root) structure struct
  @param socket       - Processor socket to check
  @param level        - DDR or LRDIMM Host side, LRDIMM Backside, etc...
  @param group        - Parameter to be margined
                         RxDqsDelay    : Margin Rx DqDqs
                         TxDqDelay     : Margin Tx DqDqs
                         RxVref         : Margin Rx Vref
                         TxVref         : Margin Tx Vref
                         RecEnDelay    : Margin Receive Enable
                         WrLvlDelay    : Margin Write Level
                         PcodeMailboxStatus = 0;
                         CmdGrp0        : Margin CMD group 0
                         CmdGrp1        : Margin CMD group 1
                         CmdGrp2        : Margin CMD group 2
                         CmdAll         : Margin all CMD groups
  @param mode         - Test mode to use
                         MODE_XTALK            BIT0: Enable crosstalk (placeholder)
                         MODE_VIC_AGG          BIT1: Enable victim/aggressor
                         MODE_START_ZERO       BIT2: Start at margin 0
                         MODE_POS_ONLY         BIT3: Margin only the positive side
                         MODE_NEG_ONLY         BIT4: Margin only the negative side
                         MODE_DATA_MPR         BIT5: Enable MPR Data pattern
                         MODE_DATA_FIXED       BIT6: Enable Fixed Data pattern
                         MODE_DATA_LFSR        BIT7: Enable Data LFSR
                         MODE_ADDR_LFSR        BIT8: Enable Address LFSR
                         MODE_ADDR_CMD0_SETUP  BIT9
                         MODE_CHECK_PARITY     BIT10: Enable parity checking
                         MODE_DESELECT         BIT11: Enable deselect patterns
                         MODE_VA_DESELECT      BIT12: Enable Victim - Aggressor deselect patterns
  @param scope        - Margin granularity
                         SCOPE_SOCKET      0: Margin per processor socket
                         SCOPE_CH        1: Margin per channel
                         SCOPE_DIMM      2: Margin per DIMM
                         SCOPE_RANK      3: Margin per rank
                         SCOPE_STROBE    4: Margin per strobe group
                         SCOPE_BIT       5: Margin per bit
  @param filter        - Mask of bits to exclude from testing
  @param results       - Pointer to the structure to store the margin results. The structure type varies based on scope
  @param patternLength - RankMarginTest Pattern length
  @param chMask        - Bit Mask of channels to not be used
  @param rankMask      - Bit Mask of ranks to not be used
  @param stepSize      - Step size
  @param numPoints     - Number of points to test

  @retval None

**/
void   GetMultiVref(PSYSHOST host, UINT8 socket, GSM_LT level, GSM_GT group, UINT16 mode, UINT8 scope, struct bitMask  *filter, void *results,
         UINT32 patternLength, UINT8 chMask, UINT8 rankMask, UINT8 stepSize, UINT8 numPoints);   // CORE

/**

  Evaluate test results for one strobe group

  @param host          - Pointer to sysHost, the system host (root) structure struct
  @param socket        - Processor socket to check
  @param ch            - Current Channel
  @param dimm          - Dimm number (0-based)
  @param rank          - Rank number (0-based)
  @param strobe        - Current Strobe
  @param bitMask       - Bit Mask of currently found passes
  @param status        - Status
  @param offset        - Pointer to offsets
  @param sign          - The direction we are currently going (up, down, left or right)
  @param searchUp      - Direction to search
  @param foundPass     - Bit mask marking passes
  @param foundFail     - Bit mask marking fails
  @param marginData    - pointer to Margin data
  @param scope         - Margin granularity
  @param mode          - Test mode to use

  @retval None

**/
void   EvaluateStrobeStatus (PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, UINT8 rank, UINT8 strobe, UINT8 bitMask,
     UINT32 status, INT16 offset, UINT8 sign, UINT8 searchUp, UINT8 foundPass[MAX_CH][MAX_RANK_CH][MAX_STROBE/2],
     UINT8 foundFail[MAX_CH][MAX_RANK_CH][MAX_STROBE/2], void *marginData, UINT8 scope, UINT16 mode);     // CORE only

// Emulation Polling Loops
#define LabelPrint(){                                                         \
if (host->var.common.CurrentReset == POST_RESET_POWERGOOD)                    \
  rcPrintf ((host, "\nLABEL_c_mbox%d:\n", host->var.common.LoopBackNumber));    \
else                                                                          \
  rcPrintf ((host, "\nLABEL_w_mbox%d:\n", host->var.common.LoopBackNumber));    \
}

#define MaskPrint(tempLabelCount) {                                           \
rcPrintf ((host, "and eax, 0%08xh\n", mask));                                   \
if (host->var.common.CurrentReset == POST_RESET_POWERGOOD)                    \
  rcPrintf ((host, "jz LABEL_c_mbox%d\n", tempLabelCount));                 \
else                                                                          \
  rcPrintf ((host, "jz LABEL_w_mbox%d\n", tempLabelCount));                 \
}

#define ReadByteAndShift { \
  rcPrintf ((host, "mov bl, BYTE PTR ds:[0%08xh]\n", RegAddr)); \
  rcPrintf ((host, "shl ecx, 8\n")); \
  rcPrintf ((host, "or cl, bl\n")); \
}

void    JedecInitDdrChannel(PSYSHOST host, UINT8 socket, UINT8 ch);

UINT8
CheckS3Jedec(
PSYSHOST host,
UINT8 socket,
UINT8 ch,
UINT8 dimm
);

/**

  Routine Description: EmulationPoll is an assembly polling loop for reading CSRs
  EmulationPoll allows you to poll multiple bits

  @param host    - Pointer to sysHost, the system host (root) structure struct
  @param socket  - Socket Id
  @param BoxInst - Box Instance, 0 based
  @param Offset  - Register offset; values come from the auto generated header file
  @param mask    - Bit mask of bits we are polling
  @param logic   - Polling for 0 or 1

  @retval None

**/
void EmulationPoll (PSYSHOST host, UINT8 socket, UINT8 BoxInst, UINT32 Offset, UINT32 mask, UINT8 logic);

// Chip/OEM API via Core

/**

  Wrapper code to call the OemInitializePlatformData routine

  @param host - Pointer to sysHost, the system host (root) structure struct

  @retval None

**/
VOID OemInitializePlatformDataCore (PSYSHOST host);

/**

  Stubbed function

  @param host          - Pointer to sysHost, the system host (root) structure struct
  @param UncoreStatus  -

  @retval None

**/
VOID OemPostUncoreInitCore (PSYSHOST host, UINT32 UncoreStatus);
#if defined(ME_SUPPORT_FLAG) && ME_SUPPORT_FLAG
  //
  //  Call ME BIOS hook before triggering a reset
  //

/**

  Wrapper code to call MEBIOSCheckAndHandleResetRequests routine

  @param host - Pointer to sysHost, the system host (root) structure struct

  @retval None

**/
VOID MEBIOSCheckAndHandleResetRequestsCore(PSYSHOST host);
#endif // ME_SUPPORT_FLAG

/**

  Inphi A1 WA

  @param host   - Pointer to sysHost, the system host (root) structure struct
  @param socket - Socket Id
  @param ch     - Channel number (0-based)
  @param dimm   - Current dimm

  @retval None

**/
void InphiPIWA(PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm);
#endif // _memfunc_h
