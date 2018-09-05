//****************************************************************************
//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//****************************************************************************
//  File History
//
//  Rev. 1.00
//      Bug Fixed:  add RC error log to BMC and report error DIMM To screen(refer to Grantley)
//      Reason:     
//      Auditor:    Timmy Wang
//      Date:       May/26/2016
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
 * Reference Code
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
 *
 ************************************************************************/
#ifdef _MSC_VER
#pragma warning (disable: 4127 4100)     // disable C4127: constant conditional expression
#endif
#include "SysFunc.h"
#include "Pipe.h"
#include "ChipApi.h"
#include "UBOX_MISC.h"		//SMCPKG_SUPPORT++

//
// Local Prototypes
//
UINT64_STRUCT ReadMsrRemotePipe(PSYSHOST host, UINT8 socket, UINT32 msr);
void   PipeRdMsr(PSYSHOST host);
void   WriteMsrRemotePipe(PSYSHOST host, UINT8 socket, UINT32 msr, UINT64_STRUCT msrReg);
void   PipeWrMsr(PSYSHOST host);
UINT8  GetLocalSktId(PSYSHOST host);
void   ReadDataPipe(PSYSHOST Host, UINT8 Id, UINT32 *Data);
void   WriteDataPipe(PSYSHOST Host, UINT8 Id, UINT32 Data);
void   ListenToCommandPipe(PSYSHOST Host, UINT8 Id, UINT32 *CmdPipe);

UINT8
GetLocalSktId (
  PSYSHOST host
  )
/*++

  Get CPU socket id for the current processor

  @param host  - Pointer to the system host (root) structure

  @retval Current processor's Socket ID

--*/
{

/*
  UINT32 NumOfBitShift = 0;
#ifdef IA32
  _asm
  {
    mov  eax, 0xb
    mov   ecx , 1
    cpuid
    and eax ,0x1f
   mov   NumOfBitShift, eax
  }
#else
  {
    EFI_CPUID_REGISTER cpuIdRegisters;
    AsmCpuidEx(0xb, 1, &NumOfBitShift, NULL, NULL, NULL);
    NumOfBitShift &= 0x1F;
  }
#endif
  if (host == NULL) {
    return (UINT8)((GetProcApicId() >> NumOfBitShift) & 0x7);
  } else {
    return host->var.common.socketId;
  } */

  return host->var.common.socketId;
}



void
PipeDispatcher (
               PSYSHOST host
               )
/*++

  Dispatcher function for all non-SBSPs

  @param host  - Pointer to the system host (root) structure

  @retval N/A

--*/
{
  UINT32 Command = PIPE_DISPATCH_NULL;

#ifdef   SERIAL_DBG_MSG
  if (host->setup.common.serialDebugMsgLvl & (SDBG_MAX | SDBG_MIN)) {
    rcPrintf ((host, "N%d Checked into Pipe\n", host->var.common.socketId));
  }
#endif
  while (Command != PIPE_DISPATCH_EXIT) {
    switch (Command) {

    case PIPE_DISPATCH_MEMORY_INIT:
      //
      // Reset the command reg to null
      //
      CHIP_FUNC_CALL(host, WriteDispatchPipeCsr(host, GetLocalSktId(host), PIPE_DISPATCH_NULL));

#ifdef   SERIAL_DBG_MSG
      if (host->setup.common.serialDebugMsgLvl & (SDBG_MAX | SDBG_MIN)) {
        rcPrintf ((host, "N%d Entering MRC\n", host->var.common.socketId));
      }
#endif
      MemStart(host);
      break;

    case PIPE_RD_MSR:

      //
      // Reset the command reg to null
      //
      CHIP_FUNC_CALL(host, WriteDispatchPipeCsr(host, GetLocalSktId(host), PIPE_DISPATCH_NULL));

      PipeRdMsr(host);

      break;

    case PIPE_WR_MSR:

      //
      // Reset the command reg to null
      //
      CHIP_FUNC_CALL(host, WriteDispatchPipeCsr(host, GetLocalSktId(host), PIPE_DISPATCH_NULL));

      PipeWrMsr(host);

      break;

    default:
      //
      // Do nothing
      //
      break;
    }

    Command = CHIP_FUNC_CALL(host, GetDispatchPipeCsr(host, GetLocalSktId(host)));
  }

  return;
}

UINT32
PipeInit (
         PSYSHOST host
         )
/*++

  Entry point for starting pipe process on Non-SBSPs

  @param host  - Pointer to the system host (root) structure

  @retval 0 - Success
  @retval 1 - Failure

--*/
{
  //
  // Only non-system BSPs enter dispatch loop
  //
  if (GetLocalSktId(host) != GetSbspSktId(host)) {
    PipeDispatcher(host);
    return 1;
  } else {
    return 0;
  }
}

BOOLEAN
AcquireCommandPipe (
                   PSYSHOST host,
                   UINT8    Id
                   )
