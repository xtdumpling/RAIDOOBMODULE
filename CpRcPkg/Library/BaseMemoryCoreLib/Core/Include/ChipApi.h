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
  Copyright (c) 2006 - 2016, Intel Corporation All Rights Reserved.

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
  ChipApi.h

@brief
   This file contains API declarations defined in the MRC Chip layer.

 ************************************************************************/
#ifndef  _CHIP_API_H_
#define  _CHIP_API_H_

#include "DataTypes.h"
#include "SysHost.h"
#include <UsraAccessType.h>

#include "MemHostChipCommon.h"
#include "MrcCommonTypes.h"
#include "Cpgc.h"
#include "MemApiSkx.h"
#include "BiosSsaChipFunc.h"

/**
  Accquire or Release the global semaphore
  Called by the BSP of a socket
  Used for ensuring exclusive access to resoruces among CPU sockets \n
       IMPORTANT: \n
           This function must be called in the sequence below: \n
                ...... \n
                AcquireReleaseGlobalSemaphore (host, SYSTEMSEMAPHORE0, ACQUIRE);   // Acquire ownership of semaphore \n
                do_something();                                                    //  Perform intended task \n
           AcquireReleaseGlobalSemaphore (host, SYSTEMSEMAPHORE0, RELEASE);    //  Release ownership of semaphore \n
                ...... \n

  @param host               - Pointer to sysHost, the system host (root) structure
  @param SysSemaphoreNum    - SYSTEMSEMAPHORE register number (0 or 1)
  @param OpCode             - 1/0   ACQUIRE_SEMAPHORE / RELEASE_SEMAPHORE

  @retval  None

**/
void
AcquireReleaseGlobalSemaphore (
   PSYSHOST  host,                              // Pointer to the system host structure
   UINT8     SysSemaphoreNum,                   // SYSTEMSEMAPHORE register number (0 or 1)
   UINT8     OpCode                             // 1 = ACQUIRE_SEMAPHORE
                                                // 0 = RELEASE_SEMAPHORE
   );

/*++

  Checks the serial debug message level

  @param host  - Pointer to sysHost, the system host (root) structure
  @param level - Debug message level to check

  @retval 0 - Message level is not set
  @retval 1 - Message level is set

--*/
UINT8
checkMsgLevel (
  PSYSHOST   host,                              // Pointer to sysHost, the system host (root) structure
  UINT32     level                              // Debug message level to check
  );

/*++

  Disables serial debug messages

  @param host  - Pointer to sysHost, the system host (root) structure

  @retval None

--*/
void
disableAllMsg (
  PSYSHOST    host                              // Pointer to sysHost, the system host (root) structure
  );

/*++

  Restores serial debug messages

  @param host  - Pointer to sysHost, the system host (root) structure

  @retval None

--*/
void
restoreMsg (
  PSYSHOST    host                              // Pointer to sysHost, the system host (root) structure
  );

/*++

Routine Description:

  MRC specific print to serial output

  @param host      - Pointer to sysHost, the system host (root) structure
  @param dbgLevel  - debug level
  @param socket    - Processor socket number
  @param ch        - Channel number
  @param dimm      - DIMM number
  @param rank      - Rank
  @param strobe    - Strobe
  @param bit       - Bit
  @param Format    - string format

  @retval None

--*/
void
debugPrintMem (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT32      dbgLevel,                         // Minimum debug level in order to print
  UINT8       socket,                           // Processor socket number
  UINT8       ch,                               // Channel number
  UINT8       dimm,                             // DIMM number
  UINT8       rank,                             // Rank
  UINT8       strobe,                           // Strobe
  UINT8       bit,                              // Bit
  char*       Format,                           // Printing formatting
  ...                                           // list of variables to print based on formatting
  );

