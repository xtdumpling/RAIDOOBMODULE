//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file LegacySredir.h
    Legacy console redirection Protocol header file
*/

#ifndef __LEGACY_SREDIR_PROTOCOL_H__
#define __LEGACY_SREDIR_PROTOCOL_H__

#ifdef __cplusplus
extern "C" {
#endif

#define AMI_LEGACY_SREDIR_PROTOCOL_GUID \
  { 0xA062CF1F, 0x8473, 0x4aa3, { 0x87, 0x93, 0x60, 0x0B, 0xC4, 0xFF, 0xA9, 0xA9 } }

#define EFI_LEGACY_SREDIR_PROTOCOL_GUID AMI_LEGACY_SREDIR_PROTOCOL_GUID

typedef struct _AMI_LEGACY_SREDIR_PROTOCOL AMI_LEGACY_SREDIR_PROTOCOL;



/**
    Enable the Legacy Serial Redirection 

    @param This Legacy Serial Redirection Protocol address

    @retval EFI_SUCCESS     Redirection Enabled 
    @retval EFI_NOT_FOUND   Redirection Not enabled 
*/
typedef
EFI_STATUS (EFIAPI *LEGACY_SREDIR_ENABLE) (
    IN  AMI_LEGACY_SREDIR_PROTOCOL  *This
);

/**
    Disable the Legacy Serial Redirection 

    @param  This    Legacy Serial Redirection Protocol address

    @retval EFI_SUCCESS     Redirection Disabled 
    @retval EFI_NOT_FOUND   Redirection Not disabled 
*/
typedef
EFI_STATUS (EFIAPI *LEGACY_SREDIR_DISABLE) (
    IN  AMI_LEGACY_SREDIR_PROTOCOL  *This
);

typedef struct _AMI_LEGACY_SREDIR_PROTOCOL {
    LEGACY_SREDIR_ENABLE    EnableLegacySredir;
    LEGACY_SREDIR_DISABLE   DisableLegacySredir;
} AMI_LEGACY_SREDIR_PROTOCOL;

#define EFI_LEGACY_SREDIR_PROTOCOL AMI_LEGACY_SREDIR_PROTOCOL

extern EFI_GUID gEfiLegacySredirProtocolGuid;

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
