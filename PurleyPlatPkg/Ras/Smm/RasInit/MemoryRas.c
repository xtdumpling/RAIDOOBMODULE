/*++
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
--*/
/*++

Copyright (c) 2008 - 2017 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  MemoryRas.c

Abstract:

  This module implements the memory RAS flow control for memory hot
  adding/removal through the interfaces provided by MemRas module.

--*/
#pragma warning(disable :4005) //remove this when all macro re-definitions are solved
#pragma warning(disable :4102) // disable warnings due to unused variables; expected to be fixed
                               // when Platform code is fully implemented
#pragma warning(disable :4101) // warning due to unused local variables


#include "MemoryRas.h"
#include "MemFunc.h"
#include <CpuRegs.h>
#include <Protocol/CpuCsrAccess.h>


extern BIOS_ACPI_PARAM                *mAcpiParameter;
extern BOOLEAN                        mIsSoftSmiRequest;
extern UINT8                          mHpRequest[6];
extern UINT32                         mSmiParam[4];
extern BOOLEAN                    mExitImmediately;
extern UINT64                         mTsegUcBase;
extern UINT8                          mQpiCpuSktHotPlugEn;

//
// Vairable used to backup the old values
//
static BOOLEAN                        mForceRequest;
static BOOLEAN                        mNotifyOs;
static UINT8                          mOldMirrorSetupOption;

UINT8                                 mCurrentMemSocket; //store the Socket Number for MemOnline
BOOLEAN                               mBeforeQuiesce   = FALSE;
BOOLEAN                               mAbortQuiesce    = FALSE;
UINT64                                mRasFlags        = 0;
EFI_MEM_RAS_PROTOCOL                  *mMemRas  = NULL;
EFI_QUIESCE_SUPPORT_PROTOCOL          *mQuiesceSupport = NULL;
EFI_CPU_CSR_ACCESS_PROTOCOL           *mCpuCsrAccess = NULL;
EFI_RAS_MPLINK_PROTOCOL               *mRasMpLink = NULL;
extern  SPS_RAS_NOTIFY_PROTOCOL       *mSpsRasNotifyProtocol;

BOOLEAN                               mPatrolCKEStopped = FALSE;


#pragma optimize( "", off )

VOID
InitMemoryRas (
  VOID
)
//
//  Description:
//    Initialization for the memory RAS
//
{
  UINT8                       node;
  UINT8                       socket, mc;
  UINT16                      MemSltSts;
  UINT32                      Data32;
  UINT64                      Size;
  EFI_STATUS                  Status;
  // MEMHPCTRL_IIO_MEMHP_STRUCT  MemHpCtrl;
  struct sysHost              *pSysHost = NULL;

  Status = gSmst->SmmLocateProtocol (&gEfiQuiesceProtocolGuid, NULL, &mQuiesceSupport);
  ASSERT_EFI_ERROR (Status);

  Status = gSmst->SmmLocateProtocol (&gEfiMemRasProtocolGuid, NULL, &mMemRas);
  ASSERT_EFI_ERROR (Status);

  Status = gSmst->SmmLocateProtocol (&gEfiCpuCsrAccessGuid, NULL, &mCpuCsrAccess);
  ASSERT_EFI_ERROR (Status);

  Status = gSmst->SmmLocateProtocol (&gEfiRasMpLinkProtocolGuid, NULL, &mRasMpLink);
  ASSERT_EFI_ERROR (Status);

  pSysHost = (struct sysHost *)mMemRas->pSysHost;
  DEBUG((EFI_D_LOAD|EFI_D_INFO, "\nMemory Hot-Add Driver Install:\n"));
  DEBUG((EFI_D_LOAD|EFI_D_INFO, "\tMRAS Signature = %08x\n", mMemRas->Signature));
  DEBUG((EFI_D_LOAD|EFI_D_INFO, "\nstruct sysHost.setup.mem {\n", pSysHost->var.mem.memSize));
  DEBUG((EFI_D_LOAD|EFI_D_INFO, "\n  options:       %08X\n", pSysHost->setup.mem.options));
  DEBUG((EFI_D_LOAD|EFI_D_INFO, "\n  RASmodeEx:     %02X\n", pSysHost->setup.mem.RASmodeEx));
  DEBUG((EFI_D_LOAD|EFI_D_INFO, "\n  RASmode:       %02X\n", pSysHost->setup.mem.RASmode));
  DEBUG((EFI_D_LOAD|EFI_D_INFO, "\n  Interleave:    %02X }\n", pSysHost->setup.mem.socketInter));
  DEBUG((EFI_D_LOAD|EFI_D_INFO, "\tCPU Type = %d\n", pSysHost->var.common.cpuType));
  //DEBUG((EFI_D_LOAD|EFI_D_INFO, "\tCPU Sub-Type = %d\n", pSysHost->var.common.cpuSubType));
  DEBUG((EFI_D_LOAD|EFI_D_INFO, "\tCPU Stepping = %d\n", pSysHost->var.common.stepping));
  DEBUG((EFI_D_LOAD|EFI_D_INFO, "\tCPU SBSP = %d\n", pSysHost->nvram.common.sbspSocketId));
  DEBUG((EFI_D_LOAD|EFI_D_INFO, "\tCPU Socket Present Bit Map = %x\n", pSysHost->var.common.socketPresentBitMap));

  DEBUG((EFI_D_LOAD|EFI_D_INFO, "\tCPU MMCFG Base = %08x\n", pSysHost->var.common.mmCfgBase));
  DEBUG((EFI_D_LOAD|EFI_D_INFO, "\tCPU PM Base = %08x\n", pSysHost->var.common.pmBase));
  DEBUG((EFI_D_LOAD|EFI_D_INFO, "\tCPU SAD Number = %d\n", pSysHost->var.mem.SADNum));
  DEBUG((EFI_D_LOAD|EFI_D_INFO, "\tCPU MAX Memory Address = %08x\n", pSysHost->setup.common.maxAddrMem));
  DEBUG((EFI_D_LOAD|EFI_D_INFO, "\tCPU Memory High Gap = %d\n", pSysHost->setup.common.highGap));
  DEBUG((EFI_D_LOAD|EFI_D_INFO, "\tCPU System Memory Size = %d\n", pSysHost->var.mem.memSize));

  for (node = 0; node < MC_MAX_NODE; node++) {
    socket = NODE_TO_SKT(node);
    mc = NODE_TO_MC(node);
    pSysHost->nvram.mem.socket[socket].imc[mc].imcNodeId = (NODE_TO_SKT(node) & 3) | ((NODE_TO_SKT(node) & 4) << 1) | (NODE_TO_MC(node) << 2);
  }

  //
  // Save the RAS backdoor request pointer to IOH SR 17
  // It will be used for memory RAS request
  //
  Data32 = (UINT32)(UINTN)mAcpiParameter->SmiRequestParam;
  //
  // Publish the Request param address in a scratch pad
  //

  //
  // Update the ACPI parameter block for all memory nodes at boot time
  // So that OS will see correct base and size for all populated memory nodes.
  //
  UpdateAcpiMemoryParams (ALL_SOCKETS, ALL_BRANCHES, FALSE);

  //
  // Determine if we should do time sliced RAS
  //
  mMemRas->ModifyRasFlag (RAS_FLAG_TIME_SLICING, 0);

}

