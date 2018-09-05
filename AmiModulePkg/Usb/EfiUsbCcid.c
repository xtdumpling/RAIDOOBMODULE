//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file EfiUsbCcid.c
    USB CCID EFI driver implementation

**/

#include "AmiDef.h"
#include "UsbDef.h"
#include "Uhcd.h"
#include "ComponentName.h"
#include "UsbBus.h"
#include "AmiUsbRtCcid.h"
#include "AmiUsbSmartCardReader.h"

extern  USB_GLOBAL_DATA *gUsbData;

// Below gEfiSmartCardReaderProtocolGuid definiion will be removed once
// it is added into MdePkg
EFI_GUID gEfiSmartCardReaderProtocolGuid = EFI_SMART_CARD_READER_PROTOCOL_GUID;

EFI_STATUS
EFIAPI
SupportedUSBCCID(
    EFI_DRIVER_BINDING_PROTOCOL *This,
    EFI_HANDLE                  Controller,
    EFI_DEVICE_PATH_PROTOCOL    *DevicePath
);

EFI_STATUS
EFIAPI
StartUSBCCID(
    EFI_DRIVER_BINDING_PROTOCOL *This,
    EFI_HANDLE                  Controller,
    EFI_DEVICE_PATH_PROTOCOL    *DevicePath
);

EFI_STATUS
EFIAPI
StopUSBCCID(
    EFI_DRIVER_BINDING_PROTOCOL *This,
    EFI_HANDLE                  Controller,
    UINTN                       NumberOfChildren,
    EFI_HANDLE                  *Children
);

EFI_STATUS
GetReturnValue(
    UINT8    bRetValue
);

ICC_DEVICE*
GetICCDevice(
    DEV_INFO        *fpDevInfo, 
    UINT8            Slot
);

EFI_STATUS
EFIAPI
USBCCIDAPISmartClassDescriptor(
    IN AMI_CCID_IO_PROTOCOL             *This,
    OUT UINT8                           *ResponseBuffer
);

EFI_STATUS
EFIAPI
USBCCIDAPIGetAtr(
    IN AMI_CCID_IO_PROTOCOL *This,
    UINT8 Slot,
    UINT8 *ATRData
);

EFI_STATUS
EFIAPI
USBCCIDAPIPowerupSlot(
    IN AMI_CCID_IO_PROTOCOL             *This,
    OUT UINT8                           *bStatus,
    OUT UINT8                           *bError,
    OUT UINT8                           *ATRData
);

EFI_STATUS
EFIAPI
USBCCIDAPIPowerDownSlot(
    IN AMI_CCID_IO_PROTOCOL             *This,
    OUT UINT8                           *bStatus,
    OUT UINT8                           *bError
);

EFI_STATUS
EFIAPI
USBCCIDAPIGetSlotStatus(
    IN AMI_CCID_IO_PROTOCOL              *This,
    OUT UINT8                            *bStatus,
    OUT UINT8                            *bError,
    OUT UINT8                            *bClockStatus

);

EFI_STATUS
EFIAPI
USBCCIDAPIXfrBlock(
    IN AMI_CCID_IO_PROTOCOL             *This,
    IN UINTN                            CmdLength,
    IN UINT8                            *CmdBuffer,
    IN UINT8                            ISBlock,
    OUT UINT8                           *bStatus,
    OUT UINT8                           *bError,
    IN OUT UINTN                        *ResponseLength,
    OUT UINT8                           *ResponseBuffer
);

EFI_STATUS
EFIAPI
USBCCIDAPIGetParameters(
    IN AMI_CCID_IO_PROTOCOL             *This,
    OUT UINT8                           *bStatus,
    OUT UINT8                           *bError,
    IN OUT UINTN                        *ResponseLength,
    OUT UINT8                           *ResponseBuffer
);

EFI_GUID gAmiCCIDIoProtocolGuid = AMI_CCID_IO_PROTOCOL_GUID;
EFI_GUID gAmiCCIDPresenceGuid = AMI_CCID_PRESENCE_GUID;

EFI_EVENT       gEvICCEnumTimer = 0;
UINTN           gCounterCCIDEnumTimer = 0;
UINTN           gICCLock = 0;

#define    USBCCID_DRIVER_VERSION        1

NAME_SERVICE_T Names;

EFI_DRIVER_BINDING_PROTOCOL CcidBindingProtocol = {
        SupportedUSBCCID,
        StartUSBCCID,
        StopUSBCCID,
        USBCCID_DRIVER_VERSION,
        NULL,
        NULL 
};

/**
    Function installs EFI_SMART_CARD_READER_PROTOCOL_GUID for Smart card reader's slot

    @param
        EFI_HANDLE      CCIDHandle
        DEV_INFO        *fpCCIDDevice
        ICC_DEVICE      *fpICCDevice

    @retval EFI STATUS

**/

EFI_STATUS
InstallUSBSCardReaderProtocolOnSlot(
    EFI_HANDLE      CCIDHandle,
    DEV_INFO        *fpCCIDDevice,
    ICC_DEVICE      *fpICCDevice
)
{

    EFI_STATUS             Status = EFI_INVALID_PARAMETER;
    EFI_USB_IO_PROTOCOL    *UsbIo = NULL;
    USB_SCARD_DEV          *SCardDev=NULL;

    // Install Protocol for the SCard Reader Slot
    if(!fpCCIDDevice || !((SMARTCLASS_DESC*)(fpCCIDDevice->pCCIDDescriptor)) ) {
        return EFI_INVALID_PARAMETER;
    }

    gBS->AllocatePool(EfiBootServicesData, sizeof(USB_SCARD_DEV), &SCardDev);
    SCardDev->ChildHandle = 0;
    SCardDev->ControllerHandle = CCIDHandle;
    SCardDev->Slot       = fpICCDevice->Slot;
    SCardDev->DevInfo = fpCCIDDevice;

    SCardDev->EfiSmartCardReaderProtocol.SCardConnect = USBSCardReaderAPIConnect;
    SCardDev->EfiSmartCardReaderProtocol.SCardStatus = USBSCardReaderAPIStatus;
#if defined(MDE_PKG_VERSION) && (MDE_PKG_VERSION>=10)
    SCardDev->EfiSmartCardReaderProtocol.SCardDisconnect = USBSCardReaderAPIDisconnect;
    SCardDev->EfiSmartCardReaderProtocol.SCardTransmit = USBSCardReaderAPITransmit;
#else
    SCardDev->EfiSmartCardReaderProtocol.SCardDisConnect = USBSCardReaderAPIDisconnect;
    SCardDev->EfiSmartCardReaderProtocol.ScardTransmit = USBSCardReaderAPITransmit;
#endif
    SCardDev->EfiSmartCardReaderProtocol.SCardControl = USBSCardReaderAPIControl;
    SCardDev->EfiSmartCardReaderProtocol.SCardGetAttrib = USBSCardReaderAPIGetAttrib;

    Status = gBS->InstallProtocolInterface(
                        &SCardDev->ChildHandle,
                        &gEfiSmartCardReaderProtocolGuid,
                        EFI_NATIVE_INTERFACE,
                        &SCardDev->EfiSmartCardReaderProtocol
                        );
    ASSERT_EFI_ERROR(Status);

    fpICCDevice->SCardChildHandle = SCardDev->ChildHandle;

    Status = gBS->OpenProtocol (
                        CCIDHandle,
                        &gEfiUsbIoProtocolGuid,
                        &UsbIo,
                        CcidBindingProtocol.DriverBindingHandle,
                        SCardDev->ChildHandle,
                        EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER );

    ASSERT_EFI_ERROR(Status);

    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "Install SCard on %lx status = %x SCardDev->ChildHandle %x \n", CCIDHandle, Status, SCardDev->ChildHandle);

    return Status;
}

