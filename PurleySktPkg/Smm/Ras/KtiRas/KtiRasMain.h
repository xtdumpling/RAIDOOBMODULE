//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//
/*++

Copyright (c) 2008 - 2014 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  KtiRasMain.h

Abstract:

--*/

#ifndef _CSI_RAS_MAIN_H
#define _CSI_RAS_MAIN_H

RAS_MPLINK_STATUS
ValidateParameters (
  IN UINT8 EventType,
  IN INT32 SocketType,
  IN INT32 SocketId
  );

RAS_MPLINK_STATUS
PowerUpOnlinedSocket (
  VOID
  );

RAS_MPLINK_STATUS
ReleaseResetOnlinedSocket (
  VOID
  );

RAS_MPLINK_STATUS
PowerDownOnlinedSocket (
  IN INT32            SocketId,
  IN KTI_SOCKET_DATA  *RasSocketData
  );

RAS_MPLINK_STATUS
PrepareHostDataStruct (
  VOID
  );

RAS_MPLINK_STATUS
DataDumpRasMpLink (
  VOID
  );

RAS_MPLINK_STATUS
OpenSbspConfigSpaceForHotAdd (
  VOID
  );

RAS_MPLINK_STATUS
AdjustSbspConfigSpaceForHotRemoval (
  VOID
  );

RAS_MPLINK_STATUS
CollectLepForOnlineEvent (
  VOID
  );

RAS_MPLINK_STATUS
CollectLepForOfflineEvent (
  VOID
  );

RAS_MPLINK_STATUS
CopyTopologyInfoToHostParm (
  VOID
  );

RAS_MPLINK_STATUS
SetConfigAndBootPath (
  VOID
  );

RAS_MPLINK_STATUS
UpdateCpuList (
  IN BOOLEAN AllCpus
  );

RAS_MPLINK_STATUS
VerifyCpuCheckIn (
  VOID
  );

RAS_MPLINK_STATUS
HaltPbsp (
  VOID
  );

RAS_MPLINK_STATUS
IssueWarmResetOfOnlinedCpu (
  IN UINT8 IssueOnlineCmd
  );

RAS_MPLINK_STATUS
SyncUpPbspForRas (
  SYSHOST                  *host,
  KTI_SOCKET_DATA          *SocketData,
  KTI_HOST_INTERNAL_GLOBAL *KtiInternalGlobal,
  UINT8                    RstType
  );

RAS_MPLINK_STATUS
SyncUpDramSad (
  VOID
  );

RAS_MPLINK_STATUS
AdjustIntDomainForHotAdd (
  VOID
  );

RAS_MPLINK_STATUS
PrepareForSlowSpeedTraining (
  IN KTI_SOCKET_DATA *SocketData
  );

KTI_STATUS
KtiFullSpeedTransitionForHotAdd (
  VOID
  );

KTI_STATUS
MemSpeedModeTransitionForHotAdd (
  VOID
  );

RAS_MPLINK_STATUS
KtiPhyLayerFullSpeedTransitionForCpuHotAdd (
  VOID
  );

KTI_STATUS
ProgramLateActionForHotAdd (
  IN UINT8 Value
  );

RAS_MPLINK_STATUS
ProgramUnlockDfx (
 );

RAS_MPLINK_STATUS
ProgramPhyLayerForHotAdd (
  VOID
  );

RAS_MPLINK_STATUS
ProgramLinkLayerForHotAdd (
  VOID
  );

KTI_STATUS
VerifyLinkStatusAfterWarmReset (
  VOID
  );

RAS_MPLINK_STATUS
SetupSbspConfigAccessPath (
  SYSHOST                  *host,
  KTI_SOCKET_DATA          *SocketData,
  KTI_HOST_INTERNAL_GLOBAL *KtiInternalGlobal,
  TOPOLOGY_TREE_NODE       *Interm,
  TOPOLOGY_TREE_NODE       *Dest,
  UINT8                    DestPort
  );

RAS_MPLINK_STATUS
SetupRemoteCpuBootPath (
  SYSHOST                  *host,
  KTI_SOCKET_DATA          *SocketData,
  KTI_HOST_INTERNAL_GLOBAL *KtiInternalGlobal,
  TOPOLOGY_TREE_NODE       *Interm,
  TOPOLOGY_TREE_NODE       *Dest,
  UINT8                    DestPort
  );

KTI_STATUS
AddNodeToTopologyTree (
  SYSHOST            *Host,
  TOPOLOGY_TREE_NODE *Tree,
  TOPOLOGY_TREE_NODE *Node
  );

RAS_MPLINK_STATUS
UpdateCpuInfoInKtiInternal (
  IN SYSHOST               *Host,
  KTI_HOST_INTERNAL_GLOBAL *KtiInternalGlobal,
  UINT8                    SocktId
  );

BOOLEAN
CheckThisSocketInfoWithSbsp (
  IN     SYSHOST           *Host,
  KTI_HOST_INTERNAL_GLOBAL *KtiInternalGlobal,
  UINT8                    SbspSktId,
  UINT8                    SocketId
  );

RAS_MPLINK_STATUS
PreProcessKtiLinks (
  IN     SYSHOST           *Host,
  IN OUT KTI_SOCKET_DATA   *SocketData,
  KTI_HOST_INTERNAL_GLOBAL *KtiInternalGlobal
  );

RAS_MPLINK_STATUS
KtiFinalCpuBusCfg (
  SYSHOST                  *host,
  KTI_SOCKET_DATA          *SocketData,
  KTI_HOST_INTERNAL_GLOBAL *KtiInternalGlobal
  );

