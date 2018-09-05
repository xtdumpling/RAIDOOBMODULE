/**
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c) 2008 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:
  KtiRas.c

Abstract:
  This driver implements the KTI RAS building block of the RAS reference code package. It reuses
  most of the KTIRC code; some routines that are inherently specific to RAS flow is rewritten in this
  driver. The KTIRC code is built as a library and used by this driver.

  It exposes the following APIs to other RAS modules:
    1. RasMpLinkCheckEvent - To check if a socket can be On/offlined
    2. RasMpLinkHandler - To handle O*L event. Can be time sliced to meet OS SMM requirement.
    3. RasMpLinkAdjustIpiList - To be called to inlcude or exclude the onlined socket in the IPI target list

  This driver relies on the following services from other modules.
    1. Quiesce services - To commit the register settings that need to be programmed under system Quiesce
    2. Platform Info service - To get the platform type; this is used only to load the KTI electrical values.
        (To facilitate using the same binary image on multiple platforms)
    3. Memory Allocation Services - To allocate buffer for storing various state machine information
    4. Reference Code Host structure (syshost) produced during early system boot

Note:
  This driver should not call any of the register access functions until mRasHost structure is initialized
  since they need this structure to be initialized.

**/
#include <SysFunc.h>
#include "KtiRas.h"
#include "KtiRasMain.h"
#include "KtiRasSupport.h"
#include <Protocol/RasMpLinkProtocol.h>

EFI_QUIESCE_SUPPORT_PROTOCOL *mQuiesceProtocol;
EFI_RAS_MPLINK_PROTOCOL      *mRasMpLink;
EFI_IIO_UDS_PROTOCOL         *mIioUds;
IIO_UDS                      *mIioUdsData;
EFI_CPU_CSR_ACCESS_PROTOCOL  *mCpuCsrAccess;
EFI_HP_IOX_ACCESS_PROTOCOL   *mHpIOXAccess;
SPS_RAS_NOTIFY_PROTOCOL      *mSpsRasNotifyProtocol;

UINT8  *mQuiesceBuf;        // Points to Quiesce driver's buffer area; not expected to change once initialized
UINT32 mQuiesceBufSize = 0; // Quiesce Buffer size; not expected to change once initialized

SYSHOST                  *mRasHost;           // Copy of Host structure on which driver will work
SYSHOST                  *mMemRasHost;        // Copy of Host structure from MemRas
KTI_SOCKET_DATA          *mRasSocketData;     // Contains the socket data collected during the O*L handling
KTI_SOCKET_DATA          *mRasSocketDataOrig; // Contains the socket data before the RAS event started
KTI_HOST_INTERNAL_GLOBAL *mKtiInternalGlobal;
BOOLEAN                  mClearShadowBuffer;  // Flag to keep track of if the shawdow buffer is dirty and needs to cleared

BOOLEAN               mInProgress = FALSE;                         // Flag to indicate the state machine is working on handling an event
RAS_MPLINK_EVENT_TYPE mPendingEvtType = RAS_MPLINK_EVNT_RESET_SM;  // Tracks the event type being handled
INT32                 mPendingSocType = RAS_MPLINK_SOCKET_TYPE_NA; // Tracks the socket type being handled
INT32                 mPendingSocId = RAS_MPLINK_SOCKET_ID_NA;     // Tracks the socket id being handled
UINT8                 mSbspId = 0xFF;                              // SBSP ID; can differ from the SBSP chosen during boot; can change across O*L events to support offline the SBSP itself

UINT32                    mNextMileStoneIdx = 0;      // Keeps track of the Milestone being executed for an O*L event
UINT32                    mRasMpLinkDelayCounter = 0; // A delay counter
QUIESCE_DATA_TABLE_ENTRY *mQuieseBufStart = NULL;
UINT64                    mExpiredTime;

#ifdef RAS_KTI_DEBUG
UINT32                       mCsrWriteCtr;
#endif

static EFI_SMM_CPU_SERVICE_PROTOCOL *mSmmCpuServiceProtocol;

#ifdef RAS_KTI_UNIT_TEST
UINT16                       SmbBase;
UINT32                       MmCfgBase;
#endif

//
// All the milestones to be invoked in time sliced manner to handle an O*L event
//
RAS_MPLINK_STATUS (*mRasMpLinkMileStones[])(VOID) = {
  RasMpLinkMileStone0,
  RasMpLinkMileStone1,
  RasMpLinkMileStone2,
  RasMpLinkMileStone3,
  RasMpLinkMileStone4,
  RasMpLinkMileStone5,
  RasMpLinkMileStone6,
  RasMpLinkMileStone7,
  RasMpLinkMileStone8,
  RasMpLinkMileStone9,
  RasMpLinkMileStone10,
  RasMpLinkMileStone11,
  RasMpLinkMileStone12,
  NULL
};


/**

  This function brings imcoming socket's PBSP out of spin loop in ParkNpsb in SEC to initialize PBSP's
  NEM host struct.

  Note:
    Only SMM Monarch must call this routine.

  @param struct sysHost    *host

  @retval EFI_SUCCESS - Success
  @retval other - failure

**/
EFI_STATUS
EFIAPI
RasInitializePipe (
  struct sysHost    *host
  )
{
  UINT8   socket;
  UINT32  NumberOfDwords;
  UINT32  CurrentDwordCount;
  UINT32  HostDataDword;
  UINT32  *Buffer;
  UINT32  bootMode;

  socket = (UINT8)mPendingSocId;
  bootMode = NormalBoot;

  NumberOfDwords = (sizeof (struct sysHost) / (sizeof (UINT32)) + 1);

  //
  // Control will come here only on SBSP
  // (non-SBSP's are looping on their local SR_MEMORY_DATA_STORAGE_DISPATCH_PIPE_CSR waiting for a jmp address)
  //
  Buffer = (UINT32 *) host;

  //
  // Do nothing if this socket is not present
  //
  if((host->var.common.socketPresentBitMap & (BIT0 << mPendingSocId)) != 0) {

    //
    // Send new stack pointer (that protects future host location)
    //
    WriteCpuPciCfgEx (host, socket, 0, SR_MEMORY_DATA_STORAGE_DATA_PIPE_CSR, (UINT32) host->var.common.HostAddress);
    WriteCpuPciCfgEx (host, socket, 0, SR_MEMORY_DATA_STORAGE_COMMAND_PIPE_CSR, PIPE_DISPATCH_SYNCH_PSYSHOST);
    WriteCpuPciCfgEx (host, socket, 0, SR_MEMORY_DATA_STORAGE_DISPATCH_PIPE_CSR, 0x1);

    KtiDebugPrintInfo0((host, KTI_DEBUG_INFO0, "Pass PIPE_DISPATCH_SYNCH_PSYSHOST, HostAddr %08X\n", host->var.common.HostAddress));

    while (ReadCpuPciCfgEx (host, socket, 0, SR_MEMORY_DATA_STORAGE_DISPATCH_PIPE_CSR) != 0x00);

    //
    // Send &PeiPipeSlaveInit
    //
    WriteCpuPciCfgEx (host, socket, 0, SR_MEMORY_DATA_STORAGE_DATA_PIPE_CSR, (UINT32)host->var.common.SlavePipeAddress);
    WriteCpuPciCfgEx (host, socket, 0, SR_MEMORY_DATA_STORAGE_DISPATCH_PIPE_CSR, 0x1);
    KtiDebugPrintInfo0((host, KTI_DEBUG_INFO0, "Pass PeiPipeSlaveInit\n"));
    while (ReadCpuPciCfgEx (host, socket, 0, SR_MEMORY_DATA_STORAGE_DISPATCH_PIPE_CSR) != 0x00);


    //
    // Just go
    //
    WriteCpuPciCfgEx (host, socket, 0, SR_MEMORY_DATA_STORAGE_DISPATCH_PIPE_CSR, 0x1);
    while (ReadCpuPciCfgEx (host, socket, 0, SR_MEMORY_DATA_STORAGE_DISPATCH_PIPE_CSR) != 0x00);

    //
    // Read previous error status flag from slave
    //
    if (ReadCpuPciCfgEx (host, socket, 0, SR_MEMORY_DATA_STORAGE_DATA_PIPE_CSR)) {
      host->var.mem.previousBootError |= 1;
      KtiDebugPrintInfo0((host, KTI_DEBUG_INFO0, "\nSocket %d indicates an uncorrectable error\n", socket));
    }

    //
    // Send host structure address (host)
    //
    WriteCpuPciCfgEx (host, socket, 0, SR_MEMORY_DATA_STORAGE_DATA_PIPE_CSR, host->var.common.HostAddress);
    WriteCpuPciCfgEx (host, socket, 0, SR_MEMORY_DATA_STORAGE_DISPATCH_PIPE_CSR, 0x1);

    KtiDebugPrintInfo0((host, KTI_DEBUG_INFO0, "Pass pointer to host\n"));

    while (ReadCpuPciCfgEx (host, socket, 0, SR_MEMORY_DATA_STORAGE_DISPATCH_PIPE_CSR) != 0x00);

    //
    // Send host structure size in DWORDs (rounded up) ((sizeof sysHost)/(sizeof DWORD) + 1) [# of DWORDS]
    //
    WriteCpuPciCfgEx (host, socket, 0, SR_MEMORY_DATA_STORAGE_DATA_PIPE_CSR, NumberOfDwords);
    WriteCpuPciCfgEx (host, socket, 0, SR_MEMORY_DATA_STORAGE_DISPATCH_PIPE_CSR, 0x1);

    KtiDebugPrintInfo0((host, KTI_DEBUG_INFO0, "Pass host structure size in DWORDs = %d\n", NumberOfDwords));

    while (ReadCpuPciCfgEx (host, socket, 0, SR_MEMORY_DATA_STORAGE_DISPATCH_PIPE_CSR) != 0x00);

    //
    // Send bootMode
    //
    WriteCpuPciCfgEx (host, socket, 0, SR_MEMORY_DATA_STORAGE_DATA_PIPE_CSR, bootMode);
    WriteCpuPciCfgEx (host, socket, 0, SR_MEMORY_DATA_STORAGE_DISPATCH_PIPE_CSR, 0x1);

    KtiDebugPrintInfo0((host, KTI_DEBUG_INFO0, "Pass boot mode\n"));

    while (ReadCpuPciCfgEx (host, socket, 0, SR_MEMORY_DATA_STORAGE_DISPATCH_PIPE_CSR) != 0x00);

    KtiDebugPrintInfo0((host, KTI_DEBUG_INFO0, "\nSynch of host structure to  Socket %x starting...\n", socket));

    for (CurrentDwordCount = 0; CurrentDwordCount < NumberOfDwords; CurrentDwordCount++) {
      HostDataDword = Buffer[CurrentDwordCount];

      //
      // Send DWORD[n] value
      //
      while (ReadCpuPciCfgEx (host, socket, 0, SR_MEMORY_DATA_STORAGE_DISPATCH_PIPE_CSR) != 0x00);
      WriteCpuPciCfgEx (host, socket, 0, SR_MEMORY_DATA_STORAGE_DATA_PIPE_CSR, HostDataDword);
      WriteCpuPciCfgEx (host, socket, 0, SR_MEMORY_DATA_STORAGE_DISPATCH_PIPE_CSR, 0x1);
    }

    KtiDebugPrintInfo0((host, KTI_DEBUG_INFO0, "Transfer complete \n"));

  }

  return EFI_SUCCESS;
}