/**
    Function installs AMI_CCID_IO_PROTOCOL_GUID for the ICC card

    @param
        EFI_HANDLE      CCIDHandle
        DEV_INFO        *fpCCIDDevice
        ICC_DEVICE      *fpICCDevice

    @retval EFI STATUS

**/

EFI_STATUS
InstallUSBCCID(
    EFI_HANDLE      CCIDHandle,
    DEV_INFO        *fpCCIDDevice, 
    ICC_DEVICE      *fpICCDevice
)
{

    EFI_STATUS                  Status = EFI_INVALID_PARAMETER;
    USB_ICC_DEV                 *ICCDev;
    EFI_USB_IO_PROTOCOL         *UsbIo;

    //
    // Install Protocol irrespective of device found or not. 
    // By checking ChildHandle here, avoid repeated protocol installation.
    //

    if (fpICCDevice && !fpICCDevice->ChildHandle) {
    
        USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_USBBUS, "InstallUSBCCID ....\n" );

        gBS->AllocatePool(EfiBootServicesData, sizeof(USB_ICC_DEV), &ICCDev);
        ICCDev->ChildHandle = 0;
        ICCDev->ControllerHandle = 0;
        ICCDev->DevInfo    = fpCCIDDevice;
        ICCDev->Slot       = fpICCDevice->Slot;


        ICCDev->CCIDIoProtocol.USBCCIDAPISmartClassDescriptor = USBCCIDAPISmartClassDescriptor;
        ICCDev->CCIDIoProtocol.USBCCIDAPIGetAtr = USBCCIDAPIGetAtr;
        ICCDev->CCIDIoProtocol.USBCCIDAPIPowerupSlot = USBCCIDAPIPowerupSlot;
        ICCDev->CCIDIoProtocol.USBCCIDAPIPowerDownSlot = USBCCIDAPIPowerDownSlot;
        ICCDev->CCIDIoProtocol.USBCCIDAPIGetSlotStatus = USBCCIDAPIGetSlotStatus;
        ICCDev->CCIDIoProtocol.USBCCIDAPIXfrBlock = USBCCIDAPIXfrBlock;
        ICCDev->CCIDIoProtocol.USBCCIDAPIGetParameters = USBCCIDAPIGetParameters;
    
        Status = gBS->InstallProtocolInterface(
                        &ICCDev->ChildHandle,
                        &gAmiCCIDIoProtocolGuid,
                        EFI_NATIVE_INTERFACE,
                        &ICCDev->CCIDIoProtocol
                        );

        ASSERT_EFI_ERROR(Status);

        fpICCDevice->ChildHandle = ICCDev->ChildHandle;

        USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "ICCDev->ChildHandle = %x\n", ICCDev->ChildHandle);

        Status = gBS->OpenProtocol (
                        CCIDHandle,
                        &gEfiUsbIoProtocolGuid,
                        &UsbIo,
                        CcidBindingProtocol.DriverBindingHandle,
                        ICCDev->ChildHandle,
                        EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER );
    
        USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "Install CCID on %x status = %r\n", CCIDHandle, Status);
        ASSERT_EFI_ERROR(Status);

    }

    return Status;

}

/**
    Uninstall EFI_SMART_CARD_READER_PROTOCOL for Smart card reader's slot

    @param 
        EFI_HANDLE    Controller 
        EFI_HANDLE    ScardHandle 
        EFI_HANDLE    DriverBindingHandle

    @retval
        EFI_STATUS

**/

EFI_STATUS
UnInstallSCardProtocol (
    EFI_HANDLE    Controller,
    EFI_HANDLE    ScardHandle,
    EFI_HANDLE    DriverBindingHandle
)
{

    EFI_STATUS                       Status;
    EFI_SMART_CARD_READER_PROTOCOL   *SmartCardReaderProtocol;

    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_USBBUS, "UnInstallSCardProtocol ....ChildHandle : %lx", ScardHandle);

    Status = gBS->OpenProtocol ( ScardHandle,
                                &gEfiSmartCardReaderProtocolGuid,
                                &SmartCardReaderProtocol,
                                DriverBindingHandle,
                                ScardHandle,
                                EFI_OPEN_PROTOCOL_GET_PROTOCOL);
    
    if(!EFI_ERROR(Status)) {

        Status = gBS->CloseProtocol (Controller, 
                                     &gEfiUsbIoProtocolGuid, 
                                     DriverBindingHandle, 
                                     ScardHandle);
        
        ASSERT_EFI_ERROR(Status);

        Status = pBS->UninstallProtocolInterface ( ScardHandle, 
                                                   &gEfiSmartCardReaderProtocolGuid, 
                                                   SmartCardReaderProtocol);

        ASSERT_EFI_ERROR(Status);

        Status = gBS->FreePool(SmartCardReaderProtocol);
        ASSERT_EFI_ERROR(Status);
    }

    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_USBBUS, " Status : %r\n", Status);
    return Status;
}

/**
    Uninstall AMI_CCID_IO_PROTOCOL_GUID for each slot

    @param 
        EFI_HANDLE    CCIDHandle - SmartCard Reader Handle
        EFI_HANDLE    ChildHandle - Smart Card Handle

    @retval 

**/

