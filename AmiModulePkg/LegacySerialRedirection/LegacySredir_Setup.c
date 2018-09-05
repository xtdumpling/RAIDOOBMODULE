//***************************************************************************
//**                                                                       **
//**       (C)Copyright 1993-2016 Supermicro Computer, Inc.                **
//**                                                                       **
//**                                                                       **
//***************************************************************************
//***************************************************************************
//
//  File History
//    Rev.  1.00
//     Bug Fix:     Patch COM2/SOL console redirection works abnormally.
//     Reason:      Compare with previous revision (LegacySreDir_002)
//     Auditor:     Jimmy Chiu
//     Date:        Jul/22/2016
//
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
/** @file LegacySredir_Setup.c
    This File is used to get the Terminal Setup values from EFI
*/

#pragma warning ( disable : 4214 )

//---------------------------------------------------------------------------

#include "Token.h"
#include "Protocol/AmiLegacySredir.h"
#include <Setup.h>
#include <AmiDxeLib.h>
#include <Protocol/AmiSio.h>
#include <Protocol/DevicePath.h>
#include <AcpiRes.h>
#include <Protocol/PciIo.h>
#include <Pci.h>
#include <Protocol/TerminalAmiSerial.h>
#include <Include/TerminalSetupVar.h>
#include <LegacySredirElink.h>
#include "LegacySredir.h"
#include "TerminalElink.h"

//---------------------------------------------------------------------------

extern AMI_COM_PARAMETERS   gAmiComParameters;
extern UINTN                gUartInputClock;
//extern UINT32 gTotalSioSerialPorts;
UINT32 gTotalSioSerialPorts = TOTAL_SIO_SERIAL_PORTS;

EFI_GUID    gTerminalVarGuid   = TERMINAL_VAR_GUID;

BOOLEAN             IsFound = FALSE;
BOOLEAN             IsPciDevice;

UINT32              gComBaudRates[8] = {0, 0, 0, 9600, 19200, 38400, 57600, 115200};

INVALID_PCICOM InvalidPciCom[] = {INVALID_PCICOM_DEVICELIST
                                  {0xFFFF, 0xFFFF}
                                 };

PCI_DEVICE_UART_CLOCK_WIDTH gPciDeviceUartClockAndWidth[] = {
     PCI_DEVICE_UART_CLOCK_WIDTH_LIST
    { 0xFFFF, 0xFFFF, 0xFF,0xFF }
};

/**
    This Function is used to Initialize the Non Standard 
    Serial Port Registers 

    @param AmiComParameters-Address of the COM port parameters Structure

    @retval EFI_STATUS

    @note  Some of the MMIO COM ports has non Standard bits in Registers
              Those registers are all initialized on this function.
*/

EFI_STATUS
InitilizeNonCommonSerialRegsiters (
    IN  AMI_COM_PARAMETERS  *AmiComParameters
)
{
    return EFI_SUCCESS;
}

/**
    This Function is used to Read the

    @param BaseAddress 
    @param Offset 
    @param MmioDevice 
  
    @retval Data

*/

UINT8
SerialReadPort (
    IN  UINT32  BaseAddress, 
    IN  BOOLEAN MmioDevice,
    IN  UINT32  Offset
)
{
    UINT8   TempData8;

    if(MmioDevice) {
        //
        // Write into the Scratch Pad Reg
        //
        UINT8  MmioWidth = gAmiComParameters.MmioWidth;
        if (MmioWidth == DWORD_WIDTH) {
            UINT32  TempData32;
            TempData32=*(UINT32*)(BaseAddress +(Offset*DWORD_WIDTH));
            return (UINT8)(TempData32);
        } else if (MmioWidth == WORD_WIDTH ) {
            UINT16  TempData16;
            TempData16=*(UINT16*)(BaseAddress +(Offset*WORD_WIDTH));
            return (UINT8)(TempData16);
        } else {
            TempData8=*(UINT8*)(BaseAddress +(Offset*BYTE_WIDTH));
            return TempData8;
        }
    } 
    TempData8=IoRead8(BaseAddress + Offset);
    return TempData8;
}

/**
    This Function is used to Read the

    @param BaseAddress 
    @param Offset 
    @param MmioDevice 
    @param Data 
  
    @retval EFI_STATUS

*/

EFI_STATUS
SerialWritePort( 
    IN  UINT32  BaseAddress, 
    IN  BOOLEAN MmioDevice,
    IN  UINT32  Offset,
    IN  UINT8   Data
)
{

    if(MmioDevice) {
        //
        // Write into the Scratch Pad Reg
        //
        UINT8  MmioWidth = gAmiComParameters.MmioWidth;
        if (MmioWidth == DWORD_WIDTH) {
            UINT32  TempData32=(UINT32)Data;
            *(UINT32*)(BaseAddress +(Offset*DWORD_WIDTH))=TempData32;
        } else if (MmioWidth == WORD_WIDTH ) {
            UINT16  TempData16=(UINT16)Data;
            *(UINT16*)(BaseAddress +(Offset*WORD_WIDTH))=(UINT16)TempData16;
        } else {
            *(UINT8*)(BaseAddress +(Offset*BYTE_WIDTH))=(UINT8)Data;
        }
    } else {

        IoWrite8(BaseAddress + Offset, Data);
    }

    return EFI_SUCCESS;
}

