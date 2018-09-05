/**
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c) 2008 - 2015, Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:
  KtiRasMain.c

Abstract:
  KTI RAS Support.

Note:
  This driver should not call any of the register access functions until
  mRasHost structure is initialized.

**/
#include <SysFunc.h>
#include "KtiRas.h"
#include "KtiRasMain.h"
#include "KtiRasSupport.h"

extern  EFI_HP_IOX_ACCESS_PROTOCOL *mHpIOXAccess;

/**

  Do some checking to make sure the parameters passed to the driver APIs are valid.

  @param EventType  - RAS_MPLINK_EVNT_ONLINE or RAS_MPLINK_EVNT_OFFLINE
  @param SocketType - should always be RAS_MPLINK_SOCKET_TYPE_CPU
  @param SocketId   - Socket ID to be on/offlined

  @retval RAS_MPLINK_SUCCESS
  @retval RAS_MPLINK_FAILURE_INVALID_EVTTYPE

**/
RAS_MPLINK_STATUS
ValidateParameters (
  IN UINT8 EventType,
  IN INT32 SocketType,
  IN INT32 SocketId
  )
{

  if (EventType != RAS_MPLINK_EVNT_ONLINE && EventType != RAS_MPLINK_EVNT_OFFLINE) {
    KtiDebugPrintInfo0((mRasMpLink->Host, KTI_DEBUG_INFO0 , "\n  Invalid Event Type - Must be online or offline event type \n"));
    return RAS_MPLINK_FAILURE_INVALID_EVTTYPE;
  }

  if (SocketType == RAS_MPLINK_SOCKET_TYPE_CPU) {
    if (SocketId >= MAX_SOCKET){
      KtiDebugPrintInfo0((mRasMpLink->Host, KTI_DEBUG_INFO0 , "\n  Invalid CPU Socket Id. \n"));
      return RAS_MPLINK_FAILURE_INVALID_EVTTYPE;
    }
    if ((EventType == RAS_MPLINK_EVNT_ONLINE && mRasMpLink->Host->var.kti.CpuInfo[SocketId].Valid == TRUE) ||
        (EventType == RAS_MPLINK_EVNT_OFFLINE && mRasMpLink->Host->var.kti.CpuInfo[SocketId].Valid == FALSE)) {
      KtiDebugPrintInfo0((mRasMpLink->Host, KTI_DEBUG_INFO0 , "\n  Can not online/offline a CPU which is already onlined/offlined. \n"));
      return RAS_MPLINK_FAILURE_INVALID_EVTTYPE;
    }
  } else {
    KtiDebugPrintInfo0((mRasMpLink->Host, KTI_DEBUG_INFO0 , "\n  Invalid Socket Type - Must be CPU or IOH socket. \n"));
    return RAS_MPLINK_FAILURE_INVALID_EVTTYPE;
  }

  return RAS_MPLINK_SUCCESS;
} // ValidateParameters

/**

  Issue a Power On PLD command to apply power to the new socket.

  @retval RAS_MPLINK_SUCCESS - on successful completion

**/
RAS_MPLINK_STATUS
PowerUpOnlinedSocket (
 VOID
  )
{
  EFI_STATUS  SubStatus;

  SubStatus = mHpIOXAccess->IssuePldCmdThroIox (ISSUE_POWER_ON, (UINT8)mPendingSocId, FALSE);

  return RAS_MPLINK_SUCCESS;
} // PowerUpOnlinedSocket

/**

  Issue an Online PLD command to release reset on the new socket.

  @retval RAS_MPLINK_SUCCESS - on successful completion

**/
RAS_MPLINK_STATUS
ReleaseResetOnlinedSocket (
 VOID
  )
{
  EFI_STATUS  SubStatus;

  //
  // SKX_TODO: TimeSliced should be TRUE instead of FALSE
  //
  SubStatus = mHpIOXAccess->IssuePldCmdThroIox (ISSUE_ONLINE, (UINT8)mPendingSocId, FALSE);

  return RAS_MPLINK_SUCCESS;
} // ReleaseResetOnlinedSocket

/**

  Issue an Offline PLD command.

  @param SocketId      - Socket ID to offline
  @param RasSocketData - pointer to the KTI RAS Socket Data struct

  @retval RAS_MPLINK_SUCCESS - on successful completion

**/
RAS_MPLINK_STATUS
PowerDownOnlinedSocket (
  IN INT32            SocketId,
  IN KTI_SOCKET_DATA  *RasSocketData
  )
{
  EFI_STATUS  SubStatus;

  //
  // SKX_TODO: TimeSliced should be TRUE instead of FALSE
  //
  SubStatus = mHpIOXAccess->IssuePldCmdThroIox (ISSUE_OFFLINE, (UINT8)mPendingSocId, FALSE);
  //
  // Delay a little bit
  //
  KtiFixedDelay(mRasMpLink->Host, 5000);
  ClearCinitbeginOnInvalidLinks (RasSocketData);

  return RAS_MPLINK_SUCCESS;
}

/**

  Fill the mRasHost and mKtiInternal with latest info, and save some original settings:
    1. update mRasMpLink->Host->var.common.cpuFreq.hi, mRasMpLink->Host->var.common.cpuFreq.lo using InitCpuTimer
    2. Copy mRasMpLink->Host into mRasHost. From here on we update mRasHost
    3. Initialize like at cold boot phase:
        mRasHost->var.common.numCpus = MAX_SOCKET
        mRasHost->var.common.socketPresentBitMap = FLD_MASK(MAX_SOCKET, 0);
        mRasHost->var.common.SocketFirstBus[x]
        mRasHost->var.common.StackBus[x][y]
        mRasHost->var.common.SocketLastBus[x]
        mRasHost->var.common.sbsp
        mRasHost->var.kti.RasInProgress
        mRasHost->var.kti.RasEvtType
        mRasHost->var.kti.RasSocId
        mKtiInternalGlobal->SbspSoc
    4. Call UpdateCpuInfoInKtiInternal for SBSP
    5. Clear mRasSocketDataOrig, and copy over from mRasHost:
        mRasSocketDataOrig->Cpu[x].Valid
        mRasSocketDataOrig->Cpu[x].SocId
        mRasSocketDataOrig->Cpu[x].LinkData

  @retval RAS_MPLINK_SUCCESS on successful completion

**/
RAS_MPLINK_STATUS
PrepareHostDataStruct (
   VOID
  )
{
  UINT8   Idx1;

  DEBUG((EFI_D_ERROR, "\n  Preparing the Host structure for O*L event. "));
  DEBUG((EFI_D_ERROR, "\n  If Hot Plug enabled, assuming MAX_SOCKET for socketPresentBitMap"));

  //
  // Re-initialize cpuFreq in RAS because it has got the incorrect uninitialized
  // value from boot time before mpcpu initialized for update.
  //
  DEBUG((EFI_D_ERROR, "\n    Before update: "));
  DEBUG((EFI_D_ERROR, "\n    mRasMpLink->Host->var.common.cpuFreq.hi = %u ", mRasMpLink->Host->var.common.cpuFreq.hi));
  DEBUG((EFI_D_ERROR, "\n    mRasMpLink->Host->var.common.cpuFreq.lo = %u ", mRasMpLink->Host->var.common.cpuFreq.lo));
  InitCpuTimer (mRasMpLink->Host);
  DEBUG((EFI_D_ERROR, "\n    After update: "));
  DEBUG((EFI_D_ERROR, "\n    mRasMpLink->Host->var.common.cpuFreq.hi = %u ", mRasMpLink->Host->var.common.cpuFreq.hi));
  DEBUG((EFI_D_ERROR, "\n    mRasMpLink->Host->var.common.cpuFreq.lo = %u ", mRasMpLink->Host->var.common.cpuFreq.lo));

  MemCopy ((VOID *)mRasHost, (VOID *)mRasMpLink->Host, sizeof(SYSHOST));
  MemSetLocal ((UINT8 *) mRasHost->var.kti.WarningLog, 0, sizeof(mRasHost->var.kti.WarningLog));

  //
  // platformType is needed to be used in loading EPARAM as in boot time.
  // It is updated here because nvram is not passed in the HOB as setup and var.
  //
  mRasHost->nvram.common.platformType = PcdGet8(PcdPlatformType);

  //
  // Make hot plug just like the cold reset boot flow, assuming MAX_SOCKET.
  // Fill the host common structure with the Uncore & IIO bus numbers
  // so that the common CSR access routines become usable for all sockets once they get configured.
  // Later, warm reset topology discover will system wide re-program all routing on all sockets over
  // again, overriding this assumption.
  //
  mRasHost->var.common.numCpus = MAX_SOCKET;
  mRasHost->var.common.socketPresentBitMap = FLD_MASK(MAX_SOCKET, 0);

  mRasHost->var.common.sbsp = mSbspId;
  mRasHost->var.kti.RasInProgress = (UINT8) mInProgress;
  mRasHost->var.kti.RasEvtType = (UINT8) mPendingEvtType;
  mRasHost->var.kti.RasSocId = (UINT8) mPendingSocId;

  mKtiInternalGlobal->SbspSoc = mSbspId;
  mKtiInternalGlobal->MmcfgBase = mRasHost->setup.common.mmCfgBase;
  UpdateCpuInfoInKtiInternal (mRasHost, mKtiInternalGlobal, mSbspId);

  //
  // The mem section is not copied from the hob, but is used for MC accesses for example.
  // Populate what we need for now. Final solution to be added once MemOnline flow finalized.
  //
  mRasHost->var.mem.maxIMC = MAX_IMC;
  mRasHost->var.mem.numChPerMC = MAX_MC_CH;


  //
  // Initialize the SocketData to remember what is the original LEP before the O*L event occured
  //
  MemSetLocal ((VOID *)mRasSocketDataOrig, 0, sizeof(KTI_SOCKET_DATA));
  for (Idx1 = 0; Idx1 < MAX_SOCKET; ++Idx1) {
    if (mRasHost->var.kti.CpuInfo[Idx1].Valid == TRUE) {
      mRasSocketDataOrig->Cpu[Idx1].Valid = TRUE;
      mRasSocketDataOrig->Cpu[Idx1].SocId = Idx1;
      MemCopy ((VOID *)(mRasSocketDataOrig->Cpu[Idx1].LinkData), (VOID *)(mRasHost->var.kti.CpuInfo[Idx1].LepInfo), sizeof(mRasSocketDataOrig->Cpu[Idx1].LinkData));
    }
  }

  return RAS_MPLINK_SUCCESS;
} // PrepareHostDataStruct