EFI_STATUS
UnInstallUSBCCID (
    EFI_HANDLE    CCIDHandle,
    EFI_HANDLE    ChildHandle
)
{

    EFI_STATUS                Status;
    AMI_CCID_IO_PROTOCOL      *CCIDIoProtocol;

    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_USBBUS, "UnInstallUSBCCID ....ChildHandle : %lx", ChildHandle );

    Status = gBS->OpenProtocol ( ChildHandle, 
                                &gAmiCCIDIoProtocolGuid, 
                                &CCIDIoProtocol, 
                                CcidBindingProtocol.DriverBindingHandle, 
                                ChildHandle, 
                                EFI_OPEN_PROTOCOL_GET_PROTOCOL);

    //
    // CCID protocol not found on ChildHandle. return with error.
    //
    if(!EFI_ERROR(Status)) {
        // If CCID protocol Found, close the protocol and uninstall the protocol interface.

        Status = gBS->CloseProtocol (CCIDHandle, 
                                     &gEfiUsbIoProtocolGuid, 
                                     CcidBindingProtocol.DriverBindingHandle, 
                                     ChildHandle);

        Status = pBS->UninstallProtocolInterface ( ChildHandle, 
                                                   &gAmiCCIDIoProtocolGuid, 
                                                   CCIDIoProtocol);
    
        if(!EFI_ERROR(Status)){
            gBS->FreePool(CCIDIoProtocol);
        }
    }

    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_USBBUS, " Status : %r\n", Status);
    return Status;
}

/**
    Generates a SW SMI to get the SMART Class Descriptor for the CCID device

        
    @param This 
    @param ResponseBuffer 

    @retval 
        EFI_STATUS and returns SMART Class Descriptor in ResponseBuffer

**/

EFI_STATUS
EFIAPI
USBCCIDAPISmartClassDescriptor(
    IN AMI_CCID_IO_PROTOCOL             *This,
    OUT UINT8                           *ResponseBuffer
)
{

    EFI_STATUS  Status;
    URP_STRUC   Parameters = {0};

    Parameters.bFuncNumber = USB_API_CCID_DEVICE_REQUEST;
    Parameters.bSubFunc = USB_CCID_SMARTCLASSDESCRIPTOR;

    Parameters.ApiData.CCIDSmartClassDescriptor.fpDevInfo = (UINTN)((USB_ICC_DEV *)This)->DevInfo;
    Parameters.ApiData.CCIDSmartClassDescriptor.Slot = ((USB_ICC_DEV *)This)->Slot;
    Parameters.ApiData.CCIDSmartClassDescriptor.fpResponseBuffer = (UINTN)ResponseBuffer;
     
    InvokeUsbApi(&Parameters);

    Status = GetReturnValue(Parameters.bRetValue);

    return Status;

}

/**
    Generates a SW SMI to get the ATR data

        
    @param This 
    @param Slot 
    @param ATRData 

    @retval 
        EFI_STATUS and returns  ATR data if available

    @note  
      ATRData buffer length should be 32 bytes long. Caller should allocate 
      memory for *ATRData. 

**/

EFI_STATUS
EFIAPI
USBCCIDAPIGetAtr(
    IN AMI_CCID_IO_PROTOCOL             *This,
    IN UINT8                            Slot,
    OUT UINT8                           *ATRData
)
{

    EFI_STATUS  Status;
    URP_STRUC   Parameters = { 0 };

    Parameters.bFuncNumber = USB_API_CCID_DEVICE_REQUEST;
    Parameters.bSubFunc = USB_CCID_ATR;

    Parameters.ApiData.CCIDAtr.fpDevInfo = (UINTN)((USB_ICC_DEV *)This)->DevInfo;
    Parameters.ApiData.CCIDAtr.Slot = ((USB_ICC_DEV *)This)->Slot;
    Parameters.ApiData.CCIDAtr.ATRData = (UINTN)ATRData;
    
    InvokeUsbApi(&Parameters);

    Status = GetReturnValue(Parameters.bRetValue);

    return Status;

}

/**
    Generates a SW SMI to power up the slot in CCID

        
    @param This 
    @param Slot 
    @param bStatus 
    @param bError 
    @param ATRData 

    @retval 
        EFI_STATUS and returns  ATR data if Card powered up successfully.

    @note  
              ATRData buffer length should be 32 bytes long. Caller should allocate memory for *ATRData. 
              Presence/Absence of card can be determined from *bStatus/*bError.

**/

EFI_STATUS
EFIAPI
USBCCIDAPIPowerupSlot(
    IN AMI_CCID_IO_PROTOCOL                 *This,
    OUT UINT8                               *bStatus,
    OUT UINT8                               *bError,
    OUT UINT8                               *ATRData
)
{

    EFI_STATUS              Status;
    URP_STRUC               Parameters = { 0 };

    Parameters.bFuncNumber = USB_API_CCID_DEVICE_REQUEST;
    Parameters.bSubFunc = USB_CCID_POWERUP_SLOT;

    Parameters.ApiData.CCIDPowerupSlot.fpDevInfo = (UINTN)((USB_ICC_DEV *)This)->DevInfo;
    Parameters.ApiData.CCIDPowerupSlot.Slot = ((USB_ICC_DEV *)This)->Slot;
    Parameters.ApiData.CCIDPowerupSlot.ATRData = (UINTN)ATRData;
    
	InvokeUsbApi(&Parameters);

    *bStatus = Parameters.ApiData.CCIDPowerupSlot.bStatus; 
    *bError = Parameters.ApiData.CCIDPowerupSlot.bError;

    Status = GetReturnValue(Parameters.bRetValue);

    return Status;

}

/**
    Generates a SW SMI to power down the slot in CCID.

        
    @param This 
    @param Slot 
    @param bStatus 
    @param bError 

    @retval 
        EFI_STATUS 

**/

EFI_STATUS
EFIAPI
USBCCIDAPIPowerDownSlot(
    IN AMI_CCID_IO_PROTOCOL                 *This,
    OUT UINT8                               *bStatus,
    OUT UINT8                               *bError
)
{

    EFI_STATUS              Status;
    URP_STRUC               Parameters = { 0 };

    Parameters.bFuncNumber = USB_API_CCID_DEVICE_REQUEST;
    Parameters.bSubFunc = USB_CCID_POWERDOWN_SLOT;

    Parameters.ApiData.CCIDPowerdownSlot.fpDevInfo = (UINTN)((USB_ICC_DEV *)This)->DevInfo;
    Parameters.ApiData.CCIDPowerdownSlot.Slot = ((USB_ICC_DEV *)This)->Slot;

	InvokeUsbApi(&Parameters);

    *bStatus = Parameters.ApiData.CCIDPowerdownSlot.bStatus; 
    *bError = Parameters.ApiData.CCIDPowerdownSlot.bError;

    Status = GetReturnValue(Parameters.bRetValue);

    return Status;

}

