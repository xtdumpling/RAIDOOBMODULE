/**
  This file contains 'Framework Code' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may not be modified, except as allowed by
  additional terms of your license agreement.
**/
/**
// **************************************************************************
// *                                                                        *
// *      Intel Restricted Secret                                           *
// *                                                                        *
// *      KTI Reference Code                                                *
// *                                                                        *
// *      ESS - Enterprise Silicon Software                                 *
// *                                                                        *
// *      Copyright (c) 2004 - 2016 Intel Corp.                             *
// *                                                                        *
// *      This program has been developed by Intel Corporation.             *
// *      Licensee has Intel's permission to incorporate this source code   *
// *      into their product, royalty free.  This source code may NOT be    *
// *      redistributed to anyone without Intel's written permission.       *
// *                                                                        *
// *      Intel specifically disclaims all warranties, express or           *
// *      implied, and all liability, including consequential and other     *
// *      indirect damages, for the use of this code, including liability   *
// *      for infringement of any proprietary rights, and including the     *
// *      warranties of merchantability and fitness for a particular        *
// *      purpose.  Intel does not assume any responsibility for any        *
// *      errors which may appear in this code nor any responsibility to    *
// *      update it.                                                        *
// *                                                                        *
// **************************************************************************
// **************************************************************************
// *                                                                        *
// *  PURPOSE:                                                              *
// *                                                                        *
// *                                                                        *
// **************************************************************************
**/

#include "DataTypes.h"
#include "KtiPlatform.h"
#include "SysHost.h"
#include "SysFunc.h"
#include "RcRegs.h"
#include "KtiLib.h"
#include "KtiMisc.h"
#include "KtiSetupDefinitions.h"
#include "KtiSi.h"
#include "EvAutoRecipe.h"
#include "SysHostChip.h"
// APTIOV_SERVER_OVERRIDE_RC_START
#include <AmiMrcHooksWrapperLib\AmiMrcHooksWrapperLib.h>
// APTIOV_SERVER_OVERRIDE_RC_END
UINT8
GetSiliconRevision(
  PSYSHOST  host,
  UINT8     Socket,
  UINT8     CpuStep,
  UINT8     Cf8Access
);

#ifdef RAS_SUPPORT
//
// Note: WriteCpuPciCfgRas()/ReadCpuPciCfgRas(), and if any, all RAS related code must be in the RAS folder and KTI RC can use extern.
// Otherwise, all RAS #define flag will not be available for KTI RAS build even with ifndef IA32.
// Fortunately, for now. KtiLib.c / WriteCpuPciCfgRas() is the only file and it only needs #define KTI_RAS_DEBUG as in KtiRas.h
//
#define KTI_RAS_DEBUG
#define KTI_RAS_DEBUG_REG_DETAIL
#pragma message( " X64 Compiling " __FILE__ )

#include <Protocol/QuiesceProtocol.h>

#define MAX_REG_OFFSET             0x200
#define MAX_BUF_PER_FUN            MAX_REG_OFFSET
#define MAX_BUF_PER_DEV            (8 * MAX_BUF_PER_FUN)
#define MAX_BUF_PER_BUS            (32 * MAX_BUF_PER_DEV)
#define MAX_SHADOW_BUS             4
#define MAX_BUF_PER_CPU            (MAX_SHADOW_BUS * MAX_BUF_PER_BUS) // creating shadow buffer only for 4 stacks

// Every bit keeps track of a BYTE
#define MAX_FLAG_BUF_PER_CPU       (MAX_BUF_PER_CPU / 8)

#define CPU_SHADOW_BUF_SIZE        (MAX_SOCKET * MAX_BUF_PER_CPU)
#define CPU_SHADOW_FLAG_SIZE       (MAX_SOCKET * MAX_FLAG_BUF_PER_CPU)

UINT8                       *mCpuShadowFlag = NULL;  // 16KB per CPU, to keep track of every byte of the shadow buffer
UINT8                       *mCpuShadowBuf  = NULL;  // 128KB per CPU, Byte/Word/Dword accesses
QUIESCE_DATA_TABLE_ENTRY    *mNextEntry     = NULL;  // Points to first available slot in the quiesce buffer; used as semaphore as well
BOOLEAN                     mDirectCommit   = TRUE;  // Flag to control whether register writes should be committed immediately

#else
#pragma message( " IA32 Compiling " __FILE__ )
#endif

/**

   Returns WayNess Type.

   @param host          - add argument and description to function comment
   @param SocId         - Soc ID of the CPU who's Ways is requested, 0...7
   @param SocketWays    - Socket ways

  @retval 0 - Successful
  @retval 1 - Failure
 **/
KTI_STATUS
GetSocketWays (
  struct sysHost   *host,
  UINT8            SocId,
  UINT8            *SocketWays
  )
{
  CAPID0_PCU_FUN3_STRUCT  Capid0;

  Capid0.Data = KtiReadPciCfg (host, SocId, 0, CAPID0_PCU_FUN3_REG);
  *SocketWays = (UINT8) Capid0.Bits.wayness;

  return KTI_SUCCESS;
}


/**

  Returns boolean if Sku has FPGA for a given CPU socket.

  @param host     - add argument and description to function comment
  @param SocId    - Soc ID of the CPU who's sku infor requested, 0...3

  @retval TRUE  - Fpga sku
  @retval FALSE - Not Fpga Sku

**/
BOOLEAN
IsSocketFpga (
  struct sysHost   *host,
  UINT8            SocId,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal
  )
{

  CAPID0_PCU_FUN3_STRUCT  Capid0;
  BOOLEAN                 rc = FALSE;
  //
  // If -P sku
  //
  Capid0.Data = KtiReadPciCfg (host, SocId, 0, CAPID0_PCU_FUN3_REG);
  if (Capid0.Bits.segment == SEGMENT_TYPE_FPGA) {
    rc = TRUE;
  }

  return rc;
}

/**

  Returns boolean if FPGA active for a given CPU socket.

  @param host     - add argument and description to function comment
  @param SocId    - Soc ID of the CPU who's enabled LLC slice info is requested, 0...3

  @retval TRUE  - BBS was downloaded for this socket
  @retval FALSE - BBS was not downloaded for this socket
**/
BOOLEAN
WasSocketFpgaInitialized (
  struct sysHost   *host,
  UINT8            SocId,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal
  )
{

  BOOLEAN                 rc = FALSE;
  //
  // If -P sku and attempting to enable FPGA and bitstream initialized
  // Only look at the spad[5] on sbsp
  // Todo: add
  //
  if ((SocketFpgasInitialized (host, KtiInternalGlobal)) && (IsSocketFpga(host, SocId, KtiInternalGlobal)) && (KtiInternalGlobal->FpgaEnable[SocId])) {
    rc = TRUE;
  }

  return rc;
}

/**

  Returns boolean if FPGA active for a given CPU socket.

  @param host     - add argument and description to function comment
  @param SocId    - Soc ID of the CPU

  @retval TRUE  - Blue Bitstream previously downloaded
  @retval FALSE - Blue Bitstream not already downloaded

**/
BOOLEAN
SocketFpgasInitialized (
  struct sysHost   *host,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal
  )
{

  BOOLEAN                 rc = FALSE;
  //
  // if Blue Bitstream already downloaded, return tru
  // Todo: Maybe switch to nvram variable instead of scratchpad
  //
  if (BIT5 & KtiReadPciCfg (host, KtiInternalGlobal->SbspSoc, 0, BIOSSCRATCHPAD1_UBOX_MISC_REG)) {
    rc = TRUE;
  }

  return rc;
}

/**

  Returns the total Chas and List of Chas (& LLCs) enabled for a given CPU socket.

  @param host     - add argument and description to function comment
  @param SocId    - Soc ID of the CPU who's enabled LLC slice info is requested, 0...3
  @param ChaCount - Total Chas in given socket
  @param ChaMap   - List of Chas in given socket

  @retval 0 - Successful
  @retval 1 - Failure

**/
KTI_STATUS
GetChaCountAndList (
  struct sysHost   *host,
  UINT8            SocId,
  UINT8            *CboCount,
  UINT32           *CboMap
  )
{
//Update for CHA count
  UINT8   Ctr, MaxCba;
  UINT8   Count = 0;
  UINT32  LlcMap = 0;
  CAPID6_PCU_FUN3_STRUCT  Capid6;

  //
  // Make sure we are not accessing more than what is supported for the given CPU
  //
  MaxCba = MAX_SKX_CHA;

  Capid6.Data = KtiReadPciCfg (host, SocId, 0, CAPID6_PCU_FUN3_REG);
  LlcMap = Capid6.Bits.llc_slice_en;

  for (Ctr = 0; Ctr < MaxCba; Ctr++) {
    if (LlcMap & (1 << Ctr)) {
       Count++;
    }
  }

  *CboCount = Count;
  *CboMap   = LlcMap;

  return KTI_SUCCESS;
}

/**

   Returns the total M2Mem enabled for a given CPU socket.

   @param host       - add argument and description to function comment
   @param SocId      - Soc ID of the CPU who's enabled M2Mem info is requested, 0...3
   @param M2MemCount - Total M2Mems in given socket

  @retval 0 - Successful
  @retval 1 - Failure

**/
KTI_STATUS
GetM2MemCount (
  struct sysHost   *host,
  UINT8            SocId,
  UINT8            *M2MemCount
  )
{
  UINT8   Ctr;
  UINT8   Count = 0;
  UINT16  Data16;

  for (Ctr = 0; Ctr < MAX_SKX_M2MEM; Ctr++) {
    Data16 = (UINT16) KtiReadPciCfg (host, SocId, Ctr, VID_M2MEM_MAIN_REG);
    if (Data16 != 0xFFFF) {
      Count++;
    }
  }

  *M2MemCount = Count;
  return KTI_SUCCESS;
}

/**

  Returns the total M2Pcie enabled for a given CPU socket.

  @param host        - add argument and description to function comment
  @param SocId       - Soc ID of the CPU who's enabled M2Pcie info is requested, 0...3
  @param M2PcieCount - Total M2Pcie in given socket

  @retval 0 - Successful
  @retval 1 - Failure

**/
KTI_STATUS
GetM2PcieCount (
  struct sysHost   *host,
  UINT8            SocId,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal
  )
{
  UINT8   Ctr;
  UINT8   Count = 0;
  UINT16  Data16;

  for (Ctr = 0; Ctr < MAX_SKX_M2PCIE; Ctr++) {
    Data16 = (UINT16) KtiReadPciCfg (host, SocId, Ctr, VID_M2UPCIE_MAIN_REG);
    if (Data16 != 0xFFFF) {
      host->var.common.M2PciePresentBitmap[SocId] |= 1 << (Ctr);
      Count++;
    }
  }
  host->var.kti.CpuInfo[SocId].TotM2Pcie = Count;

  // update active M2pcie bit map and count by checking with IIO stack bit map
  Count = 0;
  KtiInternalGlobal->M2PcieActiveBitmap[SocId] = (host->var.common.M2PciePresentBitmap[SocId] & (host->var.common.stackPresentBitmap[SocId] >> 1));
  for (Ctr = 0; Ctr < MAX_SKX_M2PCIE; Ctr++) {
    if (KtiInternalGlobal->M2PcieActiveBitmap[SocId] & (1 << Ctr)){
       Count++;
    }
  }

  KtiInternalGlobal->TotActiveM2Pcie[SocId] = Count;

  return KTI_SUCCESS;
}

/**

  Returns the total Iios enabled for a given CPU socket.

  @param host     - add argument and description to function comment
  @param SocId    - Soc ID of the CPU who's enabled IIOs info is requested, 0...3
  @param IIOCount - Total IIOs in given socket

  @retval 0 - Successful
  @retval 1 - Failure

**/
KTI_STATUS
GetIioCount (
  struct sysHost   *host,
  UINT8            SocId,
  UINT8            *IioCount
  )
{
  UINT8   Ctr;
  UINT8   Count = 0;
  UINT16  Data16;

  for (Ctr = 0; Ctr < MAX_IIO_STACK; Ctr++) {
    Data16 = (UINT16) KtiReadPciCfg (host, SocId, Ctr, VID_IIO_VTD_REG);
    if (Data16 != 0xFFFF) {
      Count++;
      host->var.common.stackPresentBitmap[SocId] |= 1 << (Ctr);
    }
  }
  host->var.kti.CpuInfo[SocId].CpuRes.stackPresentBitmap = host->var.common.stackPresentBitmap[SocId];
  *IioCount = Count;

  return KTI_SUCCESS;
}

/**

  Returns the total M3KTIs supported by this CPU socket.

  @param host       - add argument and description to function comment
  @param SocId      - Soc ID of the CPU who's enabled LLC slice info is requested, 0...3
  @param M3KtiCount - Total M3KTI in given socket

  @retval 0 - Successful
  @retval 1 - Failure

**/
KTI_STATUS
GetM3KTICount (
  struct sysHost   *host,
  UINT8            SocId,
  UINT8            *M3KtiCount
  )
{
  UINT8   Ctr;
  UINT8   Count = 0;
  UINT16  Data16;

  for (Ctr = 0; Ctr < MAX_SKX_M3KTI; Ctr++) {
    Data16 = (UINT16) KtiReadPciCfg (host, SocId, Ctr, VID_M3KTI_MAIN_REG);
    if (Data16 != 0xFFFF) {
      Count++;
    }
  }
  *M3KtiCount = Count;
  return KTI_SUCCESS;
}

/**

  Returns the total KTIAgents supported by this CPU socket.

  @param host          - add argument and description to function comment
  @param SocId         - Soc ID of the CPU who's enabled LLC slice info is requested, 0...3
  @param KtiAgentCount - Total KTIAgent in given socket

  @retval 0 - Successful
  @retval 1 - Failure

**/
KTI_STATUS
GetKtiAgentCount (
  struct sysHost   *host,
  UINT8            SocId,
  UINT8            *KtiAgentCount
  )
{
  UINT8   Ctr;
  UINT8   Count = 0;
  UINT16  Data16;

  for (Ctr = 0; Ctr < MAX_SKX_KTIAGENT; Ctr++) {
    Data16 = (UINT16) KtiReadPciCfg (host, SocId, Ctr, VID_KTI_LLPMON_REG);
    if (Data16 != 0xFFFF) {
      Count++;
    }
  }
  *KtiAgentCount = Count;
  return KTI_SUCCESS;
}


/**

  Update struc with Ras type

  @param host          - add argument and description to function comment
  @param SocId         - Soc ID of the CPU who's enabled Ras info is requested, 0...3
  @param KtiInternalGlobal - KTIRC internal variables.

  @retval 0 - Successful
  @retval 1 - Failure

**/
KTI_STATUS
GetRasType (
  struct sysHost   *host,
  UINT8            SocId,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal
  )
{
  CAPID0_PCU_FUN3_STRUCT  Capid0;
  CAPID1_PCU_FUN3_STRUCT  Capid1;
  CAPID5_PCU_FUN3_STRUCT  Capid5;
  UINT32                  RasType;

  Capid0.Data = KtiReadPciCfg (host, SocId, 0, CAPID0_PCU_FUN3_REG);
  Capid1.Data = KtiReadPciCfg (host, SocId, 0, CAPID1_PCU_FUN3_REG);
  Capid5.Data = KtiReadPciCfg (host, SocId, 0, CAPID5_PCU_FUN3_REG);

  if ((host->var.common.cpuType == CPU_SKX) && (host->var.common.stepping < B0_REV_SKX)) {
    if (Capid1.Bits.cpu_hot_add_en) {
      KtiInternalGlobal->RasType[SocId] = ADVANCED_RAS;
    } else if (Capid0.Bits.segment == 0) {
      KtiInternalGlobal->RasType[SocId] = BASIC_RAS_AX;
      if ((host->var.common.emulation & SIMICS_FLAG) && (Capid0.Bits.scalable == 1)) {
        KtiInternalGlobal->RasType[SocId] = STANDARD_RAS;
      }
    } else {
      KtiInternalGlobal->RasType[SocId] = STANDARD_RAS;
    }
  } else{
    /*
    CAPID5.28 CAPID0.7  CAPID0.6  Recipe
    RAS2      RAS1      RAS0

    0         1         1         Adv
    0         0         1         Std
    1         1         1         FPGA
    1         1         0         Core
    1         0         1         1SWS
    0         0         0         HEDT
    */

    RasType = 00;
    if(Capid0.Bits.basic_ras == 1)
      RasType |= 01;
    if(Capid0.Bits.advanced_ras == 1)
      RasType |= (01 << 01);
    if ((Capid5.Bits.spare2 & 2) == 2)
      RasType |= (01 << 02);

    switch ( RasType) {
      case 00:
        KtiInternalGlobal->RasType[SocId] = HEDT_RAS;
        break;
      case 05:
        KtiInternalGlobal->RasType[SocId] = S1WS_RAS;
        break;
      case 06:
        KtiInternalGlobal->RasType[SocId] = CORE_RAS;
        break;
      case 01:
        KtiInternalGlobal->RasType[SocId] = STANDARD_RAS;
        break;
      case 07:
        KtiInternalGlobal->RasType[SocId] = FPGA_RAS;
        break;
      case 03:
        KtiInternalGlobal->RasType[SocId] = ADVANCED_RAS;
        break;
      default :
        KtiInternalGlobal->RasType[SocId] = UNDEFINED_RAS;
        break;
    }
  }

  //
  // Set system to lowest common Ras set
  //
  if (host->var.kti.OutSystemRasType > KtiInternalGlobal->RasType[SocId]) {
    host->var.common.SystemRasType = host->var.kti.OutSystemRasType = KtiInternalGlobal->RasType[SocId];
  }

  return KTI_SUCCESS;
}

/**

  Update struct with CPU type

  @param host          - add argument and description to function comment
  @param SocId         - Soc ID of the CPU who's enabled Ras info is requested, 0...3

  @retval 0 - Successful

**/
KTI_STATUS
GetCpuType (
  struct sysHost   *host,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal,
  UINT8            SocId
  )
{
  if ((host->var.common.stackPresentBitmap[SocId] & (1 << IIO_PSTACK3)) != 0) {
    if (IsSocketFpga (host, SocId, KtiInternalGlobal) == TRUE) {
      host->var.kti.CpuInfo[SocId].CpuType = CPU_TYPE_P;      
    } else {
      host->var.kti.CpuInfo[SocId].CpuType = CPU_TYPE_F;
    }
  } else {
    host->var.kti.CpuInfo[SocId].CpuType = CPU_TYPE_STD;
  }

  return KTI_SUCCESS;
}

/**

  Hide MCP devices on all valid sockets

  @param host              - KTI input/output structure
  @param KtiInternalGlobal - KTIRC internal variables

  @retval KTI_SUCCESS - on successful completion

**/
KTI_STATUS
HideMcpDevices (
  struct sysHost   *host,
  KTI_SOCKET_DATA  *SocketData
  )
{
  UINT8  Socket, Func;
  UINT32 PcuDevhide, UboxDevhide, VcuDevhide;
  UINT32 Pstack3PcuDevhideRegs[]  = {DEVHIDE_B4F0_PCU_FUN6_REG, DEVHIDE_B4F1_PCU_FUN6_REG,
                                     DEVHIDE_B4F2_PCU_FUN6_REG, DEVHIDE_B4F3_PCU_FUN6_REG,
                                     DEVHIDE_B4F4_PCU_FUN6_REG, DEVHIDE_B4F5_PCU_FUN6_REG,
                                     DEVHIDE_B4F6_PCU_FUN6_REG, DEVHIDE_B4F7_PCU_FUN6_REG };
  UINT32 Pstack3UboxDevhideRegs[] = {DEVHIDE0_4_UBOX_MISC_REG,  DEVHIDE1_4_UBOX_MISC_REG,
                                     DEVHIDE2_4_UBOX_MISC_REG,  DEVHIDE3_4_UBOX_MISC_REG,
                                     DEVHIDE4_4_UBOX_MISC_REG,  DEVHIDE5_4_UBOX_MISC_REG,
                                     DEVHIDE6_4_UBOX_MISC_REG,  DEVHIDE7_4_UBOX_MISC_REG };
  UINT32 Pstack3VcuDevhideRegs[]  = {DEVHIDE_B4F0_VCU_FUN2_REG, DEVHIDE_B4F1_VCU_FUN2_REG,
                                     DEVHIDE_B4F2_VCU_FUN2_REG, DEVHIDE_B4F3_VCU_FUN2_REG,
                                     DEVHIDE_B4F4_VCU_FUN2_REG, DEVHIDE_B4F5_VCU_FUN2_REG,
                                     DEVHIDE_B4F6_VCU_FUN2_REG, DEVHIDE_B4F7_VCU_FUN2_REG };
  for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
    if (SocketData->Cpu[Socket].Valid == TRUE) {
      if (host->var.common.stackPresentBitmap[Socket] & IIO_PSTACK3) {
        //
        // We assume that upper 24 bits in devhide registers are the same
        // across all functions. This is true for SKX.
        //
        PcuDevhide = KtiReadPciCfg (host, Socket, 0, DEVHIDE_B4F0_PCU_FUN6_REG);
        PcuDevhide |= 0xff;
        UboxDevhide = KtiReadPciCfg (host, Socket, 0, DEVHIDE0_4_UBOX_MISC_REG);
        UboxDevhide |= 0xff;
        VcuDevhide = KtiReadPciCfg (host, Socket, 0, DEVHIDE_B4F0_VCU_FUN2_REG);
        VcuDevhide |= 0xff;
        for (Func = 0; Func < 8; Func++) {
          KtiWritePciCfg (host, Socket, 0, Pstack3PcuDevhideRegs[Func], PcuDevhide);
          KtiWritePciCfg (host, Socket, 0, Pstack3UboxDevhideRegs[Func], UboxDevhide);
          KtiWritePciCfg (host, Socket, 0, Pstack3VcuDevhideRegs[Func], VcuDevhide);
        }
        host->var.common.stackPresentBitmap[Socket] &= ~(1 << IIO_PSTACK3);
      } // PStack3 present
    } // socket valid
  } // for all sockets

  return KTI_SUCCESS;
}

/**

  Returns the Legacy Pch Socket Id for this CPU socket.

  @param host           - add argument and description to function comment
  @param SocId          - Soc ID of the calling CPU
  @param LegacyPchSktId - Legacy PCH Socket ID

  @retval 0 - Successful
  @retval 1 - Failure

**/
KTI_STATUS
GetLegacyPchTargetSktId (
  struct sysHost   *host,
  UINT8            SocId,
  UINT8            *LegacyPchSktId
  )
{

  *LegacyPchSktId = host->nvram.common.sbspSocketId;

  return KTI_SUCCESS;
}

/**

  Parse the Link Exchange Parameter (LEP) for a given CPU socket.

  @param host              - KTI input/output structure
  @param SocketData        - add argument and description to function comment
  @param KtiInternalGlobal - KTIRC internal variables.
  @param SocId             - CPU Socket Number, 0 based

  @retval 0 - found
  @retval 1 - not found

**/
KTI_STATUS
ParseCpuLep (
  struct sysHost             *host,
  KTI_SOCKET_DATA            *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal,
  UINT8                      SocId
  )
{
  KTILS_KTI_LLPMON_STRUCT          KtiLs;
  KTIPCSTS_KTI_LLPMON_STRUCT       PcSts;
  KTI_LEP0                Lep0;
  UINT8                   Port;
  UINT8                   PeerSocId;
  UINT8                   ApSocId;
  UINT16                  Data16;
  UINT32                  Data32;

  UINT32                  Ctr;
  UINT32                  WaitTime;
  UINT8                   FpgaPort = 0xFF;
  BOOLEAN                 CheckInFlag, UseKtiPcSts;

  KTI_ASSERT ((SocId < MAX_SOCKET), ERR_SW_CHECK, MINOR_ERR_SW_CHECK_7);

  //
  // Parse the LEP of the socket
  //
  KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\n   CPU%u Link Exchange", SocId));

  //
  // Detect if this socket is an FPGA SKU and indicate port number
  //
  if (IsSocketFpga (host, SocId, KtiInternalGlobal)) {
    FpgaPort = XEON_FPGA_PORT;
  }

  //
  // During RAS online operation, use KTILS_KTI_LLPMON_REG for pre-existing sockets because pcode will not be executed and KTIPCSTS_KTI_LLPMON_REG is updated by pcode.
  // The online socket will be using KTIPCSTS_KTI_LLPMON_REG as required as before if its LEP gets needed.
  //
  //
  UseKtiPcSts = (BOOLEAN) (((host->var.kti.RasInProgress != TRUE) || ((host->var.kti.RasInProgress == TRUE) && (host->var.kti.RasSocId == SocId))) ? TRUE : FALSE);
  KtiDebugPrintInfo1 ((host, KTI_DEBUG_INFO0, "\n   UseKtiPcSts = %u", UseKtiPcSts));

  if ((host->var.kti.RasInProgress == TRUE) && (host->var.kti.RasSocId == SocId)) {
    KtiDebugPrintInfo1 ((host, KTI_DEBUG_INFO0, "\n   ParseCpuLep() for online CPU%u", SocId));
  }

  for (Port = 0; Port < host->var.common.KtiPortCnt; Port++) {
    Data16 = (UINT16) KtiReadPciCfg (host, SocId, Port, VID_KTI_LLPMON_REG);
    if (Data16 == 0xFFFF) {
      continue;
    }

    PcSts.Data = KtiReadPciCfg (host, SocId, Port, KTIPCSTS_KTI_LLPMON_REG);
    KtiLs.Data = KtiReadPciCfg (host, SocId, Port, KTILS_KTI_LLPMON_REG);

    if ((UseKtiPcSts  && ((PcSts.Bits.ll_status == LINK_SM_INIT_DONE)  || (PcSts.Bits.ll_status == LINK_SM_CRD_RETURN_STALL))) ||
        (!UseKtiPcSts && ((KtiLs.Bits.init_state == LINK_SM_INIT_DONE) || (KtiLs.Bits.init_state == LINK_SM_CRD_RETURN_STALL)))) {
      //
      // Identify the Peer Port number
      //
      Lep0.RawData = KtiReadPciCfg (host, SocId,Port, KTILP0_KTI_LLPMON_REG);
      KTI_ASSERT ((Lep0.Fields.SendingPort < host->var.common.KtiPortCnt), ERR_UNSUPPORTED_TOPOLOGY, MINOR_ERR_INVALID_TOPOLOGY);

      PeerSocId = (UINT8)(Lep0.Fields.NodeId);
      if(PeerSocId >= MAX_SOCKET) {
        KtiDebugPrintFatal ((host, KTI_DEBUG_ERROR, "\n\n CPU%u Link %u has invalid LEP0", SocId, Port));
        KTI_ASSERT (FALSE, ERR_UNSUPPORTED_TOPOLOGY, MINOR_ERR_INVALID_TOPOLOGY);
      }

      KtiDebugPrintInfo0((host, KTI_DEBUG_INFO0, " : LEP%u(%u,%s%u)", Port, Lep0.Fields.SendingPort, (FpgaPort == Port) ? "PGA":"CPU", PeerSocId));

      // Invalidate this link if either the current port or its peer port were set to "Disabled" via Setup.
      if ((host->setup.kti.PhyLinkPerPortSetting[SocId].Link[Port].KtiPortDisable == TRUE) ||
          (host->setup.kti.PhyLinkPerPortSetting[PeerSocId].Link[Lep0.Fields.SendingPort].KtiPortDisable == TRUE)) {

        // Before disabling the link, if it's attached to the SBSP, make sure the AP (non-SBSP) socket
        // has completed its reset and has "checked-in" with the SBSP.  Otherwise,
        // disabling the link may cause MCA when socket becomes isolated.
        // Skip if Fpga EP
        if (((SocId == KtiInternalGlobal->SbspSoc) || (PeerSocId == KtiInternalGlobal->SbspSoc)) && (FpgaPort != Port)) {
          ApSocId = ((SocId == KtiInternalGlobal->SbspSoc) ? PeerSocId : SocId);  //Non-SBSP Socket
          CheckInFlag = FALSE;
          OemWaitTimeForPSBP(host, &WaitTime);
          for (Ctr = 0; Ctr < WaitTime; Ctr++) {
            Data32 = KtiReadPciCfg (host, (UINT8)ApSocId, 0, BIOSNONSTICKYSCRATCHPAD2_UBOX_MISC_REG);
            if ((Data32 & 0x1) != 0) {            //check bit 0 to indicate AP has checked-in
              CheckInFlag = TRUE;
              KtiDebugPrintInfo0((host, KTI_DEBUG_INFO0, "-Disabled"));
              break;
            }
            KtiFixedDelay(host, 1000); // 1000 microsecs
          }
          if (CheckInFlag == FALSE) {
            KtiDebugPrintFatal ((host, KTI_DEBUG_ERROR, "\n\n AP Socket %u is discovered but hasn't come out of reset. System Halted!!!", ApSocId));
            KTI_ASSERT (FALSE, ERR_UNSUPPORTED_TOPOLOGY, MINOR_ERR_INVALID_SOCKET_TYPE);
          }
        }//if SBSP link

        // SBSP neighbor (AP) socket has checked-in.  The link is no longer needed so mark it 'disabled' and move on to the next link.
        SocketData->Cpu[SocId].LinkData[Port].Valid = FALSE;
        continue;
      }//if link set to "disabled"

      //
      // Mark the Link as valid for the socket
      //
      SocketData->Cpu[SocId].LinkData[Port].Valid       = TRUE;
      SocketData->Cpu[SocId].LinkData[Port].PeerPort    = (UINT8) Lep0.Fields.SendingPort;
      SocketData->Cpu[SocId].LinkData[Port].PeerSocId   = PeerSocId;
      if (FpgaPort == Port) {
        SocketData->Cpu[SocId].LinkData[Port].PeerSocType = SOCKET_TYPE_FPGA;
      } else {
        SocketData->Cpu[SocId].LinkData[Port].PeerSocType = SOCKET_TYPE_CPU;
      }

    } else if (!SocketFpgasInitialized (host, KtiInternalGlobal)) {
      //
      // FPGA will not train until BBS is downloaded into it.  Prior to that we pretend it trained
      //
      if (FpgaPort == Port) {
        if (KtiInternalGlobal->FpgaEnable[SocId]) {
          SocketData->Cpu[SocId].LinkData[Port].Valid       = TRUE;
          SocketData->Cpu[SocId].LinkData[Port].PeerPort    = FPGA_PORT;
          SocketData->Cpu[SocId].LinkData[Port].PeerSocId   = SocId + FPGA_NID_OFFSET;
          SocketData->Cpu[SocId].LinkData[Port].PeerSocType = SOCKET_TYPE_FPGA;
          KtiDebugPrintInfo0((host, KTI_DEBUG_INFO0, " : SLEP%u(%u,%s%u)", Port, SocketData->Cpu[SocId].LinkData[Port].PeerPort, "PGA", SocketData->Cpu[SocId].LinkData[Port].PeerSocId));
        }
      }
    }
  }

  KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\n"));

  return KTI_SUCCESS;
}



