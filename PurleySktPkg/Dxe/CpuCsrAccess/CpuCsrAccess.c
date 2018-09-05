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

#include "CpuCsrAccess.h"

#pragma optimize("", off)

EFI_CPU_CSR_ACCESS_PROTOCOL     mCpuCsrAccessProtocol;
static EFI_IIO_UDS_PROTOCOL            *mIioUds;
IIO_UDS                        *mIioUdsData;

EFI_SMM_BASE2_PROTOCOL          *mSmmBase = NULL;
EFI_SMM_SYSTEM_TABLE2           *mSmst = NULL;

PSYSHOST host = NULL;
UINT32 CurrentCheckPoint;
extern CPU_CSR_ACCESS_VAR CpuCsrAccessVarGlobal;

/**

  Computes address of CPU Uncore & IIO PCI configuration space using the MMIO mechanism

  @param SocId     - CPU Socket Node number (Socket ID)
  @param BoxInst   - Box Instance, 0 based
  @param Offset    - Register offset; values come from the auto generated header file
  @param Size      - Register size in bytes (may be updated if pseudo-offset)

  @retval Address

**/
UINT64
GetCpuCsrAddress (
  UINT8    SocId,
  UINT8    BoxInst,
  UINT32   Offset,
  UINT8    *Size
  )
{
  return (UINT64) GetCpuPciCfgAddress (host, SocId, BoxInst, Offset, Size);
}


/**

  Reads CPU Uncore & IIO PCI configuration space using the MMIO mechanism

  @param SocId     - CPU Socket Node number (Socket ID)
  @param BoxInst   - Box Instance, 0 based
  @param RegOffset - Register offset; values come from the auto generated header file

  @retval Register value

**/
UINT32
ReadCpuCsr (
  UINT8    SocId,
  UINT8    BoxInst,
  UINT32   Offset
  )
{
  CSR_OFFSET  RegOffset;
  UINT8       boxType;

  RegOffset.Data = Offset;
  boxType = (UINT8)RegOffset.Bits.boxtype;

  if( boxType == BOX_MC || boxType == BOX_MCDDC || boxType == BOX_MCIO )
    return MemReadPciCfgEp(host, SocId, BoxInst, Offset);
  else
    return ReadCpuPciCfgEx(host, SocId, BoxInst, Offset);
}


/**

  Writes CPU Uncore & IIO PCI configuration space using the MMIO mechanism

  @param SocId     - CPU Socket Node number (Socket ID)
  @param BoxInst   - Box Instance, 0 based
  @param Offset    - Register offset; values come from the auto generated header file
  @param Data      - Register data to be written

  @retval None

**/
void
WriteCpuCsr (
  UINT8    SocId,
  UINT8    BoxInst,
  UINT32   Offset,
  UINT32   Data
  )
{
  CSR_OFFSET  RegOffset;
  UINT8       boxType;

  RegOffset.Data = Offset;
  boxType = (UINT8)RegOffset.Bits.boxtype;

  if( boxType == BOX_MC || boxType == BOX_MCDDC || boxType == BOX_MCIO )
    MemWritePciCfgEp(host, SocId, BoxInst, Offset, Data);
  else
    WriteCpuPciCfgEx(host, SocId, BoxInst, Offset, Data);
  return;
}

/**

   Writes the given command to BIOS to PCU Mailbox Interface CSR register

  @param socket  - CPU Socket number (Socket ID)
  @param dwordCommand - Pcu mailbox command to write
  @param dworddata - Pcu mailbox data

  @retval error code from the Pcu mailbox (0 = NO ERROR)

**/
UINT32
Bios2PcodeMailBoxWrite (
  UINT8  socket,
  UINT32 command,
  UINT32 data
  )
{
  return WriteBios2PcuMailboxCommand(host, socket, command, data);
}

/**

   Writes the given command to BIOS to PCU Mailbox Interface CSR register

  @param socket  - CPU Socket number (Socket ID)
  @param dwordCommand - Pcu mailbox command to write
  @param dworddata - Pcu mailbox data

  @retval error code from the Pcu mailbox (0 = NO ERROR)

**/
UINT64
Bios2VcodeMailBoxWrite (
  UINT8  socket,
  UINT32 command,
  UINT32 data
  )
{
  UINT64_STRUCT vcodeData;
  vcodeData = WriteBios2VcuMailboxCommand(host, socket, command, data);
  return (((UINT64)vcodeData.hi << 32) | vcodeData.lo);
}

/**

  Reads CPU Memory Controller configuration space using the MMIO mechanism

  @param SocId        - Socket ID
  @param McId         - Memory controller ID
  @param Offset        - Register offset; values come from the auto generated header file

  @retval Register value

**/
UINT32
ReadMcCpuCsr (
  UINT8    SocId,
  UINT8    McId,
  UINT32   Offset
  )
{
  return MemReadPciCfgMC(host, SocId, McId, Offset);
}

