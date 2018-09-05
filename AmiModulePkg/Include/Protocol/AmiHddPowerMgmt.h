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
AMI defined Protocol header file Hdd Power Management Protocol
 **/

#ifndef _AMI_HDD_POWER_MGMT_PROTOCOL_H
#define _AMI_HDD_POWER_MGMT_PROTOCOL_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Hdd Power Management Protocol Definition 
 */

#define AMI_HDD_POWER_MGMT_PROTOCOL_GUID \
        { 0x67BC3883, 0x7E79, 0x4BC1, { 0xA3, 0x3E, 0x3A, 0xF7, 0xD1, 0x75, 0x89, 0xBA } }

typedef struct _AMI_HDD_POWER_MGMT_PROTOCOL AMI_HDD_POWER_MGMT_PROTOCOL;

/**
    Returns the Current State of the device

    @param IdePowerMgmtInterface Ami Hdd Power Management Protocol Interface
    @param PowerMode 

    @retval 
        EFI_STATUS, Current state of the device.

    @note  PowerMode : 00 Standby Mode, 80h : Idle Mode, FF : Active/Idle Mode


**/
typedef 
EFI_STATUS  
(EFIAPI *AMI_HDD_POWER_MGMT_CHECK_POWER_MODE)(
    IN AMI_HDD_POWER_MGMT_PROTOCOL      *This,
    IN OUT UINT8                        *PowerMode
);

/**
    Puts the device in Idle Mode

    @param IdePowerMgmtInterface    Ami Hdd Power Management Protocol Interface 
    @param StandbyTimeout 
  
    @retval 
        EFI_STATUS


**/
typedef 
EFI_STATUS  
(EFIAPI *AMI_HDD_POWER_MGMT_IDLE_MODE)(
    IN AMI_HDD_POWER_MGMT_PROTOCOL      *This,
    IN UINT8                            StandbyTimeout
);

/**
    Puts the device in StandbyMode

    @param IdePowerMgmtInterface    Ami Hdd Power Management Protocol Interface  
    @param StandbyTimeout           Value in Minutes. (0 : Disabled) 
  
    @retval 
        EFI_STATUS

**/
typedef 
EFI_STATUS  
(EFIAPI *AMI_HDD_POWER_MGMT_STANDBY_MODE)(
    IN AMI_HDD_POWER_MGMT_PROTOCOL      *This,
    IN UINT8                            StandbyTimeout
);

/**
    Enables Advance Power Management

    @param IdePowerMgmtInterface    Ami Hdd Power Management Protocol Interface 
    @param AdvPowerMgmtLevel 
            AdvPowerMgmtLevel ( 0 : Disable, FEh : Max. Performance, 81h - FDh Intermediate performance without Standby
            80h : Min. Power Consumption without Standby, 02h - 7Fh Intermediate Performance with Standby
            01h : Min. Power Consumption with Standby)

    @retval 
        EFI_STATUS

**/
typedef 
EFI_STATUS  
(EFIAPI *AMI_HDD_POWER_MGMT_ADVANCE_POWER_MGMT_MODE)(
    IN AMI_HDD_POWER_MGMT_PROTOCOL      *This,
    IN UINT8                            AdvPowerMgmtLevel
);

struct _AMI_HDD_POWER_MGMT_PROTOCOL {
    AMI_HDD_POWER_MGMT_CHECK_POWER_MODE                CheckPowerMode;
    AMI_HDD_POWER_MGMT_IDLE_MODE                       IdleMode;
    AMI_HDD_POWER_MGMT_STANDBY_MODE                    StandbyMode;
    AMI_HDD_POWER_MGMT_ADVANCE_POWER_MGMT_MODE         AdvancePowerMgmtMode;
};


extern EFI_GUID gAmiHddPowerMgmtProtocolGuid;

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
