/**
  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement
**/
/**

Copyright (c) 2015 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

@file:

  IeHob.h

@brief:

  This file is SampleCode for Intel IE Hob.

**/

#ifndef IE_HOB_H
#define IE_HOB_H

typedef struct {
  EFI_HOB_GUID_TYPE     EfiHobGuidType;
  INT8                  IeDisabledInSoftStraps;
  INT8                  DfxIe;
} IE_HOB;

#define IE_HOB_GUID \
  { \
    0x081077fa, 0x577d, 0x43b8, 0xa8, 0xac, 0x9e, 0x0d, 0x31, 0x65, 0xf4, 0xee \
  }

extern EFI_GUID gIeHobGuid;

#endif // IE_HOB_H