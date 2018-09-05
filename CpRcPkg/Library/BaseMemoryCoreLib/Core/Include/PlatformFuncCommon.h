//
// This file contains an 'Intel Pre-EFI Module' and is licensed
// for Intel CPUs and Chipsets under the terms of your license
// agreement with Intel or your vendor.  This file may be
// modified by the user, subject to additional terms of the
// license agreement
//
/**

   Reference Code

   ESS - Enterprise Silicon Software

   INTEL CONFIDENTIAL

@copyright
  Copyright 2006 - 2016, Intel Corporation All Rights Reserved.

  The source code contained or described herein and all documents
  related to the source code ("Material") are owned by Intel
  Corporation or its suppliers or licensors. Title to the Material
  remains with Intel Corporation or its suppliers and licensors.
  The Material contains trade secrets and proprietary and confidential
  information of Intel or its suppliers and licensors. The Material
  is protected by worldwide copyright and trade secret laws and treaty
  provisions.  No part of the Material may be used, copied, reproduced,
  modified, published, uploaded, posted, transmitted, distributed, or
  disclosed in any way without Intel's prior express written permission.

  No license under any patent, copyright, trade secret or other
  intellectual property right is granted to or conferred upon you
  by disclosure or delivery of the Materials, either expressly,
  by implication, inducement, estoppel or otherwise. Any license
  under such intellectual property rights must be express and
  approved by Intel in writing.

@file
  PlatformFuncCommon.h

@brief
  This file contains platform common functions

**/
#ifndef  _platformfunccommon_h
#define  _platformfunccommon_h

#ifndef ASM_INC
#include "DataTypes.h"
#include "SysHost.h"

// OemMemoryQpiInit.c / OemProcMemInit.c

/**

  Initialize host structure with OEM specific setup data

  @param host - Pointer to sysHost, the system host (root) structure

  @retval None

**/
VOID   OemInitializePlatformData (struct sysHost *host);

/**

  OEM hook before checking and triggering the proper type of reset.
  Executed by System BSP only.

  @param host - Pointer to sysHost, the system host (root) structure

  @retval None

**/
VOID   OemCheckAndHandleResetRequests (struct sysHost *host);

/**

  OEM Hook for resetting the platform.
  Executed by System BSP only.

  @param host - Pointer to sysHost, the system host (root) structure
  @param ResetType - Warm == 0x02, Cold == 0x04

  @retval None

**/
VOID   OemIssueReset (struct sysHost *host, UINT8 ResetType);

/**

  MEBIOS hook before checking and triggering the proper type of reset.

  @param host - Pointer to sysHost, the system host (root) structure

  @retval None

**/
VOID   MEBIOSCheckAndHandleResetRequests (struct sysHost *host);

/**

  OEM hook to return Memory Riser Information

  @param host - Pointer to sysHost, the system host (root) structure

  @retval SUCCESS

**/
UINT32 OemMemRiserInfo (struct sysHost *host);

/**

  OEM hook for OEM RAS functionality

  @param host - Pointer to sysHost, the system host (root) structure
  @param flag - 0 = Pre MRC RAS hook
                1 = Post MRC RAS hook

  @retval Status - default to SUCCESS

**/
UINT32 OemMemRASHook (struct sysHost *host, UINT8  flag ); // flag = 0 (Pre MRC RAS hook), flag = 1 (Post MRC RAS hook)

/**

  OEM hook to return platform type

  @param host - Pointer to sysHost, the system host (root) structure

  @retval Platform Type

**/
UINT8  OemGetPlatformType (struct sysHost *host);

/**

  OEM hook to return platform revision ID

  @param host - Pointer to sysHost, the system host (root) structure

  @retval Platform revision ID

**/
UINT32 OemGetPlatformRevId (struct sysHost *host);

/**

  Puts the host structure variables for POST

  @param host - Pointer to sysHost, the system host (root) structure

  @retval None

**/
VOID   OemPublishDataForPost (struct sysHost *host);

/**

  Determine if VRs should be turned off on HEDT

  @param host - Pointer to sysHost, the system host (root) structure

  @retval None

**/
VOID   OemTurnOffVrsForHedt (struct sysHost *host);

/**

  OEM hook for initializing Tcrit to 105 temp offset & the config register
  which is inside of initialization of memory throttling

  @param host        - Pointer to sysHost, the system host (root) structure
  @param socket      - Socket number
  @param dev         - SMB Device
  @param byteOffset  - byte Offset
  @param data        - data

  @retval None

**/
VOID   OemPlatformHookMst (PSYSHOST host,UINT8 socket, struct smbDevice dev, UINT8 byteOffset, UINT8 *data);
#ifdef SSA_FLAG
/**

  OEM hook meant to be used by customers where they can use it to write their own jumper detection code
  and have it return FALSE when no jumper is present, thereby, blocking calling of the BSSA loader; else have it
  return TRUE if the concerned jumper is physically present. This check ensures that someone will have to be
  physically present by the platform to enable the BSSA Loader.

  @param host - Pointer to sysHost, the system host (root) structure

  @retval TRUE - This function returns TRUE by default as OEMs are meant to implement their own jumper detection
                 code(DetectPhysicalPresence).

**/
BOOLEAN OemDetectPhysicalPresenceSSA (struct sysHost *host);
#endif  //SSA_FLAG

