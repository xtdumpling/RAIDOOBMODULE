/** @file
  Header file for Firmware Status Smbios Table Interface implementation

@copyright
 Copyright (c) 2015 Intel Corporation. All rights reserved
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
#ifndef _FW_STS_SMBIOS_TABLE_H_
#define _FW_STS_SMBIOS_TABLE_H_

#include <IndustryStandard/SmBios.h>
#include <MeChipset.h>

/**
  BIOS FWSTS Structure SMBIOS Table DEF for OEM Type
  FWSTS structure is defined as SMBIOS OEM type structure (from 128(0x80) through 255 (0xFF)).
  OEM structure type is dynamic type extracted from the "Type 14 - Group Associations Structure"
  to avoid conflicts introduced by hardcode OEM type.
  The structure can have multiple instance with the same type, each table can specify the name string
  (or PFA) to indicate the FWSTS for any HECI device specified for extension consideration.
  Version 1 -
    The Type14 defined in SMBIOS Specification provides OEMs to specify the arrangement or hierarchy
    of certain components. FWSTS Structure SMBIOS Table type can be extracted from the
    Type 14 "Item Type" field if the "Group Name" is "$MEI". There shall be only one Type14 structure
    with Group Name "$MEI".

    The following is the definition for the SMBIOS table entry Type14 that will contain the
    BIOS FWSTS Structure SMBIOS Table.
    -------------------------------------------------------------------------------------------------
      Offset    Name                  Length          Description
    -------------------------------------------------------------------------------------------------
      00h       Type                  BYTE            14, Group Associations Indicator
    -------------------------------------------------------------------------------------------------
      01h       Length                BYTE            Computed as 5 + (3 bytes for each item in the group).
                                                      The user of this structure determines the number of
                                                      items as (Length -5)/3
    -------------------------------------------------------------------------------------------------
      02h       Handle                WORD            Specify the handle of the structure, a unique
                                                      16-bit number in the range 0 to 0FFFEh
                                                      (for version 2.0) or 0 to 0FEFFh
                                                      (for version 2.1 and later).
    -------------------------------------------------------------------------------------------------
      04h      Group name             BYTE            String number of the string (L "$MEI")
                                                      describing the group
    -------------------------------------------------------------------------------------------------
      05h      Item Type              BYTE            Item (Structure) Type of this member.
                                                      The default value is 0xDB, it can be overrided
                                                      by gSiPkgTokenSpaceGuid.PcdFwStsSmbiosType.
    -------------------------------------------------------------------------------------------------
      06h      Item Handle            WORD            Handle corresponding to this structure
    -------------------------------------------------------------------------------------------------


    The following is the definition for the BIOS FWSTS Structure SMBIOS Table that will contain
    the FWSTS values.
    -------------------------------------------------------------------------------------------------
      Offset    Name                  Length          Description
    -------------------------------------------------------------------------------------------------
      00h       Type                  BYTE            Specify the type of the structure Value is
                                                      defined in SMBIOS Type 14 - Group Associations
                                                      Structure
    -------------------------------------------------------------------------------------------------
      01h       Length                BYTE            Specify the length of the structure, starting
                                                      at the Type field. Length 6 + (25 * N)
    -------------------------------------------------------------------------------------------------
      02h       Handle                WORD            Specify the handle of the structure, a unique
                                                      16-bit number in the range 0 to 0FFFEh
                                                      (for version 2.0) or 0 to 0FEFFh
                                                      (for version 2.1 and later).
    -------------------------------------------------------------------------------------------------
      04h       Version               BYTE            Version of implemented structure.
                                                      Current version is 1.
    -------------------------------------------------------------------------------------------------
      05h       Count                 BYTE            Number of MEI device blocks included (n).
                                                      N =1 - One set of FWSTS registers exposed
    -------------------------------------------------------------------------------------------------
      06h + 25  Component Name        BYTE            Number of Null terminated string.
      * (n-1)                                         N = 1 - One set of FWSTS registers exposed
                                                      (L "MEI1" for 0:22:0, L"MEI2" for 0:22:1,
                                                      L"MEI3" for 0:22:4)
    -------------------------------------------------------------------------------------------------
      07h + 25  FWSTS1                DWORD           The value of 0:22:x PCI offset 0x40
      * (n-1)                                         X : 0, 1, 4
    -------------------------------------------------------------------------------------------------
      0Bh + 25  FWSTS2                DWORD           The value of 0:22:x PCI offset 0x48
      * (n-1)                                         X : 0, 1, 4
    -------------------------------------------------------------------------------------------------
      0Fh + 25  FWSTS3                DWORD           The value of 0:22:x PCI offset 0x60
      * (n-1)                                         X : 0, 1, 4
    -------------------------------------------------------------------------------------------------
      13h + 25  FWSTS4                DWORD           The value of 0:22:x PCI offset 0x64
      * (n-1)                                         X : 0, 1, 4
    -------------------------------------------------------------------------------------------------
      17h + 25  FWSTS5                DWORD           The value of 0:22:x PCI offset 0x68
      * (n-1)                                         X : 0, 1, 4
    -------------------------------------------------------------------------------------------------
      18h + 25  FWSTS6                DWORD           The value of 0:22:x PCI offset 0x6C
      * (n-1)                                         X : 0, 1, 4
    -------------------------------------------------------------------------------------------------

**/
#define MAX_FWSTS_REG             6
#define T14_FWSTS_STRING          "$MEI"
#define MEI1_FWSTS_STRING         "MEI1"
#define MEI2_FWSTS_STRING         "MEI2"
#define MEI3_FWSTS_STRING         "MEI3"

#pragma pack(1)
typedef struct  {
  HECI_DEVICE   HeciDev;
  CHAR8         *String;
} MEI_DEV_STRING;

typedef struct  {
  /**
  Number of Null terminated string.
  N = 1 - One set of FWSTS registers exposed
       (L"MEI1" for 0:22:0, L"MEI2" for 0:22:1, L"MEI3" for 0:22:4)
  **/
  UINT8         ComponentName;
  /**
  The value of 0:22:x PCI offset 0x40
  The value of 0:22:X PCI offset 0x48
  The value of 0:22:X PCI offset 0x60
  The value of 0:22:X PCI offset 0x64
  The value of 0:22:X PCI offset 0x68
  The value of 0:22:X PCI offset 0x6C
  **/
  UINT32        Reg[MAX_FWSTS_REG];
} FWSTS_SET_RECORD;

typedef struct {
  SMBIOS_STRUCTURE  SmbiosHeader;
  UINT8             Version;                  ///< Version of implemented structure, current version is 1
  UINT8             Count;                    ///< Number of MEI device groups included
  FWSTS_SET_RECORD  Group[1];
} FWSTS_SMBIOS_RECORD;

#pragma pack()

#endif
