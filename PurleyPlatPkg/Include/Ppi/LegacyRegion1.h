//
// This file contains 'Framework Code' and is licensed as such 
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.                 
//
/**

Copyright (c)  2007 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file LegacyRegion.h

  This PPI manages the legacy memory regions between 0xc0000 - 0xfffff

**/

#ifndef _EFI_LEGACY_REGION_PPI_H_
#define _EFI_LEGACY_REGION_PPI_H_

#define PEI_LEGACY_REGION_PPI_GUID \
  { \
    0x1fae70ac, 0x8203, 0x48f0, 0xb9, 0xef, 0xe8, 0x73, 0xff, 0xec, 0xc1, 0xf2 \
  }

typedef struct _PEI_LEGACY_REGION_PPI PEI_LEGACY_REGION_PPI;

typedef
EFI_STATUS
(EFIAPI *PEI_LEGACY_REGION_DECODE) (
  IN  EFI_PEI_SERVICES                    **PeiServices,
  IN  PEI_LEGACY_REGION_PPI               * This,
  IN  UINT32                              Start,
  IN  UINT32                              Length,
  IN  BOOLEAN                             *On
  );

typedef
EFI_STATUS
(EFIAPI *PEI_LEGACY_REGION_LOCK) (
  IN  EFI_PEI_SERVICES                    **PeiServices,
  IN  PEI_LEGACY_REGION_PPI               * This,
  IN  UINT32                              Start,
  IN  UINT32                              Length,
  OUT UINT32                              *Granularity OPTIONAL
  );

typedef
EFI_STATUS
(EFIAPI *PEI_LEGACY_REGION_BOOT_LOCK) (
  IN  EFI_PEI_SERVICES                    **PeiServices,
  IN  PEI_LEGACY_REGION_PPI               * This,
  IN  UINT32                              Start,
  IN  UINT32                              Length,
  OUT UINT32                              *Granularity OPTIONAL
  );

typedef
EFI_STATUS
(EFIAPI *PEI_LEGACY_REGION_UNLOCK) (
  IN  EFI_PEI_SERVICES                    **PeiServices,
  IN  PEI_LEGACY_REGION_PPI               * This,
  IN  UINT32                              Start,
  IN  UINT32                              Length,
  OUT UINT32                              *Granularity OPTIONAL
  );

typedef struct _PEI_LEGACY_REGION_PPI {
  PEI_LEGACY_REGION_DECODE    Decode;
  PEI_LEGACY_REGION_LOCK      Lock;
  PEI_LEGACY_REGION_BOOT_LOCK BootLock;
  PEI_LEGACY_REGION_UNLOCK    UnLock;
} PEI_LEGACY_REGION_PPI;

extern EFI_GUID gPeiLegacyRegionPpiGuid;

#endif