/**

  Print out some KTI RAS variables to serial log.

  @retval RAS_MPLINK_SUCCESS - on successful completion

**/
RAS_MPLINK_STATUS
DataDumpRasMpLink (
   VOID
  )
{
    KtiDebugPrintInfo1 ((mRasHost, KTI_DEBUG_INFO0, "\n  DataDumpRasMpLink for mRasHost:    "));
    KtiDebugPrintInfo1 ((mRasHost, KTI_DEBUG_INFO0, "\n    platformType  %u ", mRasHost->nvram.common.platformType));
    KtiDebugPrintInfo1 ((mRasHost, KTI_DEBUG_INFO0, "\n    numCpus  %u ", mRasHost->var.common.numCpus));
    KtiDebugPrintInfo1 ((mRasHost, KTI_DEBUG_INFO0, "\n    socketPresentBitMap  0x%08X ", mRasHost->var.common.socketPresentBitMap));

    return RAS_MPLINK_SUCCESS;
} // DataDumpRasMpLink

/**

  Set up SBSP's MMCFG SAD so that the incoming socket's config space can be accessed from SBSP.

  @retval RAS_MPLINK_SUCCESS - on successful completion

**/
RAS_MPLINK_STATUS
OpenSbspConfigSpaceForHotAdd (
   VOID
  )
{
  MMCFG_RULE_N0_CHABC_SAD1_STRUCT MmcfgRule0;
  UINT8                           Idx, NumBuses,TargetsPerSocket;
  UINT32                          MmcfgGranularity;

  //
  // We pre-program the target list during boot, and will only need to update
  // MMCFG_RULE0.valid_targets in this function.
  //
  MmcfgRule0.Data = ReadCpuPciCfgRas(mRasHost, (UINT8)mSbspId, 0, MMCFG_RULE_N0_CHABC_SAD1_REG);

  if (mRasHost->setup.common.mmCfgSize <= 0x10000000) {
    MmcfgGranularity = SAD_BUS_GRANTY;
  } else {
    MmcfgGranularity = (mRasHost->setup.common.mmCfgSize / 1024 / 1024 / SAD_BUS_TARGETS);
  }

  Idx = 0;
  NumBuses = (UINT8)(((((mRasHost->var.kti.CpuInfo[mPendingSocId].CpuRes.BusLimit - mRasHost->var.kti.CpuInfo[mPendingSocId].CpuRes.BusBase + 1) / MmcfgGranularity) - 1)) + 1);
  TargetsPerSocket = NumBuses;
  while (NumBuses) {
    MmcfgRule0.Bits.valid_targets |= 1 << (Idx + TargetsPerSocket * mPendingSocId);
    NumBuses--;
    Idx++;
  }

  WriteCpuPciCfgRas (mRasHost, (UINT8)mSbspId, 0, MMCFG_RULE_N0_CHABC_SAD1_REG, MmcfgRule0.Data);

  //
  // Plan is to move the StackBus programming to boot flow as well,
  // but looks like currently boot flow leaves the StackBus values at 0xff
  //
  mRasHost->var.common.SocketFirstBus[mPendingSocId] = 0x20 * TargetsPerSocket * ((UINT8) mPendingSocId);
  for (Idx = 0; Idx < MAX_IIO_STACK; ++Idx) {
    mRasHost->var.common.StackBus[mPendingSocId][Idx] = mRasHost->var.common.SocketFirstBus[mPendingSocId] + Idx;
  }
  mRasHost->var.common.SocketLastBus[mPendingSocId] = mRasHost->var.common.SocketFirstBus[mPendingSocId] + ((0x20 * TargetsPerSocket) - 1);

  return RAS_MPLINK_SUCCESS;
} // OpenSbspConfigSpaceForHotAdd

/**

  The offline socket's config space needs to be removed from SBSP's MMCFG SAD.
  It will be copied to all active PBSP later in RasMpLinkMileStone10.

  @retval RAS_MPLINK_SUCCESS - on successful completion

**/
RAS_MPLINK_STATUS
AdjustSbspConfigSpaceForHotRemoval (
   VOID
  )
{
  MMCFG_RULE_N0_CHABC_SAD1_STRUCT MmcfgRule0;
  UINT8                           Idx, NumBuses,TargetsPerSocket;
  UINT32                          MmcfgGranularity;

  //
  // We will only need to update MMCFG_RULE0.valid_targets in this function
  // No need to change the StackBus programming here
  //
  MmcfgRule0.Data = ReadCpuPciCfgRas(mRasHost, (UINT8)mSbspId, 0, MMCFG_RULE_N0_CHABC_SAD1_REG);

  if (mRasHost->setup.common.mmCfgSize <= 0x10000000) {
    MmcfgGranularity = SAD_BUS_GRANTY;
  } else {
    MmcfgGranularity = (mRasHost->setup.common.mmCfgSize / 1024 / 1024 / SAD_BUS_TARGETS);
  }

  Idx = 0;
  NumBuses = (UINT8)(((((mRasHost->var.kti.CpuInfo[mPendingSocId].CpuRes.BusLimit - mRasHost->var.kti.CpuInfo[mPendingSocId].CpuRes.BusBase + 1) / MmcfgGranularity) - 1)) + 1);
  TargetsPerSocket = NumBuses;
  while (NumBuses) {
    MmcfgRule0.Bits.valid_targets &= ~(1 << (Idx + TargetsPerSocket * mPendingSocId));
    NumBuses--;
    Idx++;
  }

  WriteCpuPciCfgRas (mRasHost, (UINT8)mSbspId, 0, MMCFG_RULE_N0_CHABC_SAD1_REG, MmcfgRule0.Data);

  return RAS_MPLINK_SUCCESS;
} // AdjustSbspConfigSpaceForHotRemoval

