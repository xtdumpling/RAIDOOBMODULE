//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//
/**

Copyright (c)  1999 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file MemRasProtocol.h

  Mem Ras ProtocolCode

**/

#ifndef _MEM_RAS_PROTOCOL_H_
#define _MEM_RAS_PROTOCOL_H_

#include <Guid/MemoryMapData.h>

#define EFI_MEM_RAS_PROTOCOL_GUID \
  {0x6d7e4a32, 0x9a73, 0x46ba, 0x94, 0xa1, 0x5f, 0x2f, 0x25, 0xef, 0x3e, 0x29}

#define EFI_FORWARD_DECLARATION(x) typedef struct _##x x
EFI_FORWARD_DECLARATION (EFI_MEM_RAS_PROTOCOL);

//#define  RAS_DEBUG(x)   do { \
//	if (mRasFlags & RAS_FLAG_RAS_DEBUG) { \
//	EfiDebugPrint x; \
//	} while (0)
// APTIOV_SERVER_OVERRIDE_RC_START : Disabled RAS debug messages in Release mode
#ifdef EFI_DEBUG
#define  RAS_DEBUG(x)  DebugPrint x
#else
#define  RAS_DEBUG(x)
#endif
// APTIOV_SERVER_OVERRIDE_RC_END : Disabled RAS debug messages in Release mode
extern UINT64 mRasFlags;

// BIT0-BIT31 will be checked by MRC code
#define  RAS_FLAG_KEEP_POWERON                BIT0
#define  RAS_FLAG_INJ_ECC                     BIT30
#define  RAS_FLAG_RESERVE                     BIT31   // please avoid use this to avoid sign extension

// BIT32-BIT61 will be consumed by RAS only
#define  RAS_FLAG_MRC_DEBUG                   BIT63
#define  RAS_FLAG_TIME_SLICING                BIT62
#define  RAS_FLAG_RAS_DEBUG                   BIT61
#define  RAS_FLAG_QUIESCE_MIGRATION           BIT60


#define  RAS_PARAM_MEASURE_DATA               0
#define  RAS_PARAM_QUICSES_DATA               1

#define  SAVE_DISABLE_CKE_LOW                 1
#define  RESTORE_CKE_LOW                      2
#define  B2B_DCK_ENABLE                       1
#define  B2B_DCK_DISABLE                      0

#define PARAM_UNUSED  0xFF // Used in place of ChIdOrBoxInst in MemRead/WritePciCfg for MC functional blocks
                           // that have only 1 instance per MC

//
// RAS Event type
//
typedef enum {
  NEW_EVENTS,             // New events (events yet to be handled)
  EVENTS_IN_PROGRESS,     // Events that are in progress
} EVENT_TYPE;

typedef enum {
  ddr3dimmType = 0,
  ddr4dimmType = 1,
  ddrtdimmType = 2,
  dimmTypeUnknown
} TRANSLATED_DIMM_TYPE;

typedef struct _TRANSLATED_ADDRESS {
  UINT64  SystemAddress;
  UINT64  NmSystemAddress;
  UINT64  SpareSystemAddress;
  UINT64  DPA;
  UINT8   SocketId;
  UINT8   SadId;
  UINT8   TadId;
  UINT8   MemoryControllerId;
  UINT8   TargetId;
  UINT8   LogChannelId;
  UINT64  ChannelAddress;
  UINT64  NmChannelAddress;
  UINT8   ChannelId;
  UINT8   NmChannelId;
  UINT64  RankAddress;
  UINT64  NmRankAddress;
  UINT8   PhysicalRankId;
  UINT8   NmPhysicalRankId;
  UINT8   DimmSlot;
  UINT8   NmDimmSlot;
  UINT8   DimmRank;
  UINT32  Row;
  UINT32  Col;
  UINT8   Bank;
  UINT8   BankGroup;
  UINT8   LockStepRank;
  UINT8   LockStepPhysicalRank;
  UINT8   LockStepBank;
  UINT8   LockStepBG;
  UINT8   ChipSelect;
  UINT8   NmChipSelect;
  UINT8   Node;
  UINT8   ChipId;
  UINT8   RasModesApplied;  //BIT0 = Rank sparing, BIT1 = RANK VLS, BIT2 = BANK VLS BIT3 = MIRROR
  UINT8   MemType;
  TRANSLATED_DIMM_TYPE DimmType;
} TRANSLATED_ADDRESS, *PTRANSLATED_ADDRESS;