/**

  Parse the Link Exchange Parameter (LEP) for a given Fpga.
  Currently we don't examine the LEP register, we assume it and
  update data strcuture for consistency

  @param host              - KTI input/output structure
  @param SocketData        - add argument and description to function comment
  @param KtiInternalGlobal - KTIRC internal variables.
  @param SocId             - Fpga Socket Number, 0 based

  @retval 0 - found
  @retval 1 - not found

**/
KTI_STATUS
ParseFpgaLep (
  struct sysHost             *host,
  KTI_SOCKET_DATA            *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal,
  UINT8                      SocId
  )
{
  KTI_LEP0                Lep0;
  UINT8                   PeerSocId;
  UINT8                   Port = FPGA_PORT;

  //
  // Fake the LEP of the Fpga
  //
  Lep0.RawData = 0;
  Lep0.Fields.SendingPort = XEON_FPGA_PORT;
  Lep0.Fields.NodeId = SocId - FPGA_NID_OFFSET;

  //
  // Identify the Peer Port number
  //
  PeerSocId = (UINT8)(Lep0.Fields.NodeId);
  KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\n   PGA%u Link Exchange\n", SocId));
  KtiDebugPrintInfo0((host, KTI_DEBUG_INFO0, "   LEP%u(%u,CPU%u)", Port, Lep0.Fields.SendingPort, PeerSocId));

  // Invalidate this link if either the current port or its peer port were set to "Disabled" via Setup
  if ((PeerSocId >= MAX_SOCKET) ||
      (host->setup.kti.PhyLinkPerPortSetting[SocId].Link[Port].KtiPortDisable == TRUE) ||
      (host->setup.kti.PhyLinkPerPortSetting[PeerSocId].Link[Lep0.Fields.SendingPort].KtiPortDisable == TRUE)) {
    SocketData->Cpu[SocId].LinkData[Port].Valid = FALSE;
  } else {
    //
    // Mark the Link as valid for the socket
    //
    SocketData->Cpu[SocId].LinkData[Port].Valid       = TRUE;
    SocketData->Cpu[SocId].LinkData[Port].PeerPort    = (UINT8) Lep0.Fields.SendingPort;
    SocketData->Cpu[SocId].LinkData[Port].PeerSocId   = PeerSocId;
    SocketData->Cpu[SocId].LinkData[Port].PeerSocType = SOCKET_TYPE_CPU;
  }

  KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\n"));

  return KTI_SUCCESS;
}

/**

  Ration the Bus/IO/MMIO/IOAPIC resources using the ratio specified for the CPU
  sockets populated. If equl distribution is not possible, assign the remainings resources
  to the CPUs with lowest NID. Legacy CPU is always allocated space at the base of the
  resource region. Other CPUs are assigned resources in the increasing order of Socket ID.
  Example: CPU2's resource base address will be greater than CPU1's resource base address.

  @param host              - Input/Output structure of the KTIRC
  @param SocketData        - CPU related info
  @param KtiInternalGlobal - KTIRC internal variables.

  @retval KTI_SUCCESS

**/
KTI_STATUS
AllocateIioResources (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal
  )
{
  UINT8          TmpTotCpu;
  UINT8          BusSize[MAX_SOCKET];

  KtiCheckPoint(0xFF, 0xFF, 0xFF, STS_PROGRAM_FINAL_IO_SAD, MINOR_STS_ALLOCATE_CPU_RESOURCE, host);

  if (KtiInternalGlobal->KtiCpuSktHotPlugEn == TRUE) {
    if (host->setup.kti.KtiCpuSktHotPlugTopology == 0) {
      TmpTotCpu =  4;         //online/offline based on 4S topology
    } else {
      TmpTotCpu =  8;         //online/offline based on 8S topology
    }
  } else {
    TmpTotCpu =  KtiInternalGlobal->TotCpu;
  }

  //
  // Check we don't overrun structures
  //
  KTI_ASSERT ((TmpTotCpu <= MAX_SOCKET), ERR_SW_CHECK, MINOR_ERR_SW_CHECK_8);

  //
  // Fill in stack structure for hotplug systems based on SBSP.  This allows for preallocation of resources
  //
  if (KtiInternalGlobal->KtiCpuSktHotPlugEn == TRUE) {
    UpdateStructureForHotlug (host, SocketData, KtiInternalGlobal, TmpTotCpu);
  }

  // Calculate the resources for each CPU based on input given to KTIRC
  CalculateBusResourceRatio (host, SocketData, KtiInternalGlobal, BusSize, TmpTotCpu);
  CalculateBusResources (host, SocketData, KtiInternalGlobal, BusSize, TmpTotCpu);
  CalculateIoResources (host, SocketData, KtiInternalGlobal, TmpTotCpu);
  CalculateIoApicResources (host, SocketData, KtiInternalGlobal);
  CalculateMmiolResources (host, SocketData, KtiInternalGlobal, TmpTotCpu);
  CalculateMmiohResources (host, SocketData, KtiInternalGlobal, TmpTotCpu);

  //
  // Check for OEM override
  //
  CheckForOemResourceUpdate (host, SocketData, KtiInternalGlobal);

  //
  // Get min granularity per stack for later perf programming.
  //
  CalculateMmiolGranularity (host, SocketData, KtiInternalGlobal);
  CalculateMmiohGranularity (host, SocketData, KtiInternalGlobal);

  //
  // Make sure at least 32M of MMCFG space is available for each CPU.
  //
  if (host->setup.common.mmCfgSize < (UINT32)(TmpTotCpu * 32 * 1024 * 1024)) {
    KtiCheckPoint(0xFF, 0xFF, 0xFF, ERR_SAD_SETUP, MINOR_ERR_INSUFFICIENT_RESOURCE_SPACE, host);
    KtiDebugPrintFatal((host, KTI_DEBUG_ERROR, "\n Invalid MMCFG Size Supplied. "));
    KtiDebugPrintFatal((host, KTI_DEBUG_ERROR, "\n Each CPU must be allocated at least 32MB of MMCFG space"));
    KTI_ASSERT(FALSE, ERR_SAD_SETUP, MINOR_ERR_INSUFFICIENT_RESOURCE_SPACE);
  }


  //
  // Export Granularity info in IioUds
  //
  host->var.kti.OutIoGranularity    = SAD_LEG_IO_GRANTY;
  host->var.kti.OutMmiolGranularity = SAD_MMIOL_GRANTY;

  host->var.kti.OutMmiohGranularity.lo = 1;
  host->var.kti.OutMmiohGranularity.hi = 0;
  host->var.kti.OutMmiohGranularity = LShiftUINT64(host->var.kti.OutMmiohGranularity, KtiInternalGlobal->MmiohGranularity);

  //
  // Dump Resource allocation
  //
  DataDumpKtiResourceAllocation(host, SocketData, KtiInternalGlobal);

  return KTI_SUCCESS;
}


/**

  For Hotplug systems, this routine updates the internal IIO stack structures to allow for preallocation of system resources.

  @param host              - Input/Output structure of the KTIRC
  @param SocketData        - CPU related info
  @param KtiInternalGlobal - KTIRC internal variables.
  @param TmpTotCpu         - NumberofCpus for purpose of resource allocation

  @retval KTI_SUCCESS

**/
KTI_STATUS
UpdateStructureForHotlug (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal,
  UINT8                TmpTotCpu
  )
{
  UINT8               Stack;
  UINT8               Socket;

  //
  // Check input
  //
  KTI_ASSERT ((TmpTotCpu <= MAX_SOCKET), ERR_SW_CHECK, MINOR_ERR_SW_CHECK_9);

  for (Socket = 0; Socket < TmpTotCpu; Socket++) {
    if (SocketData->Cpu[Socket].Valid != TRUE) {
      for (Stack=0; Stack < MAX_IIO_STACK; Stack++) {
        host->var.kti.CpuInfo[Socket].CpuRes.StackRes[Stack].Personality = host->var.kti.CpuInfo[KtiInternalGlobal->SbspSoc].CpuRes.StackRes[Stack].Personality;
        if ((host->var.kti.CpuInfo[Socket].CpuRes.StackRes[Stack].Personality == TYPE_UBOX_IIO) || (host->var.kti.CpuInfo[Socket].CpuRes.StackRes[Stack].Personality == TYPE_MCP)) {
          KtiInternalGlobal->TotSadTargets = KtiInternalGlobal->TotSadTargets + 1;
        }
      }
    }
  }

  return KTI_SUCCESS;

}

/**

  This routine assigns resources for CPUs that are populated. It consults the appropriate inputs given to KTIRC and
  applies the following set of rules:
    1. Resource will be allocated only for the CPUs that are currently populated.
       (Except IOAPIC range which is always reserved for all 4 sockets; this makes ACPI & other IOAPIC related setting easier)
    2. If a CPU is populated, bus resource must be requested by the platform. Otherwise force the resource ratio to 1 for that CPU.

  @param host              - Input/Output structure of the KTIRC
  @param SocketData        - CPU related info
  @param KtiInternalGlobal - KTIRC internal variables.
  @param BusSize           - Calculated Bus size for CPUs 0..3 (Granularity is 32 buses)
  @param TmpTotCpu         - NumberofCpus for purpose of resource allocation

  @retval KTI_SUCCESS

**/
KTI_STATUS
CalculateBusResourceRatio (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal,
  UINT8                  *BusSize,
  UINT8                TmpTotCpu
  )
{
  BOOLEAN        WarnFlag;
  UINT8          Idx1, Idx2, AvailBusChunk;
  UINT8          TotBusChunk;
  UINT8          ResiBusChunk;
  UINT8          TmpBusRatio[MAX_SOCKET];
  UINT8          MaxSocket;
  UINT32         SocketMmcfgSize;
  UINT8          TmpBusSize;
  UINT8          FpgaCount = 0;



  //
  // Check input
  //
  KTI_ASSERT ((TmpTotCpu <= MAX_SOCKET), ERR_SW_CHECK, MINOR_ERR_SW_CHECK_A);

  MaxSocket = TmpTotCpu;

  if (host->setup.common.mmCfgSize > 256*1024*1024) {
    //
    // Multiseg mode.  Assign at most a whole segment to each socket
    //

    SocketMmcfgSize = ((host->setup.common.mmCfgSize / MaxSocket) >> 20);
    if (SocketMmcfgSize > 256) {
      SocketMmcfgSize = 256;
    }

    //
    // Assign SBSP first
    //
    host->var.kti.CpuInfo[KtiInternalGlobal->SbspSoc].CpuRes.PciSegment = 0;
    TmpBusSize = BusSize[KtiInternalGlobal->SbspSoc] = (UINT8)(SocketMmcfgSize/SAD_BUS_GRANTY);

    if (TmpBusSize % SAD_BUS_TARGETS == 0) {
      Idx2 = 1;
    } else {
      Idx2 = 0;
    }

    for (Idx1 = 0; Idx1 < MAX_SOCKET; Idx1++) {
      if (Idx1 != KtiInternalGlobal->SbspSoc) {
        if (SocketData->Cpu[Idx1].Valid == TRUE || KtiInternalGlobal->KtiCpuSktHotPlugEn == TRUE ) {
          BusSize[Idx1] = (UINT8)(SocketMmcfgSize/SAD_BUS_GRANTY);
          TmpBusSize += BusSize[Idx1];
          host->var.kti.CpuInfo[Idx1].CpuRes.PciSegment = Idx2;

          if (TmpBusSize % SAD_BUS_TARGETS == 0) {
            Idx2 = Idx2 + 1;
          }
        }
      }
    }
  } else {

    AvailBusChunk = (UINT8)(host->setup.common.mmCfgSize >> 25); // Convert the available MMCFG space into 32M chunck size

    // Sanity check the resource allocation requested by the platform
    for (Idx1 = 0; Idx1 < MAX_SOCKET; Idx1++) {
      if ( ((SocketData->Cpu[Idx1].Valid == TRUE) || (KtiInternalGlobal->KtiCpuSktHotPlugEn == TRUE)) && (SocketData->Cpu[Idx1].SocType == SOCKET_TYPE_CPU) ) {
        WarnFlag = FALSE;

        //
        // Make sure Bus resource is requested. Otherwise log a warning and force the ratio for this CPU to 1.
        //
        if (host->setup.kti.BusRatio[Idx1] == 0) {
          TmpBusRatio[Idx1] = 1;
          WarnFlag = TRUE;
        } else {
          TmpBusRatio[Idx1] = host->setup.kti.BusRatio[Idx1];
        }

        if (WarnFlag == TRUE) {
          KtiLogWarning(host, (UINT8)(1 << Idx1), SOCKET_TYPE_CPU, 0xFF, WARN_RESOURCE_NOT_REQUESTED_FOR_CPU);
          KtiDebugPrintWarn((host, KTI_DEBUG_WARN, "\n WARNING:  A resource is not requested for CPU%u. Forcing resource allocation. ", Idx1));
        }
      } else {
        //
        // The CPU is not present, so don't allocate resources.
        //
        TmpBusRatio[Idx1] = 0;
      }
    }

    //
    // Calculate total resource ratio requested by the platform
    //
    TotBusChunk = 0;
    for (Idx1 = 0; Idx1 < MAX_SOCKET; Idx1++) {
      if ( ((SocketData->Cpu[Idx1].Valid == TRUE) || (KtiInternalGlobal->KtiCpuSktHotPlugEn == TRUE)) && (SocketData->Cpu[Idx1].SocType == SOCKET_TYPE_CPU) ) {
        TotBusChunk    = TotBusChunk + TmpBusRatio[Idx1];
      } else if (SocketData->Cpu[Idx1].SocType == SOCKET_TYPE_FPGA) {
        FpgaCount += 1;
      }
    }

    // Log a warning if the platform requested resource is more than what is available
    if ((TotBusChunk > AvailBusChunk)) {
      KtiLogWarning(host, 0xFF, 0xFF, 0xFF, WARN_RESOURCE_REQUEST_NOT_MET);
      KtiDebugPrintWarn((host, KTI_DEBUG_WARN, "\n WARNING:  Requested Resource can not be allocated. Forcing to maximum possible value. "));

      //
      // Force the resource to maximum possible value and force the resource ratio to 1 for each socket
      // for which the resource is requested
      //
      if (TotBusChunk > AvailBusChunk) {
        for (Idx1 = 0; Idx1 < MAX_SOCKET; Idx1++) {
          if (SocketData->Cpu[Idx1].Valid == TRUE || KtiInternalGlobal->KtiCpuSktHotPlugEn == TRUE ) {
            if (TmpBusRatio[Idx1] != 0) {
              TmpBusRatio[Idx1] = 1;
            }
          }
        }
      }

      //
      // Recalculate total resource ratio requested by the platform
      //
      TotBusChunk = 0;
      for (Idx1 = 0; Idx1 < MAX_SOCKET; Idx1++) {
        if ( ((SocketData->Cpu[Idx1].Valid == TRUE) || (KtiInternalGlobal->KtiCpuSktHotPlugEn == TRUE)) && (SocketData->Cpu[Idx1].SocType == SOCKET_TYPE_CPU) ) {
          TotBusChunk    = TotBusChunk + TmpBusRatio[Idx1];
        }
      }
    }

    //
    // Calculate the residual resources
    //
    ResiBusChunk    = (AvailBusChunk - FpgaCount) % TotBusChunk;

    //
    // Distribute the "whole" resources
    //
    for (Idx1 = 0; Idx1 < MAX_SOCKET; Idx1++) {
      if ( ((SocketData->Cpu[Idx1].Valid == TRUE) || (KtiInternalGlobal->KtiCpuSktHotPlugEn == TRUE)) && (SocketData->Cpu[Idx1].SocType == SOCKET_TYPE_CPU) ) {
        BusSize[Idx1]    = TmpBusRatio[Idx1] * ((AvailBusChunk - FpgaCount) / TotBusChunk);
      } else {
        BusSize[Idx1]    = 1;
      }
    }

    //
    // Distribute the "residual" resources
    //
    for (Idx1 = 0; Idx1 < MAX_SOCKET; Idx1++) {
      if ( ((SocketData->Cpu[Idx1].Valid == TRUE) || (KtiInternalGlobal->KtiCpuSktHotPlugEn == TRUE)) && (SocketData->Cpu[Idx1].SocType == SOCKET_TYPE_CPU) ) {
        if (ResiBusChunk && (TmpBusRatio[Idx1] != 0) ) {
          BusSize[Idx1]++;
          ResiBusChunk--;
        }
      }
    }

    //
    // Anything left, apply to Legacy socket
    //
    BusSize[KtiInternalGlobal->SbspSoc] = BusSize[KtiInternalGlobal->SbspSoc] + ResiBusChunk;

    //
    // Verify the resource allocation
    //
    TotBusChunk = 0;
    for (Idx1 = 0; Idx1 < MAX_SOCKET; Idx1++) {
      if (SocketData->Cpu[Idx1].Valid == TRUE || KtiInternalGlobal->KtiCpuSktHotPlugEn == TRUE ) {
        TotBusChunk    = TotBusChunk + BusSize[Idx1];
      }
    }
    KTI_ASSERT ((TotBusChunk == AvailBusChunk), ERR_SW_CHECK, MINOR_ERR_SW_CHECK_B);
  }
  return KTI_SUCCESS;
}


/**

  Assigns resources for CPUs.

  @param host              - Input/Output structure of the KTIRC
  @param SocketData        - CPU related info
  @param KtiInternalGlobal - KTIRC internal variables.
  @param TmpTotCpu         - NumberofCpus for purpose of resource allocation

  @retval KTI_SUCCESS

**/
KTI_STATUS
CalculateBusResources (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal,
  UINT8                  *BusSize,
  UINT8                   TmpTotCpu
  )
{
  UINT8          Socket, Stack;
  UINT32         BusBase, NumberBusPerSocket;
  UINT8          NumberUbox[MAX_SOCKET], NumberIio[MAX_SOCKET], NumberMcp[MAX_SOCKET], NumberFpga[MAX_SOCKET];
  UINT8          BusPerIio, RemainingBus, TmpBusBase, TmpBusLimit, ReservedBus;

  MemSetLocal ((VOID *) NumberUbox, 0x00, sizeof (NumberUbox));
  MemSetLocal ((VOID *) NumberIio, 0x00, sizeof (NumberIio));
  MemSetLocal ((VOID *) NumberMcp, 0x00, sizeof (NumberMcp));
  MemSetLocal ((VOID *) NumberFpga, 0x00, sizeof(NumberFpga));


  //
  // Check input
  //
  KTI_ASSERT ((TmpTotCpu <= MAX_SOCKET), ERR_SW_CHECK, MINOR_ERR_SW_CHECK_C);

  //
  // Initialize the Base value of the resources
  //
  BusBase = TmpBusBase = TmpBusLimit = 0;
  KtiInternalGlobal->MmcfgBase = host->setup.common.mmCfgBase;
  KtiInternalGlobal->IsMmcfgAboveMmiol= FALSE;

  //
  // Determine number of each type stack per socket.
  //
  for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
    if (SocketData->Cpu[Socket].Valid == TRUE || KtiInternalGlobal->KtiCpuSktHotPlugEn == TRUE ) {
      if (SocketData->Cpu[Socket].SocType == SOCKET_TYPE_CPU) {
        NumberUbox[Socket] = NumberIio[Socket] = NumberMcp[Socket] = 0;
        for (Stack=0; Stack < MAX_IIO_STACK; Stack++) {
          switch (host->var.kti.CpuInfo[Socket].CpuRes.StackRes[Stack].Personality) {
            case TYPE_NONE:
              break;
            case TYPE_UBOX_IIO:
              //
              // We only evenly split across Pcie Stacks.   Dmi stack will be handled as special case below
              //
              if (Stack != IIO_CSTACK) {
                NumberIio[Socket] = NumberIio[Socket] + 1;
              }
            case TYPE_UBOX:
              NumberUbox[Socket] = NumberUbox[Socket] + 1;
              break;
            case TYPE_MCP:
              NumberMcp[Socket] = NumberMcp[Socket] + 1;
              break;
            case TYPE_FPGA:
              NumberFpga[Socket] = NumberFpga[Socket] + 1;
              break;
          }
        }
      } else {
        //
        // Fpga, reserve 1 bus
        //
        NumberIio[Socket] = NumberMcp[Socket] = 0;
        NumberUbox[Socket] = 1;
      }
    }
  }

  Socket = KtiInternalGlobal->SbspSoc;
  host->var.kti.CpuInfo[Socket].CpuRes.SegMmcfgBase.hi = 0;
  host->var.kti.CpuInfo[Socket].CpuRes.SegMmcfgBase.lo = host->setup.common.mmCfgBase + (host->var.kti.CpuInfo[Socket].CpuRes.PciSegment * 256*1024*1024);
  host->var.kti.CpuInfo[Socket].CpuRes.BusBase  = (UINT8)BusBase;
  host->var.kti.CpuInfo[Socket].CpuRes.BusLimit = (UINT8)(BusBase + (BusSize[Socket] * SAD_BUS_GRANTY) - 1);
  NumberBusPerSocket = host->var.kti.CpuInfo[Socket].CpuRes.BusLimit - BusBase + 1;
  ReservedBus = NumberUbox[Socket] + (BUS_PER_MCP * NumberMcp[Socket]) + (BUS_PER_FPGA * NumberFpga[Socket]) + BUS_PER_LEG_STK;
  BusPerIio = (UINT8)(NumberBusPerSocket - ReservedBus) / NumberIio[Socket];
  RemainingBus = (UINT8)(NumberBusPerSocket - ReservedBus) % NumberIio[Socket];

  //
  // Assign the resources for CPUs. Legacy socket's resource range starts at the base of the resource.
  //
  for (Stack=0; Stack < MAX_IIO_STACK; Stack++) {
    switch (host->var.kti.CpuInfo[Socket].CpuRes.StackRes[Stack].Personality) {
      case TYPE_NONE:
        TmpBusBase = 0xff;
        TmpBusLimit = 0;
        break;
      case TYPE_UBOX_IIO:
        TmpBusBase = (UINT8)BusBase;
        if (Stack == IIO_CSTACK) {
          //
          // Assign preallocated and remaining buses to Cstack
          //
          TmpBusLimit = TmpBusBase + BUS_PER_LEG_STK + RemainingBus;
        } else {
          TmpBusLimit = TmpBusBase + BusPerIio;
        }
        break;
      case TYPE_UBOX:
        TmpBusBase = (UINT8)BusBase;
        TmpBusLimit = TmpBusBase + BUS_PER_UBOX - 1;
        break;
      case TYPE_MCP:
        TmpBusBase = (UINT8)BusBase;
        TmpBusLimit = TmpBusBase + BUS_PER_MCP - 1;
        break;
      case TYPE_FPGA:
        TmpBusBase = (UINT8)BusBase;
        TmpBusLimit = TmpBusBase + BUS_PER_FPGA - 1;
        break;
    }

    host->var.kti.CpuInfo[Socket].CpuRes.StackRes[Stack].BusBase = TmpBusBase;
    host->var.kti.CpuInfo[Socket].CpuRes.StackRes[Stack].BusLimit = TmpBusLimit;

    //
    // Only increment if stack was enabled
    //
    if (TmpBusLimit) {
      BusBase = TmpBusLimit + 1;
    }

  }


  //
  // Assign the resources for CPUs. Legacy socket's resource range starts at the base of the resource.
  //
  for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
    if (Socket == KtiInternalGlobal->SbspSoc) {
      continue;
    }
    //
    // if socket exists or Hot Plug enabled (reserved for onlining sockets)
    //
    if (((SocketData->Cpu[Socket].Valid == TRUE) || (KtiInternalGlobal->KtiCpuSktHotPlugEn == TRUE)) ) {
      // Update Bus Base & Limit
      host->var.kti.CpuInfo[Socket].CpuRes.SegMmcfgBase.hi = 0;
      host->var.kti.CpuInfo[Socket].CpuRes.SegMmcfgBase.lo = host->setup.common.mmCfgBase + (host->var.kti.CpuInfo[Socket].CpuRes.PciSegment * 256*1024*1024);
      host->var.kti.CpuInfo[Socket].CpuRes.BusBase  = (UINT8)BusBase;
      host->var.kti.CpuInfo[Socket].CpuRes.BusLimit = (UINT8)(BusBase + (BusSize[Socket] * SAD_BUS_GRANTY) - 1);

      NumberBusPerSocket = host->var.kti.CpuInfo[Socket].CpuRes.BusLimit - BusBase + 1;
      ReservedBus = NumberUbox[Socket] + (BUS_PER_MCP * NumberMcp[Socket]) + (BUS_PER_FPGA * NumberFpga[Socket]) + BUS_PER_NONLEG_STK;
      BusPerIio = (NumberIio[Socket] == 0) ? 0 : (UINT8)(NumberBusPerSocket - ReservedBus) / NumberIio[Socket];
      RemainingBus = (NumberIio[Socket] == 0) ? 0 : (UINT8)(NumberBusPerSocket - ReservedBus) % NumberIio[Socket];

      for (Stack=0; Stack < MAX_IIO_STACK; Stack++) {
        switch (host->var.kti.CpuInfo[Socket].CpuRes.StackRes[Stack].Personality) {
          case TYPE_NONE:
            TmpBusBase = 0xff;
            TmpBusLimit = 0;
            break;
          case TYPE_UBOX_IIO:
            TmpBusBase = (UINT8)BusBase;
            if (Stack == IIO_CSTACK) {
              //
              // Assign preallocated and remaining buses to Cstack
              //
              TmpBusLimit = TmpBusBase + BUS_PER_NONLEG_STK + RemainingBus;
            } else {
              TmpBusLimit = TmpBusBase + BusPerIio;
            }
            break;
          case TYPE_UBOX:
            TmpBusBase = (UINT8)BusBase;
            TmpBusLimit = TmpBusBase + BUS_PER_UBOX - 1;
            break;
          case TYPE_MCP:
            TmpBusBase = (UINT8)BusBase;
            TmpBusLimit = TmpBusBase + BUS_PER_MCP - 1;
            break;
          case TYPE_FPGA:
            TmpBusBase = (UINT8)BusBase;
            TmpBusLimit = TmpBusBase + BUS_PER_FPGA - 1;
            break;
        }

        host->var.kti.CpuInfo[Socket].CpuRes.StackRes[Stack].BusBase = TmpBusBase;
        host->var.kti.CpuInfo[Socket].CpuRes.StackRes[Stack].BusLimit = TmpBusLimit;

        //
        // Only increment if stack was enabled
        //
        if (TmpBusLimit) {
          BusBase = TmpBusLimit + 1;
        }
      }
      //
      // Init next BusBase.  This handles case of socket types that do not full utilize their alloted bus assignments (FPGA)
      //
      BusBase = host->var.kti.CpuInfo[Socket].CpuRes.BusLimit + 1;
    } else {
      host->var.kti.CpuInfo[Socket].CpuRes.BusBase = 0xFF;
      host->var.kti.CpuInfo[Socket].CpuRes.BusLimit = 0;
      host->var.kti.CpuInfo[Socket].CpuRes.SegMmcfgBase.lo = 0xffffffff;
      host->var.kti.CpuInfo[Socket].CpuRes.SegMmcfgBase.hi = 0xffffffff;
    }
  }

  return KTI_SUCCESS;

}


/**

  This routine assigns IO resources for CPUs/stacks that are populated. It consults the appropriate inputs given to KTIRC and
  applies the following set of rules:
    1. Resource will be allocated only for the CPUs that are currently populated
     1a. If hot pluggable system, will preallocate resources for all sockets
     1b. On EX Hotpluggable systems, assume same number stacks per socket as sbsp

  @param host              - Input/Output structure of the KTIRC
  @param SocketData        - CPU related info
  @param KtiInternalGlobal - KTIRC internal variables.
  @param TmpTotCpu         - NumberofCpus for purpose of resource allocation


  @retval KTI_SUCCESS

**/
KTI_STATUS
CalculateIoResources (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal,
  UINT8               TmpTotCpu

  )
{
  UINT8          Stack;
  UINT32         TmpIoSize, IoBase;
  UINT8          Socket;
  UINT8          IoAlloc[MAX_SOCKET][MAX_IIO_STACK];
  UINT8          RemainingIoResources;

  MemSetLocal ((UINT8 *) &IoAlloc, 0, sizeof (IoAlloc));
  IoBase = 0;

  //
  // Assign IO to legacy stack
  //
  IoAlloc[KtiInternalGlobal->SbspSoc][IIO_CSTACK] = SAD_LEG_IO_PCH_SIZE;
  RemainingIoResources = (UINT8)((MAX_SAD_TARGETS - SAD_LEG_IO_PCH_SIZE) / 2);

  //
  // Assign remaining IO to each regular IIO.   Not all IIOs will have IO in a fully populated 8S
  //
  while (RemainingIoResources != 0) {
    for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
      if (SocketData->Cpu[Socket].Valid == TRUE || KtiInternalGlobal->KtiCpuSktHotPlugEn == TRUE) {
        for (Stack=0; Stack < MAX_IIO_STACK; Stack++) {
          if ((Socket == KtiInternalGlobal->SbspSoc) && (Stack == IIO_CSTACK)) {
            continue;
          }

          if (host->var.kti.CpuInfo[Socket].CpuRes.StackRes[Stack].Personality == TYPE_UBOX_IIO) {
            if (RemainingIoResources != 0) {
              IoAlloc[Socket][Stack] = IoAlloc[Socket][Stack] + 2;
              RemainingIoResources = RemainingIoResources - 1;
            } else {
              Socket = MAX_SOCKET;
              break;
            }
          }
        }
      }
    }
  }

  //
  // Assign IO addresses to SBSP first
  //
  host->var.kti.CpuInfo[KtiInternalGlobal->SbspSoc].CpuRes.IoBase  = (UINT16)IoBase;

  for (Stack=0; Stack < MAX_IIO_STACK; Stack++) {
    if (IoAlloc[KtiInternalGlobal->SbspSoc][Stack] == 0) {
      host->var.kti.CpuInfo[KtiInternalGlobal->SbspSoc].CpuRes.StackRes[Stack].IoBase = 0xFFFF;
      host->var.kti.CpuInfo[KtiInternalGlobal->SbspSoc].CpuRes.StackRes[Stack].IoLimit = 0;
    } else {
      host->var.kti.CpuInfo[KtiInternalGlobal->SbspSoc].CpuRes.StackRes[Stack].IoBase = (UINT16)IoBase;
      TmpIoSize = (IoAlloc[KtiInternalGlobal->SbspSoc][Stack] * SAD_LEG_IO_GRANTY) - 1;
      host->var.kti.CpuInfo[KtiInternalGlobal->SbspSoc].CpuRes.StackRes[Stack].IoLimit = (UINT16)(IoBase + TmpIoSize);
      host->var.kti.CpuInfo[KtiInternalGlobal->SbspSoc].CpuRes.IoLimit = (UINT16)(IoBase + TmpIoSize);
      IoBase = host->var.kti.CpuInfo[KtiInternalGlobal->SbspSoc].CpuRes.StackRes[Stack].IoLimit + 1;
    }
  }

  //
  // Assign to non-sbsps
  //
  for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
    if (Socket == KtiInternalGlobal->SbspSoc) {
      continue;
    }
    if (SocketData->Cpu[Socket].Valid == TRUE || KtiInternalGlobal->KtiCpuSktHotPlugEn == TRUE ) {
      host->var.kti.CpuInfo[Socket].CpuRes.IoBase  = (UINT16)IoBase;
      for (Stack=0; Stack < MAX_IIO_STACK; Stack++) {
        if (IoAlloc[Socket][Stack] == 0) {
          host->var.kti.CpuInfo[Socket].CpuRes.StackRes[Stack].IoBase = 0xFFFF;
          host->var.kti.CpuInfo[Socket].CpuRes.StackRes[Stack].IoLimit = 0;
        } else {
          host->var.kti.CpuInfo[Socket].CpuRes.StackRes[Stack].IoBase = (UINT16)IoBase;
          TmpIoSize = (IoAlloc[Socket][Stack] * SAD_LEG_IO_GRANTY) - 1;
          host->var.kti.CpuInfo[Socket].CpuRes.StackRes[Stack].IoLimit = (UINT16)(IoBase + TmpIoSize);
          host->var.kti.CpuInfo[Socket].CpuRes.IoLimit = (UINT16)(IoBase + TmpIoSize);
          IoBase = host->var.kti.CpuInfo[Socket].CpuRes.StackRes[Stack].IoLimit + 1;
        }
      }
      if (host->var.kti.CpuInfo[Socket].CpuRes.IoBase >= host->var.kti.CpuInfo[Socket].CpuRes.IoLimit) {
        // Indicate the resources are not valid by forcing Base > Limit
        host->var.kti.CpuInfo[Socket].CpuRes.IoBase = 0xFFFF;
        host->var.kti.CpuInfo[Socket].CpuRes.IoLimit = 0;
      }

    } else {
      // Indicate the resources are not valid by forcing Base > Limit
      host->var.kti.CpuInfo[Socket].CpuRes.IoBase = 0xFFFF;
      host->var.kti.CpuInfo[Socket].CpuRes.IoLimit = 0;
      for (Stack=0; Stack < MAX_IIO_STACK; Stack++) {
        host->var.kti.CpuInfo[Socket].CpuRes.StackRes[Stack].IoBase = 0xFFFF;
        host->var.kti.CpuInfo[Socket].CpuRes.StackRes[Stack].IoLimit = 0;
      }
    }
  }

  return KTI_SUCCESS;

}


