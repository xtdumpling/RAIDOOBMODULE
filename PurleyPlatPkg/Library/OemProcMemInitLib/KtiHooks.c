/**
  This file contains an 'Intel Pre-EFI Module' and is licensed
  for Intel CPUs and Chipsets under the terms of your license
  agreement with Intel or your vendor.  This file may be
  modified by the user, subject to additional terms of the
  license agreement
**/
/*************************************************************************
 *
 * KTI Reference Code
 *
 * ESS - Enterprise Silicon Software
 *
 * INTEL CONFIDENTIAL
 *
 * Copyright 2006 - 2016, Intel Corporation All Rights Reserved.
 *
 * The source code contained or described herein and all documents
 * related to the source code ("Material") are owned by Intel
 * Corporation or its suppliers or licensors. Title to the Material
 * remains with Intel Corporation or its suppliers and licensors.
 * The Material contains trade secrets and proprietary and confidential
 * information of Intel or its suppliers and licensors. The Material
 * is protected by worldwide copyright and trade secret laws and treaty
 * provisions.  No part of the Material may be used, copied, reproduced,
 * modified, published, uploaded, posted, transmitted, distributed, or
 * disclosed in any way without Intel's prior express written permission.
 *
 * No license under any patent, copyright, trade secret or other
 * intellectual property right is granted to or conferred upon you
 * by disclosure or delivery of the Materials, either expressly,
 * by implication, inducement, estoppel or otherwise. Any license
 * under such intellectual property rights must be express and
 * approved by Intel in writing.
 *
 ************************************************************************/

#include "SysFunc.h"
#include "KtiMisc.h"
#ifndef MINIBIOS_BUILD
#include <Guid/SocketPciResourceData.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Library/SetupLib.h>
#include <Library/DebugLib.h>
#include <Ppi/SystemBoard.h>
#endif
#include <PlatformFuncCommon.h>

// APTIOV_SERVER_OVERRIDE_RC_START
#include <AmiMrcHooksWrapperLib\AmiMrcHooksWrapperLib.h>
// APTIOV_SERVER_OVERRIDE_RC_END

UINT8
OemGetPlatformType (
  struct sysHost          *host
  );

/**

  Get MMIO high base and size

  @param Host - pointer to SYSHOST structure
  @param MmiohSize - Region Size
  @param MmiohBase - Region Base

  @retval TRUE - get MMIO OK
  @retval FALSE - no MMIO is provided

**/
BOOLEAN
OemKtiGetMmioh (
  struct sysHost *host,
  UINT16 *MmiohSize,
  UINT16 *MmiohBaseHi
  )
{
  return FALSE;
}

/**

  Get MMIO Low base and size

  @param Host - pointer to SYSHOST structure
  @param MmiolSize - Region Size
  @param MmiolBase - Region Base

  @retval TRUE - get MMIO OK
  @retval FALSE - no MMIO is provided

**/
BOOLEAN
OemKtiGetMmiol (
  struct sysHost *host,
  UINT8 *MmiolSize,
  UINT32 *MmiolBase
  )
{
  return FALSE;
}

/**

  Check if CPU part was swapped

  @param Host - pointer to SYSHOST structure

  @retval TRUE - CPU has been swapped
  @retval FALSE - CPU has not been swapped

**/
BOOLEAN
OemCheckCpuPartsChangeSwap (
  struct sysHost *host
  )
{
  return FALSE;
}

/**

    Get adapted EQ settings

    @param Host            - pointer to SYSHOST structure
    @param Speed           - Link Speed
    @param AdaptationTable - Adaptation Table

    @retval TRUE
    @retval FALSE

**/
BOOLEAN OemGetAdaptedEqSettings (
  struct sysHost *host,
  UINT8 Speed,
  VOID *AdaptTbl
  )
{
  return FALSE;
}

/**

  KTI specific print to serial output

  @param host      - Pointer to the system host (root) structure
  @param DbgLevel  - KTI specific debug level
  @param Format    - string format

  @retval VOID

**/
VOID
OemDebugPrintKti (
  struct sysHost *host,
  UINT32 DbgLevel,
  char* Format,
  ...
  )
{

#ifdef SERIAL_DBG_MSG
  if (host->setup.kti.DebugPrintLevel & DbgLevel) {
    va_list  Marker;
    va_start (Marker, Format);
    rcVprintf (host, Format, Marker);
    va_end (Marker);
  }
#endif

  return;
}