/**

  This function release the PBSP to teadown it's NEM after memory initialized.

  Note:
    Only SMM Monarch must call this routine.

  @param struct sysHost    *host

  @retval none

**/
VOID
EFIAPI
RasExitPipe (
  struct sysHost    *host
  )
{
  //
  //  Do nothing but return if a reset request is pending
  //  Note: Since a reset will be triggered soon anyway, we keep pipe slaves looping on pipe command
  //        to avoid potentail issues with INVD instuction in TearDownNem() on a cold boot path
  //        when there are 4 CPU packages present and QPI RC has not yet fully completed the
  //        routings among non-SBSP sockets.
  //
  if (host->var.common.resetRequired)  return;

  //
  // if this socket is not present
  //
  if((host->var.common.socketPresentBitMap & (BIT0 << mPendingSocId)) != 0) {

    //
    // Send "Exit Pipe" command to the slave
    //
    WriteCpuPciCfgEx (host, (UINT8)mPendingSocId, 0, SR_MEMORY_DATA_STORAGE_DISPATCH_PIPE_CSR, PIPE_DISPATCH_EXIT);
    //
    // Wait for the slave to complete so we can make sure all non-BSP sockets are done
    //
    while (ReadCpuPciCfgEx (host, (UINT8)mPendingSocId, 0, SR_MEMORY_DATA_STORAGE_DISPATCH_PIPE_CSR) != PIPE_DISPATCH_NULL);

  }

  return;
}

/**

  This function prepare the Host data structure to be used for PBSP to do memory initialization.

  Note:
    Only SMM Monarch must call this routine.

  @param struct sysHost    *host

  @retval EFI_SUCCESS - Success
  @retval other - failure

**/
EFI_STATUS
EFIAPI
RasInitializeHostForMemOL (
  struct sysHost    *host
  )
{
  //
  //  1. add code to init the mRasHost before InitlaizePipe for incoming socket's mem init
  //  2. update serialDebugMsgLvl once we have way to sync print between SBSP in SMM and
  //     PBSP in NEM
  //  3. Replace sbspSocketId hardcode
  //
  host->var.common.MemRasFlag    = 1;
  host->var.common.inComingSktId = (UINT8)mPendingSocId;
  host->var.common.bootMode      = NormalBoot;
  host->var.common.SystemRasType = ADVANCED_RAS;
  host->var.mem.socket[mPendingSocId].imcEnabled[0] = 1;
  host->var.mem.socket[mPendingSocId].imcEnabled[1] = 1;
  host->nvram.common.sbspSocketId  = mSbspId;

  return EFI_SUCCESS;
}


/**

  This function command PBSP to start memory initialization.

  Note:
    Only SMM Monarch must call this routine.

  @param struct sysHost    *host

  @retval EFI_SUCCESS - Success
  @retval other - failure

**/
EFI_STATUS
EFIAPI
RasStartMemInit (
  struct sysHost    *host
  )
{
  if((host->var.common.socketPresentBitMap & (BIT0 << mPendingSocId)) != 0) {
    //
    // Wait PBSP done Host copy
    //
    while (ReadCpuPciCfgEx (host, (UINT8)mPendingSocId, 0, SR_MEMORY_DATA_STORAGE_DISPATCH_PIPE_CSR) != 0x00);

    KtiDebugPrintInfo0((host, KTI_DEBUG_INFO0, "Relase PBSP to start Mem init\n"));

    WriteCpuPciCfgEx (host, (UINT8)mPendingSocId, 0, SR_MEMORY_DATA_STORAGE_DISPATCH_PIPE_CSR, PIPE_DISPATCH_MEMORY_INIT);
  }

  return EFI_SUCCESS;

}

/**

  This function check if PBSP finished it's Mem init and handle ....

  Note:
    Only SMM Monarch must call this routine.

  @param struct sysHost    *host

  @retval TRUE - if MEM init done
  @retval FALSE - if MEM init has not done yet

**/
BOOLEAN
EFIAPI
RasMemInitDoneCheckAndHandle (
  struct sysHost    *host
  )
{
  BOOLEAN   status = FALSE;
  UINT8     socket;
  UINT32    MemInitStage = 0;

  socket = (UINT8)mPendingSocId;

  MemInitStage = ReadCpuPciCfgEx (host, (UINT8)socket, 0, SR_MEMORY_DATA_STORAGE_DISPATCH_PIPE_CSR);
  if(MemInitStage != 0) {
    WriteCpuPciCfgEx (host, socket, 0, SR_MEMORY_DATA_STORAGE_DISPATCH_PIPE_CSR, 0); //release PBSP
  }

  switch(MemInitStage) {
    case PIPE_DISPATCH_DETECT_DIMM_DONE:
      GetSlaveData(host);
      break;
    case PIPE_DISPATCH_CHECK_STATUS:
      SendStatus(host);
      break;
    case PIPE_DISPATCH_DIMMRANK_DONE:
      SendData(host);
      break;
    case PIPE_DISPATCH_MEMINIT_DONE:
      //get PBSP mem init error info
      host->var.common.socketId = mSbspId;
      SyncErrors(host);

      //get PBSP nvRam data
      host->var.common.rcWriteRegDump = 0;
      GetPipePackage(host, socket, (UINT8*)&host->nvram.mem.socket[socket], sizeof(struct socketNvram));

      status = TRUE;
      break;
    default:
      break;
  }

  return status;
}

/**

  KTI RAS driver RC entry point that initializes the driver and exposes the APIs to be called during runtime to
  handle a O*L event. This entry must be called only once during boot.

  Note:
    Only SBSP must call this routine.

  @param ImageHandle
  @param SystemTable

  @retval EFI_SUCCESS - Success
  @retval other - failure

**/
EFI_STATUS
EFIAPI
InitializeRasMpLink (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_HANDLE                Handle = NULL;
  EFI_STATUS                Status = EFI_SUCCESS;
  EFI_HOB_GUID_TYPE         *GuidHob;
  struct sysRasHost         *RasHost;

  //
  // Make sure the RAS driver is not initialized already
  //
  Status = gSmst->SmmLocateProtocol (&gEfiRasMpLinkProtocolGuid, NULL, &mRasMpLink);
  if (!EFI_ERROR (Status)) {
    return EFI_SUCCESS;
  }

  //
  // Obtain Quiesce buffer address & size
  //
  Status = gSmst->SmmLocateProtocol (&gEfiQuiesceProtocolGuid, NULL, &mQuiesceProtocol);
  ASSERT_EFI_ERROR (Status);
  Status = mQuiesceProtocol->GetQuiesceDataMemoryInfo((UINTN *)&mQuiesceBuf, (UINTN *)&mQuiesceBufSize);
  DEBUG((EFI_D_ERROR, "\n  InitializeRasMpLink() mQuiesceBuf: 0x%08X  mQuiesceBufSize: 0x%X \n", mQuiesceBuf, mQuiesceBufSize));
  ASSERT_EFI_ERROR (Status);


  Status = gSmst->SmmLocateProtocol (&gEfiHpIoxAccessGuid, NULL, &mHpIOXAccess);
  ASSERT_EFI_ERROR (Status);

  Status = gSmst->SmmLocateProtocol (&gSpsRasNotifyProtocolGuid, NULL,&mSpsRasNotifyProtocol);

  //
  // Allocate buffer for all the internal data structures and state machine
  //
  RasMpLinkAllocateBuffer ();

  //
  // Allocate the buffer for protocol data structure
  //
  Status = gSmst->SmmAllocatePool (EfiRuntimeServicesData, sizeof (EFI_RAS_MPLINK_PROTOCOL), &mRasMpLink);
  ASSERT_EFI_ERROR (Status);

  //
  // Allocate & clear the buffer for Host data structure
  //
  Status = gSmst->SmmAllocatePool (EfiRuntimeServicesData, sizeof (SYSHOST), &mRasMpLink->Host);
  ASSERT_EFI_ERROR (Status);
  MemSetLocal ((UINT8 *) mRasMpLink->Host, 0, sizeof (SYSHOST));

  //
  // Obtain the  SYSRASHOST structure produced during early boot
  //
  GuidHob = GetFirstGuidHob (&gEfiRasHostGuid);
  if (GuidHob == NULL) {
    ASSERT (FALSE);
    return EFI_NOT_FOUND;
  }
  RasHost = (SYSRASHOST *) GET_GUID_HOB_DATA (GuidHob);

  //
  // Copy SYSRASHOST to partial of SYSHOST
  //
  MemCopy ((VOID *)&(mRasMpLink->Host->setup.cpu), (VOID *)&(RasHost->setup.cpu), sizeof (struct cpuSetup));
  MemCopy ((VOID *)&(mRasMpLink->Host->setup.kti), (VOID *)&(RasHost->setup.kti), sizeof (KTI_HOST_IN));
  MemCopy ((VOID *)&(mRasMpLink->Host->setup.common), (VOID *)&(RasHost->setup.common), sizeof (struct commonSetup));

  MemCopy ((VOID *)&(mRasMpLink->Host->var.cpu), (VOID *)&(RasHost->var.cpu), sizeof (struct cpuVar));
  MemCopy ((VOID *)&(mRasMpLink->Host->var.kti), (VOID *)&(RasHost->var.kti), sizeof (KTI_HOST_OUT));
  MemCopy ((VOID *)&(mRasMpLink->Host->var.common), (VOID *)&(RasHost->var.common), sizeof (struct commonVar));

  mSbspId = mRasMpLink->Host->nvram.common.sbspSocketId = GetSbspSktId (NULL);
  mRasMpLink->HostParmValid = TRUE;

  //
  // Locate CpuCsrAccess protocol in order to update its variables during the onlining flow
  //
  Status = gSmst->SmmLocateProtocol (&gEfiCpuCsrAccessGuid, NULL, &mCpuCsrAccess);
  ASSERT_EFI_ERROR (Status);

  //
  // Obtain pointer to UDS strucutre to keep it up to date after a RAS event occurs
  //
  Status = SystemTable->BootServices->LocateProtocol (&gEfiIioUdsProtocolGuid, NULL, &mIioUds);
  ASSERT_EFI_ERROR (Status);
  mIioUdsData = (IIO_UDS *)mIioUds->IioUdsPtr;

  DEBUG((EFI_D_ERROR, "IIOUDS Reserved memory type address in RasMpLink is %x\n",mIioUdsData));
  DEBUG((EFI_D_ERROR, "The SbspID is %x\n",mSbspId));

  mRasMpLink->RasMpLinkCheckEvent= RasMpLinkCheckEvent;
#ifdef RAS_KTI_TIME_SLICE
  mRasMpLink->RasMpLinkHandler = RasMpLinkHandlerSliced;
#else
  mRasMpLink->RasMpLinkHandler = RasMpLinkHandlerNonSliced;
#endif
  mRasMpLink->RasMpLinkAdjustIpiList = RasMpLinkAdjustIpiList;
  mRasMpLink->RasMpLinkGetMemRasHost = RasMpLinkGetMemRasHost;

#ifdef RAS_KTI_DEBUG
  mRasMpLink->Host->setup.kti.DebugPrintLevel = 0x0F;  // Enable all the runtime debug message outputs
#else
  mRasMpLink->Host->setup.kti.DebugPrintLevel = 0x00;  // Disable all the runtime debug message outputs
#endif

  //
  // Install the protocol to expose the APIs
  //
  Status = gSmst->SmmInstallProtocolInterface (&Handle, &gEfiRasMpLinkProtocolGuid, EFI_NATIVE_INTERFACE, mRasMpLink);
  ASSERT_EFI_ERROR (Status);

  Status = gSmst->SmmLocateProtocol (&gEfiSmmCpuServiceProtocolGuid, NULL, &mSmmCpuServiceProtocol);
  ASSERT_EFI_ERROR (Status);

#ifdef RAS_KTI_UNIT_TEST
#if 0
  MmCfgBase = mRasMpLink->Host->var.common.mmCfgBase;
  //RegisterSwSmi ();
  CpuDeadLoop();
  MemCopy ((VOID *)mRasHost, (VOID *)mRasMpLink->Host, sizeof (SYSHOST));
  mRasHost->var.common.sbsp = mSbspId;
  KtiRasDebugHandler (0, 0, 0, 0);
#endif
#endif
  return EFI_SUCCESS;
}