/**

  This routine assigns IoApic resources for CPUs/stacks that are populated. It consults the appropriate inputs given to KTIRC and
  applies the following set of rules:
    1. IOAPIC range which is always reserved for every possible stack; this makes ACPI & other IOAPIC related setting easier

  @param host              - Input/Output structure of the KTIRC
  @param SocketData        - CPU related info
  @param KtiInternalGlobal - KTIRC internal variables.

  @retval KTI_SUCCESS

**/
KTI_STATUS
CalculateIoApicResources (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal
  )
{
  UINT8          Idx1;
  UINT8          Stack;
  UINT8          SadIndex;

  SadIndex = 0;
  KTI_ASSERT (KtiInternalGlobal->TotSadTargets, ERR_SW_CHECK, MINOR_ERR_SW_CHECK_D);
  KTI_ASSERT (host->var.kti.CpuInfo[KtiInternalGlobal->SbspSoc].CpuRes.TotEnabledStacks <= MAX_IIO_STACK, ERR_SW_CHECK, MINOR_ERR_SW_CHECK_E);



  //
  // Assign IoApic addresses to SBSP first
  //
  host->var.kti.CpuInfo[KtiInternalGlobal->SbspSoc].CpuRes.IoApicBase = IOAPIC_BASE + (SadIndex * SAD_IOAPIC_GRANTY);

  for (Stack=0; Stack < MAX_IIO_STACK; Stack++) {
    if ((host->var.kti.CpuInfo[KtiInternalGlobal->SbspSoc].CpuRes.StackRes[Stack].Personality < TYPE_DISABLED) && \
       (host->var.kti.CpuInfo[KtiInternalGlobal->SbspSoc].CpuRes.StackRes[Stack].Personality > TYPE_UBOX)) {
      host->var.kti.CpuInfo[KtiInternalGlobal->SbspSoc].CpuRes.StackRes[Stack].IoApicBase = IOAPIC_BASE + (SadIndex * SAD_IOAPIC_GRANTY);
      host->var.kti.CpuInfo[KtiInternalGlobal->SbspSoc].CpuRes.StackRes[Stack].IoApicLimit = IOAPIC_BASE + (SadIndex * SAD_IOAPIC_GRANTY) + SAD_IOAPIC_GRANTY - 1 ;
      host->var.kti.CpuInfo[KtiInternalGlobal->SbspSoc].CpuRes.IoApicLimit = host->var.kti.CpuInfo[KtiInternalGlobal->SbspSoc].CpuRes.StackRes[Stack].IoApicLimit;
      SadIndex = SadIndex + 1;
    } else {
      host->var.kti.CpuInfo[KtiInternalGlobal->SbspSoc].CpuRes.StackRes[Stack].IoApicBase = 0xFFFFFFFF;
      host->var.kti.CpuInfo[KtiInternalGlobal->SbspSoc].CpuRes.StackRes[Stack].IoApicLimit = 0;
    }
  }

  for (Idx1 = 0; Idx1 < MAX_SOCKET; Idx1++) {
    if (Idx1 == KtiInternalGlobal->SbspSoc) {
      continue;
    }
    //
    // if socket exists or Hot Plug enabled (reserved for onlining sockets)
    //
    if ( ((SocketData->Cpu[Idx1].Valid == TRUE) || (KtiInternalGlobal->KtiCpuSktHotPlugEn == TRUE)) && (SocketData->Cpu[Idx1].SocType == SOCKET_TYPE_CPU) ) {
      host->var.kti.CpuInfo[Idx1].CpuRes.IoApicBase = IOAPIC_BASE + (SadIndex * SAD_IOAPIC_GRANTY);
      // Assign to each stacks.
      for (Stack=0; Stack < MAX_IIO_STACK; Stack++) {
        if ((host->var.kti.CpuInfo[Idx1].CpuRes.StackRes[Stack].Personality < TYPE_DISABLED) && \
           (host->var.kti.CpuInfo[Idx1].CpuRes.StackRes[Stack].Personality > TYPE_UBOX)) {
          host->var.kti.CpuInfo[Idx1].CpuRes.StackRes[Stack].IoApicBase = IOAPIC_BASE + (SadIndex * SAD_IOAPIC_GRANTY);
          host->var.kti.CpuInfo[Idx1].CpuRes.StackRes[Stack].IoApicLimit = IOAPIC_BASE + (SadIndex * SAD_IOAPIC_GRANTY) + SAD_IOAPIC_GRANTY - 1 ;
          host->var.kti.CpuInfo[Idx1].CpuRes.IoApicLimit = host->var.kti.CpuInfo[Idx1].CpuRes.StackRes[Stack].IoApicLimit;
          SadIndex = SadIndex + 1;
        } else {
          host->var.kti.CpuInfo[Idx1].CpuRes.StackRes[Stack].IoApicBase = 0xFFFFFFFF;
          host->var.kti.CpuInfo[Idx1].CpuRes.StackRes[Stack].IoApicLimit = 0;
        }
      }
    } else { // Indicate the resources are not valid by setting Base > Limit
      host->var.kti.CpuInfo[Idx1].CpuRes.IoApicBase = 0xFFFFFFFF;
      host->var.kti.CpuInfo[Idx1].CpuRes.IoApicLimit = 0;

      for (Stack=0; Stack < MAX_IIO_STACK; Stack++) {
        host->var.kti.CpuInfo[Idx1].CpuRes.StackRes[Stack].IoApicBase = 0xFFFFFFFF;
        host->var.kti.CpuInfo[Idx1].CpuRes.StackRes[Stack].IoApicLimit = 0;
      }
    }
  }
  return KTI_SUCCESS;
}


/**

  This routine assigns Mmiol resources for CPUs/stacks that are populated. It consults the appropriate inputs given to KTIRC and
  applies the following set of rules:
  For hot plug systems resources will be preallocated

  @param host              - Input/Output structure of the KTIRC
  @param SocketData        - CPU related info
  @param KtiInternalGlobal - KTIRC internal variables.
  @param TmpTotCpu         - NumberofCpus for purpose of resource allocation

  @retval KTI_SUCCESS

**/
KTI_STATUS
CalculateMmiolResources (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal ,
  UINT8               TmpTotCpu
  )
{
  UINT8          Idx1;
  UINT16         TmpMmiolRatio, ExtraMmiol;
  UINT8          Stack;
  UINT16         AvailMmiolChunk;
  UINT32         TmpMmiolSize, MmiolBase, Data32;

  //
  // MMIOL = MMCFG Base + MMCFG Size. Make sure at least 64M of MMIOL space is available for each CPU.
  //
  Data32 = MMIOL_LIMIT - (host->setup.common.mmCfgBase + host->setup.common.mmCfgSize - 1) ;
  if((Data32 >> 26)  < TmpTotCpu) {
    KtiCheckPoint(0xFF, 0xFF, 0xFF, ERR_SAD_SETUP, MINOR_ERR_INSUFFICIENT_RESOURCE_SPACE, host);
    KtiDebugPrintFatal((host, KTI_DEBUG_ERROR, "\n Invalid MMCFG Base Supplied. Insufficient MMIOL space. "));
    KtiDebugPrintFatal((host, KTI_DEBUG_ERROR, "\n Each CPU must be allocated at least 64MB of MMIOL space"));
    KTI_ASSERT(FALSE, ERR_SAD_SETUP, MINOR_ERR_INSUFFICIENT_RESOURCE_SPACE);
  }
  MmiolBase = host->setup.common.mmCfgBase + host->setup.common.mmCfgSize;

  //MmiolLimit = TmpMmiolBase + (MmiolSize * SAD_MMIOL_GRANTY);
  AvailMmiolChunk  = (UINT16)((UINT32)((MMIOL_LIMIT - MmiolBase) + 1) >> 22); // Convert the available MMIOL space into 4M chunck size

  //
  // Evenly split Mmiol across all enabled system stacks
  //
  KTI_ASSERT (KtiInternalGlobal->TotSadTargets, ERR_SW_CHECK, MINOR_ERR_SW_CHECK_F);
  KTI_ASSERT (host->var.kti.CpuInfo[KtiInternalGlobal->SbspSoc].CpuRes.TotEnabledStacks <= MAX_IIO_STACK, ERR_SW_CHECK, MINOR_ERR_SW_CHECK_10);
//  TmpMmiolRatio = (MAX_SAD_TARGETS / KtiInternalGlobal->TotSadTargets);
//  ExtraMmiol = (MAX_SAD_TARGETS % KtiInternalGlobal->TotSadTargets);
  TmpMmiolRatio = (AvailMmiolChunk / KtiInternalGlobal->TotSadTargets);
  ExtraMmiol = (AvailMmiolChunk % KtiInternalGlobal->TotSadTargets);


  //
  // Assign any extra Mmiol to SBSP
  //
  TmpMmiolSize = (((host->var.kti.CpuInfo[KtiInternalGlobal->SbspSoc].CpuRes.TotEnabledStacks * TmpMmiolRatio) + ExtraMmiol) * SAD_MMIOL_GRANTY);
  host->var.kti.CpuInfo[KtiInternalGlobal->SbspSoc].CpuRes.MmiolBase  = MmiolBase;
  host->var.kti.CpuInfo[KtiInternalGlobal->SbspSoc].CpuRes.MmiolLimit = MmiolBase + (TmpMmiolSize - 1);

  //
  // Assign extra Mmiol to DMI/Ctack
  //
  host->var.kti.CpuInfo[KtiInternalGlobal->SbspSoc].CpuRes.StackRes[IIO_CSTACK].MmiolBase = MmiolBase;
  host->var.kti.CpuInfo[KtiInternalGlobal->SbspSoc].CpuRes.StackRes[IIO_CSTACK].MmiolLimit = MmiolBase + (((TmpMmiolRatio + ExtraMmiol) * SAD_MMIOL_GRANTY) - 1);
  MmiolBase = host->var.kti.CpuInfo[KtiInternalGlobal->SbspSoc].CpuRes.StackRes[IIO_CSTACK].MmiolLimit + 1;
  TmpMmiolSize = ((TmpMmiolRatio * SAD_MMIOL_GRANTY) - 1);

  for (Stack = IIO_PSTACK0; Stack < MAX_IIO_STACK; Stack++) {
    if ((host->var.kti.CpuInfo[KtiInternalGlobal->SbspSoc].CpuRes.StackRes[Stack].Personality < TYPE_DISABLED) && \
       (host->var.kti.CpuInfo[KtiInternalGlobal->SbspSoc].CpuRes.StackRes[Stack].Personality > TYPE_UBOX)) {
      host->var.kti.CpuInfo[KtiInternalGlobal->SbspSoc].CpuRes.StackRes[Stack].MmiolBase = MmiolBase;
      host->var.kti.CpuInfo[KtiInternalGlobal->SbspSoc].CpuRes.StackRes[Stack].MmiolLimit = MmiolBase + TmpMmiolSize;
      MmiolBase = host->var.kti.CpuInfo[KtiInternalGlobal->SbspSoc].CpuRes.StackRes[Stack].MmiolLimit + 1;
    } else {
      host->var.kti.CpuInfo[KtiInternalGlobal->SbspSoc].CpuRes.StackRes[Stack].MmiolBase = 0xFFFFFFFF;
      host->var.kti.CpuInfo[KtiInternalGlobal->SbspSoc].CpuRes.StackRes[Stack].MmiolLimit = 0;
    }
  }

  for (Idx1 = 0; Idx1 < MAX_SOCKET; Idx1++) {
    if (Idx1 == KtiInternalGlobal->SbspSoc) {
      continue;
    }
    //
    // if socket exists or Hot Plug enabled (reserved for onlining sockets)
    //
    if ( ((SocketData->Cpu[Idx1].Valid == TRUE) || (KtiInternalGlobal->KtiCpuSktHotPlugEn == TRUE)) && (SocketData->Cpu[Idx1].SocType == SOCKET_TYPE_CPU) ) {
      host->var.kti.CpuInfo[Idx1].CpuRes.MmiolBase  = MmiolBase;
      // Evenly split Mmio amongst all possible stacks.
      for (Stack=0; Stack < MAX_IIO_STACK; Stack++) {
        if ((host->var.kti.CpuInfo[Idx1].CpuRes.StackRes[Stack].Personality < TYPE_DISABLED) && \
           (host->var.kti.CpuInfo[Idx1].CpuRes.StackRes[Stack].Personality > TYPE_UBOX)) {
          host->var.kti.CpuInfo[Idx1].CpuRes.StackRes[Stack].MmiolBase = MmiolBase;
          host->var.kti.CpuInfo[Idx1].CpuRes.StackRes[Stack].MmiolLimit = MmiolBase + TmpMmiolSize;
          host->var.kti.CpuInfo[Idx1].CpuRes.MmiolLimit = MmiolBase + TmpMmiolSize;
          MmiolBase = host->var.kti.CpuInfo[Idx1].CpuRes.StackRes[Stack].MmiolLimit + 1;
        } else {
          host->var.kti.CpuInfo[Idx1].CpuRes.StackRes[Stack].MmiolBase = 0xFFFFFFFF;
          host->var.kti.CpuInfo[Idx1].CpuRes.StackRes[Stack].MmiolLimit = 0;
        }
      }
    } else { // Indicate the resources are not valid by setting Base > Limit
      host->var.kti.CpuInfo[Idx1].CpuRes.MmiolBase = 0xFFFFFFFF;
      host->var.kti.CpuInfo[Idx1].CpuRes.MmiolLimit = 0;

      for (Stack=0; Stack < MAX_IIO_STACK; Stack++) {
        host->var.kti.CpuInfo[Idx1].CpuRes.StackRes[Stack].MmiolBase = 0xFFFFFFFF;
        host->var.kti.CpuInfo[Idx1].CpuRes.StackRes[Stack].MmiolLimit = 0;
      }
    }
  }

  return KTI_SUCCESS;
}

/**

  This routine assigns Mmioh resources for CPUs/stacks that are populated. It consults the appropriate inputs given to KTIRC and
  applies the following set of rules:
  For hot plug systems resources will be preallocated

  @param host              - Input/Output structure of the KTIRC
  @param SocketData        - CPU related info
  @param KtiInternalGlobal - KTIRC internal variables.
  @param TmpTotCpu         - NumberofCpus for purpose of resource allocation

  @retval KTI_SUCCESS

**/
KTI_STATUS
CalculateMmiohResources (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal,
  UINT8               TmpTotCpu
  )
{
  UINT8          Idx1;
  UINT16         TmpMmiohRatio;
  UINT8          Stack;
  UINT16         MmiohSize;
  UINT64_STRUCT  MmiohBase, TmpMmiohSize, MaxMmiohSizeCheck;

  //
  // Get MmiohBase from setup
  //
  MmiohBase.lo = 0x0;
  MmiohBase.hi = host->setup.common.mmiohBase;

  //
  // Mmioh granularity size.
  // Interleave size = 2**(size*2) GB.
  //
  MmiohSize = KtiInternalGlobal->MmiohTargetSize;

  //
  // Check for valid address
  //
  if (MmiohBase.hi == 0) {
    KtiLogWarning(host, 0xFF, 0xFF, 0xFF, WARN_MMIOH_BASE_UNSUPPORTED);
    KTI_ASSERT (FALSE, ERR_SW_CHECK, MINOR_ERR_SW_CHECK_14);
  }

  //
  // Evenly split Mmioh across all enabled system stacks
  //
  KTI_ASSERT (KtiInternalGlobal->TotSadTargets, ERR_SW_CHECK, MINOR_ERR_SW_CHECK_11);
  KTI_ASSERT (host->var.kti.CpuInfo[KtiInternalGlobal->SbspSoc].CpuRes.TotEnabledStacks <= MAX_IIO_STACK, ERR_SW_CHECK, MINOR_ERR_SW_CHECK_12);
  TmpMmiohRatio = 1<<(MmiohSize*2) ;

  //
  // Assign Mmioh to SBSP
  //
  TmpMmiohSize.lo = 1*1024*1024*1024;
  TmpMmiohSize.hi = 0;
  TmpMmiohSize = MultUINT64x32(TmpMmiohSize, (host->var.kti.CpuInfo[KtiInternalGlobal->SbspSoc].CpuRes.TotEnabledStacks * TmpMmiohRatio));
  host->var.kti.CpuInfo[KtiInternalGlobal->SbspSoc].CpuRes.MmiohBase  = MmiohBase;
  host->var.kti.CpuInfo[KtiInternalGlobal->SbspSoc].CpuRes.MmiohLimit = AddUINT64(MmiohBase, SubUINT64x32(TmpMmiohSize, 1));

  //
  // Assign Mmioh to each stack
  //
  TmpMmiohSize.lo = 1*1024*1024*1024;
  TmpMmiohSize.hi = 0;
  TmpMmiohSize = MultUINT64x32(TmpMmiohSize, TmpMmiohRatio);
  for (Stack = 0; Stack < MAX_IIO_STACK; Stack++) {
    if ((host->var.kti.CpuInfo[KtiInternalGlobal->SbspSoc].CpuRes.StackRes[Stack].Personality < TYPE_DISABLED) && \
       (host->var.kti.CpuInfo[KtiInternalGlobal->SbspSoc].CpuRes.StackRes[Stack].Personality > TYPE_UBOX)) {
      host->var.kti.CpuInfo[KtiInternalGlobal->SbspSoc].CpuRes.StackRes[Stack].MmiohBase = MmiohBase;
      host->var.kti.CpuInfo[KtiInternalGlobal->SbspSoc].CpuRes.StackRes[Stack].MmiohLimit = AddUINT64(MmiohBase, SubUINT64x32(TmpMmiohSize, 1));
      MmiohBase = AddUINT64x32(host->var.kti.CpuInfo[KtiInternalGlobal->SbspSoc].CpuRes.StackRes[Stack].MmiohLimit, 1);
    } else {
      host->var.kti.CpuInfo[KtiInternalGlobal->SbspSoc].CpuRes.StackRes[Stack].MmiohBase.lo = 0xFFFFFFFF;
      host->var.kti.CpuInfo[KtiInternalGlobal->SbspSoc].CpuRes.StackRes[Stack].MmiohBase.hi = 0xFFFFFFFF;
      host->var.kti.CpuInfo[KtiInternalGlobal->SbspSoc].CpuRes.StackRes[Stack].MmiohLimit.lo = 0;
      host->var.kti.CpuInfo[KtiInternalGlobal->SbspSoc].CpuRes.StackRes[Stack].MmiohLimit.hi = 0;
    }
  }

  for (Idx1 = 0; Idx1 < MAX_SOCKET; Idx1++) {
    if (Idx1 == KtiInternalGlobal->SbspSoc) {
      continue;
    }
    //
    // if socket exists or Hot Plug enabled (reserved for onlining sockets)
    //
    if ( ((SocketData->Cpu[Idx1].Valid == TRUE) || (KtiInternalGlobal->KtiCpuSktHotPlugEn == TRUE)) && (SocketData->Cpu[Idx1].SocType == SOCKET_TYPE_CPU) ) {
      host->var.kti.CpuInfo[Idx1].CpuRes.MmiohBase  = MmiohBase;
      // Evenly assign mmioh amongst all enabled stacks.
      for (Stack=0; Stack < MAX_IIO_STACK; Stack++) {
        if ((host->var.kti.CpuInfo[Idx1].CpuRes.StackRes[Stack].Personality < TYPE_DISABLED) && \
           (host->var.kti.CpuInfo[Idx1].CpuRes.StackRes[Stack].Personality > TYPE_UBOX)) {
          host->var.kti.CpuInfo[Idx1].CpuRes.StackRes[Stack].MmiohBase = MmiohBase;
          host->var.kti.CpuInfo[Idx1].CpuRes.StackRes[Stack].MmiohLimit = AddUINT64(MmiohBase, SubUINT64x32(TmpMmiohSize, 1));
          host->var.kti.CpuInfo[Idx1].CpuRes.MmiohLimit = AddUINT64(MmiohBase, SubUINT64x32(TmpMmiohSize, 1));
          MmiohBase = AddUINT64x32(host->var.kti.CpuInfo[Idx1].CpuRes.StackRes[Stack].MmiohLimit, 1);
        } else {
          host->var.kti.CpuInfo[Idx1].CpuRes.StackRes[Stack].MmiohBase.lo = 0xFFFFFFFF;
          host->var.kti.CpuInfo[Idx1].CpuRes.StackRes[Stack].MmiohBase.hi = 0xFFFFFFFF;
          host->var.kti.CpuInfo[Idx1].CpuRes.StackRes[Stack].MmiohLimit.lo = 0;
          host->var.kti.CpuInfo[Idx1].CpuRes.StackRes[Stack].MmiohLimit.hi = 0;
        }
      }
    } else { // Indicate the resources are not valid by setting Base > Limit
      host->var.kti.CpuInfo[Idx1].CpuRes.MmiohBase.lo = 0xFFFFFFFF;
      host->var.kti.CpuInfo[Idx1].CpuRes.MmiohBase.hi = 0xFFFFFFFF;
      host->var.kti.CpuInfo[Idx1].CpuRes.MmiohLimit.lo = 0;
      host->var.kti.CpuInfo[Idx1].CpuRes.MmiohLimit.hi = 0;

      for (Stack=0; Stack < MAX_IIO_STACK; Stack++) {
        host->var.kti.CpuInfo[Idx1].CpuRes.StackRes[Stack].MmiohBase.lo = 0xFFFFFFFF;
        host->var.kti.CpuInfo[Idx1].CpuRes.StackRes[Stack].MmiohBase.hi = 0xFFFFFFFF;
        host->var.kti.CpuInfo[Idx1].CpuRes.StackRes[Stack].MmiohLimit.lo = 0;
        host->var.kti.CpuInfo[Idx1].CpuRes.StackRes[Stack].MmiohLimit.hi = 0;
      }
    }
  }

  //
  // Check that Mmioh did not exceed upper limit
  //
  MmiohBase = SubUINT64x32(MmiohBase, 1);
  MaxMmiohSizeCheck = RShiftUINT64(MmiohBase, MAX_ADDR_LINE);
  KTI_ASSERT(MaxMmiohSizeCheck.lo == 0, ERR_SW_CHECK, MINOR_ERR_SW_CHECK_13);

  return KTI_SUCCESS;
}

/**

  This routine determines the granularity of the mmiol resources

  @param host              - Input/Output structure of the KTIRC
  @param SocketData        - CPU related info
  @param KtiInternalGlobal - KTIRC internal variables.

  @retval KTI_SUCCESS

**/
KTI_STATUS
CalculateMmiolGranularity (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal
  )
{
  UINT8          Idx1;
  UINT8          Stack;
  UINT32         AddressMap = 0;
  UINT8          BitPos;


  for (Idx1 = 0; Idx1 < MAX_SOCKET; Idx1++) {
    //
    // if socket exists or Hot Plug enabled (reserved for onlining sockets)
    //
    if ((SocketData->Cpu[Idx1].Valid == TRUE) || (KtiInternalGlobal->KtiCpuSktHotPlugEn == TRUE)) {
      for (Stack=0; Stack < MAX_IIO_STACK; Stack++) {
        if (host->var.kti.CpuInfo[Idx1].CpuRes.StackRes[Stack].MmiolBase != 0xFFFFFFFF) {
          AddressMap = AddressMap | host->var.kti.CpuInfo[Idx1].CpuRes.StackRes[Stack].MmiolBase;
        }
      }
    }
  }

  BitPos = 0;
  while (!(AddressMap & 1)) {
    BitPos = BitPos + 1;
    AddressMap = AddressMap >> 1;
  }

  KtiInternalGlobal->MmiolGranularity = BitPos;

  return KTI_SUCCESS;
}

/**

  This routine determines the granularity of the mmioh resources

  @param host              - Input/Output structure of the KTIRC
  @param SocketData        - CPU related info
  @param KtiInternalGlobal - KTIRC internal variables.

  @retval KTI_SUCCESS

**/
KTI_STATUS
CalculateMmiohGranularity (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal
  )
{
  UINT8          Idx1;
  UINT8          Stack;
  UINT64_STRUCT  AddressMap;
  UINT8          BitPos;

  AddressMap.lo = 0;
  AddressMap.hi = 0;


  for (Idx1 = 0; Idx1 < MAX_SOCKET; Idx1++) {
    //
    // if socket exists or Hot Plug enabled (reserved for onlining sockets)
    //
    if ((SocketData->Cpu[Idx1].Valid == TRUE) || (KtiInternalGlobal->KtiCpuSktHotPlugEn == TRUE)) {
      for (Stack=0; Stack < MAX_IIO_STACK; Stack++) {
        if (host->var.kti.CpuInfo[Idx1].CpuRes.StackRes[Stack].MmiohBase.hi != 0xFFFFFFFF) {
          AddressMap.lo = AddressMap.lo | host->var.kti.CpuInfo[Idx1].CpuRes.StackRes[Stack].MmiohBase.lo;
          AddressMap.hi = AddressMap.hi | host->var.kti.CpuInfo[Idx1].CpuRes.StackRes[Stack].MmiohBase.hi;
        }
      }
    }
  }


  if (AddressMap.lo == 0) {
    AddressMap.lo = AddressMap.hi;
    BitPos = 32;
  } else {
    BitPos = 0;
  }

  while (!(AddressMap.lo & 1)) {
    BitPos = BitPos + 1;
    AddressMap.lo = AddressMap.lo >> 1;
  }

  KtiInternalGlobal->MmiohGranularity = BitPos;

  return KTI_SUCCESS;
}

/**
  
  This routine prints the column headers in the CPU Resource Map.
  To print the details of the Resource Map according to these column headings,
  call PrintCpuResourceMapDetails()

  @param host - Pointer to the host structure
  
**/
VOID
PrintResourceMapHeader (
  PSYSHOST host
  )
{
  KtiDebugPrintInfo0((host, KTI_DEBUG_INFO1, "\n-----------------------------------------------------------------------------------------------------------------------------------------------------"));
  KtiDebugPrintInfo0((host, KTI_DEBUG_INFO1, "\n      | Seg |    Bus      |        Io       |          IoApic         |          Mmiol          |                   Mmioh                   | StkBmp |"));
  KtiDebugPrintInfo0((host, KTI_DEBUG_INFO1, "\n-----------------------------------------------------------------------------------------------------------------------------------------------------"));
}

/**
  
  This routine prints the details of a CPU Resource map.
  This should be called immediately after printing the column headings, PrintResourceMapHeader()

  @param host         - Input/Output structure of the KTIRC
  @param SocketId     - Socket associated with this resource map
  @param CpuRes       - CPU Resource map data structure

**/
VOID
PrintCpuResourceMapDetails (
  PSYSHOST         host,
  UINT8            Socket,
  KTI_SOCKET_DATA  *SocketData,
  KTI_CPU_RESOURCE CpuRes
  )
{
  UINT8 Idx1;
  KtiDebugPrintInfo0((host, KTI_DEBUG_INFO0, "\n%s%u  |  %1u  | 0x%02X - 0x%02X | 0x%04X - 0x%04X | 0x%08X - 0x%08X ",
        (SocketData->Cpu[Socket].SocType == SOCKET_TYPE_FPGA)  ? "PGA":"CPU",
        Socket,
        CpuRes.PciSegment,
        CpuRes.BusBase,
        CpuRes.BusLimit,
        CpuRes.IoBase,
        CpuRes.IoLimit,
        CpuRes.IoApicBase,
        CpuRes.IoApicLimit
  ));
  KtiDebugPrintInfo0((host, KTI_DEBUG_INFO0, "| 0x%08X - 0x%08X | 0x%08X %08X - 0x%08X %08X |  0x%02X  |",
        CpuRes.MmiolBase,
        CpuRes.MmiolLimit,
        CpuRes.MmiohBase.hi,
        CpuRes.MmiohBase.lo,
        CpuRes.MmiohLimit.hi,
        CpuRes.MmiohLimit.lo,
        CpuRes.stackPresentBitmap
  ));
  for (Idx1=0; Idx1 < MAX_IIO_STACK; Idx1++) {
    KtiDebugPrintInfo0((host, KTI_DEBUG_INFO0, "\n "));
    KtiDebugPrintInfo0((host, KTI_DEBUG_INFO0, "Stk%1d |  %1u  | 0x%02X - 0x%02X | 0x%04X - 0x%04X | 0x%08X - 0x%08X ",
        Idx1,
        CpuRes.PciSegment,
        CpuRes.StackRes[Idx1].BusBase,
        CpuRes.StackRes[Idx1].BusLimit,
        CpuRes.StackRes[Idx1].IoBase,
        CpuRes.StackRes[Idx1].IoLimit,
        CpuRes.StackRes[Idx1].IoApicBase,
        CpuRes.StackRes[Idx1].IoApicLimit
    ));

    KtiDebugPrintInfo0((host, KTI_DEBUG_INFO0, "| 0x%08X - 0x%08X | 0x%08X %08X - 0x%08X %08X |        |",
        CpuRes.StackRes[Idx1].MmiolBase,
        CpuRes.StackRes[Idx1].MmiolLimit,
        CpuRes.StackRes[Idx1].MmiohBase.hi,
        CpuRes.StackRes[Idx1].MmiohBase.lo,
        CpuRes.StackRes[Idx1].MmiohLimit.hi,
        CpuRes.StackRes[Idx1].MmiohLimit.lo
    ));
  }
  KtiDebugPrintInfo0((host, KTI_DEBUG_INFO0, "\n"));
}

