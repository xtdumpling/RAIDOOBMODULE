//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
//**********************************************************************

/** @file
	SecureBoot.h: Common Secure Boot module definitions
**/

#ifndef _SECURE_BOOT_MODE_H_
#define _SECURE_BOOT_MODE_H_

///
/// "SecureBootSetup" variable stores current secure boot Setup control settings.
/// The value type is SECURE_BOOT_SETUP_VAR.
///
#define AMI_SECURE_BOOT_SETUP_VAR  L"SecureBootSetup"
///
/// "SecureVarPresent" variable stores current state of secure boot secure boot variables
/// The value type is UINT8[numSecVars]
///
#define AMI_SECURE_VAR_PRESENT_VAR  L"SecureVarPresent"
///
/// "DeploymentModeNV" variable stores current secure boot deployment mode.
///
/// GUID: gAmiDeploymentModeNVGuid
/// The value type is SECURE_BOOT_MODE_TYPE.
///
#define AMI_DEPLOYMENT_MODE_VARIABLE_NAME  L"DeploymentModeNv"
///
/// GUID used to "DeploymentModeNv" variable
/// {97E8965F-C761-4f48-B6E4-9FFA9CB2A2D6}
///
#define AMI_DEPLOYMENT_MODE_VARIABLE_GUID \
    { 0x97e8965f, 0xc761, 0x4f48, 0xb6, 0xe4, 0x9f, 0xfa, 0x9c, 0xb2, 0xa2, 0xd6 }
///
///  "VendorKeysNv" variable to record the out of band secure boot keys modification.
///  This variable is a read-only NV variable that indicates whether someone other than
///  the platform vendor has used a mechanism not defined by the UEFI Specification to
///  transition the system to setup mode or to update secure boot keys.
///
///  GUID: gAmiVendorKeysNvGuid
///  The value type is UINT8
///
#define EFI_VENDOR_KEYS_NV_VARIABLE_NAME  L"VendorKeysNv"
//#define VENDOR_KEYS_VALID             1
//#define VENDOR_KEYS_MODIFIED          0
///
/// GUID used to "VendorKeysNv" variable to record the out of band secure boot keys modification.
/// This variable is a read-only NV variable that indicates whether someone other than the platform vendor has used a 
/// mechanism not defined by the UEFI Specification to transition the system to setup mode or to update secure boot keys.
/// {550E42E1-B6FA-4e99-BBD9-1A901F001D7A}
#define AMI_VENDOR_KEYS_NV_VARIABLE_GUID \
    { 0x550e42e1, 0xb6fa, 0x4e99, 0xbb, 0xd9, 0x1a, 0x90, 0x1f, 0x0, 0x1d, 0x7a }

#pragma pack(1)
typedef struct{
    UINT8 SecureBootSupport;
    UINT8 SecureBootMode;
    UINT8 DefaultKeyProvision;
    UINT8 BackDoorVendorKeyChange; // obsolete
    UINT8 Load_from_OROM;
    UINT8 Load_from_REMOVABLE_MEDIA;
    UINT8 Load_from_FIXED_MEDIA;
} SECURE_BOOT_SETUP_VAR;

typedef struct{
    UINT8 Value;
} SETUP_MODE_VAR;

typedef struct{
    UINT8 Value;
} SECURE_BOOT_VAR;

typedef struct{
    UINT8 Value;
} AUDIT_MODE_VAR;

typedef struct{
    UINT8 Value;
} DEPLOYED_MODE_VAR;

typedef struct{
    UINT8 DBX;
    UINT8 DBT;
    UINT8 DBR;
    UINT8 DB;
    UINT8 KEK;
    UINT8 PK;
} SECURE_VAR_INSTALL_VAR;

typedef struct{
    UINT8 Value;
} SECURE_BOOT_VENDOR_KEY_VAR;

#pragma pack()

// SecureBoot Control /0-Disabled/1-Enabled
#define SECURE_BOOT                  1

// SetupMode variable
#define USER_MODE                    0
#define SETUP_MODE                   1

///
/// Value definition for SetupMode/DeployedMode/AuditMode variable
///
#ifndef DEPLOYED_MODE_ENABLE
#define SETUP_MODE_ENABLE                 1
#define SETUP_MODE_DISABLE                0
#define DEPLOYED_MODE_ENABLE              1
#define DEPLOYED_MODE_DISABLE             0
#define AUDIT_MODE_ENABLE                 1
#define AUDIT_MODE_DISABLE                0
#endif

// SecureBoot Mode
#define STANDARD_SECURE_BOOT         0
#define CUSTOM_SECURE_BOOT           1

#ifndef EFI_SECURE_BOOT_NAME
#define EFI_SECURE_BOOT_NAME         L"SecureBoot"
#endif
// EFI_IMAGE_SECURITY_DATABASE_DEFAULT must be defined in GlobalVariable.h or ImageAuthentication.h
// UEFI 2.4: Install Factory defaults as Read-only volatile variables for key distribution.
#ifndef EFI_DB_DEFAULT_VARIABLE_NAME
#define EFI_PK_DEFAULT_VARIABLE_NAME  L"PKDefault"
#define EFI_KEK_DEFAULT_VARIABLE_NAME L"KEKDefault"
#define EFI_DB_DEFAULT_VARIABLE_NAME  L"dbDefault"
#define EFI_DBX_DEFAULT_VARIABLE_NAME L"dbxDefault"
#endif
#ifndef EFI_IMAGE_SECURITY_DATABASE2
#define EFI_IMAGE_SECURITY_DATABASE2 L"dbt"
#define EFI_DBT_DEFAULT_VARIABLE_NAME L"dbtDefault"
#endif
#ifndef EFI_IMAGE_SECURITY_DATABASE3
#define EFI_IMAGE_SECURITY_DATABASE3 L"dbr"
#define EFI_DBR_DEFAULT_VARIABLE_NAME L"dbrDefault"
#endif
#ifndef EFI_VENDOR_KEYS_VARIABLE_NAME
#define EFI_VENDOR_KEYS_VARIABLE_NAME L"VendorKeys"
#endif
#ifndef EFI_OS_RECOVERY_ORDER_VARIABLE_NAME
#define EFI_OS_RECOVERY_ORDER_VARIABLE_NAME L"OsRecoveryOrder"
#endif
#ifndef EFI_OS_RECOVERY_XXXX_VARIABLE_NAME
#define EFI_OS_RECOVERY_XXXX_VARIABLE_NAME L"OsRecovery"
#endif
#ifndef EFI_AUDIT_MODE_NAME
#define EFI_AUDIT_MODE_NAME L"AuditMode"
#endif
#ifndef EFI_DEPLOYED_MODE_NAME
#define EFI_DEPLOYED_MODE_NAME L"DeployedMode"
#endif
#define SIGSUPPORT_NUM 8 
#define SIGSUPPORT_LIST EFI_CERT_X509_SHA256_GUID, EFI_CERT_X509_SHA384_GUID, EFI_CERT_X509_SHA512_GUID, EFI_CERT_SHA256_GUID, EFI_CERT_X509_GUID, EFI_CERT_RSA2048_GUID, EFI_CERT_RSA2048_SHA256_GUID, EFI_CERT_RSA2048_SHA1_GUID

#endif //_SECURE_BOOT_MODE_H_
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
