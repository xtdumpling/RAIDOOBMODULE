//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//
/**+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) 2008 - 2016 Intel Corporation.  All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license.  Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

    @file MemRas.h
   
    MemRas functions and platform data, which needs to be 
    ported.

---------------------------------------------------------------------------**/


#ifndef _MEM_RAS_H_
#define _MEM_RAS_H_

//
// Statements that include other header files
//

#include <Library/SmmServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PciLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/IoLib.h>
#include <Library/UefiDecompressLib.h>

#include <Protocol/RasMpLinkProtocol.h>
#include <Protocol/MemRasProtocol.h>
#include <Protocol/CpuCsrAccess.h>
#include <Protocol/IioUds.h>
#include <Protocol/CpuCsrAccess.h>
#include <Protocol/SmmSxDispatch2.h>
#include <Protocol/SmmVariable.h>
#include <Protocol/Decompress.h>
#include <Protocol/QuiesceProtocol.h>
#include <Protocol/CrystalRidge.h>

#include <Protocol/CpuPpmProtocol.h>
#include <Library/CpuPpmLib.h>

#include <Protocol/SmmPeriodicTimerDispatch2.h>
#include "MmrcProjectDefinitionsGenerated.h"
#include "SysHostChip.h"
// APTIOV_SERVER_OVERRIDE_RC_START
//
// RAS Event Name
//
typedef enum {
  SPARE_COPY_START,
  SPARE_COPY_COMPLETE,
  SDDC_START,
  SDDC_COMPLETE,
  ADDDC_START,
  ADDDC_COMPLETE
} EVENT_NAME;
// APTIOV_SERVER_OVERRIDE_RC_END

//
// Status get/set commands; used by GetOrSetErrorHandlingStatus()
//
typedef enum {
  UpdateEventProgressStatus,
  UpdateEventPendingStatus,
  GetEventProgressStatus,
  GetEventPendingStatus
} ERR_STATUS_OPCODE;

#define ENTRY_INVALID 0xFF

#define MAX_REGION_EP  1
#define MAX_REGION_EX  2
#define MAX_REGION_SVL 4
#define MAX_STRIKE    4
#define BANK_STRIKE 1
#define RANK_STRIKE 0
#define RANKVALID   0
#define BANKVALID   1
#define DEVICEVALID 2
#define PCODE_QUIESCE_TIME_50_MS       50

typedef enum {
  ADDDC_EP,       // Rank Sparing
  ADDDC_EX,    // ADDDC Sparing
} ADDDC_MODE;

extern   EFI_CPU_CSR_ACCESS_PROTOCOL    *mCpuCsrAccess;

UINT32   SchedulingCall (VOID *, ...);  // The parameter MUST be less or equal to 4

#define MEM_NODE_ON                     BIT0
#define MEM_NODE_OFF                    BIT1
#define MEM_RCOMP_OFF                   BIT2
#define MEM_DAT_UPDATE                  BIT3
#define MEM_NODE_INIT                   BIT4
#define MEM_MAP_UPDATE                  BIT5
#define MEM_SKT_ONE_TIME                BIT6
#define MEM_REQ_BOTH_BR                 BIT7

#define MEM_OFFLINE                     (MEM_NODE_INIT | MEM_MAP_UPDATE | MEM_NODE_OFF)
#define MEM_ONLINE                      (MEM_NODE_INIT | MEM_MAP_UPDATE | MEM_NODE_ON)

VOID *
EFIAPI
AsmFlushCacheLine (
  IN      VOID                      *LinearAddress
  );

extern EFI_MEM_RAS_PROTOCOL  *mMemRas;

typedef struct {
  UINTN                          Signature;
  EFI_HANDLE                     Handle;
  EFI_MEM_RAS_PROTOCOL           *mMemRas; // Policy protocol this driver installs
} EFI_MEM_RAS_DRIVER_PRIVATE;

EFI_STATUS
EFIAPI
InitializeMemRasData(
);

EFI_STATUS
EFIAPI
MemoryNodeOnline(
    IN UINT8      Node,
    IN OUT UINT8  *MemHpStatus
);

EFI_STATUS 
EFIAPI
MemoryNodeOffline(
    IN UINT8      Node,
    IN OUT UINT8 *MemHpStatus
);

EFI_STATUS
EFIAPI
IsMemNodeEnabled(
    IN UINT8 Node,
    OUT BOOLEAN *IsMemNodeEnabledFlag
);

EFI_STATUS
EFIAPI
UpdateConfigInQuiesce(
    IN UINT8 Operation
    ); 