/**

  Collect LEP on all pre-existing sockets, and check if any of the links
  are connected to the socket being onlined. If yes, report the decoded LEP
  and update mRasSocketData->Cpu[mPendingSocId].LinkData[x]. If no ports are
  connected to new socket it means the links failed to train in slow mode.

  @retval RAS_MPLINK_SUCCESS - if at least one of the pre-existing sockets
                               connects to the pending socket
  @retval RAS_MPLINK_FAILURE_SOC_UNREACHABLE - if no links report connection
                                               to the new socket

**/
RAS_MPLINK_STATUS
CollectLepForOnlineEvent (
   VOID
  )
{
  UINT8   Index;
  UINT8   KtiLink, LocalKtiLink;
  UINT8   PeerSocId;
  BOOLEAN SocValid= FALSE;

  KtiDebugPrintInfo0 ((mRasMpLink->Host, KTI_DEBUG_INFO0 , "\n  Collecting LEP for ONLINE event."));
  MemSetLocal ((VOID *)mRasSocketData, 0, sizeof(KTI_SOCKET_DATA));

  //
  // Since LEP is affected by the O*L event, recollect the LEP of the sockets that exist
  // already. Note that we can not reach the incoming socket itself since the route is not
  // yet set for it. But we don't need to reach the incoming socket to know its LEP. We can
  // deduce its LEP from its immediate neighbors' LEP.
  //

  //
  // Update the mRasSocketData structure (Equivalent to identifying what sockets are out there already)
  //
  for (Index = 0; Index < MAX_SOCKET; ++Index) {
    if (mRasHost->var.kti.CpuInfo[Index].Valid == TRUE) {
      mRasSocketData->Cpu[Index].Valid = TRUE;
      mRasSocketData->Cpu[Index].SocId = Index;
    }
  }

  //
  // We don't know who are the immediate neighbors of the incoming socket. So recollect the LEP
  // of all the existing sockets.
  //
  for (Index = 0; Index < MAX_SOCKET; ++Index) {
    if (mRasSocketData->Cpu[Index].Valid == TRUE) {
      ParseCpuLep (mRasHost, mRasSocketData, mKtiInternalGlobal, Index);
    }
  }

  //
  // Deduce the LEP of the Onlined socket from its neighbor's LEP
  //
  KtiDebugPrintInfo0 ((mRasMpLink->Host, KTI_DEBUG_INFO0, "\n  Onlined Socket CPU%u Link Exchange", mPendingSocId));
  for (Index = 0; Index < MAX_SOCKET; ++Index) {
    if (mRasSocketData->Cpu[Index].Valid == TRUE) {
      for (KtiLink = 0; KtiLink < mRasMpLink->Host->var.common.KtiPortCnt; ++KtiLink) {
        if (mRasSocketData->Cpu[Index].LinkData[KtiLink].Valid == TRUE &&
            mRasSocketData->Cpu[Index].LinkData[KtiLink].PeerSocId == (UINT32)mPendingSocId &&
            mRasSocketData->Cpu[Index].LinkData[KtiLink].PeerSocType == (UINT32)mPendingSocType) {
          SocValid = TRUE;
          LocalKtiLink = (UINT8)mRasSocketData->Cpu[Index].LinkData[KtiLink].PeerPort;
          KtiDebugPrintInfo0((mRasMpLink->Host, KTI_DEBUG_INFO0, " : LEP%u(%u,CPU%u)", LocalKtiLink, KtiLink, Index));
          mRasSocketData->Cpu[mPendingSocId].LinkData[LocalKtiLink].Valid = TRUE;
          mRasSocketData->Cpu[mPendingSocId].LinkData[LocalKtiLink].PeerSocId = Index;
          mRasSocketData->Cpu[mPendingSocId].LinkData[LocalKtiLink].PeerSocType = SOCKET_TYPE_CPU;
          mRasSocketData->Cpu[mPendingSocId].LinkData[LocalKtiLink].PeerPort = KtiLink;
        }
      } // for (KtiLink...)
    }
  } // for (Index...)

  //
  // Make sure the socket being Onlined is reachable
  //
  if (SocValid != TRUE) {
    KtiDebugPrintFatal ((mRasMpLink->Host, KTI_DEBUG_ERROR, "\n  Onlined socket is unreachable. At least one link to the socket must reach Normal Opeartion.\n  Operation Aborted"));
    return RAS_MPLINK_FAILURE_SOC_UNREACHABLE;
  }

  //
  // Mark the socket as valid
  //
  mRasSocketData->Cpu[mPendingSocId].Valid = TRUE;
  mRasSocketData->Cpu[mPendingSocId].SocId = (UINT8)mPendingSocId;

  //
  // The CSI RAS state machine is written to handle only one online event at a time. So even if the system
  // onlined more than one socket, this step will ensure that LEP of the links connected to socket other than
  // the one being handled (i.e the socket id/type passed to CSI RAS driver) is marked as "Invalid".
  //
  for (Index = 0; Index < MAX_SOCKET; ++Index) {
    if (mRasSocketData->Cpu[Index].Valid == TRUE) {
      for (KtiLink = 0; KtiLink < mRasMpLink->Host->var.common.KtiPortCnt; ++KtiLink) {
        if (mRasSocketData->Cpu[Index].LinkData[KtiLink].Valid == TRUE) {
          PeerSocId = (UINT8)mRasSocketData->Cpu[Index].LinkData[KtiLink].PeerSocId;
          if (mRasSocketData->Cpu[PeerSocId].Valid != TRUE) {
            KtiDebugPrintInfo0 ((mRasMpLink->Host, KTI_DEBUG_INFO0, " Clearing LEP of CPU%u Link%u connected to offlined CPU %u", Index, KtiLink, PeerSocId));
            MemSetLocal ((VOID *)&mRasSocketData->Cpu[Index].LinkData[KtiLink], 0, sizeof(mRasSocketData->Cpu[Index].LinkData[KtiLink]));
          }
        }
      } // for (KtiLink...)
    }
  } // for (Index...)

  return RAS_MPLINK_SUCCESS;
} // CollectLepForOnlineEvent

/**

  Mark the socket being offlined as invalid and also update LEP info to mark the ports
  connected to the socket being offlined as invalid.

  @retval RAS_MPLINK_SUCCESS - on successful completion

**/
RAS_MPLINK_STATUS
CollectLepForOfflineEvent (
   VOID
  )
{
  UINT8             Index;
  UINT8             KtiLink;

  KtiDebugPrintInfo0 ((mRasHost, KTI_DEBUG_INFO0 , "\n    Adjusting the LEP for OFFLINE event. "));
  MemSetLocal ((VOID *)mRasSocketData, 0, sizeof(KTI_SOCKET_DATA));

  //
  // Update the mRasSocketData structure (Equivalent to identifying what sockets are out there already)
  // and exclude the socket that is being offlined (i.e it is marked as invalid)
  //
  for (Index = 0; Index < MAX_SOCKET; ++Index) {
    if (mRasHost->var.kti.CpuInfo[Index].Valid != TRUE || Index == mPendingSocId) {
      continue;
    }
    mRasSocketData->Cpu[Index].Valid = TRUE;
    mRasSocketData->Cpu[Index].SocId = Index;
    MemCopy((VOID *)mRasSocketData->Cpu[Index].LinkData, (VOID *)mRasHost->var.kti.CpuInfo[Index].LepInfo, sizeof(mRasSocketData->Cpu[Index].LinkData));
  }

  //
  // Mark the immediate neighbors' links connecting to the offlined socket as Invalid
  //
  for (Index = 0; Index < MAX_SOCKET; ++Index) {
    if (mRasSocketData->Cpu[Index].Valid == TRUE) {
      for (KtiLink = 0; KtiLink < mRasHost->var.common.KtiPortCnt; ++KtiLink) {
        if (mRasSocketData->Cpu[Index].LinkData[KtiLink].Valid == TRUE &&
            mRasSocketData->Cpu[Index].LinkData[KtiLink].PeerSocId == (UINT32)mPendingSocId &&
            mRasSocketData->Cpu[Index].LinkData[KtiLink].PeerSocType == (UINT32)mPendingSocType) {
            MemSetLocal((VOID *)&mRasSocketData->Cpu[Index].LinkData[KtiLink], 0, sizeof(mRasSocketData->Cpu[Index].LinkData[KtiLink]));
        }
      } // for(KtiLink...)
    }
  } // for(Index...)
  return RAS_MPLINK_SUCCESS;
} // CollectLepForOfflineEvent

/**

  Copy LEP and Topology Tree info to mRasHost from mRasSocketData.

  @retval RAS_MPLINK_SUCCESS - on successful completion

**/
RAS_MPLINK_STATUS
CopyTopologyInfoToHostParm (
  )
{
  UINT8                      Index;

  for (Index = 0; Index < MAX_SOCKET; ++Index) {
    if (mRasSocketData->Cpu[Index].Valid == TRUE) {
      mRasHost->var.kti.CpuInfo[Index].Valid = mRasSocketData->Cpu[Index].Valid;
      mRasHost->var.kti.CpuInfo[Index].SocId = mRasSocketData->Cpu[Index].SocId;

      // Copy LEP info
      MemCopy(
         (VOID *) mRasHost->var.kti.CpuInfo[Index].LepInfo,
         (VOID *) mRasSocketData->Cpu[Index].LinkData,
          sizeof(mRasHost->var.kti.CpuInfo[Index].LepInfo)
         );

      // Copy Topology tree info
      MemCopy(
        (VOID *)mRasHost->var.kti.CpuInfo[Index].TopologyInfo,
        (VOID *)mRasSocketData->CpuTree[Index],
        sizeof(mRasHost->var.kti.CpuInfo[Index].TopologyInfo)
        );
    }
  }
  return RAS_MPLINK_SUCCESS;
} // CopyTopologyInfoToHostParm