/**

  We can derive some segment data from the CpuResource map.
  Check each segment's bus assignments. Each PCI Segment should have 255 Buses
  
  @param host              - Input/Output structure of the KTIRC
  @param SocketData        - CPU related info
  @param KtiInternalGlobal - KTIRC internal variables.
  @param CpuRes            - Array of CPU Resource Maps. Can be initialized in the OEM hook OemGetResourceMapUpdate
  
  @retval ReturnValue - KTI_FAILURE if Resource Map requirements are not met, else KTI_SUCCESS
  
**/
KTI_STATUS
CheckOemSegmentResources(
PSYSHOST                 host,
KTI_SOCKET_DATA          *SocketData,
KTI_HOST_INTERNAL_GLOBAL *KtiInternalGlobal,
KTI_CPU_RESOURCE         *CpuRes
)
{
  UINT8  SegmentValidatedBitMap = 0;
  UINT8  CurrentSegment = 0;
  UINT8  ExpectedSegments = 1;
  UINT8  SegmentMax = 0;
  UINT32 BusIterator = 0;
  UINT32 BusMax = 0;
  UINT8  SbspSocketId = KtiInternalGlobal->SbspSoc;
  UINT8  Idx1 = 0;
  UINT8  Idx2 = 0;
  UINT32 MmcfgSizeMB = 0;
  UINT32 MmcfgGranularity = SAD_BUS_GRANTY;

  KTI_STATUS ReturnValue = KTI_SUCCESS;

  //
  //Calculate MMCFG Granularity, MMCFG allocated per socket, Upper limit of MMCFG
  //
  MmcfgSizeMB = (host->setup.common.mmCfgSize / 1024 / 1024);
  if (MmcfgSizeMB >= 256) {
    //Greater than 256 MB MMCFG
    BusMax = 255;
    MmcfgGranularity = (MmcfgSizeMB / SAD_BUS_TARGETS);
    ExpectedSegments = (UINT8) (MmcfgSizeMB / 256);
  } else {
    //Less than 256 MB MMCFG
    BusMax = MmcfgSizeMB - 1;
    MmcfgGranularity = SAD_BUS_GRANTY;
    ExpectedSegments = 1;
  }
  if ((CpuRes[SbspSocketId].SegMmcfgBase.lo + host->setup.common.mmCfgSize - 1) > (MMIOL_LIMIT)) {
    KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\nMMCFG Limit is too high\n", Idx1));
    ReturnValue = KTI_FAILURE;
  }

  //  
  // Validate MMCFG Resources for each segment
  // Note that Idx1 is NOT iterated by this for expression
  //
  for (Idx1 = 0; Idx1 < MAX_SOCKET;) {

    //Select the next segment to validate. Always start with SBSP segment
    if (SegmentValidatedBitMap == 0) {
      //Since we haven't validated any segments yet, start with SBSP segment
      Idx1 = SbspSocketId;
    } else {
      //Iterate through the non-SBSP sockets to find a segment not yet validated
      if ((Idx1 == SbspSocketId) ||
          ((SocketData->Cpu[Idx1].Valid == FALSE) && (KtiInternalGlobal->KtiCpuSktHotPlugEn == FALSE)) ||
          (((SegmentValidatedBitMap >> CpuRes[Idx1].PciSegment) & 1) == 1)) {
        Idx1++;
        continue;
      }
    }

    //Check the MMCFG boundaries of this segment
    CurrentSegment = CpuRes[Idx1].PciSegment;
    BusIterator = 0;
    if (CurrentSegment > SegmentMax) {
      SegmentMax = CurrentSegment;
    }
    if (CpuRes[Idx1].SegMmcfgBase.hi != 0) {
      KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\nSocket %d MMCFG Base is above 4GB\n", Idx1));
      ReturnValue = KTI_FAILURE;
    }
    if (CpuRes[Idx1].SegMmcfgBase.lo % 0x10000000 != 0) {
      KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\nSocket %d MMCFG Base is not 256MB aligned\n", Idx1));
      ReturnValue = KTI_FAILURE;
    }
    if (CpuRes[Idx1].BusBase != 0) {
      KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "Socket %d is 1st in segment %d, but does not start with bus 0", Idx1, CurrentSegment));
      ReturnValue = KTI_FAILURE;
    }

    //Validate MMCFG resources for all sockets that belong to this segment
    for (Idx2 = 0; Idx2 < MAX_SOCKET; Idx2++) {
      if (((SocketData->Cpu[Idx2].Valid == TRUE) || (KtiInternalGlobal->KtiCpuSktHotPlugEn == TRUE)) && (CpuRes[Idx2].PciSegment == CurrentSegment)) {
        //Update the Bus Iterator according to this socket's Bus Limit
        if (BusIterator == CpuRes[Idx2].BusBase) {
          BusIterator = CpuRes[Idx2].BusLimit + 1;
        } else {
          KtiDebugPrintInfo0((host, KTI_DEBUG_INFO0, "\nSocket %d first numbered bus is not the next available in segment %d\n", Idx2, CurrentSegment));
          ReturnValue = KTI_FAILURE;
        }
        // Ensure that at least the minimum number of buses are assigned
        if ((CpuRes[Idx2].BusLimit - CpuRes[Idx2].BusBase) < (UINT8) (MmcfgGranularity - 1)) {
          KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0,"%d buses must be assigned to Socket %d", MmcfgGranularity, Idx2));
          ReturnValue = KTI_FAILURE;
        }
        //Ensure that this socket's segment MMCFG base matches all others in this segment
        if (IsXEqualYUINT64(CpuRes[Idx1].SegMmcfgBase, CpuRes[Idx2].SegMmcfgBase) == FALSE) {
          KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\nSocket %d MMCFG Base does not match others in this segment\n", Idx1));
          ReturnValue = KTI_FAILURE;
        }
      }
      if (ReturnValue == KTI_FAILURE) {
        break;
      }
    }

    //Ensure correct number of bus resources are assigned for the current segment
    BusIterator--;
    if (BusIterator != BusMax){
      KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\nSegment %d has incomplete bus assignments\n", CurrentSegment));
      ReturnValue = KTI_FAILURE;
    }

    //Iterate Idx1. Always start with SBSP, then check sockets in ascending order by socket ID
    if (SegmentValidatedBitMap == 0) {
      SegmentValidatedBitMap = (1 << CpuRes[SbspSocketId].PciSegment);
      Idx1 = 0;
    } else {
      SegmentValidatedBitMap = SegmentValidatedBitMap | (1 << CpuRes[Idx1].PciSegment);
      Idx1++;
    }

    if (ReturnValue == KTI_FAILURE) {
      break;
    }
  }

  //
  // Ensure that the number of segments encountered is acceptable based on the MMCFG Size allocated
  //
  if (ExpectedSegments == 1) {
    if (SegmentMax != 0) {
      KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "We expect exactly 1 segment in resource map, based on MMCFG Size"));
      ReturnValue = KTI_FAILURE;
    }
  } else {
    //Don't reject when less than expected number of segments are found - invalid sockets could "own" the wasted buses
    if (SegmentMax > (ExpectedSegments - 1)) {
      KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "We expect up to %d segments in resource map, based on MMCFG Size", ExpectedSegments));
      ReturnValue = KTI_FAILURE;
    }
  }

  return ReturnValue;
}

/**

  This routine checks each socket's resource map. Make sure valid and present, and resources fit within expected ranges.
  
  @param host              - Input/Output structure of the KTIRC
  @param SocketData        - CPU related info
  @param KtiInternalGlobal - KTIRC internal variables.
  @param CpuRes            - Array of CPU Resource Maps. Can be initialized in the OEM hook OemGetResourceMapUpdate
  
  @retval ReturnValue - KTI_FAILURE if Resource Map requirements are not met, else KTI_SUCCESS
  
**/
KTI_STATUS
CheckOemSocketResources(
  PSYSHOST                 host, 
  KTI_SOCKET_DATA          *SocketData, 
  KTI_HOST_INTERNAL_GLOBAL *KtiInternalGlobal, 
  KTI_CPU_RESOURCE         *CpuRes
)
{

  UINT8 Idx1;
  KTI_STATUS ReturnValue = KTI_SUCCESS; // bitmask for CpuRes socket index that fails the resource checks (non-zero indicates failure)
  UINT64_STRUCT NegativeOne64Bit;
  UINT64_STRUCT Zero64Bit;
  UINT8 IoStarved;
  UINT8 IoApicStarved;
  UINT8 MmiolStarved;
  UINT8 MmiohStarved;

  NegativeOne64Bit.hi = 0xFFFFFFFF;
  NegativeOne64Bit.lo = 0xFFFFFFFF;
  Zero64Bit.hi = 0;
  Zero64Bit.lo = 0;

  for (Idx1 = 0; Idx1 < MAX_SOCKET; Idx1++) {
    if ((SocketData->Cpu[Idx1].Valid == TRUE) || (KtiInternalGlobal->KtiCpuSktHotPlugEn == TRUE)) {
      if (CpuRes[Idx1].stackPresentBitmap != host->var.kti.CpuInfo[Idx1].CpuRes.stackPresentBitmap) {
        KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\nOEM Socket %d stack present bitmap, 0x%x, does not match known stack present bitmap, 0x%x"));
        ReturnValue = KTI_FAILURE;
      }

      //Determine which resources are starved for socket Idx1
      IoStarved = ((CpuRes[Idx1].IoLimit == 0) && (CpuRes[Idx1].IoBase == 0xFFFF));
      IoApicStarved = ((CpuRes[Idx1].IoApicLimit == 0) && (CpuRes[Idx1].IoApicBase == 0xFFFFFFFF));
      MmiolStarved = ((CpuRes[Idx1].MmiolLimit == 0) && (CpuRes[Idx1].MmiolBase == 0xFFFFFFFF));
      MmiohStarved = (IsXEqualYUINT64(CpuRes[Idx1].MmiohLimit, Zero64Bit) && IsXEqualYUINT64(CpuRes[Idx1].MmiohBase, NegativeOne64Bit));

      /** LEGACY IO **/
      if (IoStarved == FALSE) {
        //Check Io Limit > Io Base 
        if ((CpuRes[Idx1].IoLimit < CpuRes[Idx1].IoBase)) {
          KtiDebugPrintInfo0((host, KTI_DEBUG_INFO0, "\nSocket %d IO Limit is less than IO Base\n", Idx1));
          ReturnValue = KTI_FAILURE;
        }
        //Check Legacy IO is a multiple of 2KB
        if ((CpuRes[Idx1].IoLimit - CpuRes[Idx1].IoBase + 1) % 0x0800 != 0) {
          KtiDebugPrintInfo0((host, KTI_DEBUG_INFO0, "\nSocket %d IO range is not a multiple of 2KB\n", Idx1));
          ReturnValue = KTI_FAILURE;
        }
      }

      /** IO APIC **/
      if (IoApicStarved == FALSE) {
        //Ensure Limit > Base
        if (CpuRes[Idx1].IoApicLimit < CpuRes[Idx1].IoApicBase) {
          KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\nSocket %d IO APIC limit is less than IO APIC base\n", Idx1));
          ReturnValue = KTI_FAILURE;
        }
      }

      /** MMIOL **/
      if (MmiolStarved == FALSE) {
        //Check Mmiol Limit >= Mmiol Base 
        if (CpuRes[Idx1].MmiolLimit < CpuRes[Idx1].MmiolBase) {
          KtiDebugPrintInfo0((host, KTI_DEBUG_INFO0, "\nSocket %d MMIOL limit is less than MMIOL base\n", Idx1));
          ReturnValue = KTI_FAILURE;
        }
        //MMIOL Base must be 4MB aligned
        if ((CpuRes[Idx1].MmiolBase % 0x400000 != 0)) {
          KtiDebugPrintInfo0((host, KTI_DEBUG_INFO0, "\nSocket %d MMIOL is not 4MB aligned\n", Idx1));
          ReturnValue = KTI_FAILURE;
        }
        //Limit must be less than 64MB Below 4GB
        if (CpuRes[Idx1].MmiolLimit > (MMIOL_LIMIT)) {
          KtiDebugPrintInfo0((host, KTI_DEBUG_INFO0, "\nSocket %d MMIOL limit is too high\n", Idx1));
          ReturnValue = KTI_FAILURE;
        }
      }

      /** MMIOH **/
      if (MmiohStarved == FALSE) {
        //Check Mmioh Base >= 4GB
        if (CpuRes[Idx1].MmiohBase.hi == 0) {
          KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\nSocket %d MMIOH base is below 4GB\n", Idx1));
          ReturnValue = KTI_FAILURE;
        }
        //Check Mmioh Limit >= Mmioh Base
        if (IsXGreaterThanYUINT64 (CpuRes[Idx1].MmiohBase, CpuRes[Idx1].MmiohLimit)) {
          KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\nSocket %d MMIOH Limit is less than MMIOH base\n", Idx1));
          ReturnValue = KTI_FAILURE;
        }
      }
    } else {
      //This socket is not marked valid, should have no resources assigned to it
      if ((CpuRes[Idx1].BusBase != 0xFF) || (CpuRes[Idx1].BusLimit != 0)) {
        KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\nSocket %d has bus resources assigned, but is invalid\n", Idx1));
        ReturnValue = KTI_FAILURE;
      }
      if ((CpuRes[Idx1].IoBase != 0xFFFF) || (CpuRes[Idx1].IoLimit != 0)) {
        KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\nSocket %d has IO resources assigned, but is invalid\n", Idx1));
        ReturnValue = KTI_FAILURE;
      }
      if ((CpuRes[Idx1].IoApicBase != 0xFFFFFFFF) || (CpuRes[Idx1].IoApicLimit != 0)) {
        KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\nSocket %d has IoApic resources assigned, but is invalid\n", Idx1));
        ReturnValue = KTI_FAILURE;
      }
      if ((CpuRes[Idx1].MmiolBase != 0xFFFFFFFF) || (CpuRes[Idx1].MmiolLimit != 0)) {
        KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\nSocket %d has MMIOL resources assigned, but is invalid\n", Idx1));
        ReturnValue = KTI_FAILURE;
      }
      if ((CpuRes[Idx1].MmiohBase.lo != 0xFFFFFFFF) || (CpuRes[Idx1].MmiohBase.hi != 0xFFFFFFFF) || (CpuRes[Idx1].MmiohLimit.lo != 0) || (CpuRes[Idx1].MmiohLimit.hi != 0)) {
        KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\nSocket %d has MMIOH resources assigned, but is invalid\n", Idx1));
        ReturnValue = KTI_FAILURE;
      }
    }
    if (ReturnValue != KTI_SUCCESS) {
      break;
    }
  }
  return ReturnValue;
}
/**

  "Walk" each Socket resource, starting from each lowest base to each highest limit.
  There should be no gaps, and no overlapping ranges
  
  @param host              - Input/Output structure of the KTIRC
  @param SocketData        - CPU related info
  @param KtiInternalGlobal - KTIRC internal variables.
  @param CpuRes            - Array of CPU Resource Maps. Can be initialized in the OEM hook OemGetResourceMapUpdate
  
  @retval ReturnValue - KTI_FAILURE if Resource Map requirements are not met, else KTI_SUCCESS
  
**/
KTI_STATUS
WalkOemSocketResources(
PSYSHOST                 host,
KTI_SOCKET_DATA          *SocketData, 
KTI_HOST_INTERNAL_GLOBAL *KtiInternalGlobal,
KTI_CPU_RESOURCE         *CpuRes
)
{
  UINT8         Idx1;
  KTI_STATUS    ReturnValue = KTI_SUCCESS;
  UINT64_STRUCT MmiohTotal;
  UINT64_STRUCT MmiohGranularity;
  UINT8         SocketCoveredBitMap = 0;
  BOOLEAN IoStarved;
  BOOLEAN IoApicStarved;
  BOOLEAN MmiolStarved;
  BOOLEAN MmiohStarved;

  //
  // Check sockets against each other, ensure they don't overlap or have gaps
  //
  UINT32        IoIterator     = CpuRes[KtiInternalGlobal->SbspSoc].IoBase;
  UINT32        IoApicIterator = CpuRes[KtiInternalGlobal->SbspSoc].IoApicBase;
  UINT32        MmiolIterator  = CpuRes[KtiInternalGlobal->SbspSoc].MmiolBase;
  UINT64_STRUCT MmiohIterator  = CpuRes[KtiInternalGlobal->SbspSoc].MmiohBase;

  MmiohGranularity.lo = 1;
  MmiohGranularity.hi = 0;

  //Walk through each resource's range
  //NOTE: Idx1 is NOT iterated by this for() statement!
  for (Idx1 = 0; Idx1 < MAX_SOCKET;) {
    //Always start with SBSP, then ensure all IO ranges are in ascending order by Socket ID
    if (SocketCoveredBitMap == 0) {
      Idx1 = KtiInternalGlobal->SbspSoc;
    } else {
      if (Idx1 == KtiInternalGlobal->SbspSoc) {
        Idx1++;
        continue;
      }
    }

    //If this socket is enabled
    if ((SocketData->Cpu[Idx1].Valid == TRUE) || (KtiInternalGlobal->KtiCpuSktHotPlugEn == TRUE)) {

      //Determine which resources are starved for socket Idx1
      IoStarved = ((CpuRes[Idx1].IoLimit == 0) && (CpuRes[Idx1].IoBase == 0xFFFF));
      IoApicStarved = ((CpuRes[Idx1].IoApicLimit == 0) && (CpuRes[Idx1].IoApicBase == 0xFFFFFFFF));
      MmiolStarved = ((CpuRes[Idx1].MmiolLimit == 0) && (CpuRes[Idx1].MmiolBase == 0xFFFFFFFF));
      MmiohStarved = ((CpuRes[Idx1].MmiohLimit.lo == 0) && (CpuRes[Idx1].MmiohLimit.hi == 0) &&
                      (CpuRes[Idx1].MmiohBase.hi == 0xFFFFFFFF) && (CpuRes[Idx1].MmiohBase.lo == 0xFFFFFFFF));

      //Update all iterator values, if they follow the correct socket order
      //IO
      if (IoStarved == FALSE) {
        if (IoIterator == CpuRes[Idx1].IoBase) {
          IoIterator = CpuRes[Idx1].IoLimit + 1;
        } else {
          ReturnValue = KTI_FAILURE;
          KtiDebugPrintInfo0((host, KTI_DEBUG_INFO0, "\nIO resources are assigned to sockets out-of-order!\n"));
        }
      }

      //IO APIC
      if (IoApicStarved == FALSE) {
        if (IoApicIterator == CpuRes[Idx1].IoApicBase) {
          IoApicIterator = CpuRes[Idx1].IoApicLimit + 1;
        } else {
          ReturnValue = KTI_FAILURE;
          KtiDebugPrintInfo0((host, KTI_DEBUG_INFO0, "\nIO APIC resources are assigned to sockets out-of-order!\n"));
        }
      }

      //MMIOL
      if (MmiolStarved == FALSE) {
        if (MmiolIterator == CpuRes[Idx1].MmiolBase) {
          MmiolIterator = CpuRes[Idx1].MmiolLimit + 1;
        } else {
          ReturnValue = KTI_FAILURE;
          KtiDebugPrintInfo0((host, KTI_DEBUG_INFO0, "\nMMIOL Resources are assigned to sockets out-of-order!\n"));
        }
      }

      //MMIOH
      if (MmiohStarved == FALSE) {
        if (IsXEqualYUINT64(MmiohIterator, CpuRes[Idx1].MmiohBase)) {
          MmiohIterator.hi = CpuRes[Idx1].MmiohLimit.hi;
          MmiohIterator.lo = CpuRes[Idx1].MmiohLimit.lo + 1;
          if (MmiohIterator.lo == 0) {
            MmiohIterator.hi++;
          }
        } else {
          ReturnValue = KTI_FAILURE;
          KtiDebugPrintInfo0((host, KTI_DEBUG_INFO0, "\nMMIOH Ranges are assigned to sockets out-of-order!\n"));
        }
      }

      SocketCoveredBitMap |= (1 << Idx1);
    } else {
      //This socket is invalid, move to next socket ID
      Idx1++;
      continue;
    }

    //Break if a socket's resources are assigned out of order, continue of no problems found
    if (ReturnValue == KTI_FAILURE){
      break;
    } else {
      Idx1++;
    }
  }//Socket Loop

  //subtract one from each iterator variable, now indicates upper range for the system
  IoIterator--;
  MmiolIterator--;
  MmiohIterator.lo--;
  if (MmiohIterator.lo == 0xFFFFFFFF) {
    MmiohIterator.hi--;
  }
  MmiohTotal = SubUINT64(MmiohIterator, CpuRes[KtiInternalGlobal->SbspSoc].MmiohBase);
  MmiohGranularity = LShiftUINT64(MmiohGranularity, (2 * KtiInternalGlobal->MmiohTargetSize) + 30);

  if (ReturnValue == KTI_SUCCESS) {
    //last part of cleanup, examine the upper ranges of each resource
    if ((IoIterator != 0xFFFF) && (CpuRes[KtiInternalGlobal->SbspSoc].IoBase != 0xFFFF)) {
      KtiDebugPrintInfo0((host, KTI_DEBUG_INFO0, "\nIO Range is less than 64KB!\n"));
      ReturnValue = KTI_FAILURE;
    }
    if (RShiftUINT64(MmiohIterator, MAX_ADDR_LINE).lo != 0) {
      KtiDebugPrintInfo0((host, KTI_DEBUG_INFO0, "\nMMIOH upper limit is too high! only %d address lines allowed\n", MAX_ADDR_LINE));
      ReturnValue = KTI_FAILURE;
    }
    if (KtiInternalGlobal->TotCpu == 2) {
      if (KtiDetermineIfRequireNonInterleavedDecoder(host, SocketData, KtiInternalGlobal, CpuRes) == KTI_FAILURE) {
        KtiDebugPrintInfo0((host, KTI_DEBUG_INFO0, "\nMMIOH resources are more than can be covered by non-interleaved decoders\n"));
        ReturnValue = KTI_FAILURE;
      }
    } else {
      if (IsXGreaterThanYUINT64(MmiohTotal, MultUINT64x32(MmiohGranularity, 32))) {
        KtiDebugPrintInfo0((host, KTI_DEBUG_INFO0, "\nMMIOH upper limit is too high! only allow up to 32*MmiohGranularity of MMIOH\n"));
        ReturnValue = KTI_FAILURE;
      }
    }

    //MMCFG cannot overlap MMIOL region
    if (CpuRes[KtiInternalGlobal->SbspSoc].MmiolBase > CpuRes[KtiInternalGlobal->SbspSoc].SegMmcfgBase.lo) {
      if (CpuRes[KtiInternalGlobal->SbspSoc].SegMmcfgBase.lo + host->setup.common.mmCfgSize > CpuRes[KtiInternalGlobal->SbspSoc].MmiolBase) {
        KtiDebugPrintInfo0((host, KTI_DEBUG_INFO0, "\nMMCFG limit extends into MMIOL range\n"));
        ReturnValue = KTI_FAILURE;
      }
    }

    //Ensure that there is no MMIO hole between MMIOL and 4GB-64MB
    if (MmiolIterator != MMIOL_LIMIT) {
      KtiDebugPrintInfo0((host, KTI_DEBUG_INFO0, "\nMMIO hole exists between MMIOL Limit and 4GB-64MB!\n"));
      ReturnValue = KTI_FAILURE;
    }
  }
  return ReturnValue;
}

/**

  This routine checks each stack resource map in each socket. Make sure stack is valid and present, and resources fit within expected ranges.
  
  @param host              - Input/Output structure of the KTIRC
  @param SocketData        - CPU related info
  @param KtiInternalGlobal - KTIRC internal variables.
  @param CpuRes            - Array of CPU Resource Maps. Can be initialized in the OEM hook OemGetResourceMapUpdate
  
  @retval ReturnValue - KTI_FAILURE if Resource Map requirements are not met, else KTI_SUCCESS
  
**/
KTI_STATUS
CheckOemStackResources(
  PSYSHOST                 host,
  KTI_SOCKET_DATA          *SocketData,
  KTI_HOST_INTERNAL_GLOBAL *KtiInternalGlobal,
  KTI_CPU_RESOURCE         *CpuRes
  )
{
  UINT8 Idx1;
  UINT8 StackIdx1;
  KTI_STATUS ReturnValue = KTI_SUCCESS;
  BOOLEAN IoStarved;
  BOOLEAN IoApicStarved;
  BOOLEAN MmiolStarved;
  BOOLEAN MmiohStarved;
  UINT64_STRUCT MmiohGranularity;
  UINT64_STRUCT NegativeOne64Bit;
  UINT64_STRUCT Zero64Bit;
  NegativeOne64Bit.hi = 0xFFFFFFFF;
  NegativeOne64Bit.lo = 0xFFFFFFFF;
  Zero64Bit.hi = 0;
  Zero64Bit.lo = 0;
  MmiohGranularity.lo = 1;
  MmiohGranularity.hi = 0;

  MmiohGranularity = LShiftUINT64(MmiohGranularity, (2 * KtiInternalGlobal->MmiohTargetSize) + 30);
  MmiohGranularity = SubUINT64x32(MmiohGranularity, 1);
  //Perform checks on each stack, on each socket
  for (Idx1 = 0; Idx1 < MAX_SOCKET; Idx1++) {
    if ((SocketData->Cpu[Idx1].Valid == TRUE) || (KtiInternalGlobal->KtiCpuSktHotPlugEn == TRUE)) {
      for (StackIdx1 = 0; StackIdx1 < CpuRes[Idx1].TotEnabledStacks; StackIdx1++) {
        IoStarved = ((CpuRes[Idx1].StackRes[StackIdx1].IoLimit == 0) && (CpuRes[Idx1].StackRes[StackIdx1].IoBase == 0xFFFF));
        IoApicStarved = ((CpuRes[Idx1].StackRes[StackIdx1].IoApicLimit == 0) && (CpuRes[Idx1].StackRes[StackIdx1].IoApicBase == 0xFFFFFFFF));
        MmiolStarved = ((CpuRes[Idx1].StackRes[StackIdx1].MmiolLimit == 0) && (CpuRes[Idx1].StackRes[StackIdx1].MmiolBase == 0xFFFFFFFF));
        MmiohStarved = (IsXEqualYUINT64(CpuRes[Idx1].StackRes[StackIdx1].MmiohLimit, Zero64Bit) && IsXEqualYUINT64(CpuRes[Idx1].StackRes[StackIdx1].MmiohBase, NegativeOne64Bit));
        //Stacks 0-3 must have at least one bus each
        if (StackIdx1 <= 3){
          if (CpuRes[Idx1].StackRes[StackIdx1].BusLimit < CpuRes[Idx1].StackRes[StackIdx1].BusBase) {
            KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\nSocket %d Stack %d does not have 1 or more buses assigned\n", Idx1, StackIdx1));
            ReturnValue = KTI_FAILURE;
          }
        }

        //Ensure that each stack's range base >= Socket's range base, for all resources
        if (CpuRes[Idx1].StackRes[StackIdx1].IoBase < CpuRes[Idx1].IoBase) {
          KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\nSocket %d Stack %d IO Base is below socket's IO Range\n", Idx1, StackIdx1));
          ReturnValue = KTI_FAILURE;
        }
        if (CpuRes[Idx1].StackRes[StackIdx1].IoApicBase < CpuRes[Idx1].IoApicBase) {
          KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\nSocket %d Stack %d IO APIC Base is below socket's IO APIC Range\n", Idx1, StackIdx1));
          ReturnValue = KTI_FAILURE;
        }
        if (CpuRes[Idx1].StackRes[StackIdx1].MmiolBase < CpuRes[Idx1].MmiolBase) {
          KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\nSocket %d Stack %d MMIOL Base is below socket's MMIOL Range\n", Idx1, StackIdx1));
          ReturnValue = KTI_FAILURE;
        }
        if ((IsXGreaterThanYUINT64(CpuRes[Idx1].MmiohBase, CpuRes[Idx1].StackRes[StackIdx1].MmiohBase) == TRUE) && 
           (IsXEqualYUINT64(CpuRes[Idx1].MmiohBase, CpuRes[Idx1].StackRes[StackIdx1].MmiohBase) == FALSE)) {
          KtiDebugPrintInfo0((host, KTI_DEBUG_INFO0, "\nSocket %d Stack %d MMIOH Base is below socket's MMIOH Range\n", Idx1, StackIdx1));
          ReturnValue = KTI_FAILURE;
        }


        //Check Stack's Io Limit >= Stack's Io Base
        if (IoStarved == FALSE) {
          if (CpuRes[Idx1].StackRes[StackIdx1].IoLimit < CpuRes[Idx1].StackRes[StackIdx1].IoBase) {
            KtiDebugPrintInfo0((host, KTI_DEBUG_INFO0, "\nSocket %d Stack %d IO Limit < IO Base\n", Idx1, StackIdx1));
            ReturnValue = KTI_FAILURE;
          }
        }

        //Check Stack's IoApic Limit >= Stack's IoApic Base
        if (IoApicStarved == FALSE) {
          if (CpuRes[Idx1].StackRes[StackIdx1].IoApicLimit < CpuRes[Idx1].StackRes[StackIdx1].IoApicBase) {
            KtiDebugPrintInfo0((host, KTI_DEBUG_INFO0, "\nSocket %d Stack %d IO APIC Limit < IO APIC Base\n", Idx1, StackIdx1));
            ReturnValue = KTI_FAILURE;
          }
        }

        //Check Stack's Mmiol Limit >= Stack's Mmiol Base, and stack's MMIOL alignment
        if (MmiolStarved == FALSE) {
          if (CpuRes[Idx1].StackRes[StackIdx1].MmiolLimit < CpuRes[Idx1].StackRes[StackIdx1].MmiolBase) {
            KtiDebugPrintInfo0((host, KTI_DEBUG_INFO0, "\nSocket %d Stack %d MMIOL Limit < MMIOL Base\n", Idx1, StackIdx1));
            ReturnValue = KTI_FAILURE;
          }
          //Stack Mmiol must be 4MB aligned
          if ((CpuRes[Idx1].StackRes[StackIdx1].MmiolBase % 0x400000 != 0) && !(CpuRes[Idx1].StackRes[StackIdx1].MmiolBase == 0xFFFFFFFF)) {
            KtiDebugPrintInfo0((host, KTI_DEBUG_INFO0, "\nSocket %d Stack %d MMIOL is not 4MB aligned\n", Idx1, StackIdx1));
            ReturnValue = KTI_FAILURE;
          }
        }

        //Ensure Stack's MMIOH Limit >= Stack's MMIOH Base
        if (MmiohStarved == FALSE) {
          if (IsXGreaterThanYUINT64(CpuRes[Idx1].StackRes[StackIdx1].MmiohBase, CpuRes[Idx1].StackRes[StackIdx1].MmiohLimit) &&
             (IsXEqualYUINT64(CpuRes[Idx1].StackRes[StackIdx1].MmiohBase, CpuRes[Idx1].StackRes[StackIdx1].MmiohLimit) == FALSE)) {
            KtiDebugPrintInfo0((host, KTI_DEBUG_INFO0, "\nSocket %d Stack %d MMIOH Limit < MMIOH Base\n", Idx1, StackIdx1));
            ReturnValue = KTI_FAILURE;
          }
          if (IsXGreaterThanYUINT64(MmiohGranularity, SubUINT64(CpuRes[Idx1].StackRes[StackIdx1].MmiohLimit, CpuRes[Idx1].StackRes[StackIdx1].MmiohBase)) &&
             (IsXEqualYUINT64(MmiohGranularity, SubUINT64(CpuRes[Idx1].StackRes[StackIdx1].MmiohLimit, CpuRes[Idx1].StackRes[StackIdx1].MmiohBase)) == FALSE)) {
            KtiDebugPrintInfo0((host, KTI_DEBUG_INFO0, "\nSocket %d Stack %d MMIOH resources are less than the granularity\n", Idx1, StackIdx1));
            ReturnValue = KTI_FAILURE;
          }
        }

        //Exit the Stack loop if any problems are found with the resource map
        if (ReturnValue == KTI_FAILURE) {
          break;
        }
      }
    }//Stack Loop

    //Exit the Socket loop if any problems are found with the resource map
    if (ReturnValue == KTI_FAILURE) {
      break;
    }
  }//Socket loop

  return ReturnValue;
}