EFI_STATUS
CheckMrcSetupOption (
  IN  INTERNAL_STATE   State
)
//
//  Description
//    This function will check if the RAS request is valid in current configuration
//
//  Parameters
//    State  - Indicates what RAS request is going to be exectued
//
//  Returns
//    EFI_SUCCESS     - The RAS request is valid under current setup configuration
//    EFI_UNSUPPORTED - The RAS request is invalid under current setup configuration
//
{
  EFI_STATUS   Status;

  Status = EFI_UNSUPPORTED;

  switch (State) {
  case EnumMemoryOnlineOfflineEntry:
    //
    // Interleave has to be non-interleave or intra-socket-interleave for memory RAS (hotadd / remove)
    // Check the current interleave option
    if((mMemRas->SystemMemInfo->MemSetup.options & NUMA_AWARE) &&
       (mMemRas->SystemMemInfo->MemSetup.socketInter == SOCKET_1WAY  || mMemRas->SystemMemInfo->MemSetup.socketInter == SOCKET_2WAY)) {
      Status = EFI_SUCCESS;
    }
    break;

  default:
    break;
  }

  return Status;
}


EFI_STATUS
GetMemoryState (
  IN  UINT8                  Socket,
  OUT ONLINE_OFFLINE_STATE  *MemState
)
//
//  Description
//    This function will get current memory branch states behind a given socket
//
//  Parameters
//    Socket   - CPU Socket Index
//    MemState - Pointer to an array of Node to accept the state.
//               On return it holds the memory state for each branch
//
//  Returns
//    EFI_SUCCESS - Always success
//
{
  EFI_STATUS Status = EFI_SUCCESS;
  UINT8     Br;
  UINT8 Present;

  for (Br = 0; Br < MAX_IMC; Br++) {
    if ((Status = mMemRas->GetCurrentMemoryNodeState(SKTMC_TO_NODE(Socket, Br), &Present)) != EFI_SUCCESS)
      return Status;

    if (Present)
      MemState[Br] = EnumOnline;
    else
      MemState[Br] = EnumOffline;
    }

  return Status;
}

EFI_STATUS
OnlineOfflineSingleMemoryNode (
  UINT8 Action,
  UINT8 Node
  )
//
//  Description
//  This function abstracts the MRC Memory Online/Offline APIs which returns
//  various return status to the starts STATUS_DONE, IN_PROGRESS or ERROR_EXIT
//  aligning with the rest of memory RAS state machine flows
//
//  Parameters
//    Action - Actions to execute
//    Node - Memory Node
//
//  Returns
//    STATUS_DONE - OL Memory Operation is completely successfully
//    IN_PROGRESS - OL Memory Opertion is still working in progress
//    ERROR_EXIT  - OL Memory Operation failed
//
{
  static UINT8   MrcStatus     = MemNodeNop;
  static UINT8   MrcEndStatus  = MemNodeNop;
  EFI_STATUS     Status;
  UINT8 skt = 0;
  MEM_ONLINE_PARAMS Params;
  UINTN CpuNumber;
  EFI_PROCESSOR_INFORMATION ProcInfo;

  if (mMemRas->MemoryNodeOnline == NULL || mMemRas->pSysHost == NULL) {
    DEBUG((EFI_D_LOAD|EFI_D_INFO, "\t\tERROR! Detect MemoryNodeOnline NULL pointer\n"));
    return ERROR_EXIT;
  }

  skt = NODE_TO_SKT(Node);

  mBeforeQuiesce = FALSE;
  Status  = IN_PROGRESS;
  if (Action & MEM_NODE_ON) {
    if (MrcStatus == MemNodeNop) {
      if (Action & MEM_NODE_INIT)   {
        // Initialize MemNode/DDR only
        Action         &= ~(MEM_MAP_UPDATE);
        MrcStatus      = MemNodeOnline;
        MrcEndStatus   = MemNodeInstallInProgressEntry;
      } else if (Action & MEM_MAP_UPDATE) {
        // Update SAD/TAD only
        Action         &= ~(MEM_NODE_INIT);
        MrcStatus      = MemNodeInstallInProgressEntry;
        MrcEndStatus   = MemNodeOnlineComplete;
        if(mSpsRasNotifyProtocol != NULL) {
          Status = mSpsRasNotifyProtocol->RasNotify(RasNotificationDmiQuiescence);
        }
      } else {
        // Should not be here, just do nothing
        MrcStatus      = MemNodeOnlineComplete;
        MrcEndStatus   = MemNodeOnlineComplete;
        if(mSpsRasNotifyProtocol != NULL) {
          Status = mSpsRasNotifyProtocol->RasNotify(RasNotificationDmiUnQuiescence);
        }
      }

      DEBUG((EFI_D_LOAD|EFI_D_INFO, "\t\tSTART MEMORY ONLINE ON NODE[N%d] ACTION = %02x. START %d END %d\n",
            Node, Action, MrcStatus, MrcEndStatus));
    }

    //
    // Provide a hook point where the RAS handler can check the condition
    // before quiesce execution to decide if it should go ahead or abort
    //
    if (MrcStatus == MemNodeInstallQuiesce && mAbortQuiesce == TRUE) {
      MrcStatus = MemNodeInstallAbortQuiesce;
    }

    mMemRas->MemoryNodeOnline( Node, &MrcStatus);

  } else {
    //
    // Invalide options, just do nothing
    //
    MrcStatus      = MemNodeNop;
    MrcEndStatus   = MemNodeNop;
  }

  //
  // At this point, either MemoryNodeOnline or MemoryNodeOffline is completed in one of its internal states
  // Determine what state it is in now!
  //
  switch (MrcStatus) {
  case MemNodeError:
    // Error, exit right away
    Status = ERROR_EXIT;
    MrcStatus = MemNodeNop;
    break;

  case MemNodeRemoveQuiesce:
  case MemNodeInstallQuiesce:
    mBeforeQuiesce = TRUE;
    mAbortQuiesce  = FALSE;

  default:
    // Check whether start/stop meets based on specific action!
    if (MrcStatus == MrcEndStatus) {
      Status = STATUS_DONE;
      MrcStatus = MemNodeNop;
    } else {
      Status = IN_PROGRESS;
    }

    break;
  }

  return Status;
}


EFI_STATUS
OnlineOfflineMemoryLibCall (
  UINT8 Action,
  UINT8 Socket,
  UINT8 Branch
  )
