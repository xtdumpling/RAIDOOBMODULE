//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/** @file
  Definition for a struture sharing information for CPU hot plug.

  Copyright (c) 2011 - 2014, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/
#ifndef _CPU_HOT_PLUG_DATA_H_
#define _CPU_HOT_PLUG_DATA_H_

#define  CPU_HOT_PLUG_DATA_REVISION_1      0x00000001

typedef struct {
  UINT32    Revision;          // Used for version identification for this structure
  UINT32    ArrayLength;       // The entries number of the following ApicId array and SmBase array
  //
  // Data required for SMBASE relocation
  //
  UINT64    *ApicId;           // Pointer to ApicId array
  UINTN     *SmBase;           // Pointer to SmBase array
  UINT32    IEDBase;
  UINT32    SmrrBase;
  UINT32    SmrrSize;
} CPU_HOT_PLUG_DATA;

#endif