/**

  To check if onlining/offlining a given socket can succeed. If the onlined socket is not reachable
  (i.e none of the existing sockets are immediate neighbors of the onlined socket)
  or if the socket to be offlined is part of a pass thru path and if it is the only path to reach other
  sockets in the system, then the socket online/offline request will be rejected.

  @param EventType     - KTI RAS event type
  @param SocketInfo    - Socket Type & ID affected by RAS event

  @retval RAS_MPLINK_SUCCESS   - Online/Offlining the socket is allowed
  @retval all other codes  - Online/Offlining is not allowed

**/
RAS_MPLINK_STATUS
RasMpLinkCheckEvent (
  IN UINT8 EventType,
  IN UINT8 SocketInfo
  )
{
  STATIC BOOLEAN      CheckEventPending = FALSE;
  RAS_MPLINK_STATUS   Status = RAS_MPLINK_SUCCESS;
  INT32               SocketType;
  INT32               SocketId;

#ifdef RAS_KTI_MEASURE_TIME
  GetExpiredTimeEntry (0);
#endif

  //
  // If the Shadow Buffer needs to be cleared, do it in multiple steps to meet SMM timing constraints
  //
  if (mClearShadowBuffer == TRUE) {
    if (RasMpLinkClearShadowCpuBufferInSteps() == RAS_MPLINK_SUCCESS) {
      mClearShadowBuffer = FALSE;
    }
#ifdef RAS_KTI_MEASURE_TIME
    GetExpiredTimeExit ();
#endif
    return RAS_MPLINK_PENDING;
  }

  // All the registers written by this step can be done without Quiesce
  mDirectCommit = TRUE;

  //
  // For offline event this step can be done in single call. For online events this needs to be called multiple times
  // since the link training and link parameter exchange process takes few SMIs to complete. Skip certain things
  // accordingly if this is not called for the first time.
  //

  if (CheckEventPending == FALSE) {
    if (SocketInfo != 0xFF) {
      SocketType = RAS_MPLINK_SOCKET_TYPE_CPU;
      SocketId = (SocketInfo & 0xFF);
    } else {
      return RAS_MPLINK_FAILURE_INVALID_EVTTYPE;
    }

    // Do some checking to make sure the parameters passed to the APIs are valid
    Status = ValidateParameters (EventType, SocketType, SocketId);
    if (Status != RAS_MPLINK_SUCCESS) {
      return Status;
    }

    if (mInProgress == TRUE) {
      KtiDebugPrintFatal((mRasMpLink->Host, KTI_DEBUG_ERROR, "\n    RasMpLink handler already working on Event: %01u  SocketID: %01u ", mPendingEvtType, mPendingSocId));
      return RAS_MPLINK_FAILURE_EVT_IN_PROGRESS;
    }

    Status = RasMpLinkStartStateMachine(EventType, SocketType, SocketId);
    ASSERT_EFI_ERROR(Status);

    Status = PrepareHostDataStruct ();
    ASSERT_EFI_ERROR(Status);
  }

  if (mPendingEvtType == RAS_MPLINK_EVNT_OFFLINE) {
    if (CollectLepForOfflineEvent () != RAS_MPLINK_SUCCESS) {
      KtiDebugPrintFatal((mRasMpLink->Host, KTI_DEBUG_ERROR, "\n    Collecting the LEP failed. "));
      Status = RAS_MPLINK_FAILURE_CANNOT_OFFLINE;
      goto ErrExitOnOffChk;
    }
    if (PreProcessKtiLinks(mRasHost, mRasSocketData, mKtiInternalGlobal) != KTI_SUCCESS) {
      KtiDebugPrintFatal((mRasMpLink->Host, KTI_DEBUG_ERROR, "\n    KTI link preprocessing failed. "));
      Status = RAS_MPLINK_FAILURE_CANNOT_OFFLINE;
      goto ErrExitOnOffChk;
    }

    if (KtiTopologyDiscovery(mRasHost, mRasSocketData, mKtiInternalGlobal) != KTI_SUCCESS) {
      KtiDebugPrintFatal((mRasMpLink->Host, KTI_DEBUG_ERROR, "\n    KTI topology sanity check failed. "));
      Status = RAS_MPLINK_FAILURE_CANNOT_OFFLINE;
    }
  }

ErrExitOnOffChk:
  RasMpLinkClearKtiInfoBuffer();
  RasMpLinkResetStateMachine();
#ifndef RAS_KTI_UNIT_TEST
//
// The following label becomes needed when the above #ifdef 0 gets resolved. Debugging in progress.
//
// ErrExitOnOffChk_1:

#endif
  mDirectCommit = FALSE;
#ifdef RAS_KTI_MEASURE_TIME
  GetExpiredTimeExit ();
#endif

  return Status;
}

/**

  Expose this API to other modules if the KTI RAS driver has to handle the RAS event with no time slicing.
  This routine calls the time sliced version of the handler repeatively.

  @param EventType     - KTI RAS event type
  @param SocketInfo    - Socket Type & ID affected by RAS event; set to NA if not applicable

  @retval RAS_MPLINK_SUCCESS   - Event handling completed successfully
  @retval RAS_MPLINK_PENDING   - Some more work is pending
  @retval RAS_MPLINK_FAILURE   - Event handling failed

**/
RAS_MPLINK_STATUS
RasMpLinkHandlerNonSliced (
  IN UINT8 EventType,
  IN UINT8 SocketInfo
  )
{
  RAS_MPLINK_STATUS       Status;

  do {
    Status = RasMpLinkHandlerSliced(EventType, SocketInfo);
  } while (Status == RAS_MPLINK_PENDING);

  return Status;
}

/**

  To be called for each O*L events that affects the KTI fabric. A KTI RAS event is handled in multiple phases
  depending on the event type. Each phase in turn has multiple milestones. In time sliced handling, a milestone
  is executed during each slice. If a milestone writes a register that doesn't require queisce it is directly written
  into the h/w register. Otherwise it is stored in the quiesce buffer to be programmed at the end of the phase
  it belongs to. At the end of successfull completion of each phase, the KTI RAS driver will call the Quiesce
  driver to program the register settings stored in the Quiesce buffer. hile the online event takes more than
  phase to handle, the offline event is handled in single phase (i.e single Quiesce).

  @param EventType     - KTI RAS event type
  @param SocketInfo    - Socket Type & ID affected by RAS event; set to NA if not applicable

  @retval RAS_MPLINK_SUCCESS                 - Event handling completed successfully
  @retval RAS_MPLINK_PENDING                 - Some more work is pending
  @retval RAS_MPLINK_FAILURE_EVT_IN_PROGRESS - RasMpLink event is already in progress (only one event is allowed at a time)
  @retval RAS_MPLINK_FAILURE_INVALID_EVTTYPE - Invalid event type is passed

**/
RAS_MPLINK_STATUS
RasMpLinkHandlerSliced (
  IN UINT8 EventType,
  IN UINT8 SocketInfo
  )
{
  RAS_MPLINK_STATUS       Status, Status1;
  INT32               SocketType;
  INT32               SocketId;

#ifdef RAS_KTI_MEASURE_TIME
  GetExpiredTimeEntryPeriod();
  GetExpiredTimeEntry (mNextMileStoneIdx);
#endif

  Status = RAS_MPLINK_SUCCESS;

  //
  // If the Shadow Buffer needs to be cleared, do it in multiple steps to meet SMM timing constraints
  //
  if (mClearShadowBuffer == TRUE) {
    if (RasMpLinkClearShadowCpuBufferInSteps() == RAS_MPLINK_SUCCESS) {
      mClearShadowBuffer = FALSE;
    }
#ifdef RAS_KTI_MEASURE_TIME
    GetExpiredTimeExit ();
#endif
    return RAS_MPLINK_PENDING;
  }

  // If any of the milestones request for a delay, obey the request
  if (mRasMpLinkDelayCounter ) {
    --mRasMpLinkDelayCounter;
#ifdef RAS_KTI_MEASURE_TIME
    GetExpiredTimeExit ();
#endif
    return RAS_MPLINK_PENDING;
  }

  // Handle the reset state machine request
  if (EventType == RAS_MPLINK_EVNT_RESET_SM) {
    KtiDebugPrintInfo0((mRasMpLink->Host, KTI_DEBUG_INFO0, "\n  ************ RasMpLink State Machine Reset ************"));
    if (mNextEntry) RasMpLinkReleaseQuiesceBuf();
    mClearShadowBuffer = TRUE;
    RasMpLinkClearKtiInfoBuffer();
    return RasMpLinkResetStateMachine();
  }

  if (SocketInfo != 0xFF) {
    SocketType = RAS_MPLINK_SOCKET_TYPE_CPU;
    SocketId = (SocketInfo & 0xFF);
  } else {
    SocketType = RAS_MPLINK_SOCKET_TYPE_NA;
    SocketId = RAS_MPLINK_SOCKET_ID_NA;
  }

  // Do some checking to make sure the parameters passed to the API is valid
  Status = ValidateParameters (EventType, SocketType, SocketId);
  if (Status != RAS_MPLINK_SUCCESS) {
    return Status;
  }

  //
  // If an event is already in progress, make sure this is a follow-up call
  //
  if ((mInProgress == TRUE) && ((EventType != mPendingEvtType) || (SocketType != mPendingSocType) || (SocketId != mPendingSocId))) {
    KtiDebugPrintFatal((mRasMpLink->Host, KTI_DEBUG_ERROR, "\n    RasMpLink handler already working on Event: %01u  SocketID: %01u ", mPendingEvtType, mPendingSocId));
    return RAS_MPLINK_FAILURE_EVT_IN_PROGRESS;
  }

  if (mInProgress != TRUE) {
    KtiDebugPrintInfo0((mRasMpLink->Host, KTI_DEBUG_INFO0, "\n  ************ RasMpLink Event Handler Entry ************"));
  } else {
    KtiDebugPrintInfo0((mRasMpLink->Host, KTI_DEBUG_INFO0, "\n  ************ RasMpLink Event Handler Entry (Follow-up Call) ************"));
  }
  KtiDebugPrintInfo0((mRasMpLink->Host, KTI_DEBUG_INFO0, "\n    EventType: %s  SocketID: %01u ",
          (EventType == RAS_MPLINK_EVNT_ONLINE) ? "ONLINE" : "OFFLINE", SocketId));


  switch (EventType) {
  case RAS_MPLINK_EVNT_ONLINE:
  case RAS_MPLINK_EVNT_OFFLINE:
    // Start the state machine if an event is not in progress already
    if (mInProgress == FALSE) {
      RasMpLinkStartStateMachine(EventType, SocketType, SocketId);
      RasMpLinkAcquaireQuiesceBuf();
    }

    //
    // If any Online event milestone fails to complete, restore all the registers and reset the state machine.
    // For offline event any milstone failure is a hard failure and the system can not be restored. But such
    // failure should not happen if the API to check if the offlining is possible is checked by the caller.
    //
    Status = (mRasMpLinkMileStones[mNextMileStoneIdx])();
    if (Status != RAS_MPLINK_SUCCESS) {
      KtiDebugPrintFatal((mRasMpLink->Host, KTI_DEBUG_ERROR, "\n    KTI RAS handler milestone %u failed to complete.", mNextMileStoneIdx));
      KtiDebugPrintFatal((mRasMpLink->Host, KTI_DEBUG_ERROR, "\n    O*L event handling aborted - Restoring the system and Reseting RasMpLink State Machine.\n"));
      if (EventType == RAS_MPLINK_EVNT_OFFLINE) {
        KtiDebugPrintFatal((mRasMpLink->Host, KTI_DEBUG_ERROR, "\n    Offlining failed - System Halted."));
        CpuDeadLoop();
      }
      if (mNextMileStoneIdx >= 1 && mNextMileStoneIdx <= 8) {
        CpuDeadLoop();
        RasMpLinkRestoreSadAndRta ();
        mDirectCommit = TRUE;
        PowerDownOnlinedSocket (mPendingSocId, mRasSocketDataOrig);
        mDirectCommit = FALSE;
      } else if (mNextMileStoneIdx >= 9 && mNextMileStoneIdx <= 12) {
        Status1 = RasMpLinkFailureHandler (mPendingSocType, mPendingSocId);
        ASSERT_EFI_ERROR(Status1);
      }
      if (mNextEntry) RasMpLinkReleaseQuiesceBuf();
      mClearShadowBuffer = TRUE;
      RasMpLinkClearKtiInfoBuffer();
      RasMpLinkResetStateMachine();
#ifdef RAS_KTI_MEASURE_TIME
      GetExpiredTimeExit ();
#endif
      return Status;
    }

    //
    // Advance to next milestone; if we are done with all milestones, reset the state machine & return success.
    // Otherwise return pending to indicate the caller that further calls are needed.
    //
    //  Assuming RasMpLinkAdjustIpiList() is called with the input argument Include == 2 as the very last call to KTI RAS code.
    //  Therefore, changed to not reset the state mashine here.
    //
    ++mNextMileStoneIdx;
    if (mRasMpLinkMileStones[mNextMileStoneIdx] == NULL) {
      KtiUpdateUdsStructure();
      //
      // Copy mRasHost back into mRasMpLink->Host which is exposed by the protocol
      //
      MemCopy ((VOID *)mRasMpLink->Host, (VOID *)mRasHost, sizeof(SYSHOST));

      //
      // For online, IPI domain remains to be handled properly.
      // For offline, IPI list is already updated and it is done here.
      //
      Status = (EventType == RAS_MPLINK_EVNT_ONLINE) ? RAS_MPLINK_INT_DOMAIN_PENDING : RAS_MPLINK_SUCCESS;
      if(EventType == RAS_MPLINK_EVNT_OFFLINE){
        RasMpLinkReleaseQuiesceBuf();
        mClearShadowBuffer = TRUE;
        RasMpLinkClearKtiInfoBuffer();
        RasMpLinkResetStateMachine();
      }
    } else {
      // Indicate that the handling is still in progress and further calls are needed
      Status = RAS_MPLINK_PENDING;
    }
    break;

  default:
    KtiDebugPrintFatal((mRasMpLink->Host, KTI_DEBUG_ERROR, "\n    Invalid Event Type passed to KTI RAS handler"));
    return RAS_MPLINK_FAILURE_INVALID_EVTTYPE;
  }

#ifdef RAS_KTI_MEASURE_TIME
  GetExpiredTimeExit ();
#endif

  KtiDebugPrintInfo0((mRasMpLink->Host, KTI_DEBUG_INFO0, "\n  ************ RasMpLink Event Handler Exit ************\n"));

  return Status;
}