/**
    Checks if a hardware loop back plug is attached and sets 
    the result in Parameters->SerialDevicePresentOnPort. 

    @param SerialIo 

    @retval BOOLEAN

*/

BOOLEAN
CheckForLoopbackDevice (
    IN  UINT32  BaseAddress, 
    IN  BOOLEAN MmioDevice
)
{
#if  CHECK_FOR_LOOPBACK_DEVICE
    UINT8   Byte;
    UINT8   Byte2;
    UINT8   FcrValue;


    FcrValue = SerialReadPort(BaseAddress, MmioDevice, SERIAL_REGISTER_FCR);

    //
    // Program the FCR
    //
    SerialWritePort(BaseAddress, MmioDevice, SERIAL_REGISTER_FCR, 
                            TRFIFOE|RESETRF|RESETTF);

    Byte = SerialReadPort(BaseAddress, MmioDevice, SERIAL_REGISTER_FCR);
    if(Byte == 0xff) {
        SerialWritePort(BaseAddress, MmioDevice, SERIAL_REGISTER_FCR, FcrValue); 
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
        Byte = SerialReadPort( BaseAddress, MmioDevice, SERIAL_REGISTER_LSR );
        if(Byte & DR) {
            Byte2 = SerialReadPort( BaseAddress, MmioDevice, SERIAL_REGISTER_RBR );
        }
    } while ((Byte & DR) == TRUE );

    //
    // Write into THR
    //
    SerialWritePort(BaseAddress, MmioDevice, SERIAL_REGISTER_THR,0x80); 

    //
    // Send BackSpace to clear the character(0x80) sent for testing
    // the presence of Loop Back Device.
    //
    SerialWritePort(BaseAddress, MmioDevice, SERIAL_REGISTER_THR,0x08);
    SerialWritePort(BaseAddress, MmioDevice, SERIAL_REGISTER_THR,0x20);
    SerialWritePort(BaseAddress, MmioDevice, SERIAL_REGISTER_THR,0x08);

    //
    // Wait for 5ms is sufficient for the next byte
    //
    pBS->Stall(50000);
   
    Byte = SerialReadPort( BaseAddress, MmioDevice, SERIAL_REGISTER_LSR );

    if(Byte & DR) {
        Byte2 = SerialReadPort( BaseAddress, MmioDevice, SERIAL_REGISTER_RBR );
        if(Byte2 == 0x80) {
            SerialWritePort(BaseAddress, MmioDevice, SERIAL_REGISTER_FCR, FcrValue); 
            return TRUE ;  
        }
    }

    return FALSE;
    
#else
    return FALSE;
#endif
    
}

/**
    Validate the COM port using Scratch Pad Registers. 

    @param BaseAddress 
    @param MmioDevice 

    @retval BOOLEAN
*/
BOOLEAN
ValidateComPort (
    IN  UINT32  BaseAddress, 
    IN  BOOLEAN MmioDevice
)
{
    UINT32  TempData32=0xAA;
    UINT8   TempData8=0xAA;

    if(MmioDevice) {
        //
        // Write into the Scratch Pad Reg
        //

        UINT8  MmioWidth = gAmiComParameters.MmioWidth;
        if (MmioWidth == DWORD_WIDTH) {
            *(UINT32*)(BaseAddress +(SERIAL_REGISTER_SCR*DWORD_WIDTH))=TempData32;
            TempData32=*(UINT32*)(BaseAddress +(SERIAL_REGISTER_SCR*DWORD_WIDTH));
        } else if (MmioWidth == WORD_WIDTH ) {
            *(UINT16*)(BaseAddress +(SERIAL_REGISTER_SCR*WORD_WIDTH))=(UINT16)TempData32;
            TempData32=*(UINT16*)(BaseAddress +(SERIAL_REGISTER_SCR*WORD_WIDTH));
        } else {
            *(UINT8*)(BaseAddress +(SERIAL_REGISTER_SCR*BYTE_WIDTH))=(UINT8)TempData32;
            TempData32=*(UINT8*)(BaseAddress +(SERIAL_REGISTER_SCR*BYTE_WIDTH));
        }
        //
        // Compare the read value Write Value, Both are same, Port is Valid
        //
        if((UINT8)TempData32 == 0xAA) {
            return TRUE;
        } else {
            return FALSE;
        }
    } else {
        //
        // Write into Scratch Pad ISA COM port 
        //
        IoWrite8(BaseAddress + SERIAL_REGISTER_SCR, TempData8);
        TempData8=IoRead8(BaseAddress + SERIAL_REGISTER_SCR);
        //
        // Compare the read value Write Value, Both are same, Port is Valid
        //
        if(TempData8 == 0xAA) {
            return TRUE;
        } else {
            return FALSE;
        }
    }
    return FALSE;
}

