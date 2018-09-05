/**
  This file contains an 'Intel Pre-EFI Module' and is licensed
  for Intel CPUs and Chipsets under the terms of your license
  agreement with Intel or your vendor.  This file may be
  modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c) 2005-2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

  @file ProcMemInit.c

--*/

//
// Warning 4740 : flow in or out of asm code suppresses global optimization
//
#ifdef _MSC_VER
#pragma warning(disable : 4740)
#endif //_MSC_VER
#include "CoreApi.h"
#include "ProcMemInit.h"
#include "CpuCsrAccessDefine.h"
#include "SysHostChip.h"
#include "MemProjectSpecific.h"
#include "MemHostChip.h"
#include "CpuPciAccess.h"
#include "ScratchPadReg.h"
#include "KtiLib.h"
#include "BiosSsaChipFunc.h"
#include "MemApiSkx.h"



#include "IioInitLib.h"

#ifdef SSA_FLAG
#define  MRC_HEAP_SIZE   (128*1024) //For BSSA we have the option of having a MAX HEAP SIZE of 128*1024bytes - BiosMalloc()
#endif  //SSA_FLAG

#ifndef MINIBIOS_BUILD
#ifdef ME_SUPPORT_FLAG
#include "Library/IoLib.h"
#endif // ME_SUPPORT_FLAG
#include <Ppi/Dwr.h>
#include <Library/PeiServicesLib.h>
#endif // MINIBIOS_BUILD


#include "Rc_Revision.h"

#define LEGACY_CAPID4_PCU_FUN3_REG                     PCI_REG_ADDR(1,30,3,0x94)



UINTN
EFIAPI
SetSocketMmcfgTable (
  IN UINT8                SocketLastBus[],
  IN UINT8                SocketFirstBus[],
  IN UINT8                segmentSocket[],
  IN UINT32               mmCfgBaseH[],
  IN UINT32               mmCfgBaseL[],
  IN UINT8                NumOfSocket
  );


//
// Function Declarations
//
VOID
OemInitializePlatformData (
  struct sysHost             *host
  );

VOID
OemCheckAndHandleResetRequests (
  struct sysHost             *host
  );

extern VOID PeiPipeSlaveInit(
  UINT32    SocketId,
  UINT32    CfgAddrBDF
  );

VOID
OemCheckForBoardVsCpuConflicts (
  PSYSHOST host
  );

VOID
DisplayWarningLog (
  PSYSHOST host
  );

VOID
OemIssueReset(
  struct sysHost *host,
  UINT8          ResetType
);


UINT8
GetSiliconRevision(
  PSYSHOST  host,
  UINT8     Socket,
  UINT8     CpuStep,
  UINT8     Cf8Access
);
//
// Function Implementations
//