/*++

Routine Description:

  MRC specific print to serial output

  @param host      - Pointer to sysHost, the system host (root) structure
  @param dbgLevel  - debug level
  @param socket    - Processor socket number
  @param ch        - Channel number
  @param dimm      - DIMM number
  @param rank      - Rank
  @param strobe    - Strobe
  @param bit       - Bit
  @param Format    - string format
  @param Marker    - va_list that points to the arguments for Format that are on the stack

  @retval None

--*/
void
debugPrintMemFunction (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT32      dbgLevel,                         // Minimum debug level in order to print
  UINT8       socket,                           // Processor socket number
  UINT8       ch,                               // Channel number
  UINT8       dimm,                             // DIMM number
  UINT8       rank,                             // Rank
  UINT8       strobe,                           // Strobe
  UINT8       bit,                              // Bit
  char*       Format,                           // Printing formatting
  va_list     Marker                            // va_list that points to the arguments for Format that are on the stack
  );

/**

  Outputs error code hand halts if halt on fatal error, else exits reference code

  @param host      - Pointer to sysHost, the system host (root) structure
  @param majorCode - Major error code to log
  @param minorCode - Minor error code to log

  @retval None

**/
void
FatalError (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       majorCode,                        // Major error code
  UINT8       minorCode                         // Minor error code
  );

/**

  Writes the given command to BIOS to PCU Mailbox Interface CSR register

  @param host         - Pointer to sysHost, the system host (root) structure
  @param socket       - Processor socket
  @param dwordCommand - Pcu mailbox command to write
  @param dworddata    - Pcu mailbox data

  @retval error code from the Pcu mailbox (0 = NO ERROR)

**/
UINT32
WriteBios2PcuMailboxCommand (
  struct sysHost  *host,                        // Pointer to sysHost, the system host (root) structure structure on stack
  UINT8       socket,                           // CPU Socket Node number (Socket ID)
  UINT32      dwordCommand,                     // Pcu mailbox command to write
  UINT32      dworddata                         // Pcu mailbox data
  );

/**
  This Lib return PCIE MMCFG Base Address 

  @param  Address: A pointer of the address of the Common Address Structure for PCIE type.
  @param  Buffer: A pointer of buffer for the value read from platform.

  @retval <>NULL The function completed successfully.
  @retval NULL Returen Error
**/

UINTN
EFIAPI
GetPcieSegMmcfgBaseAddress (
  IN USRA_ADDRESS    *Address
  );

/**

  Reads PCI configuration space using the MMIO mechanism

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - CPU Socket Node number (Socket ID)
  @param reg     - input parameter "reg" uses the format in the Bus_Dev_Func_CFG.H files

  @retval Value in requested reg

**/
UINT32
ReadPciCfg (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // CPU Socket Node number (Socket ID)
  UINT32      reg                               // input parameter "reg" uses the format in the Bus_Dev_Func_CFG.H files
  );

/**

  Writes specified data to PCI configuration space using the MMIO mechanism

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - CPU Socket Node number (Socket ID)
  @param reg     - input parameter "reg" uses the format in the Bus_Dev_Func_CFG.H files
  @param data    - Value to write

  @retval None

**/
void
WritePciCfg (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // CPU Socket Node number (Socket ID)
  UINT32      reg,                              // input parameter "reg" uses the format in the Bus_Dev_Func_CFG.H files
  UINT32      data                              // Value to write
  );

UINT32
GetPciCfgAddress (
  PSYSHOST    host,                             // Pointer to the system host (root) structure
  UINT8       socket,                           // CPU Socket Node number (Socket ID)
  UINT32      reg                               // input parameter "reg" uses the format in the Bus_Dev_Func_CFG.H files
  );

/**

  Reads CPU Uncore & IIO PCI configuration space using the MMIO mechanism

  @param host      - Pointer to sysHost, the system host (root) structure
  @param SocId     - CPU Socket Node number (Socket ID)
  @param BoxInst   - Box Instance, 0 based
  @param Offset    - Offset values come from the auto generated header file

  @retval Register value

**/
UINT32
ReadCpuPciCfgEx (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       SocId,                            // CPU Socket Node number (Socket ID)
  UINT8       BoxInst,                          // Box Instance, 0 based
  UINT32      Offset                            // Register offset; values come from the auto generated header file
  );

