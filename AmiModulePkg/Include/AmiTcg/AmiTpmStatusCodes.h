//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2010, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//*************************************************************************
// $Header: $
//
// $Revision: $
//
// $Date:  $
//*************************************************************************
//<AMI_FHDR_START>
//
// Name:	AmiTpmStatusCodes.h
//
// Description:	Header file for AmiTpmStatusCodes
//
//<AMI_FHDR_END>
//*************************************************************************
#ifndef _AMI_TPM_STATUS_CODES_H_
#define _AMI_TPM_STATUS_CODES_H_

#include <Pi/PiStatusCode.h>

///////////////////////////////////////////////////////////////////////////////
// AMI Tpm Progress Codes
///////////////////////////////////////////////////////////////////////////////
//Reported by TCG 
#define AMI_SPECIFIC_TPM_PROGRESS_CODE_BASE EFI_SOFTWARE_UNSPECIFIED | EFI_OEM_SPECIFIC | AMI_TPM_PROGRESS_CODE_BASE

#define AMI_SPECIFIC_TPM_1_2_STARTUP_CMD_SENT      (AMI_SPECIFIC_TPM_PROGRESS_CODE_BASE | 0x00000001)
#define AMI_SPECIFIC_TPM_2_0_STARTUP_CMD_SENT      (AMI_SPECIFIC_TPM_PROGRESS_CODE_BASE | 0x00000002)
#define AMI_SPECIFIC_STARTUP_CMD_SKIP              (AMI_SPECIFIC_TPM_PROGRESS_CODE_BASE | 0x00000003)
#define AMI_SPECIFIC_TPM_1_2_SELFTEST_CMD_SENT     (AMI_SPECIFIC_TPM_PROGRESS_CODE_BASE | 0x00000004)
#define AMI_SPECIFIC_TPM_2_0_SELFTEST_CMD_SENT     (AMI_SPECIFIC_TPM_PROGRESS_CODE_BASE | 0x00000005)
#define AMI_SPECIFIC_TPM_1_2_PP_LOCK_CMD_SENT      (AMI_SPECIFIC_TPM_PROGRESS_CODE_BASE | 0x00000006)
#define AMI_SPECIFIC_TPM_1_2_DEVICE_DISCOVERED     (AMI_SPECIFIC_TPM_PROGRESS_CODE_BASE | 0x00000007)
#define AMI_SPECIFIC_TPM_2_0_DISCRETE_DEVICE_DISCOVERED     (AMI_SPECIFIC_TPM_PROGRESS_CODE_BASE | 0x00000008)
#define AMI_SPECIFIC_TPM_2_0_FTPM_DEVICE_DISCOVERED         (AMI_SPECIFIC_TPM_PROGRESS_CODE_BASE | 0x00000009)
#define AMI_SPECIFIC_TPM_1_2_DEVICE_ENABLED                 (AMI_SPECIFIC_TPM_PROGRESS_CODE_BASE | 0x0000000A)
#define AMI_SPECIFIC_TPM_1_2_DEVICE_ACTIVATED               (AMI_SPECIFIC_TPM_PROGRESS_CODE_BASE | 0x0000000B)
#define AMI_SPECIFIC_TPM_1_2_DEVICE_DISABLED                (AMI_SPECIFIC_TPM_PROGRESS_CODE_BASE | 0x0000000C)
#define AMI_SPECIFIC_TPM_1_2_DEVICE_DEACTIVATED             (AMI_SPECIFIC_TPM_PROGRESS_CODE_BASE | 0x0000000D)
#define AMI_SPECIFIC_TPM_DEVICE_CLEARED                     (AMI_SPECIFIC_TPM_PROGRESS_CODE_BASE | 0x0000000E)
#define AMI_SPECIFIC_BIOS_FWVOL_MEASURED                    (AMI_SPECIFIC_TPM_PROGRESS_CODE_BASE | 0x0000000F)
#define AMI_SPECIFIC_TPM_SHA_1_ACTIVE                       (AMI_SPECIFIC_TPM_PROGRESS_CODE_BASE | 0x00000010)
#define AMI_SPECIFIC_TPM_SHA_256_ACTIVE                       (AMI_SPECIFIC_TPM_PROGRESS_CODE_BASE | 0x00000011)
#define AMI_SPECIFIC_TPM_SHA_384_ACTIVE                       (AMI_SPECIFIC_TPM_PROGRESS_CODE_BASE | 0x00000012)
#define AMI_SPECIFIC_TPM_SHA_512_ACTIVE                       (AMI_SPECIFIC_TPM_PROGRESS_CODE_BASE | 0x00000013)
#define AMI_SPECIFIC_TPM_SHA_SM3_ACTIVE                       (AMI_SPECIFIC_TPM_PROGRESS_CODE_BASE | 0x00000014)
#define AMI_SPECIFIC_SECURE_BOOT_VARIABLES_MEASURED           (AMI_SPECIFIC_TPM_PROGRESS_CODE_BASE | 0x00000015)
#define AMI_SPECIFIC_SECURE_BOOT_CERTIFICATE_MEASURED         (AMI_SPECIFIC_TPM_PROGRESS_CODE_BASE | 0x00000016)
#define AMI_SPECIFIC_CRTM_VERSION_MEASURED                    (AMI_SPECIFIC_TPM_PROGRESS_CODE_BASE | 0x00000017)
#define AMI_SPECIFIC_UEFI_IMAGE_MEASURED                      (AMI_SPECIFIC_TPM_PROGRESS_CODE_BASE | 0x00000018)
#define AMI_SPECIFIC_UEFI_GPT_PARTITION_MEASURED              (AMI_SPECIFIC_TPM_PROGRESS_CODE_BASE | 0x00000019)
#define AMI_SPECIFIC_UEFI_SEPARATORS_MEASURED                 (AMI_SPECIFIC_TPM_PROGRESS_CODE_BASE | 0x0000001A)
#define AMI_SPECIFIC_TPM_2_0_PH_RANDOMIZED                    (AMI_SPECIFIC_TPM_PROGRESS_CODE_BASE | 0x0000001B)
#define AMI_SPECIFIC_TPM_2_0_PH_ENABLED                       (AMI_SPECIFIC_TPM_PROGRESS_CODE_BASE | 0x0000001C)
#define AMI_SPECIFIC_TPM_2_0_SH_ENABLED                       (AMI_SPECIFIC_TPM_PROGRESS_CODE_BASE | 0x0000001D)
#define AMI_SPECIFIC_TPM_2_0_EH_ENABLED                       (AMI_SPECIFIC_TPM_PROGRESS_CODE_BASE | 0x0000001E)
#define AMI_SPECIFIC_TPM_2_0_PH_DISABLED                      (AMI_SPECIFIC_TPM_PROGRESS_CODE_BASE | 0x0000001F)
#define AMI_SPECIFIC_TPM_2_0_SH_DISABLED                      (AMI_SPECIFIC_TPM_PROGRESS_CODE_BASE | 0x00000020)
#define AMI_SPECIFIC_TPM_2_0_EH_DISABLED                      (AMI_SPECIFIC_TPM_PROGRESS_CODE_BASE | 0x00000021)
#define AMI_SPECIFIC_TPM_COMM_INTERFACE_TIS                   (AMI_SPECIFIC_TPM_PROGRESS_CODE_BASE | 0x00000022)
#define AMI_SPECIFIC_TPM_COMM_INTERFACE_CRB                   (AMI_SPECIFIC_TPM_PROGRESS_CODE_BASE | 0x00000023)
#define AMI_SPECIFIC_TPM_PROTOCOL_SPEC_VERSION_1_1            (AMI_SPECIFIC_TPM_PROGRESS_CODE_BASE | 0x00000024)
#define AMI_SPECIFIC_TPM_PROTOCOL_SPEC_VERSION_1_0            (AMI_SPECIFIC_TPM_PROGRESS_CODE_BASE | 0x00000025)
#define AMI_SPECIFIC_TPM_PROTOCOL_GET_CAPABILITY_REQUEST      (AMI_SPECIFIC_TPM_PROGRESS_CODE_BASE | 0x00000026)
#define AMI_SPECIFIC_TPM_PROTOCOL_HASH_LOG_EXTEND_REQUEST      (AMI_SPECIFIC_TPM_PROGRESS_CODE_BASE | 0x00000027)
#define AMI_SPECIFIC_TPM_PROTOCOL_SET_ACTIVE_PCRS_REQUEST      (AMI_SPECIFIC_TPM_PROGRESS_CODE_BASE | 0x00000028)
#define AMI_SPECIFIC_MOR_REQUEST_ACK_EXECUTED                      (AMI_SPECIFIC_TPM_PROGRESS_CODE_BASE | 0x00000029)
#define AMI_SPECIFIC_MOR_BIT_CLEARED                               (AMI_SPECIFIC_TPM_PROGRESS_CODE_BASE | 0x0000002A)
#define AMI_SPECIFIC_TPM_FW_UPDATE_COMPLETED                       (AMI_SPECIFIC_TPM_PROGRESS_CODE_BASE | 0x0000002B)