//
//  Description
//    Abstract the memory online/offline to support both risers behind a
//    socket. All other routines are supposed to call this function for
//    onlining and offlining
//
//  Parameters
//    Action - Actions to execute
//    Socket - Socket Index (0 - MAX_SOCKET)
//    Branch - Branch Index within socket (0 - MAX_IMC)
//
//  Returns
//    STATUS_DONE - OL Memory Operation is completely successfully
//    IN_PROGRESS - OL Memory Opertion is still working in progress
//    ERROR_EXIT  - OL Memory Operation failed
//
{
  static  INTERNAL_STATE   StateMachine = EnumMemoryLibCallInit;
  EFI_STATUS               Status;

  Status = IN_PROGRESS;
  switch (StateMachine) {
    case EnumMemoryLibCallInit:
      StateMachine  = EnumMemoryLibCallExecute;

    case EnumMemoryLibCallExecute:
      DEBUG((EFI_D_LOAD|EFI_D_INFO, "\tMEM START Online[Skt%d,MC%d] ACTION = %02x\n", Socket, Branch, Action));
      Status = OnlineOfflineSingleMemoryNode (Action, SKTMC_TO_NODE(Socket, Branch));
      if (Status == IN_PROGRESS)
        break;

      DEBUG((EFI_D_LOAD|EFI_D_INFO, "\tMEM END Online[Skt%d,MC%d] STATUS = %x\n\n\n", Socket, Branch, Status));
          StateMachine   = EnumMemoryLibCallDone;

    case EnumMemoryLibCallDone:
    default:
      StateMachine = EnumMemoryLibCallInit;
      break;
  }

  return Status;
}

VOID
UpdateMemoryBranchState (
  IN  UINT8                  Branch,
  OUT ONLINE_OFFLINE_STATE  *MemState
)
//
//  Description
//    Adjust the request accordingly for onlining and offlining
//    by either toggling the request or forcing the request
//
//  Parameters
//    Socket   - Socket Index (0 - MAX_SOCKET)
//    Branch   - Branch Index within socket (0 - MAX_IMC)
//    MemState - Hold the requested memory state on return
//
//  Returns
//    NONE
//
{
  UINT8                 Br;
  UINT8                 ForceOnOff;

  if (mIsSoftSmiRequest && mSmiParam[0] == SMI_MEM_ONLINE_OFFLINE) {
    // Handle Memory online/offline event
    ForceOnOff = (UINT8)(mSmiParam[1]) & 3;
    for (Br = 0; Br < MAX_IMC; Br++) {
      if (Branch == Br || Branch == ALL_BRANCHES) {
        if (ForceOnOff == MEM_RAS_FLAG_NODE_OFF) {
          // Request force offlining
          if (MemState[Br] == EnumOffline) {
            MemState[Br] = EnumUnchanged;
          } else {
            MemState[Br] = EnumOffline;
          }
        }else if (ForceOnOff == MEM_RAS_FLAG_NODE_ON) {
          // Request onlining
          if (MemState[Br] == EnumOnline) {
            MemState[Br] = EnumUnchanged;
          } else {
            MemState[Br] = EnumOnline;
          }
        }else {
          // It should never come here!
          MemState[Br] = EnumUnchanged;
        }
      } else {
        MemState[Br] = EnumUnchanged;
      }
    }
  } else {
    // Handle memory online/offline associated with CPU online/offline event
    for (Br = 0; Br < MAX_IMC; Br++) {
      if (Branch == Br || Branch == ALL_BRANCHES) {
        if (MemState[Br] == EnumOnline) {
#ifdef MEM_RAS_FLAG_NODE_OFF_SUPPORT
          MemState[Br] = EnumOffline;
#else
          MemState[Br] = EnumUnchanged;
#endif
        } else if (MemState[Br] == EnumOffline) {
          MemState[Br] = EnumOnline;
        } else {
          MemState[Br] = EnumUnchanged;
        }
      } else {
        MemState[Br] = EnumUnchanged;
      }
    }
  }

  return;
}

UINT32
SwapBits (
  UINT8         Index0,
  UINT8         Index1,
  UINT32        Value
)
//
//  Description
//    This function will swap the bits inside a DWORD
//
//  Parameters
//    Index0   - Bit index 0 (0 - 31)
//    Index1   - Bit index 0 (0 - 31)
//    Value    - Input DWORD value to swap
//
//  Returns
//    The swapped the valude
//
{
  UINT32   Mask;
  UINT32   Result;

  Mask = 0;
  if (Value & (1<<Index0)) {
    Mask |= (1<<Index1);
  }
  if (Value & (1<<Index1)) {
    Mask |= (1<<Index0);
  }
  Result = Value & ~((1<<Index0)|(1<<Index1)) | Mask;

  return Result;
}

VOID
SwapMemoryNode (
  UINT8         Index0,
  UINT8         Index1
)
//
//  Description
//    This function will swap the memory node inside the ACPI
//    parameter block
//
//  Parameters
//    Index0   - Memory node 0 index (0 - MC_MAX_NODE)
//    Index1   - Memory node 1 index (0 - MC_MAX_NODE)
//
//  Returns
//    None
//
{
  UINT64  Temp;

  ASSERT(Index0 < MC_MAX_NODE);
  ASSERT(Index1 < MC_MAX_NODE);

  mAcpiParameter->MemoryBoardChgEvent = (UINT16)SwapBits (Index0, Index1, mAcpiParameter->MemoryBoardChgEvent);
  mAcpiParameter->MemoryBoardBitMask  = (UINT16)SwapBits (Index0, Index1, mAcpiParameter->MemoryBoardBitMask);

  Temp = mAcpiParameter->MemoryBoardBase[Index0];
  mAcpiParameter->MemoryBoardBase[Index0] = mAcpiParameter->MemoryBoardBase[Index1];
  mAcpiParameter->MemoryBoardBase[Index1] = Temp;

  Temp = mAcpiParameter->MemoryBoardRange[Index0];
  mAcpiParameter->MemoryBoardRange[Index0] = mAcpiParameter->MemoryBoardRange[Index1];
  mAcpiParameter->MemoryBoardRange[Index1] = Temp;
}

