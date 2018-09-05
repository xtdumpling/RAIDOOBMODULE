//****************************************************************************
//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//****************************************************************************
//  File History
//
//  Rev. 1.00
//      Bug Fixed:  Log/Show MRC error/warning by major code (refer Intel Purley MRC Error Codes_draft_0.3.xlsx)
//      Reason:     
//      Auditor:    Jimmy Chiu
//      Date:       Jun/05/2017
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

#include "SysFunc.h"
#include "SysHostChip.h"
#ifndef IA32
#ifndef MINIBIOS_BUILD
#include <Library/TimerLib.h>
#include <Library/DebugLib.h>
#endif
#endif

/**

   Clear the RUN_BUSY flag by performing a ReadCpuPciCfgEx() poll with 
   a timeout of 5 Milli Seconds.

  @param host          - Pointer to the system host (root) structure
  @param socket        - CPU Socket Node number (Socket ID)
  @param BoxInst       - Box Instance, 0 based
  @param RegOffset     - Register offset
  @param MailboxStatus - Used to return the result of ReadCpuPciCfgEx()

  @retval If error return the value of the last read. If success return 0;

**/
UINT32
ClearRunBusyFlag (
  PSYSHOST host,
  UINT8    socket,
  UINT8    BoxInst,
  UINT32   RegOffset
  )
{
  UINT32 StartCount     = 500;
  UINT32 MailBoxStatus  = 0;

  MailBoxStatus = ReadCpuPciCfgEx (host, socket, BoxInst, RegOffset); //returns back data

  //After 5ms if run_busy is still 1, then SW cannot write to the CSRs; error needs to be O/P(last read is O/P)
  //Poll with a max timeout of 5ms
  while (MailBoxStatus & BIT31) {
#if defined(IA32) || defined(MINIBIOS_BUILD)
    FixedDelay (host, 10);
#else
    MicroSecondDelay (10);
#endif

    MailBoxStatus = ReadCpuPciCfgEx (host, socket, BoxInst, RegOffset);
    StartCount--;
    if (StartCount == 0) break;
  }

  if (MailBoxStatus & BIT31) {
    return MailBoxStatus;
  }
  
  return 0;
}