/**

  "Walk" each Stack resource, starting from each lowest base to each highest limit.
  There should be no gaps, and no overlapping ranges

  Operating with assumption that each stack resource range is above the previous stack. 
  (unless a stack has no resources of that type allocated)

  @param host              - Input/Output structure of the KTIRC
  @param SocketData        - CPU related info
  @param KtiInternalGlobal - KTIRC internal variables.
  @param CpuRes            - Array of CPU Resource Maps. Can be initialized in the OEM hook OemGetResourceMapUpdate

  @retval ReturnValue - KTI_FAILURE if Resource Map requirements are not met, else KTI_SUCCESS

**/
KTI_STATUS
WalkOemStackResources(
  PSYSHOST                 host,
  KTI_SOCKET_DATA          *SocketData,
  KTI_HOST_INTERNAL_GLOBAL *KtiInternalGlobal,
  KTI_CPU_RESOURCE         *CpuRes
  )
{
  UINT8 Idx1;
  UINT8 StackIdx1;
  KTI_STATUS ReturnValue = KTI_SUCCESS;

  UINT8         BusIterator;
  UINT32        IoIterator;
  UINT32        IoApicIterator;
  UINT32        MmiolIterator;
  UINT64_STRUCT MmiohIterator;
  UINT64_STRUCT NegativeOne64Bit;
  UINT64_STRUCT Zero64Bit;
  BOOLEAN       IoStarved;
  BOOLEAN       IoApicStarved;
  BOOLEAN       MmiolStarved;
  BOOLEAN       MmiohStarved;
  NegativeOne64Bit.hi = 0xFFFFFFFF;
  NegativeOne64Bit.lo = 0xFFFFFFFF;
  Zero64Bit.hi = 0;
  Zero64Bit.lo = 0;  

  //Check each stack against other stacks in the socket
  /**Walk map to make sure that each range is complete, with no gaps**/
  for (Idx1 = 0; Idx1 < MAX_SOCKET; Idx1++) {
    if ((SocketData->Cpu[Idx1].Valid == TRUE ) || (KtiInternalGlobal->KtiCpuSktHotPlugEn == TRUE)) {
      if (SocketData->Cpu[Idx1].SocType == SOCKET_TYPE_CPU) {  
        BusIterator = CpuRes[Idx1].BusBase;
        IoIterator = CpuRes[Idx1].IoBase;
        IoApicIterator = CpuRes[Idx1].IoApicBase;
        MmiolIterator = CpuRes[Idx1].MmiolBase;
        MmiohIterator = CpuRes[Idx1].MmiohBase;

        //Stacks must be in ascending order for all resources, if resources are assigned 
        for (StackIdx1 = 0; StackIdx1 < MAX_IIO_STACK; StackIdx1++) {
          if ((host->var.kti.CpuInfo[Idx1].CpuRes.stackPresentBitmap >> StackIdx1) & 1) {
            IoStarved = ((CpuRes[Idx1].StackRes[StackIdx1].IoLimit == 0) && (CpuRes[Idx1].StackRes[StackIdx1].IoBase == 0xFFFF));
            IoApicStarved = ((CpuRes[Idx1].StackRes[StackIdx1].IoApicLimit == 0) && (CpuRes[Idx1].StackRes[StackIdx1].IoApicBase == 0xFFFFFFFF));
            MmiolStarved = ((CpuRes[Idx1].StackRes[StackIdx1].MmiolLimit == 0) && (CpuRes[Idx1].StackRes[StackIdx1].MmiolBase == 0xFFFFFFFF));
            MmiohStarved = (IsXEqualYUINT64(CpuRes[Idx1].StackRes[StackIdx1].MmiohLimit, Zero64Bit) && IsXEqualYUINT64(CpuRes[Idx1].StackRes[StackIdx1].MmiohBase, NegativeOne64Bit));
            
            if ((BusIterator == CpuRes[Idx1].StackRes[StackIdx1].BusBase) && (CpuRes[Idx1].StackRes[StackIdx1].BusBase != 0xFF)) {
              BusIterator = CpuRes[Idx1].StackRes[StackIdx1].BusLimit + 1;
            } else {
              if (((CpuRes[Idx1].StackRes[StackIdx1].BusBase == 0xFF) && (CpuRes[Idx1].StackRes[StackIdx1].BusLimit == 0x00)) == FALSE) {
                KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\nSocket %d stack %d Bus assignments are incomplete or overlapping\n", Idx1, StackIdx1));
                ReturnValue = KTI_FAILURE;
              }
            }
            if (IoIterator == CpuRes[Idx1].StackRes[StackIdx1].IoBase) {
              IoIterator = CpuRes[Idx1].StackRes[StackIdx1].IoLimit + 1;
            } else {
              if (IoStarved == FALSE) {
                KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\nSocket %d stack %d IO assignments are incomplete or overlapping\n", Idx1, StackIdx1));
                ReturnValue = KTI_FAILURE;
              }
            }
            if (IoApicIterator == CpuRes[Idx1].StackRes[StackIdx1].IoApicBase) {
              IoApicIterator = CpuRes[Idx1].StackRes[StackIdx1].IoApicLimit + 1;
            } else {
              if (IoApicStarved == FALSE) {
                KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\nSocket %d stack %d IO APIC assignments are incomplete or overlapping\n", Idx1, StackIdx1));
                ReturnValue = KTI_FAILURE;
              }
            }
            if (MmiolIterator == CpuRes[Idx1].StackRes[StackIdx1].MmiolBase) {
              MmiolIterator = CpuRes[Idx1].StackRes[StackIdx1].MmiolLimit + 1;
            } else {
              if (MmiolStarved == FALSE) {
                KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\nSocket %d stack %d MMIOL assignments are incomplete or overlapping\n", Idx1, StackIdx1));
                ReturnValue = KTI_FAILURE;
              }
            }
            if ((MmiohIterator.hi == CpuRes[Idx1].StackRes[StackIdx1].MmiohBase.hi) && (MmiohIterator.lo == CpuRes[Idx1].StackRes[StackIdx1].MmiohBase.lo)){
              MmiohIterator.lo = CpuRes[Idx1].StackRes[StackIdx1].MmiohLimit.lo + 1;
              MmiohIterator.hi = CpuRes[Idx1].StackRes[StackIdx1].MmiohLimit.hi;
              if (!MmiohIterator.lo) {
                MmiohIterator.hi++;
              }
            } else {
              if (MmiohStarved == FALSE) {
                KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\nSocket %d stack %d MMIOH assignments are incomplete or overlapping\n", Idx1, StackIdx1));
                ReturnValue = KTI_FAILURE;
              }
            }
          }
        }
        if (ReturnValue == KTI_FAILURE) {
          return KTI_FAILURE;
        }
        
        BusIterator--;
        IoIterator--;
        IoApicIterator--;
        MmiolIterator--;
        MmiohIterator.lo--;
        if (MmiohIterator.lo == 0xFFFFFFFF) {
          MmiohIterator.hi--;
        }
    
        if (BusIterator != CpuRes[Idx1].BusLimit) {
          KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\nSocket %d stack Bus assignments are incomplete or overlapping\n", Idx1));
          ReturnValue = KTI_FAILURE;
        }
        if ((IoIterator != CpuRes[Idx1].IoLimit) && (CpuRes[Idx1].IoBase != 0xFFFF)) {
          KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\nSocket %d stack IO assignments are incomplete or overlapping\n", Idx1));
          ReturnValue = KTI_FAILURE;
        }
        if ((IoApicIterator != CpuRes[Idx1].IoApicLimit) && (CpuRes[Idx1].IoApicBase != 0xFFFFFFFF)) {
          KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\nSocket %d stack IO APIC assignments are incomplete or overlapping\n", Idx1));
          ReturnValue = KTI_FAILURE;
        }
        if ((MmiolIterator != CpuRes[Idx1].MmiolLimit) && (CpuRes[Idx1].MmiolBase != 0xFFFFFFFF)) {
          KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\nSocket %d stack MMIOL assignments are incomplete or overlapping\n", Idx1));
          ReturnValue = KTI_FAILURE;
        }
        if (((MmiohIterator.hi != CpuRes[Idx1].MmiohLimit.hi) || (MmiohIterator.lo != CpuRes[Idx1].MmiohLimit.lo)) &&
           (((CpuRes[Idx1].MmiohBase.lo == 0xFFFFFFFF) && (CpuRes[Idx1].MmiohBase.lo == 0xFFFFFFFF)) == FALSE)) {
          KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\nSocket %d stack MMIOH assignments are incomplete or overlapping\n", Idx1));
          ReturnValue = KTI_FAILURE;
        }
        
      } else if (SocketData->Cpu[Idx1].SocType == SOCKET_TYPE_FPGA) {
        for (StackIdx1 = 0; StackIdx1 < MAX_IIO_STACK; StackIdx1++) {
          if ((host->var.kti.CpuInfo[Idx1].CpuRes.stackPresentBitmap >> StackIdx1) & 1) {
            if (StackIdx1 > 0) {
              KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\nFPGA %d only allow 1 IIO Stack enabled\n", Idx1));
              ReturnValue = KTI_FAILURE;
            }
            // Only 1 bus is assigned to the FPGA Stack 0
            if (CpuRes[Idx1].StackRes[StackIdx1].BusLimit != CpuRes[Idx1].StackRes[StackIdx1].BusBase) {
              KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\nFPGA %d Bus resource is illegal!!!\n", Idx1));
              ReturnValue = KTI_FAILURE;
            }
            if (!((CpuRes[Idx1].StackRes[StackIdx1].IoLimit == 0) && (CpuRes[Idx1].StackRes[StackIdx1].IoBase == 0xFFFF))) {
              KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\nFPGA %d IO resource is illegal!!!\n", Idx1));
              ReturnValue = KTI_FAILURE;   
            }
            if (!((CpuRes[Idx1].StackRes[StackIdx1].IoApicLimit == 0) && (CpuRes[Idx1].StackRes[StackIdx1].IoApicBase == 0xFFFFFFFF))) {
              KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\nFPGA %d IoApic resource is illegal!!!\n", Idx1));
              ReturnValue = KTI_FAILURE;   
            }
            if (!((CpuRes[Idx1].StackRes[StackIdx1].MmiolLimit == 0) && (CpuRes[Idx1].StackRes[StackIdx1].MmiolBase == 0xFFFFFFFF))) {
              KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\nFPGA %d mmiol resource is illegal!!!\n", Idx1));
              ReturnValue = KTI_FAILURE;   
            }
            if (!(IsXEqualYUINT64(CpuRes[Idx1].StackRes[StackIdx1].MmiohLimit, Zero64Bit) && IsXEqualYUINT64(CpuRes[Idx1].StackRes[StackIdx1].MmiohBase, NegativeOne64Bit))) {
              KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\nFPGA %d mmioh resource is illegal!!!\n", Idx1));
              ReturnValue = KTI_FAILURE;   
            }
          }
        } 
      }
    }
    if (ReturnValue != KTI_SUCCESS){
      break;
    }
  }

  return ReturnValue;
}

/**

  This routine calls out for OEM modifications to the resource map

  @param host              - Input/Output structure of the KTIRC
  @param SocketData        - CPU related info
  @param KtiInternalGlobal - KTIRC internal variables.

  @retval KTI_SUCCESS

**/
KTI_STATUS
CheckForOemResourceUpdate (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal
  )
{

  KTI_CPU_RESOURCE CpuRes[MAX_SOCKET];
  KTI_OEMCALLOUT   SystemParams;
  UINT32           ResourceMapChange;
  UINT64_STRUCT    MmiohGranularitySize;
  UINT8            Socket;
  UINT32         OemMmiolBase;
  UINT32           OemMmcfgBase;
  UINT8          OemTolm;
  KTI_STATUS       OemResourceMapStatus;

  OemResourceMapStatus = KTI_SUCCESS;
  Socket = 0;
  OemMmcfgBase = host->setup.common.mmCfgBase;
  OemMmiolBase = host->setup.common.mmiolBase;
  OemTolm = FOUR_GB_MEM - (OemMmcfgBase >> 26);

  SystemParams.SizeOfCpuRes= sizeof(CpuRes);
  SystemParams.CpuList = KtiInternalGlobal->CpuList;
  SystemParams.Sbsp = KtiInternalGlobal->SbspSoc;
  SystemParams.mmcfgSize = host->setup.common.mmCfgSize;
  MmiohGranularitySize.lo = 1;
  MmiohGranularitySize.hi = 0;
  SystemParams.mmiohGranularity = LShiftUINT64(MmiohGranularitySize, (2 * KtiInternalGlobal->MmiohTargetSize) + 30);
  for (Socket=0; Socket < MAX_SOCKET; Socket++) {
    MemCopy ((UINT8 *)&CpuRes[Socket], (UINT8 *)&host->var.kti.CpuInfo[Socket].CpuRes, sizeof (KTI_CPU_RESOURCE));
  }
  //
  // Call to OEM hook
  //
  OemGetResourceMapUpdate (host, CpuRes, SystemParams);

  //IF OEM did not change resource map, do nothing and return success.
  ResourceMapChange = 0;
  for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
    ResourceMapChange = MemCompare ((UINT8 *)&CpuRes[Socket], (UINT8 *)&host->var.kti.CpuInfo[Socket].CpuRes, sizeof (KTI_CPU_RESOURCE));
    if (ResourceMapChange){
      break; //Something changed, need to validate entire resource map.
    }
  }
  if (!ResourceMapChange) {
    //No OEM change, exit before we do anything else
    return SUCCESS;
  }
  
  KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\n  OEM Overriding Resource Allocation \n"));

  // Cannot support MMCFG above 4GB, and SBSP must be in first segment/lowest bus
  if (host->setup.common.mmCfgBase < host->setup.common.mmiolBase) {
    if (host->setup.common.mmiolBase != CpuRes[KtiInternalGlobal->SbspSoc].MmiolBase) {
      KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\nOEM Resource map MMIOL Base does not match setup variable\n"));
      KtiDebugPrintInfo0((host, KTI_DEBUG_INFO0, "This usually indicates MMCFG base or MMCFG size has changed\n"));
      OemResourceMapStatus = KTI_FAILURE;
    }
  }

  //Validate CpuResourceMap from OEM hook.
  //1. Check Segment Resources
  if (OemResourceMapStatus == KTI_SUCCESS) {
    OemResourceMapStatus = CheckOemSegmentResources (host, SocketData, KtiInternalGlobal, CpuRes);
  }
  
  //2. Check intra-Socket Resources
  if (OemResourceMapStatus == KTI_SUCCESS) {
    OemResourceMapStatus = CheckOemSocketResources (host, SocketData, KtiInternalGlobal, CpuRes);
  }

  //3. Check Sockets against each other, walking from SBSP resource bases to the highest limits on each socket
  if (OemResourceMapStatus == KTI_SUCCESS) {
    OemResourceMapStatus = WalkOemSocketResources (host, SocketData, KtiInternalGlobal, CpuRes);
  }

  //4. Check that each stack resource makes sense in the context of its parent socket
  if (OemResourceMapStatus == KTI_SUCCESS) {
    OemResourceMapStatus = CheckOemStackResources (host, SocketData, KtiInternalGlobal, CpuRes);
  }

  //5. Starting from each resource base on each socket, iterate through stacks to reach the socket's resource limit
  //   ensure that stacks consume entire resources of socket
  if (OemResourceMapStatus == KTI_SUCCESS) {
    OemResourceMapStatus = WalkOemStackResources (host, SocketData, KtiInternalGlobal, CpuRes);
  }

  // Find top of "low" memory, and assign it to host's lowGap member variable
  // Find MmcfgBase, assign back to KtiInternalGlobal's MmcfgBase  member variable
//  APTIOV_SERVER_OVERRIDE_RC_START : Changes done to resolve the CATERR 
//                                    After OOR NVRAM Created and if MMCFG is lowered by OOR and in next boot if MMCFG
//                                    is changed by user in setup, in some systems CATERR was seen upon reboot.
  if (OemResourceMapStatus == KTI_SUCCESS) {
  if (OemMmcfgBase != CpuRes[KtiInternalGlobal->SbspSoc].SegMmcfgBase.lo) {
    OemMmcfgBase = CpuRes[KtiInternalGlobal->SbspSoc].SegMmcfgBase.lo;
  }
//  APTIOV_SERVER_OVERRIDE_RC_END : Changes done to resolve the CATERR 
//                                    After OOR NVRAM Created and if MMCFG is lowered by OOR and in next boot if MMCFG
//                                    is changed by user in setup, in some systems CATERR was seen upon reboot.
  //
  // Check for new mmiolbase
  //
  for (Socket = 0; Socket < MAX_SOCKET; Socket++){
    //
    // if socket exists or Hot Plug enabled (reserved for onlining sockets)
    //
    if ((SocketData->Cpu[Socket].Valid == TRUE) || (KtiInternalGlobal->KtiCpuSktHotPlugEn == TRUE)){
      if (OemMmiolBase > CpuRes[Socket].MmiolBase) {
        OemMmiolBase = CpuRes[Socket].MmiolBase;
      }
    }
  }

  if (OemMmiolBase < OemMmcfgBase) {
    KtiInternalGlobal->IsMmcfgAboveMmiol = TRUE;
    OemTolm = FOUR_GB_MEM - (OemMmiolBase >> 26);
  } else {
    KtiInternalGlobal->IsMmcfgAboveMmiol = FALSE;
    OemTolm = FOUR_GB_MEM - (OemMmcfgBase >> 26);
  }

  //
  // setup.common.lowgap should be moved to var.common.lowgap to for consistent usage, but will require MRC change
  //
  host->setup.common.lowGap = OemTolm;
  KtiInternalGlobal->MmcfgBase = OemMmcfgBase;
  //
  // need to update this var based on MmiohBase but MRC requires update first to consume
  //
  //host->setup.common.highGap = HIGH_GAP;            // High MMIO gap = 256 MB in 256MB chunks
  //

//  APTIOV_SERVER_OVERRIDE_RC_START : Changes done to resolve the CATERR 
//                                    After OOR NVRAM Created and if MMCFG is lowered by OOR and in next boot if MMCFG
//                                    is changed by user in setup, in some systems CATERR was seen upon reboot.
 // if (OemResourceMapStatus == KTI_SUCCESS) {
//  APTIOV_SERVER_OVERRIDE_RC_END : Changes done to resolve the CATERR 
//                                    After OOR NVRAM Created and if MMCFG is lowered by OOR and in next boot if MMCFG
//                                    is changed by user in setup, in some systems CATERR was seen upon reboot. 
    //
    // Check for special case in 2S where need to make use of non-interleaved decoder
    //

    for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
      MemCopy ((UINT8 *)&host->var.kti.CpuInfo[Socket].CpuRes, (UINT8 *)&CpuRes[Socket], sizeof(KTI_CPU_RESOURCE));
    }
  } else {
    KtiDebugPrintWarn ((host, KTI_DEBUG_WARN, "\n OEM Resource Map is invalid. Using default resource map"));
    KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\nThe following OEM Defined CPU Resource map will be discarded: "));
    PrintResourceMapHeader(host);
    for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
      if ((SocketData->Cpu[Socket].Valid == TRUE) || (KtiInternalGlobal->KtiCpuSktHotPlugEn == TRUE)){
        PrintCpuResourceMapDetails (host, Socket, SocketData, CpuRes[Socket]);
      }
    }
    KtiLogWarning (host, 0xFF, 0, 0, ERR_SAD_SETUP);
  }

  return KTI_SUCCESS;
}


/**

  This routine calls out for OEM modifications to the resource map

  @param host              - Input/Output structure of the KTIRC
  @param SocketData        - CPU related info
  @param KtiInternalGlobal - KTIRC internal variables.
  @param CpuRes            - Custom CpuRes definitions.

  @retval KTI_SUCCESS

**/
KTI_STATUS
KtiDetermineIfRequireNonInterleavedDecoder (
  struct sysHost           *host,
  KTI_SOCKET_DATA          *SocketData,
  KTI_HOST_INTERNAL_GLOBAL *KtiInternalGlobal,
  KTI_CPU_RESOURCE         *CpuRes
  )
{
  UINT8          Idx1, Idx2;
  UINT64_STRUCT  MmiohGranularitySize, MaxMmiohInterleavedSize, TotalMmiohSize;
  UINT64_STRUCT  SocketMmiohRequestedSize[2] = {{0, 0}, {0, 0}};

  MmiohGranularitySize.lo = 1;
  MmiohGranularitySize.hi = 0;
  MmiohGranularitySize = LShiftUINT64(MmiohGranularitySize, (2 * KtiInternalGlobal->MmiohTargetSize) + 30);
  MaxMmiohInterleavedSize = MultUINT64x32(MmiohGranularitySize, MAX_SAD_TARGETS);

  if (KtiInternalGlobal->TotCpu == 2) {
    Idx2 = 0;
    for (Idx1=0; Idx1 < KtiInternalGlobal->TotCpu; Idx1++) {
      if (SocketData->Cpu[Idx1].Valid && (SocketData->Cpu[Idx1].SocType == SOCKET_TYPE_CPU)) {
        SocketMmiohRequestedSize[Idx2] = AddUINT64x32(SubUINT64(CpuRes[Idx1].MmiohLimit, CpuRes[Idx1].MmiohBase), (UINT32)1);
        Idx2++;
      }
    }

    TotalMmiohSize = AddUINT64(SocketMmiohRequestedSize[0], SocketMmiohRequestedSize[1]);

    if (IsXGreaterThanYUINT64(TotalMmiohSize, MaxMmiohInterleavedSize)) {
      //
      // request is for more than interleaved decoder can cover
      //
      if ((IsXGreaterThanYUINT64(MaxMmiohInterleavedSize, SocketMmiohRequestedSize[0]) || IsXEqualYUINT64(MaxMmiohInterleavedSize, SocketMmiohRequestedSize[0])) &&
          (IsXGreaterThanYUINT64(MaxMmiohInterleavedSize, SocketMmiohRequestedSize[1]) || IsXEqualYUINT64(MaxMmiohInterleavedSize, SocketMmiohRequestedSize[1]))) {
        //
        // But each individual socket can fit within the limit, so we will use interleaved to map local mmioh and noninterleaved to map remote mmioh
        //
        KtiInternalGlobal->UseNonInterleavedMmioh = TRUE;
      } else {
        return KTI_FAILURE;
      }
    }
  }
  return KTI_SUCCESS;
}


/**

  Determines if X > Y

  @param X
  @param Y

  @retval TRUE/FALSE

**/
BOOLEAN
IsXGreaterThanYUINT64 (
  UINT64_STRUCT X,
  UINT64_STRUCT Y
  )
{
  BOOLEAN Rc;
  if (X.hi > Y.hi) {
   Rc = TRUE;
  } else if (X.hi < Y.hi) {
    Rc = FALSE;
  } else if (X.lo >= Y.lo) {
    Rc = TRUE;
  } else {
    Rc = FALSE;
  }
  return Rc;
}


/**

  Determines if X == Y

  @param X
  @param Y

  @retval TRUE/FALSE

**/
BOOLEAN
IsXEqualYUINT64 (
  UINT64_STRUCT X,
  UINT64_STRUCT Y
  )
{
  BOOLEAN Rc;
  if ((X.hi == Y.hi) && (X.lo == Y.lo)) {
    Rc = TRUE;
  } else {
    Rc = FALSE;
  }
  return Rc;
}


/**

  Calculate the M3KTI pass-thru route encoding for given input & output port

  @param InPort       - input KTI port for route through
  @param OutPort      - Output KTI port for route through
  @param DestSocId    - endpoint processor socket ID
  @param EncodedRoute - pointer to current m3kti routing table value

  @retval 0 - Successful
  @retval 1 - Failure

**/
KTI_STATUS
ConvertM3KTIRoutingTable (
  UINT8                   InPort,
  UINT8                   OutPort,
  UINT8                   DestSocId,
  UINT32                  *Data32
  )
{
  UINT32       StartBit;

  StartBit = 16 * (InPort & 0x1) + (2 * DestSocId);   // even port uses bit 0-15, odd port uses bit 16-31
  *Data32 &= (~(3 << StartBit));
  *Data32 |= (OutPort << StartBit);   // routing through for dest socket

  return KTI_SUCCESS;
}

/**

  Routine to interpret the system topology in the format used by Enhanced Warning Log
  I.E. 32 bit bit mask:
  [31:28] - link bit mask for socket 7
  [27:24] - link bit mask for socket 6
   ....
  [7:4]   - link bit mask for socket 1
  [3:0]   - link bit mask for socket 0

  @param host              - Input/Output structure of the KTIRC
  @param SocketData        - Info such as CPUs populated and their LEP.
  @param KtiInternalGlobal - KTIRC internal variables.

  @retval KTI_SUCCESS - on successful completion

**/
UINT32
GetTopologyBitmap(
  PSYSHOST                 host,
  KTI_SOCKET_DATA          *SocketData,
  KTI_HOST_INTERNAL_GLOBAL *KtiInternalGlobal
) {

  UINT8  Index;
  UINT8  Port;
  UINT32 TopologyBitmap = 0;

  for (Index = 0; Index < MAX_SOCKET; Index++) {
    if (SocketData->Cpu[Index].Valid == TRUE) {
      for (Port = 0; Port < host->var.common.KtiPortCnt; Port++) {
        if (SocketData->Cpu[Index].LinkData[Port].Valid == TRUE) {
          TopologyBitmap = TopologyBitmap | (1 << ((Index * 4) + Port));
        }
      }
    }
  }
  return TopologyBitmap;
}

/**

  Routine to check if the topology changed after warm reset. Any such topology change requires
  KTIRC cold reset flow to be forced on the system since the uncore mesh programming is no longer correct for the
  changed topology. Note that when such topology change occurs it could be only due to some links not training; it can not
  be due to some links that failed to train before but trained successfully this time around. The reason is a link
  that failt to train is disabled by clearing c_init_begin. So such links can successfully train only after Cold Reset.

  @param host              - Input/Output structure of the KTIRC
  @param SocketData        - Info such as CPUs populated and their LEP.
  @param KtiInternalGlobal - KTIRC internal variables.

  @retval KTI_SUCCESS - on successful completion

**/
KTI_STATUS
CheckForTopologyChange (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal
  )
{
  UINT8           Index, Port;
  UINT32          Data32, Sr02;
  UINT8           ScratchpadBits;
  UINT32          Scratchpad2Topology;
  UINT32          NewTopologyBitmap;

  // Collect the previous topology/LEP from SR02
  Data32 = KtiReadPciCfg(host, KtiInternalGlobal->SbspSoc, 0, BIOSSCRATCHPAD2_UBOX_MISC_REG);
  Sr02 = (Data32 & 0xFFFFFF00) >> 8;

  ScratchpadBits = 8;
  Scratchpad2Topology = 0;
  for (Index = 0; Index < MAX_SOCKET; Index++){
    for (Port = 0; Port < SI_MAX_KTI_PORTS; Port++){
      if (1 << ScratchpadBits & Data32){
        Scratchpad2Topology |= (1 << ((Index * 4) + Port));
      }
      ScratchpadBits++;
    }
  }

  // Check if the current LEP matches previous boot LEP
  for (Index = 0; Index < MAX_SOCKET; Index++) {
    if (SocketData->Cpu[Index].Valid == TRUE) {
      for (Port = 0; Port < host->var.common.KtiPortCnt; Port++) {
        if (SocketData->Cpu[Index].LinkData[Port].Valid != ((Sr02 >> ((Index * SI_MAX_KTI_PORTS) + Port)) & 0x1)) {
          NewTopologyBitmap = GetTopologyBitmap (host, SocketData, KtiInternalGlobal);
          KtiLogTopologyChange (host, Scratchpad2Topology, NewTopologyBitmap, LinkFail);
          KtiLogWarning (host, Index, SOCKET_TYPE_CPU, Port, WARN_LINK_FAILURE);
          KtiDebugPrintWarn ((host, KTI_DEBUG_WARN, "\n    CPU%u Link%u failed to train. Topology changed across reset!!! ", Index, Port));
          KtiInternalGlobal->ForceColdResetFlow = TRUE; // Indicate that the Cold Reset flow needs to be redone
          if (host->setup.kti.DfxParm.DfxHaltLinkFailReset == KTI_ENABLE) {
            KTI_ASSERT(FALSE, ERR_UNSUPPORTED_TOPOLOGY, MINOR_ERR_TOPOLOGY_CHANGE);
          }
        }
      }
    }
  }

  return KTI_SUCCESS;
}

