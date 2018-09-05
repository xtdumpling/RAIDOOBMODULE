//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
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
  @brief ROM Layout GUID Definitions.

  The file contains definitions of the GUID associated with the ROM Layout infrastructure.
**/

#ifndef __AMI_RESET_SYSTEM_EVENT_GUID__H__
#define __AMI_RESET_SYSTEM_EVENT_GUID__H__

/// Event group GUID.
/// This event is signaled by a driver that implements reset architectural protocol prior to resetting the system.
/// The event is only signaled at boot time (before ExitBootServices).
#define AMI_RESET_SYSTEM_EVENT_GUID \
    { 0x62da6a56, 0x13fb, 0x485a, { 0xa8, 0xda, 0xa3, 0xdd, 0x79, 0x12, 0xcb, 0x6b } }

/// see ::AMI_RESET_SYSTEM_EVENT_GUID
extern EFI_GUID gAmiResetSystemEventGuid;

#endif
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
