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
#include "SysFuncChip.h"

#ifdef _MSC_VER
#pragma warning(disable : 4305)
#endif

#ifndef IA32
extern CPU_CSR_ACCESS_VAR CpuCsrAccessVarGlobal;
#endif

//
// Local Prototypes
//
//UINT32 GetMemPCIAddr (PSYSHOST host, UINT8 socket, UINT8 ChIdOrBoxInst, UINT32 Offset);

/**

  Converts NodeId, ChannelId to instance of type BoxType/FuncBlk based on
  Cpu type and cpu sub type

  @param host      - Pointer to the system host (root) structure
  @param NodeId    - Memory controller index
  @param BoxType   - Box Type; values come from CpuPciAccess.h
  @param ChannelId - DDR channel Id within a Memory controller, 0 based, 0xFF if not used
  @param FuncBlk   - Functional Block; values come from CpuPciAccess.h

  @retval Box Instance

**/
STATIC
UINT8
MemGetBoxInst (
    PSYSHOST host,
    UINT8    McId,
    UINT8    BoxType,
    UINT8    ChannelId,
    UINT8    FuncBlk,
    UINT8    CpuType,
    UINT8    MaxCh,
    UINT8    NumChPerMC
  )
{
  UINT8 BoxInst = 0xFF;

  switch (BoxType) {
  case BOX_MC:
    if (FuncBlk > 1) BoxInst = McId * 3;
    else BoxInst = ChannelId;
    break;

  case BOX_MCDDC:
    if (ChannelId < MaxCh) {
      BoxInst = ChannelId;             // One instance per DDR Channel
    } else {
      CpuCsrAccessError (host, "Wrong Channel ID parameter: 0x%08x passed with BOX_MCDDC\n", (UINT32) ChannelId);
    }
    break;

  case BOX_MCIO:
    switch (FuncBlk) {
    case MCIO_DDRIO:
    case MCIO_DDRIOEXT:
    case MCIO_DDRIOMCC:
    case MCIO_DDRIOEXTMCC:
      BoxInst = ChannelId;                                        // one instance per DDR Channel
      break;
    default:
      CpuCsrAccessError (host, "Invalid FuncBlk: 0x%08x passed with BOX_MCIO\n", (UINT32) FuncBlk);
    } // funcblk
    break;

  default:
    CpuCsrAccessError (host, "Invalid BoxType: 0x%08x passed\n", (UINT32) BoxType);
  }
  return BoxInst;
}

/**

  Returns regBase with true offset (after processing pseudo offset, if needed)

  @param host          - Pointer to the system host (root) structure
  @param NodeId        - Memory controller index
  @param ChIdOrBoxInst - DDR channel Id within a memory controller
                         or Box Instance (in case of non-MC boxes), 0 based, 0xFF if not used
  @param regBase       - Register offset; values come from the auto generated header file
                       - may be a pseudo offset

  @retval Updated Register OFfset

**/
/*
UINT32
MemPciCfgOffset (
  PSYSHOST host,
  UINT8    NodeId,
  UINT8    ChIdOrBoxInst,
  UINT32   regBase,
  UINT8    CpuType
  )
{
  CSR_OFFSET RegOffset;
  CSR_OFFSET TempReg;

  RegOffset.Data = regBase;

  //
  // Adjust offset if pseudo
  //
  if (RegOffset.Bits.pseudo) {
    TempReg.Data = GetRegisterOffset (host, RegOffset, CpuType);
    RegOffset.Bits.offset = TempReg.Bits.offset;
    RegOffset.Bits.size = TempReg.Bits.size;
    //
    // Clear offset bit
    //
    RegOffset.Bits.pseudo = 0;
  }

  //
  // Return updated reg offset
  //
  return RegOffset.Data;
}*/

