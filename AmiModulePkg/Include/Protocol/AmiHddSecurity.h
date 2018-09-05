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
AMI defined Protocol header file for Hdd Security Protocol
 **/

#ifndef _AMI_HDD_SECURITY_PROTOCOL_H
#define _AMI_HDD_SECURITY_PROTOCOL_H

/**
 * AMI HDD Security Protocol Definition 
*/
    
#define AMI_HDD_SECURITY_PROTOCOL_GUID \
        { 0xF4F63529, 0x281E, 0x4040, { 0xA3, 0x13, 0xC1, 0xD6, 0x76, 0x63, 0x84, 0xBE } }

#define AMI_TCG_STORAGE_SECURITY_PROTOCOL_GUID \
		{ 0xb396da3a, 0x52b2, 0x4cd6, { 0xa8, 0x9a, 0x13, 0xe7, 0xc4, 0xae, 0x97, 0x90 } }

typedef struct _AMI_HDD_SECURITY_PROTOCOL AMI_HDD_SECURITY_PROTOCOL;

typedef UINT16 AMI_HDD_SECURITY_FLAGS;
// BIT0 : Security Supported
// BIT1 : Security Enabled
// BIT2 : Security Locked
// BIT3 : Security Frozen
// BIT4 : Security Count Expired
// BIT5 : Enhanced Security Erase supported
// BIT6 : Enhanced Security Erase supported
// BIT8 : Security Level 0:High, 1:Maximum

typedef UINT32 AMI_HDD_PASSWORD_FLAGS;
// BIT0  User Password Support
// BIT1  Master Password Support
// BIT2  Display User Password
// BIT3  Display Master Password
// BIT11 OEM Default Master Password
// BIT16 MasterPassword Set Status.
// BIT17 UserPassword Set Status.

/**
    Returns whether the Device supports Security Mode or not.
    If supported, the current status of the device is returned.
    Security status information will be retrieved from buffer
    updated by IDENTIFY DEVICE(Command Code : 0xEC) command.
        
    @param This     Ami Hdd Security Protocol
    @param SecurityStatus 

    @retval 
        EFI_SUCCESS                     : Security Mode supported
        EFI_UNSUPPORTED                 : Not supported

        UINT16                          *SecurityStatus
        Bit 0 : Security Supported
        Bit 1 : Security Enabled
        Bit 2 : Security Locked
        Bit 3 : Security Frozen
        Bit 4 : Security Count Expired
        Bit 5 : Enhanced Security Erase supported
        Bit 6 : Enhanced Security Erase supported
        Bit 8 : Security Level 0:High, 1:Maximum

**/
typedef EFI_STATUS (EFIAPI *AMI_HDD_RETURN_SECURITY_STATUS) (
    IN AMI_HDD_SECURITY_PROTOCOL    *This,
    AMI_HDD_SECURITY_FLAGS          *SecurityStatus
);

/**
    Sets User/Master Password using SECURITY_SET_PASSWORD
    (Command Code : 0xF1) security command.

    @param This     Ami Hdd Security Protocol
    @param Control  Bit 0 : 0/1 User/Master Password
                    Bit 8 : 0/1 High/Maximum Security level
    @param Buffer (32 Bytes)
    @param RevisionCode (Valid when Control Bit 0 = 1 of Master Password)

    @retval 
        EFI_SUCCESS     : Password set.
        EFI_ERROR       : Error in setting Password.

    @note  
  1. Buffer should be 32 bytes long (Mandatory). Even if Password is not 32
     bytes long,pad it with zeros to create a buffer of 32 bytes.
  2. If a MasterPassword with a valid Revision code(0x0001 through 0xFFFE)
     already exists, a new MasterPassword with a different revision code
     cannot be installed.MasterPassword needs to be unlocked, disabled and
     then a new password can be installed.

**/
typedef 
EFI_STATUS 
(EFIAPI *AMI_HDD_SECURITY_SET_PASSWORD) (
    IN AMI_HDD_SECURITY_PROTOCOL    *This,
    UINT16                          Control,
    UINT8                           *Buffer,
    UINT16                          RevisionCode
);