/**

   Executed by SBSP only. Initialize sysHost structure with default data.

  @param host - pointer to sysHost structure on stack

**/
VOID
InitializeDefaultData (
                      struct sysHost             *host
                      )
{
  UINT32  csrReg;
  CAPID0_PCU_FUN3_STRUCT capid0;
  UINT64_STRUCT  msrData;
  UINT32 imc_cnt;
  UINT8  socket, bus0, bus1, bus2, bus3, bus4, bus5, SocIdx, Index;
  UINT32 i;
  UINT32  RegEax;
  struct sysNvram  *nvramPtr;
#ifdef SERIAL_DBG_MSG
  UINT8   *serialDebugMsgLvl;
#endif
  UINT32  RegEbx, RegEcx, RegEdx;
  UINT32      RcRevision;
UINT8         CpuidStep;

#if !defined(MINIBIOS_BUILD) && !defined(KTI_SW_SIMULATION) && !defined(SIM_BUILD)
  PcdSet32S (PcdRcRevision, RC_REVISION);
  RcRevision = PcdGet32(PcdRcRevision);
#else
  RcRevision = RC_REVISION;
#endif
  host->var.common.rcVersion = RcRevision;
  host->nvram.common.rcVersion = RcRevision;

  //
  // Initialize  cpuFamily, cpuType, stepping
  //
  RegEax = 1; RegEcx = 0;
  GetCpuID(&RegEax, &RegEbx, &RegEcx, &RegEdx);

  host->var.common.cpuFamily = RegEax >> 4;
  host->var.common.rdrandSupported = ((RegEcx >> 30) & BIT0);
  CpuidStep = (UINT8)(RegEax & 0x0F);
  if (host->var.common.cpuFamily == CPU_FAMILY_SKX) {
    host->var.common.cpuType = CPU_SKX;
    host->var.common.MaxCoreCount = MAX_CORE;
  } else {
    RC_ASSERT(FALSE, ERR_RC_INTERNAL, RC_ASSERT_MINOR_CODE_26);
  }


  PopulateMemChipPolicy(host);

  //
  // Get number of LLC ways
  //
  RegEax = 4; RegEcx = 3;
  GetCpuID(&RegEax, &RegEbx, &RegEcx, &RegEdx);
  // RegEbx bit[31:22] = ways
  RegEbx = ((RegEbx >> 22) & 0x3ff) + 1;
  host->var.common.LlcWays= (UINT16) RegEbx;
  //
  // Get CPU bus numbers from MSR 0x300
  //
  msrData = ReadMsrLocal(0x300);
  rcPrintf ((host, " InitializeDefaultData():  MSR 300 = 0x%x %x\n", msrData.hi, msrData.lo));
  if ( (msrData.lo == 0) || ((msrData.hi & 0x80000000) == 0)) {
    rcPrintf ((host, "  ERROR: MSR 300 returns incorrect data\n"));
    rcPrintf ((host, "  Hardcode bus0=0, bus1=1, bus2=2, bus3=3, bus4=4, bus5=5\n"));
    bus0 = 0;
    bus1 = 1;
    bus2 = 2;
    bus3 = 3;
    bus4 = 4;
    bus5 = 5;
  } else {
    bus0 = (UINT8) (msrData.lo         & 0xff);
    bus1 = (UINT8) ((msrData.lo >>  8) & 0xff);
    bus2 = (UINT8) ((msrData.lo >> 16) & 0xff);
    bus3 = (UINT8) ((msrData.lo >> 24) & 0xff);
    bus4 = (UINT8) ((msrData.hi      ) & 0xff);
    bus5 = (UINT8) ((msrData.hi >>  8) & 0xff);
  }
  socket = GetSbspSktId(NULL);
  if (socket >= MAX_SOCKET) {
    rcPrintf((host, "ERROR socket = %d and MAX_SOCKET = %d\n", socket,MAX_SOCKET));
    return;
  }
  host->var.common.socketId = host->nvram.common.sbspSocketId = socket;
  rcPrintf ((host, "SBSP socket = %d\n", socket));
  host->var.common.socketPresentBitMap = 1 << socket;
  host->var.common.sbsp = 1;

  CoreGetSetupOptions (host);
  host->var.common.meRequestedSize = 0;

  //
  // Get MMCFG base address
  //
  csrReg = GetMmcfgAddress(host);
  //
  // csrReg = MMCFG_Base
  //
  host->var.common.mmCfgBase = csrReg;

  // For O*L
  host->var.common.HostAddress = (UINT32)host;
  host->var.common.SlavePipeAddress = (UINT32)&PeiPipeSlaveInit;
  host->var.common.MemRasFlag = 0;
  host->var.common.inComingSktId = 0xFF;
  //
  // Out of reset, SEC code assigned 32 buses to each socket
  //

  //
  // Update for SBSP first
  //
  host->var.common.mmCfgBaseL[socket] = csrReg;
  host->var.common.mmCfgBaseH[socket] = 0;
  host->var.common.StackBus[socket][IIO_CSTACK]  = bus0;
  host->var.common.StackBus[socket][IIO_PSTACK0] = bus1;
  host->var.common.StackBus[socket][IIO_PSTACK1] = bus2;
  host->var.common.StackBus[socket][IIO_PSTACK2] = bus3;
  host->var.common.StackBus[socket][IIO_PSTACK3] = bus4;
  host->var.common.StackBus[socket][IIO_PSTACK4] = bus5;
  host->var.common.SocketLastBus[socket] = 0x1f;
  host->var.common.SocketFirstBus[socket] = bus0;
  host->var.common.segmentSocket[socket] = 0;

  //
  // Remote socket use the default busno 0,1,2,3,4,5
  //
  bus0 = 0;
  bus1 = 0x1;
  bus2 = 0x2;
  bus3 = 0x3;
  bus4 = 0x4;
  bus5 = 0x5;


  Index = 0;
  for (SocIdx = 0; SocIdx < MAX_SOCKET; ++SocIdx) {
    if (SocIdx != socket) {
      Index = Index + 1;
      host->var.common.mmCfgBaseL[SocIdx] = csrReg;
      host->var.common.mmCfgBaseH[SocIdx] = 0;

      host->var.common.StackBus[SocIdx][IIO_CSTACK]  = bus0 + (Index * 0x20);
      host->var.common.StackBus[SocIdx][IIO_PSTACK0] = bus1 + (Index * 0x20);
      host->var.common.StackBus[SocIdx][IIO_PSTACK1] = bus2 + (Index * 0x20);
      host->var.common.StackBus[SocIdx][IIO_PSTACK2] = bus3 + (Index * 0x20);
      host->var.common.StackBus[SocIdx][IIO_PSTACK3] = bus4 + (Index * 0x20);
      host->var.common.StackBus[SocIdx][IIO_PSTACK4] = bus5 + (Index * 0x20);
      host->var.common.SocketLastBus[SocIdx] = 0x1f + (Index * 0x20);
      host->var.common.SocketFirstBus[SocIdx] = bus0 + (Index * 0x20);
      host->var.common.segmentSocket[SocIdx] = 0;
    }
  }

  //
  // Update the Si REV in BIOS
  // CPUID stepping is not sufficient for BIOS to recognize all CPU revision
  //
  host->var.common.stepping = GetSiliconRevision (host, socket, CpuidStep, 1);

  //
  // Update MicroCode Revision
  // 0x8B = EFI_MSR_IA32_BIOS_SIGN_ID
  //

  msrData = ReadMsrLocal(0x8B);
  host->var.common.MicroCodeRev = (UINT8)(msrData.hi & 0x0FF);

  GetCpuCsrAccessVar_RC(host, &host->var.CpuCsrAccessVarHost);

  //
  // from this point on, SBSP at legacy socket can utilize ReadCpuPciCfgEx/WriteCpuPciCfgEx functions for CSR access
  //

  //
  // load local CAPID0 thru CAPID4 CSRs to host->var.common.procCom
  //
  host->var.common.procCom[socket].capid0 = ReadCpuPciCfgEx(host, socket, 0, CAPID0_PCU_FUN3_REG);
  host->var.common.procCom[socket].capid1 = ReadCpuPciCfgEx(host, socket, 0, CAPID1_PCU_FUN3_REG);
  host->var.common.procCom[socket].capid2 = ReadCpuPciCfgEx(host, socket, 0, CAPID2_PCU_FUN3_REG);
  host->var.common.procCom[socket].capid3 = ReadCpuPciCfgEx(host, socket, 0, CAPID3_PCU_FUN3_REG);
  host->var.common.procCom[socket].capid4 = ReadCpuPciCfgEx(host, socket, 0, CAPID4_PCU_FUN3_REG);
  host->var.common.procCom[socket].capid5 = ReadCpuPciCfgEx(host, socket, 0, CAPID5_PCU_FUN3_REG);
  host->var.common.procCom[socket].capid6 = ReadCpuPciCfgEx(host, socket, 0, CAPID6_PCU_FUN3_REG);

  //
  // Check Maximum CPU Topology
  //
  capid0.Data = host->var.common.procCom[socket].capid0;
  if (capid0.Bits.wayness < 2) {
    host->var.common.socketType = SOCKET_2S;
  } else {
    host->var.common.socketType = SOCKET_4S;
  }


  //
  // init emulation flag in sysHost structure
  //
  csrReg = ReadCpuPciCfgEx (host, socket, EMULATION_FLAG_CSR_BOXINSTANCE, CSR_EMULATION_FLAG_UBOX_CFG_REG);

  //
  // read SoftSim flag from CSR
  //
  if (csrReg == 0xFFFFFFFF) {
    host->var.common.emulation = 0;
  } else {
    host->var.common.emulation = (UINT8) (csrReg & 0xff);
  }

#ifdef SERIAL_DBG_MSG
  if ((csrReg & QUIET_MODE) && (csrReg != 0xFFFFFFFF)) {
    serialDebugMsgLvl   = (UINT8 *) &host->setup.common.serialDebugMsgLvl;
    *serialDebugMsgLvl  = SDBG_MIN;
  }
#endif

  //
  // Check for optional OEM NVRAM image
  //
  nvramPtr = (struct sysNvram *) host->setup.common.nvramPtr;
  if ((nvramPtr != NULL) && (&host->nvram != nvramPtr)) {
    MemCopy((UINT8 *)&host->nvram, (UINT8 *)nvramPtr, sizeof(host->nvram));
  }


  imc_cnt = MAX_IMC;


  host->var.mem.maxIMC = (UINT8)imc_cnt;


  rcPrintf ((host, "\nsizeof sysHost = %d\n", sizeof(struct sysHost)));
  rcPrintf ((host, "\nnumber of iMC = %d\n", host->var.mem.maxIMC));
  // Assert that build structure size is adequate for runtime loops
  if (MAX_IMC < host->var.mem.maxIMC) {
    rcPrintf ((host, "\nMRC build error!  MAX_IMC %d is less than maxIMC %d\n", MAX_IMC, host->var.mem.maxIMC));
    RC_ASSERT (host, ERR_MRC_STRUCT, ERR_IMC_NUMBER_EXCEEDED);
  }

  host->var.mem.numChPerMC = MAX_MC_CH;
  host->var.common.KtiPortCnt = SI_MAX_KTI_PORTS;

  //
  // init the scaling to 100
  //

  if (host->var.common.KtiPortCnt > MAX_KTI_PORTS) {
    rcPrintf ((host, "\n\n**** Reported KTI port count (%d) exceeds max num of KTI ports supported by BIOS (%d) ****", host->var.common.KtiPortCnt, MAX_KTI_PORTS));
    RC_ASSERT (host, 0, 0);
  }

  for (i = 0; i < host->var.mem.maxIMC; i++) {
    host->var.mem.socket[socket].imcEnabled[i] = 1;
  }

  for (socket = 0; socket < MAX_SOCKET; socket++) {
    //
    // Continue if this socket is not present
    //
    if ((host->var.common.socketPresentBitMap & (BIT0 << socket)) == 0) continue;
    host->var.mem.socket[socket].ddrFreqCheckFlag = 0;
    host->var.mem.ioInitdone[socket] = 0;
  }
  //
  // Now that iMC info available, update the structure
  //
  GetCpuCsrAccessVar_RC(host, &host->var.CpuCsrAccessVarHost);
}

