//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2010, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
//**********************************************************************
//
// $Header: $
//
// $Revision: $
//
// $Date: $
//
//**********************************************************************
// Revision History
// ----------------
// $Log: $
// 
//
//**********************************************************************
/** @file AmtLockKBD.c
    AMT Lock KeyBoard Functions.

**/
#include <token.h>
#include <EFI.h>
#include <DXE.h>
#include <AmiDxeLib.h>
#include <../Include/Protocol/ConsoleControl.h>
#if (defined(CSM_SUPPORT) && (CSM_SUPPORT != 0))
#include "csm/csm.h"
#endif
#include "../Include/Protocol/DevicePath.h"
#include "../Include/Protocol/DataHub.h"
#include "../Include/Protocol/SimpleTextIn.h"
#include "../Include/Protocol/SimpleTextInEx.h"
#include <Protocol/AlertStandardFormat.h>
#include "Include/Protocol/LegacyRegion.h"
#include <Protocol/SmmPowerButtonDispatch.h>   
#if (defined(LEGACYSREDIR_SUPPORT) && (LEGACYSREDIR_SUPPORT != 0))
#include <Protocol/AmiLegacySredir.h>
#include "Protocol/LegacyRegion2.h"
#endif
//============================================================================
// Local defines for transaction types
//============================================================================
#define EFI_SIGNATURE_16(A,B)             ((A) | ((B)<<8))
#define EFI_SIGNATURE_32(A,B,C,D) \
   (EFI_SIGNATURE_16((A),(B)) | (EFI_SIGNATURE_16((C),(D)) << 16))

#define AMT_INT9_SIGNATURE EFI_SIGNATURE_32 ('$', 'L', 'K', 'b')
#define SREDIR_FLAG_SIGNATURE EFI_SIGNATURE_32 ('$', 'S', 'r', 'F')

#define BDS_ALL_DRIVERS_CONNECTED_PROTOCOL_GUID \
 { 0xdbc9fd21, 0xfad8, 0x45b0, 0x9e, 0x78, 0x27, 0x15, 0x88, 0x67, 0xcc, 0x93 }
EFI_GUID    gBdsAllDriversConnectedProtocolGuid = \
                                    BDS_ALL_DRIVERS_CONNECTED_PROTOCOL_GUID;
EFI_GUID    gEfiSimpleTextInExProtocolGuid = \
                                    EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL_GUID;
EFI_GUID    gEfiSimpleTextInProtocolGuid = EFI_SIMPLE_TEXT_INPUT_PROTOCOL_GUID;
EFI_GUID    ConInStartedProtocolGuid = CONSOLE_IN_DEVICES_STARTED_PROTOCOL_GUID;

#define BDS_CONNECT_DRIVERS_PROTOCOL_GUID \
 { 0x3aa83745, 0x9454, 0x4f7a, 0xa7, 0xc0, 0x90, 0xdb, 0xd0, 0x2f, 0xab, 0x8e }
EFI_GUID    gEfiSmmPowerButtonDispatchProtocolGuid = \
                                    EFI_SMM_POWER_BUTTON_DISPATCH_PROTOCOL_GUID;
EFI_GUID    gBdsConnectDriversProtocolGuid = \
                                    BDS_CONNECT_DRIVERS_PROTOCOL_GUID;