/*++

  Take control of the Command Pipe

  @param host  - Pointer to the system host (root) structure
  @param Id    - PIPE Slave Id

  @retval TRUE - Command taken
  @retval FALSE - Command not given

--*/
{
  UINT8 SbspSktId;

  SbspSktId = GetSbspSktId(host);

  /*
  if (SbspSktId == SKT_INVALID_ID) {
      //
      //OemDebugPrintM(FBD_D_INFO, "MRC - AcquireCommandPipe: FATAL ERROR - Failure to obtain SBSP ID\n");
      //OemDebugPrintM(FBD_D_INFO, "MRC - Check CSI reference code for SR_CPU_SOCKET_INFO_OFFSET at UBOX_CSR_SR64?\n");
      //
      // Can not run MRC without SBSP. Hang here! Need to check to ensure CSI reference
      // still assigns SR_CPU_SOCKET_INFO_OFFSET at UBOX_CSR_SR64?
      //
      //OemAssertion(PIPE_NO_SBSP_FOUND);
      //
  }
   */

  return(Id == SbspSktId) ? TRUE : FALSE;
}

void
ListenToCommandPipe (
                    PSYSHOST host,
                    UINT8    Id,
                    UINT32   *CmdPipe
                    )
/*++

    Stay in loop listenning to the Command PIPE until a command is received. This
    function should be called only by a PIPE Slave.

  @param host    - Pointer to the system host (root) structure
  @param Id      - PIPE Slave Id
  @param CmdPipe - Pointer to a command PIPE data structure

  @retval N/A

--*/
{
  COMMAND_PIPE CommandPipe;

  while (TRUE) {
    CommandPipe.RawPipe = ReadCpuPciCfgEx (host, Id, 0, SR_MEMORY_DATA_STORAGE_COMMAND_PIPE_CSR_ADDR(host));
    if (CommandPipe.Pipe.CmdRequest == PIPE_REQUEST &&  CommandPipe.Pipe.TargetId == Id)
      break;  /* Yes, it's me */
  }
  *CmdPipe = CommandPipe.RawPipe;
}

void
UnlockCommandPipe (
                  PSYSHOST host,
                  UINT8    Id
                  )
/*++

  Unlocks the command pipe (releases control)

  @param host  - Pointer to the system host (root) structure
  @param Id      - PIPE Slave Id

  @retval N/A

--*/
{
  WriteCpuPciCfgEx (host, (UINT8)Id, 0, SR_MEMORY_DATA_STORAGE_COMMAND_PIPE_CSR_ADDR(host), COMMAND_PIPE_UNLOCK(Id));
}

void
LockCommandPipe (
                PSYSHOST host,
                UINT8    Id
                )
/*++

  Lock a specific slave's command pipe

  @param host  - Pointer to the system host (root) structure
  @param Id      - PIPE Slave Id

  @retval N/A

--*/
{
  WriteCpuPciCfgEx (host, Id, 0, SR_MEMORY_DATA_STORAGE_COMMAND_PIPE_CSR_ADDR(host), COMMAND_PIPE_LOCK(Id));
}

void
WaitForCommandPipeUnLocked (
                           PSYSHOST host,
                           UINT8    Id
                           )
/*++

  Wait for a specific slave's command pipe to be unlocked

  @param host    - Pointer to the system host (root) structure
  @param Id      - PIPE Slave Id

  @retval N/A

--*/
{
  UINT32 CmdPipe;
  UINT32 Key = COMMAND_PIPE_UNLOCK(Id);

  //
  // Setup CSR SR Register
  //
  while (TRUE) {
    CmdPipe = ReadCpuPciCfgEx (host, Id, 0, SR_MEMORY_DATA_STORAGE_COMMAND_PIPE_CSR_ADDR(host));
    if (CmdPipe == Key)
      break;
  }
}

UINT32
WaitForPipeCommand (
                   PSYSHOST host,
                   UINT8    Id,
                   UINT32   CmdType
                   )
/*++

  Wait for a specific slave processor's pipe's command

  @param host    - Pointer to the system host (root) structure
  @param Id      - PIPE Slave Id
  @param CmdPipe - Pointer to a command PIPE data structure

  @retval Command that arrived in the slave processor's pipe

--*/
{
  COMMAND_PIPE CmdPipe;

  while (TRUE) {
    ListenToCommandPipe(host, Id, &CmdPipe.RawPipe);
    if (CmdType & (1 << CmdPipe.Pipe.InternalCmd))
      return(UINT32)(1 << CmdPipe.Pipe.InternalCmd);

    CmdPipe.Pipe.Status = PIPE_INVALID;
    CmdPipe.Pipe.NumOfBytes = 0;
    CmdPipe.Pipe.Width_ = 0;
    CmdPipe.Pipe.CmdRequest = PIPE_ACK;
    WriteCommandPipe(host, Id, &CmdPipe.RawPipe);
  }
}

void
WriteCommandPipe (
                 PSYSHOST host,
                 UINT8    Id,
                 UINT32   *CmdPipe
                 )
/*++

  Writes a command to a specific slave's pipe

  @param host    - Pointer to the system host (root) structure
  @param Id      - PIPE Slave Id
  @param CmdPipe - Pointer to a command PIPE data structure

  @retval N/A

--*/
{
  WriteCpuPciCfgEx (host, Id, 0, SR_MEMORY_DATA_STORAGE_COMMAND_PIPE_CSR_ADDR(host), *CmdPipe);
}

void
ReadDataPipe (
             PSYSHOST host,
             UINT8    Id,
             UINT32   *Data
             )
