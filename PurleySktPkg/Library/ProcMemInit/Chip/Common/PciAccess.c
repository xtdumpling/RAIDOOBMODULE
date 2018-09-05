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
#include "RcRegs.h"
#include "SysHostChip.h"

//
// Disable warning for unsued input parameters
//
#ifdef _MSC_VER
#pragma warning(disable : 4100)
#pragma warning(disable : 4306)
#endif

/**

    CSR trace in serial log file that can later be used

    @param host:     Pointer to sysHost structure on stack
    @param socket:  CPU socket ID
    @param regAddr - CSR register address
    @param reg     - CSR register name as defined in register header files
    @param data    - CSR data
    @param RwFlag  -  0 for Rd, or 1 for Wr

    @retval None

**/
VOID
PciCfgAccessTraceCap (
  PSYSHOST host,
  UINT8 socket,
  UINT8 *regAddr,
  UINT32 reg,
  UINT32 data,
  UINT8 RwFlag
)
{
  if (host == NULL) return;
#ifdef IA32
#ifdef MRC_TRACE
#ifdef SERIAL_DBG_MSG
  if (RwFlag == READ_ACCESS) {
    /*
     *Socket, Type, Address, Direction, Size, Data

     Where:
     - Socket = processor socket # that executes the instruction
     - Type = 0 for IO, or 1 for Memory
     - Address = 16-bit IO or 32-bit Memory address (the MMIO address will encode bus, device, function, offset according to MMCFG format)
     - Direction = 0 for Rd, or 1 for Wr
     - Size = 1, 2, or 4 bytes
     - Data = hex data corresponding to the size

     For example:

     *0, 0, 0x80, 1, 1, 0xA0
     *0, 1, 0x100800dc, 1, 4, 0x00000055
     *0, 1, 0x10000000, 0, 4, 0x36008086
     *0, 1, 0x10000008, 0, 1, 0x00
    */
    if (checkMsgLevel(host, SDBG_TRACE)) {
      if (!((reg  == SR_POST_CODE_CSR)||(reg  == SR_BIOS_SERIAL_DEBUG_CSR)||(reg  == SR_PRINTF_SYNC_CSR))) {
        getPrintFControl(host);
        rcPrintf ((host, "*%d, 1, 0x%x, 0, %d, ", socket, regAddr, reg >> 28));

        //
        // Determine register size
        //
        switch (reg >> 28) {
        case sizeof (UINT32):
          rcPrintf ((host, "0x%08x\n", (UINT32) data));
          break;

        case sizeof (UINT16):
          rcPrintf ((host, "0x%04x\n", (UINT16) data));
          break;

        case sizeof (UINT8):
          rcPrintf ((host, "0x%02x\n", (UINT8) data));
          break;

        default:
          rcPrintf ((host, "Invalid register size in reg = 0x%X.\n", reg));
          RC_ASSERT(FALSE, ERR_INVALID_REG_ACCESS, RC_ASSERT_MINOR_CODE_0);
        }
        releasePrintFControl(host);
      }
    }
  } else {
    if (checkMsgLevel(host, SDBG_TRACE)) {
      if (!((reg  == SR_POST_CODE_CSR)||(reg  == SR_BIOS_SERIAL_DEBUG_CSR)||(reg  == SR_PRINTF_SYNC_CSR))) {
        getPrintFControl(host);
        rcPrintf ((host, "*%d, 1, 0x%x, 1, %d, ", socket, regAddr, reg >> 28));

        //
        // Determine register size
        //
        switch (reg >> 28) {
        case sizeof (UINT32):
          rcPrintf ((host, "0x%08x\n", (UINT32) data));
          break;

        case sizeof (UINT16):
          rcPrintf ((host, "0x%04x\n", (UINT16) data));
          break;

        case sizeof (UINT8):
          rcPrintf ((host, "0x%02x\n", (UINT8) data));
          break;

        default:
          rcPrintf ((host, "Invalid register size in reg = 0x%X.\n", reg));
          RC_ASSERT(FALSE, ERR_INVALID_REG_ACCESS, RC_ASSERT_MINOR_CODE_1);
        }
        releasePrintFControl(host);
      }
    }
  }
  //
  // if SDBG_TRACE
  //
#endif
#endif
#endif
}