typedef
UINT32
(EFIAPI *EFI_READ_JCK_CFG) (
  IN UINT8    NodeId,
  IN UINT8    DdrCh,
  IN UINT8    Link,
  IN UINT32   Offset
  );

typedef
void
(EFIAPI *EFI_WRITE_JCK_CFG) (
  IN UINT8    NodeId,
  IN UINT8    DdrCh,
  IN UINT8    Link,
  IN UINT32   Offset,
  IN UINT32   Data
  );

typedef
EFI_STATUS
(EFIAPI *EFI_MEM_NODE_ONLINE) (
  IN UINT8 Node,
  IN OUT UINT8 *MemHpStatus
  );

typedef
EFI_STATUS
(EFIAPI *EFI_MEM_NODE_OFFLINE) (
  IN UINT8 Node,
  IN OUT UINT8 *MemHpStatus
  );

typedef
void
(EFIAPI *EFI_B2B_OPERATION) (
    IN UINT64  Command,
    IN UINT64* ParamArray
);

typedef
EFI_STATUS
(EFIAPI *EFI_IS_MEM_NODE_ENABLED) (
  IN UINT8 Node,
  OUT BOOLEAN *IsMemNodeEnabledFlag
  );



typedef
EFI_STATUS
(EFIAPI *EFI_UPDATE_CONFIG_IN_QUIESCE) (
  IN UINT8 Operation
  );

typedef
EFI_STATUS
(EFIAPI *EFI_ENABLE_ERROR_SIGNAL_TO_BIOS) (
  IN UINT8 Node
  );

typedef
EFI_STATUS
(EFIAPI *EFI_REENABLE_PATROL_SCRUB_ENGINE) (
  IN UINT8 NodeId
  );

typedef
EFI_STATUS
(EFIAPI *EFI_DISABLE_PATROL_SCRUB_ENGINE) (
  IN UINT8 NodeId
  );

typedef
EFI_STATUS
(EFIAPI *EFI_SAVE_PATROL_SCRUB_ENGINE) (
  IN UINT8 NodeId
  );

typedef
EFI_STATUS
(EFIAPI *EFI_MIGRATION_POSSIBLE) (
    IN UINT8 MasterNodeId,
    IN UINT8 SlaveNodeId,
    OUT BOOLEAN *MigrationPossibleFlag,
    IN BOOLEAN IsMirrorMigration
    );

typedef
EFI_STATUS
(EFIAPI *EFI_SETUP_MEM_MIGRATION) (
    IN UINT8 SrcNodeId,
    IN UINT8 DestNodeId
    );

typedef
EFI_STATUS
(EFIAPI *EFI_DISABLE_MEM_MIGRATION) (
    IN UINT8 SrcNodeId,
    IN UINT8 DestNodeId
    );

typedef
EFI_STATUS
(EFIAPI *EFI_CALL_MEM_MIGRATION_STATE_MACHINE) (
    IN UINT8 MasterNodeId,
    IN UINT8 SlaveNodeId,
    IN UINT8 CacheLineSize,    // In Bytes
    IN UINT64 BlockSize,       // In Bytes
    OUT UINT8 *MigrationOpFlag
    );

typedef
EFI_STATUS
(EFIAPI *EFI_GET_CURRENT_MEM_NODE_STATE) (
    IN UINT8      Node,
    IN OUT UINT8 *MemState
);

typedef
EFI_STATUS
(EFIAPI *EFI_MODIFY_RAS_FLAG) (
    IN UINT64                         Flag,
    IN UINT8                          Value
);

typedef
EFI_STATUS
(EFIAPI *EFI_GET_PARAMETER) (
    IN UINT64                         ParamId,
    IN UINT64                        *Value
);

typedef
EFI_STATUS
(EFIAPI  *EFI_INITIALIZE_DOUBLE_CHIPKILL)(
);

typedef
EFI_STATUS
(EFIAPI *EFI_SV_MEM_RAS_SERVICE) (
    IN UINT64                         ParamIn0,
    IN UINT64                         ParamIn1,
    IN UINT64                         ParamIn2,
    IN UINT64                         *ParamOut,
    OUT BOOLEAN						  *PeriodicCallbackDisFlag
);

typedef
EFI_STATUS
(EFIAPI *EFI_CKE_Low_Ctrl) (
    IN UINT8         Node,
    IN UINT8         Operation
);

typedef
EFI_STATUS
(EFIAPI *EFI_SYSTEM_ADDRESS_TO_DIMM_ADDRESS) (
    IN  UINT64  SystemAddress,
    OUT PTRANSLATED_ADDRESS  TA
);