/**

  To be called to restore the system setting should the RasMpLink handler milestone 1 - 8 fail to handle
  an Online event.

**/
RAS_MPLINK_STATUS
RasMpLinkRestoreSadAndRta (
  )
{
  ASSERT(0);
  return EFI_UNSUPPORTED;
}

/**

  To be called to restore the system setting should the RasMpLink handler milestone 9 - 12 fail to handle
  an Online event.

**/
RAS_MPLINK_STATUS
RasMpLinkFailureHandler (
  )
{
  RAS_MPLINK_STATUS       Status = RAS_MPLINK_SUCCESS;

  //
  // Adjust the state machine so that it can handle the offlining of the socket
  //
  RasMpLinkClearBuffer();
  mClearShadowBuffer = FALSE;
  mPendingEvtType = RAS_MPLINK_EVNT_OFFLINE;
  mNextEntry= mQuieseBufStart;
#ifdef RAS_KTI_DEBUG
  mCsrWriteCtr = 0;
#endif
  mDirectCommit = FALSE;
  mNextMileStoneIdx = 0;

  while (mRasMpLinkMileStones[mNextMileStoneIdx] != NULL) {
    Status = (mRasMpLinkMileStones[mNextMileStoneIdx])();
    if (Status != RAS_MPLINK_SUCCESS) {
      KtiDebugPrintFatal((mRasMpLink->Host, KTI_DEBUG_ERROR, "\n    KTI RAS handler milestone %u failed to complete during RAS event restore.", mNextMileStoneIdx));
      return Status;
    }
    if (mClearShadowBuffer == TRUE) {
      while (RasMpLinkClearShadowCpuBufferInSteps() != RAS_MPLINK_SUCCESS);
      mClearShadowBuffer = FALSE;
    }
    ++mNextMileStoneIdx;
  }

  return Status;
}

/**

  To be called to include/exclude the onlined socket in IPI target list of the SBSP
  By this time of flow, the o*line ras operation is considered finished as far as KTI is concerned.
  As internal structures have been cleared, use mRasMpLink->Host when needed.

  Use Quiesce here.

  Assuming this function is called with the input argument Include == 2 as the very last call to KTI RAS code.
  Therefore, Restting the state mashine is moved to here from Milestone12.

  @param Include     - TRUE: If the onlined socket to be included; FALSE otherwise.

  @retval RAS_MPLINK_SUCCESS         - Completed successfully
  @retval RAS_MPLINK_FAILURE_GENERAL - Failure

**/
RAS_MPLINK_STATUS
RasMpLinkAdjustIpiList (
  IN UINT8 Include
  )
{
  UINT32          Data32;
  UINT8           Idx1;

  KtiDebugPrintInfo0((mRasMpLink->Host, KTI_DEBUG_INFO0, "\n  RasMpLink Adjust SBSP's IPI target list"));

  if (!(mRasHost->var.common.emulation & SIMICS_FLAG)) {
    mDirectCommit = FALSE;
  } else {
    KtiDebugPrintInfo0 ((mRasMpLink->Host, KTI_DEBUG_INFO0, "\n  Simics Quiesce Entry "));
  }

  //
  // Adjust the SBSP's IPI target list
  //
  if (Include == 0) {
    Data32 = ReadCpuPciCfgRas(mRasMpLink->Host, mSbspId, 0, BCASTLIST_UBOX_CFG_REG);
    KtiDebugPrintInfo1 ((mRasMpLink->Host, KTI_DEBUG_INFO0, "\n   Include = %u Read Data32 = 0x%08X", Include, Data32));
    Data32 &= (~(1 << mPendingSocId));
    WriteCpuPciCfgRas (mRasMpLink->Host, mSbspId, 0, BCASTLIST_UBOX_CFG_REG, Data32);
    if (!(mRasHost->var.common.emulation & SIMICS_FLAG)) {
      CommitMilestone ();
    } else {
      KtiDebugPrintInfo0 ((mRasMpLink->Host, KTI_DEBUG_INFO0, "\n  Simics Quiesce Commit "));
    }
  } else  if (Include == 1) {
    Data32 = ReadCpuPciCfgRas (mRasMpLink->Host, mSbspId, 0, BCASTLIST_UBOX_CFG_REG);
    KtiDebugPrintInfo1 ((mRasMpLink->Host, KTI_DEBUG_INFO0, "\n   Include = %u Read Data32 = 0x%08X", Include, Data32));
    Data32 |= (1 << mPendingSocId);
    WriteCpuPciCfgRas (mRasMpLink->Host, mSbspId, 0, BCASTLIST_UBOX_CFG_REG, Data32);
    if (!(mRasHost->var.common.emulation & SIMICS_FLAG)) {
      CommitMilestone ();
    } else {
      KtiDebugPrintInfo0 ((mRasMpLink->Host, KTI_DEBUG_INFO0, "\n  Simics Quiesce Commit "));
    }
  } else if (Include == 2) {
    for (Idx1 = 0; Idx1 < MAX_SOCKET; ++Idx1) {
      if (mRasMpLink->Host->var.common.socketPresentBitMap & (1 << Idx1)) {
        Data32 = ReadCpuPciCfgRas (mRasMpLink->Host, Idx1, 0, BCASTLIST_UBOX_CFG_REG);
        KtiDebugPrintInfo1 ((mRasMpLink->Host, KTI_DEBUG_INFO0, "\n   Include = %u Read Data32 = 0x%08X", Include, Data32));
        Data32 &= 0xffffff00;
        Data32 |= mRasMpLink->Host->var.common.socketPresentBitMap;
        WriteCpuPciCfgRas (mRasMpLink->Host, Idx1, 0, BCASTLIST_UBOX_CFG_REG, Data32);
      }
    }
    if (!(mRasHost->var.common.emulation & SIMICS_FLAG)) {
      CommitMilestone ();
    } else {
      KtiDebugPrintInfo0 ((mRasMpLink->Host, KTI_DEBUG_INFO0, "\n  Simics Quiesce Commit "));
    }
    RasMpLinkReleaseQuiesceBuf ();
    mClearShadowBuffer = TRUE;
    RasMpLinkClearKtiInfoBuffer ();
    RasMpLinkResetStateMachine ();
  }

  mDirectCommit = FALSE; // Turn off the direct commit
  return EFI_UNSUPPORTED;
}

/**

  Executed by SBSP only. Program BIOS_RESET_CPL register.

  @param host:  pointer to sysHost structure on stack

  @retval Related CPU CSRs are programmed as needed.

**/
VOID
SetRstCpl (
  struct sysHost     *host,
  UINT8              socket
  )
{
  BIOS_RESET_CPL_PCU_FUN1_STRUCT BIOSRESETCPL;

  BIOSRESETCPL.Data = ReadCpuPciCfgEx (host, socket, 0, BIOS_RESET_CPL_PCU_FUN1_REG);
  BIOSRESETCPL.Bits.rst_cpl1 = 1;
  KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\n:RAS - BIOS_RESET_CPL: BIOS_RST_CPL_1 set on S%x\n", socket));
  WriteCpuPciCfgRas (host, socket, 0, BIOS_RESET_CPL_PCU_FUN1_REG,BIOSRESETCPL.Data);
  //
  // Wait for PCU acknowledgement
  //
  if (!(host->var.common.emulation & SIMICS_FLAG)) {
    while (BIOSRESETCPL.Bits.pcode_init_done1 != 1) {
      BIOSRESETCPL.Data = ReadCpuPciCfgRas (host, socket, 0, BIOS_RESET_CPL_PCU_FUN1_REG);
    }
  }
}

/**

  This function gets the MemRasHost struct pointer.

  @param host   - pointer to sysHost structure on stack

  @retval None

**/
RAS_MPLINK_STATUS
RasMpLinkGetMemRasHost (
  IN OUT PSYSHOST     memRasHost
  )
{
  RAS_MPLINK_STATUS Status = RAS_MPLINK_SUCCESS;

  mMemRasHost = memRasHost;
  if(mMemRasHost == NULL ) {
    Status = RAS_MPLINK_FAILURE_GENERAL;
  }
  return Status;
}

/**

  Executed by SBSP only. Placeholder for configuration of CPU registers that are required to be
  setup for PCODE usage prior to setting BIOS_INIT_Done.

  @param host   - pointer to sysHost structure on stack
  @param socket - socket ID to program RESET_CPL on

**/
VOID
RasCpuInitMISC (
  struct sysHost *host,
  UINT8          socket
  )
{

}



