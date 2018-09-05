/** @file
  Routines to access scratch pad register

@copyright
  Copyright (c) 1999 - 2016 Intel Corporation. All rights
  reserved This software and associated documentation (if any)
  is furnished under a license and may only be used or copied in
  accordance with the terms of the license. Except as permitted
  by such license, no part of this software or documentation may
  be reproduced, stored in a retrieval system, or transmitted in
  any form or by any means without the express written consent
  of Intel Corporation.
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/

#include "ScratchPadReg.h"
#include "SysHostChip.h"
#include "SysFuncChip.h"
#include "MemApiSkx.h"

// BIOS scratchpad registers can not be assumed to have continuous offsets.
// Use the table below to map Scratchpad# to  CSR address offset
UINT32 BiosNonStickyScratchPadCsr[SCRATCH_PAD_NUM] = {
    BIOSNONSTICKYSCRATCHPAD0_UBOX_MISC_REG,
    BIOSNONSTICKYSCRATCHPAD1_UBOX_MISC_REG,
    BIOSNONSTICKYSCRATCHPAD2_UBOX_MISC_REG,
    BIOSNONSTICKYSCRATCHPAD3_UBOX_MISC_REG,
    BIOSNONSTICKYSCRATCHPAD4_UBOX_MISC_REG,
    BIOSNONSTICKYSCRATCHPAD5_UBOX_MISC_REG,
    BIOSNONSTICKYSCRATCHPAD6_UBOX_MISC_REG,
    BIOSNONSTICKYSCRATCHPAD7_UBOX_MISC_REG,
    BIOSNONSTICKYSCRATCHPAD8_UBOX_MISC_REG,
    BIOSNONSTICKYSCRATCHPAD9_UBOX_MISC_REG,
    BIOSNONSTICKYSCRATCHPAD10_UBOX_MISC_REG,
    BIOSNONSTICKYSCRATCHPAD11_UBOX_MISC_REG,
    BIOSNONSTICKYSCRATCHPAD12_UBOX_MISC_REG
};


#ifdef SSA_FLAG
//
// Routines to access scratch pad register
//
UINT32
readScratchPad_CMD (
  PSYSHOST host
  )
{
    UINT32 val = 0;
    val = 0x0000FFFF & ReadCpuPciCfgEx (host, 0,  0, EV_AGENT_SCRATCHPAD_CMD_REG_ADDRESS);
    return val;
}

VOID
writeScratchPad_CMD (
  PSYSHOST             host,
  UINT32               val
  )
{
  WriteCpuPciCfgEx (host, 0, 0, EV_AGENT_SCRATCHPAD_CMD_REG_ADDRESS, val);
}

UINT32
readScratchPad_DATA0 (
  PSYSHOST              host
  )
{
  UINT32 val = 0;
  val = ReadCpuPciCfgEx (host, 0,  0, EV_AGENT_SCRATCHPAD_DATA0_REG_ADDRESS);
  return val;
}

VOID
writeScratchPad_DATA0 (
  PSYSHOST              host,
  UINT32                val
  )
{
  WriteCpuPciCfgEx (host, 0, 0, EV_AGENT_SCRATCHPAD_DATA0_REG_ADDRESS, val);
}

UINT32
readScratchPad_DATA1 (
  PSYSHOST              host
  )
{
  UINT32 val = 0;
  val = ReadCpuPciCfgEx (host, 0,  0, EV_AGENT_SCRATCHPAD_DATA1_REG_ADDRESS);
  return val;
}

VOID
writeScratchPad_DATA1 (
  PSYSHOST              host,
  UINT32                val
  )
{
  WriteCpuPciCfgEx (host, 0, 0, EV_AGENT_SCRATCHPAD_DATA1_REG_ADDRESS, val);
}

UINT32
readScratchPad0 (
  PSYSHOST              host
  )
{
  UINT32 val = 0;
  val = ReadCpuPciCfgEx (host, 0,  0, EV_AGENT_SCRATCHPAD_CHECKPOINT_REG_ADDRESS);
  return val;
}

VOID
writeScratchPad_currentCheckPoint (
  PSYSHOST             host,
  UINT32               val
  )
{
  WriteCpuPciCfgEx (host, 0, 0, EV_AGENT_SCRATCHPAD_CURRENT_CHECKPOINT_REG_ADDRESS, val);
}

#else
// Future placeholder: BSSA code intentionally left out for now
#endif  //SSA_FLAG

/**

  Walks through all of the stored data and restores the scratchpad registers upon S3 Resume.

  @param host        - Pointer to sysHost
  @param socket      - Socket number

  @retval None

**/
VOID
MultiThreadS3ResumeScratchPadRestore(
  PSYSHOST              host,
  UINT8                 socket
  )
{
    UINT32  NumberOfDwords;
    UINT32  CurrentDwordCount;
    UINT32  HostDataDword;
    UINT32  *Buffer;
    UINT32  i;
    UINT32  currentDword;


    Buffer = (UINT32 *)host;
    NumberOfDwords = (sizeof(struct sysHost) / (sizeof(UINT32)) + 1);
    rcPrintf ((host, " host structure : %x  dword num:%x \n",  sizeof(struct sysHost),NumberOfDwords));
    //
    // Transfer
    //
    for (CurrentDwordCount = 0; CurrentDwordCount < NumberOfDwords; CurrentDwordCount += currentDword) {
      while (ReadCpuPciCfgEx (host, socket, 0, SR_MEMORY_DATA_STORAGE_DISPATCH_PIPE_CSR) != 0x00);

      currentDword = ((NumberOfDwords - CurrentDwordCount) <  SCRATCH_PAD_NUM ) ? (NumberOfDwords - CurrentDwordCount) : SCRATCH_PAD_NUM;

      for(i=0; i< currentDword;i++) {
        HostDataDword = Buffer[CurrentDwordCount + i];
        WriteCpuPciCfgEx (host, socket, 0,  BiosNonStickyScratchPadCsr[i], HostDataDword);
      }

      WriteCpuPciCfgEx (host, socket, 0, SR_MEMORY_DATA_STORAGE_DISPATCH_PIPE_CSR, 0x1);
    }

    while (ReadCpuPciCfgEx (host, socket, 0, SR_MEMORY_DATA_STORAGE_DISPATCH_PIPE_CSR) != 0x00);
    WriteCpuPciCfgEx (host, socket, 0, SR_MEMORY_DATA_STORAGE_DISPATCH_PIPE_CSR, 0x1);

    rcPrintf ((host, "Transfer complete\n"));
}