/**
    This API returns data from RDR_to_PC_SlotStatus

        
    @param This 
    @param bStatus 
    @param bError 
    @param bClockStatus 

    @retval 
        OUT UINT8                            *bStatus - Sec 6.2.6 if CCID spec ver 1.2
        OUT UINT8                            *bError - Sec 6.2.6 if CCID spec ver 1.2
        OUT UINT8                            *bClockStatus

**/

EFI_STATUS
EFIAPI
USBCCIDAPIGetSlotStatus(
    IN AMI_CCID_IO_PROTOCOL             *This,
    OUT UINT8                           *bStatus,
    OUT UINT8                           *bError,
    OUT UINT8                           *bClockStatus

)
{

    EFI_STATUS              Status;
    URP_STRUC               Parameters = { 0 };

    Parameters.bFuncNumber = USB_API_CCID_DEVICE_REQUEST;
    Parameters.bSubFunc = USB_CCID_GET_SLOT_STATUS;

    Parameters.ApiData.CCIDGetSlotStatus.fpDevInfo = (UINTN)((USB_ICC_DEV *)This)->DevInfo;
    Parameters.ApiData.CCIDGetSlotStatus.Slot = ((USB_ICC_DEV *)This)->Slot;

	InvokeUsbApi(&Parameters);

    *bStatus = Parameters.ApiData.CCIDGetSlotStatus.bStatus; 
    *bError = Parameters.ApiData.CCIDGetSlotStatus.bError;
    *bClockStatus = Parameters.ApiData.CCIDGetSlotStatus.bClockStatus;

    Status = GetReturnValue(Parameters.bRetValue);

    return Status;

}

/**
    This API generates a SWSMI to execute the USB_CCID_XFRBLOCK API.

        
    @param This 
    @param CmdLength Length of CmdBuffer
    @param CmdBuffer Buffer prepared to be sent to ICC through PC_TO_RDR_XFRBLOCK cmd
    @param ISBlock Valid only in T1 TDPU        
    @param bStatus 
    @param bError 
    @param ResponseLength Lenght of the Responsebuffer
    @param ResponseBuffer 
 
    @retval 
        EFI_STATUS
        OUT UINT8                   *bStatus - Sec 6.2.6 if CCID spec ver 1.2 
        OUT UINT8                   *bError - Sec 6.2.6 if CCID spec ver 1.2
        IN OUT UINTN                *ResponseLength - Actual number of Bytes returned in ResponseBuffer
        OUT UINT8                   *ResponseBuffer - Response bytes

    @note  
       ISBlock is valid only for T1. For updating IFS use S_IFS_REQUEST(0xC1).
       For WTX request use S_WTX_REQUEST (0xC3). For all others use I_BLOCK(0x0)
**/

EFI_STATUS
EFIAPI
USBCCIDAPIXfrBlock(
    IN AMI_CCID_IO_PROTOCOL                *This,
    IN UINTN                            CmdLength,
    IN UINT8                            *CmdBuffer,
    IN UINT8                             ISBlock,
    OUT UINT8                            *bStatus,
    OUT UINT8                            *bError,
    IN OUT UINTN                        *ResponseLength,
    OUT UINT8                            *ResponseBuffer
)
{

    EFI_STATUS              Status;
    URP_STRUC               Parameters = { 0 };

    Parameters.bFuncNumber = USB_API_CCID_DEVICE_REQUEST;
    Parameters.bSubFunc = USB_CCID_XFRBLOCK;
    Parameters.ApiData.CCIDXfrBlock.CmdLength = CmdLength;
    Parameters.ApiData.CCIDXfrBlock.fpCmdBuffer = (UINT32)(UINTN)CmdBuffer;
    Parameters.ApiData.CCIDXfrBlock.ISBlock = I_BLOCK;
    Parameters.ApiData.CCIDXfrBlock.ResponseLength = *ResponseLength;
    Parameters.ApiData.CCIDXfrBlock.fpResponseBuffer = (UINTN)ResponseBuffer;

    Parameters.ApiData.CCIDXfrBlock.fpDevInfo = (UINTN)((USB_ICC_DEV *)This)->DevInfo;
    Parameters.ApiData.CCIDXfrBlock.Slot = ((USB_ICC_DEV *)This)->Slot;
    
 	InvokeUsbApi(&Parameters);

    *bStatus = Parameters.ApiData.CCIDXfrBlock.bStatus ;
    *bError = Parameters.ApiData.CCIDXfrBlock.bError;
    *ResponseLength = Parameters.ApiData.CCIDXfrBlock.ResponseLength;

    Status = GetReturnValue(Parameters.bRetValue);

    return Status;

}

/**
    Returns data from PC_TO_RDR_GETPARAMETERS/RDR_to_PCParameters cmd

        
    @param This 
    @param bStatus 
    @param bError 
    @param ResponseLength 
    @param ResponseBuffer 

    @retval 
        IN AMI_CCID_IO_PROTOCOL         *This,
        OUT UINT8                       *bStatus,
        OUT UINT8                       *bError,
        IN OUT UINTN                    *ResponseLength,
        OUT UINT8                       *ResponseBuffer

**/

EFI_STATUS
EFIAPI
USBCCIDAPIGetParameters(
    IN AMI_CCID_IO_PROTOCOL                *This,
    OUT UINT8                            *bStatus,
    OUT UINT8                            *bError,
    IN OUT UINTN                        *ResponseLength,
    OUT UINT8                            *ResponseBuffer
)
{

    EFI_STATUS              Status;
    URP_STRUC               Parameters = { 0 };

    Parameters.bFuncNumber = USB_API_CCID_DEVICE_REQUEST;
    Parameters.bSubFunc = USB_CCID_GET_PARAMETERS;
    Parameters.ApiData.CCIDGetParameters.ResponseLength = *ResponseLength;
    Parameters.ApiData.CCIDGetParameters.fpResponseBuffer = (UINT32)(UINTN)ResponseBuffer;

    Parameters.ApiData.CCIDGetParameters.fpDevInfo = (UINTN)((USB_ICC_DEV *)This)->DevInfo;
    Parameters.ApiData.CCIDGetParameters.Slot = ((USB_ICC_DEV *)This)->Slot;
    
	InvokeUsbApi(&Parameters);

    *bStatus = Parameters.ApiData.CCIDGetParameters.bStatus ;
    *bError = Parameters.ApiData.CCIDGetParameters.bError;
    *ResponseLength = Parameters.ApiData.CCIDGetParameters.ResponseLength;

    Status = GetReturnValue(Parameters.bRetValue);

    return Status;

}