EFI_STATUS
UpdateAcpiMemoryParams (
  UINT8        Socket,
  UINT8        Branch,
  BOOLEAN      Ejection
)
//
//  Description
//    This function is designed to update the ACPI parameter block
//    which is used to communicate between BIOS and ACPI OS
//
//  Parameters
//    Socket   - Socket Index (0 - MAX_SOCKET)
//    Branch   - Branch Index within socket (0 - MAX_IMC)
//    Ejection - Indicate it is an ejection request or not
//
//  Returns
//    EFI_SUCCESS
//
{
  UINT8             NodeIndex;
  UINT8             EndIndex;
  UINT8             Counter;
  UINT8             i;
  UINT64            Data64;
  UINT64            MemoryBaseLimit;
  UINT32            MmCfg;
  UINT32            RequestedEvent;
  UINT32            PreviousBase;
  UINT8             socket, mc;
  struct SystemMemoryMapHob *systemMemInfo;
  struct sysHost              *pSysHost = NULL;

  Counter = 0;
  MemoryBaseLimit = 0;
  socket = 0;
  mc = 0;
  pSysHost = (struct sysHost *)mMemRas->pSysHost;
  systemMemInfo = (struct SystemMemoryMapHob *)(UINTN)mMemRas->SystemMemInfo;

  //
  // Do not update anything if it is in inter-socket interleave mode
  //
  if ((mMemRas->SystemMemInfo->MemSetup.socketInter != SOCKET_2WAY ) &&
      (mMemRas->SystemMemInfo->MemSetup.socketInter != SOCKET_1WAY)) {
      mAcpiParameter->MemoryBoardChgEvent = 0;
      mAcpiParameter->MemoryBoardBitMask  = 0;
      return EFI_SUCCESS;
  }

  PreviousBase = (UINT32)(-1);
  if (Socket == ALL_SOCKETS && Branch == ALL_BRANCHES) {
    //
    // It is called from boot time
    // So just fill in all the info without any events set
    //
    NodeIndex = 0;
    EndIndex  = MC_MAX_NODE;
    RequestedEvent = 0;

     for (NodeIndex = 0; NodeIndex < MC_MAX_NODE ; NodeIndex++) {
       mAcpiParameter->MemoryBoardBase[NodeIndex]   = 00;
       mAcpiParameter->MemoryBoardRange[NodeIndex]  = 00;
       socket = NODE_TO_SKT(NodeIndex);
       mc = NODE_TO_MC(NodeIndex);
       DEBUG((EFI_D_LOAD|EFI_D_INFO, "\tMultiple Socket/Riser Flow: N%d, enabled = %d,  systemMemInfo - RequiredMemorySize = %08x\n",
            NodeIndex,
            systemMemInfo->Socket[socket].imcEnabled[mc],
            systemMemInfo->Socket[socket].imc[mc].MemSize));
      mAcpiParameter->MemoryBoardBitMask  = 00;
      mAcpiParameter->MemoryBoardChgEvent = (UINT16)RequestedEvent;
     }

    NodeIndex = 0;

  } else if (Branch == ALL_BRANCHES) {

    //
    // It is called at runtime for socket online/offline
    // Update event mask for both of the memory risers
    //
    NodeIndex = (Socket << 1);
    EndIndex  = NodeIndex + 2;
    RequestedEvent = (3 << NodeIndex);

    DEBUG((EFI_D_LOAD|EFI_D_INFO, "\tSingle Socket, Dual Riser Flow:\n"));
    for (i = NodeIndex; i < EndIndex; i++) {
      socket = NODE_TO_SKT(NodeIndex);
      if (socket >= MAX_SOCKET) {
        break;
      }
      mc = NODE_TO_MC(i);
      systemMemInfo->Socket[socket].imcEnabled[mc]  = 1;
      systemMemInfo->Socket[socket].imc[mc].MemSize  = pSysHost->var.mem.socket[socket].imc[mc].memSize;
    } //End FOR Loop

  } else {
    //
    // It is called at runtime for memory online/offline
    // Just update event mask for changed riser
    //
    NodeIndex = ((Socket << 1) + Branch);
    EndIndex  = NodeIndex + 1;
    RequestedEvent = (1 << NodeIndex);
    systemMemInfo->Socket[Socket].imcEnabled[mc]   = 1;
    systemMemInfo->Socket[Socket].imc[NodeIndex].MemSize  = pSysHost->var.mem.socket[Socket].imc[NodeIndex].memSize;
    systemMemInfo->Socket[Socket].imc[EndIndex].MemSize  = pSysHost->var.mem.socket[Socket].imc[EndIndex].memSize;
  }

  if (Ejection) { // Request OS ejection
     mAcpiParameter->SciRequestParam[0]   = SCI_EJECTION;
     mAcpiParameter->SciRequestParam[1]   = RequestedEvent;
     mAcpiParameter->SciRequestParam[2]   = 0;
     mAcpiParameter->SciRequestParam[3]   = 0;
  } else {        // Request OS check the bus/device
     mAcpiParameter->SciRequestParam[0]   = SCI_CHECK;
     mAcpiParameter->SciRequestParam[1]   = 0;
     mAcpiParameter->SciRequestParam[2]   = 0;
     mAcpiParameter->SciRequestParam[3]   = 0;

     //
     // Set the event flag so that ASL code knows what occurred
     //
     //mAcpiParameter->MemoryBoardChgEvent = (RequestedEvent > 0) ? TRUE : FALSE;
     mAcpiParameter->MemoryBoardChgEvent = (UINT16)RequestedEvent;

     //
     // Always update memory size information for all nodes
     //
    for(Counter = 0; Counter < systemMemInfo->numberEntries; Counter++) {
      NodeIndex = systemMemInfo->Element[Counter].NodeId;
      if((mAcpiParameter->MemoryBoardBitMask) & (1 << NodeIndex)){
        MemoryBaseLimit = mAcpiParameter->MemoryBoardRange[NodeIndex] + LShiftU64(systemMemInfo->Element[Counter].ElementSize, MEM_ADDR_SHFT_VAL);
        mAcpiParameter->MemoryBoardRange[NodeIndex] = MemoryBaseLimit;
      } else {
        mAcpiParameter->MemoryBoardBitMask |= 1 << NodeIndex;
        MemoryBaseLimit = LShiftU64(systemMemInfo->Element[Counter].BaseAddress, 30);
        mAcpiParameter->MemoryBoardBase[NodeIndex] = MemoryBaseLimit;
        MemoryBaseLimit = LShiftU64((systemMemInfo->Element[Counter].BaseAddress + systemMemInfo->Element[Counter].ElementSize), MEM_ADDR_SHFT_VAL);
        mAcpiParameter->MemoryBoardRange[NodeIndex] = MemoryBaseLimit;
      }

       DEBUG((EFI_D_LOAD|EFI_D_INFO, "N%d New Memory Range[MemoryBoardBase = %08x, MemoryBoardRange = %08x]\n",
            NodeIndex,
            mAcpiParameter->MemoryBoardBase[NodeIndex],
            mAcpiParameter->MemoryBoardRange[NodeIndex]));
    }

    for (; NodeIndex < EndIndex; NodeIndex++) {
      socket = NODE_TO_SKT(NodeIndex);
      mc = NODE_TO_MC(NodeIndex);

      if ((systemMemInfo->Socket[socket].imcEnabled[mc] == 0) || (systemMemInfo->Socket[socket].imc[mc].MemSize == 0)) {
        //
        // Skip this node and mark as non-present
        //
        mAcpiParameter->MemoryBoardBitMask  &= ~(1 << NodeIndex);
        mAcpiParameter->MemoryBoardBase[NodeIndex]  = 0;
        mAcpiParameter->MemoryBoardRange[NodeIndex] = 0;
        DEBUG((EFI_D_LOAD|EFI_D_INFO, "\tSkipping Node%d because no memory found.\n", NodeIndex));
        continue;
      }
    }
  }

  //
  // If we are at runtime, we must update the spare mask with all new spares.
  //
  if (!(Socket == ALL_SOCKETS && Branch == ALL_BRANCHES)) {
    //
    // Mark all newly created spares as 1 in MemSpareMask bitmap.
    //
    mAcpiParameter->MemSpareMask = mAcpiParameter->MemoryBoardBitMask ^ mHpRequest[5];
  }

  DEBUG((EFI_D_LOAD|EFI_D_INFO, "\tmAcpiParameter->MemoryBoardBitMask = %x, MemoryBoardChgEvent = 0x%x\n", mAcpiParameter->MemoryBoardBitMask,
        mAcpiParameter->MemoryBoardChgEvent));

  for (NodeIndex = 0; NodeIndex < MC_MAX_NODE; NodeIndex++) {

    DEBUG((EFI_D_LOAD|EFI_D_INFO, "N%d, New Memory Range - MemoryBoardBase = %08x, MemoryBoardRange = %08x\n",
        NodeIndex,
        mAcpiParameter->MemoryBoardBase[NodeIndex],
        mAcpiParameter->MemoryBoardRange[NodeIndex]));
  }

  return EFI_SUCCESS;
}

