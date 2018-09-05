//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//  Rev. 1.00
//    Bug Fix:  Fixed COM2/SOL console redirection can't work after update BIOS.
//    Reason:	When system after update BIOS, the serial port UID will miss.
//              Add workaround for UID confuse problem when first boot.  
//    Auditor:  Jimmy Chiu (Refer GreenllowRefresh Workstation - SVN_2572)
//    Date:     Mar/28/2017
//
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file TerminalSetup.c
    This file contains Porting functions and setup values for the 
    Terminal and SerialIo protocol

**/

//---------------------------------------------------------------------------
#include <Token.h>
#include <AmiDxeLib.h>
#include "TerminalBoard.h"
#include <Protocol/SerialIo.h>
#include <Setup.h>
#include "TerminalSetupVar.h"
#include <Protocol/PciRootBridgeIo.h>
#include <Pci.h>

//---------------------------------------------------------------------------

#define PCI_CFG_ADDRESS(bus,dev,func,reg) \
    ((UINT64)((((UINTN)bus) << 24) + (((UINTN)dev) << 16) + (((UINTN)func) << 8) + ((UINTN)reg)))& 0x00000000ffffffff

#define HARDWARE_LOOPBACK       1
#define SOFTWARE_LOOPBACK       2



BOOLEAN ValidateNvram                   = FALSE;

UINT32 TimeoutForDeterminingLoneEscChar = 
                                    TIMEOUT_FOR_DETERMINING_LONE_ESC_CHAR;

UINT32 EscSequenceCheckingIntervel = 
                                    (TIMEOUT_FOR_DETERMINING_LONE_ESC_CHAR/NUMBER_OF_TIME_FOR_ESC_SEQUENCE_CHECKING);

CHAR16 RefreshScreenKey     = REFRESH_SCREEN_KEY;
UINT8 SPCR_OEM_ID[6]     = CONVERT_TO_STRING(SPCR_ACPI_OEM_ID);
UINT8 SPCR_OEM_TBL_ID[8] = CONVERT_TO_STRING(SPCR_ACPI_OEM_TBL_ID);
UINT32 gTotalSioSerialPorts = TOTAL_SIO_SERIAL_PORTS;
UINT32 gTotalPciSerialPorts = TOTAL_PCI_SERIAL_PORTS;
UINTN  TotalTerminalDev=TOTAL_SERIAL_PORTS;
UINT32 Uart_Fifo_Size=UART_FIFO_SIZE;
BOOLEAN ClearTerminalKBBufferReadyToBoot= CLEAR_TERMINAL_KB_BUFFER_AT_READYTOBOOT;
BOOLEAN ASCIIControlCodeSupport= ASCII_CONTROL_CODE_SUPPORT;

EFI_GUID gTerminalVarGuid   = TERMINAL_VAR_GUID;
EFI_GUID gSetupGuid         = SETUP_GUID;
EFI_GUID gDebuggerTerminalVarGuid   = DEBUGGER_TERMINAL_VAR_GUID;
EFI_GUID  gEfiPciRootBridgeIoProtocolGuid = EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_GUID;

UINT64 gAcpiSpcrTableComBaudRates[8] =
                                {0, 0, 0, 9600, 19200, 38400, 57600, 115200};

CHAR16 gPciSerialPortsDevicePathVarName[MAX_PCI_SERIAL_PORTS][6];

SETUP_DATA gSetupData;

//Terminal Driver global variables for SDL tokens.

/**
    Variable to replace DONT_SEND_ASCII_CONTROL_PRINTABLE_CHARACTERS token.

    @retval BOOLEAN

**/

BOOLEAN  Dont_Send_Ascii_Control_Printable_Characters =
#ifdef DONT_SEND_ASCII_CONTROL_PRINTABLE_CHARACTERS
    DONT_SEND_ASCII_CONTROL_PRINTABLE_CHARACTERS
#else
    0
#endif
    ;

/**
    Variable to replace spcr_interface_type token.

@retval UINT8

**/

UINT8 SpcrInterfaceType =
#ifdef spcr_interface_type
    spcr_interface_type
#else
    0
#endif
    ;
/**
    Variable to replace spcr_addr_spc_id token.

    @retval UINT8

**/

UINT8 SpcrAddrSpcId =
#ifdef spcr_addr_spc_id
    spcr_addr_spc_id
#else
    0
#endif
    ;

/**
    Variable to replace spcr_addr_bit_width token.

    @retval UINT8

**/

UINT8 SpcrAddrBitWidth =
#ifdef spcr_addr_bit_width
    spcr_addr_bit_width
#else
    0
#endif
    ;

/**
    Variable to replace spcr_addr_bit_offset token.

    @retval UINT8	

**/

UINT8 SpcrAddrBitOffset =
#ifdef spcr_addr_bit_offset
    spcr_addr_bit_offset
#else
    0
#endif
    ;

/**
    Variable to replace spcr_access_size token.

    @retval UINT8

**/

UINT8 SpcrAccessSize =
#ifdef spcr_access_size
    spcr_access_size
#else
    0
#endif
    ;

/**
    Variable to replace spcr_interrupt_type token.

    @retval UINT8	

**/

UINT8 SpcrInterruptType =
#ifdef spcr_interrupt_type
    spcr_interrupt_type
#else
    0
#endif
    ;
/**
    Variable to replace spcr_parity token.

    @retval UINT8

**/
UINT8 SpcrParity =
#ifdef spcr_parity
    spcr_parity
#else
    0
#endif
    ;

/**
    Variable to replace spcr_stop_bits token.

    @retval UINT8

**/

UINT8 SpcrStopBits =
#ifdef spcr_stop_bits
    spcr_stop_bits
#else
    0
#endif
    ;

/**
    Variable to replace spcr_pci_device_id token.

    @retval UINT16

**/

UINT16 SpcrPciDeviceId =
#ifdef spcr_pci_device_id
    spcr_pci_device_id
#else
    0
#endif
    ;

/**
    Variable to replace spcr_pci_vendor_id token.

    @retval UINT16

**/

UINT16 SpcrPciVendorId =
#ifdef spcr_pci_vendor_id
    spcr_pci_vendor_id
#else
    0
#endif
    ;

/**
    Variable to replace spcr_pci_bus_number token.

    @retval UINT8

**/

