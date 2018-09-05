/**
  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement
**/
/**

Copyright (c) 2015 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

@file:

  IePlatformPolicyPei.h

@brief:

  IE Platform Policy for IE PEIMs

**/
#ifndef _PEI_IE_PLATFORM_POLICY_PPI_H_
#define _PEI_IE_PLATFORM_POLICY_PPI_H_

///
/// Extern the GUID for PPI users.
///
extern EFI_GUID gPeiIePlatformPolicyPpiGuid;

///
/// PPI definition
///
typedef struct _PEI_IE_PLATFORM_POLICY_PPI {
  UINT16  SubsystemVendorId;
  UINT16  SubsystemId;
  UINT8   IeHeci1Enabled;
  UINT8   IeHeci2Enabled;
  UINT8   IeHeci3Enabled;
  UINT8   IeIderEnabled;
  UINT8   IeKtEnabled;
  UINT8   IeDisabledInSoftStraps;
  UINT8   DfxIe;

} PEI_IE_PLATFORM_POLICY_PPI;

#endif
