//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//
/**

Copyright (c)  1999 - 2015 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file FlashMap.h

  Platform specific flash layout, All PCD's are defined in .fdf file.

**/

#ifndef _FLASH_MAP_H_
#define _FLASH_MAP_H_


#define FIXED_FLASH_BLOCK_SIZE     FixedPcdGet32(PcdFlashBlockSize)

//
// APTIOV_SERVER_OVERRIDE_RC_START : Added to resolve the redefinition error
//
#ifndef FLASH_BASE
#define FLASH_BASE                                                          PcdGet32(PcdBiosBase)
#endif
#ifndef FLASH_SIZE
#define FLASH_SIZE                                                          PcdGet32(PcdBiosSize)
#endif
//
// APTIOV_SERVER_OVERRIDE_RC_END : Added to resolve the redefinition error.
//
#define FLASH_SIZE_FIXED                                                    FixedPcdGet32(PcdBiosSize)

// PEI FV definitions

#define FLASH_REGION_FV_MRC_NORMAL_BASE                                     FixedPcdGet32(PcdFlashFvMrcNormalBase)
#define FLASH_REGION_FV_MRC_NORMAL_SIZE                                     FixedPcdGet32(PcdFlashFvMrcNormalSize)

#define FLASH_REGION_MICROCODE_FV_BASE                                      FixedPcdGet32(PcdFlashNvStorageMicrocodeBase)
#define FLASH_REGION_MICROCODE_FV_SIZE                                      FixedPcdGet32(PcdFlashNvStorageMicrocodeSize)

// no #defines for BG NV storage

// No #defines for Policy

// No #defines for PEI ACM base

// No #defines for PEI ACM Oem base

#define FLASH_REGION_FV_SECPEI_BASE                                         FixedPcdGet32(PcdFlashFvSecPeiBase)
#define FLASH_REGION_FV_SECPEI_SIZE                                         FixedPcdGet32(PcdFlashFvSecPeiSize)


// Main FV definitions


#define FLASH_REGION_FVMAIN_BASE                                            FixedPcdGet32(PcdFlashFvMainBase)
#define FLASH_REGION_FVMAIN_SIZE                                            FixedPcdGet32(PcdFlashFvMainSize)

#define FLASH_REGION_OEM_FV_BASE                                            FixedPcdGet32(PcdFlashFvOemLogoBase)
#define FLASH_REGION_OEM_FV_SIZE                                            FixedPcdGet32(PcdFlashFvOemLogoSize)


#define FLASH_REGION_FVWHEA_BASE                                            FixedPcdGet32(PcdFlashFvWheaBase)
#define FLASH_REGION_FVWHEA_SIZE                                            FixedPcdGet32(PcdFlashFvWheaSize)


#define FLASH_REGION_RUNTIME_UPDATABLE_BASE                                 FixedPcdGet32(PcdFlashNvStorageVariableBase)
#define FLASH_REGION_RUNTIME_UPDATABLE_SIZE                                 FixedPcdGet32(PcdFlashNvStorageVariableSize)

#define FLASH_REGION_RUNTIME_UPDATABLE_SUBREGION_NV_VARIABLE_STORE_BASE     FixedPcdGet32(PcdFlashNvStorageVariableBase)
#define FLASH_REGION_RUNTIME_UPDATABLE_SUBREGION_NV_VARIABLE_STORE_SIZE     FixedPcdGet32(PcdFlashNvStorageVariableSize)

#define FLASH_REGION_RUNTIME_UPDATABLE_SUBREGION_NV_EVENT_LOG_BASE          FixedPcdGet32(PcdFlashFvNvStorageEventLogBase)
#define FLASH_REGION_RUNTIME_UPDATABLE_SUBREGION_NV_EVENT_LOG_SIZE          FixedPcdGet32(PcdFlashFvNvStorageEventLogSize)

#define FLASH_REGION_RUNTIME_UPDATABLE_SUBREGION_NV_FTW_WORKING_BASE        FixedPcdGet32(PcdFlashNvStorageFtwWorkingBase)
#define FLASH_REGION_RUNTIME_UPDATABLE_SUBREGION_NV_FTW_WORKING_SIZE        FixedPcdGet32(PcdFlashNvStorageFtwWorkingSize)

#define FLASH_REGION_NV_FTW_SPARE_BASE                                      FixedPcdGet32(PcdFlashNvStorageFtwSpareBase)
#define FLASH_REGION_NV_FTW_SPARE_SIZE                                      FixedPcdGet32(PcdFlashNvStorageFtwSpareSize)

#define FLASH_REGION_BIOS_ACM_FV_HEADER_BASE                                FixedPcdGet32(PcdFlashFvAcmHeaderBase)
#define FLASH_REGION_BIOS_ACM_FV_HEADER_SIZE                                FixedPcdGet32(PcdFlashFvAcmHeaderSize)

#define FLASH_REGION_BIOS_ACM_BASE                                          FixedPcdGet32(PcdFlashFvAcmBase)
#define FLASH_REGION_BIOS_ACM_SIZE                                          FixedPcdGet32(PcdFlashFvAcmSize)

#define FLASH_REGION_SINIT_ACM_BASE                                         FixedPcdGet32(PcdFlashFvAcmSinitBase)
#define FLASH_REGION_SINIT_ACM_SIZE                                         FixedPcdGet32(PcdFlashFvAcmSinitSize)







#endif // #ifndef _FLASH_MAP_H_