typedef
EFI_STATUS
(EFIAPI *EFI_DIMM_ADDRESS_TO_SYSTEM_ADDRESS) (
    IN  OUT PTRANSLATED_ADDRESS  TA
);

// 0 - error in independent region, 1- failure occured in adddc region. 2- failure occured in rank svls region;  3- failed rank is in one of the lock step mode.
typedef enum {
  ErrInIndReg ,
  ErrInAdddcReg ,
  ErrInSvlsReg ,
  FailedRankInLSMode,
} FAILURE_REGION_TYPE;

typedef enum {
  ErrInIndepReg,
  ErrInFailedRank,
  ErrInNonFailedRank,
  ErrInFailedBank,
  ErrInNonFailedBank,
} FAILURE_REGION_SUB_TYPE;

typedef struct _FAILED_DIMM_INFO {
  UINTN   ValidBits;    // bit0 = 1 error valid. used currently in ADDDC and +1 modes. 
  											// will be used next in failed dimm silation of ecc errros in legacy mode.
			  							  // Bit usage will be defined further based on future usage.
  FAILURE_REGION_TYPE   RegionType;  // 0 - error in independent region, 1- failure occured in adddc region. 2- faile occured in rank svls region;  3- failed rank is in one of the lock step mode.
  FAILURE_REGION_SUB_TYPE   RegionSubType;// 1 - failed region, 2- non failed region
  UINT8   Socket;
  UINT8   SktCh;
  UINT8   ddr3_width;
  UINT8   FailedRank;
  UINT8   FailedSubRank;
  UINT8   FailedDimm;
  UINT8   FailedDev;
  UINT8   FailedBank;
  UINT8   FailedBG;
  UINT32  FailedRow;
  UINT32  FailedCol;
  UINT8   BuddyRank;
  UINT8   BuddySubRank;
  UINT8   BuddyBank;
  UINT8   BuddyBG;
} FAILED_DIMM_INFO, *PFAILED_DIMM_INFO;

typedef
EFI_STATUS
(EFIAPI *EFI_ENABLE_LOGGING_ECC_CORR_ERR) (
    IN UINT8 Node
 );

typedef
EFI_STATUS
(EFIAPI *EFI_INJECT_POISON) (
    IN UINT8   NodeId,
    IN UINT64  ErrorLogAddress              // error log Address
                                            // 63:0 - SystemAddress
 );

typedef
EFI_STATUS
( EFIAPI * EFI_MODIFY_CACHE_LINE_AND_LOG_ERR) (
   IN UINT64   Start,         // start  logical(page mapped virtual) address for migration chunk   (do you need physical address here?)
   IN UINT64   Len,           // length for the chunk, must be aligned to CachelineSize
   IN UINT64  *ErrLog,        // The mbox local address log buffer  for error logging (1024 entries available)
                              // Bits 33:0 - MBOX local address, BIT63:56 - NodeIdx
   IN UINT8    CachelineSize, // 8 bytes  for UC;  64 bytes for WB
   IN UINT8    CacheFlush,    //  Is cflush required after each cache line copy
   IN UINT8    Node
   );

typedef
EFI_STATUS
(EFIAPI * EFI_INITIALIZE_RANK_SPARING)(
 );

typedef
EFI_STATUS
(EFIAPI * EFI_INITIALIZE_DBDC_SPARING) (
 );

typedef
EFI_STATUS
(EFIAPI * EFI_INITIALIZE_ADDDC_MR) (
 );

typedef
EFI_STATUS
(EFIAPI * EFI_INITIALIZE_ADDDC_AFTER_MIRROR) (
 );

typedef
EFI_STATUS
(EFIAPI * EFI_INITIALIZE_SDDC_PLUSONE)(
 );

typedef
void
(EFIAPI * EFI_INITIALIZE_VLS_INFO)(
 );


typedef
EFI_STATUS
(EFIAPI * EFI_CHECK_AND_HANDLE_ADDDC_SPARING)(
    IN UINT8 NodeId,
    IN EVENT_TYPE EventType,
    OUT BOOLEAN *IsEventHandled
 );

typedef
EFI_STATUS
(EFIAPI * EFI_CHECK_AND_HANDLE_RANK_SPARING)(
    IN UINT8 NodeId,
    IN EVENT_TYPE EventType,
    OUT BOOLEAN *IsEventHandled
 );

typedef
EFI_STATUS
(EFIAPI * EFI_CHECK_AND_HANDLE_SDDC_PLUSONE_SPARING)(
    IN UINT8 NodeId,
    IN EVENT_TYPE EventType,
    OUT BOOLEAN *IsEventHandled
 );

