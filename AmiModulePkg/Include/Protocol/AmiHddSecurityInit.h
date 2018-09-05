//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file
AMI defined Protocol header file for Hdd Security Init Protocol
 **/
#ifndef _AMI_HDD_SECURITY_INIT_PROTOCOL_H
#define _AMI_HDD_SECURITY_INIT_PROTOCOL_H

#include <AmiStorageFeatures.h>

/**
 * HddSecurity Init Protocol Definition
*/  
#define AMI_HDD_SECURITY_INIT_PROTOCOL_GUID \
        { 0xce6f86bb, 0xb800, 0x4c71, { 0xb2, 0xd1, 0x38, 0x97, 0xa3, 0xbc, 0x1d, 0xae } }

/**
 * TcgStorageSecurity Init Protocol GUID
*/  
#define AMI_TCG_STORAGE_SECURITY_INIT_PROTOCOL_GUID \
		{ 0x734aa01d, 0x95ec, 0x45b7, { 0xa2, 0x3a, 0x2d, 0x86, 0xd8, 0xfd, 0xeb, 0xb6 } }

typedef struct _AMI_HDD_SECURITY_INIT_PROTOCOL AMI_HDD_SECURITY_INIT_PROTOCOL;

/**
    Checks whether the device supports Security mode or not.
    If yes, installs SecurityProtocol.

    @param BusInterface 	- Pointer to a bus interface 
    @param BusInterfaceType - Bus Interface type (IDE, AHCE, NVMe , etc)

    @retval 
        EFI_SUCCESS                      : Security Mode supported.
        EFI_DEVICE_ERROR/EFI_UNSUPPORTED : Not supported.

**/
typedef 
EFI_STATUS 
(EFIAPI *AMI_HDD_SECURITY_INIT_INSTALL) (
    IN VOID                         *BusInterface,
    IN AMI_STORAGE_BUS_INTERFACE    BusInterfaceType
);

/**
    Stops the security mode support by uninstalling the
    security interface.
        
    @param BusInterface 	- Pointer to a bus interface 
    @param BusInterfaceType - Bus Interface type (IDE, AHCE, NVMe , etc)

    @retval 
        EFI_SUCCESS         : Security Mode supported
        EFI_DEVICE_ERROR    : Not supported

**/
typedef 
EFI_STATUS 
(EFIAPI *AMI_HDD_SECURITY_INIT_UNINSTALL) (
    IN VOID                         *BusInterface,
    IN AMI_STORAGE_BUS_INTERFACE    BusInterfaceType
);

struct _AMI_HDD_SECURITY_INIT_PROTOCOL {
    AMI_HDD_SECURITY_INIT_INSTALL    InstallSecurityInterface;
    AMI_HDD_SECURITY_INIT_UNINSTALL  StopSecurityModeSupport;
};


extern EFI_GUID gAmiHddSecurityInitProtocolGuid;
extern EFI_GUID gAmiTcgStorageSecurityInitProtocolGuid;

#endif
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
