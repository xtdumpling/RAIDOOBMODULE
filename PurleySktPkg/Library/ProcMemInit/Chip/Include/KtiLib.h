//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
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
// *      This file contains platfrom specific routines which might need    *
// *      porting for a particular platform.                                *
// *                                                                        *
// **************************************************************************
**/

#ifndef _KTI_LIB_H_
#define _KTI_LIB_H_

#include "DataTypes.h"
#include "PlatformHost.h"
#include "SysHost.h"
#include "CpuPciAccess.h"

KTI_STATUS
GetSocketWays (
  struct sysHost             *host,
  UINT8                      SocId,
  UINT8                      *SocketWays
  );

BOOLEAN
IsSocketFpga (
  struct sysHost             *host,
  UINT8                      SocId,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal
  );

BOOLEAN
WasSocketFpgaInitialized (
  struct sysHost             *host,
  UINT8                      SocId,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal
  );

BOOLEAN
SocketFpgasInitialized (
  struct sysHost             *host,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal
  );

KTI_STATUS
GetChaCountAndList (
  struct sysHost             *host,
  UINT8                      SocId,
  UINT8                      *CboCount,
  UINT32                     *CboMap
  );

KTI_STATUS
GetIioCount (
  struct sysHost             *host,
  UINT8                      SocId,
  UINT8                      *McpLinkCount
  );

KTI_STATUS
GetM2PcieCount (
  struct sysHost             *host,
  UINT8                      SocId,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal
  );

KTI_STATUS
GetM3KTICount (
  struct sysHost             *host,
  UINT8                      SocId,
  UINT8                      *M3KtiCount
  );

KTI_STATUS
GetKtiAgentCount (
  struct sysHost             *host,
  UINT8                      SocId,
  UINT8                      *KtiAgentCount
  );

KTI_STATUS
GetRasType (
  struct sysHost             *host,
  UINT8                      SocId,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal
  );

KTI_STATUS
GetCpuType (
  struct sysHost   *host,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal,
  UINT8            SocId
  );

KTI_STATUS
GetM2MemCount (
  struct sysHost             *host,
  UINT8                      SocId,
  UINT8                      *M2MemCount
  );

UINT32
GetTopologyBitmap(
  PSYSHOST                 host,
  KTI_SOCKET_DATA          *SocketData,
  KTI_HOST_INTERNAL_GLOBAL *KtiInternalGlobal
);

KTI_STATUS
ProgramM2mCreditReg (
  struct sysHost      *host,
  UINT8               Soc,
  UINT8               M2mem,
  UINT8               Port,
  UINT8               CrdtType,
  UINT8               CrdtCnt
  );

KTI_STATUS
ProgramM2mCreditSetting (
  struct sysHost      *host,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal,
  UINT8               Soc
  );

KTI_STATUS
CalculateChaToM2mCredit(
  struct sysHost             *host,
  UINT8                      Soc,
  UINT8                      NumOfCha,
  UINT32                     *Data32_AD,
  UINT32                     *Data32_BL
  );

KTI_STATUS
ReCalAndProgramChaToM2mCreditWhenSncEn(
  struct sysHost             *host,
  MEM_INFO       *MemInfo,
  UINT8          SocId
  );

KTI_STATUS
GetLegacyPchTargetSktId (
  struct sysHost             *host,
  UINT8                      SocId,
  UINT8                      *LegacyPchSktId
  );

KTI_STATUS
ParseCpuLep (
  struct sysHost             *host,
  KTI_SOCKET_DATA            *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal,
  UINT8                      SocId
  );

KTI_STATUS
ParseFpgaLep (
  struct sysHost             *host,
  KTI_SOCKET_DATA            *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal,
  UINT8                      SocId
  );

KTI_STATUS
UpdateStructureForHotlug (
  struct sysHost          *host,
  KTI_SOCKET_DATA         *SocketData,
  KTI_HOST_INTERNAL_GLOBAL  *KtiInternalGlobal,
  UINT8                   TmpTotCpu
  );