EFI_STATUS
GenerateRasSci (
    VOID
)
//
//  Routine Description:
//    This function generates an SCI interrupt for Ras OS Consumption
//
//  Arguments:
//    None
//
//  Returns:
//    Success
//
{
  UINT8  GpeCtrl = 0;
  UINT16 PmBase = 0;

  // PM base address
  PmBase = MmioRead16 (MmPciBase (DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_PMC, PCI_FUNCTION_NUMBER_PCH_PMC) + R_PCH_PMC_ACPI_BASE);
  PmBase = (UINT16) (PmBase & 0xFF80);

  // Generate SCI event
  gSmst->SmmIo.Io.Read (&gSmst->SmmIo, SMM_IO_UINT8, (PmBase + R_PCH_ACPI_GPE_CNTL), 1, &GpeCtrl);
  GpeCtrl |= B_PCH_ACPI_GPE_CNTL_SWGPE_CTRL;
  gSmst->SmmIo.Io.Write (&gSmst->SmmIo, SMM_IO_UINT8, (PmBase + R_PCH_ACPI_GPE_CNTL), 1, &GpeCtrl);
  return EFI_SUCCESS;
}

EFI_STATUS
MemOsNotify (
  IN  UINT8                  Socket,
  IN  ONLINE_OFFLINE_STATE  *MemState
)
//
//  Description
//    Notify OS through SCI for the RAS event
//
//  Parameters
//    Socket     - Socket index
//    MemState   - Memory state array
//
//  Returns
//    EFI_SUCCESS
//
{
  EFI_STATUS             Status = IN_PROGRESS;
  STATIC INTERNAL_STATE  StateMachine = EnumOsNotifyInit;

  Status = IN_PROGRESS;
  switch (StateMachine) {
  case EnumOsNotifyInit:
    RAS_DEBUG ((RAS_DBG_LEVEL(3), "MemOnlineOffline - OS notification started\n"));
    GenerateRasSci ();
    StateMachine = EnumOsNotifyInit;
    break;

  case EnumOsNotifyWait:
    RAS_DEBUG ((RAS_DBG_LEVEL(3), "MemOnlineOffline - Waiting OS to complete SCI\n"));
    StateMachine = EnumOsNotifyDone;
    break;

  case EnumOsNotifyDone:
  default:
    RAS_DEBUG ((RAS_DBG_LEVEL(3), "MemOnlineOffline - OS notification completed\n"));
    Status = STATUS_DONE;
    StateMachine = EnumOsNotifyInit;
    break;
  }

  return  Status;
}


EFI_STATUS
MemOnlineOffline (
  IN  UINT8                   SocketId,
  IN  ONLINE_OFFLINE_STATE   *MemState
)
//
//  Description
//    This routine handle the memory online/offline behind a socket
//
//  Parameters
//    SocketId      - Socket index for the onlining/offlining request
//    MemState      - Memory state (online/offline) array for the request of eitheir one or both
//
//  Returns
//    EFI_SUCCESS
//
{
  EFI_STATUS              Status;
  EFI_STATUS              RetStatus = STATUS_DONE;
  STATIC UINT8            Action;
  STATIC BOOLEAN          IsRiserPair;
  STATIC INT8             Branch = ALL_BRANCHES;
  STATIC UINT32           MemRasError = RAS_NO_ERROR;
  STATIC INTERNAL_STATE   StateMachine = EnumMemoryOnlineOfflineEntry;

  Status = IN_PROGRESS;

  do {
  switch (StateMachine) {
  case EnumMemoryOnlineOfflineEntry:

    StateMachine = EnumMemorySwitchBranch;

  case EnumMemorySwitchBranch:

    StateMachine  = EnumMemoryOnlining;

  case EnumMemoryOnlining:
    StateMachine  = EnumMemoryUpdate;
    Action = (MEM_NODE_ON | MEM_MAP_UPDATE);

  case EnumMemoryUpdate:
    //
    // Only update the SAD/TAD and ACPI for the last branch
    // It should be a single call for the SAD/TAD programming
    //
    DEBUG((EFI_D_LOAD|EFI_D_INFO, "\nEnumMemoryUpdate[%d] Update SAD TAD MEM MAP on Skt%d. Action = %02x\n",
           EnumMemoryUpdate, SocketId, Action));
    RetStatus = OnlineOfflineMemoryLibCall(Action, SocketId, Branch);
    if (RetStatus != IN_PROGRESS) {
      // Either completed or error occured?
      if (RetStatus != STATUS_DONE) {
        // Assume SAD/TAD are unchanged!
        if (Action & MEM_NODE_ON) {
          DEBUG((EFI_D_LOAD|EFI_D_INFO, "\t\t\tSAD TAD ERROR! Ready to power-off!\n"));
          Action &= ~(MEM_NODE_INIT | MEM_MAP_UPDATE);
          Action |= MEM_NODE_OFF;
          if (MemState[0] == EnumOnline) MemRasError |= BIT0;
          if (MemState[1] == EnumOnline) MemRasError |= BIT1;
          Branch = 0;
          StateMachine = EnumMemoryPowerOff;
        }else {
          StateMachine = EnumMemoryOnlineOfflineDone;
        }

        break;
      } // ERROR ENCOUNTED

      //
      // At this point, either success all or partial success!
      //
      if (Action & MEM_NODE_OFF) {
        // Offline event is generated from Bios ACPI. Expect OS has acknowledged about memory offline!
        Action &= ~MEM_MAP_UPDATE;
        Action |= MEM_NODE_OFF;
        StateMachine = EnumMemoryPowerOff;
      }else {
        if (mNotifyOs) {
          // Need to notify OS about new memory added to the system!
          UpdateAcpiMemoryParams(SocketId, ALL_BRANCHES, FALSE);
        }

        if (MemRasError == RAS_NO_ERROR) {
          StateMachine = EnumMemoryOnlineOfflineDone;
        }else {
          // Init Branch for power-off!
          // Either one of two risers passes!
          if (MemRasError & BIT0) Branch = 0;
          if (MemRasError & BIT1) Branch = 1;
          Action &= ~MEM_MAP_UPDATE;
          Action |= MEM_NODE_OFF;
          StateMachine = EnumMemoryPowerOff;
        }
      }
    } // IN_PROGRESS

    break;

  case EnumMemoryOfflining:
    DEBUG((EFI_D_LOAD|EFI_D_INFO, "ERROR State EnumMemoryOfflining[%d] Offline MEM on Skt%d\n", EnumMemoryOfflining, SocketId));
    Status = EFI_UNSUPPORTED;
    break;

  case EnumMemoryPowerOff:
    DEBUG((EFI_D_LOAD|EFI_D_INFO, "EnumMemoryPowerOff[%d] Power off MEM(s) on Skt%d. Action = %02x\n", EnumMemoryPowerOff, SocketId, Action));
    StateMachine = EnumMemoryOnlineOfflineDone;

  case EnumMemoryOnlineOfflineDone:
    if (MemRasError == RAS_NO_ERROR) Status = STATUS_DONE;
    else if (Action & MEM_NODE_ON) Status = RAS_MEM_ERR_ONLINE | MemRasError;
    else Status = RAS_MEM_ERR_OFFLINE | MemRasError;

    DEBUG((EFI_D_LOAD|EFI_D_INFO, "EnumMemoryOnlineOfflineDone[%d] Completed MEM online/offline for Skt%d. RET STATUS = %08x\n",
          EnumMemoryOnlineOfflineDone, SocketId, Status));

  default:
    StateMachine = EnumMemoryOnlineOfflineEntry;
    break;
   }
 }while (StateMachine != EnumMemoryOnlineOfflineEntry && RetStatus != IN_PROGRESS);

  return Status;
}

