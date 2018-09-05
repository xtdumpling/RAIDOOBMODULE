//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file IPMISelfTestLogProtocol.h
    IPMI Self test log protocol definitions

**/

#ifndef _IPMI_SELFTEST_H_
#define _IPMI_SELFTEST_H_

//----------------------------------------------------------------------

#include <Include/IpmiNetFnAppDefinitions.h>

//----------------------------------------------------------------------

//----------------------------------------------------------------------

typedef struct _EFI_BMC_SELF_TEST_LOG_PROTOCOL EFI_BMC_SELF_TEST_LOG_PROTOCOL;

#define EFI_BMC_SELF_TEST_LOG_PROTOCOL_GUID \
  { \
    0x9fe6a332, 0x1a1f, 0x43e3, { 0xbd, 0x25, 0xf7, 0x89, 0x1c, 0x6f, 0xc7, 0x4a } \
  }

//
// Computing Unit Firmware Processor Subclass Error Code definitions.
//
#define EFI_CU_FP_EC_HARD_FAIL                        (EFI_SUBCLASS_SPECIFIC | 0x00000000)
#define EFI_CU_FP_EC_SOFT_FAIL                        (EFI_SUBCLASS_SPECIFIC | 0x00000001)
#define EFI_CU_FP_EC_COMM_ERROR                       (EFI_SUBCLASS_SPECIFIC | 0x00000002)
#define EFI_CU_FP_EC_SEL_NOT_ACCESSIBLE               (EFI_SUBCLASS_SPECIFIC | 0x00000003)
#define EFI_CU_FP_EC_SDR_REPOSITORY_NOT_ACCESSABLE    (EFI_SUBCLASS_SPECIFIC | 0x00000004)
#define EFI_CU_FP_EC_BMC_FRU_NOT_ACCESSIBLE           (EFI_SUBCLASS_SPECIFIC | 0x00000005)
#define EFI_CU_FP_EC_IPMB_NOT_RESPONDING              (EFI_SUBCLASS_SPECIFIC | 0x00000006)
#define EFI_CU_FP_EC_SDR_EMPTY                        (EFI_SUBCLASS_SPECIFIC | 0x00000007)
#define EFI_CU_FP_EC_BMC_FRU_CORRUPTED                (EFI_SUBCLASS_SPECIFIC | 0x00000008)
#define EFI_CU_FP_EC_BB_CORRUPTED                     (EFI_SUBCLASS_SPECIFIC | 0x00000009)
#define EFI_CU_FP_EC_FIRMWARE_CORRUPTED               (EFI_SUBCLASS_SPECIFIC | 0x0000000a)
#define EFI_CU_FP_EC_FORCE_UPDATE_MODE                (EFI_SUBCLASS_SPECIFIC | 0x0000000b)  // APTIO 3.0 Features into 3.5 Changes

#define BST_LOG_SIZE     20

/**
   BMC Self-test protocol
*/
typedef struct _EFI_BMC_SELF_TEST_LOG_PROTOCOL {
  UINT32                         BmcSelfTestErrorCount;
  UINT32                         BmcSelfTestError[BST_LOG_SIZE + 1];
  BOOLEAN                        IsGetDeviceIdCommandPassed;
  IPMI_GET_DEVICE_ID_RESPONSE    GetDeviceIdResponse;
} EFI_BMC_SELF_TEST_LOG_PROTOCOL;

//
//Guid Defined to install BmcSelftTestLog protocol
// which is used in BmcSelfTest log page to display BMC self test results
//
extern EFI_GUID gEfiBmcSelfTestLogProtocolGuid;

//----------------------------------------------------------------------

#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