/**
    This Function is used to get the ComPort number to map Terminal Driver Setup values

    @param Device 
    @param Function 

    @retval ComPort number
*/
UINT32
GetPciSerialComPortNumber (
    IN  UINT8   Device,
    IN  UINT8   Function,
    IN  UINT8   PortNo
)
{
#if (TOTAL_PCI_SERIAL_PORTS > 0)
    UINT32      ComPort;
    UINT32      i = 0;
    UINTN       PciSerialPortsLocationVarSize = 
                                    sizeof(PCI_SERIAL_PORTS_LOCATION_VAR);
    UINT32      PciSerialPortsLocationVarAttributes=0;
    PCI_SERIAL_PORTS_LOCATION_VAR PciSerialPortsLocationVar; 
    EFI_STATUS  Status;

    UINTN SerialPortsEnabledVarSize = sizeof(SERIAL_PORTS_ENABLED_VAR); 
    UINT32 SerialPortsEnabledVarAttributes=0;
    SERIAL_PORTS_ENABLED_VAR SerialPortsEnabledVar; 

    Status = pRS->GetVariable(SERIAL_PORTS_ENABLED_VAR_C_NAME, 
                                &gTerminalVarGuid,
                                &SerialPortsEnabledVarAttributes, 
                                &SerialPortsEnabledVarSize, 
                                &SerialPortsEnabledVar);

    if(EFI_ERROR(Status)) {
        return 0xFF;
    }

    Status = pRS->GetVariable(PCI_SERIAL_PORTS_LOCATION_VAR_C_NAME, 
                                &gTerminalVarGuid, 
                                &PciSerialPortsLocationVarAttributes, 
                                &PciSerialPortsLocationVarSize, 
                                &PciSerialPortsLocationVar);

    if(EFI_ERROR(Status)) {
        return 0xFF;
    }

    ComPort = 0xFF; 

    for (i = 0; i < TOTAL_PCI_SERIAL_PORTS; i++) {
        if ((SerialPortsEnabledVar.PortsEnabled[gTotalSioSerialPorts+i]) && 
            (PciSerialPortsLocationVar.Device[i] == Device) && 
            (PciSerialPortsLocationVar.Function[i] == Function)) {
                if( PciSerialPortsLocationVar.AmiPciSerialPresent[i] ) {
                     if( PciSerialPortsLocationVar.Port[i] != PortNo ) continue;
                }
                ComPort = gTotalSioSerialPorts+i; 
                break;            
        }
    }   
    return ComPort;
#else 
    return 0xFF;
#endif
}

/**
    Skip the Invalid PCI COM device that is provided in the 
    InvalidPciComDeviceList 

    @param VendorId 
    @param DeviceId 

    @retval TRUE If the device has to be skipped
    @retval FALSE Don't Skip the device
*/
BOOLEAN
SkipInvalidPciComDevice (
    IN  UINT16 VendorId, 
    IN  UINT16 DeviceId 
)
{
    UINT8   i=0;

    while(  InvalidPciCom[i].VendorId != 0xFFFF && 
            InvalidPciCom[i].DeviceId != 0xFFFF ) {

        if( InvalidPciCom[i].VendorId == VendorId && 
            InvalidPciCom[i].DeviceId == DeviceId ) {

            return TRUE;
        }
        i++;
    }

    return FALSE;
}
    

/**
    This Function is used to get the Setup Values of Terminal Module

    @param  OUT AMI_COM_PARAMETERS  *AmiComParameters

    @retval EFI_STATUS
*/