/**

  interrogatest discovered topology for errors

  @param host              - Pointer to the system host (root) structure
  @param SocketData        - socket specific data structure
  @param KtiInternalGlobal - KTIRC internal variables.


  @retval 0 - Successful
  @retval 1 - Failures

**/
KTI_STATUS
SanityCheckDiscoveredTopology (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal
  )
{
  BOOLEAN             ErrFlag;
  UINT8               Index;
  UINT8               Index1;
  UINT32              TempCpuList;
  TOPOLOGY_TREE_NODE  *TopologyTree;

  ErrFlag = FALSE;

  //
  // Topology Tree sanity check. Check the topology tree of each socket to see if it has all the other sockets in its tree.
  //
  for (Index = 0; Index < MAX_SOCKET; Index++) {
    TempCpuList = 0;
    if ((SocketData->Cpu[Index].Valid == TRUE) && (SocketData->Cpu[Index].SocType == SOCKET_TYPE_CPU)) {
      TopologyTree  = SocketData->CpuTree[Index];
      TempCpuList   = KtiInternalGlobal->CpuList;
      for (Index1 = 0; TopologyTree[Index1].Node.Valid != FALSE; Index1++) {
        if (TopologyTree[Index1].Node.SocType == SOCKET_TYPE_CPU) {
          if (TempCpuList) {
            TempCpuList &= (~(1 << TopologyTree[Index1].Node.SocId));
          } else {
            KtiDebugPrintFatal ((host, KTI_DEBUG_ERROR, "\n CPU%u not found in CPU%u Topology Tree!!! ", Index1, Index));
            ErrFlag = TRUE;
            break;
          }
        } else if (TopologyTree[Index1].Node.SocType == SOCKET_TYPE_FPGA) {
        } else {
          KtiDebugPrintFatal ((host, KTI_DEBUG_ERROR, "\n Invalid Socket Type found in CPU%u Topology Tree!!! ", Index));
          ErrFlag = TRUE;
        }
      }
    }

    if (TempCpuList) {
      KtiDebugPrintFatal ((host, KTI_DEBUG_ERROR, "\n One or more CPUs not found in CPU%u Topology Tree!!! ", Index));
      ErrFlag = TRUE;
    }
  }

  if (ErrFlag == TRUE) {
    KtiDebugPrintFatal ((host, KTI_DEBUG_ERROR, "\n Topology Discovery Failed!!!\n"));
    if (host->var.kti.RasInProgress == TRUE) {
      return KTI_FAILURE;
    } else {
      KTI_ASSERT (FALSE, ERR_TOPOLOGY_DISCOVERY, MINOR_ERR_SANITY_CHECK);
    }
  }

  return KTI_SUCCESS;
}

/**

  Sanity checks the LEP data.

  @param host    - Pointer to the system host (root) structure
  @param CpuData - socket specific data structure

  @retval KTI_SUCCESS - on successful completion

**/
KTI_STATUS
SanityCheckLep (
  struct sysHost         *host,
  KTI_CPU_SOCKET_DATA    *CpuData
  )
{
  UINT8   Idx, Port;
  UINT8   PeerSocId, PeerPort;

  //
  // Do some sanity checking of the link exchange data such as link Valid,
  // Peer Socket ID and Peer Port are consistent at both the ends of a given link.
  //

  for (Idx = 0; Idx < MAX_SOCKET; Idx++) {
    if (CpuData[Idx].Valid == TRUE) {
      for (Port = 0; Port < host->var.common.KtiPortCnt; Port++) {
        if (CpuData[Idx].LinkData[Port].Valid == TRUE) {
          PeerSocId = (UINT8) CpuData[Idx].LinkData[Port].PeerSocId;
          PeerPort  = (UINT8) CpuData[Idx].LinkData[Port].PeerPort;
          if (CpuData[PeerSocId].Valid != TRUE ||
              CpuData[PeerSocId].LinkData[PeerPort].Valid != TRUE ||
              CpuData[PeerSocId].LinkData[PeerPort].PeerSocId != Idx ||
              CpuData[PeerSocId].LinkData[PeerPort].PeerPort != Port) {
            KtiDebugPrintFatal ((host, KTI_DEBUG_ERROR, "\n CPU%u Port%u has incosistent LEP with Peer socket. Topology Not Supported ", Idx, Port));
            KTI_ASSERT (FALSE, ERR_SW_CHECK, MINOR_ERR_SW_CHECK_1B);
          }
        }
      }
    }
  }

  return KTI_SUCCESS;
}

/**

  Chooses config precedence topology vs feature

  @param host              - Input/Output structure of the KTIRC
  @param SocketData        - CPU related info
  @param KtiInternalGlobal - KTIRC internal variables.

  @retval KTI_SUCCESS - on successful completion

**/
KTI_STATUS
PreProcessFeatureVsTopology (
  struct sysHost             *host,
  KTI_SOCKET_DATA            *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal
  )
{
  UINT8 topology = host->var.kti.SysConfig;
  UINT32 PreDegradeTopo;
  UINT32 PostDegradeTopo;
  PreDegradeTopo = GetTopologyBitmap(host, SocketData, KtiInternalGlobal);
  //
  // By default we will choose topology as precedence for defining configuration, if setup option is set then choose feature as precedence
  // If topology precedence is set, PrimeHostStructureIsoc() will disable isoc in unsupported topologies (i.e. 2S1L)
  //
  if (host->setup.kti.DegradePrecedence == FEATURE_PRECEDENCE) {
    //Isoc will only be enabled in 1S, and 2S2L
    //if 2S1L topology with FEATURE_PRECEDENCE set, reduce to 1S
    if (host->setup.common.isocEn == 1 &&
        topology == SYS_CONFIG_2S1L){
      host->var.kti.SysConfig = SYS_CONFIG_1S;
      DegradeTo1S (host, SocketData, KtiInternalGlobal);
      PostDegradeTopo = GetTopologyBitmap(host, SocketData, KtiInternalGlobal);
      KtiLogTopologyChange (host, PreDegradeTopo, PostDegradeTopo, FeatureVsTopology);
    }
  }
  return KTI_SUCCESS;
}

/**

  Dump topology info to output device

  @param host  - Pointer to the system host (root) structure
  @param Cpu   - socket specific data structure

  @retval None

**/
VOID
TopologyInfoDump (
  struct sysHost      *host,
  KTI_CPU_SOCKET_DATA     *Cpu,
  TOPOLOGY_TREE_NODE     (*CpuTree)[MAX_TREE_NODES],
  BOOLEAN                    SiPort
  )
{
#if (KTI_DEBUG_LEVEL & KTI_DEBUG_INFO1)
  UINT8               Index, Index1, Index2;
  UINT8               Port;
  UINT8               RootVn;
  UINT8               TempStr[MAX_SOCKET * 2];
  TOPOLOGY_TREE_NODE  *TopologyTree;
  TOPOLOGY_TREE_NODE  Parent, Child;

  for (Index = 0; Index < MAX_SOCKET; Index++) {
    if ((Cpu[Index].Valid == TRUE) && (Cpu[Index].SocType == SOCKET_TYPE_CPU)) {
      TopologyTree = CpuTree[Index];
      KtiDebugPrintInfo1 ((host, KTI_DEBUG_INFO1, "\n\nCPU%u Topology Tree", Index));
      KtiDebugPrintInfo1 ((host, KTI_DEBUG_INFO1, "\n-------------------"));
      KtiDebugPrintInfo1 ((host, KTI_DEBUG_INFO1, "\nIndex  Socket  ParentSocket  ParentPort  ParentIndex  Hop"));
       for (Index1 = 0; TopologyTree[Index1].Node.Valid != FALSE; Index1++) {
        if (Index1 == 0) {
          KtiDebugPrintInfo1 ((host, KTI_DEBUG_INFO1, "\n %02u     CPU%u      %s           %s          %s        %1u",
                          Index1, TopologyTree[Index1].Node.SocId, " -- ", "--", "--", TopologyTree[Index1].Node.Hop));
        } else {
          KtiDebugPrintInfo1 ((host, KTI_DEBUG_INFO1, "\n %02u     %s%u      CPU%u           %02u          %02u        %1u",
                          Index1,
                          (TopologyTree[Index1].Node.SocType == SOCKET_TYPE_CPU) ? "CPU" : "PGA",
                          TopologyTree[Index1].Node.SocId,
                          TopologyTree[TopologyTree[Index1].Node.ParentIndex].Node.SocId,
                          TopologyTree[Index1].Node.ParentPort,
                          TopologyTree[Index1].Node.ParentIndex, TopologyTree[Index1].Node.Hop));
        }
      }
    }
  }

  for (Index = 0; Index < MAX_SOCKET; Index++) {
    if ((Cpu[Index].Valid == TRUE) &&  (Cpu[Index].SocType == SOCKET_TYPE_CPU)) {
      KtiDebugPrintInfo1 ((host, KTI_DEBUG_INFO1, "\n"));
      TopologyTree = CpuTree[Index];
      RootVn = Cpu[TopologyTree[0].Node.SocId].Vn;
      for (Index1 = 1; TopologyTree[Index1].Node.Valid != FALSE; Index1++) {
        MemSetLocal ((VOID *) TempStr, '$', sizeof (TempStr));
        Index2 = 0;
        Child = TopologyTree[Index1];
        while (Child.Node.Hop != 0) {
          if (Index2 >= (MAX_SOCKET * 2)) {
            KtiDebugPrintFatal ((host, KTI_DEBUG_ERROR, "\nERR_SW_CHECK: 0x%X!!!!  Invalid value. Index2: 0x%X is greater or equal to (MAX_SOCKET * 2): 0x%X",
                                 MINOR_ERR_SW_CHECK_51, Index2, (MAX_SOCKET * 2)));
            KTI_ASSERT (FALSE, ERR_SW_CHECK, MINOR_ERR_SW_CHECK_51);
          }
          Parent = TopologyTree[Child.Node.ParentIndex];
          Port = GetChildLocalPortFromLEP (host, Cpu, &Parent, &Child);
          TempStr[Index2++] = (UINT8)((Child.Node.SocId  << 4) | Child.Node.ParentPort);
          if (SiPort == TRUE) {
            Port = TranslateToSiPort (host, Port);
            TempStr[Index2++] = (UINT8)(Parent.Node.SocId << 4) | Port;
          } else {
            TempStr[Index2++] = (UINT8)(Parent.Node.SocId << 4) | Port;
          }
          Child = Parent;
        }

        if (Index2 > (MAX_SOCKET * 2)) {
          KTI_ASSERT (FALSE, ERR_SW_CHECK, MINOR_ERR_SW_CHECK_1C);
        } else {
          KtiDebugPrintInfo1 ((host, KTI_DEBUG_INFO1, "\n"));
          for (Index2 = Index2 - 1; Index2 != 0; Index2--) {
            KtiDebugPrintInfo1 ((host, KTI_DEBUG_INFO1, "\"S%u P%u VN%u %s\" -> ", (TempStr[Index2] >> 4), (TempStr[Index2] & 0xF),
                                        RootVn, (Index2 % 2) ? "TX" : "RX"));
          }
          KtiDebugPrintInfo1 ((host, KTI_DEBUG_INFO1, "\"S%u P%u VN%u RX\";", (TempStr[Index2] >> 4), (TempStr[Index2] & 0xF), RootVn));
        } // checking Index2 value
      }
    }
  }
#endif
  return;
}

/**

  Dump topology info to output device

  @param host              - Pointer to the system host (root) structure
  @param SocketData        - socket specific data structure
  @param KtiInternalGlobal - KTIRC internal variables.

  @retval VOID

**/
VOID
RouteTableDump (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal
  )
{
#if (KTI_DEBUG_LEVEL & KTI_DEBUG_INFO1)
  UINT8               Index, Index1;
  UINT32              Data32;
  UINT8               PortFieldWidth;
  UINT8               PortFieldMask;

  PortFieldWidth = 2; // Two bit field to encode link 0, 1 & 2
  PortFieldMask  = 0x3;

  //
  // Dump the Route Table
  //
  for (Index = 0; Index < MAX_SOCKET; Index++) {
    if ((SocketData->Cpu[Index].Valid == TRUE) && (SocketData->Cpu[Index].SocType == SOCKET_TYPE_CPU)) {
      KtiDebugPrintInfo1 ((host, KTI_DEBUG_INFO1, "\n\n\nCPU %1u Routing Table", Index));
      KtiDebugPrintInfo1 ((host, KTI_DEBUG_INFO1, "\n-------------------"));
      KtiDebugPrintInfo1 ((host, KTI_DEBUG_INFO1, "\nDestSocket  Port"));
      for (Index1 = 0; Index1 < MAX_SOCKET; Index1++) {
        if (SocketData->Cpu[Index1].Valid == TRUE && SocketData->Cpu[Index1].SocType == SOCKET_TYPE_CPU && Index1 != Index) { // Exclude Self
          Data32 = PortFieldWidth * Index1;
          KtiDebugPrintInfo1 ((host, KTI_DEBUG_INFO1, "\n   CPU%1u       %1u", Index1,((SocketData->Route[Index].ChaKtiRt >> Data32) & PortFieldMask)));
        }
      }
    }
  }

  KtiDebugPrintInfo1 ((host, KTI_DEBUG_INFO1, "\n\n"));
#endif
  return ;
}

/**

  Dumps Link Exchange parameters to output device

  @param host    - Pointer to the system host (root) structure
  @param CpuData - socket specific data structure

  @retval VOID

**/
VOID
LepDump (
  struct sysHost         *host,
  KTI_CPU_SOCKET_DATA    *CpuData
  )
{
  UINT8         Index;

  UINT8         Index1;
  KTI_LINK_DATA *LinkData;

  KtiDebugPrintInfo1 ((host, KTI_DEBUG_INFO1, "\n\n\nLink Exchange Parameter"));
  KtiDebugPrintInfo1 ((host, KTI_DEBUG_INFO1, "\n-----------------------"));

  for (Index = 0; Index < MAX_SOCKET; Index++) {
    if (CpuData[Index].Valid == TRUE) {
      LinkData = CpuData[Index].LinkData;
      KtiDebugPrintInfo1 ((host, KTI_DEBUG_INFO1, "\n%s%u ", (CpuData[Index].SocType == SOCKET_TYPE_FPGA)  ? "PGA":"CPU", Index));
      for (Index1 = 0; Index1 < host->var.common.KtiPortCnt; Index1++) {
        if (LinkData[Index1].Valid == TRUE) {
          KtiDebugPrintInfo1 ((host, KTI_DEBUG_INFO1, ": LEP%u(%u:%s%u) ", Index1, LinkData[Index1].PeerPort, (LinkData[Index1].PeerSocType == SOCKET_TYPE_FPGA) ? "PGA":"CPU", LinkData[Index1].PeerSocId));
        }
      }
    }
  }

  return ;
}

/**

  Translate Si port to gen port number

  @param host   - Pointer to the system host (root) structure
  @param SiPort - SiPort number to be translated

  @retval port number

**/
UINT8
TranslateToGenPort (
  struct sysHost             *host,
  UINT8                   SiPort
  )
{

  // Currently for all supported CPUs, Si port numbering and generic port numbering is same
  return SiPort;
}

/**

  Translate generic port number to Si port

  @param host    - Pointer to the system host (root) structure
  @param GenPort - GenPort number to be translated

  @retval port number

**/
UINT8
TranslateToSiPort (
  struct sysHost             *host,
  UINT8                   GenPort
  )
{

  // Currently for all supported CPUs, Si port numbering and generic port numbering is same
  return GenPort;
}

/**

   Returns TRUE if the CPU is connected to Legacy CPU.

   @param host              - add argument and description to function comment
   @param SocketData        - add argument and description to function comment
   @param KtiInternalGlobal - KTIRC internal variables.
   @param SocId             - CPU Socket Number, 0 based

   @retval TRUE  - CPU is connected to Legacy CPU
   @retval FALSE - CPU is not connected to Legacy CPU

**/
BOOLEAN
CheckCpuConnectedToLegCpu (
  struct sysHost             *host,
  KTI_SOCKET_DATA            *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal,
  UINT8                      SocId
  )
{
  UINT8 Index;

  //
  // Check the peer of each link to see if the given CPU is directly connected to Legacy CPU
  //
  for (Index = 0; Index < host->var.common.KtiPortCnt; Index++) {
    if (SocketData->Cpu[SocId].LinkData[Index].Valid == TRUE &&
        SocketData->Cpu[SocId].LinkData[Index].PeerSocType == SOCKET_TYPE_CPU &&
        SocketData->Cpu[SocId].LinkData[Index].PeerSocId == KtiInternalGlobal->SbspSoc) {
      return TRUE;
    }
  }

  return FALSE;
}

/**

   Checks if dual-link or triple-link exists between parent & child from their LEP.

  @param CpuData - Contains the LEP of Parent & Child nodes
  @param Parent  - Socket at one end of the link
  @param Child   - Socket at the other end of the link

  @retval TRUE   - Dual-link exists
  @retval FALSE  - Dual-link doesn't exist

**/
BOOLEAN
DuplicateOrTripleLinkExists (
  struct sysHost          *host,
  KTI_CPU_SOCKET_DATA     *CpuData,
  TOPOLOGY_TREE_NODE      *Parent,
  TOPOLOGY_TREE_NODE      *Child
  )
{
  UINT8         KtiLink;
  KTI_LINK_DATA TempLep;
  UINT8         Count;

  KTI_ASSERT ((CpuData != NULL), ERR_SW_CHECK, MINOR_ERR_SW_CHECK_1D);
  KTI_ASSERT ((Parent != NULL), ERR_SW_CHECK, MINOR_ERR_SW_CHECK_1E);
  KTI_ASSERT ((Child != NULL), ERR_SW_CHECK, MINOR_ERR_SW_CHECK_1F);

  Count = 0;

  if (Parent->Node.SocType == SOCKET_TYPE_CPU) {
    for (KtiLink = 0; KtiLink < host->var.common.KtiPortCnt; KtiLink++) {
      TempLep = CpuData[Parent->Node.SocId].LinkData[KtiLink];
      if (TempLep.Valid == TRUE && TempLep.PeerSocId == Child->Node.SocId && TempLep.PeerSocType == Child->Node.SocType) {
        Count++;
      }
    }
  }

  if (Count > 1) {
    return TRUE;
  }

  return FALSE;
}

/**

  This routine checks if the given non-2S topology has more than link connecting any 2 given sockets. If
  found, it retains only one and marks the other link(s) as invalid.This routine should not be called if the
  given system is 2S.

  @param host       - Input/Output structure of the KTIRC
  @param SocketData - Socket specific data structure

  @retval 0 - Successful

**/
KTI_STATUS
RemoveDualLink (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData
  )
{
  UINT8   Idx1, Idx2, Idx3;
  UINT32  PeerSocId;
  UINT32  PeerPort;

  for (Idx1 = 0; Idx1 < MAX_SOCKET; Idx1++) {
    if (SocketData->Cpu[Idx1].Valid == TRUE) {
      for (Idx2 = 0; Idx2 < host->var.common.KtiPortCnt; Idx2++) {
        if (SocketData->Cpu[Idx1].LinkData[Idx2].Valid == TRUE) {
          for (Idx3 = 0; Idx3 < host->var.common.KtiPortCnt; Idx3++) {
            if ((Idx3 != Idx2) && (SocketData->Cpu[Idx1].LinkData[Idx3].Valid == TRUE)) {
              //
              // Check if this is a dual link and mark it as invalid
              //
              if ((SocketData->Cpu[Idx1].LinkData[Idx2].PeerSocId == SocketData->Cpu[Idx1].LinkData[Idx3].PeerSocId) &&
                  (SocketData->Cpu[Idx1].LinkData[Idx2].PeerSocType == SocketData->Cpu[Idx1].LinkData[Idx3].PeerSocType)) {
                PeerSocId    = SocketData->Cpu[Idx1].LinkData[Idx3].PeerSocId;
                PeerPort     = SocketData->Cpu[Idx1].LinkData[Idx3].PeerPort;
                KtiLogWarning(host, Idx1, SOCKET_TYPE_CPU, Idx3, WARN_INVALID_LINK);
                KtiDebugPrintWarn ((host, KTI_DEBUG_WARN, "\n  CPU%u has more than one link (%u, %u) connecting to CPU%u (%u, %u).   Disabling link CPU%uP%u <-> CPU%uP%u. ",
                                Idx1, Idx2, Idx3,                                                    //Soc # and local dual-link ports
                                PeerSocId, SocketData->Cpu[Idx1].LinkData[Idx2].PeerPort, PeerPort,  //Peer Soc # and Peer dual-link ports
                                Idx1, Idx3, PeerSocId, PeerPort));                                   //Link to be disabled:  Soc # and port (both ends)
                SocketData->Cpu[Idx1].LinkData[Idx3].Valid = 0;
                SocketData->Cpu[PeerSocId].LinkData[PeerPort].Valid = 0;
              }
            }
          }
        }
      }
    }
  }

  return KTI_SUCCESS;
}


/**

  Returns the Kti port numbers of the dual or triple link of Node1 socket.
  @param host    - Input/Output structure of the KTIRC
  @param CpuData - Contains the LEP of Node1 & Node2 nodes
  @param Node1   - Socket at one end of the link
  @param Node2   - Socket at the other end of the link
  @param Port1   - Port1 of the dual or triple link
  @param Port2   - Port2 of the dual or triple link
  @param Port3   - Port3 of the dual or triple link

  @retval Port1 - Port number of 1st link
  @retval Port2 - Port number of 2nd link
  @retval Port3 - Port number of 3rd link

**/
KTI_STATUS
GetDualLinkOrTripleLinkFromLEP (
  struct sysHost          *host,
  KTI_CPU_SOCKET_DATA     *CpuData,
  TOPOLOGY_TREE_NODE      *Node1,
  TOPOLOGY_TREE_NODE      *Node2,
  UINT8                   *Port1,
  UINT8                   *Port2,
  UINT8                   *Port3
  )
{
  UINT8         KtiLink;
  KTI_LINK_DATA TempLep;

  KTI_ASSERT ((CpuData != NULL), ERR_SW_CHECK, MINOR_ERR_SW_CHECK_20);
  KTI_ASSERT ((Node1 != NULL), ERR_SW_CHECK, MINOR_ERR_SW_CHECK_21);
  KTI_ASSERT ((Node2 != NULL), ERR_SW_CHECK, MINOR_ERR_SW_CHECK_22);
  KTI_ASSERT ((Port1 != NULL), ERR_SW_CHECK, MINOR_ERR_SW_CHECK_23);
  KTI_ASSERT ((Port2 != NULL), ERR_SW_CHECK, MINOR_ERR_SW_CHECK_24);
  KTI_ASSERT ((Port3 != NULL), ERR_SW_CHECK, MINOR_ERR_SW_CHECK_25);

  *Port1  = 0xFF;
  *Port2  = 0xFF;
  *Port3  = 0xFF;

  //
  // Only CPU sockets can have a dual-link
  //
  if (Node1->Node.SocType == SOCKET_TYPE_CPU) {
    for (KtiLink = 0; KtiLink < host->var.common.KtiPortCnt; KtiLink++) {
      TempLep = CpuData[Node1->Node.SocId].LinkData[KtiLink];
      if (TempLep.Valid == TRUE && TempLep.PeerSocId == Node2->Node.SocId && TempLep.PeerSocType == Node2->Node.SocType) {
        if (*Port1 == 0xFF) {
          *Port1 = KtiLink;
        } else if (*Port2 == 0xFF) {
          *Port2 = KtiLink;
        } else if (*Port3 == 0xFF) {
          *Port3 = KtiLink;
        }else {
          //
          // parent has more than 3 links to child
          //
          KtiDebugPrintFatal ((host, KTI_DEBUG_ERROR, "\n CPU %u has more than three links to CPU %u. Topology not supported ", Node1->Node.SocId, Node2->Node.SocId));
          KTI_ASSERT(FALSE, ERR_UNSUPPORTED_TOPOLOGY, MINOR_ERR_INVALID_TOPOLOGY);
        }
      }
    }
  }

  return KTI_SUCCESS;
}

/**

  Returns port number of the Parent socket that connects it to Child socket.

  @param CpuData - Contains the LEP of Parent & Child nodes
  @param Parent  - Socket at one end of the link
  @param Child   - Socket at the other end of the link

  @retval Port of the Parent socket that connects it to the child socket
  @retval 0xFF - Child is not an immediate neighbor of Parent

**/
UINT8
GetChildLocalPortFromLEP (
  struct sysHost             *host,
  KTI_CPU_SOCKET_DATA    *CpuData,
  TOPOLOGY_TREE_NODE      *Parent,
  TOPOLOGY_TREE_NODE      *Child
  )
{
  UINT8         KtiLink;
  KTI_LINK_DATA TempLep;

  KTI_ASSERT ((CpuData != NULL), ERR_SW_CHECK, MINOR_ERR_SW_CHECK_26);
  KTI_ASSERT ((Parent != NULL), ERR_SW_CHECK, MINOR_ERR_SW_CHECK_27);
  KTI_ASSERT ((Child != NULL), ERR_SW_CHECK, MINOR_ERR_SW_CHECK_28);

  if (Parent->Node.SocType == SOCKET_TYPE_CPU) {
    for (KtiLink = 0; KtiLink < host->var.common.KtiPortCnt; KtiLink++) {
      TempLep = CpuData[Parent->Node.SocId].LinkData[KtiLink];
      if (TempLep.Valid != TRUE || TempLep.DualLink == TRUE || TempLep.TwoSkt3Link == TRUE) { // Skip the Invalid/Duplicate Links
        continue;
      }

      if (TempLep.PeerSocId == Child->Node.SocId && TempLep.PeerSocType == Child->Node.SocType) {
        return KtiLink;
      }
    }
  }

  return 0xFF;
}

/**

   Returns port number of the Child socket that connects it to the Parent socket.

  @param CpuData - Contains the LEP of Parent & Child nodes
  @param Parent  - Socket at one end of the link
  @param Child   - Socket at the other end of the link

  @retval Port of the Child socket that connects it to the Parent socket
  @retval 0xFF - Child is not an immediate neighbor of Parent

**/
UINT8
GetChildPeerPortFromLEP (
  struct sysHost             *host,
  KTI_CPU_SOCKET_DATA    *CpuData,
  TOPOLOGY_TREE_NODE      *Parent,
  TOPOLOGY_TREE_NODE      *Child
  )
{
  UINT8         KtiLink;
  KTI_LINK_DATA TempLep;

  KTI_ASSERT ((CpuData != NULL), ERR_SW_CHECK, MINOR_ERR_SW_CHECK_29);
  KTI_ASSERT ((Parent != NULL), ERR_SW_CHECK, MINOR_ERR_SW_CHECK_2A);
  KTI_ASSERT ((Child != NULL), ERR_SW_CHECK, MINOR_ERR_SW_CHECK_2B);

  if (Parent->Node.SocType == SOCKET_TYPE_CPU) {
    for (KtiLink = 0; KtiLink < host->var.common.KtiPortCnt; KtiLink++) {
      TempLep = CpuData[Parent->Node.SocId].LinkData[KtiLink];
      if (TempLep.Valid != TRUE || TempLep.DualLink == TRUE || TempLep.TwoSkt3Link == TRUE) { // Skip the Invalid/Duplicate Links/3 Links
        continue;
      }

      if (TempLep.PeerSocId == Child->Node.SocId && TempLep.PeerSocType == Child->Node.SocType) {
        return (UINT8) TempLep.PeerPort;
      }
    }
  }

  return 0xFF;
}


/**

   Returns immediate Parent of Child socket in Topology Tree.

  @param TopologyTree - Topology Tree
  @param Child        - Child Socket
  @param Parent       - Parent Socket

  @retval 0 - Successful, parent found
  @retval 1 - Failure

**/
KTI_STATUS
GetParentSocket (
  struct sysHost             *host,
  TOPOLOGY_TREE_NODE      *TopologyTree,
  TOPOLOGY_TREE_NODE      *Child,
  TOPOLOGY_TREE_NODE      *Parent
  )
{

  KTI_ASSERT ((TopologyTree != NULL), ERR_SW_CHECK, MINOR_ERR_SW_CHECK_2C);
  KTI_ASSERT ((Parent != NULL), ERR_SW_CHECK, MINOR_ERR_SW_CHECK_2D);
  KTI_ASSERT ((Child != NULL), ERR_SW_CHECK, MINOR_ERR_SW_CHECK_2E);

  Parent->RawNode = TopologyTree[Child->Node.ParentIndex].RawNode;
  if (Parent->Node.Valid != TRUE) {
    KtiDebugPrintFatal ((host, KTI_DEBUG_ERROR, "\n\n Parent Node not found."));
    KTI_ASSERT (FALSE, ERR_SW_CHECK, MINOR_ERR_SW_CHECK_2F);
  }

  return KTI_SUCCESS;

}

/**

  finds the child's parent in tree

  @param CpuData - socket specific data structure
  @param Tree    - system KTI fabric node tree
  @param Child   - current node tree

  @retval ParentIndex - Success
  @retval 0xFF - Failure

**/
UINT8
GetParentIndex (
  struct sysHost             *host,
  KTI_CPU_SOCKET_DATA    *CpuData,
  TOPOLOGY_TREE_NODE   *Tree,
  TOPOLOGY_TREE_NODE   *Child
  )
{
  UINT8         Index;
  KTI_LINK_DATA Lep;

  KTI_ASSERT ((Tree != NULL), ERR_SW_CHECK, MINOR_ERR_SW_CHECK_30);
  KTI_ASSERT ((Child != NULL), ERR_SW_CHECK, MINOR_ERR_SW_CHECK_31);
  KTI_ASSERT ((Child->Node.Valid == TRUE), ERR_SW_CHECK, MINOR_ERR_SW_CHECK_32);

  if (Child->Node.SocType == SOCKET_TYPE_CPU) {
    Lep = CpuData[Child->Node.SocId].LinkData[Child->Node.ParentPort];

    for (Index = 0; Index < MAX_TREE_NODES; Index++) {
      if (Tree[Index].Node.Valid == TRUE &&
          Tree[Index].Node.SocId == Lep.PeerSocId &&
          Tree[Index].Node.SocType == Lep.PeerSocType
          ) {
        return Index;
      }
    }
  } else {
    KTI_ASSERT (FALSE, ERR_SW_CHECK, MINOR_ERR_SW_CHECK_33);
  }

  return 0xFF;
}