/**

  Writes the given command to the BIOS to PCU Mailbox Interface CSR register.

  @param host - pointer to sysHost structure on stack
  @param socket  - CPU Socket Node number (Socket ID)
  @param dwordCommand - Pcu mailbox command to write
  @param dworddata - Pcu mailbox data

  @retval: Error code from the Pcu mailbox (0 = NO ERROR)

**/
UINT32
WriteBios2PcuMailboxCommand (
  struct sysHost    *host,
  UINT8  socket,
  UINT32 dwordCommand,
  UINT32 dworddata
  )
{
  UINT32              MailBoxStatus = 0;

  //
  // Wait until pCode Mailbox is available (i.e. run_busy flag is clear)
  //
#if defined (IA32) || defined (SIM_BUILD)
#ifdef SERIAL_DBG_MSG
  if (checkMsgLevel(host, SDBG_MAX)) {
    rcPrintf ((host, "\n  Wait for mailbox ready\n"));
  }
#endif
#endif

  MailBoxStatus = ClearRunBusyFlag (host, socket, 0, BIOS_MAILBOX_INTERFACE);
  if (MailBoxStatus & BIT31) {
#if defined(IA32) || defined(MINIBIOS_BUILD)
    rcPrintf ((host, "\n  **ERROR! Timeout reached when trying to clear RUN_BUSY flag in PCU. \n"));
    RC_ASSERT (FALSE, ERR_PCU, PCU_NOT_RESPONDING);
#else
    DEBUG ((EFI_D_ERROR, "\n  **ERROR! Timeout reached when trying to clear RUN_BUSY flag in PCU. \n"));
    ASSERT (FALSE);
#endif

    return MailBoxStatus;
  }


  //
  // Write data to the BIOS to Pcode Mailbox data register (now that run_busy != 1)
  //
#if defined (IA32) || defined (SIM_BUILD)
#ifdef SERIAL_DBG_MSG
  if (checkMsgLevel(host, SDBG_MAX)) {
    rcPrintf ((host, "  Send Data portion of command.  Socket = %u Data sent == 0x%x\n", socket, dworddata));
  }
#endif //SERIAL_DBG_MSG
#endif
  WriteCpuPciCfgEx (host, socket, 0, BIOS_MAILBOX_DATA, (UINT32) dworddata);

  //
  // Write pcode mailbox command code (now that run_busy != 1)
  //
#if defined (IA32) || defined (SIM_BUILD)
#ifdef SERIAL_DBG_MSG
  if (checkMsgLevel(host, SDBG_MAX)) {
    rcPrintf ((host, "  Send Pcode mailbox command. Socket = %u Command sent == 0x%x\n", socket, dwordCommand));
  }
#endif // SERIAL_DBG_MSG
#endif
  WriteCpuPciCfgEx (host, socket, 0, BIOS_MAILBOX_INTERFACE, (UINT32) (dwordCommand | BIT31));

#if defined (IA32) || defined (SIM_BUILD)
#ifdef SERIAL_DBG_MSG
  if (checkMsgLevel(host, SDBG_MAX)) {
    rcPrintf ((host, "  Wait for mailbox ready\n"));
  }
#endif // SERIAL_DBG_MSG
#endif

  MailBoxStatus = ClearRunBusyFlag (host, socket, 0, BIOS_MAILBOX_INTERFACE);
  if (MailBoxStatus & BIT31) {
#if defined(IA32) || defined(MINIBIOS_BUILD)
    rcPrintf ((host, "\n  **ERROR! Timeout reached when trying to clear RUN_BUSY flag in PCU. \n"));
    RC_ASSERT (FALSE, ERR_PCU, PCU_NOT_RESPONDING);
#else
    DEBUG ((EFI_D_ERROR, "\n  **ERROR! Timeout reached when trying to clear RUN_BUSY flag in PCU. \n"));
    ASSERT (FALSE);
#endif
    return MailBoxStatus;
  }


  //
  // Read Mailbox data (updated/read by Pcode)
  //
  MailBoxStatus = ReadCpuPciCfgEx (host, socket, 0, BIOS_MAILBOX_INTERFACE);
#ifdef IA32
  if (MailBoxStatus & BIT31) { //probably means Pcode is hung as run_busy bit not cleared by it
#ifdef SERIAL_DBG_MSG
    rcPrintf ((host, "PCU Mailbox Not responding\n"));
#endif //SERIAL_DBG_MSG
#if SMCPKG_SUPPORT
    OutputError (host, ERR_PCU, PCU_NOT_RESPONDING, socket, 0xFF, 0xFF, 0xFF);
#else
    FatalError(host, ERR_PCU, PCU_NOT_RESPONDING);
#endif
  }
#endif //IA32


  // This function returns the error code from the PCU mailbox (0 = NO ERROR)
  // PCODE responds to the CMD(dwordCommand) and reads data(dworddata) written by SW but
  // if it finds an error in execution the error gets entered by PCODE in bits (7:0) and is O/P as below
  if (MailBoxStatus & 0xFF) {
#if defined (IA32) || defined (SIM_BUILD)
#ifdef SERIAL_DBG_MSG
    if (checkMsgLevel(host, SDBG_MAX)) {
      rcPrintf ((host, "\n  **ERROR! PCODE returned MailBox(MB) Error code = %d, MBCommand = %d, MBdata = %d. \n", (MailBoxStatus & 0xFF), dwordCommand, dworddata));
    }
#endif // SERIAL_DBG_MSG
#endif
  }

  return MailBoxStatus;
}