/*++

  Read data pipe of a specific slave processor

  @param host  - Pointer to the system host (root) structure
  @param Id    - PIPE Slave Id
  @param Data  - Pointer to a command PIPE data variable

  @retval N/A

--*/
{
  *Data = ReadCpuPciCfgEx (host, Id, 0, SR_MEMORY_DATA_STORAGE_DATA_PIPE_CSR_ADDR(host));
}

void
WriteDataPipe (
              PSYSHOST host,
              UINT8    Id,
              UINT32   Data
              )
/*++

  Write data pipe of a specific slave processor

  @param host  - Pointer to the system host (root) structure
  @param Id    - PIPE Slave Id
  @param Data - Data to write to pipe

  @retval N/A

--*/
{
  WriteCpuPciCfgEx (host, Id, 0, SR_MEMORY_DATA_STORAGE_DATA_PIPE_CSR_ADDR(host), Data);
}

UINT32
SendCommandPipe (
                PSYSHOST host,
                UINT8    Id,
                UINT32   *CmdPipe
                )
/*++

    Send a command to the Command PIPE until an ACK is received. This
    function should be called only by a PIPE Master.

    @param host    - Pointer to the system host (root) structure
    @param Id      - PIPE Slave Id
    @param CmdPipe - Pointer to a command PIPE data structure

    @retval PIPE Status

--*/
{
  PIPE            *Pipe;

  WriteCommandPipe(host, Id, CmdPipe);

  //
  // Setup CSR SR Register
  //
  while (TRUE) {
    *CmdPipe = ReadCpuPciCfgEx (host, Id, 0, SR_MEMORY_DATA_STORAGE_COMMAND_PIPE_CSR_ADDR(host));
    Pipe = (PIPE *)CmdPipe;
    if (Pipe->CmdRequest == PIPE_ACK)
      break;
  }

  return Pipe->Status;
}

UINT32
GetPipePackage (
               PSYSHOST host,
               UINT8    Id,
               UINT8    *pBuffer,
               UINT32   Size
               )
/*++

    Provide PIPE communication for receiveing package data. Upon this call, this function
    will enter PIPE package-received mode, as a PIPE master.
    Calling this function when a PIPE master is ready to receive package from a PIPE slave.
    Note: Caller should send Query Command to determine the size of HOB before calling this
    function.

  @param host    - Pointer to the system host (root) structure
  @param Id      - PIPE Slave Id
  @param pBuffer - Pointer to a buffer full with package data being received
  @param Size    - Size of HOB needed to receive

  @retval CmdPipe.Pipe.Status of last action

--*/
{
  COMMAND_PIPE CmdPipe;
  UINT32 Status = PIPE_SUCCESS;
  UINT32 Data;
  UINT32 NumOfBytes, ByteRemained;
  UINT8 *pBuf = pBuffer;
  UINT8 *pData = (UINT8 *)&Data;
  UINT8 i;

  //
  // Send Read Package Request
  //
  NumOfBytes = Size;
  if (NumOfBytes) {
    CmdPipe.RawPipe = 0;
    CmdPipe.Pipe.Toggle = 0;        // Setup toggle bit
    CmdPipe.Pipe.Begin = 1;         // Signal the begin of data transfer
    CmdPipe.Pipe.TargetId = Id;
    CmdPipe.Pipe.InternalCmd = PIPE_READ_COMMAND;
    ByteRemained = NumOfBytes;
    while (ByteRemained > 0) {
      if (ByteRemained <= sizeof(UINT32))
        CmdPipe.Pipe.End_ = 1;   // Signal the end of data transfer

      CmdPipe.Pipe.CmdRequest = PIPE_REQUEST;
      if ((Status = SendCommandPipe(host, Id, &CmdPipe.RawPipe)) != PIPE_SUCCESS)
        return Status;

      ByteRemained = (UINT32)CmdPipe.Pipe.NumOfBytes;
      NumOfBytes -= (CmdPipe.Pipe.Width_ + 1);
      ReadDataPipe(host, Id, &Data);
      for (i = 0; i <= CmdPipe.Pipe.Width_; i++, pBuf++)
        *pBuf = *(pData + i);

      CmdPipe.Pipe.Toggle = (CmdPipe.Pipe.Toggle == 1) ? 0 : 1;
      CmdPipe.Pipe.Begin = 0;     // Clear begin indicator bit
    }

    if (NumOfBytes)                 // Double check! Expect this to be zero
      Status = PIPE_INCOMPLETE_ERROR;
  }

  while (ReadCpuPciCfgEx (host, Id, 0, SR_MEMORY_DATA_STORAGE_DISPATCH_PIPE_CSR_ADDR(host)) != PIPE_DISPATCH_FREEZE);
  WriteCpuPciCfgEx (host, Id, 0, SR_MEMORY_DATA_STORAGE_DISPATCH_PIPE_CSR_ADDR(host), PIPE_DISPATCH_NULL);

  return Status;
}

UINT32
SendPipePackage (
                PSYSHOST host,
                UINT8    Id,
                UINT8    *pBuffer,
                UINT32   Size
                )
