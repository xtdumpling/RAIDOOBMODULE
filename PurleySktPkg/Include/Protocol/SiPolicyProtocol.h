/** @file
  Protocol used for specifying platform related Silicon information and policy setting.

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2015 - 2016 Intel Corporation.

  The source code contained or described herein and all documents related to the
  source code ("Material") are owned by Intel Corporation or its suppliers or
  licensors. Title to the Material remains with Intel Corporation or its suppliers
  and licensors. The Material may contain trade secrets and proprietary and
  confidential information of Intel Corporation and its suppliers and licensors,
  and is protected by worldwide copyright and trade secret laws and treaty
  provisions. No part of the Material may be used, copied, reproduced, modified,
  published, uploaded, posted, transmitted, distributed, or disclosed in any way
  without Intel's prior express written permission.

  No license under any patent, copyright, trade secret or other intellectual
  property right is granted to or conferred upon you by disclosure or delivery
  of the Materials, either expressly, by implication, inducement, estoppel or
  otherwise. Any license under such intellectual property rights must be
  express and approved by Intel in writing.

  Unless otherwise agreed by Intel in writing, you may not remove or alter
  this notice or any other notice embedded in Materials by Intel or
  Intel's suppliers or licensors in any way.

  This file contains an 'Intel Peripheral Driver' and is uniquely identified as
  "Intel Reference Module" and is licensed for Intel CPUs and chipsets under
  the terms of your license agreement with Intel or your vendor. This file may
  be modified by the user, subject to additional terms of the license agreement.

@par Specification
**/
#ifndef _SI_POLICY_PROTOCOL_H_
#define _SI_POLICY_PROTOCOL_H_

#include <IndustryStandard/Hsti.h>

//
// DXE_SI_POLICY_PROTOCOL revisions
//
#define DXE_SI_POLICY_PROTOCOL_REVISION 2

extern EFI_GUID gDxeSiPolicyProtocolGuid;

#pragma pack (push,1)

/**
  The protocol allows the platform code to publish a set of configuration information that the
  Silicon drivers will use to configure the processor in the DXE phase.
  This Policy Protocol needs to be initialized for Silicon configuration.
  @note The Protocol has to be published before processor DXE drivers are dispatched.
**/
typedef struct {
  /**
  This member specifies the revision of the Si Policy protocol. This field is used to indicate backward
  compatible changes to the protocol. Any such changes to this protocol will result in an update in the revision number.

  <b>Revision 1</b>:
   - Initial version
  <b>Revision 2</b>:
   - Added SmbiosOemTypeFirmwareVersionInfo to determines the SMBIOS OEM type
  **/
  UINT8                          Revision;
  UINT8                          ReservedByte[7];  ///< Reserved bytes, align to multiple 8.
  /**
    This member describes a pointer to Hsti results from previous boot. In order to mitigate the large performance cost
    of performing all of the platform security tests on each boot, we can save the results across boots and retrieve
    and point this policy to them prior to the launch of HstiSiliconDxe. Logic should be implemented to not populate this
    upon major platform changes (i.e changes to setup option or platform hw)to ensure that results accurately reflect the
    configuration of the platform.
  **/
  ADAPTER_INFO_PLATFORM_SECURITY *Hsti;    ///< This is a pointer to Hsti results from previous boot
  UINTN                          HstiSize; ///< Size of results, if setting Hsti policy to point to previous results
} DXE_SI_POLICY_PROTOCOL;
 
#pragma pack (pop)

#endif