/**

  Writes CPU Uncore & IIO PCI configuration space using the MMIO mechanism

  @param host      - Pointer to sysHost, the system host (root) structure
  @param SocId     - CPU Socket Node number (Socket ID)
  @param BoxInst   - Box Instance, 0 based
  @param Offset    - Register offset; values come from the auto generated header file
  @param Data      - Register data to be written

  @retval None

**/
void
WriteCpuPciCfgEx (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       SocId,                            // CPU Socket Node number (Socket ID)
  UINT8       BoxInst,                          // Box Instance, 0 based
  UINT32      Offset,                           // Register offset; values come from the auto generated header file
  UINT32      Data                              // Register data to be written
  );

UINTN
GetCpuPciCfgExAddr(
  PSYSHOST    host,                             // Pointer to the system host (root) structure
  UINT8       SocId,                            // CPU Socket Node number (Socket ID)
  UINT8       BoxInst,                          // Box Instance, 0 based
  UINT32      Offset                            // Register offset; values come from the auto generated header file
  );

/**
  Test if the system is 2HA

  @param host    - Pointer to the system host (root) structure

  @retval TRUE if 2HA
  @retval FALSE if not 2HA

**/
UINT8
Is2HA (
  PSYSHOST    host                              // Pointer to sysHost, the system host (root) structure
  );


/**

  Chip specific code path to get CPU socket id for the system boot strap processor

  @param host - Pointer to sysHost, the system host (root) structure

  @retval (UINT8)Socket ID for the SBSP

**/
UINT8
GetSbspSktId(
    PSYSHOST  host                              // Pointer to sysHost, the system host (root) structure
    );

/**

  Chip specific code path to halt the system

  @param haltOnError - To halt or not on error

  @retval None

**/
VOID RcDeadLoop (
  UINT8 haltOnError                             // To halt or not on error
  );

#ifdef HBM_SUPPORT
//
// HBM related functions
//

/**

  Chip specific code path to Check for HBM support

  @param host - Pointer to sysHost, the system host (root) structure

  @retval TRUE if supported
  @retval FALSE if not supported

**/
UINT32
IsHbmSupported (
  PSYSHOST host                              // Pointer to sysHost, the system host (root) structure
  );

/**

  Chip specific code path to Check for HBM S3 support

  @param host - Pointer to sysHost, the system host (root) structure

  @retval TRUE if supported
  @retval FALSE if not supported

**/
UINT32
IsHbmS3Supported (
  PSYSHOST host                              // Pointer to sysHost, the system host (root) structure
  );

/**

  Initialize internal chip data structures

  @param host   - Pointer to sysHost, the system host (root) structure
  @param socket - Socket Id

  @retval None

**/
VOID
HbmInitChip (
  PSYSHOST host,                             // Pointer to sysHost, the system host (root) structure
  UINT8  socket                              // Socket Id
  );

/**

  Initialize channel specific registers after training and before normal mode

  @param host - Pointer to sysHost, the system host (root) structure

  @retval None

**/
VOID
HbmLateConfig (
  PSYSHOST host                              // Pointer to sysHost, the system host (root) structure
  );


/**

  Programs input parameters for the HBM MRC

  @param host - Pointer to the host structure

  @retval N/A

**/
UINT32
HbmFsmTrainingSetup  (
  PSYSHOST host                              // Pointer to sysHost, the system host (root) structure
  );

/**

  This deasserts resets all HBM MC IO modules

  @param host      - Pointer to sysHost, the system host (root) structure
  @param socket    - Socket Id
  @param ioModule  - HBM IO Module number (0-based)
  @param set       - Select to set or clear reset in HBM

  @retval None

**/
VOID
SetClearHbmReset (
  PSYSHOST host,                             // Pointer to sysHost, the system host (root) structure
  UINT8    socket,                           // Socket Id
  UINT8    ioModule,                         // HBM IO Module number (0-based)
  BOOLEAN  set                               // Select to set or clear reset in HBM
  );

/*++

  This toggles Cke ctrl in all HBM MC IO channels

  @param host      - Pointer to sysHost
  @param socket    - Socket Id
  @param ioModule  - HBM IO Module number (0-based)
  @param set       - Assert/deasert (Boolean)

  @retval N/A

--*/