/**

  Set up the Config Access Path and Boot Path (if applicable) for the online event. To minimize the
  number of RTA entries touched, the onlined socket is appended to the topology tree of the
  SBSP for setting up Config Access Path. For Boot Path, the Onlined CPU socket uses one of its
  immediate neighbor's route to LIOH.

  @retval RAS_MPLINK_SUCCESS - on successful completion
  @retval RAS_MPLINK_FAILURE_SOC_UNREACHABLE - if no sockets show a link connected to the new socket
  @retval RAS_MPLINK_FAILURE_GENERAL

**/
RAS_MPLINK_STATUS
SetConfigAndBootPath (
   VOID
  )
{
  TOPOLOGY_TREE_NODE         Parent, Child, Temp;
  UINT8                      Index, KtiLink = 0;

  KtiDebugPrintInfo0((mRasMpLink->Host, KTI_DEBUG_INFO0 , "\n    Setting config path for online event"));

  //
  // Use SBSP's optimum route tree as the minimum path tree
  //
  MemCopy((VOID *)mRasSocketData->SbspMinPathTree, (VOID *)mRasHost->var.kti.CpuInfo[mSbspId].TopologyInfo, sizeof(mRasHost->var.kti.CpuInfo[mSbspId].TopologyInfo));

  //
  // Append the Hot Added socket to SBSP's minimum path tree
  //
  Parent.RawNode = 0;
  Index = 0;
  do {
    Temp.RawNode = mRasSocketData->SbspMinPathTree[Index].RawNode;
    if (Temp.Node.Valid == TRUE && Temp.Node.SocType == SOCKET_TYPE_CPU) {
      for(KtiLink = 0; KtiLink < mRasMpLink->Host->var.common.KtiPortCnt; ++KtiLink) {
        //
        // Check if this node has a link connected to the Hot Added socket
        //
        if (mRasSocketData->Cpu[Temp.Node.SocId].LinkData[KtiLink].Valid == TRUE &&
            mRasSocketData->Cpu[Temp.Node.SocId].LinkData[KtiLink].PeerSocId == (UINT32)mPendingSocId &&
            mRasSocketData->Cpu[Temp.Node.SocId].LinkData[KtiLink].PeerSocType== (UINT32)mPendingSocType) {
          Parent.RawNode = Temp.RawNode;
          break;
        }
      }
    }
    if (Parent.RawNode != 0) break;
    ++Index;
  } while (Index < MAX_TREE_NODES);

  if (Parent.Node.Valid != TRUE) {
    KtiDebugPrintFatal((mRasMpLink->Host, KTI_DEBUG_ERROR, "\n    Onlined socket is unreachable. Appending the Hot Added socket to SBSP Min Path Tree failed.\n    Operation Aborted!!!"));
    return RAS_MPLINK_FAILURE_SOC_UNREACHABLE;
  }

  Child.RawNode = 0;
  Child.Node.Valid = TRUE;
  Child.Node.SocId = mPendingSocId;
  Child.Node.SocType = mPendingSocType;
  //
  // One level down from parent:
  //
  Child.Node.Hop = Parent.Node.Hop + 1;
  //
  // Remember who is the immediate parent of this child:
  //
  Child.Node.ParentIndex = Index;
  Child.Node.ParentPort = mRasSocketData->Cpu[Parent.Node.SocId].LinkData[KtiLink].PeerPort;

  //
  // Add the node to the tree
  //
  KtiDebugPrintInfo0((mRasMpLink->Host, KTI_DEBUG_INFO0 , "\n    Adding Socket%u to SBSP Min Path tree", Child.Node.SocId));
  if (AddNodeToTopologyTree(mRasHost, mRasSocketData->SbspMinPathTree, &Child) != KTI_SUCCESS) {
    KtiDebugPrintFatal((mRasMpLink->Host, KTI_DEBUG_ERROR, "\n    Adding CPU%u to SBSP Min Path tree failed - Data Structure Overflow.",  Child.Node.SocId));
    return RAS_MPLINK_FAILURE_GENERAL;
  }

  SetupSbspConfigAccessPath(mRasHost, mRasSocketData, mKtiInternalGlobal, &Parent, &Child, KtiLink);
  MemCopy((VOID *)mRasSocketData->SbspMinPathTree,
                      (VOID *)mRasHost->var.kti.CpuInfo[Parent.Node.SocId].TopologyInfo,
                      sizeof(mRasHost->var.kti.CpuInfo[Parent.Node.SocId].TopologyInfo));

  //
  // Update onlining socket internal data
  //
  UpdateCpuInfoInKtiInternal(mRasHost, mKtiInternalGlobal, (UINT8)mPendingSocId);
  SetupRemoteCpuBootPath(mRasHost, mRasSocketData, mKtiInternalGlobal, &Parent, &Child, KtiLink);
  return RAS_MPLINK_SUCCESS;
} // SetConfigAndBootPath

/**

  Make sure the new socket's SEC code checked into Nonstickypad2[0].

  @retval RAS_MPLINK_SUCCESS - if new socket successfully checked in
  @retval RAS_MPLINK_FAILURE_CHKIN - on failure to check-in

**/
RAS_MPLINK_STATUS
VerifyCpuCheckIn (
   VOID
  )
{
  UINT32 NonStickyPad2;

  KtiDebugPrintInfo0 ((mRasHost, KTI_DEBUG_INFO0 , "\n    Verifying if the onlined CPU checked in."));
  NonStickyPad2 = ReadCpuPciCfgRas(mRasHost, (UINT8)mPendingSocId, 0, BIOSNONSTICKYSCRATCHPAD2_UBOX_MISC_REG);
  if (!(NonStickyPad2 & 0x1)) {
    KtiDebugPrintFatal ((mRasHost, KTI_DEBUG_ERROR, "\n    Onlined socket failed to check in."));
    return RAS_MPLINK_FAILURE_CHKIN;
  }

  KtiDebugPrintInfo0 ((mRasHost, KTI_DEBUG_INFO0, "\n    Onlined socket successfully checked in."));
  return RAS_MPLINK_SUCCESS;
} // VerifyCpuCheckIn

/**

  Instruct the Onlined CPU to tear down NEM and halt.

  @retval RAS_MPLINK_SUCCESS - on successful completion

**/
RAS_MPLINK_STATUS
HaltPbsp (
   VOID
  )
{
  WriteCpuPciCfgRas (mRasHost, (UINT8)mPendingSocId, 0, SR_MEMORY_DATA_STORAGE_DISPATCH_PIPE_CSR, (UINT32)-1);
  return RAS_MPLINK_SUCCESS;
} // HaltPbsp

/**

  Issue Warm Reset through a platform PLD command.

  @param TimeSliced - TRUE/FALSE indicating if we're in time-sliced mode

  @retval RAS_MPLINK_SUCCESS - on successful completion

**/
RAS_MPLINK_STATUS
IssueWarmResetOfOnlinedCpu (
  IN UINT8          TimeSliced
  )
{
  mHpIOXAccess->IssuePldCmdThroIox (ISSUE_WARM_RESET, (UINT8)mPendingSocId, TimeSliced);
  return RAS_MPLINK_SUCCESS;
} // IssueWarmResetOfOnlinedCpu