UINT8 SpcrPciBusNumber =
#ifdef spcr_pci_bus_number
    spcr_pci_bus_number
#else
    0
#endif
    ;

/**
    Variable to replace spcr_interface_type token.

    @retval UINT8

**/

UINT8 SpcrPciDeviceNumber =
#ifdef spcr_pci_device_number
    spcr_pci_device_number
#else
    0
#endif
    ;

/**
    Variable to replace spcr_pci_function_number token.

    @retval UINT8

**/

UINT8 SpcrPciFunctionNumber =
#ifdef spcr_pci_function_number
    spcr_pci_function_number
#else
    0
#endif
    ;

/**
    Variable to replace spcr_pci_flags token.

    @retval UINT8

**/

UINT8 SpcrPciFlags =
#ifdef spcr_pci_flags
    spcr_pci_flags
#else
    0
#endif
    ;

/**
    Variable to replace spcr_pci_segment token.

    @retval UINT8

**/

UINT8 SpcrPciSegment =
#ifdef spcr_pci_segment
    spcr_pci_segment
#else
    0
#endif
    ;

/**
    Variable to replace ACPI_OEM_REV token.

    @retval UINT64

**/

UINT64 AcpiOemRev =
#ifdef ACPI_OEM_REV
    ACPI_OEM_REV
#else
    0
#endif
    ;

/**
    Variable to replace with CORE_COMBINED_VERSION token.

    @retval UINT32

**/

UINT32 CoreRevision =
#ifdef CORE_COMBINED_VERSION
     CORE_COMBINED_VERSION
#else
    0
#endif
    ;

/**
    Initializes the variables SERIAL_PORTS_ENABLED_VAR and
    PCI_SERIAL_PORTS_LOCATION_VAR.
    SERIAL_PORTS_ENABLED_VAR keeps track of which serial ports
    (both SuperIO and PCI) are enabled.
    PCI_SERIAL_PORTS_LOCATION_VAR keeps contains location
    information about the pci serial ports (device and function
    numbers).

    @retval VOID

**/

VOID
InitSerialPortsEnabledVar (VOID)
{
    UINTN DebuggerSerialPortsEnabledVarSize = sizeof(DEBUGGER_SERIAL_PORTS_ENABLED_VAR);
    DEBUGGER_SERIAL_PORTS_ENABLED_VAR DebuggerSerialPortsEnabledVar;
    EFI_STATUS Status;

#if (TOTAL_SERIAL_PORTS > 0)

    UINTN SerialPortsEnabledVarSize = sizeof(SERIAL_PORTS_ENABLED_VAR);
    SERIAL_PORTS_ENABLED_VAR SerialPortsEnabledVar;

#if (TOTAL_PCI_SERIAL_PORTS > 0)
    UINTN PciSerialPortsLocationVarSize =
                                    sizeof(PCI_SERIAL_PORTS_LOCATION_VAR);
    PCI_SERIAL_PORTS_LOCATION_VAR PciSerialPortsLocationVar;
#endif
#if (TOTAL_SIO_SERIAL_PORTS > 0)
    UINTN SioSerialPortsLocationVarSize =
                                    sizeof(SIO_SERIAL_PORTS_LOCATION_VAR);
    SIO_SERIAL_PORTS_LOCATION_VAR SioSerialPortsLocationVar;
#endif

    pBS->SetMem(&SerialPortsEnabledVar,
                sizeof(SERIAL_PORTS_ENABLED_VAR),
                0);
    Status = pRS->SetVariable(SERIAL_PORTS_ENABLED_VAR_C_NAME,
                                &gTerminalVarGuid,
                                EFI_VARIABLE_BOOTSERVICE_ACCESS,
                                SerialPortsEnabledVarSize,
                                &SerialPortsEnabledVar);
    ASSERT_EFI_ERROR(Status);

#if (TOTAL_SIO_SERIAL_PORTS > 0)
    Status = pRS->GetVariable(SIO_SERIAL_PORTS_LOCATION_VAR_C_NAME,
                                &gTerminalVarGuid,
                                NULL,
                                &SioSerialPortsLocationVarSize,
                                &SioSerialPortsLocationVar);
    if (EFI_ERROR(Status)) {
        pBS->SetMem(&SioSerialPortsLocationVar,
                    sizeof(SIO_SERIAL_PORTS_LOCATION_VAR),
                    0);
        Status = pRS->SetVariable(SIO_SERIAL_PORTS_LOCATION_VAR_C_NAME,
                                    &gTerminalVarGuid,
                                    (EFI_VARIABLE_BOOTSERVICE_ACCESS |
                                            EFI_VARIABLE_NON_VOLATILE),
                                    SioSerialPortsLocationVarSize,
                                    &SioSerialPortsLocationVar);
        ASSERT_EFI_ERROR(Status);
    }
#endif

#if (TOTAL_PCI_SERIAL_PORTS > 0)
    Status = pRS->GetVariable(PCI_SERIAL_PORTS_LOCATION_VAR_C_NAME,
                                &gTerminalVarGuid,
                                NULL,
                                &PciSerialPortsLocationVarSize,
                                &PciSerialPortsLocationVar);
    if (EFI_ERROR(Status)) {
        pBS->SetMem(&PciSerialPortsLocationVar,
                    sizeof(PCI_SERIAL_PORTS_LOCATION_VAR),
                    0);
        Status = pRS->SetVariable(PCI_SERIAL_PORTS_LOCATION_VAR_C_NAME,
                                    &gTerminalVarGuid,
                                    (EFI_VARIABLE_BOOTSERVICE_ACCESS |
                                            EFI_VARIABLE_NON_VOLATILE),
                                    PciSerialPortsLocationVarSize,
                                    &PciSerialPortsLocationVar);
        ASSERT_EFI_ERROR(Status);
    }
#endif

#endif
    //
    // Initilize the Amidebugger Serial Port variables.
    //
    pBS->SetMem(&DebuggerSerialPortsEnabledVar, 
                sizeof(DEBUGGER_SERIAL_PORTS_ENABLED_VAR),
                0);
    Status = pRS->SetVariable(DEBUGGER_SERIAL_PORTS_ENABLED_VAR_C_NAME,
                                &gDebuggerTerminalVarGuid,
                                EFI_VARIABLE_BOOTSERVICE_ACCESS,
                                DebuggerSerialPortsEnabledVarSize,
                                &DebuggerSerialPortsEnabledVar);
    ASSERT_EFI_ERROR(Status);
}