/*++

    Provide PIPE communication for transferring package data. Upon this call, this function
    will enter PIPE package-transfered mode, as a PIPE slave, until a CmdPipe.Pipe.End
    is sent by a PIPE master.
    Calling this function when a PIPE slave is ready to enter PIPE package-transfered
    mode (PIPE Read Command is received)


  @param host    - Pointer to the system host (root) structure
  @param Id      - PIPE Slave Id
  @param pBuffer - Pointer to a buffer full with package data
  @param Size    - Size of the package for transfer

  @retval CmdPipe.Pipe.Status of last action.

--*/
{
  COMMAND_PIPE CmdPipe;
  UINT32 Data;
  UINT32 ByteRemained;
  UINT32 i;
  UINT8 *pBuf = pBuffer;
  UINT8 *pData = (UINT8 *)&Data;
  UINT8 Toggle = 0;
  BOOLEAN Done = FALSE;
  BOOLEAN Start = FALSE;

  CmdPipe.RawPipe = 0;
  ByteRemained = Size;
  pBuf = pBuffer;
  while (!Done) {
    ListenToCommandPipe(host, Id, &CmdPipe.RawPipe);
    switch (CmdPipe.Pipe.InternalCmd) {
    case PIPE_READ_COMMAND:
      if (CmdPipe.Pipe.Begin) {
        if (Start || ByteRemained != Size) {
          //
          // Detect invalid command, "Begin" bit must not set here.
          // Don't transfer any thing
          //
          CmdPipe.Pipe.Status = PIPE_INVALID;
          CmdPipe.Pipe.NumOfBytes = ByteRemained;
          CmdPipe.Pipe.Width_ = 0;
          break;
        }
        Toggle = (UINT8) CmdPipe.Pipe.Toggle;       // Save Toggle bit
        Start = TRUE;
      } else if (!Start || (UINT8)CmdPipe.Pipe.Toggle == Toggle) {
        //
        // Detect invalid command, "Toggle" bit is invalid.
        // Don't transfer any thing
        //
        CmdPipe.Pipe.Status = (Start == FALSE) ? PIPE_INVALID : PIPE_TOGGLE_ERROR;
        CmdPipe.Pipe.NumOfBytes = ByteRemained;
        CmdPipe.Pipe.Width_ = 0;
        break;
      }

      if (CmdPipe.Pipe.End_) {
        Done = TRUE;
        if (ByteRemained > sizeof(UINT32)) {
          //
          // Detect invalid command. "End" bit must not set here unless "Abort".
          // Don't transfer any thing
          //
          CmdPipe.Pipe.Status = PIPE_INCOMPLETE_ERROR;
          CmdPipe.Pipe.NumOfBytes = ByteRemained;
          CmdPipe.Pipe.Width_ = 0;
          break;
        }
      }

      //
      // At this point, ready to do DATA Ack
      //
      CmdPipe.Pipe.Status = PIPE_SUCCESS;
      if (ByteRemained > sizeof(UINT32)) {
        CmdPipe.Pipe.Width_ = sizeof(UINT32) - 1;    // 0 = 1 byte, 1 = 2 bytes, ...
        ByteRemained -= sizeof(UINT32);             // Update bytes remained
      } else if (ByteRemained > 0) {
        CmdPipe.Pipe.Width_ = ByteRemained - 1;      // No more bytes remained
        ByteRemained = 0;
      } else {
        CmdPipe.Pipe.Status = PIPE_INVALID;         // Detect invalid command
        CmdPipe.Pipe.Width_ = 0;
      }
      CmdPipe.Pipe.NumOfBytes = ByteRemained;
      Toggle = (UINT8) CmdPipe.Pipe.Toggle;           // Save Toggle bit

      if (CmdPipe.Pipe.Status == PIPE_SUCCESS) {
        //
        // Write to Command/Data Pipe
        //
        Data = 0;
        for (i = 0; i <= CmdPipe.Pipe.Width_; i++, pBuf++)
          *(pData + i) = *pBuf;

        WriteDataPipe(host, Id, Data);
      }
      break;
    default:
      //
      // Detect invalid command. Don't transfer any thing
      //
      CmdPipe.Pipe.Status = PIPE_INVALID;
      CmdPipe.Pipe.NumOfBytes = ByteRemained;
      CmdPipe.Pipe.Width_ = 0;
      break;
    }
    //
    // Write to Command Pipe with ACK set
    //
    CmdPipe.Pipe.CmdRequest = PIPE_ACK;
    WriteCommandPipe(host, Id, &CmdPipe.RawPipe);
  }

  WriteCpuPciCfgEx (host, Id, 0, SR_MEMORY_DATA_STORAGE_DISPATCH_PIPE_CSR_ADDR(host), PIPE_DISPATCH_FREEZE);
  while (ReadCpuPciCfgEx (host, Id, 0, SR_MEMORY_DATA_STORAGE_DISPATCH_PIPE_CSR_ADDR(host)) == PIPE_DISPATCH_FREEZE);

  return CmdPipe.Pipe.Status;
}

UINT32
SyncErrorsTbl (
            PSYSHOST host
            )
/*++

  Sync data between SBSP and other sockets

  @param host      - Pointer to the system host (root) structure

  @retval Fatal Error flag

--*/
{
  OutputExtendedCheckpoint((host, STS_CHANNEL_TRAINING, STS_MINOR_NORMAL_MODE, (UINT16)((SUB_DATA_SYNC << 8)|host->var.mem.currentSocket)));
  return SyncErrors(host);
}