VOID
SetClearHbmCke (
  PSYSHOST host,
  UINT8    socket,
  UINT8    ioModule,
  BOOLEAN  set
  );

VOID
HbmIOReset(
  PSYSHOST host,                             // Pointer to sysHost, the system host (root) structure
  UINT8    socket,                           // Socket Id
  UINT8    ioModule                          // HBM IO Module number (0-based)
  );

/*++

This disables HBM memory subSistem and all HBM channels within IO module

@param host    - Pointer to sysHost
@param socket    - Socket Id
@param ioModule  - HBM IO Module number (0-based)

@retval N/A

--*/

VOID
DisableHbmMod(
  PSYSHOST host,                             // Pointer to sysHost, the system host (root) structure
  UINT8    socket,                           // Socket Id
  UINT8    ioModule                          // HBM IO Module number (0-based)
  );

/*++

This disables HBM memory subSistem channel within IO module

@param host    - Pointer to sysHost
@param socket  - Socket Id
@param ch      - Channel number (0-based)

@retval N/A

--*/

VOID
DisableHbmCh(
  PSYSHOST host,
  UINT8    socket,
  UINT8    ch
  );

/*++

This disables Chip specific HBM memory subSistem and all HBM channels
within IO module

@param host    - Pointer to sysHost
@param socket    - Socket Id
@param ioModule  - HBM IO Module number (0-based)

@retval N/A

--*/

VOID
DisableHbmModChip(
  PSYSHOST host,
  UINT8    socket,
  UINT8    ioModule
  );

/*++

This disables Chip specific HBM memory subSistem channel within IO module

@param host    - Pointer to sysHost
@param socket  - Socket Id
@param ch      - Channel number (0-based)

@retval N/A

--*/

VOID
DisableHbmChChip(
  PSYSHOST host,
  UINT8    socket,
  UINT8    ch
  );

/**

  Writes HBM mode registers using IEEE1500

  @param host     - Pointer to sysHost, the system host (root) structure
  @param socket   - Socket Id
  @param ioModule - HBM IO Module number (0-based)
  @param mrs      - MRS register number (0-based)
  @param data     - Data for the MRS write

  @retval None

**/
VOID
WriteMrs1500 (
  PSYSHOST host,                             // Pointer to sysHost, the system host (root) structure
  UINT8    socket,                           // Socket Id
  UINT8    ioModule,                         // HBM IO Module number (0-based)
  UINT8    mrs,                              // MRS register number (0-based)
  UINT8    data                              // Data for the MRS write
  );


/**

Initialize  DWORD Rx IO via FSM mechanism.

@param host   - Pointer to sysHost
@param socket - processor socket ID

@retval MRC_STATUS

**/
UINT32
BasicDwordRxTrainingFSM (
  PSYSHOST host,                          // Pointer to sysHost, the system host (root) structure
  UINT8    socket,                        // Socket Id
  UINT8    ioModule                       // HBM IO Module number (0-based)
  );

/**

  Enable FSM HBM using IEEE1500

  @param host      - Pointer to sysHost, the system host (root) structure
  @param socket    - Socket Id
  @param ioModule  - HBM IO Module number (0-based)
  @param step      - Step of FSM to run

  @retval None

**/
VOID
HbmFsm(
  PSYSHOST host,                             // Pointer to sysHost, the system host (root) structure
  UINT8    socket,                           // Socket Id
  UINT8    ioModule,                         // HBM IO Module number (0-based)
  UINT8    step                              // Step of FSM to run
  );

/**

  Check HBM FSM results using IEEE1500

  @param host      - Pointer to sysHost, the system host (root) structure
  @param socket    - Socket Id
  @param ioModule  - HBM IO Module number (0-based)
  @param step      - Step of FSM to run

  @retval (UINT32) status

**/
UINT32
CheckHbmFsmResult (
  PSYSHOST host,                             // Pointer to sysHost, the system host (root) structure
  UINT8    socket,                           // Socket Id
  UINT8    ioModule,                         // HBM IO Module number (0-based)
  UINT8    step                              // Step of FSM to run
);