/**

  Executed by SBSP only. Set emca_mode in VIRAL_CONTROL register.

  @param host   - pointer to sysHost structure on stack
  @param socket - incoming socket ID

**/
VOID
SetMsmiIsolation (
  struct sysHost *host,
  UINT8          socket
  )
{
  VIRAL_CONTROL_PCU_FUN4_STRUCT ViralControlPcuFun4;

  ViralControlPcuFun4.Data = ReadCpuPciCfgRas (host, socket, 0, VIRAL_CONTROL_PCU_FUN4_REG);
  ViralControlPcuFun4.Bits.emca_mode = 1;
  WriteCpuPciCfgRas (host, socket, 0, VIRAL_CONTROL_PCU_FUN4_REG, ViralControlPcuFun4.Data);
}

/**

  KTI RAS Milestone0 has 3 passes:
    1. Prepare Host structure
       Power up pending socket
    2. Prepare for slow mode training
    3. Release pending socket from reset

  @retval RAS_MPLINK_SUCCESS on successful completion of each pass

**/
RAS_MPLINK_STATUS
RasMpLinkMileStone0 (
  VOID
  )
{
  RAS_MPLINK_STATUS Status;
  STATIC UINT8      Pass = 0;

  if (mPendingEvtType == RAS_MPLINK_EVNT_ONLINE) {
    KtiDebugPrintInfo0 ((mRasMpLink->Host, KTI_DEBUG_INFO0, "\n  MileStone0: Prepare Host structure, power up pending socket, prepare for\n  slow mode training, release pending socket from reset."));
    KtiDebugPrintInfo0 ((mRasMpLink->Host, KTI_DEBUG_INFO0, "\n  Pass %u", Pass));
    if (Pass == 0) {
      mDirectCommit = TRUE;
      Status = PrepareHostDataStruct ();
      ASSERT_EFI_ERROR (Status);

      UpdateCpuInfoInKtiInternal(mRasHost, mKtiInternalGlobal, mSbspId);
      KtiDebugPrintInfo0 ((mRasMpLink->Host, KTI_DEBUG_INFO0, "\n  Issue POWER UP command to Socket%u\n", mPendingSocId));
      PowerUpOnlinedSocket ();

#ifndef RAS_KTI_TIME_SLICE
      KtiFixedDelay (mRasMpLink->Host, 50000); // 50 ms delay
#endif
      //
      // Wait for the PLD to power up the socket
      //
      mRasMpLinkDelayCounter = DELAY_COUNTER;
      mNextMileStoneIdx -= 1;
      Pass = 1;
    } else if (Pass == 1) {
      mDirectCommit = TRUE;
      PrepareForSlowSpeedTraining (mRasSocketData);
#ifndef RAS_KTI_TIME_SLICE
      KtiFixedDelay (mRasMpLink->Host, 50000); // 50 ms delay
#endif
      //
      // Wait for Phy init to complete
      //
      mRasMpLinkDelayCounter = DELAY_COUNTER;
      mNextMileStoneIdx -= 1;
      Pass = 2;
    } else if (Pass == 2) {
      mDirectCommit = TRUE;
      KtiDebugPrintInfo0 ((mRasMpLink->Host, KTI_DEBUG_INFO0, "\n  Issue RELEASE RESET command to Socket%u\n", mPendingSocId));
      ReleaseResetOnlinedSocket ();
      Pass = 0;
#ifndef RAS_KTI_TIME_SLICE
      KtiFixedDelay (mRasMpLink->Host, 50000); // 50 ms delay
#endif
      //
      // Wait for Pcode to complete KTI link init
      //
      mRasMpLinkDelayCounter = DELAY_COUNTER;
    }
  } // Online event

  return RAS_MPLINK_SUCCESS;
} // RasMpLinkMileStone0

/**

  KTI RAS Milestone1 has 3 passes:
    0. Collect LEP to confirm links trained in slow mode
    1. Open SBSP's config space for new socket in Quiesce
    2. Commit the Quiesce buffer

  @retval RAS_MPLINK_SUCCESS on successful completion of each pass

**/
RAS_MPLINK_STATUS
RasMpLinkMileStone1 (
  VOID
  )
{
  RAS_MPLINK_STATUS Status = RAS_MPLINK_SUCCESS;
  STATIC UINT8      Pass = 0;

  if (mPendingEvtType == RAS_MPLINK_EVNT_ONLINE) {
    KtiDebugPrintInfo0 ((mRasMpLink->Host, KTI_DEBUG_INFO0, "\n  MileStone1: Collect LEP, set config access path, update variables for new socket."));
    KtiDebugPrintInfo0 ((mRasMpLink->Host, KTI_DEBUG_INFO0, "\n  Pass %u", Pass));
    if (Pass == 0) {
      mDirectCommit = TRUE;
      //
      // Collect the LEP of the system and check if the onlined socket is reachable on at least one of the links.
      //
      if ((Status = CollectLepForOnlineEvent ()) != RAS_MPLINK_SUCCESS) {
        KtiDebugPrintFatal ((mRasMpLink->Host, KTI_DEBUG_ERROR, "\n  MileStone1: Collecting LEP for O*L event failed."));
        return Status;
      }
      mNextMileStoneIdx -= 1;
      Pass = 1;
    } else if (Pass == 1) {
      if (!(mRasHost->var.common.emulation & SIMICS_FLAG)) {
        mDirectCommit = FALSE;
      } else {
        KtiDebugPrintInfo0 ((mRasMpLink->Host, KTI_DEBUG_INFO0, "\n  Simics Quiesce Entry "));
      }
      //
      // The SBSP config path to the online socket has to commit first before reading/updating its routing related CSRs.
      // This step requires quiescing since we are altering route tables, SADs etc.
      //
      Status = OpenSbspConfigSpaceForHotAdd ();
      mNextMileStoneIdx -= 1;
      Pass = 2;
    } else if (Pass == 2) {
      if (!(mRasHost->var.common.emulation & SIMICS_FLAG)) {
        CommitMilestone ();
      } else {
        KtiDebugPrintInfo0 ((mRasMpLink->Host, KTI_DEBUG_INFO0, "\n  Simics Quiesce Commit "));
      }
      Pass = 0;
    }
  } else {
    //
    // Just collect the LEP by excluding the socket being offlined. No register accesses needed.
    //
    Status = CollectLepForOfflineEvent ();
    Status = AdjustSbspConfigSpaceForHotRemoval ();
    ASSERT_EFI_ERROR (Status);
    mNextMileStoneIdx += 4;   // Milestones 2 to 5 are not applicable for offline event, so skip them
    Pass = 0;
  }

  return Status;
} // RasMpLinkMileStone1

/**

  KTI RAS Milestone2 has one pass.

  In this pass we set config and boot path to new socket, check pCode status on
  the new socket, then update KtiInternalGlobal with SKU and CHA/M3/M2 etc
  counts for all sockets but SBSP. After that we make sure the SKU and CHA/M3
  counts are the same between SBSP and the new socket.

  Finally, we pre-process KTI links options and prime host structure. These two
  steps do not require CSR accesses.

  @retval RAS_MPLINK_SUCCESS - on successful completion
  @retval RAS_MPLINK_FAILURE_CHKIN - if pCode didn't finish on new socket
  @retval RAS_MPLINK_FAILURE_SOC_MISMATCH - on mismatch between SBSP and new socket

**/
RAS_MPLINK_STATUS
RasMpLinkMileStone2 (
  VOID
  )
{
  RAS_MPLINK_STATUS Status = RAS_MPLINK_SUCCESS;
  UINT32            NonStickyPad0;
  UINT8             Socket;

  if (mPendingEvtType == RAS_MPLINK_EVNT_ONLINE) {
    KtiDebugPrintInfo0 ((mRasHost, KTI_DEBUG_INFO0, "\n  MileStone2: Check pCode status, update KtiInternalGlobal, check for\n  mismatches, pre-process links, prime host structure, allocate IIO resources"));
    mDirectCommit = TRUE;

    if ((Status = SetConfigAndBootPath ()) != RAS_MPLINK_SUCCESS) {
      KtiDebugPrintFatal ((mRasHost, KTI_DEBUG_ERROR, "\n  MileStone2: Setting the config and boot path failed"));
      return Status;
    }

    //
    // At this point need to update the CpuCsrAccess protocol variables
    // To do this first need to update the UDS variables
    //
    KtiUpdateUdsStructure();
    mCpuCsrAccess->UpdateCpuCsrAccessVar();

    //
    // Make sure pCode has initialized the socket before any access to the socket is made
    // BIOSNONSTICKYSCRATCHPAD0[15:8] should be 0x2 (LLC BIST complete) before the socket can be accessed
    //
    NonStickyPad0 = ReadCpuPciCfgRas (mRasHost, (UINT8)mPendingSocId, 0, BIOSNONSTICKYSCRATCHPAD0_UBOX_MISC_REG);
    if ((NonStickyPad0 & 0xFF00) != 0x0200) {
      return RAS_MPLINK_FAILURE_CHKIN;
    }

    //
    // Make sure the onlined socket matches SBSP in terms of SKU type and CHA and M3KTI count
    // SBSP has already been updated for mKtiInternalGlobal, so no need to redo it
    //
    for (Socket = 0; Socket < MAX_SOCKET; ++Socket) {
      if ((mRasSocketData->Cpu[Socket].Valid == TRUE) && (Socket != mSbspId)) {
        UpdateCpuInfoInKtiInternal (mRasHost, mKtiInternalGlobal, Socket);
      }
    }

    if (CheckThisSocketInfoWithSbsp (mRasHost, mKtiInternalGlobal, mSbspId, (UINT8) mPendingSocId) == TRUE) {
      return RAS_MPLINK_FAILURE_SOC_MISMATCH;
    }

    PreProcessKtiLinks (mRasHost, mRasSocketData, mKtiInternalGlobal);
    PrimeHostStructure (mRasHost, mRasSocketData, mKtiInternalGlobal);
  }

  return Status;
} // RasMpLinkMileStone2

/**

  KTI RAS Milestone3 has one pass.

  In this pass we sync up scratchpads with the new socket (LEP, SBSP ID, local
  node id, hotadd flag) and then issue BOOT_GO to it. After that we verify that
  the new socket's BIOS SEC code checks in.

  @retval RAS_MPLINK_SUCCESS - on successful completion
  @retval RAS_MPLINK_FAILURE_CHKIN - if BIOS SEC code didn't check in

**/
RAS_MPLINK_STATUS
RasMpLinkMileStone3 (
  VOID
)
{
  RAS_MPLINK_STATUS Status = RAS_MPLINK_SUCCESS;
  UINT32            CheckCount;

  if (mPendingEvtType == RAS_MPLINK_EVNT_ONLINE) {
    KtiDebugPrintInfo0 ((mRasHost, KTI_DEBUG_INFO0, "\n  MileStone3: Sync up with new socket, issue BOOT_GO, verify SEC code check-in."));
    mDirectCommit = TRUE;

    SyncUpPbspForRas (mRasHost, mRasSocketData, mKtiInternalGlobal, POST_RESET_POWERGOOD);

    //
    // Make sure that the onlined CPU has checked-in. Otherwise it is a fatal error.
    // SBSP can not return before making sure the hot added socket is halted. Otherwise
    // when the SBSP returns to the OS and if the OS issues Lock cycles, the PBSP code fetch will fail.
    // (At least until the PBSP turns on NEM mode, SBSP should wait here). We need to add the
    // hand-shake code here.
    //
    for (CheckCount = 0; CheckCount < MAX_LOOP; CheckCount++) {
      KtiFixedDelay (mRasHost, 1000);
      if ((Status = VerifyCpuCheckIn ()) != KTI_SUCCESS) {
        if (CheckCount ==  (MAX_LOOP - 1)) {
          KtiDebugPrintFatal((mRasHost, KTI_DEBUG_ERROR, "\n    Onlined socket failed to chek-in."));
          Status = IssueWarmResetOfOnlinedCpu (FALSE);
          ASSERT (FALSE);
          mDirectCommit = FALSE;
          return RAS_MPLINK_FAILURE_CHKIN;
        }
      } else {
        //
        // CPU checked-in successfully
        //
        break;
      }
    }
  }

  return Status;
} // RasMpLinkMileStone3