/**

  Routine to sync up the onlined socket, update the necessary scratch registers and issue "BOOT_GO"
  command to bring it under BIOS control.

  @param host              - platform host structure pointer
  @param SocketData        - info such as CPUs populated and their LEP
  @param KtiInternalGlobal - structure with KTIRC global variables
  @param RstType           - reset type - post power good cycle or post warm reset

  @retval RAS_MPLINK_SUCCESS - on successful completion

**/
RAS_MPLINK_STATUS
SyncUpPbspForRas (
  struct sysHost           *host,
  KTI_SOCKET_DATA          *SocketData,
  KTI_HOST_INTERNAL_GLOBAL *KtiInternalGlobal,
  UINT8                    RstType
  )
{
  UINT32                       Data32;
  SAD_TARGET_CHABC_SAD1_STRUCT SadTarget;
  LT_CONTROL_CHABC_SAD1_STRUCT LtControlSbsp;
  LT_CONTROL_CHABC_SAD1_STRUCT LtControlPending;
  CPUNODEID_UBOX_CFG_STRUCT    CpuNodeId;
  UINT32                       Sr07;

  if (RstType == POST_RESET_POWERGOOD) {
    //
    // Update the Ubox sticky scratch register 07:
    // [19:16]  - SBSP socket id
    //
    Sr07 = (UINT32) (KtiInternalGlobal->SbspSoc << 16);

    Data32 = ReadCpuPciCfgRas (host, (UINT8)mPendingSocId, 0, BIOSSCRATCHPAD7_UBOX_MISC_REG);
    Data32 = (Data32 & 0xFFF0FFFF) | Sr07;
    WriteCpuPciCfgRas (host, (UINT8)mPendingSocId, 0, BIOSSCRATCHPAD7_UBOX_MISC_REG, Data32);
    UpdateCpuList (FALSE);
  }

  //
  // BIOS has to initialize the following fields of SAD_TARGET_CHABC_SAD1 register for the new socket.
  //
  SadTarget.Data = ReadCpuPciCfgRas (host, (UINT8)mPendingSocId, 0, SAD_TARGET_CHABC_SAD1_REG);
  SadTarget.Bits.flash_target      = KtiInternalGlobal->SbspSoc;
  SadTarget.Bits.legacy_pch_target = KtiInternalGlobal->SbspSoc;
  WriteCpuPciCfgRas (host, (UINT8)mPendingSocId, 0, SAD_TARGET_CHABC_SAD1_REG, SadTarget.Data);

  //
  // BIOS has to initialize the LT_CONTROL register for the new socket.
  // So we just replicate the register setting from SBSP to the new socket.
  //
  LtControlSbsp.Data = ReadCpuPciCfgRas (host, KtiInternalGlobal->SbspSoc, 0, LT_CONTROL_CHABC_SAD1_REG);
  if (LtControlSbsp.Bits.lt_target_enable & 0x1) { // LT_Target_enable is set?
    LtControlPending.Data = ReadCpuPciCfgRas (host, (UINT8)mPendingSocId, 0, LT_CONTROL_CHABC_SAD1_REG);
    LtControlPending.Bits.lt_target_enable = LtControlSbsp.Bits.lt_target_enable;
    LtControlPending.Bits.lt_target        = LtControlSbsp.Bits.lt_target;
    WriteCpuPciCfgRas (host, (UINT8)mPendingSocId, 0, LT_CONTROL_CHABC_SAD1_REG, LtControlPending.Data);
  }

  //
  // In LT enabled system, releasing the new socket may cause a LT_SHUTDOWN, which causes a surprise warm reset.
  // Set bit29 of SP7 to indicate we are releasing the onlined socket, so that if LT_SHUTDOWN happens,
  // BIOS can detect this and handle it.
  // Also, set bit 6 - hot add flag.
  //
  Sr07 = ReadCpuPciCfgRas (host, (UINT8)mPendingSocId, 0, BIOSSCRATCHPAD7_UBOX_MISC_REG);
  Sr07 = Sr07 | (1 << 29) | (1 << 6);
  WriteCpuPciCfgRas (host, (UINT8)mPendingSocId, 0, BIOSSCRATCHPAD7_UBOX_MISC_REG, Sr07);

  //
  // Local Node ID in UBOX
  //
  CpuNodeId.Data = ReadCpuPciCfgRas (host, (UINT8)mPendingSocId, 0, CPUNODEID_UBOX_CFG_REG);
  CpuNodeId.Bits.lclnodeid = mPendingSocId;
  WriteCpuPciCfgRas (host, (UINT8)mPendingSocId, 0, CPUNODEID_UBOX_CFG_REG, CpuNodeId.Data);

  //
  // Issue BOOT_GO command
  //
  KtiDebugPrintInfo0 ((mRasMpLink->Host, KTI_DEBUG_INFO0, "\n    Issuing BOOT_GO command to CPU%u. ", mPendingSocId));
  Data32 = ReadCpuPciCfgRas (host, (UINT8)mPendingSocId, 0, BIOSNONSTICKYSCRATCHPAD1_UBOX_MISC_REG);
  Data32 = (Data32 & 0xFFFFFF00) | 0x2;
  WriteCpuPciCfgRas (host, (UINT8)mPendingSocId, 0, BIOSNONSTICKYSCRATCHPAD1_UBOX_MISC_REG, Data32);

  return RAS_MPLINK_SUCCESS;
} // SyncUpPbspForRas

RAS_MPLINK_STATUS
UpdateCpuList (
   IN BOOLEAN AllCpus
  )
{
  UINT8                           Index, Port;
  UINT32                          Sr02;
  MMCFG_RULE_N0_CHABC_SAD1_STRUCT MmcfgRule0;

  //
  // Update the Sr02
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
  for (Index = 0; Index < MAX_SOCKET; ++Index) {
    if (mRasSocketData->Cpu[Index].Valid == TRUE) {
      //
      // bit[7:0] used for CPU list
      //
      Sr02 |= (1 << Index); 
      for (Port = 0; Port < mRasHost->var.common.KtiPortCnt; ++Port) {
          if (mRasSocketData->Cpu[Index].LinkData[Port].Valid == TRUE) {
            Sr02 = Sr02 | (1 << (((Index * SI_MAX_KTI_PORTS) + Port) + 8));
          }
      }
    }
  }

  if (AllCpus == TRUE) {
    //
    // Update it on all CPUs populated
    //
    MmcfgRule0.Data = ReadCpuPciCfgRas(mRasHost, (UINT8)mSbspId, 0, MMCFG_RULE_N0_CHABC_SAD1_REG);
    for (Index = 0; Index < MAX_SOCKET; ++Index) {
      if (mRasSocketData->Cpu[Index].Valid == TRUE){
        WriteCpuPciCfgRas(mRasHost, (UINT8)Index, 0, BIOSSCRATCHPAD2_UBOX_MISC_REG, Sr02);

        //
        // Update MMCFG_RULE0.valid_targets on all sockets but SBSP
        //
        if (Index != (UINT8)mSbspId) {
          WriteCpuPciCfgRas (mRasHost, Index, 0, MMCFG_RULE_N0_CHABC_SAD1_REG, MmcfgRule0.Data);
        }
      }
    }
  } else {
    //
    // Only on Onlined CPU socket
    //
    WriteCpuPciCfgRas(mRasHost, (UINT8)mPendingSocId, 0, BIOSSCRATCHPAD2_UBOX_MISC_REG, Sr02);
  }

  return RAS_MPLINK_SUCCESS;
}

/**

  Program the new socket's DRAM decoder registers to add the pre-existing
  sockets' DRAM to its memory map, thus allowing threads on new socket to
  reach the pre-existing sockets' memory.

  @retval RAS_MPLINK_SUCCESS - on successful completion

**/
RAS_MPLINK_STATUS
SyncUpDramSad (
  VOID
  )
{
  //
  // SKX_TODO: This should be designed into the Memory Map flow for Onlining
  //
#if 0
  UINT8                               i;
  UINT32                              dramRuleBaseAddr;
  UINT32                              interleaveListBaseAddr;

  DRAM_RULE_0_CBOBC_MEMSAD_STRUCT     cboDramRule;

  UINT32 Data32;

  //
  // Replicate the SBSP's DRAM decoder setting into the onlined CPU socket
  //
  dramRuleBaseAddr        = GetPciCfgOffset(mRasHost, DRAM_RULE_CBOBC_MEMSAD_REG);
  interleaveListBaseAddr  = GetPciCfgOffset(mRasHost, INTERLEAVE_LIST_CBOBC_MEMSAD_REG);

  // Replicate the DRAM decoders starting with entry 0.
  for (i = 0; i < DRAM_SAD_MAX; i++) {
    // Read SBSP's DRAM decoder entries
    cboDramRule.Data = ReadCpuPciCfgRas(mRasHost, (UINT8)mSbspId, 0, dramRuleBaseAddr + i*8 );
    WriteCpuPciCfgRas(mRasHost, (UINT8)mPendingSocId, 0, dramRuleBaseAddr + i*8 , cboDramRule.Data);

    // The DRAM entry with NXM attribute should point to the local Ubox.
    if ( cboDramRule.Bits.attr == 2){
      Data32 =  mPendingSocId * 0x11111111;
    } else {
      Data32 = ReadCpuPciCfgRas(mRasHost, (UINT8)mSbspId, 0, interleaveListBaseAddr + i*8 );
    }
    WriteCpuPciCfgRas(mRasHost, (UINT8)mPendingSocId, 0, interleaveListBaseAddr + i*8, Data32);
  }

  Data32 = ReadCpuPciCfgRas(mRasHost, (UINT8)mSbspId, 0, TOLM_IIO_VTD_REG);
  WriteCpuPciCfgRas (mRasHost, (UINT8)mPendingSocId, 0, TOLM_IIO_VTD_REG, Data32);

  Data32 = ReadCpuPciCfgRas(mRasHost, (UINT8)mSbspId, 0, TOHM_0_IIO_VTD_REG);
  WriteCpuPciCfgRas (mRasHost, (UINT8)mPendingSocId, 0, TOHM_0_IIO_VTD_REG, Data32);
  Data32 = ReadCpuPciCfgRas(mRasHost, (UINT8)mSbspId, 0, TOHM_1_IIO_VTD_REG);
  WriteCpuPciCfgRas (mRasHost, (UINT8)mPendingSocId, 0, TOHM_1_IIO_VTD_REG, Data32);

  Data32 = ReadCpuPciCfgRas(mRasHost, (UINT8)mSbspId, 0, TOLM_CBOBC_COH_REG);
  WriteCpuPciCfgRas (mRasHost, (UINT8)mPendingSocId, 0, TOLM_CBOBC_COH_REG, Data32);
  Data32 = ReadCpuPciCfgRas(mRasHost, (UINT8)mSbspId, 0, TOHM_CBOBC_COH_REG);
  WriteCpuPciCfgRas (mRasHost, (UINT8)mPendingSocId, 0, TOHM_CBOBC_COH_REG, Data32);

  WriteCpuPciCfgRas (mRasHost, (UINT8)mPendingSocId, 0, MCMTR_MC_MAIN_REG, 0x2000);          //forus to vmse 1:1 lock step
  WriteCpuPciCfgRas (mRasHost, (UINT8)mPendingSocId, 1, MCMTR_MC_MAIN_REG, 0x2000);
#endif
  return RAS_MPLINK_SUCCESS;
}