/**

  Writes CPU Memory Controller configuration space using the MMIO mechanism

  @param SocId        - Socket ID
  @param McId         - Memory controller ID
  @param Offset        - Register offset; values come from the auto generated header file
  @param Data          - Register data to be written

  @retval None

**/
void
WriteMcCpuCsr (
  UINT8    SocId,
  UINT8    McId,
  UINT32   Offset,
  UINT32   Data
  )
{
  MemWritePciCfgMC(host, SocId, McId, Offset, Data);
}

/**

  Get CPU Memory Controller configuration space address used by MMIO mechanism

  @param SocId        - Socket ID
  @param McId         - Memory controller ID
  @param Offset       - Register offset; values come from the auto generated header file

  @retval MC Register MMCFG address

**/
UINTN
GetMcCpuCsrAddress (
  UINT8    SocId,
  UINT8    McId,
  UINT32   Offset
  )
{
  return MemGetPciCfgMCAddr(host, SocId, McId, Offset);
}


/**

  Reads PCI configuration space using the MMIO mechanism

  @param reg   - input parameter "reg" uses the format in the Bus_Dev_Func_CFG.H files

  @retval Value in requested reg

**/
UINT32
ReadPciCsr (
  UINT8    socket,
  UINT32    reg
  )
{
  return ReadPciCfg (host, socket, reg);
}

/**

  Writes specified data to PCI configuration space using the MMIO mechanism

  @param reg   - input parameter "reg" uses the format in the Bus_Dev_Func_CFG.H files
  @param data  - Value to write

  @retval VOID

**/
void
WritePciCsr (
  UINT8    socket,
  UINT32   reg,
  UINT32   data
  )
{
  WritePciCfg(host, socket, reg, data);
}

/**

  Get PCI configuration space address used MMIO mechanism

  @param reg   - input parameter "reg" uses the format in the Bus_Dev_Func_CFG.H files

  @retval Address of requested reg

**/
UINT32
GetPciCsrAddress (
  UINT8    socket,
  UINT32    reg
  )
{
  return GetPciCfgAddress (host, socket, reg);
}

/**

  Writes the checkpoint code to the checkpoint CSR and breaks if match with debug breakpoint

  @param majorCode - Major Checkpoint code to write
  @param minorCode - Minor Checkpoint code to write
  @param data      - Data specific to the minor checkpoint is written to
                     low word of the checkpoint CSR

  @retval VOID

**/
void
BreakAtCheckpoint (
  UINT8    majorCode,
  UINT8    minorCode,
  UINT16   data
  )
{
  UINT16 checkpoint;
  UINT32 csrReg;
  UINT8  SocketId = 0;

  checkpoint = (UINT32)((majorCode << 8) | minorCode);
  csrReg = (UINT32)((checkpoint << 16) | data);

  //
  // Write checkpoint to local scratchpad
  //
  WriteCpuPciCfgEx (host, SocketId, 0, SR_POST_CODE_CSR, csrReg);

  //
  // Update checkpoint in the host structure
  //
  CurrentCheckPoint = csrReg;

  IoWrite8 (0x80, (UINT8)(CurrentCheckPoint >> 24));

  //
  // Output the progress code
  //
  DEBUG((EFI_D_INFO, "Checkpoint Code: Socket %d, 0x%02X, 0x%02X, 0x%04X\n", SocketId, majorCode, minorCode, data));

  //
  // Compare the breakpoint CSR
  //
  csrReg = ReadCpuPciCfgEx (host, SocketId,  0, SR_BIOS_SERIAL_DEBUG_CSR);
  if ((csrReg >> 16) == checkpoint) {
    DEBUG((EFI_D_INFO, "Breakpoint match found. S:%x waiting...\n", SocketId));
    // Stall the boot until breakpoint changes
    while (checkpoint == (ReadCpuPciCfgEx (host, SocketId,  0, SR_BIOS_SERIAL_DEBUG_CSR) >> 16));
  }
}