/**
    Unlocks the devices for media access using
    SECURITY_UNLOCK(Command Code : 0xF2) security command.

    @param This     Ami Hdd Security Protocol
    @param Control  Bit 0 : 0/1 User/Master Password
    @param Buffer   (32 Bytes)    Pointer to password buffer which will be 
                                used to unlock the drive.

    @retval 
        EFI_SUCCESS         : Hard disk is unlocked.
        EFI_DEVICE_ERROR    : Failed to unlock the drive.

    @note  
  1. Buffer should be 32 bytes long (Mandatory). Even if Password is not
     32 bytes long, pad it with zeros to create a buffer of 32 bytes.
  2. If the password compare fails then the device returns command aborted
     to the host and decrements the unlock counter. This counter is initially
     set to five and is decremented for each password mismatch when SECURITY
     UNLOCK is issued and the device is locked. When this counter reaches
     zero then SECURITY UNLOCK and SECURITY ERASE UNIT commands are command
     aborted until a power-on reset or a hardware reset.SECURITY UNLOCK
     commands issued when the device is unlocked have no effect on the unlock
     counter.
  3. SECURITY_DISABLE_PASSWORD and SECURITY_SET_PASSWORD commands used when
     security password is set and HDD is in unlocked state. This is done to
     have consistency of output from this function.
**/
typedef 
EFI_STATUS 
(EFIAPI *AMI_HDD_SECURITY_UNLOCK_PASSWORD) (
    IN AMI_HDD_SECURITY_PROTOCOL    *This,
    UINT16                          Control,
    UINT8                           *Buffer
);

/**
    Disables Password authentication before media access.
    SECURITY_DISABLE_PASSWORD(Command Code : 0xF6) security
    command is used to remove the user password.

    @param This     Ami Hdd Security Protocol
    @param Control  Bit 0 : 0/1 User/Master Password
    @param Buffer   (32 Bytes) Pointer to password buffer which will be 
            used to when sending SECURITY_DISABLE_PASSWORD command.

    @retval 
        EFI_SUCCESS      : Security mode feature disabled.
        EFI_DEVICE_ERROR : Security command failed.

    @note  
  1. Buffer should be 32 bytes long (Mandatory). Even if Password is not
     32 bytes long,pad it with zeros to create a buffer of 32 bytes.
  2. Password should be unlocked before disabling it.
  3. SECURITY_DISABLE_PASSWORD command does not change the Master password
     that may be reactivated later by setting a User password.
  4. Normal erase mode is selected, the SECURITY ERASE UNIT command writes
     binary zeroes to all user data areas.

**/
typedef
EFI_STATUS 
(EFIAPI *AMI_HDD_SECURITY_DISABLE_PASSWORD) (
    IN AMI_HDD_SECURITY_PROTOCOL    *This,
    UINT16                          Control,
    UINT8                           *Buffer
);

/**
    Prevents any further alteration of security status on the
    device leaving the hard disk in Frozen mode. Frozen mode is
    disabled by power-off or hardware reset. SECURITY_FREEZE_LOCK
    (Command Code : 0xF5)security command is used to freeze hard disk.

    @param This     Ami Hdd Security Protocol

    @retval 
        EFI_SUCCESS : Hard disk is successdully kept in FROZEN state.
        EFI_ERROR   : Security Freeze command failed.

    @note  
  Commands disabled by SECURITY FREEZE LOCK are:
      - SECURITY SET PASSWORD
      - SECURITY UNLOCK
      - SECURITY DISABLE PASSWORD
      - SECURITY ERASE PREPARE
      - SECURITY ERASE UNIT
**/
typedef 
EFI_STATUS 
(EFIAPI *AMI_HDD_SECURITY_FREEZE_LOCK) (
    IN AMI_HDD_SECURITY_PROTOCOL    *This
);