/**

  HBM FSM Pi results using IEEE1500

  @param host          - Pointer to sysHost
  @param socket        - Socket Id
  @param ioModule      - ioModule number (0-based)
  @param step          - Step of FSM to run

  @retval N/A

**/

VOID
HbmFsmPiResult (
  PSYSHOST host,         // Pointer to the system host (root) structure
  UINT8    socket,       // Socket Id 
  UINT8    ioModule,     // HBM IO Module number (0-based)
  UINT8    step          // Step of FSM to run
);

/**

  Set Pseudo Channel Mask for FSM HBM 

  @param host          - Pointer to sysHost
  @param socket        - Socket Id
  @param ioModule      - ioModule number (0-based)
  @param pseudo        - even or odd pseudo ch number (0-based)


  @retval N/A

**/
void
HbmFsmPseudoChMask (
  PSYSHOST host,         // Pointer to the system host (root) structure
  UINT8    socket,       // Socket Id 
  UINT8    ioModule,     // HBM IO Module number (0-based)
  UINT8    pseudo        // even or odd pseudo ch number (0-based)
);

/**

  Generate HBM commands IEEE1500

  @param host      - Pointer to sysHost, the system host (root) structure
  @param socket    - Socket Id
  @param ioModule  - HBM IO Module number (0-based)
  @param ch        - Channel number (0-based)
  @param cmd       - Command to IEEE1500
  @param size      - Size of data to Rd/Wr to/from IEEE1500
  @param data      - Data return path

  @retval None

**/
VOID
Hbm1500PortCmd (
  PSYSHOST host,                             // Pointer to sysHost, the system host (root) structure
  UINT8    socket,                           // Socket Id
  UINT8    ioModule,                         // HBM IO Module number (0-based)
  UINT8    ch,                               // Channel number (0-based)
  UINT8    cmd,                              // Command to IEEE1500
  UINT8    size,                             // Size of data to Rd/Wr to/from IEEE1500
  UINT8    *data                             // Data return path
  );

/**

Get HBM stack routing per channel

@param host          - Pointer to sysHost
@param socket        - Socket Id
@param ioModule      - ioModule number (0-based)

@retval N/A

**/

VOID
HbmModRoutingPoll (
  PSYSHOST host,         // Pointer to the system host (root) structure
  UINT8    socket,       // Socket Id 
  UINT8    ioModule      // HBM IO Module number (0-based)
  );

/**

  Check HBM supported configurations

  @param host - Pointer to sysHost, the system host (root) structure

  @retval SUCCESS

**/
UINT32
HbmCheckPOR(
  PSYSHOST host                              // Pointer to sysHost, the system host (root) structure
  );

//
// HBM GetSet API functions
//

/**

  Programs HBM IO delays

  @param host        - Pointer to the system host (root) structure
  @param level       - IO level to access
  @param groupLimits - Parameter limit

  @retval MRC_STATUS

**/
MRC_STATUS
GetDataHbmGroupLimits (
  PSYSHOST  host,          // Pointer to the system host (root) structure
  GSM_LT    level,         // IO level to access
  GSM_LIMIT *groupLimits   // Parameter limit
);

/**

  Get or Set RxDelay for HBM

  @param host    - Pointer to the system host (root) structure
  @param socket  - Socket Id
  @param ch      - Channel number (0-based)
  @param strobe  - Dqs data group within the rank
  @param level   - IO level to access
  @param group   - Data group to access
  @param mode    - 0 - do not read from cache, 1 - read from cache, 2 - use cache to\n
                     program register (used for fast boot or S3)
  @param value   - Pointer to absolute value or offset based on mode

  @retval MRC_STATUS

**/
MRC_STATUS
GetSetRxDelayHbm (
  PSYSHOST  host,        // Pointer to the system host (root) structure
  UINT8     socket,      // Socket Id
  UINT8     ch,          // Channel number (0-based)
  UINT8     strobe,      // Dqs data group within the rank
  GSM_LT    level,       // IO level to access
  GSM_GT    group,       // Data group to access
  UINT8     mode,        // 0 - do not read from cache, 1 - read from cache, 2 - use cache to
                         //   program register (used for fast boot or S3)
  INT16     *value       // Pointer to absolute value or offset based on mode
 );