/**

  counts hops to child from current socket

  @param Tree    - system KTI fabric node tree
  @param SocId   - processor socket id
  @param SocType - device type in socket

  @retval 0 - Successful
  @retval 1 - Failure

**/
UINT8
GetChildHopLength (
  struct sysHost             *host,
  TOPOLOGY_TREE_NODE   *Tree,
  UINT8                SocId,
  UINT8                SocType
  )
{
  UINT8 Index;

  KTI_ASSERT  ((Tree != NULL), ERR_SW_CHECK, MINOR_ERR_SW_CHECK_34);

  for (Index = 0; Index < MAX_TREE_NODES; Index++) {
    if (Tree[Index].Node.Valid == TRUE && Tree[Index].Node.SocId == SocId && Tree[Index].Node.SocType == SocType) {
      return (UINT8) Tree[Index].Node.Hop;
    }
  }

  return 0xFF;
}

/**

  Find a KTI node within overall KTI fabric

  @param Tree     - KTI fabric tree structure
  @param LeafNode - KTI fabric tree structure

  @retval 0 - Successful
  @retval 1 - Failure

**/
BOOLEAN
NodeFoundInTopologyTree (
  struct sysHost             *host,
  TOPOLOGY_TREE_NODE    *Tree,
  TOPOLOGY_TREE_NODE    *SearchNode
  )
{
  UINT8 Index;

  KTI_ASSERT ((Tree != NULL), ERR_SW_CHECK, MINOR_ERR_SW_CHECK_35);
  KTI_ASSERT ((SearchNode != NULL), ERR_SW_CHECK, MINOR_ERR_SW_CHECK_36);

  for (Index = 0; Index < MAX_TREE_NODES; Index++) {
    //
    // If there are no more valid nodes, then the node in question is not in the tree
    //
    if (Tree[Index].Node.Valid == FALSE) {
      return FALSE;
    }
    //
    // If socket ID and Type matches then we found the node in the tree
    //
    if (Tree[Index].Node.SocId == SearchNode->Node.SocId &&
        Tree[Index].Node.SocType == SearchNode->Node.SocType &&
        Tree[Index].Node.Hop == SearchNode->Node.Hop &&
        Tree[Index].Node.ParentPort == SearchNode->Node.ParentPort
        ) {
      return TRUE;
    }
  }

  return FALSE;
}

/**

   Searches the Topology tree to see if the node under search is already found in the given SBSP min path tree.

  @param host       - Pointer to the system host (root) structure
  @param Tree       - Topology Tree
  @param SearchNode -  Node to search in the topology tree

  @retval TRUE   - Node already exists in tree
  @retval FALSE  - Node doesn't exist in tree

**/
BOOLEAN
NodeFoundInMinPathTree (
  struct sysHost          *host,
  TOPOLOGY_TREE_NODE      *Tree,
  TOPOLOGY_TREE_NODE      *SearchNode
  )
{
  UINT8 Index;

  KTI_ASSERT ((Tree != NULL), ERR_SW_CHECK, MINOR_ERR_SW_CHECK_37);
  KTI_ASSERT ((SearchNode != NULL), ERR_SW_CHECK, MINOR_ERR_SW_CHECK_38);

  for (Index = 0; Index < MAX_TREE_NODES; Index++) {
    //
    // If there are no more valid nodes, then the node in question is not in the tree
    //
    if (Tree[Index].Node.Valid == FALSE) {
      return FALSE;
    }
    //
    // If socket ID & Type matches we found the node
    //
    if (Tree[Index].Node.SocId == SearchNode->Node.SocId && Tree[Index].Node.SocType == SearchNode->Node.SocType) {
      return TRUE;
    }
  }

  return FALSE;
}

/**

  Adds the current node to the tree

  @param Tree  - system KTI fabric node tree
  @param Node  - current node tree

  @retval 0 - Successful
  @retval 1 - Failure

**/
KTI_STATUS
AddNodeToTopologyTree (
  struct sysHost             *host,
  TOPOLOGY_TREE_NODE      *Tree,
  TOPOLOGY_TREE_NODE      *Node
  )
{
  UINT8 Index;

  KTI_ASSERT ((Tree != NULL), ERR_SW_CHECK, MINOR_ERR_SW_CHECK_39);
  KTI_ASSERT ((Node != NULL), ERR_SW_CHECK, MINOR_ERR_SW_CHECK_3A);

  for (Index = 0; Index < MAX_TREE_NODES; Index++) {
    //
    // Place the tree in first available empty slot in the array
    //
    if (Tree[Index].Node.Valid == FALSE) {
      Tree[Index] = *Node;
      return KTI_SUCCESS;
    }
  }

  return (KTI_STATUS) KTI_FAILURE;
}

/**

  Adds the current node to the tree

  @param Tree  - system KTI fabric node tree
  @param Node  - current node tree

  @retval 0 - Successful
  @retval 1 - Failure

**/
KTI_STATUS
AddNodeToRingTree (
  struct sysHost          *host,
  TOPOLOGY_TREE_NODE      *Tree,
  TOPOLOGY_TREE_NODE      *Node
  )
{
  UINT8 Index;

  KTI_ASSERT ((Tree != NULL), ERR_SW_CHECK, MINOR_ERR_SW_CHECK_3B);
  KTI_ASSERT ((Node != NULL), ERR_SW_CHECK, MINOR_ERR_SW_CHECK_3C);

  for (Index = 0; Index < MAX_RING_TREE_NODES; Index++) {
    //
    // Place the tree in first available empty slot in the array
    //
    if (Tree[Index].Node.Valid == FALSE) {
      Tree[Index] = *Node;
      return KTI_SUCCESS;
    }
  }

  return (KTI_STATUS) KTI_FAILURE;
}


/**

  Counts hops in path

  @param Tree  - system KTI fabric node tree
  @param Node  - current node tree

  @retval 0 - Successful
  @retval 1 - Failure

**/
UINT8
CountImmediateChildren (
  struct sysHost             *host,
  TOPOLOGY_TREE_NODE      *Tree,
  TOPOLOGY_TREE_NODE      *Parent
  )
{
  UINT8 Idx;
  UINT8 ParentIdx;
  UINT8 Children;

  KTI_ASSERT ((Tree != NULL), ERR_SW_CHECK, MINOR_ERR_SW_CHECK_3D);
  KTI_ASSERT ((Parent != NULL), ERR_SW_CHECK, MINOR_ERR_SW_CHECK_3E);

  ParentIdx = 0xFF;
  Children  = 0;

  for (Idx = 0; Idx < MAX_TREE_NODES; Idx++) {
    if (Tree[Idx].Node.Valid == FALSE) {
      return Children;
    }

    if (Tree[Idx].Node.SocId == Parent->Node.SocId &&
        Tree[Idx].Node.SocType == Parent->Node.SocType &&
        Tree[Idx].Node.Hop == Parent->Node.Hop &&
        Tree[Idx].Node.ParentPort == Parent->Node.ParentPort
        ) {
      ParentIdx = Idx;
      break;
    }
  }

  if (ParentIdx != 0xFF) {
    for (Idx = 0; Idx < MAX_TREE_NODES; Idx++) {
      if (Tree[Idx].Node.Valid == FALSE) {
        return Children;
      }

      if (Tree[Idx].Node.ParentIndex == ParentIdx) {
        Children++;
      }
    }
  }

  return Children;
}

/**

   Searches the Topology tree to see if the node under search is already found in the
   tree at lower Hop levels. In effect, it checks if there is a shorter path exists
   from the root node to this node.

  @param Tree       - Topology Tree
  @param SearchNode - Node to search in the topology tree

  @retval TRUE   - Shorter path exists
  @retval FALSE  - Shorter path doesn't exist

**/
BOOLEAN
ShorterPathExists (
  struct sysHost             *host,
  TOPOLOGY_TREE_NODE      *Tree,
  TOPOLOGY_TREE_NODE      *SearchNode
  )
{
  UINT8 Index;

  KTI_ASSERT ((Tree != NULL), ERR_SW_CHECK, MINOR_ERR_SW_CHECK_3F);
  KTI_ASSERT ((SearchNode != NULL), ERR_SW_CHECK, MINOR_ERR_SW_CHECK_40);

  for (Index = 0; Index < MAX_TREE_NODES; Index++) {
    //
    // If there are no more valid nodes, then the node in question is not in the tree
    //
    if (Tree[Index].Node.Valid == FALSE) {
      return FALSE;
    }
    //
    // If socket ID & Type matches and the hop level of the matching socket is less than the hop level of the node under search,
    // we found a shorter path
    //
    if (Tree[Index].Node.SocId == SearchNode->Node.SocId &&
        Tree[Index].Node.SocType == SearchNode->Node.SocType &&
        Tree[Index].Node.Hop < SearchNode->Node.Hop
        ) {
      return TRUE;
    }

  }

  return FALSE;
}

/**

  Compares to Ring arrays.

  @param host  - pointer to system host (root) data
  @param Ring1 - first ring to compare
  @param Ring2 - second ring to compare

  @retval TRUE - Rings are same
  @retval FALSE - Rings are different

**/
BOOLEAN
CompareRing (
  struct sysHost       *host,
  UINT8                *Ring1,
  UINT8                *Ring2
  )
{
  UINT8 Idx1, Idx2;
  UINT8 Match = 0;

  KTI_ASSERT ((Ring1 != NULL), ERR_SW_CHECK, MINOR_ERR_SW_CHECK_41);
  KTI_ASSERT ((Ring2 != NULL), ERR_SW_CHECK, MINOR_ERR_SW_CHECK_42);

  for (Idx1 = 0; Idx1 < 4; Idx1++) {
    for (Idx2 = 0; Idx2 < 4; Idx2++) {
      if (Ring1[Idx1] == Ring2[Idx2]) {
        Match = Match | (1 << Idx1);
        break;
      }
    }
  }

  if (Match == 0xF) {
    return TRUE;
  } else {
    return FALSE;
  }
}


/**

  Routine to display Error/Status code; it also updates a scratch register with additional
  info such as socket, link related to the Error/Status.

  @param SocId     - Bit mask, where bit x corresponds to CPU (CPU:0...3). Set to 0xFF if the Error/Status is System Wide.
  @param SocType   - 0: CPU socket. Set to 0xFF if the Error/Status is System Wide.
  @param KtiLink   - Bit mask, where bit x corresponds to CPU KTI port 0/1. Set to 0xFF if the Error/Status is system wide, 0 if Link Not Applicable.
  @param MajorCode - Major Status or Error code
  @param MinorCode - Minor Status or Error code
  @param host      - add argument and description to function comment

  @retval None.

**/
VOID
KtiCheckPoint (
  UINT8                      SocId,
  UINT8                      SocType,
  UINT8                      KtiLink,
  UINT8                      MajorCode,
  UINT8                      MinorCode,
  struct sysHost             *host
  )
{
  UINT16  data;
  // APTIOV_SERVER_OVERRIDE_RC_START : Added OEM hook support for KtiCheckPoint
  AmiKtiCheckPointWrapper(SocId, SocType, KtiLink, MajorCode, MinorCode, host);
  // APTIOV_SERVER_OVERRIDE_RC_END : Added OEM hook support for KtiCheckPoint

  //
  // Update the scratch register.
  //
  // Register Usage:
  // [31:24] - Error/Status Major Code
  // [23:16] - Error/Status Minor Code
  // [15:13] - Reserved for future use
  // [12:5] - Bitmask of CPU Sockets affected, 0xF - System Wide Error/Status
  // [4:3] - Socket Type. 0: CPU Socket, 0x3 System Wide Error/Status
  // [2:0] - Bit mask of KTI Links affected, 0 - Link Not Applicable, 0x3 - System Wide Error/Status

  data = ((SocId & 0xFF) << 5) | ((SocType & 0x3) << 3) | ((KtiLink & 0x7) << 0);
  OutputCheckpoint (host, MajorCode, MinorCode, data);

  return ;
}


/**

  Routine to call platformfatal and assert


  @param MajorCode - Major Status or Error code
  @param MinorCode - Minor Status or Error code
  @param host      - add argument and description to function comment

  @retval Does not return

**/
VOID
KtiAssert (
  struct sysHost             *host,
  UINT8                      majorCode,
  UINT8                      minorCode
  )

{
  UINT8           haltOnError;

  PlatformFatalError (host, majorCode, minorCode, &haltOnError);
  //
  //haltOnError is used by OEM to indicate if wants to halt or not.  This is useful in MRC, but not Kti.  Kti will ignore and always halt
  //
  haltOnError = 1;
  while (*(volatile UINT8 *) &haltOnError);

  return;
}

/**

  Routine to Log Warning in KTIRC output structure.

  These warnings will only be added to the Enhanced Warning Log, if it is defined at build time

  @param host     - To be filled with the warning log
  @param SocId    - Bit mask, where bit x corresponds to CPU (CPU:0...3). Set to 0xFF if the Warning is system wide.
  @param SocType  - 0: CPU socket. Set to 0xFF if the Warning is system wide.
  @param KtiLink  - Bit mask, where bit x corresponds to CPU KTI port 0/1. Set to 0xFF if the Warning is system wide, 0 if Link Not Applicable.
  @param WarnCode - Warning Code

  @retval None.

**/
VOID
KtiLogWarning (
  struct sysHost             *host,
  UINT8                      SocId,
  UINT8                      SocType,
  UINT8                      KtiLink,
  UINT8                      WarnCode
  )
{
#if ENHANCED_WARNING_LOG
  EWL_ENTRY_TYPE6 type6;

  type6.Header.Size     = sizeof(EWL_ENTRY_TYPE6);
  type6.Header.Type     = EwlType6;
  type6.Header.Severity = EwlSeverityWarning;

  type6.Context.MajorCheckpoint  = ((host->var.common.checkpoint) >> 24) & 0xFF;
  type6.Context.MinorCheckpoint  = ((host->var.common.checkpoint) >> 16) & 0xFF;
  type6.Context.MajorWarningCode = WarnCode;
  type6.Context.MinorWarningCode = 0xFF;//No minor code passed to KtiLogWarning

  type6.SocketMask = SocId;
  type6.SocketType = SocType;
  type6.Port       = KtiLink;

  //
  // Output warning details to the Serial Log
  //
  KtiDebugPrintWarn ((host, KTI_DEBUG_WARN, "\n\n**** KTI Warning ****"));
  KtiDebugPrintWarn ((host, KTI_DEBUG_WARN, "\nWarning code:         0x%02x", WarnCode));
  KtiDebugPrintWarn ((host, KTI_DEBUG_WARN, "\nMajor Checkpoint:     0x%02x", type6.Context.MajorCheckpoint));
  KtiDebugPrintWarn ((host, KTI_DEBUG_WARN, "\nMinor Checkpoint:     0x%02x", type6.Context.MinorCheckpoint));
  KtiDebugPrintWarn ((host, KTI_DEBUG_WARN, "\nAffected Socket mask: 0x%02x", SocId));
  KtiDebugPrintWarn ((host, KTI_DEBUG_WARN, "\nSocketType:           0x%02x", SocType));
  KtiDebugPrintWarn ((host, KTI_DEBUG_WARN, "\nAffected port mask:   0x%02x\n\n", KtiLink));

  //
  //log the warning in the Enhanced Warning Log structure
  //
  PlatformEwlLogEntry(host, &type6.Header);

#else
  UINT32  Ctr;
  // APTIOV_SERVER_OVERRIDE_RC_START : To provide OEM hook for logging KTI warnings.
  AmiKtiLogWarningWrapper(host,SocId,SocType,KtiLink,WarnCode);
  // APTIOV_SERVER_OVERRIDE_RC_END : To provide OEM hook for logging KTI warnings.
  //
  // Log the Warning information in the KTI output structure
  // [31:20] - Reserved, set to 0
  // [20:13] - Bitmask of CPU Sockets affected, 0xF - System wide Warning
  // [12:11] - Socket Type. 0 - CPU Socket, 0x3 - System Wide Warning
  // [10:08] - Bit mask of KTI Links affected. 0x3 - System Wide Warning, 0 - Link Not Applicable
  // [07:00] - Warning Code
  //
  // Identify unused log entry; last entry is used to indicate that the log is overflowing
  //
  for (Ctr = 0; Ctr < MAX_WARNING_LOGS - 1; Ctr++) {
    if (host->var.kti.WarningLog[Ctr] == 0) {
      break;
    }
  }

  if (Ctr == (MAX_WARNING_LOGS - 1)) {
    KtiDebugPrintWarn ((host, KTI_DEBUG_WARN, "\n\n **** Warning Log Overflowed ****\n"));
    host->var.kti.WarningLog[Ctr] = (UINT32) WARNING_LOG_OVERFLOW;
    return ;
  }

  host->var.kti.WarningLog[Ctr] = ((SocId & 0xFF) << 13) | ((SocType & 0x3) << 11) | ((KtiLink & 0x7) << 8) | WarnCode;
#endif

  return ;
}

/**

  Routine to log topology change in EWL output structure.
  These warnings will only be added to the Enhanced Warning Log, if it is defined at build time

  @param host           - To be filled with the warning log
  @param TopologyBefore - Bit mask, where bit x corresponds to CPU (CPU:0...3). Set to 0xFF if the Warning is system wide.
  @param TopologyAfter  - 0: CPU socket. Set to 0xFF if the Warning is system wide.
  @param Reason         - Bit mask, where bit x corresponds to CPU KTI port 0/1. Set to 0xFF if the Warning is system wide, 0 if Link Not Applicable.

  @retval None.

**/VOID
KtiLogTopologyChange(
  PSYSHOST                host,
  UINT32                  TopologyBefore,
  UINT32                  TopologyAfter,
  TOPOLOGY_DEGRADE_REASON Reason
)
{
#if ENHANCED_WARNING_LOG
  EWL_ENTRY_TYPE21 type21;

  type21.Header.Size     = sizeof(EWL_ENTRY_TYPE21);
  type21.Header.Type     = EwlType21;
  type21.Header.Severity = EwlSeverityWarning;

  type21.DegradedFrom = TopologyBefore;
  type21.NewTopology  = TopologyAfter;
  type21.Reason       = Reason;

  //
  // Output warning details to the Serial Log
  //
  KtiDebugPrintWarn ((host, KTI_DEBUG_WARN, "\n\n**** KTI Topology Change Logged ****"));
  KtiDebugPrintWarn ((host, KTI_DEBUG_WARN, "\nTopology before degrade: 0x%08x", type21.DegradedFrom));
  KtiDebugPrintWarn ((host, KTI_DEBUG_WARN, "\nTopology after degrade:  0x%08x", type21.NewTopology));
  KtiDebugPrintWarn ((host, KTI_DEBUG_WARN, "\nDegrade reason code   :  0x%x\n",   type21.Reason));


  //
  //log the warning in the Enhanced Warning Log structure
  //
  PlatformEwlLogEntry(host, &type21.Header);

#endif
}

/**

    KTI specific read PCI Configuration

  @param Host      - Pointer to the system host (root) structure
  @param SocId     - CPU Socket Node number (Socket ID)
  @param BoxInst   - Box Instance, 0 based
  @param RegOffset - Register offset; values come from the auto generated header file

  @retval register value read

**/
UINT32
KtiReadPciCfg(
  struct sysHost        *host,
  UINT8                 SocId,
  UINT8                 BoxInst,
  UINT32                RegOffset
)
{
#ifndef RAS_SUPPORT
  return ReadCpuPciCfgEx (host, SocId, BoxInst, RegOffset);
#else
  return ReadCpuPciCfgRas(host, SocId, BoxInst, RegOffset);
#endif
}

/**

    KTI specific write PCI Configuration

  @param Host      - Pointer to the system host (root) structure
  @param SocId     - CPU Socket Node number (Socket ID)
  @param BoxInst   - Box Instance, 0 based
  @param RegOffset - Register offset; values come from the auto generated header file
  @param data      - Data to write


  @retval register value read

**/
VOID
KtiWritePciCfg(
  struct sysHost        *host,
  UINT8                 SocId,
  UINT8                 BoxInst,
  UINT32                RegOffset,
  UINT32                data
  )
{
#ifndef RAS_SUPPORT
  WriteCpuPciCfgEx (host, SocId, BoxInst, RegOffset, data);
#else
  WriteCpuPciCfgRas(host, SocId, BoxInst, RegOffset, data);
#endif

}

/**

  Read IAR register.

  @param Host      - Pointer to the system host (root) structure
  @param SocId     - CPU Socket Node number (Socket ID)
  @param BoxInst   - Box Instance, 0 based
  @param UnitId    - Unit ID (lane 0-19=0-19, fwdc=20, reserved=21-62, PQD=62, Broadcast Data Lane=63)
                     Broadcast Data Lane UnitId should NOT be used for reads.
  @param RegOffset - Register offset; values come from the auto generated header file
  @param data      - Data to write


  @retval register value read

**/
UINT8
KtiReadIar(
  struct sysHost        *host,
  UINT8                 SocId,
  UINT8                 BoxInst,
  UINT8                 UnitId,
  UINT8                 RegOffset
  )
{
  PQ_CSR_AFEIAR_KTI_CIOPHYDFX_STRUCT AfeIar;
  UINT8   Data8;
  UINT32  WaitTime, Ctr;

  //Wait for "write_en" bit to be cleared before reading KTI IAR register
  Ctr      = 0;
  WaitTime = 5000;
  AfeIar.Data = 0;
  if (!(host->var.common.emulation & SIMICS_FLAG)) {
    for (Ctr = 0; Ctr < WaitTime; Ctr++) {
      AfeIar.Data = KtiReadPciCfg (host, SocId, BoxInst, PQ_CSR_AFEIAR_KTI_CIOPHYDFX_REG);
      if (AfeIar.Bits.write_en == 0) {
        break;
      }
      //
      // Delay 10 microsecs
      //
      KtiFixedDelay(host, 10);
    }
  }
  // Halt the system if timeout
  if (Ctr == WaitTime) {
    KtiDebugPrintFatal((host, KTI_DEBUG_ERROR, "\nERR_IAR_READ for Socket %d, BoxInst %d, Offset 0x%x!!!! \nPQ_CSR_AFEIAR_KTI_CIOPHYDFX_REG value 0x%08x \nCheck PQ_CSR_AFEIAR_KTI_CIOPHYDFX_STRUCT.write_en is TIMEOUT\n", SocId, BoxInst, RegOffset, AfeIar.Data));
    KTI_ASSERT (FALSE, ERR_FULL_SPEED_TRANSITION, MINOR_ERR_S0_TIMEOUT_IAR | SocId);
  }
  
  //Once write-en bit is cleared, free to write hw_override_en bit and then read the IAR
  AfeIar.Data = 0;
  AfeIar.Bits.register_id = RegOffset & 0x3F;
  AfeIar.Bits.unit_id = UnitId & 0x3F;
  AfeIar.Bits.write_en = 0;
  AfeIar.Bits.hw_override_en = 1;
  KtiWritePciCfg (host, SocId, BoxInst, PQ_CSR_AFEIAR_KTI_CIOPHYDFX_REG, AfeIar.Data);
  AfeIar.Data = KtiReadPciCfg (host, SocId, BoxInst, PQ_CSR_AFEIAR_KTI_CIOPHYDFX_REG);
  Data8 = (UINT8) AfeIar.Bits.read_data;

  return Data8;
}

/**

  Write to IAR register.

  @param Host      - Pointer to the system host (root) structure
  @param SocId     - CPU Socket Node number (Socket ID)
  @param BoxInst   - Box Instance, 0 based
  @param UnitId    - Unit ID (lane 0-19=0-19, fwdc=20, reserved=21-62, PQD=62, Broadcast Data Lane=63)
  @param RegOffset - Register offset; values come from the auto generated header file
  @param data      - Data to write

**/
VOID
KtiWriteIar(
  struct sysHost        *host,
  UINT8                 SocId,
  UINT8                 BoxInst,
  UINT8                 UnitId,
  UINT8                 RegOffset,
  UINT8                 data
  )
{
  PQ_CSR_AFEIAR_KTI_CIOPHYDFX_STRUCT AfeIar;
  UINT32                             WaitTime, Ctr;
  Ctr      = 0;
  WaitTime = 5000;

  AfeIar.Data = 0;
  AfeIar.Bits.write_en = 1;
  if (!(host->var.common.emulation & SIMICS_FLAG)) {
    for (Ctr = 0; Ctr < WaitTime; Ctr++) {
      AfeIar.Data = KtiReadPciCfg (host, SocId, BoxInst, PQ_CSR_AFEIAR_KTI_CIOPHYDFX_REG);
      if (AfeIar.Bits.write_en == 0) {
        break;
      }
      //
      // Delay 10 microsecs
      //
      KtiFixedDelay(host, 10);
    }
  }

  //
  // Halt the system once timeout
  //
  if(Ctr == WaitTime){
    KtiDebugPrintFatal((host, KTI_DEBUG_ERROR, "\nERR_IAR_WRITE for Socket %d, BoxInst %d, Offset 0x%x, Data 0x%x, Ctr %d!!!! \nPQ_CSR_AFEIAR_KTI_CIOPHYDFX_REG value 0x%08x \nCheck PQ_CSR_AFEIAR_KTI_CIOPHYDFX_STRUCT.write_en is TIMEOUT\n", SocId, BoxInst, RegOffset, data, Ctr, AfeIar.Data));
    KTI_ASSERT (FALSE, ERR_FULL_SPEED_TRANSITION, MINOR_ERR_S0_TIMEOUT_IAR | SocId);
  }

  AfeIar.Data = 0;
  AfeIar.Bits.register_id = RegOffset & 0x3F;
  AfeIar.Bits.unit_id = UnitId & 0x3F;
  AfeIar.Bits.wr_data = (UINT8) data;
  AfeIar.Bits.write_en = 1;
  AfeIar.Bits.hw_override_en = 1;
  KtiWritePciCfg (host, SocId, BoxInst, PQ_CSR_AFEIAR_KTI_CIOPHYDFX_REG, AfeIar.Data);
}

/**

  Write to LBC register.

  Note, depending on the value of the parameter LbcType, this function
  will write into LBC_PER_IOU_CONTROL (AFE type) or into KTIMISC_LBC_CONTROL
  (COM type). This function assumes that the bitfield structure of both
  registers is the same and just varies the write destination based on the
  LbcType.

  @param host      - Pointer to the system host (root) structure
  @param SocId     - CPU Socket Node number (Socket ID)
  @param Port      - KTI Port Index, 0 based
  @param LbcType   - LBC_AFE or LBC_COM
  @param LaneMask  - 20-bit wide lane mask
  @param LoadSel   - 7-bit wide load select (address within LBC)
  @param Data      - 14-bit data value to write

**/
VOID
KtiWriteLbc (
  struct sysHost        *host,
  UINT8                 SocId,
  UINT8                 Port,
  UINT8                 LbcType,
  UINT32                LaneMask,
  UINT32                LoadSel,
  UINT32                Data
  )
{
  LBC_PER_IOU_DATA_KTI_CIOPHYDFX_STRUCT    LbcData;
  LBC_PER_IOU_CONTROL_KTI_CIOPHYDFX_STRUCT LbcControl;
  UINT32                                   DataReg;
  UINT32                                   ControlReg;
  UINT32                                   WaitTime, Ctr;
  Ctr      = 0;
  WaitTime = 5000;
  //
  // Setup the Data and Control registers that we'll be using for the windowed interface
  //
  DataReg = (LbcType == TYPE_LBC_AFE) ? LBC_PER_IOU_DATA_KTI_CIOPHYDFX_REG : KTIMISC_LBC_DATA_KTI_LLDFX_REG;
  ControlReg = (LbcType == TYPE_LBC_AFE) ? LBC_PER_IOU_CONTROL_KTI_CIOPHYDFX_REG : KTIMISC_LBC_CONTROL_KTI_LLDFX_REG;

  //
  // Make sure LBC_PER_IOU_CONTROL.lbc_start == 0x0
  //
  LbcControl.Data = 0;
  LbcControl.Bits.lbc_start = 1;
  if (!(host->var.common.emulation & SIMICS_FLAG)) {
     for (Ctr = 0; Ctr < WaitTime; Ctr++) {
       LbcControl.Data = KtiReadPciCfg (host, SocId, Port, ControlReg);
       if(LbcControl.Bits.lbc_start == 0){
         break;
       }
       //
       // Delay 10 microsecs
       //
       KtiFixedDelay(host, 10);
     }
  }

  //
  // Halt the system once timeout
  //
  if(Ctr == WaitTime){
    KtiDebugPrintFatal((host, KTI_DEBUG_INFO1 , "\n Check LBC_PER_IOU_CONTROL.lbc_start is TIMEOUT"));
    KTI_ASSERT (FALSE, ERR_FULL_SPEED_TRANSITION, MINOR_ERR_S0_TIMEOUT_LBC | SocId);
  }

  //
  // Fill LBC_PER_IOU_DATA.lbc_data and write
  //
  LbcData.Data = 0;
  LbcData.Bits.lbc_data = Data & 0x3fff;
  KtiWritePciCfg (host, SocId, Port, DataReg, LbcData.Data);

  //
  // Fill LBC_PER_IOU_CONTROL.lbc_start = 1, lbc_req = 2, lbc_ld_sel, lbc_ln_sel and write
  //
  LbcControl.Bits.lbc_start  = 1;
  LbcControl.Bits.lbc_req    = 2;
  LbcControl.Bits.lbc_ld_sel = LoadSel & 0x7f;
  LbcControl.Bits.lbc_ln_sel = LaneMask & 0x3fffff;
  KtiWritePciCfg (host, SocId, Port, ControlReg, LbcControl.Data);
}

/**

   Delay routine

   @param MicroSecs     - Number of micro seconds to delay

**/
VOID
KtiFixedDelay (
  struct sysHost      *host,
  UINT32          MicroSecs
  )

{

  FixedDelay (host, MicroSecs);

  return;
}

