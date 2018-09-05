//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//
/*++

Copyright (c) 2008 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  KtiRas.h

Abstract:

--*/

#ifndef _CSI_RAS_H
#define _CSI_RAS_H


// CSI RAS driver build options

#define RAS_KTI_TIME_SLICE
#define RAS_KTI_DEBUG
#define RAS_KTI_DEBUG_REG_DETAIL
//#define RAS_KTI_UNIT_TEST
#define RAS_KTI_MEASURE_TIME

#include <Protocol/RasMpLinkProtocol.h>
#include <Protocol/SpsRasNotifyProtocol.h>

#include "KtiLib.h"
#include "SysFunc.h"
#include "KtiHost.h"
#include "KtiMisc.h"
#include "Pipe.h"
#include "RcRegs.h"
#include "SysHostChip.h"
#include "KtiSetupDefinitions.h"
#include <Library/OemProcMemInit.h>

#include <CpuRegs.h>

#include <Library/UefiLib.h>
#include <Library/HobLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Protocol/QuiesceProtocol.h>
#include <Protocol/IioUds.h>
#include <Protocol/SmmCpuService.h>
//#include <Library\ProcMemInitLib.h>
#include <Protocol/SmmSwDispatch2.h>

#include <Protocol/SmmBase2.h>
#include <Library/SmmServicesTableLib.h>
#include <Protocol/HpIoxAccess.h>

//#include "SmbAccess.h"
#include <Library/UefiBootServicesTableLib.h>
#include <Library/PciLib.h>

extern EFI_GUID gEfiRasHostGuid;

#define RAS_MPLINK_SOCKET_TYPE_NA       -1
#define RAS_MPLINK_SOCKET_ID_NA         -1
#define DELAY_COUNTER               5
#define MAX_LOOP                    1000


// ******************************** IMPORTANT NOTE ****************************************
// Shadow buffer space is allocated for all 32 devices of of a CPU. That includes 8 IIO devices and 24 Uncore
// devices. All 8 functions are included in the buffer space even though some functions may not be implemented
// by a device. This is to reduce the lookup time. All devices/functions except Phy layer and some link layer
// DFX, have registers within offset 0x200. The registers that are with offset > 0x200, all can be programmed
// without Queisce. Only the registers that require Quiesce need to be tracked in shadow buffer. So we allocate
// buffer space only for first 0x200 registers of each device/function. Since some of the registers are byte/word
// long, we have to maintain the flag to track at byte granularity. These buffers needs to be cleared at start of
// the state machine as well as after the buffer is committed using Quiesce. Note that clearing only the flags
// is sufficient.
// So the shadow buffer consumption as follows:
//  For 4S: 4Soc * 32Dev * 8Fun * 512B =  512KB
//  For 8S: 8Soc * 32Dev * 8Fun * 512B =  1MB
// The shadow flag size is as follows:
//  For 4S: 512KB / 8 = 64KB
//  For 8S: 1MB / 8 = 128KB
// *****************************************************************************************
#define MAX_REG_OFFSET             0x200
#define MAX_BUF_PER_FUN            MAX_REG_OFFSET
#define MAX_BUF_PER_DEV            (8 * MAX_BUF_PER_FUN)
#define MAX_BUF_PER_CPU            (32 * MAX_BUF_PER_DEV)
#define MAX_FLAG_BUF_PER_CPU       (MAX_BUF_PER_CPU / 8) // Every bit keeps track of a BYTE
#define CPU_SHADOW_BUF_SIZE        (MAX_SOCKET * MAX_BUF_PER_CPU)
#define CPU_SHADOW_FLAG_SIZE       (MAX_SOCKET * MAX_FLAG_BUF_PER_CPU)
#define DRAM_SAD_MAX               20

extern EFI_QUIESCE_SUPPORT_PROTOCOL   *mQuiesceProtocol;
extern EFI_RAS_MPLINK_PROTOCOL        *mRasMpLink;
extern EFI_IIO_UDS_PROTOCOL           *mIioUds;