/**
    Function to connect the Smard Card reader/ICC card in order to access the Smart
         reader/ICC card

    @param This 
    @param AccessMode 
    @param CardAction
    @param PreferredProtocols
    @param ActiveProtocol

    @retval 
        EFI_STATUS

**/

EFI_STATUS
EFIAPI
USBSCardReaderAPIConnect(
    IN EFI_SMART_CARD_READER_PROTOCOL *This,
    IN UINT32                         AccessMode,
    IN UINT32                         CardAction,
    IN UINT32                         PreferredProtocols,
    OUT UINT32                        *ActiveProtocol
)
{
    URP_STRUC               Parameters = { 0 };
    
    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_USBBUS, "USBSCardReaderAPIConnect AccessMode : %x CardAction : %x  PreferredProtocols : %x \n",\
        AccessMode, CardAction, PreferredProtocols);

    // Return error if AccessMode or CardAction input parameter is Invalid
    if( !((AccessMode==SCARD_AM_READER)||(AccessMode==SCARD_AM_CARD)) || 
            !((CardAction >=SCARD_CA_NORESET) && (CardAction <=SCARD_CA_EJECT)) ) {
        return EFI_INVALID_PARAMETER;
    }

    // Invalid Input parameter for ScardReader connect call
    if(AccessMode == SCARD_AM_READER) {
        // if AccessMode is set to SCARD_AM_READER,PreferredProtocol must be set to 
        // SCARD_PROTOCOL_UNDEFINED,CardAction to SCARD_CA_NORESET else error
        if( CardAction!=SCARD_CA_NORESET || PreferredProtocols!=SCARD_PROTOCOL_UNDEFINED ) {
            return EFI_INVALID_PARAMETER;
        }
    }

    Parameters.bFuncNumber = USB_API_CCID_DEVICE_REQUEST;
    Parameters.bSubFunc = USB_SMART_CARD_READER_CONNECT;
    
    Parameters.ApiData.SmartCardReaderConnect.fpDevInfo = (UINTN)((USB_SCARD_DEV *)This)->DevInfo;
    Parameters.ApiData.SmartCardReaderConnect.Slot = (UINTN)((USB_SCARD_DEV *)This)->Slot;
    Parameters.ApiData.SmartCardReaderConnect.AccessMode = AccessMode;
    Parameters.ApiData.SmartCardReaderConnect.CardAction = CardAction;
    Parameters.ApiData.SmartCardReaderConnect.PreferredProtocols = PreferredProtocols;
    Parameters.ApiData.SmartCardReaderConnect.ActiveProtocol = (UINTN)ActiveProtocol;
    
    InvokeUsbApi(&Parameters);

    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_USBBUS, "USBSCardReaderAPIConnect Status : %r ActiveProtocol : %x\n", \
             Parameters.ApiData.SmartCardReaderConnect.EfiStatus, *ActiveProtocol);
    return Parameters.ApiData.SmartCardReaderConnect.EfiStatus;
}

/**
    Function disconnects Scard Reader/ICC card

    @param This 
    @param CardAction

    @retval 
        EFI_STATUS

**/

EFI_STATUS
EFIAPI
USBSCardReaderAPIDisconnect(
    IN EFI_SMART_CARD_READER_PROTOCOL  *This,
    IN UINT32                          CardAction
)
{
    URP_STRUC               Parameters = { 0 };

    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_USBBUS, "USBSCardReaderAPIDisconnect CardAction : %x \n", CardAction);

    Parameters.bFuncNumber = USB_API_CCID_DEVICE_REQUEST;
    Parameters.bSubFunc = USB_SMART_CARD_READER_DISCONNECT;

    Parameters.ApiData.SmartCardReaderDisconnect.fpDevInfo = (UINTN)((USB_SCARD_DEV *)This)->DevInfo;
    Parameters.ApiData.SmartCardReaderDisconnect.Slot = (UINTN)((USB_SCARD_DEV *)This)->Slot;
    Parameters.ApiData.SmartCardReaderDisconnect.CardAction = CardAction;
    Parameters.ApiData.SmartCardReaderDisconnect.EfiStatus = 0;

    InvokeUsbApi(&Parameters);

    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_USBBUS, "USBSCardReaderAPIDisconnect Status : %r \n", \
         Parameters.ApiData.SmartCardReaderDisconnect.EfiStatus );

    return Parameters.ApiData.SmartCardReaderDisconnect.EfiStatus;
}

/**
    Function to get the status of the ICC card connected in Smart card reader

    @param This 
    @param ReaderName
    @param ReaderNameLength
    @param State
    @param CardProtocol
    @param Atr
    @param AtrLength

    @retval
        EFI_STATUS

**/

EFI_STATUS
EFIAPI
USBSCardReaderAPIStatus(
    IN EFI_SMART_CARD_READER_PROTOCOL *This,
    OUT CHAR16                        *ReaderName,
    IN OUT UINTN                      *ReaderNameLength,
    OUT UINT32                        *State,
    OUT UINT32                        *CardProtocol,
    OUT UINT8                         *Atr,
    IN OUT UINTN                      *AtrLength
)
{
    URP_STRUC                  Parameters = { 0 };

    if( EFI_ERROR( GetSmartCardReaderName( ((USB_SCARD_DEV *)This)->ControllerHandle,
                            ReaderName, 
                            ReaderNameLength ) ) ) {
        return EFI_BUFFER_TOO_SMALL;
    }


    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_USBBUS, "USBSCardReaderAPIStatus ReaderName : %lx ReaderNameLength : %lx Atr : %lx AtrLength : %lx \n",\
        ReaderName, *ReaderNameLength, Atr, *AtrLength);
    
    Parameters.bFuncNumber = USB_API_CCID_DEVICE_REQUEST;
    Parameters.bSubFunc = USB_SMART_CARD_READER_STATUS;
    
    Parameters.ApiData.SmartCardReaderStatus.fpDevInfo = (UINTN)((USB_SCARD_DEV *)This)->DevInfo;
    Parameters.ApiData.SmartCardReaderStatus.Slot = (UINTN)((USB_SCARD_DEV *)This)->Slot;
    Parameters.ApiData.SmartCardReaderStatus.ReaderName = (UINTN)ReaderName;
    Parameters.ApiData.SmartCardReaderStatus.ReaderNameLength = (UINTN)ReaderNameLength;
    Parameters.ApiData.SmartCardReaderStatus.State = (UINTN)State;
    Parameters.ApiData.SmartCardReaderStatus.CardProtocol = (UINTN)CardProtocol;
    Parameters.ApiData.SmartCardReaderStatus.Atr = (UINTN)Atr;
    Parameters.ApiData.SmartCardReaderStatus.AtrLength = (UINTN)AtrLength;
      
    InvokeUsbApi(&Parameters);

    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_USBBUS, "USBSCardReaderAPIStatus ReaderNameLength : %lx AtrLength : %lx  Status : %r\n",\
         *ReaderNameLength, *AtrLength, Parameters.ApiData.SmartCardReaderStatus.EfiStatus);
    
    return Parameters.ApiData.SmartCardReaderStatus.EfiStatus;
}