EFI_STATUS
EFIAPI
EnableErrorSignallingtoBIOS(
    IN UINT8 Node
);

EFI_STATUS
EFIAPI
ModifyRasFlag (
    IN UINT64                         FlagBit,
    IN UINT8                          Value
);

EFI_STATUS
EFIAPI
MigrationPossible (
    IN UINT8 SrcNode,
    IN UINT8 DstNode,
    OUT BOOLEAN *MigrationPossibleFlag,
    IN BOOLEAN  IsMirrorMigration
);

EFI_STATUS
EFIAPI
SetupMemoryMigration(
    IN UINT8 SrcNodeId,
    IN UINT8 DstNodeId
 );


EFI_STATUS
EFIAPI
DisableMemoryMigration(
    IN UINT8 SrcNodeId,
    IN UINT8 DstNodeId
 );

EFI_STATUS
EFIAPI
InjectPoison(
    IN UINT8   NodeId,
    IN UINT64  ErrorLogAddress               // error log Address
);

EFI_STATUS
InitializeAdddcAfterMirror(
    IN UINT8  McIndex
    );

EFI_STATUS
EFIAPI
GetCurrentMemoryNodeState (
    IN UINT8      Node,
    IN OUT UINT8 *MemState
);


UINT8
GetLogicalToPhysicalRankId(
    IN UINT8  NodeId,
    IN UINT8  DdrCh,
    IN UINT8  LogicalRank
 );

UINT8
GetSupportedMemRasModes(
    IN UINT8  NodeId,
    IN UINT8  DdrCh,
    IN UINT8  Rank
 );

BOOLEAN
EFIAPI
GetSpareInProgressState(
    IN        UINT8   NodeId,
    IN        UINT8   ddrCh,
    IN        UINT8   rank
    );

void
EFIAPI
PciWriteCfg (
    IN UINT8    SktId,
    IN UINT32   Offset,
    IN UINT32   Data
);

UINT32
EFIAPI
PciReadCfg (
    IN UINT8    SktId,
    IN UINT32   Offset
);



void
EFIAPI
GetOrSetErrorHandlingStatus (
  IN UINT8                           NodeId,
  IN ERR_STATUS_OPCODE               Opcode,
  IN OUT UINT32                      *Value
 );

BOOLEAN
IsErrorExcdForThisRank(
    IN UINT8 NodeId,
    IN UINT8 DdrCh,
    IN UINT8 Rank
 );

void
ClearErrorsForThisRank(
    IN UINT8 NodeId,
    IN UINT8 DdrCh,
    IN UINT8 Rank
 );

void
EFIAPI
GetBitMapOfNodeWithEventInProgress(
    IN OUT UINT32                     * EventProgressBitmap
  );

void
EFIAPI
GetBitmapOfNodeWithErrors(
    IN OUT UINT32                     * ErrorBitmap
  );

void
EFIAPI
GetLastErrInfo(
    IN UINT8                          NodeId,
    IN UINT8                          *DdrCh,
    IN UINT8                          *Dimm,
    IN UINT8                          *Rank
 );

EFI_STATUS
EFIAPI
OpenPamRegion(
    IN UINT8 SocketId
 );

EFI_STATUS
EFIAPI
RestorePamRegion(
    IN UINT8 SocketId
 );

EFI_STATUS
EFIAPI
GetErrorLogSysAddr(
    IN  UINT8    NodeId,
    OUT UINT64   *ErrorLogSysAddr
 );


VOID
IniMemRasS3 (
  VOID
);

EFI_STATUS
EFIAPI
SystemAddressToDimmAddress (
  IN  UINT64  SystemAddress,
  OUT PTRANSLATED_ADDRESS TA
);

EFI_STATUS
EFIAPI
DimmAddressToSystemAddress (
  IN OUT PTRANSLATED_ADDRESS TA
);

EFI_STATUS
EFIAPI
SmmRasUefiDecompressGetInfo (
  IN EFI_DECOMPRESS_PROTOCOL            *This,
  IN   VOID                             *Source,
  IN   UINT32                           SourceSize,
  OUT  UINT32                           *DestinationSize,
  OUT  UINT32                           *ScratchSize
  );

EFI_STATUS
EFIAPI
SmmRasUefiDecompress (
  IN     EFI_DECOMPRESS_PROTOCOL          *This,
  IN     VOID                             *Source,
  IN     UINT32                           SourceSize,
  IN OUT VOID                             *Destination,
  IN     UINT32                           DestinationSize,
  IN OUT VOID                             *Scratch,
  IN     UINT32                           ScratchSize
  );