extern UINT8              *mQuiesceBuf;
extern UINT32             mQuiesceBufSize;
extern SYSHOST            *mRasHost;
extern KTI_SOCKET_DATA    *mRasSocketData;
extern KTI_SOCKET_DATA    *mRasSocketDataOrig;
extern KTI_HOST_INTERNAL_GLOBAL   *mKtiInternalGlobal;
extern UINT8              *mCpuShadowBuf;
extern UINT8              *mCpuShadowFlag;
extern BOOLEAN            mDirectCommit;
extern BOOLEAN            mClearShadowBuffer;

extern BOOLEAN            mInProgress;
extern RAS_MPLINK_EVENT_TYPE              mPendingEvtType;
extern INT32              mPendingSocType;
extern INT32              mPendingSocId;
extern UINT8              mSbspId;
extern UINT64             mExpiredTime;
extern BOOLEAN            mDirectCommit;                // Flag to control whether register writes should be committed immediately

extern QUIESCE_DATA_TABLE_ENTRY    *mNextEntry;         // Points to first available slot in the quiesce buffer; used as semaphore as well
extern QUIESCE_DATA_TABLE_ENTRY    *mQuieseBufStart;

#ifdef RAS_KTI_DEBUG
extern UINT32                      mCsrWriteCtr;
#endif

RAS_MPLINK_STATUS
RasMpLinkCheckEvent (
  IN UINT8          EventType,
  IN UINT8          SocketInfo
  );

RAS_MPLINK_STATUS
RasMpLinkHandlerNonSliced (
  IN UINT8           EventType,
  IN UINT8          SocketInfo
  );

RAS_MPLINK_STATUS
RasMpLinkHandlerSliced (
  IN UINT8           EventType,
  IN UINT8          SocketInfo
  );

RAS_MPLINK_STATUS
RasMpLinkRestoreSadAndRta (
  );

RAS_MPLINK_STATUS
RasMpLinkFailureHandler (
  );

RAS_MPLINK_STATUS
RasMpLinkAdjustIpiList (
  IN BOOLEAN Include
  );

RAS_MPLINK_STATUS
RasMpLinkGetMemRasHost (
  IN OUT PSYSHOST     memRasHost
  );
  
RAS_MPLINK_STATUS
RasMpLinkMileStone0 (
  VOID
  );

RAS_MPLINK_STATUS
RasMpLinkMileStone1 (
  VOID
  );

RAS_MPLINK_STATUS
RasMpLinkMileStone2 (
  VOID
  );

RAS_MPLINK_STATUS
RasMpLinkMileStone3 (
  VOID
  );

RAS_MPLINK_STATUS
RasMpLinkMileStone4 (
  VOID
  );

RAS_MPLINK_STATUS
RasMpLinkMileStone5 (
  VOID
  );

RAS_MPLINK_STATUS
RasMpLinkMileStone6 (
  VOID
  );

RAS_MPLINK_STATUS
RasMpLinkMileStone7 (
  VOID
  );

RAS_MPLINK_STATUS
RasMpLinkMileStone8 (
  VOID
  );

RAS_MPLINK_STATUS
RasMpLinkMileStone9 (
  VOID
  );

RAS_MPLINK_STATUS
RasMpLinkMileStone10 (
  VOID
  );

RAS_MPLINK_STATUS
RasMpLinkMileStone11 (
  VOID
  );

RAS_MPLINK_STATUS
RasMpLinkMileStone12 (
  VOID
  );

RAS_MPLINK_STATUS
RasMpLinkStartStateMachine (
  IN INT32          EventType,
  IN INT32          SocketType,
  IN INT32          SocketId
  );

RAS_MPLINK_STATUS
RasMpLinkResetStateMachine (
  VOID
  );

RAS_MPLINK_STATUS
RasMpLinkAcquaireQuiesceBuf (
  VOID
  );

RAS_MPLINK_STATUS
RasMpLinkReleaseQuiesceBuf (
  VOID
  );

RAS_MPLINK_STATUS
RasMpLinkCommitSettings (
  VOID
  );

RAS_MPLINK_STATUS
CommitMilestone (
   VOID
  );

RAS_MPLINK_STATUS
KtiUpdateUdsStructure (
  VOID
  );

#endif