/**
    This function sends a command to the card or reader and returns its response.

    @param This 
    @param CAPDU
    @param CAPDULength
    @param RAPDU
    @param RAPDULength

    @retval
        EFI_STATUS

**/

EFI_STATUS
EFIAPI
USBSCardReaderAPITransmit(
    IN EFI_SMART_CARD_READER_PROTOCOL *This,
    IN UINT8                          *CAPDU,
    IN UINTN                          CAPDULength,
    OUT UINT8                         *RAPDU,
    IN OUT UINTN                      *RAPDULength
)
{
    URP_STRUC               Parameters = { 0 };
    
    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_USBBUS, "USBSCardReaderAPITransmit CAPDU : %lx CAPDULength : %lx RAPDU : %lx RAPDULength : %lx \n",\
          CAPDU, CAPDULength, RAPDU, *RAPDULength);

    Parameters.bFuncNumber = USB_API_CCID_DEVICE_REQUEST;
    Parameters.bSubFunc = USB_SMART_CARD_READER_TRANSMIT;
    
    Parameters.ApiData.SmartCardReaderTransmit.fpDevInfo = (UINTN)((USB_SCARD_DEV *)This)->DevInfo;
    Parameters.ApiData.SmartCardReaderTransmit.Slot = (UINTN)((USB_SCARD_DEV *)This)->Slot;
    Parameters.ApiData.SmartCardReaderTransmit.CAPDU = (UINTN)CAPDU;
    Parameters.ApiData.SmartCardReaderTransmit.CAPDULength = CAPDULength;
    Parameters.ApiData.SmartCardReaderTransmit.RAPDU = (UINTN)RAPDU;
    Parameters.ApiData.SmartCardReaderTransmit.RAPDULength = (UINTN)RAPDULength;
     
    InvokeUsbApi(&Parameters);

    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_USBBUS, "USBSCardReaderAPITransmit RAPDULength : %lx Status : %r\n",\
            *RAPDULength, Parameters.ApiData.SmartCardReaderTransmit.EfiStatus);

    return Parameters.ApiData.SmartCardReaderTransmit.EfiStatus;
}

/**
    This function is the API function of SMART CARD READER PROTOCOL

    @param This
    @param ControlCode
    @param InBuffer
    @param InBufferLength
    @param OutBuffer
    @param OutBufferLength

    @retval
        EFI_STATUS

**/

EFI_STATUS
EFIAPI
USBSCardReaderAPIControl(
    IN EFI_SMART_CARD_READER_PROTOCOL *This,
    IN UINT32                         ControlCode,
    IN UINT8                          *InBuffer,
    IN UINTN                          *InBufferLength,
    OUT UINT8                         *OutBuffer,
    IN OUT UINTN                      *OutBufferLength
)
{
    return EFI_UNSUPPORTED;
}

/**
    This function is the API function of SMART CARD READER PROTOCOL

    @param This
    @param Attrib
    @param OutBuffer
    @param OutBufferLength

    @retval
        EFI_STATUS

**/

EFI_STATUS
EFIAPI
USBSCardReaderAPIGetAttrib(
    IN EFI_SMART_CARD_READER_PROTOCOL *This,
    IN UINT32                         Attrib,
    OUT UINT8                         *OutBuffer,
    IN OUT UINTN                      *OutBufferLength
)
{
    return EFI_UNSUPPORTED;
}


/**
    This function to get SMART CARD reader name

    @param ControllerHandle
    @param ReaderName
    @param ReaderNameLength

    @retval
        EFI_STATUS

**/
EFI_STATUS
GetSmartCardReaderName (
    EFI_HANDLE    ControllerHandle,
    CHAR16        *ReaderName,
    UINTN         *ReaderNameLength
)
{
    EFI_STATUS                 Status;
    EFI_USB_IO_PROTOCOL        *UsbIo;
    DEVGROUP_T                 *DevGroup;
    EFI_USB_STRING_DESCRIPTOR  *StringDesc;
    USBDEV_T                   *CCIDDev;

    // Get UsbIo Interface pointer
    Status = gBS->HandleProtocol ( ControllerHandle,
                                   &gEfiUsbIoProtocolGuid,
                                   &UsbIo );
    if( EFI_ERROR(Status)) {
        return Status;
    }

    CCIDDev = UsbIo2Dev(UsbIo);

    DevGroup = (DEVGROUP_T*)CCIDDev->node.parent->data;

    StringDesc = DevGroup->ProductStrDesc;

    if( StringDesc->DescriptorType == DESC_TYPE_STRING && StringDesc->Length > 2 ) {

        if( *ReaderNameLength < StringDesc->Length ) {
            Status = EFI_BUFFER_TOO_SMALL;
        } else {
           EfiCopyMem(ReaderName, StringDesc->String, StringDesc->Length-2 );
           ReaderName[((StringDesc->Length-2)>>1)] = L'\0';
        }

       *ReaderNameLength = StringDesc->Length;
    }

    return Status;
}

/**
    Search the linked list to find the ICC_DEVICE for the given slot

    @param 
        DEV_INFO        *fpDevInfo
        UINT8            Slot

    @retval 
        ICC_DEVICE Pointer

**/
ICC_DEVICE*
GetICCDevice(
    DEV_INFO        *fpDevInfo, 
    UINT8            Slot
)
{

    ICC_DEVICE        *fpICCDevice;
    DLINK           *dlink;

    dlink = fpDevInfo->ICCDeviceList.pHead;

    for ( ; dlink; dlink = dlink->pNext) {

        fpICCDevice = OUTTER(dlink, ICCDeviceLink, ICC_DEVICE);

        //
        // Slot # matches
        //
        if (fpICCDevice->Slot == Slot) {
            return fpICCDevice;
        }      
          
    }

    //
    // No Device Found. Return with NULL pointer
    //
    return NULL;

}