EFI_STATUS
EFIAPI
GetFailedDimmErrorInfo(
  IN UINT8 Node,
  IN UINT8 NodeCh,
  IN UINT8 ValidBits,
                    /*   // Bit[0-1]  = 00 - use retry register to get failed rank.
                                        01 - Use rank over flow sts register to get the failed dimm
                                        02/03 - future use.
                         // Bit1  = 01 -  clear per rank error counters and error overflow status.
                                  = 00 -  don't clear rank error counters
                         // Bit2  = 01 -  clear per Retry read error log register..
                                  = 00 -  don't clear the retry read error log register
                     */
  IN UINT8 Rank,  // Rank is ignored. if needed we use it in future.
  OUT PFAILED_DIMM_INFO pFailedDimmInfo
);

typedef struct MEMRAS_S3_PARAM {
  UINT8 chFailed[MC_MAX_NODE][MAX_MC_CH];
  UINT8 ngnChFailed[MC_MAX_NODE][MAX_MC_CH];
  UINT8 spareLogicalRank[MC_MAX_NODE][MAX_MC_CH];
  UINT8 spareInUse[MC_MAX_NODE][MAX_MC_CH];
  UINT8 oldLogicalRank[MC_MAX_NODE][MAX_MC_CH];
  UINT8 MultiSpareRank[MC_MAX_NODE][MAX_MC_CH];
  UINT8 devTag[MC_MAX_NODE][MAX_MC_CH][MAX_RANK_CH];
} MEMRAS_S3_PARAM;


typedef struct STRIKE_STRUCT {
    BOOLEAN   Valid;
    UINT8     StrikeNum;
    UINT8     PrevStrike;
    UINT8     StrikeType; // Is the strike bank or rank - 1 is bank 0 is rank
    UINT8     Dimm;
    UINT8     Rank;
    UINT8     SubRank;
    UINT8     Bank;
    UINT8     BankGroup;
    UINT8     Device;
    UINT8     RegionNum;
    UINT8     ReverseRemapPrevStrike;
    UINT8     RegionType;  // 0 - error in independent region, 1- failure occured in adddc region. 2- failure occured in rank svls region;  3- failed rank is in one of the lock step mode.
    UINT8     RegionSubType;// 1 - failed region, 2- non failed region
} STRIKE_STRUCT;

typedef struct ADDDC_REGION_STRUCT {
    UINT8   Valid; // Initialize to 0
    UINT8   SocketNum;
    UINT8   ChNum;
    UINT8   StrikeNum;
    UINT8   RegionNum;
    UINT8   PrimaryRank;
    UINT8   PrimarySubRank;
    UINT8   PrimaryBank;
    UINT8   PrimaryBankGrp;
    UINT8   BuddyRank;
    UINT8   BuddySubRank;
    UINT8   BuddyBank;
    UINT8   BuddyBankGrp;
    UINT8   FailedDevice;
    BOOLEAN ReverseRegion;
  } ADDDC_REGION_STRUCT;


typedef struct SPARING_EVENT_STRUCT {
  ADDDC_REGION_STRUCT Region[MAX_REGION_EX];  //Should the region struct be an array of regions or every channel can have only one region associated
  STRIKE_STRUCT       Strike[MAX_STRIKE];
  UINT8               NumStrikes;
  UINT8               NumAvlRegion;  // NOTE: WHILE INITIALIZING MAKE NUMAVLREGION to the MAX REGION for that SKU
  BOOLEAN             SddcPlusOneDone;  //Assert ADDDC flow if this is already done on a channel.
}SPARING_EVENT_STRUCT;


typedef enum {
  SddcPlusOneAvailable,        // SDDC Sparing available
  SddcPlusOneInProgress,	   // SDDC Sparing in progress
  SddcPlusOneDone,			   // SDDC sparing completed
  SddcPlusOneNotAvailable      // SDDC Sparing is not available
} SDDC_PLUSONE_STATUS;

//
// Per Channel ADDDC state structure
//
typedef enum {
  NoAdddcInProgress,
  BAdddcInProgress,
  RAdddcInProgress,
  UB2RReverseInProgress,
  UB2RRAdddcInProgress,
  RRMBReverseInProgress,
  RRMB1SetInProgress,
  RRMB2SetInProgress,
  RRMRReverseInProgress,
  RRMR1SetInProgress,
  RRMR2SetInProgress, 
  SDDCPlus1InProgress,
  RUPReverseInProgress,
  AdddcUnavailable
} ADDDC_STATE_STRUCT;

