/**
  This file contains an 'Intel Pre-EFI Module' and is licensed
  for Intel CPUs and Chipsets under the terms of your license
  agreement with Intel or your vendor.  This file may be
  modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c) 2005-2017 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

  @file OemProcMemInitX64.c

--*/

#ifdef _MSC_VER
#pragma warning(disable : 4100)
#endif //_MSC_VER

#ifndef MINIBIOS_BUILD
#include "EdkProcMemInit.h"
#include <Library/OemProcMemInit.h>
#define FILE_NAME "OemProcMemInit.c"
#include <Platform.h>
#include <Guid/MemoryOverwriteControl.h>
#include <Library/SetupLib.h>

#if ME_SUPPORT_FLAG
#include <HeciRegs.h>
#include <MeAccess.h>
#include <MeUma.h>
#endif // ME_SUPPORT_FLAG

#include <Library/ReportStatusCodeLib.h>
#include <Library/PlatformStatusCodes.h>
#include <Library/BootGuardLib.h>
#else //MINIBIOS_BUILD
#include "OemProcMemInit.h"
#endif //MINIBIOS_BUILD

#ifndef MINIBIOS_BUILD
extern EFI_GUID gEfiMemoryConfigDataGuid;
extern EFI_GUID gEfiOcDataGuid;
EFI_GUID gEfiAfterMrcGuid = EFI_AFTER_MRC_GUID;
EFI_GUID gEfiAfterKtircGuid = EFI_AFTER_QPIRC_GUID;



VOID   OemInitSerialDebug (PSYSHOST host);

//
// Function Implementations
//
void setCPLDClk(UINT8 level)
{
///TODO:Add setting CPLD clock with proper GPIOs for LBG/SPT platforms
}

void setCPLDData(UINT8 level)
{
///TODO:Add setting CPLD clock with proper GPIOs for LBG/SPT platforms
}


void spinloop(UINT32 count)
{
  volatile UINT32 i;
  for (i = 0; i < count; i++);
}

void sendCPLDByte(UINT8 data)
{
  int i;

  for (i=0; i<8; i++) {
    setCPLDClk(0);
    spinloop(50000);

    setCPLDData(((data & 0x80) ? 1 : 0));
    spinloop(50000);

    setCPLDClk(1);
    data = data << 1;
    spinloop(50000);
  }
}



#endif // MINIBIOS_BUILD
/**
  Get the size of ME Segment from the ME FW.

  @param host - pointer to the system host structure

  @retval host->var.common.meRequestedSize is updated
**/
VOID
OemGetMeRequestedSegmentSize (
  PSYSHOST host
  )
{
}
#ifndef MINIBIOS_BUILD
/**
**/
VOID
OemGetPlatformSetupValueforCommonRc(
  struct sysHost             *host,
  // APTIOV_SERVER_OVERRIDE_RC_START : To solve redefinition error
  INTEL_SETUP_DATA        *SetupData 
  // APTIOV_SERVER_OVERRIDE_RC_END : To solve redefinition error
  )
{
}

VOID
OemGetPlatformSetupValueforCpu(
  struct sysHost             *host,
  // APTIOV_SERVER_OVERRIDE_RC_START : To solve redefinition error
  INTEL_SETUP_DATA        *SetupData
  // APTIOV_SERVER_OVERRIDE_RC_END : To solve redefinition error
  )
{
  
}

 
/**
**/
VOID
OemGetPlatformSetupValueforKTI(
  struct sysHost             *host,
  // APTIOV_SERVER_OVERRIDE_RC_START : To solve redefinition error
  INTEL_SETUP_DATA        *SetupData
  // APTIOV_SERVER_OVERRIDE_RC_END : To solve redefinition error
  )
{
}

//
// Function Implementations
//
/**
**/
VOID
OemGetPlatformSetupValue(
  struct sysHost             *host,
  // APTIOV_SERVER_OVERRIDE_RC_START : To solve redefinition error
  INTEL_SETUP_DATA        *SetupData
  // APTIOV_SERVER_OVERRIDE_RC_END : To solve redefinition error
  )
{

}
#endif // MINIBIOS_BUILD


/**

    GC_TODO: add routine description

    @param host - GC_TODO: add arg description

    @retval SUCCESS - GC_TODO: add retval description

**/
UINT32
OemMemRiserInfo (
    struct sysHost             *host
  )
{
  return SUCCESS;
}
/*++

Routine Description:

  OEM hook to enable ADR related platform specific features.
  This routine enables batterybacked feature on all the supported channels.
  This routine also sets up CPLD into one of two mode (battery backed mode or C2F mode)
  Note that CPLD setup code is specific to Emerald Point platform.
  This routine needs to be updated with any other board specific changes related to ADR.

Arguments:

  host    - Pointer to SysHost structure.
  SetupData - SetupData

Returns:

  None

--*/