UINT8
SyncErrors (
         PSYSHOST host
         )
/*++

  Sync data between SBSP and other sockets

  @param host      - Pointer to the system host (root) structure

  @retval Fatal Error flag

--*/
{
  UINT8   fatalError  = 0;
  UINT8   SocketFatalError = 0;
  UINT8   socket      = 0;
  UINT8   SocketCtr   = 0;
  UINT8   SckId       = 0;
  UINT8   haltOnError;
  UINT32  data;
  UINT32  code;

#if ENHANCED_WARNING_LOG
  EWL_PRIVATE_DATA      *ewlPrivateData   = &host->var.common.ewlPrivateData;
  EWL_PUBLIC_DATA       *log              = &host->var.common.ewlPrivateData.status;
  UINT32                bspBufferOffset;
  UINT32                apBufferIndex;
  UINT32                apBufferSize;
#else
  UINT8   numWarnings = 0;
  UINT8   warningIndex;
#endif

  //
  // BSP Gets Status of APs
  //
  UINT8 rcWriteRegDumpCurrent = host->var.common.rcWriteRegDump;
  host->var.common.rcWriteRegDump = 0;
  if (host->var.common.socketId == host->nvram.common.sbspSocketId) {
    for (socket = 0; socket < MAX_SOCKET; socket++) {
      if (socket == host->nvram.common.sbspSocketId) {
        continue;
      }
      //
      // Check if socket is valid
      //
      if ((host->var.common.socketPresentBitMap & (BIT0 << socket)) == 0) continue;
      if (CHIP_FUNC_CALL(host, CheckSktId(host, socket))) continue;
#if ENHANCED_WARNING_LOG
      //
      // Get number of bytes in EWL buffer
      GetPipePackage(host, socket, (UINT8*)&apBufferSize, sizeof(UINT32));

      // Get the number of bytes omitted due to space restrictions from AP
      GetPipePackage(host, socket, (UINT8 *)&data, sizeof(UINT32));
      ewlPrivateData->bufSizeOverflow += data;

      bspBufferOffset = log->Header.FreeOffset;
      //if SBSP buffer does not have room for entire AP buffer
      if (sizeof(log->Buffer) - bspBufferOffset < apBufferSize){
        ewlPrivateData->bufSizeOverflow += apBufferSize - (sizeof(log->Buffer) - bspBufferOffset);
      }

      //iterate through AP's EWL buffer, copy to SBSP if there is room in the buffer
      apBufferIndex = 0;
      while (apBufferIndex < apBufferSize){
        GetPipePackage(host, socket, (UINT8 *)&code, sizeof(UINT32));
        //copy each dword to sbsp buffer until within 4 bytes of buffer end
        if (bspBufferOffset + apBufferIndex < sizeof(log->Buffer) - 4){
          MemCopy(&log->Buffer[bspBufferOffset + apBufferIndex], (UINT8 *)&code, sizeof(UINT32));
        }
        //If there is a partial dword of space remaining, only copy that amount
        else if (bspBufferOffset + apBufferIndex < sizeof(log->Buffer)){
          MemCopy(&log->Buffer[bspBufferOffset + apBufferIndex], (UINT8 *)&code, (sizeof(log->Buffer) - bspBufferOffset - apBufferIndex));
          rcPrintf((host, "Warning Log ran out of space during sync with other sockets!\n"));
        }
        // AP is going to send the rest of the log, must keep in lock step even though we are out of room in BSP buffer
        apBufferIndex += sizeof(UINT32);
      }

      //update bsp host pointer to the end of the buffer
      if (bspBufferOffset + apBufferSize > sizeof(log->Buffer)){
        log->Header.FreeOffset = sizeof(log->Buffer);
      }
      else{
        log->Header.FreeOffset += (apBufferSize);
      }

      //update current bsp header info about # of logged warnings
      ewlPrivateData->numEntries = CountWarningsInLog(host);

      //recalculate crc with copied warnings
      CalculateCrc32((UINT8 *)log, sizeof(EWL_PUBLIC_DATA), &log->Header.Crc);

#else
      //
      // Get number of warnings logged
      //
      GetPipePackage(host, socket, (UINT8*)&numWarnings, sizeof(UINT8));

      //
      // Get and log each warning
      //
      for (warningIndex = 0; warningIndex < numWarnings; warningIndex++) {
        GetPipePackage(host, socket, (UINT8*)&data, sizeof(UINT32));
        GetPipePackage(host, socket, (UINT8*)&code, sizeof(UINT32));
        LogWarning(host, (UINT8)(code >> 8), (UINT8)(code & 0xFF), data);
      } // warningIndex loop
#endif

      //
      // Get fatal error status
      //
      GetPipePackage(host, socket, (UINT8*)&host->var.mem.socket[socket].fatalError, sizeof(UINT8));

      //
      // BSP found a fatal error on AP
      //
      if (host->var.mem.socket[socket].fatalError) {
        GetPipePackage(host, socket, (UINT8*)&host->var.mem.socket[socket].majorCode , sizeof(UINT8));
        GetPipePackage(host, socket, (UINT8*)&host->var.mem.socket[socket].minorCode , sizeof(UINT8));
#if SMCPKG_SUPPORT
        GetPipePackage(host, socket, (UINT8*)&host->var.mem.socket[socket].logData , sizeof(UINT32));  //SMCPKG_SUPPORT++
#endif
        rcPrintf ((host, "\n ERROR: BSP Found error on SocketId = %d registered Major Code = %x, Minor Code = %x \n",
                  socket,
                  host->var.mem.socket[socket].majorCode,
                  host->var.mem.socket[socket].minorCode));
        fatalError = host->var.mem.socket[socket].fatalError;
        SocketFatalError = socket;
      }// if Fatal Error
    }//socket loop
  } else {
    //
    // AP Report Status
    //
    SckId   = host->var.common.socketId ;

#if ENHANCED_WARNING_LOG
    //
    // send number of bytes in socket's warning log buffer
    //
    SendPipePackage(host, SckId, (UINT8*)&log->Header.FreeOffset, sizeof(UINT32));

    //
    // send the number of bytes omitted from AP's EWL buffer
    //
    SendPipePackage(host, SckId, (UINT8*)&ewlPrivateData->bufSizeOverflow, sizeof(UINT32));

    //
    // for each warning in buffer send type, then send pointer to header (size data determined by header)
    //
    apBufferIndex = 0;
    while (apBufferIndex < log->Header.FreeOffset){
      code = *((UINT32 *)&log->Buffer[apBufferIndex]);
      SendPipePackage(host, SckId, (UINT8*)&code, sizeof(UINT32));
      apBufferIndex += sizeof(UINT32);
    }

    //
    // Reset warning log after transfering data to the master socket
    //
    ewlPrivateData->bufSizeOverflow = 0;
    ewlPrivateData->numEntries = 0;
    log->Header.FreeOffset = 0;
    CalculateCrc32((UINT8 *)log, sizeof(EWL_PUBLIC_DATA), &log->Header.Crc);

#else
    //
    // Get number of warnings logged
    //
    SendPipePackage(host, SckId, (UINT8*)&host->var.common.status.index, sizeof(UINT8));

    //
    // Get and log each warning
    //
    for (warningIndex = 0; warningIndex < host->var.common.status.index; warningIndex++) {
      SendPipePackage(host, SckId, (UINT8*)&host->var.common.status.log[warningIndex].data, sizeof(UINT32));
      SendPipePackage(host, SckId, (UINT8*)&host->var.common.status.log[warningIndex].code, sizeof(UINT32));
    }

    //
    // Reset warning log after transfering data to the master socket
    //
    host->var.common.status.index = 0;
#endif
    //
    // Get fatal error status
    //
    SendPipePackage(host, SckId, (UINT8*)&host->var.mem.socket[SckId].fatalError, sizeof(UINT8));

    if (host->var.mem.socket[SckId].fatalError) {
      SendPipePackage(host, SckId, (UINT8*)&host->var.mem.socket[SckId].majorCode, sizeof(UINT8));
      SendPipePackage(host, SckId, (UINT8*)&host->var.mem.socket[SckId].minorCode, sizeof(UINT8));
#if SMCPKG_SUPPORT
      SendPipePackage(host, SckId, (UINT8*)&host->var.mem.socket[SckId].logData, sizeof(UINT32));		//SMCPKG_SUPPORT++
#endif
      //
      // Non-BSP has a fatal error and it must halt
      //
      rcPrintf ((host, "\n ERROR: AP has an error on SocketId = %d registered Major Code = %x,Minor Code = %x \n",
                SckId,
                host->var.mem.socket[SckId].majorCode,
                host->var.mem.socket[SckId].minorCode));
      fatalError = host->var.mem.socket[SckId].fatalError;
      SocketFatalError = SckId;
    }// if Fatal Error
  }//If BSP Check STATUS

  //
  // BSP Reports Fatal ERRORS to AP's
  //
  if (CHIP_FUNC_CALL(host, CheckMemRas(host))){

    for (SocketCtr = 0; SocketCtr < MAX_SOCKET; SocketCtr++) {

      if ((host->var.common.socketPresentBitMap & (BIT0 << SocketCtr)) == 0) continue;

      for (socket = 0; socket < MAX_SOCKET; socket++) {
        if (socket == host->nvram.common.sbspSocketId) {
          continue;
        }
        //
        // Check if socket is valid, don't send to AP its own data
        //
        if (((host->var.common.socketPresentBitMap & (BIT0 << socket)) == 0) ||(socket == SocketCtr)) continue;

        //
        // Send fatal error status
        //
        SendPipePackage(host, socket, (UINT8*)&host->var.mem.socket[SocketCtr].fatalError, sizeof(UINT8));

        if (host->var.mem.socket[SocketCtr].fatalError) {
          SendPipePackage(host, socket, (UINT8*)&host->var.mem.socket[SocketCtr].majorCode, sizeof(UINT8));
          SendPipePackage(host, socket, (UINT8*)&host->var.mem.socket[SocketCtr].minorCode, sizeof(UINT8));
#if SMCPKG_SUPPORT
          SendPipePackage(host, socket, (UINT8*)&host->var.mem.socket[SocketCtr].logData, sizeof(UINT32));  //SMCPKG_SUPPORT++
#endif
          //
          // BSP has a fatal error and it must halt
          //
          rcPrintf ((host, "\n ERROR: BSP has an error Reported to SocketId = %d registered Major Code = %x, Minor Code = %x \n",
                    socket,
                    host->var.mem.socket[SocketCtr].majorCode,
                    host->var.mem.socket[SocketCtr].minorCode));
          fatalError = host->var.mem.socket[socket].fatalError;
          SocketFatalError = socket;
        }// if Fatal Error
      }//end socket Loop
    }//end SocketCtr Loop
  } else {
    //
    // AP Get status to other AP's including BSP
    //
    SckId   = host->var.common.socketId ;
    for (socket = 0; socket < MAX_SOCKET; socket++) {
      //
      // Check if socket is valid and dont get your own data
      //
      if (((host->var.common.socketPresentBitMap & (BIT0 << socket)) == 0) || (socket == SckId)) continue;

      //
      // Get fatal error status
      //
      GetPipePackage(host, SckId, (UINT8*)&host->var.mem.socket[socket].fatalError, sizeof(UINT8));

      if (host->var.mem.socket[socket].fatalError) {
        GetPipePackage(host, SckId, (UINT8*)&host->var.mem.socket[socket].majorCode, sizeof(UINT8));
        GetPipePackage(host, SckId, (UINT8*)&host->var.mem.socket[socket].minorCode, sizeof(UINT8));
#if SMCPKG_SUPPORT
        GetPipePackage(host, SckId, (UINT8*)&host->var.mem.socket[socket].logData , sizeof(UINT32));	//SMCPKG_SUPPORT++
#endif
        //
        // AP has detected a fatal error on other CPUs and it must halt
        //
        rcPrintf ((host, "\n ERROR: AP detected error on SocketId = %d registered Major Code = %x,Minor Code = %x \n",
                  socket,
                  host->var.mem.socket[socket].majorCode,
                  host->var.mem.socket[socket].minorCode));
        fatalError = host->var.mem.socket[socket].fatalError;
        SocketFatalError = socket;
      }// if fatal error
    }//socket loop
  }//end Send Status

  host->var.common.rcWriteRegDump = rcWriteRegDumpCurrent;
#if SMCPKG_SUPPORT
	if ((host->var.common.socketId == 0)&&(host->var.common.numCpus>1))
	{
		if (fatalError)
			OemIpmiWarningReport(host);//for CPU2.
		else
		{
			host->var.mem.socket[1].majorCode = (UINT8)(ReadCpuPciCfgEx (host, 1, 0, BIOSNONSTICKYSCRATCHPAD10_UBOX_MISC_REG) >> 16);
			if(host->var.mem.socket[1].majorCode)
  {
			host->var.mem.socket[1].minorCode = (UINT8)ReadCpuPciCfgEx (host, 1, 0, BIOSNONSTICKYSCRATCHPAD10_UBOX_MISC_REG);
			host->var.mem.socket[1].logData = ReadCpuPciCfgEx (host, 1, 0, BIOSNONSTICKYSCRATCHPAD11_UBOX_MISC_REG);
			//OemIpmiWarningReport(host);//for CPU2.
  }
		}
	}
#endif
  if (fatalError) {
    if (host->var.common.socketId == host->nvram.common.sbspSocketId) {
      if (host->setup.common.options & HALT_ON_ERROR_EN) {
        haltOnError = 1;
      } else {
        haltOnError = 0;
      }

      //
      // Call platform hook to handle fatal error
      //
      PlatformFatalError (host, host->var.mem.socket[SocketFatalError].majorCode, host->var.mem.socket[SocketFatalError].minorCode, &haltOnError);
    }

    //
    // Either halt or exit the MRC
    //
    HaltOnError(host, host->var.mem.socket[SocketFatalError].majorCode, host->var.mem.socket[SocketFatalError].minorCode);
  }

  return fatalError;
} // SyncErrors