/**

  Reads CPU Memory Controller configuration space using the MMIO mechanism

  @param host          - Pointer to the system host (root) structure
  @param socket          - Socket number
  @param ChIdOrBoxInst - DDR channel Id within a memory controller
                         or Box Instance (in case of non-MC boxes), 0 based, 0xFF if not used
  @param Offset        - Register offset; values come from the auto generated header file

  @retval Register value

**/
UINT32
MemReadPciCfgEp (
  PSYSHOST host,
  UINT8    socket,
  UINT8    ChIdOrBoxInst,
  UINT32   Offset
  )
{
  UINT8 BoxInst;
  UINT8 SocId;
  UINT8 McId;
  UINT8 BoxType;
  UINT8 FuncBlk;
  CSR_OFFSET RegOffset;
  CPU_CSR_ACCESS_VAR *CpuCsrAccessVar;

#if defined (IA32) || defined (SIM_BUILD) || defined(KTI_SW_SIMULATION) || defined (HEADLESS_MRC)
  CpuCsrAccessVar = &host->var.CpuCsrAccessVarHost;
#else
  CpuCsrAccessVar = &CpuCsrAccessVarGlobal;
#endif

  SocId = socket;
  McId = CpuCsrAccessVar->mcId[socket][ChIdOrBoxInst];

  RegOffset.Data = Offset;
  BoxType = (UINT8)RegOffset.Bits.boxtype;
  FuncBlk = (UINT8)RegOffset.Bits.funcblk;
  if (RegOffset.Bits.pseudo) {
    Offset = GetRegisterOffset (host, RegOffset, CpuCsrAccessVar->cpuType);
  }
  if ( BoxType == BOX_MC || BoxType == BOX_MCDDC || BoxType == BOX_MCIO) {
    if (ChIdOrBoxInst >= CpuCsrAccessVar->maxCh) {
      CpuCsrAccessError (host, "Assert on access to ch >= maxCh\n");
    }
    //
    // Need to convert the NodeId/DDR channel ID to box Instance for MC boxes
    //
    BoxInst = MemGetBoxInst (host, McId, BoxType, ChIdOrBoxInst, FuncBlk, CpuCsrAccessVar->cpuType, CpuCsrAccessVar->maxCh, CpuCsrAccessVar->numChPerMC);
  } else {
    //
    // For non-MC boxes pass the Box Instance directly
    //
    BoxInst = ChIdOrBoxInst;
  }

  return ReadCpuPciCfgEx (host, SocId, BoxInst, Offset);
}

/**
  Reads CPU Memory Controller configuration space using the MMIO mechanism

  @param host          - Pointer to the system host (root) structure
  @param socket        - Socket number
  @param mdID          - Memory controller ID
  @param Offset        - Register offset; values come from the auto generated header file

  @retval Register value

**/
UINT32
MemReadPciCfgMC_Ch (
                 PSYSHOST host,
                 UINT8    socket,
                 UINT8    ch,
                 UINT32   Offset,
                 UINT32   chOffset
                 )
{
  UINT8 mcId;
  UINT8 mcCh;
  CPU_CSR_ACCESS_VAR *CpuCsrAccessVar;

#if defined (IA32) || defined (SIM_BUILD) || defined(KTI_SW_SIMULATION) || defined (HEADLESS_MRC)
  CpuCsrAccessVar = &host->var.CpuCsrAccessVarHost;
#else
  CpuCsrAccessVar = &CpuCsrAccessVarGlobal;
#endif

  mcId = CpuCsrAccessVar->mcId[socket][ch];
  mcCh = ch % CpuCsrAccessVar->numChPerMC;
  return (MemReadPciCfgMC (host, socket, mcId, Offset + (chOffset * mcCh)));
}


/**

  Reads CPU Memory Controller configuration space using the MMIO mechanism

  @param host          - Pointer to the system host (root) structure
  @param socket        - Socket number
  @param mdID          - Memory controller ID
  @param Offset        - Register offset; values come from the auto generated header file

  @retval Register value

**/
UINT32
MemReadPciCfgMC (
  PSYSHOST host,
  UINT8    socket,
  UINT8    mcId,
  UINT32   Offset
  )
{
  UINT8 BoxInst;
  UINT8 SocId;
  UINT8 BoxType;
  CSR_OFFSET RegOffset;
  CPU_CSR_ACCESS_VAR *CpuCsrAccessVar;

#if defined (IA32) || defined (SIM_BUILD) || defined(KTI_SW_SIMULATION) || defined (HEADLESS_MRC)
  CpuCsrAccessVar = &host->var.CpuCsrAccessVarHost;
#else
  CpuCsrAccessVar = &CpuCsrAccessVarGlobal;
#endif
  SocId = socket;
  RegOffset.Data = Offset;
  BoxType = (UINT8)RegOffset.Bits.boxtype;

  if (BoxType == BOX_MCIO) {
    if (RegOffset.Bits.pseudo) {
      //CpuCsrAccessError (host, "\nMemReadPciCfgMC: Socket-%d WARNING MCIO with Pseudo Offset = 0x%x ", socket, Offset);
    }
  }

  if ( BoxType == BOX_MC || BoxType == BOX_MCDDC || BoxType == BOX_MCIO ) {

    // SKX hack for now...
    if (mcId >= CpuCsrAccessVar->maxIMC) {
      CpuCsrAccessError (host, "Assert on access to mc >= MAX_IMC\n");
    }

    //
    // Need to convert the NodeId/DDR channel ID to box Instance for
    // MC boxes
    //
    // RGT TODO: add support for 2ch per MC
    BoxInst = mcId * CpuCsrAccessVar->numChPerMC;

  } else {
    //CpuCsrAccessError (host, "Invalid BoxType: 0x%08x passed\n", (UINT32) BoxType);
    //
    // For non-MC boxes pass the Box Instance directly
    //
    BoxInst = mcId;
  }

  return ReadCpuPciCfgEx (host, SocId, BoxInst, Offset);
} // MemReadPciCfgMC

