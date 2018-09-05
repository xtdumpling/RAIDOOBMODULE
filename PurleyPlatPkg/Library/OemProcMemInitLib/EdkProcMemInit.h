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

  @file EdkProcMemInit.h

  Memory Initialization Module.

**/

#ifndef _EDK_PROCMEM_INIT_H_
#define _EDK_PROCMEM_INIT_H_

#define size_t UINT32

#include "SysFunc.h"
#include "RcRegs.h"
#include "ChipApi.h"
#include "SysHostChip.h"
#include "Platform.h"
#include <PiPei.h>
#include <Setup/IioUniversalData.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Ppi/TxtSclean.h>
#include <Ppi/CpuIo.h>
#include <Guid/PlatformInfo.h>
#include <Guid/SetupVariable.h>
#include <Guid/SocketVariable.h>
#include <Guid/DebugMask.h>
#include <Guid/PlatformTxt.h>
#include <Guid/SetupBreakpointGuid.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/IoLib.h>
#include <Library/BiosGuardInitLib/BiosGuardInit.h>
#include <Library/CpuPpmLib.h>
#include <Ppi/BaseMemoryTest.h>
#include <Ppi/PlatformMemorySize.h>
#include <Ppi/PlatformMemoryRange.h>
#include <Guid/MemoryMapData.h>
#include <Guid/SmramMemoryReserve.h>
#include <Guid/AcpiVariableCompatibility.h>
#include <Guid/AcpiS3Context.h>
#include <Guid/MemoryTypeInformation.h>
#include <Library/PcdLib.h>
#include <Library/PciLib.h>
#include <Guid/LastBootErrorLogHob.h>
#include <Guid/NGNRecordsData.h>
#include <Library/PeiServicesLib.h>
#include <Ppi/MemoryDiscovered.h>
#include <Library/FpgaConfigurationLib.h>
// APTIOV_SERVER_OVERRIDE_RC_START : OEM Hook Support
#include <AmiMrcHooksWrapperLib/AmiMrcHooksWrapperLib.h>
// APTIOV_SERVER_OVERRIDE_RC_END : OEM Hook Support

//
// ------------------------ TSEG Base 
// ...............
// ------------------------ RESERVED_CPU_S3_SAVE_OFFSET
// CPU S3 data
// ------------------------ RESERVED_ACPI_S3_RANGE_OFFSET
// S3 Memory base structure
// ------------------------ TSEG + 1 page

#define RESERVED_CPU_S3_SAVE_OFFSET (RESERVED_ACPI_S3_RANGE_OFFSET - sizeof (SMM_S3_RESUME_STATE))

#define  RC_HEAP_SIZE   (32*1024)

extern EFI_GUID gEfiMemoryConfigDataHobGuid;
extern EFI_GUID gEfiRasHostGuid;
extern EFI_GUID gEfiPrevBootNGNDimmCfgVariableGuid; 
//
// SMRAM range definitions
//
#define MC_ABSEG_HSEG_PHYSICAL_START        0x000A0000
#define MC_ABSEG_HSEG_LENGTH                0x00020000
#define MC_ABSEG_CPU_START                  0x000A0000
#define MC_HSEG_CPU_START                   0xFEDA0000

//
// Maximum number of memory ranges supported
//
#define MAX_RANGES        (4 + 8)

#define MEM_EQU_4GB                     0x100000000
#define  RC_HEAP_SIZE                   (32*1024)

//
// Memory range types
//
typedef enum {
  DualChannelDdrMainMemory,
  DualChannelDdrSmramCacheable,
  DualChannelDdrSmramNonCacheable,
  DualChannelDdrGraphicsMemoryCacheable,
  DualChannelDdrGraphicsMemoryNonCacheable,
  DualChannelDdrReservedMemory,
#ifdef NVMEM_FEATURE_EN  
  DualChannelDdrADRMemory,
#endif  
  DualChannelDdrMaxMemoryRangeType
} PEI_DUAL_CHANNEL_DDR_MEMORY_RANGE_TYPE;

#pragma pack(1)

//
// Memory map range information
//
typedef struct {
  UINT8                                         RowNumber;
  EFI_PHYSICAL_ADDRESS                          PhysicalAddress;
  EFI_PHYSICAL_ADDRESS                          CpuAddress;
  EFI_PHYSICAL_ADDRESS                          RangeLength;
  PEI_DUAL_CHANNEL_DDR_MEMORY_RANGE_TYPE        Type;
} PEI_DUAL_CHANNEL_DDR_MEMORY_MAP_RANGE;

//
// HOB to save Memory S3_RestoreData
//
typedef struct {
  EFI_HOB_GUID_TYPE         EfiHobGuidType;
  //
  //  MRC_PARAMS_SAVE_RESTORE   S3_RestoreData;
  //
} HOB_SAVE_MEM_RESTORE_DATA;

typedef struct {
   UINT32     RcrbBar;
   UINT32     VtdBar;
} IIO_PLATFORM_FIXED_MAP;

#pragma pack()

VOID
RetrieveRequiredMemorySize (
  IN PSYSHOST host,
  OUT UINTN *Size
  );

VOID
RetrieveTxtStolenMemorySize (
  IN PSYSHOST host,
  IN OUT UINT64 *Size
  ) ;

VOID
SaveTxtStolenMemoryBase (
  IN PSYSHOST host,
  IN EFI_PHYSICAL_ADDRESS Base
  ) ;

EFI_STATUS
GetMemoryMap (
  IN OUT PEI_DUAL_CHANNEL_DDR_MEMORY_MAP_RANGE  *MemoryMap,
  IN OUT UINT8                                  *NumRanges,
  IN struct sysHost                             *host
  );

EFI_STATUS
InstallEfiMemory (
    IN struct sysHost             *host
  );

EFI_STATUS
InstallS3Memory (
    IN struct sysHost             *host
  );

#ifdef BDAT_SUPPORT
EFI_STATUS
SaveBdatDataToHob (
  struct sysHost         *host
  );
#endif

EFI_STATUS
MrcSaveS3RestoreDataToHob (
  IN struct sysHost      *host
  );

VOID
PublishHobData (
  struct sysHost             *host
  );

/**
  This function is called after PEI core discover memory and finish migration.

  @param[in] PeiServices    Pointer to PEI Services Table.
  @param[in] NotifyDesc     Pointer to the descriptor for the Notification event that
                            caused this function to execute.
  @param[in] Ppi            Pointer to the PPI data associated with this function.

  @retval EFI_STATUS        Always return EFI_SUCCESS
**/
EFI_STATUS
EFIAPI
PublishHobDataCallbackMemoryDiscoveredPpi (
  IN EFI_PEI_SERVICES              **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR     *NotifyDescriptor,
  IN VOID                          *Ppi
  );

/**
  This function is called to save MRC host structure into memory.

  @param[in] PeiServices    Pointer to host structure.

  @retval EFI_SUCCESS           The function complete successfully.
  @retval EFI_INVALID_PARAMETER Type is not equal to AllocateAnyPages.
  @retval EFI_NOT_AVAILABLE_YET Called with permanent memory not available.
  @retval EFI_OUT_OF_RESOURCES  The pages could not be allocated.
**/
EFI_STATUS
SaveHostToMemory(
  IN SYSHOST                       *Host
  );
  
#endif
