//
// This file contains an 'Intel Peripheral Driver' and is        
// licensed for Intel CPUs and chipsets under the terms of your  
// license agreement with Intel or your vendor.  This file may   
// be modified by the user, subject to additional terms of the   
// license agreement                                             
//
/** @file
  BIOS ID library definitions and functions.

  This library provides functions to get BIOS ID, VERSION, DATE and TIME.

  Copyright (c) 2011 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/

#ifndef _BIOS_ID_LIB_H_
#define _BIOS_ID_LIB_H_

//
// BIOS ID string format:
//
// $(BOARD_ID)$(BOARD_REV).$(OEM_ID).$(VERSION_MAJOR).$(BUILD_TYPE)$(VERSION_MINOR).YYMMDDHHMM
//
// Example: "TRFTCRB1.86C.0008.D03.0506081529"
//
#pragma pack(1)

typedef struct {
  CHAR16    BoardId[7];             // "TRFTCRB"
  CHAR16    BoardRev;               // "1"
  CHAR16    Dot1;                   // "."
  CHAR16    OemId[3];               // "86C"
  CHAR16    Dot2;                   // "."
  CHAR16    VersionMajor[4];        // "0008"
  CHAR16    Dot3;                   // "."
  CHAR16    BuildType;              // "D"
  CHAR16    VersionMinor[2];        // "03"
  CHAR16    Dot4;                   // "."
  CHAR16    TimeStamp[10];          // "YYMMDDHHMM"
  CHAR16    NullTerminator;         // 0x0000
} BIOS_ID_STRING;

//
// A signature precedes the BIOS ID string in the FV to enable search by external tools.
//
typedef struct {
  UINT8             Signature[8];   // "$IBIOSI$"
  BIOS_ID_STRING    BiosIdString;   // "TRFTCRB1.86C.0008.D03.0506081529"
} BIOS_ID_IMAGE;

#pragma pack()

/**
  This function returns BIOS ID by searching FV.

  @param[out] BiosIdImage   The BIOS ID got from FV.

  @retval EFI_SUCCESS               BIOS ID has been got successfully.
  @retval EFI_NOT_FOUND             BIOS ID image is not found, and no parameter will be modified.
  @retval EFI_INVALID_PARAMETER     The parameter is NULL.

**/
EFI_STATUS
GetBiosId (
  OUT BIOS_ID_IMAGE     *BiosIdImage
  );

/**
  This function returns the BIOS Version & Release Date and Time by getting and converting BIOS ID.

  @param[out] BiosVersion       The Bios Version out of the conversion.
  @param[out] BiosReleaseDate   The Bios Release Date out of the conversion.
  @param[out] BiosReleaseTime   The Bios Release Time out of the conversion.

  @retval EFI_SUCCESS               BIOS Version & Release Date and Time have been got successfully.
  @retval EFI_NOT_FOUND             BIOS ID image is not found, and no parameter will be modified.
  @retval EFI_INVALID_PARAMETER     All the parameters are NULL.

**/
EFI_STATUS
GetBiosVersionDateTime (
  OUT CHAR16    *BiosVersion, OPTIONAL
  OUT CHAR16    *BiosReleaseDate, OPTIONAL
  OUT CHAR16    *BiosReleaseTime OPTIONAL
  );

#endif