///////////////////////////////////////////////////////////////////////////////
// AMI TPM Error Codes
///////////////////////////////////////////////////////////////////////////////
//Reported by TCG 
//Minor

#define AMI_SPECIFIC_TPM_ERROR_CODE_BASE EFI_SOFTWARE_UNSPECIFIED | EFI_OEM_SPECIFIC | AMI_TPM_PROGRESS_CODE_BASE 
#define AMI_SPECIFIC_TPM_ERR_1_2_NOT_DISCOVERED        (AMI_SPECIFIC_TPM_ERROR_CODE_BASE | 0x00000001)
#define AMI_SPECIFIC_TPM_ERR_2_0_NOT_DISCOVERED        (AMI_SPECIFIC_TPM_ERROR_CODE_BASE | 0x00000002)
#define AMI_SPECIFIC_TPM_ERR_NO_SECBOOT_VAR_SECBOOT_DISABLED        (AMI_SPECIFIC_TPM_ERROR_CODE_BASE | 0x00000003)
#define AMI_SPECIFIC_TPM_ERR_TCG_PROTOCOL_NOT_INSTALLED_TPM_2_0_DEVICE_FOUND        (AMI_SPECIFIC_TPM_ERROR_CODE_BASE | 0x00000004)
#define AMI_SPECIFIC_TPM_ERR_TCG2_PROTOCOL_NOT_INSTALLED_TPM_1_2_DEVICE_FOUND       (AMI_SPECIFIC_TPM_ERROR_CODE_BASE | 0x00000005)
#define AMI_SPECIFIC_TPM_ERR_LOCKOUT_SET               (AMI_SPECIFIC_TPM_ERROR_CODE_BASE | 0x00000006)