/**

  Writes CPU Memory Controller configuration space using the MMIO mechanism

  @param host          - Pointer to the system host (root) structure
  @param socket          - Socket number
  @param ChIdOrBoxInst - DDR channel Id within a memory controller
                         or Box Instance (in case of non-MC boxes), 0 based, 0xFF if not used
  @param Offset        - Register offset; values come from the auto generated header file
  @param Data          - Register data to be written

  @retval None

**/
void
MemWritePciCfgEp (
                 PSYSHOST host,
                 UINT8    socket,
                 UINT8    ChIdOrBoxInst,
                 UINT32   Offset,
                 UINT32   Data
                 )
{
  UINT8 BoxInst;
  UINT8 SocId;
  UINT8 BoxType;
  UINT8 McId;
  UINT8 FuncBlk;
  CSR_OFFSET RegOffset;
  CPU_CSR_ACCESS_VAR *CpuCsrAccessVar;

#if defined (IA32) || defined (SIM_BUILD) || defined(KTI_SW_SIMULATION) || defined (HEADLESS_MRC)
  CpuCsrAccessVar = &host->var.CpuCsrAccessVarHost;
#else
  CpuCsrAccessVar = &CpuCsrAccessVarGlobal;
#endif
  SocId = socket;
  McId = CpuCsrAccessVar->mcId[socket][ChIdOrBoxInst];

  RegOffset.Data = Offset;
  BoxType = (UINT8)RegOffset.Bits.boxtype;
  FuncBlk = (UINT8)RegOffset.Bits.funcblk;
  if (RegOffset.Bits.pseudo) {
    Offset = GetRegisterOffset (host, RegOffset, CpuCsrAccessVar->cpuType);
  }
  if ( BoxType == BOX_MC || BoxType == BOX_MCDDC || BoxType == BOX_MCIO) {
    if (ChIdOrBoxInst >= CpuCsrAccessVar->maxCh) {
      return;
    }
    //
    // Need to convert the NodeId/DDR channel ID to box Instance for MC boxes
    //
    BoxInst = MemGetBoxInst (host, McId, BoxType, ChIdOrBoxInst, FuncBlk, CpuCsrAccessVar->cpuType, CpuCsrAccessVar->maxCh, CpuCsrAccessVar->numChPerMC);
  } else {
    //
    // For non-MC boxes pass the Box Instance directly
    //
    BoxInst = ChIdOrBoxInst;
  }
  WriteCpuPciCfgEx (host, SocId, BoxInst, Offset, Data);
}

/**

  Writes CPU Memory Controller configuration space using the MMIO mechanism

  @param host          - Pointer to the system host (root) structure
  @param socket        - Socket number
  @param mcId          - Memory controller ID
  @param Offset        - Register offset; values come from the auto generated header file
  @param Data          - Register data to be written

  @retval None

**/
void
MemWritePciCfgMC_Ch (
                 PSYSHOST host,
                 UINT8    socket,
                 UINT8    ch,
                 UINT32   Offset,
                 UINT32   chOffset,
                 UINT32   Data
                 )
{
  UINT8 mcId;
  UINT8 mcCh;
  CPU_CSR_ACCESS_VAR *CpuCsrAccessVar;

#if defined (IA32) || defined (SIM_BUILD) || defined(KTI_SW_SIMULATION) || defined (HEADLESS_MRC)
  CpuCsrAccessVar = &host->var.CpuCsrAccessVarHost;
#else
  CpuCsrAccessVar = &CpuCsrAccessVarGlobal;
#endif
  mcId = CpuCsrAccessVar->mcId[socket][ch];
  mcCh = ch % CpuCsrAccessVar->numChPerMC;
  MemWritePciCfgMC (host, socket, mcId, Offset + (chOffset * mcCh), Data);
}


