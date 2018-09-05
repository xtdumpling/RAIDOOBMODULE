/** @file
  This is an implementation of the eMCA.

  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement

  Copyright (c) 2009-2012 Intel Corporation.  All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license.  Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/



#ifndef _EMCA_H_
#define _EMCA_H_

//
// Includes
//
//
// Consumed protocols
//
#include <Uefi.h>

#define L1_DIR_PTR_LEN  8 //bytes
#define ELOG_ENTRY_LEN 0x1000
#define EMCA_L1_VERSION 0x0100
#define OS_VMM_OPT_IN_FLAG BIT0


typedef struct {
  UINT32 Version;
  UINT32 HdrLen;
  UINT64 L1DirLen;
  UINT64 ElogDirBase;
  UINT64 ElogDirLen;
  UINT32 Flags;
  UINT32 reserved0;
  UINT64 reserved1;
  UINT32 NumL1EntryLp;
  UINT32 reserved2;
  UINT64 reserved3;
} EMCA_L1_HEADER;

typedef struct {
  UINT64 McSts;
  UINT64 McAddr;
  UINT64 McMisc;
} EMCA_MC_BANK_SIGNATURE;

typedef struct {
  UINT32 ApicId;
  UINT32 Reserved0;
  EMCA_MC_BANK_SIGNATURE Signature;
} EMCA_MC_SIGNATURE_ERR_RECORD; 

#endif  //_EMCA_H_