/**

  QPI wrapper code to run after QPI Init.

  @param host        - Pointer to sysHost, the system host (root) structure
  @param QpiStatus   - Return status from MRC code execution

  @retval None

**/
VOID   OemPostQpiInit (struct sysHost *host, UINT32 QpiStatus);

/**

  QPI wrapper code to run before QPI Init.

  @param host - Pointer to sysHost, the system host (root) structure

  @retval None

**/
VOID   OemPreQpiInit (struct sysHost *host);

/**

  OEM hook to perform any platform specific uncore initialization after Uncore RC.
  This function provides the OEM hook to do any platform specific uncore initialization
  after Uncore RC.

  @param host - Pointer to sysHost, the system host (root) structure

  @retval None

**/
VOID   OemPreUncoreInit (struct sysHost *host);

// CommonSetup.c

/**

  Gets common item setup options

  @param setup - pointer to sysSetup, the setup area of sysHost structure

  @retval None

**/
void GetSetupOptions(SYSHOST *host, struct sysSetup *setup);

// MemHooks.c

/**

  Platform hook to initialize the GPIO controller

  @param host - Pointer to sysHost, the system host (root) structure

  @retval None

**/
void PlatformMemInitGpio(PSYSHOST host);

/**

  Platform hook to select the SMBus segment

  @param host - Pointer to sysHost, the system host (root) structure
  @param seg  - Desired SMBus segment

  @retval None

**/
void PlatformMemSelectSmbSeg(PSYSHOST host, UINT8 seg);

/**

  OEM hook to allow a clearing ADR status on the platform.

  @param host - Pointer to sysHost, the system host (root) structure

  @retval None

**/
void PlatformClearADR (PSYSHOST host);

/**

  OEM hook to set error LEDs.

  @param host   - Pointer to sysHost, the system host (root) structure
  @param socket - Socket number

  @retval SUCCESS    - if able to access and set LEDs.
  @retval FAILURE    - if unable to access LEDs.

**/
UINT32 PlatformSetErrorLEDs (PSYSHOST host, UINT8 socket);

/**

  This routine can be used to update CCUR

  @param host - Pointer to sysHost, the system host (root) structure

  @retval SUCCESS

**/
UINT32 OemUpdatePlatformConfig(PSYSHOST host);

/**

  OEM hook immediately before initialization of memory throttling early in the MRC

  @param host - Pointer to sysHost, the system host (root) structure

  @retval SUCCESS

**/
UINT32 OemInitThrottlingEarly (PSYSHOST host);

/**

  OEM hook immediately before DIMM detection

  @param host - Pointer to sysHost, the system host (root) structure

  @retval SUCCESS

**/
UINT32 OemDetectDIMMConfig (PSYSHOST host);

/**

  OEM hook to initialize DDR clocks

  @param host - Pointer to sysHost, the system host (root) structure

  @retval SUCCESS

**/
UINT32 OemInitDdrClocks (PSYSHOST host);

/**

  OEM hook to set the DDR frequency

  @param host - Pointer to sysHost, the system host (root) structure

  @retval SUCCESS

**/
UINT32 OemSetDDRFreq (PSYSHOST host);

/**

  OEM hook to configure XMP settings

  @param host - Pointer to sysHost, the system host (root) structure

  @retval UINT32

**/
UINT32 OemConfigureXMP (PSYSHOST host);

/**

  OEM hook to check VDD

  @param host - Pointer to sysHost, the system host (root) structure

  @retval SUCCESS

**/
UINT32 OemCheckVdd (PSYSHOST host);

/**

  OEM hook to perform early configuration

  @param host - Pointer to sysHost, the system host (root) structure

  @retval SUCCESS

**/
UINT32 OemEarlyConfig (PSYSHOST host);

/**

  OEM hook to perform late configuration

  @param host - Pointer to sysHost, the system host (root) structure

  @retval SUCCESS

**/
UINT32 OemLateConfig (PSYSHOST host);

/**

  OEM hook to initialize memory throttling

  @param host - Pointer to sysHost, the system host (root) structure

  @retval SUCCESS

**/
UINT32 OemInitThrottling (PSYSHOST host);

/**

  OEM hook to perform DRAM memory testing

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket number
  @param chEnMap - Map of enabled channels

  @retval SUCCESS

**/

/**



  @param host

  @retval UINT32

**/
UINT32 PlatformDramMemoryTest (PSYSHOST host, UINT8 socket, UINT8 chEnMap);

// CpuSetup.c

/**

  Processor specific input setup

  @param setup - pointer to sysSetup, the setup area of sysHost structure

  @retval None

**/
void GetCpuSetupOptions(struct sysSetup *setup);