/**

  Writes CPU Memory Controller configuration space using the MMIO mechanism

  @param host          - Pointer to the system host (root) structure
  @param socket        - Socket number
  @param mcId          - Memory controller ID
  @param Offset        - Register offset; values come from the auto generated header file
  @param Data          - Register data to be written

  @retval None

**/
void
MemWritePciCfgMC (
  PSYSHOST host,
  UINT8    socket,
  UINT8    mcId,
  UINT32   Offset,
  UINT32   Data
  )
{
  UINT8 BoxInst;
  UINT8 SocId;
  UINT8 BoxType;
  CSR_OFFSET RegOffset;
  CPU_CSR_ACCESS_VAR *CpuCsrAccessVar;

#if defined (IA32) || defined (SIM_BUILD) || defined(KTI_SW_SIMULATION) || defined (HEADLESS_MRC)
  CpuCsrAccessVar = &host->var.CpuCsrAccessVarHost;
#else
  CpuCsrAccessVar = &CpuCsrAccessVarGlobal;
#endif
  SocId = socket;

  RegOffset.Data = Offset;
  BoxType = (UINT8)RegOffset.Bits.boxtype;

  if (BoxType == BOX_MCIO) {
    if (RegOffset.Bits.pseudo) {
      //CpuCsrAccessError (host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
      //                "MemWritePciCfgMC: WARNING MCIO with Pseudo Offset = 0x%x ", Offset);
    }
  }

  if ( BoxType == BOX_MC || BoxType == BOX_MCDDC || BoxType == BOX_MCIO ) {

    // SKX hack for now...
    if (mcId >= CpuCsrAccessVar->maxIMC) {
      CpuCsrAccessError (host, "Assert on access to mc >= MAX_IMC\n", mcId);
    }

    //
    // Need to convert the NodeId/DDR channel ID to box Instance for
    // MC boxes
    //
    // RGT TODO: add support for 2ch per MC
    BoxInst = mcId * CpuCsrAccessVar->numChPerMC;

  } else {
    //CpuCsrAccessError (host, "Invalid BoxType: 0x%08x passed\n", (UINT32) BoxType);
    //
    // For non-MC boxes pass the Box Instance directly
    //
    BoxInst = mcId;
  }
  WriteCpuPciCfgEx (host, SocId, BoxInst, Offset, Data);
} // MemWritePciCfgMC


UINTN
MemGetPciCfgMCAddr (
  PSYSHOST host,
  UINT8    socket,
  UINT8    mcId,
  UINT32   Offset
  )
{
  UINT8 BoxInst;
  UINT8 SocId;
  UINT8 BoxType;
  CSR_OFFSET RegOffset;
  CPU_CSR_ACCESS_VAR *CpuCsrAccessVar;

#if defined (IA32) || defined (SIM_BUILD) || defined(KTI_SW_SIMULATION) || defined (HEADLESS_MRC)
  CpuCsrAccessVar = &host->var.CpuCsrAccessVarHost;
#else
  CpuCsrAccessVar = &CpuCsrAccessVarGlobal;
#endif
  SocId = socket;

  RegOffset.Data = Offset;
  BoxType = (UINT8)RegOffset.Bits.boxtype;

  if (BoxType == BOX_MCIO) {
    if (RegOffset.Bits.pseudo) {
      //CpuCsrAccessError (host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
      //                "MemWritePciCfgMC: WARNING MCIO with Pseudo Offset = 0x%x ", Offset);
    }
  }

  if ( BoxType == BOX_MC || BoxType == BOX_MCDDC || BoxType == BOX_MCIO ) {

    // SKX hack for now...
    if (mcId >= CpuCsrAccessVar->maxIMC) {
      CpuCsrAccessError (host, "Assert on access to mc >= MAX_IMC\n", mcId);
    }

    //
    // Need to convert the NodeId/DDR channel ID to box Instance for
    // MC boxes
    //
    // RGT TODO: add support for 2ch per MC
    BoxInst = mcId * CpuCsrAccessVar->numChPerMC;

  } else {
    //CpuCsrAccessError (host, "Invalid BoxType: 0x%08x passed\n", (UINT32) BoxType);
    //
    // For non-MC boxes pass the Box Instance directly
    //
    BoxInst = mcId;
  }
  return GetCpuPciCfgExAddr (host, SocId, BoxInst, Offset);
} // MemGetPciCfgMCAddr

