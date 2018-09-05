//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file CsmVideoPolicy.h

 CSM Video Policy Protocol Declaration

**********************************************************************/

#ifndef __AMI_CSM_VIDEO_POLICY_PROTOCOL__H__
#define __AMI_CSM_VIDEO_POLICY_PROTOCOL__H__
#ifdef __cplusplus
extern "C" {
#endif

#define AMI_CSM_VIDEO_POLICY_PROTOCOL_GUID\
    { 0x3a4e4376, 0x4871, 0x4b0e, { 0xa0, 0x2f, 0xed, 0x36, 0xf2, 0xae, 0xcd, 0x0 } }

typedef struct _AMI_CSM_VIDEO_POLICY_PROTOCOL AMI_CSM_VIDEO_POLICY_PROTOCOL;

/**
Returns current CSM Video Driver policy.

@param[in] This         An instance of the CSM Video Policy Protocol
@param[out] IsTextMode  TRUE  - text mode policy
                        FALSE  - graphical module policy
@retval EFI_SUCCESS  IsTextMode has been updated based on the current CSM Video Driver policy.
@retval Error Value  CSM Video Driver policy can not be retrieved.
**/
typedef EFI_STATUS (EFIAPI *AMI_CSM_VIDEO_POLICY_GET_MODE)(
    IN AMI_CSM_VIDEO_POLICY_PROTOCOL *This, IN BOOLEAN *IsTextMode
);

/**
Returns current CSM Video Driver policy.

@param[in] This              An instance of the CSM Video Policy Protocol
@param[in] TextMode          TRUE  - set text mode policy
                             FALSE  - set graphical module policy
@param[in] ForceModeChange   TRUE  - Reconnect the driver if it has already been connected
                             FALSE  - Do no reconnect the driver.
@retval EFI_SUCCESS          CSM Video Driver policy has been successfully set.
@retval Error Value          CSM Video Driver policy was not set.
**/
typedef EFI_STATUS (EFIAPI *AMI_CSM_VIDEO_POLICY_SET_MODE)(
    IN AMI_CSM_VIDEO_POLICY_PROTOCOL *This,
    IN BOOLEAN TextMode, IN BOOLEAN ForceModeChange
);

struct _AMI_CSM_VIDEO_POLICY_PROTOCOL{
    AMI_CSM_VIDEO_POLICY_GET_MODE GetMode;
    AMI_CSM_VIDEO_POLICY_SET_MODE SetMode;
};

extern EFI_GUID gAmiCsmVideoPolicyProtocolGuid;

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
