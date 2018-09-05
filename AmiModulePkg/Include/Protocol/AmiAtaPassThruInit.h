//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
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
AMI defined Protocol header file for Ata Pass thru Init protocol
 **/

#ifndef _AMI_ATA_PASS_THRU_INIT_PROTOCOL_H
#define _AMI_ATA_PASS_THRU_INIT_PROTOCOL_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Ata Pass Thru Init Protocol definition
 */

#define AMI_ATA_PASS_THRU_INIT_PROTOCOL_GUID \
        { 0xc6734411, 0x2dda, 0x4632, { 0xa5, 0x92, 0x92, 0x0f, 0x24, 0xd6, 0xed, 0x21 } }

typedef struct _AMI_ATA_PASS_THRU_INIT_PROTOCOL AMI_ATA_PASS_THRU_INIT_PROTOCOL;

/**
    Installs Ata Pass Thru APIs and provides services allowing
    ATA commands to be sent to ATA Devices attached to the controller
                 
    @param ControllerHandle Controller Handle where the protocol 
                            can be installed
    @param ModeFlag         Ide or Ahci

    @retval EFI_SUCCESS or EFI_OUT_OF_RESOURCES 

**/
typedef 
EFI_STATUS 
(EFIAPI *AMI_ATA_PASS_THRU_INSTALL) (
    IN EFI_HANDLE   ControllerHandle,
    IN BOOLEAN      ModeFlag
);

/**
    Uninstall the Ata Pass Thru protocol if installed on the handle.
    
    @param ControllerHandle Controller Handle where the protocol 
                            can be installed
    @param ModeFlag         Ide or Ahci

    @retval 

**/
typedef 
EFI_STATUS 
(EFIAPI *AMI_ATA_PASS_THRU_UNINSTALL) (
    IN EFI_HANDLE   ControllerHandle,
    IN BOOLEAN      ModeFlag
);

struct _AMI_ATA_PASS_THRU_INIT_PROTOCOL {
    AMI_ATA_PASS_THRU_INSTALL    InstallAtaPassThru;
    AMI_ATA_PASS_THRU_UNINSTALL  StopAtaPassThruSupport;
};

extern EFI_GUID gAmiAtaPassThruInitProtocolGuid;

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
//**         5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