/**
    Sets SERIAL_PORTS_ENABLED_VAR[Port] to 1.

    @param  Uid

    @retval UINT8

**/

UINT8
SetSerialPortsEnabledVar_Sio (
    IN    UINT8  Uid
)
{
    UINTN DebuggerSerialPortsEnabledVarSize = sizeof(DEBUGGER_SERIAL_PORTS_ENABLED_VAR);
    UINT32 DebuggerSerialPortsEnabledVarAttributes=0;
    DEBUGGER_SERIAL_PORTS_ENABLED_VAR DebuggerSerialPortsEnabledVar;
    EFI_STATUS Status = EFI_SUCCESS;

    if(Uid == 0xFF) {

        //
        // Handle the Serial Port that is coming from Ami debugger
        //
        Status = pRS->GetVariable(DEBUGGER_SERIAL_PORTS_ENABLED_VAR_C_NAME,
                                &gDebuggerTerminalVarGuid,
                                &DebuggerSerialPortsEnabledVarAttributes,
                                &DebuggerSerialPortsEnabledVarSize,
                                &DebuggerSerialPortsEnabledVar);
        ASSERT_EFI_ERROR(Status);

        //
        // Port Present Status.
        //
        DebuggerSerialPortsEnabledVar.PortEnabled=1;

        Status = pRS->SetVariable(DEBUGGER_SERIAL_PORTS_ENABLED_VAR_C_NAME,
                                &gDebuggerTerminalVarGuid,
                                EFI_VARIABLE_BOOTSERVICE_ACCESS,
                                DebuggerSerialPortsEnabledVarSize,
                                &DebuggerSerialPortsEnabledVar);
        ASSERT_EFI_ERROR(Status);
        //
        // Return 0xFF, so that it's known as this serial port is coming
        // from AmiDebugger
        //
        return 0xFF;
    }

#if (TOTAL_SIO_SERIAL_PORTS > 0)
{

    UINTN SerialPortsEnabledVarSize = sizeof(SERIAL_PORTS_ENABLED_VAR);
    UINT32 SerialPortsEnabledVarAttributes=0;
    SERIAL_PORTS_ENABLED_VAR SerialPortsEnabledVar;

    UINTN SioSerialPortsLocationVarSize =
                                    sizeof(SIO_SERIAL_PORTS_LOCATION_VAR);
    UINT32 SioSerialPortsLocationVarAttributes=0;
    SIO_SERIAL_PORTS_LOCATION_VAR SioSerialPortsLocationVar;
    EFI_STATUS Status = EFI_SUCCESS;
    UINT32 i = 0;
    UINT8 ComPort = 0xFF;

    Status = pRS->GetVariable(SERIAL_PORTS_ENABLED_VAR_C_NAME,
                                &gTerminalVarGuid,
                                &SerialPortsEnabledVarAttributes,
                                &SerialPortsEnabledVarSize,
                                &SerialPortsEnabledVar);
    ASSERT_EFI_ERROR(Status);

    if(EFI_ERROR(Status)) {
        return 0xFF;
    }

    Status = pRS->GetVariable(SIO_SERIAL_PORTS_LOCATION_VAR_C_NAME,
                                &gTerminalVarGuid,
                                &SioSerialPortsLocationVarAttributes,
                                &SioSerialPortsLocationVarSize,
                                &SioSerialPortsLocationVar);
    ASSERT_EFI_ERROR(Status);

    if(EFI_ERROR(Status)) {
        return 0xFF;
    }

    for (i = 0; i < TOTAL_SIO_SERIAL_PORTS; i++) {
        if ((SioSerialPortsLocationVar.PortUid[i] == Uid ) &&
            (SioSerialPortsLocationVar.Valid[i] == 0xFF)) {
            SerialPortsEnabledVar.PortsEnabled[i] = 1;
            ComPort = i;
            break;
        }
    }

    if (i == TOTAL_SIO_SERIAL_PORTS) {
        for (i = 0; i < TOTAL_SIO_SERIAL_PORTS; i++) {
            if ((!SerialPortsEnabledVar.PortsEnabled[i]) &&
                (SioSerialPortsLocationVar.Valid[i] == 0)) {
#if SMCPKG_SUPPORT
                if( i != Uid ) continue;
#endif //SMCPKG_SUPPORT
                SerialPortsEnabledVar.PortsEnabled[i] = 1;
                ComPort = i; 

                SioSerialPortsLocationVar.PortUid[i] = Uid;
                SioSerialPortsLocationVar.Valid[i] = 0xFF;

                Status = pRS->SetVariable(
                                    SIO_SERIAL_PORTS_LOCATION_VAR_C_NAME,
                                    &gTerminalVarGuid,
                                    (EFI_VARIABLE_BOOTSERVICE_ACCESS |
                                            EFI_VARIABLE_NON_VOLATILE),
                                    SioSerialPortsLocationVarSize,
                                    &SioSerialPortsLocationVar);

                ASSERT_EFI_ERROR(Status);
                break;
            }
        }
    }

    Status = pRS->SetVariable(SERIAL_PORTS_ENABLED_VAR_C_NAME,
                                &gTerminalVarGuid,
                                EFI_VARIABLE_BOOTSERVICE_ACCESS,
                                SerialPortsEnabledVarSize,
                                &SerialPortsEnabledVar); 
    ASSERT_EFI_ERROR(Status);

    return ComPort;
}
#else

    return 0xFF;

#endif
}

#if (TOTAL_PCI_SERIAL_PORTS > 0)
/**
    Checks for the Presence of PCI Serial device with values
    present in the NVRAM Variable PciSerialPortsLocationVar.
    If found donot change NVRAM variable.If not found delete
    the NVRAM variable describing the device path  and make
    current PciSerialPortsLocationVar.Bus[i],Device[i] and
    function[i] to zero.

    @param  SerialPortsEnabledVar
    @param  DevicePath

    @retval EFI_STATUS

**/