/**

   Returns neighbor and/or remote socket of the given socket.

   @param host              - Pointer to the system host (root) structure
   @param SocketData        - pointer socket specific data
   @param KtiInternalGlobal - KTIRC internal variables.
   @param Soc               - current processor socket ID
   @param NeighborSoc1      - KTI port 1 neighbor socket ID
   @param NeighborSoc2      - KTI port 2 neighbor socket ID
   @param RemoteSoc         - Remote socket ID

  @retval VOID

**/
VOID
GetNeighborSocs (
  struct sysHost             *host,
  KTI_SOCKET_DATA            *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal,
  UINT8                      Soc,
  UINT8                      *NeighborSoc1,
  UINT8                      *NeighborSoc2,
  UINT8                      *RemoteSoc
  )
{
  UINT8 Ctr;
  UINT32 TempList;

  *NeighborSoc1 = *NeighborSoc2 = *RemoteSoc = 0xFF;

  if (host->var.kti.SysConfig == SYS_CONFIG_1S) {
    return;
  } else if (host->var.kti.SysConfig == SYS_CONFIG_2S1L || host->var.kti.SysConfig == SYS_CONFIG_2S2L) {
    (Soc == KtiInternalGlobal->SbspSoc) ? (*NeighborSoc1 = 1) : (*NeighborSoc1 = KtiInternalGlobal->SbspSoc);
    return;
  }

  //
  // Is it a 4S system with only 2 CPUs populated?
  //
  if (KtiInternalGlobal->TotCpu == 2) {
    if (SocketData->Cpu[Soc].LinkData[0].Valid == TRUE) {
      *NeighborSoc1 = (UINT8) SocketData->Cpu[Soc].LinkData[0].PeerSocId;
    } else {
      *NeighborSoc1 = (UINT8) SocketData->Cpu[Soc].LinkData[1].PeerSocId;
    }
    //
    // Even though the other 2 sockets are missing, identify them for credit programming simplicity.
    //
    for (Ctr = 0; Ctr < MAX_SOCKET; Ctr++) {
      if ((KtiInternalGlobal->CpuList & (1 << Ctr)) == 0) {
        *NeighborSoc2 = Ctr;
        break;
      }
    }

    for (Ctr++; Ctr < MAX_SOCKET; Ctr++) {
      if ((KtiInternalGlobal->CpuList & (1 << Ctr)) == 0) {
        *RemoteSoc = Ctr;
        break;
      }
    }

    return;
  } else {
    //
    // Must be a 3S or  4S system
    //
    if (SocketData->Cpu[Soc].LinkData[0].Valid == TRUE && SocketData->Cpu[Soc].LinkData[1].Valid == TRUE) {
      *NeighborSoc1 = (UINT8) SocketData->Cpu[Soc].LinkData[0].PeerSocId;
      *NeighborSoc2 = (UINT8) SocketData->Cpu[Soc].LinkData[1].PeerSocId;
      TempList      = (KtiInternalGlobal->CpuList) & ((1 << Soc) | (1 << *NeighborSoc1) | (1 << *NeighborSoc2));
      for (Ctr = 0; Ctr < KtiInternalGlobal->TotCpu; Ctr++) {
        if ((TempList & (1 << Ctr)) == 0) {
          //
          // Socket that is missing in the list must be the Remote Socket
          //
          *RemoteSoc = Ctr;
          break;
        }
      }
    } else {
      if (SocketData->Cpu[Soc].LinkData[0].Valid == TRUE) {
        //
        // This must be serially connected topology with the given socket at the begining or at the end
        //
        *NeighborSoc1 = (UINT8) SocketData->Cpu[Soc].LinkData[0].PeerSocId;
      } else {
        *NeighborSoc1 = (UINT8) SocketData->Cpu[Soc].LinkData[1].PeerSocId;
      }

      if (SocketData->Cpu[*NeighborSoc1].LinkData[0].PeerSocId == Soc) {
        //
        // NeighborSoc1 must have both the links valid
        //
        *NeighborSoc2 = (UINT8) SocketData->Cpu[*NeighborSoc1].LinkData[1].PeerSocId;
      } else {
        *NeighborSoc2 = (UINT8) SocketData->Cpu[*NeighborSoc1].LinkData[0].PeerSocId;
      }

      if (KtiInternalGlobal->TotCpu == 3) {
        TempList = (KtiInternalGlobal->CpuList) & ((1 << Soc) | (1 << *NeighborSoc1) | (1 << *NeighborSoc2));
        for (Ctr = 0; Ctr < KtiInternalGlobal->TotCpu; Ctr++) {
          if ((TempList & (1 << Ctr)) == 0) {
            //
            // Socket that is missing in the list must be the Remote Socket
            //
            *RemoteSoc = Ctr;
            break;
          }
        }
      } else {
        if (SocketData->Cpu[*NeighborSoc2].LinkData[0].PeerSocId == *NeighborSoc1) {
          //
          // NeighborSoc2 must have both the links valid
          //
          *RemoteSoc = (UINT8) SocketData->Cpu[*NeighborSoc2].LinkData[1].PeerSocId;
        } else {
          *RemoteSoc = (UINT8) SocketData->Cpu[*NeighborSoc2].LinkData[0].PeerSocId;
        }
      }
    }
  }
  return;
}

#ifdef RAS_SUPPORT
/**

    PCI read during for RAS routines.

    @param host    - pointer to the system host root structure
    @param SocId   - Socket ID
    @param BoxInst - Box Instance, 0 based
    @param Offset  - Register Offset

    @retval PCI address contents

**/
UINT32
ReadCpuPciCfgRas (
  PSYSHOST host,
  UINT8    SocId,
  UINT8    BoxInst,
  UINT32   Offset
  )
{
  UINT8       *RegRealAddr, Flag, Size;
  UINT32      RegAddress, ShadowBufOffset, RegRealBus, PhysBus, RegRealDev, RegRealFun, RegRealOff, FlagOffset, Data32;
  CSR_OFFSET  RegOffset;

  //
  // Get the size of the register being accessed
  //
  RegOffset.Data = Offset;
  Size = (UINT8)RegOffset.Bits.size;

  //
  // Get the real address of this register
  //
  RegRealAddr = GetCpuPciCfgAddress (host, SocId, BoxInst, Offset, &Size);

  RegAddress = (UINT32)((UINTN)RegRealAddr);
  RegRealBus = (RegAddress >> 20) & 0xFF;
  RegRealDev = (RegAddress >> 15) & 0x1F;
  RegRealFun = (RegAddress >> 12) & 0x7;
  RegRealOff = RegAddress & 0xFFF;

  //
  // Get the physical bus number
  //
  PhysBus = GetPhysicalBusNumber (host, SocId, RegRealBus);
  KTI_ASSERT ((PhysBus < 0x100), ERR_SW_CHECK, MINOR_ERR_SW_CHECK_43);


  if (mDirectCommit == TRUE || RegRealOff >= MAX_REG_OFFSET) {
    //
    // These registers can not be in the shadow buffer, so read it from actual register
    //
    if (Size == sizeof (UINT32)) {
      Data32 = *((volatile UINT32 *)RegRealAddr);
    } else if (Size == sizeof (UINT16)) {
      Data32 = *((volatile UINT16 *)RegRealAddr);
    } else {
      Data32 = *((volatile UINT8 *)RegRealAddr);
    }
  } else {
    //
    // If the shawdow buffer has the latest copy, return it; otherwise read it from the register
    //
    ShadowBufOffset = (SocId * MAX_BUF_PER_CPU) + (PhysBus * MAX_BUF_PER_BUS) + (RegRealDev * MAX_BUF_PER_DEV) + (RegRealFun * MAX_BUF_PER_FUN) + RegRealOff;
    Flag = *(mCpuShadowFlag + (ShadowBufOffset / 8));
    FlagOffset = ShadowBufOffset % 8;
    if (Size == sizeof (UINT32)) {
      Flag = (Flag >> FlagOffset) & 0xF;
      if (Flag != 0) {
        Data32 = *((UINT32 *)(mCpuShadowBuf + ShadowBufOffset));
      } else {
        Data32 = *((volatile UINT32 *)RegRealAddr);
      }
    } else if (Size == sizeof (UINT16)) {
      Flag = (Flag >> FlagOffset) & 0x3;
      if (Flag != 0) {
        Data32 = *((UINT16 *)(mCpuShadowBuf + ShadowBufOffset));
      } else {
        Data32 = *((volatile UINT16 *)RegRealAddr);
      }
    } else {
      Flag = (Flag >> FlagOffset) & 0x1;
      if (Flag != 0) {
        Data32 = *((UINT8 *)(mCpuShadowBuf + ShadowBufOffset));
      } else {
        Data32 = *((volatile UINT8 *)RegRealAddr);
      }
    }
  }

  return Data32;

}

/**

    PCI write for RAS routines

    @param host    - pointer to the system host root structure
    @param SocId   - Socket ID
    @param BoxInst - Box Instance, 0 based
    @param Offset  - Register Offset
    @param Data    - Data to write

    @retval None

**/
void
WriteCpuPciCfgRas (
  PSYSHOST host,
  UINT8    SocId,
  UINT8    BoxInst,
  UINT32   Offset,
  UINT32   Data
  )
{
  UINT8       *RegRealAddr, Flag, Size;
  UINT32      RegAddress, ShadowBufOffset, RegRealBus, PhysBus, RegRealDev, RegRealFun, RegRealOff, FlagOffset;
  CSR_OFFSET  RegOffset;

  //
  // Get the size of the register being accessed
  //
  RegOffset.Data = Offset;
  Size = (UINT8)RegOffset.Bits.size;

  //
  // Get the real address of this register
  //
  RegRealAddr = GetCpuPciCfgAddress (host, SocId, BoxInst, Offset, &Size);

  RegAddress = (UINT32)((UINTN)RegRealAddr);
  RegRealBus = (RegAddress >> 20) & 0xFF;
  RegRealDev = (RegAddress >> 15) & 0x1F;
  RegRealFun = (RegAddress >> 12) & 0x7;
  RegRealOff = RegAddress & 0xFFF;

  //
  // Get the physical bus number
  //
  PhysBus = GetPhysicalBusNumber (host, SocId, RegRealBus);
  KTI_ASSERT ((PhysBus < 0x100), ERR_SW_CHECK, MINOR_ERR_SW_CHECK_44);


  if (mDirectCommit == TRUE || RegRealOff >= MAX_REG_OFFSET) {   // krishna.. remove mdirectCommit
    if (Size == sizeof (UINT32)) {
      *((volatile UINT32 *)RegRealAddr) = Data;
    } else if (Size == sizeof (UINT16)) {
      *((volatile UINT16 *)RegRealAddr) = (UINT16) Data;
    } else {
      *((volatile UINT8 *)RegRealAddr) = (UINT8) Data;
    }
  } else {

    //
    // Update the Shawdow Buffer and respective flag indicating that the shawdow buffer has the latest copy of the register
    //
    ShadowBufOffset = (SocId * MAX_BUF_PER_CPU) + (PhysBus * MAX_BUF_PER_BUS) + (RegRealDev * MAX_BUF_PER_DEV) + (RegRealFun * MAX_BUF_PER_FUN) + RegRealOff;
    Flag = *(mCpuShadowFlag + (ShadowBufOffset / 8));
    FlagOffset = ShadowBufOffset % 8;
    if (Size == sizeof (UINT32)) {
      *((UINT32 *)(mCpuShadowBuf + ShadowBufOffset)) = Data;
      Flag = Flag | (0xF << FlagOffset);
      *(mCpuShadowFlag + (ShadowBufOffset / 8)) = Flag;
      mNextEntry->AccessWidth = 4;
      mNextEntry->DataMask.OrMask = Data;
    } else if (Size == sizeof (UINT16)) {
      *((UINT16 *)(mCpuShadowBuf + ShadowBufOffset)) = (UINT16)Data;
      Flag = Flag | (0x3 << FlagOffset);
      *(mCpuShadowFlag + (ShadowBufOffset / 8)) = Flag;
      mNextEntry->AccessWidth = 2;
      mNextEntry->DataMask.OrMask = (UINT16)Data;
    } else {
      *((UINT8 *)(mCpuShadowBuf + ShadowBufOffset)) = (UINT8)Data;
      Flag = Flag | (0x1 << FlagOffset);
      *(mCpuShadowFlag + (ShadowBufOffset / 8)) = Flag;
      mNextEntry->AccessWidth = 1;
      mNextEntry->DataMask.OrMask = (UINT8)Data;
    }

    //
    // Update the Quiesce Buffer
    //
    mNextEntry->Operation = EnumQuiesceWrite;
    mNextEntry->CsrAddress = (UINT64) RegRealAddr;
    mNextEntry->AndMask = 0;
    mNextEntry++;
  }

  return;
}

/**

  Get physical stack number for the logical bus number passed in.
  Loops through all stacks.

  @param host    - pointer to the system host data structure
  @param Soc     - Socket ID the logical buses are checked on
  @param BusAddr - logical bus number

  @retval Bus - physical bus number

**/

UINT32
GetPhysicalBusNumber (
  PSYSHOST host,
  UINT8    Soc,
  UINT32   BusAddr
  )
{
  UINT32 Bus;

  for (Bus = 0; Bus < MAX_IIO_STACK; Bus++) {
    if (BusAddr == host->var.common.StackBus[Soc][Bus]) {
      return Bus;
    }
  }

  return 0xFFFF;
}
#endif // RAS_SUPPORT

/**

    Send BIOS to pCode mailbox command

    @param host    - pointer to the system host root structure
    @param Cpu     - Socket ID on which command is to be sent
    @param Command - Encoded command
    @param Data    - Data attached to command

    @retval KTI_SUCCESS - on successful completion

**/
KTI_STATUS
SendMailBoxCmdToPcode (
  struct sysHost      *host,
  UINT8               Cpu,
  UINT32              Command,
  UINT32              Data
  )
{

  WriteBios2PcuMailboxCommand (host, Cpu, Command, Data);
  return KTI_SUCCESS;

}

/**

  Reads 32 bit IO port with serial debug output

  @param host      - Pointer to the system host (root) structure
  @param ioAddress - IO Address to use

  @retval Value read from ioAddress

**/
UINT32
KtiInPort32 (
  PSYSHOST host,
  UINT16   ioAddress
  )
{


  return InPort32(host, ioAddress);;
}


/**

  Writes 32 bit IO port

  @param host      - Pointer to the system host (root) structure
  @param ioAddress - IO Address to use
  @param data      - Data to write to IO Address

  @retval VOID

**/
void
KtiOutPort32 (
  PSYSHOST host,
  UINT16   ioAddress,
  UINT32   data
  )
{
  OutPort32(host, ioAddress, data);

}


/**

  Dumps the resource allocation info to log

  @param host      - Pointer to the system host (root) structure
  @param KtiInternalGlobal - KTIRC internal variables.

  @retval Nothing

**/
VOID
DataDumpKtiResourceAllocation (
  struct sysHost          *host,
  KTI_SOCKET_DATA         *SocketData,
  KTI_HOST_INTERNAL_GLOBAL  *KtiInternalGlobal
  )
{

  UINT8     Idx1;
  UINT8     TmpTotCpu;

  if( KtiInternalGlobal->KtiCpuSktHotPlugEn == TRUE) {
    if (host->setup.kti.KtiCpuSktHotPlugTopology == 0) {
      TmpTotCpu =  4;         //online/offline based on 4S topology
    } else {
      TmpTotCpu =  8;         //online/offline based on 8S topology
    }
  } else {
    TmpTotCpu =  MAX_SOCKET;
  }

  //
  // Check we don't overrun structures
  //
  KTI_ASSERT ((TmpTotCpu <= MAX_SOCKET), ERR_SW_CHECK, MINOR_ERR_SW_CHECK_45);

  KtiDebugPrintInfo0((host, KTI_DEBUG_INFO0, "\n\nCPU Resource Allocation"));
  PrintResourceMapHeader(host);
  
  for (Idx1 = 0; Idx1 < TmpTotCpu; Idx1++) {
    if (SocketData->Cpu[Idx1].Valid == TRUE || KtiInternalGlobal->KtiCpuSktHotPlugEn == TRUE ){
      // Make sure the Base & Limits are naturally aligned
      KTI_ASSERT(((host->var.kti.CpuInfo[Idx1].CpuRes.BusBase % SAD_BUS_GRANTY) == 0), ERR_SW_CHECK, MINOR_ERR_SW_CHECK_46);
      KTI_ASSERT((((host->var.kti.CpuInfo[Idx1].CpuRes.BusLimit + 1) % SAD_BUS_GRANTY) == 0), ERR_SW_CHECK, MINOR_ERR_SW_CHECK_47);
      if (host->var.kti.CpuInfo[Idx1].CpuRes.IoBase < host->var.kti.CpuInfo[Idx1].CpuRes.IoLimit) {
        KTI_ASSERT(((host->var.kti.CpuInfo[Idx1].CpuRes.IoBase % SAD_LEG_IO_GRANTY) == 0), ERR_SW_CHECK, MINOR_ERR_SW_CHECK_48);
        KTI_ASSERT((((host->var.kti.CpuInfo[Idx1].CpuRes.IoLimit + 1) % SAD_LEG_IO_GRANTY) == 0), ERR_SW_CHECK, MINOR_ERR_SW_CHECK_49);
      }
      if (host->var.kti.CpuInfo[Idx1].CpuRes.IoApicBase < host->var.kti.CpuInfo[Idx1].CpuRes.IoApicLimit) {
        KTI_ASSERT(((host->var.kti.CpuInfo[Idx1].CpuRes.IoApicBase % SAD_IOAPIC_GRANTY) == 0), ERR_SW_CHECK, MINOR_ERR_SW_CHECK_4A);
        KTI_ASSERT((((host->var.kti.CpuInfo[Idx1].CpuRes.IoApicLimit + 1) % SAD_IOAPIC_GRANTY) == 0), ERR_SW_CHECK, MINOR_ERR_SW_CHECK_4B);
      }
      if (host->var.kti.CpuInfo[Idx1].CpuRes.MmiolBase < host->var.kti.CpuInfo[Idx1].CpuRes.MmiolLimit) {
        KTI_ASSERT(((host->var.kti.CpuInfo[Idx1].CpuRes.MmiolBase % SAD_MMIOL_GRANTY) == 0), ERR_SW_CHECK, MINOR_ERR_SW_CHECK_4C);
        KTI_ASSERT((((host->var.kti.CpuInfo[Idx1].CpuRes.MmiolLimit + 1) % SAD_MMIOL_GRANTY) == 0), ERR_SW_CHECK, MINOR_ERR_SW_CHECK_4D);
      }
      if (host->var.kti.CpuInfo[Idx1].CpuRes.MmiohBase.hi < host->var.kti.CpuInfo[Idx1].CpuRes.MmiohLimit.hi) {
        KTI_ASSERT(((host->var.kti.CpuInfo[Idx1].CpuRes.MmiohBase.lo % 0x40000000) == 0), ERR_SW_CHECK, MINOR_ERR_SW_CHECK_4E); // 1GB
        KTI_ASSERT((((host->var.kti.CpuInfo[Idx1].CpuRes.MmiohLimit.lo + 1) % 0x40000000) == 0), ERR_SW_CHECK, MINOR_ERR_SW_CHECK_4F);
      }
      
      PrintCpuResourceMapDetails (host, Idx1, SocketData, host->var.kti.CpuInfo[Idx1].CpuRes);
    }
  }
}

/*++
  TSC Sync for all sockets
  1. For TSC alignment, BIOS forces assertion of TSC_SYNC ( MAILBOX_BIOS_CMD_TSC_SYNC ) to SBSP.
  2. This triggers TSC capture (need to delay for 20us after completing this on all sockets).
  3. BIOS deasserts TSC_SYNC via same processor command as for assertion (except with deassert state).
  4. BIOS calculates delta between Largest TSC and all sockets ( or BSP and new sockets' TSCs for on-line). Delta =(BSP TSC - AP TSC).
  5. BIOS writes TSC_HOT_PLUG_OFFSET register in sockets or new skt (one time update only).

  @param host              - Pointer to the system host (root) structure
  @param SocketData        - add argument and description to function comment
  @UINT8 PendingSocId      - The pending sock is on-lined
  @param KtiInternalGlobal - KTIRC internal variables.

  @retval Nothing
--*/
VOID
Tsc_Sync (
  struct sysHost           *host,
  KTI_SOCKET_DATA          *SocketData,
  UINT8                    PendingSocId,
  KTI_HOST_INTERNAL_GLOBAL *KtiInternalGlobal
  )
{
  UINT8          Socket, SbspSocket, NoDelta;
  UINT64_STRUCT  TscDelta = { 0, 0 };
  UINT64_STRUCT  TscSnapShot[MAX_SOCKET];
  UINT64_STRUCT  OrgTscSnapShot[MAX_SOCKET];
  UINT64_STRUCT  LargestSnapshot = { 0, 0 };
  TSC_HP_OFFSET_N0_PCU_FUN4_STRUCT TscHpOffset0;
  TSC_HP_OFFSET_N1_PCU_FUN4_STRUCT TscHpOffset1;

  if ( (KtiInternalGlobal->TSCSyncEn != KTI_ENABLE) && (host->var.kti.RasInProgress != TRUE)) {
    return;
  }

  SbspSocket = KtiInternalGlobal->SbspSoc;
  NoDelta = 0xFF;
  MemSetLocal ((VOID *) TscSnapShot, 0x00, sizeof (TscSnapShot));
  MemSetLocal ((VOID *) OrgTscSnapShot, 0x00, sizeof (OrgTscSnapShot));

  //
  // Read all TSC Snapshot before sending the TSC_SYNC assert command
  //
  for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
    if (SocketData->Cpu[Socket].Valid == TRUE) {
      OrgTscSnapShot[Socket].lo = ReadCpuPciCfgEx(host, Socket, 0, UNC_TSC_SNAPSHOT_N0_PCU_FUN4_REG);
      OrgTscSnapShot[Socket].hi = ReadCpuPciCfgEx(host, Socket, 0, UNC_TSC_SNAPSHOT_N1_PCU_FUN4_REG);
    }
  }

  //
  // Step1: Send the TSC_SYNC assert to Pcode in SBSP
  //
  SendMailBoxCmdToPcode(host, SbspSocket, MAILBOX_BIOS_CMD_CPU_ONLY_RESET, 1);

  //
  // Step2: Need to delay for ~20uS after completing this on all sockets.
  //
  KtiFixedDelay(host, 20);

  //
  // Step3: Send TSC_SYNC de-assert to Pcode in SBSP
  //
  SendMailBoxCmdToPcode(host, SbspSocket, MAILBOX_BIOS_CMD_CPU_ONLY_RESET, 0);

  if (!host->var.kti.RasInProgress) {
    //
    // Do the TSC_SYNC in boot time
    //
    for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
      if (SocketData->Cpu[Socket].Valid == TRUE) {
        TscSnapShot[Socket].lo = ReadCpuPciCfgEx(host, Socket, 0, UNC_TSC_SNAPSHOT_N0_PCU_FUN4_REG);
        TscSnapShot[Socket].hi = ReadCpuPciCfgEx(host, Socket, 0, UNC_TSC_SNAPSHOT_N1_PCU_FUN4_REG);
      }
    }

    //
    // Check if all sockets could capture the new snapshot or not.
    //
    if ((TscSnapShot[SbspSocket].lo != OrgTscSnapShot[SbspSocket].lo) || (TscSnapShot[SbspSocket].hi != OrgTscSnapShot[SbspSocket].hi)){
      for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
        if (SocketData->Cpu[Socket].Valid == TRUE) {
          if ((TscSnapShot[Socket].lo == OrgTscSnapShot[Socket].lo) && (TscSnapShot[Socket].hi == OrgTscSnapShot[Socket].hi)){
            //
            // Socket cannot capture new TSC snapshot, show the alert message that TSC SYNC is not supported in this system
            //
            KtiDebugPrintWarn ((host, KTI_DEBUG_WARN, "\n\nWARNING: This system may not support the TSC_SYNC for all sockets. Please disable the TSC_SYNC!!!\n\n"));
            KtiLogWarning(host, 0xFF, 0xFF, 0xFF, WARN_TSC_SYNC_UNSUPPORTED);
            return;
          }
        }
      }
    } else {
      //
      // SBSP doesn't read the new snapshot, de-Assert all TSC_SYNC to see the sbsp could capture the new snapshot or not
      //
      for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
        if (SocketData->Cpu[Socket].Valid == TRUE) {
          SendMailBoxCmdToPcode(host, Socket, MAILBOX_BIOS_CMD_CPU_ONLY_RESET, 0);
          KtiFixedDelay(host, 20);
        }
      }

      // 
      // Re-send the TSC_SYNC command to capture the TSC snapshot again
      //
      SendMailBoxCmdToPcode(host, SbspSocket, MAILBOX_BIOS_CMD_CPU_ONLY_RESET, 1);
      KtiFixedDelay(host, 20);
      SendMailBoxCmdToPcode(host, SbspSocket, MAILBOX_BIOS_CMD_CPU_ONLY_RESET, 0);

      //
      // De-Assert the TSC_SYNC could get the new snapshot, read all snapshot again.
      //
      for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
        if (SocketData->Cpu[Socket].Valid == TRUE) {
          TscSnapShot[Socket].lo = ReadCpuPciCfgEx(host, Socket, 0, UNC_TSC_SNAPSHOT_N0_PCU_FUN4_REG);
          TscSnapShot[Socket].hi = ReadCpuPciCfgEx(host, Socket, 0, UNC_TSC_SNAPSHOT_N1_PCU_FUN4_REG);
        }
      }

      if ((TscSnapShot[SbspSocket].lo != OrgTscSnapShot[SbspSocket].lo) || (TscSnapShot[SbspSocket].hi != OrgTscSnapShot[SbspSocket].hi)){
        for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
          if (SocketData->Cpu[Socket].Valid == TRUE) {
            if ((TscSnapShot[Socket].lo == OrgTscSnapShot[Socket].lo) && (TscSnapShot[Socket].hi == OrgTscSnapShot[Socket].hi)){
              //
              // Socket cannot capture new TSC snapshot, show the alert message that TSC SYNC is not supported in this system
              //
              KtiDebugPrintWarn ((host, KTI_DEBUG_WARN, "\n\nWARNING: This system may not support the TSC_SYNC for all sockets. Please disable the TSC_SYNC!!!\n\n"));
              KtiLogWarning(host, 0xFF, 0xFF, 0xFF, WARN_TSC_SYNC_UNSUPPORTED);
              return;
            }
          }
        }
      } else {
        //
        // Even de-assert all sockets the sbsp still cannot capture the TSC snapshot, return without doing the TSC sync
        //
        KtiDebugPrintWarn ((host, KTI_DEBUG_WARN, "\n\nWARNING: TSC SYNC command cannot capture new snapshot. Abort the TSC SYNC\n\n"));
        KtiLogWarning(host, 0xFF, 0xFF, 0xFF, WARN_TSC_SYNC_UNSUPPORTED);
        return;
      }
    }
    //
    // Find the largest TSC snapshot
    //
    for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
      if (SocketData->Cpu[Socket].Valid == TRUE) {
        if ( IsXGreaterThanYUINT64(TscSnapShot[Socket], LargestSnapshot)){
          LargestSnapshot.hi = TscSnapShot[Socket].hi;
          LargestSnapshot.lo = TscSnapShot[Socket].lo;
          NoDelta = 0;
        } else if ( !IsXEqualYUINT64(TscSnapShot[Socket], LargestSnapshot)){
          NoDelta = 0;
        }
      }
    }

    if (NoDelta == 0xFF) { 
      KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\n All TSC Snapshot are the same, no need to do the sync!!!\n")); 
      return; 
    }


    for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
      if (SocketData->Cpu[Socket].Valid == TRUE) {
        if (IsXEqualYUINT64(TscSnapShot[Socket], LargestSnapshot)) {
          continue;  //Skip the largest snapshot socket
        }

        //
        // Step4: Calculates delta between largest TSC and sockets's TSCs
        //
        TscDelta = SubUINT64(LargestSnapshot, TscSnapShot[Socket]);

        //
        // Step 5: writes TSC_HOT_PLUG_OFFSET register in remote skt (1-time update only)
        //
        TscHpOffset0.Data = TscDelta.lo & 0xFFFFFFFF;
        WriteCpuPciCfgEx (host, Socket, 0, TSC_HP_OFFSET_N0_PCU_FUN4_REG, TscHpOffset0.Data);
        TscHpOffset1.Bits.tsc_offset = TscDelta.hi & 0x7FFFFFFF;
        TscHpOffset1.Bits.tsc_update = 1;
        WriteCpuPciCfgEx (host, Socket, 0, TSC_HP_OFFSET_N1_PCU_FUN4_REG, TscHpOffset1.Data);
      }
    }
  } else {
    //Enforce range allowed by size of TscSnapshot array
    if (PendingSocId >= MAX_SOCKET) {
      KtiDebugPrintWarn ((host, KTI_DEBUG_WARN, "\n\nWARNING: Cannot sync TSC for onlining socket %d. Max Socket = %d!\n\n", PendingSocId, MAX_SOCKET));
      KtiLogWarning(host, PendingSocId, SOCKET_TYPE_CPU, 0xFF, WARN_TSC_SYNC_UNSUPPORTED);
      return;
    }
    //
    // Do the TSC SYNC in on-lined. 
    // Read the snapshot of sbsp and pendingSoc
    //
    TscSnapShot[SbspSocket].lo = ReadCpuPciCfgEx(host, SbspSocket, 0, UNC_TSC_SNAPSHOT_N0_PCU_FUN4_REG);
    TscSnapShot[SbspSocket].hi = ReadCpuPciCfgEx(host, SbspSocket, 0, UNC_TSC_SNAPSHOT_N1_PCU_FUN4_REG);
    
    TscSnapShot[PendingSocId].lo = ReadCpuPciCfgEx(host, PendingSocId, 0, UNC_TSC_SNAPSHOT_N0_PCU_FUN4_REG);
    TscSnapShot[PendingSocId].hi = ReadCpuPciCfgEx(host, PendingSocId, 0, UNC_TSC_SNAPSHOT_N1_PCU_FUN4_REG);

    //
    // Step4: Calculates delta between BSP and sockets's TSCs
    //
    TscDelta = SubUINT64(TscSnapShot[SbspSocket], TscSnapShot[PendingSocId]);

    //
    // Step 5: writes TSC_HOT_PLUG_OFFSET register in new skt (1-time update only)
    //
    TscHpOffset0.Data = TscDelta.lo & 0xFFFFFFFF;
    WriteCpuPciCfgEx (host, PendingSocId, 0, TSC_HP_OFFSET_N0_PCU_FUN4_REG, TscHpOffset0.Data);
    TscHpOffset1.Bits.tsc_offset = TscDelta.hi & 0x7FFFFFFF;
    TscHpOffset1.Bits.tsc_update = 1;
    WriteCpuPciCfgEx (host, PendingSocId, 0, TSC_HP_OFFSET_N1_PCU_FUN4_REG, TscHpOffset1.Data);
  }





  return;
}

UINT8
KtiGetSiliconRevision(
  PSYSHOST  host,
  UINT8     Socket,
  UINT8     CpuStep,
  UINT8     Cf8Access
){
 #ifndef RAS_SUPPORT
  return GetSiliconRevision( host, Socket, CpuStep, Cf8Access);
 #else
 return host->var.cpu.packageBspStepping[Socket];
 #endif
 
}