/**

Arguments:

  host - pointer to sysHost structure on stack

**/
VOID
InitializePlatformData (
                       struct sysHost             *host
                       )
{
  UINT64_STRUCT simicsOptions;
  UINT8 sbspSktId;
  UINT32 csrReg;
  csrReg = 0;

#ifndef MINIBIOS_BUILD
  //
  // Install MrcHooksSevicesPpi
  //
  InstallMrcHooksServicesPpi (host);

  //
  // Install MrcHooksChipSevicesPpi
  //
  InstallMrcHooksChipServicesPpi (host);
#endif // MINIBIOS_BUILD

  //
  // Call platform hook for platform-specific init
  //
  OemInitializePlatformData (host);
#if ENHANCED_WARNING_LOG
  PlatformEwlInit(host);
#endif
  sbspSktId = GetSbspSktId(host);

#ifdef SERIAL_DBG_MSG
  //
  // Setup the global com port address to start with Console debug port
  //
  host->var.common.globalComPort = host->setup.common.consoleComPort;
#endif

  //Enable all memFlows by default
  //This needs to be changed when training steps are enabled
  host->memFlows = host->setup.mem.memFlows & ~(MF_L_WR_VREF_EN | MF_L_RD_VREF_EN | MF_CMD_VREF_EN);

  host->memFlowsExt = host->setup.mem.memFlowsExt & ~(MF_EXT_RTTWRT_EN | MF_EXT_NONTGTODT_EN | MF_EXT_PXC_EN);
  host->var.mem.mccpubusno = 2;
  host->var.mem.rtDefaultValue = MRC_ROUND_TRIP_DEFAULT_VALUE;


  csrReg = ReadCpuPciCfgEx (host, sbspSktId, EMULATION_FLAG_CSR_BOXINSTANCE, CSR_EMULATION_FLAG_UBOX_CFG_REG);
  switch (host->var.common.emulation) {
  case SIMICS_FLAG:
    if (csrReg & DDR_TRAINING_EN) {
      //Enable all memFlows that SIMICS supports
      simicsOptions = ReadMsrLocal(0xdeadbee0);
      host->memFlows = simicsOptions.lo;
      host->memFlowsExt = simicsOptions.hi;
    } else {
      //Disable all memFlows
      host->memFlows = 0;
      host->memFlowsExt = 0;
    }
    break;
  }


  rcPrintf ((host, "memFlows = 0x%x, memFlowsExt = 0x%x!\n", host->memFlows, host->memFlowsExt));

#ifdef MINIBIOS_BUILD
  WritePciCfg (host, 0, PWRM_BASE_REG, PWRM_BASE_ADDRESS);
  host->var.common.pwrmBase = ReadPciCfg (host, 0, PWRM_BASE_REG) & 0xFFFF0000;
#endif

  rcPrintf ((host, "Emulation Value is: %x!\n", host->var.common.emulation));
  switch (host->var.common.emulation) {
  case 0:
    rcPrintf ((host, "Running on hardware\n"));
    break;

  case SIMICS_FLAG:
    rcPrintf ((host, "Running on Simics\n"));
    rcPrintf ((host, "Revision: %x\n", host->var.common.emulation));
    break;

  case RTL_SIM_FLAG:
    rcPrintf ((host, "Running on RTL Simulator\n"));
    break;

  }
}


