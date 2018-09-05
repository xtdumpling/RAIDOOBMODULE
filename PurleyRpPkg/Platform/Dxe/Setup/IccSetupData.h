/** @file

@copyright
 Copyright (c) 2014 - 2015 Intel Corporation. All rights reserved
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

#ifndef __ICC_SETUP_DATA_H__
#define __ICC_SETUP_DATA_H__

#ifndef INTEL_ICC_GUID
#define INTEL_ICC_GUID \
{0x64192dca, 0xd034, 0x49d2, 0xa6, 0xde, 0x65, 0xa8, 0x29, 0xeb, 0x4c, 0x74}
#endif // INTEL_ICC_GUID


#define ICC_SETUP_DATA_C_NAME  L"IccAdvancedSetupDataVar"

extern EFI_GUID            gIccGuid;

//
// Ensure proper structure formats
//
#pragma pack(1)

typedef struct _ICC_SETUP_DATA {
  UINT16 Frequency[MAX_UI_CLOCKS];
  UINT8  Spread[MAX_UI_CLOCKS];
  UINT8  ShowClock[MAX_UI_CLOCKS];
  UINT8  ShowFrequency[MAX_UI_CLOCKS];
  UINT8  ShowSpread[MAX_UI_CLOCKS];
  UINT8  ShowProfile;
  UINT8  Profile;
  UINT8  RegLock;
  UINT8  AllowAdvancedOptions;
} ICC_SETUP_DATA;

#pragma pack()

#endif