EFI_STATUS
GetSetupValuesForLegacySredir (
    OUT AMI_COM_PARAMETERS  *AmiComParameters
)
{
#if (TOTAL_SERIAL_PORTS == 0)
    return EFI_NOT_FOUND;
#endif
    
#if (TOTAL_SERIAL_PORTS > 0)
    EFI_STATUS          Status;
    EFI_GUID            SetupGuid = SETUP_GUID; 
    SETUP_DATA          SetupData; 
    EFI_DEVICE_PATH_PROTOCOL    *SerialDevicePath = NULL;
    EFI_DEVICE_PATH_PROTOCOL    *AmiSioDevicePath = NULL;
    EFI_DEVICE_PATH_PROTOCOL    *TruncatedSerialDevicePath = NULL;
    ACPI_HID_DEVICE_PATH        *AcpiPrevDPNodePtr = NULL;
    AMI_SIO_PROTOCOL    *AmiSioProtocol=NULL;
    EFI_HANDLE          *HandleBuffer, AmiSioProtocolHandle=0;
    UINTN               HandleCount, Index;
    ASLRF_S_HDR         *Header=NULL;
    T_ITEM_LIST         *ResourcesList=NULL;
    UINT32              SetupDataAttributes = 0;
    UINTN               SetupDataSize = sizeof(SETUP_DATA); 
    UINT8               TerminalTypes[4] = {1, 2, 2, 0};
    UINT8               DataParityStopBit=0;
    UINT8               TempSetupValue;
    EFI_PCI_IO_PROTOCOL     *PciIo;
    EFI_HANDLE              *PciHandleBuffer;
    UINTN                   PciHandleCount;
    UINT8                   RevisionId[4];
    UINT64                  Supports=0;
    ASLR_QWORD_ASD          *Resources=NULL;
    EFI_DEVICE_PATH_PROTOCOL    *tmpdp=NULL;
    EFI_HANDLE              TempHandle;
    EFI_PCI_IO_PROTOCOL     *RootPciIo=NULL;
    EFI_DEVICE_PATH_PROTOCOL *TruncatedDevPath = NULL;
    EFI_DEVICE_PATH_PROTOCOL *TempDevPath = NULL;
    UINT16                  CommandRegValue; 
    UINT8                   ResIndex;
    UINT16                  CommandReg=0; 
    UINTN                   Bus=0, Device=0, Function=0, SegNum=0;
    UINT8                   Port=0;
    UINT32                  ComPort=0;
    UINT16                  VendorId,DeviceId; 
    UINT8                   MmioWidth;
    UINT16                  PciVendorId;
    UINT16                  PciDeviceId;
    UINT32                  PciVidDid;
    UINTN                   i;
#if (TOTAL_SIO_SERIAL_PORTS > 0)

    UINTN                   k;
    UINTN                   ComPortNo;

    
    UINTN                   SioSerialPortsLocationVarSize = 
                                    sizeof(SIO_SERIAL_PORTS_LOCATION_VAR);
    UINT32                  SioSerialPortsLocationVarAttributes=0;
    SIO_SERIAL_PORTS_LOCATION_VAR SioSerialPortsLocationVar;
#endif
    
#if (TOTAL_PCI_SERIAL_PORTS > 0)
    UINTN       PciSerialPortsLocationVarSize = 
                                    sizeof(PCI_SERIAL_PORTS_LOCATION_VAR);
    UINT32      PciSerialPortsLocationVarAttributes=0;
    PCI_SERIAL_PORTS_LOCATION_VAR PciSerialPortsLocationVar;
    UINT8       PciComPortforLegacy;
#endif
    
    AMI_SERIAL_PROTOCOL      *AmiSerialProtocol=NULL;
    BOOLEAN                  Pci;
    BOOLEAN                  Mmio;
    UINT64                  BaseAddress;

    if (IsFound) {
        *AmiComParameters = gAmiComParameters;
        return EFI_SUCCESS;
    }

    Status = pRS->GetVariable(L"Setup", &SetupGuid, &SetupDataAttributes,
                                &SetupDataSize, &SetupData);

    if (EFI_ERROR (Status)) {
        return EFI_NOT_FOUND;
    }

    if(SetupData.ComPortforLegacy >= TOTAL_SIO_SERIAL_PORTS)
    {
#if (TOTAL_PCI_SERIAL_PORTS > 0)
        PciComPortforLegacy = SetupData.ComPortforLegacy-TOTAL_SIO_SERIAL_PORTS;
        Status = pRS->GetVariable(PCI_SERIAL_PORTS_LOCATION_VAR_C_NAME, 
                                &gTerminalVarGuid, 
                                &PciSerialPortsLocationVarAttributes, 
                                &PciSerialPortsLocationVarSize, 
                                &PciSerialPortsLocationVar);

        if (EFI_ERROR (Status)) {
            return EFI_NOT_FOUND;
        }
#endif
   
        //
        // Handle AMI_SERIAL_PROTOCOL.
        //
        Status = pBS->LocateHandleBuffer (
                                    ByProtocol,
                                    &gAmiSerialProtocolGuid,
                                    NULL,
                                    &PciHandleCount,
                                    &PciHandleBuffer);

        for (Index = 0; Index < PciHandleCount; Index++) {
            Status = pBS->HandleProtocol (
                                    PciHandleBuffer[Index],
                                    &gAmiSerialProtocolGuid,
                                    &AmiSerialProtocol);
        
             if (EFI_ERROR (Status)) {
                continue;
             }

             AmiSerialProtocol->GetPciLocation(AmiSerialProtocol,&Bus,&Device,&Function,&Port);

             ComPort=GetPciSerialComPortNumber((UINT8)Device, (UINT8)Function,Port);

             if(ComPort==0xFF) {
                continue;
             }
             if (!SetupData.ConsoleRedirectionEnable[ComPort]) {
                 continue;
             }
#if (TOTAL_PCI_SERIAL_PORTS > 0)
             if((!PciSerialPortsLocationVar.AmiPciSerialPresent[PciComPortforLegacy]) ||
                (PciSerialPortsLocationVar.Bus[PciComPortforLegacy]!=Bus) ||
                (PciSerialPortsLocationVar.Device[PciComPortforLegacy]!=Device) ||
                (PciSerialPortsLocationVar.Function[PciComPortforLegacy]!=Function) ||
                (PciSerialPortsLocationVar.Port[PciComPortforLegacy]!=Port)) {
                continue;
             }
            
#endif
             pBS->SetMem(&gAmiComParameters, (sizeof(AMI_COM_PARAMETERS)), 0);

             AmiSerialProtocol->CheckPciMmio(AmiSerialProtocol,&Pci,&Mmio,&MmioWidth);
             AmiSerialProtocol->GetBaseAddress(AmiSerialProtocol,&BaseAddress);
             AmiSerialProtocol->GetUartClock(AmiSerialProtocol, &gUartInputClock);
            
             if (!MmioWidth) {
                 MmioWidth = 1; // default Byte Width
             }
             
             if(Mmio) {
                 gAmiComParameters.MMIOBaseAddress= (UINT32)BaseAddress;
             } else {    
                gAmiComParameters.BaseAddress= (UINT16)BaseAddress;
             }

             if(Pci) {
                 IsPciDevice = TRUE;
             } else {
                 IsPciDevice = FALSE;
             }
        
             AmiSerialProtocol->GetSerialIRQ(AmiSerialProtocol,&(gAmiComParameters.SerialIRQ));
             
             

             //
             //Other settings from Terminal Redirection driver
             //
             gAmiComParameters.Baudrate      = gComBaudRates[SetupData.BaudRate[ComPort]];
             gAmiComParameters.TerminalType  = TerminalTypes[SetupData.TerminalType[ComPort]];
             gAmiComParameters.FlowControl   = SetupData.FlowControl[ComPort];
             gAmiComParameters.LegacyOsResolution   = SetupData.LegacyOsResolution[ComPort];
             gAmiComParameters.RecorderMode   = SetupData.RecorderMode[ComPort];
             gAmiComParameters.VtUtf8   = SetupData.VtUtf8[ComPort];
             gAmiComParameters.PuttyKeyPad   = SetupData.PuttyFunctionKeyPad[ComPort];
             gAmiComParameters.MmioWidth   = MmioWidth;
#if  (INSTALL_LEGACY_OS_THROUGH_REMOTE == 1)
             gAmiComParameters.InstallLegacyOSthroughRemote  = SetupData.InstallLegacyOSthroughRemote[ComPort];
#endif
             gAmiComParameters.RedirectionAfterBiosPost = SetupData.RedirectionAfterBiosPost[ComPort] ;


             TempSetupValue=SetupData.Parity[ComPort];
                // Set parity bits.
             switch (TempSetupValue) {
                    case NoParity:
                            DataParityStopBit &= ~(PAREN | EVENPAR | STICPAR);
                            break;
                    case EvenParity:
                            DataParityStopBit |= (PAREN | EVENPAR);
                            DataParityStopBit &= ~STICPAR;
                            break;
                    case OddParity:
                            DataParityStopBit |= PAREN;
                            DataParityStopBit &= ~(EVENPAR | STICPAR);
                            break;
                    case SpaceParity:
                            DataParityStopBit |= (PAREN | EVENPAR | STICPAR);
                            break;
                    case MarkParity:
                            DataParityStopBit |= (PAREN | STICPAR);
                            DataParityStopBit &= ~EVENPAR;
                            break;
             }
    
             TempSetupValue=SetupData.StopBits[ComPort];

                // Set stop bits.
             switch (TempSetupValue) {
                    case OneStopBit :
                            DataParityStopBit &= ~STOPB;
                            break;
                     case OneFiveStopBits :
                     case TwoStopBits :
                            DataParityStopBit |= STOPB;
                            break;
             }

             TempSetupValue=SetupData.DataBits[ComPort];
   
                // Set data bits.
             DataParityStopBit &= ~SERIALDB;
             DataParityStopBit |= (UINT8)((TempSetupValue - 5) & 0x03);
             gAmiComParameters.DataParityStop= DataParityStopBit;

             IsFound = TRUE;
             *AmiComParameters = gAmiComParameters;
             return EFI_SUCCESS;
        }

        //
        // Handle PCI COM port.
        //
       
        //
        //Locate All devicepath handles
        //
        Status = pBS->LocateHandleBuffer (
                                    ByProtocol,
                                    &gEfiPciIoProtocolGuid,
                                    NULL,
                                    &PciHandleCount,
                                    &PciHandleBuffer);

        if (EFI_ERROR (Status)) {
            return Status;
        }

        for (Index = 0; Index < PciHandleCount; Index++) {
            Status = pBS->HandleProtocol (
                                    PciHandleBuffer[Index],
                                    &gEfiPciIoProtocolGuid,
                                    &PciIo); 


            PciIo->Pci.Read (PciIo,
                        EfiPciIoWidthUint32,
                        PCI_REV_ID_OFFSET,
                        1,
                        &RevisionId
                        );

            if ((RevisionId[3] == PCI_CL_COMM) &&
                ((RevisionId[2] == PCI_CL_COMM_CSL_SERIAL) || (RevisionId[2] == PCI_CL_COMM_CSL_OTHER))) {

                Status = PciIo->GetLocation(PciIo, &SegNum, &Bus,&Device,&Function);
                if (EFI_ERROR (Status)) {
                    continue;
                }
                
                PciIo->Pci.Read (PciIo,
                             EfiPciIoWidthUint16,
                             PCI_VID,
                             1,
                             &VendorId
                             );
                PciIo->Pci.Read (PciIo,
                             EfiPciIoWidthUint16,
                             PCI_DID,
                             1,
                             &DeviceId
                             );

                if(SkipInvalidPciComDevice(VendorId,DeviceId)) {
                    continue;
                }

                ComPort=GetPciSerialComPortNumber((UINT8)Device, (UINT8)Function, Port);
                
                if(ComPort==0xFF) {
                    continue;
                }
                if (!SetupData.ConsoleRedirectionEnable[ComPort]) {
                    continue;
                }
#if (TOTAL_PCI_SERIAL_PORTS > 0)

                if((PciSerialPortsLocationVar.Segment[PciComPortforLegacy]!=SegNum) ||
                   (PciSerialPortsLocationVar.Bus[PciComPortforLegacy]!=Bus) ||
                   (PciSerialPortsLocationVar.Device[PciComPortforLegacy]!=Device) ||
                   (PciSerialPortsLocationVar.Function[PciComPortforLegacy]!=Function)||
                   (PciSerialPortsLocationVar.Port[PciComPortforLegacy]!=Port)) {
                    continue;
                   }
#endif
                Supports = 1;

                pBS->SetMem(&gAmiComParameters, (sizeof(AMI_COM_PARAMETERS)), 0);

                for (ResIndex = 0; ResIndex < PCI_MAX_BAR_NO; ResIndex++) {
                    Status = PciIo->GetBarAttributes (
                                                PciIo,
                                                ResIndex,
                                                &Supports,
                                                &Resources
                                                );

                    //
                    //Find the Serial device Resource type. Based on that Enable the IO and Memory 
                    //
                    if (Resources->Type == ASLRV_SPC_TYPE_IO) {
                        gAmiComParameters.BaseAddress= (UINT16)Resources->_MIN;    
                        Status = PciIo->Pci.Read(PciIo, EfiPciIoWidthUint8, 0x3c, 1, &gAmiComParameters.SerialIRQ);
                        CommandReg = PCI_CMD_IO_SPACE;
                        pBS->FreePool(Resources);
                        break;
                    } else if(Resources->Type == ASLRV_SPC_TYPE_MEM) {
                        gAmiComParameters.MMIOBaseAddress= (UINT32)Resources->_MIN;    
                        Status = PciIo->Pci.Read(PciIo, EfiPciIoWidthUint8, 0x3c, 1, &gAmiComParameters.SerialIRQ);
                        CommandReg = PCI_CMD_MEMORY_SPACE;
                        pBS->FreePool(Resources);
                        break;
                    }
                }

                if(CommandReg != 0) {

                    PciIo->Pci.Read (PciIo,
                                EfiPciIoWidthUint16,
                                PCI_COMMAND_REGISTER_OFFSET,
                                1,
                                &CommandRegValue
                                );
                    CommandRegValue|=CommandReg;
                    PciIo->Pci.Write (PciIo,
                                EfiPciIoWidthUint16,
                                PCI_COMMAND_REGISTER_OFFSET,
                                1,
                                &CommandRegValue
                                );

                    Status = pBS->HandleProtocol (
                                    PciHandleBuffer[Index],
                                    &gEfiDevicePathProtocolGuid,
                                    &tmpdp); 


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
                                                    &RootPciIo); // Get Device path protocol

                            if(Status == EFI_SUCCESS) {

                                RootPciIo->Pci.Read (RootPciIo,
                                                    EfiPciIoWidthUint16,
                                                    PCI_COMMAND_REGISTER_OFFSET,
                                                    1,
                                                    &CommandRegValue
                                                    );
                                CommandRegValue|=CommandReg;
                                RootPciIo->Pci.Write (RootPciIo,
                                                    EfiPciIoWidthUint16,
                                                    PCI_COMMAND_REGISTER_OFFSET,
                                                    1,
                                                    &CommandRegValue
                                                    );
                            }
                
                        }
                
                    }while(TruncatedDevPath != NULL);
                    
                    Status = PciIo->Pci.Read (
                                                   PciIo,
                                                   EfiPciIoWidthUint32,
                                                   PCI_VID,
                                                   1,
                                                   &PciVidDid
                                                   );
                    if (!EFI_ERROR (Status)) {
                        PciVendorId = (UINT16)(PciVidDid & 0xffff);
                        PciDeviceId = (UINT16)(PciVidDid >> 16);
                    }
                     
                    for( i = 0; gPciDeviceUartClockAndWidth[i].VendorID != 0xFFFF ; i++ ) {
                        if( (gPciDeviceUartClockAndWidth[i].VendorID == PciVendorId) && (gPciDeviceUartClockAndWidth[i].DeviceID == PciDeviceId) ) {
                            gUartInputClock = gPciDeviceUartClockAndWidth[i].UartClock;
                            gAmiComParameters.MmioWidth = gPciDeviceUartClockAndWidth[i].MmioWidth;
                            break;
                        }
                    }
                    
                    if ( !gAmiComParameters.MmioWidth ) {
                        gAmiComParameters.MmioWidth = 1;
                    }

                    //
                    // Check the Port Presence
                    //
                    if(gAmiComParameters.MMIOBaseAddress != 0) {
                        if(ValidateComPort(gAmiComParameters.MMIOBaseAddress, TRUE) == FALSE) {
                            continue;
                        }
                        if(CheckForLoopbackDevice(gAmiComParameters.MMIOBaseAddress, TRUE) == TRUE) {
                            continue;
                        }
                    } else {
                        if(ValidateComPort(gAmiComParameters.BaseAddress, FALSE) == FALSE) {
                            continue;
                        }
                        if(CheckForLoopbackDevice(gAmiComParameters.BaseAddress, FALSE) == TRUE) {
                            continue;
                        }
                    }

                    //
                    //Other settings from Terminal Redirection driver
                    //
                    gAmiComParameters.Baudrate      = gComBaudRates[SetupData.BaudRate[ComPort]];
                    gAmiComParameters.TerminalType  = TerminalTypes[SetupData.TerminalType[ComPort]];
                    gAmiComParameters.FlowControl   = SetupData.FlowControl[ComPort];
                    gAmiComParameters.LegacyOsResolution   = SetupData.LegacyOsResolution[ComPort];
                    gAmiComParameters.RecorderMode   = SetupData.RecorderMode[ComPort];
                    gAmiComParameters.VtUtf8   = SetupData.VtUtf8[ComPort];
                    gAmiComParameters.PuttyKeyPad   = SetupData.PuttyFunctionKeyPad[ComPort];
#if  (INSTALL_LEGACY_OS_THROUGH_REMOTE == 1)
                    gAmiComParameters.InstallLegacyOSthroughRemote  = SetupData.InstallLegacyOSthroughRemote[ComPort];
#endif
                    gAmiComParameters.RedirectionAfterBiosPost = SetupData.RedirectionAfterBiosPost[ComPort] ;
                    TempSetupValue=SetupData.Parity[ComPort];
                    // Set parity bits.
                    switch (TempSetupValue) {
                        case NoParity:
                                DataParityStopBit &= ~(PAREN | EVENPAR | STICPAR);
                                break;
                        case EvenParity:
                                DataParityStopBit |= (PAREN | EVENPAR);
                                DataParityStopBit &= ~STICPAR;
                                break;
                        case OddParity:
                                DataParityStopBit |= PAREN;
                                DataParityStopBit &= ~(EVENPAR | STICPAR);
                                break;
                        case SpaceParity:
                                DataParityStopBit |= (PAREN | EVENPAR | STICPAR);
                                break;
                        case MarkParity:
                                DataParityStopBit |= (PAREN | STICPAR);
                                DataParityStopBit &= ~EVENPAR;
                                break;
                    }
    
                    TempSetupValue=SetupData.StopBits[ComPort];

                    // Set stop bits.
                    switch (TempSetupValue) {
                        case OneStopBit :
                                DataParityStopBit &= ~STOPB;
                                break;
                         case OneFiveStopBits :
                         case TwoStopBits :
                                DataParityStopBit |= STOPB;
                                break;
                    }

                    TempSetupValue=SetupData.DataBits[ComPort];

                    // Set data bits.
                    DataParityStopBit &= ~SERIALDB;
                    DataParityStopBit |= (UINT8)((TempSetupValue - 5) & 0x03);
                    gAmiComParameters.DataParityStop= DataParityStopBit;

                    IsFound = TRUE;
                    IsPciDevice = TRUE;
                    *AmiComParameters = gAmiComParameters;
                    return EFI_SUCCESS;
                }
            }
        }
    } //else {    // Supermicro -
    
        //
        // Handle ISA COM port.
        //


        //
        //Locate All devicepath handles
        //
        Status = pBS->LocateHandleBuffer (
                                    ByProtocol,
                                    &gEfiDevicePathProtocolGuid,
                                    NULL,
                                    &HandleCount,
                                    &HandleBuffer);

        if (EFI_ERROR (Status)) {
            return Status;
        }
    
