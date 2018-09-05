//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file AmiNvmeLegacy.h
    Protocol header file the Ami Nvme Legacy interface 

**/

#ifndef __NVME_LEGACY_PROTOCOL_H__
#define __NVME_LEGACY_PROTOCOL_H__

#ifdef __cplusplus
extern "C" {
#endif


typedef struct _NVME_LEGACY_MASS_DEVICE NVME_LEGACY_MASS_DEVICE;


/**
     This API is called by NVMe bus driver. The device is added into CSM16 
     data area for legacy boot

    @param  *NvmeLegacyMassDevice

    @retval EFI_STATUS

**/

typedef EFI_STATUS
(EFIAPI *AMI_NVME_LEGACY_DEVICE_INSTALL)(
    IN OUT NVME_LEGACY_MASS_DEVICE      *AddNvmeLegacyDevice
);

typedef struct {
    AMI_NVME_LEGACY_DEVICE_INSTALL       AddNvmeLegacyDevice;
} AMI_NVME_LEGACY_PROTOCOL;


extern EFI_GUID gAmiNvmeLegacyProtocolGuid;


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
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