EFI_STATUS
CheckMemoryRequest (
  IN OUT UINT8    *Socket,
  IN OUT UINT8    *Branch,
  IN OUT BOOLEAN  *RiserPair
  )
//
//  Description
//    Check the riser requests for onlining/offlining
//
//  Parameters
//    Socket        - Pointer to the socket index when return, 0xFF means invalid request
//    Branch        - Pointer to the branch index when return, 0xFF means invalid request
//    SocketRequest - Pointer to indicate if it is a request for riser pair
//
//  Returns
//    EFI_SUCCESS   - Always
//
{
  EFI_STATUS Status = NO_REQUEST;
  UINT8 i;
  UINT8 NodeRequestMap;
  PSYSHOST host = (PSYSHOST)mMemRas->pSysHost;

  *Branch    = REQ_INVALID;
  *Socket    = REQ_INVALID;
  if (mIsSoftSmiRequest) {
    // Event from ACPI bios for MRAS, using mSmiParam[] for determining specific event
    if (mSmiParam[0] == SMI_MEM_ONLINE_OFFLINE) {
      // Determine which socket and branch being requested
      NodeRequestMap = (UINT8)((mSmiParam[1] & MEM_RAS_FLAG_NODE_IDX1) >> 8);
      if (NodeRequestMap) {
        // Find the Socket and Branch number
        for (i = 0; i < MC_MAX_NODE; i++) {
          if ((1 << i) & NodeRequestMap) {
            *Socket = i / MAX_IMC;
            *Branch = i % MAX_IMC;
            break;
          }
        }
    // Check for Cpu Present
        if ((host->var.common.socketPresentBitMap & (BIT0 << *Socket)) == 0) {
          Status = NO_REQUEST;
          DEBUG((EFI_D_LOAD|EFI_D_INFO, "ERROR: Invalid socket for memory request S%dM%d\n", *Socket, *Branch));
        } else {
        // Determine MEM_REQ_BOTH_BR from ACPI BIOS data exchange
        *RiserPair = (BOOLEAN)((mSmiParam[1] >> 7) & 1);
        Status = EFI_SUCCESS;
        DEBUG((EFI_D_LOAD|EFI_D_INFO, "\t\t\t - (USER GUI MC event) S%dM%d\n", *Socket, *Branch));
      }
      }
    }else if (mSmiParam[0] == SMI_CPU_ONLINE_OFFLINE || mSmiParam[0] == SMI_CPU_HOT_ADD_REMOVE) {
      // Event from ACPI bios for CPU RAS
      *Socket = mSmiParam[2] & 0xf;
      *Branch = 0;
      *RiserPair = TRUE;
      Status = EFI_SUCCESS;
      DEBUG((EFI_D_LOAD|EFI_D_INFO, "\t\t\t - (USER GUI SKT event) Skt%d\n", *Socket));
    }
  }else {
    // Event from SMI signal for socket online/offline, using mHpRequest[] only (mSmiParam[0] is irrelevant).
    if (mHpRequest[0] & 0xf) {
      for (i = 0; i < MAX_SOCKET; i++) {
        if (((UINT32)1 << i) & host->var.common.socketPresentBitMap) continue;
        if (((UINT8)1 << i) & mHpRequest[0]) {
          *RiserPair = TRUE;
          Status = EFI_SUCCESS;
          break;
      }
      }

      if (i == MAX_SOCKET) {
        // Determine whether the trigger from RC2 interface for online riser?
        // CPU OL Status = CPU Present? Yes, no RC2 event!
        if (mHpRequest[0] == mHpRequest[3]) {
          Status = NO_REQUEST;
        }else {
          Status = EFI_SUCCESS;
          if (mHpRequest[2] != 0) {
            *RiserPair = FALSE;
            for (i = 0; i < MC_MAX_NODE; i++) {
              if (((1 << i) & mHpRequest[2])) {
                break;
              }
            }
          }else {
            *RiserPair = TRUE;
            for (i = 0; i < MAX_SOCKET; i++) {
              // Skip if CPU OL Status has no change!
              if (((UINT8)1 << i) & mHpRequest[0]) continue;

              // If CPU present, then RC2 triggered from that CPU for online riser
              if (((UINT8)1 << i) & mHpRequest[3]) break;
            }
          }
        }
      }

      if (Status == EFI_SUCCESS) {
        if (*RiserPair == TRUE) {
        *Branch = 0;
        *Socket = i;
        host->var.common.socketPresentBitMap |= ((UINT32)1 << i);
        DEBUG((EFI_D_LOAD|EFI_D_INFO, "\t\t\t   - (SMI signal SKT event) - Skt%d. mHpRequest = %02x (2 Risers Detected)\n", *Socket, mHpRequest[0]));
        }else {
          *Branch = NODE_TO_MC(i);
          *Socket = NODE_TO_SKT(i);
          DEBUG((EFI_D_LOAD|EFI_D_INFO, "\t\t\t   - (SMI signal SKT event) - S%dMC%d. mHpRequest = %02x (1 Riser Detected)\n", *Socket, *Branch, mHpRequest[2]));
        }
      }
    }
  }

  if (Status == NO_REQUEST)
    DEBUG((EFI_D_LOAD|EFI_D_INFO, "MemOnlineOffline - No memory RAS request\n"));

  return Status;
}



