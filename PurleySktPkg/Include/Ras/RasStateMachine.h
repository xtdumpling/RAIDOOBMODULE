//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//
/**+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) 2008 - 2014 Intel Corporation.  All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license.  Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

    @file RasStateMachine.h
   
    Mem Ras hook functions and platform data, which needs to be 
    ported.

---------------------------------------------------------------------------**/

#ifndef _RAS_STATE_MACHINE_H_
#define _RAS_STATE_MACHINE_H_

typedef enum {
  MigrationInit = 0,
  MigrationDisableMirror,
  MigrationOffline,
  MigrationOnline,
  MigrationCopyInit,
  MigrationCopyStart,
  MigrationInProgress,
  MigrationCopyComplete,
  MemoryMigrationDone,
  MigrationError,
  MemHpOperationInit,
  MemNodeNop,
  MemNodeOnline,
  MemNodeOffline,
  MemNodePowerOn,
  MemNodePowerOff,
  MemNodePowerOffEntry,
  MemNodeInitInProgress,
  MemNodeInstallInProgress,
  MemNodeInstallInProgress2,
  MemNodeInstallInProgressEntry,
  MemNodeRemoveInProgress,
  MemNodeRemoveInProgress2,
  MemNodeRemoveQuiesce,
  MemNodeRemoveAbortQuiesce,
  MemNodeInstallQuiesce,
  MemNodeInstallAbortQuiesce,
  MemNodeUpdateStructuresOnline,
  MemNodeUpdateStructuresOffline,
  MemNodeOnlineComplete,
  MemNodeOfflineComplete,
  MemNodeError,
  MemNodeNoMemory,
} SMI_TIMER_DISPATCHER_STATUS;


#define NoRequest                   0x00
#define CheckRequestValid           0x01
#define OnLineRequest               0x02


#define CpuRasMemoryOffline         0x04
#define CpuRasCheckRequestValid     0x01
#define CpuRasOnlining              0x03
#define CpuRasChangeMonarch         0x02
#define CpuRasOnlineRequest         0x10
#define CpuRasOfflineRequest        0x11
#define CpuRasInitializeMemory      0x60
#define CpuRasOnlineQpiIPIUpdate    0x50
#define CpuRasOnlineWaitForMpInit   0x51
#define CpuRasOnlineQpiDiscovery    0x52
#define CpuRasOfflineQpiLinkUpdate  0x53
#define CpuRasOfflinePowerOffFru    0x54
#define CpuRasOnlineBringInPbsp     0x55
#define CpuRasOnlinePowerOnFru      0x61
#define CpuRasUpdateAcpiList        0x56
#define CpuRasOsNotification        0x57
#define CpuRasOnlineRasStart        0x58
#define CpuRasOnlineExitRas         0x59
#define CpuOfflineSanityCheck       0x70
#define CpuOfflineRasStart          0x71
#define CpuOfflineCheckSci          0x72
#define CpuOfflineChangeMonarch     0x73
#define CpuOfflineChangeMonarch     0x73

#define CpuOfflineRasExit           0x75
#define CpuOfflineNotifyOS          0x76

#define ERR_CPU_MULTIPLE_REQ        EFI_UNSUPPORTED
#define ERR_IIO_MULTIPLE_REQ        EFI_UNSUPPORTED
#define ERR_IIO_UNSUPPORTED_REQ     EFI_UNSUPPORTED

#define NO_REQUEST                  EFI_NOT_FOUND
#define NO_SUPPORT                  EFI_UNSUPPORTED
#define IN_PROGRESS                 EFI_ALREADY_STARTED
#define ERROR_EXIT                  EFI_ABORTED
#define STATUS_DONE                 EFI_SUCCESS

// Faked SMI command definitions
#define  SMI_MEM_ONLINE_OFFLINE     0x01    
#define  SMI_CPU_ONLINE_OFFLINE     0x02
#define  SMI_MEM_MIRROR_REPLACE     0x03                  
#define  SMI_MEM_SPARE_MIGRATION    0x04
#define  SMI_IOH_ONLINE_OFFLINE     0x05
#define  SMI_CPU_HOT_ADD_REMOVE     0x06
#define  SMI_PARTIAL_MIRRORING      0x07
#define  SMI_RANK_SPARING			0x08
#define  SMI_CHIP_SPARING			0x09
#define HP_GPI_MASK                  BIT14

#define SCI_CHECK                   1
#define SCI_EJECTION                3 

#endif
