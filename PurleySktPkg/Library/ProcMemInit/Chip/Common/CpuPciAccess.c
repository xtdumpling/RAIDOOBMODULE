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

#include "DataTypes.h"
#include "PlatformHost.h"
#include "SysHostChip.h"
#include "SysFunc.h"
#include "CpuCsrAccessDefine.h"
#include "CpuPciAccess.h"

#include <UsraAccessType.h>

#define NUM_FNV_SIM  (MAX_CH * MAX_DIMM)


#include "PSEUDO_OFFSET.h"

#ifndef IA32
#include "Library/IoLib.h"
extern CPU_CSR_ACCESS_VAR CpuCsrAccessVarGlobal;
#endif



//
// Disable warning for unsued input parameters
//
#ifdef _MSC_VER
#pragma warning(disable : 4100)
#pragma warning(disable : 4013)
#pragma warning(disable : 4306)
#endif



/**

    Get maximum number of iMCs

    @param host - Pointer to the system host (root) structure

    @retval maxIMC - max number of iMCs

**/
UINT8
GetMaxImc (
  PSYSHOST host
  )
{
  UINT8 maxIMC = MAX_IMC;
  if (host != NULL) {
    maxIMC = host->var.mem.maxIMC;
  }
#ifndef IA32
  if (host == NULL) {
    maxIMC = CpuCsrAccessVarGlobal.maxIMC;
  }
#endif
  return maxIMC;
}


#ifdef DEBUG_PERFORMANCE_STATS
/**

    Count the CSR accesses.

    @param host - Pointer to the system host (root) structure

    @retval None

**/
VOID
CpuCsrAccessCount (
  PSYSHOST host
  )
{
#if defined (IA32) || defined (SIM_BUILD)
#ifdef SERIAL_DBG_MSG
  if (host != NULL) {
    countTrackingData(host, PCI_ACCESS, 1);
  }
#endif //SERIAL_DBG_MSG
#endif //#if defined (IA32) || defined (SIM_BUILD)
}
#endif //DEBUG_PERFORMANCE_STATS


/**

  Indetifies the read hardware register for given BoxType, unctional Block, & pseudo table offset

  @param host      - Pointer to the system host (root) structure
  @param Offset:  Bits <31:24> Bits <23:16>  Bit 15  Bits <14:12>  Bits <11:0>
                   Box Number  Func Number     Pseudo      Size      pseudo table Offset


  @retval Bits <31:24>  Bits <23:16>  Bit 15  Bits <14:12>  Bits <11:0>
   @retval Box Number Func Number       0       Size          PCICfg Offset


**/
UINT32
GetRegisterOffset (
  PSYSHOST host,
  CSR_OFFSET   RegOffset,
  UINT8    CpuType
  )
{
  UINT32   Reg = RegOffset.Data;
  UINT8    Error = 0, BoxType, FuncBlk;
  UINT32   PseudoOffset;
  UINT8    CpuIdx;

  //
  // Get the Pseduo table offset, Box Type, FunBlk
  //
  if (host->var.common.stepping < B0_REV_SKX) {
    CpuIdx = 0;
  } else {
    CpuIdx = 1;
  }

  PseudoOffset = RegOffset.Bits.offset;

  BoxType = (UINT8)RegOffset.Bits.boxtype;
  FuncBlk = (UINT8)RegOffset.Bits.funcblk;

  //
  // Clear the offset and size fields
  //
  Reg &= ~0x7fff;

  //
  // Translate the Box Type & Functional Block into PCI function number. Note that the box type & instance number
  // passed to this routine are assumed to be valid; here we only need to validate if the function number is correct
  // after the look up is done.
  //
  switch (BoxType) {
    case BOX_MC:
      switch (FuncBlk) {
        case MC_2LM:
          Reg |= MC_2LM_Offset[PseudoOffset][CpuIdx];
          break;
        default:
          Error = TRUE;
      }
      break;

    case BOX_KTI:
      switch (FuncBlk) {
            case KTI_LLPMON:
              Reg |= KTI_LLPMON_Offset[PseudoOffset][CpuIdx];
              break;
            default:
              Error = TRUE;
    }
    break;

  default:
    Error = TRUE;
  }

  if (Error == TRUE) {
    CpuCsrAccessError (host, "\nInvalid Pseudo Register Table for Box Type=%d, Functional Block=%d.\n", BoxType, FuncBlk);
  }

  return Reg;
}



