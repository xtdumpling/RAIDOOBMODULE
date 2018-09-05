/** 
//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//

Copyright (c)  1999 - 2012 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file Bdat.h

  This file describes the contents of the BDAT ACPI.  

**/

#ifndef _BDAT_H_
#define _BDAT_H_

//
// Statements that include other files
//
// APTIOV_SERVER_OVERRIDE_RC_START : Included to avoid EFI_ACPI_TABLE_VERSION_ALL macro inclusion for RC files
#include <AmiProtocol.h>
// APTIOV_SERVER_OVERRIDE_RC_END : Included to avoid EFI_ACPI_TABLE_VERSION_ALL macro inclusion for RC files
#include <IndustryStandard/Acpi.h>

//
// Ensure proper structure formats
//
#pragma pack(1)

#define EFI_BDAT_TABLE_SIGNATURE          SIGNATURE_32('B','D','A','T')

//
// BIOS Data ACPI structure
//
typedef struct {

  EFI_ACPI_DESCRIPTION_HEADER             Header;
  EFI_ACPI_3_0_GENERIC_ADDRESS_STRUCTURE  BdatGas;

} EFI_BDAT_ACPI_DESCRIPTION_TABLE;

//
// BIOS Data Parameter Region Generic Address
// Information
//
#define EFI_BDAT_ACPI_POINTER             0x0

#define ___INTERNAL_CONVERT_TO_STRING___(a) #a
#define CONVERT_TO_STRING(a) ___INTERNAL_CONVERT_TO_STRING___(a)

//
// This is copied from Include\Acpi.h
//
#define CREATOR_ID_INTEL 0x4C544E49  //"LTNI""INTL"(Intel)
#define CREATOR_REV_INTEL 0x20090903

// APTIOV_SERVER_OVERRIDE_RC_START : Included to avoid EFI_ACPI_TABLE_VERSION_ALL macro inclusion for RC files
#ifndef EFI_ACPI_TABLE_VERSION_ALL
#define EFI_ACPI_TABLE_VERSION_ALL  (EFI_ACPI_TABLE_VERSION_1_0B|EFI_ACPI_TABLE_VERSION_2_0|EFI_ACPI_TABLE_VERSION_3_0)
#endif
// APTIOV_SERVER_OVERRIDE_RC_END : Included to avoid EFI_ACPI_TABLE_VERSION_ALL macro inclusion for RC files
#pragma pack()

#endif
