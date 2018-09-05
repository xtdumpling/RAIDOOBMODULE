//
// This file contains an 'Intel Pre-EFI Module' and is licensed
// for Intel CPUs and Chipsets under the terms of your license
// agreement with Intel or your vendor.  This file may be
// modified by the user, subject to additional terms of the
// license agreement
//
/**

Copyright (c) 2005 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

  @file OemProcMemInit.h

  Memory Initialization Module.

**/

#ifndef _OEMPROCMEM_INIT_H_
#define _OEMPROCMEM_INIT_H_

#include "ProcMemInit.h"

#define size_t UINT32
void *memcpy(void * dst, void * src, size_t cnt);
void *memset(void *dst, char value, size_t cnt);

#define MMCFGBASE_GRANT       0x4000000
#define PCA9534_INPORT        0  // Input port
#define PCA9534_OUTPORT       1  // Output port
#define PCA9534_POLARITY      2  // Polarity inversion
#define PCA9534_CONFIG        3  // Configuration

#define PCA9535_INPORT0       0  // Input port 0
#define PCA9535_INPORT1       1  // Input port 1
#define PCA9535_OUTPORT0      2  // Output port 0
#define PCA9535_OUTPORT1      3  // Output port 1
#define PCA9535_POLARITY0     4  // Polarity inversion 0
#define PCA9535_POLARITY1     5  // Polarity inversion 1
#define PCA9535_CONFIG0       6  // Configuration 0
#define PCA9535_CONFIG1       7  // Configuration 1

//OC defines
#define DEFAULT_BCLK      0x00

typedef struct _OC_DATA_HOB{
	UINT8  DmiPeg;
}OC_DATA_HOB;

VOID
OemInitializePlatformData (
    struct sysHost             *host
  );

UINT32
OemPreMemoryInit (
  PSYSHOST host
  );

VOID
OemPostMemoryInit (
    struct sysHost   *host,
    UINT32           MrcStatus
  );

VOID
OemPreProcInit (
    struct sysHost   *host
  );

VOID
OemPostProcInit (
    struct sysHost   *host,
    UINT32           ProcStatus
  );

VOID
OemCheckForBoardVsCpuConflicts (
    PSYSHOST host
    );

VOID
OemCheckAndHandleResetRequests (
  struct sysHost             *host
  );

VOID
MEBIOSCheckAndHandleResetRequests (
  struct sysHost             *host
  );


UINT32
OemMemRiserInfo (
  struct sysHost             *host
  );

UINT32
OemMemRASHook (
  struct sysHost             *host,
  UINT8  flag                                 // flag = 0 (Pre MRC RAS hook), flag = 1 (Post MRC RAS hook)
  );

UINT8
OemGetPlatformType (
  struct sysHost             *host
  );

VOID
OemPostCpuInit (
  struct sysHost             *host,
  UINT8                      socket
  );

VOID
OemPublishDataForPost (
  struct sysHost             *host
  );

void
OemPlatformFatalError (
  PSYSHOST host,
  UINT8    majorCode,
  UINT8    minorCode,
  UINT8    *haltOnError
  );

VOID
OemTurnOffVrsForHedt (
  struct sysHost             *host
  );

void
OemPlatformHookMst (
  PSYSHOST host,
  UINT8    socket,
  struct   smbDevice dev, 
  UINT8    byteOffset, 
  UINT8    *data
  );

BOOLEAN
OemDetectPhysicalPresenceSSA (
  struct sysHost *host
  );

VOID
OemHookPreTopologyDiscovery (
  struct sysHost             *host
  );

VOID
OemHookPostTopologyDiscovery (
  struct sysHost             *host
  );

