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
AMI defined Protocol header file Scsi Pass Thru Init Protocol
 **/

#ifndef _AMI_SCSI_PASS_THRU_INIT_PROTOCOL_H
#define _AMI_SCSI_PASS_THRU_INIT_PROTOCOL_H

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Scsi Pass thru Init Protocol Definition
 */

#define AMI_SCSI_PASS_THRU_INIT_PROTOCOL_GUID \
        { 0x0fc50878, 0x1633, 0x432a, { 0xbd, 0xe4, 0x84, 0x13, 0x57, 0xfc, 0x15, 0xe9 } }

typedef struct _AMI_SCSI_PASS_THRU_INIT_PROTOCOL AMI_SCSI_PASS_THRU_INIT_PROTOCOL;

/**
    Installs EFI_EXT_SCSI_PASS_THRU_PROTOCOL that provides services allowing
    ATAPI commands to be sent to ATAPI Devices attached to controller.
        
    @param ControllerHandle Controller Handle where the protocol 
                            can be installed
    @param ModeFlag         Ide or Ahci
    
    @retval 
        EFI_SUCCESS or EFI_OUT_OF_RESOURCES   

**/
typedef 
EFI_STATUS 
(EFIAPI *AMI_SCSI_PASS_THRU_INSTALL) (
    IN EFI_HANDLE   ControllerHandle,
    IN BOOLEAN      ModeFlag
);

/**
    Uninstalls EFI_EXT_SCSI_PASS_THRU_PROTOCOL and Stops ATAPI Pass Thru
    Support.
        
    @param ControllerHandle Controller Handle where the protocol 
                            can be installed
    @param ModeFlag         Ide or Ahci
    @retval 
        Status

**/
typedef 
EFI_STATUS 
(EFIAPI *AMI_SCSI_PASS_THRU_UNINSTALL) (
    IN EFI_HANDLE   ControllerHandle,
    IN BOOLEAN      ModeFlag
);

struct _AMI_SCSI_PASS_THRU_INIT_PROTOCOL {
    AMI_SCSI_PASS_THRU_INSTALL    InstallScsiPassThruAtapi;
    AMI_SCSI_PASS_THRU_UNINSTALL  StopScsiPassThruAtapiSupport;
};

extern EFI_GUID gAmiScsiPassThruInitProtocolGuid;

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