//
// Sparing engine status
//
typedef enum {
  SparingEngineFree,       // Sparing engine is free
  SparingEngineInUse       // Sparing engine is in use
} SPARING_ENGINE_STATUS;

typedef enum {
  QuiesceAvailable,
  QuiesceInProgress,
}ADDDC_PCODE_STATUS;

EFI_STATUS
EFIAPI
ClearUnhandledCorrError(
    IN UINT8 NodeId,
    IN EVENT_TYPE EventType,
    OUT BOOLEAN *IsEventHandled
 );

#if 0
#endif // 0

#define INVALID_MEM_NODE    0xFF

extern struct SystemMemoryMapHob *mSystemMemoryMap;
// extern EFI_GUID gEfiMemoryConfigDataGuid;

//
// Macros used in Address Translation code
//
#define MAX_COL_BITS_SUPPORTED  12
#define MAX_ROW_BITS_SUPPORTED  18
#define MAX_BANK_BITS_SUPPORTED 3

#define MAX_COL_BITS_FIXED_MAP  10  // Max # of col bits that use fixed map
#define MAX_ROW_BITS_FIXED_MAP  12  // Max # of Row bits that use fixed map
#define MAX_BANK_BITS_FIXED_MAP 3   // Max # of bank its that use fixed map

#define MAX_ROW_BITS_DYNAMIC_MAP 6  // Max # of row bits that use dynamic map
#define MAX_COL_BITS_DYNAMIC_MAP 2  // Max # of col bits that use dynamic map

#define MAX_COL_BITS_SUPPORTED_DDR4  10
#define MAX_ROW_BITS_SUPPORTED_DDR4  18
#define MAX_BANK_BITS_SUPPORTED_DDR4 2
#define MAX_BG_BITS_SUPPORTED_DDR4   2
#define MAX_CS_BITS_SUPPORTED_DDR4	 3

#define MAX_COL_BITS_FIXED_MAP_DDR4  10  // Max # of col bits that use fixed map
#define MAX_ROW_BITS_FIXED_MAP_DDR4  12  // Max # of Row bits that use fixed map
#define MAX_BANK_BITS_FIXED_MAP_DDR4 2   // Max # of bank its that use fixed map
#define MAX_BG_BITS_FIXED_MAP_DDR4   2   // Max # of bank group bits that use fixed map

#define MAX_ROW_BITS_DYNAMIC_MAP_DDR4 6  // Max # of row bits that use dynamic map
#define MAX_COL_BITS_DYNAMIC_MAP_DDR4 0  // Max # of col bits that use dynamic map

//
// Opcode used by address translation functions
//
#define EXTRACT_ADDR_BITS        0
#define STUFF_ADDR_BITS          1


typedef enum {

  SVQuiesceUnQuiesce = 1,   //1
  SVMemoryFailOver,         //2
  SVMemoryOnOffLine,        //3
  SVMemoryMigration,        //4
  SVIohOnOffline,           //5
  SVCpuOnOffline,           //6
  Reserved_1,               //7
  SVChipSparing_StartSpare, //8
  SVRankSparing_SetFail,    //9 - Not supported in Platform
  SVSet_LeakyBucket,        //10- Not supported in Platform
  SVSmiExtender,            //11- Not supported in Platform
  SVMemoryChipUnerase,      //12
  SVMemoryPatrolScrub,      //13- Not supported in Platform
  SVQuiesceCodeRun,         //14
} SV_SMM_COMMAND;


#define ECC_MODE_REG_LIST      {ECC_MODE_RANK0_MC_MAIN_REG, ECC_MODE_RANK1_MC_MAIN_REG, ECC_MODE_RANK2_MC_MAIN_REG, ECC_MODE_RANK3_MC_MAIN_REG, ECC_MODE_RANK4_MC_MAIN_REG, ECC_MODE_RANK5_MC_MAIN_REG, ECC_MODE_RANK6_MC_MAIN_REG, ECC_MODE_RANK7_MC_MAIN_REG}
#define ECC_MODE_REG_EXT_LIST  {ECC_MODE_RANK0_MC_MAINEXT_REG, ECC_MODE_RANK1_MC_MAINEXT_REG, ECC_MODE_RANK2_MC_MAINEXT_REG, ECC_MODE_RANK3_MC_MAINEXT_REG, ECC_MODE_RANK4_MC_MAINEXT_REG, ECC_MODE_RANK5_MC_MAINEXT_REG, ECC_MODE_RANK6_MC_MAINEXT_REG, ECC_MODE_RANK7_MC_MAINEXT_REG}

#endif  //_MEM_RAS_H_