MRC_STATUS
GetDataHbmGroupLimits (
  PSYSHOST  host,        // Pointer to the system host (root) structure
  GSM_LT    level,       // IO level to access
  GSM_LIMIT *groupLimits   // Parameter limit
);

MRC_STATUS
GetCmdHbmGroupLimits (
  PSYSHOST  host,        // Pointer to the system host (root) structure
  GSM_LT    level,       // IO level to access
  GSM_LIMIT *groupLimits   // Parameter limit
);


MRC_STATUS
GetSetRxDelayHbm (
  PSYSHOST  host,        // Pointer to the system host (root) structure
  UINT8     socket,      // Socket Id
  UINT8     ch,          // Channel number (0-based)
  UINT8     strobe,      // Dqs data group within the rank
  GSM_LT    level,       // IO level to access
  GSM_GT    group,       // Data group to access
  UINT8     mode,        // 0 - do not read from cache, 1 - read from cache, 2 - use cache to
                         //   program register (used for fast boot or S3)
  INT16     *value       // Pointer to absolute value or offset based on mode
 );

MRC_STATUS
GetSetRxVrefHbm (
  PSYSHOST  host,        // Pointer to the system host (root) structure
  UINT8     socket,      // Socket Id
  UINT8     ch,          // Channel number (0-based)
  UINT8     strobe,      // Dqs data group within the rank
  GSM_LT    level,       // IO level to access
  GSM_GT    group,       // Data group to access
  UINT8     mode,        // 0 - do not read from cache, 1 - read from cache, 2 - use cache to
                         //   program register (used for fast boot or S3)
  INT16     *value       // Pointer to absolute value or offset based on mode
 );

MRC_STATUS
GetSetRndTrpDlyHbm (
  PSYSHOST  host,        // Pointer to the system host (root) structure
  UINT8     socket,      // Socket Id
  UINT8     ch,          // Channel number (0-based)
  UINT8     strobe,      // Dqs data group within the rank
  GSM_LT    level,       // IO level to access
  GSM_GT    group,       // Data group to access
  UINT8     mode,        // 0 - do not read from cache, 1 - read from cache, 2 - use cache to
                         //   program register (used for fast boot or S3)
  INT16     *value       // Pointer to absolute value or offset based on mode
 );

MRC_STATUS
GetSetTxDsHbm (
  PSYSHOST  host,        // Pointer to the system host (root) structure
  UINT8     socket,      // Socket Id
  UINT8     ch,          // Channel number (0-based)
  UINT8     strobe,      // Dqs data group within the rank
  GSM_LT    level,       // IO level to access
  GSM_GT    group,       // Data group to access
  UINT8     mode,        // 0 - do not read from cache, 1 - read from cache, 2 - use cache to
                         //   program register (used for fast boot or S3)
  INT16     *value       // Pointer to absolute value or offset based on mode
 );

MRC_STATUS
GetSetCmdDsHbm (
  PSYSHOST  host,        // Pointer to the system host (root) structure
  UINT8     socket,      // Socket Id
  UINT8     ch,          // Channel number (0-based)
  UINT8     strobe,      // Dqs data group within the rank
  GSM_LT    level,       // IO level to access
  GSM_GT    group,       // Data group to access
  UINT8     mode,        // 0 - do not read from cache, 1 - read from cache, 2 - use cache to
                         //   program register (used for fast boot or S3)
  INT16     *value       // Pointer to absolute value or offset based on mode
 );

MRC_STATUS
GetSetTcoHbm (
  PSYSHOST  host,        // Pointer to the system host (root) structure
  UINT8     socket,      // Socket Id
  UINT8     ch,          // Channel number (0-based)
  UINT8     strobe,      // Dqs data group within the rank
  GSM_LT    level,       // IO level to access
  GSM_GT    group,       // Data group to access
  UINT8     mode,        // 0 - do not read from cache, 1 - read from cache, 2 - use cache to
                         //   program register (used for fast boot or S3)
  INT16     *value       // Pointer to absolute value or offset based on mode
 );