/**

  KTI RAS Milestone4 has 4 passes:
    0. CPU RC calls (ActiveCores/SMT enable/disable, FlexRatio), RAS CpuInitMisc, MSMI Isolation
    1. Program mesh credits, W/A-related MSRs, MiscBeforeWR, BGF Overrides, TscSync
    2. KTI/Mem Transition to full speed, set RST CPL, update pad7, issue WR
    3. Verify Links' status, issue phy reset, sync up new socket, verify check-in,
       CPU RC/RAS calls (ActiveCores/SMT enable/disable, RAS CpuInitMisc) and halt PBSP.

  @retval RAS_MPLINK_SUCCESS         - on successful completion of each pass
  @retval RAS_MPLINK_FAILURE_GENERAL - on failure to complete a pass
  @retval RAS_MPLINK_FAILURE_CHKIN   - if KTI links don't train in full speed or new socket
                                       doesn't check in

**/
RAS_MPLINK_STATUS
RasMpLinkMileStone4 (
  VOID
  )
{
  RAS_MPLINK_STATUS Status = RAS_MPLINK_SUCCESS;
  STATIC UINT8      Pass = 0;
  UINT32            Loop, Data32;

  if (mPendingEvtType == RAS_MPLINK_EVNT_ONLINE) {
    KtiDebugPrintInfo0((mRasHost, KTI_DEBUG_INFO0, "\n  MileStone4, "));

    if (Pass == 0) {
      mDirectCommit = TRUE;
      KtiDebugPrintInfo0 ((mRasHost, KTI_DEBUG_INFO0, "Pass0: CPU RC calls, RAS CpuInitMisc, MSMI Isolation"));
      //
      // Program DesiredCores, SMTDisable for hot-added socket
      //
      SetActiveCoresAndSmtEnableDisable (mRasHost);
      if ((Status = ProgramProcessorFlexRatio (mRasHost)) != KTI_SUCCESS) {
        mDirectCommit = FALSE;
        return RAS_MPLINK_FAILURE_GENERAL;
      }
      RasCpuInitMISC (mRasHost, (UINT8)mPendingSocId);
      SetMsmiIsolation (mRasHost, (UINT8)mPendingSocId);
      mNextMileStoneIdx -= 1;
      Pass = 1;
    } else if (Pass == 1) {
      mDirectCommit = TRUE;
      KtiDebugPrintInfo0 ((mRasHost, KTI_DEBUG_INFO0, "Pass1: Program mesh credits, W/A-related MSRs, MiscBeforeWR, BGF Overrides, TscSync"));
      ProgramMeshCredits (mRasHost, mRasSocketData, mKtiInternalGlobal);
      ProgramMsrForWa (mRasHost, mRasSocketData, mKtiInternalGlobal);
      ProgramMiscBeforeWarmReset (mRasHost, mRasSocketData, mKtiInternalGlobal);
      KtiProgramBGFOverrides (mRasHost, mRasSocketData, mKtiInternalGlobal);
      Tsc_Sync (mRasHost, mRasSocketData, (UINT8)mPendingSocId, mKtiInternalGlobal);

      if(mSpsRasNotifyProtocol != NULL) {
        mSpsRasNotifyProtocol->RasNotify(RasNotificationSerialPeciUnQuiescence);
      }
      mNextMileStoneIdx -= 1;
      Pass = 2;
    } else if (Pass == 2) {
      mDirectCommit = TRUE;
      KtiDebugPrintInfo0 ((mRasHost, KTI_DEBUG_INFO0, "Pass2: KTI/Mem Transition to full speed, set RST CPL, update pad7, issue WR"));

      MemSpeedModeTransitionForHotAdd ();

      //
      // SNC enabling in progress
      //
      //UncoreEnableMeshMode (mRasHost);

      //
      // Issue Warm Reset to the onlined CPU so that it can do all the core-related
      // initialization such as Flex Ratio, HT etc. This will also trigger the KTI
      // link full speed transition (since the warm reset of the CPU is propagated
      // to the peer sockets as a Phy layer Inband Reset)
      //
      SetRstCpl (mRasHost, (UINT8)mPendingSocId);

      //
      // Warm reset has to be passed to CPU RC code via scratchpad7 bit5.
      //
      Data32 = ReadCpuPciCfgRas (mRasHost, (UINT8)mPendingSocId, 0, BIOSSCRATCHPAD7_UBOX_MISC_REG);
      Data32 |= BIT5;
      WriteCpuPciCfgRas (mRasHost, (UINT8)mPendingSocId, 0, BIOSSCRATCHPAD7_UBOX_MISC_REG, Data32);

      //
      // In the end, this step will disable the KTI links connecting to new socket
      // Can't access the new socket until after the warm reset and setting cinitbegin
      //
      KtiFullSpeedTransitionForHotAdd ();
      KtiDebugPrintInfo0 ((mRasMpLink->Host, KTI_DEBUG_INFO0, "\n  Issue WARM RESET command to Socket%u\n", mPendingSocId));
      Status = IssueWarmResetOfOnlinedCpu (FALSE);
      ASSERT (Status == RAS_MPLINK_SUCCESS);

#ifndef RAS_KTI_TIME_SLICE
      //
      // 50ms delay
      //
      KtiFixedDelay (mRasHost, 50000);
#endif
      //
      // Wait for AP come out from reset
      //
      mRasMpLinkDelayCounter = DELAY_COUNTER;
      mNextMileStoneIdx -= 1;
      Pass = 3;
    } else if (Pass == 3) {
      mDirectCommit = TRUE;
      KtiDebugPrintInfo0 ((mRasHost, KTI_DEBUG_INFO0, "Pass3: Set cinitbegin on host sockets ports."));

      //
      // Set cinitbegin on host sockets' ports
      //
      ReceivingKtiPortsSetCinitbegin (mRasHost, mKtiInternalGlobal);

      mRasMpLinkDelayCounter = DELAY_COUNTER;
      mNextMileStoneIdx -= 1;
      Pass = 4;
    } else if (Pass == 4) {
      mDirectCommit = TRUE;
      KtiDebugPrintInfo0 ((mRasHost, KTI_DEBUG_INFO0, "Pass4: Verify Links' status, issue phy reset, sync up new socket, verify check-in,\n    CPU RC/RAS calls (ActiveCores/SMT enable/disable, RAS CpuInitMisc) and halt PBSP."));
      //
      // To detect all connected links of onlined CPU are up, if not, flag error and return back
      //
      if ((Status = VerifyLinkStatusAfterWarmReset ()) != KTI_SUCCESS){
        ASSERT (FALSE);
        Pass = 0;
        mDirectCommit = FALSE;
        return RAS_MPLINK_FAILURE_GENERAL;
      }

      //
      // 4929779: Cloned From SKX Si Bug Eco: KTI Link Layer not sending LL_INIT after LL Reset
      // BIOS needs  to re-enable LL clock gating after hot-plug is done
      //
      ReceivingKtiPortsReenableLinkLayerClockGating (mRasHost);

      SyncUpPbspForRas (mRasHost, mRasSocketData, mKtiInternalGlobal, POST_RESET_WARM);

      //
      // Make sure that the onlined CPU has checked-in. Otherwise it is a fatal error.
      // SBSP can not return before making sure the hot added socket is halted. Otherwise
      // when the SBSP returns to the OS and if the OS issues Lock cycles, the PBSP code fetch will fail.
      // (At least until the PBSP turns on NEM mode, SBSP should wait here). We need to add the
      // hand-shake code here.
      //
      for (Loop = 0; Loop < MAX_LOOP; Loop++){
        KtiFixedDelay (mRasHost, 10000);
        if ((Status = VerifyCpuCheckIn ()) != KTI_SUCCESS) {
          if (Loop == (MAX_LOOP - 1)){
            KtiDebugPrintFatal ((mRasHost, KTI_DEBUG_ERROR, "\n    Onlined socket failed to chekin. "));
            ASSERT (FALSE);
            mDirectCommit = FALSE;
            return RAS_MPLINK_FAILURE_CHKIN;
          }
        } else {
          //
          // CPU checked in already
          //
          SetActiveCoresAndSmtEnableDisable (mRasHost);       //Program DesiredCores, SMTDisable for hot-added socket
          RasCpuInitMISC (mRasHost, (UINT8)mPendingSocId);
          // HaltPbsp ();                                       //Instruct the Onlined CPU to tear down NEM and halt
          break;
        }
      }
      Pass = 0;
    }
  } // if Online event

  return Status;
} // RasMpLinkMileStone4

/*

  KTI RAS Milestone5 has one pass.
  Verify if all of the links connected to the new socket have trained in fast mode.
  Recollect LEP and display it through serial log.

  @retval RAS_MPLINK_SUCCESS - on successful completion
  @retval other - if one or more links failed to train in fast mode.

*/
RAS_MPLINK_STATUS
RasMpLinkMileStone5 (
  VOID
  )
{
  RAS_MPLINK_STATUS Status = RAS_MPLINK_SUCCESS;

  if (mPendingEvtType == RAS_MPLINK_EVNT_ONLINE) {
    KtiDebugPrintInfo0 ((mRasHost, KTI_DEBUG_INFO0, "\n  MileStone5: Verify if links trained and Recollect the LEP."));
    mDirectCommit = TRUE;

    //
    // It is possible some of the links connected to the onlined fails to train at full speed. So recollect the LEP
    // before discovering the topology (i.e computing the optimum route for the entire system). Also apply
    // the relevant workarounds that needs to be applied for the links after every warm reset and log warning
    // about the links that failed to train or trained in degraded mode (Half/Quarter width).
    //
    if ((Status = CollectLepForOnlineEvent ()) != KTI_SUCCESS) {
      KtiDebugPrintFatal ((mRasHost, KTI_DEBUG_ERROR, "\n    MileStone5: Recollecting the LEP failied."));
    }
  }

  return Status;
} // RasMpLinkMileStone5

