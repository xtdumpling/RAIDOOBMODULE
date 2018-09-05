/** @file
  This file contains various definitions for IHV HSTI implementation 
  including error string definitions

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

#ifndef __HSTI_FEATURE_BIT_H__
#define __HSTI_FEATURE_BIT_H__

#define HSTI_IMPLEMENTATION_ID_PURLEY  L"Intel(R) 9-Series v1"

#define HSTI_SECURITY_FEATURE_SIZE        3

#define HSTI_ERROR                                                                    L"Error "
#define HSTI_PLATFORM_SECURITY_SPECIFICATION                                          L" Platform Security Specification"

#define HSTI_FIRMWARE_TRUSTED_CONTINUATION_CRYPTO_STRENGTH                            L" - Firmware Trust Continuation Crypto Strength - "
#define HSTI_NO_TEST_KEY_VERIFICATION                                                 L" - No Test Key Verification - "
#define HSTI_FIRMWARE_VERSION_ROLLBACK_PROTECTION                                     L" - Firmware Version Rollback Protection - "
#define HSTI_SECUREBOOT_BYPASS_CHECKING                                               L" - SecureBoot Bypass Checking - "
#define HSTI_EXTERNAL_DEVICE_DMA_PROTECTION                                           L" - External Device DMA Protection - "
#define HSTI_MOR_SUPPORT                                                              L" - MOR Support - "

#define HSTI_BYTE2_FIRMWARE_TRUSTED_CONTINUATION_CRYPTO_STRENGTH                      BIT0
#define      HSTI_BYTE2_FIRMWARE_TRUSTED_CONTINUATION_CRYPTO_STRENGTH_ERROR_CODE_1    L"0x00100001" 
#define      HSTI_BYTE2_FIRMWARE_TRUSTED_CONTINUATION_CRYPTO_STRENGTH_ERROR_STRING_1  L"SHA1\r\n"

#define HSTI_BYTE2_NO_TEST_KEY_VERIFICATION                                           BIT1
#define      HSTI_BYTE2_NO_TEST_KEY_VERIFICATION_ERROR_CODE_1                         L"0x00110001" 
#define      HSTI_BYTE2_NO_TEST_KEY_VERIFICATION_ERROR_STRING_1                       L"fail\r\n"

#define HSTI_BYTE2_FIRMWARE_VERSION_ROLLBACK_PROTECTION                               BIT2
#define      HSTI_BYTE2_FIRMWARE_VERSION_ROLLBACK_PROTECTION_ERROR_CODE_1             L"0x00120001"
#define      HSTI_BYTE2_FIRMWARE_VERSION_ROLLBACK_PROTECTION_ERROR_STRING_1           L"fail\r\n"

#define HSTI_BYTE2_SECUREBOOT_BYPASS_CHECKING                                         BIT3
#define      HSTI_BYTE2_SECUREBOOT_BYPASS_CHECKING_ERROR_CODE_1                       L"0x00130001"
#define      HSTI_BYTE2_SECUREBOOT_BYPASS_CHECKING_ERROR_STRING_1                     L"fail\r\n"


#define HSTI_BYTE2_EXTERNAL_DEVICE_DMA_PROTECTION                                     BIT4
#define      HSTI_BYTE2_EXTERNAL_DEVICE_DMA_PROTECTION_ERROR_CODE_1                   L"0x00140001"
#define      HSTI_BYTE2_EXTERNAL_DEVICE_DMA_PROTECTION_ERROR_STRING_1                 L"unsupported\r\n"


#define HSTI_BYTE2_MOR_SUPPORT                                                        BIT5
#define      HSTI_BYTE2_MOR_SUPPORT_ERROR_CODE_1                                      L"0x00150001"
#define      HSTI_BYTE2_MOR_SUPPORT_ERROR_STRING_1                                    L"unsupported\r\n"


#endif
