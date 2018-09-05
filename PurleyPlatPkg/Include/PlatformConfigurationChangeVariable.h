/** @file
  Platform configuration change variable definitions

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2015 - 2017 Intel Corporation.

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

  This file contains a 'Sample Driver' and is licensed as such under the terms
  of your license agreement with Intel or your vendor. This file may be modified
  by the user, subject to the additional terms of the license agreement.

@par Specification
**/

#ifndef __PLATFORM_CONFIGURATION_CHANGE_VARIABLE_H__
#define __PLATFORM_CONFIGURATION_CHANGE_VARIABLE_H__

#define EFI_TREE_PHYSICAL_PRESENCE_DATA_GUID \
  { \
    0xe3cacf62, 0x3062, 0x4e1d, { 0x97, 0x8e, 0x46, 0x80, 0x7a, 0xb9, 0x74, 0x7d }\
  }

#define PLATFORM_CONFIGURATION_CHANGE  L"PlatformConfigurationChange"

#define NO_CONFIG_CHANGE     0x00000000
#define FIRST_BOOT           0x00000001
#define SETUP_OPTION_CHANGE  0x00000002

typedef struct {
  UINT32   ConfigChangeType;  /// Significant change from previous boot
} CONFIGURATION_CHANGE;

extern EFI_GUID  gPlatformConfigChangeGuid;

#endif

