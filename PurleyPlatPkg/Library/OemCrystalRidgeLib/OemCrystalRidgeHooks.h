
//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//
/**

Copyright (c)  1999 - 2015 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file OemCrystalRidgeHooks.h

--*/




// OemCrystalRidgeHooks
VOID OemBeforeUpdatePCDBIOSPartition(UINT8 skt, UINT8 ch, UINT8 dimm, UINT8 **cfgCurRecBuffer, UINT32* cfgCurRecLength);
VOID OemBeforeUpdatePCDOSPartition(	UINT8 skt,UINT8 ch, UINT8 dimm, UINT8 **cfgOutRecBuffer, UINT32* cfgOutRecLength);
EFI_STATUS OemCreateNfitTable(UINT64  *Table, UINT32 TableBufferSize);
EFI_STATUS OemCreatePcatTable(UINT64 *Table, UINT64 TableBufferSize);
VOID OemUpdateNfitTable (UINT64  *Table);
VOID OemUpdatePcatTable (UINT64  *Table);