UINT64_STRUCT
ReadMsrRemotePipe (
                  PSYSHOST  host,
                  UINT8     socket,
                  UINT32    msr
                  )
/*++

  Issues the command to a slave device to read an MSR

  @param host    - Pointer to the system host (root) structure
  @param socket  - Socket number
  @param msr     - Address of MSR to read

  @retval UINT64_STRUCT - MSR value as read from CPU

--*/
{
  UINT64_STRUCT msrReg;

  UINT8 rcWriteRegDumpCurrent = host->var.common.rcWriteRegDump;
  host->var.common.rcWriteRegDump = 0;
  //
  // Dispatch slave to read an MSR
  //
  //MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
  //              "Dispatch N%d to read MSR\n", socket));
  while (ReadCpuPciCfgEx (host, socket, 0, SR_MEMORY_DATA_STORAGE_DISPATCH_PIPE_CSR_ADDR(host)) != 0x00);
  WriteCpuPciCfgEx (host, socket, 0, SR_MEMORY_DATA_STORAGE_DISPATCH_PIPE_CSR_ADDR(host), PIPE_RD_MSR);

  //
  // Send the MSR to be read to the slave
  //
  SendPipePackage(host, socket, (UINT8*)&msr, sizeof(UINT32));

  //
  // Get MSR data from slave
  //
  GetPipePackage(host, socket, (UINT8*)&msrReg, sizeof(UINT64_STRUCT));
  host->var.common.rcWriteRegDump = rcWriteRegDumpCurrent;

  return msrReg;
} // ReadMsrRemotePipe