/**

  Halts the active processor

  @param None

  @retval VOID

**/
VOID
HaltCpuLocal (
             VOID
             )
{
#ifdef IA32
#ifdef __GNUC__
  asm
  (
    "hcl_loop:\n\t"
    //
    // halt and wait for the reset sequence to finish
    //
    "hlt\n\t"
    "jmp hcl_loop\n\t"
  );
#else
  _asm
  {
    hcl_loop:
    //
    // halt and wait for the reset sequence to finish
    //
    hlt
    jmp hcl_loop
  }
#endif
#else
  EfiHalt();
#endif
}


/**

     Check if unsupported CPU/steppings are installed by these rules:
     - BIOS supports current CPU stepping N and one stepping older (N-1)
     - BIOS warns if CPU stepping is greater (newer) than N
     - BIOS halts if CPU steppings is lower (older) than N-1

  @param host - pointer to sysHost structure on stack

**/
VOID
CheckSupportedCpu (
  struct sysHost             *host
  )
{
    UINT32   CpuFamily = host->var.common.cpuFamily;
    UINT8    CpuStepping = host->var.common.stepping;
    BOOLEAN  Unsupported = TRUE;

    switch ( CpuFamily ) {

        case CPU_FAMILY_SKX:
             if (CpuStepping == A0_REV_SKX) {

                 //
                 // Normal case - BIOS supports current stepping N and older stepping N-1
                 //
                 Unsupported = FALSE;
                 rcPrintf ((host, "CPU Stepping %2x detected\n", CpuStepping));
             }
             else {
                 if (CpuStepping > A0_REV_SKX) {
                      //
                      // Abnormal case -  Warn for newer stepping N+1
                      //
                      Unsupported = FALSE;
                      rcPrintf ((host, "Warning: Newer CPU Stepping %2x detected\n", CpuStepping));
                 }
             }

             break;

    default:
             break;
   }

   if (Unsupported) {
          //
          // Error case - Unsupported CPU family and/or  stepping. Halt system
          //
          rcPrintf ((host, "Error: Unsupported CPU Family 0x%x and/or Stepping %2x detected. System halted.\n", CpuFamily, CpuStepping));
          HaltCpuLocal();
   }
}



/**

  Executed by System BSP only.
  Common software programming steps needed before warm reset and CPU-only reset.

  @param host - pointer to sysHost structure on stack

  @retval VOID
**/
VOID
PrepareForWarmReset (
                    struct sysHost  *host
                    )
{


  //
  // Set "BIOS_RESET_CPL" flag bit of all CPU sockets in no particular order
  //

  SetBIOSInitDone(host);
  rcPrintf ((host, "BIOS done set\n"));
}



