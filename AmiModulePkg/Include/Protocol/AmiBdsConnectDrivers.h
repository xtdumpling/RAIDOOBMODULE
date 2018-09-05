//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
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
  @brief AMI BDS Connect Drivers Protocol definition.

  The protocol has NULL interface. It is installed by BDS before it starts connecting drivers.
**/

#ifndef __AMI_BDS_CONNECT_DRIVERS_PROTOCOL__H__
#define __AMI_BDS_CONNECT_DRIVERS_PROTOCOL__H__

/// Protocol GUID.
/// The protocol has NULL interface. It is installed by BDS before it starts connecting drivers.
#define AMI_BDS_CONNECT_DRIVERS_PROTOCOL_GUID \
    { 0x3aa83745, 0x9454, 0x4f7a, { 0xa7, 0xc0, 0x90, 0xdb, 0xd0, 0x2f, 0xab, 0x8e } }


/// see ::AMI_BDS_CONNECT_DRIVERS_PROTOCOL_GUID
extern EFI_GUID gAmiBdsConnectDriversProtocolGuid;

#endif
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