//Major
#define AMI_SPECIFIC_TPM_ERROR_CODE_BASE_MAJOR EFI_SOFTWARE_UNSPECIFIED | EFI_OEM_SPECIFIC | AMI_TPM_PROGRESS_CODE_BASE 
#define AMI_SPECIFIC_TPM_1_2_STARTUP_ERROR           (AMI_SPECIFIC_TPM_ERROR_CODE_BASE_MAJOR | 0x00000001)
#define AMI_SPECIFIC_TPM_2_0_STARTUP_ERROR           (AMI_SPECIFIC_TPM_ERROR_CODE_BASE_MAJOR | 0x00000002)
#define AMI_SPECIFIC_TPM_1_2_PP_LOCK_ERROR           (AMI_SPECIFIC_TPM_ERROR_CODE_BASE_MAJOR | 0x00000003)
#define AMI_SPECIFIC_TPM_ERR_NO_TPM_DEVICE           (AMI_SPECIFIC_TPM_ERROR_CODE_BASE_MAJOR | 0x00000004)
#define AMI_SPECIFIC_TPM_ERR_SHA_1_ACTIVE_FAIL           (AMI_SPECIFIC_TPM_ERROR_CODE_BASE_MAJOR | 0x00000005)
#define AMI_SPECIFIC_TPM_ERR_SHA_256_ACTIVE_FAIL           (AMI_SPECIFIC_TPM_ERROR_CODE_BASE_MAJOR | 0x00000006)
#define AMI_SPECIFIC_TPM_ERR_SHA_384_ACTIVE_FAIL           (AMI_SPECIFIC_TPM_ERROR_CODE_BASE_MAJOR | 0x00000007)
#define AMI_SPECIFIC_TPM_ERR_SHA_512_ACTIVE_FAIL           (AMI_SPECIFIC_TPM_ERROR_CODE_BASE_MAJOR | 0x00000008)
#define AMI_SPECIFIC_TPM_ERR_SHA_SM3_ACTIVE_FAIL           (AMI_SPECIFIC_TPM_ERROR_CODE_BASE_MAJOR | 0x00000009)
#define AMI_SPECIFIC_TPM_ERR_SECBOOT_VAR_NOT_MEASURED      (AMI_SPECIFIC_TPM_ERROR_CODE_BASE_MAJOR | 0x0000000A)
#define AMI_SPECIFIC_TPM_ERR_PH_ENABLE_FAIL                (AMI_SPECIFIC_TPM_ERROR_CODE_BASE_MAJOR | 0x0000000B) 
#define AMI_SPECIFIC_TPM_ERR_PH_RANDOMIZE_FAIL             (AMI_SPECIFIC_TPM_ERROR_CODE_BASE_MAJOR | 0x0000000C) 
#define AMI_SPECIFIC_TPM_ERR_SH_ENABLE_FAIL                (AMI_SPECIFIC_TPM_ERROR_CODE_BASE_MAJOR | 0x0000000D)
#define AMI_SPECIFIC_TPM_ERR_EH_ENABLE_FAIL                (AMI_SPECIFIC_TPM_ERROR_CODE_BASE_MAJOR | 0x0000000E)
#define AMI_SPECIFIC_TPM_ERR_PH_DISABLE_FAIL                (AMI_SPECIFIC_TPM_ERROR_CODE_BASE_MAJOR | 0x0000000F) 
#define AMI_SPECIFIC_TPM_ERR_SH_DISABLE_FAIL                (AMI_SPECIFIC_TPM_ERROR_CODE_BASE_MAJOR | 0x000000010)
#define AMI_SPECIFIC_TPM_ERR_EH_DISABLE_FAIL                (AMI_SPECIFIC_TPM_ERROR_CODE_BASE_MAJOR | 0x000000011)
#define AMI_SPECIFIC_TPM_ERR_TCG_PROTOCOL_GET_CAPABILITY_FAIL     (AMI_SPECIFIC_TPM_ERROR_CODE_BASE_MAJOR | 0x000000012)
#define AMI_SPECIFIC_TPM_ERR_TCG_PROTOCOL_HASH_LOG_EXTEND_EVENT_FAIL     (AMI_SPECIFIC_TPM_ERROR_CODE_BASE_MAJOR | 0x000000013)
#define AMI_SPECIFIC_TPM_ERR_TPM_1_2_ENABLE_FAIL                  (AMI_SPECIFIC_TPM_ERROR_CODE_BASE_MAJOR | 0x000000014)
#define AMI_SPECIFIC_TPM_ERR_TPM_1_2_ACTIVATE_FAIL                (AMI_SPECIFIC_TPM_ERROR_CODE_BASE_MAJOR | 0x000000015)
#define AMI_SPECIFIC_TPM_ERR_TPM_1_2_DISABLE_FAIL                 (AMI_SPECIFIC_TPM_ERROR_CODE_BASE_MAJOR | 0x000000016)
#define AMI_SPECIFIC_TPM_ERR_TPM_1_2_DEACTIVATE_FAIL              (AMI_SPECIFIC_TPM_ERROR_CODE_BASE_MAJOR | 0x000000017)
#define AMI_SPECIFIC_TPM_ERR_MOR_REQUEST_FAIL                     (AMI_SPECIFIC_TPM_ERROR_CODE_BASE_MAJOR | 0x000000018)
#define AMI_SPECIFIC_TPM_ERR_COMMUNICATION_FAIL                   (AMI_SPECIFIC_TPM_ERROR_CODE_BASE_MAJOR | 0x000000019)
#define AMI_SPECIFIC_TPM_ERR_FWUPDATE_FAIL                        (AMI_SPECIFIC_TPM_ERROR_CODE_BASE_MAJOR | 0x00000001A)
#define AMI_SPECIFIC_TPM_ERR_SELFTEST_FAIL                        (AMI_SPECIFIC_TPM_ERROR_CODE_BASE_MAJOR | 0x00000001B)
#endif

