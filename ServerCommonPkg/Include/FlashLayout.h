//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//
/**

Copyright (c)  1999 - 2005 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file FlashLayout.h

  Platform specific flash layout.

**/

#ifndef _EFI_FLASH_LAYOUT_H_
#define _EFI_FLASH_LAYOUT_H_

#include "EfiFlashMap.h"


//
// Length of Efi Runtime Updatable Firmware Volume Header
//
#define EFI_RUNTIME_UPDATABLE_FV_HEADER_LENGTH  0x48
//
// APTIOV_SERVER_OVERRIDE_RC_START : Added to resolve redefinition error. 
//
#ifndef FLASH_BLOCK_SIZE
#define FLASH_BLOCK_SIZE            FixedPcdGet32 (PcdFlashBlockSize)
#endif
//
// APTIOV_SERVER_OVERRIDE_RC_END : Added to resolve redefinition error. 
//

//
// Flash supports 64K block size
//
#define FVB_MEDIA_BLOCK_SIZE        FLASH_BLOCK_SIZE

#define RUNTIME_FV_BASE_ADDRESS     FLASH_REGION_RUNTIME_UPDATABLE_BASE
#define RUNTIME_FV_LENGTH           (FLASH_REGION_RUNTIME_UPDATABLE_SIZE + FLASH_REGION_NV_FTW_SPARE_SIZE)
#define RUNTIME_FV_BLOCK_NUM        (RUNTIME_FV_LENGTH / FVB_MEDIA_BLOCK_SIZE)

#define FV_MAIN_BASE_ADDRESS        FLASH_REGION_FVMAIN_BASE
#define MAIN_BIOS_BLOCK_NUM         (FLASH_REGION_FVMAIN_SIZE / FVB_MEDIA_BLOCK_SIZE)

#define FV_MICROCODE_BASE_ADDRESS   FLASH_REGION_MICROCODE_FV_BASE
#define SYSTEM_MICROCODE_BLOCK_NUM  (FLASH_REGION_MICROCODE_FV_SIZE / FVB_MEDIA_BLOCK_SIZE)

#endif
