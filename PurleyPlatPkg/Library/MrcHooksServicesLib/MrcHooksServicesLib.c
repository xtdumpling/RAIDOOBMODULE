
//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/** @file
  This library class provides MRC Hooks Services.

  Copyright (c) 2011 - 2016, Intel Corporation. All rights reserved.
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/

#ifndef MINIBIOS_BUILD
#include <Uefi.h>
#include <PiPei.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BaseLib.h>
#include <Library/TimerLib.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiDecompressLib.h>
#include <Library/PeiServicesLib.h>

#include "DataTypes.h"
#include "SysHost.h"
#include "SysRegs.h"
#include "RcRegs.h"

#include "Ppi/MrcHooksServicesPpi.h"
#include "Ppi/MrcHooksChipServicesPpi.h"

#endif

/**

  MRC wrapper code.
  Check if Mrc Hooks Services PPI are initialized

  @param Host = pointer to sysHost structure

**/
BOOLEAN 
IsMrcHooksServicesPpiReady (
    PSYSHOST Host
  )
{
  BOOLEAN Result = FALSE;

  if ((Host != NULL) && (Host->MrcHooksServicesPpi != NULL) && (Host->MrcHooksChipServicesPpi != NULL)) {
    Result = TRUE;
  } else {
    DEBUG ((DEBUG_ERROR, "MrcHooksServicesPpiWrapper used before available\n"));
  }
  return Result;
}

/**
  LogWarning() will always log warning details to the legacy warning log
  
  @param Host      = pointer to sysHost structure
  @param majorCode = Major warning code
  @param minorCode = Minor warning code
  @param logData   = (Socket << 24) + (Channel << 16) + (Dimm << 8) + (Rank)
**/
VOID
LogWarning (
  PSYSHOST Host,
  UINT8    majorCode,
  UINT8    minorCode,
  UINT32   logData
  )
{
#ifndef MINIBIOS_BUILD
  if (IsMrcHooksServicesPpiReady (Host)) {
    Host->MrcHooksServicesPpi->LogWarning (Host, majorCode, minorCode, logData);  
  }
#endif  
}


/**
  OutputWarning() will always log warning to Enhanced Warning Log if possible.
  If EWL is not defined, it will log warning details to legacy warning log.
  
  @param Host      = pointer to sysHost structure
  @param majorCode = Major warning code
  @param minorCode = Minor warning code
  @param logData   = (Socket << 24) + (Channel << 16) + (Dimm << 8) + (Rank)
**/

VOID
OutputWarning (
  PSYSHOST Host,
  UINT8    majorCode,
  UINT8    minorCode,
  UINT8    socket,
  UINT8    ch,
  UINT8    dimm,
  UINT8    rank
  )
{
#ifndef MINIBIOS_BUILD
  if (IsMrcHooksServicesPpiReady (Host)) {
    Host->MrcHooksServicesPpi->OutputWarning (Host, majorCode, minorCode, socket, ch, dimm, rank);  
  }
#endif  
}

UINT32
SetPromoteWarningException (
  PSYSHOST Host,
  UINT8    majorCode,
  UINT8    minorCode
  )
{
#ifndef MINIBIOS_BUILD
  UINT32 ReturnValue = 0;

  if (IsMrcHooksServicesPpiReady (Host)) {
    ReturnValue = Host->MrcHooksServicesPpi->SetPromoteWarningException (Host, majorCode, minorCode);  
  }

  return ReturnValue;
#else
  return 0;  
#endif 
}

UINT32
ReadSmb (
  PSYSHOST         Host,
  UINT8            socket,
  struct smbDevice dev,
  UINT8            byteOffset,
  UINT8            *data
  )
{  
#ifndef MINIBIOS_BUILD
  UINT32 ReturnValue = 0;  

  if (IsMrcHooksServicesPpiReady (Host)) {
    ReturnValue = Host->MrcHooksServicesPpi->ReadSmb (Host, socket, dev, byteOffset, data);  
  }

  return ReturnValue;
#else
  return 0;  
#endif 
}