/**

  Computes address of CPU Uncore & IIO PCI configuration space using the MMIO mechanism

  @param host      - Pointer to the system host (root) structure
  @param SocId     - CPU Socket Node number (Socket ID)
  @param BoxInst   - Box Instance, 0 based
  @param Offset    - Register offset; values come from the auto generated header file
  @param Size      - Register size in bytes (may be updated if pseudo-offset)

  @retval Address

**/
UINT8 *
GetCpuPciCfgAddress (
  PSYSHOST host,
  UINT8    SocId,
  UINT8    BoxInst,
  UINT32   Offset,
  UINT8    *Size
  )
{
  UINT32 Bus, Dev, Fun;
  UINT8 *RegAddr;
  UINT8 BoxType;
  UINT8 FuncBlk;
  CSR_OFFSET RegOffset;
  CPU_CSR_ACCESS_VAR *CpuCsrAccessVar;
  UINT32  SegOffset;
#if defined (IA32) || defined (SIM_BUILD) || defined(KTI_SW_SIMULATION) || defined (HEADLESS_MRC)
  CpuCsrAccessVar = &host->var.CpuCsrAccessVarHost;
#else
  CpuCsrAccessVar = &CpuCsrAccessVarGlobal;
#endif

  RegOffset.Data = Offset;

  BoxType = (UINT8)RegOffset.Bits.boxtype;
  FuncBlk = (UINT8)RegOffset.Bits.funcblk;

  if (RegOffset.Bits.pseudo) {
    RegOffset.Data = GetRegisterOffset (host, RegOffset, CpuCsrAccessVar->cpuType);
    *Size = (UINT8)RegOffset.Bits.size;
  }

  //
  // Identify the PCI Bus/Device/Function number for the access
  //
  Bus = GetBusNumber (host, SocId, BoxType, BoxInst, FuncBlk, CpuCsrAccessVar);
  Dev = GetDeviceNumber (host, BoxType, BoxInst, FuncBlk, CpuCsrAccessVar);
  Fun = GetFunctionNumber (host, BoxType, BoxInst, FuncBlk, CpuCsrAccessVar);
  SegOffset = CpuCsrAccessVar->segmentSocket[SocId];
  SegOffset = SegOffset * 256*1024*1024;

  //
  // Form address
  //
  RegAddr = (UINT8 *) ((CpuCsrAccessVar->mmCfgBase + SegOffset) | PCIE_REG_ADDR(Bus, Dev, Fun, RegOffset.Bits.offset));
  return RegAddr;
} // GetCpuPciCfgAddress




/**

  Reads CPU Uncore & IIO PCI configuration space using the MMIO mechanism

  @param host      - Pointer to the system host (root) structure
  @param SocId     - CPU Socket Node number (Socket ID)
  @param BoxInst   - Box Instance, 0 based
  @param RegOffset - Register offset; values come from the auto generated header file

  @retval Register value

**/
UINT32
ReadCpuPciCfgEx (
  PSYSHOST host,
  UINT8    SocId,
  UINT8    BoxInst,
  UINT32   Offset
  )
{
  UINT32 Data = 0;
  USRA_ADDRESS    Address;

  USRA_CSR_OFFSET_ADDRESS(Address, (UINT32)SocId, BoxInst, Offset, CsrBoxInst);
#if defined (IA32) || defined (SIM_BUILD) || defined(KTI_SW_SIMULATION) || defined (HEADLESS_MRC)
  Address.Attribute.HostPtr = (UINT32 ) host;
  Address.Attribute.ConvertedType = 1;
#else
  Address.Attribute.ConvertedType = 1;
  Address.Attribute.HostPtr = 0;
#endif
  RegisterRead(&Address, &Data);

  return Data;
}


