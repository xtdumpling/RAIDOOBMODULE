//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

/** @file  SerialIo.c
    Description: Driver for serial ports (both super io and pci).  
    Installs EFI_SERIAL_IO_PROTOCOL for a serial device.  
    Adds UART_DEVICE_PATH and UART_FLOW_CONTROL_DEVICE_PATH 
    nodes to its device path.

**/
//----------------------------------------------------------------------------

#include <Token.h>
#include <AmiDxeLib.h>
#include <AcpiRes.h>
#include <Protocol/PciIo.h>
#include <Pci.h>
#include <Protocol/TerminalAmiSerial.h>
#if EFI_SIO_PROTOCOL_SUPPORT 
#include <Protocol/SuperIo.h>
#else
#include <Protocol/AmiSio.h>
#endif
#include "SerialIo.h"
#include <Include/TerminalSetupVar.h>
#include "TerminalElink.h"

PCI_DEVICE_UART_CLOCK_WIDTH gPciDeviceUartClockAndWidth[] = {
     PCI_DEVICE_UART_CLOCK_WIDTH_LIST
    { 0xFFFF, 0xFFFF, 0xFF,0xFF }
};

//
// Serial Driver Defaults
//
UINT32 UartDefaultBaudRate              = UART_DEFAULT_BAUD_RATE;
EFI_PARITY_TYPE UartDefaultParity       = UART_DEFAULT_PARITY;
UINT8 UartDefaultDataBits               = UART_DEFAULT_DATA_BITS;
EFI_STOP_BITS_TYPE UartDefaultStopBits  = UART_DEFAULT_STOP_BITS;
UINT8 SerialIo_PciSerialSupport         = SERIALIO_PCI_SERIAL_SUPPORT;
UINT8  MaxFailuresAllowed               = MAX_FAILURES_ALLOWED;
UINT32 MaximumSerialWriteErrorCount     = MAXIMUM_SERIAL_WRITE_ERROR_COUNT; 
BOOLEAN SerialMouseDetection            = SERIAL_MOUSE_DETECTION;
BOOLEAN SerialWriteErrorCheck           = SERIAL_WRITE_ERROR_CHECK;

static UINT8                ComSelector = 0;

UART_DEVICE_PATH gExampleUartDevicePath = 
{
    {
        MESSAGING_DEVICE_PATH,
        MSG_UART_DP,
        sizeof (UART_DEVICE_PATH),
    },
    0,  //Reserved
    0,  //BaudRate
    0,  //DataBits
    0,  //Parity
    0   //StopBits
};
 
#define Flow_Control_Map_Hardware   BIT00
#define Flow_Control_Map_Software   BIT01

UART_FLOW_CONTROL_DEVICE_PATH gExampleUartFlowControlDevicePath = 
{
    {
        MESSAGING_DEVICE_PATH,
        MSG_VENDOR_DP,
        sizeof (UART_FLOW_CONTROL_DEVICE_PATH),
    },
    DEVICE_PATH_MESSAGING_UART_FLOW_CONTROL,
    1
};


//==========================================================================
// Driver binding protocol instance for SerialIo Driver
//==========================================================================
EFI_DRIVER_BINDING_PROTOCOL gSerialIoDriverBinding = {
    SerialIoSupported,
    SerialIoStart,
    SerialIoStop,
    0x10,
    NULL,
    NULL
    };

//==========================================================================
// Supported SerialIo driver table
//==========================================================================
EFI_STATUS
EFIAPI
SerialIoGetDriverName (
    IN EFI_COMPONENT_NAME_PROTOCOL  *This,
    IN CHAR8                        *Language,
    OUT CHAR16                      **DriverName
);

EFI_STATUS
EFIAPI
SerialIoGetControllerName (
    IN EFI_COMPONENT_NAME_PROTOCOL  *This,
    IN EFI_HANDLE                   ControllerHandle,
    IN EFI_HANDLE                   ChildHandle OPTIONAL,
    IN CHAR8                        *Language,
    OUT CHAR16                      **ControllerName
);

CHAR16 *gSerialIoDriverName = L"AMI Serial I/O Driver";
CHAR16 *gSerialIoName[]     = {L"COM1", L"COM2",
                               L"COM3", L"COM4",
                               L"COM5", L"COM6",
                               L"COM7", L"COM8",
                               L"COM9", L"COM10" };

//==========================================================================
// Driver component name instance for SerialIo Driver
//==========================================================================

#ifndef EFI_COMPONENT_NAME2_PROTOCOL_GUID //old Core
BOOLEAN LanguageCodesEqual(
    CONST CHAR8* LangCode1, CONST CHAR8* LangCode2
){
    return    LangCode1[0]==LangCode2[0]
           && LangCode1[1]==LangCode2[1]
           && LangCode1[2]==LangCode2[2];
}
#endif

#ifndef LANGUAGE_CODE_ENGLISH
CHAR8 SupportedLanguages[] = "eng";
#else
CHAR8 SupportedLanguages[] = LANGUAGE_CODE_ENGLISH;
#endif

EFI_COMPONENT_NAME_PROTOCOL gSerialIoCtlName = {
    SerialIoGetDriverName,
    SerialIoGetControllerName,
    0
};

/**
    Clears the buffer

    @param       void    *Buffer,
    @param       UINTN   Size

    @retval      None

**/

void
SerialZeroMemory (
    void                            *Buffer,
    UINTN                           Size
)
{
    UINT8   *Ptr;
    Ptr = Buffer;
    while (Size--) {
        *(Ptr++) = 0;
    }
}

EFI_STATUS
EFIAPI
SerialIoGetDriverName (
    IN EFI_COMPONENT_NAME_PROTOCOL  *This,
    IN CHAR8                        *Language,
    OUT CHAR16                      **DriverName
)
{
    if ( !Language || !DriverName ) {
        return EFI_INVALID_PARAMETER;
    }

    if ( !LanguageCodesEqual( Language, This->SupportedLanguages) ) {
        return EFI_UNSUPPORTED;
    }

    *DriverName = gSerialIoDriverName;
    return EFI_SUCCESS;
}

/**
Gets the name of the controller.  

    @param  This
    @param  Controller
    @param  ChildHandle OPTIONAL
    @param  Language
    @param  ControllerName

    @retval EFI_STATUS

**/

EFI_STATUS
EFIAPI
SerialIoGetControllerName(
    IN EFI_COMPONENT_NAME_PROTOCOL  *This,
    IN EFI_HANDLE                   Controller,
    IN EFI_HANDLE                   ChildHandle OPTIONAL,
    IN CHAR8                        *Language,
    OUT CHAR16                      **ControllerName
)
{
    EFI_STATUS              Status=EFI_SUCCESS;
    EFI_SERIAL_IO_PROTOCOL  *SerialIo=NULL;
    SERIAL_PARAMETERS       *Parameters=NULL;

    if ( !Language || !ControllerName || !Controller) {
        return EFI_INVALID_PARAMETER;
    }

    if ( !LanguageCodesEqual( Language, This->SupportedLanguages) ) {
        return EFI_UNSUPPORTED;
    }

    //
    // Find the last device node in the device path and return "Supported"
    // for mouse and/or keyboard depending on the SDL switches.
    //
    if (ChildHandle) {
        Status = pBS->OpenProtocol (ChildHandle,
                                      &gEfiSerialIoProtocolGuid,
                                      (VOID **)&SerialIo,
                                      &gSerialIoDriverBinding,
                                      ChildHandle,
                                      EFI_OPEN_PROTOCOL_GET_PROTOCOL
                                      );

        if ( EFI_ERROR( Status )) {
            return Status;
        }

        Parameters = (SERIAL_PARAMETERS *) SerialIo;
        *ControllerName = Parameters->ControllerName; 
        return EFI_SUCCESS;
    }

    return EFI_UNSUPPORTED;
}

/**
    Installs the driver binding protocol for this driver.
 
    @param ImageHandle,
    @param *SystemTable
    
    @retval EFI_STATUS
     
    Referrals: InitAmiLib InstallMultipleProtocolInterfaces 
    Notes:   
    Here is the control flow of this function:
      1. Initialize Ami Lib.
      2. Install Driver Binding Protocol
      3. Return EFI_SUCCESS.

**/

EFI_STATUS
EFIAPI
SerialIoEntryPoint (
    IN EFI_HANDLE           ImageHandle,
    IN EFI_SYSTEM_TABLE     *SystemTable
)
{
    EFI_STATUS  Status;

    InitAmiLib(ImageHandle,SystemTable);

    //
    // Iitiaize the ImageHandle and DriverBindingHandle
    //
    gSerialIoDriverBinding.DriverBindingHandle = NULL;
    gSerialIoDriverBinding.ImageHandle = ImageHandle;

    gSerialIoCtlName.SupportedLanguages = SupportedLanguages;
    
    Status = pBS->InstallMultipleProtocolInterfaces(
                    &gSerialIoDriverBinding.DriverBindingHandle,
                    &gEfiDriverBindingProtocolGuid, &gSerialIoDriverBinding,
#ifndef EFI_COMPONENT_NAME2_PROTOCOL_GUID //old Core
                   &gEfiComponentNameProtocolGuid, &gSerialIoCtlName,
#else
                    &gEfiComponentName2ProtocolGuid, &gSerialIoCtlName,
#endif
                    NULL);

    return Status;
}


/**

    This driver supports the given controller if 
    (1) the EISA UID and HID are values are correct
    or (2) the controller supports the PciIo Protocol and the 
    Revision Id field in the configuration space indicates that
    the device is a serial port.  

    @param    This
    @param    Controller
    @param    RemainingDevicePath

    @retval  EFI_STATUS

**/

