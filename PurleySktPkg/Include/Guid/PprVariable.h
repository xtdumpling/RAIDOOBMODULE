//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
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

    PprVariable.h

Abstract:

    PPR Variable include file

Revision History

--*/

#ifndef __PPR_VARIABLE_H__
#define __PPR_VARIABLE_H__

extern EFI_GUID gEfiPprVariableGuid;

#define PPR_VARIABLE_NAME L"PprAddress"

//
// PPR max entries per CH
//
#define MAX_PPR_ADDR_ENTRIES           20


typedef struct {
  UINT8     pprAddrStatus;
  UINT8     socket;
  UINT8     mc;
  UINT8     ch;
  UINT8     dimm;
  UINT8     rank;
  UINT8     subRank;
  UINT32    dramMask;
  UINT8     bank;
  UINT32    row;
} PPR_ADDR_SETUP;

typedef struct {
  UINT16          crc16;
  PPR_ADDR_SETUP  pprAddrSetup[MAX_PPR_ADDR_ENTRIES];
} PPR_ADDR_VARIABLE;

#endif // #ifndef __PPR_VARIABLE_H__