/**

   First  writes the given 32-bit data to VCU MAILBOX_DATA CSR,
   Then writes the given 32-bit Command to VCU MAILBOX_INTERFACE CSR,

  @param host - pointer to sysHost structure on stack
  @param socket  - CPU Socket number (Socket ID)
  @param dwordCommand - VCU mailbox command to be written
  @param dworddata - VCU mailbox data to be written

   @retval 64-bit return code from the VCU mailbox:
    @retval Lo Dword
       @retval [15:0] =  Command Response Code (success, timeout, etc)
       @retval [31:16] = rsvd
    @retval Hi Dword
       @retval [31:0] = Return data if applicable

**/
UINT64_STRUCT
WriteBios2VcuMailboxCommand (
  struct sysHost    *host,
  UINT8  socket,
  UINT32 dwordCommand,
  UINT32 dworddata
  )
{
  UINT8           RetryCount;
  UINT32          cmd32 =0;
  UINT32          data32 =0;
  UINT64_STRUCT   MAILBOXSTATUS;

#if defined (IA32) || defined (SIM_BUILD)
  rcPrintf ((host, "     ** WriteBios2VcuMailboxCommand(Socket%2d)\n", socket));
#endif

  MAILBOXSTATUS.hi = MAILBOXSTATUS.lo = 0;

  for (RetryCount = 0; RetryCount < BIOS_VCU_MAILBOX_TIMEOUT_RETRY; RetryCount++) {

    //
    // Poll Mailbox RUN_BUSY flag until clear (ready)
    //
    MAILBOXSTATUS.lo = ClearRunBusyFlag (host, socket, 0, CSR_MAILBOX_INTERFACE_VCU_FUN0_REG);
  if (MAILBOXSTATUS.lo & BIT31) {
#if defined(IA32) || defined(MINIBIOS_BUILD)
    rcPrintf ((host, "\n  **ERROR! Timeout reached when trying to clear RUN_BUSY flag in VCU. \n"));
    RC_ASSERT (FALSE, VCODE_MAILBOX_CC_TIMEOUT, 0);
#else
    DEBUG ((EFI_D_ERROR, "\n  **ERROR! Timeout reached when trying to clear RUN_BUSY flag in VCU. \n"));
    ASSERT (FALSE);
#endif
    MAILBOXSTATUS.lo = VCODE_MAILBOX_CC_TIMEOUT;
    return MAILBOXSTATUS;
  }

    //
    // Write the given data / command to mailbox, setting RUN_BUSY bit to 1
    //
#if defined (IA32) || defined (SIM_BUILD)
    rcPrintf ((host, "   **  Write 0x%x to VCU_MAILBOX_DATA CSR)\n", dworddata));
#endif
    WriteCpuPciCfgEx (host, socket, 0, CSR_MAILBOX_DATA_VCU_FUN0_REG, (UINT32) dworddata);

#if defined (IA32) || defined (SIM_BUILD)
    rcPrintf ((host, "   **  Write 0x%x to VCU_MAILBOX_INTERFACE CSR)\n", (BIT31 | dwordCommand) ));
#endif
    WriteCpuPciCfgEx (host, socket, 0, CSR_MAILBOX_INTERFACE_VCU_FUN0_REG, (UINT32) (BIT31 | dwordCommand));

    //
    // Poll Mailbox RUN_BUSY flag until clear (ready)
    //
    MAILBOXSTATUS.lo = ClearRunBusyFlag (host, socket, 0, CSR_MAILBOX_INTERFACE_VCU_FUN0_REG);
  if (MAILBOXSTATUS.lo & BIT31) {
#if defined(IA32) || defined(MINIBIOS_BUILD)
    rcPrintf ((host, "\n  **ERROR! Timeout reached when trying to clear RUN_BUSY flag in VCU. \n"));
    RC_ASSERT (FALSE, VCODE_MAILBOX_CC_TIMEOUT, 0);
#else
    DEBUG ((EFI_D_ERROR, "\n  **ERROR! Timeout reached when trying to clear RUN_BUSY flag in VCU. \n"));
    ASSERT (FALSE);
#endif
    MAILBOXSTATUS.lo = VCODE_MAILBOX_CC_TIMEOUT;
    return MAILBOXSTATUS;
  }

    //
    // Read return code from Command field of MAILBOX_INTERFACE[15:0]
    //
    cmd32 = ReadCpuPciCfgEx (host, socket, 0, CSR_MAILBOX_INTERFACE_VCU_FUN0_REG) & 0xffff;

#if defined (IA32) || defined (SIM_BUILD)
    rcPrintf ((host, "   **  Read return code from VCU_MAILBOX_INTERFACE CSR [15:0]): 0x%x\n", cmd32));
#endif
    //
    // Read return data from MAILBOX_DATA[31:0]
    //
    data32 = ReadCpuPciCfgEx (host, socket, 0, CSR_MAILBOX_DATA_VCU_FUN0_REG);
#if defined (IA32) || defined (SIM_BUILD)
    rcPrintf ((host, "   **  Read return code from VCU_MAILBOX_DATA CSR [31:0]): 0x%x\n", data32));
#endif

    //
    // if timeout then retry, else stop
    //
    if (cmd32 == VCODE_MAILBOX_CC_TIMEOUT) {
      continue;
    } else {
      break;
    }

  }

  MAILBOXSTATUS.lo = cmd32;
  MAILBOXSTATUS.hi = data32;
  return MAILBOXSTATUS;

}