/**

  Returns the Dispatch Pipe CSR which is currently stored in Non Sticky Scratchpad 13.

  @param host        - Pointer to sysHost
  @param socket      - Socket number

  @retval UINT32 - returns Dispatch Pipe CSR.

**/
UINT32
GetDispatchPipeCsr(
  PSYSHOST                host,
  UINT8                   socket
  )
{
    UINT32 spReg;
    spReg = ReadCpuPciCfgEx(host, socket, 0, SR_MEMORY_DATA_STORAGE_DISPATCH_PIPE_CSR);
    SCRATCHPAD_DBG((host, "Scratchpad_Debug GetDispatchPipeCsr: Non Sticky Scratchpad13 = %x", spReg));
    return spReg;
}

/**

  Writes the Dispatch Pipe CSR which is currently stored in Non Sticky Scratchpad 13.

  @param host        - Pointer to sysHost
  @param socket      - Socket number
  @param data        - data to be written out

  @retval None.

**/
VOID
WriteDispatchPipeCsr(
  PSYSHOST              host,
  UINT8                 socket,
  UINT32                data
  )
{
    SCRATCHPAD_DBG((host, "Scratchpad_Debug WriteDispatchPipeCsr: Non Sticky Scratchpad13 = %x", data));
    WriteCpuPciCfgEx(host, socket, 0, SR_MEMORY_DATA_STORAGE_DISPATCH_PIPE_CSR, data);
}

/**

  If the mode passed in is Get (0) then return the state of the request to perform the JEDEC Init Sequence.
  This state is currently stored in BIOS Sticky Scratchpad 5 bit 24 (1 << 24).
  If this state is TRUE then perform some initialization prior to running the JEDEC Init Sequence.
  If the mode passed in is Set (1) then restore the scratchpad5 and scratchpad 6 to their original values with
  the exception of JEDEC Init Sequence request bit being cleared.

  @param host    - Pointer to sysHost
  @param socket  - Socket number
  @param mode    - 0 = Get, 1 = SET

  @retval UINT8 - returns the scratchpad value that indicates if the JEDEC Init Sequence Request should execute.

**/
UINT32
GetSetValFunctions(
  PSYSHOST                host,
  UINT8                   socket,
  UINT8                   mode,
  UINT32                  *valFuncs
  )
{
  if (mode == GET_REQ) {
    //
    // Get the JEDEC Init Sequence Request value
    //
    *valFuncs = ReadCpuPciCfgEx(host, socket, 0, BIOSSCRATCHPAD5_UBOX_MISC_REG);
    SCRATCHPAD_DBG((host, "GetValRunctions: scratchpad5 = 0x%x\n", *valFuncs));
  } else {
    //
    // Restore the registers affected by the JEDEC Init Sequence
    //
    WriteCpuPciCfgEx(host, socket, 0, BIOSSCRATCHPAD5_UBOX_MISC_REG, *valFuncs);
    SCRATCHPAD_DBG((host, "SetValRunctions: scratchpad5 = 0x%x\n", *valFuncs));
  }

  return *valFuncs;
}