RAS_MPLINK_STATUS
KtiPhyLinkAfterWarmReset (
  SYSHOST                  *host,
  KTI_SOCKET_DATA          *SocketData,
  KTI_HOST_INTERNAL_GLOBAL *KtiInternalGlobal
  );

RAS_MPLINK_STATUS
KtiTopologyDiscovery (
  SYSHOST                  *host,
  KTI_SOCKET_DATA          *SocketData,
  KTI_HOST_INTERNAL_GLOBAL *KtiInternalGlobal
  );

RAS_MPLINK_STATUS
ProgramSystemRoute (
  SYSHOST                  *host,
  KTI_SOCKET_DATA          *SocketData,
  KTI_HOST_INTERNAL_GLOBAL *KtiInternalGlobal
  );

RAS_MPLINK_STATUS
SetupSystemIoSadEntries (
  IN OUT SYSHOST           *Host,
  IN OUT KTI_SOCKET_DATA   *SocketData,
  KTI_HOST_INTERNAL_GLOBAL *KtiInternalGlobal
  );

RAS_MPLINK_STATUS
SetupKtiMisc (
  IN  SYSHOST              *Host,
  OUT KTI_SOCKET_DATA      *SocketData,
  KTI_HOST_INTERNAL_GLOBAL *KtiInternalGlobal
  );

RAS_MPLINK_STATUS
CollectAndClearErrors (
  SYSHOST                  *host,
  KTI_SOCKET_DATA          *SocketData,
  KTI_HOST_INTERNAL_GLOBAL *KtiInternalGlobal
  );

RAS_MPLINK_STATUS
SetupSystemCoherency (
  SYSHOST                  *host,
  KTI_SOCKET_DATA          *SocketData,
  KTI_HOST_INTERNAL_GLOBAL *KtiInternalGlobal
  );

RAS_MPLINK_STATUS
ProgramLateActionOnCpuLink (
   SYSHOST   *host,
   UINT8     Cpu,
   UINT8     Link,
   IN UINT32 Value
);

RAS_MPLINK_STATUS
ProgramPhyLayerControlRegisters (
  struct sysHost *host,
  UINT8          Index,
  UINT8          LinkIndex,
  UINT8          Group
  );

RAS_MPLINK_STATUS
ProgramEparams (
  struct sysHost *host,
  UINT8          Index,
  UINT8          LinkIndex,
  UINT8          Group
  );

KTI_STATUS
KtiEvAutoRecipe (
  struct sysHost *host,
  UINT8          Socket,
  UINT8          LinkIndex
  );

RAS_MPLINK_STATUS
ProgramLinkLayerControlRegisters (
  struct sysHost           *host,
  KTI_HOST_INTERNAL_GLOBAL *KtiInternalGlobal,
  UINT8                    Index,
  UINT8                    LinkIndex,
  UINT8                    Group
  );

STATIC
RAS_MPLINK_STATUS
ProgramLinkLayer (
  struct sysHost *host,
  UINT8          Group
  );

RAS_MPLINK_STATUS
NormalizeLinkOptions (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL  *KtiInternalGlobal
  );

KTI_STATUS
ClearOrSetKtiDfxLckForFullSpeedTransition (
  struct sysHost *host,
  UINT8          Value
  );

KTI_STATUS
ProgramLinkLayerOnM3Kti (
  struct sysHost           *host,
  KTI_HOST_INTERNAL_GLOBAL *KtiInternalGlobal,
  UINT8                    Index,
  UINT8                    M3Kti,
  UINT32                   VnaCredit
  );

RAS_MPLINK_STATUS
ClearCinitbeginOnInvalidLinks (
  IN KTI_SOCKET_DATA *SocketData
  );

RAS_MPLINK_STATUS
ProgramMeshCredits (
  SYSHOST                 *host,
  KTI_SOCKET_DATA          *SocketData,
  KTI_HOST_INTERNAL_GLOBAL *KtiInternalGlobal
  );

KTI_STATUS
ProgramMiscBeforeWarmReset (
  SYSHOST                  *host,
  KTI_SOCKET_DATA          *SocketData,
  KTI_HOST_INTERNAL_GLOBAL *KtiInternalGlobal
  );

RAS_MPLINK_STATUS
PrimeHostStructure (
  SYSHOST                  *host,
  KTI_SOCKET_DATA          *SocketData,
  KTI_HOST_INTERNAL_GLOBAL *KtiInternalGlobal
  );

RAS_MPLINK_STATUS
ProgramMsrForWa (
  SYSHOST                  *host,
  KTI_SOCKET_DATA          *SocketData,
  KTI_HOST_INTERNAL_GLOBAL *KtiInternalGlobal
  );

RAS_MPLINK_STATUS
KtiProgramBGFOverrides (
  SYSHOST                  *host,
  KTI_SOCKET_DATA          *SocketData,
  KTI_HOST_INTERNAL_GLOBAL *KtiInternalGlobal
  );

KTI_STATUS
ReceivingKtiPortsFullSpeedTransition (
  struct sysHost           *host,
  KTI_HOST_INTERNAL_GLOBAL *KtiInternalGlobal,
  UINT8                    Socket,
  UINT8                    LinkIndex
  );

KTI_STATUS
ReceivingKtiPortsSetCinitbegin (
  struct sysHost           *host,
  KTI_HOST_INTERNAL_GLOBAL *KtiInternalGlobal
  );

KTI_STATUS
ReceivingKtiPortsReenableLinkLayerClockGating (
  struct sysHost           *host
  );

#endif // _CSI_RAS_MAIN_H