EFI_STATUS
EFIAPI
SerialIoSupported (
    IN EFI_DRIVER_BINDING_PROTOCOL    *This,
    IN EFI_HANDLE                     Controller,
    IN EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath
)
{
    ACPI_HID_DEVICE_PATH     *acpiDP=NULL;
    EFI_STATUS               Status;
    EFI_PCI_IO_PROTOCOL      *PciIo=NULL;
    UINT8                    RevisionId[4];
    UINT64                   CommandReg=0; 
    UINT64                   Supports=0;
    ASLR_QWORD_ASD           *Resources=NULL;
    UINT8                    i;
    EFI_DEVICE_PATH_PROTOCOL *tmpdp=NULL;
    EFI_HANDLE               TempHandle;
    EFI_PCI_IO_PROTOCOL      *RootPciIo=NULL;
    EFI_DEVICE_PATH_PROTOCOL *TruncatedDevPath = NULL;
    EFI_DEVICE_PATH_PROTOCOL *TempDevPath = NULL;
#if EFI_SIO_PROTOCOL_SUPPORT
    EFI_SIO_PROTOCOL         *EfiSio=NULL;
#else
    AMI_SIO_PROTOCOL         *AmiSio=NULL;
#endif
    AMI_SERIAL_PROTOCOL      *AmiSerialProtocol=NULL;
    BOOLEAN                  Pci;
    BOOLEAN                  Mmio;
    UINT8                    MmioWidth;

    //
    // Check whether Ami Serial Protocol is installed.
    //
    Status = pBS->OpenProtocol (Controller,
                                &gAmiSerialProtocolGuid,
                                (VOID**)&AmiSerialProtocol,
                                This->DriverBindingHandle,
                                Controller,
                                EFI_OPEN_PROTOCOL_BY_DRIVER
                                );

    if (!EFI_ERROR(Status)) {

        Status = pBS->CloseProtocol (Controller,
                            &gAmiSerialProtocolGuid,
                            This->DriverBindingHandle,
                            Controller
                            );

        if (EFI_ERROR(Status)) {
            return EFI_UNSUPPORTED;
        }

        AmiSerialProtocol->CheckPciMmio(AmiSerialProtocol,&Pci,&Mmio,&MmioWidth);

        if((Pci == TRUE) && (SerialIo_PciSerialSupport != 1)) {
            return EFI_UNSUPPORTED;
        }
 
        return EFI_SUCCESS;
    }

    // Find the last device node in the device path and return "Supported"
    if(!EFI_ERROR(GetSerialIo_DP(This, Controller, &acpiDP,
                                    EFI_OPEN_PROTOCOL_BY_DRIVER,TRUE)) &&
        LookupHID(acpiDP->HID)) {
        if (RemainingDevicePath && 
            ((RemainingDevicePath->Type != MESSAGING_DEVICE_PATH) ||
             (RemainingDevicePath->SubType != MSG_UART_DP))) {
            return EFI_UNSUPPORTED;
        }

#if EFI_SIO_PROTOCOL_SUPPORT
        Status  = pBS->OpenProtocol(Controller,
                                    &gEfiSioProtocolGuid,
                                    (VOID **)&EfiSio,
                                    This->DriverBindingHandle,
                                    Controller,
                                    EFI_OPEN_PROTOCOL_BY_DRIVER
                                    );

        if ( EFI_ERROR( Status )) {
            goto Error;
        }

        Status = pBS->CloseProtocol (Controller,
                                     &gEfiSioProtocolGuid,
                                     This->DriverBindingHandle,
                                     Controller
                                     );

        if ( EFI_ERROR( Status )) {
            goto Error;
        }
#else
        Status  = pBS->OpenProtocol(Controller,
                                &gEfiAmiSioProtocolGuid,
                                (VOID **)&AmiSio,
                                This->DriverBindingHandle,
                                Controller,
                                EFI_OPEN_PROTOCOL_BY_DRIVER);

        if ( EFI_ERROR( Status )) {
            goto Error;
        }

        Status = pBS->CloseProtocol (Controller,
                                     &gEfiAmiSioProtocolGuid,
                                     This->DriverBindingHandle,
                                     Controller
                                     );

        if ( EFI_ERROR( Status )) {
            goto Error;
        }
#endif
        return EFI_SUCCESS;

    } else if(SerialIo_PciSerialSupport == 1){
        Status = pBS->OpenProtocol (Controller,
                                    &gEfiDevicePathProtocolGuid,
                                    (VOID**)&tmpdp,
                                    This->DriverBindingHandle,
                                    Controller,
                                    EFI_OPEN_PROTOCOL_GET_PROTOCOL
                                    );

        if ( EFI_ERROR( Status )) {
            goto Error;
        }

        Status = pBS->OpenProtocol (Controller,
                                    &gEfiPciIoProtocolGuid,
                                    (VOID**)&PciIo,
                                    This->DriverBindingHandle,
                                    Controller,
                                    EFI_OPEN_PROTOCOL_BY_DRIVER
                                    );
        if ( EFI_ERROR( Status )) {
            goto Error;
        }

        PciIo->Pci.Read (PciIo,
                        EfiPciIoWidthUint32,
                        PCI_REV_ID_OFFSET,
                        1,
                        &RevisionId
                        );

        Status = pBS->CloseProtocol (Controller,
                                     &gEfiPciIoProtocolGuid,
                                     This->DriverBindingHandle,
                                     Controller
                                     );
        if ( EFI_ERROR( Status )) {
            goto Error;
        }

        if ((RevisionId[3] == PCI_CL_COMM) &&
            ((RevisionId[2] == PCI_CL_COMM_CSL_SERIAL) || (RevisionId[2] == PCI_CL_COMM_CSL_OTHER))) {

            Supports = 1;

            for (i = 0; i < PCI_MAX_BAR_NO; i++) {
                Status = PciIo->GetBarAttributes (
                                                PciIo,
                                                i,
                                                &Supports,
                                                (VOID**)&Resources
                                                );

                //
                // Find the Serial device Resource type. Based on that Enable the IO and Memory 
                // Also check whether the device supports both IO and MMIO. If supports both, give priority to MMIO.
                //
                if (Resources->Type == ASLRV_SPC_TYPE_IO) {
                    CommandReg |= EFI_PCI_IO_ATTRIBUTE_IO;
                    pBS->FreePool(Resources);
                } else if(Resources->Type == ASLRV_SPC_TYPE_MEM) {
                    CommandReg |= EFI_PCI_IO_ATTRIBUTE_MEMORY;
                    pBS->FreePool(Resources);
                }
            }

            if ((CommandReg & EFI_PCI_IO_ATTRIBUTE_MEMORY) && (CommandReg & EFI_PCI_IO_ATTRIBUTE_IO)) {
                CommandReg = EFI_PCI_IO_ATTRIBUTE_MEMORY;
            }

            if(CommandReg != 0) {
                Status = PciIo->Attributes (PciIo,
                                        EfiPciIoAttributeOperationEnable,
                                        CommandReg,
                                        NULL);

                TruncatedDevPath=tmpdp;
                do {
                    TruncatedDevPath = DPCut(TruncatedDevPath);
                    if(TruncatedDevPath == NULL ) {
                        break;
                    }

                    //
                    // Locate handle using device path
                    //
                    TempHandle= NULL;
                    TempDevPath=TruncatedDevPath;
                    Status = pBS->LocateDevicePath(
                                                &gEfiPciIoProtocolGuid,
                                                &TempDevPath,
                                                &TempHandle);

                    if(Status == EFI_SUCCESS) {

                        RootPciIo=NULL;
                        Status = pBS->HandleProtocol (
                                                    TempHandle,
                                                    &gEfiPciIoProtocolGuid,
                                                    (VOID**)&RootPciIo); // Get Device path protocol

                        if(Status == EFI_SUCCESS) {

                            Status = RootPciIo->Attributes (RootPciIo,
                                                        EfiPciIoAttributeOperationEnable,
                                                        CommandReg,
                                                        NULL);   

                        }

                    }

                }while(TruncatedDevPath != NULL);

            }

            if (RemainingDevicePath && 
                ((RemainingDevicePath->Type != MESSAGING_DEVICE_PATH) ||
                 (RemainingDevicePath->SubType != MSG_UART_DP))) {
                return EFI_UNSUPPORTED;
            }
            return EFI_SUCCESS; 
        }
    }

Error:
    if( Status == EFI_ALREADY_STARTED || Status == EFI_ACCESS_DENIED ) {
        return Status;
    }
    return EFI_UNSUPPORTED;

}

/**
    Starts the driver.
    @param           *This,
    @param            Controller,
    @param            *RemainingDevicePath

    @retval        EFI_STATUS

**/