EFI_STATUS
ValidatePciSerialNvram (
    IN SERIAL_PORTS_ENABLED_VAR SerialPortsEnabledVar,
    IN EFI_DEVICE_PATH_PROTOCOL *DevicePath
)
{
    UINT64      PciAddress;
    UINT8       RevisionId[4];
    UINT8       SetNvramVariable=0;
    UINT8       i = 0;
    EFI_STATUS  Status;

    UINTN PciSerialPortsLocationVarSize = sizeof(PCI_SERIAL_PORTS_LOCATION_VAR);
    UINT32 PciSerialPortsLocationVarAttributes=0;
    PCI_SERIAL_PORTS_LOCATION_VAR PciSerialPortsLocationVar;

    UINTN PciSerialPortDevicePathVarSize = 0;
    UINT32 PciSerialPortDevicePathVarAttributes = 0;
    EFI_DEVICE_PATH_PROTOCOL *PciSerialPortDevicePathVar = NULL;

    EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL	*gPciRootBridgeIo=NULL;

    if (ValidateNvram == TRUE) {
        return EFI_SUCCESS;
    }

    Status = pBS->LocateProtocol( &gEfiPciRootBridgeIoProtocolGuid,
                                  NULL,
                                  (VOID**)&gPciRootBridgeIo );

    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status)) {
        return Status;
    }

    ValidateNvram = TRUE;
    Status = pRS->GetVariable(PCI_SERIAL_PORTS_LOCATION_VAR_C_NAME,
                              &gTerminalVarGuid, 
                              &PciSerialPortsLocationVarAttributes,
                              &PciSerialPortsLocationVarSize,
                              &PciSerialPortsLocationVar );
    if(EFI_ERROR(Status)) {
        return Status;
    }

    ASSERT_EFI_ERROR(Status);

    for (i = 0; i < TOTAL_PCI_SERIAL_PORTS; i++) {
        if (!SerialPortsEnabledVar.PortsEnabled[gTotalSioSerialPorts+i]) {
            if ((PciSerialPortsLocationVar.Bus[i] == 0) &&
                (PciSerialPortsLocationVar.Device[i] == 0) &&
                (PciSerialPortsLocationVar.Function[i] == 0)) {
                continue;
            }

            PciAddress = PCI_CFG_ADDRESS (PciSerialPortsLocationVar.Bus[i],
                                          PciSerialPortsLocationVar.Device[i],
                                          PciSerialPortsLocationVar.Function[i],
                                          PCI_REV_ID_OFFSET
                                          );
            //
            //Get the Class Code
            //
            Status = gPciRootBridgeIo->Pci.Read(
                                        gPciRootBridgeIo,
                                        EfiPciWidthUint32,
                                        PciAddress,
                                        1,
                                        &RevisionId);
            ASSERT_EFI_ERROR(Status);

            Status = GetEfiVariable(gPciSerialPortsDevicePathVarName[i],
                                    &gTerminalVarGuid,
                                    &PciSerialPortDevicePathVarAttributes,
                                    &PciSerialPortDevicePathVarSize,
                                    &PciSerialPortDevicePathVar);
            if (EFI_ERROR(Status)) {
                return Status;
            }

            //
            //Check for the presence of the PCI Serial device
            //
            if ((RevisionId[3] == PCI_CL_COMM) &&
                ((RevisionId[2] == PCI_CL_COMM_CSL_SERIAL) ||
                (RevisionId[2] == PCI_CL_COMM_CSL_OTHER)) ) {
                continue;
            }

            //
            //Its not PCI serial Device,So delete NVRAM variable describing device path of the device.
            //
            SetNvramVariable = 1;
            Status = pRS->SetVariable(gPciSerialPortsDevicePathVarName[i],
                                      &gTerminalVarGuid,
                                      (EFI_VARIABLE_BOOTSERVICE_ACCESS |
                                      EFI_VARIABLE_NON_VOLATILE),
                                       0,
                                       NULL);
            ASSERT_EFI_ERROR(Status);
            //
            //Initialise Bus,Dev,func. to Zero and set variable to represent that device is not present.
            //
            PciSerialPortsLocationVar.Bus[i] = 0;
            PciSerialPortsLocationVar.Device[i] = 0;
            PciSerialPortsLocationVar.Function[i] = 0;
        }
    }
    if (SetNvramVariable == 1) {
        Status = pRS->SetVariable(PCI_SERIAL_PORTS_LOCATION_VAR_C_NAME,
                                  &gTerminalVarGuid,
                                  (EFI_VARIABLE_BOOTSERVICE_ACCESS |
                                      EFI_VARIABLE_NON_VOLATILE),
                                  PciSerialPortsLocationVarSize,
                                  &PciSerialPortsLocationVar);
        ASSERT_EFI_ERROR(Status);
    }
    return EFI_SUCCESS;
}
#endif

/**
    Clears PCI Serial Port Variables.
    Unlike the SuperIO serial ports, pci serial ports do not have UIDs.  
    We distinguish them by their locations.  
    If it matches, we clear the serial port variable.
    If it doesn't match, we'll return EFI_NOT_FOUND
    
    @param Bus
    @param Device
    @param Function
    @param Port
    @param AmiPciSerialDetected
    
    @retval EFI_STATUS
**/