/**
    This routine disables Power Button SMI if Lock Power Button
    bit of Boot Option BitMask is set.

    @param VOID

    @retval VOID

**/
VOID
ClearPowerButtonSmi (VOID)
{
    UINT32                      dPwrMgValue;

    // Read PM_1 Status/Enable Register
    dPwrMgValue = IoRead32(PM_BASE_ADDRESS);
    dPwrMgValue |= BIT08;           // clear Power Button Status
    dPwrMgValue &= ~BIT24;          // clear Power Button Enable
    IoWrite32 ( PM_BASE_ADDRESS, dPwrMgValue );
}
/**
    This routine disables Power Button SMI if Lock Power Button
    bit of Boot Option BitMask is set.

               
    @param Event 
    @param Context 

    @retval EFI_STATUS Status


**/
EFI_STATUS
DisablePowerButtonSmiEvent  (
    IN EFI_EVENT        Event,
    IN VOID             *Context
    )
{
    EFI_STATUS                  Status;
    EFI_SMM_POWER_BUTTON_DISPATCH_PROTOCOL  *PowerButton;

    Status = pBS->LocateProtocol (  &gEfiSmmPowerButtonDispatchProtocolGuid, \
                                    NULL, \
                                    &PowerButton    );
    if(EFI_ERROR( Status )) return Status;

    ClearPowerButtonSmi();
    
    pBS->CloseEvent(Event);

    return EFI_SUCCESS;
}
/**
    This routine register a Protocol Callback for Lock Power Button
    if Lock Power Button bit of Boot Option BitMask is set.

    @param VOID

    @retval VOID


**/
VOID
ASFRegisterPowerButtonCallBack (VOID)
{
    EFI_STATUS                  Status = EFI_SUCCESS;
    EFI_EVENT                   EvtASFSmmPwrBtn, EvtASFBdsConnectDrivers;
    VOID                        *RgnASFSmmPwrBtn, *RgnASFBdsConnectDrivers;

    // Clear Power Button Status before Power Button SMI is registered.
    Status = RegisterProtocolCallback ( \
                                    &gEfiSmmPowerButtonDispatchProtocolGuid, \
                                    DisablePowerButtonSmiEvent, \
                                    NULL, \
                                    &EvtASFSmmPwrBtn, \
                                    &RgnASFSmmPwrBtn  );

    DisablePowerButtonSmiEvent ( EvtASFSmmPwrBtn, NULL );

    Status = RegisterProtocolCallback ( \
                                    &gBdsConnectDriversProtocolGuid, \
                                    DisablePowerButtonSmiEvent, \
                                    NULL, \
                                    &EvtASFBdsConnectDrivers, \
                                    &RgnASFBdsConnectDrivers  );
    return ;
}
#if (defined(CSM_SUPPORT) && (CSM_SUPPORT != 0))

/**
    Aux roution to find the Signature in  Legacy Reagon

        
    @param dStartAddress 
    @param dSize 
    @param dSignature 

    @retval UINT32 Addr


**/
UINT32
GetLegacyDriverSignaturePtr (
    IN UINT32               dStartAddress,
    IN UINT32               dSize,
    IN UINT32               dSignature
)
{
    UINT8           *pStartAddr = (UINT8 *)(UINTN)dStartAddress;

    for ( ;(UINTN)pStartAddr < (UINTN)(dStartAddress + dSize); pStartAddr++ )
        if ( *(UINT32 *)pStartAddr == dSignature ) return ((UINT32)pStartAddr);
    return FALSE;
}

