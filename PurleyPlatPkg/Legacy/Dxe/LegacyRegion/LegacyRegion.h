//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/**

Copyright (c)  1999 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file LegacyRegion.h

  This code supports a the private implementation 
  of the Legacy Region protocol.

**/

#ifndef LEGACY_REGION_H_
#define LEGACY_REGION_H_

#include <PiDxe.h>
#include <Setup/IioUniversalData.h>
#include <IndustryStandard/Pci.h>
#include <Protocol/IioUds.h>
#include <Protocol/LegacyRegion2.h>
#include <Protocol/CpuCsrAccess.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include "RcRegs.h"

#define LEGACY_REGION_INSTANCE_SIGNATURE  SIGNATURE_32 ('R', 'E', 'G', 'N')

//
// Compile time calculation of the number of elements in the mPamData[]
//
#define PAM_DATA_NO (sizeof (mPamData) / sizeof (mPamData[0]))

#define LEGACY_REGION_INSTANCE_FROM_THIS(this) \
  CR ( \
  this, \
  LEGACY_REGION_INSTANCE, \
  LegacyRegion, \
  LEGACY_REGION_INSTANCE_SIGNATURE \
  )


//
// Three valued logical type.
// Yes, logical systems based on more than two (Boolean) values exist.
//
typedef enum {
  Bool3False    = FALSE,
  Bool3True     = TRUE,
  Bool3DontKnow = TRUE + TRUE,  // Guarantees to be different from TRUE and FALSE
} BOOLEAN3;


#pragma pack(1)

//
// Structure holding
//

typedef struct {
  UINT32  Start;      // Address of the range
  UINT32  Length;     // End Addr of the range
  UINT8   PamOffset;  // Pam Offset
} PAM_DATA;

#pragma pack()

typedef struct {
  UINT32                          Signature;
  EFI_HANDLE                      Handle;
  EFI_LEGACY_REGION2_PROTOCOL     LegacyRegion;
  EFI_HANDLE                      ImageHandle;
  UINT32                          SocketPresentBitMap;
} LEGACY_REGION_INSTANCE;



/**

  Install Driver to produce Legacy Region protocol.

  @param ImageHandle  -  The image handle.
  @param SystemTable  -  The system table.

  @retval EFI_SUCCESS  -  Legacy Region protocol installed.
  @retval Other        -  No protocol installed, unload driver.

**/
EFI_STATUS
EFIAPI
LegacyRegionInstall (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
;

#endif