UINT32
WriteSmb (
  PSYSHOST         Host,
  UINT8            socket,
  struct smbDevice dev,
  UINT8            byteOffset,
  UINT8            *data
  )
{
#ifndef MINIBIOS_BUILD
  UINT32 ReturnValue = 0;  

  if (IsMrcHooksServicesPpiReady (Host)) {
    ReturnValue = Host->MrcHooksServicesPpi->WriteSmb (Host, socket, dev, byteOffset, data);  
  }

  return ReturnValue;
#else
  return 0;
#endif
}

UINT32
DiscoverUsb2DebugPort (
  PSYSHOST Host
  )
{
#ifndef MINIBIOS_BUILD
  UINT32 ReturnValue = 0;  

  if (IsMrcHooksServicesPpiReady (Host)) {
    ReturnValue = Host->MrcHooksServicesPpi->DiscoverUsb2DebugPort (Host);
  }

  return ReturnValue;
#else
  return 0;
#endif
}

UINT32
InitializeUsb2DebugPort (
  PSYSHOST Host
  )
{
#ifndef MINIBIOS_BUILD
  UINT32 ReturnValue = 0;  

  if (IsMrcHooksServicesPpiReady (Host)) {
    ReturnValue = Host->MrcHooksServicesPpi->InitializeUsb2DebugPort (Host);
  }

  return ReturnValue;
#else
  return 0;
#endif  
  
}

UINT32
ReadCpuPciCfgEx (
  PSYSHOST Host,
  UINT8    SocId,
  UINT8    BoxInst,
  UINT32   Offset
  )
{
#ifndef MINIBIOS_BUILD
  UINT32 ReturnValue = 0;

  if (IsMrcHooksServicesPpiReady (Host)) {
    ReturnValue = Host->MrcHooksChipServicesPpi->ReadCpuPciCfgEx (Host, SocId, BoxInst, Offset);  //retval Register value(data)
  }

  return ReturnValue;
#else
  return 0;
#endif
}

VOID
WriteCpuPciCfgEx (
  PSYSHOST Host,
  UINT8    SocId,
  UINT8    BoxInst,
  UINT32   Offset,
  UINT32   Data
  )
{
#ifndef MINIBIOS_BUILD 
  if (IsMrcHooksServicesPpiReady (Host)) {
    Host->MrcHooksChipServicesPpi->WriteCpuPciCfgEx (Host, SocId, BoxInst, Offset, Data);  
  }
#endif 
}

VOID
FatalError (
  PSYSHOST Host,
  UINT8    majorCode,
  UINT8    minorCode
  )
{
#ifndef MINIBIOS_BUILD
  if (IsMrcHooksServicesPpiReady (Host)) {
    Host->MrcHooksChipServicesPpi->FatalError (Host, majorCode, minorCode);  
  }
#endif    
}

int
rcVprintf (
  PSYSHOST   Host,
  const INT8 *Format,
  va_list    Marker
  )
{
#ifndef MINIBIOS_BUILD   
  if (IsMrcHooksServicesPpiReady (Host)) {
    Host->MrcHooksServicesPpi->rcVprintf (Host, Format, Marker);  
  }
#endif  

  return 0;
}

VOID
WriteMsrPipe (
  PSYSHOST      Host,
  UINT8         socket,
  UINT32        msr,
  UINT64_STRUCT msrReg
  )
{
#ifndef MINIBIOS_BUILD
  if (IsMrcHooksServicesPpiReady (Host)) {
    Host->MrcHooksServicesPpi->WriteMsrPipe (Host, socket, msr, msrReg);
  }
#endif  
}

VOID
OutputCheckpoint (
  PSYSHOST Host,
  UINT8    majorCode,
  UINT8    minorCode,
  UINT16   data
  )
{
#ifndef MINIBIOS_BUILD
  if (IsMrcHooksServicesPpiReady (Host)) {
    Host->MrcHooksServicesPpi->OutputCheckpoint (Host, majorCode, minorCode, data);  
  }
#endif  
}