/**

  Write the provided checkpoint code and data into the Non sticky scratchpad register 7.

  @param host       - Pointer to sysHost
  @param socket     - Socket number
  @param checkpoint - The major and minor codes combined in a UINT16 value
  @param data       - data to be written out

  @retval UINT32 - returns the value of the data that was written to the scratchpad register.

**/
UINT32
SetPostCode(
  PSYSHOST                host,
  UINT8                   socket,
  UINT16                  checkpoint,
  UINT16                  data
  )
{
    UINT32 csrReg;

    csrReg = (UINT32)((checkpoint << 16) | data);
    //
    // Write checkpoint to local scratchpad
    //
    SCRATCHPAD_DBG((host, "\nScratchpad_Debug SetPostCode: Non Sticky Scratchpad7 being set to:  %x\n   checkpoint = %x\n   data = %x\n", csrReg, checkpoint, data));
    SetNonStickyScratchpad (host, socket, SR_POST_CODE, csrReg);
    return csrReg;
}

/**

  Return the value of the break point CSR currently stored in the upper 16 bits of Sticky Scratchpad 6.

  @param host    - Pointer to sysHost
  @param socket  - Socket number

  @retval UINT32 - returns the scratchpad value for the breakpoint.

**/
UINT32
GetBreakpointCsr(
  PSYSHOST                host,
  UINT8                   socket
  )
{
    UINT32 spReg;
    spReg = GetStickyScratchpad(host, socket, SR_BIOS_SERIAL_DEBUG);
    SCRATCHPAD_DBG((host, "\nScratchpad_Debug GetBreakpointCsr: Getting Scratchpad6 and shift right by 16 = %x\n", (spReg >> 16)));
    return (spReg >> 16);
}

/**

  Return the value of the break point CSR currently stored in the upper 16 bits of Sticky Scratchpad 6.

  @param host    - Pointer to sysHost
  @param socket  - Socket number

  @retval UINT32 - returns the scratchpad value for the breakpoint.

**/
void
SetBreakpointCsr(
  PSYSHOST  host,
  UINT8     socket,
  UINT32    spReg
  )
{
  spReg = spReg << 16;
  spReg = spReg + (GetStickyScratchpad(host, socket, SR_BIOS_SERIAL_DEBUG) & 0xFF);
  SCRATCHPAD_DBG((host, "\nScratchpad_Debug GetBreakpointCsr: Setting Scratchpad6 and shift right by 16 = %x\n", (spReg >> 16)));
  SetStickyScratchpad(host, socket, SR_BIOS_SERIAL_DEBUG, spReg);
}

/**

  Return the value of the SSA Agent Data currently stored in the lower 16 bits of Sticky Scratchpad 6.

  @param host    - Pointer to sysHost
  @param socket  - Socket number

  @retval UINT32 - returns the scratchpad value for the SSA Agent Opcode.

**/
UINT32
GetSsaAgentData(
  PSYSHOST                host,
  UINT8                   socket
  )
{
    UINT32 spReg;
    spReg = GetStickyScratchpad(host, socket, SR_BIOS_SERIAL_DEBUG);

    SCRATCHPAD_DBG((host, "\nScratchpad_Debug GetBreakpointCsr: Getting Scratchpad6 & 0x0000FFFF = %x\n", (spReg & 0x0000FFFF)));
    return (spReg & 0x0000FFFF);
}

/**

  Clears the Breakpoint and SSA Agent Data which is currently all of Sticky Scratchpad 6.

  @param host    - Pointer to sysHost
  @param socket  - Socket number

  @retval VOID

**/
VOID
ClearBpAndAgentData(
  PSYSHOST                host,
  UINT8                   socket
  )
{
    SCRATCHPAD_DBG((host, "\nScratchpad_Debug GetBreakpointCsr: Setting Scratchpad6 to 0\n"));
    SetStickyScratchpad(host, socket, SR_BIOS_SERIAL_DEBUG, 0);
}

