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
  Copyright 2006 - 2016, Intel Corporation All Rights Reserved.

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
  SysFunc.h

@brief
  This file contains

**/

#ifndef _sysfunc_h
#define _sysfunc_h
#include "DataTypes.h"
#include "SysHost.h"
#include "SysFuncChipCommon.h"
#include "ChipApi.h"
#include "BiosSsaFunc.h"
#ifdef SSA_FLAG
#include "MrcSsaServices.h"
#endif //SSA_FLAG

//************************** CHIP to CORE API Prototypes ***************************************
//
// Printf.c
//

/**

  MRC specific print to serial output

  @param host      - Pointer to sysHost, the system host (root) structure
  @param dbgLevel  - debug level
  @param Format    - string format
  @param ...       - values matching the variables declared in
                     the Format parameter


  @retval None

**/
void   DebugPrintRc (PSYSHOST  host, UINT32 dbgLevel, char* Format, ...);                    // CORE / CHIP

//
// Pipe.c
//

/**

  Sync data between SBSP and other sockets

  @param host - Pointer to sysHost, the system host (root) structure

  @retval Fatal Error flag

**/
UINT32 SyncErrorsTbl(PSYSHOST host);                                                          // CALLTABLE

/**

  Sync data between SBSP and other sockets

  @param host - Pointer to sysHost, the system host (root) structure

  @retval Fatal Error flag

**/
UINT8  SyncErrors(PSYSHOST host);                                                             // CHIP

//
// Decompress.c
//


//
// PmTimer.c
//

/**

  OEM hook for getting the system time from RTC to update System time for Memory devices
  (Format is number of seconds from current date to 1/1/1970 12:00:00

  @retval NumOfSeconds  - Return Number of seconds from Current date to 1/1/1970 12:00:00

**/
UINT32 GetTimeInSecondsFrom1970 (VOID);

#define  NO_TIMEOUT  0xFFFFFFFF

//
// Math.c
//

/**

  Calculates the largest integer that is both
  a power of two and less than or equal to input

  @param Input - value to calculate power of two

  @retval The largest integer that is both a power of two and less than or equal to Input

**/
UINT32 GetPowerOfTwox32 (UINT32 Input);                                                       // CHIP

/**

  Find if a value is a power of 2

  @param Value  - value to be examined

  @retval TRUE - the value is a power of two
  @retval FALSE-  the value is not a power of two

**/
BOOLEAN IsPowerOfTwo (UINT32 Value);                                                          // CHIP

//
// Error.c
//

/**

  Checks MSRs with UC set and logs the register data for further use
  Collect only UNCORE MC Bank information.

  @param host - Pointer to sysHost, the system host (root) structure

  @retval None

**/
void CollectPrevBootFatalErrors(PSYSHOST host);                                               // CHIP

/**

  Checks that boundary is less than arrayBoundary to avoid array out of bounds access

  @param host           - Pointer to sysHost, the system host (root) structure
  @param boundary       - The boundary to check
  @param arrayBoundary  - The boundary of the array

  @retval None

**/
void MemCheckBoundary(PSYSHOST host, UINT32 boundary,UINT32 arrayBoundary);                   // CHIP

/**

  Checks that index is less than arraySize to avoid array out of bounds access

  @param host           - Pointer to sysHost, the system host (root) structure
  @param index          - The index to check
  @param arrayBoundary  - The boundary of the array

  @retval None
**/
void MemCheckIndex(PSYSHOST host, UINT32 index, UINT32 arrayBoundary);                      // CHIP

//
// Crc32.c
//
UINT32 CalculateCrc32(UINT8 *Data, UINTN DataSize, UINT32 *CrcOut);

//************************** CORE to CHIP API Prototypes ***************************************

//
// Smbus.c
//

//
// DebugCmd.c
//

/**

  Selects the serial pipe to be used for debug messages

  @param host - Pointer to sysHost, the system host (root) structure
  @param serialPipeNum - SERIAL_CONSOLE_PIPE for legacy ANSI messages
                       - SERIAL_BSD_PIPE for BIOS Serial Debug API (requires bsd_pipe utility on host system)
                       - SERIAL_THREAD_PIPE for per thread serial messages (requires bsd_pipe utility on host system)
  @retval None

**/
#define SERIAL_CONSOLE_PIPE      0
#define SERIAL_BSD_PIPE          1
#define SERIAL_THREAD_PIPE       2
void SwitchSerialPipe(PSYSHOST host, UINT32 serialPipeNum);

/**

  Selects the color to use for stderr

  @param ansiColorCode - ANSI color code
  @retval None

**/
void SetErrTextColor(UINT8 ansiColorCode);