KTI_STATUS
SendMailBoxCmdToPcode (
  struct sysHost      *Host,
  UINT8               Cpu,
  UINT32              Command,
  UINT32              Data
  )
{  
#ifndef MINIBIOS_BUILD
  KTI_STATUS ReturnValue = 0;

  if (IsMrcHooksServicesPpiReady (Host)) {
    ReturnValue = Host->MrcHooksChipServicesPpi->SendMailBoxCmdToPcode (Host, Cpu, Command, Data);  
  }

  return ReturnValue;  
#else
  return 0;
#endif  
}

/**

  Read SPD byte from Serial EEPROM

  @param Host       - Pointer to sysHost
  @param socket     - Socket ID
  @param ch         - Channel on socket
  @param dimm       - DIMM on channel
  @param byteOffset - Byte Offset to read
  @param data       - Pointer to data

  @retval status

**/
UINT32
ReadSpd (
  PSYSHOST         Host,
  UINT8            socket,
  UINT8            ch,
  UINT8            dimm,
  UINT16           byteOffset,
  UINT8            *data
  )
{  
#ifndef MINIBIOS_BUILD
  UINT32 ReturnValue = 0;  

  if (IsMrcHooksServicesPpiReady (Host)) {
    ReturnValue = Host->MrcHooksServicesPpi->ReadSpd (Host, socket, ch, dimm, byteOffset, data);
  }

  return ReturnValue; 
#else
  return 0;
#endif  
}

/**

  Delay time in 1 us units

  @param host    - Pointer to the system host (root) structure
  @param usDelay - number of us units to delay

  @retval VOID

**/
VOID
FixedDelayMem (
  PSYSHOST Host,
  UINT32   usDelay
  )
{
#ifndef MINIBIOS_BUILD   
  if (IsMrcHooksServicesPpiReady(Host)){
    Host->MrcHooksServicesPpi->FixedDelayMem (Host, usDelay);  
  }
#endif
}