EFI_STATUS
MemOnlineOfflineRasHandler(
  VOID
  )
//
//  Description
//    Memory online/offline child handler
//    It can also handle riser pair online or offline
//
//  Parameters
//    None
//
//  Returns
//    STATUS_DONE
//    IN_PROGRESS
//    NO_REQUEST
//
{
  EFI_STATUS              Status;
  STATIC  UINT8           Socket = 0xFF;
  STATIC  UINTN           MemRasError;
  STATIC  ONLINE_OFFLINE_STATE MemState[MAX_IMC];
  STATIC  INTERNAL_STATE       MemStateMachine = EnumMemoryEntry;
  UINT8                        node;

  DEBUG_ACCESS(0x9FFF1);

  Status = IN_PROGRESS;
  if(mRasMpLink->HostParmValid) {
    Socket = mRasMpLink->Host->var.common.inComingSktId;
  }

  switch (MemStateMachine) {
  case EnumMemoryEntry:
    DEBUG((EFI_D_LOAD|EFI_D_INFO, "MemOnlineOfflineRasHandler - EnumMemoryEntry\n"));
    if (EFI_ERROR(CheckMrcSetupOption (EnumMemoryOnlineOfflineEntry))) {
      DEBUG((EFI_D_LOAD|EFI_D_INFO, "ERROR: Invalid MRC setup options\n"));
      Status = RAS_MEM_ONOFFLINE_INVALID_SETUP;
      break;
    }

  case EnumMemoryCheckRequest:
    mForceRequest = FALSE;
    mNotifyOs = TRUE;
    if (mIsSoftSmiRequest) {

      if (mSmiParam[1] & MEM_RAS_FLAG_NO_OS_CORP)
        mForceRequest = TRUE;

      // Note: for offline, expect ACPI bios should set MEM_RAS_FLAG_NO_OS_SCI bit
      if (mSmiParam[1] & MEM_RAS_FLAG_NO_OS_SCI)
        mNotifyOs = FALSE;
    }

    MemRasError = RAS_NO_ERROR;

  case EnumMemoryGetState:
    for (node = 0; node < MAX_IMC; node++) {
      MemState[node] = EnumOnline;
    }
    MemStateMachine = EnumMemoryExecuteRequest;

  case EnumMemoryExecuteRequest:
    //
    // Call onlining or offlining state machine via MRAS protocol
    //
    Status = MemOnlineOffline (Socket, MemState);
    if (Status != IN_PROGRESS) {
      DEBUG((EFI_D_LOAD|EFI_D_INFO, "\n\n**** MEMORY INIT FLOW HAS COMPLETED!! STATUS = %08x **** \n\n", Status));
      if (Status != STATUS_DONE) {
        DEBUG((EFI_D_LOAD|EFI_D_INFO, "Error Abort!\n\n"));
        MemRasError = Status;
        MemStateMachine = EnumMemoryExit;
      } else {
        DEBUG((EFI_D_LOAD|EFI_D_INFO, "Ready to notify OS ACPI\n\n"));
        MemStateMachine = EnumMemoryNotifyOS;
      }
    }

    Status = IN_PROGRESS;
    break;

  case EnumMemoryNotifyOS:
    //
    // Send SCI to OS for RAS event handling
    //
    DEBUG((EFI_D_LOAD|EFI_D_INFO, "MemOnlineOfflineRasHandler - EnumMemoryNotifyOS[mNotifyOs = %d]\n", mNotifyOs));
    if (mNotifyOs) {
      Status = MemOsNotify (Socket, MemState);
      if (Status != IN_PROGRESS) {
        if (Status != STATUS_DONE) {
          MemRasError |= RAS_MEM_ERR_NOTIFY_OS;
        }

        MemStateMachine = EnumMemoryExit;
      }
    }else {
      MemStateMachine = EnumMemoryExit;
    }

  case EnumMemoryExit:

    DEBUG((EFI_D_LOAD|EFI_D_INFO, "\n\n  MEMORY ONLINE FLOW HAS COMPLETED!!\n"));
    DEBUG((EFI_D_LOAD|EFI_D_INFO, "  MEMORY ONLINE FLOW HAS COMPLETED!!\n"));
    DEBUG((EFI_D_LOAD|EFI_D_INFO, "  MEMORY ONLINE FLOW HAS COMPLETED!!\n\n\n"));

  default:
    MemStateMachine = EnumMemoryEntry;
      Status = STATUS_DONE;
    break;
  }

  return Status;
}



//====================================================================================
// Memory RAS SMI request parameters
// SMI[0] :  RAS event request type
// SMI[1] :  RAS event specific flags
// SMI[2] :  RAS event specific flags
// SMI[3] :  RAS event status code
//
//------------------------------------------------------------------------------------
// RAS hot add / remove
// SMI[0] :  1 - Hot add/remove
// SMI[1] :  Flags & Nodes
//           [07:00]: Node Index
//           [23:08]: Reserved
//           [25:24]: Operation code
//                    00-Toggle   01-Force online   10-Force online   11-Force offline
//           [26:26]: Reserved
//           [27:27]: 0: The index specified in Param2 is the physical node index
//                    1: The index specified in Param2 is the node index per OS perspective
//                    It may be different form physical node index in case of migration
//           [28:28]: Keep power always on
//                    0 - Power off the offlined riser after offlining
//                    1 - Keep the power of the offlined riser on after offlining
//           [29:29]  0: Update ACPI data block and notify OS
//                    1: Do not update ACPI data block or notify OS
//           [30:30]: Single riser or both risers
//                    0  - O*L the single riser
//                    1  - O*L the riser pair behind a socket
//           [31:31]: With or Witout OS coordination
//                    0 - Onlining or offlining with OS coordination
//                    1 - Force onlining or offlining without OS coordination
//------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------
// RAS Mirror replacement
// SMI[0] :  3 - Mirror
// SMI[1] :  Operation & Nodes
//           [07:00]: Node Index 0
//           [15:08]: Node Index 1
//           [23:16]: Reserved
//           [25:24]: Operation code
//                    00 - Mirror replacement (offline): It cuts off the mirroring pair and
//                         offlines one of the specified node.  Node0/node1 should be the
//                         mirrored pair and node1 will be the node going to be offlined.
//                    01 - Mirror restoration (online): Node0 should be one of the previous
//                         mirrored node and node1 should be a spare offlined node which will
//                         be used to restore the original mirroring.
//                    10 - Force failover (redirection):  It forces the master failover.
//                         Node0/node1 should be the mirrored pair
//           [27:26]: Reserved
//           [28:28]: Keep power on
//                    0  - Power off the riser to be replaced after cutting off the mirroring
//                    1  - Keep the power of the riser to be replaced on after cutting off
//                         the mirroring
//           [31:29]: Reserved
//------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------


