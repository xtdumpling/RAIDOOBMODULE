//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/*++

Copyright (c)  2015 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  Dbg2.h

Abstract:

--*/

#ifndef _DBG2_H_
#define _DBG2_H_

//
// Statements that include other files
//
#include <IndustryStandard/Acpi51.h>
#include <IndustryStandard/DebugPort2Table.h>

#pragma pack(1)
typedef 
struct {
  EFI_ACPI_DEBUG_PORT_2_DESCRIPTION_TABLE       Table;
  EFI_ACPI_DBG2_DEBUG_DEVICE_INFORMATION_STRUCT Entry;
  EFI_ACPI_5_1_GENERIC_ADDRESS_STRUCTURE        BaseAdressRegister;
  UINT32                                        AddressSize;
  CHAR8                                         NamespaceString[2];

} DBG2_DEBUG_TABLE;
#pragma pack()


#endif  //_DBG2_H_