typedef
EFI_STATUS
(EFIAPI * EFI_CLEAR_UNHANDLED_CORR_ERROR)(
    IN UINT8 NodeId,
    IN EVENT_TYPE EventType,
    OUT BOOLEAN *IsEventHandled
 );



typedef
void
(EFIAPI * EFI_GET_BITMAP_OF_NODE_WITH_ERRORS) (
    IN OUT UINT32                     * ErrorBitmap
  );

typedef
void
(EFIAPI * EFI_GET_BITMAP_OF_NODE_WITH_EVENT_IN_PROGRESS) (
    IN OUT UINT32                     * EventProgressBitmap
  );

typedef
void
(EFIAPI * EFI_GET_LAST_ERR_INFO)(
    IN UINT8                          NodeId,
    IN UINT8                          *DdrCh,
    IN UINT8                          *Dimm,
    IN UINT8                          *Rank
 );

typedef
EFI_STATUS
(EFIAPI * EFI_OPEN_PAM_REGION)(
    IN UINT8 SocketId
 );

typedef
EFI_STATUS
(EFIAPI * EFI_RESTORE_PAM_REGION)(
    IN UINT8  SocketId
 );

typedef
EFI_STATUS
(EFIAPI * EFI_GET_ERROR_LOG_SYS_ADDR)(
    IN  UINT8    NodeId,
    OUT UINT64   *ErrorLogSysAddr
 );

typedef
EFI_STATUS
(EFIAPI * EFI_ENABLE_PATROL_SCRUB_FOR_POISON_DETECTION)(
    IN UINT8 NodeId
 );

typedef
EFI_STATUS
(EFIAPI * EFI_HAS_PATROL_STOPPED_ON_ERROR)(
    IN  UINT8 NodeId,
    OUT BOOLEAN *HasStoppedOnError,
    OUT UINT64  *ErrorLogSysAddr,
    OUT BOOLEAN *IsPatrolDone
 );

typedef struct {
    UINT8   IsMapped;
    UINT8   IsMirror;
    UINT8   MirrorNode;
    UINT8   Reserved;
    UINT32  Length;
    UINT32  Base0;
    UINT32  Base1;
} BOOT_MEM_MAP;


//
// Memory RAS Workarounds
//
typedef struct{

  UINT32  hsd4030039_si4540670:1;      // Turn-off Patrol scrub after 2nd rank sparing operation
  UINT32  hsd4031075_si4677005:1;      // Turn-off rd cancel if < IVT B0
  UINT32  hsd4031322_si4029684:1;      // Re-configure BT for migration slave
  UINT32  hsd4031844_si4767036:1;      // When doing DDDC, copy wr push ecc mode to spare_ecc_mode on non-sparing CH
  UINT32  hsd4032926_si4677654:1;      // Disable ECC Mode for DDDC if JCK < C1 and IVT >= C0
  UINT32  rsvd_padding:26;             // Padding bits just to align the structure to 32-bits
} MEMRAS_WORKAROUNDS;

//
// Memopry errors enbaling
//
typedef
EFI_STATUS
(EFIAPI *EFI_ENABLE_MEM_ERRORS_NODE) (
    IN UINT8 NodeId
  );

typedef
UINT8
(EFIAPI * EFI_GET_SUPPORTED_MEMRAS_MODES)(
    IN UINT8  NodeId,
    IN UINT8  DdrCh,
    IN UINT8  Rank
 );

typedef
BOOLEAN
(EFIAPI * EFI_GET_SPARE_IN_PROGRESS_STATE)(
    IN UINT8  NodeId,
    IN UINT8  DdrCh,
    IN UINT8   rank
 );

typedef
UINT8
(EFIAPI * EFI_GET_LOGICAL_TO_PHYSICAL_RANK_ID)(
    IN UINT8  NodeId,
    IN UINT8  DdrCh,
    IN UINT8  LogicalRank
 );

typedef
EFI_STATUS
(EFIAPI * EFI_GET_FAILED_DIMM_ERROR_INFO) (
  IN UINT8 Node,
  IN UINT8 NodeCh,
  IN UINT8 ValidBits,
  IN UINT8 Rank,
  OUT PFAILED_DIMM_INFO pFailedDimmInfo
);

