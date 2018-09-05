//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//
/*++

Copyright (c)  2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

 FpgaFvDataLibInternal.h

Abstract:

  This file provides intermal function headers for FpgaFvDataLib

--*/

#include <Base.h>
#include <PiPei.h>
#include <Uefi.h>
#include <Uefi/UefiBaseType.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/UefiLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/PcdLib.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Protocol/FirmwareVolume2.h>
#include <Library/FpgaFvDataLib.h>


#ifndef FPGA_FVDATALIB_INTERNAL_H_
#define FPGA_FVDATALIB_INTERNAL_H_


/**

 Returns the Raw image from the FV

 @param NameGuid  - Guid of the File Image
 @param Buffer    - Buffer
 @param Size      - Buffer Size

 @retval EFI_NOT_FOUND
 @retval EFI_SUCCESS

 **/
EFI_STATUS FpgaGetRawImage(
  IN EFI_GUID *NameGuid,
  IN OUT VOID **Buffer, IN OUT UINTN *Size
  );

#endif /* PURLEYSKTPKG_LIBRARY_FPGACONFIGURATIONLIB_FPGACONFIGURATIONLIBINTERNAL_H_ */