KTI_STATUS
AllocateIioResources (
  struct sysHost          *host,
  KTI_SOCKET_DATA         *SocketData,
  KTI_HOST_INTERNAL_GLOBAL  *KtiInternalGlobal
  );

KTI_STATUS
CalculateBusResourceRatio (
  struct sysHost          *host,
  KTI_SOCKET_DATA         *SocketData,
  KTI_HOST_INTERNAL_GLOBAL  *KtiInternalGlobal,
  UINT8                   *BusSize,
  UINT8                   TmpTotCpu
  );

KTI_STATUS
CalculateBusResources (
  struct sysHost          *host,
  KTI_SOCKET_DATA         *SocketData,
  KTI_HOST_INTERNAL_GLOBAL  *KtiInternalGlobal,
  UINT8                   *BusSize,
  UINT8                   TmpTotCpu
  );

KTI_STATUS
CalculateIoResources (
  struct sysHost          *host,
  KTI_SOCKET_DATA         *SocketData,
  KTI_HOST_INTERNAL_GLOBAL  *KtiInternalGlobal,
  UINT8                   TmpTotCpu
  );

KTI_STATUS
CalculateIoApicResources (
  struct sysHost          *host,
  KTI_SOCKET_DATA         *SocketData,
  KTI_HOST_INTERNAL_GLOBAL  *KtiInternalGlobal
  );

KTI_STATUS
CalculateMmiolResources (
  struct sysHost          *host,
  KTI_SOCKET_DATA         *SocketData,
  KTI_HOST_INTERNAL_GLOBAL  *KtiInternalGlobal,
  UINT8                   TmpTotCpu
  );

KTI_STATUS
CalculateMmiohResources (
  struct sysHost          *host,
  KTI_SOCKET_DATA         *SocketData,
  KTI_HOST_INTERNAL_GLOBAL  *KtiInternalGlobal,
  UINT8                   TmpTotCpu
  );

KTI_STATUS
CalculateMmiolGranularity (
  struct sysHost          *host,
  KTI_SOCKET_DATA         *SocketData,
  KTI_HOST_INTERNAL_GLOBAL  *KtiInternalGlobal
  );

KTI_STATUS
CalculateMmiohGranularity (
  struct sysHost          *host,
  KTI_SOCKET_DATA         *SocketData,
  KTI_HOST_INTERNAL_GLOBAL  *KtiInternalGlobal
  );

VOID PrintResourceMapHeader (
  PSYSHOST host
  );

VOID
PrintCpuResourceMapDetails (
  PSYSHOST         host,
  UINT8            Socket,
  KTI_SOCKET_DATA  *SocketData,
  KTI_CPU_RESOURCE CpuRes
  );

KTI_STATUS
CheckOemSegmentResources(
  PSYSHOST                 host,
  KTI_SOCKET_DATA          *SocketData,
  KTI_HOST_INTERNAL_GLOBAL *KtiInternalGlobal,
  KTI_CPU_RESOURCE         CpuRes[MAX_SOCKET]
  );

KTI_STATUS
CheckOemSocketResources(
  PSYSHOST                 host,
  KTI_SOCKET_DATA          *SocketData,
  KTI_HOST_INTERNAL_GLOBAL *KtiInternalGlobal,
  KTI_CPU_RESOURCE         CpuRes[MAX_SOCKET]
  );

KTI_STATUS
WalkOemSocketResources(
  PSYSHOST                 host,
  KTI_SOCKET_DATA          *SocketData,
  KTI_HOST_INTERNAL_GLOBAL *KtiInternalGlobal,
  KTI_CPU_RESOURCE         CpuRes[MAX_SOCKET]
  );

KTI_STATUS
CheckOemStackResources(
  PSYSHOST                 host,
  KTI_SOCKET_DATA          *SocketData,
  KTI_HOST_INTERNAL_GLOBAL *KtiInternalGlobal,
  KTI_CPU_RESOURCE         CpuRes[MAX_SOCKET]
  );