/**
    Erases the Device content.SECURITY_ERASE_UNIT
    (Command code : 0xF4) security command is used for device
    erasing.
        
    @param This     Ami Hdd Security Protocol
    @param Control  Bit 0 : 0/1 User/Master Password
    @param Buffer   Pointer to password buffer which will be used 
                    to when sending SECURITY_ERASE_UNIT command

    @retval 
        EFI_SUCCESS      : Successfull completion of Erase command.
        EFI_DEVICE_ERROR : EraseUnit command failed to complete.

    @note  
  If a device is locked with USER PASSWORD and Maximum security level set
  then only way to unlock the device with MASTER password is to erase the
  contents of the device.
**/
typedef 
EFI_STATUS 
(EFIAPI *AMI_HDD_SECURITY_ERASE_UNIT) (
    IN AMI_HDD_SECURITY_PROTOCOL    *This,
    UINT16                          Control,
    UINT8                           *Buffer
);

/**
    Returns Ide Password Variables and Flags.
        
    @param This     Ami Hdd Security Protocol
    @param IdePasswordFlags 

    @retval 
        EFI_SUCCESS : IDEPassword Status Flag is returned in IdePasswordFlags.
        EFI_ERROR   : Error while retrieving the SecurityStatus.


  IdePasswordFlags:
        Only Bit0, Bit03, Bit11, Bit16, Bit17 are Supported Currently

         BIT0                             User Password Support;
         BIT1                             Master Password Support;
         BIT2                             Display User Password;
         BIT3                             Display Master Password;
         BIT4                             Display User Password Status;
         BIT5                             Display Master Password Status;
         BIT6                             Freeze Lock Hdds;
         BIT7                             Single User Password;
         BIT8                             Single Master Password;
         BIT9                             System User Password as Hdd User Password;
         BIT10                            System Admin Password as Hdd Master Password;
         BIT11                            OEM Default Master Password
         BIT12                            Prompt for User Password in Post
         BIT13                            Prompt for Master Password in Post
         BIT14                            Display Security Status in Setup
         BIT15                            DUMMY Security Token.
         BIT16                            MasterPassword Set Status.
         BIT17                            UserPassword Set Status.

**/
typedef 
EFI_STATUS 
(EFIAPI *AMI_HDD_RETURN_PASSWORD_FLAGS) (
    IN AMI_HDD_SECURITY_PROTOCOL    *This,
    AMI_HDD_PASSWORD_FLAGS                  *IdePasswordFlags
);

struct _AMI_HDD_SECURITY_PROTOCOL{ 
    AMI_HDD_RETURN_SECURITY_STATUS          ReturnSecurityStatus;
    AMI_HDD_SECURITY_SET_PASSWORD           SecuritySetPassword;
    AMI_HDD_SECURITY_UNLOCK_PASSWORD        SecurityUnlockPassword;
    AMI_HDD_SECURITY_DISABLE_PASSWORD       SecurityDisablePassword;
    AMI_HDD_SECURITY_FREEZE_LOCK            SecurityFreezeLock;
    AMI_HDD_SECURITY_ERASE_UNIT             SecurityEraseUnit;
    AMI_HDD_RETURN_PASSWORD_FLAGS           ReturnIdePasswordFlags;
};

/**
 * Common Guids used by Both IDEBUS and AHCIBUS driver for Hdd Security
 */

#define AMI_HDD_SECURITY_END_PROTOCOL_GUID \
        { 0xad77ae29, 0x4c20, 0x4fdd, { 0x85, 0x04, 0x81, 0x76, 0x61, 0x9b, 0x67, 0x6a } }

#define AMI_ONBOARD_RAID_CONTROLLER_GUID \
        { 0x5d206dd3, 0x516a, 0x47dc, { 0xa1, 0xbc, 0x6d, 0xa2, 0x04, 0xaa, 0xbe, 0x08 } }

#define AMI_HDD_PASSWORD_VERIFIED_GUID \
        { 0x1fd29be6, 0x70d0, 0x42a4, { 0xa6, 0xe7, 0xe5, 0xd1, 0x0e, 0x6a, 0xc3, 0x76 } }

extern EFI_GUID gAmiHddSecurityProtocolGuid;
extern EFI_GUID gAmiHddSecurityEndProtocolGuid;
extern EFI_GUID gAmiOnboardRaidControllerGuid;
extern EFI_GUID gAmiHddPasswordVerifiedGuid;
extern EFI_GUID gAmiTcgStorageSecurityProtocolGuid;

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