MRC_STATUS
GetSetCkCmdAllHbm (
  PSYSHOST  host,        // Pointer to the system host (root) structure
  UINT8     socket,      // Socket Id
  UINT8     ch,          // Channel number (0-based)
  UINT8     strobe,      // Dqs data group within the rank
  GSM_LT    level,       // IO level to access
  GSM_GT    group,       // Data group to access
  UINT8     mode,        // 0 - do not read from cache, 1 - read from cache, 2 - use cache to
                         //   program register (used for fast boot or S3)
  INT16     *value       // Pointer to absolute value or offset based on mode
 );

MRC_STATUS
GetSetTxDelayHbm (
  PSYSHOST  host,        // Pointer to the system host (root) structure
  UINT8     socket,      // Socket Id
  UINT8     ch,          // Channel number (0-based)
  UINT8     strobe,      // Dqs data group within the rank
  GSM_LT    level,       // IO level to access
  GSM_GT    group,       // Data group to access
  UINT8     mode,        // 0 - do not read from cache, 1 - read from cache, 2 - use cache to
                         //   program register (used for fast boot or S3)
  INT16     *value       // Pointer to absolute value or offset based on mode
 );

MRC_STATUS
GetSetTxDelayCycleHbm (
  PSYSHOST  host,        // Pointer to the system host (root) structure
  UINT8     socket,      // Socket Id
  UINT8     ch,          // Channel number (0-based)
  UINT8     strobe,      // Dqs data group within the rank
  GSM_LT    level,       // IO level to access
  GSM_GT    group,       // Data group to access
  UINT8     mode,        // 0 - do not read from cache, 1 - read from cache, 2 - use cache to
                         //   program register (used for fast boot or S3)
  INT16     *value       // Pointer to absolute value or offset based on mode
 );

//
// HBM CPGC
//

VOID
HbmProgramCADB (
  PSYSHOST            host,                 // Pointer to the system host (root) structure
  UINT8               socket,               // Socket Id 
  UINT8               ch,                   // DDR channel to setup
  UINT8               startLine,
  UINT8               numLines,
  HbmCmdPatCadbProg0  prog0[CADB_LINES]
);

UINT32
HbmCpgcAdvTrainingErrorStatus (
  PSYSHOST host,                            // Pointer to the system host (root) structure
  UINT8    socket,                          // Socket Id 
  UINT32   hbmChEnMap,                      // 32-bit bit mask of HBM channels to stop
  void     *HbmCpgcErrorStatus[MAX_HBM_CH]  // error status for each channel
 );

void
HbmSetBwErrorMask(
  PSYSHOST   host,    // Pointer to the system host (root) structure
  UINT8      socket,  // Socket Id 
  void       *mask    // Bits to be masked
);

#endif // HBM_SUPPORT
/*++
  Get the maximum number of data groups based on DIMM and group type

  @param host  - Pointer to sysHost
  @param socket  - Socket
  @param level - Level type

  @retval Max channels

--*/
UINT8
GetMaxCh(
  PSYSHOST  host,    // Pointer to host structure (outside scope of this spec)
  UINT8     socket,  // Socket
  GSM_LT    level    // Level type
);

/**

  Reads CPU Memory Controller configuration space using the MMIO mechanism

  @param host          - Pointer to sysHost, the system host (root) structure
  @param socket        - Socket number
  @param ChIdOrBoxInst - DDR channel Id within a memory controller
                         or Box Instance (in case of non-MC boxes), 0 based, 0xFF if not used
  @param Offset        - Register offset; values come from the auto generated header file

  @retval Register value

**/
UINT32
MemReadPciCfgEp (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // CPU Socket Node number (Socket ID)
  UINT8       ChIdOrBoxInst,                    // DDR/VMSE channel Id within a memory controller
                                                //   or Box Instance (in case of non-MC boxes), 0 based, 0xFF if not used
  UINT32      Offset                            // Register offset; values come from the auto generated header file
  );