KTI_STATUS
WalkOemStackResources(
  PSYSHOST                 host,
  KTI_SOCKET_DATA          *SocketData,
  KTI_HOST_INTERNAL_GLOBAL *KtiInternalGlobal,
  KTI_CPU_RESOURCE         CpuRes[MAX_SOCKET]
  );

KTI_STATUS
CheckForOemResourceUpdate (
  struct sysHost          *host,
  KTI_SOCKET_DATA         *SocketData,
  KTI_HOST_INTERNAL_GLOBAL  *KtiInternalGlobal
  );

VOID
DataDumpKtiResourceAllocation (
  struct sysHost          *host,
  KTI_SOCKET_DATA         *SocketData,
  KTI_HOST_INTERNAL_GLOBAL  *KtiInternalGlobal
  );

KTI_STATUS
ConvertM3KTIRoutingTable (
  UINT8                   InPort,
  UINT8                   OutPort,
  UINT8                   DestSocId,
  UINT32                  *EncodedRoute
  );

KTI_STATUS
CheckForTopologyChange (
  struct sysHost          *host,
  KTI_SOCKET_DATA         *SocketData,
  KTI_HOST_INTERNAL_GLOBAL  *KtiInternalGlobal
  );

KTI_STATUS
SanityCheckDiscoveredTopology (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal
  );

KTI_STATUS
SanityCheckLep (
  struct sysHost         *host,
  KTI_CPU_SOCKET_DATA    *CpuData
  );

KTI_STATUS
DegradeTo1S (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal
  );

KTI_STATUS
PreProcessFeatureVsTopology (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal
  );

VOID
TopologyInfoDump (
  struct sysHost      *host,
  KTI_CPU_SOCKET_DATA     *Cpu,
  TOPOLOGY_TREE_NODE     (*CpuTree)[MAX_TREE_NODES],
  BOOLEAN              SiPort
  );

VOID
RouteTableDump (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal
  );

VOID
LepDump (
  struct sysHost         *host,
  KTI_CPU_SOCKET_DATA    *CpuData
  );

UINT8
TranslateToGenPort (
  struct sysHost             *host,
  UINT8                   SiPort
  );

UINT8
TranslateToSiPort (
  struct sysHost          *host,
  UINT8                   GenPort
  );

BOOLEAN
CheckCpuConnectedToLegCpu (
  struct sysHost             *host,
  KTI_SOCKET_DATA            *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal,
  UINT8                      SocId
  );

BOOLEAN
DuplicateOrTripleLinkExists (
  struct sysHost          *host,
  KTI_CPU_SOCKET_DATA     *CpuData,
  TOPOLOGY_TREE_NODE      *Parent,
  TOPOLOGY_TREE_NODE      *Child
  );

KTI_STATUS
RemoveDualLink(
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData
  );

KTI_STATUS
GetDualLinkOrTripleLinkFromLEP (
  struct sysHost          *host,
  KTI_CPU_SOCKET_DATA     *CpuData,
  TOPOLOGY_TREE_NODE      *Node1,
  TOPOLOGY_TREE_NODE      *Node2,
  UINT8                   *Port1,
  UINT8                   *Port2,
  UINT8                   *Port3
  );

UINT8
GetChildLocalPortFromLEP (
  struct sysHost          *host,
  KTI_CPU_SOCKET_DATA     *CpuData,
  TOPOLOGY_TREE_NODE      *Parent,
  TOPOLOGY_TREE_NODE      *Child
  );

UINT8
GetChildPeerPortFromLEP (
  struct sysHost          *host,
  KTI_CPU_SOCKET_DATA    *CpuData,
  TOPOLOGY_TREE_NODE      *Parent,
  TOPOLOGY_TREE_NODE      *Child
  );

KTI_STATUS
GetParentSocket (
  struct sysHost         *host,
  TOPOLOGY_TREE_NODE     *TopologyTree,
  TOPOLOGY_TREE_NODE     *Child,
  TOPOLOGY_TREE_NODE     *Parent
  );

UINT8
GetParentIndex (
  struct sysHost           *host,
  KTI_CPU_SOCKET_DATA      *CpuData,
  TOPOLOGY_TREE_NODE       *Tree,
  TOPOLOGY_TREE_NODE       *Child
  );