/**

    For a CSR write, generate "mov ...."  instruction in serial log file that can later be used by emulation BIOS

    @param host:     - Pointer to sysHost structure on stack
    @param regAddr - CSR register address
    @param reg        - CSR register name as defined in register header files
    @param data      - CSR data

    @retval None

**/
VOID
PciCfgAccessWriteToLog (
  PSYSHOST host,
  UINT8 *regAddr,
  UINT32 reg,
  UINT32 data
)
{
  if (host == NULL) return;
#ifdef IA32
#endif //IA32
}

/**

  Reads PCI configuration space using the MMIO mechanism

  @param host  - Pointer to the system host (root) structure
  @param socket  - CPU Socket Node number (Socket ID)
  @param reg   - input parameter "reg" uses the format in the Bus_Dev_Func_CFG.H files

  @retval Value in requested reg

**/
UINT32
ReadPciCfg (
  PSYSHOST host,
  UINT8    socket,
  UINT32   reg
  )
{
  UINT8   *regAddr;
  UINT32  data = 0;
  UINT32  bus = 0;
  CPU_CSR_ACCESS_VAR *CpuCsrAccessVar;
//SKXTODO: Cleanup further.  Think we should deprecate this and only allow it to access DMI/PCH devices on S0
  CpuCsrAccessVar = &host->var.CpuCsrAccessVarHost;
#ifdef DEBUG_PERFORMANCE_STATS
  CpuCsrAccessCount (host);
#endif // DEBUG_PERFORMANCE_STATS
  if (socket > 0) {
    CpuCsrAccessError (host, "Invalid socket number %u.\n", socket);
    RC_ASSERT(FALSE, ERR_RC_INTERNAL, RC_ASSERT_MINOR_CODE_20);
  }
  //
  // determine if targeting IIO bus or Uncore Bus (see if bus# filed of the register address = 0)
  //
  switch ((reg & 0x0ff00000) >> 20) {
  case 0:
    //
    // IIO Bus
    //
    bus = CpuCsrAccessVar->SocketFirstBus[socket];
    break;

  case 1:
    CpuCsrAccessError (host, "Invalid bus number 0x%2X.\n", (reg & 0x0ff00000) >> 20);
    RC_ASSERT(FALSE, ERR_RC_INTERNAL, RC_ASSERT_MINOR_CODE_21);

    //
    // Uncore Bus
    //
//    bus = CpuCsrAccessVar.busUncore[socket];
    break;

  case 2:
    //
    // PCH on Bus 0
    //
    bus = 0;
    break;

  default:
    //
    // non-defined value
    //
    bus = 0;
    CpuCsrAccessError (host, "Attempt to access undefined bus number.\n");
  }
  //
  // Form address
  //
  regAddr = (UINT8 *) (CpuCsrAccessVar->mmCfgBase + (bus << 20) + (reg & 0x000FFFFF));

  //
  // bus + Dev:Func:Offset
  // Check register size and read data
  //
  switch (reg >> 28) {
  case sizeof (UINT32):
    data = *(volatile UINT32 *) regAddr;
    break;

  case sizeof (UINT16):
    data = *(volatile UINT16 *) regAddr;
    break;

  case sizeof (UINT8):
    data = *(volatile UINT8 *) regAddr;
    break;

  default:
    CpuCsrAccessError (host, "Invalid register size in reg = 0x%X.\n", reg);
  }

  PciCfgAccessTraceCap(host, socket, regAddr, reg, data, READ_ACCESS);

  //
  // Return data
  //
  return data;
}

