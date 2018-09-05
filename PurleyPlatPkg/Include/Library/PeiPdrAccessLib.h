//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/** @file
  PeiPdrAccess Liarary Definition Header File.

  Copyright (c) 2015, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
**/

#ifndef _PDR_ACCESS_PEI_LIB_H_
#define _PDR_ACCESS_PEI_LIB_H_

#include <Library/PeiServicesLib.h>
#include <Guid/Pdr.h>


typedef enum {
 RTC_SECONDS = 0,   // R/W  Range 0..59
 RTC_MINUTES = 2,   // R/W  Range 0..59
 RTC_HOURS   = 4,   // R/W  Range 1..12 or 0..23 Bit 7 is AM/PM
 RTC_DAY     = 7,   // R/W  Range 1..31
 RTC_MONTH   = 8,   // R/W  Range 1..12
 RTC_YEAR    = 9,   // R/W  Range 0..99
} RTC_ADDRESS;

/**
  Update PlatformDataRegion information, not only for UUID. This routine reads the PDR information - 64KB size to memory buffer,
  and backup the PDR data, update memory buffer and then write back to PDR.

  @param  PeiServices      Describes the list of possible PEI Services.
  @param  Buffer           Data to update.
  @param  DestOffset       Region to update, the offset of PDR.
  @param  BufferSize       The size of data to update.
  
  @return EFI_STATUS

**/
EFI_STATUS
UpdatePlatDataArea (
  IN        CONST EFI_PEI_SERVICES          **PeiServices,
  IN        UINT8                           *Buffer,
  IN        UINT32                          DestOffset,
  IN        UINT32                          BufferSize
);

/**
  Read PDR Information.

  @param  PeiServices      Describes the list of possible PEI Services.
  @param  Pdr              A pointer to PLATFORM_PDR_TABLE, which contains PDR information.
  
  @return EFI_STATUS

**/
EFI_STATUS
PlatDataRegRead (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  OUT PLATFORM_PDR_TABLE        *Pdr
  );
  
/**
  Read UUID from PDR and update to UuidInfoHob.

  @param  PeiServices      Describes the list of possible PEI Services.
  @param  UuidBuffer       A pointer to UUID info.
  
  @return EFI_STATUS

**/
EFI_STATUS
UuidGetByPdr (
  IN  CONST EFI_PEI_SERVICES     **PeiServices,
  OUT UINT8                      *UuidBuffer
  );
  
/**
  RP require UUID value cannot be full 00's or FF's for PXE PDK tests.

  @param  PeiServices      Describes the list of possible PEI Services.
  @param  Buffer           A pointer to UUID.
  
  @return EFI_STATUS

**/
EFI_STATUS
RpUuidRefresh (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN OUT UINT8                  *SystemUuid
  );

/**
  Get variable MfgSystemUuid, which was programmed by tool in MFG Mode.

  @param  PeiServices      Describes the list of possible PEI Services.
  @param  Buffer           A pointer to variable MfgSystemUuid.
  
  @return EFI_STATUS

**/
EFI_STATUS
GetMfgSystemUuidVar (
  IN EFI_PEI_SERVICES           **PeiServices,
  OUT UINT8                     *Buffer
  );
  
/**
  Check for variable containing the Mfg UUID, if exists, update PDR with new UUID, if not found, get UUID from PDR, and then sync to PlatformInfoHob.

  @param  PeiServices          Describes the list of possible PEI Services.
  @param  Buffer               A pointer to UUID INFO.
  
  @return Status

**/
EFI_STATUS
MfgUuidGet (
  IN EFI_PEI_SERVICES           **PeiServices,
  OUT UINT8                     *Buffer
  );
  
/**

  Read RTC data register and return contents after converting it to decimal.
  Needed here because it looks like other methods of getting time (ex. gRT->GetTime) tend to fail.

  @param Address RTC Register address to read
  @retval Value of data register contents converted to Decimal

**/
UINT8
RtcReadDec (
  IN RTC_ADDRESS Address
  );

#endif //_PDR_ACCESS_PEI_LIB_H_
