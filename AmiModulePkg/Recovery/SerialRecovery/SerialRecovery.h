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

/** @file SerialRecovery.h
    Header file for SerialRecovery
**/

#ifndef __SERIAL_RECOVERY__H__
#define __SERIAL_RECOVERY__H__
#ifdef __cplusplus
extern "C"
{
#endif

//----------------------------------------------------------------------

#include <Library/SerialPortLib.h>
#include <AmiPeiLib.h>
#include <Guid/AmiRecoveryDevice.h>
#include <Ppi/PeiRecoverySerialModePpi.h>
#include <Ppi/Stall.h>
#include <Ppi/DeviceRecoveryModule.h>

//----------------------------------------------------------------------

#define XMODEM_SOH              0x01        // Start Header
#define XMODEM_EOT              0x04        // End of Transfer
#define XMODEM_ACK              0x06        // Acknowledge
#define XMODEM_NAK              0x15        // Negative Acknowledge
#define XMODEM_CAN              0x18        // Cancel Transfer

BOOLEAN 
GetSerialData (
    UINT8*    ReadData 
);

VOID 
SendSerialData (
    UINT8*    d 
);

VOID 
SendSerialString (
    CHAR8*    Str 
);

VOID 
PrintCopyRightMessage ( );

EFI_STATUS 
EFIAPI 
GetNumberRecoveryCapsules (
    IN EFI_PEI_SERVICES                      **PeiServices,
    IN EFI_PEI_DEVICE_RECOVERY_MODULE_PPI    *This,
    OUT UINTN                                *NumberRecoveryCapsules 
);

EFI_STATUS 
EFIAPI 
GetRecoveryCapsuleInfo (
    IN EFI_PEI_SERVICES                      **PeiServices,
    IN EFI_PEI_DEVICE_RECOVERY_MODULE_PPI    *This,
    IN UINTN                                 CapsuleInstance,
    OUT UINTN                                *Size,
    OUT EFI_GUID                             *CapsuleType 
);

EFI_STATUS 
EFIAPI 
LoadRecoveryCapsule (
    IN OUT EFI_PEI_SERVICES                  **PeiServices,
    IN EFI_PEI_DEVICE_RECOVERY_MODULE_PPI    *This,
    IN UINTN                                 CapsuleInstance,
    OUT VOID                                 *Buffer 
);

BOOLEAN 
ReadFileFromSerial (
    CHAR8 *    Buffer,
    UINT32*    Size,
    UINT8 *    result 
);

BOOLEAN 
ReadSerialBlock (
    UINT8*    Buffer 
);

BOOLEAN 
SendNakAck (
    UINT8    * Data,
    UINT8    *XCommand 
);

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