/**

  Include the Onlined socket in System Interrupt Domain

  @retval RAS_MPLINK_SUCCESS - on successful completion

**/
RAS_MPLINK_STATUS
AdjustIntDomainForHotAdd (
   VOID
  )
{
  UINT8           Idx1, Misclist=0, CpuList=0, IpiList=0;
  UINT32          Data32 = 0;

  KtiDebugPrintInfo1 ((mRasHost, KTI_DEBUG_INFO1, "\n    Adjusting the System Interrupt Domain to comprehend the Onlined CPU socket."));

  for (Idx1 = 0; Idx1 < MAX_SOCKET; ++Idx1) {
    if (mRasSocketData->Cpu[Idx1].Valid == TRUE){
      CpuList |= (1 << Idx1);
      Misclist |= (1<< Idx1);
    }
  }

  //
  // For all the existing sockets, IPI broadcast has to exclude the online socket during milestone12.
  // For the online socket, make it to have only itself for start up of its ap threads incl. smm relocation.
  // Later, all will be set to be included all from subsequent calls by cpu ras code.
  //
  IpiList = CpuList & (~(1 << mPendingSocId));
  for (Idx1 = 0; Idx1 < MAX_SOCKET; ++Idx1) {
    if (mRasSocketData->Cpu[Idx1].Valid == TRUE && Idx1 != mPendingSocId ){
      Data32 = (CpuList << 24) | (Misclist << 16) | (CpuList << 8) | IpiList;
      WriteCpuPciCfgRas (mRasHost, Idx1, 0, BCASTLIST_UBOX_CFG_REG, Data32);
      KtiDebugPrintInfo1 ((mRasHost, KTI_DEBUG_INFO1, "\n    Cpu%u BCASTLIST_UBOX_CFG_REG = %x", Idx1, Data32 ));
    }
  }

  Data32 = (Data32 & 0xFFFFFF00) | (1 << mPendingSocId);
  WriteCpuPciCfgRas (mRasHost, (UINT8)mPendingSocId, 0, BCASTLIST_UBOX_CFG_REG, Data32);

  return RAS_MPLINK_SUCCESS;
} // AdjustIntDomainForHotAdd

/**

  In order to prepare the links for Slow mode training, on all invalid links:
    1. Set c_init_begin
    2. Clear fake_l1
    3. Issue phy reset via cp_reset

  @param SocketData - SocketData struct with information about the system

  @retval RAS_MPLINK_SUCCESS - on successful completion

**/
RAS_MPLINK_STATUS
PrepareForSlowSpeedTraining (
  IN KTI_SOCKET_DATA        *SocketData
)
{
  UINT8                           Socket, Port;
  KTIREUT_PH_CTR1_KTI_REUT_STRUCT KtiReutPhCtr1;
  KTIPHYPWRCTRL_KTI_REUT_STRUCT   KtiPhyPwrCtrl;

  for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
    if (mRasHost->var.kti.CpuInfo[Socket].Valid == TRUE) {
      for (Port = 0; Port < MAX_KTI_PORTS; ++Port) {
        if (mRasHost->var.kti.CpuInfo[Socket].LepInfo[Port].Valid == FALSE) {
          //
          // 1. Set c_init_begin
          //
          KtiReutPhCtr1.Data = ReadCpuPciCfgRas (mRasHost, Socket, Port, KTIREUT_PH_CTR1_KTI_REUT_REG);
          KtiReutPhCtr1.Bits.c_init_begin = 1;
          WriteCpuPciCfgRas (mRasHost, Socket, Port, KTIREUT_PH_CTR1_KTI_REUT_REG, KtiReutPhCtr1.Data);

          //
          // 2. Clear fake_l1
          //
          KtiPhyPwrCtrl.Data = ReadCpuPciCfgRas (mRasHost, Socket, Port, KTIPHYPWRCTRL_KTI_REUT_REG);
          KtiPhyPwrCtrl.Bits.fake_l1 = 0;
          WriteCpuPciCfgRas (mRasHost, Socket, Port, KTIPHYPWRCTRL_KTI_REUT_REG, KtiPhyPwrCtrl.Data);

          //
          // 3. Issue phy reset via cp_reset
          //
          KtiReutPhCtr1.Data = ReadCpuPciCfgRas (mRasHost, Socket, Port, KTIREUT_PH_CTR1_KTI_REUT_REG);
          KtiReutPhCtr1.Bits.cp_reset = 1;
          WriteCpuPciCfgRas (mRasHost, Socket, Port, KTIREUT_PH_CTR1_KTI_REUT_REG, KtiReutPhCtr1.Data);
        }
      }
    }
  }

  return RAS_MPLINK_SUCCESS;
} // PrepareForSlowSpeedTraining

/**

  Clear KtiReutPhCtr1.c_init_begin on all invalid KTI ports.

  @param SocketData - SocketData struct with information about the system

  @retval RAS_MPLINK_SUCCESS - on successful completion

**/
RAS_MPLINK_STATUS
ClearCinitbeginOnInvalidLinks (
  IN KTI_SOCKET_DATA        *SocketData
  )
{
  UINT8                           KtiLink, Socket;
  KTIREUT_PH_CTR1_KTI_REUT_STRUCT KtiReutPhCtr1;

  for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
    if (SocketData->Cpu[Socket].Valid == TRUE) {
      for (KtiLink = 0; KtiLink < MAX_KTI_PORTS; ++KtiLink) {
        if (SocketData->Cpu[Socket].LinkData[KtiLink].Valid != TRUE) {
          KtiReutPhCtr1.Data = ReadCpuPciCfgRas (mRasHost, Socket, KtiLink, KTIREUT_PH_CTR1_KTI_REUT_REG);
          KtiReutPhCtr1.Bits.c_init_begin = 0;
          WriteCpuPciCfgRas (mRasHost, Socket, KtiLink, KTIREUT_PH_CTR1_KTI_REUT_REG, KtiReutPhCtr1.Data);
        }
      }
    }
  }
  return RAS_MPLINK_SUCCESS;
} // ClearCinitbeginOnInvalidLinks

KTI_STATUS
KtiFullSpeedTransitionForHotAdd (
  VOID
  )
{
  UINT8         LinkIndex;
  UINT32        LinkSpeed;
  KTI_LINK_DATA *pPeerInfo;  

  //
  // Make sure per-link setup options don't conflict between the peer sockets
  //
  NormalizeLinkOptions (mRasHost, mRasSocketData, mKtiInternalGlobal);

  ClearOrSetKtiDfxLckForFullSpeedTransition (mRasHost, 0);
  ProgramLateActionForHotAdd (1);
  ProgramPhyLayerForHotAdd ();
  ProgramLinkLayerForHotAdd ();
  ProgramLateActionForHotAdd (0);
  ClearOrSetKtiDfxLckForFullSpeedTransition (mRasHost, 1);

  //
  // KTI Link Speed is selected during boot flow in SelectSupportedKtiLinkSpeed
  // and is stored in host->var.kti.OutKtiLinkSpeed and 
  // setup->kti.PhyLinkPerPortSetting[Index].Phy[LinkIndex].KtiLinkSpeed
  //
  switch (mRasHost->var.kti.OutKtiLinkSpeed) {
    case (SPEED_REC_96GT):
      LinkSpeed = 12;
      break;
    case (SPEED_REC_104GT):
      LinkSpeed = 13;
      break;
    default:
      LinkSpeed = 12;
      break;
  }
  
  SendMailBoxCmdToPcode(mRasHost, (UINT8) mPendingSocId, MAILBOX_BIOS_CMD_SET_KTI_FREQ, LinkSpeed);
  SendMailBoxCmdToPcode(mRasHost, (UINT8) mPendingSocId, MAILBOX_BIOS_CMD_SET_KTI_FREQ |(0x2 << 8), LinkSpeed);

  //
  // Prepare pre-existing sockets' KTI ports for training to op speed
  //
  for (LinkIndex = 0; LinkIndex < mRasHost->var.common.KtiPortCnt; LinkIndex++) {
    if (mRasSocketData->Cpu[mPendingSocId].LinkData[LinkIndex].Valid == TRUE) {
      pPeerInfo = &(mRasSocketData->Cpu[mPendingSocId].LinkData[LinkIndex]);
      ReceivingKtiPortsFullSpeedTransition (mRasHost, mKtiInternalGlobal, (UINT8)pPeerInfo->PeerSocId, (UINT8)pPeerInfo->PeerPort);
    }
  }

  return RAS_MPLINK_SUCCESS;
} // KtiFullSpeedTransitionForHotAdd