/**

  Writes CPU Uncore & IIO PCI configuration space using the MMIO mechanism

  @param host      - Pointer to the system host (root) structure
  @param SocId     - CPU Socket Node number (Socket ID)
  @param BoxInst   - Box Instance, 0 based
  @param Offset    - Register offset; values come from the auto generated header file
  @param Data      - Register data to be written

  @retval None

**/
void
WriteCpuPciCfgEx (
  PSYSHOST host,
  UINT8    SocId,
  UINT8    BoxInst,
  UINT32   Offset,
  UINT32   Data
  )
{
  USRA_ADDRESS    Address;

  USRA_CSR_OFFSET_ADDRESS(Address, (UINT32)SocId, BoxInst, Offset, CsrBoxInst);

#if defined (IA32) || defined (SIM_BUILD) || defined(KTI_SW_SIMULATION) || defined (HEADLESS_MRC)
  Address.Attribute.HostPtr = (UINT32 ) host;
  Address.Attribute.ConvertedType = 1;
#else //defined (IA32) || defined (SIM_BUILD) || defined(KTI_SW_SIMULATION) || defined (HEADLESS_MRC)
  Address.Attribute.HostPtr = 0;
#endif //defined (IA32) || defined (SIM_BUILD) || defined(KTI_SW_SIMULATION) || defined (HEADLESS_MRC)
  RegisterWrite(&Address, &Data);

  return;
}

UINTN
GetCpuPciCfgExAddr (
  PSYSHOST host,
  UINT8    SocId,
  UINT8    BoxInst,
  UINT32   Offset
  )
{
  UINT8 *RegAddr;

#ifdef SEGMENT_ACCESS
  USRA_ADDRESS    Address;
#else
  UINT8 Size;
#endif

#ifdef DEBUG_PERFORMANCE_STATS
  CpuCsrAccessCount (host);
#endif //DEBUG_PERFORMANCE_STATS

#ifdef SEGMENT_ACCESS
  USRA_CSR_OFFSET_ADDRESS(Address, (UINT32)SocId, BoxInst, Offset, CsrBoxInst);
#if defined (IA32) || defined (SIM_BUILD) || defined(KTI_SW_SIMULATION) || defined (HEADLESS_MRC)
  Address.Attribute.HostPtr = (UINT32 ) host;
  Address.Attribute.ConvertedType = 1;
#else
  Address.Attribute.HostPtr = 0;
#endif
  RegAddr = (UINT8 *)GetRegisterAddress(&Address);
#else
  //
  // Form address
  //
  RegAddr = GetCpuPciCfgAddress (host, SocId, BoxInst, Offset, &Size);
#endif        //SEGMENT_ACCESS

  return (UINTN)RegAddr;
}

/**

Checks the number of HA's

@param host    - Pointer to the system host (root) structure

@retval number of HA's in this system

**/
UINT8
Is2HA (
      PSYSHOST  host
      )
{
  return(1);
}//Is2HA

/**

  Get MMCFG base and limit

  @param   Ptr to host structure

  @retval Value    32-bit MMCFG base address

**/
UINT32
GetMmcfgAddress(
  PSYSHOST host
  )
{
  UINT32 CsrLegacyAddr = 0;
  UINT32 Data32;
  UINT64_STRUCT  MsrData64;

//
// CAUTION !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//   host structure may or may not be fully initialized.
//   Care must be taken to NOT assume that all its data is valid and usable
//


//
// Pick the right MMCFG_RULE CSR address accordign to current CPU type
//
  switch (host->var.common.cpuType)  {

    case CPU_SKX:
    default:
       Data32 = SKX_LEGACY_CSR_MMCFG_RULE_N0_CHABC_SAD1_REG;

  }

  //
  // Use CF8/CFC IO port  to read legacy socket's CHA MMCFG_RULE CSR
  //

  // Get current Bus# from MSR 300
  MsrData64 = ReadMsrLocal(0x300);

  // Use Bus1 for CHA
  CsrLegacyAddr =  (Data32 & 0xff00ffff) | ((MsrData64.lo & 0x0000ff00) << 8);
  //
  // Read CSR via CF8/CFC IO
  //
#if defined (IA32) || defined (SIM_BUILD)
  OutPort32 (NULL, 0x0cf8, (0x80000000 | CsrLegacyAddr));
  Data32 = InPort32 (NULL, 0x0cfc);
#else
  IoWrite32 (0x0cf8, (0x80000000 | CsrLegacyAddr));
  Data32 = IoRead32 (0x0cfc);
#endif

  Data32 &= 0xfc000000;              // Mask Bits[25:0]
  return Data32;
}