#ifndef MINIBIOS_BUILD
VOID OemStoragePolicy (
  struct sysHost             *host,
  // APTIOV_SERVER_OVERRIDE_RC_START : To solve redefinition error
  INTEL_SETUP_DATA        *SetupData
  // APTIOV_SERVER_OVERRIDE_RC_END : To solve redefinition error
  )
{
}

VOID
OemForceSetupDefault (
    struct sysHost             *host
  )
{

}

VOID
OemNmBootPolicy (
  struct sysHost             *host,
  // APTIOV_SERVER_OVERRIDE_RC_START : To solve redefinition error
  INTEL_SETUP_DATA        *SetupData,
  // APTIOV_SERVER_OVERRIDE_RC_END : To solve redefinition error
  EFI_BOOT_MODE              BootMode
  )
{
}

VOID
OemSecurityPolicy (
  struct sysHost             *host,
  // APTIOV_SERVER_OVERRIDE_RC_START : To solve redefinition error
  INTEL_SETUP_DATA        *SetupData,
  // APTIOV_SERVER_OVERRIDE_RC_END : To solve redefinition error
  EFI_BOOT_MODE              BootMode
  )
{
}

VOID
OemSaveMemNvram (
  struct sysHost             *host
  )
{
}
#endif // MINIBIOS_BUILD

/**

  MRC wrapper code.
  Initialize host structure with OEM specific setup data

    @param host = pointer to sysHost structure

**/
VOID
OemInitializePlatformData (
    struct sysHost             *host
  )
{
}
/**

  Initialize prevBootNGNDimmCfg with data from previous boot

    @param host = pointer to sysHost structure
    @param prevBootNGNDimmCfg = pointer to prevBootNGNDimmCfg structure

**/
UINT32
UpdatePrevBootNGNDimmCfg(
    struct sysHost             *host, 
    struct prevBootNGNDimmCfg  *prevBootNGNDimmCfg)
    
{
    
return SUCCESS;    
    
}
/**

  OemPreMemoryInit  - OEM hook pre memory init

  @param host  - Pointer to sysHost

**/

UINT32
OemPreMemoryInit (
  PSYSHOST host
  )
{
  return SUCCESS; 
}

/**

  KTI wrapper code.

    @param host = pointer to sysHost structure
    KtiStatus - Return status from MRC code execution

    @retval VOID

**/
VOID
OemKtiInit (
    struct sysHost   *host,
    UINT32           KtiStatus
  )
{

}

#ifdef SERIAL_DBG_MSG
/**

  Oem Init Serial Debug

    @param host = pointer to sysHost structure
    KtiStatus - Return status from MRC code execution

    @retval VOID

**/
VOID
OemInitSerialDebug (
    PSYSHOST host
    )
{

}
#endif  //  SERIAL_DBG_MSG


/**

  MRC wrapper code.
  Copy host structure to system memory buffer after MRC when memory becomes available

    @param host = pointer to sysHost structure
    MrcStatus - Return status from MRC code execution

@retval VOID

**/
VOID
OemSendDramInitDoneCommand (
    struct sysHost   *host,
    UINT32           MrcStatus
  )

{
}

/**

  MRC wrapper code.
  Copy host structure to system memory buffer after MRC when memory becomes available

    @param host = pointer to sysHost structure
    MrcStatus - Return status from MRC code execution

@retval VOID

**/
VOID
OemPostMemoryInit (
    struct sysHost   *host,
    UINT32           MrcStatus
  )

{
}

/**

  Proc wrapper code.

    @param host = pointer to sysHost structure

    @retval VOID

**/
VOID
OemPreProcInit (
    struct sysHost   *host
  )
{
}

/**

  Proc wrapper code.

    @param host = pointer to sysHost structure
    ProcStatus - Return status from Proc RC code execution

    @retval VOID

**/
VOID
OemPostProcInit (
    struct sysHost   *host,
    UINT32           ProcStatus
  )
{
}

/**

  Executed by System BSP only.
  OEM hook before checking and triggering the proper type of reset.

  @param host - pointer to sysHost structure on stack

  @retval VOID

**/
VOID
OemCheckAndHandleResetRequests (
  struct sysHost             *host
  )
{
  //
  // Perform OEM tasks before a reset might be triggered after control returns to the caller
  //
  return;
}

#if defined(ME_SUPPORT_FLAG) && ME_SUPPORT_FLAG
/**

  MEBIOS hook before checking and triggering the proper type of reset.

  @param host - pointer to sysHost structure on stack

  @retval VOID

**/
VOID
MEBIOSCheckAndHandleResetRequests(
  struct sysHost             *host
  )
{
  return;
}
#endif // ME_SUPPORT_FLAG



/**

  OEM hook to do any platform specifc init

  @param host - pointer to sysHost structure on stack
  @param Node - memory controller number ( 0 based)


  @retval UINT8

**/
VOID
OemPostCpuInit (
  struct sysHost             *host,
  UINT8                      socket
  )