// Protocol Definition
//
typedef struct _EFI_MEM_RAS_PROTOCOL {
  struct SystemMemoryMapHob         *SystemMemInfo;

  PSYSHOST                          pSysHost;
  UINT32                            Signature;
  MEMRAS_WORKAROUNDS                MemRasWa;
  BOOT_MEM_MAP                      BootMemMap[MC_MAX_NODE];
  EFI_MEM_NODE_ONLINE               MemoryNodeOnline;
  EFI_MEM_NODE_OFFLINE              MemoryNodeOffline;
  EFI_B2B_OPERATION                 B2BOperation;
  EFI_IS_MEM_NODE_ENABLED           IsMemNodeEnabled;
  //EFI_UPDATE_CONFIG_IN_QUIESCE      UpdateConfigInQuiesce;
  EFI_ENABLE_ERROR_SIGNAL_TO_BIOS   EnableErrorSignallingtoBIOS;
  EFI_REENABLE_PATROL_SCRUB_ENGINE  ReEnablePatrolScrubEngine;
  EFI_DISABLE_PATROL_SCRUB_ENGINE   DisablePatrolScrubEngine;
  EFI_SAVE_PATROL_SCRUB_ENGINE      SavePatrolScrubEngine;
  EFI_MIGRATION_POSSIBLE            MigrationPossible;
  EFI_SETUP_MEM_MIGRATION           SetupMemoryMigration;
  EFI_DISABLE_MEM_MIGRATION         DisableMemoryMigration;
  //EFI_CALL_MEM_MIGRATION_STATE_MACHINE CallMemoryMigrationStateMachine;
  EFI_GET_CURRENT_MEM_NODE_STATE    GetCurrentMemoryNodeState;
  EFI_MODIFY_RAS_FLAG               ModifyRasFlag;
  //EFI_GET_PARAMETER                 GetParameter;
  //EFI_SV_MEM_RAS_SERVICE            SvMemRasService;
  EFI_SYSTEM_ADDRESS_TO_DIMM_ADDRESS   SystemAddressToDimmAddress;
  EFI_DIMM_ADDRESS_TO_SYSTEM_ADDRESS   DimmAddressToSystemAddress;
  //EFI_ENABLE_LOGGING_ECC_CORR_ERR      EnableLoggingEccCorrErr;
  //EFI_MODIFY_CACHE_LINE_AND_LOG_ERR    ModifyCacheLineAndLogErr;
  EFI_INJECT_POISON                    InjectPoison;
  EFI_INITIALIZE_RANK_SPARING          InitializeRankSparing;
  EFI_INITIALIZE_ADDDC_MR              InitializeAdddcMR;
  EFI_INITIALIZE_ADDDC_AFTER_MIRROR    InitializeAdddcAfterMirror;
  EFI_INITIALIZE_SDDC_PLUSONE          InitializeSddcPlusOne;
  EFI_INITIALIZE_VLS_INFO              InitializeVLSInfo;
  EFI_CHECK_AND_HANDLE_ADDDC_SPARING                CheckAndHandleAdddcSparing;
  EFI_CHECK_AND_HANDLE_RANK_SPARING                 CheckAndHandleRankSparing;
  EFI_CHECK_AND_HANDLE_SDDC_PLUSONE_SPARING         CheckAndHandleSddcPlusOneSparing;
  EFI_CLEAR_UNHANDLED_CORR_ERROR       ClearUnhandledCorrError;
  EFI_GET_BITMAP_OF_NODE_WITH_ERRORS            GetBitMapOfNodeWithErrors;
  EFI_GET_BITMAP_OF_NODE_WITH_EVENT_IN_PROGRESS GetBitMapOfNodeWithEventInProgress;
  EFI_GET_LAST_ERR_INFO                         GetLastErrInfo;
  EFI_OPEN_PAM_REGION                  OpenPamRegion;
  EFI_RESTORE_PAM_REGION               RestorePamRegion;
  EFI_GET_ERROR_LOG_SYS_ADDR           GetErrorLogSysAddr;
  //EFI_ENABLE_MEM_ERRORS_NODE         EnableMemoryErrorsPerNode;
  EFI_GET_SUPPORTED_MEMRAS_MODES     GetSupportedMemRasModes;
  EFI_GET_SPARE_IN_PROGRESS_STATE     GetSpareInProgressState;
  EFI_GET_LOGICAL_TO_PHYSICAL_RANK_ID GetLogicalToPhysicalRankId;
  EFI_GET_FAILED_DIMM_ERROR_INFO      GetFailedDimmErrorInfo;
} EFI_MEM_RAS_PROTOCOL;

extern EFI_GUID gEfiMemRasProtocolGuid;

#endif