/**

  KTI RAS Milestone6 has two passes:
    0. Check scratchpad on new socket to see if it has torn down NEM and parked.
       The rest of steps are executed in Quiesce mode.
       Pre-process KTI links, prime host struct and calculate IIO resource allocation.
       Program final bus config, and Phy layer configuration after warm reset.
       Discover topology and calculate optimum route.
       Finally, copy topology info to the host structure.
    1. Commit CSR writes from the previous pass.

  @retval RAS_MPLINK_SUCCESS         - on succesful completion
  @retval RAS_MPLINK_FAILURE_CHKIN   - if onlined socket fails to tear down NEM and park
  @retval RAS_MPLINK_FAILURE_GENERAL - otherwise

**/
RAS_MPLINK_STATUS
RasMpLinkMileStone6 (
  VOID
  )
{
  RAS_MPLINK_STATUS Status = RAS_MPLINK_SUCCESS;
  UINT8             Socket;
  UINT32            Data32 = 0;
  STATIC UINT8      Pass = 0;

  if (mPendingEvtType == RAS_MPLINK_EVNT_ONLINE) {
    KtiDebugPrintInfo0 ((mRasHost, KTI_DEBUG_INFO0, "\n  MileStone6: update data structs, calculate resource allocation and program final bus config and warm reset Phy layer,\n    discover topology and calculate optimum route, \n    verify new socket has torn down NEM and parked."));
    KtiDebugPrintInfo0 ((mRasMpLink->Host, KTI_DEBUG_INFO0, "\n  Pass %u", Pass));
    if (Pass == 0) {

      //
      // The rest of the steps in this pass are done in Quiesce mode
      //
      if (!(mRasHost->var.common.emulation & SIMICS_FLAG)) {
        mDirectCommit = FALSE;
      } else {
        KtiDebugPrintInfo0 ((mRasMpLink->Host, KTI_DEBUG_INFO0, "\n  Simics Quiesce Entry "));
      }

      //
      // Shouldn't need to make these 2 calls.
      //
      PreProcessKtiLinks (mRasHost, mRasSocketData, mKtiInternalGlobal);
      PrimeHostStructure (mRasHost, mRasSocketData, mKtiInternalGlobal);

      KtiFinalCpuBusCfg (mRasHost, mRasSocketData, mKtiInternalGlobal);
      KtiPhyLinkAfterWarmReset (mRasHost, mRasSocketData, mKtiInternalGlobal);
      if (KtiTopologyDiscovery (mRasHost, mRasSocketData, mKtiInternalGlobal) != RAS_MPLINK_SUCCESS) {
        KtiDebugPrintFatal ((mRasHost, KTI_DEBUG_ERROR, "\n    MileStone6: Topology discovery failed."));
        return RAS_MPLINK_FAILURE_GENERAL;
      }
      CopyTopologyInfoToHostParm();
      mNextMileStoneIdx -= 1;
      Pass = 1;
    } else if (Pass == 1) {
      if (!(mRasHost->var.common.emulation & SIMICS_FLAG)) {
        CommitMilestone ();
      } else {
        KtiDebugPrintInfo0 ((mRasMpLink->Host, KTI_DEBUG_INFO0, "\n  Simics Quiesce Commit "));
      }
      mNextMileStoneIdx -= 1;
      Pass = 2;
    } else if (Pass == 2) {
      mDirectCommit = TRUE;
      //
      // Make sure the onlined CPU has torn down NEM and halted. Otherwise it is a fatal error.
      //
      Data32 = ReadCpuPciCfgRas (mRasHost, (UINT8)mPendingSocId, 0, SR_MEMORY_DATA_STORAGE_DISPATCH_PIPE_CSR);
      if (Data32 != 0) {
        KtiDebugPrintFatal ((mRasHost, KTI_DEBUG_ERROR, "\n    MileStone6: Onlined socket failed to tear down NEM and halt."));
        return RAS_MPLINK_FAILURE_CHKIN;
      }

      RasInitializeHostForMemOL(mRasHost);
      RasInitializePipe(mRasHost);
      RasStartMemInit(mRasHost);           //command PBSP to start MEM INIT

      Pass = 0;
    }
  } else {
    //
    // For an Offline event, update data structs and then discover topology and calculate resource
    // allocation and optimum routing
    //
    for (Socket = 0; Socket < MAX_SOCKET; ++Socket) {
      if (mRasSocketData->Cpu[Socket].Valid == TRUE) {
        UpdateCpuInfoInKtiInternal (mRasHost, mKtiInternalGlobal, Socket);
      }
    }
    mDirectCommit = FALSE;
    if (PreProcessKtiLinks (mRasHost, mRasSocketData, mKtiInternalGlobal) != KTI_SUCCESS) {
      return RAS_MPLINK_FAILURE_GENERAL;
    }
    PrimeHostStructure (mRasHost, mRasSocketData, mKtiInternalGlobal);
    AllocateIioResources (mRasHost, mRasSocketData, mKtiInternalGlobal);
    if (KtiTopologyDiscovery (mRasHost, mRasSocketData, mKtiInternalGlobal) != RAS_MPLINK_SUCCESS) {
      return RAS_MPLINK_FAILURE_GENERAL;
    }
    CopyTopologyInfoToHostParm();
  }

  return Status;
} // RasMpLinkMileStone6

/**

  KTI RAS Milestone7 has two passes:
    0. In Quiesce mode, program non-broadcast routing.
    1. Commit CSR writes from the previous pass.

  @retval RAS_MPLINK_SUCCESS - on successful completion

**/
RAS_MPLINK_STATUS
RasMpLinkMileStone7 (
  VOID
  )
{
  STATIC UINT8 Pass = 0;

  if (mPendingEvtType == RAS_MPLINK_EVNT_ONLINE) {
    KtiDebugPrintInfo0 ((mRasHost, KTI_DEBUG_INFO0, "\n  MileStone7: Program non-broadcast routing."));
    KtiDebugPrintInfo0 ((mRasHost, KTI_DEBUG_INFO0, "\n  Pass %u", Pass));
    if (Pass == 0) {
      if (!(mRasHost->var.common.emulation & SIMICS_FLAG)) {
        mDirectCommit = FALSE;
      } else {
        KtiDebugPrintInfo0 ((mRasMpLink->Host, KTI_DEBUG_INFO0, "\n  Simics Quiesce Entry "));
      }
      mRasHost->var.kti.ProgramNonBC = TRUE;
      mRasHost->var.kti.ProgramBC = FALSE;
      ProgramSystemRoute (mRasHost, mRasSocketData, mKtiInternalGlobal);
      mNextMileStoneIdx -= 1;
      Pass = 1;
    } else if (Pass == 1) {
      if (!(mRasHost->var.common.emulation & SIMICS_FLAG)) {
        CommitMilestone ();
      } else {
        KtiDebugPrintInfo0 ((mRasMpLink->Host, KTI_DEBUG_INFO0, "\n  Simics Quiesce Commit "));
      }
      Pass = 0;
    }
  }

  return RAS_MPLINK_SUCCESS;
} // RasMpLinkMileStone7

/**

  KTI RAS Milestone8 has two passes:
    0. In Quiesce mode, program system IOSAD entries.
    1. Commit CSR writes from the previous pass.

  @retval RAS_MPLINK_SUCCESS - on successful completion

**/
RAS_MPLINK_STATUS
RasMpLinkMileStone8 (
  VOID
  )
{
  STATIC UINT8 Pass = 0;

  if (mPendingEvtType == RAS_MPLINK_EVNT_ONLINE) {
    KtiDebugPrintInfo0 ((mRasHost, KTI_DEBUG_INFO0, "\n  MileStone8: Setup system IOSAD entries."));
    KtiDebugPrintInfo0 ((mRasHost, KTI_DEBUG_INFO0, "\n  Pass %u", Pass));
    if (Pass == 0) {
      if (!(mRasHost->var.common.emulation & SIMICS_FLAG)) {
        mDirectCommit = FALSE;
      } else {
        KtiDebugPrintInfo0 ((mRasMpLink->Host, KTI_DEBUG_INFO0, "\n  Simics Quiesce Entry "));
      }
      SetupSystemIoSadEntries (mRasHost, mRasSocketData, mKtiInternalGlobal);
      mNextMileStoneIdx -= 1;
      Pass = 1;
    } else if (Pass == 1) {
      if (!(mRasHost->var.common.emulation & SIMICS_FLAG)) {
        CommitMilestone ();
      } else {
        KtiDebugPrintInfo0 ((mRasMpLink->Host, KTI_DEBUG_INFO0, "\n  Simics Quiesce Commit "));
      }
      Pass = 0;
    }
  }

  return RAS_MPLINK_SUCCESS;
} // RasMpLinkMileStone8

/**

  KTI RAS Milestone9 has two passes:
    0. In Quiesce mode, replicate the SBSP's DRAM decoder setting into the onlined socket.
    1. Commit CSR writes from the previous pass.

  @retval RAS_MPLINK_SUCCESS - on successful completion

**/
RAS_MPLINK_STATUS
RasMpLinkMileStone9 (
  VOID
  )
{
  STATIC UINT8 Pass = 0;

  if (mPendingEvtType == RAS_MPLINK_EVNT_ONLINE) {
    KtiDebugPrintInfo0 ((mRasHost, KTI_DEBUG_INFO0, "\n  MileStone9: Replicate SBSP DRAM decoders into onlined socket."));
    KtiDebugPrintInfo0 ((mRasHost, KTI_DEBUG_INFO0, "\n  Pass %u", Pass));
    if (Pass == 0) {
      if (!(mRasHost->var.common.emulation & SIMICS_FLAG)) {
        mDirectCommit = FALSE;
      } else {
        KtiDebugPrintInfo0 ((mRasMpLink->Host, KTI_DEBUG_INFO0, "\n  Simics Quiesce Entry "));
      }
      SyncUpDramSad ();
      mNextMileStoneIdx -= 1;
      Pass = 1;
    } else if (Pass == 1) {
      if (!(mRasHost->var.common.emulation & SIMICS_FLAG)) {
        CommitMilestone ();
      } else {
        KtiDebugPrintInfo0 ((mRasMpLink->Host, KTI_DEBUG_INFO0, "\n  Simics Quiesce Commit "));
      }
      Pass = 0;
    }
  }

  return RAS_MPLINK_SUCCESS;
} // RasMpLinkMileStone9

/**

  KTI RAS Milestone10 has three passes:
    0. In Quiesce mode, program broadcast routing, KTI protocol and coherency settings,
       update CPU list in scratchpads.
    1. Commit the CSR writes from Pass 1.

  @retval RAS_MPLINK_SUCCESS - on successful completion

**/
RAS_MPLINK_STATUS
RasMpLinkMileStone10 (
  VOID
  )
{
  STATIC UINT8 Pass = 0;

  if (mPendingEvtType == RAS_MPLINK_EVNT_ONLINE) {
    if (Pass == 0) {
      KtiDebugPrintInfo0((mRasMpLink->Host, KTI_DEBUG_INFO0, "\n  MileStone10, Pass0: Program broadcast routing, KTI protocol and coherency settings,\n    update CPU list."));

      if (!(mRasHost->var.common.emulation & SIMICS_FLAG)) {
        mDirectCommit = FALSE;
      } else {
        KtiDebugPrintInfo0 ((mRasMpLink->Host, KTI_DEBUG_INFO0, "\n  Simics Quiesce Entry "));
      }

      mRasHost->var.kti.ProgramNonBC = FALSE;
      mRasHost->var.kti.ProgramBC = TRUE;
      ProgramSystemRoute (mRasHost, mRasSocketData, mKtiInternalGlobal);
      mRasHost->var.kti.ProgramBC = FALSE;

      SetupKtiMisc (mRasHost, mRasSocketData, mKtiInternalGlobal);
      SetupSystemCoherency (mRasHost, mRasSocketData, mKtiInternalGlobal);
      UpdateCpuList (TRUE);

      //
      // SNC enabling in progress
      //
      //SncMisc (mRasHost, mRasSocketData, mKtiInternalGlobal);

      Pass = 1;
      mNextMileStoneIdx -= 1;
    } else if (Pass == 1) {
      KtiDebugPrintInfo0((mRasMpLink->Host, KTI_DEBUG_INFO0, "\n  MileStone10, Pass1: Commit the register settings."));

      if (!(mRasHost->var.common.emulation & SIMICS_FLAG)) {
        CommitMilestone ();
      } else {
        KtiDebugPrintInfo0 ((mRasMpLink->Host, KTI_DEBUG_INFO0, "\n  Simics Quiesce Commit "));
      }

      Pass = 0;
      //
      // For Onlining, skip from Milestone 10 to 12
      //
      mNextMileStoneIdx++;
    }
  } // if Online event

  return RAS_MPLINK_SUCCESS;
} // RasMpLinkMileStone10