UINT8
GetChildHopLength (
  struct sysHost       *host,
  TOPOLOGY_TREE_NODE   *Tree,
  UINT8                SocId,
  UINT8                SocType
  );

BOOLEAN
NodeFoundInTopologyTree (
  struct sysHost           *host,
  TOPOLOGY_TREE_NODE       *Tree,
  TOPOLOGY_TREE_NODE       *SearchNode
  );

BOOLEAN
NodeFoundInMinPathTree (
  struct sysHost          *host,
  TOPOLOGY_TREE_NODE      *Tree,
  TOPOLOGY_TREE_NODE      *SearchNode
  );

KTI_STATUS
AddNodeToTopologyTree (
  struct sysHost           *host,
  TOPOLOGY_TREE_NODE       *Tree,
  TOPOLOGY_TREE_NODE       *Node
  );

KTI_STATUS
AddNodeToRingTree (
  struct sysHost          *host,
  TOPOLOGY_TREE_NODE      *Tree,
  TOPOLOGY_TREE_NODE      *Node
  );

UINT8
CountImmediateChildren (
  struct sysHost           *host,
  TOPOLOGY_TREE_NODE       *Tree,
  TOPOLOGY_TREE_NODE       *Parent
  );

BOOLEAN
ShorterPathExists (
  struct sysHost          *host,
  TOPOLOGY_TREE_NODE      *Tree,
  TOPOLOGY_TREE_NODE      *SearchNode
  );

BOOLEAN
CompareRing (
  struct sysHost       *host,
  UINT8                *Ring1,
  UINT8                *Ring2
  );

VOID
KtiCheckPoint (
  UINT8                      SocId,
  UINT8                      SocType,
  UINT8                      KtiLink,
  UINT8                      MajorCode,
  UINT8                      MinorCode,
  struct sysHost             *host
  );

VOID
KtiAssert (
  struct sysHost             *host,
  UINT8                      MajorCode,
  UINT8                      MinorCode
  );

VOID
KtiLogWarning (
  struct sysHost             *host,
  UINT8                      SocId,
  UINT8                      SocType,
  UINT8                      KtiLink,
  UINT8                      WarnCode
  );

VOID
KtiLogTopologyChange(
  PSYSHOST                host,
  UINT32                  TopologyBefore,
  UINT32                  TopologyAfter,
  TOPOLOGY_DEGRADE_REASON Reason
);

UINT32
KtiReadPciCfg(
  struct sysHost        *host,
  UINT8                 SocId,
  UINT8                 BoxInst,
  UINT32                RegOffset
);

VOID
KtiWritePciCfg(
  struct sysHost        *host,
  UINT8                 SocId,
  UINT8                 BoxInst,
  UINT32                RegOffset,
  UINT32                data
  );

#define KTILIB_IAR_IOVC  20
#define KTILIB_IAR_PDQ   62
#define KTILIB_IAR_BCAST 63

UINT8
KtiReadIar(
  struct sysHost        *host,
  UINT8                 SocId,
  UINT8                 BoxInst,
  UINT8                 UnitId,
  UINT8                 RegOffset
  );

VOID
KtiWriteIar(
  struct sysHost        *host,
  UINT8                 SocId,
  UINT8                 BoxInst,
  UINT8                 UnitId,
  UINT8                 RegOffset,
  UINT8                 data
  );

VOID
KtiWriteLbc (
  struct sysHost        *host,
  UINT8                 SocId,
  UINT8                 Port,
  UINT8                 LbcType,
  UINT32                LaneMask,
  UINT32                LoadSel,
  UINT32                Data
  );

#ifdef RAS_SUPPORT
UINT32
ReadCpuPciCfgRas (
  PSYSHOST host,
  UINT8    SocId,
  UINT8    BoxInst,
  UINT32   Offset
  );

void
WriteCpuPciCfgRas (
  PSYSHOST host,
  UINT8    SocId,
  UINT8    BoxInst,
  UINT32   Offset,
  UINT32   Data
  );