/**

    Wait for PBSP to check in

    @param host     - Pointer to the system host (root) structure
    @param WaitTime - time in milliseconds

    @retval None

**/
VOID
OemWaitTimeForPSBP (
    struct sysHost *host,
    UINT32 *WaitTime
  )
{

  // Implement the hook here that changes the wait time in units of 1000us for PBSP to check in
  *WaitTime = 30000;    // Wait for a maximum of 30 sec
  return;
}

/**

   Locates the platform EPARAMS based on socket/link

   @param host  - Pointer to the system host (root) structure
   @param Socket - SocId we are looking up
   @param Link - Link# on socket
   @param Freq - Link frequency
   *LinkSpeedParameter - return Ptr

   @retval Ptr to EPARAM table
   @retval KTI_SUCCESS if entry found
   @retval KTI_FAILURE if no entry found

**/
INT32
OemKtiGetEParams (
  struct sysHost           *host,
  UINT8                    SocketID,
  UINT8                    Link,
  UINT8                    Freq,
  VOID                     *LinkSpeedParameter
  )
{
  ALL_LANES_EPARAM_LINK_INFO       *AllLanesPtr = NULL;
  PER_LANE_EPARAM_LINK_INFO        *PerLanePtr = NULL;
  PER_LANE_EPARAM_LINK_INFO        TempPerLaneData;
  UINT32   AllLanesSizeOfTable = 0, PerLaneSizeOfTable = 0;
#ifdef IA32
  GetKtiLaneEparam (&AllLanesPtr, &AllLanesSizeOfTable, &PerLanePtr, &PerLaneSizeOfTable);


#endif  
  // APTIOV_SERVER_OVERRIDE_RC_START : Oem Hook to Override EParamTable
  OemKtiGetEParamsOverride (host, &AllLanesPtr, &AllLanesSizeOfTable);
  // APTIOV_SERVER_OVERRIDE_RC_END : Oem Hook to Override EParamTable

  //
  // Find per bundle Table first
  //
  while (AllLanesSizeOfTable) {
    if ((AllLanesPtr->SocketID == SocketID) && (AllLanesPtr->Freq & (1 << Freq)) && (AllLanesPtr->Link & (1 << Link))) {
      //
      //Convert to per lane data structure format from all lanes data structure
      //
      TempPerLaneData.SocketID = AllLanesPtr->SocketID;
      TempPerLaneData.AllLanesUseSameTxeq = ALL_LANES_TXEQ_ENABLED;
      TempPerLaneData.Freq = AllLanesPtr->Freq;
      TempPerLaneData.Link = AllLanesPtr->Link;
      TempPerLaneData.TXEQL[0] = AllLanesPtr->AllLanesTXEQ;
      TempPerLaneData.CTLEPEAK[0] = AllLanesPtr->CTLEPEAK;
      MemCopy ((UINT8*)LinkSpeedParameter, (UINT8*) &TempPerLaneData, sizeof (PER_LANE_EPARAM_LINK_INFO));
      KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\n  Full Speed EPARAM entry located for Socket %u Link %u", SocketID, Link));
      return KTI_SUCCESS;
    }

    AllLanesPtr++;
    AllLanesSizeOfTable -= sizeof(ALL_LANES_EPARAM_LINK_INFO);
  }

  //
  // Find per lane Table if we cannot find in per bundle table
  //
  while (PerLaneSizeOfTable) {
    if ((PerLanePtr->SocketID == SocketID) && (PerLanePtr->Freq & (1 << Freq)) && (PerLanePtr->Link & (1 << Link))) {
      MemCopy ((UINT8*)LinkSpeedParameter, (UINT8*)PerLanePtr, sizeof (PER_LANE_EPARAM_LINK_INFO));
      KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\n Per Lane Full Speed EPARAM entry located for Socket %u Link %u", SocketID, Link));
      return KTI_SUCCESS;
    }

    PerLanePtr++;
    PerLaneSizeOfTable -= sizeof(PER_LANE_EPARAM_LINK_INFO);
  }

  return KTI_FAILURE;
}

/**

   Callout to allow OEM to adjust the resource map.
   Used to allow adjustment of IO ranges, bus numbers, mmioh and mmiol resource assignments
   Calling code assumes the data structure is returned in a good format.
   SBSP resources must be assigned first, with rest of sockets assigned in ascending order of NID
   Resources must be assigned in ascending consecutive order with no gaps.
   Notes: This does not allow for changing the mmcfg size of mmioh granularity
          However you can allocate multiple consecutive blocks of mmioh to increase size in a particular stack
          and adjust the mmiohbase


   @param host         - Pointer to the system host (root) structure
   @param CpuRes       - Ptr to array of Cpu Resources
   @param SystemParams - structure w/ system parameters needed

**/

