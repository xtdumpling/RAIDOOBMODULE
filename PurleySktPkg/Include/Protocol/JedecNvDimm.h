//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//
/**+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) 2014 Intel Corporation.  All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license.  Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

    @file JedecNvDimm.h

    Header file for NVM tech Access protocol

---------------------------------------------------------------------------**/
#ifndef _JEDEC_NVDIMM_PROTOCOL_H_
#define _JEDEC_NVDIMM_PROTOCOL_H_

//
// JEDEC NvDimm Protocol GUID
//
// {70B4DE8C-3B01-4f0a-A2F3-93CEE977313E}
#define EFI_JEDEC_NVDIMM_GUID \
  { \
    0x70b4de8c, 0x3b01, 0x4f0a, 0xa2, 0xf3, 0x93, 0xce, 0xe9, 0x77, 0x31, 0x3e \
  }

typedef
EFI_STATUS
(EFIAPI *UPDATE_JEDEC_NVDIMM_ACPI_TABLE) (
  UINT64 *JedecNvDimmTablePtr
  );

typedef struct _EFI_JEDEC_NVDIMM_PROTOCOL {
  UPDATE_JEDEC_NVDIMM_ACPI_TABLE   UpdateJedecNvDimmAcpiTable;
} EFI_JEDEC_NVDIMM_PROTOCOL;

extern EFI_GUID         gEfiJedecNvDimmGuid;
extern EFI_GUID         gEfiJedecNvDimmSmmGuid;

#endif