EFI_STATUS 
ClearSerialPortsEnabledVar_Pci(
    IN UINT8 Bus,
    IN UINT8 Device, 
    IN UINT8 Function,
    IN UINT8 Port,
    IN BOOLEAN AmiPciSerialDetected)
{
    
    EFI_STATUS Status = EFI_SUCCESS;    
    
#if (TOTAL_PCI_SERIAL_PORTS > 0)
{
    UINTN SerialPortsEnabledVarSize = sizeof(SERIAL_PORTS_ENABLED_VAR); 
    UINT32 SerialPortsEnabledVarAttributes=0;
    SERIAL_PORTS_ENABLED_VAR SerialPortsEnabledVar; 

    UINTN PciSerialPortsLocationVarSize = sizeof(PCI_SERIAL_PORTS_LOCATION_VAR);
    UINT32 PciSerialPortsLocationVarAttributes=0;
    PCI_SERIAL_PORTS_LOCATION_VAR PciSerialPortsLocationVar; 

    UINTN PciSerialPortDevicePathVarSize = 0;
    UINT32 PciSerialPortDevicePathVarAttributes = 0;
    EFI_DEVICE_PATH_PROTOCOL *PciSerialPortDevicePathVar = NULL; 

    UINT32 i = 0;
    

    for ( i = 0; i < TOTAL_PCI_SERIAL_PORTS; i++) {
        Swprintf(gPciSerialPortsDevicePathVarName[i], L"COM%x", gTotalSioSerialPorts+i);
    }


    Status = pRS->GetVariable(SERIAL_PORTS_ENABLED_VAR_C_NAME, 
                                &gTerminalVarGuid, 
                                &SerialPortsEnabledVarAttributes, 
                                &SerialPortsEnabledVarSize, 
                                &SerialPortsEnabledVar);
    ASSERT_EFI_ERROR(Status);

    if(EFI_ERROR(Status)) {
        return Status;
    }

    Status = pRS->GetVariable(PCI_SERIAL_PORTS_LOCATION_VAR_C_NAME, 
                                &gTerminalVarGuid, 
                                &PciSerialPortsLocationVarAttributes, 
                                &PciSerialPortsLocationVarSize, 
                                &PciSerialPortsLocationVar);
    ASSERT_EFI_ERROR(Status);

    if(EFI_ERROR(Status)) {
        return Status;
    }
    
    for (i = 0; i < TOTAL_PCI_SERIAL_PORTS; i++) {
        if ((PciSerialPortsLocationVar.Bus[i] == Bus) && 
            (PciSerialPortsLocationVar.Device[i] == Device) && 
            (PciSerialPortsLocationVar.Function[i] == Function)) {
            
            SerialPortsEnabledVar.PortsEnabled[gTotalSioSerialPorts+i] = 0;
            PciSerialPortsLocationVar.Bus[i] = 0;
            PciSerialPortsLocationVar.Device[i] = 0;
            PciSerialPortsLocationVar.Function[i] = 0;
            if(AmiPciSerialDetected && PciSerialPortsLocationVar.Port[i] == Port) {
                PciSerialPortsLocationVar.AmiPciSerialPresent[i] = FALSE;
                PciSerialPortsLocationVar.Port[i] = 0;
            }
            
            Status = pRS->SetVariable(
                                gPciSerialPortsDevicePathVarName[i],
                                &gTerminalVarGuid,
                                (EFI_VARIABLE_BOOTSERVICE_ACCESS |
                                    EFI_VARIABLE_NON_VOLATILE), 
                                PciSerialPortDevicePathVarSize, 
                                PciSerialPortDevicePathVar);
            ASSERT_EFI_ERROR(Status);
            if(EFI_ERROR(Status)) {
                return Status;
            }
            break;
        }
    }

    if (i == TOTAL_PCI_SERIAL_PORTS) {
        return EFI_NOT_FOUND;
    }

    Status = pRS->SetVariable(SERIAL_PORTS_ENABLED_VAR_C_NAME, 
                                &gTerminalVarGuid, 
                                EFI_VARIABLE_BOOTSERVICE_ACCESS,
                                SerialPortsEnabledVarSize,
                                &SerialPortsEnabledVar); 
    ASSERT_EFI_ERROR(Status);
    if(EFI_ERROR(Status)) {
        return Status;
    }
    
    Status = pRS->SetVariable(
                            PCI_SERIAL_PORTS_LOCATION_VAR_C_NAME,
                            &gTerminalVarGuid, 
                            (EFI_VARIABLE_BOOTSERVICE_ACCESS |
                                    EFI_VARIABLE_NON_VOLATILE),
                            PciSerialPortsLocationVarSize, 
                            &PciSerialPortsLocationVar);
    
    ASSERT_EFI_ERROR(Status);
    if(EFI_ERROR(Status)) {
        return Status;
    }
    
}
#endif

    return Status;

}

/**
    Keeping track of which pci serial ports are enabled
    a bit complicated.
    Unlike the SuperIO serial ports, pci serial ports do not
    have UIDs.
    We distinguish them by their locations.
    For each pci serial port, we store the entire device
    path under the variable names in
    gPciSerialPortsDevicePathVarName ("COM5," "COM6", etc).
    We check the parameter "DevicePath" against our stored
    device paths.
    If it matches, we return the com port number of that
    device path.
    If it doesn't match, we use a new entry in
    SERIAL_PORTS_ENABLED_VAR for the pci serial port.
    We also use a new entry in PCI_SERIAL_PORTS_LOCATION_VAR
    and store the device and function numbers.

    @param  DevicePath
    @param  Bus
    @param  Device
    @param  Function
    @param  Port
    @param  AmiPciSerialDetected

    @retval UINT8

**/