/**

 Sequence of setting bits and issuing phy/link layer resets that prepares
 the pre-existing sockets' KTI ports for op speed training.

 @param host              - pointer to the system host root structure
 @param KtiInternalGlobal - KTIRC internal variables.
 @param Socket            - Socket ID
 @param LinkIndex         - Link number

 @retval RAS_MPLINK_SUCCESS - on successful completion

**/
KTI_STATUS
ReceivingKtiPortsFullSpeedTransition (
  struct sysHost           *host,
  KTI_HOST_INTERNAL_GLOBAL *KtiInternalGlobal,
  UINT8                    Socket,
  UINT8                    LinkIndex
  )
{
  KTIREUT_PH_CTR1_KTI_REUT_STRUCT  KtiReutPhCtr1;
  KTIREUT_PH_CIS_KTI_REUT_STRUCT   KtiReutPhCis;
  KTILCL_KTI_LLPMON_STRUCT         KtiLcl;
  KTIREUT_PH_CSS_KTI_REUT_STRUCT   KtiReutPhCss;
  KTILKPRIVC0_KTI_CIOPHYDFX_STRUCT KtiLkPrivc0;

  //
  // 1.	BIOS must clear KTIREUT.PH_CIS.sp_rx_calib on the host socket.
  //
  KtiReutPhCis.Data = KtiReadPciCfg (host, Socket, LinkIndex, KTIREUT_PH_CIS_KTI_REUT_REG);
  KtiReutPhCis.Bits.sp_rx_calib = 0;
  KtiWritePciCfg (host, Socket, LinkIndex, KTIREUT_PH_CIS_KTI_REUT_REG, KtiReutPhCis.Data);

  //
  // 2.	BIOS clears cinitbegin on host socket.
  //    This breaks the communication w/ new socket.
  //
  KtiReutPhCtr1.Data = KtiReadPciCfg (host, Socket, LinkIndex, KTIREUT_PH_CTR1_KTI_REUT_REG);
  KtiReutPhCtr1.Bits.c_init_begin = 0;
  KtiWritePciCfg (host, Socket, LinkIndex, KTIREUT_PH_CTR1_KTI_REUT_REG, KtiReutPhCtr1.Data);

  //
  // 3.	BIOS issues phy layer reset via cpreset on the host socket.
  //
  KtiReutPhCtr1.Data = KtiReadPciCfg (host, Socket, LinkIndex, KTIREUT_PH_CTR1_KTI_REUT_REG);
  KtiReutPhCtr1.Bits.cp_reset = 1;
  KtiWritePciCfg (host, Socket, LinkIndex, KTIREUT_PH_CTR1_KTI_REUT_REG, KtiReutPhCtr1.Data);

  //
  // 4. BIOS verifies that the host socket has entered reset by verifying that
  //    PH_CSS.s_tx_state is in ResetC state (encoding 0b00010 according to Table 12-26
  //    of the KTI Spec).
  //
    do {
      KtiReutPhCss.Data = KtiReadPciCfg (host, Socket, LinkIndex, KTIREUT_PH_CSS_KTI_REUT_REG);
    } while (KtiReutPhCss.Bits.s_tx_state != 0x2);

  //
  // 4929779: Cloned From SKX Si Bug Eco: KTI Link Layer not sending LL_INIT after LL Reset
  // BIOS hot-plug flow can program the clock gate disable prior to LL reset, then clear it
  // after hot-plug is done.
  //
  KtiLkPrivc0.Data = KtiReadPciCfg (host, Socket, LinkIndex, KTILKPRIVC0_KTI_CIOPHYDFX_REG);
  KtiLkPrivc0.Bits.ctl_ck_gate_dis = 1;
  KtiWritePciCfg (host, Socket, LinkIndex, KTILKPRIVC0_KTI_CIOPHYDFX_REG, KtiLkPrivc0.Data);
  
  //
  // 5. BIOS issues link layer reset on the host socket by setting KTILCL.LinkLayerReset to 1.
  //
  KtiLcl.Data = KtiReadPciCfg (host, Socket, LinkIndex, KTILCL_KTI_LLPMON_REG);
  KtiLcl.Bits.link_layer_reset = 1;
  KtiWritePciCfg (host, Socket, LinkIndex, KTILCL_KTI_LLPMON_REG, KtiLcl.Data);
  
  //
  // 6. Poll for hardware to clear this register back to 0.
  //
  do {
      KtiLcl.Data = KtiReadPciCfg (host, Socket, LinkIndex, KTILCL_KTI_LLPMON_REG);
  } while (KtiLcl.Bits.link_layer_reset != 0);

  return RAS_MPLINK_SUCCESS;
} // ReceivingKtiPortsFullSpeedTransition

/**

 Set cinitbegin on host sockets to initiate op speed training.

 @param host              - pointer to the system host root structure
 @param KtiInternalGlobal - KTIRC internal variables.

 @retval RAS_MPLINK_SUCCESS - on successful completion

**/
KTI_STATUS
ReceivingKtiPortsSetCinitbegin (
  struct sysHost           *host,
  KTI_HOST_INTERNAL_GLOBAL *KtiInternalGlobal
  )
{
  UINT8                           LinkIndex;
  KTI_LINK_DATA                   *pPeerInfo;
  KTIREUT_PH_CTR1_KTI_REUT_STRUCT KtiReutPhCtr1;

  for (LinkIndex = 0; LinkIndex < mRasHost->var.common.KtiPortCnt; LinkIndex++) {
    if (mRasSocketData->Cpu[mPendingSocId].LinkData[LinkIndex].Valid == TRUE) {
      pPeerInfo = &(mRasSocketData->Cpu[mPendingSocId].LinkData[LinkIndex]);
      KtiReutPhCtr1.Data = KtiReadPciCfg (host, (UINT8)pPeerInfo->PeerSocId, (UINT8)pPeerInfo->PeerPort, KTIREUT_PH_CTR1_KTI_REUT_REG);
      KtiReutPhCtr1.Bits.c_init_begin = 1;
      KtiWritePciCfg (host, (UINT8)pPeerInfo->PeerSocId, (UINT8)pPeerInfo->PeerPort, KTIREUT_PH_CTR1_KTI_REUT_REG, KtiReutPhCtr1.Data);
    }
  }

  return RAS_MPLINK_SUCCESS;
}

/**

 Re-enable Link Layer Clock Gating on host sockets.

 @param host              - pointer to the system host root structure
 @param KtiInternalGlobal - KTIRC internal variables.

 @retval RAS_MPLINK_SUCCESS - on successful completion

**/
KTI_STATUS
ReceivingKtiPortsReenableLinkLayerClockGating (
  struct sysHost           *host
  )
{
  UINT8                            LinkIndex;
  KTI_LINK_DATA                    *pPeerInfo;
  KTILKPRIVC0_KTI_CIOPHYDFX_STRUCT KtiLkPrivc0;

  for (LinkIndex = 0; LinkIndex < mRasHost->var.common.KtiPortCnt; LinkIndex++) {
    if (mRasSocketData->Cpu[mPendingSocId].LinkData[LinkIndex].Valid == TRUE) {
      pPeerInfo = &(mRasSocketData->Cpu[mPendingSocId].LinkData[LinkIndex]);
      KtiLkPrivc0.Data = KtiReadPciCfg (host, (UINT8)pPeerInfo->PeerSocId, (UINT8)pPeerInfo->PeerPort, KTILKPRIVC0_KTI_CIOPHYDFX_REG);
      KtiLkPrivc0.Bits.ctl_ck_gate_dis = 0;
      KtiWritePciCfg (host, (UINT8)pPeerInfo->PeerSocId, (UINT8)pPeerInfo->PeerPort, KTILKPRIVC0_KTI_CIOPHYDFX_REG, KtiLkPrivc0.Data);
    }
  }

  return RAS_MPLINK_SUCCESS;
}

