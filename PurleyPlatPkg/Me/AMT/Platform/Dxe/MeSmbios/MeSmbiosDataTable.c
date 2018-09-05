/**@file

@copyright
 Copyright (c) 2011 - 2016 Intel Corporation. All rights reserved
 This software and associated documentation (if any) is furnished
 under a license and may only be used or copied in accordance
 with the terms of the license. Except as permitted by the
 license, no part of this software or documentation may be
 reproduced, stored in a retrieval system, or transmitted in any
 form or by any means without the express written consent of
 Intel Corporation.
 This file contains a 'Sample Driver' and is licensed as such
 under the terms of your license agreement with Intel or your
 vendor. This file may be modified by the user, subject to
 the additional terms of the license agreement.

@par Specification Reference:
**/

#include "MeSmbios.h"

//
// External definitions referenced by Data Table entries.
//
MISC_SUBCLASS_TABLE_EXTERNS (
  EFI_MISC_OEM_TYPE_0x82,
  MiscOemType0x82
  );
MISC_SUBCLASS_TABLE_EXTERNS (
  EFI_MISC_OEM_TYPE_0x83,
  MiscOemType0x83
  );

//
// Data Table
//
GLOBAL_REMOVE_IF_UNREFERENCED EFI_MISC_SUBCLASS_DATA_TABLE  mMiscSubclassDataTable[] = {
  MISC_SUBCLASS_TABLE_ENTRY_DATA_AND_FUNCTION(EFI_MISC_OEM_TYPE_0x82, MiscOemType0x82, MiscOemType0x82),
  MISC_SUBCLASS_TABLE_ENTRY_DATA_AND_FUNCTION(EFI_MISC_OEM_TYPE_0x83, MiscOemType0x83, MiscOemType0x83)
};

//
// Number of Data Table entries.
//
GLOBAL_REMOVE_IF_UNREFERENCED UINTN                         mSmbiosMiscDataTableEntries = sizeof (mMiscSubclassDataTable) /
sizeof (EFI_MISC_SUBCLASS_DATA_TABLE);
