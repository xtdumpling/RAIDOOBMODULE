/** @file
  PCH policy PPI produced by a platform driver specifying Azalia Codec verb table.
  This PPI is consumed by the PCH PEI modules.

@copyright
 Copyright (c) 2013 - 2015 Intel Corporation. All rights reserved
 This software and associated documentation (if any) is furnished
 under a license and may only be used or copied in accordance
 with the terms of the license. Except as permitted by the
 license, no part of this software or documentation may be
 reproduced, stored in a retrieval system, or transmitted in any
 form or by any means without the express written consent of
 Intel Corporation.
 This file contains an 'Intel Peripheral Driver' and is uniquely
 identified as "Intel Reference Module" and is licensed for Intel
 CPUs and chipsets under the terms of your license agreement with
 Intel or your vendor. This file may be modified by the user, subject
 to additional terms of the license agreement.

@par Specification Reference:
**/
#ifndef PCH_HDA_VERB_TABLE_H_
#define PCH_HDA_VERB_TABLE_H_


/**
  Azalia verb table PPI GUID.

  To program Azalia codec PPI of this GUID must be installed by platform driver.
  PPI must consist of PCH_HDA_VERB_TABLE_HEADER followed by verb table data.
**/
extern EFI_GUID  gPchHdaVerbTablePpiGuid;


/**
  Azalia verb table PPI header
  Every verb table should contain this defined header and followed by azalia verb commands.
  Then install the verb table as PPI with GUID gPchHdaVerbTablePpiGuid.
**/
typedef struct {
  UINT16  VendorId;             ///< Codec Vendor ID
  UINT16  DeviceId;             ///< Codec Device ID
  UINT8   RevisionId;           ///< Revision ID of the codec. 0xFF matches any revision.
  UINT8   SdiNo;                ///< SDI number, 0xFF matches any SDI.
  UINT16  DataDwords;           ///< Number of data DWORDs following the header.
} PCH_HDA_VERB_TABLE_HEADER;

#ifdef _MSC_VER
//
// Disable "zero-sized array in struct/union" extension warning.
// Used for neater verb table definitions.
//
#pragma warning (push)
#pragma warning (disable: 4200)
#endif
typedef struct  {
  PCH_HDA_VERB_TABLE_HEADER  Header;
  UINT32 Data[];
} HDAUDIO_VERB_TABLE;
#ifdef _MSC_VER
#pragma warning (pop)
#endif

#endif // PCH_HDA_VERB_TABLE_H_