// KtiSetup.c

/**

  KTI specific host structure setup options

  @param setup - pointer to sysSetup, the setup area of sysHost structure

  @retval None

**/
void GetKtiSetupOptions(struct sysSetup *setup);

// MemSetup.c

/**

  Programs input parameters for the MRC

  @param host - Pointer to sysHost, the system host (root) structure

  @retval None

**/
void GetMemSetupOptions(PSYSHOST host);

#ifdef HBM_SUPPORT
// HbmSetup.c

/**

  Programs input parameters for the HBM MRC

  @param host - Pointer to sysHost, the system host (root) structure

  @retval None

**/
void GetHbmSetupOptions(PSYSHOST host);
#endif // HBM_SUPPORT

// QpiSetup.c

/**

  QPI specific host structure setup options

  @param setup - pointer to sysSetup, the setup area of sysHost structure

  @retval None

**/
void GetQpiSetupOptions(struct sysSetup *setup);

// UncoreSetup.c

/**

  Uncore specific host structure setup options.
  Fill in some uncore specific host structure setup options.

  @param setup - pointer to sysSetup, the setup area of sysHost structure

  @retval None

**/
void GetUncoreSetupOptions(struct sysSetup *setup);

// KtiHooks.c

/**

  Locates the platform EPARAMS based on socket/link

  @param host               - Pointer to sysHost, the system host (root) structure
  @param SocketID           - SocId we are looking up
  @param Link               - Link# on socket
  @param Freq               - Link frequency
  @param LinkSpeedParameter - return Ptr

  @retval Ptr to EPARAM table
  @retval KTI_SUCCESS if entry found
  @retval KTI_FAILURE if no entry found

**/
INT32 OemKtiGetEParams (struct sysHost *host, UINT8 SocketID, UINT8 Link, UINT8 Freq, VOID *LinkSpeedParameter);

BOOLEAN OemKtiGetMmioh(struct sysHost *host, UINT16 *MmiohSize, UINT16 *MmiohBaseHi);
BOOLEAN OemKtiGetMmiol(struct sysHost *host, UINT8 *MmiolSize, UINT32 *MmiolBase);

/**

  Check if CPU part was swapped

  @param host - Pointer to sysHost, the system host (root) structure

  @retval TRUE - CPU has been swapped
  @retval FALSE - CPU has not been swapped

**/
BOOLEAN OemCheckCpuPartsChangeSwap(struct sysHost *host);

//MemHooks.c

/**

    Get adapted EQ settings

    @param host     - Pointer to sysHost, the system host (root) structure
    @param Speed    - Link Speed
    @param AdaptTbl - Adaptation Table

    @retval TRUE
    @retval FALSE

**/
BOOLEAN OemGetAdaptedEqSettings (struct sysHost *host, UINT8 Speed, VOID *AdaptTbl);

/**

  KTI specific print to serial output

  @param host      - Pointer to sysHost, the system host (root) structure
  @param DbgLevel  - KTI specific debug level
  @param Format    - string format

  @retval None

**/
VOID OemDebugPrintKti (struct sysHost *host,UINT32 DbgLevel, char* Format, ...);

/**

    Wait for PBSP to check in

    @param host     - Pointer to sysHost, the system host (root) structure
    @param WaitTime - time in milliseconds

    @retval None

**/
VOID OemWaitTimeForPSBP (struct sysHost *host, UINT32 *WaitTime );

/**

  Early Video Support

  @retval None

**/
void OemEarlyVideo (VOID);

/**
    To get KTI Alllanes and PerLane Eparams
    @param Pointer to AllLanes Eparam table
           Size of AllLanes Eparam table
           Pointer to PerLane Eparam table
           Size of PerLane Eparam table

    @retval VOID
**/
VOID
GetKtiLaneEparam (
  ALL_LANES_EPARAM_LINK_INFO      **PtrAllLanes,
  UINT32                          *SizeOfAallLanesTable, 
  PER_LANE_EPARAM_LINK_INFO       **PtrPerLane,
  UINT32                          *SizeOfPerLaneTable
  );

/**

  Get the board type bitmask.
    Bits[3:0]   - Socket0
    Bits[7:4]   - Socket1
    Bits[11:8]  - Socket2
    Bits[15:12] - Socket3
    Bits[19:16] - Socket4
    Bits[23:20] - Socket5
    Bits[27:24] - Socket6
    Bits[31:28] - Socket7

  Within each Socket-specific field, bits mean:
    Bit0 = CPU_TYPE_STD support; always 1 on Socket0
    Bit1 = CPU_TYPE_F support
    Bit2 = CPU_TYPE_P support
    Bit3 = reserved
  @retval board type bitmask
**/
UINT32
GetBoardTypeBitmask();

/**

  OemAfterAddressMapConfigured - OEM hook after address map is configured.

  @param host  - Pointer to sysHost

  @retval N/A

**/
VOID OemAfterAddressMapConfigured(PSYSHOST host);

#endif // ASM_INC
#endif  //_platformfunccommon_h