KTI_STATUS
MemSpeedModeTransitionForHotAdd (
  VOID
  )
{
  UINT32      Status;
  UINT32      mailboxData;
  UINT32      mcBiosReq;
  UINT8       req_data;
  UINT8       req_type;
  UINT8       maxFreq;

  if(mRasHost->var.mem.socket[mSbspId].ddrFreqCheckFlag == 0) {   //Ckeck if it is the first time
    Status = WriteBios2PcuMailboxCommand(mRasHost, mSbspId, MAILBOX_BIOS_CMD_READ_MC_FREQ, 0x0); //get DCLK from SBSP socket
    if (Status == 0) {  // success
      mailboxData = ReadCpuPciCfgEx (mRasHost, mSbspId, 0, BIOS_MAILBOX_DATA);
      req_data = (UINT8) mailboxData & 0x3F;
      req_type = (UINT8) ((mailboxData & 0x40) >> 6);
      maxFreq   = (UINT8) ((mailboxData & 0x1F80) >> 7);
      mRasHost->var.mem.socket[mSbspId].ddrFreqCheckFlag = 1;
      mRasHost->var.mem.socket[mSbspId].ddrClkData = req_data;
      mRasHost->var.mem.socket[mSbspId].ddrClkType = req_type;
      mRasHost->var.mem.socket[mSbspId].maxFreq    = maxFreq;
    } else {
      return Status;
    }
  } else {
    req_data = mRasHost->var.mem.socket[mSbspId].ddrClkData;
    req_type = mRasHost->var.mem.socket[mSbspId].ddrClkType;
    maxFreq  = mRasHost->var.mem.socket[mSbspId].maxFreq;
  }

  mcBiosReq = ((req_type << 0x6) | req_data);
  WriteBios2PcuMailboxCommand(mRasHost, (UINT8)mPendingSocId, MAILBOX_BIOS_CMD_SET_MC_FREQ, mcBiosReq);  //set DCLK ratio to incoming socket

  return RAS_MPLINK_SUCCESS;
}

KTI_STATUS
ProgramLateActionForHotAdd (
  IN UINT8 Value
  )
{
  UINT8                         Idx2;
  KTI_LINK_DATA                 *pPeerInfo;

  for (Idx2 = 0; Idx2 < mRasHost->var.common.KtiPortCnt; Idx2++) {
    ProgramLateActionOnCpuLink (mRasHost, (UINT8)mPendingSocId, Idx2, Value);
    pPeerInfo = &(mRasSocketData->Cpu[mPendingSocId].LinkData[Idx2]);
    ProgramLateActionOnCpuLink (mRasHost, (UINT8)pPeerInfo->PeerSocId, (UINT8)pPeerInfo->PeerPort, Value );
  }
  return RAS_MPLINK_SUCCESS;
}

RAS_MPLINK_STATUS
ProgramPhyLayerForHotAdd (
  VOID
  )
{
  UINT8         Idx2;
  KTI_LINK_DATA *pPeerInfo;

  //
  // Program the PHY layer settings for RAS CPU's valid links
  //
  KtiDebugPrintInfo1((mRasHost, KTI_DEBUG_INFO1, "\n    Programming Phy layer for Online event. "));

  for (Idx2 = 0; Idx2 < mRasHost->var.common.KtiPortCnt; Idx2++) {
    if (mRasSocketData->Cpu[mPendingSocId].LinkData[Idx2].Valid == TRUE){
      pPeerInfo = &(mRasSocketData->Cpu[mPendingSocId].LinkData[Idx2]);
      //
      // Program Electrical Parameters (board-layout-dependent)
      //
      ProgramEparams (mRasHost, (UINT8)mPendingSocId, Idx2, KTI_GROUP);
      ProgramEparams (mRasHost, (UINT8)pPeerInfo->PeerSocId, (UINT8)pPeerInfo->PeerPort, KTI_GROUP);
      //
      // Program CTR1 and LDC registers
      //
      ProgramPhyLayerControlRegisters (mRasHost, (UINT8)mPendingSocId, Idx2, KTI_GROUP);
      ProgramPhyLayerControlRegisters (mRasHost, (UINT8)pPeerInfo->PeerSocId, (UINT8)pPeerInfo->PeerPort, KTI_GROUP);
      //
      // Program the UniPhy recipe (independent of board layout)
      //
      KtiEvAutoRecipe (mRasHost, (UINT8)mPendingSocId, Idx2);
      KtiEvAutoRecipe (mRasHost, (UINT8)pPeerInfo->PeerSocId, (UINT8)pPeerInfo->PeerPort);
    }
  }

  //
  // Clear cinitbegin on invalid ports
  //
  ClearCinitbeginOnInvalidLinks (mRasSocketData);
  return RAS_MPLINK_SUCCESS;
} // ProgramPhyLayerForHotAdd

/**

 Program Link Layer registers for the new links
 
 @retval RAS_MPLINK_SUCCESS - on successful completion 

**/
RAS_MPLINK_STATUS
ProgramLinkLayerForHotAdd (
  VOID
  )
{
  UINT8         LinkIndex, Socket, M3Kti;
  KTI_LINK_DATA *pPeerInfo;
  UINT32        VnaCredit;

  KtiDebugPrintInfo1((mRasMpLink->Host, KTI_DEBUG_INFO1, "\n    Programming Link layer for Online event. "));
  
  if (mRasHost->var.kti.OutVn1En == TRUE) {
    VnaCredit = KTI_LL_VNA_WITH_VN1_SKX;
  } else {
    VnaCredit = KTI_LL_VNA_NORMAL_SKX;
  }

  for (LinkIndex = 0; LinkIndex < mRasHost->var.common.KtiPortCnt; LinkIndex++) {
    if (mRasSocketData->Cpu[mPendingSocId].LinkData[LinkIndex].Valid == TRUE) {
      ProgramLinkLayerControlRegisters (mRasHost, mKtiInternalGlobal, (UINT8) mPendingSocId, LinkIndex, KTI_GROUP);
      pPeerInfo = &(mRasSocketData->Cpu[mPendingSocId].LinkData[LinkIndex]);
      ProgramLinkLayerControlRegisters (mRasHost, mKtiInternalGlobal, (UINT8) pPeerInfo->PeerSocId, (UINT8) pPeerInfo->PeerPort, KTI_GROUP);
    }
  }
  
  for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
    if ((mRasHost->var.common.socketPresentBitMap & (BIT0 << Socket)) != 0) {
      for (M3Kti = 0; M3Kti < mRasHost->var.kti.CpuInfo[Socket].TotM3Kti; M3Kti++) {
        ProgramLinkLayerOnM3Kti (mRasHost, mKtiInternalGlobal, Socket, M3Kti, VnaCredit);
      }
    }
  }

 return RAS_MPLINK_SUCCESS;
}

/**

  Make sure none of the links of the hot added socket fail to train after the
  warm reset.

  @retval RAS_MPLINK_SUCCESS         - links successfully trained
  @retval RAS_MPLINK_FAILURE_GENERAL - one of the links failed to train

**/
KTI_STATUS
VerifyLinkStatusAfterWarmReset (
  VOID
  )
{
  UINT8                      Port;
  KTIPCSTS_KTI_LLPMON_STRUCT PcSts;

  //
  // The route tables are setup only during the first warm reset of the hot added CPU. During subsequent reset
  // the route is not setup. So
  //
  KtiDebugPrintInfo1 ((mRasHost, KTI_DEBUG_INFO1, "\n    Verifying if all links of the hot added CPU reached Normal Operation after warm reset: "));

  for (Port = 0; Port < mRasHost->var.common.KtiPortCnt; Port++) {
    if (mRasSocketData->Cpu[mPendingSocId].LinkData[Port].Valid == TRUE) {
      PcSts.Data = ReadCpuPciCfgRas (mRasHost, (UINT8)mPendingSocId, Port, KTIPCSTS_KTI_LLPMON_REG);
      if (PcSts.Bits.ll_status  != LINK_SM_INIT_DONE) {
        KtiDebugPrintInfo1 ((mRasHost, KTI_DEBUG_INFO1, "Failure"));
        return RAS_MPLINK_FAILURE_GENERAL;
      }
    }
  }

 KtiDebugPrintInfo1 ((mRasHost, KTI_DEBUG_INFO1, "Success"));
 return RAS_MPLINK_SUCCESS;
}

