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
AMI defined Protocol header file Hdd Host Protected Area Protocol
 **/

#ifndef _AMI_HDD_HPA_PROTOCOL_H
#define _AMI_HDD_HPA_PROTOCOL_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * HDD Host Protected Protocol definition
 */

#define AMI_HDD_HPA_PROTOCOL_GUID\
        { 0x51AA65FC, 0x82B6, 0x49E6, { 0x95, 0xE2, 0xE6, 0x82, 0x7A, 0x8D, 0x7D, 0xB4 } }

typedef struct _AMI_HDD_HPA_PROTOCOL AMI_HDD_HPA_PROTOCOL;


/**
    Returns the Native Max LBA address supported by the device

    @param This     Ami Hdd Host protected Area Protocol Interface
    @param *LBA     LBA Address

    @retval LBA : Returns Native MAX LBA address.
            EFI_STATUS  : EFI_SUCCESS/EFI_DEVICE_ERROR

**/
typedef 
EFI_STATUS 
(EFIAPI *AMI_HDD_HPA_GET_NATIVE_MAX_ADDRESS)(
    IN  AMI_HDD_HPA_PROTOCOL        *This,
    OUT UINT64                      *LBA
);

/**
    Set the Max LBA address to Ata Device

    @param This     Ami Hdd Host protected Area Protocol Interface 
    &param LBA
    @param Volatile 

    @retval 
        EFI_STATUS  :

    @note  if Volatile = TRUE, Values will not be preserved across resets.

**/
typedef 
EFI_STATUS 
(EFIAPI *AMI_HDD_HPA_SET_MAX_ADDRESS)(
    IN  AMI_HDD_HPA_PROTOCOL        *This,
    OUT UINT64                      LBA,
    IN  BOOLEAN                     Volatile
);

/**
    Returns the valid last LBA # when HPA is disabled.

    @param This     Ami Hdd Host protected Area Protocol Interface
    @param *LBA

    @retval 
        EFI_STATUS  : EFI_SUCCESS

    @note  The LBA returned will be the maximum valid LBA number an OS can make use of.

**/
typedef 
EFI_STATUS 
(EFIAPI *AMI_HDD_HPA_DISABLED_LAST_LBA)(
    IN  AMI_HDD_HPA_PROTOCOL        *This,
    OUT UINT64                      *LBA
);

/**
    Sets the Password

    @param This     Ami Hdd Host protected Area Protocol Interface 
    @param PasswordBuffer (32 Bytes of Password)

    @retval 
        EFI_STATUS  : EFI_UNSUPPORTED/EFI_SUCCESS/EFI_DEVICE_ERROR

    @note  READ NATIVE MAX ADDRESS should not immediately be PRECEDED by this command.
          To take care of this situation, a Dummy Identify Device Command will be given before
          issuing a SET MAX PASSWORD command
          Once the device is locked, it should be unlocked with the password before
          issuing any SETMAXADDRESS.
          After issuing this command device will be in UNLOCKED state.

**/
typedef 
EFI_STATUS 
(EFIAPI *AMI_HDD_HPA_SET_MAX_PASSWORD)(
    IN  AMI_HDD_HPA_PROTOCOL        *This,
    IN  UINT8                       *PasswordBuffer
);

/**
    Locks the device from accepting commands except UNLOCK and FREEZELOCK

    @param This     Ami Hdd Host protected Area Protocol Interface 

    @retval 
        EFI_STATUS  : EFI_UNSUPPORTED/EFI_SUCCESS/EFI_DEVICE_ERROR

    @note  READ NATIVE MAX ADDRESS should not immediately be PRECEDED by this command.
          To take care of this situation, a Dummy Identify Device Command will be given before
          issuing a SET MAX LOCK/UNLOCK command.

**/
typedef 
EFI_STATUS 
(EFIAPI *AMI_HDD_HPA_SET_MAX_LOCK)(
    IN  AMI_HDD_HPA_PROTOCOL           *This
);

/**
    Unlocks the device and allows it to accept all SET MAX commands

    @param This     Ami Hdd Host protected Area Protocol Interface 
    @param PasswordBuffer 

    @retval 
        EFI_STATUS  : EFI_UNSUPPORTED/EFI_SUCCESS/EFI_DEVICE_ERROR

    @note  READ NATIVE MAX ADDRESS should not immediately be PRECEDED by this command.
          To take care of this situation, a Dummy Identify Device Command will be given before
          issuing a SET MAX LOCK/UNLOCK command.

**/
typedef 
EFI_STATUS 
(EFIAPI *AMI_HDD_HPA_SET_MAX_UNLOCK)(
    IN  AMI_HDD_HPA_PROTOCOL        *This,
    IN  UINT8                       *PasswordBuffer
);

/**
    Locks all SET MAX ADDRESS/PASSWORD/LOCK/UNLOCK command until
    next power cycle. GetNativeMaxAddress is allowed.

    @param This     Ami Hdd Host protected Area Protocol Interface 

    @retval 
        EFI_STATUS  : EFI_UNSUPPORTED/EFI_SUCCESS/EFI_DEVICE_ERROR

    @note  READ NATIVE MAX ADDRESS should not immediately be PRECEDED by this command.
          To take care of this situation, a Dummy Identify Device Command will be given before
          issuing a SET MAX FREEZELOCK command.

**/
typedef 
EFI_STATUS 
(EFIAPI *AMI_HDD_HPA_SET_MAX_FREEZE_LOCK)(
    IN  AMI_HDD_HPA_PROTOCOL           *This
);

struct  _AMI_HDD_HPA_PROTOCOL {

    AMI_HDD_HPA_GET_NATIVE_MAX_ADDRESS          GetNativeMaxAddress;
    AMI_HDD_HPA_SET_MAX_ADDRESS                 SetMaxAddress;
    AMI_HDD_HPA_DISABLED_LAST_LBA           HpaDisabledLastLBA;
    AMI_HDD_HPA_SET_MAX_PASSWORD                SetMaxPassword;
    AMI_HDD_HPA_SET_MAX_LOCK                    SetMaxLock;
    AMI_HDD_HPA_SET_MAX_UNLOCK                  SetMaxUnLock;
    AMI_HDD_HPA_SET_MAX_FREEZE_LOCK             SetMaxFreezeLock;

};

extern EFI_GUID gAmiHddHpaProtocolGuid;

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