/**

  Writes specified data to PCI configuration space using the MMIO mechanism

  @param host  - Pointer to the system host (root) structure
  @param socket  - CPU Socket Node number (Socket ID)
  @param reg   - input parameter "reg" uses the format in the Bus_Dev_Func_CFG.H files
  @param data  - Value to write

  @retval N/A

**/
void
WritePciCfg (
  PSYSHOST host,
  UINT8    socket,
  UINT32   reg,
  UINT32   data
  )
{
  UINT8   *regAddr;
  UINT32  bus = 0;
  CPU_CSR_ACCESS_VAR *CpuCsrAccessVar;

  CpuCsrAccessVar = &host->var.CpuCsrAccessVarHost;
#ifdef DEBUG_PERFORMANCE_STATS
  CpuCsrAccessCount (host);
#endif //DEBUG_PERFORMANCE_STATS
  //
  // determine if targeting IIO bus or Uncore Bus (see if bus# filed of the register address = 0)
  //
//SKXTODO: Cleanup further.  Think we should deprecate this and only allow it to access DMI/PCH devices on S0
  if (socket > 0) {
    CpuCsrAccessError (host, "Invalid socket number %u.\n", socket);
    RC_ASSERT(FALSE, ERR_RC_INTERNAL, RC_ASSERT_MINOR_CODE_22);
  }

  switch ((reg & 0x0ff00000) >> 20) {
  case 0:
    //
    // IIO Bus
    //
    bus = CpuCsrAccessVar->SocketFirstBus[socket];
    break;

  case 1:
    CpuCsrAccessError (host, "Invalid bus number 0x%2X.\n", (reg & 0x0ff00000) >> 20);
    RC_ASSERT(FALSE, ERR_RC_INTERNAL, RC_ASSERT_MINOR_CODE_23);

    //
    // Uncore Bus
    //
//    bus = CpuCsrAccessVar.busUncore[socket];
    break;

  case 2:
    //
    // PCH on Bus 0
    //
    bus = 0;
    break;

  default:
    //
    // non-defined value
    //
    bus = 0;
    CpuCsrAccessError (host, "Attempt to access undefined bus number.\n");
  }
  //
  // Form address
  //
  regAddr = (UINT8 *) (CpuCsrAccessVar->mmCfgBase + (bus << 20) + (reg & 0x000FFFFF));
  //
  // bus + Dev:Func:Offset
  // Check register size and write data
  //
  switch (reg >> 28) {
  case sizeof (UINT32):
    *(volatile UINT32 *) regAddr = data;
    break;

  case sizeof (UINT16):
    *(volatile UINT16 *) regAddr = (UINT16) data;
    break;

  case sizeof (UINT8):
    *(volatile UINT8 *) regAddr = (UINT8) data;
    break;

  default:
    CpuCsrAccessError (host, "Invalid register size in reg = 0x%X.\n", reg);
  }

  PciCfgAccessTraceCap(host, socket, regAddr, reg, data, WRITE_ACCESS);
  PciCfgAccessWriteToLog (host, regAddr, reg, data);

}

/**

  Get PCI configuration space used the MMIO mechanism

  @param host  - Pointer to the system host (root) structure
  @param socket  - CPU Socket Node number (Socket ID)
  @param reg   - input parameter "reg" uses the format in the Bus_Dev_Func_CFG.H files

  @retval Address of requested reg

**/
UINT32
GetPciCfgAddress (
  PSYSHOST host,
  UINT8    socket,
  UINT32   reg
  )
{
  UINT32  bus = 0;
  CPU_CSR_ACCESS_VAR *CpuCsrAccessVar;
//SKXTODO: Cleanup further.  Think we should deprecate this and only allow it to access DMI/PCH devices on S0
//Added while(1) hangs for everything else for time being
  CpuCsrAccessVar = &host->var.CpuCsrAccessVarHost;
#ifdef DEBUG_PERFORMANCE_STATS
  CpuCsrAccessCount (host);
#endif // DEBUG_PERFORMANCE_STATS
  if (socket > 0) {
    CpuCsrAccessError (host, "Invalid socket number %u.\n", socket);
    RC_ASSERT(FALSE, ERR_RC_INTERNAL, RC_ASSERT_MINOR_CODE_24);
  }
  //
  // determine if targeting IIO bus or Uncore Bus (see if bus# filed of the register address = 0)
  //
  switch ((reg & 0x0ff00000) >> 20) {
  case 0:
    //
    // IIO Bus
    //
    bus = CpuCsrAccessVar->SocketFirstBus[socket];
    break;

  case 1:
    CpuCsrAccessError (host, "Invalid bus number 0x%2X.\n", (reg & 0x0ff00000) >> 20);
    RC_ASSERT(FALSE, ERR_RC_INTERNAL, RC_ASSERT_MINOR_CODE_25);

    //
    // Uncore Bus
    //
//    bus = CpuCsrAccessVar.busUncore[socket];
    break;

  case 2:
    //
    // PCH on Bus 0
    //
    bus = 0;
    break;

  default:
    //
    // non-defined value
    //
    bus = 0;
    CpuCsrAccessError (host, "Attempt to access undefined bus number.\n");
  }
  //
  // Form address
  //
  return (CpuCsrAccessVar->mmCfgBase + (bus << 20) + (reg & 0x000FFFFF));
}