/**

  Executed by System BSP only.
  Check global reset reqeust flag and trigger the proper type of reset.

  @param host - pointer to sysHost structure on stack

  @retval VOID

**/
VOID
CheckAndHandleResetRequests (
                            struct sysHost             *host
                            )
{
  UINT8   socket;
  UINT32  csrReg;

  //
  //  Call OEM hook before triggering a reset
  //
  OemCheckAndHandleResetRequests(host);

  //
  // do nothing but return if no reset is required
  //
  if (host->var.common.resetRequired == 0) {
    return ;

  }

  if (host->var.common.resetRequired) {
    host->var.common.resetRequired |= POST_RESET_WARM;
  }

  // set "Comlepted Cold Reset Flow" flag in sticky scratchpad register SSR07, at 1:11:3:5ch
  // set DCU_MODE_select bit per Setup data

  if (host->var.common.resetRequired & (POST_RESET_WARM)) {
    for (socket = 0; socket < MAX_SOCKET; socket++) {
      if (host->var.common.socketPresentBitMap & (BIT0 << socket)) {
        //
        // if socket is present
        //
        csrReg = ReadCpuPciCfgEx (host, socket, 0, BIOSSCRATCHPAD7_UBOX_MISC_REG);
        if (csrReg & BIT5) {
          continue;
          //
          // skip if bit already set
          //
        } else {

          // pass DCU_Mode select setup option via Scratch Pad register Bit24 for next warm reboot
          if (host->setup.cpu.dcuModeSelect) {
            csrReg |= BIT24;
          }

          // pass debugInterfaceEn setup option via Scratch Pad register Bit27 for next warm reboot
          if (host->setup.cpu.debugInterfaceEn) {
           csrReg |= BIT27;
          }

          //
          // set "Comlepted Cold Reset Flow" bit
          //
          WriteCpuPciCfgEx (host, socket, 0, BIOSSCRATCHPAD7_UBOX_MISC_REG, (csrReg | BIT5));
        }
      }
    }
  }

#ifdef ME_SUPPORT_FLAG
  //
  //  Call ME BIOS hook before triggering a reset
  //
  MEBIOSCheckAndHandleResetRequests(host);
#endif
  //
  // Trigger requested type of reset in the overriding order of:
  //     Power-Good-Reset
  //     Warm-Reset
  //
  if (host->var.common.resetRequired & POST_RESET_POWERGOOD) {
    //
    // Power-Good Reset (aka Cold Reset)
    //
#ifndef MINIBIOS_BUILD
#ifdef ME_SUPPORT_FLAG
    rcPrintf ((host, "\nPrepare for POWER GOOD RESET.\n"));
    // Prepare RST_CPL1 for reset
    SetBIOSInitDone(host);
#endif // ME_SUPPORT_FLAG
#endif // MINIBIOS_BUILD

    rcPrintf ((host, "Issue POWER GOOD RESET!\n\n\n\n"));
    OemIssueReset(host, POST_RESET_POWERGOOD);
    HaltCpuLocal ();
  } else if (host->var.common.resetRequired & POST_RESET_WARM) {
    //
    // Platform Warm Reset
    //
    rcPrintf ((host, "Issue WARM RESET!\n\n\n\n"));
    PrepareForWarmReset(host);
    //
    // Checkpoint to indicate Warm Reset for KTIRC is about to happen.
    // Note this reset is also common to MRC and CPURC
    // STS_KTI_COMPLETE = 0xaf MINOR_STS_ABOUT_TO_RESET = 0x42
    //
    OutputCheckpoint (host, 0xAF, 0x42, 0x00);
    //
    // Trigger warm reset via hardware
    //
    OemIssueReset(host, POST_RESET_WARM);
    HaltCpuLocal ();
  }
}

VOID
ExecuteDirtyWarmResetFlow (
  struct sysHost  *host
)
{
#ifndef MINIBIOS_BUILD
  EFI_STATUS      Status;
  DWR_PPI         *dwr;

  Status = PeiServicesLocatePpi (&gDirtyWarmResetGuid, 0, NULL, &dwr);
  if (!EFI_ERROR(Status)) {
    rcPrintf ((host, "DWR: DWR flow detected\n"));
    dwr->ExecuteDirtyWarmResetFlow();
  }
#endif // MINIBIOS_BUILD
}