/**

  Update the CpuCsrAccessVarGlobal data structure from the IIO UDS

  @param  None

  @retval None

**/
void
UpdateCpuCsrAccessVar (
  void
  )
{
  UINT8 socket = 0, mc = 0, ch = 0, ctr;

  for (socket = 0; socket < MAX_SOCKET; socket++) {
    CpuCsrAccessVarGlobal.stackPresentBitmap[socket] = mIioUdsData->PlatformData.CpuQpiInfo[socket].stackPresentBitmap;
    CpuCsrAccessVarGlobal.SocketFirstBus[socket] = mIioUdsData->PlatformData.CpuQpiInfo[socket].SocketFirstBus;
    CpuCsrAccessVarGlobal.SocketLastBus[socket] = mIioUdsData->PlatformData.CpuQpiInfo[socket].SocketLastBus;
    CpuCsrAccessVarGlobal.segmentSocket[socket] = mIioUdsData->PlatformData.CpuQpiInfo[socket].segmentSocket;

    for (ctr = 0; ctr < MAX_IIO_STACK; ctr++) {
      CpuCsrAccessVarGlobal.StackBus[socket][ctr] = mIioUdsData->PlatformData.CpuQpiInfo[socket].StackBus[ctr];
    }

    for (mc = 0; mc < MAX_IMC; mc++) {
      CpuCsrAccessVarGlobal.imcEnabled[socket][mc] = mIioUdsData->SystemStatus.imcEnabled[socket][mc];
    }

    for (ch = 0; ch < MAX_CH; ch++) {
      CpuCsrAccessVarGlobal.mcId[socket][ch] = mIioUdsData->SystemStatus.mcId[socket][ch];
    }
  }
  CpuCsrAccessVarGlobal.cpuType = mIioUdsData->SystemStatus.cpuType;
  CpuCsrAccessVarGlobal.stepping = mIioUdsData->SystemStatus.MinimumCpuStepping;
  CpuCsrAccessVarGlobal.socketPresentBitMap = mIioUdsData->SystemStatus.socketPresentBitMap;
  CpuCsrAccessVarGlobal.FpgaPresentBitMap = mIioUdsData->SystemStatus.FpgaPresentBitMap;
  CpuCsrAccessVarGlobal.mmCfgBase = (UINT32)mIioUdsData->PlatformData.PciExpressBase;
  CpuCsrAccessVarGlobal.numChPerMC = mIioUdsData->SystemStatus.numChPerMC;
  CpuCsrAccessVarGlobal.maxCh = mIioUdsData->SystemStatus.maxCh;
  CpuCsrAccessVarGlobal.maxIMC = mIioUdsData->SystemStatus.maxIMC;

}

//
// Driver entry point
//
/**

    GC_TODO: add routine description

    @param ImageHandle - GC_TODO: add arg description
    @param SystemTable - GC_TODO: add arg description

    @retval EFI_SUCCESS - GC_TODO: add retval description

**/
EFI_STATUS
EFIAPI
CpuCsrAccessStart (
  IN EFI_HANDLE             ImageHandle,
  IN EFI_SYSTEM_TABLE       *SystemTable
  )
{
  EFI_STATUS                     Status;
  EFI_HANDLE                     Handle = NULL;
  BOOLEAN                        InSmm;

  //
  // Retrieve SMM Base Protocol
  //
  Status = gBS->LocateProtocol (
                  &gEfiSmmBase2ProtocolGuid,
                  NULL,
                  &mSmmBase
                  );
  if (mSmmBase == NULL) {
    InSmm = FALSE;
  } else {
    mSmmBase->InSmm (mSmmBase, &InSmm);
    mSmmBase->GetSmstLocation (mSmmBase, &mSmst);
  }

  // Locate the IIO Protocol Interface
  Status = gBS->LocateProtocol (&gEfiIioUdsProtocolGuid, NULL, &mIioUds);
  ASSERT_EFI_ERROR (Status);

  ZeroMem (&mCpuCsrAccessProtocol, sizeof (EFI_CPU_CSR_ACCESS_PROTOCOL));

  mIioUdsData = (IIO_UDS *)mIioUds->IioUdsPtr;

  mCpuCsrAccessProtocol.GetCpuCsrAddress       = GetCpuCsrAddress;
  mCpuCsrAccessProtocol.ReadCpuCsr             = ReadCpuCsr;
  mCpuCsrAccessProtocol.WriteCpuCsr            = WriteCpuCsr;
  mCpuCsrAccessProtocol.Bios2PcodeMailBoxWrite = Bios2PcodeMailBoxWrite;
  mCpuCsrAccessProtocol.Bios2VcodeMailBoxWrite = Bios2VcodeMailBoxWrite;
  mCpuCsrAccessProtocol.ReadMcCpuCsr           = ReadMcCpuCsr;
  mCpuCsrAccessProtocol.WriteMcCpuCsr          = WriteMcCpuCsr;
  mCpuCsrAccessProtocol.GetMcCpuCsrAddress     = GetMcCpuCsrAddress;
  mCpuCsrAccessProtocol.ReadPciCsr             = ReadPciCsr;
  mCpuCsrAccessProtocol.WritePciCsr            = WritePciCsr;
  mCpuCsrAccessProtocol.GetPciCsrAddress       = GetPciCsrAddress;
  mCpuCsrAccessProtocol.BreakAtCheckpoint      = BreakAtCheckpoint;
  mCpuCsrAccessProtocol.UpdateCpuCsrAccessVar  = UpdateCpuCsrAccessVar;



  UpdateCpuCsrAccessVar();

  if (InSmm) {
    Status = mSmst->SmmInstallProtocolInterface (
            &Handle,
            &gEfiCpuCsrAccessGuid,
            EFI_NATIVE_INTERFACE,
            &mCpuCsrAccessProtocol
            );
  } else {
  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gEfiCpuCsrAccessGuid,
                  EFI_NATIVE_INTERFACE,
                  &mCpuCsrAccessProtocol
                  );
  }

  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}
