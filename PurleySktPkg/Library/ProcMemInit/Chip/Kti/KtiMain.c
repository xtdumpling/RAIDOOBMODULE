//****************************************************************************
//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2015 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//****************************************************************************
//  File History
//
//  Rev. 1.00
//      Bug Fixed:  Add Early video support.
//      Reason:     Purley platform need add it after KTI initialize
//      Auditor:    Salmon Chen
//      Date:       Mar/07/2016
//
//*****************************************************************************
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
// *      Copyright (c) 2004 - 2017 Intel Corp.                             *
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
// *      This file contains entry point of KTIRC.                          *
// *                                                                        *
// **************************************************************************
**/

#include "DataTypes.h"
#include "PlatformHost.h"
#include "SysHost.h"
#include "SysFunc.h"
#include "RcRegs.h"
#include "KtiSetupDefinitions.h"
#include "KtiSi.h"
#include "CpuCsrAccessDefine.h"
#include "KtiLib.h"
#include "KtiCoh.h"
#include "KtiMisc.h"
#include "SysHostChip.h"
// APTIOV_SERVER_OVERRIDE_RC_START
#include <AmiMrcHooksWrapperLib\AmiMrcHooksWrapperLib.h>
// APTIOV_SERVER_OVERRIDE_RC_END

//SMC_PORTING
#include <PiPei.h>
#include <Library/DebugLib.h>
#include <Library/PeiServicesLib.h>

EFI_GUID gEfiEvPpiGuid = { 0xa0015941, 0x1a1, 0x4146,  { 0xb1, 0x1, 0xb1, 0xbb, 0x5, 0xd8, 0x63, 0x1c }};

typedef struct {
   BOOLEAN EarlyVideoOn;
} EFI_PEI_EV_PPI;

EFI_PEI_EV_PPI gEvPpi = {
  TRUE
};

EFI_PEI_PPI_DESCRIPTOR gEvPpiList = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiEvPpiGuid,
  &gEvPpi
};
//SMC_PORTING

VOID
DataDumpKtiRcEntry (
  struct sysHost          *host
  );

VOID
DataDumpKtiRcExit (
  struct sysHost          *host,
  KTI_HOST_INTERNAL_GLOBAL  *KtiInternalGlobal
  );

VOID
DumpCurrentBusInfo (
  struct sysHost          *host,
  KTI_SOCKET_DATA         *SocketData,
  BOOLEAN                 PrintMissingSockets
  );

KTI_STATUS
InitSocketData (
  KTI_SOCKET_DATA     *SocketData
  );

KTI_STATUS
FillEarlySystemInfo (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL  *KtiInternalGlobal
  );

KTI_STATUS
SetupSbspPathToAllSockets (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL  *KtiInternalGlobal
  );

KTI_STATUS
SetupSbspConfigAccessPath (
  struct sysHost             *host,
  KTI_SOCKET_DATA            *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal,
  TOPOLOGY_TREE_NODE         *Interm,
  TOPOLOGY_TREE_NODE         *Dest,
  UINT8                      DestPort
  );

KTI_STATUS
SetupRemoteCpuBootPath (
  struct sysHost             *host,
  KTI_SOCKET_DATA            *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal,
  TOPOLOGY_TREE_NODE         *Interm,
  TOPOLOGY_TREE_NODE         *Dest,
  UINT8                      DestPort
  );

KTI_STATUS
ProgramM3KtiRoute (
  struct sysHost            *host,
  UINT8                     InPort,
  UINT8                     InterSocket,
  UINT8                     DestSocId,
  UINT8                     OutPort
  );

KTI_STATUS
UpdateCpuInfoInKtiInternal(
 struct sysHost             *host,
 KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal,
 UINT8                      SocktId
 );

BOOLEAN
CheckThisSocketInfoWithSbsp(
 struct sysHost             *host,
 KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal,
 UINT8                      SbspSktId,
 UINT8                      SocketId
 );

KTI_STATUS
PreProcessKtiLinks (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal
  );

KTI_STATUS
PreProcessFpgaLinks (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal
  );

KTI_STATUS
CheckForDegraded4S (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal
  );

KTI_STATUS
HandleDegraded4S (
  struct sysHost           *host,
  KTI_SOCKET_DATA          *SocketData,
  KTI_HOST_INTERNAL_GLOBAL *KtiInternalGlobal,
  KTI_DISC_DATA            *DiscEngData
  );

#if MAX_SOCKET > 4
KTI_STATUS
CheckForDegraded8S (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal
  );

KTI_STATUS
HandleDegraded8S (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal
  );

KTI_STATUS
DegradeTo6S (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal,
  KTI_DISC_DATA        *DiscEngData,
  UINT8                CommonSoc
  );

KTI_STATUS
CheckForDegraded6S (
  struct sysHost             *host,
  KTI_SOCKET_DATA            *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal,
  KTI_DISC_DATA              *DiscEngData
  );

BOOLEAN
IsCpuAttachedToBranch (
  struct sysHost           *host,
  KTI_HOST_INTERNAL_GLOBAL *KtiInternalGlobal,
  KTI_CPU_SOCKET_DATA       CpuSocketData[],
  UINT8                     TargetCpuId,
  UINT8                     CpuList[],
  UINT8                     CpuListCnt,
  UINT8                    *AttachedLinkIdPtr
  );


KTI_STATUS
TrimTopology (
  struct sysHost            *host,
  KTI_HOST_INTERNAL_GLOBAL  *KtiInternalGlobal,
  KTI_DISC_DATA             *DiscEngData,
  UINT8                     CpuId
  );
#endif

KTI_STATUS
PrimeHostStructure (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal
  );

KTI_STATUS
PrimeHostStructureIsoc (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal
  );

KTI_STATUS
PrimeHostStructureStack (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal
  );

KTI_STATUS
PrimeHostStructureFpga (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal
  );

KTI_STATUS
PrimeHostStructureSkx (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal
  );

KTI_STATUS
PrimeHostStructurePhyLinkLayer (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal
  );

KTI_STATUS
SyncUpPbspForReset (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal,
  UINT8                  RstType
  );

KTI_STATUS
ProgramSystemRoute (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal
  );

KTI_STATUS
SetupSystemIoSadEntries (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal
  );

KTI_STATUS
ProgramCpuMiscSadEntries (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal,
  UINT8                  SocId
  );

KTI_STATUS
ProgramSystemAddressMap (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal,
  UINT8                  SocId
  );

KTI_STATUS
ProgramMctpBroadcastSettings (
  struct sysHost           *host,
  KTI_SOCKET_DATA          *SocketData,
  KTI_HOST_INTERNAL_GLOBAL *KtiInternalGlobal
  );

KTI_STATUS
ProgramMctpSegmentIdSize (
  struct sysHost *host,
  UINT8          SocId,
  UINT8          IioStack,
  UINT32         MmcfgRule
  );

KTI_STATUS
ProgramCpuIoEntries (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal,
  UINT8                  SocId
  );

KTI_STATUS
ProgramCpuIoEntriesKti (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal,
  UINT8                  SocId
  );

KTI_STATUS
ProgramCpuIoApicEntries (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal,
  UINT8                  SocId
  );

KTI_STATUS
ProgramCpuIoApicEntriesKti (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal,
  UINT8                  SocId
  );

KTI_STATUS
ProgramCpuMmiolEntries (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal,
  UINT8                  SocId
  );

KTI_STATUS
ProgramCpuMmiolEntriesIio (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal,
  UINT8                  SocId
  );

KTI_STATUS
ProgramCpuMmiolEntriesKti (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal,
  UINT8                  SocId
  );

KTI_STATUS
ProgramCpuMmiohEntries (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal,
  UINT8                  SocId
  );

KTI_STATUS
ProgramCpuMmiohEntriesIio (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal,
  UINT8                  SocId
  );

KTI_STATUS
ProgramCpuMmiohEntriesKti (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal,
  UINT8                  SocId
  );

KTI_STATUS
ProgramMmcfgCpuIoEntriesKti (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal,
  UINT8                  SocId
  );

KTI_STATUS
SetupSystemCoherency (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal
  );

KTI_STATUS
SetupSnoopFilter (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal
  );

KTI_STATUS
SetupKtiMisc (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal
  );

KTI_STATUS
SetupKtiMiscSkx (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal
  );

KTI_STATUS
CollectAndClearErrors (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal
  );

KTI_STATUS
SncMisc (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal
  );

KTI_STATUS
CheckS3ResumePath (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal
  );

KTI_STATUS
KtiTopologyDiscovery (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal
  );

KTI_STATUS
KtiTransitionFullSpeed (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL  *KtiInternalGlobal
  );

KTI_STATUS
KtiPcuMiscConfig (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL  *KtiInternalGlobal
  );

KTI_STATUS
PrepareDiscEngData (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal,
  KTI_DISC_DATA       *DiscEngData
  );

KTI_STATUS
LocateRings (
  struct sysHost      *host,
  KTI_DISC_DATA       *DiscEngData
  );

KTI_STATUS
KtiPhyLinkAfterWarmReset (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL  *KtiInternalGlobal
  );

KTI_STATUS
ProgramMsrForWa (
  struct sysHost         *host,
  KTI_SOCKET_DATA        *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal
  );

KTI_STATUS
ProgramMiscBeforeWarmReset (
  struct sysHost         *host,
  KTI_SOCKET_DATA        *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal
  );

KTI_STATUS
KtiProgramBGFOverrides (
  struct sysHost         *host,
  KTI_SOCKET_DATA        *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal
  );

KTI_STATUS
KtiFinalCpuBusCfg (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal
  );

KTI_STATUS
KtiFinalSbspBusCfg (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal,
  UINT8               Targets,
  UINT32              MmcfgTgtlist,
  UINT8               SocId
  );

KTI_STATUS
KtiFinalFpgaBusCfg (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal
  );


KTI_STATUS
KtiFinalPbspBusCfg (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal,
  UINT8               Targets,
  UINT32              MmcfgTgtlist,
  UINT8               SocId
  );

KTI_STATUS
KtiFinalBusCfgKti (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal,
  UINT8               SocId
  );


VOID
KtiNvramVerification (
  struct sysHost      *host,
  KTI_HOST_INTERNAL_GLOBAL  *KtiInternalGlobal
);

VOID
Init_IRQ_Threshold (
  struct sysHost     *host
);

UINT8
CountActiveKtiLink (
  struct sysHost      *host,
  UINT8             Soc
 );


/**

  KTI entry point

  @param host  - Pointer to the system host (root) structure

  @retval KTI STATUS

**/
KTI_STATUS
KtiMain (
  struct sysHost             *host
  )
{
  KTI_SOCKET_DATA SocketData;
  KTI_HOST_INTERNAL_GLOBAL   KtiInternalGlobal;

  //
  // Dump all the setup options and other input options that are expected to be initialized at this point
  //
  DataDumpKtiRcEntry (host);

  //
  // Initialize SocketData and KtiInternalGlobal
  //
  InitSocketData(&SocketData);
  MemSetLocal ((UINT8 *) &KtiInternalGlobal, 0x00, sizeof (KTI_HOST_INTERNAL_GLOBAL));
  MemCopy ((UINT8 *)&KtiInternalGlobal.PhyLinkPerPortSetting, (UINT8 *)&host->setup.kti.PhyLinkPerPortSetting, sizeof (KTI_CPU_SETTING)* MAX_SOCKET);

  //
  // Dump SEC (default) bus assignments
  //
  DumpCurrentBusInfo (host, &SocketData, TRUE);

  //
  // Fill in the system info that is known at this point
  //
  KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\n\n\n******* Collecting Early System Information - START *******"));
  FillEarlySystemInfo (host, &SocketData, &KtiInternalGlobal);
  KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\n******* Collecting Early System Information - END   *******"));

  //
  // Setup MinPath from SBSP to all sockets and collect the LEP information
  //
  KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\n\n\n******* Setting up Minimum Path - START *******\n"));
  OemHookPreTopologyDiscovery (host);
  SetupSbspPathToAllSockets (host, &SocketData, &KtiInternalGlobal);
  OemHookPostTopologyDiscovery (host);
  KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\n******* Setting up Minimum Path - END   *******"));

  //
  // Check if the sysetm has valid KTI topology. Otherwise degrade it to supported topology
  //
  KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\n\n\n******* Check for KTI Topology Degradation - START *******"));
  PreProcessKtiLinks (host, &SocketData, &KtiInternalGlobal);
  KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\n******* Check for KTI Topology Degradation - END *******"));

  //
  // Update KTIRC output structure and verify host if the input values comply with system topology; otherwise force them to default value
  //
  KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\n\n\n******* Checking KTIRC Input Structure - START *******"));
  PrimeHostStructure (host, &SocketData, &KtiInternalGlobal);
  // APTIOV_SERVER_OVERRIDE_RC_START : OEM hook to change KTI parameters
  AmiOemKtiChangeParameterWrapper (host, &SocketData, &KtiInternalGlobal);
  // APTIOV_SERVER_OVERRIDE_RC_END : OEM hook to change KTI parameters
  KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\n******* Checking KTIRC Input Structure - END   *******"));

  //
  // Verify the KTI NVRAM
  //
  KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\n\n\n******* KTI NVRAM Verification - START *******"));
  KtiNvramVerification(host, &KtiInternalGlobal);
  KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\n******* KTI NVRAM Verification - END *******"));


  KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\n\n\n******* Calculate Resource Allocation - START *******"));
  AllocateIioResources(host, &SocketData, &KtiInternalGlobal);
  KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\n******* Calculate Resource Allocation - END   *******"));

  //
  // If we are not in Cold Reset path, make sure the system topology is same as the one before reset
  //
  if (KtiInternalGlobal.CurrentReset != POST_RESET_POWERGOOD) {
    KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\n\n\n******* Check for KTI Topology change across reset - START *******\n"));
    CheckForTopologyChange (host, &SocketData, &KtiInternalGlobal);
    KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\n******* Check for KTI Topology change across reset - END *******"));
  }

  //
  // Sync up the PBSPs
  //
  KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\n\n\n******* Sync Up PBSPs - START *******\n"));
  SyncUpPbspForReset (host, &SocketData, &KtiInternalGlobal, KtiInternalGlobal.CurrentReset);
  KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\n******* Sync Up PBSPs - END   *******"));

  //
  // Program bus numbers and enable mmcfg on PBSPs
  //
  KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\n\n\n******* Program Mmcfg and bus numbers - START *******\n"));
  KtiFinalCpuBusCfg (host, &SocketData, &KtiInternalGlobal);
  KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\n******* Program Mmcfg and bus numbers - END   *******"));

  if (KtiInternalGlobal.CurrentReset == POST_RESET_POWERGOOD || KtiInternalGlobal.ForceColdResetFlow == TRUE) {
    // Program the credits that require reset to take effect
    //
    KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\n\n\n******* Programming Credits - START *******\n"));
    KtiCheckPoint(0xFF, 0xFF, 0xFF, STS_PROTOCOL_LAYER_SETTING, MINOR_STS_PROGRAM_RING_CRDT, host);
    ProgramMeshCredits (host, &SocketData, &KtiInternalGlobal);
    KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\n******* Programming Credits - END   *******"));

    //
    // Program MSR for w/a after all PBSPs are ready for subsequent warm reset
    //
    KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\n\n\n******* Programming MSR for w/a - START *******\n"));
    ProgramMsrForWa (host, &SocketData, &KtiInternalGlobal);
    KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\n******* Programming MSR for w/a - END   *******"));

    //
    // Misc programming before WR
    //
    KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\n\n\n******* Programming Misc CSRs before WR - START *******\n"));
    ProgramMiscBeforeWarmReset (host, &SocketData, &KtiInternalGlobal);
    KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\n******* Programming Misc CSRs before WR - END   *******"));
  }

  if (KtiInternalGlobal.CurrentReset == POST_RESET_POWERGOOD) {
    //
    // Program BGF Pointer Seperation Overrides
    //
    KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\n\n\n******* Programming BGF Overrides - START *******\n"));
    KtiProgramBGFOverrides(host, &SocketData, &KtiInternalGlobal);
    KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\n******* Programming BGF Overrides - END *******"));

    //
    // Transition the links to full speed operation.
    //
    KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\n\n\n******* Full Speed Transition - START *******\n"));
    KtiTransitionFullSpeed (host, &SocketData, &KtiInternalGlobal);
    KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\n******* Full Speed Transition - END *******"));

    //
    // PCU Misc Configuration through pcode mail box command
    //
    KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\n\n\n******* Pcu Misc Config - START *******"));
    KtiPcuMiscConfig(host, &SocketData, &KtiInternalGlobal);
    KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\n******* Pcu Misc Config - END *******"));


  } else {
    //
    // Phy/LL work on warm reset
    //
    KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\n\n\n******* Phy/Link Updates On Warm Reset - START *******\n"));
    KtiPhyLinkAfterWarmReset(host, &SocketData, &KtiInternalGlobal);
    KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\n******* Phy/Link Updates On Warm Reset - END *******"));

  }

  if (host->var.common.resetRequired) {
    goto ExitKtiMain;
  }

  //
  // Do the topology discovery and optimum route calculation
  //
  KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\n\n\n******* Topology Dicovery and Optimum Route Calculation - START *******"));
  KtiTopologyDiscovery (host, &SocketData, &KtiInternalGlobal);
  KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\n******* Topology Dicovery and Optimum Route Calculation - END   *******"));

  //
  // Program the route table computed in the above step for each socket. Unlike the minimum path that we set before,
  // this is the complete, optimum route setting for the topology that is discovered dynamically.
  //
  KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\n\n\n******* Program Optimum Route Table Settings - START *******\n"));
  ProgramSystemRoute (host, &SocketData, &KtiInternalGlobal);
  KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\n******* Program Optimum Route Table Settings - END   *******"));

  //
  // Setup the final IO SAD entries for all CPU sockets
  //
  KtiDebugPrintInfo0((host, KTI_DEBUG_INFO0, "\n\n\n******* Program Final IO SAD Setting - START *******\n"));
  SetupSystemIoSadEntries(host, &SocketData, &KtiInternalGlobal);
  KtiDebugPrintInfo0((host, KTI_DEBUG_INFO0, "\n******* Program Final IO SAD Setting - END   *******"));

  //
  // Setup KTI Protocol Layer miscellaneous registers
  //
  KtiDebugPrintInfo0((host, KTI_DEBUG_INFO0, "\n\n\n******* Program Misc. KTI Parameters - START *******\n"));
  SetupKtiMisc(host, &SocketData, &KtiInternalGlobal);
  KtiDebugPrintInfo0((host, KTI_DEBUG_INFO0, "\n******* Program Misc. KTI Parameters - END   *******"));

  //
  // Program System Coherency registers
  //
  KtiDebugPrintInfo0((host, KTI_DEBUG_INFO0, "\n\n\n******* Program System Coherency Registers - START *******\n"));
  SetupSystemCoherency(host, &SocketData, &KtiInternalGlobal);
  KtiDebugPrintInfo0((host, KTI_DEBUG_INFO0, "\n******* Program System Coherency Registers - END   *******"));

  //
  // S3 resume check to make sure the system configuration didn't change across S3
  //
  KtiDebugPrintInfo0((host, KTI_DEBUG_INFO0, "\n\n\n******* Check for S3 Resume - START *******\n"));
  CheckS3ResumePath(host, &SocketData, &KtiInternalGlobal);
  KtiDebugPrintInfo0((host, KTI_DEBUG_INFO0, "\n******* Check for S3 Resume - END   *******"));


  //
  // SNC Misc and Recovery if failure on previous warm reset boot
  //
  KtiDebugPrintInfo0((host, KTI_DEBUG_INFO0, "\n\n\n******* SNC Misc and Recovery - START *******\n"));
  SncMisc(host, &SocketData, &KtiInternalGlobal);
  KtiDebugPrintInfo0((host, KTI_DEBUG_INFO0, "\n******* SNC Misc and Recovery - END   *******"));

  //
  // TSC Sync for multi-sockets
  //
  KtiDebugPrintInfo0((host, KTI_DEBUG_INFO0, "\n\n\n******* TSC Sync - START *******\n"));
  Tsc_Sync(host, &SocketData, 0xFF, &KtiInternalGlobal);
  KtiDebugPrintInfo0((host, KTI_DEBUG_INFO0, "\n******* TSC Sync - END   *******"));

  //
  // Collect the errors that might have occurred in previous boot and clear the error registers
  //
  KtiDebugPrintInfo0((host, KTI_DEBUG_INFO0, "\n\n\n******* Collect Previous Boot Error - START *******\n"));
  CollectAndClearErrors(host, &SocketData, &KtiInternalGlobal);
  KtiDebugPrintInfo0((host, KTI_DEBUG_INFO0, "\n******* Collect Previous Boot Error - END   *******"));

ExitKtiMain:

  PeiServicesInstallPpi(&gEvPpiList);//SmcPkg_Support

  //
  // Dump output structure
  //
  DataDumpKtiRcExit (host, &KtiInternalGlobal);

  KtiCheckPoint (0xFF, 0xFF, 0xFF, STS_KTI_COMPLETE, MINOR_STS_KTI_COMPLETE, host);
  KtiDebugPrintInfo0((host, KTI_DEBUG_INFO0, "\n******* KTIRC Exit  *******\n\n"));
  return KTI_SUCCESS;
}

/**

    Dump KTI RC variables on KTI RC entry

    @param host - Pointer to the system host (root) structure

    @retval None

**/
VOID
DataDumpKtiRcEntry (
    struct sysHost          *host
    )
{
  UINT8 soc;          //Target socket # (loop index)
  UINT8 port;         //Target port #   (loop ind)

  //
  // Dump KTIRC input structure
  //
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\n\n\n******* KTI Setup Structure *******"));
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nBus Ratio (per socket):"));
  for (soc=0; soc < MAX_SOCKET; soc++) {
      KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "  S%u: %u", soc, host->setup.kti.BusRatio[soc]));
  }
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nLegacyVgaSoc: %u", host->setup.kti.LegacyVgaSoc));
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nLegacyVgaStack: %u", host->setup.kti.LegacyVgaStack));
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nMmioP2pDis: %u", host->setup.kti.MmioP2pDis));
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nDebugPrintLevel: %u", host->setup.kti.DebugPrintLevel));
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nDegradePrecedence: %u", host->setup.kti.DegradePrecedence));
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nKtiLinkSpeedMode: %u (%s)", host->setup.kti.KtiLinkSpeedMode, ((host->setup.kti.KtiLinkSpeedMode == 0) ? "SLOW" : "FAST")));
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nKtiLinkSpeed: %u (%s)", host->setup.kti.KtiLinkSpeed, ((host->setup.kti.KtiLinkSpeed == SPEED_REC_96GT) ? "96GT" :
                                                                                                       (host->setup.kti.KtiLinkSpeed == SPEED_REC_104GT) ? "104GT" :
                                                                                                       (host->setup.kti.KtiLinkSpeed == MAX_KTI_LINK_SPEED) ? "MAX_SUPPORTED" :
                                                                                                       (host->setup.kti.KtiLinkSpeed == FREQ_PER_LINK) ? "PER_LINK" : "UNKNOWN")));
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nKtiLinkL0pEn: %u (%s)", host->setup.kti.KtiLinkL0pEn, ((host->setup.kti.KtiLinkL0pEn == KTI_DISABLE) ? "DISABLED" :
                                                                                                       (host->setup.kti.KtiLinkL0pEn == KTI_ENABLE) ? "ENABLED" :
                                                                                                       (host->setup.kti.KtiLinkL0pEn == KTI_AUTO) ? "AUTO" : "UNKNOWN")));
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nKtiLinkL1En: %u (%s)", host->setup.kti.KtiLinkL1En, ((host->setup.kti.KtiLinkL1En == KTI_DISABLE) ? "DISABLED" :
                                                                                                     (host->setup.kti.KtiLinkL1En == KTI_ENABLE) ? "ENABLED" :
                                                                                                     (host->setup.kti.KtiLinkL1En == KTI_AUTO) ? "AUTO" : "UNKNOWN")));
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nKtiFailoverEn: %u (%s)", host->setup.kti.KtiFailoverEn, ((host->setup.kti.KtiFailoverEn == KTI_DISABLE) ? "DISABLED" :
                                                                                                         (host->setup.kti.KtiFailoverEn == KTI_ENABLE) ? "ENABLED" :
                                                                                                         (host->setup.kti.KtiFailoverEn == KTI_AUTO) ? "AUTO" : "UNKNOWN")));
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nKtiLbEn: %u", host->setup.kti.KtiLbEn));
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nSncEn: %u", host->setup.kti.SncEn));
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nIoDcMode: %u (%s)", host->setup.kti.IoDcMode, ((host->setup.kti.IoDcMode == KTI_DISABLE) ? "DISABLED" :
                                                                                               (host->setup.kti.IoDcMode == IODC_AUTO) ? "AUTO" :
                                                                                               (host->setup.kti.IoDcMode == IODC_EN_REM_INVITOM_PUSH) ? "IODC_EN_REM_INVITOM_PUSH" :
                                                                                               (host->setup.kti.IoDcMode == IODC_EN_REM_INVITOM_ALLOCFLOW) ? "IODC_EN_REM_INVITOM_ALLOCFLOW" :
                                                                                               (host->setup.kti.IoDcMode == IODC_EN_REM_INVITOM_ALLOC_NONALLOC) ? "IODC_EN_REM_INVITOM_ALLOC_NONALLOC" :
                                                                                               (host->setup.kti.IoDcMode == IODC_EN_REM_INVITOM_AND_WCILF) ? "IODC_EN_REM_INVITOM_AND_WCILF" : "UNKNOWN")));
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nDirectoryModeEn: %u", host->setup.kti.DirectoryModeEn));
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nXptPrefetchEn: %u", host->setup.kti.XptPrefetchEn));
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nKtiPrefetchEn: %u", host->setup.kti.KtiPrefetchEn));
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nRdCurForXptPrefetchEn: %u", host->setup.kti.RdCurForXptPrefetchEn));
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nStaleAtoSOptEn: %u", host->setup.kti.StaleAtoSOptEn));
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nLLCDeadLineAlloc: %u", host->setup.kti.LLCDeadLineAlloc));


  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nKtiFpgaEnable (per socket):"));
  for (soc=0; soc < MAX_SOCKET; soc++) {
      KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "  S%u: %u", soc, host->setup.kti.KtiFpgaEnable[soc]));
  }

  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nKtiCrcMode: %u (%s)", host->setup.kti.KtiCrcMode, ((host->setup.kti.KtiCrcMode == CRC_MODE_16BIT) ? "16BIT" :
                                                                                                   (host->setup.kti.KtiCrcMode == CRC_MODE_ROLLING_32BIT) ? "ROLLING_32BIT" :
                                                                                                   (host->setup.kti.KtiCrcMode == CRC_MODE_AUTO) ? "AUTO" : "UNKNOWN")));
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nKtiCpuSktHotPlugEn: %u", host->setup.kti.KtiCpuSktHotPlugEn));
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nKtiCpuSktHotPlugTopology: %u (%s)", host->setup.kti.KtiCpuSktHotPlugTopology, ((host->setup.kti.KtiCpuSktHotPlugTopology == HOT_PLUG_TOPOLOGY_4S) ? "4S" : "8S")));
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nKtiSkuMismatchCheck: %u", host->setup.kti.KtiSkuMismatchCheck));

  // Phy/Link Layer Options (per Port)
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nKtiPortDisable (per port):"));
  for (soc=0; soc < MAX_SOCKET; soc++) {
      KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\n  S%1u:", soc));
      for (port=0; port < host->var.common.KtiPortCnt; port++) {
        KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "%1u ", host->setup.kti.PhyLinkPerPortSetting[soc].Link[port].KtiPortDisable));
    }
  }

  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nKtiLinkVnaOverride (per port):"));
  for (soc=0; soc < MAX_SOCKET; soc++) {
      KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\n  S%1u:", soc));
      for (port=0; port < host->var.common.KtiPortCnt; port++) {
        KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "%1u ", host->setup.kti.PhyLinkPerPortSetting[soc].Link[port].KtiLinkVnaOverride));
    }
  }

  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nKtiLinkSpeed (per port):"));
  for (soc=0; soc < MAX_SOCKET; soc++) {
      KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\n  S%1u:", soc));
      for (port=0; port < host->var.common.KtiPortCnt; port++) {
        KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "%1u ", host->setup.kti.PhyLinkPerPortSetting[soc].Phy[port].KtiLinkSpeed));
    }
  }
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nRsvd (per port):"));
  for (soc=0; soc < MAX_SOCKET; soc++) {
      KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\n  S%1u:", soc));
      for (port=0; port < host->var.common.KtiPortCnt; port++) {
        KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "%1u ", host->setup.kti.PhyLinkPerPortSetting[soc].Phy[port].Rsvd));
    }
  }

  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\n\n\n******* KTI Setup Structure DfxParms *******"));
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nDfxHaltLinkFailReset: %u (%s)", host->setup.kti.DfxParm.DfxHaltLinkFailReset, ((host->setup.kti.DfxParm.DfxHaltLinkFailReset == KTI_AUTO) ? "AUTO" : "MANUAL")));
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nDfxKtiMaxInitAbort: %u (%s)", host->setup.kti.DfxParm.DfxKtiMaxInitAbort, ((host->setup.kti.DfxParm.DfxKtiMaxInitAbort == KTI_AUTO) ? "AUTO" : "MANUAL")));
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nDfxLlcShareDrdCrd %u (%s)", host->setup.kti.DfxParm.DfxLlcShareDrdCrd, ((host->setup.kti.DfxParm.DfxLlcShareDrdCrd == KTI_AUTO) ? "AUTO" : "MANUAL")));
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nDfxBiasFwdMode: %u (%s)", host->setup.kti.DfxParm.DfxBiasFwdMode, ((host->setup.kti.DfxParm.DfxBiasFwdMode == BIAS_FWD_AUTO) ? "AUTO" : "MANUAL")));
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nDfxSnoopFanoutEn: %u (%s)", host->setup.kti.DfxParm.DfxSnoopFanoutEn, ((host->setup.kti.DfxParm.DfxSnoopFanoutEn == KTI_AUTO) ? "AUTO" : "MANUAL")));
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nDfxHitMeEn: %u (%s)", host->setup.kti.DfxParm.DfxHitMeEn, ((host->setup.kti.DfxParm.DfxHitMeEn == KTI_AUTO) ? "AUTO" : "MANUAL")));
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nDfxFrcfwdinv %u (%s)", host->setup.kti.DfxParm.DfxFrcfwdinv, ((host->setup.kti.DfxParm.DfxFrcfwdinv == KTI_AUTO) ? "AUTO" : "MANUAL")));
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nDfxDbpEnable %u (%s)", host->setup.kti.DfxParm.DfxDbpEnable, ((host->setup.kti.DfxParm.DfxDbpEnable == KTI_AUTO) ? "AUTO" : "MANUAL")));
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nDfxOsbEn %u (%s)", host->setup.kti.DfxParm.DfxOsbEn, ((host->setup.kti.DfxParm.DfxOsbEn == KTI_AUTO) ? "AUTO" : "MANUAL")));
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nDfxHitMeRfoDirsEn %u (%s)", host->setup.kti.DfxParm.DfxHitMeRfoDirsEn, ((host->setup.kti.DfxParm.DfxHitMeRfoDirsEn == KTI_AUTO) ? "AUTO" : "MANUAL")));
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nDfxGateOsbIodcAllocEn %u (%s)", host->setup.kti.DfxParm.DfxGateOsbIodcAllocEn, ((host->setup.kti.DfxParm.DfxGateOsbIodcAllocEn == KTI_AUTO) ? "AUTO" : "MANUAL")));
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nDfxDualLinksInterleavingMode %u (%s)", host->setup.kti.DfxParm.DfxDualLinksInterleavingMode, ((host->setup.kti.DfxParm.DfxDualLinksInterleavingMode == KTI_AUTO) ? "AUTO" : "MANUAL")));
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nDfxSystemDegradeMode: %u", host->setup.kti.DfxParm.DfxSystemDegradeMode));
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nDfxVn1En: %u (%s)", host->setup.kti.DfxParm.DfxVn1En, ((host->setup.kti.DfxParm.DfxVn1En == KTI_AUTO) ? "AUTO" : "MANUAL")));
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nDfxD2cEn: %u (%s)", host->setup.kti.DfxParm.DfxD2cEn, ((host->setup.kti.DfxParm.DfxD2cEn == KTI_AUTO) ? "AUTO" : "MANUAL")));
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nDfxD2kEn: %u (%s)", host->setup.kti.DfxParm.DfxD2kEn, ((host->setup.kti.DfxParm.DfxD2kEn == KTI_AUTO) ? "AUTO" : "MANUAL")));

  //
  // DFX Phy/Link layer options (per port):
  //
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nDfxCrcMode (per port):"));
  for (soc=0; soc < MAX_SOCKET; soc++) {
      KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\n  S%1u:", soc));
      for (port=0; port < host->var.common.KtiPortCnt; port++) {
        KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "%1u (%s) ", host->setup.kti.DfxCpuCfg[soc].Link[port].DfxCrcMode, ((host->setup.kti.DfxCpuCfg[soc].Link[port].DfxCrcMode == CRC_MODE_AUTO) ? "AUTO" : "MANUAL")));
    }
  }
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nDfxL0pEnable (per port):"));
  for (soc=0; soc < MAX_SOCKET; soc++) {
      KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\n  S%1u:", soc));
      for (port=0; port < host->var.common.KtiPortCnt; port++) {
        KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "%1u (%s) ", host->setup.kti.DfxCpuCfg[soc].Link[port].DfxL0pEnable, ((host->setup.kti.DfxCpuCfg[soc].Link[port].DfxL0pEnable == KTI_AUTO) ? "AUTO" : "MANUAL")));
    }
  }
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nDfxL1Enable (per port):"));
  for (soc=0; soc < MAX_SOCKET; soc++) {
      KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\n  S%1u:", soc));
      for (port=0; port < host->var.common.KtiPortCnt; port++) {
        KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "%1u (%s) ", host->setup.kti.DfxCpuCfg[soc].Link[port].DfxL1Enable, ((host->setup.kti.DfxCpuCfg[soc].Link[port].DfxL1Enable == KTI_AUTO) ? "AUTO" : "MANUAL")));
    }
  }
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nDfxKtiFailoverEn (per port):"));
  for (soc=0; soc < MAX_SOCKET; soc++) {
      KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\n  S%1u:", soc));
      for (port=0; port < host->var.common.KtiPortCnt; port++) {
        KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "%1u (%s) ", host->setup.kti.DfxCpuCfg[soc].Phy[port].DfxKtiFailoverEn, ((host->setup.kti.DfxCpuCfg[soc].Phy[port].DfxKtiFailoverEn == KTI_AUTO) ? "AUTO" : "MANUAL")));
    }
  }


  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\n\n\n******* Common Setup Structure *******"));
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nmmCfgBase: 0x%08X ", host->setup.common.mmCfgBase));
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nmmCfgSize: 0x%08X", host->setup.common.mmCfgSize));
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nmmiolBase: 0x%08X ", host->setup.common.mmiolBase));
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nmmiolSize: 0x%08X ", host->setup.common.mmiolSize));
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nmmiohBase: 0x%08X ", host->setup.common.mmiohBase));
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nmmiohSize: %u (%uGB per stack) ", host->setup.common.mmiohSize, (1 << (2 * host->setup.common.mmiohSize))));
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nnumaEn: %u ", host->setup.common.numaEn));
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nisocEn: %u ", host->setup.common.isocEn));
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\ndcaEn: %u ", host->setup.common.dcaEn));

  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\n\n\n******* Common Var Structure *******"));
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nresetRequired: %u ", host->var.common.resetRequired));
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nstate: %u ", host->var.common.bootMode));
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nnumCpus: %u ", host->var.common.numCpus));
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nsocketPresentBitMap: 0x%02X ", host->var.common.socketPresentBitMap));
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nmmCfgBase: 0x%08X ", host->var.common.mmCfgBase));
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nmeRequestedSize: %u ", host->var.common.meRequestedSize));
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\n"));

  return;
}


/**

    Dump current bus assignments

    @param host - Pointer to the system host (root) structure
    @param SocketData - Pointer to socket specific data
    @param PrintMissingSockets - TRUE/FALSE

    @retval None

**/
VOID
DumpCurrentBusInfo (
    struct sysHost          *host,
    KTI_SOCKET_DATA         *SocketData,
    BOOLEAN                 PrintMissingSockets
    )
{
  UINT8 soc;

  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nCurrent bus numbers:"));
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nSocket | Stack0 | Stack1 | Stack2 | Stack3 | Stack4 | Stack5 | LastBus | Seg"));
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\n--------------------------------------------------------------------------------"));
  for (soc=0; soc < MAX_SOCKET; soc++) {
    if (PrintMissingSockets || ((SocketData->Cpu[soc].Valid == TRUE) && (SocketData->Cpu[soc].SocType == SOCKET_TYPE_CPU))) {
      KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\n  %u    |  0x%02X  |  0x%02X  |  0x%02X  |  0x%02X  |  0x%02X  |  0x%02X  |   0x%02X  |  %1u",\
        soc, host->var.common.StackBus[soc][IIO_CSTACK], host->var.common.StackBus[soc][IIO_PSTACK0], host->var.common.StackBus[soc][IIO_PSTACK1],\
        host->var.common.StackBus[soc][IIO_PSTACK2], host->var.common.StackBus[soc][IIO_PSTACK3], host->var.common.StackBus[soc][IIO_PSTACK4],\
        host->var.common.SocketLastBus[soc], host->var.common.segmentSocket[soc]));
    }
  }
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\n--------------------------------------------------------------------------------\n"));
  return;
}


/**

    Dump KTI RC variables on KTI RC exit.

    @param host              - pointer to the system host root structure
    @param KtiInternalGlobal - pointer to the KTI RC internal global structure

    @retval None

**/
VOID
DataDumpKtiRcExit (
    struct sysHost          *host,
    KTI_HOST_INTERNAL_GLOBAL  *KtiInternalGlobal
    )
{
  UINT8 Index,Port;
  //
  // Dump KTI output structure
  //
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\n\n\n******* KTI Output Structure *******"));
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nOutLegacyVgaSoc: %u", host->var.kti.OutLegacyVgaSoc));
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nOutLegacyVgaStack: %u", host->var.kti.OutLegacyVgaStack));
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nOutIsocEn: %u", host->var.kti.OutIsocEn));
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nOutHitMeEn: %u", host->var.kti.OutHitMeEn));
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nOutSncEn: %u", host->var.kti.OutSncEn));
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nOutKtiPrefetchEn: %u", host->var.kti.OutKtiPrefetch));
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nOutSncPrefetchEn: %u", host->var.kti.OutSncPrefetchEn));
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nOutSncGbAlignRequired: %u", host->var.kti.OutSncGbAlignRequired));
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nOutIsRouteThrough: %u", host->var.kti.OutIsRouteThrough));

  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nOutSystemRasType: %u (%s)", host->var.kti.OutSystemRasType,
                                                                          ((host->var.kti.OutSystemRasType == HEDT_RAS) ? "HEDT" :
                                                                          (host->var.kti.OutSystemRasType == BASIC_RAS_AX) ? "BASIC" :
                                                                          (host->var.kti.OutSystemRasType == S1WS_RAS) ? "S1WS" :
                                                                          (host->var.kti.OutSystemRasType == CORE_RAS) ? "CORE" :
                                                                          (host->var.kti.OutSystemRasType == FPGA_RAS) ? "FPGA" :
                                                                          (host->var.kti.OutSystemRasType == STANDARD_RAS) ? "STANDARD" :
                                                                          (host->var.kti.OutSystemRasType == ADVANCED_RAS) ? "ADVANCED" : "UNKNOWN")));

  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nOutIoDcMode: %u (%s)", host->var.kti.OutIoDcMode, ((host->var.kti.OutIoDcMode == KTI_DISABLE) ? "DISABLED" :
                                                                                                    (host->var.kti.OutIoDcMode == IODC_AUTO) ? "AUTO" :
                                                                                                    (host->var.kti.OutIoDcMode == IODC_EN_REM_INVITOM_PUSH) ? "IODC_EN_REM_INVITOM_PUSH" :
                                                                                                    (host->var.kti.OutIoDcMode == IODC_EN_REM_INVITOM_ALLOCFLOW) ? "IODC_EN_REM_INVITOM_ALLOCFLOW" :
                                                                                                    (host->var.kti.OutIoDcMode == IODC_EN_REM_INVITOM_ALLOC_NONALLOC) ? "IODC_EN_REM_INVITOM_ALLOC_NONALLOC" :
                                                                                                    (host->var.kti.OutIoDcMode == IODC_EN_REM_INVITOM_AND_WCILF) ? "IODC_EN_REM_INVITOM_AND_WCILF" : "UNKNOWN")));
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nKtiCurrentLinkSpeedMode: %u (%s)", host->var.kti.KtiCurrentLinkSpeedMode, ((host->var.kti.KtiCurrentLinkSpeedMode == 0) ? "SLOW" : "FAST")));
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nOutKtiLinkSpeed: %u (%s)", host->var.kti.OutKtiLinkSpeed, ((host->var.kti.OutKtiLinkSpeed == SPEED_REC_96GT) ? "96GT" :
                                                                                                           (host->var.kti.OutKtiLinkSpeed == SPEED_REC_104GT) ? "104GT" :
                                                                                                           (host->var.kti.OutKtiLinkSpeed == MAX_KTI_LINK_SPEED) ? "MAX_SUPPORTED" :
                                                                                                           (host->var.kti.OutKtiLinkSpeed == FREQ_PER_LINK) ? "PER_LINK" : "UNKNOWN")));
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nOutKtiLinkL0pEn: %u (%s)", host->var.kti.OutKtiLinkL0pEn, ((host->var.kti.OutKtiLinkL0pEn == KTI_DISABLE) ? "DISABLED" :
                                                                                                           (host->var.kti.OutKtiLinkL0pEn == KTI_ENABLE) ? "ENABLED" :
                                                                                                           (host->var.kti.OutKtiLinkL0pEn == KTI_AUTO) ? "AUTO" : "UNKNOWN")));
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nOutKtiLinkL1En: %u (%s)", host->var.kti.OutKtiLinkL1En, ((host->var.kti.OutKtiLinkL1En == KTI_DISABLE) ? "DISABLED" :
                                                                                                         (host->var.kti.OutKtiLinkL1En == KTI_ENABLE) ? "ENABLED" :
                                                                                                         (host->var.kti.OutKtiLinkL1En == KTI_AUTO) ? "AUTO" : "UNKNOWN")));
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nOutKtiFailoverEn: %u (%s)", host->var.kti.OutKtiFailoverEn, ((host->var.kti.OutKtiFailoverEn == KTI_DISABLE) ? "DISABLED" :
                                                                                                             (host->var.kti.OutKtiFailoverEn == KTI_ENABLE) ? "ENABLED" :
                                                                                                             (host->var.kti.OutKtiFailoverEn == KTI_AUTO) ? "AUTO" : "UNKNOWN")));
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nOutKtiCrcMode: %u (%s)", host->var.kti.OutKtiCrcMode, ((host->var.kti.OutKtiCrcMode == CRC_MODE_16BIT) ? "16BIT" :
                                                                                                       (host->var.kti.OutKtiCrcMode == CRC_MODE_ROLLING_32BIT) ? "ROLLING_32BIT" :
                                                                                                       (host->var.kti.OutKtiCrcMode == CRC_MODE_AUTO) ? "AUTO" : "UNKNOWN")));
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nOutBoardVsCpuConflict: 0x%x", host->var.kti.OutBoardVsCpuConflict));

  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nOutKtiPerLinkL1En (per port):"));
  for (Index = 0; Index < MAX_SOCKET; Index++) {
    KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\n  S%1u:", Index));
    for (Port = 0; Port < host->var.common.KtiPortCnt; Port++) {
      KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "%1u ", host->var.kti.OutKtiPerLinkL1En[Index][Port]));
    }
  }

  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nOutKtiFpgaPresent (per socket):"));
  for (Index=0; Index < MAX_SOCKET; Index++) {
      KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "  S%u: %u", Index, host->var.kti.OutKtiFpgaPresent[Index]));
  }


  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nOutKtiFpgaEnable  (per socket):"));
  for (Index=0; Index < MAX_SOCKET; Index++) {
      KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "  S%u: %u", Index, host->var.kti.OutKtiFpgaEnable[Index]));
  }

  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\n"));
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nKtiInternalGlobal->SnoopFanoutEn: %u", KtiInternalGlobal->SnoopFanoutEn));
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nKtiInternalGlobal->SysOsbEn: %u", KtiInternalGlobal->SysOsbEn));
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nKtiInternalGlobal->SysDirectoryModeEn: %u", KtiInternalGlobal->SysDirectoryModeEn));
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nKtiInternalGlobal->D2cEn: %u", KtiInternalGlobal->D2cEn));
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nKtiInternalGlobal->D2kEn: %u", KtiInternalGlobal->D2kEn));
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nKtiInternalGlobal->SnoopFilter: %u", KtiInternalGlobal->SnoopFilter));
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\nKtiInternalGlobal->DualLinksInterleavingMode: %u", KtiInternalGlobal->DualLinksInterleavingMode));

  return;
}

/**

  Routine to initialize SocketData

  @param SocketData - Pointer to socket specific data

  @retval 0 - Successful
  @retval Other - failure
**/
KTI_STATUS
InitSocketData(
  KTI_SOCKET_DATA     *SocketData
  )
{
  UINT8 SocId, M3kti, Port;
  MemSetLocal ((UINT8 *) SocketData, 0x00, sizeof (KTI_SOCKET_DATA));
  for (SocId = 0; SocId < MAX_SOCKET; SocId++) {
    for (M3kti = 0; M3kti < MAX_M3KTI; M3kti++) {
      SocketData->Route[SocId].M3KtiRt[M3kti] = ((0x3 << (SocId*2)) | (0x3 << ((SocId*2) + 16))) ;
    }
    for (Port = 0; Port <  MAX_KTI_PORTS; Port++) {
      SocketData->Route[SocId].KtiAgentRt[Port] = 0x3 << (SocId*2);
    }
  }
  return KTI_SUCCESS;
}


/*++
  Verify the KTI setup knobs are modified during the warm boot and S3, KTI requests cold boot if there is
  any knob changed

  In cold boot: update the nvram structure
  In Warm Boot: Check the BIT1 of SP0
  BIT1=0, the system is not finish the MRC, keep updatintg nvram structure.
  BIT1=1, the system is boot from OS warm boot that means the nvram was saved in variable in previous boot.
          compare the nvram and setup variable, if they are not matched, request the cold boot.
  In S3 resume: Compare the NvRam structure in memory with setup directly

--*/
VOID
KtiNvramVerification (
  struct sysHost      *host,
  KTI_HOST_INTERNAL_GLOBAL  *KtiInternalGlobal

)
{
  UINT32   Data32;
  UINT32   KtiNvramSize;
  KtiNvramSize = &host->nvram.kti.SaveSetupData.ColdResetRequestEnd - &host->nvram.kti.SaveSetupData.ColdResetRequestStart;
  if (host->var.common.bootMode == NormalBoot) {
    if (KtiInternalGlobal->CurrentReset == POST_RESET_POWERGOOD) {
      KtiDebugPrintInfo0((host, KTI_DEBUG_INFO0, "\n----------Update Kti Nvram in COLD BOOT ----------\n"));
      //
      // Update KTI Nvram
      //
      MemCopy((UINT8*)&(host->nvram.kti.SaveSetupData.ColdResetRequestStart), (UINT8*)&(host->setup.kti.ColdResetRequestStart), KtiNvramSize);
    } else if (KtiInternalGlobal->CurrentReset == POST_RESET_WARM) {
      Data32 = KtiReadPciCfg (host, (UINT8) 0, 0, BIOSSCRATCHPAD0_UBOX_MISC_REG);

      if((Data32 & BIT1)!= BIT1){
        //
        // if BIOSSCRATCHPAD0_UBOX_MISC_REG[1] is 0, don't compare the nvram variable
        //
        MemCopy((UINT8*)&(host->nvram.kti.SaveSetupData.ColdResetRequestStart), (UINT8*)&(host->setup.kti.ColdResetRequestStart), KtiNvramSize);
      } else {
        if (OemReadKtiNvram(host)){
          if(MemCompare ((UINT8 *)&host->nvram.kti.SaveSetupData.ColdResetRequestStart, (UINT8 *)&host->setup.kti.ColdResetRequestStart, KtiNvramSize)){
            KtiDebugPrintInfo0((host, KTI_DEBUG_INFO0, "\n Kti setup has been changed in warm boot, request a cold boot!!!\n"));
            host->var.common.resetRequired |= POST_RESET_POWERGOOD;
            return;
          }
        } else {
          //
          // If there is no kti nvram in variable, update the NVRAM data structure
          //
          MemCopy((UINT8*)&(host->nvram.kti.SaveSetupData.ColdResetRequestStart), (UINT8*)&(host->setup.kti.ColdResetRequestStart), KtiNvramSize);
        }
      }
    }
  } else {
    KtiDebugPrintInfo0((host, KTI_DEBUG_INFO0, "\n---------- Verify Kti Nvram in S3 Resume ---------\n"));
    //
    // Resume from S3
    //
    if(MemCompare ((UINT8 *)&host->nvram.kti.SaveSetupData.ColdResetRequestStart, (UINT8 *)&host->setup.kti.ColdResetRequestStart, KtiNvramSize)){
      KtiCheckPoint (0xFF, 0xFF, 0xFF, ERR_S3_RESUME, MINOR_ERR_S3_RESUME_SETUP_MISMATCH, host);
      KtiDebugPrintFatal ((host, KTI_DEBUG_ERROR, "\n Kti Setup Doesn't Match - S3 Resume Failed."));
      KTI_ASSERT (FALSE, ERR_S3_RESUME, MINOR_ERR_S3_RESUME_SETUP_MISMATCH);
    }
  }
}

/**

  Routine to collect information that is available at this stage of booting and needed to initialize KTI.

  @param host  - Pointer to the system host (root) structure
  @param SocketData - Pointer to socket specific data
  @param KtiInternalGlobal - KTIRC Internal Global data

  @retval 0 - Successful
  @retval Other - failure

**/
KTI_STATUS
FillEarlySystemInfo (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL  *KtiInternalGlobal
  )
{
  UINT32  Data32, BusBase, BusSize;
  UINT8   sbspSktId, Idx1, Idx2, Port;
  UINT8                               BusBaseIdx;
  KTIPCSTS_KTI_LLPMON_STRUCT          PcSts;
  KTIREUT_PH_CIS_KTI_REUT_STRUCT      KtiPhCis;
  BOOLEAN                             Fix;
  KtiCheckPoint (0xFF, 0xFF, 0xFF, STS_COLLECT_EARLY_SYSTEM_INFO, MINOR_STS_COLLECT_INFO, host);
  


  // SBSP socket id & Stepping
  sbspSktId = KtiInternalGlobal->SbspSoc = GetSbspSktId(host);

  //
  // Extract the current system reset type from Sticky Scratch 7 (updated by CPURC)
  //
  Data32 = KtiReadPciCfg (host, sbspSktId, 0, BIOSSCRATCHPAD7_UBOX_MISC_REG);
  if ((Data32 >> 5) & 0x1) {
    //
    // Indicates the successful completion of Cold Reset flow.
    //
    KtiInternalGlobal->CurrentReset = POST_RESET_WARM;
  } else {
    KtiInternalGlobal->CurrentReset = POST_RESET_POWERGOOD;
  }
  host->var.common.CurrentReset = KtiInternalGlobal->CurrentReset;
  host->var.kti.OutSystemRasType = UNDEFINED_RAS;
  UpdateCpuInfoInKtiInternal(host, KtiInternalGlobal, sbspSktId);

  KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\n  SBSP Socket: %u   Ways: 0x%02X   Ras: 0x%02X   Stepping: 0x%02X ",
                       sbspSktId, KtiInternalGlobal->SocketWays[sbspSktId], KtiInternalGlobal->RasType[sbspSktId], host->var.common.stepping));
  KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\n  Total Chas: %02u   Cha List: 0x%02X  Total M3Kti: %02u   Total KtiAgent: %02u",
                       KtiInternalGlobal->TotCha[sbspSktId], KtiInternalGlobal->ChaList[sbspSktId], host->var.kti.CpuInfo[sbspSktId].TotM3Kti, KtiInternalGlobal->TotKtiAgent[sbspSktId]));
  KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\n  Total Active M2Pcie: %02u M2Pcie Active List 0x%02X \n", KtiInternalGlobal->TotActiveM2Pcie[sbspSktId], KtiInternalGlobal->M2PcieActiveBitmap[sbspSktId]));
  //
  // Fill the host common structure with the stack bus numbers assigned to each
  // populated socket so that the CSR access routines become usable for all sockets.
  // Note that the bus number for SBSP is filled by CPURC; so the CSR access routines can be
  // used for SBSP CSR space before we do this step.
  //
  // In reset flow, assume maximal configuration and initialize the output structure accordingly
  // If S0 is not the legacy socket, then assign the bus number such that Legacy Socket (i.e SBSP) gets bus range starting from 0
  //
  host->var.common.numCpus = MAX_SOCKET;
  host->var.common.socketPresentBitMap = FLD_MASK(MAX_SOCKET, 0);
  BusBase = 0;
  BusSize = 256 / SI_MAX_CPU_SOCKETS;

  host->var.common.SocketFirstBus[sbspSktId] = (UINT8)BusBase;
  BusBaseIdx = 0;
  for (Idx2 = 0; Idx2 < MAX_IIO_STACK; Idx2++) {

    //
    // Sync the SBSP BUSNO to LEGACY_BUS_NO in ProcessorStartupUncore.inc
    //
    if(Idx2 != 0){
      BusBaseIdx = Idx2 | 0x10;
    }
    host->var.common.StackBus[sbspSktId][Idx2] = (UINT8)BusBase + BusBaseIdx;
  }
  BusBase = BusBase + BusSize;
  host->var.common.SocketLastBus[sbspSktId] = (UINT8)(BusBase - 1);

  for (Idx1 = 0; Idx1 < MAX_SOCKET; Idx1++) {
    if (Idx1 != sbspSktId) {
      host->var.common.SocketFirstBus[Idx1] = (UINT8)BusBase;
      for (Idx2 = 0; Idx2 < MAX_IIO_STACK; Idx2++) {
        host->var.common.StackBus[Idx1][Idx2] = (UINT8)BusBase + Idx2;
      }
      BusBase = BusBase + BusSize;
      host->var.common.SocketLastBus[Idx1] = (UINT8)(BusBase - 1);
    }
  }

  GetCpuCsrAccessVar_RC (host, &host->var.CpuCsrAccessVarHost);
  
  KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\n  TotCpus: %u  CpuList: 0x%02X ", host->var.common.numCpus, host->var.common.socketPresentBitMap));

  //
  // Make sure MMIOL Base/Size is naturally aligned at 64MB, MMIOL Base is >= 256M,
  // and MMIOL base + size <= MMIOL_LIMIT+1.
  //
  KTI_ASSERT((host->setup.common.mmiolBase % 0x4000000 == 0 && host->setup.common.mmiolSize % 0x4000000 == 0), ERR_SW_CHECK, MINOR_ERR_SW_CHECK_64);
  KTI_ASSERT((host->setup.common.mmiolBase >= 0x10000000 && host->setup.common.mmiolBase <= MMIOL_LIMIT), ERR_SW_CHECK, MINOR_ERR_SW_CHECK_65);
  Data32 = MMIOL_LIMIT - host->setup.common.mmiolBase + 1;
  KTI_ASSERT((host->setup.common.mmiolSize <= Data32), ERR_SW_CHECK, MINOR_ERR_SW_CHECK_66);

  //
  // Validate MMCFG base and size:
  //  - MMCFG Base: MUST be 256M aligned (HSD-3615509).  MMCFG Base is >= 256M, and MUST NOT be set to address 4G-64M.
  //  - MMCFG base + size <= MMIOL_LIMIT
  //  - MMCFG Size is 64M, 128M or 256M.
  //
  KTI_ASSERT((host->setup.common.mmCfgBase % 0x10000000 == 0 &&
              host->setup.common.mmCfgBase >= 0x10000000 && host->setup.common.mmCfgBase <= MMIOL_LIMIT), ERR_SW_CHECK, MINOR_ERR_SW_CHECK_67);
  KTI_ASSERT((host->setup.common.mmCfgSize == 0x4000000 ||
              host->setup.common.mmCfgSize == 0x8000000 || host->setup.common.mmCfgSize == 0x10000000 ||
              host->setup.common.mmCfgSize == 0x20000000 || host->setup.common.mmCfgSize == 0x40000000 || host->setup.common.mmCfgSize == 0x80000000),
              ERR_SW_CHECK, MINOR_ERR_SW_CHECK_68);

  //
  // s5332043: MMCFG data looks wrong when MmcfgBase is 1.5G if A stepping
  //

  Fix = FALSE;
  if ((host->var.common.cpuType == CPU_SKX) && (host->var.common.stepping < B0_REV_SKX)) {
    switch(host->setup.common.mmCfgSize/1024/1024){
      case 64:
      case 128:
      case 256:
        if (host->setup.common.mmCfgBase == (UINT32)0x60000000){
          Fix = TRUE;
          KtiDebugPrintFatal ((host, KTI_DEBUG_ERROR, "\n WARN:1.5G mmCfgBase with %xM mmCfgSize is not allowed!!\n", (host->setup.common.mmCfgSize/1024/1024)));
        }
        break;
      case 512:
        if ((host->setup.common.mmCfgBase == (UINT32)0x60000000) || (host->setup.common.mmCfgBase == (UINT32)0x50000000)){
          Fix = TRUE;
          KtiDebugPrintFatal ((host, KTI_DEBUG_ERROR, "\nWARN: 1.5G or 1.25G mmCfgBase with 512M mmCfgSize is not allowed!!\n"));
        }
        break;
      case 1024:
        if ((host->setup.common.mmCfgBase == (UINT32)0x70000000) ||
            (host->setup.common.mmCfgBase == (UINT32)0x60000000) ||
            (host->setup.common.mmCfgBase == (UINT32)0x50000000) ||
            (host->setup.common.mmCfgBase == (UINT32)0x40000000) ||
            (host->setup.common.mmCfgBase == (UINT32)0x30000000)){
          Fix = TRUE;
          KtiDebugPrintFatal ((host, KTI_DEBUG_ERROR, "\nWARN: 1.75G or 1.5G or 1.25G or 1G or 0.75G mmcfgbase with 1024M mmCfgSize is not allowed!!\n"));
        }
        break;
      case 2048:
        if (host->setup.common.mmCfgBase <= (UINT32)0x70000000) {
          Fix = TRUE;
          KtiDebugPrintFatal ((host, KTI_DEBUG_ERROR, "\nWARN: mmCfgBase <= 1.75G  with 2048M mmCfgSize is not allowed!!\n"));
        }
        break;
    }

    if (Fix){
      KtiDebugPrintFatal ((host, KTI_DEBUG_ERROR, "\nPlease change the MMCFG BASE to other base!!\n"));
      KTI_ASSERT(FALSE, ERR_SW_CHECK, MINOR_ERR_SW_CHECK_7C);
    }
  }

  Data32 = MMIOL_LIMIT - host->setup.common.mmCfgBase + 1;
  KTI_ASSERT((host->setup.common.mmCfgSize <= Data32), ERR_SW_CHECK, MINOR_ERR_SW_CHECK_69);

  //
  // Make sure MMCFG & MMIOL regions are not overlapping
  //
  KTI_ASSERT((host->setup.common.mmCfgBase != host->setup.common.mmiolBase), ERR_SW_CHECK, MINOR_ERR_SW_CHECK_6A);
  if (host->setup.common.mmCfgBase < host->setup.common.mmiolBase) {
    Data32 = host->setup.common.mmiolBase - host->setup.common.mmCfgBase;
    KTI_ASSERT((Data32 >= host->setup.common.mmCfgSize), ERR_SW_CHECK, MINOR_ERR_SW_CHECK_6B);
  } else {
    Data32 = host->setup.common.mmCfgBase - host->setup.common.mmiolBase;
    KTI_ASSERT((Data32 >= host->setup.common.mmiolSize), ERR_SW_CHECK, MINOR_ERR_SW_CHECK_6C);
  }

  //
  // Fpga enable?
  //
  for (Idx1 = 0; Idx1 < MAX_SOCKET; Idx1++) {
    if (host->setup.kti.KtiFpgaEnable[Idx1] != KTI_DISABLE) {
      if (host->setup.kti.PhyLinkPerPortSetting[Idx1].Link[XEON_FPGA_PORT].KtiPortDisable == FALSE) {
        KtiInternalGlobal->FpgaEnable[Idx1] = KTI_ENABLE;
      }
    }
  }

  //
  // if SBSP has no KTI Agents, set KtiPortCnt to zero. Do no programming of KTI port registers
  //
  if (KtiInternalGlobal->TotKtiAgent[sbspSktId] == 0) {
    host->var.common.KtiPortCnt = 0;
  }

  //
  // Logic to detect the system KTI link speed mode (slow or fast). Note that all the valid links
  // must be in same speed mode. i.e some can't be in full speed mode while the rest are in slow mode.
  // Find a valid link of SBSP to read the KtiMisc and PhCis registers.
  //
  host->var.kti.KtiCurrentLinkSpeedMode = KTI_LINK_SLOW_SPEED_MODE;
  for (Port = 0; Port < host->var.common.KtiPortCnt; Port++) {
    Data32 = KtiReadPciCfg (host, sbspSktId, Port, VID_KTI_LLPMON_REG);
    if ((Data32 & 0xFFFF) == 0xFFFF) {
      continue;
    }
    PcSts.Data = KtiReadPciCfg (host, sbspSktId, Port, KTIPCSTS_KTI_LLPMON_REG);
    if ((PcSts.Bits.ll_status == LINK_SM_INIT_DONE) || (PcSts.Bits.ll_status == LINK_SM_CRD_RETURN_STALL)) {
      KtiPhCis.Data = KtiReadPciCfg (host, sbspSktId, Port, KTIREUT_PH_CIS_KTI_REUT_REG);
      host->var.kti.KtiCurrentLinkSpeedMode = (UINT8) KtiPhCis.Bits.s_op_speed; //  "op speed" field
      break;
    }
  }
  KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\n  Reset Type: %s   Link Speed: %s", (KtiInternalGlobal->CurrentReset == POST_RESET_POWERGOOD) ? "Cold Reset" : "Warm Reset",
     ((host->var.kti.KtiCurrentLinkSpeedMode == KTI_LINK_SLOW_SPEED_MODE) ? "Slow Speed" : "Fast Speed")));

  return KTI_SUCCESS;
}

/**

  Routine to set minimum required route to allow config accesses coming from one of SBSP's core
  to be routed to the agent that claims the region. It also constructs the SBSP minimum tree
  and collects information such as CPU sockets populated and their LEP info to facilitate
  the topology discovery process to be done without any config access. The route set by this
  routine may not be the optimum path. Note that, the route set by this routine allows only the
  SBSP be able to access the config space of other CPU sockets in the system; not from
  one PBSP to other PBSPs.

  It also sets up the route to the Legacy CPU so that >1 hop away CPUs can be released from reset
  and brought under BIOS control.

  @param host  - Pointer to the system host (root) structure
  @param SocketData - Pointer to socket specific data
  @param KtiInternalGlobal - KTIRC Internal Global data

  @retval 0 - Successful
  @retval Other - failure

**/
KTI_STATUS
SetupSbspPathToAllSockets (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL  *KtiInternalGlobal
  )
{
  KTI_STATUS          Status;
  TOPOLOGY_TREE_NODE  GrandParent;
  TOPOLOGY_TREE_NODE  Parent;
  TOPOLOGY_TREE_NODE  Child;
  UINT8               Index;
  UINT32              Ctr;
  UINT8               KtiLink;
  UINT8               Port;
  UINT32              Data32, WaitTime;
  BOOLEAN             CheckInFlag, FeatureMismatch;
  UINT8               sbspSktId, LegacyPchTargetForAp;

  sbspSktId = KtiInternalGlobal->SbspSoc;
  KtiCheckPoint (sbspSktId, SOCKET_TYPE_CPU, 0, STS_SETUP_MINIMUM_PATH, MINOR_STS_ADD_SOCKET_TO_MIN_PATH_TREE, host);

  //
  //  Add the Sbsp to minimum path tree
  //
  KtiDebugPrintInfo1 ((host, KTI_DEBUG_INFO1, "\n\n Constructing SBSP minimum path Topology Tree "));
  KtiDebugPrintInfo1 ((host, KTI_DEBUG_INFO1, "\n -------------------------------------------- "));
  KtiDebugPrintInfo1 ((host, KTI_DEBUG_INFO1, "\n\n Adding SBSP (CPU%u) to the tree", sbspSktId));

  Parent.RawNode          = 0;
  Parent.Node.Valid       = TRUE;
  Parent.Node.SocId       = sbspSktId;
  Parent.Node.SocType     = SOCKET_TYPE_CPU;
  Parent.Node.ParentPort  = 0xF;
  Parent.Node.Hop         = 0;
  Parent.Node.ParentIndex = 0x7F;

  if ((Status = AddNodeToTopologyTree (host, SocketData->SbspMinPathTree, &Parent)) != KTI_SUCCESS) {
    KtiCheckPoint ((UINT8)(1 << Parent.Node.SocId), SOCKET_TYPE_CPU, 0, ERR_MINIMUM_PATH_SETUP, MINOR_ERR_INTERNAL_DATA_STRUCTURE, host);
    KtiDebugPrintFatal ((host, KTI_DEBUG_ERROR, "\n Adding CPU%u to Minimum Path Tree failed - Data Structure Overflow.", Parent.Node.SocId));
    KTI_ASSERT (FALSE, ERR_MINIMUM_PATH_SETUP, MINOR_ERR_INTERNAL_DATA_STRUCTURE);
  }
  //
  // Add the Sbsp to the socketdata
  //
  SocketData->Cpu[sbspSktId].Valid  = TRUE;
  SocketData->Cpu[sbspSktId].SocId  = sbspSktId;
  SocketData->Cpu[sbspSktId].SocType= SOCKET_TYPE_CPU;

  //
  // Discover the neighbors of Sbsp and setup path to neighbor's M2PCIe/Ubox. Repeat it until no
  // new sockets are found. Also, setup the boot path if a CPU is >1 hop away from Legacy CPU.
  //
  for (Index = 0; SocketData->SbspMinPathTree[Index].Node.Valid == TRUE; Index++) {

    Parent = SocketData->SbspMinPathTree[Index];
    if (Parent.Node.SocType == SOCKET_TYPE_CPU) {

      //
      // Make sure the pCode has completed initializing the socket before making any CSR access in that socket.
      // Limit delay to 5s.
      //
      CheckInFlag = FALSE;
      WaitTime = 5000;
      for (Ctr = 0; Ctr < WaitTime; Ctr++) {
        Data32 = KtiReadPciCfg (host, (UINT8) Parent.Node.SocId, 0, BIOSNONSTICKYSCRATCHPAD0_UBOX_MISC_REG);
        if ((Data32 & 0xFF00) == 0x0200) { // Bits [15:8] should be 0x2 (LLC BIST complete) before the socket can be accessed.
          CheckInFlag = TRUE;
          break;
        }
        KtiFixedDelay(host, 1000); // 1000 microsecs
      }

      if (CheckInFlag == FALSE) {
          // Halt the System
          KtiCheckPoint ((UINT8) (1 << Parent.Node.SocId), (UINT8) Parent.Node.SocType, 0, ERR_BOOT_MODE, MINOR_ERR_PBSP_CHKIN_FAILURE,  host);
          KtiDebugPrintFatal ((host, KTI_DEBUG_ERROR, "\n\npCode has not completed initilization of AP Socket %u. System Halted!!!", Parent.Node.SocId));
          KTI_ASSERT (FALSE, ERR_BOOT_MODE, MINOR_ERR_PBSP_CHKIN_FAILURE);
      }

      //
      // Make sure each discovered socket has Cha Count/List, M3KTI. Socket Sku Type that matches that of SBSP's.
      //
      if (Parent.Node.SocId != sbspSktId) {

        UpdateCpuInfoInKtiInternal(host, KtiInternalGlobal, (UINT8) Parent.Node.SocId);
        GetLegacyPchTargetSktId(host, (UINT8) Parent.Node.SocId, &LegacyPchTargetForAp);
        FeatureMismatch = CheckThisSocketInfoWithSbsp(host, KtiInternalGlobal, sbspSktId, (UINT8) Parent.Node.SocId);

        if (sbspSktId != LegacyPchTargetForAp) {
          KtiCheckPoint ((UINT8) (1 << Parent.Node.SocId), (UINT8) Parent.Node.SocType, 0, ERR_UNSUPPORTED_TOPOLOGY, MINOR_ERR_LEGACY_PCH_MISMATCH, host);
          FeatureMismatch = TRUE;
          KtiDebugPrintFatal ((host, KTI_DEBUG_ERROR, "\n Legacy PCH target NID mismatch between SBSP (CPU%u) and CPU%u", sbspSktId, Parent.Node.SocId));
        }

        if (FeatureMismatch == TRUE) {
          KtiDebugPrintFatal ((host, KTI_DEBUG_ERROR, "\n System Configuration not supported!!! "));
          KTI_ASSERT (FALSE, ERR_UNSUPPORTED_TOPOLOGY, MINOR_ERR_INVALID_TOPOLOGY);
        }
      }
      //
      // Parse the LEP of the discovered socket.
      //
      KtiCheckPoint ((UINT8) (1 << Parent.Node.SocId), (UINT8) Parent.Node.SocType, 0, STS_SETUP_MINIMUM_PATH, MINOR_STS_COLLECT_LEP, host);
      ParseCpuLep (host, SocketData, KtiInternalGlobal, (UINT8) Parent.Node.SocId);
    } else if (Parent.Node.SocType == SOCKET_TYPE_FPGA) {
      //
      // Parse the LEP of the discovered FPGA.
      //
      KtiCheckPoint ((UINT8) (1 << Parent.Node.SocId), (UINT8) Parent.Node.SocType, 0, STS_SETUP_MINIMUM_PATH, MINOR_STS_COLLECT_LEP, host);
      ParseFpgaLep (host, SocketData, KtiInternalGlobal, (UINT8) Parent.Node.SocId);

    } else {
      KTI_ASSERT(FALSE, ERR_SW_CHECK, MINOR_ERR_SW_CHECK_6D);
    }

    //
    // Make sure the parent has checked-in. NonSticky Scratch Register 02 in SBSP, is used by each AP sockets to indicate
    // that they have successfully checked-in. If it hasn't checked in yet, handle error.
    //
    if ((Parent.Node.SocId != sbspSktId) && (Parent.Node.SocType == SOCKET_TYPE_CPU)) { // SBSP has checked in, otherwise we won't be here
      KtiCheckPoint ((UINT8) (1 << Parent.Node.SocId), (UINT8) Parent.Node.SocType, 0, STS_SETUP_MINIMUM_PATH, MINOR_STS_CHK_PBSP_CHKIN, host);

      //
      // The AP that is directly connected to Legacy CPU must have checked-in.
      //
      if (CheckCpuConnectedToLegCpu (host, SocketData, KtiInternalGlobal, (UINT8) Parent.Node.SocId) == TRUE) {
        CheckInFlag = FALSE;
        OemWaitTimeForPSBP(host, &WaitTime);
        for (Ctr = 0; Ctr < WaitTime; Ctr++) {
          Data32 = KtiReadPciCfg (host, (UINT8)Parent.Node.SocId, 0, BIOSNONSTICKYSCRATCHPAD2_UBOX_MISC_REG);
          if ((Data32 & 0x1) != 0) {            //check bit 0 to indicate AP check in
            CheckInFlag = TRUE;
            break;
          }
          KtiFixedDelay(host, 1000); // 1000 microsecs
        }
        if (CheckInFlag == FALSE) {
          KtiCheckPoint ((UINT8) (1 << Parent.Node.SocId), (UINT8) Parent.Node.SocType, 0, ERR_BOOT_MODE, MINOR_ERR_PBSP_CHKIN_FAILURE,  host);
          KtiDebugPrintFatal ((host, KTI_DEBUG_ERROR, "\n\n AP Socket %u is discovered but hasn't come out of reset. System Halted!!!", Parent.Node.SocId));
          KTI_ASSERT (FALSE, ERR_BOOT_MODE, MINOR_ERR_PBSP_CHKIN_FAILURE);
        }
      } else {
        //
        // Setup the boot path for the parent which is not directly connected to Legacy CPU.
        //
        KtiCheckPoint ((UINT8) (1 << Parent.Node.SocId), (UINT8) Parent.Node.SocType, 0, STS_SETUP_MINIMUM_PATH, MINOR_STS_SET_PBSP_BOOT_PATH, host);
        GetParentSocket (host, SocketData->SbspMinPathTree, &Parent, &GrandParent);

        Port = GetChildLocalPortFromLEP (host, SocketData->Cpu, &GrandParent, &Parent);
        if (Port == 0xFF) {
          KtiCheckPoint ((UINT8) (1 << Parent.Node.SocId), (UINT8) Parent.Node.SocType, 0, ERR_MINIMUM_PATH_SETUP, MINOR_ERR_INVALID_PORT_CONNECTION, host);
          KtiDebugPrintFatal ((host, KTI_DEBUG_ERROR, "\n Invalid Port connection between CPU%u and CPU%u. ", GrandParent.Node.SocId, Parent.Node.SocId));
          KTI_ASSERT (FALSE, ERR_MINIMUM_PATH_SETUP, MINOR_ERR_INVALID_PORT_CONNECTION);
        }

        if ((Parent.Node.SocType == SOCKET_TYPE_CPU) && (Port < MAX_KTI_PORTS)) {
          SetupRemoteCpuBootPath (host, SocketData, KtiInternalGlobal, &GrandParent, &Parent, Port);
        }
      }
    }


    Child.RawNode     = 0;
    Child.Node.Valid  = TRUE;
    Child.Node.Hop    = Parent.Node.Hop + 1; // One level down from parent
    Child.Node.ParentIndex = Index; // Remember who is the immediate parent of this child
    for (KtiLink = 0; KtiLink < host->var.common.KtiPortCnt; KtiLink++) {
      if (SocketData->Cpu[Parent.Node.SocId].LinkData[KtiLink].Valid == TRUE) {
        //
        // Get the Child Socket Id, Type and port that connects it to the parent
        //
        Child.Node.SocId      = SocketData->Cpu[Parent.Node.SocId].LinkData[KtiLink].PeerSocId;
        Child.Node.SocType    = SocketData->Cpu[Parent.Node.SocId].LinkData[KtiLink].PeerSocType;
        Child.Node.ParentPort = SocketData->Cpu[Parent.Node.SocId].LinkData[KtiLink].PeerPort;

        //
        // Make sure that this socket is not already found in the tree.
        //
        if (NodeFoundInMinPathTree (host, SocketData->SbspMinPathTree, &Child) == TRUE) {
          continue;
        }

        //
        // Add the node to the tree.
        //
        KtiDebugPrintInfo1 ((host, KTI_DEBUG_INFO1, "\n\n Adding %s%u to the tree", (Child.Node.SocType == SOCKET_TYPE_FPGA) ? "PGA":"CPU", Child.Node.SocId));
        KtiCheckPoint ((UINT8) (1 << Child.Node.SocId), (UINT8) Child.Node.SocType, 0, STS_SETUP_MINIMUM_PATH, MINOR_STS_ADD_SOCKET_TO_MIN_PATH_TREE, host);
        if ((Status = AddNodeToTopologyTree (host, SocketData->SbspMinPathTree, &Child)) != KTI_SUCCESS) {
          KtiCheckPoint ((UINT8) (1 << Child.Node.SocId), (UINT8) Child.Node.SocType, 0, ERR_MINIMUM_PATH_SETUP, MINOR_ERR_INTERNAL_DATA_STRUCTURE, host);
          KtiDebugPrintFatal ((host, KTI_DEBUG_ERROR, "\n Adding %s%u to Minimum Path Tree failed - Data Structure Overflow.", (Child.Node.SocType == SOCKET_TYPE_FPGA) ? "PGA":"CPU", Child.Node.SocId));
          KTI_ASSERT (FALSE, ERR_MINIMUM_PATH_SETUP, MINOR_ERR_INTERNAL_DATA_STRUCTURE);
        }
        //
        // Add the node to the socketdata
        //
        if ((Child.Node.SocType == SOCKET_TYPE_CPU) || (Child.Node.SocType == SOCKET_TYPE_FPGA)) {
          SocketData->Cpu[Child.Node.SocId].Valid = TRUE;
          SocketData->Cpu[Child.Node.SocId].SocId = (UINT8) Child.Node.SocId;
          SocketData->Cpu[Child.Node.SocId].SocType = (UINT8)Child.Node.SocType;
        } else {
          KTI_ASSERT (FALSE, ERR_UNSUPPORTED_TOPOLOGY, MINOR_ERR_INVALID_TOPOLOGY);
        }

        if ((Child.Node.SocType == SOCKET_TYPE_CPU) || (SocketFpgasInitialized (host, KtiInternalGlobal))) {

          //
          // Setup path from Sbsp to the new found node so that the Sbsp can access the new found node's CSRs.
          //
          KtiCheckPoint ((UINT8) (1 << Child.Node.SocId), (UINT8) Child.Node.SocType, 0, STS_SETUP_MINIMUM_PATH, MINOR_STS_SET_SBSP_CONFIG_PATH, host);
          SetupSbspConfigAccessPath (host, SocketData, KtiInternalGlobal, &Parent, &Child, KtiLink);
        } else {
          KtiDebugPrintInfo1 ((host, KTI_DEBUG_INFO1, "\n"));
        }
      }
    } // for(KtiLink...)
  } // for(Index...)

  //
  // Print the Minimum Path Tree
  //
  KtiDebugPrintInfo1 ((host, KTI_DEBUG_INFO1, "\n\nSBSP Minimum Path Tree"));
  KtiDebugPrintInfo1 ((host, KTI_DEBUG_INFO1, "\n----------------------"));
  KtiDebugPrintInfo1 ((host, KTI_DEBUG_INFO1, "\nIndex  Socket  ParentPort  Hop  ParentIndex"));
  for (Index = 0; SocketData->SbspMinPathTree[Index].Node.Valid != FALSE; Index++) {
    if (SocketData->SbspMinPathTree[Index].Node.SocType ==  SOCKET_TYPE_CPU) {
      if (SocketData->SbspMinPathTree[Index].Node.SocId == sbspSktId) {
        KtiDebugPrintInfo1 ((host, KTI_DEBUG_INFO1, "\n %02u     CPU%u    --         %1u     --", Index,
          SocketData->SbspMinPathTree[Index].Node.SocId, 0));
      } else {
        KtiDebugPrintInfo1 ((host, KTI_DEBUG_INFO1, "\n %02u     CPU%u    %02u         %1u     %02u", Index,
          SocketData->SbspMinPathTree[Index].Node.SocId, SocketData->SbspMinPathTree[Index].Node.ParentPort,
          SocketData->SbspMinPathTree[Index].Node.Hop, SocketData->SbspMinPathTree[Index].Node.ParentIndex));
      }
    }
  }

  return KTI_SUCCESS;
}

/**

  Routine to setup the config access path from SBSP to the given socket.

  @param host  - Pointer to the system host (root) structure
  @param SocketData - Pointer to socket specific data
  @param KtiInternalGlobal - KTIRC Internal Global data
  @param Interm     - Immediate parent of the socket in SBSP minimum tree for which the config
                      access path needs to be set from SBSP
  @param Dest       - socket for which the config access path needs to be set from SBSP
  @param DestPort   - Port of the given socket's immediate parent that connects them.

  @retval KTI_SUCCESS

**/
KTI_STATUS
SetupSbspConfigAccessPath (
  struct sysHost            *host,
  KTI_SOCKET_DATA           *SocketData,
  KTI_HOST_INTERNAL_GLOBAL  *KtiInternalGlobal,
  TOPOLOGY_TREE_NODE        *Interm,
  TOPOLOGY_TREE_NODE        *Dest,
  UINT8                     DestPort
  )
{
  TOPOLOGY_TREE_NODE  Parent;
  TOPOLOGY_TREE_NODE  Child;
  UINT8               cLocalPort;
  UINT8               pLocalPort = 0, pLocalPort1;
  UINT8               Ctr;
  UINT32              Data32;
  UINT8               sbspSktId;
  UINT8               PortFieldWidth, ChaPortFieldWidth;

  sbspSktId = KtiInternalGlobal->SbspSoc;

  PortFieldWidth = 2; // Two bit field to encode link 0, 1 & 2
  ChaPortFieldWidth = 2; // Two bit field to encode link 0, 1 & 2

  KtiDebugPrintInfo1 ((host, KTI_DEBUG_INFO1, "\n   Setting path between SBSP and %s%u. ", (Dest->Node.SocType == SOCKET_TYPE_FPGA) ? "PGA":"CPU", Dest->Node.SocId));

  //
  // Set the path to "Dest" in all the sockets that exist between SBSP and "Interm"
  //
  Parent      = *Interm;
  Child       = *Dest;
  cLocalPort  = DestPort;
  pLocalPort1 = (UINT8) SocketData->Cpu[Interm->Node.SocId].LinkData[cLocalPort].PeerPort;

  while (1) {
    if (Parent.Node.SocId == sbspSktId && Parent.Node.SocType == SOCKET_TYPE_CPU) {
      //
      // In Sbsp, setup Cbo/M2PCIe route table for "Dest" socket; no need to setup HA route table
      // since at this point only Config transactions are active in the system.
      //
      KtiDebugPrintInfo1 ((host, KTI_DEBUG_INFO1, "\n   In SBSP setting route to %s%u using port %u.\n", (Dest->Node.SocType == SOCKET_TYPE_FPGA) ? "PGA":"CPU", Dest->Node.SocId, cLocalPort));

      //
      // Cha RT in SBSP
      //
      for (Ctr = 0; Ctr < KtiInternalGlobal->TotCha[Parent.Node.SocId]; Ctr++) {
        Data32 = KtiReadPciCfg (host, (UINT8) Parent.Node.SocId, Ctr, KTI_ROUTING_TABLE_CHA_PMA_REG);
        Data32 = Data32 & ~FLD_MASK(ChaPortFieldWidth, (ChaPortFieldWidth * Dest->Node.SocId));
        Data32 = Data32 | (cLocalPort << (ChaPortFieldWidth * Dest->Node.SocId));
        KtiWritePciCfg (host, (UINT8) Parent.Node.SocId, Ctr, KTI_ROUTING_TABLE_CHA_PMA_REG, Data32);
      }

      // M2UPCIe1 RT in SBSP socket (Ubox is behind M2PCIe1)
      Data32  = KtiReadPciCfg (host, (UINT8) Parent.Node.SocId, 1, QPI_RT_M2UPCIE_MAIN_REG);
      Data32 = Data32 & ~(3 << (PortFieldWidth * Dest->Node.SocId));
      Data32  = Data32 | (cLocalPort << (PortFieldWidth * Dest->Node.SocId));
      KtiWritePciCfg (host, (UINT8) Parent.Node.SocId, 1, QPI_RT_M2UPCIE_MAIN_REG, Data32);

      //
      // Only program return route in CPU, FPGA only has one port, and does not have ability to program a route
      //
      if (Dest->Node.SocType == SOCKET_TYPE_CPU) {
        // M2PCIe1 RT in "Dest" socket  (Ubox is behind M2PCIe1)
        KtiDebugPrintInfo1 ((host, KTI_DEBUG_INFO1, "\n   In CPU%u using port %u to set the M2UPCIe0 route.\n", Dest->Node.SocId, pLocalPort1));

        if ((host->var.common.cpuType == CPU_SKX) && (host->var.common.stepping < B0_REV_SKX)) {
          // s5332653
          // These values were written via a pcode PIF (ref: s5352242).
          // The PIF also updated the M2pcie routing table mode from dynamic to static (for static routing M2p relies on the programming in the routing table.
          // The static mode setting was a w/a for these configs due to a bug in M2pcie)
          // BIOS was able to w/a this issue by doing just a write instead of a RMW  It is ok for this moment, there are no traffics bebween PBSPs.
          // It would lose the initial value in the register but this allowed the response to make it back correctly to the issuing socket.
          // Eventually, BIOS came back to program the routing table to the correct value based on the topology.
          //
          Data32 = (pLocalPort1 << (PortFieldWidth * Parent.Node.SocId));
        } else {
          Data32 = KtiReadPciCfg (host, (UINT8) Dest->Node.SocId, 1, QPI_RT_M2UPCIE_MAIN_REG);
          Data32 = Data32 & ~(3 << (PortFieldWidth * Parent.Node.SocId));
          Data32 = Data32 | (pLocalPort1 << (PortFieldWidth * Parent.Node.SocId));
        }
        KtiWritePciCfg (host, (UINT8) Dest->Node.SocId, 1, QPI_RT_M2UPCIE_MAIN_REG, Data32);
      }
      break;
    } else if (Parent.Node.SocType == SOCKET_TYPE_CPU) {
      //
      // Setup the path in intermediate CPU socket found between Sbsp & Dest
      //
      pLocalPort = (UINT8) Parent.Node.ParentPort;
      KtiDebugPrintInfo1 ((host, KTI_DEBUG_INFO1, "\n    In CPU%u connecting port %u to %u.\n", Parent.Node.SocId, pLocalPort, cLocalPort));
      KTI_ASSERT((pLocalPort != cLocalPort), ERR_SW_CHECK, MINOR_ERR_SW_CHECK_6F);

      //
      // Forward Path in KTI Agent
      //
      Data32 = KtiReadPciCfg (host, (UINT8) Parent.Node.SocId, pLocalPort, KTIQRT_KTI_LLPMON_REG);
      Data32 &= (~(3 << (Dest->Node.SocId * 2)));
      Data32 |= (cLocalPort << (Dest->Node.SocId * 2));
      KtiWritePciCfg (host, (UINT8) Parent.Node.SocId, pLocalPort, KTIQRT_KTI_LLPMON_REG, Data32);

      //
      // Backward Path in KTI Agent
      //
      Data32 = KtiReadPciCfg (host, (UINT8) Parent.Node.SocId, cLocalPort, KTIQRT_KTI_LLPMON_REG);
      Data32 &= (~(3 << (sbspSktId * 2)));
      Data32 |= (pLocalPort << (sbspSktId * 2));
      KtiWritePciCfg (host, (UINT8) Parent.Node.SocId, cLocalPort, KTIQRT_KTI_LLPMON_REG, Data32);

      // Forward Path in M3KTI
      ProgramM3KtiRoute (host, pLocalPort, (UINT8) Parent.Node.SocId, (UINT8) Dest->Node.SocId, cLocalPort);

      // Backward Path in M3KTI
      ProgramM3KtiRoute (host, cLocalPort, (UINT8) Parent.Node.SocId, (UINT8) sbspSktId, pLocalPort);

    } else {
      KtiCheckPoint ((UINT8) (1 << Parent.Node.SocId), (UINT8) Parent.Node.SocType, 0, ERR_UNSUPPORTED_TOPOLOGY, MINOR_ERR_INVALID_SOCKET_TYPE, host);
      KtiDebugPrintFatal ((host, KTI_DEBUG_ERROR, "\n Unknown Intermediate Socket Type found. Topology not supported"));
      KTI_ASSERT (FALSE, ERR_UNSUPPORTED_TOPOLOGY, MINOR_ERR_INVALID_TOPOLOGY);
    }
    //
    // Go up one step towards the Sbsp & repeat the loop
    //
    Child = Parent;
    GetParentSocket (host, SocketData->SbspMinPathTree, &Child, &Parent);
    if (Parent.Node.SocType == SOCKET_TYPE_CPU) {
      cLocalPort = (UINT8) SocketData->Cpu[Child.Node.SocId].LinkData[pLocalPort].PeerPort;
    } else {
      KTI_ASSERT(FALSE, ERR_SW_CHECK, MINOR_ERR_SW_CHECK_70);
    }
  }

  KtiDebugPrintInfo1 ((host, KTI_DEBUG_INFO1, "\n"));
  return KTI_SUCCESS;
}

/**

  Routine to setup the boot path from Remote CPUs (i.e >1 hop away) to Legacy CPU.

  @param host  - Pointer to the system host (root) structure
  @param SocketData - Pointer to socket specific data
  @param KtiInternalGlobal - KTIRC Internal Global data
  @param Interm     - Immediate parent of the "Dest" in SBSP minimum tree
  @param Dest       - Remote CPU socket for which the boot path needs to be set
  @param DestPort   - Port of Remote CPU socket's immediate parent that connects them.

  @retval 0 - Successful
  @retval Other - failure

**/
KTI_STATUS
SetupRemoteCpuBootPath (
  struct sysHost            *host,
  KTI_SOCKET_DATA           *SocketData,
  KTI_HOST_INTERNAL_GLOBAL  *KtiInternalGlobal,
  TOPOLOGY_TREE_NODE        *Interm,
  TOPOLOGY_TREE_NODE        *Dest,
  UINT8                     DestPort
  )
{
  TOPOLOGY_TREE_NODE  Parent;
  TOPOLOGY_TREE_NODE  Child;
  UINT8               cLocalPort;
  UINT8               pLocalPort = 0;
  UINT8               Ctr;
  UINT32              Data32;
  UINT8               sbspSktId;
  UINT8               ChaPortFieldWidth;

  sbspSktId = KtiInternalGlobal->SbspSoc;
  ChaPortFieldWidth = 2; // Two bit field to encode link 0, 1 & 2

  KtiDebugPrintInfo1 ((host, KTI_DEBUG_INFO1, "\n Setting boot path for CPU%u ", Dest->Node.SocId));

  //
  // Set the boot path for Remote CPU
  //
  Parent      = *Interm;
  Child       = *Dest;
  cLocalPort  = DestPort;
  while (1) {
    if (Parent.Node.SocId == sbspSktId && Parent.Node.SocType == SOCKET_TYPE_CPU) {
      //
      // In Remote CPU, set route to Legacy CPU
      //
      pLocalPort = (UINT8) SocketData->Cpu[Interm->Node.SocId].LinkData[DestPort].PeerPort;
      KtiDebugPrintInfo1 ((host, KTI_DEBUG_INFO1, "\n    In CPU%u setting Cbo route to port %u\n", Dest->Node.SocId, pLocalPort));
      for (Ctr = 0; Ctr < KtiInternalGlobal->TotCha[Dest->Node.SocId]; Ctr++) {
        Data32 = KtiReadPciCfg (host, (UINT8) Dest->Node.SocId, Ctr, KTI_ROUTING_TABLE_CHA_PMA_REG);
        Data32 = Data32 & ~FLD_MASK(ChaPortFieldWidth, (ChaPortFieldWidth * KtiInternalGlobal->SbspSoc));
        Data32 = Data32 | (pLocalPort << (ChaPortFieldWidth * KtiInternalGlobal->SbspSoc));
        KtiWritePciCfg (host, (UINT8) Dest->Node.SocId, Ctr, KTI_ROUTING_TABLE_CHA_PMA_REG, Data32);
      }
      break;
    } else if (Parent.Node.SocType == SOCKET_TYPE_CPU) {
      //
      // Setup the pass-thru path in intermediate CPU socket
      //
      pLocalPort = (UINT8) Parent.Node.ParentPort;
      KtiDebugPrintInfo1 ((host, KTI_DEBUG_INFO1, "\n    In CPU%u connecting port %u to %u. \n", Parent.Node.SocId, pLocalPort, cLocalPort));
      KTI_ASSERT((cLocalPort != pLocalPort), ERR_SW_CHECK, MINOR_ERR_SW_CHECK_71);

      //
      // Forward Path in KTI Agent
      //
      Data32 = KtiReadPciCfg (host, (UINT8) Parent.Node.SocId, pLocalPort, KTIQRT_KTI_LLPMON_REG);
      Data32 &= (~(3 << (Dest->Node.SocId * 2)));
      Data32 |= (cLocalPort << (Dest->Node.SocId * 2));
      KtiWritePciCfg (host, (UINT8) Parent.Node.SocId, pLocalPort, KTIQRT_KTI_LLPMON_REG, Data32);

      //
      // Backward Path in KTI Agent
      //
      Data32 = KtiReadPciCfg (host, (UINT8) Parent.Node.SocId, cLocalPort, KTIQRT_KTI_LLPMON_REG);
      Data32 &= (~(3 << (sbspSktId * 2)));
      Data32 |= (pLocalPort << (sbspSktId * 2));
      KtiWritePciCfg (host, (UINT8) Parent.Node.SocId, cLocalPort, KTIQRT_KTI_LLPMON_REG, Data32);

      // Forward Path in M3KTI
      ProgramM3KtiRoute (host, pLocalPort, (UINT8) Parent.Node.SocId, (UINT8) Dest->Node.SocId, cLocalPort);

      // Backward Path in M3KTI
      ProgramM3KtiRoute (host, cLocalPort, (UINT8) Parent.Node.SocId, (UINT8) sbspSktId, pLocalPort);
    } else {
      KtiCheckPoint ((UINT8) (1 << Parent.Node.SocId), (UINT8) Parent.Node.SocType, 0, ERR_UNSUPPORTED_TOPOLOGY, MINOR_ERR_INVALID_SOCKET_TYPE, host);
      KtiDebugPrintFatal ((host, KTI_DEBUG_ERROR, "\n Unknown Intermediate Socket Type found. Topology not supported"));
      KTI_ASSERT (FALSE, ERR_UNSUPPORTED_TOPOLOGY, MINOR_ERR_INVALID_SOCKET_TYPE);
    }
    //
    // Go up one step towards the Sbsp & repeat the loop
    //
    Child = Parent;
    GetParentSocket (host, SocketData->SbspMinPathTree, &Child, &Parent);
    if (Parent.Node.SocType == SOCKET_TYPE_CPU) {
      cLocalPort = (UINT8) SocketData->Cpu[Child.Node.SocId].LinkData[pLocalPort].PeerPort;
    } else {
      KTI_ASSERT(FALSE, ERR_SW_CHECK, MINOR_ERR_SW_CHECK_72);
    }
  }

  KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\n"));

  return KTI_SUCCESS;
}

/**

    Program M3KTI routing table.

    @param host        - pointer to the system host root structure
    @param InPort      - source port
    @param InterSocket - intermediate socket ID
    @param DestSocId   - destination socket ID
    @param OutPort     - endpoint port

    @retval KTI_SUCCESS - on successful completion

**/
KTI_STATUS
ProgramM3KtiRoute (
  struct sysHost            *host,
  UINT8                     InPort,
  UINT8                     InterSocket,
  UINT8                     DestSocId,
  UINT8                     OutPort
  )
{

  UINT8  M3Kti;
  UINT32 Data32;

  M3Kti = PORT_TO_M3KTI(InPort);

  Data32 = KtiReadPciCfg (host, (UINT8) InterSocket, M3Kti, M3KKRT_M3KTI_MAIN_REG);
  ConvertM3KTIRoutingTable (InPort, OutPort, DestSocId, &Data32);
  KtiWritePciCfg (host, (UINT8) InterSocket, M3Kti, M3KKRT_M3KTI_MAIN_REG, Data32);

  return KTI_SUCCESS;
}

/**

    Update Kti Internal Global data.

    @param host              - pointer to the system host root structure
    @param KtiInternalGlobal - pointer to the KTI RC internal global structure
    @param SocktId           - Socket ID

    @retval KTI_SUCCESS - on successful completion

**/
KTI_STATUS
UpdateCpuInfoInKtiInternal(
 struct sysHost             *host,
 KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal,
 UINT8                      SocktId
 )
{
  // Socket Ways
  GetSocketWays (host, SocktId, &KtiInternalGlobal->SocketWays[SocktId]);

  // Get the number of and list of Chas present
  GetChaCountAndList (host, SocktId, &KtiInternalGlobal->TotCha[SocktId], &KtiInternalGlobal->ChaList[SocktId]);

  GetIioCount (host, SocktId, &KtiInternalGlobal->TotIio[SocktId]);

  // Get the number of M2Mem present
  GetM2MemCount (host, SocktId, &KtiInternalGlobal->TotM2Mem[SocktId]);

  // Get the number of M2Pcie Link present
  GetM2PcieCount (host, SocktId, KtiInternalGlobal);

  // Get the number of M3KTI present
  GetM3KTICount (host, SocktId, &(host->var.kti.CpuInfo[SocktId].TotM3Kti));

  // Get the number of KtiAgents present
  GetKtiAgentCount (host, SocktId, &KtiInternalGlobal->TotKtiAgent[SocktId]);

  // Get the Ras capability
  GetRasType (host, SocktId, KtiInternalGlobal);

  // Only if all parts have Advanced Ras features do we have online/offline capability
  if ((host->var.kti.OutSystemRasType == ADVANCED_RAS) && (host->setup.kti.KtiCpuSktHotPlugEn == TRUE)){
    KtiInternalGlobal->KtiCpuSktHotPlugEn = TRUE;
  } else {
    KtiInternalGlobal->KtiCpuSktHotPlugEn = FALSE;
  }

  // Collect the CPU type based on presence of MCP stack(s)
  GetCpuType (host, KtiInternalGlobal, SocktId);

  return KTI_SUCCESS;

}

/**

    Check for matching number of CHAs, MCP Links, M3KTIs and matching SKUs
    between SBSP and Socket

    @param host              - pointer to the system host root structure
    @param KtiInternalGlobal - pointer to the KTI RC internal global structure
    @param SbspSktId         - SBSP's socket ID
    @param SocketId          - Socket ID to compare SBSP to

    @retval 0     - check pass
    @retval other - check failed

**/
BOOLEAN
CheckThisSocketInfoWithSbsp(
 struct sysHost             *host,
 KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal,
 UINT8                      SbspSktId,
 UINT8                      SocketId
 )
{
  BOOLEAN  FeatureMismatch = FALSE;
  BOOLEAN  McpPresent = FALSE;

  //
  // Check for MCP
  //
  if ((host->var.kti.CpuInfo[SocketId].CpuType == CPU_TYPE_F) || (host->var.kti.CpuInfo[SbspSktId].CpuType == CPU_TYPE_F) ||
      (host->var.kti.CpuInfo[SocketId].CpuType == CPU_TYPE_P) || (host->var.kti.CpuInfo[SbspSktId].CpuType == CPU_TYPE_P)) {
    McpPresent = TRUE;
  }

  if ((KtiInternalGlobal->TotCha[SocketId] != KtiInternalGlobal->TotCha[SbspSktId])) {
    KtiCheckPoint ((UINT8) (1 << SocketId), 0, 0, ERR_UNSUPPORTED_TOPOLOGY, MINOR_ERR_CBO_COUNT_MISMATCH, host);
    FeatureMismatch = TRUE;
    KtiDebugPrintFatal ((host, KTI_DEBUG_ERROR, "\n Cbo Count/List mismatch between SBSP (CPU%u) and CPU%u", SbspSktId, SocketId));
  }

  //
  // Ignore m3kti mismatch if MCP present.  SKX allows MCP Skus to be mixed w/ non-MCP
  //
  if (McpPresent == FALSE) {
    if (host->var.kti.CpuInfo[SocketId].TotM3Kti != host->var.kti.CpuInfo[SbspSktId].TotM3Kti) {
      KtiCheckPoint ((UINT8) (1 << SocketId), 0, 0, ERR_UNSUPPORTED_TOPOLOGY, MINOR_ERR_M3KTI_COUNT_MISMATCH, host);
      FeatureMismatch = TRUE;
      KtiDebugPrintFatal ((host, KTI_DEBUG_ERROR, "\n M3KTI Count mismatch between SBSP (CPU%u) and CPU%u", SbspSktId, SocketId));
    }
  }

  return FeatureMismatch;
 }


/**

   Routine to check if the system has supported topology. Following terminology is used in
   describing the system topology:
   1S    - Only socket 0 is populated
   2S1L -Socket 0 & 1 are populated; there is only one link between them. Link number doesn't matter.
   2S2L -Socket 0 & 1 are populated; there are two links between them. (Link 0 & 1 only)
   4S -Total number of populated sockets is <= 4, populated sockets have ID < 4, and it is not a 1S/2S system.
   8S -All other configurations.

Note:
  When degraded topology is detected (i.e one or more links failed), BIOS will try to reduce it to one of the
  supported topology by forcing certain links and/or sockets out. Link/Socket removal is done by marking the
  appropriate data structures; nothing specifically done to the h/w that will disable a link or socket. That will
  take effect only after next reset. We track the number of and list of sockets populated before and after
  topology reduction is applied. While KTIRC uses only the reduced topology information, other modules
  such as the code that issues Warm reset needs to be aware of what is the list of sockets populated
  currently. So we update the host numCpus/socketPresentBitMap structure with the current number of and
  list of sockets populated, not based on the reduced topology.

  @param host  - Pointer to the system host (root) structure
  @param SocketData - Pointer to socket specific data
  @param KtiInternalGlobal - KTIRC Internal Global data

  @retval 0 - Successful
  @retval Other - failure

**/
KTI_STATUS
PreProcessKtiLinks (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal
  )
{
  UINT8   Idx, ActivelinkOnCpu0, ActivelinkOnCpu1, Port, Socket;
  UINT8   TempTotCpu;
  UINT32  TempCpuList;
#if KTI_DEBUG_LEVEL
  UINT8   Topology[SYS_CONFIG_MAX][6] = {{"1S"}, {"2S1L"}, {"2S2L"}, {"2S3L"}, {"4S"}, {"8S"}};
#endif
  UINT32  BoardTypeBitmask;
  UINT32  OriginalTopologyBitmask = GetTopologyBitmap(host, SocketData, KtiInternalGlobal);
  UINT32  FinalTopologyBitmask;

  KtiCheckPoint(0xFF, 0xFF, 0xFF, STS_TOPOLOGY_DISCOVERY, MINOR_STS_CHK_TOPOLOGY, host);

  // Sanity check the LEP info
  SanityCheckLep (host, SocketData->Cpu);
  LepDump (host, SocketData->Cpu);

  //
  // Identify the number of and list of sockets populated.
  //
  TempTotCpu  = 0;
  TempCpuList = 0;
  for (Idx = 0; Idx < MAX_SOCKET; Idx++) {
    if ((SocketData->Cpu[Idx].Valid == TRUE) && (SocketData->Cpu[Idx].SocType == SOCKET_TYPE_CPU)) {
      TempTotCpu++;
      TempCpuList = TempCpuList | (1 << Idx);
    }
  }
  KtiInternalGlobal->OrigTotCpu = TempTotCpu;
  KtiInternalGlobal->OrigCpuList = TempCpuList;

  host->var.kti.SysConfig = SYS_CONFIG_MAX;
  if (TempTotCpu == 1) { // 1S system
    host->var.kti.SysConfig = SYS_CONFIG_1S;
  } else if ((TempTotCpu == 2) && (TempCpuList == 0x3)) { // 2S system

    ActivelinkOnCpu0 = ActivelinkOnCpu1 = 0;
    //check how many links actived
    for (Port = 0; Port < host->var.common.KtiPortCnt; Port++) {
      if ((SocketData->Cpu[0].LinkData[Port].Valid == TRUE) && (SocketData->Cpu[0].LinkData[Port].PeerSocType == SOCKET_TYPE_CPU)) {
         ActivelinkOnCpu0 += 1;
      }
      if ((SocketData->Cpu[1].LinkData[Port].Valid == TRUE) && (SocketData->Cpu[1].LinkData[Port].PeerSocType == SOCKET_TYPE_CPU)) {
         ActivelinkOnCpu1 += 1;
      }
    }

    //
    // 2S3L is special case. A system will be declared as 2S only when it has sockets 0 & 1 populated, 3 KTI links are connecting
    // the 2 sockets and the 2 sockets are connected by KTI links 0 & 1, 2
    //
    if (ActivelinkOnCpu0 == 3 && ActivelinkOnCpu1 == 3) {          // mark link 1, 2 as 2S3Link
      for (Socket = 0; Socket < 2; Socket++) {
        for (Port = 1; Port < ActivelinkOnCpu1; Port++) {
          SocketData->Cpu[Socket].LinkData[Port].TwoSkt3Link = TRUE;
        }
      }
      host->var.kti.SysConfig = SYS_CONFIG_2S3L;

    } else if (ActivelinkOnCpu0 == 2 && ActivelinkOnCpu1 == 2) {

      if ((host->var.common.KtiPortCnt == 3) &&
          (((SocketData->Cpu[0].LinkData[2].PeerSocType == SOCKET_TYPE_CPU) && (SocketData->Cpu[0].LinkData[2].Valid == TRUE)) ||
          ((SocketData->Cpu[1].LinkData[2].PeerSocType == SOCKET_TYPE_CPU) && (SocketData->Cpu[1].LinkData[2].Valid == TRUE)))) {
        //
        // Remove the dual links if any KTI links 2 is one of two active KTI links.
        // KTI links 2 is not allowed for 2S2L.
        //
        RemoveDualLink (host, SocketData);
        host->var.kti.SysConfig = SYS_CONFIG_2S1L;
      } else {
        //
        // 2S2L is special case. A system will be declared as 2S only when it has sockets 0 & 1 populated, 2 KTI links are connecting
        // the 2 sockets and the 2 sockets are connected by KTI links 0 & 1.
        // Check if the dual links are twisted and mark it accordingly.
        //
        for (Socket = 0; Socket < 2; Socket++) {
          SocketData->Cpu[Socket].LinkData[1].DualLink = TRUE;       // Mark link 1 as DualLink
        }
        host->var.kti.SysConfig = SYS_CONFIG_2S2L;
        if (SocketData->Cpu[0].LinkData[0].PeerPort != 0) {          // Port 0 <-> Port 0?
          KtiInternalGlobal->TwistedKtiLinks = TRUE;
        }
      }
    } else {
      //
      // Ensure that only one link connection exist between the 2 sockets it will be treated as 2S1L
      //
      RemoveDualLink (host, SocketData);
      host->var.kti.SysConfig = SYS_CONFIG_2S1L;
    }
  } else if ((TempTotCpu <= 4) && ((TempCpuList & 0xF0) == 0)) { // 4S system
    //
    // Check and remove the dual links if there is any. Also check if it has supported topology otherwise force
    // supported topology.
    //
    RemoveDualLink (host, SocketData);
    CheckForDegraded4S (host, SocketData, KtiInternalGlobal);
  } else { // 8S system
#if MAX_SOCKET > 4
    //
    // Check and remove the dual links if there is any. Also check if it has supported topology otherwise force
    // supported topology.
    //
    RemoveDualLink (host, SocketData);
    CheckForDegraded8S (host, SocketData, KtiInternalGlobal);
#else
    KtiDebugPrintFatal ((host, KTI_DEBUG_ERROR, "\n  Unsupported topology"));
    KTI_ASSERT(FALSE, ERR_UNSUPPORTED_TOPOLOGY, MINOR_ERR_INVALID_TOPOLOGY);
#endif
  }

  FinalTopologyBitmask = GetTopologyBitmap(host, SocketData, KtiInternalGlobal);
  if (OriginalTopologyBitmask != FinalTopologyBitmask) {
    KtiLogTopologyChange (host, OriginalTopologyBitmask, FinalTopologyBitmask, InvalidTopology);
  }

  //
  // If hot plug enabled, will program credits assuming it as 4S or 8S system;
  // so that the Credits for already populated sockets need not be reprogrammed during hot add/remove event.
  //
  if (KtiInternalGlobal->KtiCpuSktHotPlugEn == TRUE){
    host->var.kti.SysConfig = SYS_CONFIG_4S;      // force to 4S for future hot plug
    TempTotCpu =  MAX_SOCKET;
    if ((host->setup.kti.KtiCpuSktHotPlugTopology == 1) && (TempTotCpu == 8)) {
      host->var.kti.SysConfig = SYS_CONFIG_8S;   // force to 8S for future hot plug
    }
  }

  if(host->var.kti.SysConfig == SYS_CONFIG_8S){
    KtiInternalGlobal->IsRouteThrough = TRUE;
  }

  //
  // Sanity check the LEP info to make sure the degraded topology handling is done properly
  //
  SanityCheckLep (host, SocketData->Cpu);

  //
  // Check for feature Vs Topology Precedence
  //
  PreProcessFeatureVsTopology (host, SocketData, KtiInternalGlobal);

  PreProcessFpgaLinks (host, SocketData, KtiInternalGlobal);

  //
  // Update the global structure with the number of and list of sockets populated
  //
  for (Idx = 0; Idx < MAX_SOCKET; Idx++) {
    if ((SocketData->Cpu[Idx].Valid == TRUE) && (SocketData->Cpu[Idx].SocType == SOCKET_TYPE_CPU)) {
      KtiInternalGlobal->TotCpu++;
      KtiInternalGlobal->CpuList = KtiInternalGlobal->CpuList | (1 << Idx);
    }
  }
  if (KtiInternalGlobal->CpuList != KtiInternalGlobal->OrigCpuList) {
    LepDump (host, SocketData->Cpu);
  } else {
    KtiDebugPrintInfo1 ((host, KTI_DEBUG_INFO1, "\n  Already Reduced to Supported Topology"));
  }

  //
  // Check to make sure the system configuration is valid
  //
  KTI_ASSERT((host->var.kti.SysConfig < SYS_CONFIG_MAX), ERR_UNSUPPORTED_TOPOLOGY, MINOR_ERR_INVALID_TOPOLOGY);

  KtiDebugPrintInfo1 ((host, KTI_DEBUG_INFO1, "\n\n  System will be treated %s Configuration", Topology[host->var.kti.SysConfig]));

  // Copy the LEP Info and Topology Tree Info into host structure
  for (Idx = 0; Idx < MAX_SOCKET; Idx++) {
    if (SocketData->Cpu[Idx].Valid == TRUE) {
      host->var.kti.CpuInfo[Idx].Valid  = SocketData->Cpu[Idx].Valid;
      host->var.kti.CpuInfo[Idx].SocId  = SocketData->Cpu[Idx].SocId;
      host->var.kti.CpuInfo[Idx].SocType  = SocketData->Cpu[Idx].SocType;
      MemCopy ((UINT8 *) host->var.kti.CpuInfo[Idx].LepInfo, (UINT8 *) SocketData->Cpu[Idx].LinkData, sizeof (host->var.kti.CpuInfo[Idx].LepInfo));
    }
  }

  //
  // Check if all valid sockets are supported by the board
  //
  BoardTypeBitmask = OemGetBoardTypeBitmask (host);
  host->var.kti.OutBoardVsCpuConflict = 0;
  for (Idx = 0; Idx < MAX_SOCKET; Idx++) {
    if ((SocketData->Cpu[Idx].Valid == TRUE) && (SocketData->Cpu[Idx].SocType == SOCKET_TYPE_CPU)) {
      if ((BoardTypeBitmask & ((1 << host->var.kti.CpuInfo[Idx].CpuType) << (4 * Idx))) == 0) {
        host->var.kti.OutBoardVsCpuConflict |= (1 << Idx);
      }
    }
  }
  if (host->var.kti.OutBoardVsCpuConflict != 0) {
    KtiDebugPrintInfo1 ((host, KTI_DEBUG_WARN, "\n WARNING:  A board VS CPU conflict detected. Hiding MCP devices."));
    HideMcpDevices (host, SocketData);
  }

  host->var.kti.Is4SRing = KtiInternalGlobal->Is4SRing;

  return KTI_SUCCESS;
}

/**

   Routine to check fpga links and degrade if needed
   Fpga only allowed in 1/2S

  @param host  - Pointer to the system host (root) structure
  @param SocketData - Pointer to socket specific data
  @param KtiInternalGlobal - KTIRC Internal Global data

  @retval 0 - Successful
  @retval Other - failure

**/
KTI_STATUS
PreProcessFpgaLinks (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal
  )
{
  UINT8     Socket;
  UINT32    FpgaList = 0;

  //
  // Update the global structure with the number of and list of fpgas populated
  //

  for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
    if ((SocketData->Cpu[Socket].Valid == TRUE) && (SocketData->Cpu[Socket].SocType == SOCKET_TYPE_FPGA)) {
       FpgaList |= (1 << Socket);
    }
  }

  if ((host->var.kti.SysConfig >= SYS_CONFIG_4S) && (FpgaList != 0)) {
    //
    // Fpga not allowed in 4s or above, reduce to 1S
    // Todo:?Could reduce to 1 or 2s or disable the fpgas and let boot?
    // TODO: Add warning, leave fpga enabled in the 1S?
    //
    DegradeTo1S (host, SocketData, KtiInternalGlobal);
    host->var.kti.SysConfig = SYS_CONFIG_1S;
  }

  if (SocketFpgasInitialized (host, KtiInternalGlobal)) {
    KtiInternalGlobal->FpgaList = FpgaList;
  }
  if (FpgaList) {
    KtiInternalGlobal->ProgramCreditFpga = 1;
  }

  return KTI_SUCCESS;
}

/**

  Check if the system has degraded 4S topology. If yes, then force certain links and/or
  sockets out to make it a supported topology.

  The following topologies are supported (in order of preference):

      4S Ring Topology
      3S Ring Topology
      3S Chain Topology
      2S Single Link Topology
      1S Topology

   This routine should be called only when sockets with ID > 3 are invalid for the system.
   (So, in case of 8S system, this should be called only after sockets 4 to 7 has been
   marked as invalid already)

   Note that, in case of certain socket(s) forced out because it is not part of the topology,
   we mark the socket as invalid in SocketData structure. This will ensure that the rest of
   code that runs after this routine will be agnostic of the sockets being removed and it
   will not assign resources for the sockets that will not be part of the system after reset.

  @param host  - Pointer to the system host (root) structure
  @param SocketData - Pointer to socket specific data
  @param KtiInternalGlobal - KTIRC Internal Global data

  @retval 0 - Successful

**/
KTI_STATUS
CheckForDegraded4S (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal
  )
{
  UINT8   Idx1, Idx2;
  BOOLEAN Degraded;

  //
  // Check if it is 4S topology if not, it is a degraded 4S topology.
  //
  Degraded = FALSE;
  for (Idx1 = 0; Idx1 < 4; Idx1++) {
    if ((SocketData->Cpu[Idx1].Valid == TRUE) && (SocketData->Cpu[Idx1].SocType == SOCKET_TYPE_CPU)) {
      for (Idx2 = 0; Idx2 < host->var.common.KtiPortCnt; Idx2++) {
        if (SocketData->Cpu[Idx1].LinkData[Idx2].Valid != TRUE) {
          Degraded = TRUE;
          break;
        }
      }
      if (Degraded == TRUE) {
        break;
      }
    }
  }

  if (Degraded == FALSE) {
    host->var.kti.SysConfig = SYS_CONFIG_4S;
    if (host->var.common.KtiPortCnt == 2) {
      KtiInternalGlobal->Is4SRing = TRUE;
      KtiInternalGlobal->IsRouteThrough = TRUE;
    }
    return KTI_SUCCESS;
  }

  //
  // It is a degraded topology; reduce it to one of the supported topology.
  //
  KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\n\n  Degraded 4S topology is detected."));
  HandleDegraded4S (host, SocketData, KtiInternalGlobal, NULL);

  return KTI_SUCCESS;
}


/**

  Identifies what is the "best" topology that can be derived out of system with degraded topology.

  If each socket has connection to at least 2 different other sockets, 4S Ring topology is possible.
  If a fully connected 3S with legacy socket is possible then 3S Ring topology is possible.
  If legacy socket is connected to 2 different other sockets then 3S Chain topology is possible.
  If legacy socket is connected to 1 other socket then 2S Single link topology is possible.
  Otherwise it is a 1S topology.

  This function should be called only when the system has degraded 4S topology.

  @param host  - Pointer to the system host (root) structure
  @param SocketData - Pointer to socket specific data
  @param KtiInternalGlobal - KTIRC Internal Global data

  @retval KTI_SUCCESS

**/
KTI_STATUS
HandleDegraded4S (
  struct sysHost           *host,
  KTI_SOCKET_DATA          *SocketData,
  KTI_HOST_INTERNAL_GLOBAL *KtiInternalGlobal,
  KTI_DISC_DATA            *DiscEngData
  )
{
  UINT8   Idx1, Idx2, Idx3;
  UINT8   Soc1, Soc2;
  UINT8   Temp1, Temp2;
  UINT8   PeerSocList[MAX_SOCKET];
  UINT8   PeerSocCnt[MAX_SOCKET];
  UINT8   Ring3SList;
  UINT8   PeerSocId, PeerPort = 0;
  UINT8   RingNumber, RingNode, RingNodeList = 0;
  BOOLEAN Is4SRing, Is3SRing, Is3SChain, Is2S, Is1S;
  BOOLEAN IsDegradedFrom4S;

  //
  // if DiscEngData is NULL, this function is called from the 4S degradation path
  //
  if (DiscEngData == NULL) {
    //
    // Degrade 4S config to 4S-Ring or less
    //
    IsDegradedFrom4S = TRUE;
  } else {
    //
    // Degrade > 4S config to 4S-Ring or less
    //
    IsDegradedFrom4S = FALSE;
  }

  if (host->setup.kti.DfxParm.DfxSystemDegradeMode == DEGRADE_TOPOLOGY_AS_IS) {
    host->var.kti.SysConfig = SYS_CONFIG_4S;
    return KTI_SUCCESS;
  } else if (host->setup.kti.DfxParm.DfxSystemDegradeMode == DEGRADE_TOPOLOGY_1S) {
    DegradeTo1S (host, SocketData, KtiInternalGlobal);
    host->var.kti.SysConfig = SYS_CONFIG_1S;
    return KTI_SUCCESS;
  }

  Is4SRing = Is3SRing = Is3SChain = Is2S = Is1S = FALSE;
  Ring3SList = 0;

  //
  // Identify the peer socket list & count for each populated socket.
  //
  for (Idx1 = 0; Idx1 < MAX_SOCKET; Idx1++) {
    PeerSocList[Idx1] = 0;
    PeerSocCnt[Idx1] = 0;
    if ((SocketData->Cpu[Idx1].Valid == TRUE) && (SocketData->Cpu[Idx1].SocType == SOCKET_TYPE_CPU)) {
      for (Idx2 = 0; Idx2 < host->var.common.KtiPortCnt; Idx2++) {
        if ((SocketData->Cpu[Idx1].LinkData[Idx2].Valid == TRUE) && (SocketData->Cpu[Idx1].LinkData[Idx2].PeerSocType == SOCKET_TYPE_CPU)) {
          if ((PeerSocList[Idx1] & (1 << SocketData->Cpu[Idx1].LinkData[Idx2].PeerSocId)) == 0) {
            PeerSocList[Idx1] = PeerSocList[Idx1] |(UINT8)(1 << SocketData->Cpu[Idx1].LinkData[Idx2].PeerSocId);
            PeerSocCnt[Idx1]++;
          }
        }
      }
    }
  }

  //
  // If legacy socket is not connected to any other sockets then it is 1S;  Else if it is conneted to one another socket, it is 2S single link.
  //
  if (PeerSocCnt[KtiInternalGlobal->SbspSoc] == 0) {
    Is1S = TRUE;
    KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\n  Reducing it to 1S"));
  } else if (PeerSocCnt[KtiInternalGlobal->SbspSoc] == 1) {
    Is2S = TRUE;
    KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\n  Reducing it to 2S Single Link "));
  } else { // 4S or larger degradation
    //
    // Determine if a 4S-Ring is possible
    //
    if (IsDegradedFrom4S) {
      //
      // If each socket has 2 or more peers (i.e links) then it can be reduced to a 4S ring
      //
      Is4SRing = TRUE;
      for (Idx1 = 0; Idx1 < 4; Idx1++) {
        if (PeerSocCnt[Idx1] < 2) {
          Is4SRing = FALSE;
          break;
        }
      } // for each Socket
    } else { // Larger than 4S degradation to 4S-Ring or less
      //
      // If there is a ring with legacy socket then it can be reduced to 4S ring
      //
      Is4SRing = ((DiscEngData->RingInfo[KtiInternalGlobal->SbspSoc][0] == 1) ?  TRUE : FALSE);
    } // Larger than a 4S degradation

    //
    // Determine the degradation landing-zone
    //
    if (Is4SRing == TRUE) {
      KtiInternalGlobal->Is4SRing = TRUE;
      KtiInternalGlobal->IsRouteThrough = TRUE;
      KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\n  Reducing it to 4S Ring "));
    } else {
      //
      // It must be either 3S Ring or Chain. If there is a triangle involving the legacy socket then it is 3S Ring.
      // Otherwise it is 3S Chain.
      //

      //
      // Detect if there is any "triangle" pattern involving the legacy socket in the topology. To detect it,
      // first we need to check if legacy socket is connected to at least 2 immediate neighbor sockets.
      // And at least 2 of those immediate neighbors have a connection between them.
      //
      for (Idx2 = 0; Idx2 < MAX_SOCKET; Idx2++) {
        if (PeerSocList[KtiInternalGlobal->SbspSoc] & (1 << Idx2)) {
          for (Idx3 = 0; Idx3 < MAX_SOCKET; Idx3++) {
            if ((PeerSocList[Idx2] & (1 << Idx3)) && (PeerSocList[KtiInternalGlobal->SbspSoc] & (1 << Idx3))) {
              //
              // If Idx2's neighbor socket Idx3 is also a neighbor of KtiInternalGlobal->SbspSoc
              // then KtiInternalGlobal->SbspSoc, Idx2 and Idx3 form a triangle.
              //
              Is3SRing = TRUE;
              Ring3SList = (1 << KtiInternalGlobal->SbspSoc)  | (1 << Idx2) | (1 << Idx3);
              break;
            }
          }
          if (Is3SRing == TRUE) {
            break;
          }
        }
      }

      if (Is3SRing == TRUE) {
        KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\n  Reducing it to 3S Ring (socket list: 0x%02X)", Ring3SList));
      } else {
        Is3SChain = TRUE;
        KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\n  Reducing it to 3S Chain "));
      }
    } // Degradation landing-zone
  } // 4S or larger degradation

  //
  // Ensure only one Reduced Topology is selected.
  //
  Temp1 = (Is4SRing << 4) | (Is3SRing << 3) | (Is3SChain << 2) | (Is2S << 1) | Is1S;
  Temp2 = 0;
  for (Idx1 = 0; Idx1 < 5; Idx1++) {
    if (Temp1 & (1 << Idx1)) Temp2++;
  }
  if (Temp2 != 1) {
    KtiDebugPrintFatal ((host, KTI_DEBUG_ERROR, "\n  Reducing to supported topology failed"));
    KTI_ASSERT (FALSE, ERR_UNSUPPORTED_TOPOLOGY, MINOR_ERR_INVALID_TOPOLOGY);
  }

  //
  // Carry out the topology reduction by marking the appropriate socket(s) and/or link(s) as invalid
  //
  if (Is1S == TRUE) {
    //
    // Mark all sockets except legacy socket as invalid; also mark all the links as invalid
    //
    host->var.kti.SysConfig = SYS_CONFIG_1S;
    for (Idx1 = 0; Idx1 < MAX_SOCKET; Idx1++) {
      if (Idx1 != KtiInternalGlobal->SbspSoc) {
        SocketData->Cpu[Idx1].Valid = FALSE;
      }
      for (Idx2 = 0; Idx2 < host->var.common.KtiPortCnt; Idx2++) {
        SocketData->Cpu[Idx1].LinkData[Idx2].Valid = FALSE;
      }
    }
  } else if (Is2S == TRUE) {
    //
    // Retain only the two sockets and the link between them
    //

    // Identify the peer socket and port
    PeerSocId = 0xFF;
    for (Idx2 = 0; Idx2 < host->var.common.KtiPortCnt; Idx2++) {
      if ((SocketData->Cpu[KtiInternalGlobal->SbspSoc].LinkData[Idx2].Valid == TRUE) &&
          (SocketData->Cpu[KtiInternalGlobal->SbspSoc].LinkData[Idx2].PeerSocType == SOCKET_TYPE_CPU)) {
        PeerSocId = (UINT8) SocketData->Cpu[KtiInternalGlobal->SbspSoc].LinkData[Idx2].PeerSocId;
        PeerPort   = (UINT8) SocketData->Cpu[KtiInternalGlobal->SbspSoc].LinkData[Idx2].PeerPort;
        break;
      }
    }

    KTI_ASSERT((PeerSocId != 0xFF), ERR_UNSUPPORTED_TOPOLOGY, MINOR_ERR_INVALID_TOPOLOGY);

    // Mark all other sockets and links as invalid
    for (Idx1 = 0; Idx1 < MAX_SOCKET; Idx1++) {
      if ((Idx1 != KtiInternalGlobal->SbspSoc) && (Idx1 != PeerSocId)) {
        SocketData->Cpu[Idx1].Valid = FALSE;
      }
      for (Idx3 = 0; Idx3 < host->var.common.KtiPortCnt; Idx3++) {
        if ((Idx1 == KtiInternalGlobal->SbspSoc) && (Idx3 == Idx2)) {
          continue;
        } else if ((Idx1 == PeerSocId) && (Idx3 == PeerPort)) {
          continue;
        }
        SocketData->Cpu[Idx1].LinkData[Idx3].Valid = FALSE;
      }
    } // for each socket

    //
    // If 2S1L, and adjacent SID is 1, then treat as 2S1L, else treat as 4S or 8S
    //
    host->var.kti.SysConfig = ((PeerSocId <= 1) ? SYS_CONFIG_2S1L : (PeerSocId <= 3) ? SYS_CONFIG_4S : SYS_CONFIG_8S );


  } else if (Is3SChain == TRUE) {
    //
    // If the third link of the legacy socket is valid, mark it as invalid. For the remaining 2 peers
    // of the legacy socket make sure all their links except the one that connects them to
    // the legacy socket are marked as invalid.
    //
    KtiInternalGlobal->IsRouteThrough = TRUE;

    if (PeerSocCnt[KtiInternalGlobal->SbspSoc] == 3) {
      // Take out the socket that is connected to the 3rd link of the legacy socket
      PeerSocId = (UINT8) SocketData->Cpu[KtiInternalGlobal->SbspSoc].LinkData[2].PeerSocId;
      SocketData->Cpu[KtiInternalGlobal->SbspSoc].LinkData[2].Valid = FALSE;
      SocketData->Cpu[PeerSocId].Valid = FALSE;
      PeerSocList[KtiInternalGlobal->SbspSoc] = PeerSocList[KtiInternalGlobal->SbspSoc] & (UINT8) (~(1 << PeerSocId));
      PeerSocCnt[KtiInternalGlobal->SbspSoc] = 2;
      for (Idx2 = 0; Idx2 < host->var.common.KtiPortCnt; Idx2++) {
        SocketData->Cpu[PeerSocId].LinkData[Idx2].Valid = FALSE;
      }
    }

    host->var.kti.SysConfig = ((PeerSocList[KtiInternalGlobal->SbspSoc] <= 0x0F) ? SYS_CONFIG_4S : SYS_CONFIG_8S);

    //
    // Mark all the links that are not part of the chain as invalid
    // (Legacy socket's link are taken care above)
    //
    for (Idx1 = 0; Idx1 < MAX_SOCKET; Idx1++) {
      if ((Idx1 != KtiInternalGlobal->SbspSoc) && (SocketData->Cpu[Idx1].Valid == TRUE) &&
          (SocketData->Cpu[Idx1].SocType == SOCKET_TYPE_CPU)) {
        if (!(PeerSocList[KtiInternalGlobal->SbspSoc] & (1 << Idx1))) { // Remove the socket is not part of  the chain
          SocketData->Cpu[Idx1].Valid = FALSE;
        }
        for (Idx2 = 0; Idx2 < host->var.common.KtiPortCnt; Idx2++) {
          if ((SocketData->Cpu[Idx1].LinkData[Idx2].Valid == TRUE) &&
              (SocketData->Cpu[Idx1].LinkData[Idx2].PeerSocType == SOCKET_TYPE_CPU) &&
              ((UINT8) SocketData->Cpu[Idx1].LinkData[Idx2].PeerSocId != KtiInternalGlobal->SbspSoc)) {
            SocketData->Cpu[Idx1].LinkData[Idx2].Valid = FALSE;
          }
        }
      }
    }
  } else if (Is3SRing == TRUE) {
    //
    // Retain only the links that are part of the 3S Ring.
    //
    for (Idx1 = 0; Idx1 < MAX_SOCKET; Idx1++) {
      if (Ring3SList & (1 << Idx1)) { // If this socket is part of the 3S ring
        for (Idx2 = 0; Idx2 < host->var.common.KtiPortCnt; Idx2++) {
          if ((SocketData->Cpu[Idx1].LinkData[Idx2].Valid == TRUE) &&
              (SocketData->Cpu[Idx1].LinkData[Idx2].PeerSocType == SOCKET_TYPE_CPU)) {
            //
            // Check if this link's peer socket is part of the 3S ring and mark it as invalid
            //
            PeerSocId = (UINT8) SocketData->Cpu[Idx1].LinkData[Idx2].PeerSocId;
            if (!(Ring3SList & (1 << PeerSocId))) {
              PeerPort  = (UINT8) SocketData->Cpu[Idx1].LinkData[Idx2].PeerPort;
              SocketData->Cpu[Idx1].LinkData[Idx2].Valid = FALSE;
              SocketData->Cpu[PeerSocId].LinkData[PeerPort].Valid = FALSE;
            }
          }
        }
      } else {
        //
        // Mark the socket that is not part of the 3S Ring and all its links as invalid
        //
        SocketData->Cpu[Idx1].Valid = FALSE;
        for (Idx2 = 0; Idx2 < host->var.common.KtiPortCnt; Idx2++) {
          SocketData->Cpu[Idx1].LinkData[Idx2].Valid = FALSE;
        }
      }
    } // for each socket in 3S Ring

    host->var.kti.SysConfig = ((Ring3SList <= 0x0F) ? SYS_CONFIG_4S : SYS_CONFIG_8S);

  } else {
    //
    // 4S or greater degradation
    //
    if (IsDegradedFrom4S) {  // 4S degradation
      //
      // If there is any socket that has more than 2 links, take one of the link out.
      // Note that since each socket is assured of at least two links at this point,
      // if a socket has 3 links there must be other socket that has 3 links as well.
      // By taking link between those 2 sockets 4S Ring can be formed.
      //
      host->var.kti.SysConfig = SYS_CONFIG_4S;
      Soc1 = Soc2 = 0xFF;
      for (Idx1 = 0; Idx1 < 4; Idx1++) {
        if (PeerSocCnt[Idx1] == 3) {
          Soc1 = Idx1;
          for (Idx2 = Idx1 + 1; Idx2 < 4; Idx2++) {
            if (PeerSocCnt[Idx2] == 3) {
              Soc2 = Idx2;
              break;
            }
          }
          break;
        }
      }

      // Mark the link between the sockets as invalid
      if (Soc1 != 0xFF) {
        KTI_ASSERT((Soc2 != 0xFF), ERR_SW_CHECK, MINOR_ERR_SW_CHECK_73);
        for (Idx2 = 0; Idx2 < host->var.common.KtiPortCnt; Idx2++) {
          if ((SocketData->Cpu[Soc1].LinkData[Idx2].Valid == TRUE) &&
              (SocketData->Cpu[Soc1].LinkData[Idx2].PeerSocType == SOCKET_TYPE_CPU) &&
              (SocketData->Cpu[Soc1].LinkData[Idx2].PeerSocId == (UINT32) Soc2)) {
            PeerPort = (UINT8) SocketData->Cpu[Soc1].LinkData[Idx2].PeerPort;
            SocketData->Cpu[Soc1].LinkData[Idx2].Valid = FALSE;
            SocketData->Cpu[Soc2].LinkData[PeerPort].Valid = FALSE;
            break;
          }
        }
      }

      // Make sure it is a 4S ring
      for (Idx1 = 0; Idx1 < 4; Idx1++) {
        Soc1 = Soc2 = 0xFF;
        for (Idx2 = 0; Idx2 < host->var.common.KtiPortCnt; Idx2++) {
          if ((SocketData->Cpu[Idx1].LinkData[Idx2].Valid == TRUE) &&
              (SocketData->Cpu[Idx1].LinkData[Idx2].PeerSocType == SOCKET_TYPE_CPU)) {
            if (Soc1 == 0xFF) {
              Soc1 = (UINT8) SocketData->Cpu[Idx1].LinkData[Idx2].PeerSocId;
            } else if (Soc2 == 0xFF) {
              Soc2 = (UINT8) SocketData->Cpu[Idx1].LinkData[Idx2].PeerSocId;
            } else {
              KTI_ASSERT(FALSE, ERR_SW_CHECK, MINOR_ERR_SW_CHECK_74);
            }
          }
        }
        if ((Soc1 == 0xFF) || (Soc2 == 0xFF)) {
          KtiDebugPrintFatal ((host, KTI_DEBUG_ERROR, "\n  Reducing the Topology to 4S Ring failed!!!"));
          KTI_ASSERT (FALSE, ERR_UNSUPPORTED_TOPOLOGY, MINOR_ERR_INVALID_TOPOLOGY);
        }
      } // for each socket
    } else {
      //
      // Greater than 4S degradation
      //
      RingNumber = DiscEngData->RingInfo[KtiInternalGlobal->SbspSoc][1];
      for (Idx1 = 0; Idx1 < CPUS_PER_RING; Idx1++) {
        RingNode = DiscEngData->Rings[RingNumber][Idx1];
        RingNodeList |= 1 << RingNode;
      }

      for (Idx1 = 0; Idx1 < MAX_SOCKET; Idx1++) {
        if (!(RingNodeList & (1 << Idx1))) {
          //
          // Remove sockets not part of the ring
          //
          SocketData->Cpu[Idx1].Valid = FALSE;
          for (Idx2 = 0; Idx2 < MAX_KTI_PORTS; Idx2++) {
            SocketData->Cpu[Idx1].LinkData[Idx2].Valid = FALSE;
          }
        } else {
          //
          // Socket is part of the ring
          //
          for (Idx2 = 0; Idx2 < MAX_KTI_PORTS; Idx2++) {
            PeerSocId = (UINT8) SocketData->Cpu[Idx1].LinkData[Idx2].PeerSocId;
            if (!(RingNodeList & (1 << PeerSocId))) {
              //
              // Peer socket not part of the ring
              // Remove the link
              //
              SocketData->Cpu[Idx1].LinkData[Idx2].Valid = FALSE;
            }
          }
        }
      } // for each socket

      host->var.kti.SysConfig = ((RingNodeList <= 0x0F) ?  SYS_CONFIG_4S : SYS_CONFIG_8S);

    } // 4S vs > 4S degradation
  } // 4S or greater degradation

  return KTI_SUCCESS;
} // HandleDegraded4S

#if MAX_SOCKET > 4
/**

  Reduces the degraded 8S topology to 6S topology if possible. Otherwise it will reduced using
  degraded 4S handling rules.

  This function should be called only when the system has degraded 4S topology.


  @param host   - Input/Output structure of the KTIRC
  @param SocketData - Socket specific data structure
  @param KtiInternalGlobal - KTIRC internal variables.

  @retval 0 - Successful

**/
KTI_STATUS
CheckForDegraded8S (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal
  )
{
  UINT8   Idx1, Idx2;
  BOOLEAN Degraded;

  //
  // Check if all the links of all the 8 sockets are valid
  //
  Degraded = FALSE;
  for (Idx1 = 0; Idx1 < 8; Idx1++) {
    if ((SocketData->Cpu[Idx1].Valid == TRUE) && (SocketData->Cpu[Idx1].SocType == SOCKET_TYPE_CPU)) {
      for (Idx2 = 0; Idx2 < host->var.common.KtiPortCnt; Idx2++) {
        if (SocketData->Cpu[Idx1].LinkData[Idx2].Valid != TRUE) {
          Degraded = TRUE;
          break;
        }
      }
      if (Degraded == TRUE) {
        break;
      }
    }
  }

  if (Degraded == FALSE) {
    host->var.kti.SysConfig = SYS_CONFIG_8S;
    return KTI_SUCCESS;
  }

  //
  // It is a degraded 8S topology; reduce it to one of the supported topology.
  //
  KtiDebugPrintInfo1 ((host, KTI_DEBUG_INFO1, "\n\n  Degraded 8S topology is detected."));
  HandleDegraded8S (host, SocketData, KtiInternalGlobal);

  return KTI_SUCCESS;
}


/**

  Reduces the degraded 8S topology to 6S topology if possible. Otherwise it will reduced using
  degraded 4S handling rules.

  This function should be called only when the system has degraded 8S topology.


  @param host   - Input/Output structure of the KTIRC
  @param SocketData - Socket specific data structure
  @param KtiInternalGlobal - KTIRC internal variables.

  @retval KTI_SUCCESS

**/
KTI_STATUS
HandleDegraded8S (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal
  )
{
  BOOLEAN                 Degrade4S = FALSE;
  UINT8                   Idx1, RingNumber, RingNode = 0;
  KTI_DISC_DATA           TmpDiscEngData;

  if (KtiInternalGlobal->KtiCpuSktHotPlugEn == TRUE){
    host->var.kti.SysConfig = SYS_CONFIG_8S;             //hard code force to 8s
    return KTI_SUCCESS;
  }

  if (host->setup.kti.DfxParm.DfxSystemDegradeMode == DEGRADE_TOPOLOGY_AS_IS) {
    host->var.kti.SysConfig = SYS_CONFIG_8S;
    return KTI_SUCCESS;
  } else if (host->setup.kti.DfxParm.DfxSystemDegradeMode == DEGRADE_TOPOLOGY_1S) {
    DegradeTo1S (host, SocketData, KtiInternalGlobal);
    host->var.kti.SysConfig = SYS_CONFIG_1S;
    return KTI_SUCCESS;
  }

  // Prepare the discovery engine data structure
  PrepareDiscEngData (host, SocketData, KtiInternalGlobal, &TmpDiscEngData);

  // Check and identify the rings present in the topology
  LocateRings (host, &TmpDiscEngData);

  //
  // Any degraded 8S topology can have maximum of 3 rings and the legacy socket
  // can be part of upto only 2 rings.
  //
  KTI_ASSERT((TmpDiscEngData.TotRings <= 3), ERR_UNSUPPORTED_TOPOLOGY, MINOR_ERR_INVALID_TOPOLOGY);
  KTI_ASSERT((TmpDiscEngData.RingInfo[KtiInternalGlobal->SbspSoc][0] <= 2), ERR_UNSUPPORTED_TOPOLOGY, MINOR_ERR_INVALID_TOPOLOGY);

  // Try to salvage a 6S topology by retaining 2 rings. Preference is given to the rings that
  // the legacy socekt is part of. If the legacy socket is not part of any ring, then we can not salvage
  // a full 6S topology.  However, we may be able to salvage a degraded 6S (6SG1) topology.  If not, use 4S degrading rules.
  if (TmpDiscEngData.RingInfo[KtiInternalGlobal->SbspSoc][0] == 2) {
    // Legacy CPU on both CPU rings.  This is the prefered config.
    DegradeTo6S (host, SocketData, KtiInternalGlobal, &TmpDiscEngData, KtiInternalGlobal->SbspSoc);

  } else if (TmpDiscEngData.RingInfo[KtiInternalGlobal->SbspSoc][0] == 1) {
    // Legacy CPU only on 1 CPU ring.  6S topology still possible but only if another CPU ring exists and is
    // adjacent to the Legacy CPU's ring i.e. one of the other CPUs on the "Legacy CPU ring" must belong to two CPU ring.
    //
    // Check if any sockets of the legacy socket's ring is part of two rings
    RingNumber = TmpDiscEngData.RingInfo[KtiInternalGlobal->SbspSoc][1];
    for (Idx1 = 0; Idx1 < CPUS_PER_RING; Idx1++) {
      RingNode = TmpDiscEngData.Rings[RingNumber][Idx1];
      if (TmpDiscEngData.RingInfo[RingNode][0] == 2) {
        break;
      }
    }//For each CPU on legacy CPU ring

    if (Idx1 < CPUS_PER_RING) {
      DegradeTo6S (host, SocketData, KtiInternalGlobal, &TmpDiscEngData, RingNode);
    } else {
      // Legacy CPU belongs to a CPU ring, but no adjacent rings so no full 6S topology.
      // However, if there are at least 6 CPUs, a degraded 6S (6SG1) topology may still be possible.
      // But, if fewer than 6 CPUs or not a 6SG1 topology, just degrade to a 4S toplogy.
      if ((TmpDiscEngData.TotCpus < 6) ||
           (CheckForDegraded6S(host, SocketData, KtiInternalGlobal, &TmpDiscEngData) != KTI_SUCCESS)) {
        Degrade4S = TRUE;
      }
    }//If-else adjacent ring exist

  } else if (TmpDiscEngData.TotRings >= 1) {
    // Atleast one CPU ring exists, but legacy CPU is not part of it.
    // However, if there are at least 6 CPUs, a degraded 6S (6SG1) topology may still be possible.
    // But, if fewer than 6 CPUs or not a 6SG1 topology, just degrade to a 4S toplogy.
    if ((TmpDiscEngData.TotCpus < 6) ||
         (CheckForDegraded6S(host, SocketData, KtiInternalGlobal, &TmpDiscEngData) != KTI_SUCCESS)) {
      Degrade4S = TRUE;
    }

  } else {
    // No CPU rings exist i.e. no support topologies more than 4 CPUs (e.g. 8S, 6S, 6SG1).
    // So, just degrade to a 4S topology.
    Degrade4S = TRUE;
  }

  if (Degrade4S == TRUE) {
    HandleDegraded4S (host, SocketData, KtiInternalGlobal, &TmpDiscEngData);
  }

  return KTI_SUCCESS;
}


/**

  Degrade the system to 6S topology.

  @param host   - Input/Output structure of the KTIRC
  @param SocketData - Socket specific data structure
  @param KtiInternalGlobal - KTIRC internal variables.
  @param CommonSoc - Socket that is part of any 2 rings

  @retval KTI_SUCCESS

**/
KTI_STATUS
DegradeTo6S (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal,
  KTI_DISC_DATA        *DiscEngData,
  UINT8                CommonSoc
  )
{
  UINT8                   Idx1, Idx2, RingNumber;
  KTI_DISC_DATA           TmpDiscEngData1;

  //
  // Prepare a mask based on the sockets that part of the 2 rings. By removing the sockets that are not
  // part of the 2 rings and the links connecting to those sockets, the topology can be reduced to 6S.
  //
  DiscEngData->CpuList = 0;
  for (Idx1 = 1; Idx1 < 3; Idx1++) {
    RingNumber = DiscEngData->RingInfo[CommonSoc][Idx1];
    for (Idx2 = 0; Idx2 < CPUS_PER_RING; Idx2++) {
      DiscEngData->CpuList = DiscEngData->CpuList | (1 << DiscEngData->Rings[RingNumber][Idx2]);
    }
  }

  // Make sure there are 6 sockets present in the 2 rings of the CommonSoc
  DiscEngData->TotCpus = 0;
  for (Idx1 = 0; Idx1 < MAX_SOCKET; Idx1++) {
    if (DiscEngData->CpuList & (1 << Idx1)) {
      DiscEngData->TotCpus++;
    }
  }
  KTI_ASSERT((DiscEngData->TotCpus == 6), ERR_UNSUPPORTED_TOPOLOGY, MINOR_ERR_INVALID_TOPOLOGY);

  // Mark the remaining 2 sockets and the link connecting to them as invalid
  for (Idx1 = 0; Idx1 < MAX_SOCKET; Idx1++) {
    if ((DiscEngData->CpuList & (1 << Idx1)) == 0) {
      MemSetLocal ((VOID *)(&DiscEngData->Cpu[Idx1]), 0x00, sizeof (DiscEngData->Cpu[Idx1]));
    }
  }
  for (Idx1 = 0; Idx1 < MAX_SOCKET; Idx1++) {
    if (DiscEngData->Cpu[Idx1].Valid == TRUE) {
      for (Idx2 = 0; Idx2 < host->var.common.KtiPortCnt; Idx2++) {
        if ((DiscEngData->Cpu[Idx1].LinkData[Idx2].Valid == TRUE) &&
            ((DiscEngData->CpuList & (1 << DiscEngData->Cpu[Idx1].LinkData[Idx2].PeerSocId)) == 0)) {
          MemSetLocal ((VOID *)(&DiscEngData->Cpu[Idx1].LinkData[Idx2]), 0x00, sizeof (DiscEngData->Cpu[Idx1].LinkData[Idx2]));
        }
      }
    }
  }

  // Make sure the topology is reduced correctly (2 rings present and the legacy socket is part of at least one of the rings)
  MemSetLocal ((VOID *)(&TmpDiscEngData1), 0x00, sizeof (TmpDiscEngData1));
  MemCopy ((UINT8 *) TmpDiscEngData1.Cpu, (UINT8 *) DiscEngData->Cpu, sizeof (TmpDiscEngData1.Cpu));
  TmpDiscEngData1.TotCpus = DiscEngData->TotCpus;
  TmpDiscEngData1.CpuList = DiscEngData->CpuList;
  LocateRings (host, &TmpDiscEngData1);
  KTI_ASSERT((TmpDiscEngData1.TotRings == 2), ERR_UNSUPPORTED_TOPOLOGY, MINOR_ERR_INVALID_TOPOLOGY);
  KTI_ASSERT((TmpDiscEngData1.RingInfo[CommonSoc][0] == 2), ERR_UNSUPPORTED_TOPOLOGY, MINOR_ERR_INVALID_TOPOLOGY);

  //
  // Update SocketData structure with the reduced topology info.
  // System will still be considered as 8S for all configuration purposes
  //
  KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\n  Reducing it to 6S Topology "));
  host->var.kti.SysConfig = SYS_CONFIG_8S;
  MemCopy ((UINT8 *) SocketData->Cpu, (UINT8 *) TmpDiscEngData1.Cpu, sizeof (SocketData->Cpu));

  return KTI_SUCCESS;
}



/**

  CheckForDegraded6S()

  Try to degrade to a 6SG1 topology.

  @param host   - Input/Output structure of the KTIRC
  @param SocketData - Socket specific data structure
  @param KtiInternalGlobal - KTIRC internal variables.
  @param CommonSoc - Socket that is part of any 2 rings

  @retval KTI_SUCCESS

**/
KTI_STATUS
CheckForDegraded6S (
  struct sysHost             *host,
  KTI_SOCKET_DATA            *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal,
  KTI_DISC_DATA              *DiscEngData
  )
{
  UINT8                   RingId;               //Roving CPU Ring Id
  UINT8                  *RingCpuIdList;        //Roving ptr to the list of CPU Ids belonging to a CPU ring

  UINT8                   CpuId;                //Roving CPU Id
  KTI_CPU_SOCKET_DATA    *CpuPtr;               //Roving ptr to CPU's info

  UINT8                   NonRingCpuIdList[MAX_SOCKET]; //List of CPU Ids not part of the (one-and-only) CPU ring
  UINT8                   NonRingCpuCnt;        //Num of valid non-ring CPUsd

  UINT8                   NonRingCpuId1;        //CPU Id of a non-ring CPU
  UINT8                   NonRingToRingLinkId1; //Non-ring CPU's link Id used to attach to the CPU ring
  UINT8                   RingCpuId1;           //CPU Id of ring CPU to which the non-ring CPU is attached

  UINT8                   NonRingCpuId2;        //Same as above
  UINT8                   NonRingToRingLinkId2;
  UINT8                   RingCpuId2;

  BOOLEAN                 Found6SG1Topo;        //Indicates if a valid 6SG1 topology has been found
  BOOLEAN                 LegacyCpuInRing;      //Indicates if legacy CPU is part of the 'current' CPU ring

  UINT8                   Idx1, Idx2;           //Generic array index

  // Init key local vars
  Found6SG1Topo = FALSE;
  NonRingCpuCnt = 0;

  // Keep compiler from complaining about "possible uninitialized local var"
  NonRingCpuId1 = 0;
  NonRingCpuId2 = 0;
  NonRingCpuIdList[0] = 0;

  // Verify prerequisits
  if (DiscEngData->TotCpus < 6) {
    return KTI_FAILURE;
  }

  // Since there may be more than one CPU ring (e.g. a broken 8S topology),
  // check each ring to see if it forms the core ring of an 6SG1 topology.
  // Note: Any ring containing CPU-0 should have preference.  However, we
  //       assume any ring(s) containing CPU-0 will be listed first.
  for (RingId=0; RingId < DiscEngData->TotRings; RingId++) {
    RingCpuIdList = &DiscEngData->Rings[RingId][0];
    LegacyCpuInRing = FALSE;

    // Scan all CPUs and identify the CPUs that are not part of the current CPU ring.
    NonRingCpuCnt = 0;
    CpuPtr = &DiscEngData->Cpu[0];
    for (CpuId=0; CpuId < MAX_SOCKET; CpuId++, CpuPtr++) {
      if (CpuPtr->Valid) {
        // Check if CPU is part of current CPU ring
        for (Idx1=0; Idx1 < CPUS_PER_RING; Idx1++) {
          if (RingCpuIdList[Idx1] == CpuId) {  //CPU in ring?
            if (CpuId == KtiInternalGlobal->SbspSoc) {
              LegacyCpuInRing = TRUE;
            }
            break;  //CPU part of current CPU ring
          }
        }//For each CPU in ring
        if (Idx1 == CPUS_PER_RING) {
          NonRingCpuIdList[NonRingCpuCnt++] = CpuId;  //Add non-ring CPU to list
        }
      }//If CPU valid
    }//For each CPU

    // Examine each pair of non-ring CPUs looking for:
    // 1) A pair of non-ring CPUs attached to each other.
    // 2) Each CPU (in the pair) attached to the CPU ring.
    // 3) The two ring CPU attachment points must be opposite corners of the ring.
    // 4) A valid 6SG1 topology that includes the legacy CPU either within the CPU ring or as one of the non-ring CPUs.
    // If found, then we have a valid 6SG1 topology.
    for (Idx1=0; Idx1 < NonRingCpuCnt; Idx1++) {
      NonRingCpuId1 = NonRingCpuIdList[Idx1];
      for (Idx2=Idx1+1; Idx2 < NonRingCpuCnt; Idx2++) {
        NonRingCpuId2 = NonRingCpuIdList[Idx2];
        // Verify legacy CPU is included in the current set of CPUs under test: Either in the CPU ring or the two non-ring CPUs.
        if ((LegacyCpuInRing == FALSE) &&
            (NonRingCpuId1 != KtiInternalGlobal->SbspSoc) &&
            (NonRingCpuId2 != KtiInternalGlobal->SbspSoc)) {
          continue;  //Legacy CPU not included so don't bother checking.
        }
        // Check if both non-ring CPUs are directly attached to each other
        // Note: Both non-ring CPUs are known to be valid from code above.
        if (IsCpuAttachedToBranch(host, KtiInternalGlobal, DiscEngData->Cpu, NonRingCpuId1, &NonRingCpuId2, 1, NULL)) {  //Non-ring CPUs neighbors?
          // Check if both non-ring CPUs are directly attached to a CPU on the ring.
          // Note: All ring CPUs has atmost 1 non-ring link so both non-ring CPUs CAN NOT be attached to the same ring CPU.
          if ((IsCpuAttachedToBranch(host, KtiInternalGlobal, DiscEngData->Cpu, NonRingCpuId1, &RingCpuIdList[0], CPUS_PER_RING, &NonRingToRingLinkId1)) &&
              (IsCpuAttachedToBranch(host, KtiInternalGlobal, DiscEngData->Cpu, NonRingCpuId2, &RingCpuIdList[0], CPUS_PER_RING, &NonRingToRingLinkId2))) {
            // Verify the two (attached) ring CPUs are on opposite corners of the ring.
            // Otherwise, it's not a 6SG1 topology since some routes have more than 2 hops.
            RingCpuId1 = (UINT8) DiscEngData->Cpu[NonRingCpuId1].LinkData[NonRingToRingLinkId1].PeerSocId;
            RingCpuId2 = (UINT8) DiscEngData->Cpu[NonRingCpuId2].LinkData[NonRingToRingLinkId2].PeerSocId;
            if (IsCpuAttachedToBranch(host, KtiInternalGlobal, DiscEngData->Cpu, RingCpuId1, &RingCpuId2, 1, NULL) == FALSE) { //Ring CPUs NOT neighbors?
              Found6SG1Topo = TRUE;  //Ring CPUs not directly attached i.e. on opposite corners
              goto EndOfOuterLoop;
            };
          }//If non-ring CPUs pair attached to ring
        }//If non-ring CPU pair attached to each other
      }//For each "remaining" non-ring CPU
    }//For each non-ring CPU
  }//For each CPU ring
EndOfOuterLoop:

  // Check if a valid 6SG1 topology was found.  If not, error out.
  if (Found6SG1Topo == TRUE) {
    // A valid 6SG1 topology was found.  Now, trim off any excess CPUs not part of the 6SG1 topology.
    // Note: Only non-ring CPUs need to be checked.
    for (Idx1=0; Idx1 < NonRingCpuCnt; Idx1++) {
      CpuId = NonRingCpuIdList[Idx1];
      if ((CpuId != NonRingCpuId1) && (CpuId != NonRingCpuId2)) {
        TrimTopology(host, KtiInternalGlobal, DiscEngData, CpuId);
      }
    }//For each non-ring CPU

    // Update system vars to reflect the final toloplgy.
    KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\n  Reducing to 6SG1 Topology"));
    host->var.kti.SysConfig = SYS_CONFIG_8S;
    MemCopy ((UINT8*)SocketData->Cpu, (UINT8*)DiscEngData->Cpu, sizeof(SocketData->Cpu));
    return KTI_SUCCESS;

  } else {
    return KTI_FAILURE;
  }//If-else 6SG1 found

}

/**
  IsCpuAttachedToBranch()
  Check to see if the target CPU is attached to a branch of the current topology.
  In other words, is the target CPU is attached to one of the CPUs identified in the CPU list.

  @param host         - Input/Output structure of the KTIRC
  @param SocketData   - Socket specific data structure
  @param KtiInternalGlobal - KTIRC internal variables.
  @param CpuId        - Socket specific data structure
  @param CpuList      - List of CPUs to check
  @param CpuListCnt   - # of CPUs in the list
  @param *LinkId      - ID (num) of the link when the target CPU is attached.

  @retval TRUE, FALSE

**/
BOOLEAN
IsCpuAttachedToBranch (
  struct sysHost           *host,
  KTI_HOST_INTERNAL_GLOBAL *KtiInternalGlobal,
  KTI_CPU_SOCKET_DATA       CpuSocketData[],
  UINT8                     TargetCpuId,
  UINT8                     CpuList[],
  UINT8                     CpuListCnt,
  UINT8                    *AttachedLinkIdPtr
  )
{
  KTI_LINK_DATA*          LinkPtr;              //Roving ptr to target CPU's links
  UINT8                   LinkId;

  KTI_CPU_SOCKET_DATA*    PeerCpuPtr;           //Ptr to peer CPU
  KTI_LINK_DATA*          PeerLinkPtr;          //Ptr to peer CPU's link

  UINT8                   CpuIdx;               //Roving index into CPU list
  UINT8                   CpuId;


  //
  // Examine each CPU link to see if it is attached to one of the CPUs in the list.
  //
  LinkPtr = &CpuSocketData[TargetCpuId].LinkData[0];
  for (LinkId=0; LinkId < host->var.common.KtiPortCnt; LinkId++, LinkPtr++) {
    if (LinkPtr->Valid) {
      // Target CPU's link is valid so it's attached to something.
      // Scan the CPU list to see if the peer CPU is in the list.
      for (CpuIdx=0; CpuIdx < CpuListCnt; CpuIdx++) {
        CpuId = CpuList[CpuIdx];
        if (CpuId == LinkPtr->PeerSocId) {
          // Target CPU is attached to one of the CPUs in the list.
          // Check the peer CPU's info (status,link) to ensure everything is consistent.
          // If the peer CPU or the peer link is not valid, then the Target CPU is not (properly) attached.
          PeerCpuPtr = &CpuSocketData[LinkPtr->PeerSocId];
          if (PeerCpuPtr->Valid) {
            PeerLinkPtr = &PeerCpuPtr->LinkData[LinkPtr->PeerPort];
            if (PeerLinkPtr->Valid) {
              KTI_ASSERT(PeerLinkPtr->PeerSocId == TargetCpuId, ERR_TOPOLOGY_DISCOVERY, MINOR_ERR_INTERNAL_DATA_STRUCTURE);
              KTI_ASSERT(PeerLinkPtr->PeerPort  == LinkId,      ERR_TOPOLOGY_DISCOVERY, MINOR_ERR_INTERNAL_DATA_STRUCTURE);
              if (AttachedLinkIdPtr != NULL) {
                *AttachedLinkIdPtr = LinkId;   //If wanted, tell caller which target CPU's link is being used
              }
              return TRUE;
            }//Peer link valid
          }//Peer CPU valid
        }//Found attached CPU
      }//For each CPU in list
    }//CPU's link valid
  }//For each CPU link

  // Target CPU not attached to any of the CPUs in the CPU list.
  return FALSE;
}


/**
  TrimTopology()
  Trim the current topology by removing the specified CPU.

  @param host         - Input/Output structure of the KTIRC
  @param SocketData   - Socket specific data structure
  @param KtiInternalGlobal - KTIRC internal variables.
  @param CpuId        - Socket specific data structure

  @retval KTI_SUCCESS

**/
KTI_STATUS
TrimTopology (
  struct sysHost            *host,
  KTI_HOST_INTERNAL_GLOBAL  *KtiInternalGlobal,
  KTI_DISC_DATA             *DiscEngData,
  UINT8                     CpuId
  )
{
  KTI_CPU_SOCKET_DATA*    CpuPtr;               //Ptr to info of CPU to be trimmed
  KTI_LINK_DATA*          LinkPtr;              //Roving ptr to target CPU's links
  UINT8                   LinkId;

  KTI_CPU_SOCKET_DATA*    PeerCpuPtr;           //Roving rtr to peer CPU
  KTI_LINK_DATA*          PeerLinkPtr;          //Roving ptr to peer CPU's link

  //
  // Trim the CPU from the topology tree by setting Valid=FALSE for
  // the CPU, all it's links, and all it's peer links (but not the peer CPUs).
  //
  CpuPtr = &DiscEngData->Cpu[CpuId];
  if (CpuPtr->Valid) {
    // Process all Links
    LinkPtr = &CpuPtr->LinkData[0];
    for (LinkId=0; LinkId < host->var.common.KtiPortCnt; LinkId++, LinkPtr++) {
      if (LinkPtr->Valid) {
        PeerCpuPtr = &DiscEngData->Cpu[LinkPtr->PeerSocId];
        //JGR - Question:  Better to replace the following 'if valid' with QPI_ASSERT(PeerCpuPtr->Valid);
        if (PeerCpuPtr->Valid) {
          PeerLinkPtr = &PeerCpuPtr->LinkData[LinkPtr->PeerPort];
          //JGR - Question:  Better to replace the following 'if valid' with QPI_ASSERT(PeerLinkPtr->Valid);
          if (PeerLinkPtr->Valid) {
            KTI_ASSERT(PeerLinkPtr->PeerSocId == CpuId,  ERR_TOPOLOGY_DISCOVERY, MINOR_ERR_INTERNAL_DATA_STRUCTURE);
            KTI_ASSERT(PeerLinkPtr->PeerPort  == LinkId, ERR_TOPOLOGY_DISCOVERY, MINOR_ERR_INTERNAL_DATA_STRUCTURE);
            PeerLinkPtr->Valid = FALSE;
          }//if peer link valid
          // Don't set Peer CPU to FALSE - PeerCpuPtr->Valid = FALSE;
        }//if peer CPU valid
        //Note: Peer CPU remains valid since some of its other links may be attached somewhereelse
      }//if CPU link valid
      LinkPtr->Valid = FALSE;
    }//For each CPU link
    CpuPtr->Valid = FALSE;
  }//if CPU valid

  return KTI_SUCCESS;
}

#endif

/**

  Degrade the system to 1S topology.

  @param host  - Pointer to the system host (root) structure
  @param SocketData - Pointer to socket specific data
  @param KtiInternalGlobal - KTIRC Internal Global data

  @retval KTI_SUCCESS

**/
KTI_STATUS
DegradeTo1S (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal
  )
{
  UINT8   Idx;

  for (Idx = 0; Idx < MAX_SOCKET; Idx++) {
    if (Idx != KtiInternalGlobal->SbspSoc) {
      // Mark all sockets except socket legacy socket as invalid.
      SocketData->Cpu[Idx].Valid = 0;
      SocketData->Cpu[Idx].SocId = 0;
    }
    MemSetLocal ((UINT8 *) SocketData->Cpu[Idx].LinkData, 0x00, sizeof (SocketData->Cpu[Idx].LinkData));
    KtiInternalGlobal->CpuList = 0x00000001;
  }

  return KTI_SUCCESS;
}


/**

  Prime the KTIRC data structure based on input option and system topology.

  @param host  - Pointer to the system host (root) structure
  @param SocketData - Pointer to socket specific data
  @param KtiInternalGlobal - KTIRC Internal Global data

  @retval 0 - Successful
  @retval Other - failure

**/
KTI_STATUS
PrimeHostStructure (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal
  )
{
  UINT8      Ctr;


  //
  // Update the sysHost common structure with the topology info
  //
  host->var.common.numCpus = KtiInternalGlobal->TotCpu;
  KTI_ASSERT (( host->var.common.numCpus <= MAX_SOCKET), ERR_UNSUPPORTED_TOPOLOGY, MINOR_ERR_INVALID_TOPOLOGY); // Defensive check for data structure overflows
  host->var.common.socketPresentBitMap = KtiInternalGlobal->CpuList;
  for (Ctr = 0; Ctr < MAX_SOCKET; Ctr++) {
    if ((KtiInternalGlobal->CpuList & (1 << Ctr)) == 0) {
      host->var.common.SocketFirstBus[Ctr] = 0;
      host->var.common.SocketLastBus[Ctr] = 0;
    }
    host->var.kti.CpuInfo[Ctr].ChaList = KtiInternalGlobal->ChaList[Ctr];
    host->var.kti.CpuInfo[Ctr].TotCha = KtiInternalGlobal->TotCha[Ctr];
  }

  //
  // Update KTIRC output structure
  //

  //
  // The host setup structure is a const strucuture. If the options provided in setup structure is
  // invalid for the given topology KTIRC will force supported value for that option. To facilitate this
  // we copy those kind of options into host kti structure and modify it to match the topology.
  //
  host->var.kti.OutLegacyVgaSoc= host->setup.kti.LegacyVgaSoc;
  host->var.kti.OutLegacyVgaStack= host->setup.kti.LegacyVgaStack;
  host->var.kti.OutKtiLinkSpeed = host->setup.kti.KtiLinkSpeed;
  host->var.kti.OutKtiLinkL0pEn = host->setup.kti.KtiLinkL0pEn;
  host->var.kti.OutKtiLinkL1En = host->setup.kti.KtiLinkL1En;
  host->var.kti.OutIsocEn = host->setup.common.isocEn;
  host->var.kti.OutKtiCrcMode = host->setup.kti.KtiCrcMode;
  host->var.kti.OutKtiFailoverEn = host->setup.kti.KtiFailoverEn;

  //
  // Initialize the KTIRC global variables and check the host input structure to make sure it is
  // consist with the system topology.
  //
  host->var.kti.OutKtiCpuSktHotPlugEn = KtiInternalGlobal->KtiCpuSktHotPlugEn;

  // Make sure the legacy VGA range target is valid socket
  if ((host->var.kti.OutLegacyVgaSoc >= MAX_SOCKET) || (SocketData->Cpu[host->var.kti.OutLegacyVgaSoc].Valid != TRUE)) {
    KtiLogWarning(host, 0xFF, 0xFF, 0xFF, WARN_VGA_TARGET_SOC_NOT_PRESENT);
    KtiDebugPrintWarn((host, KTI_DEBUG_WARN, "\n WARNING:  Legacy VGA range target not populated; assiging the range to Legacy CPU socket. "));
    host->var.kti.OutLegacyVgaSoc = KtiInternalGlobal->SbspSoc; // Make Legacy socket as the VGA range target
  }

  // Make sure the legacy VGA range target is valid stack
  if ((host->var.kti.OutLegacyVgaStack >= MAX_IIO_STACK) || (!(host->var.common.stackPresentBitmap[host->var.kti.OutLegacyVgaSoc] & (1<<host->var.kti.OutLegacyVgaStack)))) {
    KtiLogWarning(host, 0xFF, 0xFF, 0xFF, WARN_VGA_TARGET_STACK_NOT_PRESENT);
    KtiDebugPrintWarn((host, KTI_DEBUG_WARN, "\n WARNING:  Legacy VGA range target stack not present; assiging the range to Legacy Cstack. "));
    host->var.kti.OutLegacyVgaStack = IIO_CSTACK; // Make Legacy stack as the VGA range target
  }


  //
  // Process Phy/Link?
  //
  KtiInternalGlobal->ProgramPhyLink = FALSE;

  //
  // Process if Multisocket or Hotplug enabled
  //
  if ((KtiInternalGlobal->TotCpu != 1) || (host->var.kti.OutKtiCpuSktHotPlugEn == TRUE)) {
    KtiInternalGlobal->ProgramPhyLink = TRUE;
  }

  PrimeHostStructureIsoc (host, SocketData, KtiInternalGlobal);

  PrimeHostStructureStack (host, SocketData, KtiInternalGlobal);

  if (host->var.common.cpuType == CPU_SKX) {
       PrimeHostStructureSkx (host, SocketData, KtiInternalGlobal);
  } else {
    KtiCheckPoint (0xFF, 0xFF, 0xFF, ERR_UNSUPPORTED_TOPOLOGY, MINOR_ERR_INVALID_SOCKET_TYPE, host);
    KtiDebugPrintFatal ((host, KTI_DEBUG_ERROR, "\n Unknown Cpu Type"));
    KTI_ASSERT (FALSE, ERR_UNSUPPORTED_TOPOLOGY, MINOR_ERR_INVALID_SOCKET_TYPE);
  }

  PrimeHostStructureFpga (host, SocketData, KtiInternalGlobal);

  PrimeHostStructurePhyLinkLayer (host, SocketData, KtiInternalGlobal);

  host->var.kti.OutIsRouteThrough = KtiInternalGlobal->IsRouteThrough;

  //
  // Interleave size = 2**(size*2) GB.
  //
  KtiInternalGlobal->MmiohTargetSize = host->setup.common.mmiohSize;
  if (KtiInternalGlobal->MmiohTargetSize > MMIOH_MAX_GRANTY) {
    KtiLogWarning(host, 0xFF, 0xFF, 0xFF, WARN_MMIOH_SIZE_UNSUPPORTED);
    KtiDebugPrintWarn((host, KTI_DEBUG_WARN, "\n WARNING:  MMIOH size requested is invalid. Forcing it to 1GB.\n"));
    KtiInternalGlobal->MmiohTargetSize = 0;
  }

  // Default disable
  host->var.kti.OutKtiPrefetch = 0;
  Init_IRQ_Threshold(host);

  KtiInternalGlobal->TSCSyncEn = host->setup.kti.TscSyncEn;



  return KTI_SUCCESS;
}

/**

  Prime the KTIRC data structure based on input option and system topology.

  @param host  - Pointer to the system host (root) structure
  @param SocketData - Pointer to socket specific data
  @param KtiInternalGlobal - KTIRC Internal Global data

  @retval 0 - Successful
  @retval Other - failure

**/
KTI_STATUS
PrimeHostStructureIsoc (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal
  )
{
  UINT32 BiosScratchPad7;

  //
  // Allows Isoc in 1S and 2S2Link
  //
  if ((host->var.kti.SysConfig != SYS_CONFIG_1S && host->var.kti.SysConfig != SYS_CONFIG_2S2L)) {
    host->var.kti.OutIsocEn = FALSE;
  }

  //
  // Isoc not allowed in hot-pluggable systems
  //
  if (KtiInternalGlobal->KtiCpuSktHotPlugEn == TRUE) {
    host->var.kti.OutIsocEn = FALSE;
  }

  //
  // If AEP DIMM present (indicated bit StickyPad7[31]), disable Isoc
  //
  BiosScratchPad7 = KtiReadPciCfg (host, KtiInternalGlobal->SbspSoc, 0, BIOSSCRATCHPAD7_UBOX_MISC_REG);
  if (BiosScratchPad7 & BIT31) {
    host->var.kti.OutIsocEn = FALSE;
  }

  return KTI_SUCCESS;
}

/**

  Prime the KTIRC stackres data structure based on input option and system topology.

  @param host  - Pointer to the system host (root) structure
  @param SocketData - Pointer to socket specific data
  @param KtiInternalGlobal - KTIRC Internal Global data

  @retval 0 - Successful
  @retval Other - failure

**/
KTI_STATUS
PrimeHostStructureStack (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal
  )
{
  UINT8      Socket, Stack, TotTargetCount = 0;

  //
  // Update StackRes info with type of stack that is present and count number of SAD targets
  //
  for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
    if (KtiInternalGlobal->CpuList & (1 << Socket)) {
      //
      // DMI/CStack is always present
      //
      host->var.common.stackPresentBitmap[Socket] |= 1;
      host->var.kti.CpuInfo[Socket].CpuRes.TotEnabledStacks = 0;

      for (Stack = 0; Stack < MAX_IIO_STACK; Stack++) {
        if (host->var.common.stackPresentBitmap[Socket] & (1 << Stack)) {
          //
          // SKX: We do not comprehend soft disable of stacks since we don't clock gate them
          //
          if (Stack < IIO_PSTACK3) {
            host->var.kti.CpuInfo[Socket].CpuRes.StackRes[Stack].Personality = TYPE_UBOX_IIO;
          } else if (IsSocketFpga (host, Socket, KtiInternalGlobal) == TRUE) {
            host->var.kti.CpuInfo[Socket].CpuRes.StackRes[Stack].Personality = TYPE_FPGA;
          } else {
            host->var.kti.CpuInfo[Socket].CpuRes.StackRes[Stack].Personality = TYPE_MCP;
          }
          TotTargetCount += 1;
          host->var.kti.CpuInfo[Socket].CpuRes.TotEnabledStacks += 1;
        } else if (Stack < IIO_PSTACK3) {
          host->var.kti.CpuInfo[Socket].CpuRes.StackRes[Stack].Personality = TYPE_UBOX;
        } else {
          host->var.kti.CpuInfo[Socket].CpuRes.StackRes[Stack].Personality = TYPE_NONE;
        }
      }
    }
    if (SocketData->Cpu[Socket].SocType == SOCKET_TYPE_FPGA) {
      // FPGA only have 1 enabled stack
      host->var.common.stackPresentBitmap[Socket] |= 1;
      host->var.kti.CpuInfo[Socket].CpuRes.stackPresentBitmap = host->var.common.stackPresentBitmap[Socket];
      host->var.kti.CpuInfo[Socket].CpuRes.TotEnabledStacks = 1;
    }
  }

  KtiInternalGlobal->TotSadTargets = TotTargetCount;

  return KTI_SUCCESS;
}

/**

  Prime the KTIRC data structure based on fpga input option and system topology.

  @param host  - Pointer to the system host (root) structure
  @param SocketData - Pointer to socket specific data
  @param KtiInternalGlobal - KTIRC Internal Global data

  @retval 0 - Successful
  @retval Other - failure

**/
KTI_STATUS
PrimeHostStructureFpga (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal
  )
{
  UINT8      Socket, Port, PeerSocId, PeerPort;
  BOOLEAN    FpgaPresent = FALSE;

  //
  // Update structures for Fpga
  // Todo: Should clean this up to not override DFX and setup knobs
  //
  //
  // Identify all Fpga sockets for external use
  //
  for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
    if (SocketData->Cpu[Socket].Valid == TRUE && (SocketData->Cpu[Socket].SocType == SOCKET_TYPE_CPU)) {
      if (IsSocketFpga (host, Socket, KtiInternalGlobal)) {
        host->var.kti.OutKtiFpgaPresent[Socket] = 1;
      }
    }
  }

  for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
    for (Port = 0; Port < host->var.common.KtiPortCnt; Port++) {
      if ((SocketData->Cpu[Socket].LinkData[Port].Valid == TRUE) && (SocketData->Cpu[Socket].SocType == SOCKET_TYPE_FPGA)) {

        host->var.kti.OutKtiFpgaEnable[SocketData->Cpu[Socket].LinkData[Port].PeerSocId] = 1;

        KtiDebugPrintInfo1 ((host, KTI_DEBUG_INFO1, "\n  Fpga detected, switching to Per Link Freq"));
        host->var.kti.OutKtiLinkSpeed = FREQ_PER_LINK;
        KtiInternalGlobal->PhyLinkPerPortSetting[Socket].Phy[Port].KtiLinkSpeed = SPEED_REC_96GT;
        PeerSocId = (UINT8) SocketData->Cpu[Socket].LinkData[Port].PeerSocId;
        PeerPort = (UINT8) SocketData->Cpu[Socket].LinkData[Port].PeerPort;
        KtiInternalGlobal->PhyLinkPerPortSetting[PeerSocId].Phy[PeerPort].KtiLinkSpeed = SPEED_REC_96GT;

        KtiDebugPrintInfo1 ((host, KTI_DEBUG_INFO1, "\n  Fpga detected, disable L1, L0p, Failover on FPGA link"));
        host->setup.kti.DfxCpuCfg[Socket].Link[Port].DfxL1Enable = KTI_DISABLE;
        host->setup.kti.DfxCpuCfg[Socket].Link[Port].DfxL0pEnable = KTI_DISABLE;
        host->setup.kti.DfxCpuCfg[Socket].Phy[Port].DfxKtiFailoverEn = KTI_DISABLE;
        PeerSocId = (UINT8) SocketData->Cpu[Socket].LinkData[Port].PeerSocId;
        PeerPort = (UINT8) SocketData->Cpu[Socket].LinkData[Port].PeerPort;
        host->setup.kti.DfxCpuCfg[PeerSocId].Link[PeerPort].DfxL1Enable = KTI_DISABLE;
        host->setup.kti.DfxCpuCfg[PeerSocId].Link[PeerPort].DfxL0pEnable = KTI_DISABLE;
        host->setup.kti.DfxCpuCfg[PeerSocId].Phy[PeerPort].DfxKtiFailoverEn = KTI_DISABLE;

        KtiDebugPrintInfo1 ((host, KTI_DEBUG_INFO1, "\n  Fpga detected, disable CRC on FPGA link"));
        host->setup.kti.DfxCpuCfg[Socket].Link[Port].DfxCrcMode = CRC_MODE_16BIT;
        PeerSocId = (UINT8) SocketData->Cpu[Socket].LinkData[Port].PeerSocId;
        PeerPort = (UINT8) SocketData->Cpu[Socket].LinkData[Port].PeerPort;
        host->setup.kti.DfxCpuCfg[PeerSocId].Link[PeerPort].DfxCrcMode = CRC_MODE_16BIT;

        KtiInternalGlobal->ProgramPhyLink = TRUE;

        //
        // Fpga only decodes a single bus over Kti
        //
        host->var.kti.CpuInfo[Socket].CpuRes.StackRes[IIO_CSTACK].Personality  = TYPE_UBOX;
        host->var.kti.CpuInfo[Socket].CpuRes.StackRes[IIO_PSTACK0].Personality = TYPE_NONE;
        host->var.kti.CpuInfo[Socket].CpuRes.StackRes[IIO_PSTACK1].Personality = TYPE_NONE;
        host->var.kti.CpuInfo[Socket].CpuRes.StackRes[IIO_PSTACK2].Personality = TYPE_NONE;
        host->var.kti.CpuInfo[Socket].CpuRes.StackRes[IIO_PSTACK3].Personality = TYPE_NONE;
        host->var.kti.CpuInfo[Socket].CpuRes.StackRes[IIO_PSTACK4].Personality = TYPE_NONE;
        // Todo: review this, may want to move out of the stackPresentBitmap entirely.
        host->var.common.stackPresentBitmap[Socket] |= 1;
        host->var.kti.CpuInfo[Socket].CpuRes.TotEnabledStacks = 0;

        FpgaPresent = TRUE;

      }
    }

    if (FpgaPresent) {
      //
      // Directory must be enabled for FPGA snoopfilter
      //
      KtiInternalGlobal->SysDirectoryModeEn = KTI_ENABLE;
      KtiInternalGlobal->SnoopFilter = KTI_ENABLE;
      host->var.kti.OutHitMeEn = KTI_DISABLE;
      KtiInternalGlobal->SnoopFanoutEn = KTI_DISABLE;
      KtiInternalGlobal->SysOsbEn = KTI_DISABLE;  // Disable until told otherwise by design
      host->var.kti.OutIoDcMode = host->setup.kti.IoDcMode;     // set the IoDc in 2SKX-2FPGA & 1SKX-1FPGA

      if ((host->var.kti.SysConfig == SYS_CONFIG_2S1L) || (host->var.kti.SysConfig == SYS_CONFIG_2S2L)) {
        KtiInternalGlobal->IsRouteThrough = TRUE;
      }
      //
      // Force ChaID interleaving
      //
      if (host->var.kti.SysConfig == SYS_CONFIG_2S2L) {
        if (host->var.kti.OutSncEn == 1) {
          KtiInternalGlobal->DualLinksInterleavingMode = DFX_DUAL_LINK_INTLV_MODE_DISABLE_D2C;
          KtiInternalGlobal->D2cEn = 0;
          KtiInternalGlobal->D2kEn = 0; // todo: Can d2k be enabled with SNC?
        } else {
          KtiInternalGlobal->DualLinksInterleavingMode = DFX_DUAL_LINK_INTLV_MODE_CHA_INTLV;
          KtiInternalGlobal->D2kEn = 0;
        }
      }
    }
  }

  return KTI_SUCCESS;
}


/**

  Prime the KTIRC data structure based on input option and system topology.

  @param host  - Pointer to the system host (root) structure
  @param SocketData - Pointer to socket specific data
  @param KtiInternalGlobal - KTIRC Internal Global data

  @retval 0 - Successful
  @retval Other - failure

**/
KTI_STATUS
PrimeHostStructureSkx (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal
  )
{
  UINT32                      Data32;

  //
  // Directory mode can be enabled based on setup optin if multi-socket topology
  // OSB should be enabled only if multi-socket and Directory mode enabled
  //
  if ((host->var.kti.SysConfig != SYS_CONFIG_1S)) {
    KtiInternalGlobal->SysDirectoryModeEn = host->setup.kti.DirectoryModeEn;
    if(KtiInternalGlobal->SysDirectoryModeEn == KTI_ENABLE) {
      KtiInternalGlobal->SysOsbEn = KTI_ENABLE;
    } else {
      KtiInternalGlobal->SysOsbEn = KTI_DISABLE;
    }
  } else {
    KtiInternalGlobal->SysOsbEn = KTI_DISABLE;
    KtiInternalGlobal->SysDirectoryModeEn = KTI_DISABLE;
  }

  if (host->setup.kti.DfxParm.DfxOsbEn < KTI_AUTO) {
    KtiInternalGlobal->SysOsbEn = host->setup.kti.DfxParm.DfxOsbEn;
  }

  //
  // IODC only in Directory enabled configurations
  //
  if (KtiInternalGlobal->SysDirectoryModeEn == KTI_DISABLE) {
    host->var.kti.OutIoDcMode = IODC_DISABLE;
  } else {
    host->var.kti.OutIoDcMode = host->setup.kti.IoDcMode;
  }

  //
  //If Rbt is enabled, d2c or d2k can be enabled by default
  //If Rbt is disabled, d2c and d2k must be disabled
  //It is illegal to enable D2C in multi-socket systems with dir-disabled. (from SysDefature0 defint
  //It is illegal to enable D2K in multi-socket systems with dir-disabled.
  //
  //

  if (KtiInternalGlobal->SysDirectoryModeEn == KTI_ENABLE) {
    host->var.kti.OutHitMeEn = 1;
    KtiInternalGlobal->D2cEn = 1;
    KtiInternalGlobal->D2kEn = 1;
  } else {
    host->var.kti.OutHitMeEn = 0;
    KtiInternalGlobal->D2cEn = 0;
    KtiInternalGlobal->D2kEn = 0;
  }

  KtiInternalGlobal->DualLinksInterleavingMode = KTI_AUTO;
  // DfxDualLinksInterleavingMode is only valid in 2S2L
  if (host->var.kti.SysConfig == SYS_CONFIG_2S2L && host->setup.kti.DfxParm.DfxDualLinksInterleavingMode < KTI_AUTO) {
    KtiInternalGlobal->DualLinksInterleavingMode = host->setup.kti.DfxParm.DfxDualLinksInterleavingMode;
  }

  if (host->var.kti.SysConfig == SYS_CONFIG_1S) {
    KtiInternalGlobal->D2cEn = 1;
  }

  //
  // 4930353: Cloned From SKX Si Bug Eco: M2PCIE sends incomplete KTI srcinfo to IIO/Ubox for 2S-3KTI config with D2C
  // A0-only w/a: disable D2c for 2S3KTI
  //
  if (host->var.kti.SysConfig == SYS_CONFIG_2S3L &&
      host->var.common.cpuType == CPU_SKX && host->var.common.stepping < B0_REV_SKX) {
    KtiInternalGlobal->D2cEn = 0;
  }

  //
  // 5330853: M3KTI Egress zeroing out SNID for route-through BL SR-D packets
  //
  if (((((host->var.kti.SysConfig == SYS_CONFIG_4S) && (KtiInternalGlobal->Is4SRing)) || (host->var.kti.SysConfig > SYS_CONFIG_4S)) && ((host->var.common.cpuType == CPU_SKX) && (host->var.common.stepping < B0_REV_SKX))) \
      || (KtiInternalGlobal->DualLinksInterleavingMode == DFX_DUAL_LINK_INTLV_MODE_DISABLE_D2C)) {
    KtiInternalGlobal->D2cEn = 0;
  }

  KtiInternalGlobal->SnoopFanoutEn = FALSE;

  //
  // Enable  4SRing and beyond > 4S
  //
  if (((host->var.kti.SysConfig == SYS_CONFIG_4S) && (KtiInternalGlobal->Is4SRing)) || (host->var.kti.SysConfig > SYS_CONFIG_4S)) {
    KtiInternalGlobal->SnoopFanoutEn = TRUE;
  }

  //s4930292 disable fanout snoop if SKX A stepping
  if (host->var.common.cpuType == CPU_SKX && host->var.common.stepping < B0_REV_SKX) {
    KtiInternalGlobal->SnoopFanoutEn = FALSE;
  }

  //
  // If DFX snoop fanout is not "Auto", set it to the requested value
  //
  if (host->setup.kti.DfxParm.DfxSnoopFanoutEn < KTI_AUTO) {
    KtiInternalGlobal->SnoopFanoutEn = host->setup.kti.DfxParm.DfxSnoopFanoutEn;
  }

  // VN1 is not supported in EP
  host->var.kti.OutVn1En = FALSE;
  if (host->var.kti.SysConfig == SYS_CONFIG_8S) {
    host->var.kti.OutVn1En = TRUE;
  }

  //
  // If it is requesting "Auto" for D2C, enable it for all topologies
  //
  if (host->setup.kti.DfxParm.DfxD2cEn < KTI_AUTO) {
    KtiInternalGlobal->D2cEn = host->setup.kti.DfxParm.DfxD2cEn;
  }

  if (host->setup.kti.DfxParm.DfxD2kEn < KTI_AUTO) {
    KtiInternalGlobal->D2kEn = host->setup.kti.DfxParm.DfxD2kEn;
  }

  //
  // If Validation is requesting non-Auto for Hit Me, override it for all topologies
  //
  if (host->setup.kti.DfxParm.DfxHitMeEn < KTI_AUTO) {
    host->var.kti.OutHitMeEn = host->setup.kti.DfxParm.DfxHitMeEn;
  }

  //
  // If DFX VN1 is not "Auto", set it to DFX requested value.
  //
  if (host->setup.kti.DfxParm.DfxVn1En < KTI_AUTO) {
    host->var.kti.OutVn1En = host->setup.kti.DfxParm.DfxVn1En;
  }


  if (host->var.kti.OutVn1En == TRUE){
    KtiInternalGlobal->D2kEn = 0;
  }

  //
  // Need NUMA enabled and MMCFG Base aligned on 1GB to enable SNC
  //
  if (host->setup.kti.SncEn == KTI_AUTO) {
    host->var.kti.OutSncEn = 0;
  } else {
    host->var.kti.OutSncEn = host->setup.kti.SncEn;
  }

  if (host->var.kti.OutSncEn == KTI_ENABLE) {
    if ((host->setup.mem.options & NUMA_AWARE) && ((host->setup.common.mmCfgBase % 0x40000000) == 0) &&
        (KtiInternalGlobal->DualLinksInterleavingMode != DFX_DUAL_LINK_INTLV_MODE_CHA_INTLV)) {
        host->var.kti.OutSncPrefetchEn = (host->setup.kti.KtiPrefetchEn | host->setup.kti.XptPrefetchEn |host->var.kti.OutSncEn);
    } else {
      if ((host->setup.common.mmCfgBase % 0x40000000) != 0) {
        KtiDebugPrintWarn((host, KTI_DEBUG_WARN, "\n WARNING:  MMCFG Base is not aligned on 1G boundary. Forcing SNC to disabled.\n"));
      }
      host->var.kti.OutSncEn = 0;
      host->var.kti.OutSncPrefetchEn = 0;
    }
  } else {
    if ((host->setup.mem.options & NUMA_AWARE) && ((host->setup.common.mmCfgBase % 0x40000000) == 0) &&
        (KtiInternalGlobal->DualLinksInterleavingMode != DFX_DUAL_LINK_INTLV_MODE_CHA_INTLV)) {
      host->var.kti.OutSncPrefetchEn = (host->setup.kti.KtiPrefetchEn | host->setup.kti.XptPrefetchEn |host->var.kti.OutSncEn);
    } else {
      host->var.kti.OutSncPrefetchEn = 0;
    }
  }

  //
  // s5385623: SNC couldn't be enabled if enabled slices are less than 12
  //
  if (KtiInternalGlobal->TotCha[KtiInternalGlobal->SbspSoc] < 12){
    if (host->var.kti.OutSncEn == KTI_ENABLE){
      host->var.kti.OutSncEn = 0;
      host->var.kti.OutSncPrefetchEn = 0;
    }
  }

  KtiDebugPrintInfo0((host, KTI_DEBUG_INFO0, "\n   OutSncPrefetchEn=%x\n", host->var.kti.OutSncPrefetchEn));




  Data32 = KtiReadPciCfg (host, KtiInternalGlobal->SbspSoc, 0, BIOSSCRATCHPAD7_UBOX_MISC_REG);
  // SP7 bit 30 indicates snc failure during previous warm reset boot, then need to recovery back to SNC disabled mode
  if (Data32 & BIT30) {
    host->var.kti.OutSncEn = 0;       // disable OutSncEn for MRC usage
    host->var.kti.OutSncPrefetchEn = 0;       // disable OutSncPrefetchEn for internal usage
  }
  //
  // 5330736: 2nd prefetch rd to same addres, WA is disable D2C and D2K only for A*
  //
  if (host->var.common.cpuType == CPU_SKX && host->var.common.stepping < B0_REV_SKX) {
    if (host->var.kti.SysConfig != SYS_CONFIG_1S){
      if (((host->setup.kti.XptPrefetchEn == KTI_ENABLE) || (host->setup.kti.KtiPrefetchEn == KTI_ENABLE)) ||
          ((host->setup.common.numaEn != KTI_ENABLE) && (host->setup.kti.KtiPrefetchEn == KTI_ENABLE))) {
        KtiInternalGlobal->D2cEn = 0;
        KtiInternalGlobal->D2kEn = 0;
      }
    }
  }

  if (KtiInternalGlobal->DualLinksInterleavingMode == DFX_DUAL_LINK_INTLV_MODE_CHA_INTLV &&
      host->var.kti.SysConfig == SYS_CONFIG_2S2L) {
    KtiInternalGlobal->D2kEn = 0;
  }

  return KTI_SUCCESS;
}

/**

  Routine to sync up the PBSPs and update the necessary scratch registers. As part of that,
  the sockets taht are not connected to legacy are issued "BOOT_GO" command to bring them
  under BIOS control.

  @param host  - Pointer to the system host (root) structure
  @param SocketData - Pointer to socket specific data
  @param KtiInternalGlobal - KTIRC Internal Global data
  @param RstType - Boot path type.  powergood or warm reset flow
**/
KTI_STATUS
SyncUpPbspForReset (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal,
  UINT8                  RstType
  )
{
  UINT8                        Index, Port, Idx2;
  UINT8                        SteppingMismatch = FALSE;
  UINT32                       Data32, Sr02, GblNidMap0, GblNidMap1, GidNidMap;
  UINT32                       Ctr, WaitTime;
  SAD_TARGET_CHABC_SAD1_STRUCT SadTarget;
  LT_CONTROL_CHABC_SAD1_STRUCT LtControlSbsp;
  LT_CONTROL_CHABC_SAD1_STRUCT LtControlAp;
  UINT8                        CheckInFlag[MAX_SOCKET], AllCheckInFlag;

  if ((RstType == POST_RESET_POWERGOOD) || (KtiInternalGlobal->ForceColdResetFlow == TRUE)) {
    //
    // Update the SBSP Ubox sticky scratch register 02 with the LEP info. This info is used to verify
    // if the system KTI topology changed across boots and take remidial action. Note that in case
    // of reduced topology scenario, scratch registers are updated with the reduced topology, not
    // the current topology. The current reset flow has been running under the assumption of the
    // reduced topology. (For example Credits are programmed for reduced topology scenario)
    // [07:00]   - CPU list; this list reflects the CPUs after the topology is degraded, if needed
    // [10:08]   - CPU0 Link 0, 1, 2 Valid
    // [13:11]   - CPU1 Link 0, 1, 2 Valid
    // [16:14]   - CPU2 Link 0, 1, 2 Valid
    // [19:17]   - CPU3 Link 0, 1, 2 Valid
    // [22:20]   - CPU4 Link 0, 1, 2 Valid
    // [25:23]   - CPU5 Link 0, 1, 2 Valid
    // [28:26]   - CPU6 Link 0, 1, 2 Valid
    // [31:29]   - CPU7 Link 0, 1, 2 Valid
    //
    Sr02 = 0;
    for (Index = 0; Index < MAX_SOCKET; Index++) {
      if (SocketData->Cpu[Index].Valid == TRUE) {
        for (Port = 0; Port < host->var.common.KtiPortCnt; Port++) {
          if (SocketData->Cpu[Index].LinkData[Port].Valid == TRUE) {
            Sr02 = Sr02 | (1 << ((Index * SI_MAX_KTI_PORTS) + Port));
          }
        }
      }
    }
    Sr02 = (Sr02 << 8) | (KtiInternalGlobal->CpuList);

    KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\n\n    Setting Ubox Sticky SR02 to 0x%08X \n", Sr02));

    for (Index = 0; Index < MAX_SOCKET; Index++) {
      if ((SocketData->Cpu[Index].Valid == TRUE) && (SocketData->Cpu[Index].SocType == SOCKET_TYPE_CPU)) {
        KtiWritePciCfg (host, Index, 0, BIOSSCRATCHPAD2_UBOX_MISC_REG, Sr02);
      }
    }
  }

  //
  // Set the "BOOT_GO" flag in the remote socket to bring it under BIOS control.
  //
  MemSetLocal ((UINT8 *) &CheckInFlag, FALSE, sizeof (CheckInFlag));

  for (Index = 0; Index < MAX_SOCKET; Index++) {
    if((Index == KtiInternalGlobal->SbspSoc) || (SocketData->Cpu[Index].Valid != TRUE)) {
      CheckInFlag[Index] = TRUE;
      continue;
    } else if ((CheckCpuConnectedToLegCpu (host, SocketData, KtiInternalGlobal, Index) == FALSE) && (SocketData->Cpu[Index].SocType == SOCKET_TYPE_CPU)) {

      //
      // BIOS has to initialize the following fields of SAD_TARGET_CHABC_SAD1 register for the remote socket.
      // For the legacy socket pCode will set them for this register.  For sockets directly connected to legacy socket,
      // pCode will only program flash_target and lt_target.   BIOS must program legacy_pch_target
      //
      SadTarget.Data = KtiReadPciCfg (host, Index, 0, SAD_TARGET_CHABC_SAD1_REG);
      SadTarget.Bits.flash_target       = KtiInternalGlobal->SbspSoc;
      SadTarget.Bits.legacy_pch_target  = KtiInternalGlobal->SbspSoc;
      KtiWritePciCfg (host, Index, 0, SAD_TARGET_CHABC_SAD1_REG, SadTarget.Data);

      //
      // BIOS has to initialize the LT_CONTROL register for the remote socket.
      // For the legacy socket and the sockets directly connected to it, the pCode will set this register.
      // So we just replicate the register setting from SBSP to the remote socket.
      //
      LtControlSbsp.Data = KtiReadPciCfg (host, KtiInternalGlobal->SbspSoc, 0, LT_CONTROL_CHABC_SAD1_REG);
      if (LtControlSbsp.Bits.lt_target_enable == 0x1) {
        LtControlAp.Data = KtiReadPciCfg (host, Index, 0, LT_CONTROL_CHABC_SAD1_REG);
        if (LtControlSbsp.Bits.lt_target & SOCKET_LOCAL_NID) {
          LtControlAp.Bits.lt_target = KtiInternalGlobal->SbspSoc;
        } else {
          LtControlAp.Bits.lt_target = LtControlSbsp.Bits.lt_target;
        }
        LtControlAp.Bits.lt_target_enable = LtControlSbsp.Bits.lt_target_enable;
        KtiWritePciCfg (host, Index, 0, LT_CONTROL_CHABC_SAD1_REG, LtControlAp.Data);
      }


      //
      // In LT enabled system, releasing socket2 may cause a LT_SHUTDOWN, which causes a surprise warm reset.
      // Set bit29 of SP7 to indicate we are releasing the remote socket, so that if LT_SHUTDOWN happes.
      // BIOS can detect this and handle it.
      //
      for (Idx2 = 0; Idx2 < MAX_SOCKET; Idx2++) {
        if ((SocketData->Cpu[Idx2].Valid == TRUE) && (SocketData->Cpu[Idx2].SocType == SOCKET_TYPE_CPU)) {
          Data32 = KtiReadPciCfg(host, Idx2, 0,BIOSSCRATCHPAD7_UBOX_MISC_REG);
          Data32 = Data32 | 1 << 29;
          KtiWritePciCfg(host, Idx2, 0, BIOSSCRATCHPAD7_UBOX_MISC_REG, Data32);
        }
      }

      //
      // Issue BOOT_GO command
      //
      KtiCheckPoint((UINT8)(1 << Index), SOCKET_TYPE_CPU, 0, STS_PBSP_SYNC_UP, MINOR_STS_ISSUE_BOOT_GO, host);
      KtiDebugPrintInfo0((host, KTI_DEBUG_INFO0, "\n    Issuing BOOT_GO command to CPU%u. ", SocketData->Cpu[Index].SocId));
      Data32 = KtiReadPciCfg(host, Index, 0, BIOSNONSTICKYSCRATCHPAD1_UBOX_MISC_REG);
      Data32 = (Data32 & 0xFFFFFF00) | 0x2; // Issue BOOT_GO
      KtiWritePciCfg(host, Index, 0, BIOSNONSTICKYSCRATCHPAD1_UBOX_MISC_REG, Data32);
    } else {                         // skip the socket directly connected to SBSP
      CheckInFlag[Index] = TRUE;
    }
  }

  //
  // Wait for the remote CPU(s) to check-in.
  //
  KtiDebugPrintInfo0((host, KTI_DEBUG_INFO0, "\n    Verifying if the remote socket(s) checked-in. \n"));
  OemWaitTimeForPSBP(host, &WaitTime);

  // Loop until either all CPUs checked-in or the wait time expires
  Idx2=0;
  for (Ctr = 0; Ctr < WaitTime; Ctr++) {
    AllCheckInFlag = TRUE; // Assume all CPUs checked-in
    for (Index = 0; Index < MAX_SOCKET; Index++) {
      if (CheckInFlag[Index] == FALSE) { // If this CPU has not checked-in yet
        Data32 = KtiReadPciCfg (host, Index, 0, BIOSNONSTICKYSCRATCHPAD2_UBOX_MISC_REG);
        if ((Data32 & 0x1) != 0) {  //check bit 0 to indicate AP check in
          CheckInFlag[Index] = TRUE;
        } else {
          AllCheckInFlag = FALSE; // Indicate that at least one CPU hasn't checked-in
          Idx2=Index;
        }
      }
    }
    if (AllCheckInFlag == FALSE) {
      KtiFixedDelay(host, 1000); // 1000 microsecs
    } else {
      break;
    }
  }

  //
  // Handle the failure
  //
  if (Ctr == WaitTime) { // If the remote socket(s) hasn't checked-in, assert; it is a fatal condition
    KtiCheckPoint((UINT8)(1 << Index), SOCKET_TYPE_CPU, 0xFF, ERR_BOOT_MODE, MINOR_ERR_PBSP_CHKIN_FAILURE, host);
    KtiDebugPrintFatal((host, KTI_DEBUG_WARN, "\n\n  Remote Socket %u is discovered but hasn't come out of reset. System Halted!!!", Idx2));
    KTI_ASSERT(FALSE, ERR_BOOT_MODE, MINOR_ERR_PBSP_CHKIN_FAILURE);
  }

  //
  // In LT enabled system, releasing socket2 may cause a LT_SHUTDOWN, which causes a surprise warm reset.
  // If we mde it here, that is not the case, so clear the bit
  //
  for (Idx2 = 0; Idx2 < MAX_SOCKET; Idx2++) {
    if ((SocketData->Cpu[Idx2].Valid == TRUE) && (SocketData->Cpu[Idx2].SocType == SOCKET_TYPE_CPU)) {
      Data32 = KtiReadPciCfg(host, Idx2, 0, BIOSSCRATCHPAD7_UBOX_MISC_REG);
      Data32 = Data32 & ~(1 << 29);
      KtiWritePciCfg(host, Idx2, 0, BIOSSCRATCHPAD7_UBOX_MISC_REG, Data32);
    }
  }


  if ((RstType == POST_RESET_POWERGOOD) || (KtiInternalGlobal->ForceColdResetFlow == TRUE) || (host->var.common.resetRequired == POST_RESET_POWERGOOD)) {
    //
    // Update the number/list of CPUs populated
    //
    GblNidMap0 = 0;         // for socket 0-3
    GblNidMap1 = 0;         // for socket 4-7
    GidNidMap = 0;
    for (Index = 0; Index < MAX_SOCKET; Index++) {
      if ((SocketData->Cpu[Index].Valid == TRUE) && (SocketData->Cpu[Index].SocType == SOCKET_TYPE_CPU)) {
          if ( Index < 4) {
              GblNidMap0 = GblNidMap0 | (1 << (28 + Index)) | (Index << (4 * Index));
          } else {
              GblNidMap1 = GblNidMap1 | (1 << (28 + (Index - 4))) | (Index << (4 * (Index - 4)));
          }
          GidNidMap = GidNidMap | (Index << (3 * Index));
      }
    }

    for (Index = 0; Index < MAX_SOCKET; Index++) {
      if ((SocketData->Cpu[Index].Valid == TRUE) && (SocketData->Cpu[Index].SocType == SOCKET_TYPE_CPU)) {
        // Local Node id
        Data32 = KtiReadPciCfg (host, Index, 0, CPUNODEID_UBOX_CFG_REG);
        Data32 = (Data32 & 0x7)  | Index;
        KtiWritePciCfg (host, Index, 0, CPUNODEID_UBOX_CFG_REG, Data32);

        KtiWritePciCfg (host, Index, 0, GIDNIDMAP_UBOX_CFG_REG, GidNidMap);

        // CpuEnable
        Data32 = (1 << 31) | KtiInternalGlobal->CpuList; // Set the list of CPUs in the system and enable the register
        KtiWritePciCfg (host, Index, 0, CPUENABLE_UBOX_CFG_REG, Data32);

        // Bcastlist
        Data32 = (KtiInternalGlobal->CpuList << 24) | (KtiInternalGlobal->CpuList << 16) |
                 (KtiInternalGlobal->CpuList << 8) | (KtiInternalGlobal->CpuList);
        KtiWritePciCfg (host, Index, 0, BCASTLIST_UBOX_CFG_REG, Data32);

        // CpuNodeID
        KtiWritePciCfg (host, Index, 0, GLOBAL_NID_SOCKET_0_TO_3_MAP_PCU_FUN2_REG, GblNidMap0);
        if( GblNidMap1 != 0) {
          KtiWritePciCfg (host, Index, 0, GLOBAL_NID_SOCKET_4_TO_7_MAP_PCU_FUN4_REG, GblNidMap1);
        }
      }
    }

    //
    // At this point we are in cold-reset flow; now all CPUs can access all other socket's CSRs and thier
    // path to FWH agent is set. Request for reset.
    //
    host->var.common.resetRequired |= POST_RESET_WARM;
    MemCopy ((UINT8 *) host->var.kti.CpuInfo[KtiInternalGlobal->SbspSoc].TopologyInfo,
             (UINT8 *) SocketData->SbspMinPathTree, sizeof (host->var.kti.CpuInfo[KtiInternalGlobal->SbspSoc].TopologyInfo));
  }

  //
  // Read stepping from scratch pad register of each PBSP and store them into sysHost struct
  //
  for (Index = 0; Index < MAX_SOCKET; Index++) {
    if ((SocketData->Cpu[Index].Valid == TRUE) && (SocketData->Cpu[Index].SocType == SOCKET_TYPE_CPU)) {

      Data32 = KtiReadPciCfg (host, Index, 0, BIOSNONSTICKYSCRATCHPAD2_UBOX_MISC_REG);
      //
      // CSR bit[30:27] = CPU Stepping
      //
      //host->var.cpu.packageBspStepping[Index] = (UINT8) ((Data32 >> 27) & 0x0f);
      host->var.cpu.packageBspStepping[Index] = KtiGetSiliconRevision(host, Index, (UINT8) ((Data32 >> 27) & 0x0f), 0);
      KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "packageBspStepping[%x]=%x\n", Index, host->var.cpu.packageBspStepping[Index]));


      //
      // Update common.stepping to the lowest stepping level
      //
      if (host->var.cpu.packageBspStepping[Index] < host->var.common.stepping) {
        host->var.common.stepping = host->var.cpu.packageBspStepping[Index];
      }

      //
      // Log a warning if there's a stepping mismatch
      //
      if (host->var.cpu.packageBspStepping[Index] != host->var.common.stepping) {
        KtiLogWarning(host, 0xFF, 0xFF, 0xFF, WARN_CPU_STEPPING_MISMATCH);
        KtiDebugPrintWarn((host, KTI_DEBUG_WARN, "\n WARNING:  CPU stepping mismatch detected.\n"));
        SteppingMismatch = TRUE;
      }
    }
  }

  if (SteppingMismatch == TRUE) {
    KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\n  Socket | Stepping \n"));
    for (Index = 0; Index < MAX_SOCKET; Index++) {
      if ((SocketData->Cpu[Index].Valid == TRUE) && (SocketData->Cpu[Index].SocType == SOCKET_TYPE_CPU)) {
        KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "  %d      | 0x%x\n", Index, host->var.cpu.packageBspStepping[Index]));
      }
    }
  }

  return KTI_SUCCESS;
}

/**

  Routine to program the final route, VN selection and fanout setting for the given topology.
  Refer to KtiDisc.c file for more info on how the route settings are calcualted.

  @param host  - Pointer to the system host (root) structure
  @param SocketData - Pointer to socket specific data
  @param KtiInternalGlobal - KTIRC Internal Global data

  @retval 0 - Successful
  @retval Other - failure

**/
KTI_STATUS
ProgramSystemRoute (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal
  )
{
  TOPOLOGY_TREE_NODE  *SbspTree;
  TOPOLOGY_TREE_NODE  *TopologyTree;
  TOPOLOGY_TREE_NODE  TreeNode;
  UINT8               TreeIdx;
  UINT8               Ctr, Ctr1, Port;
  UINT32              Data32;
  UINT8               sbspSktId;
  UINT32              TempR2pGnCtrl[MAX_SOCKET][MAX_SKX_M2PCIE];
  CHA_RTR_SNP_CTL1_CHA_MISC_STRUCT ChaRtrSnpCtr1;
  M3KCTRL_M3KTI_MAIN_STRUCT        M3KCtrl;
  M3EFQCTRL_M3KTI_MAIN_STRUCT      M3efqctrl;
  M3EFQPRIVC2_M3KTI_MAIN_STRUCT    M3EfqPrivc2;
  NID2KTI_M2MEM_MAIN_STRUCT        Nid2KtiM2Mem;
  UINT32              NumOfHops[MAX_SOCKET];
  CBO_COH_CONFIG_CHABC_SAD_STRUCT   CboCohBc;
  SAD_TARGET_CHABC_SAD1_STRUCT      SadTarget;

  MemSetLocal ((UINT8 *) &TempR2pGnCtrl, 0x00, sizeof (TempR2pGnCtrl));
  KtiCheckPoint (0xFF, 0xFF, 0xFF, STS_PROGRAM_FINAL_ROUTE, MINOR_STS_PROGRAM_FINAL_ROUTE_START, host);

  sbspSktId = KtiInternalGlobal->SbspSoc;
  SbspTree = SocketData->CpuTree[sbspSktId];

  // Program NonBC registers
  if ((host->var.kti.RasInProgress != TRUE) || (host->var.kti.ProgramNonBC == TRUE)) {
    //
    // While the route tables are being programmed, the route is in transient state; since we have to do read-modify-write,
    // we have to read it and remember the register settings so while programming we just modify and write it.
    //
    for (Ctr = 0; Ctr < MAX_SOCKET; Ctr++) {
      if ((SocketData->Cpu[Ctr].Valid == TRUE) && (SocketData->Cpu[Ctr].SocType == SOCKET_TYPE_CPU)) {
        for (Ctr1 = 0; Ctr1 < MAX_SKX_M2PCIE; Ctr1++) {
          if(host->var.common.M2PciePresentBitmap[Ctr] & (1 << Ctr1)) {
              TempR2pGnCtrl[Ctr][Ctr1] = KtiReadPciCfg (host, Ctr, Ctr1, R2PGNCTRL_M2UPCIE_MAIN_REG);
          }
        }
      }
    }

    //
    // Program the M2PCIe & M2MCP,  M2MEM (for D2K) route table
    //
    for (TreeIdx = 0; SbspTree[TreeIdx].Node.Valid == TRUE; TreeIdx++) {
      TreeNode = SbspTree[TreeIdx];
      if (TreeNode.Node.SocType == SOCKET_TYPE_CPU) {

        for (Ctr1 = 0; Ctr1 < MAX_SKX_M2PCIE; Ctr1++) {
          if(host->var.common.M2PciePresentBitmap[TreeNode.Node.SocId] & (1 << Ctr1)) {
            Data32  = KtiReadPciCfg (host, (UINT8) TreeNode.Node.SocId, Ctr1, QPI_RT_M2UPCIE_MAIN_REG);
            Data32  = (Data32 & 0xFFFF0000) | SocketData->Route[TreeNode.Node.SocId].M2PcieKtiRt;
            KtiWritePciCfg (host, (UINT8) TreeNode.Node.SocId, Ctr1, QPI_RT_M2UPCIE_MAIN_REG, Data32);
          }
        }

        // setup D2kEn in M2MEM
        if (KtiInternalGlobal->D2kEn == TRUE) {
          for (Ctr1 = 0; Ctr1 < KtiInternalGlobal->TotM2Mem[TreeNode.Node.SocId]; Ctr1++) {
            Nid2KtiM2Mem.Data  = KtiReadPciCfg (host, (UINT8) TreeNode.Node.SocId, Ctr1, NID2KTI_M2MEM_MAIN_REG);
            Nid2KtiM2Mem.Bits.nid2ktimap =  SocketData->Route[TreeNode.Node.SocId].ChaKtiRt;
            KtiWritePciCfg (host, (UINT8) TreeNode.Node.SocId, Ctr1, NID2KTI_M2MEM_MAIN_REG, Nid2KtiM2Mem.Data);
          }
        }

      } else if (TreeNode.Node.SocType == SOCKET_TYPE_FPGA) {
      } else {
        KTI_ASSERT(FALSE, ERR_SW_CHECK, MINOR_ERR_SW_CHECK_75);
      }
    }

    //
    // Calculate NumberOfHops for Performance Response Encoding
    //
    for (Ctr = 0; Ctr < MAX_SOCKET; Ctr++) {
      NumOfHops[Ctr]=0;
      if ((SocketData->Cpu[Ctr].Valid == TRUE) && (SocketData->Cpu[Ctr].SocType == SOCKET_TYPE_CPU)) {
        TopologyTree = SocketData->CpuTree[Ctr];
        for (Ctr1 = 0; TopologyTree[Ctr1].Node.Valid != FALSE; Ctr1++) {
          if (TopologyTree[Ctr1].Node.Hop > 3) {
            TopologyTree[Ctr1].Node.Hop = 3;              //Max hop is 3 to avoid overflow. This feature is for performance only
          }
          NumOfHops[Ctr] |= TopologyTree[Ctr1].Node.Hop << (2 * TopologyTree[Ctr1].Node.SocId);
        }

        KtiWritePciCfg (host, Ctr, 0, NUM_OF_HOPS_CHABC_SAD_REG, NumOfHops[Ctr]);
      }
    }

    for (TreeIdx = 0; SbspTree[TreeIdx].Node.Valid == TRUE; TreeIdx++) {
      TreeNode = SbspTree[TreeIdx];
      if (TreeNode.Node.SocType == SOCKET_TYPE_CPU) {

        //
        // Enable the M2PCIe responses to use the RT for all topologies except 2S
        // Must start with M2PCIE instance 1 (it contains UBOX) for response to use new route-through programming
        //
        if ((host->var.kti.SysConfig != SYS_CONFIG_1S) &&
            (host->var.kti.SysConfig != SYS_CONFIG_2S2L) &&
            (host->var.kti.SysConfig != SYS_CONFIG_2S1L) &&
            (host->var.kti.SysConfig != SYS_CONFIG_2S3L)) {
           for (Ctr1 = 1; Ctr1 < MAX_SKX_M2PCIE; Ctr1++) {
             if (host->var.common.M2PciePresentBitmap[TreeNode.Node.SocId] & (1 << Ctr1)) {
                Data32 = TempR2pGnCtrl[TreeNode.Node.SocId][Ctr1];
                Data32 |= 0x2; // Set qpi_rt_sel_ndr_drs
                KtiWritePciCfg (host, (UINT8) TreeNode.Node.SocId, Ctr1, R2PGNCTRL_M2UPCIE_MAIN_REG, Data32);
             }
           }

           //
           // Then program M2PCIE instance 0 if it's present
           //
           if (host->var.common.M2PciePresentBitmap[TreeNode.Node.SocId] & 1) {
             Data32 = TempR2pGnCtrl[TreeNode.Node.SocId][0];
             Data32 |= 0x2; // Set qpi_rt_sel_ndr_drs
             KtiWritePciCfg (host, (UINT8) TreeNode.Node.SocId, 0, R2PGNCTRL_M2UPCIE_MAIN_REG, Data32);
           }
        }

        //
        // Program CHA Route Table; it is non BC register. So we need to program in each cha.
        // For 2S 2KTI/2S 3KTI case, we need to enable the CHA and M2mem interleave mode.
        //
        if (host->var.kti.SysConfig == SYS_CONFIG_2S2L) {              //2S 2KTI  (DualLink)
          for (Ctr1 = 0; Ctr1 < KtiInternalGlobal->TotM2Mem[TreeNode.Node.SocId]; Ctr1++) {
            Nid2KtiM2Mem.Data  = KtiReadPciCfg (host, (UINT8) TreeNode.Node.SocId, Ctr1, NID2KTI_M2MEM_MAIN_REG);
            Nid2KtiM2Mem.Bits.kti2slinkintlvena = 1;

            // For the "twisted" 2S topology, make sure S1 CHA/M2MEM swap the odd/even link usage
            if (((KtiInternalGlobal->TwistedKtiLinks == TRUE) && ((UINT8) TreeNode.Node.SocId == 1)) &&
                 (KtiInternalGlobal->DualLinksInterleavingMode != DFX_DUAL_LINK_INTLV_MODE_CHA_INTLV)) {
              Nid2KtiM2Mem.Bits.swapktilinkintlv = 1;
            }
            KtiWritePciCfg (host, (UINT8) TreeNode.Node.SocId, Ctr1, NID2KTI_M2MEM_MAIN_REG, Nid2KtiM2Mem.Data);
          }

          CboCohBc.Data = KtiReadPciCfg (host, (UINT8) TreeNode.Node.SocId, 0, CBO_COH_CONFIG_CHABC_SAD_REG);

          if (KtiInternalGlobal->DualLinksInterleavingMode != DFX_DUAL_LINK_INTLV_MODE_CHA_INTLV) {
            CboCohBc.Bits.enablexorbasedktilink = 1;
          }

          if (((KtiInternalGlobal->TwistedKtiLinks == TRUE) && ((UINT8) TreeNode.Node.SocId == 1)) &&
              (KtiInternalGlobal->DualLinksInterleavingMode != DFX_DUAL_LINK_INTLV_MODE_CHA_INTLV)) {
            CboCohBc.Bits.swapktilinkintlv = 1;
          }
          KtiWritePciCfg (host, (UINT8) TreeNode.Node.SocId, 0, CBO_COH_CONFIG_CHABC_SAD_REG, CboCohBc.Data);

        } else if (host->var.kti.SysConfig == SYS_CONFIG_2S3L ) {   //2S 3KTI (TwoSkt3Link)

          for (Ctr1 = 0; Ctr1 < KtiInternalGlobal->TotM2Mem[TreeNode.Node.SocId]; Ctr1++) {
            Nid2KtiM2Mem.Data  = KtiReadPciCfg (host, (UINT8) TreeNode.Node.SocId, Ctr1, NID2KTI_M2MEM_MAIN_REG);
            Nid2KtiM2Mem.Bits.d2k2s3ktiena = 1;
            KtiWritePciCfg (host, (UINT8) TreeNode.Node.SocId, Ctr1, NID2KTI_M2MEM_MAIN_REG, Nid2KtiM2Mem.Data);
          }

          SadTarget.Data = KtiReadPciCfg (host, (UINT8) TreeNode.Node.SocId, 0, SAD_TARGET_CHABC_SAD1_REG);
          SadTarget.Bits.enablemodulo3basedktilink = 1;
          KtiWritePciCfg (host, (UINT8) TreeNode.Node.SocId, 0, SAD_TARGET_CHABC_SAD1_REG, SadTarget.Data);
        }

        for (Ctr = 0; Ctr < KtiInternalGlobal->TotCha[TreeNode.Node.SocId]; Ctr++) {
          // s4930277
          if (KtiInternalGlobal->DualLinksInterleavingMode == DFX_DUAL_LINK_INTLV_MODE_CHA_INTLV) {
            //
            // Interleave based on CHA ID[0]:
            //    even CHAs get 0x0 (route to Socket1 through Port0)
            //    odd CHAs get 0x4  (route to Socket1 through Port1)
            //
            Data32 = (Ctr % 2) ? 0x4 : 0x0;
            if (TreeNode.Node.SocId == 1) {
              if (KtiInternalGlobal->TwistedKtiLinks == TRUE) {
                //
                // The opposite for Socket1 in twisted case
                //
                Data32 = (Ctr % 2) ? 0x0 : 0x1;
              } else  {
                //
                // For the Socket 1 routing if not in twisted case
                //
                Data32 = (Ctr % 2) ? 0x1 : 0x0;
              }
            }
          } else {
            //
            // Interleave based on P6 ^ P12, not based on cha id
            //
            Data32 = SocketData->Route[TreeNode.Node.SocId].ChaKtiRt;
          }

          if (host->var.kti.SysConfig == SYS_CONFIG_2S2L) {
            Data32 |= (0xFFFFFFF0 & (KtiReadPciCfg (host, (UINT8) TreeNode.Node.SocId, Ctr, KTI_ROUTING_TABLE_CHA_PMA_REG)));
          }

          KtiWritePciCfg (host, (UINT8) TreeNode.Node.SocId, Ctr, KTI_ROUTING_TABLE_CHA_PMA_REG, Data32);
        }

        //
        // Program M3KTI Route Table & Vn attribute & Snoop Fanout & pe_mode
        //
        for (Ctr = 0; Ctr < host->var.kti.CpuInfo[TreeNode.Node.SocId].TotM3Kti; Ctr++) {
          Data32  = SocketData->Route[TreeNode.Node.SocId].M3KtiRt[Ctr];
          KtiWritePciCfg (host, (UINT8) TreeNode.Node.SocId, Ctr, M3KKRT_M3KTI_MAIN_REG, Data32); //Route Table

          // program Vn attribute
          //Ingress Vn Sel for port 0
          Data32  = SocketData->Route[TreeNode.Node.SocId].M3KtiVnSel[Ctr].IgrVnSel;
          KtiWritePciCfg (host, (UINT8) TreeNode.Node.SocId, Ctr, M3KIGRVNSEL0_M3KTI_MAIN_REG, Data32);

          //Ingress Vn Sel for port 1
          Data32  = SocketData->Route[TreeNode.Node.SocId].M3KtiVnSel[Ctr].IgrVnSel;
          KtiWritePciCfg (host, (UINT8) TreeNode.Node.SocId, Ctr, M3KIGRVNSEL1_M3KTI_MAIN_REG, Data32);


          M3KCtrl.Data = KtiReadPciCfg (host, (UINT8) TreeNode.Node.SocId, Ctr, M3KCTRL_M3KTI_MAIN_REG);
          M3KCtrl.Bits.socketid = TreeNode.Node.SocId;   // SocketId

          if ((host->var.common.cpuType == CPU_SKX) && (host->var.common.stepping >= B0_REV_SKX)) {
            M3KCtrl.Bits.socketidforsnp = TreeNode.Node.SocId;
          }

          //
          // 4929504: KTI tile performance critical CSRs for BIOS programming
          //  Requires setting d2cthreshold = 2 while previously we set 4 (default Eg Size / 2)
          //  Also requires setting meshcreditpush = 1
          //
          if (KtiInternalGlobal->D2cEn == TRUE) {
            M3KCtrl.Bits.d2cthreshold = D2C_THLD_DFLT_SETTING;
          } else {
            M3KCtrl.Bits.d2cthreshold = 0;
          }
          M3KCtrl.Bits.meshcreditpush = 1;
          KtiWritePciCfg (host, (UINT8) TreeNode.Node.SocId, Ctr, M3KCTRL_M3KTI_MAIN_REG, M3KCtrl.Data);

          //
          // Program pe_mode based on isocen
          //
          M3efqctrl.Data = KtiReadPciCfg (host, (UINT8) TreeNode.Node.SocId, Ctr, M3EFQCTRL_M3KTI_MAIN_REG);
          //
          // clear pe_mode bits first
          //
          M3efqctrl.Bits.pe_mode = 0;
          if((host->var.kti.OutIsocEn == 1 || host->var.common.meRequestedSize != 0) && host->var.kti.SysConfig == SYS_CONFIG_2S2L) {
            M3efqctrl.Bits.pe_mode = 1;
          }

          //
          //  5385563 : Route Through WB winning arbitration without credits blocks local WBs from completing, can cause deadlocks.
          //
          M3efqctrl.Bits.blfqwbroutethruwaitforwbvncrd = 1;

          KtiWritePciCfg (host, (UINT8) TreeNode.Node.SocId, Ctr, M3EFQCTRL_M3KTI_MAIN_REG, M3efqctrl.Data);
        }

        //
        // Program KTI Agent Route Table
        //
        for (Port = 0; Port < host->var.common.KtiPortCnt; Port++) {
          if ((SocketData->Cpu[TreeNode.Node.SocId].LinkData[Port].Valid == TRUE)) {
            Data32 =  SocketData->Route[TreeNode.Node.SocId].KtiAgentRt[Port];
            KtiWritePciCfg (host, (UINT8) TreeNode.Node.SocId, Port, KTIQRT_KTI_LLPMON_REG, Data32);
          }
        }
      } else if (TreeNode.Node.SocType == SOCKET_TYPE_FPGA) {
      } else {
        KTI_ASSERT(FALSE, ERR_SW_CHECK, MINOR_ERR_SW_CHECK_76);
      }
    }
  }

  // Program BC registers
  if ((host->var.kti.RasInProgress != TRUE) || (host->var.kti.ProgramBC == TRUE)) {
    if (KtiInternalGlobal->SnoopFanoutEn == TRUE) {

      //
      // 4929185: Cloned From SKX Si Bug Eco: Snoop fanout related deferral logic in AD FlowQ is returning id's which are not being added to the free list
      //
      if (host->var.common.cpuType == CPU_SKX && host->var.common.stepping < B0_REV_SKX) {
        for (Ctr = 0; Ctr < MAX_SOCKET; Ctr++) {
          if ((SocketData->Cpu[Ctr].Valid == TRUE) && (SocketData->Cpu[Ctr].SocType == SOCKET_TYPE_CPU)) {
            for (Ctr1 = 0; Ctr1 < host->var.kti.CpuInfo[Ctr].TotM3Kti; Ctr1++) {
              M3EfqPrivc2.Data = KtiReadPciCfg (host, Ctr, Ctr1, M3EFQPRIVC2_M3KTI_MAIN_REG);
              M3EfqPrivc2.Bits.kti0disablepwrgatingadegrlistcoarse = 1;
              M3EfqPrivc2.Bits.kti1disablepwrgatingadegrlistcoarse = 1;
              KtiWritePciCfg (host, Ctr, Ctr1, M3EFQPRIVC2_M3KTI_MAIN_REG, M3EfqPrivc2.Data);
            }
          }
        }
      }

      for (TreeIdx = 0; SbspTree[TreeIdx].Node.Valid == TRUE; TreeIdx++) {
        TreeNode = SbspTree[TreeIdx];
        if (TreeNode.Node.SocType == SOCKET_TYPE_CPU) {
          //
          // Program CHA Snoop Fanout
          //
          for (Ctr = 0; Ctr < KtiInternalGlobal->TotCha[TreeNode.Node.SocId]; Ctr++) {
            Data32 = KtiReadPciCfg (host, (UINT8) TreeNode.Node.SocId, Ctr, CHA_RTR_SNP_CTL0_CHA_MISC_REG);
            Data32 = Data32 | SocketData->Route[TreeNode.Node.SocId].ChaKtiFanOut;           // Set send2qpiports_nx
            KtiWritePciCfg (host, (UINT8) TreeNode.Node.SocId, Ctr, CHA_RTR_SNP_CTL0_CHA_MISC_REG, Data32);

            ChaRtrSnpCtr1.Data = KtiReadPciCfg (host, (UINT8) TreeNode.Node.SocId, Ctr, CHA_RTR_SNP_CTL1_CHA_MISC_REG);
            ChaRtrSnpCtr1.Bits.enable_fanout = 0xff;
            KtiWritePciCfg (host, (UINT8) TreeNode.Node.SocId, Ctr, CHA_RTR_SNP_CTL1_CHA_MISC_REG, ChaRtrSnpCtr1.Data);
          }

          //
          // Program M3KTI Route Table & Vn attribute & Snoop Fanout & pe_mode
          //
          for (Ctr = 0; Ctr < host->var.kti.CpuInfo[TreeNode.Node.SocId].TotM3Kti; Ctr++) {
            // Snoop Fanout for P0
            Data32  = SocketData->Route[TreeNode.Node.SocId].M3KtiSnpFanOut[Ctr][0];
            KtiWritePciCfg (host, (UINT8) TreeNode.Node.SocId, Ctr, M3SNPFANOUT_P0_M3KTI_MAIN_REG, Data32);

            // Snoop Fanout for P1
            Data32  = SocketData->Route[TreeNode.Node.SocId].M3KtiSnpFanOut[Ctr][1];
            KtiWritePciCfg (host, (UINT8) TreeNode.Node.SocId, Ctr, M3SNPFANOUT_P1_M3KTI_MAIN_REG, Data32);
          }
        }
      }
    }
  }

  return KTI_SUCCESS;
}


/**

  Routine to program the final IO SAD setting for the given topology.

  @param host  - Pointer to the system host (root) structure
  @param SocketData - Pointer to socket specific data
  @param KtiInternalGlobal - KTIRC Internal Global data

  @retval 0 - Successful
  @retval Other - failure

**/
KTI_STATUS
SetupSystemIoSadEntries (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal
  )
{
  UINT8                 Index;

  KtiCheckPoint(0xFF, 0xFF, 0xFF, STS_PROGRAM_FINAL_IO_SAD, MINOR_STS_FILL_SAD_TGTLST, host);
  //
  // Setup all the IO SAD & Address Map related registers for all sockets/boxes
  //
  for (Index = 0; Index < MAX_SOCKET; Index++) {
    if ((SocketData->Cpu[Index].Valid == TRUE) && (SocketData->Cpu[Index].SocType == SOCKET_TYPE_CPU)) {
      KtiCheckPoint((UINT8)(1 << Index), SOCKET_TYPE_CPU, 0xFF, STS_PROGRAM_FINAL_IO_SAD, MINOR_STS_PROGRAM_CPU_SAD_ENTRIES, host);
      ProgramCpuIoEntries (host, SocketData, KtiInternalGlobal, Index);
      ProgramCpuIoApicEntries (host, SocketData, KtiInternalGlobal, Index);
      ProgramCpuMmiolEntries (host, SocketData, KtiInternalGlobal, Index);
      ProgramCpuMmiohEntries (host, SocketData, KtiInternalGlobal, Index);
      ProgramCpuMiscSadEntries (host, SocketData, KtiInternalGlobal, Index);
      ProgramSystemAddressMap (host, SocketData, KtiInternalGlobal, Index);
    }
  }

  //
  // s5371926/5353027: WA for A-B stepping. Should be fixed in H stepping
  //
  if ((host->var.common.cpuType == CPU_SKX) && (host->var.common.stepping >= H0_REV_SKX)) {
    ProgramMctpBroadcastSettings (host, SocketData, KtiInternalGlobal);
  }
  
  return KTI_SUCCESS;
}

/**

  Routine to program the final IOPORT SAD for the given socket

  @param host  - Pointer to the system host (root) structure
  @param SocketData - Pointer to socket specific data
  @param KtiInternalGlobal - KTIRC Internal Global data
  @param SocId  - Socket to program

  @retval 0 - Successful
  @retval Other - failure

**/
KTI_STATUS
ProgramCpuIoEntries (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal,
  UINT8                  SocId
  )
{
  UINT32    IoPortTargetOffset[MAX_SAD_TARGETS/8] = {IOPORT_TARGET_LIST_CFG_0_CHABC_SAD_REG,
                                                     IOPORT_TARGET_LIST_CFG_1_CHABC_SAD_REG,
                                                     IOPORT_TARGET_LIST_CFG_2_CHABC_SAD_REG,
                                                     IOPORT_TARGET_LIST_CFG_3_CHABC_SAD_REG};

  UINT8     Stack, Socket, TargetIndex, NumberTargets, TmpTarget;
  UINT32    IoTgtLst[MAX_SAD_TARGETS/8] = {0x88888888, 0x88888888, 0x88888888, 0x88888888};

  for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
    //
    // Socket present?
    //
    if (SocketData->Cpu[Socket].Valid == TRUE) {
      for (Stack = 0; Stack < MAX_IIO_STACK; Stack++) {
        //
        // Stack present?
        //
        if (host->var.common.stackPresentBitmap[Socket] & (1 << Stack)) {
          //
          // Build up the 4 target register contents
          //
          if (host->var.kti.CpuInfo[Socket].CpuRes.StackRes[Stack].IoBase < host->var.kti.CpuInfo[Socket].CpuRes.StackRes[Stack].IoLimit) {
            TargetIndex = (UINT8)(host->var.kti.CpuInfo[Socket].CpuRes.StackRes[Stack].IoBase / SAD_LEG_IO_GRANTY);
            NumberTargets = (UINT8)((host->var.kti.CpuInfo[Socket].CpuRes.StackRes[Stack].IoLimit + 1 - host->var.kti.CpuInfo[Socket].CpuRes.StackRes[Stack].IoBase) / SAD_LEG_IO_GRANTY);
            while ((TargetIndex < MAX_SAD_TARGETS) && (NumberTargets > 0)) {
              if (Socket == SocId) {
                TmpTarget = (Stack | SOCKET_LOCAL_NID);
              } else {
                TmpTarget = Socket;
              }
              IoTgtLst[TargetIndex / 8] = ((IoTgtLst[TargetIndex / 8] & ~(0xF << (SADTGT_SIZE * TargetIndex)))  | (TmpTarget << ((TargetIndex%8) * SADTGT_SIZE)));
              TargetIndex++;
              NumberTargets--;
            }
          }
        }
      }
    }
  }

  //
  // Program the IOPORT_TARGET list for this socket
  //
  for (TargetIndex = 0; TargetIndex < MAX_SAD_TARGETS/8; TargetIndex++) {
    KtiWritePciCfg (host, SocId, 0, IoPortTargetOffset[TargetIndex], IoTgtLst[TargetIndex]);
  }

  ProgramCpuIoEntriesKti (host, SocketData, KtiInternalGlobal, SocId);

  return KTI_SUCCESS;
}


/**

  Routine to program the final IOPORT SAD for the given socket

  @param host  - Pointer to the system host (root) structure
  @param SocketData - Pointer to socket specific data
  @param KtiInternalGlobal - KTIRC Internal Global data
  @param SocId  - Socket to program

  @retval 0 - Successful
  @retval Other - failure

**/
KTI_STATUS
ProgramCpuIoEntriesKti (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal,
  UINT8                  SocId
  )
{
  UINT32  IoBaseTmp;
  UINT16  IoLimit;


  UINT32  IoLocalLimits[2] = {0,0};

  UINT32  IoLocalTgtLst = 0;
  UINT8   TargetIndex = 0, LinkIndex, Stack;

  //
  // Setup starting base of legacy IO for the socket
  //
  IoLocalLimits[0] = (UINT32) (host->var.kti.CpuInfo[SocId].CpuRes.IoBase >> 10) & 0x3f;

  for (Stack = 0; Stack < MAX_IIO_STACK; Stack++) {
    //
    // Program a target and limit for each enabled stack
    //
    if (host->var.common.stackPresentBitmap[SocId] & (1 << Stack)) {
      IoBaseTmp = host->var.kti.CpuInfo[SocId].CpuRes.StackRes[Stack].IoBase;
      if (IoBaseTmp != 0xFFFF) {
        IoLocalTgtLst |= Stack << (TargetIndex * 3);
        IoLimit = (host->var.kti.CpuInfo[SocId].CpuRes.StackRes[Stack].IoLimit >> 10) & 0x3f;
        if (TargetIndex < 3) {
          IoLocalLimits[TargetIndex / 3] |= (IoLimit << ((TargetIndex + 1) * 8)) ;
        } else {
          IoLocalLimits[TargetIndex / 3] |= (IoLimit << ((TargetIndex - 3) * 8));
        }
        TargetIndex++;
      }
    }
  }

  //
  // bios s4928965. In KTI VGA override doesn't occur if the Addr falls outside of all 6 LegacyIO
  // ranges and send to incorrect IIO stack
  // workaround by lowering the base to Zero to ensure one range covers the low IO
  // This should only be needed when VGA is on non-SBSP socket
  //
  if ((host->var.common.cpuType == CPU_SKX) && (host->var.common.stepping < B0_REV_SKX)) {
    if (host->var.kti.OutLegacyVgaSoc == SocId) {
      // Set base = 0
      IoLocalLimits[0] = IoLocalLimits[0] & 0xFFFFFFC0;
      if (((IoLocalLimits[0] >> 8) & 0x3f) == 0) {
        // If no IO assigned to this socket, set limit = 2K to cover the legacy VGA range
        IoLocalLimits[0] = IoLocalLimits[0] | 1 << 8;
      }
    }
  }

  for (LinkIndex = 0; LinkIndex < host->var.common.KtiPortCnt; LinkIndex++) {
    if (SocketData->Cpu[SocId].LinkData[LinkIndex].Valid == TRUE){
      KtiWritePciCfg (host, SocId, LinkIndex, KTILK_TAD_IO0_KTI_LLPMON_REG, IoLocalTgtLst);
      KtiWritePciCfg (host, SocId, LinkIndex, KTILK_TAD_IO1_KTI_LLPMON_REG, IoLocalLimits[0]);
      KtiWritePciCfg (host, SocId, LinkIndex, KTILK_TAD_IO2_KTI_LLPMON_REG, IoLocalLimits[1]);
    }
  }

  return KTI_SUCCESS;
}


/**

  Routine to program the final IoApic SAD for given socket

  @param host  - Pointer to the system host (root) structure
  @param SocketData - Pointer to socket specific data
  @param KtiInternalGlobal - KTIRC Internal Global data
  @param SocId  - Socket to program

  @retval 0 - Successful
  @retval Other - failure

**/
KTI_STATUS
ProgramCpuIoApicEntries (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal,
  UINT8                  SocId
  )
{
  UINT32    IoApicTargetOffset[MAX_SAD_TARGETS/8] = {IOAPIC_TARGET_LIST_CFG_0_CHABC_SAD1_REG,
                                                     IOAPIC_TARGET_LIST_CFG_1_CHABC_SAD1_REG,
                                                     IOAPIC_TARGET_LIST_CFG_2_CHABC_SAD1_REG,
                                                     IOAPIC_TARGET_LIST_CFG_3_CHABC_SAD1_REG};

  UINT8     Stack, Socket, TargetIndex = 0, NumberTargets, TmpTarget=8;
  UINT32    IoApicTgtLst[MAX_SAD_TARGETS/8] = {0x88888888, 0x88888888, 0x88888888, 0x88888888};

  for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
    //
    // Socket present?
    //
    if (SocketData->Cpu[Socket].Valid == TRUE) {
      for (Stack = 0; Stack < MAX_IIO_STACK; Stack++) {
        //
        // Stack present?
        //
        if (host->var.common.stackPresentBitmap[Socket] & (1 << Stack)) {
          //
          // Build up the 4 target register contents
          //
          if (host->var.kti.CpuInfo[Socket].CpuRes.StackRes[Stack].IoApicBase < host->var.kti.CpuInfo[Socket].CpuRes.StackRes[Stack].IoApicLimit) {
            TargetIndex = (UINT8)((host->var.kti.CpuInfo[Socket].CpuRes.StackRes[Stack].IoApicBase - IOAPIC_BASE) / SAD_IOAPIC_GRANTY);
            NumberTargets = (UINT8)((host->var.kti.CpuInfo[Socket].CpuRes.StackRes[Stack].IoApicLimit + 1 - host->var.kti.CpuInfo[Socket].CpuRes.StackRes[Stack].IoApicBase) / SAD_IOAPIC_GRANTY);
            while ((TargetIndex < MAX_SAD_TARGETS) && (NumberTargets > 0)) {
              if (Socket == SocId) {
                TmpTarget = (Stack | SOCKET_LOCAL_NID);
              } else {
                TmpTarget = Socket;
              }
              IoApicTgtLst[TargetIndex / 8] = ((IoApicTgtLst[TargetIndex / 8] & ~(0xF << (SADTGT_SIZE * TargetIndex)))  | (TmpTarget << ((TargetIndex%8) * SADTGT_SIZE)));
              TargetIndex++;
              NumberTargets--;
            }
          } else {
            if (TargetIndex >=  MAX_SAD_TARGETS) {
              KtiDebugPrintFatal ((host, KTI_DEBUG_ERROR, "\nERR_SW_CHECK: 0x%X!!!!  TargetIndex: 0x%X is greater or equal to MAX_SAD_TARGETS: 0x%X\n",
                                   MINOR_ERR_SW_CHECK_52, TargetIndex, MAX_SAD_TARGETS));
              KTI_ASSERT (FALSE, ERR_SW_CHECK, MINOR_ERR_SW_CHECK_52);
            }
            TargetIndex++;
          }
        }
      }
    }
  }

  //
  // Program the IOPORT_TARGET list for this socket
  //
  for (TargetIndex = 0; TargetIndex < MAX_SAD_TARGETS/8; TargetIndex++) {
    KtiWritePciCfg (host, SocId, 0, IoApicTargetOffset[TargetIndex], IoApicTgtLst[TargetIndex]);
  }

  ProgramCpuIoApicEntriesKti (host, SocketData, KtiInternalGlobal, SocId);

  return KTI_SUCCESS;
}


/**

  Routine to program the final IoApic SAD for given socket

  @param host  - Pointer to the system host (root) structure
  @param SocketData - Pointer to socket specific data
  @param KtiInternalGlobal - KTIRC Internal Global data
  @param SocId  - Socket to program

  @retval 0 - Successful
  @retval Other - failure

**/
KTI_STATUS
ProgramCpuIoApicEntriesKti (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal,
  UINT8                  SocId
  )
{
  UINT32  IoApicBaseTmp;
  UINT32  IoApicLimit;


  UINT32  IoApicLocalLimits[2] = {0,0};

  UINT32  IoApicLocalTgtLst = 0;
  UINT8   TargetIndex = 0, LinkIndex, Stack;

  //
  // Setup starting base of legacy IO for the socket
  //
  IoApicLocalLimits[0] = (UINT32) (host->var.kti.CpuInfo[SocId].CpuRes.IoApicBase >> 15) & 0x1f;

  for (Stack = 0; Stack < MAX_IIO_STACK; Stack++) {
    //
    // Program a target and limit for each enabled stack
    //
    if (host->var.common.stackPresentBitmap[SocId] & (1 << Stack)) {
      IoApicBaseTmp = host->var.kti.CpuInfo[SocId].CpuRes.StackRes[Stack].IoApicBase;
      if (IoApicBaseTmp != 0xFFFFFFFF) {
        IoApicLocalTgtLst |= Stack << (TargetIndex * 3);
        IoApicLimit = (host->var.kti.CpuInfo[SocId].CpuRes.StackRes[Stack].IoApicLimit >> 15) & 0x1f;
        if (TargetIndex < 3) {
          IoApicLocalLimits[TargetIndex / 3] |= (IoApicLimit << ((TargetIndex + 1) * 8)) ;
        } else {
          IoApicLocalLimits[TargetIndex / 3] |= (IoApicLimit << ((TargetIndex - 3) * 8));
        }
        TargetIndex++;
      }
    }
  }

  for (LinkIndex = 0; LinkIndex < host->var.common.KtiPortCnt; LinkIndex++) {
    if (SocketData->Cpu[SocId].LinkData[LinkIndex].Valid == TRUE){
      KtiWritePciCfg (host, SocId, LinkIndex, KTILK_TAD_IOAPIC0_KTI_LLPMON_REG, IoApicLocalTgtLst);
      KtiWritePciCfg (host, SocId, LinkIndex, KTILK_TAD_IOAPIC1_KTI_LLPMON_REG, IoApicLocalLimits[0]);
      KtiWritePciCfg (host, SocId, LinkIndex, KTILK_TAD_IOAPIC2_KTI_LLPMON_REG, IoApicLocalLimits[1]);
    }
  }

  return KTI_SUCCESS;
}


/**

  Routine to program the final Mmmiol rules for given socket

  @param host  - Pointer to the system host (root) structure
  @param SocketData - Pointer to socket specific data
  @param KtiInternalGlobal - KTIRC Internal Global data
  @param SocId  - Socket to program

  @retval 0 - Successful
  @retval Other - failure

**/
KTI_STATUS
ProgramCpuMmiolEntries (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal,
  UINT8                  SocId
  )
{
  UINT32               MmiolRuleOffset[12][2] = {{MMIO_RULE_CFG_0_N0_CHABC_SAD1_REG, MMIO_RULE_CFG_0_N1_CHABC_SAD1_REG},
                                                 {MMIO_RULE_CFG_1_N0_CHABC_SAD1_REG, MMIO_RULE_CFG_1_N1_CHABC_SAD1_REG},
                                                 {MMIO_RULE_CFG_2_N0_CHABC_SAD1_REG, MMIO_RULE_CFG_2_N1_CHABC_SAD1_REG},
                                                 {MMIO_RULE_CFG_3_N0_CHABC_SAD1_REG, MMIO_RULE_CFG_3_N1_CHABC_SAD1_REG},
                                                 {MMIO_RULE_CFG_4_N0_CHABC_SAD1_REG, MMIO_RULE_CFG_4_N1_CHABC_SAD1_REG},
                                                 {MMIO_RULE_CFG_5_N0_CHABC_SAD1_REG, MMIO_RULE_CFG_5_N1_CHABC_SAD1_REG},
                                                 {MMIO_RULE_CFG_6_N0_CHABC_SAD1_REG, MMIO_RULE_CFG_6_N1_CHABC_SAD1_REG},
                                                 {MMIO_RULE_CFG_7_N0_CHABC_SAD1_REG, MMIO_RULE_CFG_7_N1_CHABC_SAD1_REG},
                                                 {MMIO_RULE_CFG_8_N0_CHABC_SAD1_REG, MMIO_RULE_CFG_8_N1_CHABC_SAD1_REG},
                                                 {MMIO_RULE_CFG_9_N0_CHABC_SAD1_REG, MMIO_RULE_CFG_9_N1_CHABC_SAD1_REG},
                                                 {MMIO_RULE_CFG_10_N0_CHABC_SAD1_REG, MMIO_RULE_CFG_10_N1_CHABC_SAD1_REG},
                                                 {MMIO_RULE_CFG_11_N0_CHABC_SAD1_REG, MMIO_RULE_CFG_11_N1_CHABC_SAD1_REG}};

  UINT32              MmiolTargetOffset[2]    = {MMIO_TARGET_LIST_CFG_0_CHABC_SAD1_REG,
                                                 MMIO_TARGET_LIST_CFG_1_CHABC_SAD1_REG};

  MMIO_RULE_CFG_0_N0_CHABC_SAD1_STRUCT  RuleLo;
  MMIO_RULE_CFG_0_N1_CHABC_SAD1_STRUCT  RuleHi;


  UINT32    MmiolTgtLst[2] = {0x88888888, 0x88888888};
  UINT32    TmpMmioNemRuleBase;
  UINT8     Stack, Socket, TargetIndex = 0, TmpTarget;
  UINT8     TmpTotCpu;

  //
  // Mmio11 is used to cover NEM/legacy Mmio region at 4G-64M to 4G region.
  // Up to 6 rules are used in < 8S for local socket Mmiol to route to each possible IIO stack
  // Up to 4 rules are used in 8S capable parts for local socket Mmiol to route to each possible IIO stack
  // Rest of rules are used to route off socket to other possible 7 cpus
  //

  if (KtiInternalGlobal->KtiCpuSktHotPlugEn == TRUE) {
    if (host->setup.kti.KtiCpuSktHotPlugTopology == 0) {
      TmpTotCpu =  4;         //online/offline based on 4S topology
    } else {
      TmpTotCpu =  8;         //online/offline based on 8S topology
    }
  } else {
    TmpTotCpu =  MAX_SOCKET;
  }

  KTI_ASSERT((TmpTotCpu <= MAX_SOCKET), ERR_SW_CHECK, MINOR_ERR_SW_CHECK_77);

  for (Socket = 0; Socket < TmpTotCpu; Socket++) {
    //
    // Socket present?
    //
    if ((SocketData->Cpu[Socket].Valid == TRUE) || (KtiInternalGlobal->KtiCpuSktHotPlugEn == TRUE)) {
      if (Socket == SocId) {
        for (Stack = 0; Stack < MAX_IIO_STACK; Stack++) {
          //
          // Program a local rule for each enabled stack
          //
          if (host->var.common.stackPresentBitmap[Socket] & (1 << Stack)) {
            if (host->var.kti.CpuInfo[Socket].CpuRes.StackRes[Stack].MmiolBase < host->var.kti.CpuInfo[Socket].CpuRes.StackRes[Stack].MmiolLimit) {
              RuleLo.Data = 1; // Enable the rule
              RuleHi.Data = 0;
              RuleHi.Bits.baseaddress_vlsb_bits = host->var.kti.CpuInfo[Socket].CpuRes.StackRes[Stack].MmiolBase >> 22;
              RuleLo.Bits.baseaddress_lsb_bits = host->var.kti.CpuInfo[Socket].CpuRes.StackRes[Stack].MmiolBase >> 24;
              RuleLo.Bits.baseaddress = host->var.kti.CpuInfo[Socket].CpuRes.StackRes[Stack].MmiolBase >> 26;
              RuleHi.Bits.limitaddress_vlsb_bits = host->var.kti.CpuInfo[Socket].CpuRes.StackRes[Stack].MmiolLimit >> 22;
              RuleLo.Bits.limitaddress_lsb_bits = host->var.kti.CpuInfo[Socket].CpuRes.StackRes[Stack].MmiolLimit >> 24;
              RuleLo.Bits.limitaddress = host->var.kti.CpuInfo[Socket].CpuRes.StackRes[Stack].MmiolLimit >> 26;
              KtiWritePciCfg (host, SocId, 0, MmiolRuleOffset[TargetIndex][1], RuleHi.Data);
              KtiWritePciCfg (host, SocId, 0, MmiolRuleOffset[TargetIndex][0], RuleLo.Data);
              TmpTarget = (Stack | SOCKET_LOCAL_NID);
              MmiolTgtLst[TargetIndex / 8] = ((MmiolTgtLst[TargetIndex / 8] & ~(0xF << (SADTGT_SIZE * TargetIndex)))  | (TmpTarget << ((TargetIndex%8) * SADTGT_SIZE)));
              TargetIndex++;
            }
          }
        }
      } else {

        //
        // Program off socket rule
        //
        // MMIOL Rule
        if (host->var.kti.CpuInfo[Socket].CpuRes.MmiolBase < host->var.kti.CpuInfo[Socket].CpuRes.MmiolLimit) {
          RuleLo.Data = 1; // Enable the rule
          RuleHi.Data = 0;
          RuleHi.Bits.baseaddress_vlsb_bits = host->var.kti.CpuInfo[Socket].CpuRes.MmiolBase >> 22;
          RuleLo.Bits.baseaddress_lsb_bits = host->var.kti.CpuInfo[Socket].CpuRes.MmiolBase >> 24;
          RuleLo.Bits.baseaddress = host->var.kti.CpuInfo[Socket].CpuRes.MmiolBase >> 26;
          RuleHi.Bits.limitaddress_vlsb_bits = host->var.kti.CpuInfo[Socket].CpuRes.MmiolLimit >> 22;
          RuleLo.Bits.limitaddress_lsb_bits = host->var.kti.CpuInfo[Socket].CpuRes.MmiolLimit >> 24;
          RuleLo.Bits.limitaddress = host->var.kti.CpuInfo[Socket].CpuRes.MmiolLimit >> 26;
          KtiWritePciCfg (host, SocId, 0, MmiolRuleOffset[TargetIndex][1], RuleHi.Data);
          KtiWritePciCfg (host, SocId, 0, MmiolRuleOffset[TargetIndex][0], RuleLo.Data);
          if (SocketData->Cpu[Socket].Valid == FALSE) {
            TmpTarget = SOCKET_LOCAL_NID;
          } else {
            TmpTarget = Socket;
          }
          MmiolTgtLst[TargetIndex / 8] = ((MmiolTgtLst[TargetIndex / 8] & ~(0xF << (SADTGT_SIZE * TargetIndex)))  | (TmpTarget << ((TargetIndex%8) * SADTGT_SIZE)));
          TargetIndex++;
        }
      }
    }
  }

  if (KtiInternalGlobal->IsMmcfgAboveMmiol == TRUE) {
    //
    // Handle nonval case where OEM attempts to put mmiol below mmcfg.
    //
    RuleLo.Data = KtiReadPciCfg (host, SocId, 0, MMIO_RULE_CFG_11_N0_CHABC_SAD1_REG);
    RuleHi.Data = KtiReadPciCfg (host, SocId, 0, MMIO_RULE_CFG_11_N1_CHABC_SAD1_REG);
    //
    // To cover any gap, lower Rule11 to start at end of mmcfg for NEM/Ego coverage and point at legacy cstack
    //
    TmpMmioNemRuleBase = KtiInternalGlobal->MmcfgBase + host->setup.common.mmCfgSize + 1;
    RuleHi.Bits.baseaddress_vlsb_bits = TmpMmioNemRuleBase >> 22;
    RuleLo.Bits.baseaddress_lsb_bits = TmpMmioNemRuleBase >> 24;
    RuleLo.Bits.baseaddress = TmpMmioNemRuleBase >> 26;

    KtiWritePciCfg (host, SocId, 0, MMIO_RULE_CFG_11_N0_CHABC_SAD1_REG, RuleLo.Data);
    KtiWritePciCfg (host, SocId, 0, MMIO_RULE_CFG_11_N1_CHABC_SAD1_REG, RuleHi.Data);

  }

  //
  // Always point rule11 to legacy cstack
  //
  if (SocId != KtiInternalGlobal->SbspSoc) {
    MmiolTgtLst[1] = ((MmiolTgtLst[1] & 0xFFFF0FFF) | KtiInternalGlobal->SbspSoc << 12);
  }

  //
  // Program the MMIO_TARGET list for this socket
  //
  for (TargetIndex = 0; TargetIndex < 2; TargetIndex++) {
    KtiWritePciCfg (host, SocId, 0, MmiolTargetOffset[TargetIndex], MmiolTgtLst[TargetIndex]);
  }

  ProgramCpuMmiolEntriesIio (host, SocketData, KtiInternalGlobal, SocId);
  ProgramCpuMmiolEntriesKti (host, SocketData, KtiInternalGlobal, SocId);

  return KTI_SUCCESS;

}

/**

  Routine to program the final Mmmiol rules for given socket

  @param host  - Pointer to the system host (root) structure
  @param SocketData - Pointer to socket specific data
  @param KtiInternalGlobal - KTIRC Internal Global data
  @param SocId  - Socket to program

  @retval 0 - Successful
  @retval Other - failure

**/
KTI_STATUS
ProgramCpuMmiolEntriesIio (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal,
  UINT8                  SocId
  )
{
  UINT32               MmiolRuleOffset[12]    = {MMIOL_RULE0_IIO_VTD_REG,
                                                 MMIOL_RULE1_IIO_VTD_REG,
                                                 MMIOL_RULE2_IIO_VTD_REG,
                                                 MMIOL_RULE3_IIO_VTD_REG,
                                                 MMIOL_RULE4_IIO_VTD_REG,
                                                 MMIOL_RULE5_IIO_VTD_REG,
                                                 MMIOL_RULE6_IIO_VTD_REG,
                                                 MMIOL_RULE7_IIO_VTD_REG,
                                                 MMIOL_RULE8_IIO_VTD_REG,
                                                 MMIOL_RULE9_IIO_VTD_REG,
                                                 MMIOL_RULE10_IIO_VTD_REG,
                                                 MMIOL_RULE11_IIO_VTD_REG};

  UINT32              MmiolTargetOffset[2]    = {MMIOL_TGT_LIST0_IIO_VTD_REG,
                                                 MMIOL_TGT_LIST1_IIO_VTD_REG};

  MMIOL_RULE0_IIO_VTD_STRUCT    Rule[12];
  SAD_CONTROL_IIO_VTD_STRUCT    SadControlIio;
  UINT32    MmiolTgtLst[2] = {0x88888888, 0x00008888};
  UINT8     Stack, Socket, TargetIndex = 0, TmpTarget;
  UINT8     MaxSocket;
  UINT32    TmpMmioNemRuleBase;

  MemSetLocal ((UINT8 *) &Rule, 0x00, (sizeof (MMIOL_RULE0_IIO_VTD_STRUCT) * 12));

  if (KtiInternalGlobal->KtiCpuSktHotPlugEn == TRUE) {
    if (host->setup.kti.KtiCpuSktHotPlugTopology == 0) {
      MaxSocket =  4;         //online/offline based on 4S topology
    } else {
      MaxSocket =  8;         //online/offline based on 8S topology
    }
  } else {
    MaxSocket =  MAX_SOCKET;
  }

  KTI_ASSERT((MaxSocket <= MAX_SOCKET), ERR_SW_CHECK, MINOR_ERR_SW_CHECK_78);


  //
  // Mmio11 is used to cover NEM/legacy Mmio region at 4G-64M to 4G region.
  // Up to 6 rules are used in < 8S for local socket Mmiol to route to each possible IIO stack
  // Up to 4 rules are used in 8S capable parts for local socket Mmiol to route to each possible IIO stack
  // Rest of rules are used to route off socket to other possible 7 cpus
  //
  for (Socket = 0; Socket < MaxSocket; Socket++) {
    //
    // Socket present?
    //
    if ((SocketData->Cpu[Socket].Valid == TRUE) || (KtiInternalGlobal->KtiCpuSktHotPlugEn == TRUE)) {
      if (Socket == SocId) {
        for (Stack = 0; Stack < MAX_IIO_STACK; Stack++) {
          //
          // Program a local rule for each enabled stack
          //
          if (host->var.common.stackPresentBitmap[Socket] & (1 << Stack)) {
            if (host->var.kti.CpuInfo[Socket].CpuRes.StackRes[Stack].MmiolBase < host->var.kti.CpuInfo[Socket].CpuRes.StackRes[Stack].MmiolLimit) {
              Rule[TargetIndex].Data = 1; // Enable the rule
              Rule[TargetIndex].Bits.baseaddress = host->var.kti.CpuInfo[Socket].CpuRes.StackRes[Stack].MmiolBase >> 22;
              Rule[TargetIndex].Bits.limitaddress = host->var.kti.CpuInfo[Socket].CpuRes.StackRes[Stack].MmiolLimit >> 22;
              Rule[TargetIndex].Bits.serializerule = 1;
              TmpTarget = (Stack | SOCKET_LOCAL_NID);
              MmiolTgtLst[TargetIndex / 8] = ((MmiolTgtLst[TargetIndex / 8] & ~(0xF << (SADTGT_SIZE * TargetIndex)))  | (TmpTarget << ((TargetIndex%8) * SADTGT_SIZE)));
              TargetIndex++;
            }
          }
        }
      } else {

        //
        // Program off socket rule
        //
        // MMIOL Rule
        if (host->var.kti.CpuInfo[Socket].CpuRes.MmiolBase < host->var.kti.CpuInfo[Socket].CpuRes.MmiolLimit) {
          Rule[TargetIndex].Bits.ruleenable = 1; // Enable the rule
          Rule[TargetIndex].Bits.baseaddress = host->var.kti.CpuInfo[Socket].CpuRes.MmiolBase >> 22;
          Rule[TargetIndex].Bits.limitaddress = host->var.kti.CpuInfo[Socket].CpuRes.MmiolLimit >> 22;
          Rule[TargetIndex].Bits.serializerule = 2;
          if (SocketData->Cpu[Socket].Valid == TRUE) {
            TmpTarget = Socket;
          } else {
            //
            // Program to local DMI for hotplug reservaton
            //
            TmpTarget = SOCKET_LOCAL_NID;
          }
          MmiolTgtLst[TargetIndex / 8] = ((MmiolTgtLst[TargetIndex / 8] & ~(0xF << (SADTGT_SIZE * TargetIndex)))  | (TmpTarget << ((TargetIndex%8) * SADTGT_SIZE)));
          TargetIndex++;
        }
      }
    }
  }

  //
  // Program Rule11 for NEM/Ego coverage and point locally
  //
  Rule[11].Bits.ruleenable = 1; // Enable the rule
  Rule[11].Bits.baseaddress = (MMIOL_LIMIT + 1) >> 22;
  Rule[11].Bits.limitaddress = 0xFFFFFFFF >> 22;

  if (KtiInternalGlobal->IsMmcfgAboveMmiol == TRUE) {
    //
    // Handle nonval case where OEM attempts to put mmiol below mmcfg.
    // To cover any gap, lower Rule11 to start at end of mmcfg for NEM/Ego coverage and point locally
    //
    TmpMmioNemRuleBase = KtiInternalGlobal->MmcfgBase + host->setup.common.mmCfgSize + 1;
    Rule[11].Bits.baseaddress = TmpMmioNemRuleBase >> 22;
  }

  //
  // Always point rule11 to legacy cstack
  //
  if (SocId != KtiInternalGlobal->SbspSoc) {
    MmiolTgtLst[1] = ((MmiolTgtLst[1] & 0xFFFF0FFF) | KtiInternalGlobal->SbspSoc << 12);
  }

  for (Stack = 0; Stack < MAX_IIO_STACK; Stack++) {
    if (host->var.common.stackPresentBitmap[SocId] & (1 << Stack)) {
      //
      // Program the MmioRules
      //
      for (TargetIndex = 0; TargetIndex < 12; TargetIndex++) {
        KtiWritePciCfg (host, SocId, Stack, MmiolRuleOffset[TargetIndex], Rule[TargetIndex].Data);
      }
      //
      // Program the MMIO_TARGET list for this socket
      //
      for (TargetIndex = 0; TargetIndex < 2; TargetIndex++) {
        KtiWritePciCfg (host, SocId, Stack, MmiolTargetOffset[TargetIndex], MmiolTgtLst[TargetIndex]);
      }

      //
      // Mmiol granularity.
      // lsb of address check for serialize rule 2 is mmiol_granularity + 22.
      // So when allocating low MMIO with a minimum granularity of 4MB per IIO stack program to 0, for 8MB program to 1, etc.
      //
      SadControlIio.Data = KtiReadPciCfg (host, SocId, Stack, SAD_CONTROL_IIO_VTD_REG);
      SadControlIio.Bits.mmiol_granularity = KtiInternalGlobal->MmiolGranularity - 22;
      KtiWritePciCfg (host, SocId, Stack, SAD_CONTROL_IIO_VTD_REG, SadControlIio.Data);
    }
  }

  return KTI_SUCCESS;

}


/**

  Routine to program the final Mmmiol rules for given socket

  @param host  - Pointer to the system host (root) structure
  @param SocketData - Pointer to socket specific data
  @param KtiInternalGlobal - KTIRC Internal Global data
  @param SocId  - Socket to program

  @retval 0 - Successful
  @retval Other - failure

**/
KTI_STATUS
ProgramCpuMmiolEntriesKti (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal,
  UINT8                  SocId
  )
{

  UINT32  MmiolBaseTmp;
  UINT16  MmiolLimit;
  KTILK_TAD_MMIOL1_KTI_LLPMON_STRUCT MmiolLocalLimits[3];
  UINT32    MmiolLocalTgtLst = 0;
  UINT8     TargetIndex = 0, LinkIndex, Stack;

  MemSetLocal ((UINT8 *) &MmiolLocalLimits, 0x00, (sizeof (KTILK_TAD_MMIOL1_KTI_LLPMON_STRUCT) * 3));

  for (Stack = 0; Stack < MAX_IIO_STACK; Stack++) {
    //
    // Program a target and limit for each enabled stack
    //
    if (host->var.common.stackPresentBitmap[SocId] & (1 << Stack)) {
      MmiolBaseTmp = host->var.kti.CpuInfo[SocId].CpuRes.StackRes[Stack].MmiolBase;
      if (MmiolBaseTmp != 0xFFFFFFFF) {
        MmiolLocalTgtLst |= Stack << (TargetIndex * 3);
        MmiolLimit = host->var.kti.CpuInfo[SocId].CpuRes.StackRes[Stack].MmiolLimit >> 22;
        if (TargetIndex % 2) {
          MmiolLocalLimits[TargetIndex / 2].Bits.mmiol_limit1 = MmiolLimit;
        } else {
          MmiolLocalLimits[TargetIndex / 2].Bits.mmiol_limit0 = MmiolLimit;
        }
        TargetIndex++;
      }
    }
  }

  for (LinkIndex = 0; LinkIndex < host->var.common.KtiPortCnt; LinkIndex++) {
    if (SocketData->Cpu[SocId].LinkData[LinkIndex].Valid == TRUE){
      KtiWritePciCfg (host, SocId, LinkIndex, KTILK_TAD_MMIOL0_KTI_LLPMON_REG, MmiolLocalTgtLst);
      KtiWritePciCfg (host, SocId, LinkIndex, KTILK_TAD_MMIOL1_KTI_LLPMON_REG, MmiolLocalLimits[0].Data);
      KtiWritePciCfg (host, SocId, LinkIndex, KTILK_TAD_MMIOL2_KTI_LLPMON_REG, MmiolLocalLimits[1].Data);
      KtiWritePciCfg (host, SocId, LinkIndex, KTILK_TAD_MMIOL3_KTI_LLPMON_REG, MmiolLocalLimits[2].Data);
    }
  }

  return KTI_SUCCESS;
}


/**

  Routine to program the final IO SAD setting for the given topology.

  @param host  - Pointer to the system host (root) structure
  @param SocketData - Pointer to socket specific data
  @param KtiInternalGlobal - KTIRC Internal Global data
  @param SocId  - Socket to program

  @retval 0 - Successful
  @retval Other - failure

**/
KTI_STATUS
ProgramCpuMmiohEntries (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal,
  UINT8                  SocId
  )
{

  UINT32 MmiohTargetOffset[MAX_SAD_TARGETS/8] = {MMIO_TARGET_INTERLEAVE_LIST_CFG_0_CHABC_SAD1_REG,
                                                 MMIO_TARGET_INTERLEAVE_LIST_CFG_1_CHABC_SAD1_REG,
                                                 MMIO_TARGET_INTERLEAVE_LIST_CFG_2_CHABC_SAD1_REG,
                                                 MMIO_TARGET_INTERLEAVE_LIST_CFG_3_CHABC_SAD1_REG};


  //
  // Even though only allocating mmioh as needed, initialize unused target entries to point to local dmi
  //
  UINT32    MmiohTgtLst[MAX_SAD_TARGETS/8] = {0x88888888, 0x88888888, 0x88888888, 0x88888888};

  MMIOH_INTERLEAVE_RULE_N0_CHABC_SAD1_STRUCT  MmiohInterleaveRule0;
  MMIOH_INTERLEAVE_RULE_N1_CHABC_SAD1_STRUCT  MmiohInterleaveRule1;
  MMIOH_NONINTERLEAVE_RULE_N0_CHABC_SAD1_STRUCT MmiohNonInterleaveRule0;
  MMIOH_NONINTERLEAVE_RULE_N1_CHABC_SAD1_STRUCT MmiohNonInterleaveRule1;

  UINT8     Stack, Socket, TargetIndex = 0, TmpTarget;
  UINT64_STRUCT MmiohBaseTmp, MmiohLimitTmp;
  UINT64_STRUCT MmiohGranularitySize;
  UINT64_STRUCT MmiohTmp, MmiohTmp1;
  UINT16    MmiohSize;
  UINT8     MaxSocket;

  if (KtiInternalGlobal->KtiCpuSktHotPlugEn == TRUE) {
    if (host->setup.kti.KtiCpuSktHotPlugTopology == 0) {
      MaxSocket =  4;         //online/offline based on 4S topology
    } else {
      MaxSocket =  8;         //online/offline based on 8S topology
    }
  } else {
    MaxSocket =  MAX_SOCKET;
  }

  KTI_ASSERT((MaxSocket <= MAX_SOCKET), ERR_SW_CHECK, MINOR_ERR_SW_CHECK_79);

  MmiohInterleaveRule0.Data = 0;
  MmiohInterleaveRule1.Data = 0;
  MmiohNonInterleaveRule0.Data = 0;
  MmiohNonInterleaveRule1.Data = 0;

  //
  // Assumes Sbsp always has some mmioh assigned
  //
  MmiohInterleaveRule0.Bits.baseaddress = (UINT16)(host->var.kti.CpuInfo[KtiInternalGlobal->SbspSoc].CpuRes.MmiohBase.lo >> 26);
  MmiohInterleaveRule0.Bits.baseaddress_lsb_bits = (UINT16)(host->var.kti.CpuInfo[KtiInternalGlobal->SbspSoc].CpuRes.MmiohBase.lo >> 24);
  MmiohInterleaveRule0.Bits.baseaddress |= (UINT32)(host->var.kti.CpuInfo[KtiInternalGlobal->SbspSoc].CpuRes.MmiohBase.hi << 6);
  MmiohLimitTmp = host->var.kti.CpuInfo[KtiInternalGlobal->SbspSoc].CpuRes.MmiohLimit;

  //
  // Mmioh granularity size.
  // Interleave size = 2**(size*2) GB.
  //
  MmiohSize = KtiInternalGlobal->MmiohTargetSize;
  MmiohGranularitySize.lo = 1;
  MmiohGranularitySize.hi = 0;
  //
  // Calc actual size
  //
  MmiohGranularitySize = LShiftUINT64(MmiohGranularitySize, (2 * MmiohSize) + 30);
  MmiohInterleaveRule1.Bits.interleave_mode = MmiohSize;
  MmiohInterleaveRule0.Bits.rule_enable = 1;

  //
  // Mmioh is enabled via the MMIOH_INTERLEAVE_RULE and MMIOH_TARGET_INTERLEAVE registers
  // Interleaved rule must exist within confines of the non-interleaved rule.   Can use the entire range or a subset.
  //

  //
  // Handle SBSP first, or if using both decoders, only allocate the local socket in the interleaved decoder
  //
  if (KtiInternalGlobal->UseNonInterleavedMmioh) {
    if (SocId == KtiInternalGlobal->SbspSoc) {
      KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\n Mapping Mmioh using both decoders"));
    }
    Socket = SocId;
  } else {
    Socket = KtiInternalGlobal->SbspSoc;
  }

  //
  // s5332834, The address bits used to index into the target list are [(30 +2 * mode +4): (30 +2 * mode)],
  //
  MmiohBaseTmp= RShiftUINT64(host->var.kti.CpuInfo[KtiInternalGlobal->SbspSoc].CpuRes.MmiohBase, ((2 * MmiohSize) + 30));
  TargetIndex = (UINT8)(0x1F & (UINT8)MmiohBaseTmp.lo);

  for (Stack = 0; Stack < MAX_IIO_STACK; Stack++) {
    //
    // Program a target for each enabled stack
    //
    MmiohBaseTmp = host->var.kti.CpuInfo[Socket].CpuRes.StackRes[Stack].MmiohBase;
    if ((MmiohBaseTmp.hi != 0xFFFFFFFF) && (MmiohBaseTmp.lo != 0xFFFFFFFF)) {
      if (Socket == SocId) {
        TmpTarget = (Stack | SOCKET_LOCAL_NID);
      } else {
        TmpTarget = Socket;
      }

      MmiohTmp1 = AddUINT64x32(host->var.kti.CpuInfo[Socket].CpuRes.StackRes[Stack].MmiohLimit, 1);
      MmiohTmp = host->var.kti.CpuInfo[Socket].CpuRes.StackRes[Stack].MmiohBase;
      while ((MmiohTmp.lo != MmiohTmp1.lo) || (MmiohTmp.hi != MmiohTmp1.hi) ) {
        MmiohTgtLst[TargetIndex / 8] = ((MmiohTgtLst[TargetIndex / 8] & ~(0xF << (SADTGT_SIZE * TargetIndex)))  | (TmpTarget << ((TargetIndex%8) * SADTGT_SIZE)));
        TargetIndex++;
        TargetIndex &= 0x1F;
        MmiohTmp = AddUINT64(MmiohTmp, MmiohGranularitySize);
      }
    }
  }


  if (KtiInternalGlobal->UseNonInterleavedMmioh == FALSE) {
    //
    // Process other sockets normally
    //
    for (Socket = 0; Socket < MaxSocket; Socket++) {
      if (Socket == KtiInternalGlobal->SbspSoc) {
        continue;
      }
      //
      // Socket present?
      //
      if ((SocketData->Cpu[Socket].Valid == TRUE) || (KtiInternalGlobal->KtiCpuSktHotPlugEn == TRUE)) {
        MmiohBaseTmp = host->var.kti.CpuInfo[Socket].CpuRes.MmiohBase;
        //
        // Don't process is socket starved
        //
        if ((MmiohBaseTmp.hi != 0xFFFFFFFF) && (MmiohBaseTmp.lo != 0xFFFFFFFF)) {
          MmiohLimitTmp = host->var.kti.CpuInfo[Socket].CpuRes.MmiohLimit;

          for (Stack = 0; Stack < MAX_IIO_STACK; Stack++) {
            //
            // Program a target for each enabled stack
            //
            MmiohBaseTmp = host->var.kti.CpuInfo[Socket].CpuRes.StackRes[Stack].MmiohBase;
            if ((MmiohBaseTmp.hi != 0xFFFFFFFF) && (MmiohBaseTmp.lo != 0xFFFFFFFF)) {
              if (SocketData->Cpu[Socket].Valid == TRUE) {
                if (Socket == SocId) {
                  TmpTarget = (Stack | SOCKET_LOCAL_NID);
                } else {
                  TmpTarget = Socket;
                }
              } else {
                //
                // Program to local DMI for hotplug reservaton
                //
                TmpTarget = SOCKET_LOCAL_NID;
              }

              MmiohTmp1 = AddUINT64x32(host->var.kti.CpuInfo[Socket].CpuRes.StackRes[Stack].MmiohLimit, 1);
              MmiohTmp = host->var.kti.CpuInfo[Socket].CpuRes.StackRes[Stack].MmiohBase;
              while ((MmiohTmp.lo != MmiohTmp1.lo) || (MmiohTmp.hi != MmiohTmp1.hi) ) {
                MmiohTgtLst[TargetIndex / 8] = ((MmiohTgtLst[TargetIndex / 8] & ~(0xF << (SADTGT_SIZE * TargetIndex)))  | (TmpTarget << ((TargetIndex%8) * SADTGT_SIZE)));
                TargetIndex++;
                TargetIndex &= 0x1F;
                MmiohTmp = AddUINT64(MmiohTmp, MmiohGranularitySize);
              }
            }
          }
        }
      }
    }

    MmiohInterleaveRule0.Bits.limitaddress_lsb_bits = (UINT16)(MmiohLimitTmp.lo >> 24);
    MmiohInterleaveRule0.Bits.limitaddress = (UINT16)(MmiohLimitTmp.lo >> 26);
    MmiohInterleaveRule1.Bits.limitaddress = (UINT16)(MmiohLimitTmp.hi);

    //
    // Update the noninterleaved rule
    //
    MmiohNonInterleaveRule0.Bits.rule_enable = 1;
    MmiohNonInterleaveRule0.Bits.baseaddress = MmiohInterleaveRule0.Bits.baseaddress;
    MmiohNonInterleaveRule0.Bits.baseaddress_lsb_bits = MmiohInterleaveRule0.Bits.baseaddress_lsb_bits;
    MmiohNonInterleaveRule0.Bits.limitaddress_lsb_bits = MmiohInterleaveRule0.Bits.limitaddress_lsb_bits;
    MmiohNonInterleaveRule0.Bits.limitaddress = MmiohInterleaveRule0.Bits.limitaddress;
    MmiohNonInterleaveRule1.Bits.limitaddress = MmiohInterleaveRule1.Bits.limitaddress;
    MmiohNonInterleaveRule1.Bits.target = SOCKET_LOCAL_NID;
  } else {
    //
    // Handle dual decoders
    //
    if (SocId == KtiInternalGlobal->SbspSoc) {
      //
      // Programming local to SBSP, so align the non-interleaved and interleavedrule to common base
      // and set the mmioh interleaved limit to limit of local socket
      //
      MmiohInterleaveRule0.Bits.limitaddress_lsb_bits = (UINT16)(host->var.kti.CpuInfo[SocId].CpuRes.MmiohLimit.lo >> 24);
      MmiohInterleaveRule0.Bits.limitaddress = (UINT16)(host->var.kti.CpuInfo[SocId].CpuRes.MmiohLimit.lo >> 26);
      MmiohInterleaveRule1.Bits.limitaddress = (UINT16)(host->var.kti.CpuInfo[SocId].CpuRes.MmiohLimit.hi);

      MmiohNonInterleaveRule0.Bits.rule_enable = 1;
      MmiohNonInterleaveRule0.Bits.baseaddress = MmiohInterleaveRule0.Bits.baseaddress;
      MmiohNonInterleaveRule0.Bits.baseaddress_lsb_bits = MmiohInterleaveRule0.Bits.baseaddress_lsb_bits;

      //
      // Update the noninterleaved rule
      // Start by finding the second socket
      //
      for (Socket = 0; Socket < MaxSocket; Socket++) {
        if (Socket == KtiInternalGlobal->SbspSoc) {
          continue;
        }
        //
        // Socket present?
        //
        if ((SocketData->Cpu[Socket].Valid == TRUE) || (KtiInternalGlobal->KtiCpuSktHotPlugEn == TRUE)) {

          MmiohNonInterleaveRule0.Bits.limitaddress_lsb_bits = (UINT16)(host->var.kti.CpuInfo[Socket].CpuRes.MmiohLimit.lo >> 24);
          MmiohNonInterleaveRule0.Bits.limitaddress = (UINT16)(host->var.kti.CpuInfo[Socket].CpuRes.MmiohLimit.lo >> 26);
          MmiohNonInterleaveRule1.Bits.limitaddress = (UINT16)(host->var.kti.CpuInfo[Socket].CpuRes.MmiohLimit.hi);
          MmiohNonInterleaveRule1.Bits.target = Socket;
          break;
        }
      }
    } else {
      //
      // Programming local to non-sbsp, so align the interleaverule offset into the non-interleaved
      //
      MmiohInterleaveRule0.Bits.limitaddress_lsb_bits = (UINT16)(host->var.kti.CpuInfo[SocId].CpuRes.MmiohLimit.lo >> 24);
      MmiohInterleaveRule0.Bits.limitaddress = (UINT16)(host->var.kti.CpuInfo[SocId].CpuRes.MmiohLimit.lo >> 26);
      MmiohInterleaveRule1.Bits.limitaddress = (UINT16)(host->var.kti.CpuInfo[SocId].CpuRes.MmiohLimit.hi);

      MmiohInterleaveRule0.Bits.baseaddress = (UINT16)(host->var.kti.CpuInfo[SocId].CpuRes.MmiohBase.lo >> 26);
      MmiohInterleaveRule0.Bits.baseaddress_lsb_bits = (UINT16)(host->var.kti.CpuInfo[SocId].CpuRes.MmiohBase.lo >> 24);
      MmiohInterleaveRule0.Bits.baseaddress |= (UINT32)(host->var.kti.CpuInfo[SocId].CpuRes.MmiohBase.hi << 6);

      //
      // Update the noninterleaved rule
      //
      Socket = KtiInternalGlobal->SbspSoc;
      MmiohNonInterleaveRule0.Bits.rule_enable = 1;
      MmiohNonInterleaveRule0.Bits.baseaddress = (UINT16)(host->var.kti.CpuInfo[Socket].CpuRes.MmiohBase.lo >> 26);
      MmiohNonInterleaveRule0.Bits.baseaddress_lsb_bits  = (UINT16)(host->var.kti.CpuInfo[Socket].CpuRes.MmiohBase.lo >> 24);
      MmiohNonInterleaveRule0.Bits.baseaddress |= (UINT32)(host->var.kti.CpuInfo[Socket].CpuRes.MmiohBase.hi << 6);

      MmiohNonInterleaveRule0.Bits.limitaddress_lsb_bits = (UINT16)(host->var.kti.CpuInfo[SocId].CpuRes.MmiohLimit.lo >> 24);
      MmiohNonInterleaveRule0.Bits.limitaddress = (UINT16)(host->var.kti.CpuInfo[SocId].CpuRes.MmiohLimit.lo >> 26);
      MmiohNonInterleaveRule1.Bits.limitaddress = (UINT16)(host->var.kti.CpuInfo[SocId].CpuRes.MmiohLimit.hi);
      MmiohNonInterleaveRule1.Bits.target = Socket;
    }
  }

  //
  // Program the MMIO_TARGET_INTERLEAVE list for this socket
  //
  for (TargetIndex = 0; TargetIndex < (MAX_SAD_TARGETS/8); TargetIndex++) {
    KtiWritePciCfg (host, SocId, 0, MmiohTargetOffset[TargetIndex], MmiohTgtLst[TargetIndex]);
  }

  //
  // Program the rules
  //
  KtiWritePciCfg (host, SocId, 0, MMIOH_INTERLEAVE_RULE_N1_CHABC_SAD1_REG, MmiohInterleaveRule1.Data);
  KtiWritePciCfg (host, SocId, 0, MMIOH_INTERLEAVE_RULE_N0_CHABC_SAD1_REG, MmiohInterleaveRule0.Data);

  KtiWritePciCfg (host, SocId, 0, MMIOH_NONINTERLEAVE_RULE_N1_CHABC_SAD1_REG, MmiohNonInterleaveRule1.Data);
  KtiWritePciCfg (host, SocId, 0, MMIOH_NONINTERLEAVE_RULE_N0_CHABC_SAD1_REG, MmiohNonInterleaveRule0.Data);


  // Program entries in IIO and KTI
  ProgramCpuMmiohEntriesIio (host, SocketData, KtiInternalGlobal, SocId);
  ProgramCpuMmiohEntriesKti (host, SocketData, KtiInternalGlobal, SocId);

  return KTI_SUCCESS;
}


/**

  Routine to program the final IO SAD setting for the given topology.

  @param host  - Pointer to the system host (root) structure
  @param SocketData - Pointer to socket specific data
  @param KtiInternalGlobal - KTIRC Internal Global data
  @param SocId  - Socket to program

  @retval 0 - Successful
  @retval Other - failure

**/
KTI_STATUS
ProgramCpuMmiohEntriesIio (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal,
  UINT8                  SocId
  )
{

  UINT32 MmiohTargetOffset[MAX_SAD_TARGETS/8] = {MMIOH_INTLV_TGT_LIST0_IIO_VTD_REG,
                                                 MMIOH_INTLV_TGT_LIST1_IIO_VTD_REG,
                                                 MMIOH_INTLV_TGT_LIST2_IIO_VTD_REG,
                                                 MMIOH_INTLV_TGT_LIST3_IIO_VTD_REG};

  UINT32 ChaMmiohTargetRegs[MAX_SAD_TARGETS/8]= {MMIO_TARGET_INTERLEAVE_LIST_CFG_0_CHABC_SAD1_REG,
                                                 MMIO_TARGET_INTERLEAVE_LIST_CFG_1_CHABC_SAD1_REG,
                                                 MMIO_TARGET_INTERLEAVE_LIST_CFG_2_CHABC_SAD1_REG,
                                                 MMIO_TARGET_INTERLEAVE_LIST_CFG_3_CHABC_SAD1_REG};


  //
  // Even though only allocating mmioh as needed, initialize unused target entries to point to local dmi
  //
  UINT32 MmiohTgtLst[MAX_SAD_TARGETS/8]       = {0x88888888, 0x88888888, 0x88888888, 0x88888888};

  MMIOH_INTERLEAVE_N0_IIO_VTD_STRUCT            IioMmiohInterleaveRule0;
  MMIOH_INTERLEAVE_N1_IIO_VTD_STRUCT            IioMmiohInterleaveRule1;

  MMIOH_NON_INTERLEAVE_N0_IIO_VTD_STRUCT        IioMmiohNonInterleaveRule0;
  MMIOH_NON_INTERLEAVE_N1_IIO_VTD_STRUCT        IioMmiohNonInterleaveRule1;
  MMIOL_TGT_LIST1_IIO_VTD_STRUCT                IioMmiohNonInterleavedTarget;

  SAD_CONTROL_IIO_VTD_STRUCT                    SadControlIio;


  UINT8     Stack, Socket, TargetIndex = 0;
  UINT64_STRUCT MmiohLimitTmp;
  UINT16    MmiohSize;
  UINT8     MaxSocket;
  UINT8     MmiohNonInterleaveTgt = SOCKET_LOCAL_NID;

  if (KtiInternalGlobal->KtiCpuSktHotPlugEn == TRUE) {
    if (host->setup.kti.KtiCpuSktHotPlugTopology == 0) {
      MaxSocket =  4;         //online/offline based on 4S topology
    } else {
      MaxSocket =  8;         //online/offline based on 8S topology
    }
  } else {
    MaxSocket =  MAX_SOCKET;
  }

  KTI_ASSERT((MaxSocket <= MAX_SOCKET), ERR_SW_CHECK, MINOR_ERR_SW_CHECK_7A);

  IioMmiohInterleaveRule0.Data = 0;
  IioMmiohInterleaveRule1.Data = 0;
  IioMmiohNonInterleaveRule0.Data = 0;
  IioMmiohNonInterleaveRule1.Data = 0;
  if (KtiInternalGlobal->UseNonInterleavedMmioh == FALSE) {
    Socket = KtiInternalGlobal->SbspSoc;
  } else {
    Socket = SocId;
  }
  IioMmiohInterleaveRule0.Bits.baseaddress = (UINT16)(host->var.kti.CpuInfo[Socket].CpuRes.MmiohBase.lo >> 24);
  IioMmiohInterleaveRule0.Bits.baseaddress |= (UINT32)(host->var.kti.CpuInfo[Socket].CpuRes.MmiohBase.hi << 8);

  // Typically handle SBSP first
  MmiohLimitTmp = host->var.kti.CpuInfo[Socket].CpuRes.MmiohLimit;

  //
  // Mmioh granularity size.
  // Interleave size = 2**(size*2) GB.
  //
  MmiohSize = KtiInternalGlobal->MmiohTargetSize;
  IioMmiohInterleaveRule0.Bits.interleavemode = MmiohSize;
  IioMmiohInterleaveRule0.Bits.ruleenable = 1;
  IioMmiohInterleaveRule0.Bits.remoteserializerule = 2;
  IioMmiohInterleaveRule1.Bits.localserializerule = 2;

  if (KtiInternalGlobal->UseNonInterleavedMmioh == FALSE) {
    //
    // Mmioh is enabled via the MMIOH_INTERLEAVE_RULE and MMIOH_TARGET_INTERLEAVE registers
    //
    for (Socket = 0; Socket < MaxSocket; Socket++) {
      if (Socket == KtiInternalGlobal->SbspSoc) {
        continue;
      }
      //
      // Socket present?
      //
      if ((SocketData->Cpu[Socket].Valid == TRUE) || (KtiInternalGlobal->KtiCpuSktHotPlugEn == TRUE)) {
        // 
        // if the MMIOH is assigned
        //
        if (IsXGreaterThanYUINT64 (host->var.kti.CpuInfo[Socket].CpuRes.MmiohLimit, host->var.kti.CpuInfo[Socket].CpuRes.MmiohBase)) {
          MmiohLimitTmp = host->var.kti.CpuInfo[Socket].CpuRes.MmiohLimit;
        }
      }
    }

    IioMmiohInterleaveRule1.Bits.limitaddress = (MmiohLimitTmp.lo >> 24);
    IioMmiohInterleaveRule1.Bits.limitaddress |= (MmiohLimitTmp.hi << 8);

    IioMmiohNonInterleaveRule0.Bits.ruleenable = 1;
    IioMmiohNonInterleaveRule0.Bits.baseaddress = IioMmiohInterleaveRule0.Bits.baseaddress;
    IioMmiohNonInterleaveRule1.Bits.limitaddress = IioMmiohInterleaveRule1.Bits.limitaddress;
  } else {
    IioMmiohInterleaveRule1.Bits.limitaddress = (host->var.kti.CpuInfo[Socket].CpuRes.MmiohLimit.lo >> 24);
    IioMmiohInterleaveRule1.Bits.limitaddress |= (host->var.kti.CpuInfo[Socket].CpuRes.MmiohLimit.hi << 8);
    IioMmiohNonInterleaveRule0.Bits.ruleenable = 1;

    IioMmiohNonInterleaveRule0.Bits.baseaddress = (UINT16)(host->var.kti.CpuInfo[KtiInternalGlobal->SbspSoc].CpuRes.MmiohBase.lo >> 24);
    IioMmiohNonInterleaveRule0.Bits.baseaddress |= (UINT32)(host->var.kti.CpuInfo[KtiInternalGlobal->SbspSoc].CpuRes.MmiohBase.hi << 8);

    if (Socket == KtiInternalGlobal->SbspSoc) {
      //
      // Find 2nd socket
      //
      for (Socket = 0; Socket < MaxSocket; Socket++) {
        if (Socket == KtiInternalGlobal->SbspSoc) {
          continue;
        }
        //
        // Socket present?
        //
        if ((SocketData->Cpu[Socket].Valid == TRUE) || (KtiInternalGlobal->KtiCpuSktHotPlugEn == TRUE)) {
          break;
        }
      }
      MmiohNonInterleaveTgt = Socket;
    } else {
      MmiohNonInterleaveTgt = KtiInternalGlobal->SbspSoc;
    }
    IioMmiohNonInterleaveRule1.Bits.limitaddress = (host->var.kti.CpuInfo[Socket].CpuRes.MmiohLimit.lo >> 24);
    IioMmiohNonInterleaveRule1.Bits.limitaddress |= (host->var.kti.CpuInfo[Socket].CpuRes.MmiohLimit.hi << 8);
    IioMmiohNonInterleaveRule0.Bits.serializerule = 1;
  }

  //
  // bugeco305537/purley_bios s4929982 ; mmioh interleave decoder incorrectly uses noninterleaved serialize rule
  //
  if (host->var.common.cpuType == CPU_SKX && host->var.common.stepping < B0_REV_SKX) {
    IioMmiohNonInterleaveRule0.Bits.serializerule = IioMmiohInterleaveRule1.Bits.localserializerule;
  }

  //
  // Target registers are same as CHA, so copy from there instead of rebuilding
  //
  for (TargetIndex=0; TargetIndex < MAX_SAD_TARGETS/8; TargetIndex++) {
    MmiohTgtLst[TargetIndex] = KtiReadPciCfg (host, SocId, 0, ChaMmiohTargetRegs[TargetIndex]);
  }
  //
  // Program the MMIO_TARGET_INTERLEAVE list for this socket
  //
  for (Stack = 0; Stack < MAX_IIO_STACK; Stack++) {
    if (host->var.common.stackPresentBitmap[SocId] & (1 << Stack)) {
      for (TargetIndex = 0; TargetIndex < (MAX_SAD_TARGETS/8); TargetIndex++) {
        KtiWritePciCfg (host, SocId, Stack, MmiohTargetOffset[TargetIndex], MmiohTgtLst[TargetIndex]);
      }
      //
      // Program the rules
      //
      KtiWritePciCfg (host, SocId, Stack, MMIOH_INTERLEAVE_N1_IIO_VTD_REG, IioMmiohInterleaveRule1.Data);
      KtiWritePciCfg (host, SocId, Stack, MMIOH_INTERLEAVE_N0_IIO_VTD_REG, IioMmiohInterleaveRule0.Data);

      //
      // Mmioh Non-interleaved target is programmed in Mmiol_Tgt_List1 register.  Mark target as local DMI
      //
      IioMmiohNonInterleavedTarget.Data = KtiReadPciCfg (host, SocId, Stack, MMIOL_TGT_LIST1_IIO_VTD_REG);
      //
      // Set target to Local DMI to abort any possible miss
      //
      IioMmiohNonInterleavedTarget.Bits.mmioh_non_interleave_tgt = MmiohNonInterleaveTgt;
      KtiWritePciCfg (host, SocId, Stack, MMIOL_TGT_LIST1_IIO_VTD_REG, IioMmiohNonInterleavedTarget.Data);

      KtiWritePciCfg (host, SocId, Stack, MMIOH_NON_INTERLEAVE_N1_IIO_VTD_REG, IioMmiohNonInterleaveRule1.Data);
      KtiWritePciCfg (host, SocId, Stack, MMIOH_NON_INTERLEAVE_N0_IIO_VTD_REG, IioMmiohNonInterleaveRule0.Data);

      //
      // Mmioh granularity.
      // lsb of address check for serialize rule 2 is mmiol_granularity + 24.
      // So when allocating high MMIOH with a minimum granularity of 16MB per IIO stack, program to 0, 32MB, program to 1, etc.
      //
      SadControlIio.Data = KtiReadPciCfg (host, SocId, 0, SAD_CONTROL_IIO_VTD_REG);
      SadControlIio.Bits.mmioh_granularity = KtiInternalGlobal->MmiohGranularity - 24;
      KtiWritePciCfg (host, SocId, Stack, SAD_CONTROL_IIO_VTD_REG, SadControlIio.Data);

    }
  }
  return KTI_SUCCESS;
}


/**

  Routine to program the final IO SAD setting for the given topology.

  @param host  - Pointer to the system host (root) structure
  @param SocketData - Pointer to socket specific data
  @param KtiInternalGlobal - KTIRC Internal Global data
  @param SocId  - Socket to program

  @retval 0 - Successful
  @retval Other - failure

**/
KTI_STATUS
ProgramCpuMmiohEntriesKti (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal,
  UINT8                  SocId
  )
{

  UINT64_STRUCT TempMmiohLimit;
  UINT64_STRUCT MmiohBaseTmp;
  UINT16        MmiohLimit;
  KTILK_TAD_MMIOH1_KTI_LLPMON_STRUCT MmiohLocalLimits[3];
  UINT32    MmiohLocalTgtLst = 0;
  UINT8     TargetIndex = 0, LinkIndex, Stack;

  MemSetLocal ((UINT8 *) &MmiohLocalLimits, 0x00, (sizeof (KTILK_TAD_MMIOH1_KTI_LLPMON_STRUCT) * 3));

  for (Stack = 0; Stack < MAX_IIO_STACK; Stack++) {
    //
    // Program a target and limit for each enabled stack
    //
    if (host->var.common.stackPresentBitmap[SocId] & (1 << Stack)) {
      MmiohBaseTmp = host->var.kti.CpuInfo[SocId].CpuRes.StackRes[Stack].MmiohBase;
      if ((MmiohBaseTmp.hi != 0xFFFFFFFF) && (MmiohBaseTmp.lo != 0xFFFFFFFF)) {
        MmiohLocalTgtLst |= Stack << (TargetIndex * 3);
        TempMmiohLimit = host->var.kti.CpuInfo[SocId].CpuRes.StackRes[Stack].MmiohLimit;
        MmiohLimit = TempMmiohLimit.lo >> 30;
        MmiohLimit |= TempMmiohLimit.hi << 2;
        if (TargetIndex % 2) {
          MmiohLocalLimits[TargetIndex / 2].Bits.mmioh_limit1 = MmiohLimit;
        } else {
          MmiohLocalLimits[TargetIndex / 2].Bits.mmioh_limit0 = MmiohLimit;
        }
        TargetIndex++;
      }
    }
  }

  for (LinkIndex = 0; LinkIndex < host->var.common.KtiPortCnt; LinkIndex++) {
    if (SocketData->Cpu[SocId].LinkData[LinkIndex].Valid == TRUE){
      KtiWritePciCfg (host, SocId, LinkIndex, KTILK_TAD_MMIOH0_KTI_LLPMON_REG, MmiohLocalTgtLst);
      KtiWritePciCfg (host, SocId, LinkIndex, KTILK_TAD_MMIOH1_KTI_LLPMON_REG, MmiohLocalLimits[0].Data);
      KtiWritePciCfg (host, SocId, LinkIndex, KTILK_TAD_MMIOH2_KTI_LLPMON_REG, MmiohLocalLimits[1].Data);
      KtiWritePciCfg (host, SocId, LinkIndex, KTILK_TAD_MMIOH3_KTI_LLPMON_REG, MmiohLocalLimits[2].Data);
    }
  }

  return KTI_SUCCESS;
}


/**

  Routine to program the final IO SAD setting for the given topology.

  @param host  - Pointer to the system host (root) structure
  @param SocketData - Pointer to socket specific data
  @param KtiInternalGlobal - KTIRC Internal Global data
  @param SocId  - Socket to program

  @retval 0 - Successful
  @retval Other - failure

**/
KTI_STATUS
ProgramCpuMiscSadEntries (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal,
  UINT8                  SocId
  )
{
  SAD_TARGET_CHABC_SAD1_STRUCT          SadTargetCha;
  SAD_TARGET_IIO_VTD_STRUCT             SadTargetIio;
  SAD_CONTROL_IIO_VTD_STRUCT            SadControlIio;
  KTILK_TAD_LEG_KTI_LLPMON_STRUCT       SadTargetKti;
  UINT8     IioStack, LinkIndex;

  //
  // SAD Target - program the legacy VGA range target id, SourceID and Enable the SourceID featrue
  //
  SadTargetCha.Data = KtiReadPciCfg (host, SocId, 0, SAD_TARGET_CHABC_SAD1_REG);
  if (SocId == host->var.kti.OutLegacyVgaSoc) {
    SadTargetCha.Bits.vga_target = SOCKET_LOCAL_NID | host->var.kti.OutLegacyVgaStack;
  } else {
    SadTargetCha.Bits.vga_target = host->var.kti.OutLegacyVgaSoc;
  }

  //
  // Program legacy_pch_target in CHA
  //
  if (SocId == KtiInternalGlobal->SbspSoc) {
    SadTargetCha.Bits.legacy_pch_target = SOCKET_LOCAL_NID;
  } else {
    SadTargetCha.Bits.legacy_pch_target = KtiInternalGlobal->SbspSoc;
  }

  SadTargetCha.Bits.sourceid = SocId;
  SadTargetCha.Bits.sourceiden = 1;
  KtiWritePciCfg (host, SocId, 0, SAD_TARGET_CHABC_SAD1_REG, SadTargetCha.Data);

  //
  // SAD Control - program the local socket id (node id)
  //
  KtiWritePciCfg (host, SocId, 0, SAD_CONTROL_CHABC_SAD1_REG, SocId);

  //
  // Sad Control and Sad Target in IIO
  //
  for (IioStack = 0; IioStack < MAX_IIO_STACK; IioStack++) {
    if (host->var.common.stackPresentBitmap[SocId] & (1 << IioStack)) {
      SadTargetIio.Data = KtiReadPciCfg (host, SocId, IioStack, SAD_TARGET_IIO_VTD_REG);
      if (SocId == KtiInternalGlobal->SbspSoc) {
        SadTargetIio.Bits.legacy_pch_target = (SOCKET_LOCAL_NID | IIO_CSTACK);
      } else {
        SadTargetIio.Bits.legacy_pch_target = KtiInternalGlobal->SbspSoc;
      }
      SadTargetIio.Bits.pch_stack = IIO_CSTACK;

      if (SocId == host->var.kti.OutLegacyVgaSoc) {
        SadTargetIio.Bits.vga_target = (SOCKET_LOCAL_NID | host->var.kti.OutLegacyVgaStack);
      } else {
        SadTargetIio.Bits.vga_target = host->var.kti.OutLegacyVgaSoc;
      }
      SadTargetIio.Bits.vga_stack = host->var.kti.OutLegacyVgaStack;

      KtiWritePciCfg (host, SocId, IioStack, SAD_TARGET_IIO_VTD_REG, SadTargetIio.Data);

      SadControlIio.Data = KtiReadPciCfg (host, SocId, IioStack, SAD_CONTROL_IIO_VTD_REG);
      SadControlIio.Bits.local_nodeid = SocId;
      KtiWritePciCfg (host, SocId, IioStack, SAD_CONTROL_IIO_VTD_REG, SadControlIio.Data);
    }
  }

  //
  // Kti SadTarget
  //
  for (LinkIndex = 0; LinkIndex < host->var.common.KtiPortCnt; LinkIndex++) {
    if (SocketData->Cpu[SocId].LinkData[LinkIndex].Valid == TRUE) {
      SadTargetKti.Data = KtiReadPciCfg (host, SocId, LinkIndex, KTILK_TAD_LEG_KTI_LLPMON_REG);
      if (SocId == host->var.kti.OutLegacyVgaSoc) {
        SadTargetKti.Bits.vga_enable = 1;
        SadTargetKti.Bits.vga_target = host->var.kti.OutLegacyVgaStack;
      } else {
        SadTargetKti.Bits.vga_enable = 0;
        SadTargetKti.Bits.vga_target = 0;
      }
      KtiWritePciCfg (host, SocId, LinkIndex, KTILK_TAD_LEG_KTI_LLPMON_REG, SadTargetKti.Data);
    }
  }

  return KTI_SUCCESS;

}

/**

  Routine to program the IIO registers related to system address space.

  @param host        - Input/Output structure of the KTIRC
  @param SocketData  - CPU socket info
  @param SocID       - processor socket ID

  @retval 0 - Successful
  @retval Other - failure

**/
KTI_STATUS
ProgramSystemAddressMap (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal,
  UINT8                  SocId
  )
{

  UINT8     IioStack;
  UINT32    MmcfgRule, MmcfgLocalRule0, MmcfgLocalRule1, MmcfgTarget, MmcfgLocalTarget, MmcfgLimit;
  CPUBUSNO_IIO_VTD_STRUCT       CpuBusNo;
  MMCFG_BASE_N0_IIO_VTD_STRUCT  MmcfgBase0;
  MMCFG_BASE_N1_IIO_VTD_STRUCT  MmcfgBase1;
  MMCFG_LIMIT_N0_IIO_VTD_STRUCT MmcfgLimit0;
  MMCFG_LIMIT_N1_IIO_VTD_STRUCT MmcfgLimit1;
  BUSID_GLOB_RULE_IIO_VTD_STRUCT  GlobBusIdRule;
  UINT8     Index;
  UINT32    Target_Offset;
  UINT32    MmcfgGranularity;

  if (host->setup.common.mmCfgSize <= 0x10000000) {
    MmcfgGranularity = SAD_BUS_GRANTY;
  } else {
    MmcfgGranularity = (host->setup.common.mmCfgSize / 1024 / 1024 / SAD_BUS_TARGETS);
  }

  //
  // Get global mmcfg rules from CHA
  //
  MmcfgRule = KtiReadPciCfg (host, SocId, 0, MMCFG_RULE_N0_CHABC_SAD1_REG);
  MmcfgRule = MmcfgRule & 0x3ffffff;
  
  // s5372108: [FPGA] ME in Operational mode causes Inband PECI messages to FPGA
  // FPGA does not support inband PECI messages and will not respond to them. SPS FW is placing a fix to 
  // restrict inband PECI cycles to listed valid targets in iio_busid_glob_rule_b0d05f0 register.  
  //
  // Clear the related bits which indicate the bus range of FPGA NID in valid_targets field
  // 
  GlobBusIdRule.Data = MmcfgRule;
  for (Index = 0; Index < MAX_SOCKET; Index++) {
    if ((SocketData->Cpu[Index].Valid == TRUE) && (SocketData->Cpu[Index].SocType == SOCKET_TYPE_FPGA)) {
      Target_Offset = (UINT8)(host->var.kti.CpuInfo[Index].CpuRes.BusBase / MmcfgGranularity);
      GlobBusIdRule.Bits.valid_targets &= (~(1 << Target_Offset));
    }
  }
  
  MmcfgTarget = KtiReadPciCfg (host, SocId, 0, MMCFG_TARGET_LIST_CHABC_SAD1_REG);
  //
  // Local rules previously calculated
  //
  MmcfgLocalRule0 = KtiInternalGlobal->MmcfgLocalRules[SocId][0];
  MmcfgLocalRule1 = KtiInternalGlobal->MmcfgLocalRules[SocId][1];
  MmcfgLocalTarget = KtiInternalGlobal->MmcfgLocalRules[SocId][2];

  MmcfgLimit = KtiInternalGlobal->MmcfgBase + host->setup.common.mmCfgSize - 1;

  //
  // Program MMCFG rules, BUSID* registers, CPUBUSNO, and MCTP Segment ID size in each stack
  //
  for (IioStack = 0; IioStack < MAX_IIO_STACK; IioStack++) {
    if (host->var.common.stackPresentBitmap[SocId] & (1 << IioStack)) {
      // MmcfgBase
      MmcfgBase0.Data = KtiReadPciCfg (host, SocId, IioStack, MMCFG_BASE_N0_IIO_VTD_REG);
      MmcfgBase0.Bits.mmcfg_base_addr = KtiInternalGlobal->MmcfgBase >> 26;
      KtiWritePciCfg (host, SocId, IioStack, MMCFG_BASE_N0_IIO_VTD_REG, MmcfgBase0.Data);
      MmcfgBase1.Data = KtiReadPciCfg (host, SocId, IioStack, MMCFG_BASE_N1_IIO_VTD_REG);
      MmcfgBase1.Bits.mmcfg_base_addr = 0;
      KtiWritePciCfg (host, SocId, IioStack, MMCFG_BASE_N1_IIO_VTD_REG, MmcfgBase1.Data);
      // MmcfgLimit
      MmcfgLimit0.Data = KtiReadPciCfg (host, SocId, IioStack, MMCFG_LIMIT_N0_IIO_VTD_REG);
      MmcfgLimit0.Bits.mmcfg_limit_addr = MmcfgLimit >> 26;
      KtiWritePciCfg (host, SocId, IioStack, MMCFG_LIMIT_N0_IIO_VTD_REG, MmcfgLimit0.Data);
      MmcfgLimit1.Data = KtiReadPciCfg (host, SocId, IioStack, MMCFG_LIMIT_N1_IIO_VTD_REG);
      MmcfgLimit1.Bits.mmcfg_limit_addr = 0;
      KtiWritePciCfg (host, SocId, IioStack, MMCFG_LIMIT_N1_IIO_VTD_REG, MmcfgLimit1.Data);

      // BUSID
      KtiWritePciCfg (host, SocId, IioStack, BUSID_GLOB_RULE_IIO_VTD_REG, GlobBusIdRule.Data);
      KtiWritePciCfg (host, SocId, IioStack, BUSID_GLOB_RULE_TGT_IIO_VTD_REG, MmcfgTarget);
      KtiWritePciCfg (host, SocId, IioStack, BUSID_LOCAL_RULE_ADDRESS_0_IIO_VTD_REG, MmcfgLocalRule0);
      KtiWritePciCfg (host, SocId, IioStack, BUSID_LOCAL_RULE_ADDRESS_1_IIO_VTD_REG, MmcfgLocalRule1);
      KtiWritePciCfg (host, SocId, IioStack, BUSID_LOCAL_RULE_TGT_IIO_VTD_REG, MmcfgLocalTarget);

      // Cpubusno
      CpuBusNo.Data = KtiReadPciCfg (host, SocId, IioStack, CPUBUSNO_IIO_VTD_REG);
      CpuBusNo.Bits.bus0 = host->var.kti.CpuInfo[SocId].CpuRes.StackRes[IioStack].BusBase;
      CpuBusNo.Bits.bus1 = host->var.kti.CpuInfo[SocId].CpuRes.StackRes[IioStack].BusLimit;
      CpuBusNo.Bits.valid = 1;
      KtiWritePciCfg (host, SocId, IioStack, CPUBUSNO_IIO_VTD_REG, CpuBusNo.Data);

      // MCTP Segment ID size
      ProgramMctpSegmentIdSize (host, SocId, IioStack, MmcfgRule);
    }
  }

  return KTI_SUCCESS;

}

/**

  Routine to program the TCSKTSAD register on Cstack of Legacy Socket.
  This configures MCTP Broadcast traffic routing.

  @param host       - Input/Output structure of the KTIRC
  @param SocketData - CPU socket info

  @retval 0 - Successful
  @retval Other - failure

**/
KTI_STATUS
ProgramMctpBroadcastSettings (
  struct sysHost           *host,
  KTI_SOCKET_DATA          *SocketData,
  KTI_HOST_INTERNAL_GLOBAL *KtiInternalGlobal
  )
{
  TCSKTSAD_IIO_VTD_STRUCT TcSktSad;
  UINT8                   SocketsPresentBitmap = 0;
  UINT8                   Index;

  //
  // This register gets locked by IIO_DFX_LCK_CTL.DBGBUSLCK by IioLateInitialize code later
  //
  TcSktSad.Data = KtiReadPciCfg (host, KtiInternalGlobal->SbspSoc, IIO_CSTACK, TCSKTSAD_IIO_VTD_REG);

  //
  // mctpb_loc_skt is a vector of the enabled IIO stack IDs in the socket
  // excluding the stack that is being programmed. This indicates where
  // within the local socket MCTP broadcast messages should be directed.
  //
  TcSktSad.Bits.mctpb_loc_skt = host->var.common.stackPresentBitmap[KtiInternalGlobal->SbspSoc] & (~(1 << IIO_CSTACK));

  //
  // mctpb_rem_skt is a vector of the remote node-ids in the system
  // excluding the nodeid of the socket being programmed. This indicates
  // which remote sockets MCTP broadcast messages should be programmed.
  //
  for (Index = 0; Index < MAX_SOCKET; Index++) {
    if ((SocketData->Cpu[Index].Valid == TRUE) && (Index != KtiInternalGlobal->SbspSoc) && (SocketData->Cpu[Index].SocType == SOCKET_TYPE_CPU)) {
      SocketsPresentBitmap |= 1 << Index;
    }
  }
  TcSktSad.Bits.mctpb_rem_skt = SocketsPresentBitmap;
  KtiWritePciCfg (host, KtiInternalGlobal->SbspSoc, IIO_CSTACK, TCSKTSAD_IIO_VTD_REG, TcSktSad.Data);

  return KTI_SUCCESS;
}

/**

  Routine to program MCTP Segment ID Size field.
  This is required to enable MCTP traffic routing.
  Needs to be programmed on each present stack.

  @param host      - Input/Output structure of the KTIRC
  @param SocId     - processor socket ID
  @param IioStack  - IIO stack ID
  @param MmcfgRule - contents of BUSID_GLOB_RULE

  @retval 0     - Successful
  @retval Other - failure

**/
KTI_STATUS
ProgramMctpSegmentIdSize (
  struct sysHost *host,
  UINT8          SocId,
  UINT8          IioStack,
  UINT32         MmcfgRule
  )
{
  BUSID_GLOB_RULE_IIO_VTD_STRUCT BusIdGlobRule;
  ITCCTRL23_N1_IIO_VTD_STRUCT    ItcCtrl23;

  BusIdGlobRule.Data = MmcfgRule;
  ItcCtrl23.Data = KtiReadPciCfg (host, SocId, IioStack, ITCCTRL23_N1_IIO_VTD_REG);

  switch (BusIdGlobRule.Bits.length) {
    case 2:
      //
      // 010: MaxBusNumber = 512 (ie 512MB MMCFG range)
      // 2 segments of 256 buses, thus 1-bit-wide SegmentID
      //
      ItcCtrl23.Bits.mctp_segid_size = 1;
      break;
    case 1:
      //
      // 001: MaxBusNumber = 1024 (ie 1G MMCFG range)
      // 4 segments of 256 buses, thus 2-bit-wide SegmentID
      //
      ItcCtrl23.Bits.mctp_segid_size = 2;
      break;
    case 0:
      //
      // 000: MaxBusNumber = 2048 (ie 2G MMCFG range)
      // 8 segments of 256 buses, thus 3-bit-wide SegmentID
      //
      ItcCtrl23.Bits.mctp_segid_size = 3;
      break;
    default:
      //
      // 256 or less buses in the system, 1 segment
      // No SegmentID field required
      //
      ItcCtrl23.Bits.mctp_segid_size = 0;
      break;
  }

  //
  // Program the local socket segment ID
  //
  ItcCtrl23.Bits.cfg_my_seg = host->var.common.segmentSocket[SocId];

  KtiWritePciCfg (host, SocId, IioStack, ITCCTRL23_N1_IIO_VTD_REG, ItcCtrl23.Data);
  return KTI_SUCCESS;
}


/**

    Setup OSB settings.

    @param host              - pointer to the system host root structure
    @param KtiInternalGlobal - pointer to the KTI RC internal global structure
    @param socket            - Socket ID to configure
    @param cha               - CHA index
    @param HaCoh             - HA/CHA coherency data

    @retval KTI_SUCCESS - on successful completion

**/
KTI_STATUS
ConfigureOsbSetting(
  struct sysHost             *host,
    KTI_HOST_INTERNAL_GLOBAL *KtiInternalGlobal,
    UINT8                    socket,
    UINT8                    cha,
    HA_COH_CHA_MISC_STRUCT   *HaCoh,
    BOOLEAN                  Is4SFC
    )
{
  if (KtiInternalGlobal->SysOsbEn == KTI_DISABLE) {
    HaCoh->Bits.en_osb_loc_invitoe = 0;
  } else {
    HaCoh->Bits.en_osb_loc_invitoe = 1;
    //
    // 4929489: CHA tile performance recommended CSR values for BIOS programming
    //          This will change in future based on the part core count
    //
    HaCoh->Bits.osb_ad_vna_thr = 0;
    HaCoh->Bits.osb_snp_thr = 1;

    //
    // s5370161: OSB Setting for 4S Ring, 4S FC and 8S
    //
    if (host->var.kti.SysConfig == SYS_CONFIG_4S){
      if (KtiInternalGlobal->Is4SRing){
        HaCoh->Bits.osb_ad_vna_thr = 2;
      } else if (Is4SFC){
        HaCoh->Bits.osb_ad_vna_thr = 1;
      }
    } else if (host->var.kti.SysConfig == SYS_CONFIG_8S){
      HaCoh->Bits.osb_ad_vna_thr = 1;
    }

  }

  return KTI_SUCCESS;
}

/**

    Initialize M2Mem tiles.

    @param host              - pointer to the system host root structure
    @param Socket            - socket ID
    @param Ctr               - M2Mem index
    @param KtiInternalGlobal - pointer to the KTI RC internal global structure

    @retval KTI_SUCCESS - on successful completion

**/
KTI_STATUS
InitM2mem (
  struct sysHost      *host,
  UINT8               Socket,
  UINT8               Ctr,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal
  )
{
  SYSFEATURES0_M2MEM_MAIN_STRUCT    SysDefeatures0;
  MODE_M2MEM_MAIN_STRUCT            ModeM2Mem;
  TOPOLOGY_M2MEM_MAIN_STRUCT        TopologyM2Mem;
  ADD1_LUT_N1_M2MEM_MAIN_STRUCT     Add1Lut;
  DEFEATURES0_M2MEM_MAIN_STRUCT     Defeatures0;

  //
  // M2MEM settings
  //
  // * A single socket system is expected to at least configure following
  // * (different from reset values):
  /* *         CHA HA_COH_CFG[Dis_Directory] = 1 (functionality?TBD?)
     *         M2M SysFeatures[FrcDirI]      = 1 (functionality)
     *         M2M SysFeatures[DirWrDisable] = 1 (performance)
     *
     * A multi-socket directory disabled system is expected to at least configure
     * following (different from reset values):
     *         CHA HA_COH_CFG[Dis_Directory] = 1 (functionality)
     *         M2M SysFeatures[D2kDisable]   = 1 (functionality)
     *         M2M SysFeatures[D2cDisable]   = 1 (functionality)
     *         M2M SysFeatures[DirWrDisable] = 1 (performance) */

  SysDefeatures0.Data = KtiReadPciCfg (host, (UINT8) Socket, Ctr, SYSFEATURES0_M2MEM_MAIN_REG);
  if (KtiInternalGlobal->D2kEn == TRUE) {
    SysDefeatures0.Bits.d2kdisable = 0;
    //
    //  [4929215][Rev:2][New Sighting][Cloned From SKX Si Bug Eco: R_M2M_BAD_SADATA_RTNA1 firing on a D2K request needing DirUp]
    // (force all D2K returns to Add0 port)
    // At least for A0, possibly perm WA
    //
    if (host->var.common.cpuType == CPU_SKX && host->var.common.stepping < B0_REV_SKX) {
      Add1Lut.Data = KtiReadPciCfg (host, (UINT8) Socket, Ctr, ADD1_LUT_N1_M2MEM_MAIN_REG);
      Add1Lut.Bits.ktitoadd1map = 0x0;
      KtiWritePciCfg (host, (UINT8) Socket, Ctr, ADD1_LUT_N1_M2MEM_MAIN_REG, Add1Lut.Data);
    }
  } else {
    SysDefeatures0.Bits.d2kdisable = 1;
  }

  if (KtiInternalGlobal->D2cEn == TRUE) {
    SysDefeatures0.Bits.d2cdisable = 0;
  } else {
    SysDefeatures0.Bits.d2cdisable = 1;
  }

  if ((host->var.kti.SysConfig == SYS_CONFIG_1S) && (KtiInternalGlobal->FpgaList == 0)) {
    SysDefeatures0.Bits.frcdiri = 1;
  }

  if( KtiInternalGlobal->SysDirectoryModeEn == KTI_DISABLE) {
    SysDefeatures0.Bits.dirwrdisable = 1;
  }

  KtiWritePciCfg (host, (UINT8) Socket, Ctr, SYSFEATURES0_M2MEM_MAIN_REG, SysDefeatures0.Data);

  ModeM2Mem.Data = KtiReadPciCfg (host, (UINT8) Socket, Ctr, MODE_M2MEM_MAIN_REG);
  if (host->var.kti.OutIsocEn == TRUE) {
    ModeM2Mem.Bits.isoch = 1;
  } else {
    ModeM2Mem.Bits.isoch = 0;
  }
  KtiWritePciCfg (host, (UINT8) Socket, Ctr, MODE_M2MEM_MAIN_REG, ModeM2Mem.Data);

  //Update NodeId for d2c and PCLS
  TopologyM2Mem.Data = KtiReadPciCfg (host, (UINT8) Socket, Ctr, TOPOLOGY_M2MEM_MAIN_REG);
  TopologyM2Mem.Bits.mynid = Socket;
  TopologyM2Mem.Bits.sncluster = 1;
  TopologyM2Mem.Bits.basecluster1 = 0;
  KtiWritePciCfg (host, (UINT8) Socket, Ctr, TOPOLOGY_M2MEM_MAIN_REG, TopologyM2Mem.Data);

  //
  // s4930384: Cloned From SKX Si Bug Eco: Cloned from SKX-A0: [perf] M2M->CHA credit sideband: M2M control portion
  // Enable the credit sideband for MC->CHA credits prior to issuing memory traffic, starting on B0
  //
  if (host->var.common.cpuType == CPU_SKX && host->var.common.stepping >= B0_REV_SKX) {
    if (host->var.common.MicroCodeRev != 0){
      Defeatures0.Data = KtiReadPciCfg (host, Socket, Ctr, DEFEATURES0_M2MEM_MAIN_REG);
      Defeatures0.Bits.enablerdcredsideband = 0x1;
      Defeatures0.Bits.enablewrcredsideband = 0x1;
      KtiWritePciCfg (host, Socket, Ctr, DEFEATURES0_M2MEM_MAIN_REG, Defeatures0.Data);
    }
  }

  return KTI_SUCCESS;
}


/**

    Setup system coherency.

    @param host              - pointer to the system host root structure
    @param SocketData        - pointer to the socket data structure
    @param KtiInternalGlobal - pointer to the KTI RC internal global structure

    @retval KTI_SUCCESS - on successful completion

**/
KTI_STATUS
SetupSystemCoherency (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal
  )
{
  UINT8           Idx1, Port, Ctr;
  CBO_COH_CONFIG_CHABC_SAD_STRUCT   CboCohBc;
  HA_COH_CHA_MISC_STRUCT            HaCoh;
  HA_COH_CFG_TOR_CHA_PMA_STRUCT     HaCohCfgTor;
  HA_COH_CFG_1_CHA_MISC_STRUCT      HaCohCfg1;
  KTIAGCTRL_KTI_LLPMON_STRUCT       KtiAgCtrl;
  DBP_CONFIG_CHABC_SAD_STRUCT       DbpConfigChaBc;
  HA_THRESHOLDS_CHA_MISC_STRUCT     HaThresholds;
  MCMTR_MC_MAIN_STRUCT              McMtr;
  BOOLEAN                           Is4SFC, SocCnt;;

  KtiCheckPoint(0xFF, 0xFF, 0xFF, STS_SYSTEM_COHERENCY_SETUP, MINOR_STS_SYSTEM_COHERENCY_START, host);

  //
  // Check the system is 4S Full Connection or not
  //
  Is4SFC=FALSE;
  SocCnt=0;
  if ((host->var.kti.SysConfig == SYS_CONFIG_4S) && !KtiInternalGlobal->Is4SRing) {
    for (Idx1 = 0; Idx1 < MAX_SOCKET; Idx1++) {
      if ((SocketData->Cpu[Idx1].Valid == TRUE) && (SocketData->Cpu[Idx1].SocType == SOCKET_TYPE_CPU)) {
        if(CountActiveKtiLink (host, Idx1) < 3) {
          break;
        }
        SocCnt++;
      }
    }
    if (SocCnt == 4){
      Is4SFC=TRUE;
    }
  }

  for (Idx1 = 0; Idx1 < MAX_SOCKET; Idx1++) {
    if ((SocketData->Cpu[Idx1].Valid == TRUE) && (SocketData->Cpu[Idx1].SocType == SOCKET_TYPE_CPU)) {

      // M2MEM
      for (Ctr = 0; Ctr < KtiInternalGlobal->TotM2Mem[Idx1]; Ctr++) {
        InitM2mem (host, Idx1, Ctr, KtiInternalGlobal);
      }

      // CHABC

      // Cha coherency settings
      //   mtoibias, biasfwd, biasfwdlocalhome : Set to 1 for all configs
      //   biasfwddoubledata: Set 1 for all configs
      //   ego: Set 1 for 1S-noncod; leave it to default for all other configs
      //   disisoctorres, disisocegrres, disisocrtidres: Set all to 1 for non-ISOCH configs; leave it to default for all other configs
      //   These are removed in HSX: conddisablef, localkeepffwds: Set to 1 for all Directory Snoop modes; leave it to default for Home Snoop mode
      //

      CboCohBc.Data = KtiReadPciCfg (host, Idx1, 0, CBO_COH_CONFIG_CHABC_SAD_REG);
      CboCohBc.Bits.mtoibias                = 1;
      //
      // s5371341: [SKX H0 TI] CLONE SKX Sighting: (4S Only) Silent Data Corruption (SDC) in presence of Core C6
      // Do this for B0 > 2S
      //
      if ((host->var.common.cpuType == CPU_SKX) && (host->var.common.stepping >= B0_REV_SKX)) {
        if (host->var.kti.SysConfig >= SYS_CONFIG_4S) {
          CboCohBc.Bits.mtoibias              = 0;
        }
      }

      CboCohBc.Bits.biasfwd                 = 1;
      CboCohBc.Bits.biasfwdlocalhome        = 1;
      CboCohBc.Bits.biasfwddoubledata       = 1;
      CboCohBc.Bits.waitfordatacmp          = 0;
      CboCohBc.Bits.llcmissendrd            = 1;
      CboCohBc.Bits.drdgosonem              = 0;

      if ((host->var.kti.SysConfig == SYS_CONFIG_1S) && (KtiInternalGlobal->FpgaList == 0)) {
        CboCohBc.Bits.ego                 = 1;
        CboCohBc.Bits.ha_disable          = 1;
      }

      if (host->var.kti.OutIsocEn == TRUE && Idx1 != KtiInternalGlobal->SbspSoc) {
        CboCohBc.Bits.disisoctorres       = 1;
      }

      if (host->var.kti.OutIoDcMode == IODC_EN_REM_INVITOM_AND_WCILF) {
        CboCohBc.Bits.micwcilfen = 1;
      }

      if (host->setup.kti.DfxParm.DfxBiasFwdMode == BIAS_FWD_MODE0) {
          CboCohBc.Bits.biasfwd             = 1;
          CboCohBc.Bits.biasfwddoubledata   = 0;
          CboCohBc.Bits.biasfwdlocalhome    = 1;
      } else if (host->setup.kti.DfxParm.DfxBiasFwdMode == BIAS_FWD_MODE1) {
          CboCohBc.Bits.biasfwd             = 1;
          CboCohBc.Bits.biasfwddoubledata   = 0;
          CboCohBc.Bits.biasfwdlocalhome    = 0;
      } else if (host->setup.kti.DfxParm.DfxBiasFwdMode == BIAS_FWD_MODE2) {
          CboCohBc.Bits.biasfwd             = 0;
      } else if (host->setup.kti.DfxParm.DfxBiasFwdMode == BIAS_FWD_MODE3) {
          CboCohBc.Bits.biasfwd             = 1;
          CboCohBc.Bits.biasfwddoubledata   = 1;
          CboCohBc.Bits.biasfwdlocalhome    = 0;
      } else if (host->setup.kti.DfxParm.DfxBiasFwdMode == BIAS_FWD_MODE4) {
          CboCohBc.Bits.biasfwd             = 1;
          CboCohBc.Bits.biasfwddoubledata   = 1;
          CboCohBc.Bits.biasfwdlocalhome    = 1;
      }

      //
      // s5330687 CLONE SKX Sighting: IDI_Stress on DP produces MULT_LLC_WAY_TAG_MATCH MCE in CHA
      //
      if (host->var.kti.SysConfig != SYS_CONFIG_1S) {
        if (host->var.common.cpuType == CPU_SKX && host->var.common.stepping < B0_REV_SKX) {
          CboCohBc.Bits.frcfwdinv = 1;
        }
      }

      if (host->setup.kti.DfxParm.DfxFrcfwdinv < KTI_AUTO) {
          CboCohBc.Bits.frcfwdinv = host->setup.kti.DfxParm.DfxFrcfwdinv;
      }

      if (host->setup.kti.DfxParm.DfxLlcShareDrdCrd < KTI_AUTO) {
        CboCohBc.Bits.llcsharedrdcrd = host->setup.kti.DfxParm.DfxLlcShareDrdCrd;
      }
      KtiWritePciCfg (host, Idx1, 0, CBO_COH_CONFIG_CHABC_SAD_REG, CboCohBc.Data);

      DbpConfigChaBc.Data = KtiReadPciCfg (host, Idx1, 0, DBP_CONFIG_CHABC_SAD_REG);
      DbpConfigChaBc.Bits.enable = 1;

      if (host->setup.kti.LLCDeadLineAlloc == KTI_DISABLE) {
        DbpConfigChaBc.Bits.deadonvalidllc = 1;
      } else {
        DbpConfigChaBc.Bits.deadonvalidllc = 0;
      }
      
      if (host->setup.kti.DfxParm.DfxDbpEnable < KTI_AUTO) {
        DbpConfigChaBc.Bits.enable = host->setup.kti.DfxParm.DfxDbpEnable;
      }
      KtiWritePciCfg (host, Idx1, 0, DBP_CONFIG_CHABC_SAD_REG, DbpConfigChaBc.Data);

      //
      // CHA settings
      //
      for (Ctr = 0; Ctr < KtiInternalGlobal->TotCha[Idx1]; Ctr++) {

        HaCoh.Data = KtiReadPciCfg (host, (UINT8) Idx1, Ctr, HA_COH_CHA_MISC_REG);
        if (host->var.kti.OutIoDcMode != IODC_DISABLE) {
          HaCoh.Bits.iodc_enable = 1;
        }

        if (KtiInternalGlobal->SysDirectoryModeEn == KTI_DISABLE) {
          HaCoh.Bits.dis_directory = 1;
        } else {
          HaCoh.Bits.dis_directory = 0;
        }

        ConfigureOsbSetting(host, KtiInternalGlobal, Idx1, Ctr, &HaCoh, Is4SFC);

        KtiWritePciCfg (host, (UINT8) Idx1, Ctr, HA_COH_CHA_MISC_REG, HaCoh.Data);

        HaThresholds.Data = KtiReadPciCfg (host, (UINT8) Idx1, Ctr, HA_THRESHOLDS_CHA_MISC_REG);

        if (host->var.kti.OutIsocEn == TRUE) {
          HaThresholds.Bits.disegressisocres = 0x0;
        }
        KtiWritePciCfg (host, (UINT8) Idx1, Ctr, HA_THRESHOLDS_CHA_MISC_REG, HaThresholds.Data);

        HaCohCfgTor.Data = KtiReadPciCfg (host, (UINT8) Idx1, Ctr, HA_COH_CFG_TOR_CHA_PMA_REG);
        if (host->var.kti.OutHitMeEn == TRUE) {
          HaCohCfgTor.Bits.hitme_enable = 1;
          HaCohCfgTor.Bits.hitme_rfo_dirs = 1;
          if (host->setup.kti.DfxParm.DfxHitMeRfoDirsEn < KTI_AUTO) {
            HaCohCfgTor.Bits.hitme_rfo_dirs = host->setup.kti.DfxParm.DfxHitMeRfoDirsEn;
          }
          HaCohCfgTor.Bits.hitme_shar = 1;
        } else {
          HaCohCfgTor.Bits.hitme_enable = 0;
          HaCohCfgTor.Bits.hitme_rfo_dirs = 0;
          HaCohCfgTor.Bits.hitme_shar = 0;
        }

        KtiWritePciCfg (host, (UINT8) Idx1, Ctr, HA_COH_CFG_TOR_CHA_PMA_REG, HaCohCfgTor.Data);

        HaCohCfg1.Data = KtiReadPciCfg (host, (UINT8) Idx1, Ctr, HA_COH_CFG_1_CHA_MISC_REG);

        HaCohCfg1.Bits.ha_mini_bypass_dis = 0;

        if (host->setup.kti.StaleAtoSOptEn == KTI_ENABLE) {
          HaCohCfg1.Bits.staleatosopten = KTI_ENABLE;
        } else {
          HaCohCfg1.Bits.staleatosopten = KTI_DISABLE;
        }

        //
        // 0 for 1-socket, 2-socket
        // 1 for 4-socket, 8-socket
        //
        if (host->var.kti.SysConfig < SYS_CONFIG_4S) {
          HaCohCfg1.Bits.gateosbiodcalloc = 0;
        } else {
          HaCohCfg1.Bits.gateosbiodcalloc = 1;
        }

        if (host->setup.kti.DfxParm.DfxGateOsbIodcAllocEn < KTI_AUTO) {
          HaCohCfg1.Bits.gateosbiodcalloc  = host->setup.kti.DfxParm.DfxGateOsbIodcAllocEn;
        }

        KtiWritePciCfg (host, (UINT8) Idx1, Ctr, HA_COH_CFG_1_CHA_MISC_REG, HaCohCfg1.Data);
      }

      //
      // MCMTR Programming; M2memCount is the same as iMC count.
      //
      for (Ctr = 0; Ctr < KtiInternalGlobal->TotM2Mem[Idx1]; Ctr++) {
        McMtr.Data = KtiReadPciCfg(host, Idx1, Ctr * host->var.mem.numChPerMC, MCMTR_MC_MAIN_REG);
        if (KtiInternalGlobal->SysDirectoryModeEn == KTI_DISABLE) {
          McMtr.Bits.dir_en = 0;
        } else {
          McMtr.Bits.dir_en = 1;
        }
        KtiWritePciCfg (host, Idx1, Ctr * host->var.mem.numChPerMC, MCMTR_MC_MAIN_REG, McMtr.Data);
      }

      //
      // KTIAgent
      //
      for (Port = 0; Port < host->var.common.KtiPortCnt; Port++) {
        if (SocketData->Cpu[Idx1].LinkData[Port].Valid == TRUE) {
          KtiAgCtrl.Data = KtiReadPciCfg (host, Idx1,Port, KTIAGCTRL_KTI_LLPMON_REG);
          if (KtiInternalGlobal->D2cEn == TRUE) {
            KtiAgCtrl.Bits.rbt_rd_enable = 1;
          } else {
            KtiAgCtrl.Bits.rbt_rd_enable = 0;
          }
          KtiWritePciCfg (host, Idx1, Port, KTIAGCTRL_KTI_LLPMON_REG, KtiAgCtrl.Data);
        }
      }

    }
  }

  //
  // Program snoopfilter if fpga devices enabled
  //
  SetupSnoopFilter (host, SocketData, KtiInternalGlobal);

  return KTI_SUCCESS;
}

/**

    Setup system snoop filter for fpga.

    @param host              - pointer to the system host root structure
    @param SocketData        - pointer to the socket data structure
    @param KtiInternalGlobal - pointer to the KTI RC internal global structure

    @retval KTI_SUCCESS - on successful completion

**/
KTI_STATUS
SetupSnoopFilter (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal
  )
{
  UINT8   Socket, Ctr;
  REMOTE_FPGA_CHA_MISC_STRUCT RemoteFpga;
  QPI_XNC_MASK_CHA_PMA_STRUCT QpiXncmask;

  if (SocketFpgasInitialized (host, KtiInternalGlobal)) {
    if (KtiInternalGlobal->SnoopFilter == 1) {
      RemoteFpga.Data = 0;
      QpiXncmask.Data = 0;

      for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
        if ((SocketData->Cpu[Socket].Valid == TRUE) && (SocketData->Cpu[Socket].SocType == SOCKET_TYPE_FPGA)) {
          if (RemoteFpga.Bits.fpga_0_valid == 0) {
            RemoteFpga.Bits.fpga_0_nodeid = Socket;
            RemoteFpga.Bits.fpga_0_valid = 1;
          } else {
            RemoteFpga.Bits.fpga_1_nodeid = Socket;
            RemoteFpga.Bits.fpga_1_valid = 1;
          }
        }
      }

      //
      // if fpga installed, activate the filter and program
      //
      if (RemoteFpga.Bits.fpga_0_valid == 1) {
        RemoteFpga.Bits.snoop_filter_active = 1;
        RemoteFpga.Bits.snoop_filter_size = 0;
        
        for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
          if (KtiInternalGlobal->CpuList & (1 << Socket)) {
            for (Ctr = 0; Ctr < KtiInternalGlobal->TotCha[Socket]; Ctr++) {
              KtiWritePciCfg (host, Socket, Ctr, REMOTE_FPGA_CHA_MISC_REG, RemoteFpga.Data);
              KtiWritePciCfg (host, Socket, Ctr, QPI_XNC_MASK_CHA_PMA_REG, QpiXncmask.Data);
            }
          }
        }
      }
    }
  }
  return KTI_SUCCESS;
}

/**

    Program misc KTI link settings.

    @param host              - pointer to the system host root structure
    @param SocketData        - pointer to the socket data structure
    @param KtiInternalGlobal - pointer to the KTI RC internal global structure

    @retval KTI_SUCCESS - on successful completion

**/
KTI_STATUS
SetupKtiMisc (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal
  )
{
  SetupKtiMiscSkx (host, SocketData, KtiInternalGlobal);

  return KTI_SUCCESS;
}


/**

    SKX-specific misc KTI reg programming.

    @param host              - pointer to the system host root structure
    @param SocketData        - pointer to the socket data structure
    @param KtiInternalGlobal - pointer to the KTI RC internal global structure

    @retval KTI_SUCCESS - on successful completion

**/
KTI_STATUS
SetupKtiMiscSkx (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal
  )
{
  UINT8                                  Idx1, Idx2, Ctr, IioStack;
  UINT32                                 Data32, Misclist, CpuList;
  UINT32                                 GidNidMap, GblNidMap0, GblNidMap1;
  CIPCTRL_IIO_VTD_STRUCT                 Cipctrl;
  IIOMISCCTRL_N0_IIO_VTD_STRUCT          IioMiscCtrl;
  CBO_ISOC_CONFIG_CHABC_SAD_STRUCT       CboIsocConfig;
  UBOXERRCTL_UBOX_CFG_STRUCT             UboxErrCtl;
  HA_COH_CHA_MISC_STRUCT                 HaCoh;
  CSIPISOCRES_IIO_DFX_GLOBAL_STRUCT      CSipiSocRes;
  VTDPRIVC0_IIO_DFX_VTD_STRUCT           VtdPrivC0;
  CREDITTHRESHOLDS_M2MEM_MAIN_STRUCT     CreditThresholds;
  MCDECS_CHICKEN_BITS_MC_MAIN_STRUCT     McDecsChickenBits;
  BL_VNA_CREDIT_CONFIG_N1_CHA_PMA_STRUCT BlVnaCreditConfig1;
  HA_AD_CREDITS_CHA_MISC_STRUCT          HaAdCredit;
  HA_BL_CREDITS_CHA_MISC_STRUCT          HaBlCredit;
  M3KINGCTL_M3KTI_MAIN_STRUCT            M3kIngCtl;
  TCERRDIS_IIO_DFX_GLOBAL_STRUCT         TcErrDis;
  UINT8                                  FpgaList;

  KtiCheckPoint(0xFF, 0xFF, 0xFF, STS_PROTOCOL_LAYER_SETTING, MINOR_STS_KTI_MISC_SETUP, host);

  //
  // Prepare the NID map
  //
  GblNidMap0 = 0;         // for socket 0-3
  GblNidMap1 = 0;         // for socket 4-7
  GidNidMap = 0;
  FpgaList = 0;

  //
  // Build fpga nid list
  //
  for (Idx1 = 0; Idx1 < MAX_SOCKET; Idx1++) {
    if ((SocketData->Cpu[Idx1].Valid == TRUE) && (SocketData->Cpu[Idx1].SocType == SOCKET_TYPE_FPGA)) {
      FpgaList |= (1 << Idx1);
    }
  }


  Misclist = KtiInternalGlobal->CpuList;                //Broadcast list for WbInvdAck/InvdAck/EOI

  for (Idx1 = 0; Idx1 < MAX_SOCKET; Idx1++) {
    if (KtiInternalGlobal->CpuList & (1 << Idx1)) {
      if ( Idx1 < 4) {
         GblNidMap0 = GblNidMap0 | (1 << (28 + Idx1)) | (Idx1 << (4 * Idx1));
      } else {
         GblNidMap1 = GblNidMap1 | (1 << (28 + (Idx1 - 4))) | (Idx1 << (4 * (Idx1 - 4)));
      }

      GidNidMap = GidNidMap | (Idx1 << (3 * Idx1));
    }
  }

  for (Idx1 = 0; Idx1 < MAX_SOCKET; Idx1++) {
    if (KtiInternalGlobal->CpuList & (1 << Idx1)) {

      //Program M2M credit setting
      ProgramM2mCreditSetting(host, KtiInternalGlobal, Idx1);

      //
      // IIO Interface
      //
      for (IioStack = 0; IioStack < MAX_IIO_STACK; IioStack++) {
        if (host->var.common.stackPresentBitmap[Idx1] & (1 << IioStack)) {
          //
          // Coherent Interface Protocol Control
          //
          Cipctrl.Data = KtiReadPciCfg (host, Idx1, IioStack, CIPCTRL_IIO_VTD_REG);
          Cipctrl.Bits.diswrcomb = 0;

          if (host->setup.common.dcaEn == TRUE) {
            Cipctrl.Bits.dcaen = 1;  //set the dcaen field
          }
          KtiWritePciCfg (host, Idx1, IioStack, CIPCTRL_IIO_VTD_REG, Cipctrl.Data);

          //
          // IIO Misc Control
          //
          IioMiscCtrl.Data = KtiReadPciCfg (host, Idx1, IioStack, IIOMISCCTRL_N0_IIO_VTD_REG);

          IioMiscCtrl.Bits.azalia_on_vcp = 0;

          if (host->var.kti.OutLegacyVgaSoc == Idx1) {
            IioMiscCtrl.Bits.rvgaen = 0;
          } else {
            IioMiscCtrl.Bits.rvgaen = 1;
          }

          IioMiscCtrl.Bits.tocm = MAX_ADDR_LINES - 32;
          IioMiscCtrl.Bits.tocmvalid = 1;

          if (host->var.kti.OutIsocEn == TRUE && Idx1 == KtiInternalGlobal->SbspSoc && IioStack == IIO_CSTACK) {
            IioMiscCtrl.Bits.dmi_vc1_write_ordering = 0x1;
            IioMiscCtrl.Bits.dmi_vc1_vt_d_fetch_ordering = 0x0;
          }
          KtiWritePciCfg (host, Idx1, IioStack, IIOMISCCTRL_N0_IIO_VTD_REG, IioMiscCtrl.Data);

          if (host->var.kti.OutIsocEn == TRUE && Idx1 == KtiInternalGlobal->SbspSoc && IioStack == IIO_CSTACK) {
            CSipiSocRes.Data = KtiReadPciCfg (host, Idx1, IioStack, CSIPISOCRES_IIO_DFX_GLOBAL_REG);
            CSipiSocRes.Bits.isoc_enabled = 0x1;
            KtiWritePciCfg (host, Idx1, IioStack, CSIPISOCRES_IIO_DFX_GLOBAL_REG, CSipiSocRes.Data);

            VtdPrivC0.Data = KtiReadPciCfg (host, Idx1, IioStack, VTDPRIVC0_IIO_DFX_VTD_REG);
            VtdPrivC0.Bits.irp = 0x0;
            KtiWritePciCfg (host, Idx1, IioStack, VTDPRIVC0_IIO_DFX_VTD_REG, VtdPrivC0.Data);
          } // if IsocEn, SBSP Cstack
        } // if IioStack is present
      } // for IioStack loop

      //
      // s5330373: CLONE SKX Sighting: PCIE:  CSR Value for Isoch credits causes ITC credit overflow to be logged
      // Prior to programming ItcCtrl45.max_isoch, set tcerrdis.itc_irp_cred_of = 1; clear after WR; permanent W/A
      //
      TcErrDis.Data = KtiReadPciCfg (host, Idx1, IIO_CSTACK, TCERRDIS_IIO_DFX_GLOBAL_REG);
      TcErrDis.Bits.itc_irp_cred_of = 0;
      KtiWritePciCfg (host, Idx1, IIO_CSTACK, TCERRDIS_IIO_DFX_GLOBAL_REG, TcErrDis.Data);

      //
      // Ubox
      //

      // Local Node id
      Data32 = KtiReadPciCfg (host, Idx1, 0, CPUNODEID_UBOX_CFG_REG);
      Data32 = (Data32 & 0xFFFFE318)  | (KtiInternalGlobal->SbspSoc << 10)  | (KtiInternalGlobal->SbspSoc << 5) | Idx1; // Set lgcnodeid, locknodeid, lclnodeid
      KtiWritePciCfg (host, Idx1, 0, CPUNODEID_UBOX_CFG_REG, Data32);

      // CpuEnable
      Data32 = (1 << 31) | KtiInternalGlobal->CpuList; // Set the list of CPUs in the system and enable the register
      KtiWritePciCfg (host, Idx1, 0, CPUENABLE_UBOX_CFG_REG, Data32);

      // Bcastlist
      if (host->var.kti.RasInProgress == TRUE && host->var.kti.RasSocId == Idx1) {
        // CPU Socket being onlined should not be in the system interrupt domain until the last quiesce operation
        Data32 = 0;
      } else if (host->var.kti.RasInProgress == TRUE && host->var.kti.RasSocId != Idx1) {
        // CPU Socket being onlined should not be in the system interrupt domain until the last quiesce operation
        Misclist &= (~(1<<  (host->var.kti.RasSocId)));
        CpuList = (KtiInternalGlobal->CpuList & (~(1 <<  (host->var.kti.RasSocId))));
        Data32 = (CpuList << 24) | (Misclist << 16) | (CpuList << 8) | (CpuList);
      } else {
        Data32 = (KtiInternalGlobal->CpuList << 24) | (Misclist << 16) | (KtiInternalGlobal->CpuList << 8) | (KtiInternalGlobal->CpuList);
      }

      KtiWritePciCfg (host, Idx1, 0, BCASTLIST_UBOX_CFG_REG, Data32);

      // Lock Control
      Data32 = KtiReadPciCfg (host, Idx1, 0, LOCKCONTROL_UBOX_CFG_REG);
      Data32 = Data32 & (~(1 << 4)); // Disable compatibility mode
      Data32 = Data32 & 0xFFFFFFFE; // Enable the lock
      KtiWritePciCfg (host, Idx1, 0, LOCKCONTROL_UBOX_CFG_REG, Data32);

      // Group Id to Node Id Map
      KtiWritePciCfg (host, Idx1, 0, GIDNIDMAP_UBOX_CFG_REG, GidNidMap);

      // s4031310 Ubot timeout To set timeout duration 5 sec at 1.2GHz.
      //              NM  = (T0 * f )/NB     = (5.0 * 1.2E+9)/0xFFFFFF = 357.627889969 ~ 0x166[9:0]
      //              The count match register field specifies [9:4] of NM [9:0]
      //              0x166>>4 = 0x16[9:4]  ' 0x17[9:4] masking sure above 5 sec.

      UboxErrCtl.Data = KtiReadPciCfg (host, Idx1, 0, UBOXERRCTL_UBOX_CFG_REG);
      UboxErrCtl.Bits.smitimeoutcountmatch = 0x17;
      UboxErrCtl.Bits.masterlocktimeoutcountmatch = 0x17;
      KtiWritePciCfg (host, Idx1, 0, UBOXERRCTL_UBOX_CFG_REG, UboxErrCtl.Data);

      //
      // PCU
      //

      // CpuNodeID
      KtiWritePciCfg (host, Idx1, 0, GLOBAL_NID_SOCKET_0_TO_3_MAP_PCU_FUN2_REG, GblNidMap0);  // Set the NID of CPUs in the system and enable the register
      if( GblNidMap1 != 0) {
         KtiWritePciCfg (host, Idx1, 0, GLOBAL_NID_SOCKET_4_TO_7_MAP_PCU_FUN4_REG, GblNidMap1);
      }

      //
      // VCU
      //

      // CpuNodeID
      KtiWritePciCfg (host, Idx1, 0, GLOBAL_NID_SOCKET_0_TO_3_MAP_VCU_FUN2_REG, GblNidMap0);  // Set the NID of CPUs in the system and enable the register
      if( GblNidMap1 != 0) {
         KtiWritePciCfg (host, Idx1, 0, GLOBAL_NID_SOCKET_4_TO_7_MAP_VCU_FUN2_REG, GblNidMap1);
      }

      //
      // CHABC
      //

      // Isoc or MESEG Enable
      CboIsocConfig.Data = KtiReadPciCfg (host, Idx1, 0, CBO_ISOC_CONFIG_CHABC_SAD_REG);
      if (host->var.kti.OutIsocEn  == TRUE) {
        CboIsocConfig.Bits.isoc_enable = 1;     //Set ISOC flag
      }
      KtiWritePciCfg (host, Idx1, 0, CBO_ISOC_CONFIG_CHABC_SAD_REG, CboIsocConfig.Data);

      //
      // CHA
      //
      for (Ctr = 0; Ctr < KtiInternalGlobal->TotCha[Idx1]; Ctr++) {

        HaCoh.Data = KtiReadPciCfg (host, Idx1, Ctr, HA_COH_CHA_MISC_REG);
        HaCoh.Bits.active_node_mask = KtiInternalGlobal->CpuList;   // Program active_node_mask_bit  field
        HaCoh.Bits.active_node_mask |= KtiInternalGlobal->FpgaList; // include any fpga

        for (Idx2 = 0; Idx2 < MAX_KTI_PORTS; Idx2++) {
          if (SocketData->Cpu[Idx1].LinkData[Idx2].Valid == TRUE) {
            if(Idx2 == 0) {
              HaCoh.Bits.kti0enabled = 1;
            } else if(Idx2 == 1) {
              HaCoh.Bits.kti1enabled = 1;
            } else if(Idx2 == 2) {
              HaCoh.Bits.kti2enabled = 1;
            }
          } else {
            if(Idx2 == 0) {
              HaCoh.Bits.kti0enabled = 0;
            } else if(Idx2 == 1) {
              HaCoh.Bits.kti1enabled = 0;
            } else if(Idx2 == 2) {
              HaCoh.Bits.kti2enabled = 0;
            }
          }
        }

        if ((host->var.kti.SysConfig == SYS_CONFIG_1S) && (KtiInternalGlobal->FpgaList == 0)) {
          HaCoh.Bits.rdinvownacceptsm = 0;
          HaCoh.Bits.rddatamigacceptsm = 0;
        } else {
          HaCoh.Bits.rdinvownacceptsm = 1;
          HaCoh.Bits.rddatamigacceptsm = 1;
        }
        KtiWritePciCfg (host, Idx1, Ctr, HA_COH_CHA_MISC_REG, HaCoh.Data);

        //
        // Isoc enabling in CHA
        //
        if (host->var.kti.OutIsocEn == TRUE) {

          BlVnaCreditConfig1.Data = KtiReadPciCfg (host, Idx1, Ctr, BL_VNA_CREDIT_CONFIG_N1_CHA_PMA_REG);
          BlVnaCreditConfig1.Bits.disisocvnres = 0;
          KtiWritePciCfg (host, Idx1, Ctr, BL_VNA_CREDIT_CONFIG_N1_CHA_PMA_REG, BlVnaCreditConfig1.Data);

          HaAdCredit.Data = KtiReadPciCfg (host, Idx1, Ctr, HA_AD_CREDITS_CHA_MISC_REG);
          HaAdCredit.Bits.disadvnaisocres = 0;

          //
          // Set disblvnaisocres when one of VNA_PORT0/1 is 1
          // This can happen with SNC programming
          //
          if ((HaAdCredit.Bits.ad_vna_port0 == 1)||(HaAdCredit.Bits.ad_vna_port1 == 1)){
            HaAdCredit.Bits.disadvnaisocres = 1;
          }
          KtiWritePciCfg (host, Idx1, Ctr, HA_AD_CREDITS_CHA_MISC_REG, HaAdCredit.Data);

          HaBlCredit.Data = KtiReadPciCfg (host, Idx1, Ctr, HA_BL_CREDITS_CHA_MISC_REG);
          HaBlCredit.Bits.disblvnaisocres = 0;
          //
          // Set disblvnaisocres when one of VNA_PORT0/1 is 1
          // This can happen with SNC programming
          //
          if ((HaBlCredit.Bits.bl_vna_port0 == 1)||(HaBlCredit.Bits.bl_vna_port1 == 1)){
            HaBlCredit.Bits.disblvnaisocres = 1;
          }
          KtiWritePciCfg (host, Idx1, Ctr, HA_BL_CREDITS_CHA_MISC_REG, HaBlCredit.Data);
        } // Isoc Enabled
      } // CHA loop

      //
      // M2MEm
      //
      if (host->var.kti.OutIsocEn == TRUE) {
        for (Ctr = 0; Ctr < KtiInternalGlobal->TotM2Mem[Idx1]; Ctr++) {
          CreditThresholds.Data = KtiReadPciCfg (host, Idx1, Ctr, CREDITTHRESHOLDS_M2MEM_MAIN_REG);
          CreditThresholds.Bits.critrdthresh          = 0x2;
          CreditThresholds.Bits.critwrthresh          = 0x2;
          CreditThresholds.Bits.critpartialwdsbthresh = 0x2;
          KtiWritePciCfg (host, Idx1, Ctr, CREDITTHRESHOLDS_M2MEM_MAIN_REG, CreditThresholds.Data);
        }
      }

      //
      // MCDECS
      //
      if ( (host->var.kti.OutIsocEn == TRUE) || ((host->setup.kti.DirectoryModeEn == KTI_ENABLE) && (host->var.common.stepping >= H0_REV_SKX)) ){
        for (Ctr = 0; Ctr < KtiInternalGlobal->TotM2Mem[Idx1] * MAX_MC_CH; Ctr++) {
          McDecsChickenBits.Data = (UINT8) KtiReadPciCfg (host, Idx1, Ctr, MCDECS_CHICKEN_BITS_MC_MAIN_REG);
          //
          // 4929475: Cloned From SKX Si Bug Eco: ISOC can overflow RPQ
          // All-steppings SKX workaround; Each MC (M2mem) has 3 McDecs
          //
          if (host->var.kti.OutIsocEn == TRUE) {
            McDecsChickenBits.Bits.dis_isoch_rd = 1;
          }
          //
          // 5372503: CLONE SKX Sighting: Unexpected failover seen with UC mirror scrub injection.
          // H0 stepping SKX workaround; Each MC (M2mem) has 3 McDecs
          //
          if ( (host->setup.kti.DirectoryModeEn == KTI_ENABLE) && (host->var.common.stepping >= H0_REV_SKX) ) {
            McDecsChickenBits.Bits.defeature_3 = 1;
          }
          KtiWritePciCfg (host, Idx1, Ctr, MCDECS_CHICKEN_BITS_MC_MAIN_REG, McDecsChickenBits.Data);
        }
      }
      //
      // M3KTI
      // 4929504: KTI tile performance critical CSRs for BIOS programming
      //
      for (Ctr = 0; Ctr < host->var.kti.CpuInfo[Idx1].TotM3Kti; Ctr++) {
        M3kIngCtl.Data = KtiReadPciCfg (host, Idx1, Ctr, M3KINGCTL_M3KTI_MAIN_REG);
        M3kIngCtl.Bits.validtxcyclemode = 2;
        M3kIngCtl.Bits.flitsinbgfifothresholdunnnh = 6;
        KtiWritePciCfg (host, Idx1, Ctr, M3KINGCTL_M3KTI_MAIN_REG, M3kIngCtl.Data);
      }
    }
  }

  return KTI_SUCCESS;
}



/**

    Collect and clear error info.

    @param host              - pointer to the system host root structure
    @param SocketData        - pointer to the socket data structure
    @param KtiInternalGlobal - pointer to the KTI RC internal global structure

    @retval KTI_SUCCESS - on successful completion

**/
KTI_STATUS
CollectAndClearErrors (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal
  )
{
   UINT8                                Soc, LinkIndex, Ctr;
   UINT32                               Data32;
   KTIDBGERRSTDIS1_KTI_CIOPHYDFX_STRUCT KtiDbgErrStDis1;
   KTIERRCNT0_MASK_KTI_LLPMON_STRUCT    KtiErrCnt0Mask;
   KTIERRCNT1_MASK_KTI_LLPMON_STRUCT    KtiErrCnt1Mask;
   KTIERRCNT2_MASK_KTI_LLPMON_STRUCT    KtiErrCnt2Mask;
   R2PINGERRMSK0_M2UPCIE_MAIN_STRUCT    r2pIngErrReg;
   M3INGERRMASK0_M3KTI_MAIN_STRUCT      M3IngerrMask0;
   M3INGERRMASK1_M3KTI_MAIN_STRUCT      M3IngerrMask1;
   BIOS_KTI_ERR_MISC_N1_KTI_LLPMON_STRUCT  KtiErrMisc1;
   KTICERRLOGCTRL_KTI_LLPMON_STRUCT        KtiCerrLogCtrl;
   Data32 = 0;

   for (Soc = 0; Soc < MAX_SOCKET; Soc++) {
     if ((SocketData->Cpu[Soc].Valid == TRUE) && (SocketData->Cpu[Soc].SocType == SOCKET_TYPE_CPU)) {
        // M3KTI
        for (Ctr = 0; Ctr < host->var.kti.CpuInfo[Soc].TotM3Kti; Ctr++) {

          Data32 = KtiReadPciCfg (host, Soc, Ctr, M3EGRERRLOG0_M3KTI_MAIN_REG); // RW1C
          host->var.kti.CpuInfo[Soc].CpuErrLog.M3Kti.M3Egrerrlog0 = Data32;
          KtiWritePciCfg (host, Soc, Ctr, M3EGRERRLOG0_M3KTI_MAIN_REG, Data32);

          Data32 = KtiReadPciCfg (host, Soc, Ctr, M3EGRERRLOG1_M3KTI_MAIN_REG); // RW1C
          host->var.kti.CpuInfo[Soc].CpuErrLog.M3Kti.M3Egrerrlog1 = Data32;
          KtiWritePciCfg (host, Soc, Ctr, M3EGRERRLOG1_M3KTI_MAIN_REG, Data32);

          Data32 = KtiReadPciCfg (host, Soc, Ctr, M3INGERRLOG0_M3KTI_MAIN_REG); // RW1C
          host->var.kti.CpuInfo[Soc].CpuErrLog.M3Kti.M3Ingerrlog0 = Data32;
          KtiWritePciCfg (host, Soc, Ctr, M3INGERRLOG0_M3KTI_MAIN_REG, Data32);

          Data32 = KtiReadPciCfg (host, Soc, Ctr, M3INGERRLOG1_M3KTI_MAIN_REG); // RW1C
          host->var.kti.CpuInfo[Soc].CpuErrLog.M3Kti.M3Ingerrlog1 = Data32;
          KtiWritePciCfg (host, Soc, Ctr, M3INGERRLOG1_M3KTI_MAIN_REG, Data32);

          Data32 = KtiReadPciCfg (host, Soc, Ctr, M3INGERRLOG2_M3KTI_MAIN_REG); // RW1C
          host->var.kti.CpuInfo[Soc].CpuErrLog.M3Kti.M3Ingerrlog1 = Data32;
          KtiWritePciCfg (host, Soc, Ctr, M3INGERRLOG2_M3KTI_MAIN_REG, Data32);

          if ((host->var.common.cpuType == CPU_SKX) && (host->var.common.stepping >= B0_REV_SKX)) {
            M3IngerrMask0.Data = KtiReadPciCfg (host, Soc, Ctr, M3INGERRMASK0_M3KTI_MAIN_REG);
            M3IngerrMask0.Bits.blpumperrorfirstnot0mask =1;
            M3IngerrMask0.Bits.blpumperrorsecondnot1mask =1;
            M3IngerrMask0.Bits.remotevnacreditoverflowmask = 0;
            KtiWritePciCfg (host, Soc, Ctr, M3INGERRMASK0_M3KTI_MAIN_REG, M3IngerrMask0.Data);

            M3IngerrMask1.Data = KtiReadPciCfg (host, Soc, Ctr, M3INGERRMASK1_M3KTI_MAIN_REG);
            M3IngerrMask1.Bits.blpumperrorfirstnot0mask =1;
            M3IngerrMask1.Bits.blpumperrorsecondnot1mask =1;
            M3IngerrMask1.Bits.remotevnacreditoverflowmask = 0;
            KtiWritePciCfg (host, Soc, Ctr, M3INGERRMASK1_M3KTI_MAIN_REG, M3IngerrMask1.Data);
          }
        }

        // M2PCIe
        for (Ctr = 0; Ctr < MAX_SKX_M2PCIE; Ctr++){
           if(host->var.common.M2PciePresentBitmap[Soc] & (1 << Ctr)) {
              Data32 = KtiReadPciCfg (host, Soc, Ctr, R2EGRERRLOG_M2UPCIE_MAIN_REG);
              host->var.kti.CpuInfo[Soc].CpuErrLog.M2Pcie.R2Egrerrlog = Data32;
              KtiWritePciCfg (host, Soc, Ctr, R2EGRERRLOG_M2UPCIE_MAIN_REG, Data32);

              Data32 = KtiReadPciCfg (host, Soc, Ctr, R2PINGERRLOG0_M2UPCIE_MAIN_REG);// RW1CS
              host->var.kti.CpuInfo[Soc].CpuErrLog.M2Pcie.R2Ingerrlog0 = Data32;
              KtiWritePciCfg (host, Soc, Ctr, R2PINGERRLOG0_M2UPCIE_MAIN_REG, Data32);

              // Si W/A 310710: Mask parity error on rsingrblpm with CSR bit
              r2pIngErrReg.Data = KtiReadPciCfg (host, Soc, Ctr, R2PINGERRMSK0_M2UPCIE_MAIN_REG);
              r2pIngErrReg.Bits.parerring0msk = 1;
              KtiWritePciCfg (host, Soc, Ctr, R2PINGERRMSK0_M2UPCIE_MAIN_REG, r2pIngErrReg.Data);
           }
        }

        //
        // Set up link error masks to avoid false errors.
        //
        for (LinkIndex = 0; LinkIndex < host->var.common.KtiPortCnt; LinkIndex++) {
          if (SocketData->Cpu[Soc].LinkData[LinkIndex].Valid != TRUE) {
            //
            // Link is unused, disable all error reporting for it
            //
            Data32 = 0xFFFFFFFF;
            KtiWritePciCfg (host, Soc, LinkIndex, KTIERRDIS0_KTI_LLPMON_REG, Data32);
            KtiWritePciCfg (host, Soc, LinkIndex, KTIERRDIS1_KTI_LLPMON_REG, Data32);
          }

          if (host->var.kti.CpuInfo[Soc].LepInfo[LinkIndex].Valid == TRUE){
              
            // WA: s5372549 --- as Temporary WA
            // [FPGA] Correctable and some uncorrectable errors seen on SKX UPI during boot
            // Clear KTI Link2 (FPGA Link) related registers
            if (host->var.kti.OutKtiFpgaPresent[Soc] && (LinkIndex == XEON_FPGA_PORT) && (!SocketFpgasInitialized (host, KtiInternalGlobal))) {
              //
              // Set the dis_ce_log for the Kti Link2 to disable the error logging in mc_st before BBS loaded   
              KtiCerrLogCtrl.Data = KtiReadPciCfg (host, Soc, LinkIndex, KTICERRLOGCTRL_KTI_LLPMON_REG); 
              KtiCerrLogCtrl.Bits.dis_ce_log = 1;
              KtiWritePciCfg (host, Soc, LinkIndex, KTICERRLOGCTRL_KTI_LLPMON_REG, KtiCerrLogCtrl.Data); 
            }
            if (host->var.kti.OutKtiFpgaEnable[Soc] && SocketFpgasInitialized (host, KtiInternalGlobal) && (LinkIndex == XEON_FPGA_PORT)) {  
              // clear KTI Link2 BIOS_ERR_ST
              KtiWritePciCfg (host, Soc, LinkIndex, BIOS_KTI_ERR_ST_N0_KTI_LLPMON_REG, 0);  
              KtiWritePciCfg (host, Soc, LinkIndex, BIOS_KTI_ERR_ST_N1_KTI_LLPMON_REG, 0);
            
              // Clear KTI Link2 BIOS_ERR_MISC.cor_err [63:54] = 0 and [47] = 0
              KtiErrMisc1.Data = KtiReadPciCfg (host, Soc, LinkIndex, BIOS_KTI_ERR_MISC_N1_KTI_LLPMON_REG);
              KtiErrMisc1.Bits.cor_err = 0;
              KtiErrMisc1.Data &= (UINT32)(~ BIT15);
              KtiWritePciCfg (host, Soc, LinkIndex, BIOS_KTI_ERR_MISC_N1_KTI_LLPMON_REG, KtiErrMisc1.Data);
            
              // Set KTI Link2 CRCERRCNT = 0x7fffffff
              KtiWritePciCfg (host, Soc, LinkIndex, KTICRCERRCNT_KTI_LLPMON_REG, 0x7fffffff);
              
              // Re-enable the dis_ce_log for the KTI link2 after the Link is trained
              KtiCerrLogCtrl.Data = KtiReadPciCfg (host, Soc, LinkIndex, KTICERRLOGCTRL_KTI_LLPMON_REG); 
              KtiCerrLogCtrl.Bits.dis_ce_log = 0;
              KtiWritePciCfg (host, Soc, LinkIndex, KTICERRLOGCTRL_KTI_LLPMON_REG, KtiCerrLogCtrl.Data);
            }   
            //
            // A0 Stepping workaround: s4929014
            // B0 WA: s5371481
            // All bits in disable_ctrl_flowqoverunder were set before programming KTIA2RCRCTRL and need to
            // be cleared after WR.
            //
            KtiDbgErrStDis1.Data = KtiReadPciCfg (host, Soc, LinkIndex, KTIDBGERRSTDIS1_KTI_CIOPHYDFX_REG);
            KtiDbgErrStDis1.Bits.disable_ctrl_flowqoverunder = 0;
            KtiWritePciCfg (host, Soc, LinkIndex, KTIDBGERRSTDIS1_KTI_CIOPHYDFX_REG, KtiDbgErrStDis1.Data);
            //
            // A0 Stepping WA: s5332034
            // UPI PM L1 is causing 0x20 Initialization Aborts at polling state
            //
            if (host->var.common.cpuType == CPU_SKX && host->var.common.stepping < B0_REV_SKX) {
              if (host->var.kti.OutKtiLinkL1En == KTI_ENABLE){
                Data32 = 0x30003;
                KtiWritePciCfg (host, Soc, LinkIndex, KTIERRDIS0_KTI_LLPMON_REG, Data32);
              }

            }
          }

          //
          // s5330420: UPI PO : Set UPI Error Counter0/1/2 Masks
          //
          KtiErrCnt0Mask.Data = KtiReadPciCfg (host, Soc, LinkIndex, KTIERRCNT0_MASK_KTI_LLPMON_REG);
          if (host->var.common.cpuType == CPU_SKX && host->var.common.stepping < B0_REV_SKX) {
            KtiErrCnt0Mask.Bits.mask = 0x100000;
          } else {
            //s5370330
            KtiErrCnt0Mask.Bits.mask = 0x1000;
          }
          KtiWritePciCfg (host, Soc, LinkIndex, KTIERRCNT0_MASK_KTI_LLPMON_REG, KtiErrCnt0Mask.Data);

          KtiErrCnt1Mask.Data = KtiReadPciCfg (host, Soc, LinkIndex, KTIERRCNT1_MASK_KTI_LLPMON_REG);
          if (host->var.common.cpuType == CPU_SKX && host->var.common.stepping < B0_REV_SKX) {
            KtiErrCnt1Mask.Bits.mask = 0x3c000;
          } else {
            //s5370330
            KtiErrCnt1Mask.Bits.mask = 0x13c000;
          }
          KtiWritePciCfg (host, Soc, LinkIndex, KTIERRCNT1_MASK_KTI_LLPMON_REG, KtiErrCnt1Mask.Data);

          KtiErrCnt2Mask.Data = KtiReadPciCfg (host, Soc, LinkIndex, KTIERRCNT2_MASK_KTI_LLPMON_REG);
          KtiErrCnt2Mask.Bits.mask = 0x9e7;
          KtiWritePciCfg (host, Soc, LinkIndex, KTIERRCNT2_MASK_KTI_LLPMON_REG, KtiErrCnt2Mask.Data);
        }
     }
   }

   MCAErrWa(host, SocketData, FALSE);

   return KTI_SUCCESS;
}

/**

    S3 Resume check and data structure update. If we are not in S3 resume path, update the NVRAM
    structure with the system topology info. If we are in S3 resume path, verify that the system topology
    matches the pre-S3 resume path topology info stored previously in NVRAM. We have to do the check
    only when no resets are pending; this is to make sure any transient system configuration is not
    interpreted as system configuration change in S3 resume path.

    @param host              - pointer to the system host root structure
    @param SocketData        - pointer to the socket data structure
    @param KtiInternalGlobal - pointer to the KTI RC internal global structure

    @retval KTI_SUCCESS - on successful completion

**/
KTI_STATUS
CheckS3ResumePath (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal
  )
{
  UINT8      Ctr, Port;
  BOOLEAN    S3ResumeFail;

  // When we are not in S3 Resume path copy the system configuration info to the output structure
  if (host->var.common.bootMode == NormalBoot) {
    host->nvram.kti.Cpus = KtiInternalGlobal->CpuList;
    host->nvram.kti.Chas = KtiInternalGlobal->ChaList[KtiInternalGlobal->SbspSoc];
    for (Ctr = 0; Ctr < MAX_SOCKET; Ctr++) {
      if (SocketData->Cpu[Ctr].Valid == TRUE) {
        MemCopy ((UINT8 *)host->nvram.kti.LepInfo[Ctr], (UINT8 *)SocketData->Cpu[Ctr].LinkData, sizeof (host->nvram.kti.LepInfo[Ctr]));
      }
    }
    return KTI_SUCCESS;
  }

  // We are in S3 resume path; check to make sure config didn't change
  S3ResumeFail = FALSE;
  if (host->var.common.resetRequired == 0) {
    if (host->nvram.kti.Cpus != KtiInternalGlobal->CpuList || host->nvram.kti.Chas != KtiInternalGlobal->ChaList[KtiInternalGlobal->SbspSoc]) {
      S3ResumeFail = TRUE;
    }
    for (Ctr = 0; Ctr < MAX_SOCKET; Ctr++) {
      if (S3ResumeFail == TRUE) {
        break;
      }
      if (SocketData->Cpu[Ctr].Valid == TRUE) {
        for (Port = 0; Port < host->var.common.KtiPortCnt; Port++) {
          if (host->nvram.kti.LepInfo[Ctr][Port].Valid != SocketData->Cpu[Ctr].LinkData[Port].Valid) {
            S3ResumeFail = TRUE;
            break;
          }
        }
      }
    }
  }

  if (S3ResumeFail == TRUE) {
    KtiCheckPoint (0xFF, 0xFF, 0xFF, ERR_S3_RESUME, MINOR_ERR_S3_RESUME_TOPOLOGY_MISMATCH, host);
    KtiDebugPrintFatal ((host, KTI_DEBUG_ERROR, "\n Topology Doesn't Match - S3 Resume Failed."));
    KTI_ASSERT (FALSE, ERR_S3_RESUME, MINOR_ERR_S3_RESUME_TOPOLOGY_MISMATCH);
  }

  return KTI_SUCCESS;
}

/**

  Program MSR as workarounds for KTI RC.
  SBSP writs the value into UBOX Sticky Scratch 07 in each cpu socket, BSP itself as well as all PBSPs
  At subsequent warm resets, each cpu socket will program MSRs as instructed by the value in its UBOX Sticky Scratch 07.

  @param host  - Pointer to the system host (root) structure
  @param SocketData - Pointer to socket specific data
  @param KtiInternalGlobal - KTIRC Internal Global data

**/
KTI_STATUS
ProgramMsrForWa (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal
  )
{
  return KTI_SUCCESS;
}

/**

  Program misc registers that need to be programmed before WR.

  @param host  - Pointer to the system host (root) structure
  @param SocketData - Pointer to socket specific data
  @param KtiInternalGlobal - KTIRC Internal Global data

**/
KTI_STATUS
ProgramMiscBeforeWarmReset (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal
  )
{

  UINT8 Idx;
  QPI_TIMEOUT_CTRL_CHABC_SAD_STRUCT  KtiTorTimeout;
  QPI_TIMEOUT_CTRL2_CHABC_SAD_STRUCT KtiTorTimeout2;
  ITCCTRL45_N0_IIO_VTD_STRUCT        ItcCtrl45n0;
  ITCCTRL45_N1_IIO_VTD_STRUCT        ItcCtrl45n1;
  TCERRDIS_IIO_DFX_GLOBAL_STRUCT     TcErrDis;

  for (Idx = 0; Idx < MAX_SOCKET; Idx++) {
    if ((SocketData->Cpu[Idx].Valid == TRUE) && (SocketData->Cpu[Idx].SocType == SOCKET_TYPE_CPU)) {
      KtiTorTimeout.Data = KtiReadPciCfg (host, Idx, 0, QPI_TIMEOUT_CTRL_CHABC_SAD_REG);
      KtiTorTimeout2.Data = KtiReadPciCfg (host, Idx, 0, QPI_TIMEOUT_CTRL2_CHABC_SAD_REG);
      KtiTorTimeout.Bits.enabletortimeout  = 0x1;
      KtiTorTimeout.Bits.entry_ctr_inc_ctl = 0x6;
      KtiTorTimeout.Bits.level0_offset     = 0x16;
      KtiTorTimeout.Bits.level1_offset     = 0x16;
      KtiTorTimeout.Bits.level2_offset     = 0x16;
      KtiTorTimeout.Bits.level3_offset     = 0x16;
      KtiTorTimeout.Bits.level4_offset     = 0x16;
      KtiTorTimeout2.Bits.level6_offset    = 0x16;
      KtiWritePciCfg (host, Idx, 0, QPI_TIMEOUT_CTRL_CHABC_SAD_REG, KtiTorTimeout.Data);
      KtiWritePciCfg (host, Idx, 0, QPI_TIMEOUT_CTRL2_CHABC_SAD_REG, KtiTorTimeout2.Data);

      //
      // s5330373: CLONE SKX Sighting: PCIE:  CSR Value for Isoch credits causes ITC credit overflow to be logged
      // Prior to programming ItcCtrl45.max_isoch, set tcerrdis.itc_irp_cred_of = 1; clear after WR; permanent W/A
      //
      TcErrDis.Data = KtiReadPciCfg (host, Idx, IIO_CSTACK, TCERRDIS_IIO_DFX_GLOBAL_REG);
      TcErrDis.Bits.itc_irp_cred_of = 1;
      KtiWritePciCfg (host, Idx, IIO_CSTACK, TCERRDIS_IIO_DFX_GLOBAL_REG, TcErrDis.Data);

      //
      // Isoc/MeSeg enabling in IIO, OK to program on every boot regardless of Isoc/MeSeg enabled
      // To be programmed only on Cstacks before warm reset
      //
      ItcCtrl45n0.Data = KtiReadPciCfg (host, Idx, IIO_CSTACK, ITCCTRL45_N0_IIO_VTD_REG);
      ItcCtrl45n1.Data = KtiReadPciCfg (host, Idx, IIO_CSTACK, ITCCTRL45_N1_IIO_VTD_REG);
      ItcCtrl45n0.Bits.dfx_max_cache = 0x2e;
      ItcCtrl45n1.Bits.max_isoch = 0x2;
      //
      // 4928588: Cloned From SKX Si Bug Eco: Isoch trafiic does not get priority over Non-isoch traffic for credit returns
      // Note: permanent workaround for SKX, but this workaround won't be needed for CNX
      // This bitfield only makes a difference if there is Isoc traffic, otherwise benign:
      //
      ItcCtrl45n0.Bits.isoch_absolute_priority = 0x1;
      KtiWritePciCfg (host, Idx, IIO_CSTACK, ITCCTRL45_N0_IIO_VTD_REG, ItcCtrl45n0.Data);
      KtiWritePciCfg (host, Idx, IIO_CSTACK, ITCCTRL45_N1_IIO_VTD_REG, ItcCtrl45n1.Data);
    }
  }

  return KTI_SUCCESS;
}

/**

  Program BGF Pointer Separation Overrides as workarounds for KTI RC.
  SBSP writes these values via Mailbox Commands to all populated sockets.  Values will take effect after a warm reset.

  @param host  - Pointer to the system host (root) structure
  @param SocketData - Pointer to socket specific data
  @param KtiInternalGlobal - KTIRC Internal Global data

**/
KTI_STATUS
KtiProgramBGFOverrides (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal
  )
{
  UINT8  Socket;

  if (host->var.common.cpuType == CPU_SKX) {
    for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
       if ((host->var.kti.RasInProgress == FALSE && (KtiInternalGlobal->CpuList & (1 << Socket))) ||
           (host->var.kti.RasInProgress == TRUE && host->var.kti.RasSocId == Socket)) {
        // BIOS_MAILBOX_DATA: pointer separation data:
        // [2:0]: GV side pointer separation (pointer separation for M2PCIe, M3KTI, or M2Mem)
        // [6:4]: Non-GV side pointer separation (pointer separation for PCIe, KTI, or MC)
        // Other bits reserved
        SendMailBoxCmdToPcode(host, Socket, MAILBOX_BIOS_BGF_PTR_SEPARATION_OVERRIDE | (MAILBOX_BIOS_BGF_INTERFACE_KTI_M3KTI << 8),
                                                                    (BGF_POINTER_OVERRIDE_KTI_NON_GV << 4) | BGF_POINTER_OVERRIDE_KTI_GV);
        SendMailBoxCmdToPcode(host, Socket, MAILBOX_BIOS_BGF_PTR_SEPARATION_OVERRIDE | (MAILBOX_BIOS_BGF_INTERFACE_IIO_M2PCIE << 8),
                                                                    (BGF_POINTER_OVERRIDE_IIO_NON_GV << 4) | BGF_POINTER_OVERRIDE_IIO_GV);
        SendMailBoxCmdToPcode(host, Socket, MAILBOX_BIOS_BGF_PTR_SEPARATION_OVERRIDE | (MAILBOX_BIOS_BGF_INTERFACE_MC_M2M_133 << 8),
                                                                    (BGF_POINTER_OVERRIDE_MEM_NON_GV << 4) | BGF_POINTER_OVERRIDE_MEM_GV);
        SendMailBoxCmdToPcode(host, Socket, MAILBOX_BIOS_BGF_PTR_SEPARATION_OVERRIDE | (MAILBOX_BIOS_BGF_INTERFACE_MC_M2M_100 << 8),
                                                                    (BGF_POINTER_OVERRIDE_MEM_NON_GV << 4) | BGF_POINTER_OVERRIDE_MEM_GV);
      }
    }
    host->var.common.resetRequired |= POST_RESET_WARM;
  }

  return KTI_SUCCESS;
}

/**

  SBSP programs bus numbers and mmcfg rules to each socket.
  SBSP has PBSP activate changes

  @param host  - Pointer to the system host (root) structure
  @param SocketData - Pointer to socket specific data
  @param KtiInternalGlobal - KTIRC Internal Global data

**/
KTI_STATUS
KtiFinalCpuBusCfg (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal
  )
{
  UINT8  Index, Targets = 0, Idx2, NumBuses = 0;
  UINT32 WaitTime, MmcfgTgtlist;
  UINT8  TmpTotCpu;
  UINT32 MmcfgGranularity;



  //
  // Create list of valid targets
  // Create MmmcfgTargetList
  // Write new value for MMCFG_RULE.
  //
  if (KtiInternalGlobal->KtiCpuSktHotPlugEn == TRUE) {
    if (host->setup.kti.KtiCpuSktHotPlugTopology == 0) {
      TmpTotCpu =  4;         //online/offline based on 4S topology
    } else {
      TmpTotCpu =  8;         //online/offline based on 8S topology
    }
  } else {
    TmpTotCpu =  MAX_SOCKET;
  }

  KTI_ASSERT((TmpTotCpu <= MAX_SOCKET), ERR_SW_CHECK, MINOR_ERR_SW_CHECK_7B);

  if (host->setup.common.mmCfgSize <= 0x10000000) {
    MmcfgGranularity = SAD_BUS_GRANTY;
  } else {
    MmcfgGranularity = (host->setup.common.mmCfgSize / 1024 / 1024 / SAD_BUS_TARGETS);
  }

  MmcfgTgtlist = 0;
  Idx2 = 0;
  for (Index = 0; Index < TmpTotCpu; Index++) {
    if ((SocketData->Cpu[Index].Valid == TRUE) || (KtiInternalGlobal->KtiCpuSktHotPlugEn == TRUE)) {
      NumBuses = (UINT8)(((((host->var.kti.CpuInfo[Index].CpuRes.BusLimit - host->var.kti.CpuInfo[Index].CpuRes.BusBase + 1) / MmcfgGranularity) - 1)) + 1);
      //Idx2 = (UINT8)(host->var.kti.CpuInfo[Index].CpuRes.BusBase / MmcfgGranularity);
      while (NumBuses) {
        //
        // If hotplug system, preprogram the target list.
        // But only program valid_target for the currently present sockets.
        // During ONL flow this field will be updated
        //
        if (SocketData->Cpu[Index].Valid == TRUE) {
          if ((SocketData->Cpu[Index].SocType == SOCKET_TYPE_CPU) || (SocketFpgasInitialized (host, KtiInternalGlobal))) {
            Targets = Targets | (1 << Idx2);
          }
        }
        if ((SocketData->Cpu[Index].Valid == FALSE) && (KtiInternalGlobal->KtiCpuSktHotPlugEn == FALSE)) {
          MmcfgTgtlist |= SOCKET_LOCAL_NID << (Idx2*4);
        } else {
          MmcfgTgtlist |= Index << (Idx2*4);
        }

        NumBuses--;
        Idx2++;
      }
    }
  }

  //
  // Look through all PBSPs and update bus programming
  //
  for (Index = 0; Index < MAX_SOCKET; Index++) {
    if (((host->var.kti.RasInProgress == FALSE && (KtiInternalGlobal->CpuList & (1 << Index))) ||
        (host->var.kti.RasInProgress == TRUE && host->var.kti.RasSocId == Index))) {

      if (KtiInternalGlobal->SbspSoc != Index) {
        // Check for PBSP_READY
        WaitTime = 5000;
        while ((KtiReadPciCfg (host, Index, 0, SR_MEMORY_DATA_STORAGE_DISPATCH_PIPE_CSR) != BUSCFGSYNC_PBSP_READY) && (WaitTime > 0)){
          KtiFixedDelay(host, 1000); // 1000 microsecs
          WaitTime = WaitTime - 1;
        }
        KTI_ASSERT(WaitTime, ERR_BOOT_MODE, MINOR_ERR_PBSP_S0_BUS_PROGRAM_FAILURE | Index);

        KtiFinalPbspBusCfg (host, SocketData, KtiInternalGlobal, Targets, MmcfgTgtlist, Index);
      }
    }
  }

  //
  // Update bus numbers in FPGAs
  //
  KtiFinalFpgaBusCfg (host, SocketData, KtiInternalGlobal);

  //
  // Now update SBSP.
  //
  KtiFinalSbspBusCfg (host, SocketData, KtiInternalGlobal, Targets, MmcfgTgtlist, KtiInternalGlobal->SbspSoc);
  DumpCurrentBusInfo (host, SocketData, FALSE);

  KtiFinalBusCfgKti (host, SocketData, KtiInternalGlobal, KtiInternalGlobal->SbspSoc);

  //
  // Check that all sockets completed before continuing
  //
  for (Index = 0; Index < MAX_SOCKET; Index++) {
    if (((host->var.kti.RasInProgress == FALSE && (KtiInternalGlobal->CpuList & (1 << Index))) ||
        (host->var.kti.RasInProgress == TRUE && host->var.kti.RasSocId == Index))) {

      if (KtiInternalGlobal->SbspSoc != Index) {
        // Check for PBSP_DONE
        KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\n Wait for S%u to update\n", Index));
        WaitTime = 5000;
        while ((KtiReadPciCfg (host, Index, 0, SR_MEMORY_DATA_STORAGE_DISPATCH_PIPE_CSR) != BUSCFGSYNC_PBSP_DONE) && (WaitTime > 0)){
          KtiFixedDelay(host, 1000); // 1000 microsecs
          WaitTime = WaitTime - 1;
        }
        KTI_ASSERT(WaitTime, ERR_SW_CHECK, MINOR_ERR_SW_CHECK_6E);

        KtiWritePciCfg (host, Index, 0, SR_MEMORY_DATA_STORAGE_DISPATCH_PIPE_CSR, 0);

      }
    }
  }

  //
  // Update KTI bus TADs in PBSPs
  //
  for (Index = 0; Index < MAX_SOCKET; Index++) {
    if ((KtiInternalGlobal->CpuList & (1 << Index)) && (KtiInternalGlobal->SbspSoc != Index)) {
      KtiFinalBusCfgKti (host, SocketData, KtiInternalGlobal, Index);
    }
  }

  return KTI_SUCCESS;
}

/**

  SBSP programs bus numbers and mmcfg rules to each socket.

  @param host  - Pointer to the system host (root) structure
  @param SocketData - Pointer to socket specific data
  @param KtiInternalGlobal - KTIRC Internal Global data
  @param Targets  - Enabled socket targets
  @param MmcfgTgtlist  - Common mmcfg target list
  @param SocId  - Socket to program

**/
KTI_STATUS
KtiFinalPbspBusCfg (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal,
  UINT8               Targets,
  UINT32              MmcfgTgtlist,
  UINT8               SocId
  )
{
  MMCFG_RULE_N0_CHABC_SAD1_STRUCT mmcfgrule0;
  MMCFG_RULE_UBOX_MISC_STRUCT MmcfgRuleUbox;
  UINT32  Data32, Data32_1, MmcfgLocalRule = 0, ChaMmcfgLocalRule = 0;
  UINT32  CpuBusNoValid = 0;
  UINT8   MmcfgLength = 0, MmcfgLengthUbox = 0;
  UINT8   Idx1, Idx3;

  switch (host->setup.common.mmCfgSize/1024/1024) {
    case 64:
      MmcfgLength = 0x6;
      MmcfgLengthUbox = 0x2;
      break;
    case 128:
      MmcfgLength = 0x5;
      MmcfgLengthUbox = 0x1;
      break;
    case 256:
      MmcfgLength = 0x4;
      break;
    case 512:
      MmcfgLength = 0x2;
      break;
    case 1024:
      MmcfgLength = 0x1;
      break;
    case 2048:
      MmcfgLength = 0x0;
      break;
  }

  //
  // Reprogram bus/mmcfg on PBSP sockets
  //
  mmcfgrule0.Data = KtiReadPciCfg (host, SocId, 0, MMCFG_RULE_N0_CHABC_SAD1_REG);
  mmcfgrule0.Bits.rule_enable = 0;
  mmcfgrule0.Bits.length = MmcfgLength;
  mmcfgrule0.Bits.base_address = KtiInternalGlobal->MmcfgBase >> 26;
  mmcfgrule0.Bits.valid_targets = Targets;
  KtiWritePciCfg (host, SocId, 0, MMCFG_RULE_N0_CHABC_SAD1_REG, mmcfgrule0.Data);
  Data32 = 0;
  KtiWritePciCfg (host, SocId, 0, MMCFG_RULE_N1_CHABC_SAD1_REG, Data32);

  //
  // Write new value for MMCFG_TARGET_LIST
  //
  Data32 = MmcfgTgtlist;
  for (Idx1 = 0; Idx1 < SI_MAX_CPU_SOCKETS; Idx1++) {
    if (((Data32 >> Idx1*SADTGT_SIZE) & 0xF) == SocId) {
      Data32 = (Data32 & ~(0xF << (SADTGT_SIZE * Idx1))) | ((SOCKET_LOCAL_NID | SocId) << (SADTGT_SIZE * Idx1));
    }
  }
  KtiWritePciCfg (host, SocId, 0, MMCFG_TARGET_LIST_CHABC_SAD1_REG, Data32);

  //
  // Write new value for MMCFG_RULE_LOCAL_ADDRESS
  //

  //
  // Build and write new value for MMCFG_RULE_LOCAL_ADDRESS
  // Build and write final value for MMCFG_RULE_LOCAL and CpuBusNoValid for PCU and VCU
  //
  Data32 = (UINT8) host->var.kti.CpuInfo[SocId].CpuRes.StackRes[IIO_CSTACK].BusBase;
  Data32_1 = 0;
  Idx3 = 0;
  for (Idx1 = 0; Idx1 < MAX_IIO_STACK; Idx1++) {
    //
    // Check for valid resources
    //
    if (host->var.kti.CpuInfo[SocId].CpuRes.StackRes[Idx1].BusLimit >= host->var.kti.CpuInfo[SocId].CpuRes.StackRes[Idx1].BusBase) {
      if (Idx3 < IIO_PSTACK2) {
        Data32 |= (host->var.kti.CpuInfo[SocId].CpuRes.StackRes[Idx1].BusLimit << (8+8*Idx3));
      } else {
        Data32_1 |= (host->var.kti.CpuInfo[SocId].CpuRes.StackRes[Idx1].BusLimit << (8*(Idx3 - IIO_PSTACK2)));
      }

      ChaMmcfgLocalRule |= Idx1 << (Idx3*4);
      MmcfgLocalRule |= Idx1 << (Idx3*4);
      CpuBusNoValid |= (1 << Idx1);
      Idx3++;
    }
  }

  //
  // Save for use later in KTI/IIO programming
  //
  KtiInternalGlobal->MmcfgLocalRules[SocId][0] = Data32;
  KtiInternalGlobal->MmcfgLocalRules[SocId][1] = Data32_1;
  KtiInternalGlobal->MmcfgLocalRules[SocId][2] = MmcfgLocalRule;

  KtiWritePciCfg (host, SocId, 0, MMCFG_LOCAL_RULE_ADDRESS_CFG_0_CHABC_SAD1_REG, Data32);
  KtiWritePciCfg (host, SocId, 0, MMCFG_LOCAL_RULE_ADDRESS_CFG_1_CHABC_SAD1_REG, Data32_1);

  KtiWritePciCfg (host, SocId, 0, CPUBUSNO_PCU_FUN4_REG, Data32);
  KtiWritePciCfg (host, SocId, 0, CPUBUSNO1_PCU_FUN4_REG, Data32_1);

  CpuBusNoValid |= 0x80000000;
  //
  // Set mode bits for all
  //
  ChaMmcfgLocalRule |= (0x3F << 24);
  KtiWritePciCfg (host, SocId, 0, MMCFG_LOCAL_RULE_CHABC_SAD1_REG, ChaMmcfgLocalRule);

  //
  // Write new value for CPUBUSNO0/1
  //
  Data32 = 0;
  Data32_1 = 0;
  for (Idx1 = 0; Idx1 < MAX_IIO_STACK; Idx1++) {
    //
    // If valid bus assigned, else leave as Zero
    //
    if (host->var.kti.CpuInfo[SocId].CpuRes.StackRes[Idx1].BusLimit) {
      if (Idx1 < IIO_PSTACK3) {
        Data32 |= (host->var.kti.CpuInfo[SocId].CpuRes.StackRes[Idx1].BusBase << (8*Idx1));
      } else {
        Data32_1 |= (host->var.kti.CpuInfo[SocId].CpuRes.StackRes[Idx1].BusBase << (8*(Idx1 - IIO_PSTACK3)));
      }
    }
  }
  KtiWritePciCfg (host, SocId, 0, CPUBUSNO_UBOX_MISC_REG, Data32);
  KtiWritePciCfg (host, SocId, 0, CPUBUSNO1_UBOX_MISC_REG, Data32_1);

  KtiWritePciCfg (host, SocId, 0, CPUBUSNO_VCU_FUN0_REG, Data32);
  KtiWritePciCfg (host, SocId, 0, CPUBUSNO1_VCU_FUN0_REG, Data32_1);

  MmcfgRuleUbox.Data = KtiReadPciCfg (host, SocId, 0, MMCFG_RULE_UBOX_MISC_REG);
  MmcfgRuleUbox.Bits.rule_enable = 0;
  KtiWritePciCfg (host, SocId, 0, MMCFG_RULE_UBOX_MISC_REG, MmcfgRuleUbox.Data);
  MmcfgRuleUbox.Bits.rule_enable = 1;
  MmcfgRuleUbox.Bits.length = MmcfgLengthUbox;
  KtiWritePciCfg (host, SocId, 0, MMCFG_RULE_UBOX_MISC_REG, MmcfgRuleUbox.Data);


  //
  // Enable Vcu decoder.  Pcu decoder is sampled at end of CPL3 processing
  //
  KtiWritePciCfg (host, SocId, 0, CPUBUSNO_VALID_VCU_FUN0_REG, CpuBusNoValid);

  //
  // Pass socket relative mmcfgbase to PBSP for local usage
  //
  KtiWritePciCfg (host, SocId, 0, SR_MEMORY_DATA_STORAGE_DATA_PIPE_CSR, host->var.kti.CpuInfo[SocId].CpuRes.SegMmcfgBase.lo);

  KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\n Initiate S%u update\n", SocId));

  //
  // Tell PBSP to reprogram
  //
  KtiWritePciCfg (host, SocId, 0, SR_MEMORY_DATA_STORAGE_DISPATCH_PIPE_CSR, BUSCFGSYNC_PBSP_GO);

  //
  // Update struc so that we begin accessing socket via new bus number
  //
  host->var.common.SocketFirstBus[SocId] = host->var.kti.CpuInfo[SocId].CpuRes.BusBase;
  host->var.common.SocketLastBus[SocId] = host->var.kti.CpuInfo[SocId].CpuRes.BusLimit;

  host->var.common.mmCfgBaseH[SocId] = host->var.kti.CpuInfo[SocId].CpuRes.SegMmcfgBase.hi;
  host->var.common.mmCfgBaseL[SocId] = host->var.kti.CpuInfo[SocId].CpuRes.SegMmcfgBase.lo;
  host->var.common.segmentSocket[SocId] = host->var.kti.CpuInfo[SocId].CpuRes.PciSegment;

  for (Idx1 = 0; Idx1 < MAX_IIO_STACK; Idx1++) {
    host->var.common.StackBus[SocId][Idx1] = host->var.kti.CpuInfo[SocId].CpuRes.StackRes[Idx1].BusBase;
  }
  return KTI_SUCCESS;
}


/**

  SBSP programs bus numbers and mmcfg rules for self.

  @param host  - Pointer to the system host (root) structure
  @param SocketData - Pointer to socket specific data
  @param KtiInternalGlobal - KTIRC Internal Global data
  @param Targets  - Enabled socket targets
  @param MmcfgTgtlist  - Common mmcfg target list
  @param SocId  - Socket to program

**/
KTI_STATUS
KtiFinalSbspBusCfg (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal,
  UINT8               Targets,
  UINT32              MmcfgTgtlist,
  UINT8               SocId
  )
{
  CPUBUSNO_VALID_UBOX_MISC_STRUCT cpubusnovalid;
  MMCFG_RULE_N0_CHABC_SAD1_STRUCT mmcfgrule0;
  MMCFG_RULE_UBOX_MISC_STRUCT MmcfgRuleUbox;
  UINT32  Data32, Data32_1, MmcfgLocalRule = 0, ChaMmcfgLocalRule = 0;
  UINT8   Idx1, Idx2, Idx3;
  UINT32  CpuBusNoValid = 0;
  UINT8   MmcfgLength = 0, MmcfgLengthUbox = 0;

  switch (host->setup.common.mmCfgSize/1024/1024) {
    case 64:
      MmcfgLength = 0x6;
      MmcfgLengthUbox = 0x2;
      break;
    case 128:
      MmcfgLength = 0x5;
      MmcfgLengthUbox = 0x1;
      break;
    case 256:
      MmcfgLength = 0x4;
      break;
    case 512:
      MmcfgLength = 0x2;
      break;
    case 1024:
      MmcfgLength = 0x1;
      break;
    case 2048:
      MmcfgLength = 0x0;
      break;
  }

  //
  // Reprogram bus/mmcfg on SBSP socket
  //
  //
  // This code depends on SBSP residing on Bus0,1,2,3,4,5.  We take advantage of the default bus decoding when
  // MMCFG and CPUBUSNO are disabled to complete the final programming.
  //

  MmcfgRuleUbox.Data = KtiReadPciCfg (host, SocId, 0, MMCFG_RULE_UBOX_MISC_REG);
  MmcfgRuleUbox.Bits.rule_enable = 0;
  KtiWritePciCfg (host, SocId, 0, MMCFG_RULE_UBOX_MISC_REG, MmcfgRuleUbox.Data);
  MmcfgRuleUbox.Bits.rule_enable = 1;
  MmcfgRuleUbox.Bits.length = MmcfgLengthUbox;
  KtiWritePciCfg (host, SocId, 0, MMCFG_RULE_UBOX_MISC_REG, MmcfgRuleUbox.Data);

  //
  // Disable current CPUBUSNO Valid bit
  //
  cpubusnovalid.Data = KtiReadPciCfg (host, SocId, 0, CPUBUSNO_VALID_UBOX_MISC_REG);
  cpubusnovalid.Bits.busno_programmed = 0;
  KtiWritePciCfg (host, SocId, 0, CPUBUSNO_VALID_UBOX_MISC_REG, cpubusnovalid.Data);

  //
  // At this point, local CSRs are moved back to default buses
  // Disable current MMCFG_RULE (at default bus#)
  //
  KtiOutPort32 (host, 0x0cf8, LEGACY_CSR_MMCFG_RULE_N0);
  mmcfgrule0.Data = InPort32 (host, 0x0cfc);
  mmcfgrule0.Bits.rule_enable = 0;
  KtiOutPort32 (host, 0x0cfc, mmcfgrule0.Data);

  //
  // At this point, only local CSRs are accessiable via CF8/CFC IO ports and on default buses
  //
  // Write new value for MMCFG_TARGET_LIST
  KtiOutPort32 (host, 0x0cf8, LEGACY_CSR_MMCFG_TARGET_LIST);
  Data32 = MmcfgTgtlist;

  for (Idx1 = 0; Idx1 < SI_MAX_CPU_SOCKETS; Idx1++) {
    if (((Data32 >> Idx1*SADTGT_SIZE) & 0xF) == SocId) {
      Data32 = (Data32 & ~(0xF << (SADTGT_SIZE * Idx1))) | ((8 | SocId) << (SADTGT_SIZE * Idx1));
    }
  }

  KtiOutPort32 (host, 0x0cfc, Data32);

  //
  // Build and write new value for MMCFG_RULE_LOCAL_ADDRESS
  // Build and write final value for MMCFG_RULE_LOCAL
  //
  Data32 = (UINT8) host->var.kti.CpuInfo[SocId].CpuRes.StackRes[IIO_CSTACK].BusBase;
  Data32_1 = 0;
  Idx3 = 0;
  for (Idx1 = 0; Idx1 < MAX_IIO_STACK; Idx1++) {
    //
    // Check for valid resources
    //
    if (host->var.kti.CpuInfo[SocId].CpuRes.StackRes[Idx1].BusLimit >= host->var.kti.CpuInfo[SocId].CpuRes.StackRes[Idx1].BusBase) {
      if (Idx3 < IIO_PSTACK2) {
        Data32 |= (host->var.kti.CpuInfo[SocId].CpuRes.StackRes[Idx1].BusLimit << (8+8*Idx3));
      } else {
        Data32_1 |= (host->var.kti.CpuInfo[SocId].CpuRes.StackRes[Idx1].BusLimit << (8*(Idx3 - IIO_PSTACK2)));
      }

      ChaMmcfgLocalRule |= Idx1 << (Idx3*4);
      MmcfgLocalRule |= Idx1 << (Idx3*4);
      CpuBusNoValid |= (1 << Idx1);
      Idx3++;
    }
  }

  //
  // Save for use later in KTI/IIO programming
  //
  KtiInternalGlobal->MmcfgLocalRules[SocId][0] = Data32;
  KtiInternalGlobal->MmcfgLocalRules[SocId][1] = Data32_1;
  KtiInternalGlobal->MmcfgLocalRules[SocId][2] = MmcfgLocalRule;


  KtiOutPort32 (host, 0x0cf8, LEGACY_CSR_MMCFG_LOCAL_RULE_ADDRESS_CFG_0);
  KtiOutPort32 (host, 0x0cfc, Data32);

  KtiOutPort32 (host, 0x0cf8, LEGACY_CSR_MMCFG_LOCAL_RULE_ADDRESS_CFG_1);
  KtiOutPort32 (host, 0x0cfc, Data32_1);

  KtiOutPort32 (host, 0x0cf8, LEGACY_CSR_CPUBUSNO_PCU);
  KtiOutPort32 (host, 0x0cfc, Data32);

  KtiOutPort32 (host, 0x0cf8, LEGACY_CSR_CPUBUSNO1_PCU);
  KtiOutPort32 (host, 0x0cfc, Data32_1);

  CpuBusNoValid |= 0x80000000;
  //
  // Set mode bits for all but Cstack
  //
  ChaMmcfgLocalRule |= (0x3E << 24);
  KtiOutPort32 (host, 0x0cf8, LEGACY_CSR_MMCFG_LOCAL_RULE);
  KtiOutPort32 (host, 0x0cfc, ChaMmcfgLocalRule);

  //
  // Write new value for CPUBUSNO0/1
  //
  Data32 = 0;
  Data32_1 = 0;
  for (Idx1 = 0; Idx1 < MAX_IIO_STACK; Idx1++) {
    //
    // If valid bus assigned, else leave as Zero
    //
    if (host->var.kti.CpuInfo[SocId].CpuRes.StackRes[Idx1].BusLimit) {
      if (Idx1 < IIO_PSTACK3) {
        Data32 |= (host->var.kti.CpuInfo[SocId].CpuRes.StackRes[Idx1].BusBase << (8*Idx1));
      } else {
        Data32_1 |= (host->var.kti.CpuInfo[SocId].CpuRes.StackRes[Idx1].BusBase << (8*(Idx1 - IIO_PSTACK3)));
      }
    }
  }

  KtiOutPort32 (host, 0x0cf8, LEGACY_CSR_CPUBUSNO_UBOX);
  KtiOutPort32 (host, 0x0cfc, Data32);

  KtiOutPort32 (host, 0x0cf8, LEGACY_CSR_CPUBUSNO1_UBOX);
  KtiOutPort32 (host, 0x0cfc, Data32_1);

  KtiOutPort32 (host, 0x0cf8, LEGACY_CSR_CPUBUSNO_VCU);
  KtiOutPort32 (host, 0x0cfc, Data32);

  KtiOutPort32 (host, 0x0cf8, LEGACY_CSR_CPUBUSNO1_VCU);
  KtiOutPort32 (host, 0x0cfc, Data32_1);

  //
  // Enable Vcu decoder.  Pcu decoder is sampled at end of CPL3 processing
  //
  KtiOutPort32 (host, 0x0cf8, LEGACY_CSR_CPUBUSNO_VALID_VCU);
  KtiOutPort32 (host, 0x0cfc, CpuBusNoValid);

  //
  // Write new value for MMCFG_RULE
  // And enable MMCFG
  //
  Data32 = 0;
  KtiOutPort32 (host, 0x0cf8, LEGACY_CSR_MMCFG_RULE_N1);
  KtiOutPort32 (host, 0x0cfc, Data32);

  KtiOutPort32 (host, 0x0cf8, LEGACY_CSR_MMCFG_RULE_N0);
  mmcfgrule0.Data = InPort32 (host, 0x0cfc);
  mmcfgrule0.Bits.rule_enable = 1;
  mmcfgrule0.Bits.length = MmcfgLength;
  mmcfgrule0.Bits.base_address = KtiInternalGlobal->MmcfgBase >> 26;
  mmcfgrule0.Bits.valid_targets= Targets;
  KtiOutPort32 (host, 0x0cfc, mmcfgrule0.Data);

  //
  // Update struc so that we begin accessing socket via new bus number
  //
  host->var.common.SocketFirstBus[SocId] = host->var.kti.CpuInfo[SocId].CpuRes.BusBase;
  host->var.common.SocketLastBus[SocId] = host->var.kti.CpuInfo[SocId].CpuRes.BusLimit;

  host->var.common.mmCfgBase = KtiInternalGlobal->MmcfgBase;
  host->var.common.mmCfgBaseH[SocId] = host->var.kti.CpuInfo[SocId].CpuRes.SegMmcfgBase.hi;
  host->var.common.mmCfgBaseL[SocId] = host->var.kti.CpuInfo[SocId].CpuRes.SegMmcfgBase.lo;
  host->var.common.segmentSocket[SocId] = host->var.kti.CpuInfo[SocId].CpuRes.PciSegment;

  //
  // Dump out newly activated strucs
  //
  KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\n host->var.common.mmCfgBase         = 0x%08x \n", host->var.common.mmCfgBase));
// APTIOV_SERVER_OVERRIDE_RC_START : Dynamic mmcfg base address Support change
#ifdef DYNAMIC_MMCFG_BASE_FLAG
  PcdSet64 (PcdPciExpressBaseAddress, host->var.common.mmCfgBase);
  KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\nPcdPciExpressBaseAddress:%x\n",PcdGet64 (PcdPciExpressBaseAddress)));
#endif
// APTIOV_SERVER_OVERRIDE_RC_END : Dynamic mmcfg base address Support change
  for (Idx1 = 0; Idx1 < MAX_SOCKET; Idx1++) {
    if ((SocketData->Cpu[Idx1].Valid == TRUE) ) {
      KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, " host->var.common.segmentSocket[%u]  =       0x%02x  \n", Idx1, host->var.common.segmentSocket[Idx1]));
      KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, " host->var.common.SocketFirstBus[%u] =       0x%02x  \n", Idx1, host->var.common.SocketFirstBus[Idx1]));
      KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, " host->var.common.SocketLastBus[%u]  =       0x%02x  \n", Idx1, host->var.common.SocketLastBus[Idx1]));
      KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, " host->var.common.mmCfgBaseH[%u]     = 0x%08x  \n", Idx1, host->var.common.mmCfgBaseH[Idx1]));
      KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, " host->var.common.mmCfgBaseL[%u]     = 0x%08x  \n\n", Idx1, host->var.common.mmCfgBaseL[Idx1]));

    }
  }
  //
  // Enable Ubox decoder.  This works since always on Bus0 of the socket
  //
  DisableCsrAddressCache(host);
  Data32 = KtiReadPciCfg (host, SocId, 0, CPUBUSNO_VALID_UBOX_MISC_REG);
  Data32 = Data32 | 0x80000000;
  KtiWritePciCfg (host, SocId, 0, CPUBUSNO_VALID_UBOX_MISC_REG, Data32);
  EnableCsrAddressCache(host);

  for (Idx1 = 0; Idx1 < MAX_IIO_STACK; Idx1++) {
    host->var.common.StackBus[SocId][Idx1] = host->var.kti.CpuInfo[SocId].CpuRes.StackRes[Idx1].BusBase;
  }

  GetCpuCsrAccessVar_RC (host, &host->var.CpuCsrAccessVarHost);

  //
  // Make missing socket/stacks -1
  //
  for (Idx1 = 0; Idx1 < MAX_SOCKET; Idx1++) {
    if (SocketData->Cpu[Idx1].Valid == FALSE) {
      for (Idx2 = 0; Idx2 < MAX_IIO_STACK; Idx2++) {
        host->var.common.StackBus[Idx1][Idx2] = 0xFF;
      }
    }
  }

  return KTI_SUCCESS;
}

/**

  SBSP programs bus numbers for fpgas.

  @param host  - Pointer to the system host (root) structure
  @param SocketData - Pointer to socket specific data
  @param KtiInternalGlobal - KTIRC Internal Global data

**/
KTI_STATUS
KtiFinalFpgaBusCfg (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal
  )
{
  UINT32    Data32;
  UINT8     Socket, Idx1;

  if (SocketFpgasInitialized (host, KtiInternalGlobal)) {
    for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
      if ((SocketData->Cpu[Socket].Valid == TRUE) && (SocketData->Cpu[Socket].SocType == SOCKET_TYPE_FPGA)) {
        //
        // Update common structure FpgaPresentBitMap to enable CSR access routines
        //
        host->var.common.FpgaPresentBitMap |= (1 << Socket);
        GetCpuCsrAccessVar_RC (host, &host->var.CpuCsrAccessVarHost);
        //
        // Program bus number into Fpga.  This is first CSR access to the device
        //
        Data32 = KtiReadPciCfg (host, Socket, 0, FPGA_BUSNO_FPGA_CCI_REG);
        Data32 = 0x80000000 | host->var.kti.CpuInfo[Socket].CpuRes.BusBase;
        KtiWritePciCfg (host, Socket, 0, FPGA_BUSNO_FPGA_CCI_REG, Data32);

        //
        // Update common structures w/ updated bus numbers for FPGA to be used in CSR access routines after SBSP reprograms self
        //
        host->var.common.SocketFirstBus[Socket] = host->var.kti.CpuInfo[Socket].CpuRes.BusBase;
        host->var.common.SocketLastBus[Socket] = host->var.kti.CpuInfo[Socket].CpuRes.BusLimit;
        host->var.common.mmCfgBaseH[Socket] = host->var.kti.CpuInfo[Socket].CpuRes.SegMmcfgBase.hi;
        host->var.common.mmCfgBaseL[Socket] = host->var.kti.CpuInfo[Socket].CpuRes.SegMmcfgBase.lo;
        host->var.common.segmentSocket[Socket] = host->var.kti.CpuInfo[Socket].CpuRes.PciSegment;

        for (Idx1 = 0; Idx1 < MAX_IIO_STACK; Idx1++) {
          host->var.common.StackBus[Socket][Idx1] = host->var.kti.CpuInfo[Socket].CpuRes.StackRes[Idx1].BusBase;
        }
      }
    }
  }
  return KTI_SUCCESS;

}

/**

  Routine to program the final MMCFG SAD for the given socket links

  @param host  - Pointer to the system host (root) structure
  @param SocketData - Pointer to socket specific data
  @param KtiInternalGlobal - KTIRC Internal Global data
  @param SocId  - Socket to program

  @retval 0 - Successful
  @retval Other - failure

**/
KTI_STATUS
KtiFinalBusCfgKti (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal,
  UINT8                  SocId
  )
{
  UINT8   BusBaseTmp;
  UINT8   BusLimit;


  UINT32  BusLocalLimits[2] = {0,0};

  UINT32  BusLocalTgtLst = 0;
  UINT8   TargetIndex = 0, LinkIndex, Stack;

  //
  // If PCH attached route dev 16-31 down DMI link
  //
  if (SocId == KtiInternalGlobal->SbspSoc) {
    BusLocalTgtLst |= 1 << 24;
  }
  //
  // Setup starting base of legacy IO for the socket
  //
  BusLocalLimits[0] = (UINT32) (host->var.kti.CpuInfo[SocId].CpuRes.BusBase);

  for (Stack = 0; Stack < MAX_IIO_STACK; Stack++) {
    //
    // Program a target and limit for each enabled stack
    //
    if (host->var.common.stackPresentBitmap[SocId] & (1 << Stack)) {
      BusBaseTmp = host->var.kti.CpuInfo[SocId].CpuRes.StackRes[Stack].BusBase;
      if (BusBaseTmp != 0xFF) {
        BusLocalTgtLst |= Stack << (TargetIndex * 3);
        BusLimit = host->var.kti.CpuInfo[SocId].CpuRes.StackRes[Stack].BusLimit;
        if (TargetIndex < 3) {
          BusLocalLimits[TargetIndex / 3] |= (BusLimit << ((TargetIndex + 1) * 8)) ;
        } else {
          BusLocalLimits[TargetIndex / 3] |= (BusLimit << ((TargetIndex - 3) * 8));
        }
        TargetIndex++;
      }
    }
  }

  for (LinkIndex = 0; LinkIndex < host->var.common.KtiPortCnt; LinkIndex++) {
    if (SocketData->Cpu[SocId].LinkData[LinkIndex].Valid == TRUE){
      KtiWritePciCfg (host, SocId, LinkIndex, KTILK_TAD_CFG0_KTI_LLPMON_REG, BusLocalTgtLst);
      KtiWritePciCfg (host, SocId, LinkIndex, KTILK_TAD_CFG1_KTI_LLPMON_REG, BusLocalLimits[0]);
      KtiWritePciCfg (host, SocId, LinkIndex, KTILK_TAD_CFG2_KTI_LLPMON_REG, BusLocalLimits[1]);
    }
  }

  return KTI_SUCCESS;
}

UINT32 ChaSncBaseRegisterOffset[MAX_CLUSTERS +1] = {SNC_BASE_1_CHABC_PMA_REG,
                                                    SNC_BASE_2_CHABC_PMA_REG,
                                                    SNC_BASE_3_CHABC_PMA_REG,
                                                    SNC_BASE_4_CHABC_PMA_REG,
                                                    SNC_BASE_5_CHABC_PMA_REG};

UINT32 IioSncBaseRegisterOffset[MAX_CLUSTERS +1] = {SNC_BASE_1_IIO_VTD_REG,
                                                    SNC_BASE_2_IIO_VTD_REG,
                                                    SNC_BASE_3_IIO_VTD_REG,
                                                    SNC_BASE_4_IIO_VTD_REG,
                                                    SNC_BASE_5_IIO_VTD_REG};

UINT32 KtiSncBaseRegisterOffset[MAX_CLUSTERS +1] = {KTILK_SNC_BASE_1_KTI_LLPMON_REG,
                                                    KTILK_SNC_BASE_2_KTI_LLPMON_REG,
                                                    KTILK_SNC_BASE_3_KTI_LLPMON_REG,
                                                    KTILK_SNC_BASE_4_KTI_LLPMON_REG,
                                                    KTILK_SNC_BASE_5_KTI_LLPMON_REG};

/**
  Program Kti Prefetch setting

  This routine is to enable kti prefetch feature

  @param  host                    Pointer to the system host (root) structure
  @param  Socket                  Socket Id
  @param  Minisadmodeselect       Minisad mode
  @param  MiniSadTable            Minisad table value

  @retval None

**/
VOID
ProgramKtiPrefetchRegister(
  struct sysHost             *host,
  UINT8                      Socket,
  UINT32                     Minisadmodeselect,
  UINT32                     MiniSadTable
  )
{
  M3KPRECTRL_M3KTI_MAIN_STRUCT             M3KPreCtrl;
  KTIAGCTRL_KTI_LLPMON_STRUCT              KtiAgCtrl;
  UINT8                                    Ctr;

  // M3KTI
  for (Ctr = 0; Ctr < host->var.kti.CpuInfo[Socket].TotM3Kti; Ctr++) {
    // mini sad mode select
    M3KPreCtrl.Data = KtiReadPciCfg (host, Socket, Ctr, M3KPRECTRL_M3KTI_MAIN_REG);
    M3KPreCtrl.Bits.mini_sad_mode = Minisadmodeselect;
    //
    // 4929504: KTI tile performance critical CSRs for BIOS programming
    //
    M3KPreCtrl.Bits.remspecrdthreshold = 2;
    KtiWritePciCfg (host, Socket, Ctr, M3KPRECTRL_M3KTI_MAIN_REG, M3KPreCtrl.Data);
    //set mini sad table
    KtiWritePciCfg (host, Socket, Ctr, M3KPRETL_M3KTI_MAIN_REG , MiniSadTable);
  }

  // KTIAgent
  for (Ctr = 0; Ctr < host->var.common.KtiPortCnt; Ctr++) {
    if (host->var.kti.CpuInfo[Socket].LepInfo[Ctr].Valid == TRUE) {
      KtiAgCtrl.Data = KtiReadPciCfg (host, Socket, Ctr, KTIAGCTRL_KTI_LLPMON_REG);
      KtiAgCtrl.Bits.enableprefetch = 1;
      host->var.kti.OutKtiPrefetch = 1;
      if (host->setup.kti.RdCurForXptPrefetchEn < KTI_AUTO) {
        if (host->setup.kti.RdCurForXptPrefetchEn == KTI_ENABLE) {
          KtiAgCtrl.Bits.suppress_mem_rd_prefetch_rdcur = 0;          //enable RdCur
        } else {
          KtiAgCtrl.Bits.suppress_mem_rd_prefetch_rdcur = 1;          //disable RdCur
        }
      }
      KtiWritePciCfg (host, Socket, Ctr, KTIAGCTRL_KTI_LLPMON_REG, KtiAgCtrl.Data);
    }
  }
}

/**
  To ensure that XPT prefetch that maps to MMIO hole is handled, here is algorithm need to check

  If( MMIO_Low size <= DRAM_Low size AND
     total system DRAM capacity >= 2GB )  option (i) config
  Else If( MMIO_Low size <= DRAM_High DRAM capacity ) option (ii) config,
  Else
      disable prefetch.

  i.  M2M PrefSadConfig[PrefMmioBaseAddr] = start address of the MMIO_Low,
      M2M PrefSadConfig[FirstPfTadID]  = 0,
      M2M PrefSadConfig[SecondPfTadID]  = 1,
      MC tadchnilvoffset0[tad_offset]  = 0,
      MC tadchnilvoffset1[tad_offset]  = size of MMIO_Low.

 ii.  M2M PrefSadConfig[PrefMmioBaseAddr] = start address of DRAM-High,
      M2M PrefSadConfig[FirstPfTadID]  = 0,
      M2M PrefSadConfig[SecondPfTadID]  = 1,
      MC tadchnilvoffset0[tad_offset]  = 0,
      MC tadchnilvoffset1[tad_offset]  = size of MMIO_Low.

  @param  host                    Pointer to the system host (root) structure

  @retval Option
**/
UINT8
PrefetchOption(
  struct sysHost             *host
)
{
  UINT8  Option;
  if ((host->setup.common.lowGap  <= host->var.mem.lowMemSize) && (host->var.mem.memSize >= 0x20)){
    Option = 1;
  } else if ( host->setup.common.lowGap <= host->var.mem.highMemSize){
    Option = 2;
  } else {
    Option = 0;  // disabled xpt prefetch
  }


  KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "Option %d lowGap %x total_memSize %x lowMemSize %x  highMemBase %x highMemSize %x )\n",
                             Option, host->setup.common.lowGap, host->var.mem.memSize, host->var.mem.lowMemSize, host->var.mem.highMemBase, host->var.mem.highMemSize));
  return Option;
}


/*++
  Program Prefetech M2MEM registers for XPT and KTI prefetech

  @param  host                    Pointer to the system host (root) structure
  @param  Socket                  Socket Id
  @param  MemInfo                 Memory Information structure.
  @param  UINT8                   Option

  @retval None
--*/
VOID
PrefetchM2mem(
  struct sysHost             *host,
  UINT8                      Socket,
  MEM_INFO                   *MemInfo,
  UINT8                      Option
)
{
  UINT8                                    Ctr, Entry;
  PREFETCHSAD_M2MEM_MAIN_STRUCT            M2MemPrefetchSad;
  PREFSADCONFIG_M2MEM_MAIN_STRUCT          M2MemPrefSadConfig;
  SYSFEATURES0_M2MEM_MAIN_STRUCT           SysFeatures0;


  // M2MEM
  for (Ctr = 0; Ctr < host->var.mem.maxIMC; Ctr++) {
    if (host->nvram.mem.socket[Socket].imc[Ctr].enabled == 0) continue;
      M2MemPrefSadConfig.Data = 0;
      M2MemPrefetchSad.Data = 0;

      if (MemInfo->SncInfo[Socket].InterleaveEn == 1){        // if inteleave, then get from mc0
        M2MemPrefetchSad.Bits.prefsadchnls = MemInfo->McInfo[Socket][0].ChannelInterBitmap;
      } else {
        M2MemPrefetchSad.Bits.prefsadchnls = MemInfo->McInfo[Socket][Ctr].ChannelInterBitmap;
      }
      //
      // 4929958: should set prefectsad interleave granularity to 0x2 for 256B (xml header file to be updated)
      //
      if (MemInfo->SncInfo[Socket].Interleave_256B == 1){
        M2MemPrefetchSad.Bits.prefsadinterlv = 2;            //256B (with 256B channel granularity)
      }

      // update tadid to specify the mmio base address for the mc has mmiol range
      if (MemInfo->McInfo[Socket][Ctr].SecondPfTadID != 0){
         M2MemPrefSadConfig.Bits.firstpftadid = MemInfo->McInfo[Socket][Ctr].FirstPfTadID;
         M2MemPrefSadConfig.Bits.secondpftadid = MemInfo->McInfo[Socket][Ctr].SecondPfTadID;

         if (Option == 1) {       // for option 1
           M2MemPrefSadConfig.Bits.prefmmiobaseaddr = (UINT32)(host->var.common.tolmLimit >> 2);    //256MB boundary
         } else {                 // for option 2
           M2MemPrefSadConfig.Bits.prefmmiobaseaddr = (UINT32)(host->var.mem.highMemBase >> 2);     //256MB boundary
         }
      }

      //Since each iMC only supports 1 SNC now, all 4 entries in the SNC table will need to be programmed to the same value.
      for (Entry = 0; Entry < MAX_CLUSTERS; Entry++) {

        M2MemPrefetchSad.Bits.prefsadentry = Entry;    //SNC table entry
        M2MemPrefetchSad.Bits.prefsadwren = 0;         //write enable command, not commit yet
        KtiWritePciCfg (host, Socket, Ctr, PREFETCHSAD_M2MEM_MAIN_REG, M2MemPrefetchSad.Data);
        // update tadid to specify the mmio base address per entry setting, i.e. cannot move this write out of entry loop
        if (MemInfo->McInfo[Socket][Ctr].SecondPfTadID != 0){
          KtiWritePciCfg (host, Socket, Ctr, PREFSADCONFIG_M2MEM_MAIN_REG, M2MemPrefSadConfig.Data);
        }
        
        M2MemPrefetchSad.Bits.prefsadvld = 1;
        KtiWritePciCfg(host, Socket, Ctr, PREFETCHSAD_M2MEM_MAIN_REG, M2MemPrefetchSad.Data);
        
        M2MemPrefetchSad.Bits.prefsadwren = 1;        //write enable command, commit now
        KtiWritePciCfg (host, Socket, Ctr, PREFETCHSAD_M2MEM_MAIN_REG, M2MemPrefetchSad.Data);
        M2MemPrefetchSad.Bits.prefsadwren = 0;        //Clear write enable command, 
        KtiWritePciCfg(host, Socket, Ctr, PREFETCHSAD_M2MEM_MAIN_REG, M2MemPrefetchSad.Data);

      } // Snc entry loop

      // prefetch enabled
      SysFeatures0.Data = KtiReadPciCfg(host, Socket, Ctr, SYSFEATURES0_M2MEM_MAIN_REG);
      SysFeatures0.Bits.prefdisable = 0;
      KtiWritePciCfg (host, Socket, Ctr, SYSFEATURES0_M2MEM_MAIN_REG, SysFeatures0.Data);

      // program into m2mem prefetch ad ingress credit register
      // Credit Type 0xb : per Mc
      ProgramM2mCreditReg(host, Socket, Ctr, 0, 0xb, 8);

  } // M2mem loop

}

/*++
  Disable XPT Prefetech

  @param  host                    Pointer to the system host (root) structure
  @param  Socket                  Socket Id

  @retval None
--*/
VOID
DisableXptPrefetch(
  struct sysHost             *host,
  UINT8                      Socket
)
{
  XPT_PREFETCH_CONFIG1_CHABC_UTIL_STRUCT   XptPrefetchConfig1;
  SYSFEATURES0_M2MEM_MAIN_STRUCT           SysFeatures0;
  UINT8                                    Ctr;

  // CHA
  XptPrefetchConfig1.Data = KtiReadPciCfg (host, Socket, 0, XPT_PREFETCH_CONFIG1_CHABC_UTIL_REG);
  XptPrefetchConfig1.Bits.xptprefetchdisable = 1;
  KtiWritePciCfg (host, Socket, 0, XPT_PREFETCH_CONFIG1_CHABC_UTIL_REG , XptPrefetchConfig1.Data);

  // M2M
  for (Ctr = 0; Ctr < host->var.mem.maxIMC; Ctr++) {
    SysFeatures0.Data = KtiReadPciCfg(host, Socket, Ctr, SYSFEATURES0_M2MEM_MAIN_REG);
    SysFeatures0.Bits.prefdisable = 1;
    KtiWritePciCfg (host, Socket, Ctr, SYSFEATURES0_M2MEM_MAIN_REG, SysFeatures0.Data);
  }

  return;
}


/**
  Determine SNC setting

  This routine is to enable xpt prefetch feature

  @param  host                    Pointer to the system host (root) structure
  @param  Socket                  Socket Id
  @param  MemInfo                 Memory Information structure.

  @retval None

**/
VOID
ProgramXptKtiPrefetch(
  struct sysHost             *host,
  UINT8                      Socket,
  MEM_INFO                   *MemInfo
  )
{
  XPT_PREFETCH_CONFIG1_CHABC_UTIL_STRUCT   XptPrefetchConfig1;
  XPT_PREFETCH_CONFIG2_CHABC_UTIL_STRUCT   XptPrefetchConfig2;

  UINT32                                   MiniSadTable, XptMiniSadTable_CHABC;
  UINT32                                   Minisadmodeselect;
  UINT32                                   BiosScratchPad7;
  UINT8                                    Option;

  MiniSadTable          = 0;
  XptMiniSadTable_CHABC = 0;

  Option = PrefetchOption(host);
  //Check xpt prefetch if need to be disabled
  if ((host->var.kti.OutSncPrefetchEn == 0) || (MemInfo->SncInfo[Socket].PrefetchEn == 0) || (Option == 0) || (MemInfo->PrefetchAll == 0)){
    KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\nSocket %d XPT and KTI Disabled", Socket));
    DisableXptPrefetch(host, Socket);
    return;
  }

  if(host->nvram.mem.RASmode & FULL_MIRROR_1LM || host->nvram.mem.RASmode & FULL_MIRROR_2LM  || host->nvram.mem.RASmode & PARTIAL_MIRROR_1LM || host->nvram.mem.RASmode & PARTIAL_MIRROR_2LM) {
    KtiDebugPrintWarn((host, KTI_DEBUG_WARN, "\n WARNING:  KTI Disabled - cannot enable KTI and Mirror together"));
    DisableXptPrefetch(host, Socket);
    return;
  }

  //
  //WA: s5372910 - (B step) Disable XPT prefetch whenever DDRT is in the system
  //
  if (host->var.common.stepping == B0_REV_SKX || host->var.common.stepping == L0_REV_SKX || host->var.common.stepping == B1_REV_SKX) {
    BiosScratchPad7 = KtiReadPciCfg(host, GetSbspSktId(host), 0, BIOSSCRATCHPAD7_UBOX_MISC_REG); // If AEP DIMM present (indicated bit StickyPad7[31]), update MemInfo structure
    if (BiosScratchPad7 & BIT31) {
      KtiDebugPrintWarn((host, KTI_DEBUG_WARN, "\n Workaround: XptPrefetch disabled to support DDRT on B step"));
      DisableXptPrefetch(host, Socket);
      return;
    }
  }

  KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\nSocket %d XPT or KTI Enabled(Interleave En = %d, Interleave 256B = %d)\n",
                            Socket, MemInfo->SncInfo[Socket].InterleaveEn, MemInfo->SncInfo[Socket].Interleave_256B));

  //Program XPT MiniSAD entries
  if (MemInfo->SncInfo[Socket].InterleaveEn == 0){   // NUMA non-interleave

    MiniSadTable = 0x11001100;
    XptMiniSadTable_CHABC = 0x99889988;
    if ((MemInfo->McInfo[Socket][0].Enabled == 0) && (MemInfo->McInfo[Socket][1].Enabled == 1)){
      MiniSadTable = 0x00110011;
      XptMiniSadTable_CHABC = 0x88998899;
    }

    Minisadmodeselect = 0;
  } else {                                           // NUMA interleave
    // s5370398
    if ((host->var.common.cpuType == CPU_SKX) && (host->var.common.stepping >= B0_REV_SKX)) {
      MiniSadTable = 0x10101010;
      XptMiniSadTable_CHABC = 0x98989898;              // {Generate_Prefetch, 0, 0, MC_ID}
    } else {
      // check if 2 channels per Mc, then use P9 or P7 to select Mc, P8 or P6 to select channel
      // Currently we don't support P8 or P6 to select Mc, P9 or P7 to select channel
      if ((MemInfo->McInfo[Socket][0].ChannelInterBitmap == BITMAP_CH0_CH1)
          ||(MemInfo->McInfo[Socket][0].ChannelInterBitmap == BITMAP_CH1_CH2)
          ||(MemInfo->McInfo[Socket][0].ChannelInterBitmap == BITMAP_CH0_CH2)) {
        MiniSadTable = 0x11110000;
        XptMiniSadTable_CHABC = 0x99998888;              // {Generate_Prefetch, 0, 0, MC_ID}
      } else {
        MiniSadTable = 0x10101010;
        XptMiniSadTable_CHABC = 0x98989898;              // {Generate_Prefetch, 0, 0, MC_ID}
      }
    }

    if (MemInfo->SncInfo[Socket].Interleave_256B == 1){
      Minisadmodeselect = 1;  // 256B interleave, set to mode 1
    } else {
      Minisadmodeselect = 0;  // 64B interleave, set to mode 0
    }
  }

  // Enable XptPrefetch
  KtiWritePciCfg (host, Socket, 0, XPT_MINISAD_TABLE_CHABC_UTIL_REG , XptMiniSadTable_CHABC);

  //Set MiniSAD Mode
  XptPrefetchConfig2.Data = KtiReadPciCfg (host, Socket, 0, XPT_PREFETCH_CONFIG2_CHABC_UTIL_REG);
  XptPrefetchConfig2.Bits.xptminisadmodeselect = Minisadmodeselect;
  XptPrefetchConfig2.Bits.xptfeedbackwincntlimit=0x186a0;
  XptPrefetchConfig2.Bits.xptforceprefetch      =0;
  if(host->setup.kti.XptPrefetchEn == KTI_ENABLE) {
    KtiWritePciCfg (host, Socket, 0, XPT_PREFETCH_CONFIG2_CHABC_UTIL_REG , XptPrefetchConfig2.Data);
  }

  // M2MEM
  PrefetchM2mem(host, Socket, MemInfo, Option);

  //
  //Enable xpt_prefetch
  //
  // CHA
  XptPrefetchConfig1.Data = KtiReadPciCfg (host, Socket, 0, XPT_PREFETCH_CONFIG1_CHABC_UTIL_REG);
  XptPrefetchConfig1.Bits.xptprefetchdisable = 0;

  //
  // WA: 5331330
  // XptPrefetchConfig1.Bits.xptprefetchsubgrp1dis = 0;
  // XptPrefetchConfig1.Bits.xptprefetchsubgrp2en = 0;
  // XptPrefetchConfig1.Bits.xptprefetchsubgrp3en = 0;
  // XptPrefetchConfig1.Bits.xptprefetchsubgrp4en = 0;
  // XptPrefetchConfig1.Bits.xpthitpredictthresh = 0x14;
  // XptPrefetchConfig1.Bits.xptmispredthresh = 0x100;
  //
  XptPrefetchConfig1.Data = (XptPrefetchConfig1.Data & 0xFC000000) | 0x00040280;
  if(host->setup.kti.XptPrefetchEn == KTI_ENABLE) {
    KtiWritePciCfg (host, Socket, 0, XPT_PREFETCH_CONFIG1_CHABC_UTIL_REG , XptPrefetchConfig1.Data);
  }


  // Enable KtiPrefetch
  if (host->setup.kti.KtiPrefetchEn != KTI_DISABLE) {
    ProgramKtiPrefetchRegister(host, Socket, Minisadmodeselect, MiniSadTable);
  }
}

/**

    SNC Misc and Recovrry if failure

    @param host              - pointer to the system host root structure
    @param SocketData        - pointer to the socket data structure
    @param KtiInternalGlobal - pointer to the KTI RC internal global structure

    @retval KTI_SUCCESS - on successful completion

**/
KTI_STATUS
SncMisc (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal
  )
{
   SNC_CONFIG_CHABC_PMA_STRUCT     SncConfigPma;
   UINT8                           Soc;

  if (host->var.kti.OutSncPrefetchEn) {
     for (Soc = 0; Soc < MAX_SOCKET; Soc++) {
       if ((SocketData->Cpu[Soc].Valid == TRUE) && (SocketData->Cpu[Soc].SocType == SOCKET_TYPE_CPU)) {
          //Update SNC cluster information for MRC usage during warm reset flow
          SncConfigPma.Data = KtiReadPciCfg (host, Soc, 0, SNC_CONFIG_CHABC_PMA_REG);
          if (SncConfigPma.Bits.snc_ind_en == 1) {
            host->var.kti.OutSncGbAlignRequired = 1;
            if (SncConfigPma.Bits.full_snc_en == 1) {
              host->var.kti.OutNumOfCluster[Soc] = 2;
            } else {
              host->var.kti.OutNumOfCluster[Soc] = 1;
            }
          }

          if (host->var.kti.OutNumOfCluster[Soc] != 0) {        // update into  OutNumOfClusterPerSystem for IioUds usage
            host->var.kti.OutNumOfClusterPerSystem = host->var.kti.OutNumOfCluster[Soc];
          }
       } // socket valid
     } // socket loop

  } // Snc enabled

  //
  // s5370269: Disable XPT Prefetch even SNC is disabled
  //
  if(host->setup.kti.XptPrefetchEn == KTI_DISABLE){

    for(Soc = 0; Soc < MAX_SOCKET; Soc++) {
      // check if socket is valid
      if  ((host->var.common.socketPresentBitMap & (BIT0 << Soc)) != 0) {
        DisableXptPrefetch(host, Soc);
      }
    }
  }



  return KTI_SUCCESS;
}

/**
  Determine SNC setting

  This routine determiness SNC CHA slices for each cluster and number of clusters within this socket.
  This setting is based on mc0/1 population and NUMA non-interleave cross mc or interleave cross mc

  @param  host                    Pointer to the system host (root) structure
  @param  MemInfo                 Memory Information structure.

  @retval KTI_SUCCESS - on successful

**/
KTI_STATUS
DetermineSncSetting (
  struct sysHost             *host,
  MEM_INFO                   *MemInfo
  )
{
  UINT8   TempNumOfMcEn, Ch, Soc;

  TempNumOfMcEn = 0xFF;
  for (Soc = 0; Soc < MAX_SOCKET; Soc++) {
    if (host->var.common.socketPresentBitMap & (BIT0 << Soc)) {
      if ( TempNumOfMcEn == 0xFF){            // 1st time to update this variable
         TempNumOfMcEn = MemInfo->SncInfo[Soc].NumOfMcEnabled;
      } else if ( TempNumOfMcEn != MemInfo->SncInfo[Soc].NumOfMcEnabled) {  //if any sockets have different number of Mc then exit to disable SNC
         KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\nDifferent Mc numbers on existing sockets. Disable SNC!!"));
         return KTI_FAILURE;
      }
    }
  }

  // if both Mc have memory on all sockets, set to 2 clusters
  for (Soc = 0; Soc < MAX_SOCKET; Soc++) {
    if (host->var.common.socketPresentBitMap & (BIT0 << Soc)) {
      if (TempNumOfMcEn == 2){
        if (host->setup.mem.imcInter == IMC_1WAY){       // NUMA non-interleave, set SNC enabled, 2 clusters
          MemInfo->SncInfo[Soc].NumOfChaPerCluster = (host->var.kti.CpuInfo[Soc].TotCha)/2;
          MemInfo->SncInfo[Soc].NumOfCluster = 2;
        } else {                                         //NUMA & interleave , then set SNC enabled, 1 cluster
          for (Ch = 0; Ch < MAX_MC_CH; Ch++) {           //scan all channels, to validate channel map and size are equal on both Mc
             if (MemInfo->McInfo[Soc][0].ChannelMemSize[Ch] != MemInfo->McInfo[Soc][1].ChannelMemSize[Ch]){
               KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\n Interleave Enable -> Socket %d channel %d has different size between MC", Soc, Ch));
               return KTI_FAILURE;
             }
          }
          MemInfo->SncInfo[Soc].NumOfChaPerCluster = (host->var.kti.CpuInfo[Soc].TotCha);
          MemInfo->SncInfo[Soc].NumOfCluster = 1;
        }

      //if either Mc0 or Mc1 has memory only on all sockets, set to 1 cluster
      } else if (TempNumOfMcEn == 1){
          MemInfo->SncInfo[Soc].NumOfChaPerCluster = (host->var.kti.CpuInfo[Soc].TotCha);
          MemInfo->SncInfo[Soc].NumOfCluster = 1;
      }

      KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\nSocket %d Snc Enable = %d, Num of Cha Per Cluster = %d, Num of Cluster = %d",
                           Soc, host->var.kti.OutSncEn, MemInfo->SncInfo[Soc].NumOfChaPerCluster, MemInfo->SncInfo[Soc].NumOfCluster));

      //
      // 1-cluster : Enable SNC_IND if any Prefetch is enabled
      // 2-clusters: Enable SNC_FULL by the SNC_EN knob is enabled
      //
      if(MemInfo->SncInfo[Soc].NumOfCluster == 1){
        if (((host->setup.kti.KtiPrefetchEn == KTI_DISABLE) && (host->setup.kti.XptPrefetchEn == KTI_DISABLE)) || (host->var.kti.OutSncEn == KTI_ENABLE)){
          KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\nWARN: Cannot enable SNC_IND - all prefetch knobs are disabled!!!\n"));
          return KTI_FAILURE;
        }
      }else if(MemInfo->SncInfo[Soc].NumOfCluster == 2){
        if (host->var.kti.OutSncEn == KTI_DISABLE){
          return KTI_FAILURE;
        }
      }
    } // socket valid
  }  // scan socket

  return KTI_SUCCESS;
}

/**
  Get memory channel Information.

  This routine is to scan memory channel information and update into MemInfo

  @param  host                    Pointer to the system host (root) structure
  @param  MemInfo                 Memory Information structure.
  @param  Socket                  Socket

  @retval None

**/

VOID
GetMemoryChannelInfo (
  struct sysHost             *host,
  MEM_INFO                   *MemInfo,
  UINT8                      Socket
  )
{
   UINT8  Mc, Ch, ChannelNumber;
   UINT16 McDdr4MemSize, ChannelDdr4MemSize;
   UINT16 McAepMemSize, ChannelAepMemSize;
   struct ddrChannel (*channelList)[MAX_CH];

   //
   // The following code to scan each channel is for debug purpose
   //
   channelList = &host->var.mem.socket[Socket].channelList;

   for (Mc = 0; Mc < host->var.mem.maxIMC; Mc++){
      McDdr4MemSize = McAepMemSize = 0;
      for (Ch = 0; Ch < MAX_MC_CH; Ch++) {
        // Detects total memory installed on this channel

        ChannelDdr4MemSize = ChannelAepMemSize = 0;
        ChannelNumber = Ch + Mc *MAX_MC_CH;

        ChannelDdr4MemSize = (*channelList)[ChannelNumber].memSize;     //get ddr4 value
        ChannelAepMemSize = (*channelList)[ChannelNumber].volSize + (*channelList)[ChannelNumber].perSize;     //get 2LM size and per size

        MemInfo->McInfo[Socket][Mc].ChannelMemSize[Ch] = ChannelDdr4MemSize;
        KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\n  channel %d DDR4 Size = %dG volSize = %dG perSize = %dG", ChannelNumber, ChannelDdr4MemSize >>4,
                             (*channelList)[ChannelNumber].volSize >> 4,
                             (*channelList)[ChannelNumber].perSize >> 4));


        if (ChannelDdr4MemSize) {
          McDdr4MemSize +=  ChannelDdr4MemSize >> 4;   //in GB
        }

        if (ChannelAepMemSize) {
          McAepMemSize +=  ChannelAepMemSize >> 4;     //in GB
        }
      }  // Ch loop

      MemInfo->McInfo[Socket][Mc].McMemSize = ((McAepMemSize !=0 ) ? McAepMemSize: McDdr4MemSize);
      KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\n  Mc %d MemorySize = %dG", Mc, MemInfo->McInfo[Socket][Mc].McMemSize));
   }  // Mc loop
}

/**
  Get SAD Entries Information.

  This routine is to scan SAD entries information and update into MemInfo Base & Limit

  @param  host                    Pointer to the system host (root) structure
  @param  MemInfo                 Memory Information structure.
  @param  Socket                  Socket

  @retval None

**/
VOID
GetSadEntryInfo (
  struct sysHost             *host,
  MEM_INFO                   *MemInfo,
  UINT8                      Socket
  )
{
  UINT8  Mc, i, NumOfCluster;
  struct SADTable  *SADEntry;
  struct TADTable  *TADEntry;
  struct memVar    *mem;
  mem = &host->var.mem;

  if (Socket * 2 >= MC_MAX_NODE || Socket * 2 + 1 >= MC_MAX_NODE) {
    KtiDebugPrintFatal ((host, KTI_DEBUG_ERROR, "\nERR_SW_CHECK: 0x%X!!!!   \"UINT8 Socket\" with value: 0x%X, will cause overflow on \"mem->socket[Socket].SADintList[i][Socket *2]\"\n",
                         MINOR_ERR_SW_CHECK_53, Socket));
    KTI_ASSERT (FALSE, ERR_SW_CHECK, MINOR_ERR_SW_CHECK_53);
  }

  KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\nSocket %d Tolm %x", Socket, host->var.common.tolmLimit));

  // Initialize InterleaveEn & Mc to 0
  MemInfo->SncInfo[Socket].InterleaveEn = 0;

  //Scan TAD enties to get tadid between mmio base
  for (Mc = 0; Mc < host->var.mem.maxIMC; Mc++) {
    if (host->nvram.mem.socket[Socket].imc[Mc].enabled == 1) {
      KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\n Home Agent %d\n", Mc));
      for (i = 0; i < TAD_RULES; i++) {
        TADEntry = &mem->socket[Socket].imc[Mc].TAD[i];
        if (TADEntry->Enable == 1) {
          KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\n Tad Entry %d Limit 0x%x", i, TADEntry->Limit));
          if( (TADEntry->Limit & 0xFFFFFFF0) == (UINT32)(host->var.common.tolmLimit)){
            MemInfo->McInfo[Socket][Mc].FirstPfTadID = i;           //update tadid which less than mmio base
            MemInfo->McInfo[Socket][Mc].SecondPfTadID = i + 1;      //update tadid which greater than or equal mmio base
            KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\n Firsttadid %d 2ndTadid %d", i, i+1));
          }
        } // Tad rule eanbled
      } //scan Tad rules
    } // Mc enabled
  } //scan Mc

  //Scan SAD entries
  for (i = 0; i < MAX_SAD_RULES; i++) {
    SADEntry = &mem->socket[Socket].SAD[i];
    if (SADEntry->Enable == 0) {
      break; // end of SAD table
    }
    //
    // Display SADEntry information
    //
    KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\n  SADEntry %d local %d base 0x%05x limit 0x%05x SADintList_MC0 %d SADintList_MC1 %d MC0 channelInterBitmap 0x%08x MC1 channelInterBitmap 0x%08x MC0 FMchannelInterBitmap 0x%08x MC1 FMchannelInterBitmap 0x%08x type = ",
                         i, SADEntry->local, (i==0 || SADEntry->Limit == 0) ? 0 : mem->socket[Socket].SAD[i-1].Limit, SADEntry->Limit,
                         mem->socket[Socket].SADintList[i][Socket *2],
                         mem->socket[Socket].SADintList[i][Socket *2 + 1],
                         SADEntry->channelInterBitmap[0], SADEntry->channelInterBitmap[1], SADEntry->FMchannelInterBitmap[0], SADEntry->FMchannelInterBitmap[1]
    ));

    switch (SADEntry->type ) {
      case MEM_TYPE_1LM:
        KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "1LM"));
        break;
      case MEM_TYPE_2LM:
        KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "2LM"));
        break;
      case MEM_TYPE_PMEM:
        KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "PMEM"));
        break;
      case MEM_TYPE_PMEM_CACHE:
        KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "PMEM_CACHE"));
        break;
      case MEM_TYPE_BLK_WINDOW:
        KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "BLK_WINDOW"));
        break;
      case MEM_TYPE_CTRL:
        KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "BLK_CTRL"));
        break;
    }

    if (SADEntry->type == MEM_TYPE_1LM){           //if 1LM, display granularity becasue xpt prefetch can be enabled if 1LM only.
      KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, " Granularity = "));
      switch (SADEntry->granularity ) {
        case MEM_INT_GRAN_1LM_256B_256B:
          KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "256B"));
          break;
        case MEM_INT_GRAN_1LM_64B_64B:
          KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "64B"));
          break;
        default:
          KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, ""));
          break;
      }
    }

    if (SADEntry->local){

        // check this SAD entry is 1LM, 2LM or PMEM Cache
        if ((SADEntry->type == MEM_TYPE_1LM ) || ( SADEntry->type == MEM_TYPE_2LM ) || ( SADEntry->type == MEM_TYPE_PMEM_CACHE ) ) {

          //
          // Determin which mc infomation will be updated for base and limit
          //
          // if this entry for MC0 only
          if (mem->socket[Socket].SADintList[i][Socket *2] == 1 &&  mem->socket[Socket].SADintList[i][Socket *2 + 1] == 0){
            Mc = 0;
          // if this entry for MC1 only
          } else if (mem->socket[Socket].SADintList[i][Socket *2] == 0 && mem->socket[Socket].SADintList[i][Socket *2 + 1] == 1){
            Mc = 1;
          // this entry is MC0/MC1 interleave
          } else {
            MemInfo->SncInfo[Socket].InterleaveEn = 1;
            Mc = 0;
          }

          // update channel interleave bit map
          if (SADEntry->type == MEM_TYPE_1LM ){
            if (SADEntry->granularity == MEM_INT_GRAN_1LM_256B_256B){
              MemInfo->SncInfo[Socket].Interleave_256B = 1;
            }

            // check if this 1st SAD engry for this Mc
            if (MemInfo->McInfo[Socket][Mc].ChannelInterBitmap == 0){
              MemInfo->SncInfo[Socket].PrefetchEn = 1;               //if 1LM, PrefetchEn can be enabled. Otherwise, need to be disabled
              MemInfo->McInfo[Socket][Mc].ChannelInterBitmap = SADEntry->channelInterBitmap[Mc];
            } else if (MemInfo->McInfo[Socket][Mc].ChannelInterBitmap != SADEntry->channelInterBitmap[Mc]) {
              MemInfo->SncInfo[Socket].PrefetchEn = 0;              // that means multiple SAD entries have different channel interleave bit mapping, and disable xpt prefetch
                                                                    //      because M2M doesn't allow different channel bit map within one Mc
            }
          }

          if (MemInfo->McInfo[Socket][Mc].Enabled == 0){      //this is 1st time
              MemInfo->McInfo[Socket][Mc].MemBase = (i==0 || SADEntry->Limit == 0) ? 0 : mem->socket[Socket].SAD[i-1].Limit;
              MemInfo->McInfo[Socket][Mc].Enabled = 1;
              // check if finds dimm 1st time, if yes, then update socket info
              if (MemInfo->SocketIdWithFirst4G == 0 ) {
                  MemInfo->SocketIdWithFirst4G = (UINT8)((1 << 7) | Socket);   //bit 7 is to indicate the valid bit
              }
          }
          MemInfo->McInfo[Socket][Mc].MemLimit = SADEntry->Limit;
        }

    }   //SAD entry local
  }  // SAD entry loop

  //
  // display each mc base & limit
  //
  for (Mc = 0; Mc < host->var.mem.maxIMC; Mc++){
     if (MemInfo->McInfo[Socket][Mc].Enabled == 1){
        KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\n Mc %d Interleave %d PrefetchEn %d Membase 0x%05x MemLimit 0x%05x",
                             Mc, MemInfo->SncInfo[Socket].InterleaveEn, MemInfo->SncInfo[Socket].PrefetchEn,
                             MemInfo->McInfo[Socket][Mc].MemBase, MemInfo->McInfo[Socket][Mc].MemLimit));
     }
  }

  if (MemInfo->SncInfo[Socket].InterleaveEn == 1){                  // if interleave, then num of cluster is 1
     NumOfCluster = 1;
  } else {
    if (MemInfo->McInfo[Socket][0].Enabled == 1 && MemInfo->McInfo[Socket][1].Enabled == 1){    //both Mc have memory and non-interleave, set to 2 clusters
      NumOfCluster = 2;
    } else if (MemInfo->McInfo[Socket][0].Enabled == 1 ){           //if Mc0 has memory only, set to 1 cluster
      NumOfCluster = 1;
    } else if (MemInfo->McInfo[Socket][1].Enabled == 1 ){           //if Mc1 has memory only, set to 1 cluster
      NumOfCluster = 1;
    } else {                                                        //if both Mc have no memory, set to 0 cluster
      NumOfCluster = 0;
    }
  }

  MemInfo->SncInfo[Socket].NumOfCluster = NumOfCluster;

  if (NumOfCluster == 1){
    MemInfo->SncInfo[Socket].NumOfChaPerCluster = (host->var.kti.CpuInfo[Socket].TotCha);
  } else if (NumOfCluster == 2){
    MemInfo->SncInfo[Socket].NumOfChaPerCluster = (host->var.kti.CpuInfo[Socket].TotCha)/2;
  }

  KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\n  Number of clusters = %d", MemInfo->SncInfo[Socket].NumOfCluster));
}

const   UINT16 rankSizeDDR4_1[MAX_TECH] = {  64,  32,  16, 128,  64,  32, 256, 128,  64, 512, 256, 128};   // Rank size in 64 MB units
const   UINT16 rankSizeAEP_1[MAX_TECH_AEP] = {128, 256,  512, 1024, 2048,4096,8192,16384}; // Rank Size in 64 MB Units

/**
  Read Channel Dimm Information

  This routine read nvram dimm info for this channel

  @param  host                    Pointer to the system host (root) structure
  @param  Socket                  Socket
  @param  Mc                      Memory controller
  @param  Ch                      Channel
  @param  ChannelMemSize          Pointer to ChannelMemSize
  @param  AepDimmPresent          Pointer to AepDimmPresent

  @retval None

**/
VOID
ReadChannelDimmInfo (
  struct sysHost             *host,
  UINT8  Socket,
  UINT8  Mc,
  UINT8  Ch,
  UINT16 *ChannelMemSize,
  UINT8  *AepDimmPresent
  )
{
  UINT8 Dimm, Rank, TechIndex, ChannelNumber;
  UINT16 DimmSize, RankSize;
  struct memNvram  *nvramMem;
  struct dimmNvram (*dimmNvList)[MAX_DIMM];
  struct channelNvram (*channelNvList)[MAX_CH];

  nvramMem = &host->nvram.mem;
  channelNvList = &nvramMem->socket[Socket].channelList;
  ChannelNumber = Ch + Mc *MAX_MC_CH;

  dimmNvList = &nvramMem->socket[Socket].channelList[ChannelNumber].dimmList;
  for (Dimm = 0; Dimm < (*channelNvList)[ChannelNumber].maxDimm; Dimm++) {
     DimmSize = 0;
     if ((*dimmNvList)[Dimm].dimmPresent) {

        KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\n Socket %d Mc %d Ch %d maxDimm %d Dimm = %d numDramRanks= %d ",
                            Socket,
                            Mc,
                            Ch,
                            (*channelNvList)[ChannelNumber].maxDimm,
                            Dimm,
                            (*channelNvList)[ChannelNumber].dimmList[Dimm].numDramRanks
                            ));

        //get dimm size for this dimm
        if ((*dimmNvList)[Dimm].aepDimmPresent){
            *AepDimmPresent = TRUE;
            KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "Aep TechIndex = "));
            TechIndex = (*dimmNvList)[Dimm].aepTechIndex;
            RankSize = rankSizeAEP_1[TechIndex];
        } else {
            KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "    TechIndex = "));
            TechIndex = (*dimmNvList)[Dimm].techIndex;
            RankSize = rankSizeDDR4_1[TechIndex];
        }
        KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "%d", TechIndex));

        for (Rank = 0; Rank < (*dimmNvList)[Dimm].numDramRanks; Rank++) {
           DimmSize += RankSize;
        }  // Rank Loop

        KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, " DimmSize 0x%x", DimmSize));
        // update mem size for this channel
        *ChannelMemSize += DimmSize;
     }
  }  // dimm loop

}

/**
  Check DDR Memory Population.

  This routine calculates number of channels populated on each memory controller, channel
  memory size and indicate if all channels are of same size.

  @param  host                    Pointer to the system host (root) structure
  @param  MemInfo                 Memory Information structure.

  @retval None

**/
KTI_STATUS
CheckDdrMemoryPopulation (
  struct sysHost             *host,
  MEM_INFO                   *MemInfo
  )
{
  UINT8  Socket, Mc, Ch, ChannelNumber, AepDimmPresent;
  struct memNvram  *nvramMem;
  struct channelNvram (*channelNvList)[MAX_CH];
  UINT16 ChannelMemSize;
  KTI_STATUS Status;

  nvramMem = &host->nvram.mem;

  //get memory infomation from nvram dimm for each socket
  if (host->var.common.CurrentReset == POST_RESET_POWERGOOD){    // Power good path
    for (Socket = 0; Socket < MAX_SOCKET; Socket++){
      if ((host->var.common.socketPresentBitMap & (BIT0 << Socket)) != 0){
        channelNvList = &nvramMem->socket[Socket].channelList;
        AepDimmPresent = 0;

        for (Mc = 0; Mc < host->var.mem.maxIMC; Mc++){
          for (Ch = 0; Ch < MAX_MC_CH; Ch++) {
            // Detects which channel have memory and update MemInfo for furture use
            ChannelMemSize = 0;
            ChannelNumber = Ch + Mc *MAX_MC_CH;
            KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\nSocket %d channel %d enabled %d", Socket, ChannelNumber, nvramMem->socket[Socket].channelList[ChannelNumber].enabled ));
            if ((*channelNvList)[ChannelNumber].enabled) {
              ReadChannelDimmInfo(host, Socket, Mc, Ch, &ChannelMemSize, &AepDimmPresent);
              MemInfo->McInfo[Socket][Mc].ChannelMemSize[Ch] = ChannelMemSize >> 4;
              KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\n  Memory Size = 0x%x", ChannelMemSize));

              MemInfo->McInfo[Socket][Mc].Enabled = 1;
              if (MemInfo->SocketIdWithFirst4G == 0) {
                MemInfo->SocketIdWithFirst4G = (UINT8)((1 << 7) | Socket);   //bit 7 is to indicate the valid bit
                KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\nSocket %d has 1st 4G memory on Channel %d", Socket, ChannelNumber));
              }
            } //Ch enable?
          } // Ch
        } // Mc

        //update number of Mc enabled
        if (MemInfo->McInfo[Socket][0].Enabled == 1 && MemInfo->McInfo[Socket][1].Enabled == 1){
          MemInfo->SncInfo[Socket].NumOfMcEnabled = 2;
        } else if (MemInfo->McInfo[Socket][0].Enabled == 1 || MemInfo->McInfo[Socket][1].Enabled == 1){
          MemInfo->SncInfo[Socket].NumOfMcEnabled = 1;
        }

      } // Socket valid?
    } // Socket

    Status = DetermineSncSetting(host, MemInfo);
    return Status;

  } else {      // warm reset path
    for (Socket = 0; Socket < MAX_SOCKET; Socket++){
      if ((host->var.common.socketPresentBitMap & (BIT0 << Socket)) != 0){
         GetSadEntryInfo(host, MemInfo, Socket);
         GetMemoryChannelInfo(host, MemInfo, Socket);        //Debug purpose only
      }
    }

    Status = KTI_SUCCESS;
    return Status;
  }  // warm reset path

}

/**
  Verify any memory topology change after warm reset

  This routine is to verify any memory topology change after warm reset.

  @param  host                    Pointer to the system host (root) structure
  @param  MemInfo                 Memory Information structure
  @param  Socket                  Socket

  @retval KTI_STATUS              KTI_SUCCESS if no changes

**/
KTI_STATUS
VerifyConfigChangedAfterWarmReset (
  struct sysHost             *host,
  MEM_INFO                   *MemInfo,
  UINT8                      Socket
  )
{
  BOOLEAN                        IOwn4GInColdReset;
  UINT8                          Soc, Ch, TempNumOfCluster;
  SNC_BASE_1_CHABC_PMA_STRUCT    SncBase;

  //
  // Step 1: check the socket owns 0-4G in cold reset flow still valid in warm reset
  //        if any of SNC_BASE registers are different, then the socket owned 0-4G in cold reset path
  //
  IOwn4GInColdReset = FALSE;
  SncBase.Data = KtiReadPciCfg (host, Socket, 0, SNC_BASE_1_CHABC_PMA_REG);

  if (SncBase.Bits.starting_addr_0 == 0){
    IOwn4GInColdReset = TRUE;
  }

  // Socket owns 4G in cold reset but Socket doesn't own 4G in warm reset, then exit with failure
  if (IOwn4GInColdReset && (MemInfo->SocketIdWithFirst4G & 0x7F) != Socket){
    KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\nSocket owns 4G is changed in warm reset"));
    return KTI_FAILURE;
  }

  //
  // Step 2: check if # of cluster still remain the same as cold reset flow
  //         OutNumOfCluster is updated by reading SNC_CONFIG_CHABC_PMA_REG
  //
  if (MemInfo->SncInfo[Socket].NumOfCluster != host->var.kti.OutNumOfCluster[Socket]){
    KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\n number of cluster is changed on Socket %d", Socket));

    TempNumOfCluster = 0xFF;
    for (Soc = 0; Soc < MAX_SOCKET; Soc++) {
      if (host->var.common.socketPresentBitMap & (BIT0 << Soc)) {
        if ( TempNumOfCluster == 0xFF){            // 1st time to update this variable
          TempNumOfCluster = MemInfo->SncInfo[Soc].NumOfCluster;
        } else if ( TempNumOfCluster != MemInfo->SncInfo[Soc].NumOfCluster) {  //if any sockets have different number of Mc then exit to disable SNC
          KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\nDifferent Cluster numbers on existing sockets. Disable SNC!!"));
          return KTI_FAILURE;
        }
      }
    }
    return KTI_SNC_CHANGED;      //number of clusers changes on all existing sockets, all sockets have the same number of clustsrs
  }

  //
  // Step 3: if both MC enabled & NUMA & 2 WAY interleave cross both MC
  //         check if channel mapping and size are the same on both MC
  //
  if (MemInfo->SncInfo[Socket].InterleaveEn == 1){
    if (host->setup.mem.options & NUMA_AWARE){              //Check if Numa enabled
      if (host->setup.mem.imcInter == IMC_2WAY){            //interleave cross both MC
         for (Ch = 0; Ch < MAX_MC_CH; Ch++) {              //scan all channels
           if (host->var.mem.socket[Socket].channelList[Ch].memSize != host->var.mem.socket[Socket].channelList[Ch + MAX_MC_CH].memSize){
             KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\n Interleave Enable -> Socket %d channel %d has different size between MC", Socket, Ch));
             return KTI_FAILURE;
           }
         }
      }
    }
  }

  return KTI_SUCCESS;
}

/**
  API callable from outside UncoreRC to calculate  Clustering (SNC) base

  @param  host                    Pointer to the system host (root) structure.
  @param  MemInfo                 Memory Information structure.

  @retval UNCORE_SUCCESS          SNC mode is programmed successfully; or system is not configured for SNC mode operation.
  @retval UNCORE_FAILURE          Memory profile is not suitable for SNC mode operation.

**/
KTI_STATUS
CalculateClusterBase (
  struct sysHost             *host,
  MEM_INFO                   *MemInfo
  )
{
  UINT8           Socket, Mc, Cluster;
  KTI_STATUS      Status;

  //
  // These conditions must be met to enable the SNC mode.
  //     - Each cluster gets multiple of 1GB DDR4 memory, if DDR4 memory is allocated
  //

  for (Socket = 0; Socket < MAX_SOCKET; Socket++){

    //check if socket is valid and snc enabled
    if ((host->var.common.socketPresentBitMap & (BIT0 << Socket)) != 0)  {

      if (host->var.kti.OutSncPrefetchEn == 0) continue;
      KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\nSocket %d", Socket));
      if (host->var.common.CurrentReset == POST_RESET_POWERGOOD) {    // Power good
         for (Cluster = 0; Cluster <= MAX_CLUSTERS; Cluster++) {
           //  If powergood, find which socket & which mc owns 0-4G range
           if ((MemInfo->SocketIdWithFirst4G & 0x7F) == Socket && Cluster == 0){
             MemInfo->ClusterMemBase[Socket][Cluster] = 0;                    //Cluster 0 starts from 0G
           } else {
             MemInfo->ClusterMemBase[Socket][Cluster] = FOUR_GB_MEM >> 4;     //in GB
           }
           KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\n  SNC_Base_%d = %04d GB", Cluster+1, MemInfo->ClusterMemBase[Socket][Cluster]));
         }

      } else { // reset type is warm reset
        Status = VerifyConfigChangedAfterWarmReset(host, MemInfo, Socket);
        // if KTI_FAILURE
        //    a.  socket owns 0-4G have been changed
        //    b.  sockets have been changed with different number of clusters,
        //    c.  sockets have different channel mapping and size if both MC enabled & NUMA & 2 WAY interleave cross both MC,
        //    then, SNC need to be disable
        // else if KTI_SNC_CHANGED, number of clusters have been changed, but all sockets have the same number of clusters
        if (Status != KTI_SUCCESS){
          return  Status;
        }

        //update cluster memory base
        Cluster = 0;
        for (Mc = 0; Mc < host->var.mem.maxIMC; Mc++){
          if (MemInfo->McInfo[Socket][Mc].Enabled == 1){
            if(Cluster == 0) {

              MemInfo->ClusterMemBase[Socket][Cluster] = MemInfo->McInfo[Socket][Mc].MemBase >> 4;
              if(MemInfo->McInfo[Socket][Mc].MemBase % 16) {
                MemInfo->ClusterMemBase[Socket][Cluster] += 1;
              }
              Cluster++;
            }

            if((MemInfo->McInfo[Socket][Mc].MemLimit & 0x0F) != 0 ){          //check if not in 1G multiple beacause SNC base register is in 1G multiples
              KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\n  Mc %d Limit = %03d NOT in GB alignment", Socket, Mc, MemInfo->McInfo[Socket][Mc].MemLimit));
              return  KTI_FAILURE;
            }
            MemInfo->ClusterMemBase[Socket][Cluster] = MemInfo->McInfo[Socket][Mc].MemLimit >> 4;
            if(MemInfo->McInfo[Socket][Mc].MemLimit % 16) {
               MemInfo->ClusterMemBase[Socket][Cluster] += 1;
            }
            Cluster++;
          }
        }

        for (Cluster = 0; Cluster <= MAX_CLUSTERS; Cluster++) {
          //update non-used cluster base to the last cluster base, even this cluster doesn't exist
          if( Cluster != 0 && MemInfo->ClusterMemBase[Socket][Cluster] == 0){
             MemInfo->ClusterMemBase[Socket][Cluster] = MemInfo->ClusterMemBase[Socket][Cluster -1];
          }
          KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\n  SNC_Base_%d = %04d GB", Cluster+1, MemInfo->ClusterMemBase[Socket][Cluster]));
        }

      } // reset type
    } // socket valid & snc enable
  } // scan socket

  return KTI_SUCCESS;
}

/**
  API callable from outside UncoreRC to program Sub Numa Clustering (SNC) mode.

  @param  host                    Pointer to the system host (root) structure.
  @param  MemInfo                 Memory Information structure.
  @param  Socket                  Socket
  @param  Operation               Operation type: Normal or Recovery operation

  @retval UNCORE_SUCCESS          SNC mode is programmed successfully; or system is not configured for SNC mode operation.
  @retval UNCORE_FAILURE          Memory profile is not suitable for SNC mode operation.

**/
KTI_STATUS
ProgramSncColdResetRegister (
  struct sysHost             *host,
  MEM_INFO                   *MemInfo,
  UINT8                      Socket,
  UINT8                      Operation
  )
{
  UINT8                           Ctr, Cha, NumOfCluster;
  SNC_CONFIG_CHABC_PMA_STRUCT     SncConfigPma;
  CHA_SNC_CONFIG_CHABC_SAD_STRUCT SncConfigSad;
  HA_AD_CREDITS_CHA_MISC_STRUCT   HaAdCredit;
  HA_BL_CREDITS_CHA_MISC_STRUCT   HaBlCredit;
  UINT32                          Data32_AD;
  UINT32                          Data32_BL;

  //
  // Calculate the SNC Base Size for each cluster. Note there is no holes allowed between the clusters except for the MMIOL hole.
  //
  if (Operation == NORMAL_OPERATION) {    // SNC normal cold reset setting in cold reset flow

      NumOfCluster = MemInfo->SncInfo[Socket].NumOfCluster;
      //
      // Program SNC registers. In SNC2 case each cluster has 19 CHAs. In SNC4 case, SNC 0&2 have 10 CHAs; SNC 1&3 have 9 CHAs.
      //
      SncConfigSad.Data = SncConfigPma.Data = 0;
      if (NumOfCluster == 2) {            // 2'b00 2 Cluster, 2'b01 3 Clusters, 2'b10 4 Clusters, 2'b11 RSVD
        SncConfigSad.Bits.full_snc_en = SncConfigPma.Bits.full_snc_en = 1;
        SncConfigSad.Bits.num_snc_clu = SncConfigPma.Bits.num_snc_clu = 0;
      }
      SncConfigSad.Bits.snc_ind_en = SncConfigPma.Bits.snc_ind_en = 1;
      SncConfigPma.Bits.lowest_cbo_id_clus1 = MemInfo->SncInfo[Socket].NumOfChaPerCluster;
      SncConfigPma.Bits.lowest_cbo_id_clus2 = SncConfigPma.Bits.lowest_cbo_id_clus3 = host->var.kti.CpuInfo[Socket].TotCha;
      SncConfigSad.Bits.sncmidpointid = MemInfo->SncInfo[Socket].NumOfChaPerCluster - 1 ;
      KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\nSocket %d SNC%d mode requested SNC_CONFIG = 0x%08x (num_snc_clu = %02d clus1= %02d clus2= %02d clus3= %02d\n",
                                                Socket, NumOfCluster, SncConfigPma.Data,
                                                SncConfigPma.Bits.num_snc_clu, SncConfigPma.Bits.lowest_cbo_id_clus1,
                                                SncConfigPma.Bits.lowest_cbo_id_clus2, SncConfigPma.Bits.lowest_cbo_id_clus3));

      // Re-adjust and program CHA->M2M credit based on NumOfChaPerCluster and SNC configuration
      ReCalAndProgramChaToM2mCreditWhenSncEn(host, MemInfo, Socket);

  } else {                 // RECOVERY_OPERATION,  SNC failure detection during warm reset
      SncConfigSad.Data = SncConfigPma.Data = 0;
      // Recovry back to snc disabled credit for CHA->M2M credit
      CalculateChaToM2mCredit(host, Socket, host->var.kti.CpuInfo[Socket].TotCha, &Data32_AD, &Data32_BL);

      for (Cha = 0; Cha < host->var.kti.CpuInfo[Socket].TotCha; Cha++) {
        HaAdCredit.Data = KtiReadPciCfg (host, Socket, Cha, HA_AD_CREDITS_CHA_MISC_REG);
        HaAdCredit.Data &= 0xFFFFFF00;
        HaAdCredit.Data |= Data32_AD;
        KtiWritePciCfg (host, Socket, Cha, HA_AD_CREDITS_CHA_MISC_REG, HaAdCredit.Data);

        HaBlCredit.Data = KtiReadPciCfg (host, Socket, Cha, HA_BL_CREDITS_CHA_MISC_REG);
        HaBlCredit.Data &= 0xFFFFFF00;
        HaBlCredit.Data |= Data32_BL;
        KtiWritePciCfg (host, Socket, Cha, HA_BL_CREDITS_CHA_MISC_REG, HaBlCredit.Data);
      }
  }

  KtiWritePciCfg (host, Socket, 0, SNC_CONFIG_CHABC_PMA_REG, SncConfigPma.Data);

  // Program SNC config register in IIO PSTACK 0/1/2/3/4 & Kti Agents
  for (Ctr = 0; Ctr < host->var.common.KtiPortCnt; Ctr++) {
    if (host->var.kti.CpuInfo[Socket].LepInfo[Ctr].Valid == TRUE){
      KtiWritePciCfg (host, Socket, Ctr, KTILK_SNC_CONFIG_KTI_LLPMON_REG, SncConfigPma.Data);
    }
  }

  for (Ctr = 1; Ctr < MAX_IIO_STACK; Ctr++) {
    if (host->var.common.stackPresentBitmap[Socket] & (1 << Ctr)) {
      KtiWritePciCfg (host, Socket, Ctr, SNC_CONFIG_IIO_VTD_REG, SncConfigPma.Data);
    }
  }

  // Program SNC config and base in CHABC
  KtiWritePciCfg (host, Socket, 0, CHA_SNC_CONFIG_CHABC_SAD_REG, SncConfigSad.Data);

  return KTI_SUCCESS;
}


/**
  API callable from outside UncoreRC to program Sub Numa Clustering (SNC) mode.

  @param  host                    Pointer to the system host (root) structure.
  @param  MemInfo                 Memory Information structure.

  @retval UNCORE_SUCCESS          SNC mode is programmed successfully; or system is not configured for SNC mode operation.
  @retval UNCORE_FAILURE          Memory profile is not suitable for SNC mode operation.

**/
KTI_STATUS
ConfigureSncMode (
  struct sysHost             *host,
  MEM_INFO                   *MemInfo,
  KTI_STATUS                 Status
  )
{
  UINT8                            Socket, Ctr, Cluster, PrefetchEn;
  SNC_BASE_1_CHABC_PMA_STRUCT      SncBase;
  TOPOLOGY_M2MEM_MAIN_STRUCT       TopologyM2Mem;
  PREFETCHTUNING_M2MEM_MAIN_STRUCT PrefetchTuningM2Mem;

  PrefetchEn = 0xFF;
  //
  // Calculate the SNC Base Size for each cluster. Note there is no holes allowed between the clusters except for the MMIOL hole.
  //
  KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\n"));

  //
  // Prefetch should be enabled or disabled for all sockets.
  //
  MemInfo->PrefetchAll = 1;
  for (Socket = 0; Socket < MAX_SOCKET; Socket++){
    if ((host->var.common.socketPresentBitMap & (BIT0 << Socket)) != 0){
      if (PrefetchEn == 0xFF){
        PrefetchEn = MemInfo->SncInfo[Socket].PrefetchEn;
      } else {
        if (PrefetchEn != MemInfo->SncInfo[Socket].PrefetchEn){
          MemInfo->PrefetchAll = 0;
          break;
        }
      }
    }
  }

  for(Socket = 0; Socket < MAX_SOCKET; Socket++) {
    // check if socket is valid and snc enabled
    if  ((host->var.common.socketPresentBitMap & (BIT0 << Socket)) != 0)  {
      if (host->var.kti.OutSncPrefetchEn == 0) {
        continue;
      }
      if (host->var.common.CurrentReset == POST_RESET_POWERGOOD ) {    // power good reset
        ProgramSncColdResetRegister(host, MemInfo, Socket, NORMAL_OPERATION);
      } else {                                                         // warm reset path

        if(host->var.kti.OutSncPrefetchEn && (Status == KTI_SUCCESS)) {         // normal warm reset path, i.e. no snc failure detection during warm reset
          //
          // Program SNC base register in IIO PSTACK 0/1/2/3/4  & KTI Agents
          //
          for (Cluster = 0; Cluster <= MAX_CLUSTERS; Cluster++) {
            for (Ctr = 0; Ctr < host->var.common.KtiPortCnt; Ctr++) {
              if (host->var.kti.CpuInfo[Socket].LepInfo[Ctr].Valid == TRUE){
                SncBase.Data = KtiReadPciCfg (host, Socket, Ctr, KtiSncBaseRegisterOffset[Cluster]);
                SncBase.Bits.starting_addr_0 = MemInfo->ClusterMemBase[Socket][Cluster];
                KtiWritePciCfg (host, Socket, Ctr, KtiSncBaseRegisterOffset[Cluster], SncBase.Data);
              }
            } // KTI port loop

            for (Ctr = 1; Ctr < MAX_IIO_STACK; Ctr++) {
              if (host->var.common.stackPresentBitmap[Socket] & (1 << Ctr)) {
                SncBase.Data = KtiReadPciCfg (host, Socket, Ctr, IioSncBaseRegisterOffset[Cluster]);
                SncBase.Bits.starting_addr_0 = MemInfo->ClusterMemBase[Socket][Cluster];
                KtiWritePciCfg (host, Socket, Ctr, IioSncBaseRegisterOffset[Cluster], SncBase.Data);
              }
            } // iio stack loop
          } // Cluster loop

          //
          // Need to program SNC setting in MEMEM here in warm reset flow because these fields are not sticky
          //
          if ((host->var.common.cpuType == CPU_SKX) && (host->var.common.stepping >= B0_REV_SKX)) {//5331728
            for (Ctr = 0; Ctr < host->var.mem.maxIMC; Ctr++) {
              TopologyM2Mem.Data = KtiReadPciCfg (host, Socket, Ctr, TOPOLOGY_M2MEM_MAIN_REG);
              if (MemInfo->SncInfo[Socket].NumOfCluster == 1){
                TopologyM2Mem.Bits.sncluster = 0;
              } else {
                TopologyM2Mem.Bits.sncluster = Ctr;
              }
              TopologyM2Mem.Bits.basecluster1 = MemInfo->SncInfo[Socket].NumOfChaPerCluster;
              KtiWritePciCfg (host, Socket, Ctr, TOPOLOGY_M2MEM_MAIN_REG, TopologyM2Mem.Data);
            }
          }

          for (Ctr = 0; Ctr < host->var.mem.maxIMC; Ctr++) {
            //
            //WA: 5331330
            //PrefectM2Mem.Bits.prefrpqthreshold = 0xa;
            //PrefectM2Mem.Bits.prefwpqthreshold = 0xc;
            //PrefectM2Mem.Bits.prefkticamthreshold = 0x20;
            //PrefectM2Mem.Bits.prefxptcamthreshold = 0x20;
            //PrefectM2Mem.Bits.pftuningen = 1;
            //PrefectM2Mem.Bits.cfgdirupdpfcamevictdisable = 0;
            //
            PrefetchTuningM2Mem.Data = KtiReadPciCfg (host, Socket, Ctr, PREFETCHTUNING_M2MEM_MAIN_REG);
            PrefetchTuningM2Mem.Data = (PrefetchTuningM2Mem.Data & 0xFC000000) | 0x0182030A;;
            KtiWritePciCfg (host, Socket, Ctr, PREFETCHTUNING_M2MEM_MAIN_REG, PrefetchTuningM2Mem.Data);
          }

          //
          // Program Xpt Prefetch setting
          //
          if ((host->setup.kti.XptPrefetchEn != KTI_DISABLE) || (host->setup.kti.KtiPrefetchEn != KTI_DISABLE)) {
            ProgramXptKtiPrefetch(host, Socket, MemInfo);
          }
        } else if (host->var.kti.OutSncPrefetchEn && (Status == KTI_SNC_CHANGED)) {     // warm reset path, but number of cluster changed & all sockets have the same number of cluters
          ProgramSncColdResetRegister(host, MemInfo, Socket, NORMAL_OPERATION);
        } else {                                                         // recovery to snc disabled  i.e.  snc failure detection during warm reset
          ProgramSncColdResetRegister(host, MemInfo, Socket, RECOVERY_OPERATION);
        }
      }  // warm reset

      if (host->var.kti.OutSncPrefetchEn && (Status != KTI_SNC_CHANGED)) {              // if cold reset, warm reset with KTI_SUCCESS & KTI_FAILURE, need to program snc_base
        // Program SNC base register in CHABC (need to program during cold reset/warm reset flow
        for (Cluster = 0; Cluster <= MAX_CLUSTERS; Cluster++) {
          SncBase.Data = KtiReadPciCfg (host, Socket, 0, ChaSncBaseRegisterOffset[Cluster]);
          SncBase.Bits.starting_addr_0 = MemInfo->ClusterMemBase[Socket][Cluster];
          KtiWritePciCfg (host, Socket, 0, ChaSncBaseRegisterOffset[Cluster], SncBase.Data);
        }
      }

    } // socket valid and snc enabled
  } // socket loop


  return KTI_SUCCESS;
}

/**
  Enable Kti Prefeth when UMA enabled.

  @param  host                    Pointer to the system host (root) structure.
  @param  MemInfo                 Memory Information structure.
**/

KTI_STATUS
EnableUmaKtiPrefetch (
  struct sysHost             *host,
  MEM_INFO                   *MemInfo
  )
{
  //KTI_STATUS                          Status;
  UINT8                                 Socket, Mc, Ctr, Cluster;
  UINT8                                 InterleaveEn;
  UINT8                                 PrefetchEn;
  KTILK_SNC_CONFIG_KTI_LLPMON_STRUCT    SncConfig;
  KTILK_SNC_BASE_1_KTI_LLPMON_STRUCT    SncBase;
  UINT32                                MemBase;
  UINT32                                MemLimit, MemLimit_1, MiniSadTable, Minisadmodeselect;
  BOOLEAN                               KtiPrefetchSupport = TRUE;
  TOPOLOGY_M2MEM_MAIN_STRUCT            TopologyM2Mem;
  UINT8                                 Option;
  SYSFEATURES0_M2MEM_MAIN_STRUCT        SysFeatures0;

  //Initialize InterleaveEn to 0xff to indicate the initial value
  InterleaveEn = 0xFF;
  PrefetchEn = 0;
  MemBase = 0;
  MemLimit = 0;

  //Step 1: Scan all SAD entries on all sockets and update the MemInfo per MC
  //        (e.g InterleaveEn (within socket), PrefetchEn (from channel bit mapping),
  //
  for (Socket = 0; Socket < MAX_SOCKET; Socket++){
    if ((host->var.common.socketPresentBitMap & (BIT0 << Socket)) != 0){
      GetSadEntryInfo(host, MemInfo, Socket);
      GetMemoryChannelInfo(host, MemInfo, Socket);        //Debug purpose only
      if (MemInfo->SncInfo[Socket].PrefetchEn == 1){
        PrefetchEn |= (BIT0 << Socket);
      }

    }
  }

  KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\nPrefetchEn = %x\n", PrefetchEn));

  //Step 2: Check if kti prefetch disabled in setup or Mirror
  //
  //       For example                          Index    P7  Snc P6  |   Mc
  //       0xXXXXXX10 (SKT0)  "Cache0 (64B)       0       0   0   0  |   0 (SKT0)
  //                           C    1             1       0   0   1  |   1 (SKT0)
  //       0xXX10XXXX (SKT1)   C    2             4       1   0   0  |   0 (SKT1)
  //
  if ((host->setup.kti.KtiPrefetchEn == KTI_DISABLE) || (PrefetchEn == 0) || (host->nvram.mem.RASmode & FULL_MIRROR_1LM) || (host->nvram.mem.RASmode & PARTIAL_MIRROR_1LM)){
    KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\nKTI Prefetch Disabled in UMA"));
    // M2M
    for (Socket = 0; Socket < MAX_SOCKET; Socket++){
      if ((host->var.common.socketPresentBitMap & (BIT0 << Socket)) != 0){
        for (Ctr = 0; Ctr < host->var.mem.maxIMC; Ctr++) {
          if (host->nvram.mem.socket[Socket].imc[Ctr].enabled == 1){
            SysFeatures0.Data = KtiReadPciCfg(host, Socket, Ctr, SYSFEATURES0_M2MEM_MAIN_REG);
            SysFeatures0.Bits.prefdisable = 1;
            KtiWritePciCfg (host, Socket, Ctr, SYSFEATURES0_M2MEM_MAIN_REG, SysFeatures0.Data);
          }
        }
      }
    }
    return KTI_FAILURE;
  }

  KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\nKti Prefetch enabling with UMA"));

  PrefetchEn = 0;

  //Step 3: make sure MemInfo per Mc are identical across all Mc and sockets
  //        If yes, then KtiPrefetch can be enabled for UMA/2S topology
  //        If not, then skip KtiPrefetch enabling and return.
  for (Socket = 0; Socket < MAX_SOCKET; Socket++){
    if ((host->var.common.socketPresentBitMap & (BIT0 << Socket)) != 0){
       for (Mc = 0; Mc < host->var.mem.maxIMC; Mc++){
         if (MemInfo->McInfo[Socket][Mc].Enabled == 1){
            if (InterleaveEn == 0xFF){              //check if 1st time to update, if yes, then save the first Mc info for later checking
               InterleaveEn = MemInfo->SncInfo[Socket].InterleaveEn;
               PrefetchEn = MemInfo->SncInfo[Socket].PrefetchEn;
               MemBase = MemInfo->McInfo[Socket][Mc].MemBase;
               MemLimit = MemInfo->McInfo[Socket][Mc].MemLimit;
               KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\nSocket %d Mc %x InterleaveEn %x PrefetchEn %x MemBase %x MemLimit %x", Socket, Mc, InterleaveEn, PrefetchEn, MemBase, MemLimit ));
            } else {
               if ((InterleaveEn != MemInfo->SncInfo[Socket].InterleaveEn) || (PrefetchEn != MemInfo->SncInfo[Socket].PrefetchEn) ||  \
                   (MemBase != MemInfo->McInfo[Socket][Mc].MemBase) || (MemLimit != MemInfo->McInfo[Socket][Mc].MemLimit)) {
                   KtiPrefetchSupport = FALSE;
               }
            }
         }
      }
    }
  }

  if (KtiPrefetchSupport == FALSE) {
    KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\nFail to enable Kti Prefetch with UMA"));

    //
    // 5330736: WARNING MESSAGE: Kti Prefetch is disabled since the HW doesn't support.
    //          But the D2c/D2K already be disabled because Kti Prefetch is enabled in setup
    //
    if (host->var.common.cpuType == CPU_SKX && host->var.common.stepping < B0_REV_SKX) {
      if (host->var.kti.SysConfig != SYS_CONFIG_1S){
        if (host->setup.kti.KtiPrefetchEn==1){
          KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "WARNING MESSAGE: The platform doesn't support the KtiPrefetch, please disable the KtiPrefetch in SETUP\n"));
        }
      }
    }
    return KTI_FAILURE;
  }

  //Step 4: a. Set Snc config value
  //        b. Calculate the MiniSadTable and mode based on MemInfo->SncInfo[Socket].InterleaveEn
  //
  //
  MemLimit_1 = MemLimit >> 4;
  if( MemLimit % 16) {
     MemLimit_1 += 1;
  }

  SncConfig.Data = 0;
  SncConfig.Bits.snc_ind_en = 1;
  SncConfig.Bits.lowest_cbo_id_clus1 = MemInfo->SncInfo[0].NumOfChaPerCluster;
  SncConfig.Bits.lowest_cbo_id_clus2 = SncConfig.Bits.lowest_cbo_id_clus3 = host->var.kti.CpuInfo[0].TotCha;

  MiniSadTable = Minisadmodeselect = 0;

  for (Socket = 0; Socket < MAX_SOCKET; Socket++){
    if ((host->var.common.socketPresentBitMap & (BIT0 << Socket)) != 0){
      if (MemInfo->SncInfo[Socket].Interleave_256B == 1){
        Minisadmodeselect = 1;   // 256B interleave, set to mode 1
      } else {
        Minisadmodeselect = 0;   // 64B interleave, set to mode 0
      }
      if ( MemInfo->SncInfo[Socket].InterleaveEn == 1){       // both Mc has memory on this socket
        // s5370398.
        if ((host->var.common.cpuType == CPU_SKX) && (host->var.common.stepping >= B0_REV_SKX)) {
          MiniSadTable = 0x10101010;
        } else {
          // check if 2 channels per Mc, then use P9 or P7 to select Mc, P8 or P6 to select channel
          // Currently we don't support P8 or P6 to select Mc, P9 or P7 to select channel
          if ((MemInfo->McInfo[Socket][0].ChannelInterBitmap == BITMAP_CH0_CH1) ||
              (MemInfo->McInfo[Socket][0].ChannelInterBitmap == BITMAP_CH1_CH2) ||
              (MemInfo->McInfo[Socket][0].ChannelInterBitmap == BITMAP_CH0_CH2)) {
            MiniSadTable = 0x11110000;                                                 // actually is 0xXX11XX00
          } else {
            MiniSadTable = 0x10101010;                                                 // actually is 0xXX10XX10
          }
        } //B0_REV_SKX
      } else {                                               // only Mc0 or Mc1 has memory on this socket
        for (Mc = 0; Mc < host->var.mem.maxIMC; Mc++){
          if (MemInfo->McInfo[Socket][Mc].Enabled == 1){
            if ( Mc == 0){                            // only Mc0 has memory on this socket
              MiniSadTable = 0x00000000;
            } else {
              MiniSadTable = 0x11111111;              // only Mc1 has memory on this socket
            }
          }
        }
      }
    }
  }

  //Step 5: Ready to program Kti Prefetch register
  //
  //
  for (Socket = 0; Socket < MAX_SOCKET; Socket++){
    if ((host->var.common.socketPresentBitMap & (BIT0 << Socket)) != 0){
      Option = PrefetchOption(host);
      PrefetchM2mem(host, Socket, MemInfo, Option);
      for (Ctr = 0; Ctr < host->var.common.KtiPortCnt; Ctr++) {
        if (host->var.kti.CpuInfo[Socket].LepInfo[Ctr].Valid == TRUE){
           KtiWritePciCfg (host, Socket, Ctr, KTILK_SNC_CONFIG_KTI_LLPMON_REG, SncConfig.Data);
           for (Cluster = 0; Cluster <= MAX_CLUSTERS; Cluster++) {

             SncBase.Data = KtiReadPciCfg (host, Socket, Ctr, KtiSncBaseRegisterOffset[Cluster]);
             if (Cluster == 0){
                SncBase.Bits.starting_addr_0 = MemBase;
             } else {
                SncBase.Bits.starting_addr_0 = MemLimit_1;
             }

             KtiWritePciCfg (host, Socket, Ctr, KtiSncBaseRegisterOffset[Cluster], SncBase.Data);
           } //
        } //kti port valid??
      } //Kti port loop
      ProgramKtiPrefetchRegister(host, Socket, Minisadmodeselect, MiniSadTable);
      KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\nUMA Enable Socket %x KtiPrefetch:SncConfig %x Minisadmodeselect %x MiniSadTable %x",
                       Socket, SncConfig.Data, Minisadmodeselect, MiniSadTable));
    } //Socket valid??
  }  //Socket

  //
  // Need to program SNC setting in MEMEM here in warm reset flow because these fields are not sticky
  //
  if ((host->var.common.cpuType == CPU_SKX) && (host->var.common.stepping >= B0_REV_SKX)) {//5331728
    for (Socket = 0; Socket < MAX_SOCKET; Socket++){
      if ((host->var.common.socketPresentBitMap & (BIT0 << Socket)) != 0){
        for (Ctr = 0; Ctr < host->var.mem.maxIMC; Ctr++) {
          TopologyM2Mem.Data = KtiReadPciCfg (host, Socket, Ctr, TOPOLOGY_M2MEM_MAIN_REG);
          TopologyM2Mem.Bits.sncluster = 0;
          TopologyM2Mem.Bits.basecluster1 = host->var.kti.CpuInfo[Socket].TotCha;
          KtiWritePciCfg (host, Socket, Ctr, TOPOLOGY_M2MEM_MAIN_REG, TopologyM2Mem.Data);
        }
      }
    }
  }


  return KTI_SUCCESS;
}

/**
  Disable SNC Flag

  This routine to disable SNC flag for warm reset

  @param  host                    Pointer to the system host (root) structure.

  @retval None

**/
VOID
DisabledSncFlag (
  struct sysHost             *host
)
{
  UINT32         Data32;
  UINT8          SbspId;

  //
  // Set the flag in CMOS to indicate the populated memory is not suitable for SNC mode operation.
  // Trigger a warm reset to reconfigure the system to operate in non-SNC mode.
  //
  KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\nFail to boot in SNC mode!\n"));
  host->var.kti.OutSncEn = 0;
  //
  // Store the following in Sticky scratchpad 7 bit [30] - Set SNC Mode Enable failure/recovery flag
  //
  SbspId = GetSbspSktId(host);
  Data32 = KtiReadPciCfg (host, SbspId, 0, BIOSSCRATCHPAD7_UBOX_MISC_REG);
  Data32 = Data32 | BIT30;
  KtiWritePciCfg (host, SbspId, 0, BIOSSCRATCHPAD7_UBOX_MISC_REG, Data32);


  //
  // 5330736: WARNING MESSAGE: that SNC/Prefetch is disabled since the HW cannot support. But the D2c/D2K alreeady disable because SNC/Prefetch enable
  //
  if (host->var.common.cpuType == CPU_SKX && host->var.common.stepping < B0_REV_SKX) {
    if (host->var.kti.SysConfig != SYS_CONFIG_1S){
      if (host->setup.kti.SncEn==1){
        KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "WARNING MESSAGE: The platform doesn't  the SNC and Prefetch mode, please disable the SNCEN and XPT/KTI Prefetch in SETUP\n"));
      }
    }
  }
}

/**
  Programs SNC Mode

  This routine programs the SNC mode.

  @param  host                    Pointer to the system host (root) structure.
  @param  MemInfo                 Memory Information structure.

  @retval UNCORE_SUCCESS          SNC mode is programmed successfully.
  @retval UNCORE_FAILURE          Memory profile is not suitable for SNC mode operation.

**/
KTI_STATUS
EnableSncMode (
  struct sysHost             *host,
  MEM_INFO                   *MemInfo
  )
{
  KTI_STATUS     Status;

  Status = KTI_SUCCESS;
  if (CheckDdrMemoryPopulation (host, MemInfo) == KTI_FAILURE) {    // Update MemInfo Data Structure
     if (host->var.common.CurrentReset == POST_RESET_POWERGOOD){    // Power good path
         DisabledSncFlag(host);
         return KTI_FAILURE;
     }
  }

  //
  // Fall back to All2All mode with one cluster. Also if the memory mode is Cache, there will be only
  // one node in the system; in which disable the NUMA.
  //
  Status = CalculateClusterBase(host, MemInfo);
  if (Status == KTI_FAILURE) {
    DisabledSncFlag(host);
    //
    // Set the flag in CMOS to indicate the populated memory is not suitable for SNC mode operation.
    // Trigger a warm reset to reconfigure the system to operate in non-SNC mode.
    //
    KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\nRequest disable SNC and issue WARM RESET RESET!"));
    host->var.common.resetRequired |= POST_RESET_WARM;
  } else if (Status == KTI_SNC_CHANGED) {
    //
    // Trigger a warm reset to reconfigure the system to operate in non-SNC mode.
    //
    KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\nRequest to change number of snc cluster and issue WARM RESET RESET!"));
    host->var.common.resetRequired |= POST_RESET_WARM;
  }

  //Write to SNC related registers now
  ConfigureSncMode (host, MemInfo, Status);

  return Status;
}

/**
  Send Mesh Mode mailbox command to Pcode

  This routine sends the Mesh Mode BIOS to Pcode Mailbox command.

  @param  host                    Pointer to the system host (root) structure.

  @retval None

**/
VOID
SendMeshModePcodeMailboxCommand (
  struct sysHost    *host
  )
{
  UINT8         Socket;
  UINT32        MailboxData;

  if (host->var.kti.OutSncEn) {
     MailboxData = MAILBOX_BIOS_MESH_MODE_ID_SNC | (MAILBOX_BIOS_SUBCMD_MESH_MODE_SNC2 << 8);
  } else {
     MailboxData = MAILBOX_BIOS_MESH_MODE_ID_ALLTOALL;
  }

  // Send Mesh Mode Mailbox command to Pcode
  for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
    if ((host->var.common.socketPresentBitMap & (BIT0 << Socket)) != 0) {
      SendMailBoxCmdToPcode (host, Socket, MAILBOX_BIOS_CMD_MESH_MODE, MailboxData);
    }
  }
}

/**
  Programs hash mask to 4G as default if 2LM Mode

  This routine programs the SNC mode.

  @param  host                    Pointer to the system host (root) structure
  @param  Socket                  Socket
  @param  Enable                  2LM enabled or disabled

  @retval None

**/
VOID
Program2LMHashRegister (
  struct sysHost             *host,
  UINT8                      Socket,
  BOOLEAN                    Enable
  )
{
  SNC_BASE_1_CHABC_PMA_STRUCT             SncBase1_Cha;
  SNC_BASE_2_CHABC_PMA_STRUCT             SncBase2_Cha;
  SNC_BASE_1_IIO_VTD_STRUCT               SncBase1_Iio;
  SNC_BASE_2_IIO_VTD_STRUCT               SncBase2_Iio;
  KTILK_SNC_BASE_1_KTI_LLPMON_STRUCT      SncBase1_Kti;
  KTILK_SNC_BASE_2_KTI_LLPMON_STRUCT      SncBase2_Kti;
  UINT32                                  address_mask_45_40;
  UINT32                                  address_mask_39_31;
  UINT8                                   Ctr;

  KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\nConfigure  Socket %d 2LM Hash ", Socket));
  if (Enable == TRUE) {               //Set TO 4G
    KtiDebugPrintInfo0((host, KTI_DEBUG_INFO0, "-Enabled\n"));
    address_mask_45_40 = 0x3f;
    address_mask_39_31 = 0x1fe;
  } else {                            //Clear to 0
    KtiDebugPrintInfo0((host, KTI_DEBUG_INFO0, "-Disabled\n"));
    address_mask_45_40 = 0;
    address_mask_39_31 = 0;
  }

  //CHA
  SncBase1_Cha.Data = KtiReadPciCfg (host, Socket, 0, SNC_BASE_1_CHABC_PMA_REG);
  SncBase1_Cha.Bits.address_mask_45_40 = address_mask_45_40;
  KtiWritePciCfg (host, Socket, 0, SNC_BASE_1_CHABC_PMA_REG, SncBase1_Cha.Data);

  SncBase2_Cha.Data = KtiReadPciCfg (host, Socket, 0, SNC_BASE_2_CHABC_PMA_REG);
  SncBase2_Cha.Bits.address_mask_39_31 = address_mask_39_31;
  KtiWritePciCfg (host, Socket, 0, SNC_BASE_2_CHABC_PMA_REG, SncBase2_Cha.Data);

  //KTI
  for (Ctr = 0; Ctr < host->var.common.KtiPortCnt; Ctr++) {
     if (host->var.kti.CpuInfo[Socket].LepInfo[Ctr].Valid == TRUE){
        SncBase1_Kti.Data = KtiReadPciCfg (host, Socket, Ctr, KTILK_SNC_BASE_1_KTI_LLPMON_REG);
        SncBase1_Kti.Bits.addrmask_45_40 = address_mask_45_40;
        KtiWritePciCfg (host, Socket, Ctr, KTILK_SNC_BASE_1_KTI_LLPMON_REG, SncBase1_Kti.Data);

        SncBase2_Kti.Data = KtiReadPciCfg (host, Socket, Ctr, KTILK_SNC_BASE_2_KTI_LLPMON_REG);
        SncBase2_Kti.Bits.addrmask_39_31 = address_mask_39_31;
        KtiWritePciCfg (host, Socket, Ctr, KTILK_SNC_BASE_2_KTI_LLPMON_REG, SncBase2_Kti.Data);
     }
  }

  //IIO
  for (Ctr = 1; Ctr < MAX_IIO_STACK; Ctr++) {
    if (host->var.common.stackPresentBitmap[Socket] & (1 << Ctr)) {
      SncBase1_Iio.Data = KtiReadPciCfg (host, Socket, Ctr, SNC_BASE_1_IIO_VTD_REG);
      SncBase1_Iio.Bits.address_mask_45_40 = address_mask_45_40;
      KtiWritePciCfg (host, Socket, Ctr, SNC_BASE_1_IIO_VTD_REG, SncBase1_Iio.Data);

      SncBase2_Iio.Data = KtiReadPciCfg (host, Socket, Ctr, SNC_BASE_2_IIO_VTD_REG);
      SncBase2_Iio.Bits.address_mask_39_31 = address_mask_39_31;
      KtiWritePciCfg (host, Socket, Ctr, SNC_BASE_2_IIO_VTD_REG, SncBase2_Iio.Data);
    }
  }
}


/**
  Programs hash mask to 4G as default if 2LM Mode

  @param  host                    Pointer to the system host (root) structure.

  @retval UNCORE_SUCCESS
  @retval UNCORE_FAILURE

**/
KTI_STATUS
TwoLmHashSetting (
  struct sysHost             *host
  )
{

  SNC_BASE_2_CHABC_PMA_STRUCT             SncBase2_Cha;
  HA_COH_CFG_TOR_CHA_PMA_STRUCT           haCohCfgTor;
  KTI_STATUS     Status;
  UINT8  Socket, Mc, Ch, ChannelNumber, AepDimmPresent;
  UINT16 ChannelMemSize;
  struct channelNvram (*channelNvList)[MAX_CH];
  struct memNvram  *nvramMem;

  Status = KTI_SUCCESS;
  if (host->var.common.CurrentReset == POST_RESET_POWERGOOD){    // cold reset path

    nvramMem = &host->nvram.mem;
    for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
      if ((host->var.common.socketPresentBitMap & (BIT0 << Socket)) != 0) {

        //Step 1: check if Aep Dimm Present on this socket
        channelNvList = &nvramMem->socket[Socket].channelList;
        AepDimmPresent = 0;
        for (Mc = 0; Mc < host->var.mem.maxIMC; Mc++){
          for (Ch = 0; Ch < MAX_MC_CH; Ch++) {
            // Detects which channel have memory and update MemInfo for furture use
            ChannelMemSize = 0;
            ChannelNumber = Ch + Mc *MAX_MC_CH;
            if ((*channelNvList)[ChannelNumber].enabled) {
              ReadChannelDimmInfo(host, Socket, Mc, Ch, &ChannelMemSize, &AepDimmPresent);
            } //Ch enable?
          } // Ch
        } // Mc

        //Step 2: check if user setup is 2LM or "auto" and Aep dimm present, then set to 4G (PA32) for hash fuction in CHA/IIO/KTI
        if ((host->setup.mem.volMemMode == VOL_MEM_MODE_2LM || host->setup.mem.volMemMode == VOL_MEM_MODE_AUTO) && AepDimmPresent == TRUE){
          Program2LMHashRegister(host, Socket, TRUE);
        } // 2LM enabled

      } // Socket valid
    } // scan Socket

  } else {            //warm reset flow
     for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
       if ((host->var.common.socketPresentBitMap & (BIT0 << Socket)) != 0) {
         //
         // Compare between 2LM enabled bit (is set by warm reset MRC flow) and 2LM hash register (is set by this routine during cold reset)
         //
         haCohCfgTor.Data = ReadCpuPciCfgEx(host, Socket, 0, HA_COH_CFG_TOR_CHA_PMA_REG);
         SncBase2_Cha.Data = KtiReadPciCfg (host, Socket, 0, SNC_BASE_2_CHABC_PMA_REG);

         // Two cases Need to be re-program 2LM hashing setting based on MRC 2LM setting and issues a warm reset

         // Case 1: MRC sets to 1LM during warm reset but this routine set to 2LM during cold reset prediction
         if ((haCohCfgTor.Bits.twolm == 0) && (SncBase2_Cha.Bits.address_mask_39_31 == 0x1fe)) {
           Program2LMHashRegister(host, Socket, FALSE);
           host->var.common.resetRequired |= POST_RESET_WARM;
         // Case 2: MRC sets to 2LM during warm reset but this routine set to 1LM during cold reset prediction (but this case, it seems impossible)
         } else if ((haCohCfgTor.Bits.twolm == 1) && (SncBase2_Cha.Bits.address_mask_39_31 == 0)){
           Program2LMHashRegister(host, Socket, TRUE);
           host->var.common.resetRequired |= POST_RESET_WARM;
         }

       } // socket valid
     } // scan socket
  } // warm reset

  return Status;
}

/**
  Enable Mesh Mode

  This routine performs the necessary setup for mesh mode and send Mesh Mode
  mailbox command to Pcode.

  @param  host                    Pointer to the system host (root) structure.

  @retval None

**/
VOID
UncoreEnableMeshMode (
  struct sysHost             *host
  )
{
  MEM_INFO                         MemInfo;
  DEFEATURES0_M2MEM_MAIN_STRUCT    m2mDefeatures0;
  TIMEOUT_M2MEM_MAIN_STRUCT        m2mTimeout;
  UINT8                            Socket, mcId;
  XPT_PREFETCH_CONFIG1_CHABC_UTIL_STRUCT   XptPrefetchConfig1;
  UINT32                           TimeOutLockVal;


  KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\n\n******* Configure Mesh Mode - START *******\n"));


  // setup 2LM hasting setting
  TwoLmHashSetting(host);

  MemSetLocal ((UINT8 *) &MemInfo, 0x00, sizeof (MemInfo));
  if (host->var.kti.OutSncPrefetchEn && (host->setup.common.numaEn == KTI_ENABLE)) {
    EnableSncMode (host, &MemInfo);

  //check if need to enable Kti prefetch if warm-reset and UMA case
  //  } else if (host->var.common.CurrentReset != POST_RESET_POWERGOOD && !(host->setup.mem.options & NUMA_AWARE)) {
  //s5370476    EnableUmaKtiPrefetch(host, &MemInfo);
  }


  //
  //s5370431: Disable M2M timeout if Xpt or Kti prefetch enabled, WA for A-B stepping. Should be fixed in H stepping
  //
  if (host->var.common.stepping < H0_REV_SKX) {
    for(Socket = 0; Socket < MAX_SOCKET; Socket++) {
      if  ((host->var.common.socketPresentBitMap & (BIT0 << Socket)) != 0)  {
        for (mcId = 0; mcId < host->var.mem.maxIMC; mcId++) {
          if (host->var.mem.socket[Socket].imcEnabled[mcId] == 0) continue;
          m2mDefeatures0.Data = KtiReadPciCfg (host, Socket, mcId, DEFEATURES0_M2MEM_MAIN_REG);
          XptPrefetchConfig1.Data = KtiReadPciCfg (host, Socket, 0, XPT_PREFETCH_CONFIG1_CHABC_UTIL_REG);
  
          m2mTimeout.Data = KtiReadPciCfg (host, Socket, mcId, TIMEOUT_M2MEM_MAIN_REG);
          TimeOutLockVal = m2mTimeout.Bits.timeoutlock;
          // clear lock
          m2mTimeout.Bits.timeoutlock = 0 ;
          KtiWritePciCfg (host, Socket, mcId, TIMEOUT_M2MEM_MAIN_REG, m2mTimeout.Data);
  
          // clear timeouten
          if ((m2mDefeatures0.Bits.ingbypdis == 0) && ((XptPrefetchConfig1.Bits.xptprefetchdisable == 0) || (host->var.kti.OutKtiPrefetch == 1))) {
            m2mTimeout.Bits.timeouten = 0;
          }
          KtiWritePciCfg (host, Socket, mcId, TIMEOUT_M2MEM_MAIN_REG, m2mTimeout.Data);
  
          // restore the lock if its there
          m2mTimeout.Bits.timeoutlock = TimeOutLockVal;
          KtiWritePciCfg (host, Socket, mcId, TIMEOUT_M2MEM_MAIN_REG, m2mTimeout.Data);
        } // mcId
      }
    } // socket loop
  } //if stepping < H0


  //
  // Send Mesh Mode mailbox command to Pcode before warm reset
  //
  //if (Status == KTI_SUCCESS && host->var.common.CurrentReset == POST_RESET_POWERGOOD ) {
  //  SendMeshModePcodeMailboxCommand (host);
  //}

  KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\n******* Configure Mesh Mode - END   *******\n"));
}

/**
  Pcu Misc Config

  This routine performs the misc mail box commands

    @param host              - pointer to the system host root structure
    @param SocketData        - pointer to the socket data structure
    @param KtiInternalGlobal - pointer to the KTI RC internal global structure

    @retval KTI_SUCCESS - on successful completion


**/

KTI_STATUS
KtiPcuMiscConfig (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal
  )
{
  UINT8  Index;
  UINT32 Data32;


  for (Index = 0; Index < MAX_SOCKET; Index++) {
    if ((SocketData->Cpu[Index].Valid == TRUE) && (SocketData->Cpu[Index].SocType == SOCKET_TYPE_CPU)) {
      //
      // Enable IDI Parity at all times
      //
      SendMailBoxCmdToPcode(host, Index, MAILBOX_BIOS_CMD_READ_PCU_MISC_CONFIG, 0);
      Data32 = KtiReadPciCfg (host, Index, 0, BIOS_MAILBOX_DATA);
      KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\nMAILBOX_BIOS_CMD_READ_PCU_MISC_CONFIG returned %u", Data32));
      Data32 = Data32 | BIT26;
      SendMailBoxCmdToPcode(host, Index, MAILBOX_BIOS_CMD_WRITE_PCU_MISC_CONFIG, Data32);
    }
  }
  return KTI_SUCCESS;
}


/**
  s5331840: Program the KTI IRQ Threshold for 4S ring or 8S

  @param host:  pointer to sysHost structure on stack

  @retval VOID
**/
VOID
Init_IRQ_Threshold (
  struct sysHost     *host
)
{
  UINT16              IRQTable[3][5]={{RRQ_IRQ_THRESHOLD_DIS, RRQ_IRQ_2S4SFC_AUTO, RRQ_IRQ_2S4SFC_LOW, RRQ_IRQ_2S4SFC_MEDIUM, RRQ_IRQ_2S4SFC_HIGH},
                                      {RRQ_IRQ_THRESHOLD_DIS, RRQ_IRQ_4SRING_AUTO, RRQ_IRQ_4SRING_LOW, RRQ_IRQ_4SRING_MEDIUM, RRQ_IRQ_4SRING_HIGH},
                                      {RRQ_IRQ_THRESHOLD_DIS, RRQ_IRQ_8S_AUTO, RRQ_IRQ_8S_LOW, RRQ_IRQ_8S_MEDIUM, RRQ_IRQ_8S_HIGH}};
  UINT8               ConfigIndex = 0;
  UINT8               SetupKnob;

  //
  // 1S or Disable : ConfigIndex=0, IrqThreshold=0 for disable 
  // 2S or 4SFC    : ConfigIndex=0
  // 4S Ring       : ConfigIndex=1
  // 8S            : ConfigIndex=2

  SetupKnob = host->setup.kti.IrqThreshold;
  if ((host->setup.kti.IrqThreshold == KTI_DISABLE) || (host->var.kti.SysConfig == SYS_CONFIG_1S)) {
    ConfigIndex = 0;
    SetupKnob = 0;
  } else if ((host->var.kti.SysConfig == SYS_CONFIG_4S) && host->var.kti.Is4SRing){
    ConfigIndex=1;
  } else if ((host->var.kti.SysConfig == SYS_CONFIG_8S)){
    ConfigIndex=2;
  } else {
    ConfigIndex=0;
  }

  host->var.kti.OutIRQThreshold = IRQTable[ConfigIndex][SetupKnob] & 0x00FF;
  host->var.kti.OutRrqThreshold = (IRQTable[ConfigIndex][SetupKnob] & 0xFF00) >> 8; 


}