// CommonHooks.c
void PlatformCheckpoint(PSYSHOST host);
void PlatformLogWarning(PSYSHOST host, UINT8 majorCode, UINT8 minorCode, UINT32 logData);
void PlatformFatalError(PSYSHOST host, UINT8 majorCode, UINT8 minorCode, UINT8 *haltOnError);
void PlatformInitSmb(PSYSHOST host);
UINT8 PlatformReadSmb(PSYSHOST host, UINT8 socket, struct smbDevice dev, UINT8 byteOffset, UINT8 *data);
UINT8 PlatformWriteSmb(PSYSHOST host, struct smbDevice dev, UINT8 byteOffset, UINT8 *data);
void PlatformInitGpio(PSYSHOST host);
UINT32 PlatformReadGpio(PSYSHOST host, UINT16 address, UINT32 *data);
UINT32 PlatformWriteGpio(PSYSHOST host, UINT16 address, UINT32 data);
void DummyOemHooks(PSYSHOST host);

// MemHooks.c
UINT32 PlatformMemInitWarning(PSYSHOST host);
void PlatformMemLogWarning(PSYSHOST host, UINT8 majorCode, UINT8 minorCode, UINT32 logData);
// APTIOV_SERVER_OVERRIDE_RC_START
void PlatformInitializeData (PSYSHOST host, UINT8 socket);
// APTIOV_SERVER_OVERRIDE_RC_END
void PlatformMemInitGpio(PSYSHOST host);
void PlatformMemSelectSmbSeg(PSYSHOST host, UINT8 seg);
UINT32 PlatformMemReadDimmVref(PSYSHOST host, UINT8 socket, UINT8 *vref);
UINT32 PlatformMemWriteDimmVref(PSYSHOST host, UINT8 socket, UINT8 vref);
UINT8 PlatformSetVdd(PSYSHOST host, UINT8 socket);
void PlatformReleaseADRClamps (PSYSHOST host, UINT8 socket);
UINT32 PlatformDetectADR (PSYSHOST host);
UINT32 PlatformSetErrorLEDs (PSYSHOST host, UINT8 socket);
UINT32 OemInitThrottlingEarly (PSYSHOST host);
UINT32 OemDetectDIMMConfig (PSYSHOST host);
UINT32 OemInitDdrClocks (PSYSHOST host);
UINT32 OemSetDDRFreq (PSYSHOST host);
UINT32 OemConfigureXMP (PSYSHOST host);
UINT32 OemCheckVdd (PSYSHOST host);
UINT32 OemEarlyConfig (PSYSHOST host);
UINT32 OemLateConfig (PSYSHOST host);
UINT32 OemInitThrottling (PSYSHOST host);
void PlatformCheckPORCompat(PSYSHOST host);
void PlatformHookMst(PSYSHOST host, UINT8 socket, struct smbDevice dev, UINT8 byteOffset, UINT8 *data);
UINT32 PlatformDramMemoryTest (PSYSHOST host, UINT8 socket, UINT8 chEnMap);
void OemLookupDdr4OdtTable (PSYSHOST host, struct ddr4OdtValueStruct **ddr4OdtValueStructPtr, UINT16 *ddr4OdtValueStructSize);
void OemLookupFreqTable (PSYSHOST host, struct DimmPOREntryStruct **freqTablePtr, UINT16 *freqTableLength);
VOID PlatformExMemoryLinkReset (PSYSHOST host);
UINT8 PlatformVRsSVID (PSYSHOST  host, UINT8     socket, UINT8     mcId);

// KtiHooks.c
INT32 OemKtiGetEParams (struct sysHost *host, UINT8 SocketID, UINT8 Link, UINT8 Freq, VOID *LinkSpeedParameter);
BOOLEAN OemKtiGetMmioh(struct sysHost *host, UINT16 *MmiohSize, UINT16 *MmiohBaseHi);
BOOLEAN OemKtiGetMmiol(struct sysHost *host, UINT8 *MmiolSize, UINT32 *MmiolBase);
BOOLEAN OemCheckCpuPartsChangeSwap(struct sysHost *host);
BOOLEAN OemGetAdaptedEqSettings (struct sysHost *host, UINT8 Speed, VOID *AdaptTbl);
VOID OemDebugPrintKti (struct sysHost *host,UINT32 DbgLevel, char* Format, ...);
VOID OemWaitTimeForPSBP (struct sysHost *host, UINT32 *WaitTime );
BOOLEAN OemReadKtiNvram (struct sysHost *host );


#endif // _OEMPROCMEM_INIT_H_
