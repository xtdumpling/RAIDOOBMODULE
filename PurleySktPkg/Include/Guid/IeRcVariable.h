//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//
/**

Copyright (c) 2014 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file IERcVariable.h

  Data format for Universal Data Structure

**/
#ifndef _IE_RC_VARIABLE_H_
#define _IE_RC_VARIABLE_H_

#if IE_SUPPORT

extern EFI_GUID gEfiIeRcVariableGuid;
#define IE_RC_CONFIGURATION_NAME L"IeRcConfiguration"


#pragma pack(1)
typedef struct {
  UINT16  SubsystemVendorId;
  UINT16  SubsystemId;
  UINT8   IeHeci1Enabled;
  UINT8   IeHeci2Enabled;
  UINT8   IeHeci3Enabled;
  UINT8   IeIderEnabled;
  UINT8   IeKtEnabled;
  UINT8   IeDisabledInSoftStraps;
  UINT8   DfxIe;

} IE_RC_CONFIGURATION;
#pragma pack()

#endif //IE_SUPPORT

#endif // _IE_RC_VARIABLE_H_