//
{
#ifndef MINIBIOS_BUILD
  //
  // Report Status Code EFI_CU_PC_INIT_BEGIN
  //
  REPORT_STATUS_CODE (
    EFI_PROGRESS_CODE,
    (EFI_COMPUTING_UNIT_HOST_PROCESSOR | EFI_CU_PC_INIT_BEGIN)
    );
#endif // !MINIBIOS_BUILD
}

/**

  Puts the host structure variables for POST

  @param host - pointer to sysHost structure on stack

@retval VOID

**/
VOID
OemPublishDataForPost (
  struct sysHost             *host
  )
{

}

#ifdef MINIBIOS_BUILD
// Define intrinsic functions to satisfy the .NET 2008 compiler with size optimization /O1
// compiler automatically inserts memcpy/memset fn calls in certain cases
void *memcpy(void *dst, void *src, size_t cnt);
void *memset(void *dst, char value, size_t cnt);
#ifndef __GNUC__
#pragma function(memcpy,memset)
#endif //__GNUC__

void *memcpy(void *dst, void *src, size_t cnt)
{
  MemCopy((UINT8 *)dst, (UINT8 *)src, (UINT32) cnt);
  return dst;
}

void *memset(void *dst, char value, size_t cnt)
{
  MemSetLocal((UINT8 *)dst, (UINT32) value, (UINT32) cnt);
  return dst;
}
#endif

/**

    GC_TODO: add routine description

    @param host        - GC_TODO: add arg description
    @param majorCode   - GC_TODO: add arg description
    @param minorCode   - GC_TODO: add arg description
    @param haltOnError - GC_TODO: add arg description

    @retval None

**/
void
OemPlatformFatalError (
  PSYSHOST host,
  UINT8    majorCode,
  UINT8    minorCode,
  UINT8    *haltOnError
  )
{
  return;
}


/**

  Determine if VRs should be turned off on HEDT

  @param host - pointer to sysHost structure on stack

@retval VOID

**/
VOID
OemTurnOffVrsForHedt (
  struct sysHost             *host
  )
{
#ifndef MINIBIOS_BUILD
  //
  // If HEDT
  //
  if (host->nvram.common.platformType == TypeHedtCRB) {
    if (host->setup.cpu.vrsOff ) {
      //
      // Determine which GPIO pins to assert based on memory configuration. These pins turn off power rails to memory VRs
      //
      /* SKX_TODO - Update GPIO
      if ( !(host->nvram.mem.socket[0].channelList[0].enabled) && !(host->nvram.mem.socket[0].channelList[1].enabled) ) {
        //
        // Channmel 0/1 is empty, so assert V_SM_C01_LOT26 (GPIO15)
        //
        IoWrite32((host->var.common.gpioBase + 0x0C),(IoRead32((host->var.common.gpioBase + 0x0C)) | (1 << 15)));
      }

      if ( !(host->nvram.mem.socket[0].channelList[2].enabled) && !(host->nvram.mem.socket[0].channelList[3].enabled) ) {
        //
        // Channmel 2/3 is empty, so assert V_SM_C23_LOT26 (GPIO28)
        //
        IoWrite32((host->var.common.gpioBase + 0x0C),(IoRead32((host->var.common.gpioBase + 0x0C)) | (1 << 28)));
      }*/
    }  // If setup = enable
  }    // If HEDT
#endif // MINIBIOS_BUILD
  return;
}

/**

    GC_TODO: add routine description

    @param host                 - GC_TODO: add arg description
    @param SizeOfTable          - GC_TODO: add arg description
    @param per_lane_SizeOfTable - GC_TODO: add arg description
    @param ptr                  - GC_TODO: add arg description
    @param per_lane_ptr         - GC_TODO: add arg description

    @retval FALSE - GC_TODO: add retval description

**/
UINT32
OemPlatformSpecificEParam (
  struct sysHost             *host,
  UINT32                     *SizeOfTable,
  UINT32                     *per_lane_SizeOfTable,
  ALL_LANES_EPARAM_LINK_INFO **ptr,
  PER_LANE_EPARAM_LINK_INFO  **per_lane_ptr
  )
{
  return FALSE;
}

/**

  OEM hook for initializing Tcrit to 105 temp offset & the config register
  which is inside of initialization of memory throttling

  @param host              - pointer to sysHost
  @param socket            - socket number
  @param smbDevice dev     - SMB Device
  @param byteOffset        - byte Offset
  @param data              - data

**/
void
OemPlatformHookMst (
  PSYSHOST host,
  UINT8    socket,
  struct   smbDevice dev,
  UINT8    byteOffset,
  UINT8    *data
  )
{
  return;
}


BOOLEAN
OemReadKtiNvram (
  struct sysHost             *host
  )
  {
    return FALSE;
  }