/**
    Search the linked list to find the CCID Device for the given ICC

    @param 
        ICC_DEVICE  *fpICCDevice

    @retval 
        DEV_INFO Pointer

**/
DEV_INFO*
GetCCIDDevice(
    ICC_DEVICE        *fpICCDevice
)
{


    DEV_INFO        *fpCCIDDevice;
    ICC_DEVICE      *fpTempICCDevice;
    DLINK           *dlink;
    UINTN           i;

    for (i = 0; i < MAX_DEVICES; i++ ){

        fpCCIDDevice = &(gUsbData->aDevInfoTable[i]);
        
        //
        // If not a CCID device continue the loop
        //
        if (!(fpCCIDDevice->bDeviceType == BIOS_DEV_TYPE_CCID)) continue;

        dlink = fpCCIDDevice->ICCDeviceList.pHead;

        for ( ; dlink; dlink = dlink->pNext) {
    
            fpTempICCDevice = OUTTER(dlink, ICCDeviceLink, ICC_DEVICE);

            //    
            // Match found?
            //
            if (fpICCDevice == fpTempICCDevice) {
                return fpCCIDDevice;
            }      
          
        }

    }

    //
    // Device Not Found, Return with NULL
    //
    return NULL;
}

/**
    Convert CCID return Value to EFI_STATUS

    @param 
        UINT8    bRetValue

         
    @retval EFI_STATUS Return the EFI Status

**/

EFI_STATUS
GetReturnValue(
    UINT8    bRetValue
)
{
    EFI_STATUS    Status;

    switch (bRetValue) {

            case USB_SUCCESS:
                Status = EFI_SUCCESS;
                break;

            default:
                Status = EFI_DEVICE_ERROR;
    }

    return Status;
}

/**
    Timer call-back routine that is used to monitor insertion/removal 
    of ICC(Smart card) in the smart card reader.

    @param 
        EFI_EVENT   Event,
        VOID        *Context

         
    @retval VOID

**/

VOID
EFIAPI
ICCOnTimer(
    EFI_EVENT   Event,
    VOID        *Context
)
{
    ICC_DEVICE        *fpICCDevice = NULL;
    DEV_INFO          *fpCCIDDevice = NULL; 
    UINTN              Lock;
    EFI_HANDLE        CCIDHandle;

    ATOMIC({Lock = gICCLock; gICCLock=1;});

    if( Lock ){
        USB_DEBUG(DEBUG_WARN, DEBUG_LEVEL_USBBUS, "ICCOnTimer::  is locked; return\n" );
        return;
    }

    do {

        ATOMIC({fpICCDevice = (ICC_DEVICE *)QueueGet(&gUsbData->ICCQueueCnnctDisc);});

        if (fpICCDevice == NULL) { 
            break;
        }

        fpCCIDDevice = GetCCIDDevice(fpICCDevice);

        if (!fpCCIDDevice) {
            continue;
        }

        CCIDHandle = (EFI_HANDLE) *(UINTN*)fpCCIDDevice->Handle;

        USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "Controller %lx\n", CCIDHandle);
        USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "Insert/Removal: fpDevInfo %lx fpICCDevice %lx fpICCDevice->ChildHandle %lx fpICCDevice->ConfiguredStatus %x\n", \
             fpCCIDDevice, fpICCDevice, fpICCDevice->ChildHandle, fpICCDevice->ConfiguredStatus );

#if CCID_USE_INTERRUPT_INSERTION_REMOVAL
        if (!fpICCDevice->ChildHandle && (fpICCDevice->ConfiguredStatus & ICCPRESENT)) {
            USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "InstallUSBCCID ....\n" );
            //
            // Install EFI interface to communicate with Smart Card/CCID
            //
            InstallUSBCCID(CCIDHandle, fpCCIDDevice, fpICCDevice);
        }

        if (fpICCDevice->ConfiguredStatus & CARDREMOVED) {
            USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "UnInstallUSBCCID ....\n" );
            //
            // Device Removed. Uninstall the existing Device
            //
             UnInstallUSBCCID (CCIDHandle, fpICCDevice->ChildHandle);
             // Indication to the SMI handler that the CARD has been uninstalled.
             // This Handle will be checked before device is inserted into the queue.
             fpICCDevice->ChildHandle = NULL;
             
        }
#else
        InstallUSBCCID(CCIDHandle, fpCCIDDevice, fpICCDevice);     
#endif

        // When card is removed, ScardChildHandle will still be valid so it will not do anything.
        // If the handle is valid, it means the protocol is already installed
        if (!fpICCDevice->SCardChildHandle) {
            InstallUSBSCardReaderProtocolOnSlot(CCIDHandle, fpCCIDDevice, fpICCDevice);
        }
    } while ( 1 );

    gICCLock = 0;

    return;
}

/**
    Verifies if usb CCID support can be installed on a device

        
    @param This pointer to driver binding protocol
    @param ControllerHandle controller handle to install driver on
    @param RemainingDevicePath pointer to device path

         
    @retval EFI_SUCCESS driver supports given controller
    @retval EFI_UNSUPPORTED driver doesn't support given controller

**/

EFI_STATUS
EFIAPI
SupportedUSBCCID(
    EFI_DRIVER_BINDING_PROTOCOL *This,
    EFI_HANDLE                  Controller,
    EFI_DEVICE_PATH_PROTOCOL    *DevicePath
)
{
    EFI_USB_INTERFACE_DESCRIPTOR    Desc;
    EFI_STATUS                      Status;
    EFI_USB_IO_PROTOCOL             *UsbIo;
    DEV_INFO                        *DevInfo;

    Status = gBS->OpenProtocol (Controller, &gEfiUsbIoProtocolGuid,
                                &UsbIo, This->DriverBindingHandle,
                                Controller, EFI_OPEN_PROTOCOL_GET_PROTOCOL 
                                );

    if (EFI_ERROR(Status)) {
        return Status;
    }

    DevInfo = FindDevStruc(Controller);

    if (DevInfo == NULL) {
        return EFI_UNSUPPORTED;
    }

    Status = UsbIo->UsbGetInterfaceDescriptor(UsbIo, &Desc);

    if(EFI_ERROR(Status)) {
        return EFI_UNSUPPORTED;
    }

    if ( Desc.InterfaceClass == BASE_CLASS_CCID_STORAGE &&
        Desc.InterfaceSubClass == SUB_CLASS_CCID &&
        Desc.InterfaceProtocol == PROTOCOL_CCID) {
        return EFI_SUCCESS;
    } else {
        return EFI_UNSUPPORTED;
    }

    return EFI_UNSUPPORTED;
}

/**
    Installs CCID protocol on a given handle

           
    @param This pointer to driver binding protocol
    @param ControllerHandle controller handle to install driver on
    @param RemainingDevicePath pointer to device path

         
    @retval EFI_SUCCESS driver started successfully
    @retval EFI_UNSUPPORTED driver didn't start

**/