//
// ProcMemInit.c
//
/**
  SAD target CF8 value for get SBSP.
  @retval SAD target CF8 value

**/
UINT32 SadTargetCf8Chip (void);

/**
  Get SBSP id for legacy remote SBSP
  @param host             -  Pointer to sysHost, the system host (root) structure
  @param legacyPchTarget  -  legacy Pch Target

  @retval legacy remote SBSP

**/
UINT8  LegacyRemoteSBSPChip (PSYSHOST host, UINT32 legacyPchTarget);

//
//************************** CORE only Prototypes *************************************************
//

//
// Pipe.c
//

/**

  Unlocks the command pipe (releases control)

  @param Host - Pointer to sysHost, the system host (root) structure
  @param Id   - PIPE Slave Id

  @retval None

**/
void   UnlockCommandPipe(PSYSHOST Host, UINT8 Id);                                          // CORE

/**

  Lock a specific slave's command pipe

  @param Host - Pointer to sysHost, the system host (root) structure
  @param Id   - PIPE Slave Id

  @retval None

**/
void   LockCommandPipe(PSYSHOST Host, UINT8 Id);                                            // CORE

/**

  Wait for a specific slave's command pipe to be unlocked

  @param Host - Pointer to sysHost, the system host (root) structure
  @param Id   - PIPE Slave Id

  @retval None

**/
void   WaitForCommandPipeUnLocked(PSYSHOST Host, UINT8 Id);                                 // CORE

/**

  Take control of the Command Pipe

  @param Host - Pointer to sysHost, the system host (root) structure
  @param Id   - PIPE Slave Id

  @retval TRUE - Command taken
  @retval FALSE - Command not given

**/
BOOLEAN AcquireCommandPipe(PSYSHOST Host, UINT8 Id);                                        // CORE

/**

  Writes a command to a specific slave's pipe

  @param Host    - Pointer to sysHost, the system host (root) structure
  @param Id      - PIPE Slave Id
  @param CmdPipe - Pointer to a command PIPE data structure

  @retval None

**/
void   WriteCommandPipe(PSYSHOST Host, UINT8 Id, UINT32 *CmdPipe);                          // CORE

/**

  Send a command to the Command PIPE until an ACK is received. This
  function should be called only by a PIPE Master.

  @param Host     - Pointer to sysHost, the system host (root) structure
  @param Id       - PIPE Slave Id
  @param CmdPipe  - Pointer to a command PIPE data structure

  @retval PIPE Status

**/
UINT32 SendCommandPipe(PSYSHOST Host, UINT8 Id, UINT32 *CmdPipe);                           // CORE

/**

  Provide PIPE communication for receiveing package data. Upon this call, this function
  will enter PIPE package-received mode, as a PIPE master.
  Calling this function when a PIPE master is ready to receive package from a PIPE slave.
  Note: Caller should send Query Command to determine the size of HOB before calling this
  function.

  @param Host    - Pointer to sysHost, the system host (root) structure
  @param Id      - PIPE Slave Id
  @param pBuffer - Pointer to a buffer full with package data being received
  @param Size    - Size of HOB needed to receive

  @retval CmdPipe.Pipe.Status of last action

**/
UINT32 GetPipePackage(PSYSHOST Host, UINT8 Id, UINT8 *pBuffer, UINT32 Size);                // CORE

/**

  Provide PIPE communication for transferring package data. Upon this call, this function
  will enter PIPE package-transfered mode, as a PIPE slave, until a CmdPipe.Pipe.End
  is sent by a PIPE master.
  Calling this function when a PIPE slave is ready to enter PIPE package-transfered
  mode (PIPE Read Command is received)

  @param Host    - Pointer to sysHost, the system host (root) structure
  @param Id      - PIPE Slave Id
  @param pBuffer - Pointer to a buffer full with package data being received
  @param Size    - Size of HOB needed to receive

  @retval CmdPipe.Pipe.Status of last action.

**/
UINT32 SendPipePackage(PSYSHOST Host, UINT8 Id, UINT8 *pBuffer, UINT32 Size);               // CORE

/**

  Wait for a specific slave processor's pipe's command

  @param Host    - Pointer to sysHost, the system host (root) structure
  @param Id      - PIPE Slave Id
  @param CmdType - Command Type

  @retval Command that arrived in the slave processor's pipe

**/
UINT32 WaitForPipeCommand(PSYSHOST Host, UINT8 Id, UINT32 CmdType);                         // CORE