/**

 Invocation of Memory and KTI initialization

  @param - Pointer to sysHost structure
  @param - (EFI BIOS only) host->var.common.oemVariable = PeiServices, i.e., General purpose services available to every PEIM.

  host - pointer to sysHost structure on stack
  @retval VOID

**/
VOID
ProcMemInit (
            struct sysHost             *host
            )
{
  UINT8   sbspSktId;
  UINT32  data32;
#ifdef SSA_FLAG
#ifdef  SSA_OEM_HOOK
  UINT8   heap[MRC_HEAP_SIZE];
#endif  // SSA_OEM_HOOK
#endif  // SSA_FLAG

  //
  // Initialize host structure defaults
  //
  rcPrintf ((host, "BIOSSIM: InitializeDefaultData() \n"));
  InitializeDefaultData (host);

  //
  // Initialize platform segment of sysHost
  //
  rcPrintf ((host, "BIOSSIM: InitializePlatformData() \n"));
  InitializePlatformData (host);

  //
  // Initialize the power management timer base address
  //
  InitPmTimer (host);

#ifdef SERIAL_DBG_MSG
  host->var.mem.serialDebugMsgLvl = host->setup.common.serialDebugMsgLvl; //s5332491
  host->var.mem.varStructInit = 1; //Set to indicate setup values have been retrieved into sysVar
#endif // SERIAL_DBG_MSG


#ifdef SSA_FLAG
//Setting the heap size before calling InitHeap()
#ifdef  SSA_OEM_HOOK
  if (((OemDetectPhysicalPresenceSSA(host) == TRUE) && (host->setup.mem.enableBiosSsaLoader)) || (host->setup.mem.enableBiosSsaRMT)) { //Initialize the heap for only the BSSA loader
    host->var.common.heapBase = (UINT32)heap;
    host->var.common.heapSize = MRC_HEAP_SIZE;
  }
#endif  //SSA_OEM_HOOK
#endif  //SSA_FLAG

  //
  // Initialize Heap
  //
  rcPrintf ((host, "BIOSSIM: InitHeap() \n"));
  InitHeap (host);

  //
  // Initialize Cpu Timer
  //
  InitCpuTimer (host);

  //
  // Configure Serial Debug Messages
  //
#ifdef SERIAL_DBG_MSG
  SetupSerialDebugMessages(host);
#endif // SERIAL_DBG_MSG

  if (host->var.common.cpuType == CPU_SKX) {
    rcPrintf ((host, "\nSKX processor detected\n"));
  }
  sbspSktId = GetSbspSktId(host);
  //
  //  Check CPU stepping
  //
  CheckSupportedCpu(host);

  // Save the RC revision to scratchpad and output if necessary
  //
  WriteCpuPciCfgEx (host, sbspSktId, 0, SR_RC_REVISION_CSR, RC_REVISION);

  rcPrintf ((host, "\nRC Version: %08X \n", RC_REVISION));
  rcPrintf ((host, "CCMRC_REVISION_CHANGELIST  : %08d \n", CCMRC_REVISION_CHANGELIST  ));
  rcPrintf ((host, "host = %08X  (pointer to sysHost structure)\n", (UINT32) host));

  //
  // Initialize the gpio base address
  //
#ifdef RC_BEGIN_END_DEBUG_HOOKS
  OutPort8(host, 0x98, 0xde);
#endif
  //
  // Invoke KTI initialization (KTIRC)
  //
  rcPrintf ((host, "KTI Init starting..."));
  KtiInit (host);

#ifdef SEGMENT_ACCESS
  //
  SetSocketMmcfgTable(
    &host->var.common.SocketLastBus[0],
    &host->var.common.SocketFirstBus[0],
    &host->var.common.segmentSocket[0],
    &host->var.common.mmCfgBaseH[0],
    &host->var.common.mmCfgBaseL[0],
    MAX_SOCKET
    );
#endif

  rcPrintf ((host, "KTI Init completed! Reset Requested: %x\n", host->var.common.resetRequired));

      IioEarlyLinkInit(host);
    rcPrintf ((host, "IIO EarlyLink Init completed! Reset Requested: %x\n", host->var.common.resetRequired));

  //
  // If reset pending, clr this bit so SetBIOSInitDone() will not set BIOS_REST_CPL_1 early
  //
  if (host->var.common.resetRequired != 0) {
    WriteCpuPciCfgEx (host, sbspSktId, 0, BIOSSCRATCHPAD1_UBOX_MISC_REG, (~BIT4 & (ReadCpuPciCfgEx (host, sbspSktId, 0, BIOSSCRATCHPAD1_UBOX_MISC_REG))));
  }

  SetBIOSInitDone(host);

  //
  // Initialize Pipe Architecture for use by MRC
  //
  rcPrintf ((host, "Pipe Init starting...\n"));
  InitializePipe (host);
  rcPrintf ((host, "Pipe Init completed! Reset Requested: %x\n", host->var.common.resetRequired));
  //
  // CPU feature early config (Flex Ratio, Desired Cores, SMT etc. that needs a reset to take effect)
  //

  rcPrintf ((host, "CPU Feature Early Config starting...\n"));
  CpuInit (host);
  rcPrintf ((host, "CPU Feature Early Config completed! Reset Requested: %x\n", host->var.common.resetRequired));

  if ((host->var.common.emulation==0) && (host->var.common.resetRequired == 0)) {
    //
    // Collect previous boot errors if any
    //
    CollectPrevBootFatalErrors (host);
  }

  //
  // Check and execute Dirty Warm Reset flow
  //
  ExecuteDirtyWarmResetFlow(host);

  //
  // Invoke memory initialization (MRC)
  //
  {
    rcPrintf ((host, "START_MRC_RUN\n"));
    data32 = host->var.common.printfDepth;
    MemoryInit (host);
    //
    // check if global semaphore ownership has been properly managed
    //
    if (host->var.common.printfDepth != data32) {
        rcPrintf ((host, "MRC internal error: Global Semaphore Ownership Out of Sync\n"));
        rcPrintf ((host, "printfDepth was %x before MRC, is %x after MRC\n", data32, host->var.common.printfDepth));
        FatalError (host, PRINTF_CONTROL_OUTOF_SYNC_ERR_MAJOR, PRINTF_CONTROL_OUTOF_SYNC_ERR_MINOR);
    }
    rcPrintf ((host, "Reset Requested: %x\n", host->var.common.resetRequired));
  }

    if (host->var.common.resetRequired == 0) {
      IioEarlyPostLinkInit(host);
    }

  //
  // Exit Pipe Architecture
  //
  rcPrintf ((host, "Pipe Exit starting...\n"));
  ExitPipe (host);
  rcPrintf ((host, "Pipe Exit completed! Reset Requested: %x\n", host->var.common.resetRequired));
#ifdef SERIAL_DBG_MSG
  //
  // Display Warning Log
  //
  DisplayWarningLog(host);
#endif

  //
  // Restore Serial Debug Messages
  //
#ifdef SERIAL_DBG_MSG
  RestoreSerialDebugMessages(host);
#endif // SERIAL_DBG_MSG
  //
  // Check and Handle requested resets
  //
  rcPrintf ((host, "Checking for Reset Requests ...  \n"));
  CheckAndHandleResetRequests (host);
  //
  // If control comes here, warm boot path has completed.
  // Return and continue with the reset of BIOS POST...
  //
  rcPrintf ((host, "None \nContinue with system BIOS POST ...\n\n"));

  //
  // OEM call to check for Board VS CPU conflicts
  //
  OemCheckForBoardVsCpuConflicts (host);

  //
  // Publish sysHost information to be used in Post Phase
  //
  OemPublishDataForPost (host);

  //
  // A platform PEIM should enable R/W access to E/F segment in the S3 boot path
  // otherwise, this AP wakeup buffer can't be accessed during CPU S3 operation.
  //
  if (host->var.common.bootMode == S3Resume) {
    UINT8                                 Socket;
    PAM0123_CHABC_SAD_STRUCT       Pam0123Reg;
    PAM456_CHABC_SAD_STRUCT        Pam456Reg;
    for (Socket = 0; Socket < MAX_SOCKET; ++Socket) {
      if(host->var.common.socketPresentBitMap & (1 << Socket)) {
        Pam0123Reg.Data = ReadCpuPciCfgEx (host, Socket, 0, PAM0123_CHABC_SAD_REG);
        Pam0123Reg.Bits.pam0_hienable = 3;
        WriteCpuPciCfgEx (host, Socket, 0, PAM0123_CHABC_SAD_REG, Pam0123Reg.Data);
        Pam456Reg.Data = ReadCpuPciCfgEx (host, Socket, 0, PAM456_CHABC_SAD_REG);
        Pam456Reg.Bits.pam5_loenable = 3;
        Pam456Reg.Bits.pam5_hienable = 3;
        Pam456Reg.Bits.pam6_loenable = 3;
        Pam456Reg.Bits.pam6_hienable = 3;
        WriteCpuPciCfgEx (host, Socket, 0, PAM456_CHABC_SAD_REG, Pam456Reg.Data);
      }
    }
  }

}