EFI_STATUS
EFIAPI
StartUSBCCID(
    EFI_DRIVER_BINDING_PROTOCOL *This,
    EFI_HANDLE                  Controller,
    EFI_DEVICE_PATH_PROTOCOL    *DevicePath
)
{
    EFI_STATUS              Status;
    EFI_USB_IO_PROTOCOL     *UsbIo;

    USB_DEBUG(DEBUG_INFO, DEBUG_USBHC_LEVEL,
        "USB: installUSBCCID: starting...Controller : %lx\n", Controller);

    //
    // Open Protocols
    //
    //ALREADY_STARTED is a normal condition
    Status = gBS->OpenProtocol ( Controller,  &gEfiUsbIoProtocolGuid,
                                &UsbIo, This->DriverBindingHandle,
                                Controller, EFI_OPEN_PROTOCOL_BY_DRIVER );
    if( EFI_ERROR(Status)) {
        return Status;
    }

    {
        USBDEV_T* CCIDDev = UsbIo2Dev(UsbIo);
        HC_STRUC* HcData;
        UINT8     UsbStatus;

        ASSERT(CCIDDev);
        HcData = gUsbData->HcTable[CCIDDev->dev_info->bHCNumber - 1];
        UsbStatus = UsbSmiReConfigDevice(HcData, CCIDDev->dev_info);

        if(UsbStatus != USB_SUCCESS) {
            USB_DEBUG(DEBUG_ERROR, DEBUG_USBHC_LEVEL,
                "installUSBCCID: failed to Reconfigure CCID: %d\n", UsbStatus );

            return EFI_DEVICE_ERROR;
        }

        //
        // Install AMI CCID Presence protocol to identify SMART Card reader is detected
        //
        Status = gBS->InstallProtocolInterface(
                            &Controller,
                            &gAmiCCIDPresenceGuid,
                            EFI_NATIVE_INTERFACE,
                            NULL
                            );

        ASSERT_EFI_ERROR(Status);

        //
        // Setting up Timer to to detect ICC card insertion removal 
        //
        gCounterCCIDEnumTimer++;
        if (gEvICCEnumTimer == 0) {
            USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_USBBUS, "USBBUS: Start: setup timer callback %x\n", &ICCOnTimer );
            gBS->CreateEvent(EFI_EVENT_TIMER | EFI_EVENT_NOTIFY_SIGNAL,
                                    EFI_TPL_CALLBACK, ICCOnTimer,0,&gEvICCEnumTimer);
            gBS->SetTimer(gEvICCEnumTimer, TimerPeriodic, MILLISECOND);
        }

    }

    USB_DEBUG(DEBUG_INFO, DEBUG_USBHC_LEVEL,
        "USB: installCCID: done (%x).\n", Status);


    return Status;
}


/**
    Uninstalls CCID protocol on a given handle

           
    @param This pointer to driver binding protocol
    @param ControllerHandle controller handle to install driver on
    @param NumberOfChildren number of childs on this handle
    @param ChildHandleBuffer pointer to child handles array

         
    @retval EFI_SUCCESS driver stopped successfully
    @retval EFI_INVALID_PARAMETER invalid values passed for NumberOfChildren or
        ChildHandleBuffer
**/

EFI_STATUS
EFIAPI
StopUSBCCID(
    EFI_DRIVER_BINDING_PROTOCOL *This,
    EFI_HANDLE                  Controller,
    UINTN                       NumberOfChildren,
    EFI_HANDLE                  *Children
)
{

    EFI_STATUS Status;

    if (!NumberOfChildren) {

        // Closer the timer event when all CCID devices have been stopped
        if(--gCounterCCIDEnumTimer==0){

            gBS->SetTimer(gEvICCEnumTimer, TimerCancel, ONESECOND);
            gBS->CloseEvent(gEvICCEnumTimer);
            gEvICCEnumTimer=0;

            USB_DEBUG(DEBUG_INFO, DEBUG_USBHC_LEVEL, "USB: Timer Stopped\n");
        }

        //
        // uninstall gAmiCCIDPresenceGuid 
        //
        Status = pBS->UninstallProtocolInterface ( Controller, 
                                                &gAmiCCIDPresenceGuid, 
                                                NULL);

        Status = gBS->CloseProtocol (Controller, 
                                    &gEfiUsbIoProtocolGuid, 
                                    This->DriverBindingHandle, 
                                    Controller);
        ASSERT_EFI_ERROR(Status);
        
        USB_DEBUG(DEBUG_INFO, DEBUG_USBHC_LEVEL, "USB: StopUSBCCID: done : %r gCounterCCIDEnumTimer : %x\n", \
                 Status,  gCounterCCIDEnumTimer);
    }

    while (NumberOfChildren){

        //
        // Uninstall AMI_CCID_IO_PROTOCOL for each slot
        //
        Status = UnInstallUSBCCID (Controller, Children[NumberOfChildren -  1]);

        if( EFI_ERROR(Status) ) {
            Status = UnInstallSCardProtocol (Controller, Children[NumberOfChildren -  1], This->DriverBindingHandle);
        }
        ASSERT_EFI_ERROR(Status);

        NumberOfChildren--;

    }

    return Status;

}


/**
    Returns the Controller Name 

    @param 
        EFI_HANDLE Controller,
        EFI_HANDLE Child

         
    @retval CHAR16 Pointer to the buffer

**/

CHAR16*
UsbCCIDGetControllerName(
    EFI_HANDLE Controller,
    EFI_HANDLE Child
)
{
    return NULL;
}
 

/**
    CCID driver entry point

    @param 
        EFI_HANDLE  ImageHandle,
        EFI_HANDLE  ServiceHandle

         
    @retval EFI_STATUS Efi Status.

**/

EFI_STATUS
UsbCCIDInit(
    EFI_HANDLE  ImageHandle,
    EFI_HANDLE  ServiceHandle
)
{

    CcidBindingProtocol.DriverBindingHandle = ServiceHandle;
    CcidBindingProtocol.ImageHandle = ImageHandle;

    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "USB CCID binding:\n\t");
    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "SupportedUSBCCID:%x\n", &SupportedUSBCCID );
    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "\tInstallUSBCCID:%x\n", &StartUSBCCID);
    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "\tUninstallUSBCCID:%x\n", &StopUSBCCID );

    return gBS->InstallMultipleProtocolInterfaces(
        &CcidBindingProtocol.DriverBindingHandle,
        &gEfiDriverBindingProtocolGuid, &CcidBindingProtocol,
        &gEfiComponentName2ProtocolGuid, InitNamesProtocol(&Names,	//(EIP69250)
              L"AMI USB CCID driver", UsbCCIDGetControllerName),
        NULL);
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