/**

  Dispatcher function for all non-SBSPs

  @param host - Pointer to sysHost, the system host (root) structure

  @retval None

**/
void   PipeDispatcher(PSYSHOST host);                                                       // CORE

/**

  Entry point for starting pipe process on Non-SBSPs

  @param host - Pointer to sysHost, the system host (root) structure

  @retval 0 - Success
  @retval 1 - Failure

**/
UINT32 PipeInit(PSYSHOST host);                                                             // CORE

//
// PipeDataSync.c - CORE
//

/**

  Get socket's imc mem varialbe segment of the SYSHOST structure from the specified slave processor

  @param host      - Pointer to sysHost, the system host (root) structure
  @param socketId  - PIPE Slave Id


  @retval None

**/
void GetMemVarData(PSYSHOST host, UINT8 socketId);              // CORE

/**

  Send BSP's socket imc mem varialbe segment of the SYSHOST structure to the specified slave processor

  @param host      - Pointer to sysHost, the system host (root) structure
  @param socketId  - PIPE Slave Id

  @retval None

**/
void SendMemVarData(PSYSHOST host, UINT8 socketId);             // CORE

/**

  Get socket's imc nvram segment of the SYSHOST structure from the specified slave processor

  @param host      - Pointer to sysHost, the system host (root) structure
  @param socketId  - PIPE Slave Id

  @retval None

**/
void GetMemNvramCommonData(PSYSHOST host, UINT8 socketId);      // CORE

/**

  Send BSP socket imc nvram segment of the SYSHOST structure to the specified slave processor

  @param host      - Pointer to sysHost, the system host (root) structure
  @param socketId  - PIPE Slave Id

  @retval None

**/
void SendMemNvramCommonData(PSYSHOST host, UINT8 socketId);     // CORE

/**

  Get socket's imc nvram segment of the SYSHOST structure from the specified slave processor

  @param host      - Pointer to sysHost, the system host (root) structure
  @param socketId  - PIPE Slave Id

  @retval None

**/
void GetMemNvramData(PSYSHOST host, UINT8 socketId);            // CORE

/**

  Send BSP socket imc nvram segment of the SYSHOST structure to the specified slave processor

  @param host      - Pointer to sysHost, the system host (root) structure
  @param socketId  - PIPE Slave Id

  @retval None

**/
void SendMemNvramData(PSYSHOST host, UINT8 socketId);           // CORE

//
// IoAccess.c
//

/**

  Reads 8 bit IO port

  @param ioAddress - IO address to read

  @retval Value read from ioAddress

**/
UINT8  InPort8_(UINT16 ioAddress);                                        // CORE

/**

  Writes 8 bit IO port with designated value

  @param ioAddress - IO Address to use
  @param data      - Data to write to IO Address

  @retval None

**/
void   OutPort8_(UINT16 ioAddress, UINT8 data);                           // CORE

//
// CpuAccess.c
//

/**

  Reads 8-bit APIC ID from the CPU executing this code

  @retval APIC ID

**/
UINT8  GetProcApicId();                                        // CORE

//
// UsbDebugPort.c
//

/**

  Send data to the USB2 debug port

  @param host   - Pointer to sysHost, the system host (root) structure
  @param Data   - Pointer to data to send
  @param Length - Size of data to send

  @retval 0     - Success
  @retval Other - failure

**/
UINT32 Usb2DebugPortSend (PSYSHOST host, UINT8 *Data, UINT32 *Length);   // CORE

/**

  Get character from debug port

  @param host      - Pointer to sysHost, the system host (root) structure
  @param c         - Pointer to store character
  @param usTimeout - timeout to wait fo get character in microseconds

  @retval 0     - Success
  @retval Other - Failure

**/
UINT32 Usb2DebugGetChar (PSYSHOST host, INT8 *c, UINT32 usTimeout);      // CORE

//
// MemUtil.c
//

/**

  Finds if a specific 32-bit pattern is in a block of memory

  @param dest      - Pointer to memory location
  @param value     - 32-bit value to find in memory block
  @param numBytes  - number of bytes to flood with value provided

  @retval 0 - pattern present
  @retval 1 - patern not present

**/
UINT32 MemScan(UINT8 *dest, UINT32 value, UINT32 numBytes);              // NONE

//
// CpuPciAccess.c
//
//
//Moved from MemHooks.c to Error.c
//

/**

  Used to add warnings to the promote warning exception list. Directs call to Chip layer.

  @param host - Pointer to sysHost, the system host (root) structure

  @retval SUCCESS

**/
UINT32 MemInitWarning(PSYSHOST host);

#endif // _sysfunc_h