/**

  KTI RAS Milestone11 is only used during the offlining flow.
  Disable all links connected to the offlined socket and power down the links.

  @retval RAS_MPLINK_SUCCESS

**/
RAS_MPLINK_STATUS
RasMpLinkMileStone11 (
  VOID
  )
{
  if (mPendingEvtType == RAS_MPLINK_EVNT_OFFLINE) {
    mDirectCommit = TRUE;
    PowerDownOnlinedSocket (mPendingSocId, mRasSocketData);
    CollectAndClearErrors(mRasHost, mRasSocketData, mKtiInternalGlobal);
  }

  return RAS_MPLINK_SUCCESS;
} // RasMpLinkMileStone11

/**

  KTI RAS Milestone12 has one pass.
  Collect and clear errors, add the new socket into the system interrup domain.
  Adjust flags in scratchpad7, reprogram RstCPL, publish mRasHost struct.

  @retval RAS_MPLINK_SUCCESS - on successful completion

**/
RAS_MPLINK_STATUS
RasMpLinkMileStone12 (
  VOID
)
{
  RAS_MPLINK_STATUS Status = RAS_MPLINK_SUCCESS;
  UINT32            Data32;
  STATIC UINT8      Pass = 0;
  STATIC BOOLEAN    MemInitDone = FALSE;


  if (mPendingEvtType == RAS_MPLINK_EVNT_ONLINE) {
    if (Pass == 0) {
      KtiDebugPrintInfo0((mRasMpLink->Host, KTI_DEBUG_INFO0, "\n  MileStone12: Collect and clear errors, add the Onlined CPU socket into System\n    Interrupt Domain, reprogram RstCPL, publish mRasHost."));

      if (!(mRasHost->var.common.emulation & SIMICS_FLAG)) {
        mDirectCommit = FALSE;
      } else {
        KtiDebugPrintInfo0 ((mRasMpLink->Host, KTI_DEBUG_INFO0, "\n  Simics Quiesce Entry "));
      }

      CollectAndClearErrors (mRasHost, mRasSocketData, mKtiInternalGlobal);
      if ((Status = AdjustIntDomainForHotAdd ()) != RAS_MPLINK_SUCCESS) {
        KtiDebugPrintFatal((mRasMpLink->Host, KTI_DEBUG_ERROR, "\n    MileStone12: Adding the onlined socket into System Interrupt Domain failed."));
        return Status;
      }

      //
      // Clear hot add flag (run time) & clear cold reset flow.
      // Set hot plug enabled for platform warm reset
      //
      Data32 = ReadCpuPciCfgRas(mRasHost, (UINT8)mPendingSocId, 0, BIOSSCRATCHPAD7_UBOX_MISC_REG);
      Data32 &= (~(BIT6+ BIT4));
      Data32 |=  BIT1;
      WriteCpuPciCfgRas(mRasHost, (UINT8)mPendingSocId, 0, BIOSSCRATCHPAD7_UBOX_MISC_REG, Data32);

      if (!(mRasHost->var.common.emulation & SIMICS_FLAG)) {
        CommitMilestone ();
      } else {
        KtiDebugPrintInfo0 ((mRasMpLink->Host, KTI_DEBUG_INFO0, "\n  Simics Quiesce Commit "));
      }

      //
      // Reprogram RstCPL register, since the register got cleared after warm reset
      //
      mDirectCommit = TRUE;
      SetRstCpl (mRasHost, (UINT8)mPendingSocId);
      mNextMileStoneIdx -= 1;
      Pass = 1;
    } else if (Pass == 1) {
      MemInitDone = RasMemInitDoneCheckAndHandle(mRasHost);
      //
      // for now add switch to !MemInitDone
      //
      if (!MemInitDone) {
        RasExitPipe(mRasHost);
        Pass = 0;
        //
        // Publish the new RasMpLink Host structure to other modules
        //
        MemCopy((VOID *)mRasMpLink->Host, (VOID *)mRasHost, sizeof(SYSHOST));
        mRasMpLink->HostParmValid = TRUE;
        DataDumpRasMpLink ();
        MemInitDone = FALSE;
      } else {
        mNextMileStoneIdx -= 1;
      }
    } //end if Pass 1
  }

  return Status;
} // RasMpLinkMileStone12


RAS_MPLINK_STATUS
RasMpLinkStartStateMachine (
  IN INT32           EventType,
  IN INT32           SocketType,
  IN INT32           SocketId
)
{

  // Initialize the state machine control variables for start operation
  mRasMpLink->HostParmValid = FALSE;
  mInProgress = TRUE;
  mPendingEvtType = EventType;
  mPendingSocType = SocketType;
  mPendingSocId = SocketId;
  mRasMpLinkDelayCounter = 0;

  return RAS_MPLINK_SUCCESS;
}

RAS_MPLINK_STATUS
RasMpLinkResetStateMachine (
  VOID
)
{
  // Reset the state machine control variables
  mRasMpLink->HostParmValid = TRUE;
  mDirectCommit = FALSE;
  mInProgress = FALSE;
  mPendingEvtType = RAS_MPLINK_EVNT_RESET_SM;

  //
  // Commented out the following code. Debugging in progress.
  //
  // mPendingSocType = RAS_MPLINK_SOCKET_TYPE_NA;
  // mPendingSocId = RAS_MPLINK_SOCKET_ID_NA;
  // mSbspId = 0;

  mNextEntry = NULL;
  mNextMileStoneIdx = 0;
  mRasMpLinkDelayCounter = 0;

  return RAS_MPLINK_SUCCESS;
}

RAS_MPLINK_STATUS
RasMpLinkAcquaireQuiesceBuf (
  VOID
)
{
  EFI_STATUS        Status;

  mNextEntry = NULL;
  Status = mQuiesceProtocol->AquireDataBuffer((EFI_PHYSICAL_ADDRESS *)&mNextEntry);
  ASSERT_EFI_ERROR (Status);
  mQuieseBufStart = mNextEntry;
#ifdef RAS_KTI_DEBUG
  mCsrWriteCtr = 0;
#endif
  return RAS_MPLINK_SUCCESS;
}

RAS_MPLINK_STATUS
RasMpLinkReleaseQuiesceBuf (
  VOID
)
{
  EFI_STATUS        Status;

  Status = mQuiesceProtocol->ReleaseDataBuffer((EFI_PHYSICAL_ADDRESS)mNextEntry);
  ASSERT_EFI_ERROR (Status);
  mNextEntry = NULL;
  mQuieseBufStart = NULL;
#ifdef RAS_KTI_DEBUG
  mCsrWriteCtr = 0;
#endif

  return RAS_MPLINK_SUCCESS;
}

RAS_MPLINK_STATUS
RasMpLinkCommitSettings (
  VOID
)
{
  EFI_STATUS        Status;

  Status = mQuiesceProtocol->QuiesceMain();
  ASSERT_EFI_ERROR (Status);

  return RAS_MPLINK_SUCCESS;
}


RAS_MPLINK_STATUS
CommitMilestone (
   VOID
  )
{
  if(mSpsRasNotifyProtocol != NULL) {
    mSpsRasNotifyProtocol->RasNotify(RasNotificationDmiQuiescence);
  }
#ifdef RAS_KTI_DEBUG
  MarkEndOfCsrWrite();
  DumpQuiesceBuffer();
#endif
  RasMpLinkReleaseQuiesceBuf();
  RasMpLinkCommitSettings();
  mClearShadowBuffer = TRUE;
  RasMpLinkAcquaireQuiesceBuf();

  if(mSpsRasNotifyProtocol != NULL) {
    mSpsRasNotifyProtocol->RasNotify(RasNotificationDmiUnQuiescence);
  }
  return RAS_MPLINK_SUCCESS;
}


RAS_MPLINK_STATUS
KtiUpdateUdsStructure (
  VOID
)
{
  UINT8  Socket, KtiPort, Stack, Mc, Channel;
  UINT32 Data32;

  for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
    mIioUdsData->PlatformData.CpuQpiInfo[Socket].stackPresentBitmap = mRasHost->var.common.stackPresentBitmap[Socket];
    mIioUdsData->PlatformData.CpuQpiInfo[Socket].SocketFirstBus = mRasHost->var.common.SocketFirstBus[Socket];
    mIioUdsData->PlatformData.CpuQpiInfo[Socket].SocketLastBus = mRasHost->var.common.SocketLastBus[Socket];
    mIioUdsData->PlatformData.CpuQpiInfo[Socket].segmentSocket = mRasHost->var.common.segmentSocket[Socket];

    mIioUdsData->PlatformData.CpuQpiInfo[Socket].Valid = (UINT8)mRasHost->var.kti.CpuInfo[Socket].Valid;
    mIioUdsData->PlatformData.CpuQpiInfo[Socket].SocId = (UINT8)mRasHost->var.kti.CpuInfo[Socket].SocId;

    for (KtiPort = 0; KtiPort < MAX_KTI_PORTS; ++KtiPort) {
      mIioUdsData->PlatformData.CpuQpiInfo[Socket].PeerInfo[KtiPort].Valid =
          (UINT8)mRasHost->var.kti.CpuInfo[Socket].LepInfo[KtiPort].Valid;
      mIioUdsData->PlatformData.CpuQpiInfo[Socket].PeerInfo[KtiPort].PeerSocId =
          (UINT8)mRasHost->var.kti.CpuInfo[Socket].LepInfo[KtiPort].PeerSocId;
      mIioUdsData->PlatformData.CpuQpiInfo[Socket].PeerInfo[KtiPort].PeerSocType =
          (UINT8)mRasHost->var.kti.CpuInfo[Socket].LepInfo[KtiPort].PeerSocType;
      mIioUdsData->PlatformData.CpuQpiInfo[Socket].PeerInfo[KtiPort].PeerPort =
          (UINT8)mRasHost->var.kti.CpuInfo[Socket].LepInfo[KtiPort].PeerPort;
    }

    for (Stack = 0; Stack < MAX_IIO_STACK; Stack++) {
      mIioUdsData->PlatformData.CpuQpiInfo[Socket].StackBus[Stack] = mRasHost->var.common.StackBus[Socket][Stack];
    }

    for (Mc = 0; Mc < MAX_IMC; Mc++) {
      mIioUdsData->SystemStatus.imcEnabled[Socket][Mc] = mRasHost->var.mem.socket[Socket].imcEnabled[Mc];
    }

    for (Channel = 0; Channel < MAX_CH; Channel++) {
      mIioUdsData->SystemStatus.mcId[Socket][Channel] = mRasHost->var.mem.socket[Socket].channelList[Channel].mcId;
    }
  }
  mIioUdsData->SystemStatus.cpuType = mRasHost->var.common.cpuType;
  mIioUdsData->SystemStatus.MinimumCpuStepping = mRasHost->var.common.stepping;
  mIioUdsData->SystemStatus.socketPresentBitMap = mRasHost->var.common.socketPresentBitMap;
  mIioUdsData->PlatformData.PciExpressBase = mRasHost->var.common.mmCfgBase;
  mIioUdsData->SystemStatus.numChPerMC = mRasHost->var.mem.numChPerMC;
  mIioUdsData->SystemStatus.maxCh = MAX_CH;
  mIioUdsData->SystemStatus.maxIMC = mRasHost->var.mem.maxIMC;

  //
  // Program the BspApicId
  //
  if (mPendingEvtType == RAS_MPLINK_EVNT_ONLINE) {
     Data32 = ReadCpuPciCfgRas (mRasHost, (UINT8)mPendingSocId, 0, SR_PBSP_CHECKIN_CSR);
     Data32 = (Data32 >> 8) & 0xFF;
  } else {
     Data32 = 0xFF;
  }
  mIioUdsData->PlatformData.packageBspApicID[mPendingSocId]  = Data32;

  return RAS_MPLINK_SUCCESS;
}
