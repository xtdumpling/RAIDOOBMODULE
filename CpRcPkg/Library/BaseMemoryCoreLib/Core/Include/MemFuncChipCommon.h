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
  Copyright 2006 - 2016 Intel Corporation All Rights Reserved.

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
  MemFuncChipCommon.h

@brief
  This file contains memory detection and initialization for
  IMC and DDR3 modules compliant with JEDEC specification.

**/
#ifndef  _memfuncchipcommon_h
#define  _memfuncchipcommon_h
#ifndef ASM_INC

#include "DataTypes.h"
#include "SysHost.h"
#include "Cpgc.h"

typedef struct {
  UINT32              (*mrcTask) (PSYSHOST host); ///< Ptr to function to execute, with parameter list.
  UINT8               postCodeMajor;      ///< BIOS post code output to the debug port if value <> 0.
  UINT8               postCodeMinor;      ///< BIOS post code output to the debug port if value <> 0.
  UINT8               policyFlag;        ///< Call table flags
  UINT8               mpFlag;
#ifdef SERIAL_DBG_MSG
  char                *debug_string_ptr;  ///< Output string describing this task (potentially output to debug serial port).
#endif //MRC_DEBUG_PRINT
} CallTableEntryType, **PPCallTableEntryType;

#ifdef SERIAL_DBG_MSG
#define CALL_TABLE_STRING(x) x
#else
#define CALL_TABLE_STRING(x)
#endif



// Function declarations

//
// Chip layer only Prototypes. Functions defined and used only in the chip layer.
//

// InitMemChip.c

//
//

/**

  RAS hook for validating Current config (CCUR)

  @param host - Pointer to sysHost, the system host (root) structure struct

  @retval UINT32

**/
UINT32 ValidateCurrentConfigOemHook(PSYSHOST host);

// Address mapping
// MemAddrMap.c - Chip

//
/**

  Get Start Address routed in this iMC, copy value in host structure

  @param host   - Pointer to sysHost, the system host (root) structure struct
  @param socket - Socket Id
  @param mcId   - Memory controller ID

  @retval None

**/
void   GetMcBaseAddr(PSYSHOST host, UINT8 socket, UINT8 mcId);                    // CHIP
//

// MemSPDChip.c

//
/**

  Collects data from SPD for DDR4

  @param host - Pointer to sysHost, the system host (root) structure struct

  @retval SUCCESS

**/
UINT32 GatherSpdDataStage2(PSYSHOST host);                                                              // CALLTAB
//
// Memory Commands
// MemIOControl.c

#define NOT_CACHED  0
#define CACHED      1

/**



  @param host    - Pointer to sysHost, the system host (root) structure struct
  @param socket  - Socket Id
  @param ch      - Channel number (0-based)
  @param dimm    - Current dimm
  @param rank    - Rank number (0-based)
  @param strobe  - Strobe number (0-based)
  @param bit     - Bit number
  @param level   - CSM_LT - Level to program (DDR, VMSE, Buffer)
  @param group   - CSM_GT - Parameter to program
  @param mode    - 0 - do not read from cache, 1 - read from cache, 2 - use cache to program register (used for fast boot or S3)
  @param value   - Data to program

  @retval SUCCESS

**/
MRC_STATUS GetSetTxVref(PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, UINT8 rank, UINT8 strobe, UINT8 bit, GSM_LT level,
                        GSM_GT group, UINT8 mode, INT16 *value);                                                               // CHIP

/**

  Find and return the largest RCVEN logic delay of all strobes
  for all the ranks on this channel.

  @param host    - Pointer to sysHost, the system host (root) structure struct
  @param socket  - Socket Id
  @param ch      - Channel number (0-based)

  @retval 0 - if All RCVEN pi delays are 0.
  @retval Other - The largest RCVEN pi delay in QCLKs rounded up to next QCLK.

**/
UINT8  GetMaxRcvenLogicDelayChannel (PSYSHOST host, UINT8 socket, UINT8 ch);

// MemThrot.c

// MemRAS.c

/**

  Configure Lockstep mode

  @param host    - Pointer to sysHost, the system host (root) structure struct
  @param socket  - Socket Id

  @retval None

**/
void   SetLockstepMode (struct sysHost *host, UINT8 socket);                    // CHIP

/**

  Configure on the fly burst mode for Static Virtual Lockstep mode

  @param host    - Pointer to sysHost, the system host (root) structure struct
  @param socket  - Socket Id

  @retval None

**/
void   SetLockstepModeLate (struct sysHost *host, UINT8 socket);                // CHIP

// MemXoverCalib.c

// MemTrainingChip.c

// CMD/CTL/CLK Access
// MemCmdControl.c


// Timing
// MemTimingChip.c


/**

  Adjust write latency to move DQ/DQS relative to CMD

  @param host        - Pointer to sysHost, the system host (root) structure
  @param socket      - Socket number
  @param ch          - Channel number
  @param offset      - DQ/DQS to CMD offset
  @param updateCache - Indicates if the affected variables in NVRAM structure should be updated

  @retval None

**/
void   WrLvlDqsToCmdDelay (PSYSHOST host, UINT8 socket, UINT8 ch, INT16 offset, BOOLEAN updateCache); // CHIP

// MemCpg.c


// MemProjectSpecific.c

/**

  There is only one set of roundtrip registers per channel.
  This routine ensures all ranks support the same roundtrip values.

  @param host   - Pointer to sysHost, the system host (root) structure
  @param socket - Socket number

  @retval None

**/
void SetCommonRoundtrip (PSYSHOST host, UINT8 socket);                                              // CHIP

/**

  Exectues early control/clock training with Loopback mode

  @param host - Pointer to sysHost, the system host (root) structure

  @retval MRC_STATUS_SUCCESS
  @retval MRC_STATUS_SIGNAL_NOT_SUPPORTED

**/
UINT32 EarlyCtlClkLoopback(PSYSHOST host);                                                          // CHIP

// FnvAccess.c


// MemPciAccess.c

/**

  Reads CPU Memory Controller configuration space using the MMIO mechanism

  @param host      - Pointer to sysHost, the system host (root) structure
  @param socket    - Socket number
  @param ch        - Channel number (0-based)
  @param Offset    - Register offset; values come from the auto generated header file
  @param chOffset  - Channel offset

  @retval Register value

**/
UINT32 MemReadPciCfgMC_Ch  (PSYSHOST host, UINT8 socket, UINT8 ch, UINT32 Offset, UINT32 chOffset); // CHIP

/**

  Writes CPU Memory Controller configuration space using the MMIO mechanism

  @param host     - Pointer to sysHost, the system host (root) structure
  @param socket   - Socket number
  @param ch       - Channel number (0-based)
  @param Offset   - Register offset; values come from the auto generated header file
  @param chOffset - Channel offset
  @param Data     - Register data to be written

  @retval None

**/
void   MemWritePciCfgMC_Ch (PSYSHOST host, UINT8 socket, UINT8 ch, UINT32 Offset, UINT32 chOffset, UINT32 Data); // CHIP



#endif // ASM_INC

#endif   //  _memfuncchipcommon_h