#if (TOTAL_SIO_SERIAL_PORTS > 0)
        Status = pRS->GetVariable(SIO_SERIAL_PORTS_LOCATION_VAR_C_NAME, 
                                &gTerminalVarGuid, 
                                &SioSerialPortsLocationVarAttributes,
                                &SioSerialPortsLocationVarSize, 
                                &SioSerialPortsLocationVar);

        if (EFI_ERROR (Status)) {
            return Status;
        }

        for (Index = 0; Index < HandleCount; Index++) {
            Status = pBS->HandleProtocol (
                                    HandleBuffer[Index],
                                    &gEfiDevicePathProtocolGuid,
                                    &SerialDevicePath); // Get Device path protocol

        if (EFI_ERROR (Status) || (SerialDevicePath == NULL)) continue;

            TruncatedSerialDevicePath = SerialDevicePath;
            //
            //Check for the Serial Port device path
            //
            for (;!isEndNode(TruncatedSerialDevicePath);TruncatedSerialDevicePath = NEXT_NODE(TruncatedSerialDevicePath)) {
                AcpiPrevDPNodePtr = (ACPI_HID_DEVICE_PATH *)(TruncatedSerialDevicePath); //get ACPI device path
                if ((AcpiPrevDPNodePtr->Header.Type == ACPI_DEVICE_PATH) 
                    && (AcpiPrevDPNodePtr->Header.SubType == ACPI_DP)
                    && (AcpiPrevDPNodePtr->HID == EISA_PNP_ID(0x501))) {

                    if(TOTAL_SIO_SERIAL_PORTS == 0) {
                        continue;
                    }

                    //
                    // Find the Port number ( Setup option offset)
                    //
                    for (k = 0; k < TOTAL_SIO_SERIAL_PORTS; k++) {
                        if ((SioSerialPortsLocationVar.PortUid[k] == (AcpiPrevDPNodePtr->UID) ) &&
                            //(SioSerialPortsLocationVar.Valid[k] == 0xFF) &&    // Supermicro -
                            //(k == SetupData.ComPortforLegacy)) {               // Supermicro -
                            (SioSerialPortsLocationVar.Valid[k] == 0xFF)) {      // Supermicro +
                            ComPortNo = k; 
                            break;
                        }
                    }

                    if(k == TOTAL_SIO_SERIAL_PORTS) {
                        continue;
                    }
                    //
                    //Check Terminal redirection device status.
                    //
                    if (!SetupData.ConsoleRedirectionEnable[ComPortNo]) continue;

                    //
                    //Locate AmiSioProtocol form this handle to get current resource of this device.
                    //
                    AmiSioDevicePath = SerialDevicePath;        //Truncate End device path

                    Status = pBS->LocateDevicePath(&gEfiDevicePathProtocolGuid, 
                                               &AmiSioDevicePath, 
                                               &AmiSioProtocolHandle);

                    if (EFI_ERROR (Status)) continue;

                    Status = pBS->HandleProtocol (
                                                AmiSioProtocolHandle,
                                                &gEfiAmiSioProtocolGuid,
                                                &AmiSioProtocol);

                    if (EFI_ERROR (Status) || (AmiSioProtocol == NULL)) continue;

                    Status = AmiSioProtocol->CurrentRes(AmiSioProtocol, 
                                                    FALSE, 
                                                    &ResourcesList);
                    if (EFI_ERROR(Status)) continue;
                
                    if(ResourcesList){
                        for(i=0; i<ResourcesList->ItemCount; i++){
                            Header=(ASLRF_S_HDR*)ResourcesList->Items[i];
                            switch(Header->Name) {
                                case ASLV_RT_FixedIO: 
                                    gAmiComParameters.BaseAddress=((ASLR_FixedIO*)Header)->_BAS;
                                    break;
                                case ASLV_RT_IO: 
                                    gAmiComParameters.BaseAddress=((ASLR_IO*)Header)->_MIN;
                                    break;
                                case ASLV_RT_IRQ:
                                    gAmiComParameters.SerialIRQ = (UINT8)((ASLR_IRQNoFlags*)Header)->_INT;
                                    break; 
                            }
                        }
                    }
                    if(ValidateComPort(gAmiComParameters.BaseAddress, FALSE) == FALSE) {
                        continue;
                    }
                
                    if(CheckForLoopbackDevice(gAmiComParameters.BaseAddress, FALSE) == TRUE) {
                        continue;
                    }
                    //
                    //Other settings from Terminal Redirection driver
                    //
                    gAmiComParameters.Baudrate      = gComBaudRates[SetupData.BaudRate[ComPortNo]];
                    gAmiComParameters.TerminalType  = TerminalTypes[SetupData.TerminalType[ComPortNo]];
                    gAmiComParameters.FlowControl   = SetupData.FlowControl[ComPortNo];
                    gAmiComParameters.LegacyOsResolution   = SetupData.LegacyOsResolution[ComPortNo];
                    gAmiComParameters.RecorderMode   = SetupData.RecorderMode[ComPortNo];
                    gAmiComParameters.VtUtf8   = SetupData.VtUtf8[ComPortNo];
                    gAmiComParameters.PuttyKeyPad   = SetupData.PuttyFunctionKeyPad[ComPortNo];
#if  (INSTALL_LEGACY_OS_THROUGH_REMOTE == 1)
                    gAmiComParameters.InstallLegacyOSthroughRemote  = SetupData.InstallLegacyOSthroughRemote[ComPortNo];
#endif
#ifdef UART_INPUT_CLOCK
                    gUartInputClock = UART_INPUT_CLOCK;
#endif
                    gAmiComParameters.RedirectionAfterBiosPost = SetupData.RedirectionAfterBiosPost[ComPortNo] ;
                    TempSetupValue=SetupData.Parity[ComPortNo];
                    // Set parity bits.
                    switch (TempSetupValue) {
                        case NoParity:
                            DataParityStopBit &= ~(PAREN | EVENPAR | STICPAR);
                            break;
                        case EvenParity:
                            DataParityStopBit |= (PAREN | EVENPAR);
                            DataParityStopBit &= ~STICPAR;
                            break;
                        case OddParity:
                            DataParityStopBit |= PAREN;
                            DataParityStopBit &= ~(EVENPAR | STICPAR);
                            break;
                        case SpaceParity:
                            DataParityStopBit |= (PAREN | EVENPAR | STICPAR);
                            break;
                        case MarkParity:
                            DataParityStopBit |= (PAREN | STICPAR);
                            DataParityStopBit &= ~EVENPAR;
                            break;
                    }
    
                    TempSetupValue=SetupData.StopBits[ComPortNo];

                    // Set stop bits.
                    switch (TempSetupValue) {
                        case OneStopBit :
                            DataParityStopBit &= ~STOPB;
                            break;
                        case OneFiveStopBits :
                        case TwoStopBits :
                            DataParityStopBit |= STOPB;
                            break;
                    }

                    TempSetupValue=SetupData.DataBits[ComPortNo];
   
                    // Set data bits.
                    DataParityStopBit &= ~SERIALDB;
                    DataParityStopBit |= (UINT8)((TempSetupValue - 5) & 0x03);
                    gAmiComParameters.DataParityStop= DataParityStopBit;

                    IsFound = TRUE;
                    IsPciDevice = FALSE;
                    *AmiComParameters = gAmiComParameters;
                      return EFI_SUCCESS;
                }
            }
        }
#else
    return EFI_NOT_FOUND;
#endif
//    }    // Supermicro -

    if (!IsFound) {
        return EFI_NOT_FOUND;
    }
    *AmiComParameters = gAmiComParameters;
    return EFI_SUCCESS;
#endif
}

//
//TERMINAL TYPE:
//              db      0               ;ANSI
//              db      1               ;VT100
//              db      2               ;VT-UTF8
//FLOWCONTROLlIST:
//              db      0               ;none
//              db      1               ;hardware Flow control
//              db      2               ;software

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