void
PipeRdMsr (
          PSYSHOST host
          )
/*++

  Read MSR function for slave device using PIPE

  @param host  - Pointer to the system host (root) structure

  @retval N/A

--*/
{
  UINT32        msr;
  UINT64_STRUCT msrReg;

  UINT8 rcWriteRegDumpCurrent = host->var.common.rcWriteRegDump;
  host->var.common.rcWriteRegDump = 0;
  //
  // Get MSR to read
  //
  GetPipePackage(host, host->var.common.socketId, (UINT8*)&msr, sizeof(UINT32));

  //
  // Read MSR
  //
  msrReg = ReadMsrLocal (msr);

  //
  // Return data
  //
  SendPipePackage(host, host->var.common.socketId, (UINT8*)&msrReg.lo, sizeof(UINT64_STRUCT));
  host->var.common.rcWriteRegDump = rcWriteRegDumpCurrent;
} // PipeRdMsr

void
WriteMsrRemotePipe (
                   PSYSHOST      host,
                   UINT8         socket,
                   UINT32        msr,
                   UINT64_STRUCT msrReg
                   )
/*++

  Issues the command to a slave device to write an MSR

  @param host    - Pointer to the system host (root) structure
  @param socket  - Socket number
  @param msr     - Address of MSR to read
  @param msrReg  - MSR Data

  @retval N/A

--*/
{

  UINT8 rcWriteRegDumpCurrent = host->var.common.rcWriteRegDump;
  host->var.common.rcWriteRegDump = 0;
  //
  // Dispatch slave to read an MSR
  //
  WriteCpuPciCfgEx (host, socket, 0, SR_MEMORY_DATA_STORAGE_DISPATCH_PIPE_CSR_ADDR(host), PIPE_WR_MSR);

  //
  // Send the MSR to be read to the slave
  //
  SendPipePackage(host, socket, (UINT8*)&msr, sizeof(UINT32));

  //
  // Send the MSR to be read to the slave
  //
  SendPipePackage(host, socket, (UINT8*)&msrReg, sizeof(UINT64_STRUCT));
  host->var.common.rcWriteRegDump = rcWriteRegDumpCurrent;

} // WriteMsrRemotePipe

