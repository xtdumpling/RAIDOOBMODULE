/**
  This file contains an 'Intel Pre-EFI Module' and is licensed
  for Intel CPUs and Chipsets under the terms of your license
  agreement with Intel or your vendor.  This file may be
  modified by the user, subject to additional terms of the
  license agreement
**/
/*************************************************************************
 *
 * UBA KTI Library
 *
 * ESS - Enterprise Silicon Software
 *
 * INTEL CONFIDENTIAL
 *
 * Copyright 2016, Intel Corporation All Rights Reserved.
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

#ifndef MINIBIOS_BUILD
#include <Library/DebugLib.h>
#include <Library/UbaKtiLib.h>
#include <Ppi/UbaCfgDb.h>
#include <KtiSetupDefinitions.h>
#include <Library/PeiServicesLib.h>
#include <Library/PcdLib.h>

extern EFI_GUID gPlatformKtiEparamUpdateDataGuid; 
#else
#include "KtiLib.h"
#include "KtiMisc.h"
#include "SysHost.h"
#include "SysFunc.h"
#include "KtiPlatform.h"
#include "KtiSetupDefinitions.h"
#endif

#pragma pack(1)

ALL_LANES_EPARAM_LINK_INFO  Kti8SAllLanesEparamTable[] = {
  //
  //SocketID, Freq, Link, TXEQ, CTLEPEAK
  //  
  
  //
  // Socket 0
  //
  {0x0, (1 << SPEED_REC_96GT) | (1 << SPEED_REC_104GT), (1 << KTI_LINK0), 0x313A363F, ADAPTIVE_CTLE},
  {0x0, (1 << SPEED_REC_96GT) | (1 << SPEED_REC_104GT), (1 << KTI_LINK1), 0x333E343F, ADAPTIVE_CTLE},
  {0x0, (1 << SPEED_REC_96GT) | (1 << SPEED_REC_104GT), (1 << KTI_LINK2), 0x2D34383F, ADAPTIVE_CTLE},

  //
  // Socket 1
  //
  {0x1, (1 << SPEED_REC_96GT) | (1 << SPEED_REC_104GT), (1 << KTI_LINK0), 0x2E35383F, ADAPTIVE_CTLE},
  {0x1, (1 << SPEED_REC_96GT) | (1 << SPEED_REC_104GT), (1 << KTI_LINK1), 0x313B353F, ADAPTIVE_CTLE},
  {0x1, (1 << SPEED_REC_96GT) | (1 << SPEED_REC_104GT), (1 << KTI_LINK2), 0x2F38363F, ADAPTIVE_CTLE},
  
  //
  // Socket 2
  //
  {0x2, (1 << SPEED_REC_96GT) | (1 << SPEED_REC_104GT), (1 << KTI_LINK0), 0x2F37373F, ADAPTIVE_CTLE},
  {0x2, (1 << SPEED_REC_96GT) | (1 << SPEED_REC_104GT), (1 << KTI_LINK1), 0x343E353F, ADAPTIVE_CTLE},
  {0x2, (1 << SPEED_REC_96GT) | (1 << SPEED_REC_104GT), (1 << KTI_LINK2), 0x2A31383F, ADAPTIVE_CTLE},

  //
  // Socket 3
  //
  {0x3, (1 << SPEED_REC_96GT) | (1 << SPEED_REC_104GT), (1 << KTI_LINK0), 0x313B353F, ADAPTIVE_CTLE},
  {0x3, (1 << SPEED_REC_96GT) | (1 << SPEED_REC_104GT), (1 << KTI_LINK1), 0x323B363F, ADAPTIVE_CTLE},
  {0x3, (1 << SPEED_REC_96GT) | (1 << SPEED_REC_104GT), (1 << KTI_LINK2), 0x2E35383F, ADAPTIVE_CTLE},

  //
  // Socket 4
  //
  {0x4, (1 << SPEED_REC_96GT) | (1 << SPEED_REC_104GT), (1 << KTI_LINK0), 0x2831363F, ADAPTIVE_CTLE},
  {0x4, (1 << SPEED_REC_96GT) | (1 << SPEED_REC_104GT), (1 << KTI_LINK1), 0x2830373F, ADAPTIVE_CTLE},
  {0x4, (1 << SPEED_REC_96GT) | (1 << SPEED_REC_104GT), (1 << KTI_LINK2), 0x2830373F, ADAPTIVE_CTLE},

  //
  // Socket 5
  //
  {0x5, (1 << SPEED_REC_96GT) | (1 << SPEED_REC_104GT), (1 << KTI_LINK0), 0x2932363F, ADAPTIVE_CTLE},
  {0x5, (1 << SPEED_REC_96GT) | (1 << SPEED_REC_104GT), (1 << KTI_LINK1), 0x2931373F, ADAPTIVE_CTLE},
  {0x5, (1 << SPEED_REC_96GT) | (1 << SPEED_REC_104GT), (1 << KTI_LINK2), 0x2931373F, ADAPTIVE_CTLE},

  //
  // Socket 6
  //
  {0x6, (1 << SPEED_REC_96GT) | (1 << SPEED_REC_104GT), (1 << KTI_LINK0), 0x2A33363F, ADAPTIVE_CTLE},
  {0x6, (1 << SPEED_REC_96GT) | (1 << SPEED_REC_104GT), (1 << KTI_LINK1), 0x2932363F, ADAPTIVE_CTLE},
  {0x6, (1 << SPEED_REC_96GT) | (1 << SPEED_REC_104GT), (1 << KTI_LINK2), 0x2932363F, ADAPTIVE_CTLE},

  //
  // Socket 7
  //
  {0x7, (1 << SPEED_REC_96GT) | (1 << SPEED_REC_104GT), (1 << KTI_LINK0), 0x2A33363F, ADAPTIVE_CTLE},
  {0x7, (1 << SPEED_REC_96GT) | (1 << SPEED_REC_104GT), (1 << KTI_LINK1), 0x2932363F, ADAPTIVE_CTLE},
  {0x7, (1 << SPEED_REC_96GT) | (1 << SPEED_REC_104GT), (1 << KTI_LINK2), 0x2932363F, ADAPTIVE_CTLE}
};

UINT32 Kti8SAllLanesEparamTableSize  = sizeof (Kti8SAllLanesEparamTable);

#pragma pack()

/**
    To get default KTI Alllanes and PerLane Eparams
    @param Pointer to AllLanes Eparam table
           Size of AllLanes Eparam table
           Pointer to PerLane Eparam table
           Size of PerLane Eparam table

    @retval VOID
**/
VOID
GetDefaultKtiLaneEparam (
  IN OUT ALL_LANES_EPARAM_LINK_INFO           **PtrAllLanes,
  IN OUT UINT32                               *SizeOfAllLanesTable,
  IN OUT PER_LANE_EPARAM_LINK_INFO            **PtrPerLane,
  IN OUT UINT32                               *SizeOfPerLaneTable
  )
{

  *SizeOfAllLanesTable = Kti8SAllLanesEparamTableSize;
  *PtrAllLanes = (ALL_LANES_EPARAM_LINK_INFO *) &Kti8SAllLanesEparamTable;
  *SizeOfPerLaneTable = 0;
  *PtrPerLane  = NULL;
#ifndef MINIBIOS_BUILD
  DEBUG((DEBUG_ERROR,"[UBA KTI] Platform kti lane eparam default config data is loaded! \n"));
#endif
}

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
  IN OUT ALL_LANES_EPARAM_LINK_INFO          **PtrAllLanes,
  IN OUT UINT32                               *SizeOfAallLanesTable, 
  IN OUT PER_LANE_EPARAM_LINK_INFO           **PtrPerLane,
  IN OUT UINT32                               *SizeOfPerLaneTable
  )
{

#ifndef MINIBIOS_BUILD

  EFI_STATUS                        Status;
  UBA_CONFIG_DATABASE_PPI           *UbaConfigPpi = NULL;
  PLATFORM_KTI_EPARAM_UPDATE_TABLE  KtiEparamTable;
  UINTN                             TableSize;
  
  Status = PeiServicesLocatePpi (
              &gUbaConfigDatabasePpiGuid, 
              0,
              NULL,
              &UbaConfigPpi
              );
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_ERROR,"[UBA KTI] Locate UbaConfigPPi failure - %r\n", Status));
    ASSERT_EFI_ERROR (Status);
  }
  
  TableSize = sizeof(KtiEparamTable);
  Status = UbaConfigPpi->GetData (
                                 UbaConfigPpi, 
                                 &gPlatformKtiEparamUpdateDataGuid, 
                                 &KtiEparamTable, 
                                 &TableSize
                                 );
                                 
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_ERROR,"[UBA KTI] Get platform kti lane eparam data failure - %r\n", Status));
    ASSERT_EFI_ERROR (Status);
  }
  
  ASSERT (KtiEparamTable.Signature == PLATFORM_KTIEP_UPDATE_SIGNATURE);
  ASSERT (KtiEparamTable.Version == PLATFORM_KTIEP_UPDATE_VERSION);
  
  *PtrAllLanes = KtiEparamTable.AllLanesEparamTablePtr;
  *SizeOfAallLanesTable = (UINT32) (KtiEparamTable.SizeOfAllLanesEparamTable/sizeof(ALL_LANES_EPARAM_LINK_INFO));
  
  *PtrPerLane = KtiEparamTable.PerLaneEparamTablePtr;
  *SizeOfPerLaneTable = (UINT32)(KtiEparamTable.SizeOfPerLaneEparamTable/sizeof(PER_LANE_EPARAM_LINK_INFO));
  
  return;

#else

  GetDefaultKtiLaneEparam (PtrAllLanes, SizeOfAallLanesTable, PtrPerLane, SizeOfPerLaneTable);

#endif
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
  @retval board type bitmask
**/

UINT32
GetBoardTypeBitmask (
  )
{

//
// Allow standard CPUs on all possible 8 slots in any board.
//
UINT32 BoardTypeBitmask = CPU_TYPE_STD_MASK | (CPU_TYPE_STD_MASK << 4) | (CPU_TYPE_STD_MASK << 8) |
                      (CPU_TYPE_STD_MASK << 12) | (CPU_TYPE_STD_MASK << 16) | (CPU_TYPE_STD_MASK << 20) |
                      (CPU_TYPE_STD_MASK << 24) | (CPU_TYPE_STD_MASK << 28);

#ifndef MINIBIOS_BUILD
  BoardTypeBitmask |= PcdGet32(PcdBoardTypeBitmask);
#else
  BoardTypeBitmask |= (CPU_TYPE_F_MASK | (CPU_TYPE_F_MASK << 4));
#endif //MINIBIOS_BUILD


  return BoardTypeBitmask;
}