EFI_STATUS
MemRasHandler(
  VOID
  )
//
//  Description
//    This function is the top handler for all memory related RAS events
//    It will handle:
//      OnlineOffline    : Hot add or remove memory nodes
//      SpareMigrate     : Migrate memory from an active node to a spare node
//      MirrorReplacement: Cut off or rebuild mirroring node pairs
//
{
  EFI_STATUS        Status = NO_REQUEST;
  STATIC EFI_STATUS Progress = NO_REQUEST;
  BOOLEAN           UpdateStatus;
  UINT8             i = 0;
  UINT64            LastCounter;
  STATIC UINT64     Counter;
  PSYSHOST          host = (PSYSHOST)mMemRas->pSysHost;

  UpdateStatus = TRUE;
  Status = IN_PROGRESS;
  //
  //Uncomment the following line to enable MRC serial output during Runtime.
  //
#ifdef SERIAL_DBG_MSG
  host->var.common.globalComPort = host->setup.common.consoleComPort;
#endif

 if (mIsSoftSmiRequest) {
    //
    // SMI request invoked by inverting GPI
    // BIOS will handle all memory RAS event types through GPI inversion
    //
    if (mSmiParam[0] == SMI_MEM_ONLINE_OFFLINE ||
        mSmiParam[0] == SMI_CPU_ONLINE_OFFLINE ||
        mSmiParam[0] == SMI_CPU_HOT_ADD_REMOVE) {
      //
      // Platform MRAS event always comes from ACPI bios unless new socket is online!
      //
      DEBUG((EFI_D_LOAD|EFI_D_INFO, "\n\tMemRasHandler::OS SMI Online - START  MemOnlineOfflineRasHandler\n"));
      if ((Status = MemOnlineOfflineRasHandler ()) == NO_REQUEST)
        return Status;
    } else {
      return NO_REQUEST;
    }
  } else  {
    //
    // Determine whether RAS event signal is for MRAS socket online?
    // check SMI event?
    //
    if (Progress == NO_REQUEST) {
      if (mHpRequest[0] & 0xf) {
        for (i = 0; i < MAX_SOCKET; i++) {
          if (((UINT32)1 << i) & host->var.common.socketPresentBitMap) continue;
          if (((UINT8)1 << i) & mHpRequest[0]) {
            mCurrentMemSocket = i; //Save the SocketNumber
            Progress = IN_PROGRESS;
            break;
          }
        }
      }

      if (i == MAX_SOCKET) {
        // Determine whether the trigger from RC2 interface for online riser?
        // CPU OL Status = CPU Present? Yes, no RC2 event!
        if (mHpRequest[0] == mHpRequest[3]) {
          // INVALID REQUEST!
          DEBUG((EFI_D_LOAD|EFI_D_INFO, "\tMemRasHandler::STATUS = NO_REQUEST\n"));
          return NO_REQUEST;
        }else {
          // Determine it from attention button pressed AND RC2 event?
          // mHpRequest[2] = MemBrdAttnBtn
          // mHpRequest[5] = MemBrdPresent
          if (mHpRequest[2] != 0) { // attention present event occurs?
            for (i = 0; i < MC_MAX_NODE; i++) {
              if (((1 << i) & mHpRequest[2]) && !((1 << i) & mHpRequest[5])) {
                DEBUG((EFI_D_LOAD|EFI_D_INFO, "\tMemRasHandler::Attention button pressed on N%d\n", i));
                i = NODE_TO_SKT(i);
                break;
              }

              if (i == MC_MAX_NODE) {
                DEBUG((EFI_D_LOAD|EFI_D_INFO, "\tMemRasHandler::STATUS = NO_REQUEST\n"));
                return NO_REQUEST;
              }
            }
          }else {
            for (i = 0; i < MAX_SOCKET; i++) {
              // Skip if CPU OL Status has no change!
              if (((UINT8)1 << i) & mHpRequest[0]) continue;

              // If CPU present, then RC2 triggered from that CPU for online riser
              if (((UINT8)1 << i) & mHpRequest[3]) break;
            }

            if (i == MAX_SOCKET) {
              DEBUG((EFI_D_LOAD|EFI_D_INFO, "\tMemRasHandler::STATUS = NO_REQUEST\n"));
              return NO_REQUEST;
            }
          }

          Progress = IN_PROGRESS;
        }
      }
    }

    //
    // Online/offline both risers (if existed) associated with this online/offline socket
    //

    DEBUG((EFI_D_LOAD|EFI_D_INFO, "\n\nMemRasHandler::Signal SMI Online - START  MemOnlineOfflineRasHandler for Skt%d\n", mCurrentMemSocket));
    if ((Status = MemOnlineOfflineRasHandler ()) == NO_REQUEST)
      return Status;

    if (Status == RAS_MEM_ONOFFLINE_REDIRECT) {
      //
      // Require to add or remove a mirrored node
      // Redirect the service to mirror replacement handler through SMI RAS request
      //
      mIsSoftSmiRequest = TRUE;
      Status            = IN_PROGRESS;
    }
  }

  // Store event status
  if (UpdateStatus) {
    mSmiParam[3] = (UINT32)Status;
  }

  // Pause Patrol Scrub and CKE Low if memory RAS under-going
  if( (Status == IN_PROGRESS) && (mPatrolCKEStopped == FALSE) ) {
     mPatrolCKEStopped = TRUE;
  }

  LastCounter = Counter;
  if (Status != IN_PROGRESS) {
    Counter = 0;
    if( (mPatrolCKEStopped == TRUE) ) {  // Resume Patrol Scrub and CKE Low setting
       mPatrolCKEStopped = FALSE;
     }
  } else {
    Counter ++;
  }

  // DEBUG ((EFI_D_ERROR, "\nSMI Entry (%4d) %8duS  at  %8dus", LastCounter, (Exit - Entry) * 3 / 400,  Exit * 3 / 400));
  if (Status == STATUS_DONE) {
    Progress = NO_REQUEST;
    if (!mIsSoftSmiRequest) mExitImmediately = TRUE;
    }
  return Status;
}
