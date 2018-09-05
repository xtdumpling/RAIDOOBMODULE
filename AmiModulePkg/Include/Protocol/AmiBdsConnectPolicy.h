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

/** @file
  BDS Connect Policy Protocol

  This protocol can be used to get or set platform specific device connection policy
**/

#ifndef __AMI_BDS_CONNECT_POLICY_PROTOCOL__H__
#define __AMI_BDS_CONNECT_POLICY_PROTOCOL__H__

#include <Protocol/BootManagerPolicy.h>

#define AMI_BDS_CONNECT_POLICY_PROTOCOL_GUID\
    { 0x38d65ec3, 0x8f39, 0x4660, { 0xb8, 0xa6, 0xf3, 0x6a, 0xa3, 0x92, 0x54, 0x75 } }

#define AMI_BDS_CONNECT_POLICY_PROTOCOL_REVISION 0x00010000

typedef struct _AMI_BDS_CONNECT_POLICY_PROTOCOL AMI_BDS_CONNECT_POLICY_PROTOCOL;

typedef enum{
    AmiBdsConnectPolicyEnable,  // Connect drivers to a device
    AmiBdsConnectPolicyDisable, // If driver comes from PCI OpROM, do not launch the driver, otherwise do not connect drivers to a device. 
    AmiBdsConnectPolicyMax
} AMI_BDS_CONNECT_POLICY;

/**
  Gets platform connect policy for a device path.

  The GetDevicePathPolicy() function allows the caller to get connect policy for a DevicePath.

  @param[in]  This       A pointer to the AMI_BDS_CONNECT_POLICY_SET_DEVICE_PATH_POLICY instance.
  @param[in]  DevicePath Points to the start of the EFI device path to connect.
  @param[out] Policy     Current DevicePath connect policy

  @retval EFI_SUCCESS            The policy has been successfully retrieved.
  @retval EFI_INVALID_PARAMETER  The device path is invalid
  @retval EFI_INVALID_PARAMETER  The Policy is NULL
**/
typedef EFI_STATUS (EFIAPI *AMI_BDS_CONNECT_POLICY_GET_DEVICE_PATH_POLICY)(
    IN AMI_BDS_CONNECT_POLICY_PROTOCOL *This, IN CONST EFI_DEVICE_PATH_PROTOCOL *DevicePath,
    OUT AMI_BDS_CONNECT_POLICY *Policy
);

/**
  Sets platform connect policy for a device path.

  The SetDevicePathPolicy() function allows the caller to set connect policy for a DevicePath.

  @param[in] This       A pointer to the AMI_BDS_CONNECT_POLICY_SET_DEVICE_PATH_POLICY instance.
  @param[in] DevicePath Points to the start of the EFI device path to connect.
  @param[in] Policy     Connect policy to apply to DevicePath

  @retval EFI_SUCCESS            The policy has been successfully applied.
  @retval EFI_INVALID_PARAMETER  The device path is invalid
**/
typedef EFI_STATUS (EFIAPI *AMI_BDS_CONNECT_POLICY_SET_DEVICE_PATH_POLICY)(
    IN AMI_BDS_CONNECT_POLICY_PROTOCOL *This, IN CONST EFI_DEVICE_PATH_PROTOCOL *DevicePath,
    IN AMI_BDS_CONNECT_POLICY Policy
);


/**
  Sets platform connect policy for a class of devices.

  The SetDeviceClassPolicy() function allows the caller to set connect policy for a class of devices defined by GUID.
  The function accepts all device class GUID defined by BootManagerPolicy protocol.
  
  @param[in] This       A pointer to the EFI_BOOT_MANAGER_POLICY_PROTOCOL instance.
  @param[in] Class      A pointer to an EFI_GUID that represents a class of devices to set policy for.
  @param[in] Policy     Connect policy to apply to all devices that belong to a class specified by Class GUID.

  @retval EFI_SUCCESS      The policy has been successfully applied.
  @retval EFI_UNSUPPORTED  The Class is not supported.
**/        
typedef EFI_STATUS (EFIAPI *AMI_BDS_CONNECT_POLICY_SET_DEVICE_CLASS_POLICY)(
    IN AMI_BDS_CONNECT_POLICY_PROTOCOL *This, IN EFI_GUID *Class,
    IN AMI_BDS_CONNECT_POLICY Policy
);

struct _AMI_BDS_CONNECT_POLICY_PROTOCOL {
  UINT64 Revision;
  AMI_BDS_CONNECT_POLICY_GET_DEVICE_PATH_POLICY  GetDevicePathPolicy;
  AMI_BDS_CONNECT_POLICY_SET_DEVICE_PATH_POLICY  SetDevicePathPolicy;
  AMI_BDS_CONNECT_POLICY_SET_DEVICE_CLASS_POLICY SetDeviceClassPolicy;
};

extern EFI_GUID gAmiBdsConnectPolicyProtocolGuid;
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