UINT8
SetSerialPortsEnabledVar_Pci (
    IN EFI_DEVICE_PATH_PROTOCOL *DevicePath,
    IN UINT8 Bus,
    IN UINT8 Device,
    IN UINT8 Function,
    IN UINT8 Port,
    IN BOOLEAN AmiPciSerialDetected
)
{
#if (TOTAL_PCI_SERIAL_PORTS > 0)
    UINTN SerialPortsEnabledVarSize = sizeof(SERIAL_PORTS_ENABLED_VAR);
    UINT32 SerialPortsEnabledVarAttributes=0;
    SERIAL_PORTS_ENABLED_VAR SerialPortsEnabledVar;

    UINTN PciSerialPortsLocationVarSize = 
                                    sizeof(PCI_SERIAL_PORTS_LOCATION_VAR);
    UINT32 PciSerialPortsLocationVarAttributes=0;
    PCI_SERIAL_PORTS_LOCATION_VAR PciSerialPortsLocationVar;

    UINTN PciSerialPortDevicePathVarSize = 0;
    UINT32 PciSerialPortDevicePathVarAttributes = 0;
    EFI_DEVICE_PATH_PROTOCOL *PciSerialPortDevicePathVar = NULL;

    EFI_STATUS Status = EFI_SUCCESS;
    UINT32 i = 0;
    UINT8 ComPort=0xFF;

    for ( i = 0; i < TOTAL_PCI_SERIAL_PORTS; i++) {
        Swprintf(gPciSerialPortsDevicePathVarName[i], L"COM%x", gTotalSioSerialPorts+i);
    }

    Status = pRS->GetVariable(SERIAL_PORTS_ENABLED_VAR_C_NAME,
                                &gTerminalVarGuid,
                                &SerialPortsEnabledVarAttributes,
                                &SerialPortsEnabledVarSize,
                                &SerialPortsEnabledVar);
    ASSERT_EFI_ERROR(Status);

    if(EFI_ERROR(Status)) {
        return 0xFF;
    }

	Status = ValidatePciSerialNvram(SerialPortsEnabledVar,DevicePath);

    if (EFI_ERROR(Status)) {
        return 0xFF;
    }

    Status = pRS->GetVariable(PCI_SERIAL_PORTS_LOCATION_VAR_C_NAME,
                                &gTerminalVarGuid, 
                                &PciSerialPortsLocationVarAttributes,
                                &PciSerialPortsLocationVarSize,
                                &PciSerialPortsLocationVar);
    ASSERT_EFI_ERROR(Status);

    if(EFI_ERROR(Status)) {
        return 0xFF;
    }

    for (i = 0; i < TOTAL_PCI_SERIAL_PORTS; i++) {
        if ((PciSerialPortsLocationVar.Device[i] == Device) &&
            (PciSerialPortsLocationVar.Function[i] == Function)) {
            Status = GetEfiVariable(gPciSerialPortsDevicePathVarName[i],
                                    &gTerminalVarGuid,
                                    &PciSerialPortDevicePathVarAttributes,
                                    &PciSerialPortDevicePathVarSize,
                                    &PciSerialPortDevicePathVar);
            if (!EFI_ERROR(Status)) {
                if (!DPCmp(DevicePath, PciSerialPortDevicePathVar)) {
                    SerialPortsEnabledVar.PortsEnabled[gTotalSioSerialPorts+i] = 1;
                    ComPort = gTotalSioSerialPorts+i;
                    break;
                }
            }
        }
    }

    if (i == TOTAL_PCI_SERIAL_PORTS) {
        for (i = 0; i < TOTAL_PCI_SERIAL_PORTS; i++) {
            if ((!SerialPortsEnabledVar.PortsEnabled[gTotalSioSerialPorts+i]) &&
                (PciSerialPortsLocationVar.Device[i] == 0) &&
                (PciSerialPortsLocationVar.Function[i] == 0)) {
                Status = GetEfiVariable(
                                    gPciSerialPortsDevicePathVarName[i],
                                    &gTerminalVarGuid,
                                    &PciSerialPortDevicePathVarAttributes,
                                    &PciSerialPortDevicePathVarSize,
                                    &PciSerialPortDevicePathVar);
                if (Status == EFI_NOT_FOUND) {
                    SerialPortsEnabledVar.PortsEnabled[gTotalSioSerialPorts+i] = 1;
                    ComPort = gTotalSioSerialPorts+i;

                    PciSerialPortDevicePathVarSize = DPLength(DevicePath);
                    Status = pRS->SetVariable(
                                        gPciSerialPortsDevicePathVarName[i],
                                        &gTerminalVarGuid,
                                        (EFI_VARIABLE_BOOTSERVICE_ACCESS |
                                            EFI_VARIABLE_NON_VOLATILE),
                                        PciSerialPortDevicePathVarSize,
                                        DevicePath);
                    ASSERT_EFI_ERROR(Status);

                    PciSerialPortsLocationVar.Bus[i] = Bus;
                    PciSerialPortsLocationVar.Device[i] = Device;
                    PciSerialPortsLocationVar.Function[i] = Function;
                    if(AmiPciSerialDetected) {
                        PciSerialPortsLocationVar.AmiPciSerialPresent[i] = TRUE;
                        PciSerialPortsLocationVar.Port[i] = Port;
                    }

                    Status = pRS->SetVariable(
                                    PCI_SERIAL_PORTS_LOCATION_VAR_C_NAME,
                                    &gTerminalVarGuid,
                                    (EFI_VARIABLE_BOOTSERVICE_ACCESS |
                                            EFI_VARIABLE_NON_VOLATILE),
                                    PciSerialPortsLocationVarSize,
                                    &PciSerialPortsLocationVar);
                    ASSERT_EFI_ERROR(Status);

                    break;
                }
            }
        }
    }

    Status = pRS->SetVariable(SERIAL_PORTS_ENABLED_VAR_C_NAME,
                                &gTerminalVarGuid,
                                EFI_VARIABLE_BOOTSERVICE_ACCESS,
                                SerialPortsEnabledVarSize,
                                &SerialPortsEnabledVar);
    ASSERT_EFI_ERROR(Status);

    return ComPort;

#else

    return 0xFF;

#endif
}

/**
    Returns the user configured SERIAL_IO_MODE values.

    @param  Port
    @param  SerialIoMode

    @retval VOID

**/

VOID
GetSetupValuesForSerialIoMode (
    IN UINT8              Port,
    IN OUT SERIAL_IO_MODE *SerialIoMode
)
{

    UINT8 FlowControl = 0;

#if (TOTAL_SERIAL_PORTS == 0)

    FlowControl             = UART_DEFAULT_FLOW_CONTROL;
    SerialIoMode->BaudRate  =
                gAcpiSpcrTableComBaudRates[UART_DEFAULT_BAUD_RATE_INDEX];
    SerialIoMode->Parity    = UART_DEFAULT_PARITY;
    SerialIoMode->DataBits  = UART_DEFAULT_DATA_BITS;
    SerialIoMode->StopBits  = UART_DEFAULT_STOP_BITS;

#else

    UINT32 SetupDataAttributes = 0;
    UINTN SetupDataVarSize = sizeof(SETUP_DATA);
    EFI_STATUS Status;
    
    Status = pRS->GetVariable(L"Setup", &gSetupGuid, &SetupDataAttributes,
                                &SetupDataVarSize, &gSetupData);

    if (EFI_ERROR(Status)) {
        FlowControl             = UART_DEFAULT_FLOW_CONTROL;
        SerialIoMode->BaudRate  =
                gAcpiSpcrTableComBaudRates[UART_DEFAULT_BAUD_RATE_INDEX];
        SerialIoMode->Parity    = UART_DEFAULT_PARITY;
        SerialIoMode->DataBits  = UART_DEFAULT_DATA_BITS;
        SerialIoMode->StopBits  = UART_DEFAULT_STOP_BITS;
    } else {
        FlowControl             = gSetupData.FlowControl[Port];
        SerialIoMode->BaudRate  =
                    gAcpiSpcrTableComBaudRates[gSetupData.BaudRate[Port]];
        SerialIoMode->Parity    = gSetupData.Parity[Port];
        SerialIoMode->DataBits  = gSetupData.DataBits[Port];
        SerialIoMode->StopBits  = gSetupData.StopBits[Port];
    }

#endif

    SerialIoMode->ControlMask = 0;
    if (FlowControl == HARDWARE_FLOW_CONTROL_SETUP_OPTION)
        SerialIoMode->ControlMask |= EFI_SERIAL_HARDWARE_FLOW_CONTROL_ENABLE;
    else if (FlowControl == SOFTWARE_FLOW_CONTROL_SETUP_OPTION)
        SerialIoMode->ControlMask |= EFI_SERIAL_SOFTWARE_FLOW_CONTROL_ENABLE;
}