VOID
debugPrintMemFunction (
  PSYSHOST  Host,
  UINT32    dbgLevel,
  UINT8     socket,
  UINT8     ch,
  UINT8     dimm,
  UINT8     rank,
  UINT8     strobe,
  UINT8     bit,
  char*     Format,
  va_list   Marker
  )
{
#ifdef SERIAL_DBG_MSG
#ifndef MINIBIOS_BUILD
  if (IsMrcHooksServicesPpiReady (Host)) {
    Host->MrcHooksChipServicesPpi->DebugPrintMemFunction (Host, dbgLevel, socket, ch, dimm, rank, strobe, bit, Format, Marker);  
  }
#endif
#endif // SERIAL_DBG_MSG
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Below functions are not part of MrcHooksServicesPpi 
// Only wrap other UEFI libraries to avoid change OemProcMemInitLib to save effort for OEMs
//
INT16 Crc16 (
  char *ptr, int count
  )
{
  INT16 crc, i;
  crc = 0;
  while (--count >= 0) {
    crc = crc ^ (INT16)(int)*ptr++ << 8;
    for (i = 0; i < 8; ++i) {
      if (crc & 0x8000) {
        crc = crc << 1 ^ 0x1021;
      } else {
        crc = crc << 1;
      }
    }
  }
  return (crc & 0xFFFF);
}


UINT8
InPort8 (
  PSYSHOST Host,
  UINT16   ioAddress
  )
{
  return IoRead8 (ioAddress);
}

UINT16
InPort16 (
  PSYSHOST Host,
  UINT16   ioAddress
  )
{
  return IoRead16 (ioAddress);
}

UINT32
InPort32 (
  PSYSHOST Host,
  UINT16   ioAddress
  )
{
  return IoRead32 (ioAddress);
}

VOID
OutPort8 (
  PSYSHOST Host,
  UINT16   ioAddress,
  UINT8    data
  )
{
  IoWrite8 (ioAddress, data);
}

VOID
OutPort16 (
  PSYSHOST Host,
  UINT16   ioAddress,
  UINT16   data
  )
{
  IoWrite16 (ioAddress, data);
}
VOID
OutPort32 (
  PSYSHOST Host,
  UINT16   ioAddress,
  UINT32   data
  )
{
  IoWrite32 (ioAddress, data);
}

UINT32
CalculateCrc32 (
  IN  UINT8                             *Data,
  IN  UINTN                             DataSize,
  IN OUT UINT32                         *CrcOut
  )
{
  *CrcOut = CalculateCheckSum32 ((UINT32*) Data, (DataSize / 4));
  return 0;
}

VOID
MemCopy (
        UINT8  *dest,
        UINT8  *src,
        UINT32 numBytes
        )
{
  CopyMem (dest, src, numBytes);
}

VOID
MemSetLocal (
  UINT8  *dest,
  UINT32 value,
  UINT32 numBytes
  )
{
  SetMem (dest, numBytes, (UINT8)value);
}

UINT32
Decompress (
  VOID    *Source,
  UINT32  SrcSize,
  VOID    *Destination,
  UINT32  DstSize,
  VOID    *Scratch,
  UINT32  ScratchSize,
  UINT8   Version
  )
{
  EFI_STATUS Status;
  Status =  UefiDecompress (Source, Destination,  Scratch);
  return EFI_ERROR (Status);
}

int
rcPrintfunction (
  PSYSHOST   Host,
  const INT8 *Format,
  ...
  )
{
#ifdef SERIAL_DBG_MSG
  UINT32 rcVprintfstatus = 0;

  va_list Marker;

  if (Host->var.mem.varStructInit == 0) {
    Host->var.mem.serialDebugMsgLvl = Host->setup.common.serialDebugMsgLvl;
  }

  if (Host->var.mem.serialDebugMsgLvl) {
    va_start (Marker, Format);
    rcVprintfstatus = rcVprintf (Host, Format, Marker);
    va_end (Marker);
    return rcVprintfstatus;
  } else {
    return 0;
  }
#else
  //
  // If serial debug is disabled, we simply return 0 characters printed
  //
  return 0;
#endif
}

VOID
debugPrintMem (
  PSYSHOST  host,
  UINT32    dbgLevel,
  UINT8     socket,
  UINT8     ch,
  UINT8     dimm,
  UINT8     rank,
  UINT8     strobe,
  UINT8     bit,
  char*     Format,
  ...
  )
{
#ifdef SERIAL_DBG_MSG
  va_list Marker;
  va_start (Marker, Format);
  debugPrintMemFunction (host, dbgLevel, socket, ch, dimm, rank, strobe, bit, Format, Marker);
  va_end (Marker);
#endif // SERIAL_DBG_MSG
}

#ifdef IA32
/**

  Specific print to serial output

  @param Host      - Pointer to the system Host (root) structure
  @param DbgLevel  - specific debug level
  @param Format    - string format

  @retval None

--*/
VOID
IioDebugPrint (
  IN  IIO_GLOBALS *IioGlobalData,
  IN  UINT32 DbgLevel,
  IN  char* Format,
  ...
  )
{
#ifdef SERIAL_DBG_MSG
  va_list  Marker;
  va_start (Marker, Format);
  rcVprintf ((PSYSHOST) IioGlobalData->IioVar.IioOutData.PtrAddress.Address32bit.Value, Format, Marker);
#endif
}
#endif // IA32

UINT8
checkMsgLevel (
              PSYSHOST  Host,
              UINT32    level
              )
/*++

Routine Description:

  Checks the serial debug message level

  @param Host  - Pointer to the system Host (root) structure
  @param level - Debug message level to check

  @retval 1 - Message level is set
  @retval 0 - Message level is not set

--*/
{
  UINT8 status = 0;
#ifdef SERIAL_DBG_MSG
  if (Host->var.mem.serialDebugMsgLvl & level) {
    status = 1;
  }
#endif // SERIAL_DBG_MSG
  return status;
}

VOID
FixedDelay (
  PSYSHOST Host,
  UINT32   usDelay
  )
{
  NanoSecondDelay (usDelay * 1000);
}

