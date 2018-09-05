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

/** @file NvmeStaticSetup.h
    Contains structure declaration.
     
**/


#ifndef _NVME_STATIC_SETUP_H_
#define _NVME_STATIC_SETUP_H_
#ifdef __cplusplus
extern "C" {
#endif


#define NVME_DEVICE_MANAGER_GUID \
{0xc9456c5d, 0x6ca5, 0x4b5b, 0xb1, 0xb2, 0xc7, 0x5c, 0x90, 0x5b, 0xb9, 0xf}

#pragma pack(1)

typedef struct
{
    UINT8 ShowNvmeDrive[4];
    UINT8 DeviceCount;
} NVME_DEVICE_MANAGER;

#pragma pack()
/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif

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
