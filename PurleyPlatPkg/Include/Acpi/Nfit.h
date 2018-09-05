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


  @file Nfit.h

--*/

//
// The version of NvDimm F/W Interface Table (Nfit) supported has been updated to ASWG-ECR-Draft4-RC6_93.doc
// This file contains definitions for Acpi side for the Nfit creation and publication of Nfit tables.
// A BIG NOTE: When ever the NVDIMM_FW_INTERFACE_TABLE defined either here or what is in Nfit.h of CrystalRidge Protocol driver
// changes, the same changes need to relect in both the files.

#ifndef _NFIT_DEFS_H_
#define _NFIT_DEFS_H_

#include <IndustryStandard/Acpi60.h>

#define NVDIMM_FW_INTERFACE_TABLE_SIGNATURE EFI_ACPI_6_0_NVDIMM_FIRMWARE_INTERFACE_TABLE_STRUCTURE_SIGNATURE
#define NVDIMM_FW_INTERFACE_TABLE_REVISION  EFI_ACPI_6_0_NVDIMM_FIRMWARE_INTERFACE_TABLE_REVISION

#define MAX_NFIT_SIZE                       0x18000


//
// NVDIMM Firmware Interface Table struct
// IMPORTANT NOTE: Num of various sub-table fields of the Nfit has been eliminated in the latest Nfit Spec (0.8k)
// And also since there is NO memory allocation scheme for Nfit (all globals), Adding a buffer NfitTables with the,
// max nfit size currently set to 4k. Will need to watch if we will ever overrun this buffer. It is expected that
// Nfit parser will need to parse the NfitTables field to understand the various other Nfit Tables in this buffer
// based on the Type and Length field of these tables.
//
typedef struct _NVDIMM_FW_INT_TBL {
  UINT32       Signature;                    // 'NFIT' should be the signature for this table
  UINT32       Length;                       // Length in bytes fir the entire table
  UINT8        Revision;                     // Revision # of this table, initial is '1'
  UINT8        Checksum;                     // Entire Table Checksum must sum to 0
  UINT8        OemID[6];                     // OemID
  UINT8        OemTblID[8];                  // Should be Manufacturer's Model #
  UINT32       OemRevision;                  // Oem Revision of for Supplied OEM Table ID
  UINT32       CreatorID;                    // Vendor ID of the utility that is creating this table
  UINT32       CreatorRev;                   // Revision of utility that is creating this table
  UINT32       Reserved;
  UINT8        NfitTables[MAX_NFIT_SIZE];    // See Description above (IMPORTANT).
} NVDIMM_FW_INTERFACE_TABLE, *PNVDIMM_FW_INTERFACE_TABLE;

#pragma pack()

#endif  // _NFIT_DEFS_H_