/**

Arguments:

  ch      - Channel to read
  RegBase - MMIO Reg address of first base device

  @retval EPMC main value

**/
UINT32
MemReadPciCfgMain (
  PSYSHOST  host,
  UINT8     socket,
  UINT32    regBase
  )
{
  UINT32  data = 0;
  UINT8 i;
  CPU_CSR_ACCESS_VAR *CpuCsrAccessVar;

#if defined (IA32) || defined (SIM_BUILD) || defined(KTI_SW_SIMULATION) || defined (HEADLESS_MRC)
  CpuCsrAccessVar = &host->var.CpuCsrAccessVarHost;
#else
  CpuCsrAccessVar = &CpuCsrAccessVarGlobal;
#endif

  for (i = 0; i < CpuCsrAccessVar->maxIMC; i++) {
    if (CpuCsrAccessVar->imcEnabled[socket][i]) {
      data = MemReadPciCfgMC (host, socket, i, regBase);
    }
  }
  return data;

} // MemReadPciCfgMain

/**

  Get a Memory channel's EPMC Main value

  @param ch      - Channel to read
  @param RegBase - MMIO Reg address of first base device

  @retval EPMC main value

**/
void
MemWritePciCfgMain (
  PSYSHOST  host,
  UINT8     socket,
  UINT32    regBase,
  UINT32    data
  )
{
  UINT8 i;
  CPU_CSR_ACCESS_VAR *CpuCsrAccessVar;

#if defined (IA32) || defined (SIM_BUILD) || defined(KTI_SW_SIMULATION) || defined (HEADLESS_MRC)
  CpuCsrAccessVar = &host->var.CpuCsrAccessVarHost;
#else
  CpuCsrAccessVar = &CpuCsrAccessVarGlobal;
#endif

  for (i = 0; i < CpuCsrAccessVar->maxIMC; i++) {
    if (CpuCsrAccessVar->imcEnabled[socket][i]) {
      MemWritePciCfgMC (host, socket, i, regBase, data);
    }
  }
} // MemWritePciCfgMain


// /**

  // Gets PCI MMIO address

  // @param host          - Pointer to the system host (root) structure
  // @param socket          - Socket number
  // @param ChIdOrBoxInst - DDR channel Id within a memory controller
                         // or Box Instance (in case of non-MC boxes), 0 based, 0xFF if not used
  // @param Offset        - Register offset; values come from the auto generated header file

  // @retval None

// **/
// UINT32
// GetMemPCIAddr (
  // PSYSHOST host,
  // UINT8    socket,
  // UINT8    ChIdOrBoxInst,
  // UINT32   Offset
  // )
// {
  // UINT8 BoxInst;
  // UINT8 SocId;
  // UINT8 BoxType;
  // UINT8 McId;
  // UINT8 FuncBlk;
  // UINT8 Size;
  // CSR_OFFSET RegOffset;
  // CPU_CSR_ACCESS_VAR CpuCsrAccessVar;

  // GetCpuCsrAccessVar (host, &CpuCsrAccessVar);
  // SocId = socket;
  // McId = CpuCsrAccessVar.mcId[socket][ChIdOrBoxInst];

  // RegOffset.Data = Offset;
  // BoxType = (UINT8)RegOffset.Bits.boxtype;
  // FuncBlk = (UINT8)RegOffset.Bits.funcblk;

  // if ( BoxType == BOX_MC || BoxType == BOX_MCDDC || BoxType == BOX_MCIO ) {
    // //
    // // Need to convert the NodeId/DDR channel ID to box Instance for MC boxes
    // //
    // BoxInst = MemGetBoxInst (host, McId, BoxType, ChIdOrBoxInst, FuncBlk, CpuCsrAccessVar.cpuType, CpuCsrAccessVar.maxCh, CpuCsrAccessVar.numChPerMC);
  // } else {
    // //
    // // For non-MC boxes pass the Box Instance directly
    // //
    // BoxInst = 0;
  // }

  // return(UINT32)GetCpuPciCfgAddress (host, SocId, BoxInst, Offset, &Size);
// }



