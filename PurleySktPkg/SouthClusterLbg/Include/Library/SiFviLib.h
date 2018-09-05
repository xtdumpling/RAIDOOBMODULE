/** @file
  Header file for Reference code Firmware Version Info Interface Lib implementation.

@copyright
 Copyright (c) 2011 - 2014 Intel Corporation. All rights reserved
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
#ifndef _SI_FVI_LIB_H_
#define _SI_FVI_LIB_H_

#include <IndustryStandard/SmBios.h>


///
/// Non-static SMBIOS table data to be filled later with a dynamically generated value
///
#define TO_BE_FILLED  0
#define TO_BE_FILLED_STRING  " "        ///< Initial value should not be NULL

///
/// String references in SMBIOS tables. This eliminates the need for pointers.
/// See the DMTF SMBIOS Specification v2.7.1, section 6.1.3.
///
#define NO_STRING_AVAILABLE  0
#define STRING_1  1
#define STRING_2  2
#define STRING_3  3
#define STRING_4  4
#define STRING_5  5
#define STRING_6  6
#define STRING_7  7

//
// Default FVI smbios type, drivers update it dynamically.
//
#define DEFAULT_FVI_SMBIOS_TYPE 0xDD

#pragma pack(1)

///
/// This structure contains the RC version details for FVI SMBIOS records.
///
typedef struct {
  UINT8   MajorVersion;
  UINT8   MinorVersion;
  UINT8   Revision;
  UINT16  BuildNum;
} RC_VERSION;

typedef struct {
  SMBIOS_STRUCTURE  SmbiosHeader;
  UINT8             Count;  ///< Number of (FVI_DATA) data blocks included
} FVI_HEADER;

///
/// The string number for ComponentName and VersionString is always calculated dynamically.
/// The initial value is ignored and should always be TO_BE_FILLED.
///
typedef struct {
  UINT8       ComponentName;
  UINT8       VersionString;
  RC_VERSION  Version;
} FVI_DATA;

#define FVI_NUMBER_OF_STRINGS  2
typedef struct {
  CHAR8       *ComponentName;
  CHAR8       *VersionString;
} FVI_STRINGS;

#define DEFAULT_FVI_HEADER_DATA(Data) \
{ \
  { \
    DEFAULT_FVI_SMBIOS_TYPE, \
    sizeof(FVI_HEADER) + sizeof (Data), \
    0, \
  }, \
  sizeof (Data) / sizeof (FVI_DATA) \
}

#define DEFAULT_FVI_DATA(Name) \
{ \
  TO_BE_FILLED, \
  TO_BE_FILLED, \
  { \
    (UINT8)   (((Name ## _RC_VERSION) & 0xFF000000) >> 24), \
    (UINT8)   (((Name ## _RC_VERSION) & 0x00FF0000) >> 16), \
    (UINT8)   (((Name ## _RC_VERSION) & 0x0000FF00) >> 8), \
    (UINT16)  (((Name ## _RC_VERSION) & 0x000000FF)), \
  } \
}

#define FVI_VERSION(Version) \
{ \
  TO_BE_FILLED, \
  TO_BE_FILLED, \
  Version \
}

#define DEFAULT_FVI_STRINGS(Name) \
{ \
  Name ## _FVI_STRING, \
  NULL \
}

#define DEFAULT_FVI_VERSION \
  { \
    0xFF, 0xFF, 0xFF, 0xFFFF \
  }
#pragma pack()

/**
  Create the Reference code version info as per Firmware Version Info (FVI) Interface Spec v0.8
  and add the SMBIOS table using the SMBIOS protocol

  Invoke this routine to add the table entry when all the Fvi data is finalized.

  - @pre
    - EFI_SMBIOS_PROTOCOL in Native mode

  @param[in] Header    The expanded header includes the standard SMBIOS table header, plus the Count
                       of the number of elements in the Data and Strings arrays.
  @param[in] *Data     Pointer to an array of Data blocks.
  @param[in] *Strings  Pointer to an array of Strings. There are FVI_NUMBER_OF_STRINGS * Count strings total.

  @retval EFI_SUCCESS          - if the data is successfully reported.
  @retval EFI_OUT_OF_RESOURCES - if not able to get resources.
  @retval EFI_UNSUPPORTED      - if required DataHub or SMBIOS protocol is not available.
**/
EFI_STATUS
AddFviEntry (
  IN FVI_HEADER  Header,
  IN FVI_DATA    *Data,
  IN FVI_STRINGS *Strings
  );

#endif