#endif

VOID
KtiFixedDelay (
  struct sysHost      *host,
  UINT32          MicroSecs
  );

KTI_STATUS
KtiMain (
  struct sysHost             *host
  );

KTI_STATUS
KtiEvAutoRecipe (
  struct sysHost      *host,
  UINT8               Socket,
  UINT8               LinkIndex
  );

INT32
OemKtiGetEParams (
  struct sysHost           *host,
  UINT8                    SocketID,
  UINT8                    Link,
  UINT8                    Freq,
  VOID                     *LinkSpeedParameter
  );

VOID
OemGetResourceMapUpdate (
  struct sysHost    *host,
  KTI_CPU_RESOURCE  CpuRes[MAX_SOCKET],
  KTI_OEMCALLOUT    SystemParams
  );

BOOLEAN
OemCheckCpuPartsChangeSwap(
  struct sysHost           *host
  );

BOOLEAN
OemGetAdaptedEqSettings (
  struct sysHost           *host,
  UINT8                     Speed,
  VOID                     *AdaptTbl
);

VOID
OemDebugPrintKti (
    struct sysHost *host,
    UINT32 DbgLevel,
    char* Format,
    ...
  );

VOID
OemWaitTimeForPSBP (
    struct sysHost *host,
    UINT32 *WaitTime
  );

VOID
OemHookPreTopologyDiscovery (
  struct sysHost             *host
  );

VOID
OemHookPostTopologyDiscovery (
  struct sysHost             *host
  );

UINT32
OemGetBoardTypeBitmask (
  struct sysHost *host
  );

KTI_STATUS
HideMcpDevices (
  struct sysHost   *host,
  KTI_SOCKET_DATA  *SocketData
  );

KTI_STATUS
EnableSncMode (
  struct sysHost           *host,
  MEM_INFO                 *MemInfo
  );

KTI_STATUS
KtiDetermineIfRequireNonInterleavedDecoder (
  struct sysHost           *host,
  KTI_SOCKET_DATA          *SocketData,
  KTI_HOST_INTERNAL_GLOBAL *KtiInternalGlobal,
  KTI_CPU_RESOURCE          CpuRes[MAX_SOCKET]
  );

BOOLEAN
IsXGreaterThanYUINT64 (
  UINT64_STRUCT X,
  UINT64_STRUCT Y
  );

BOOLEAN
IsXEqualYUINT64 (
  UINT64_STRUCT X,
  UINT64_STRUCT Y
  );

VOID
SendMeshModePcodeMailboxCommand (
  struct sysHost           *host
  );

VOID
GetNeighborSocs (
  struct sysHost             *host,
  KTI_SOCKET_DATA            *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal,
  UINT8                      Soc,
  UINT8                      *NeighborSoc1,
  UINT8                      *NeighborSoc2,
  UINT8                      *RemoteSoc
  );

KTI_STATUS
SendMailBoxCmdToPcode (
  struct sysHost      *host,
  UINT8               Cpu,
  UINT32              Command,
  UINT32              Data
  );

void
KtiOutPort32 (
  PSYSHOST host,
  UINT16   ioAddress,
  UINT32   data
  );

UINT32
KtiInPort32 (
  PSYSHOST host,
  UINT16   ioAddress
  );

UINT8
EfiDisableInterrupts_1(
VOID
  );

UINT8
EfiHalt_1(
VOID
  );

UINT32
GetPhysicalBusNumber (
  PSYSHOST host,
  UINT8    Soc,
  UINT32   BusAddr
  );

BOOLEAN
OemReadKtiNvram (
  struct sysHost             *host
  );

VOID
Tsc_Sync (
  struct sysHost           *host,
  KTI_SOCKET_DATA          *SocketData,
  UINT8                    PendingSocId,
  KTI_HOST_INTERNAL_GLOBAL *KtiInternalGlobal
);

UINT8
KtiGetSiliconRevision(
  PSYSHOST  host,
  UINT8     Socket,
  UINT8     CpuStep,
  UINT8     Cf8Access
);
#endif // _KTI_LIB_H_