UINT8
GetSbspSktId (
  PSYSHOST host
  )
/*++

  Get CPU socket id for the system boot strap processor
  This function only works on SBSP prior to KTIRC completion.

  @param host  - Pointer to the system host (root) structure

  @retval system boot strap processor socket ID

--*/
{
  if (host == NULL) {
    UINT32 sadTarget;
    UINT32 sadControl;
    UINT8 legacyPchTarget;
    UINT8 haltOnError;

#if defined (IA32) || defined (SIM_BUILD)
    UINT64_STRUCT  MsrData64;


    MsrData64 = ReadMsrLocal(0x300);

    //
    //get BusNo from MSR[300]
    //
    MsrData64.lo = (MsrData64.lo & 0x0000FF00) >> 8;


    OutPort32 (NULL, 0x0cf8, (0x80000000 | (MsrData64.lo << 16) | (29 << 11) | (1 << 8) | 0xF0));  // CSR 1:29:1:0xF0
    sadTarget = InPort32 (NULL, 0x0cfc);
    OutPort32 (NULL, 0x0cf8, (0x80000000 | (MsrData64.lo << 16) | (29 << 11) | (1 << 8) | 0xF4));  // CSR 1:29:1:0xF4
    sadControl = InPort32 (NULL, 0x0cfc);
#else
    //
    // SKXTODO: Should be able to eliminate use of this call in RAS flow
    //
    UINT32  Data32;
    IoWrite32 (0x0cf8, (0x80000000 | (0 << 16) | (8 << 11) | (2 << 8) | 0xCC));  // CSR 0:8:2:0xCC
    Data32 = IoRead32 (0x0cfc);
    Data32 = (Data32 << 8) & 0xFF0000;
    Data32 = Data32 | 0x80000000 | (29 << 11) | (1 << 8) | 0xF0;                 // CSR 1:29:1:0xF0
    IoWrite32 (0x0cf8, Data32);
    sadTarget = IoRead32 (0x0cfc);
    Data32 = (Data32 & 0xFFFFFF00) | 0xF4;                                       // CSR 1:29:1:0xF4
    IoWrite32 (0x0cf8, Data32);
    sadControl = IoRead32 (0x0cfc);
#endif
    legacyPchTarget = (UINT8)((sadTarget >> 4) & 0xF);
    //
    // If LegacyPchTarget[3] = 1 that means this is for local so SADCONTROL[2:0] = NodeID else for remote so SADTARGET[6:4] = NodeId
    //
    if (legacyPchTarget & BIT3) {
      return (sadControl & 0x7);
    } else {
      //
      // We choose SBSP based on legacy PCH location.   This can only be determined via register on the socket with the
      // legacy_pch prior to KTIRC programming this field on non-SBSP sockets.  Assert if this function  called from non-sbsp prior
      // to host structure available
      //
      haltOnError = 1;
      while (*(volatile UINT8 *) &haltOnError);
      return 0;
    }
  } else {
    return host->nvram.common.sbspSocketId;
  }
}

/**
  Enable the CSR address cache

  @param host  - Pointer to the system host (root) structure

**/
VOID
EnableCsrAddressCache(
  struct sysHost             *host
)
{
  host->CsrCachingEnable = 1;
  host->LastCsrAddress[0] = 0;
  host->LastCsrAddress[1] = 0;
}

/**
  Disable the CSR address cache

  @param host  - Pointer to the system host (root) structure
**/
VOID
DisableCsrAddressCache(
  struct sysHost             *host
)
{
  host->CsrCachingEnable = 0;
}
