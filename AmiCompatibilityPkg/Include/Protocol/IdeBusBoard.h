//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
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
  defines Platform IDE Protocol
*/

#ifndef __IdeBusBoard__H__
#define __IdeBusBoard__H__

#ifdef __cplusplus
extern "C" {
#endif

#include <Efi.h>

#define PLATFORM_IDE_PROTOCOL_GUID \
    { 0x6737f69b, 0xb8cc, 0x45bc, 0x93, 0x27, 0xcc, 0xf5, 0xee, 0xf7, 0xc, 0xde }

//
// Protocol revision number
// Any backwards compatible changes to this protocol will result in an update in the revision number
//
// Revision 2: Changed from Original version to support for ATA/ATAPI timeout values as TOKENS.
//
#define PLATFORM_IDE_PROTOCOL_REVISION  2
//
// Protocol Revision Number supported by IdeBus Source.
//
#define PLATFORM_IDE_PROTOCOL_SUPPORTED_REVISION  2

//
// Interface structure for PLATFORM_IDE_PROTOCOL
//
typedef struct _PLATFORM_IDE_PROTOCOL {
    UINT8       Revision;
    BOOLEAN     MasterSlaveEnumeration;
    BOOLEAN     IdeBusMasterSupport; 
    BOOLEAN     AtapiBusMasterSupport; 
    BOOLEAN     AcousticManagementSupport; 
    BOOLEAN     IdePowerManagementSupport; 
    INT16       StandbyTimeout; 
    BOOLEAN     AdvPowerManagementSupport; 
    UINT8       AdvPowerManagementLevel; 
    BOOLEAN     PowerupInStandbySupport;
    BOOLEAN     PowerupInStandbyMode; 
    BOOLEAN     IdePwrManagementInterfaceSupport; 
    BOOLEAN     HostProtectedAreaSupport; 
    BOOLEAN     IdeHPSupport;
    BOOLEAN     EfiIdeProtocol; 
    BOOLEAN     AhciCompatibleMode; 
    BOOLEAN     SBIdeSupport;
    BOOLEAN     HddPowerLossInS3; 
    BOOLEAN     DiPMSupport;
    BOOLEAN     DisableSoftSetPrev;
    BOOLEAN     ForceHddPasswordPrompt;
    EFI_GUID    gIdeControllerProtocolGuid;
    EFI_GUID    gComponentNameProtocolGuid;
    UINT32      S3BusyClearTimeout;
    UINT32      DmaAtaCompleteCommandTimeout;
    UINT32      DmaAtaPiCompleteCommandTimeout;
    UINT32      AtaPiResetCommandTimeout;
    UINT32      AtaPiBusyClearTimeout;
    UINT32      PoweonBusyClearTimeout;
} PLATFORM_IDE_PROTOCOL;

extern EFI_GUID gPlatformIdeProtocolGuid;

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
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