VOID
OemGetResourceMapUpdate (
  struct sysHost    *host,
  KTI_CPU_RESOURCE  CpuRes[MAX_SOCKET],
  KTI_OEMCALLOUT    SystemParams
)
{

 
#ifndef MINIBIOS_BUILD
  EFI_STATUS                             Status;
  UINT8                                  Socket;
  UINT8                                  Stack;
  UINTN                                  VariableSize;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI        *PeiVariable;
  SOCKET_PCI_RESOURCE_CONFIGURATION_DATA SocketPciResourceConfiguration;
  EFI_PEI_SERVICES                       **PeiServices;
  
  PeiServices = (EFI_PEI_SERVICES **) host->var.common.oemVariable;
  Status = (*PeiServices)->LocatePpi (
                    PeiServices,
                    &gEfiPeiReadOnlyVariable2PpiGuid,
                    0,
                    NULL,
                    &PeiVariable
                  );
  ASSERT_EFI_ERROR (Status);
  if(EFI_ERROR(Status)){
    return;
  }
  
  VariableSize = sizeof (SOCKET_PCI_RESOURCE_CONFIGURATION_DATA);
  Status = PeiVariable->GetVariable (
                        PeiVariable,
                        SOCKET_PCI_RESOURCE_CONFIGURATION_DATA_NAME,
                        &gEfiSocketPciResourceDataGuid,
                        NULL,
                        &VariableSize,
                        &SocketPciResourceConfiguration
                        );
  
// APTIOV_SERVER_OVERRIDE_RC_START : Oem Hook to Override Resource Map
OemGetResourceMapUpdateOverride (Status, host, PeiVariable, CpuRes, SystemParams);
// APTIOV_SERVER_OVERRIDE_RC_END : Oem Hook to Override Resource Map   
    
  if(Status == EFI_NOT_FOUND){
    // First boot or variable is not initialize yet for use.
    return;
  }
  
  ASSERT_EFI_ERROR (Status);
  if(EFI_ERROR(Status)){
    return;
  }

  for(Socket=0; Socket < MAX_SOCKET; Socket++){
    for(Stack=0; Stack <  MAX_IIO_STACK; Stack++){
      // Equal is no change
     if ((((UINT16)SocketPciResourceConfiguration.PciResourceIoBase[(Socket* MAX_IIO_STACK)+ Stack]) < ((UINT16)SocketPciResourceConfiguration.PciResourceIoLimit[(Socket* MAX_IIO_STACK)+ Stack])) ||
            (((UINT16)SocketPciResourceConfiguration.PciResourceIoBase[(Socket* MAX_IIO_STACK)+ Stack]) > ((UINT16)SocketPciResourceConfiguration.PciResourceIoLimit[(Socket* MAX_IIO_STACK)+ Stack]))){
        CpuRes[Socket].StackRes[Stack].IoBase = (UINT16)SocketPciResourceConfiguration.PciResourceIoBase[(Socket* MAX_IIO_STACK)+ Stack];
        CpuRes[Socket].StackRes[Stack].IoLimit = (UINT16)SocketPciResourceConfiguration.PciResourceIoLimit[(Socket* MAX_IIO_STACK)+ Stack];
        
        // Equal is no change
        if ((SocketPciResourceConfiguration.PciSocketIoBase[Socket] < SocketPciResourceConfiguration.PciSocketIoLimit[Socket]) ||
              (SocketPciResourceConfiguration.PciSocketIoBase[Socket] > SocketPciResourceConfiguration.PciSocketIoLimit[Socket])) {
          CpuRes[Socket].IoBase = SocketPciResourceConfiguration.PciSocketIoBase[Socket];
          CpuRes[Socket].IoLimit = SocketPciResourceConfiguration.PciSocketIoLimit[Socket];
        }
     }

     if (((UINT32)SocketPciResourceConfiguration.PciResourceMem32Base[(Socket* MAX_IIO_STACK)+ Stack] !=0) && ((UINT32)SocketPciResourceConfiguration.PciResourceMem32Limit[(Socket* MAX_IIO_STACK)+ Stack] !=0) ||
           ((UINT32)SocketPciResourceConfiguration.PciResourceMem32Base[(Socket* MAX_IIO_STACK)+ Stack] == (UINT32)(-1)) && ((UINT32)SocketPciResourceConfiguration.PciResourceMem32Limit[(Socket* MAX_IIO_STACK)+ Stack] == 0)){
        CpuRes[Socket].StackRes[Stack].MmiolBase = (UINT32)SocketPciResourceConfiguration.PciResourceMem32Base[(Socket* MAX_IIO_STACK)+ Stack];
        CpuRes[Socket].StackRes[Stack].MmiolLimit = (UINT32)SocketPciResourceConfiguration.PciResourceMem32Limit[(Socket* MAX_IIO_STACK)+ Stack];

        // Equal is no change
        if ((SocketPciResourceConfiguration.PciSocketMmiolBase[Socket] < SocketPciResourceConfiguration.PciSocketMmiolLimit[Socket]) ||
              (SocketPciResourceConfiguration.PciSocketMmiolBase[Socket] > SocketPciResourceConfiguration.PciSocketMmiolLimit[Socket])){
          CpuRes[Socket].MmiolBase = SocketPciResourceConfiguration.PciSocketMmiolBase[Socket];
          CpuRes[Socket].MmiolLimit = SocketPciResourceConfiguration.PciSocketMmiolLimit[Socket];
        }
     }

     if (((UINT64)SocketPciResourceConfiguration.PciResourceMem64Base[(Socket* MAX_IIO_STACK)+ Stack] !=0) && ((UINT64)SocketPciResourceConfiguration.PciResourceMem64Limit[(Socket* MAX_IIO_STACK)+ Stack] !=0) ||
           ((UINT64)SocketPciResourceConfiguration.PciResourceMem64Base[(Socket* MAX_IIO_STACK)+ Stack] == (UINT64)(-1)) && ((UINT64)SocketPciResourceConfiguration.PciResourceMem64Limit[(Socket* MAX_IIO_STACK)+ Stack] == 0)){
        CpuRes[Socket].StackRes[Stack].MmiohBase.lo = (UINT32)(SocketPciResourceConfiguration.PciResourceMem64Base[(Socket* MAX_IIO_STACK)+ Stack] & 0x00000000FFFFFFFF);
        CpuRes[Socket].StackRes[Stack].MmiohBase.hi = (UINT32)((SocketPciResourceConfiguration.PciResourceMem64Base[(Socket* MAX_IIO_STACK)+ Stack] & 0xFFFFFFFF00000000) >> 32);
        CpuRes[Socket].StackRes[Stack].MmiohLimit.lo = (UINT32)(SocketPciResourceConfiguration.PciResourceMem64Limit[(Socket* MAX_IIO_STACK)+ Stack] & 0x00000000FFFFFFFF);
        CpuRes[Socket].StackRes[Stack].MmiohLimit.hi = (UINT32)((SocketPciResourceConfiguration.PciResourceMem64Limit[(Socket* MAX_IIO_STACK)+ Stack] & 0xFFFFFFFF00000000) >> 32);
        
        // Equal is no change
        if ((SocketPciResourceConfiguration.PciSocketMmiohBase[Socket] < SocketPciResourceConfiguration.PciSocketMmiohLimit[Socket]) ||
              (SocketPciResourceConfiguration.PciSocketMmiohBase[Socket] > SocketPciResourceConfiguration.PciSocketMmiohLimit[Socket])){
          CpuRes[Socket].MmiohBase.lo = (UINT32)(SocketPciResourceConfiguration.PciSocketMmiohBase[Socket] & 0x00000000FFFFFFFF);
          CpuRes[Socket].MmiohBase.hi = (UINT32)((SocketPciResourceConfiguration.PciSocketMmiohBase[Socket] & 0xFFFFFFFF00000000) >> 32);
          CpuRes[Socket].MmiohLimit.lo = (UINT32)(SocketPciResourceConfiguration.PciSocketMmiohLimit[Socket] & 0x00000000FFFFFFFF);
          CpuRes[Socket].MmiohLimit.hi = (UINT32)((SocketPciResourceConfiguration.PciSocketMmiohLimit[Socket] & 0xFFFFFFFF00000000) >> 32);
        }
     }
    } // for Stack 
  } // for Socket
#endif // MINIBIOS_BUILD

  return;
}

/**
  OEM hook before topology discovery in KTIRC.

  @param host - pointer to syshost
**/
VOID
OemHookPreTopologyDiscovery (
  struct sysHost             *host
  )
{
  return;
}

/**
  OEM hook at the end of topology discovery in KTIRC to be used for error reporting.

  @param host - pointer to syshost
**/
VOID
OemHookPostTopologyDiscovery (
  struct sysHost             *host
  )
{
  return;
}

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

  @param host - pointer to syshost

  @retval board type bitmask

**/
UINT32
OemGetBoardTypeBitmask (
  struct sysHost         *host
  )
{
  UINT32 BoardTypeBitmask = 0xFFFFFFFF;
#ifdef IA32  
  BoardTypeBitmask = GetBoardTypeBitmask();
#endif
  return BoardTypeBitmask;
}