/**
    Returns the default settings of the ConsoleRedirection
    enable status

    @param  Port

    @retval UINT8

**/

UINT8
GetDefaultConsoleRedirectionStatus (
    IN UINT8 Port
)
{
    switch (Port) {
#if (TOTAL_SIO_SERIAL_PORTS > 0)
        case 0:
                return COM0_DEFAULT_CONSOLE_REDIRECTION_ENABLE;
#endif

#if (TOTAL_SIO_SERIAL_PORTS > 1)
        case 1:
                return COM1_DEFAULT_CONSOLE_REDIRECTION_ENABLE;
#endif

#if (TOTAL_SIO_SERIAL_PORTS > 2)
        case 2:
                return COM2_DEFAULT_CONSOLE_REDIRECTION_ENABLE;
#endif

#if (TOTAL_SIO_SERIAL_PORTS > 3)
        case 3:
                return COM3_DEFAULT_CONSOLE_REDIRECTION_ENABLE;
#endif

#if (TOTAL_SIO_SERIAL_PORTS > 4)
        case 4:
                return COM4_DEFAULT_CONSOLE_REDIRECTION_ENABLE;
#endif

#if (TOTAL_SIO_SERIAL_PORTS > 5)
        case 5:
                return COM5_DEFAULT_CONSOLE_REDIRECTION_ENABLE;
#endif

#if (TOTAL_SIO_SERIAL_PORTS > 6)
        case 6:
                return COM6_DEFAULT_CONSOLE_REDIRECTION_ENABLE;
#endif

#if (TOTAL_SIO_SERIAL_PORTS > 7)
        case 7:
                return COM7_DEFAULT_CONSOLE_REDIRECTION_ENABLE;
#endif

#if (TOTAL_SIO_SERIAL_PORTS > 8)
        case 8:
                return COM8_DEFAULT_CONSOLE_REDIRECTION_ENABLE;
#endif

#if (TOTAL_SIO_SERIAL_PORTS > 9)
        case 9:
                return COM9_DEFAULT_CONSOLE_REDIRECTION_ENABLE;
#endif

#if (TOTAL_PCI_SERIAL_PORTS > 0)
        case (TOTAL_SIO_SERIAL_PORTS):
                return PCI0_DEFAULT_CONSOLE_REDIRECTION_ENABLE;
#endif

#if (TOTAL_PCI_SERIAL_PORTS > 1)
        case (TOTAL_SIO_SERIAL_PORTS+1):
                return PCI1_DEFAULT_CONSOLE_REDIRECTION_ENABLE;
#endif

#if (TOTAL_PCI_SERIAL_PORTS > 2)
        case (TOTAL_SIO_SERIAL_PORTS+2):
                return PCI2_DEFAULT_CONSOLE_REDIRECTION_ENABLE;
#endif

#if (TOTAL_PCI_SERIAL_PORTS > 3)
        case (TOTAL_SIO_SERIAL_PORTS+3):
                return PCI3_DEFAULT_CONSOLE_REDIRECTION_ENABLE;
#endif

    }

    return 0;
}

/**
    Returns the Setup setttings for the terminal
    (whether redirection is enabled and the terminal type).
 
    @param     Port
    @param     ConsoleRedirectionEnable
    @param     TerminalType
    @param     SupportExRes
    @param     VtUtf8
    @param     RecorderMode
    @param     PuttyKeyPad
    @param     Disable_Terminal_For_SCT_Test
 
    @retval    VOID

**/