EFI_STATUS
EFIAPI
SerialIoStart (
    IN EFI_DRIVER_BINDING_PROTOCOL    *This,
    IN EFI_HANDLE                     Controller,
    IN EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath
)
{
    EFI_STATUS                  Status=EFI_SUCCESS;
    SERIAL_PARAMETERS           *Parameters=NULL;
#if EFI_SIO_PROTOCOL_SUPPORT
    ACPI_RESOURCE_HEADER_PTR    ResourceList;
    EFI_SIO_PROTOCOL            *EfiSio=NULL;
    ACPI_SMALL_RESOURCE_HEADER  *Hdr=NULL;
#else
    T_ITEM_LIST                 *ResList=NULL;
    AMI_SIO_PROTOCOL            *AmiSio=NULL;
    ASLRF_S_HDR                 *hdr=NULL;
#endif
    ACPI_HID_DEVICE_PATH        *acpiDP=NULL;
    UART_DEVICE_PATH            *UartDevicePath=NULL;
    EFI_DEVICE_PATH_PROTOCOL    *tmpdp=NULL;
    UINTN                       i=0;
    EFI_PCI_IO_PROTOCOL         *PciIo=NULL;
    UINT64                      Supports=0;
    ASLR_QWORD_ASD              *Resources=NULL;
    UINTN                       Segment=0;
    UINTN                       Bus=0;
    UINTN                       Device=0;
    UINTN                       Function=0;
    EFI_DEVICE_PATH_PROTOCOL    *TempDevPathProtocol;
    AMI_SERIAL_PROTOCOL         *AmiSerialProtocol=NULL;
    BOOLEAN                     Pci;
    UINT8                       Port=0;
    UINT8                       AmiSerialUid;
    UINT32                      PciVidDid;
    UINT16                      PciVendorId;
    UINT16                      PciDeviceId;
    UINT64                      PciAttributes;
    ACPI_HID_DEVICE_PATH        DummySioDevicePath = {
                                {ACPI_DEVICE_PATH,
                                ACPI_DP,
                                sizeof(ACPI_HID_DEVICE_PATH)},
                                EISA_PNP_ID(0x501),
                                0};

    AMI_SERIAL_VENDOR_DEVICE_PATH AmiSerialVendorDevicePath = {
                                    {{HARDWARE_DEVICE_PATH,
                                    HW_VENDOR_DP,
                                    sizeof(AMI_SERIAL_VENDOR_DEVICE_PATH)},
                                    AMI_SERIAL_VENDOR_DEVICE_PATH_GUID},
                                    0,
                                    0,
                                    0,
                                    0};

//------------------------------------
 
    Parameters = MallocZ(sizeof(SERIAL_PARAMETERS));
    if ( !Parameters ) {
        return EFI_OUT_OF_RESOURCES;
    }

    Status = pBS->OpenProtocol (Controller,
                                &gAmiSerialProtocolGuid,
                               (VOID**) &AmiSerialProtocol,
                                This->DriverBindingHandle,
                                Controller,
                                EFI_OPEN_PROTOCOL_BY_DRIVER
                                );

    if (!EFI_ERROR(Status)) {
        //
        // Get the Base address and also check whether its a PCI device.
        //
        AmiSerialProtocol->GetBaseAddress(AmiSerialProtocol,&(Parameters->BaseAddress));
        AmiSerialProtocol->CheckPciMmio(AmiSerialProtocol,&Pci,&(Parameters->MMIODevice),&(Parameters->MmioWidth));
        AmiSerialProtocol->GetUID(AmiSerialProtocol, &AmiSerialUid);
        AmiSerialProtocol->GetUartClock(AmiSerialProtocol, &(Parameters->UartClock));
        if(Pci) { 
            AmiSerialProtocol->GetPciLocation(AmiSerialProtocol,&Bus,&Device,&Function,&Port);
            Parameters->ControllerName = (CHAR16 *) MallocZ(75);
            Swprintf(Parameters->ControllerName, L"COM (Pci Dev%X, Func%X, Port%X)",
                                                        Device, Function, Port);
            //
            // Install AMI serial Vendor device path.
            //
            AmiSerialVendorDevicePath.Bus      = (UINT8)Bus;
            AmiSerialVendorDevicePath.Device   = (UINT8)Device;
            AmiSerialVendorDevicePath.Function = (UINT8)Function;
            AmiSerialVendorDevicePath.Port     = Port;
            Parameters->ParentDevicePath = DPAddNode(NULL,NULL);
            Parameters->ParentDevicePath = DPAddNode(Parameters->ParentDevicePath,
                                            (EFI_DEVICE_PATH_PROTOCOL *)&AmiSerialVendorDevicePath);
            Parameters->IsPciAmiSerial = TRUE;
        } else {
            Parameters->ControllerName = gSerialIoName[ComSelector];
            ComSelector++;
            DummySioDevicePath.UID = AmiSerialUid;

            //
            // Install Dummy ACPI device path for non-pci device.
            //
            Parameters->ParentDevicePath = DPAddNode(NULL,NULL);
            Parameters->ParentDevicePath = DPAddNode(Parameters->ParentDevicePath,
                                             (EFI_DEVICE_PATH_PROTOCOL *)&DummySioDevicePath);

        }

        Parameters->AmiSerialDetected = TRUE;

        goto SerialTest;
    }

    Status = pBS->OpenProtocol(Controller, &gEfiDevicePathProtocolGuid,
                                (VOID**)&tmpdp, This->DriverBindingHandle,
                                Controller, EFI_OPEN_PROTOCOL_GET_PROTOCOL);
    if ( EFI_ERROR( Status )) {
        goto Error;
    }

    //1. Make a copy of the device path of the UART Parent 
    //- it has to be ACPI device path
    Parameters->ParentDevicePath = DPCopy(tmpdp);

#if EFI_SIO_PROTOCOL_SUPPORT
    //2. Open EfiSio Protocol BY_DRIVER.
    //Don't close it.
    Status  = pBS->OpenProtocol(Controller,
                                &gEfiSioProtocolGuid,
                                (VOID **)&EfiSio,
                                This->DriverBindingHandle,
                                Controller,
                                EFI_OPEN_PROTOCOL_BY_DRIVER);

    if(!EFI_ERROR(Status)) {

        ResourceList.SmallHeader = NULL;
        Status = EfiSio->GetResources(EfiSio,
                                      &ResourceList);

        if ( EFI_ERROR( Status )) {
            goto Error;
        }

        if (ResourceList.SmallHeader){
            for(Hdr=ResourceList.SmallHeader; (Hdr->Byte)!=ASLV_END_TAG_HDR; Hdr+=((sizeof(ACPI_SMALL_RESOURCE_HEADER))+Hdr->Bits.Length)) {
                switch(Hdr->Bits.Name) {
                    case ASLV_RT_FixedIO:
                        Parameters->BaseAddress = ((ASLR_FixedIO*)Hdr)->_BAS;
                        break;
                    case ASLV_RT_IO: 
                        Parameters->BaseAddress = ((ASLR_IO*)Hdr)->_MIN;
                        break;
                }
             }
         }
#else
    //2. Open AmiSio Protocol BY_DRIVER.
    //Don't close it.
    Status  = pBS->OpenProtocol(Controller,
                                &gEfiAmiSioProtocolGuid,
                                (VOID **)&AmiSio,
                                This->DriverBindingHandle,
                                Controller,
                                EFI_OPEN_PROTOCOL_BY_DRIVER);
    if(!EFI_ERROR(Status)) {

        Status = AmiSio->CurrentRes(AmiSio, 0, &ResList);
        if ( EFI_ERROR( Status )) {
            goto Error;
        }

        if (ResList){
            for(i = 0; i < ResList->ItemCount; i++) {
                //We only need Base Addr 
                hdr = (ASLRF_S_HDR*)ResList->Items[i];
                switch(hdr->Name){
                    case ASLV_RT_FixedIO: 
                        Parameters->BaseAddress = ((ASLR_FixedIO*)hdr)->_BAS;
                        break;
                    case ASLV_RT_IO: 
                        Parameters->BaseAddress = ((ASLR_IO*)hdr)->_MIN;
                        break;
                }
            }
        }
#endif

        if(!EFI_ERROR(GetSerialIo_DP(&gSerialIoDriverBinding, Controller,
                                    &acpiDP, EFI_OPEN_PROTOCOL_GET_PROTOCOL,
                                    TRUE)) &&
            LookupHID(acpiDP->HID) )
        {
        #ifdef UART_INPUT_CLOCK
            Parameters->UartClock = UART_INPUT_CLOCK;
        #endif
            Parameters->ControllerName = gSerialIoName[ComSelector];
            ComSelector++;
        }
    } else {
        //
        // Check for PCI serial Device.
        //
        Status = pBS->OpenProtocol (
                      Controller,
                      &gEfiPciIoProtocolGuid,
                      (VOID **)&PciIo,
                      This->DriverBindingHandle,
                      Controller,
                      EFI_OPEN_PROTOCOL_BY_DRIVER
                      );
        if (EFI_ERROR (Status)) {
            Parameters->PciIo = NULL;
            goto Error;
        }

        Parameters->PciIo = PciIo;

        Status = PciIo->GetLocation(PciIo, &Segment, &Bus,
                                    &Device, &Function);
        if (EFI_ERROR(Status)) {
            Parameters->PciIo = NULL;
            goto Error;
        }

        Status = PciIo->Pci.Read (
                               PciIo,
                               EfiPciIoWidthUint32,
                               PCI_VID,
                               1,
                               &PciVidDid
                               );
        if (EFI_ERROR (Status)) {
            Parameters->PciIo = NULL;
            goto Error;
        }

        PciVendorId = (UINT16)(PciVidDid & 0xffff);
        PciDeviceId = (UINT16)(PciVidDid >> 16);
        
        Parameters->ControllerName = (CHAR16 *) MallocZ(50);
        Swprintf(Parameters->ControllerName, L"COM (Pci Dev%X, Func%X)",
                                                        Device, Function);

        Supports = 1;

        Status = PciIo->Attributes (PciIo,
                                    EfiPciIoAttributeOperationGet,
                                    0,
                                    &PciAttributes);
        if (EFI_ERROR(Status)) {
            Parameters->PciIo = NULL;
            goto Error;
        }

        Status = EFI_DEVICE_ERROR; // UEFI 2.2 spec -- return proper error coce
        for (i = 0; i < PCI_MAX_BAR_NO; i++) {

            Status = Parameters->PciIo->GetBarAttributes (
                                                        PciIo,
                                                        (UINT8) i,
                                                        &Supports,
                                                        (VOID**)&Resources
                                                       );
            //
            // Select the BAR index based on PCI attributes (IO/MMIO)
            //
            if ((Resources->Type == ASLRV_SPC_TYPE_IO) && (PciAttributes & EFI_PCI_IO_ATTRIBUTE_IO)) {
                Parameters->BarIndex = (UINT8)i;
                Status = EFI_SUCCESS;
                pBS->FreePool(Resources);
                break;
            }
            if ((Resources->Type == ASLRV_SPC_TYPE_MEM) && (PciAttributes & EFI_PCI_IO_ATTRIBUTE_MEMORY)) {
                Parameters->MMIODevice=TRUE;
                Parameters->BarIndex = (UINT8)i;
                Status = EFI_SUCCESS;
                pBS->FreePool(Resources);
                break;
            }
            pBS->FreePool(Resources);
        }
        if (EFI_ERROR(Status)) {
            Parameters->PciIo = NULL;
            goto Error;
        }

        for( i = 0; gPciDeviceUartClockAndWidth[i].VendorID != 0xFFFF ; i++ ) {
            if( (gPciDeviceUartClockAndWidth[i].VendorID == PciVendorId) && (gPciDeviceUartClockAndWidth[i].DeviceID == PciDeviceId) ) {
                Parameters->UartClock = (UINTN)gPciDeviceUartClockAndWidth[i].UartClock;
                Parameters->MmioWidth = gPciDeviceUartClockAndWidth[i].MmioWidth;
                break;
            }
        }

    }

SerialTest:
    //
    // Check for a valid Baseaddress.
    //
    if ( (!Parameters->PciIo) && (Parameters->BaseAddress == 0) ) {
        goto Error;
    }

    //
    // Check whether a port is there
    //
    SerialWritePort(Parameters, SERIAL_REGISTER_SCR, 0xaa);
    if ( SerialReadPort( Parameters, SERIAL_REGISTER_SCR ) != 0xaa ) {
        goto Error;
    }
    
    // Check for the LoopBack device or Serial device in the Port
    if(CheckForLoopbackDevice(Parameters)){
        goto Error;
    }

    SetUartType(Parameters);
    if ( !Parameters->UartClock ) {
        // Set the default value((24000000/13)MHz input clock) if the Uart Clock is not set.
        Parameters->UartClock=1843200;
    }
    
    if ( !Parameters->MmioWidth ) {
        // Set MMIO Width to byte width if no MMIO Width is set
        Parameters->MmioWidth=1;
    }


    Parameters->WaitForXon = FALSE; // Allow input flow until halted.
    Parameters->ErrorCount = 0;
    Parameters->Fifohead = 0;
    Parameters->Fifotail = 0;
    Parameters->DataCount = 0;
    SerialZeroMemory(&Parameters->LoopbackBuffer[0],UART_MAX_RECEIVE_FIFO_DEPTH);
    //
    // Initialize Serial I/O
    //
    Parameters->SerialIo.Revision       = EFI_SERIAL_IO_PROTOCOL_REVISION;
    Parameters->SerialIo.Reset          = EfiSerialReset;
    Parameters->SerialIo.SetAttributes  = EfiSerialSetAttributes;
    Parameters->SerialIo.SetControl     = EfiSerialSetControl;
    Parameters->SerialIo.GetControl     = EfiSerialGetControl;
    Parameters->SerialIo.Write          = EfiSerialWrite;
    Parameters->SerialIo.Read           = EfiSerialRead;
    Parameters->SerialIo.Mode           = &(Parameters->SerialMode);
    //
    //  Initialize the SerialIo driver Signature 
    //

    Parameters->Signature = EFI_SERIAL_DRIVER_SIGNATURE;
    Parameters->UartDevicePath = gExampleUartDevicePath;

    Status = Parameters->SerialIo.SetAttributes(&Parameters->SerialIo,
                    0,              //Parameters->SerialMode.BaudRate
                    0,              //Parameters->SerialMode.ReceiveFifoDepth
                    0,              //Parameters->SerialMode.Timeout
                    DefaultParity,  //Parameters->SerialMode.Parity
                    0,              //Parameters->SerialMode.DataBits
                    DefaultStopBits); //Parameters->SerialMode.StopBits

    if ( EFI_ERROR( Status )) {
        goto Error;
    }

    

    if (RemainingDevicePath) {
        UartDevicePath = (UART_DEVICE_PATH *)RemainingDevicePath;
        UartDevicePath->BaudRate = Parameters->SerialMode.BaudRate;
        UartDevicePath->Parity = Parameters->SerialMode.Parity;
        UartDevicePath->DataBits = Parameters->SerialMode.DataBits;
        UartDevicePath->StopBits = Parameters->SerialMode.StopBits;
    }
    //
    //Create This UART Device Device Path
    //
    TempDevPathProtocol = 
        DPAddNode( (EFI_DEVICE_PATH_PROTOCOL*) Parameters->ParentDevicePath,
                (EFI_DEVICE_PATH_PROTOCOL*) &Parameters->UartDevicePath );

    Parameters->UartFlowControlDevicePath =
                                        gExampleUartFlowControlDevicePath;

    Parameters->DevPathProtocol = 
        DPAddNode( (EFI_DEVICE_PATH_PROTOCOL*) TempDevPathProtocol,
                (EFI_DEVICE_PATH_PROTOCOL*) &Parameters->UartFlowControlDevicePath );

    pBS->FreePool(TempDevPathProtocol);

    //
    // Install SerialIO protocol interfaces for the serial device.
    //
    Status = pBS->InstallMultipleProtocolInterfaces (
                    &Parameters->Handle,
                    &gEfiSerialIoProtocolGuid, &Parameters->SerialIo,
                    &gEfiDevicePathProtocolGuid, Parameters->DevPathProtocol,
                    NULL);

    if (EFI_ERROR(Status)) {
        goto Error;
    }

    //
    // Open the protocol for the created child handle.
    //
    if (Parameters->AmiSerialDetected) {
        //
        // Open the Ami Serial protocol as child for the Serial IO.
        //
        Status=pBS->OpenProtocol(Controller,
                                &gAmiSerialProtocolGuid,
                                (VOID**)&AmiSerialProtocol,
                                This->DriverBindingHandle,
                                Parameters->Handle,
                                EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER);

    } else if (!Parameters->PciIo) {
#if EFI_SIO_PROTOCOL_SUPPORT
        Status=pBS->OpenProtocol(Controller,
                                 &gEfiSioProtocolGuid,
                                 (VOID**)&Parameters->EfiSio,
                                 This->DriverBindingHandle,
                                 Parameters->Handle,
                                 EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER);
#else
        Status=pBS->OpenProtocol(Controller,
                                &gEfiAmiSioProtocolGuid,
                                (VOID**)&Parameters->AmiSio,
                                This->DriverBindingHandle,
                                Parameters->Handle,
                                EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER);
#endif
    } else {

        Status = pBS->OpenProtocol (Controller,
                                    &gEfiPciIoProtocolGuid,
                                    (VOID**)&PciIo,
                                    This->DriverBindingHandle,
                                    Parameters->Handle,
                                    EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER
                                    );
    }

    if (EFI_ERROR(Status)) {
        goto Error;
    }

    Parameters->Started=TRUE;

    Parameters->LineStatusRegOverrunErrorBit = FALSE; 

    //
    // Create a Event to check the Serial Port status.
    //
    Status = pBS->CreateEvent(
        EVT_TIMER | EFI_EVENT_NOTIFY_SIGNAL,
        TPL_NOTIFY,
        CheckThreBit,
        (VOID*)Parameters,
        &Parameters->SerialErrorReEnableEvent
    );
    ASSERT_EFI_ERROR(Status);

    Parameters->TimerEventActive=FALSE;

    return EFI_SUCCESS;

Error:

#if EFI_SIO_PROTOCOL_SUPPORT
    pBS->CloseProtocol(Controller,
                       &gEfiSioProtocolGuid,
                       This->DriverBindingHandle,
                       Controller
                       );
#else
    pBS->CloseProtocol(Controller,
                       &gEfiAmiSioProtocolGuid,
                       This->DriverBindingHandle,
                       Controller
                       );
#endif
    pBS->CloseProtocol(Controller,
                       &gEfiPciIoProtocolGuid,
                       This->DriverBindingHandle,
                       Controller
                       );

    pBS->CloseProtocol(Controller,
                       &gAmiSerialProtocolGuid,
                       This->DriverBindingHandle,
                       Controller
                       );

    if ( Parameters ) {
        pBS->FreePool(Parameters);
    }

    return EFI_DEVICE_ERROR;

}

/**
    Stops the driver.

    @param  This,
    @param  Controller,
    @param  NumberOfChildren,
    @param  ChildHandleBuffer

    @retval EFI_STATUS

**/ 
EFI_STATUS
EFIAPI
SerialIoStop (
    IN  EFI_DRIVER_BINDING_PROTOCOL     *This,
    IN  EFI_HANDLE                      Controller,
    IN  UINTN                           NumberOfChildren,
    IN  EFI_HANDLE                      *ChildHandleBuffer
)
{
    EFI_STATUS              Status = EFI_SUCCESS;
    EFI_SERIAL_IO_PROTOCOL  *SerialIo = NULL;
    SERIAL_PARAMETERS       *Parameters = NULL;
    
    if (!NumberOfChildren) {
#if EFI_SIO_PROTOCOL_SUPPORT
        Status = pBS->CloseProtocol(
                        Controller,
                        &gEfiSioProtocolGuid,
                        This->DriverBindingHandle,
                        Controller
                        );
#else
        Status = pBS->CloseProtocol(
                        Controller,
                        &gEfiAmiSioProtocolGuid,
                        This->DriverBindingHandle,
                        Controller
                        );
#endif
        if (EFI_ERROR(Status)) {
            Status = pBS->CloseProtocol(
                        Controller,
                        &gEfiPciIoProtocolGuid,
                        This->DriverBindingHandle,
                        Controller
                        );
            if ( EFI_ERROR( Status )) {
                Status = pBS->CloseProtocol(
                                Controller,
                                &gAmiSerialProtocolGuid,
                                This->DriverBindingHandle,
                                Controller
                                );
                if ( EFI_ERROR( Status )) {
                    return EFI_DEVICE_ERROR;
                }
            }
        }
        return EFI_SUCCESS;
    }
    //
    // Uninstall all protocols installed installed in DriverBindingStart
    //

    //
    // open the Serial Io protocol
    //
    Status = pBS->OpenProtocol (
                    *ChildHandleBuffer,
                    &gEfiSerialIoProtocolGuid,
                    (VOID **)&SerialIo,
                    This->DriverBindingHandle,
                    *ChildHandleBuffer,
                    EFI_OPEN_PROTOCOL_GET_PROTOCOL
                    );

    if ( EFI_ERROR( Status )) {
        return EFI_DEVICE_ERROR;
    }

    // Find the pointer to the parent structure
    //  in this case, SerialIo is the first element in the data structure
    Parameters = (SERIAL_PARAMETERS *) SerialIo;

    if(Parameters->AmiSerialDetected) {
        Status = pBS->CloseProtocol(Controller,
                                    &gAmiSerialProtocolGuid,
                                    This->DriverBindingHandle,
                                    Parameters->Handle
                                    );
    } else if (Parameters->PciIo) {
        Status = pBS->CloseProtocol(Controller,
                                    &gEfiPciIoProtocolGuid,
                                    This->DriverBindingHandle,
                                    Parameters->Handle
                                    );
    } else {
#if EFI_SIO_PROTOCOL_SUPPORT
        Status = pBS->CloseProtocol(Controller,
                                    &gEfiSioProtocolGuid,
                                    This->DriverBindingHandle,
                                    Parameters->Handle
                                    );
#else
        Status = pBS->CloseProtocol(Controller,
                                    &gEfiAmiSioProtocolGuid,
                                    This->DriverBindingHandle,
                                    Parameters->Handle
                                    );
#endif
    }

    if ( EFI_ERROR( Status )) {
        goto Error;
    }

    //
    // Uninstall the protocol interface
    //
    Status = pBS->UninstallMultipleProtocolInterfaces (
                                                Parameters->Handle,
                                                &gEfiSerialIoProtocolGuid,
                                                &Parameters->SerialIo,
                                                &gEfiDevicePathProtocolGuid,
                                                Parameters->DevPathProtocol,
                                                NULL);

Error:

    pBS->FreePool(Parameters);

    if ( EFI_ERROR( Status )) {
        return EFI_DEVICE_ERROR;
    }

    return EFI_SUCCESS;
}

//
// PROTOCOL INTERFACE FUNCTIONS
//

/**
    Resets the serial device.

    @param    This

    @retval  EFI_STATUS

**/

EFI_STATUS
EFIAPI
EfiSerialReset (
    IN EFI_SERIAL_IO_PROTOCOL  *This
)
{
    EFI_STATUS              Status;
    SERIAL_PARAMETERS       *Parameters = (SERIAL_PARAMETERS *)This;
    UINT8                   Temp;

    //
    // Divisor Access Latch must be 0 to program UART.
    //
    Temp = SerialReadPort(Parameters, SERIAL_REGISTER_LCR);
    Temp &= ~DLAB;
    SerialWritePort(Parameters, SERIAL_REGISTER_LCR, Temp);
    
    //
    // Turn off interrupts via Interrupt Enable Register.
    //
    Temp = SerialReadPort(Parameters, SERIAL_REGISTER_IER);
    Temp &= ~(RAVIE | THEIE | RIE | MIE);
    SerialWritePort(Parameters, SERIAL_REGISTER_IER, Temp);

    //
    // Reset the FIFO.
    //
    Temp = RESETRF | RESETTF;
    SerialWritePort(Parameters, SERIAL_REGISTER_FCR, Temp);

    EnableFifoBuffers(Parameters, UART_MAX_RECEIVE_FIFO_DEPTH);
    //
    // Use Modem Control Register to disable interrupts and loopback.
    //
    Temp = SerialReadPort(Parameters, SERIAL_REGISTER_MCR);
    Temp &= ~(OUT1 | OUT2 | LME);
    SerialWritePort(Parameters, SERIAL_REGISTER_MCR, Temp);

    //
    // Clear the scratch pad register
    //
    SerialWritePort(Parameters, SERIAL_REGISTER_SCR, 0);

    //
    // Go set the current attributes
    //
    Status = This->SetAttributes(
        This,
        This->Mode->BaudRate,
        This->Mode->ReceiveFifoDepth,
        This->Mode->Timeout,
        This->Mode->Parity,
        (UINT8)This->Mode->DataBits,
        This->Mode->StopBits);

    if (EFI_ERROR(Status)) {
        return EFI_DEVICE_ERROR;
    }

    //
    // Go set the current control bits
    //
    Status = This->SetControl(
        This,
        This->Mode->ControlMask);

    if (EFI_ERROR(Status)) {
        return EFI_DEVICE_ERROR;
    }

    Parameters->WaitForXon = FALSE; // Allow input flow until halted.
    Parameters->ErrorCount = 0;
    Parameters->Fifohead = 0;
    Parameters->Fifotail = 0;
    Parameters->DataCount = 0;
    SerialZeroMemory(&Parameters->LoopbackBuffer[0],UART_MAX_RECEIVE_FIFO_DEPTH);

    Temp = SerialReadPort(Parameters, SERIAL_REGISTER_MCR);
  	Temp |= DTRC;  // Tell Hyperterminal that it can write to us now.
    SerialWritePort( Parameters, SERIAL_REGISTER_MCR, Temp );
    //
    // Empty input
    //
    SerialReadPort( Parameters, SERIAL_REGISTER_RBR );
    //
    // Device reset is complete
    //
    return EFI_SUCCESS;
}

/**
    Protocol interface to set the serial device attributes.
    Sets the baud rate, receive FIFO depth, transmit/receice time out,
    parity, data buts, and stop bits on a serial device.
	
    @param      This
    @param      BaudRate
    @param      ReceiveFifoDepth
    @param      Timeout
    @param      Parity
    @param      DataBits
    @param      StopBits

    @retval     EFI_STATUS
    Referrals: SerialReadPort, SerialWritePort, AppendDevicePathNode
**/

EFI_STATUS
EFIAPI
EfiSerialSetAttributes (
    IN EFI_SERIAL_IO_PROTOCOL           *This,
    IN UINT64                           BaudRate,
    IN UINT32                           ReceiveFifoDepth,
    IN UINT32                           Timeout,
    IN EFI_PARITY_TYPE                  Parity,
    IN UINT8                            DataBits,
    IN EFI_STOP_BITS_TYPE               StopBits
)
{
    EFI_STATUS                  Status;
    SERIAL_PARAMETERS           *Parameters = (SERIAL_PARAMETERS *)This;
    UINT32                      Divisor;
    UINTN                       Remainder;
    UINT8                       Temp;
    EFI_DEVICE_PATH_PROTOCOL    *NewDevicePath;
    UINTN                       UartInputClock;
    static UINT32               BaudRateTable[] = { 50, 75, 110, 134, 
                                                    150, 300, 600, 1200,
                                                    1800, 2000, 2400, 3600,
                                                    4800, 7200, 9600, 19200,
                                                    38400, 57600, 115200, 230400 };
    
    UartInputClock = Parameters->UartClock;

    //
    // If anything is zero, fill in the default value.
    //
    if ( BaudRate == 0 ) {
        BaudRate = UartDefaultBaudRate;
    }

    if ( ReceiveFifoDepth == 0 ) {
        ReceiveFifoDepth = UART_DEFAULT_RECEIVE_FIFO_DEPTH;
    }

    if ( Timeout == 0 ) {
        Timeout = UART_DEFAULT_TIMEOUT;
    }

    if ( Parity == DefaultParity ) {
        Parity = UartDefaultParity;
    }

    if ( DataBits == 0 ) {
        DataBits = UartDefaultDataBits;
    }

    if ( StopBits == DefaultStopBits ) {
        StopBits = UartDefaultStopBits;
    }

    //
    // Check for valid parameters.
    //
    // Don't allow less than 7 data bits on 16550A.
    if ( Parameters->UartType == UART16550A ) {
        if ( DataBits < 7 ) {
            return EFI_INVALID_PARAMETER;
        }
    }

    // Check baud rate
    if ( BaudRate < 50 ) {
        return EFI_INVALID_PARAMETER;
    }

    for (Temp = 0; Temp < (sizeof(BaudRateTable)/sizeof(BaudRateTable[0]))-1; Temp++)
    {
        if ( BaudRate < BaudRateTable[Temp + 1] ) {
            BaudRate = BaudRateTable[Temp];
            break;
        }
    }

    if ( BaudRate >= 115200 ) {
        BaudRate = 115200;
    }

    if ((ReceiveFifoDepth < 1) || 
        (ReceiveFifoDepth > UART_MAX_RECEIVE_FIFO_DEPTH)) {
        return EFI_INVALID_PARAMETER;
    }

    if ((Timeout < UART_MIN_TIMEOUT) || (Timeout > UART_MAX_TIMEOUT)) {
        return EFI_INVALID_PARAMETER;
    }

    if ((Parity < NoParity) || (Parity > SpaceParity)) {
        return EFI_INVALID_PARAMETER;
    }

    if ((DataBits < 5) || (DataBits > 8)) {
        return EFI_INVALID_PARAMETER;
    }

    if ((StopBits < OneStopBit) || (StopBits > TwoStopBits)) {
        return EFI_INVALID_PARAMETER;
    }

    //
    // Do not allow OneFiveStopBits for 6, 7, or 8 data bits.
    //
    if ((DataBits >=6) && (DataBits <=8) && (StopBits == OneFiveStopBits)) {
        return EFI_INVALID_PARAMETER;
    }

    //
    // If the new atributes are the same as the current ones, exit with 
    // success.
    //
    if (Parameters->UartDevicePath.BaudRate     == BaudRate         &&
        Parameters->UartDevicePath.DataBits     == DataBits         &&
        Parameters->UartDevicePath.Parity       == Parity           &&
        Parameters->UartDevicePath.StopBits     == StopBits         &&
        This->Mode->ReceiveFifoDepth            == ReceiveFifoDepth &&
        This->Mode->Timeout                     == Timeout) {
        return EFI_SUCCESS;
    }

    if (This->Mode->ReceiveFifoDepth != ReceiveFifoDepth) {
        EnableFifoBuffers(Parameters, ReceiveFifoDepth); 
    }

    //
    // Compute the baud rate divisor.
    //
    Divisor = (UINT32) Div64 (UartInputClock,
                                ((UINT32)BaudRate * 16),
                                &Remainder);
    if ( Remainder ) {
        Divisor += 1;
    }

    if ((Divisor == 0) || (Divisor & 0xffff0000)) {
        return EFI_INVALID_PARAMETER;
    }

    //
    // Read the Line Control Register and save it
    //
    Temp = SerialReadPort(Parameters, SERIAL_REGISTER_LCR);

    //
    // Put serial port in Divisor Latch Mode
    //
    Temp |= DLAB;
	
    // Wait for the serial port to be ready.
    while ((SerialReadPort (Parameters, SERIAL_REGISTER_LSR) & (TEMT | THRE)) != (TEMT | THRE));
    SerialWritePort(Parameters, SERIAL_REGISTER_LCR, Temp);

    //
    // Write the baud rate divisor to the serial port
    //
    SerialWritePort(Parameters, 
                    SERIAL_REGISTER_DLL, 
                    (UINT8)(Divisor & 0xff));
    SerialWritePort(Parameters, 
                    SERIAL_REGISTER_DLM,
                    (UINT8)((Divisor>>8) & 0xff));

    //
    // Reset divisor latch bit in order to set remaining attributes.
    //
    Temp &= ~DLAB;    // Kill divisor latch bit.

    // Set parity bits.
    switch (Parity) {
        case NoParity:
            Temp &= ~(PAREN | EVENPAR | STICPAR);
        break;
        case EvenParity:
            Temp |= (PAREN | EVENPAR);
            Temp &= ~STICPAR;
        break;
        case OddParity:
            Temp |= PAREN;
            Temp &= ~(EVENPAR | STICPAR);
        break;
        case SpaceParity:
            Temp |= (PAREN | EVENPAR | STICPAR);
        break;
        case MarkParity:
            Temp |= (PAREN | STICPAR);
            Temp &= ~EVENPAR;
        break;
        default:
        break;
    }
    //
    // Set stop bits.
    //
    switch (StopBits) {
        case OneStopBit :
            Temp &= ~STOPB;
        break;
        case OneFiveStopBits :
        case TwoStopBits :
            Temp |= STOPB;
        break;
        default:
        break;
    }
    //
    // Set data bits.
    //
    Temp &= ~SERIALDB;
    Temp |= (UINT8)((DataBits - 5) & 0x03);

    //
    // Now write the LCR register back.
    //
    SerialWritePort(Parameters, SERIAL_REGISTER_LCR, Temp);

    //
    //Set the Serial Mode parameters.
    //
    This->Mode->BaudRate         = BaudRate;
    This->Mode->ReceiveFifoDepth = ReceiveFifoDepth;
    This->Mode->Timeout          = Timeout;
    This->Mode->Parity           = Parity;
    This->Mode->DataBits         = DataBits;
    This->Mode->StopBits         = StopBits;
    
    //
    // If the parameters in the device path node are the same as the 
    // current ones, return success.
    //
    if (Parameters->UartDevicePath.BaudRate     == BaudRate &&
        Parameters->UartDevicePath.DataBits     == DataBits &&
        Parameters->UartDevicePath.Parity       == Parity   &&
        Parameters->UartDevicePath.StopBits     == StopBits    ) {
        return EFI_SUCCESS;
    }

    //
    // If the parameters have changed, update the device path.
    //
    Parameters->UartDevicePath.BaudRate = BaudRate;
    Parameters->UartDevicePath.DataBits = DataBits;
    Parameters->UartDevicePath.Parity   = (UINT8)Parity;
    Parameters->UartDevicePath.StopBits = (UINT8)StopBits;

    NewDevicePath = DPAddNode (
                    Parameters->ParentDevicePath,
                    (EFI_DEVICE_PATH_PROTOCOL *)&Parameters->UartDevicePath
                    );
    //
    // Adding the UartFlowControlDevicePath Node with Updated UartDevicePath Node.
    //
    NewDevicePath = DPAddNode(
                    NewDevicePath,
                    (EFI_DEVICE_PATH_PROTOCOL*) &Parameters->UartFlowControlDevicePath
                    );
    if (NewDevicePath == NULL) {
        return EFI_DEVICE_ERROR;
    }

    if (Parameters->Handle != NULL) {
        Status = pBS->ReinstallProtocolInterface (
                    Parameters->Handle,
                    &gEfiDevicePathProtocolGuid,
                    Parameters->DevPathProtocol,
                    NewDevicePath
                    );
        if ( EFI_ERROR( Status )) {
            return Status;
        }
    }

    if (Parameters->DevPathProtocol) {
        pBS->FreePool (Parameters->DevPathProtocol);
    }
    Parameters->DevPathProtocol = NewDevicePath;
    return EFI_SUCCESS;
}

/**
    Protocol interface to set the serial device control bits.
	
    @param      This
    @param      Control

    @retval     EFI_STATUS

    Referrals: SerialReadPort, SerialWritePort

**/

EFI_STATUS
EFIAPI
EfiSerialSetControl (
    IN EFI_SERIAL_IO_PROTOCOL   *This,
    IN UINT32                   Control
)
{
    SERIAL_PARAMETERS       *Parameters = (SERIAL_PARAMETERS *)This;
    UINT8                   Temp;
    
    //
    // Check for valid parameters. Only the controls checked here can be set.
    //
    if (Control & ~(EFI_SERIAL_DATA_TERMINAL_READY |
                    EFI_SERIAL_REQUEST_TO_SEND |
                    EFI_SERIAL_HARDWARE_LOOPBACK_ENABLE |
                    EFI_SERIAL_SOFTWARE_LOOPBACK_ENABLE |
                    EFI_SERIAL_HARDWARE_FLOW_CONTROL_ENABLE)) {
        return EFI_UNSUPPORTED;
    }

    Temp = SerialReadPort(Parameters, SERIAL_REGISTER_MCR);
    Temp &= ~(DTRC | RTS | LME);    // Kill DTRC, RTS, and LME bits.

    if (Control & EFI_SERIAL_DATA_TERMINAL_READY) {
        Temp |= DTRC;
    }

    if (Control & EFI_SERIAL_REQUEST_TO_SEND) {
        Temp |= RTS;
    }

    if (Control & EFI_SERIAL_HARDWARE_LOOPBACK_ENABLE) {
        Temp |= LME;
    }
    
  SerialWritePort(Parameters, SERIAL_REGISTER_MCR, Temp);

    Parameters->SerialMode.ControlMask = 0;
    if (Control & EFI_SERIAL_HARDWARE_FLOW_CONTROL_ENABLE) {
        Parameters->SerialMode.ControlMask |= 
                                EFI_SERIAL_HARDWARE_FLOW_CONTROL_ENABLE;
    }

    if (Control & EFI_SERIAL_HARDWARE_LOOPBACK_ENABLE) {
        Parameters->SerialMode.ControlMask |= 
                                    EFI_SERIAL_HARDWARE_LOOPBACK_ENABLE;
    } else if (Control & EFI_SERIAL_SOFTWARE_LOOPBACK_ENABLE) {
        Parameters->SerialMode.ControlMask |= 
                                    EFI_SERIAL_SOFTWARE_LOOPBACK_ENABLE;
    } 

    return EFI_SUCCESS;
}

/**
    Protocol interface to get the serial device control bits.
	
    @param      This
    @param      Control

    @retval     EFI_STATUS

    Referrals: SerialReadPort, SerialWritePort, SerialReceiveTransmit

**/

EFI_STATUS
EFIAPI
EfiSerialGetControl (
    IN EFI_SERIAL_IO_PROTOCOL   *This,
    OUT UINT32                  *Control
)
{
    SERIAL_PARAMETERS       *Parameters = (SERIAL_PARAMETERS *)This;
    
    UINT8                   Temp;
    UINT8                   LSRValue; 
    
    *Control = 0;

    //
    // Set the Modem Status Register controls.
    //
    Temp = SerialReadPort(Parameters, SERIAL_REGISTER_MSR);

    if (Temp & CTS) {
        *Control |= EFI_SERIAL_CLEAR_TO_SEND;
    }

    if (Temp & DSR) {
        *Control |= EFI_SERIAL_DATA_SET_READY;
    }

    if (Temp & RI) {
        *Control |= EFI_SERIAL_RING_INDICATE;
    }

    if (Temp & DCD) {
        *Control |= EFI_SERIAL_CARRIER_DETECT;
    }

    //
    // Set the Modem Control Register controls.
    //
    Temp = SerialReadPort(Parameters, SERIAL_REGISTER_MCR);
    
    if (Temp & DTRC) {
        *Control |= EFI_SERIAL_DATA_TERMINAL_READY;
    }

    if (Temp & RTS) {
        *Control |= EFI_SERIAL_REQUEST_TO_SEND;
    }

    if (Temp & LME) {
        *Control |= EFI_SERIAL_HARDWARE_LOOPBACK_ENABLE;
    }

    if (Parameters->SerialMode.ControlMask & 
                                EFI_SERIAL_HARDWARE_FLOW_CONTROL_ENABLE) {
        *Control |= EFI_SERIAL_HARDWARE_FLOW_CONTROL_ENABLE;
    }

    //
    // If the Transmit FIFO is empty, set the control bit for that.
    //
    LSRValue = SerialReadPort(Parameters, SERIAL_REGISTER_LSR);
    if (LSRValue & THRE) {
        *Control |= EFI_SERIAL_OUTPUT_BUFFER_EMPTY;
    }

    //
    // If the Receive FIFO is empty, set the control bit for that.
    //

    if (LSRValue & OE) {
        Parameters->LineStatusRegOverrunErrorBit = TRUE;
    }
    if (!(LSRValue & DR)) {
        *Control |= EFI_SERIAL_INPUT_BUFFER_EMPTY;
    }

    //
    // If loopback has been enabled, set the control bit for that.
    //
    if (Parameters->SerialMode.ControlMask
                                & EFI_SERIAL_SOFTWARE_LOOPBACK_ENABLE) {
        *Control |= EFI_SERIAL_SOFTWARE_LOOPBACK_ENABLE;
    }

    return EFI_SUCCESS;
}

/**
    Protocol interface to write data to a serial device.
    We use a semaphore Parameters->ReadWriteSemaphore.
			  
    @param  This
    @param  BufferSize
    @param  Buffer

    @retval EFI_STATUS

    Referrals: SerialReceiveTransmit, SerialFifoPut, CheckSerialFifoEmpty

**/

EFI_STATUS
EFIAPI
EfiSerialWrite (
    IN EFI_SERIAL_IO_PROTOCOL   *This,
    IN OUT UINTN                *BufferSize,
    IN VOID                     *Buffer
)
{
    SERIAL_PARAMETERS       *Parameters = (SERIAL_PARAMETERS *)This;
    UINT8                   *ByteBuffer;
    UINT8                   Byte = 0;
    UINT32                  Index;
    UINTN                   WaitTime = 0;
    UINTN                   BytesWritten = 0;
    BOOLEAN                 Handshaking = FALSE;

    BOOLEAN                 HardwareLoopbackMode = 
                                (Parameters->SerialMode.ControlMask &
                                    EFI_SERIAL_HARDWARE_LOOPBACK_ENABLE) ?
                                        TRUE : FALSE;
    EFI_STATUS              HardwareLoopbackModeStatus = EFI_SUCCESS;
    EFI_STATUS              Status;
    UINTN                   DataSize=0;

    if (*BufferSize == 0) {
        return EFI_SUCCESS;
    }

    if (!Buffer) {
        return EFI_DEVICE_ERROR;
    }

    if (Parameters->SerialDevicePresentOnPort) {
        *BufferSize = 0;
        return EFI_DEVICE_ERROR;
    }
    //
    // if the Serial Write Error check enabled and if it reaches the maximum error count
    // return serial port write as error always and return error if host is not ready
    // to accept data(i.e CTS is not Set) when flow control is enabled.
    //
    if((SerialWriteErrorCheck && Parameters->SerialPortErrorCount >= MaximumSerialWriteErrorCount) || (Parameters->FlowCtrlCTSnotSet)) {
        *BufferSize = 0;

        if( Parameters->TimerEventActive == FALSE) {

            //
            // Enable the Port active checking for every one second. This event will
            // also check if CTS is Set(host ready to accept data).
            //
            pBS->SetTimer(Parameters->SerialErrorReEnableEvent,
                            TimerPeriodic,
                            10000000);

            Parameters->TimerEventActive=TRUE;
        }

        return EFI_DEVICE_ERROR;
    }

    ByteBuffer = (UINT8 *)Buffer;

    if (HardwareLoopbackMode) {
        if (*BufferSize > 16) {
            *BufferSize = 16;
            HardwareLoopbackModeStatus = EFI_TIMEOUT;
        } else {
            HardwareLoopbackModeStatus = EFI_SUCCESS;
        }
    }

    if ( Parameters->ReadWriteSemaphore ) {
        return EFI_TIMEOUT;
    }
    Parameters->ReadWriteSemaphore = 1;

    //
    // Software loopback section. Minimum code to pass SCT test.
    //
    if (Parameters->SerialMode.ControlMask &
        EFI_SERIAL_SOFTWARE_LOOPBACK_ENABLE) {
        while(DataSize < *BufferSize) {
            if(CheckSerialFifoFull(Parameters)) {
                AddSerialFifo(Parameters,ByteBuffer[DataSize]);
            } else {
                *BufferSize=DataSize;
                Status = EFI_OUT_OF_RESOURCES;
                goto ReleaseSemaphore;
            }
            DataSize++;
        }
        Status = EFI_SUCCESS;
        goto ReleaseSemaphore;
    }

    // Software flow control section.
    // Software flow control path
    if (Parameters->SerialMode.ControlMask &
        EFI_SERIAL_SOFTWARE_FLOW_CONTROL_ENABLE) {
        if (Parameters->WaitForXon) {
            // read in loop until Xon received
            do
            {
                Byte = SerialReadPort( Parameters, SERIAL_REGISTER_LSR );

                if ( Byte & OE ) {
                    Parameters->LineStatusRegOverrunErrorBit = TRUE;
                }
            } while ((Byte & DR) == FALSE );
            Byte =     SerialReadPort( Parameters, SERIAL_REGISTER_RBR );

            if ( Byte == XON ) {
                Parameters->WaitForXon = FALSE;
            }
            *BufferSize = BytesWritten;
            Status = EFI_TIMEOUT;
            goto ReleaseSemaphore;
        }
    }

    // Hardware flow control section.
    // Note: This code takes in to consideration two facts about hardware
    // handshaking:
    // 1. Hardware handshaking is for the benefit of the terminal, not the
    //    serial port.
    //    Therefore, the handshaking loop is outside the port write loop.
    // 2. Hardware handshaking is NOT standardized. What is done here works
    //    with Hyperterminal, and it is not known if it will work with other
    //    terminal programs or terminals.
    // Hardware flow control path
    if ((Parameters->SerialMode.ControlMask &
        EFI_SERIAL_HARDWARE_FLOW_CONTROL_ENABLE) && !(Parameters->FlowCtrlCTSnotSet)) {
        Handshaking = TRUE;
        WaitTime = 0;
        Parameters->ErrorCount = 0;
        Byte = SerialReadPort(Parameters, SERIAL_REGISTER_MCR);
        Byte |= RTS; // Set RTS
        SerialWritePort(Parameters, SERIAL_REGISTER_MCR, Byte);

        do { // Wait for CTS
            Byte = SerialReadPort(Parameters, SERIAL_REGISTER_MSR);

            if ((Byte & CTS) == FALSE) {
                pBS->Stall(1);
                WaitTime += 1;
                if (WaitTime == 100000) { // 1/10 sec
                    WaitTime = 0;
                    Parameters->ErrorCount += 1;
                    // Turn off Redirection if too many errrors.
                    if (Parameters->ErrorCount == MaxFailuresAllowed) {
                        //
                        // Disbale Redirection if host is not ready to accept data.
                        // Redirection will be enabled again once CTS is set.
                        //
                        Parameters->FlowCtrlCTSnotSet = TRUE;
                        Handshaking = FALSE;
                        Status = EFI_DEVICE_ERROR;
                        goto ReleaseSemaphore;
                    }
                }
            }
        } while ((Byte & CTS) == FALSE);
    }

    // Serial port write loop.
    for (Index = 0; Index < *BufferSize; Index ++) {
        WaitTime = 0;
        do {
            pBS->Stall(1);
            WaitTime+= 1;
            if (WaitTime == 100000) { // 1/10 sec
                *BufferSize = BytesWritten;
                Status = EFI_SUCCESS;
                //
                //Increase the Error Count on Serial Write.
                //
                Parameters->SerialPortErrorCount++;
                goto ReleaseSemaphore;
            }
            Byte = SerialReadPort(Parameters, SERIAL_REGISTER_LSR);
            if (Byte & OE) {
                Parameters->LineStatusRegOverrunErrorBit = TRUE;
            }
            // Loop until the serial port is ready for the next byte
        } while ((Byte & THRE) == FALSE);

        //
        // Reset the Error Count When there is a successfull Serial Write.
        //
        Parameters->SerialPortErrorCount=0;
        SerialWritePort(Parameters, SERIAL_REGISTER_THR, ByteBuffer[Index]);
        BytesWritten++; // Count bytes written

        if (HardwareLoopbackMode) {
            WaitTime = 0;
            do {
                pBS->Stall(1);
                WaitTime+= 1;
                if (WaitTime == 100000) { // 1/10 sec
                    *BufferSize = BytesWritten;
                    Status = EFI_TIMEOUT;
                    goto ReleaseSemaphore;
                }
                Byte = SerialReadPort(Parameters, SERIAL_REGISTER_LSR);
                if (Byte & OE) {
                    Parameters->LineStatusRegOverrunErrorBit = TRUE;
                }
            } while ((Byte & DR) == FALSE);
        }

    } // end of index loop

    if ((Parameters->SerialMode.ControlMask &
        EFI_SERIAL_HARDWARE_FLOW_CONTROL_ENABLE) ||
        Handshaking) {
        Byte = SerialReadPort(Parameters, SERIAL_REGISTER_MCR);
        Byte &= ~(RTS); // Clear RTS (signal write is done)
        SerialWritePort(Parameters, SERIAL_REGISTER_MCR, Byte);
        pBS->Stall(1); // Let him see it.
    }

    // Hyperterminal could be set to hardware handshaking even if we are not.
    // By setting DTR and RTS here unconditionally, we can still get input.
    Byte = SerialReadPort(Parameters, SERIAL_REGISTER_MCR);
    //
    // Tell Hyperterminal that it can write to us now.
    //
    Byte |= (DTRC | RTS);
    SerialWritePort( Parameters, SERIAL_REGISTER_MCR, Byte );

    //
    // Return bytes written IAW spec
    //
    *BufferSize = BytesWritten;

    if ( HardwareLoopbackMode ) {
        Status = HardwareLoopbackModeStatus;
    } else {
        Status = EFI_SUCCESS;
    }

ReleaseSemaphore:
    Parameters->ReadWriteSemaphore = 0;
    return Status; 
}

/**
    Description: Protocol interface to read data from a serial device.
    In the case of a buffer overrun error, we dump the receive 
    FIFO buffer.
    We use a semaphore (Parameters->ReadWriteSemaphore).

    @param  This
    @param  BufferSize
    @param  Buffer

    @retval EFI_STATUS

    Referrals: SerialReceiveTransmit, SerialFifoGet

**/

EFI_STATUS
EFIAPI
EfiSerialRead (
    IN EFI_SERIAL_IO_PROTOCOL           *This,
    IN OUT UINTN                        *BufferSize,
    OUT VOID                            *Buffer
)

{
    EFI_STATUS              Status;
    SERIAL_PARAMETERS       *Parameters = (SERIAL_PARAMETERS *)This;
    UINT32                  Index;
    UINT8                   *ByteBuffer;
    UINT8                   LSRValue;

    UINTN                   PassedInBufferSize = 0;

    ByteBuffer = (UINT8 *)Buffer;

    if (*BufferSize == 0) {
        return EFI_SUCCESS;
    }

    if (!Buffer) {
        return EFI_DEVICE_ERROR;
    }

    if (Parameters->ReadWriteSemaphore) {
      *BufferSize = 0;
      return EFI_TIMEOUT;
    }

    //
    // if the Serial Error check enabled and if it reaches the maximum error count
    // return serial port Read as error always.
    //
    if( (SerialWriteErrorCheck && Parameters->SerialPortErrorCount >= MaximumSerialWriteErrorCount) || (Parameters->FlowCtrlCTSnotSet)) {
        *BufferSize = 0;
        if( Parameters->TimerEventActive == FALSE) {

            //
            // Enable the Port active checking for every one second
            //
            pBS->SetTimer(Parameters->SerialErrorReEnableEvent,
                            TimerPeriodic,
                            10000000);

            Parameters->TimerEventActive=TRUE;
        }
        return EFI_DEVICE_ERROR;
    }

    Parameters->ReadWriteSemaphore = 1;

    //
    // Software loopback section. Minimum code to pass SCT test.
    //
    if (Parameters->SerialMode.ControlMask &
        EFI_SERIAL_SOFTWARE_LOOPBACK_ENABLE) {

        while(PassedInBufferSize < *BufferSize) {
            if(!CheckSerialFifoEmpty(Parameters)) {
                ByteBuffer[PassedInBufferSize]= RemoveSerialFifo(Parameters);
            } else {
                *BufferSize=PassedInBufferSize;
                Status = EFI_TIMEOUT;
                goto ReleaseSemaphore;
            }
            PassedInBufferSize++;
        }
        if ( PassedInBufferSize != *BufferSize ) {
            Status = EFI_TIMEOUT;
        } else {
            Status = EFI_SUCCESS;
        }
        goto ReleaseSemaphore; 
    }

DumpFIFOBufferBecauseOfOverrunError:
    if (Parameters->LineStatusRegOverrunErrorBit) {
        //dump everything in the FIFO buffer.
        for (Index = 0; Index < This->Mode->ReceiveFifoDepth; Index++) {
            SerialReadPort(Parameters, SERIAL_REGISTER_RBR);
        }
        Parameters->LineStatusRegOverrunErrorBit = FALSE;
        *BufferSize = 0;
        Status =  EFI_DEVICE_ERROR;
        goto ReleaseSemaphore;
    }

    Index = 0;
    PassedInBufferSize = *BufferSize;
    while (TRUE) {
        LSRValue = SerialReadPort(Parameters, SERIAL_REGISTER_LSR);

        if (LSRValue & OE) {
            Parameters->LineStatusRegOverrunErrorBit = TRUE;
            goto DumpFIFOBufferBecauseOfOverrunError;
        }

        if (LSRValue & DR) {
            ByteBuffer[Index++] =
                        SerialReadPort(Parameters, SERIAL_REGISTER_RBR);
            if (Index == *BufferSize) {
                Status = EFI_SUCCESS;
                goto ReleaseSemaphore;
            }
        } else {
            *BufferSize = Index;
            if ( PassedInBufferSize != *BufferSize ) {
                Status = EFI_TIMEOUT;
            } else {
                Status = EFI_SUCCESS;
            }
            goto ReleaseSemaphore;
        }
    }
ReleaseSemaphore:
    Parameters->ReadWriteSemaphore = 0;
    return Status; 
}

//
// LOCAL FUNCTIONS
//

/**
Add the data to the loopback buffer

    @param  Parameters
    @param  Data

    @retval VOID

**/

VOID
AddSerialFifo (
    IN SERIAL_PARAMETERS *Parameters,
	IN UINT8 Data
) 
{
    Parameters->LoopbackBuffer[Parameters->Fifotail] = Data;
    Parameters->Fifotail++;
    Parameters->DataCount++;
    if(Parameters->Fifotail >= Parameters->SerialIo.Mode->ReceiveFifoDepth) {
        Parameters->Fifotail = 0;
    }
    return;
}

/**
    Remove the data from the loopback buffer

    @param  Parameters

    @retval UINT8

**/

UINT8
RemoveSerialFifo (
    IN SERIAL_PARAMETERS *Parameters
) 
{
    UINT8   Data;
    Data=Parameters->LoopbackBuffer[Parameters->Fifohead];
    Parameters->Fifohead++;
    Parameters->DataCount--;
    if(Parameters->Fifohead >= Parameters->SerialIo.Mode->ReceiveFifoDepth) {
        Parameters->Fifohead = 0;
    }
    return Data;
}

/**
    Check the loopback buffer empty or not

    @param      Parameters

    @retval     BOOLEAN

**/

BOOLEAN
CheckSerialFifoEmpty (
    IN SERIAL_PARAMETERS *Parameters 
)
{
    if(Parameters->DataCount == 0) {
        return TRUE;
    }
    return FALSE;
}

/**
    check the loop back buffer full or not

    @param  Parameters

    @retval BOOLEAN

**/

BOOLEAN
CheckSerialFifoFull (
    IN SERIAL_PARAMETERS *Parameters 
) 
{
    //
    // count of free items in the queue
    //
    if( Parameters->DataCount >= Parameters->SerialIo.Mode->ReceiveFifoDepth ) {
        return FALSE;
    }

    return TRUE;
}

//
// Read and write functions
//

/**
    Reads a byte from the serial port.
	
    @param     Parameters
    @param     Offset
 
    @retval    UINT8

**/

UINT8
SerialReadPort (
    IN SERIAL_PARAMETERS    *Parameters,
    IN UINT32               Offset
)
{
    UINT8 Data; 

    if(Parameters->AmiSerialDetected) {
        if(!Parameters->MMIODevice) {
            return IoRead8((UINT32)Parameters->BaseAddress + Offset);
        } else {
            if(Parameters->MmioWidth == 4) {
                UINT32  TempData;
                Offset = Offset * 4;
                TempData = *(volatile UINT32*)(Parameters->BaseAddress + Offset);
                Data=(UINT8)TempData;
                return Data;
            } else if(Parameters->MmioWidth == 2) {
                UINT16  TempData;
                Offset = Offset * 2;
                TempData = *(volatile UINT16*)(Parameters->BaseAddress + Offset);
                Data=(UINT8)TempData;
                return Data;
            } else {
                return *(volatile UINT8*)(Parameters->BaseAddress + Offset);
            }
        }
    }

    if (!Parameters->PciIo) {
        return IoRead8((UINT32)Parameters->BaseAddress + Offset);
    }
    if (Parameters->MMIODevice == TRUE) {

        if(Parameters->MmioWidth == 4) {
            UINT32  TempData;
            //
            // Mmio access is Dword Length.
            //
            Offset=Offset * 4;
            Parameters->PciIo->Mem.Read (Parameters->PciIo,
                                EfiPciIoWidthUint32,
                                Parameters->BarIndex,
                                (UINT16) Offset,
                                1,
                                &TempData
                                );
            Data=(UINT8)TempData;
            return Data;
        } else if(Parameters->MmioWidth == 2) {
            UINT16  TempData;
            //
            // Mmio access is Word Length.
            //
            Offset=Offset * 2;
            Parameters->PciIo->Mem.Read (Parameters->PciIo,
                                EfiPciIoWidthUint16,
                                Parameters->BarIndex,
                                (UINT16) Offset,
                                1,
                                &TempData
                                );
            Data=(UINT8)TempData;
            return Data;
        } else {
            //
            // Mmio access is Byte Length.
            //
            Parameters->PciIo->Mem.Read (Parameters->PciIo,
                                EfiPciIoWidthUint8,
                                Parameters->BarIndex,
                                (UINT16) Offset,
                                1,
                                &Data
                                );
            return Data;
        }
    }
    Parameters->PciIo->Io.Read (Parameters->PciIo,
                                EfiPciIoWidthUint8,
                                Parameters->BarIndex,
                                (UINT16) Offset,
                                1,
                                &Data
                                );
    return Data;

}


/**
    Write the byte "Data" to the serial port.

    @param  Parameters
    @param  Offset
    @param  Data

    @retval VOID

**/

VOID
SerialWritePort (
    IN SERIAL_PARAMETERS    *Parameters,
    IN UINT32               Offset,
    IN UINT8                Data
)
{

    if(Parameters->AmiSerialDetected) {
        if(!Parameters->MMIODevice) {
            IoWrite8((UINT32)Parameters->BaseAddress + Offset, Data);
        } else {
            if(Parameters->MmioWidth == 4) {
                UINT32  TempData=(UINT32)Data;
                Offset=Offset * 4;
                *(volatile UINT32*)(Parameters->BaseAddress + Offset)= TempData;
            } else if(Parameters->MmioWidth == 2) {
                UINT16  TempData=(UINT16)Data;
                Offset=Offset * 2;
                *(volatile UINT16*)(Parameters->BaseAddress + Offset) = TempData;
            } else {
                *(volatile UINT8*)(Parameters->BaseAddress + Offset) = Data;
            }
        }
        return;
    }

    if (!Parameters->PciIo) {
        IoWrite8((UINT32)Parameters->BaseAddress + Offset, Data);
    } else if (Parameters->MMIODevice == TRUE) {
        if(Parameters->MmioWidth == 4) {
            UINT32  TempData=(UINT32)Data;
            //
            // Mmio access is Dword Length.
            //
            Offset=Offset * 4;
            Parameters->PciIo->Mem.Write (Parameters->PciIo,
                                EfiPciIoWidthUint32,
                                Parameters->BarIndex,
                                (UINT16) Offset,
                                1,
                                &TempData
                                );
            return;
        } else if(Parameters->MmioWidth == 2) {
            UINT16  TempData=(UINT16)Data;
            //
            // Mmio access is Word Length.
            //
            Offset=Offset * 2;
            Parameters->PciIo->Mem.Write (Parameters->PciIo,
                                EfiPciIoWidthUint16,
                                Parameters->BarIndex,
                                (UINT16) Offset,
                                1,
                                &TempData
                                );
            return ;
        } else {
            //
            // Mmio access is Byte Length.
            //
            Parameters->PciIo->Mem.Write (Parameters->PciIo,
                                EfiPciIoWidthUint8,
                                Parameters->BarIndex,
                                (UINT16) Offset,
                                1,
                                &Data
                                );
            return;
        }

    } else {
        Parameters->PciIo->Io.Write (Parameters->PciIo,
                                    EfiPciIoWidthUint8,
                                    Parameters->BarIndex,
                                    (UINT16) Offset,
                                    1,
                                    &Data
                                    );
    }

    return;
}

/**
    "hid" should match EISA_PNP_ID(0x501).

    @param  hid

    @retval BOOLEAN

**/

BOOLEAN
LookupHID (
    UINT32          hid
)
{
    return hid == EISA_PNP_ID(0x501);
}

/**
    This fuction returns the last node in the device 
    path for the given controller, which is the Acpi device path
    for the Serial port.
    @param  This
    @param  Controller
    @param  SerialIodp
    @param  Attributes
    @param  Test

    @retval  EFI_STATUS

**/

EFI_STATUS
GetSerialIo_DP (
    IN EFI_DRIVER_BINDING_PROTOCOL *This,
    IN EFI_HANDLE                  Controller,
    IN OUT ACPI_HID_DEVICE_PATH**  SerialIodp,
    IN UINT32                      Attributes,
    IN BOOLEAN                     Test
)
{
    EFI_STATUS Status;
    ACPI_HID_DEVICE_PATH        *acpiDP;
    EFI_DEVICE_PATH_PROTOCOL    * SerialIoDevPath;

    // Get device path from Controller handle.
    //
    Status = pBS->OpenProtocol (
                    Controller,
                    &gEfiDevicePathProtocolGuid,
                    (void **)&SerialIoDevPath,
                    This->DriverBindingHandle,
                    Controller,
                    Attributes
                    );
 
    if (EFI_ERROR(Status)) {
        return EFI_UNSUPPORTED;
    }

    acpiDP = *SerialIodp =
                    (ACPI_HID_DEVICE_PATH *)DPGetLastNode(SerialIoDevPath);

    Status = (acpiDP->Header.Type == ACPI_DEVICE_PATH &&
                acpiDP->Header.SubType == ACPI_DP) ?
                EFI_SUCCESS : EFI_UNSUPPORTED;
    if(Test == TRUE){
        pBS->CloseProtocol (
            Controller,
            &gEfiDevicePathProtocolGuid,
            This->DriverBindingHandle,
            Controller
            );
    }
    return Status;
}

/**
    In our implementation, we check the characteristics of 
    the FIFO buffers and set the Uart type accordingly to 
    UART16550A, UART16550, or UART16450.

    @param      Parameters

    @retval     VOID

**/ 

VOID
SetUartType (
    SERIAL_PARAMETERS *Parameters
)
{
    UINT8 IIRValue = 0;
    UINT8 FifoEnableStatus = 0; 

    SerialWritePort(Parameters, SERIAL_REGISTER_FCR, TRFIFOE);
    IIRValue = SerialReadPort(Parameters, SERIAL_REGISTER_IIR);
    FifoEnableStatus = IIRValue & FIFOES;

    if (FifoEnableStatus == FIFO_ENABLED) {
        Parameters->UartType = UART16550A;
    } else if (FifoEnableStatus == FIFO_ENABLED_UNUSABLE) {
        Parameters->UartType = UART16550;
    } else if (FifoEnableStatus == FIFO_DISABLED) {
        Parameters->UartType = UART16450; //UartType is 16450 or 8250
    }

}

/**
    If the UART is of type 16550A or a later one, 
    if "NewFifoDepth" is 1, the FIFO buffers are disabled;
    else they are enabled.
    If the UART is of a earlier type than 16550A 
    (in which case there are no FIFO buffers), 
    the FIFO buffers are disabled. 

    @param  Parameters
    @param  NewFifoDepth
 
    @retval VOID

**/ 

VOID
EnableFifoBuffers (
    IN SERIAL_PARAMETERS *Parameters,
    IN UINT32 NewFifoDepth
)
{
    if (Parameters->UartType < UART16550A) {
        SerialWritePort(Parameters, SERIAL_REGISTER_FCR, ~TRFIFOE);
    } else {
        if (NewFifoDepth == 1) {
            SerialWritePort(Parameters, SERIAL_REGISTER_FCR, ~TRFIFOE);
        } else {
            SerialWritePort(Parameters, SERIAL_REGISTER_FCR, 
                            TRFIFOE|RESETRF|RESETTF);
        }
    }

}

/**
    Checks if a hardware loopback plug is attached and sets 
    the result in Parameters->SerialDevicePresentOnPort. 

    @param      SerialIo

    @retval     BOOLEAN

**/

BOOLEAN
CheckForLoopbackDevice (
    IN SERIAL_PARAMETERS *Parameters
)
{
    UINT8   Byte;
    UINT8   Byte2;
    UINT8   FcrValue, McrValue;

    //
    //  If the Signature doesn't match with the Serial Driver Signature,
    //  return FALSE
    //
    if(Parameters->Signature!=EFI_SERIAL_DRIVER_SIGNATURE){
        return FALSE;
    }
    
    FcrValue = SerialReadPort(Parameters, SERIAL_REGISTER_FCR);

    //
    // Program the FCR
    //
    SerialWritePort(Parameters, SERIAL_REGISTER_FCR,
                            TRFIFOE|RESETRF|RESETTF);

    Byte = SerialReadPort(Parameters, SERIAL_REGISTER_FCR);
    if(Byte == 0xff) {
        SerialWritePort(Parameters, SERIAL_REGISTER_FCR, FcrValue);
        Parameters->SerialDevicePresentOnPort = TRUE;
        return TRUE;
    }

    //
    // Wait for 2ms is sufficient for the next byte
    //
    pBS->Stall(2000);

    //
    //check if RECV DATA AVAILABLE IS SET. If Available,Read the data till all data is read
    //
    do {
        Byte = SerialReadPort( Parameters, SERIAL_REGISTER_LSR );
        if(Byte & DR) {
            Byte2 = SerialReadPort( Parameters, SERIAL_REGISTER_RBR );
        }
    } while ((Byte & DR) == TRUE );

    //
    // Write into THR
    //
    SerialWritePort(Parameters, SERIAL_REGISTER_THR,0x80);
    //
    // Send BackSpace to clear the character(0x80) sent for testing
    // the presence of Loop Back Device.
    //
    SerialWritePort(Parameters, SERIAL_REGISTER_THR,0x08);
    SerialWritePort(Parameters, SERIAL_REGISTER_THR,0x20);
    SerialWritePort(Parameters, SERIAL_REGISTER_THR,0x08);

    //
    // Wait for 5ms is sufficient for the next byte
    //
    pBS->Stall(50000);

    Byte = SerialReadPort( Parameters, SERIAL_REGISTER_LSR );

    if(Byte & DR) {
        Byte2 = SerialReadPort( Parameters, SERIAL_REGISTER_RBR );
        if(Byte2 == 0x80) {
            SerialWritePort(Parameters, SERIAL_REGISTER_FCR, FcrValue);
            Parameters->SerialDevicePresentOnPort = TRUE;
            return TRUE ;
        }
    }

    if(SerialMouseDetection) {
        //
        // Check for Mouse device on the Serial Port.
        // If we Toggle the DTR BIT, mouse sends data to RCVR BUFFER.
        //

        //
        // Program the FCR
        //
        SerialWritePort(Parameters, SERIAL_REGISTER_FCR,
                                TRFIFOE|RESETRF|RESETTF);

        //
        // Wait for 5ms is sufficient for the next byte
        //
        pBS->Stall(5000);

        //
        //check if RECV DATA AVAILABLE IS SET. If Available,Read the data till all data is read
        //
        do {
            Byte = SerialReadPort( Parameters, SERIAL_REGISTER_LSR );
            if(Byte & DR) {
                SerialReadPort( Parameters, SERIAL_REGISTER_RBR );
            }
        } while ((Byte & DR) == TRUE );

        McrValue = SerialReadPort( Parameters, SERIAL_REGISTER_MCR );
        Byte = SerialReadPort( Parameters, SERIAL_REGISTER_MCR );

        //
        // Wait for 2ms is sufficient for the next byte
        //
        pBS->Stall(2000);

        if(Byte & DTRC) {
            Byte &= ~(DTRC);
        } else {
            Byte |= DTRC;
        }

        SerialWritePort(Parameters, SERIAL_REGISTER_MCR, Byte);

        //
        // Wait for 5ms is sufficient for the next byte
        //
        pBS->Stall(5000);

        Byte = SerialReadPort( Parameters, SERIAL_REGISTER_LSR );

        SerialWritePort(Parameters, SERIAL_REGISTER_FCR, FcrValue); 
        SerialWritePort(Parameters, SERIAL_REGISTER_MCR, McrValue);

        //
        // If mouse device connected in Serial port, will be sending the data.
        //
        if(Byte & DR) {
            Parameters->SerialDevicePresentOnPort = TRUE;
            return TRUE;
        }
    }
    return FALSE;
}

/**
    Serial Read and Write will be failed incase if it's reaches the maximum error count
    Once it reaches the error count, this event will be activated to monitor the serial port
    working state. If Transmistter Holding Register ready to accept the data, Error count will 
    be reset so that redirection will start working again.

    Also this event fucntion will check if host can accept data(CTS is Set)
    when flow control is enabled. If Set it resets FlowCtrlCTSnotSet so that
    redirection will start working again.

    @param  Event
    @param  Context

    @retval VOID

**/

VOID
EFIAPI
CheckThreBit (
    EFI_EVENT Event, 
    VOID *Context
)
{
    SERIAL_PARAMETERS   *Parameters=(SERIAL_PARAMETERS*)((UINTN*)Context);
    UINT8               Byte=0;

    if(Parameters == NULL) {
        return;
    }

    if( !Parameters->FlowCtrlCTSnotSet ) {

        Byte = SerialReadPort(Parameters, SERIAL_REGISTER_LSR);

        //
        // Check the Transmistter Holding Register Empty Status
        //
        if(!(Byte & THRE)) {
            return;
        }

        //
        // Serial Port ready to accept the data.
        //
        Parameters->SerialPortErrorCount=0;
    } else {

        Byte = SerialReadPort(Parameters, SERIAL_REGISTER_MSR);

        //
        // Check whether Host can accept data.
        //
        if(!(Byte & CTS)) {
            return;
        }
        Parameters->FlowCtrlCTSnotSet = FALSE;

    }

    //
    // Disable the Event
    //
    pBS->SetTimer(Parameters->SerialErrorReEnableEvent,
                        TimerCancel,
                        0);

    Parameters->TimerEventActive=FALSE;

    return;
}

//****************************************************************************
//****************************************************************************
//**                                                                        **
//**             (C)Copyright 1985-2016, American Megatrends, Inc.          **
//**                                                                        **
//**                          All Rights Reserved.                          **
//**                                                                        **
//**             5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093       **
//**                                                                        **
//**                          Phone (770)-246-8600                          **
//**                                                                        **
//****************************************************************************
//****************************************************************************