/**

  Writes CPU Memory Controller configuration space using the MMIO mechanism

  @param host          - Pointer to sysHost, the system host (root) structure
  @param socket        - Socket number
  @param ChIdOrBoxInst - DDR channel Id within a memory controller
                         or Box Instance (in case of non-MC boxes), 0 based, 0xFF if not used
  @param Offset        - Register offset; values come from the auto generated header file
  @param Data          - Register data to be written

  @retval None

**/
void
MemWritePciCfgEp (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // CPU Socket Node number (Socket ID)
  UINT8       ChIdOrBoxInst,                    // DDR/VMSE channel Id within a memory controller
                                                //   or Box Instance (in case of non-MC boxes), 0 based, 0xFF if not used
  UINT32      Offset,                           // Register offset; values come from the auto generated header file
  UINT32      Data                              // Register data to be written
  );

/**

  Reads CPU Memory Controller configuration space using the MMIO mechanism

  @param host          - Pointer to sysHost, the system host (root) structure
  @param socket        - Socket number
  @param mcId          - Memory controller ID
  @param Offset        - Register offset; values come from the auto generated header file

  @retval Register value

**/
UINT32
MemReadPciCfgMC (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // CPU Socket Node number (Socket ID)
  UINT8       mcId,                             // Memory controller ID
  UINT32      Offset                            // Register offset; values come from the auto generated header file
  );

/**

  Writes CPU Memory Controller configuration space using the MMIO mechanism

  @param host          - Pointer to sysHost, the system host (root) structure
  @param socket        - Socket number
  @param mcId          - Memory controller ID
  @param Offset        - Register offset; values come from the auto generated header file
  @param Data          - Register data to be written

  @retval None

**/
void
MemWritePciCfgMC (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // CPU Socket Node number (Socket ID)
  UINT8       mcId,                             // Memory controller ID
  UINT32      Offset,                           // Register offset; values come from the auto generated header file
  UINT32      Data                              // Register data to be written
  );

UINTN
MemGetPciCfgMCAddr (
  PSYSHOST host,                             // Pointer to the system host (root) structure
  UINT8    socket,                           // CPU Socket Node number (Socket ID)
  UINT8    mcId,                             // Memory controller ID
  UINT32   Offset                            // Register offset; values come from the auto generated header file
  );

/**

  Get a Memory channel's EPMC Main value

  @param host      - Pointer to sysHost, the system host (root) structure
  @param socket    - CPU Socket Node number (Socket ID)
  @param regBase   - MMIO Reg address of first base device

  @retval EPMC main value
**/
UINT32
MemReadPciCfgMain (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // CPU Socket Node number (Socket ID)
  UINT32      regBase                           // MMIO Reg address of first base device
  );

/**

  Write a Memory channel's EPMC Main value

  @param host      - Pointer to sysHost, the system host (root) structure
  @param socket    - CPU Socket Node number (Socket ID)
  @param regBase   - MMIO Reg address of first base device
  @param data      - Data to write

  @retval None

**/
void
MemWritePciCfgMain (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // CPU Socket Node number (Socket ID)
  UINT32      regBase,                          // MMIO Reg address of first base device
  UINT32      data                              // Data to write
  );

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

UINT32
IsCustomRefreshRate(
  PSYSHOST host,
  UINT8    socket
);

#ifdef SERIAL_DBG_MSG

/**

Displays the DRAM manufacturer

@param host  - Pointer to sysHost
@param MfgId - DRAM Manufacturer Id

@retval N/A

**/

void   
DisplayManf(
  PSYSHOST host,   // Pointer to sysHost
  UINT16 MfgId     // DRAM Manufacturer Id
);
#endif // SERIAL_DBG_MSG

/**

  Install the Mrc Hooks Chip Layer Services PPI interface

  @param host     - Pointer to sysHost, the system host (root) structure

  @retval 0 - Successfully installed PPI
  @retval non-zero - Failure occurs when installing PPI

**/
UINT32
InstallMrcHooksChipServicesPpi (
  PSYSHOST host
  );

#endif // _CHIP_API_H_