VOID
GetSetupValuesForTerminal (
    IN UINT8 Port, 
    IN OUT UINT8 *ConsoleRedirectionEnable, 
    IN OUT UINT8 *TerminalType,
    IN OUT BOOLEAN *SupportExRes, 
    IN OUT BOOLEAN *VtUtf8, 
    IN OUT BOOLEAN *RecorderMode, 
    IN OUT BOOLEAN *PuttyKeyPad,
    IN OUT UINT8 *Disable_Terminal_For_SCT_Test
)
{
    UINT32 SetupDataAttributes = 0;
    UINTN SetupDataSize = sizeof(SETUP_DATA);

    EFI_STATUS Status;

    *Disable_Terminal_For_SCT_Test = DISABLE_TERMINAL_FOR_SCT_TEST;

    Status = pRS->GetVariable(L"Setup", &gSetupGuid, &SetupDataAttributes,
                                    &SetupDataSize, &gSetupData);
    ASSERT_EFI_ERROR(Status);

    if(Port == (UINT8)-1){
        //
        // If the redirection comes Via Ami debugger
        //
#ifdef CONSOLE_REDIRECTION_SUPPORT
#if CONSOLE_REDIRECTION_SUPPORT == 1
        if(EFI_ERROR(Status)) {
            //
            // If the Setup Variable is not Found Return the Default values
            //
            *ConsoleRedirectionEnable = DEFAULT_DEBUGGER_CONSOLE_REDIRECTION_ENABLE;
            *TerminalType = DEFAULT_TERMINAL_TYPE;
            *SupportExRes = FALSE;
            *VtUtf8 = VTUTF8_ENABLE;
            *RecorderMode = FALSE;
            *PuttyKeyPad = PUTTY_VT100;
        } else {
            //
            // Setup Values Found for the Debugger Serial port.
            //
            *ConsoleRedirectionEnable=gSetupData.DebuggerConsoleRedirectionEnable;
            *TerminalType = gSetupData.DebuggerTerminalType;
            *SupportExRes = FALSE;
            *VtUtf8 = VTUTF8_ENABLE;
            *RecorderMode = FALSE;
            *PuttyKeyPad = PUTTY_VT100;
        }
#else
        //
        // CONSOLE_REDIRECTION_SUPPORT token is defined in Amidebugger module.
        // But if this token is disabled, return the default values.
        //
        *ConsoleRedirectionEnable = DEFAULT_DEBUGGER_CONSOLE_REDIRECTION_ENABLE;
        *TerminalType = DEFAULT_TERMINAL_TYPE;
        *SupportExRes = FALSE;
        *VtUtf8 = VTUTF8_ENABLE;
        *RecorderMode = FALSE;
        *PuttyKeyPad = PUTTY_VT100;
#endif
#else 
        //
        // CONSOLE_REDIRECTION_SUPPORT token is defined in Amidebugger module.
        // If this token is not present, return the default values.
        //
        *ConsoleRedirectionEnable = DEFAULT_DEBUGGER_CONSOLE_REDIRECTION_ENABLE;
        *TerminalType = DEFAULT_TERMINAL_TYPE;
        *SupportExRes = FALSE;
        *VtUtf8 = VTUTF8_ENABLE;
        *RecorderMode = FALSE;
        *PuttyKeyPad = PUTTY_VT100;

#endif

        return;
    }

#if (TOTAL_SERIAL_PORTS == 0)

    *ConsoleRedirectionEnable = GetDefaultConsoleRedirectionStatus(Port);
    *TerminalType = DEFAULT_TERMINAL_TYPE;
    *SupportExRes = FALSE;
    *VtUtf8 = VTUTF8_ENABLE;
    *RecorderMode = FALSE;
    *PuttyKeyPad = PUTTY_VT100;

#else
    if (EFI_ERROR(Status)) {
        *ConsoleRedirectionEnable = GetDefaultConsoleRedirectionStatus(Port);
        *TerminalType = DEFAULT_TERMINAL_TYPE;
        *SupportExRes = FALSE;
        *VtUtf8 = VTUTF8_ENABLE;
        *RecorderMode = FALSE;
        *PuttyKeyPad = PUTTY_VT100;
    } else {
        *ConsoleRedirectionEnable =
                                    gSetupData.ConsoleRedirectionEnable[Port];
        *TerminalType = gSetupData.TerminalType[Port];
        *SupportExRes = (gSetupData.Resolution[Port] == 0) ? FALSE : TRUE;
        *VtUtf8 = (gSetupData.VtUtf8[Port] == 0) ? FALSE : TRUE;
        *RecorderMode = (gSetupData.RecorderMode[Port] == 0) ? FALSE : TRUE;
        *PuttyKeyPad = gSetupData.PuttyFunctionKeyPad[Port];
    }
#endif
}

/**
    Returns the configurable aspects of the acpi spcr table 
    com port (the com port number, whether the console 
    redirection is enabled, the terminal type, baude rate, 
    and flow control type used). 

    @param     AcpiSpcrTableComPort
    @param     AcpiSpcrTableConsoleRedirectionEnable
    @param     AcpiSpcrTableTerminalType
    @param     AcpiSpcrTableBaudRate
    @param     AcpiSpcrTableFlowControl

    @retval    VOID

**/

VOID
GetAcpiSpcrTableValues (
    IN OUT UINT8 *AcpiSpcrTableComPort,
    IN OUT UINT8 *AcpiSpcrTableConsoleRedirectionEnable,
    IN OUT UINT8 *AcpiSpcrTableTerminalType,
    IN OUT UINT8 *AcpiSpcrTableBaudRate,
    IN OUT UINT8 *AcpiSpcrTableFlowControl)
{

#if (TOTAL_SERIAL_PORTS == 0)

    *AcpiSpcrTableComPort = DEFAULT_ACPI_SPCR_COM_PORT;
    *AcpiSpcrTableConsoleRedirectionEnable =
                            DEFAULT_ACPI_SPCR_CONSOLE_REDIRECTION_ENABLE;
    *AcpiSpcrTableTerminalType = DEFAULT_ACPI_SPCR_TABLE_TERMINAL_TYPE;
    *AcpiSpcrTableBaudRate = UART_DEFAULT_BAUD_RATE_INDEX;
    *AcpiSpcrTableFlowControl = UART_DEFAULT_FLOW_CONTROL;

#else

    UINT32 SetupDataAttributes = 0;
    UINTN SetupDataSize = sizeof(SETUP_DATA);
    EFI_STATUS Status;
    
    Status = pRS->GetVariable(L"Setup", &gSetupGuid, &SetupDataAttributes,
                                &SetupDataSize, &gSetupData);

    if (EFI_ERROR(Status)) {
        *AcpiSpcrTableComPort = DEFAULT_ACPI_SPCR_COM_PORT;
        *AcpiSpcrTableConsoleRedirectionEnable =
                            DEFAULT_ACPI_SPCR_CONSOLE_REDIRECTION_ENABLE;
        *AcpiSpcrTableTerminalType = DEFAULT_ACPI_SPCR_TABLE_TERMINAL_TYPE;
        *AcpiSpcrTableBaudRate = UART_DEFAULT_BAUD_RATE_INDEX;
        *AcpiSpcrTableFlowControl = UART_DEFAULT_FLOW_CONTROL;
    } else {
#if EFI_SPECIFICATION_VERSION<0x2000A
        *AcpiSpcrTableComPort = gSetupData.AcpiSpcrPort-1;
#else
        *AcpiSpcrTableComPort = gSetupData.AcpiSpcrPort;
#endif

        *AcpiSpcrTableConsoleRedirectionEnable =
                                gSetupData.AcpiSpcrConsoleRedirectionEnable;
        *AcpiSpcrTableTerminalType = gSetupData.AcpiSpcrTerminalType;
        *AcpiSpcrTableBaudRate =
                            gSetupData.AcpiSpcrBaudRate;
        *AcpiSpcrTableFlowControl =
                            gSetupData.AcpiSpcrFlowControl;
    }

#endif
}

#include <Protocol/ComponentName.h>
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

//****************************************************************************
//****************************************************************************
//**                                                                        **
//**             (C)Copyright 1985-2015, American Megatrends, Inc.          **
//**                                                                        **
//**                          All Rights Reserved.                          **
//**                                                                        **
//**             5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093       **
//**                                                                        **
//**                          Phone (770)-246-8600                          **
//**                                                                        **
//****************************************************************************
//****************************************************************************