/**
    Sub-roution lock keyboard.

    @param VOID

    @retval VOID


**/
VOID
LegacyBiosLockLocalKeyboard (VOID)
{

    EFI_STATUS                    Status;
    UINT32                      dAMTInt9Ptr = 0, dSRedirFlagPtr = 0;
    EFI_LEGACY_REGION2_PROTOCOL *iRegion;
    UINT32					    Granularity;

    // Get AMT Int 9h signature pointer.
    dAMTInt9Ptr = GetLegacyDriverSignaturePtr ( \
                                    0xe0000, 0x20000, AMT_INT9_SIGNATURE );
    // Get Legacy Serial Redirection Flag signature pointer.
    dSRedirFlagPtr = GetLegacyDriverSignaturePtr ( \
                                    0x90000, 0x10000, SREDIR_FLAG_SIGNATURE );
    if ( !dAMTInt9Ptr ) return ;
    // Locate Legacy Bios Protocol for lock/unlock shadow ram.
    Status = pBS->LocateProtocol(&gEfiLegacyRegion2ProtocolGuid, NULL, &iRegion);
    if (EFI_ERROR(Status)) return;

    // Unlock Shadow ram.
    Status = iRegion->UnLock( iRegion, 0xE0000, 0x20000,  &Granularity);
    if (EFI_ERROR(Status)) return;
    // Update Legacy Serial Redirection Flag pointer to AMT Int 9h.
    if(!dSRedirFlagPtr) 
        dSRedirFlagPtr = (dAMTInt9Ptr + 4);
    *(UINT32*)(dAMTInt9Ptr + 4) = (UINT32)(((UINTN)dSRedirFlagPtr & 0xFFFF) + \
                                  (((UINTN)dSRedirFlagPtr & 0xF0000) << 12) + \
                                  sizeof ( SREDIR_FLAG_SIGNATURE ));
    // Lock Shadow ram.
    iRegion->Lock( iRegion, 0xE0000, 0x20000, &Granularity );

}
#endif
/**
    Sub-Roution to check if need disable keyboard or not
    from IntelAMT extersion ASF Boot Option Message.

    @param Event 
    @param Context 

    @retval VOID


**/
VOID
EventAllConsoleDevicesStarted 
(
    IN EFI_EVENT Event,
    IN VOID *Context
)
{
    EFI_STATUS                            Status;
    EFI_HANDLE                            *SimpleTextInHandle;
    UINTN                               HandleNum, Index;
    EFI_DEVICE_PATH_PROTOCOL            *DevicePathNode;
    EFI_DEVICE_PATH_PROTOCOL            *DevicePath;
    EFI_SIMPLE_TEXT_INPUT_PROTOCOL      *SimpleTextIn;
    EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL   *SimpleTextInEx;
    PCI_DEVICE_PATH                     *PciDevicePath;
    ALERT_STANDARD_FORMAT_PROTOCOL  *Asf;
    ASF_BOOT_OPTIONS             *mAsfBootOptions;

    Status = pBS->LocateProtocol (
                  &gAlertStandardFormatProtocolGuid,
                  NULL,
                  &Asf
                  );

    if (EFI_ERROR (Status)) {
      return;
    }

    Status = Asf->GetBootOptions (Asf, &mAsfBootOptions);
    // If not Lock keyboard, return !!
    if(!((mAsfBootOptions->BootOptions) & 0x20))
        return;

    // 2. Uninstall all the console spliter drivers except SOL.
    // Locate Simple Text Input Protocol Handle Buffer.
    Status = pBS->LocateHandleBuffer (  ByProtocol, \
                                        &gEfiSimpleTextInProtocolGuid, \
                                        NULL, \
                                        &HandleNum, \
                                        &SimpleTextInHandle  );
       if ( EFI_ERROR ( Status ) ) return ;

    // Search USB keyboard and Generic PS2 Keyboard Device Path.
    for ( Index = 0; Index < HandleNum; Index++ ) {

        // Locate Simple Text Input Protocol.
        Status = pBS->HandleProtocol (  SimpleTextInHandle[Index], \
                                        &gEfiDevicePathProtocolGuid, \
                                        (VOID *)&DevicePath );
        if ( EFI_ERROR ( Status ) ) continue;

        DevicePathNode = DevicePath;
        while ( !isEndNode ( DevicePathNode ) ) {
            PciDevicePath = (PCI_DEVICE_PATH *) DevicePathNode;
            // Is USB Device Path or Generic SIO Device Path (SouthBridge)?
            if ((( DevicePathNode->Type == MESSAGING_DEVICE_PATH ) && \
                ( DevicePathNode->SubType == MSG_USB_DP )) || \
                (( DevicePathNode->Type == HARDWARE_DEVICE_PATH ) && \
                ( DevicePathNode->SubType == HW_PCI_DP ) && \
                ( PciDevicePath->Device == 0x1f ) && \
                ( PciDevicePath->Function == 0 ))) {

                //  Locate Simple Text Input Protocol.
                Status = pBS->HandleProtocol (  SimpleTextInHandle[Index], \
                                                &gEfiSimpleTextInProtocolGuid, \
                                                (VOID **)&SimpleTextIn  );
                if ( EFI_ERROR ( Status ) ) break;

                // Uninstall Simple Text Input Protocol for stopping keyboard.
                Status = pBS->UninstallProtocolInterface ( \
                                            SimpleTextInHandle[Index], \
                                            &gEfiSimpleTextInProtocolGuid, \
                                            SimpleTextIn    );
                if ( EFI_ERROR ( Status ) ) break;

                    // Locate Simple Text Input EX Protocol.
                    Status = pBS->HandleProtocol ( \
                                            SimpleTextInHandle[Index], \
                                            &gEfiSimpleTextInExProtocolGuid, \
                                            (VOID **)&SimpleTextInEx  );
                    if ( EFI_ERROR ( Status ) ) break;

                    // Uninstall Simple Text Input EX Protocol for stopping
                    // SIO keyboard.
                    Status = pBS->UninstallProtocolInterface ( \
                                            SimpleTextInHandle[Index], \
                                            &gEfiSimpleTextInExProtocolGuid, \
                                            SimpleTextInEx  );
                break;
            }
            DevicePathNode = NEXT_NODE ( DevicePathNode );
        }
    }
}
/**
    Sub-Roution to check if need disable keyboard or not
    from IntelAMT extersion ASF Boot Option Message.

    @param Event 
    @param Context 

    @retval VOID


**/
VOID
LockLegacyKBD 
(
    IN EFI_EVENT Event,
    IN VOID *Context
)
{
#if (defined(LEGACYSREDIR_SUPPORT) && (LEGACYSREDIR_SUPPORT != 0))
    EFI_STATUS                            Status;
    EFI_GUID gEfiLegacySredirProtocolGuid = EFI_LEGACY_SREDIR_PROTOCOL_GUID;
    EFI_LEGACY_SREDIR_PROTOCOL  *pLegacySredir;


    Status = pBS->LocateProtocol(&gEfiLegacySredirProtocolGuid, NULL, &pLegacySredir);
    if(EFI_ERROR(Status)) return;
    Status = pLegacySredir->EnableLegacySredir(pLegacySredir);
    if(EFI_ERROR(Status)) return;
    // 1. Hook Int 9h for locking local keyboard but keep the Legacy SOL
    // working in CSM and Legacy OS.
#if (defined(CSM_SUPPORT) && (CSM_SUPPORT != 0))
    LegacyBiosLockLocalKeyboard ();
#endif
#endif
}
/**
    Lock keyboard Entrypoint.

    @param ImageHandle 
    @param SystemTable 

    @retval EFI_STATUS Status

**/
EFI_STATUS AMTLOCKKBDEntryPoint(
    IN EFI_HANDLE       ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable
)
{
    EFI_STATUS  Status;
    EFI_EVENT   ConInConnectEvt;
    VOID        *ConInNotifyReg;
    EFI_EVENT   ConInExConnectEvt;
    VOID        *ConInExNotifyReg;
    EFI_EVENT   AllDriverConnectEvt;
    VOID        *AllDriverExNotifyReg;

    ALERT_STANDARD_FORMAT_PROTOCOL  *Asf;
    ASF_BOOT_OPTIONS             *mAsfBootOptions;

    InitAmiLib(ImageHandle, SystemTable);
    Status = pBS->LocateProtocol (&gAlertStandardFormatProtocolGuid, \
                                    NULL, &Asf );
    if (EFI_ERROR (Status)) return Status;

    Status = Asf->GetBootOptions (Asf, &mAsfBootOptions);
    if (EFI_ERROR (Status)) return Status;
        
    // If not Lock keyboard, return !!
    if (mAsfBootOptions->BootOptions & 0x20) {

        Status = RegisterProtocolCallback(
                 &gEfiSimpleTextInExProtocolGuid,
                 EventAllConsoleDevicesStarted,
                 NULL,
                 &ConInExConnectEvt,
                 &ConInExNotifyReg);
    
        Status = RegisterProtocolCallback(
                 &gEfiSimpleTextInProtocolGuid,
                 EventAllConsoleDevicesStarted,
                 NULL,
                 &ConInConnectEvt,
                 &ConInNotifyReg);
    
        Status = RegisterProtocolCallback(
                 &gBdsAllDriversConnectedProtocolGuid,
                 LockLegacyKBD,
                 NULL,
                 &AllDriverConnectEvt,
                 &AllDriverExNotifyReg);

    }
    if (mAsfBootOptions->BootOptions & 0x2) ASFRegisterPowerButtonCallBack ();    
    return EFI_SUCCESS;
}
//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2010, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