#ifdef SERIAL_DBG_MSG
/**

  Displays any entries found in the warning log
  There are two versions of this function:
  One for the Enhanced Warning Log
  One for the legacy warning log

  @param - Pointer to sysHost structure

  @retval VOID

**/
#if ENHANCED_WARNING_LOG
VOID
DisplayWarningLog (
  PSYSHOST host
  )
{
  UINT32 logOffset = 0;
  rcPrintf((host, "Enhanced Warning Log: \n"));

  //
  // Return if debug messages are disabled
  //
  if (host->var.mem.serialDebugMsgLvl == 0) {
    return;
  }

  while (logOffset < (host->var.common.ewlPrivateData.status.Header.FreeOffset)) {
    //
    // For now, just treat as legacy warning and print code/checkpoint/memloc
    //
    EWL_ENTRY_HEADER *warningHeader = (EWL_ENTRY_HEADER*) &(host->var.common.ewlPrivateData.status.Buffer[logOffset]);
    EwlPrintWarning (host, warningHeader);
    logOffset += warningHeader->Size;
  }
} // DisplayWarningLog

#else // not ENHANCED_WARNING_LOG

VOID
DisplayWarningLog (
  PSYSHOST host
  )
{
  UINT8 entry;

  //
  // Return if debug messages are disabled
  //
  if (host->var.mem.serialDebugMsgLvl == 0) {
    return;
  }

  for (entry = 0; entry < host->var.common.status.index; entry++) {
    rcPrintf ((host,"Entry %d: Warning Code = 0x%X, Minor Warning Code = 0x%X, Data = 0x%X\n", entry,
              (host->var.common.status.log[entry].code >> 8) & 0xFF, host->var.common.status.log[entry].code & 0xFF,
              host->var.common.status.log[entry].data));

    //
    // Check if this is a memory related warning
    //
    if ((((host->var.common.status.log[entry].code >> 8) & 0xFF) >= 0xE8) &&
        (((host->var.common.status.log[entry].code >> 8) & 0xFF) <= 0xEF)) {

      if (((host->var.common.status.log[entry].data >> 24) & 0xFF) != 0xFF) {
        rcPrintf ((host, "Socket = %d", (host->var.common.status.log[entry].data >> 24) & 0xFF));
      }

      if (((host->var.common.status.log[entry].data >> 16) & 0xFF) != 0xFF) {
        rcPrintf ((host, " Channel = %d", (host->var.common.status.log[entry].data >> 16) & 0xFF));
      }

      if (((host->var.common.status.log[entry].data >> 8) & 0xFF) != 0xFF) {
        rcPrintf ((host, " DIMM = %d", (host->var.common.status.log[entry].data >> 8) & 0xFF));
      }

      if ((host->var.common.status.log[entry].data & 0xFF) != 0xFF) {
        rcPrintf ((host, " Rank = %d", host->var.common.status.log[entry].data & 0xFF));
      }

      rcPrintf ((host, "\n\n"));
    }
  } // entry loop
} // DisplayWarningLog
#endif // ENHANCED_WARNING_LOG
#endif // SERIAL_DBG_MSG