void
PipeWrMsr (
          PSYSHOST host
          )
/*++

  Write MSR function for slave device using PIPE

  @param host  - Pointer to the system host (root) structure

  @retval N/A

--*/
{
  UINT32        msr;
  UINT64_STRUCT msrReg;

  UINT8 rcWriteRegDumpCurrent = host->var.common.rcWriteRegDump;
  host->var.common.rcWriteRegDump = 0;
  //
  // Get MSR to write
  //
  GetPipePackage(host, host->var.common.socketId, (UINT8*)&msr, sizeof(UINT32));

  //
  // Get data to write
  //
  GetPipePackage(host, host->var.common.socketId, (UINT8*)&msrReg, sizeof(UINT64_STRUCT));

  //
  // Write MSR
  //
  WriteMsrLocal (msr, msrReg);
  host->var.common.rcWriteRegDump = rcWriteRegDumpCurrent;

} // PipeWrMsr

UINT64_STRUCT
ReadMsrPipe (
            PSYSHOST  host,
            UINT8     socket,
            UINT32    msr
            )
/*++

  Calls the correct read MSR function based on single vs multi-threaded execution

  @param host    - Pointer to the system host (root) structure
  @param socket  - Socket number
  @param msr     - Address of MSR to read

  @retval UINT64_STRUCT - MSR value as read from CPU

--*/
{
  UINT64_STRUCT msrReg;

  UINT8 rcWriteRegDumpCurrent = host->var.common.rcWriteRegDump;
  host->var.common.rcWriteRegDump = 0;
  if (socket != host->var.common.socketId) {
    msrReg = ReadMsrRemotePipe (host, socket, msr);
  } else {
    msrReg = ReadMsrLocal (msr);
  }
  host->var.common.rcWriteRegDump = rcWriteRegDumpCurrent;

  return msrReg;
} // ReadMsrPipe

void
WriteMsrPipe (
             PSYSHOST      host,
             UINT8         socket,
             UINT32        msr,
             UINT64_STRUCT msrReg
             )
/*++

  Calls the correct write MSR function based on single vs multi-threaded execution

  @param host    - Pointer to the system host (root) structure
  @param socket  - Socket number
  @param msr     - Address of MSR to read
  @param msrReg  - MSR Data

  @retval N/A

--*/
{
  UINT8 rcWriteRegDumpCurrent = host->var.common.rcWriteRegDump;
  host->var.common.rcWriteRegDump = 0;
  if (socket != host->var.common.socketId) {
    WriteMsrRemotePipe (host, socket, msr, msrReg);
  } else {
    WriteMsrLocal (msr, msrReg);
  }
  host->var.common.rcWriteRegDump = rcWriteRegDumpCurrent;
} // WriteMsrPipe