EFI_STATUS 
EFIAPI
TpmPeiReportStatusCode(  IN EFI_STATUS_CODE_TYPE   Type,
                         IN EFI_STATUS_CODE_VALUE  Value);

EFI_STATUS 
EFIAPI
TpmDxeReportStatusCode(  IN EFI_STATUS_CODE_TYPE   Type,
                         IN EFI_STATUS_CODE_VALUE  Value);

EFI_STATUS 
EFIAPI
TpmDxeReportStatusCodeEx( IN EFI_STATUS_CODE_TYPE   Type,
        IN EFI_STATUS_CODE_VALUE  Value,
        IN UINT32                 Instance,
        IN CONST EFI_GUID         *CallerId          OPTIONAL,
        IN CONST EFI_GUID         *ExtendedDataGuid  OPTIONAL,
        IN CONST VOID             *ExtendedData      OPTIONAL,
        IN UINTN                  ExtendedDataSize);

EFI_STATUS 
EFIAPI
TpmPeiReportStatusCodeEx( IN EFI_STATUS_CODE_TYPE   Type,
        IN EFI_STATUS_CODE_VALUE  Value,
        IN UINT32                 Instance,
        IN CONST EFI_GUID         *CallerId          OPTIONAL,
        IN CONST EFI_GUID         *ExtendedDataGuid  OPTIONAL,
        IN CONST VOID             *ExtendedData      OPTIONAL,
        IN UINTN                  ExtendedDataSize);


#define TCG_FWEXTENDED_DATA_GUID \
    {0x58053681, 0x13f3, 0x47f6, 0xb1, 0x37, 0xcd, 0xb3, 0xe8, 0x88, 0xd9, 0xa4}

#define TCG_IMGEXTENDED_DATA_GUID \
    {0xa7f9d067, 0xad4b, 0x41cf, 0xa2, 0x94, 0xda, 0xa6, 0xa1, 0x49, 0x8b, 0x8d}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2010, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**     5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093            **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