/**
  Get SBSP id for legacy remote SBSP

  @param   -  pointer to host struct
  @param   -  legacy Pch Target

  @retval legacy remote SBSP

**/

UINT8
LegacyRemoteSBSPChip (
  PSYSHOST host,
  UINT32 legacyPchTarget
  )
{
  //
  // We choose SBSP based on legacy PCH location.   This can only be determined via register on the socket with the
  // legacy_pch prior to KTIRC programming this field on non-SBSP sockets.  Assert if this function  called from non-sbsp prior
  // to host structure available
  //
  //
  RC_ASSERT (FALSE, ERR_RC_INTERNAL, RC_ASSERT_MINOR_CODE_28);
  return 0;
} // LegacyRemoteSBSPChip

/**
  Update the Silicon revisoon by stepping, chop type
  Xy_CPU_STEP is the physical stepping number in CPUID
  Xy_REV_SKX is the Si revision number used by BIOS

 SKX   REV_ID  SiliconID  SteppingID  CPUID       ChopType
 A0    0       0          0           0x50650     3
 A1    1       1          0           0x50650     3
 A2    2       2          1           0x50651     3
 B0    3       3          2           0x50652     3
 L0    4       4          2           0x50652     2
 B1    5       5          3           0x50653     3
 H0    6       6          4           0x50654     3 (xcc)
 M0    7       6          4           0x50654     2 (hcc)
 U0    8       6          4           0x50654     0 (lcc)

  @param     -  pointer to host struct
  @Socket    -  Socket ID
  @CpuStep   -  CPU Stepping of CPUID
  @Cf8Access -  1: Use CF8/CFC to access CSR reg, 0: Use ReadCpuPciCfgEx to access CSR reg

  @retval    -  Logical Silicon stepping
**/

UINT8
GetSiliconRevision(
  PSYSHOST  host,
  UINT8     Socket,
  UINT8     CpuStep,
  UINT8     Cf8Access
)
{
  CAPID4_PCU_FUN3_STRUCT Capid4;
  UINT8                  SiRevision,Bus1;
  UINT32                 CsrLegacyAddr = 0;
  UINT32                 Data32;

  SiRevision = 0;
  Data32 = 0;
  Bus1 = host->var.common.StackBus[Socket][IIO_PSTACK0];

  if (Cf8Access) {
    //
    // Pick the right MMCFG_RULE CSR address accordign to current CPU type
    //
    switch (host->var.common.cpuType)  {
      case CPU_SKX:
        Data32 = LEGACY_CAPID4_PCU_FUN3_REG;
        break;
      //case CPU_ICX:
      default:
       break;
    }

    //
    // Use CF8/CFC IO port  to read legacy socket's CAPID4_PCU_FUN3_REG CSR
    //
    CsrLegacyAddr =  (Data32 & 0xff00ffff) | (Bus1 << 16);
    //
    // Read CSR via CF8/CFC IO
    //
#if defined (IA32) || defined (SIM_BUILD) || defined(HEADLESS_MRC)
    OutPort32 (NULL, 0x0cf8, (0x80000000 | CsrLegacyAddr));
    Capid4.Data = InPort32 (NULL, 0x0cfc);
#else
    IoWrite32 (0x0cf8, (0x80000000 | CsrLegacyAddr));
    Capid4.Data = IoRead32 (0x0cfc);
#endif
  } else {
    Capid4.Data = ReadCpuPciCfgEx(host, Socket, 0, CAPID4_PCU_FUN3_REG); 
  }

  if ((host->var.common.cpuType == CPU_SKX)) {
    host->var.common.chopType[Socket] = (UINT8) Capid4.Bits.physical_chop;
    switch (CpuStep) {
      case A0_CPU_STEP:
      //case A1_CPU_STEP:
        SiRevision = A0_REV_SKX;
        break;
      case A2_CPU_STEP:
        SiRevision = A2_REV_SKX;
        break;
      case B0_CPU_STEP:
        if (Capid4.Bits.physical_chop == SKX_HCC_CHOP) {
          SiRevision = L0_REV_SKX;
        } else {
          SiRevision = B0_REV_SKX;
        }
        break;
      case B1_CPU_STEP:
        SiRevision = B1_REV_SKX;
        break;
      case H0_CPU_STEP:
        if (Capid4.Bits.physical_chop == SKX_LCC_CHOP) {
          SiRevision = U0_REV_SKX;
        } else if (Capid4.Bits.physical_chop == SKX_HCC_CHOP) {
          SiRevision = M0_REV_SKX;
        } else {
          SiRevision = H0_REV_SKX;
        }
        break;
    }
  }

  return SiRevision;
}
