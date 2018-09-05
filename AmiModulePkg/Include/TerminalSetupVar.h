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

/** @file TerminalSetupVar.h
    Description: Contains definition of PCI_SERIAL_PORTS_LOCATION_VAR 
    and of varstore declaration SERIAL_PORTS_ENABLED_VAR.
**/

#ifndef _TERMINAL_SETUP_VAR_H
#define _TERMINAL_SETUP_VAR_H
#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------------
#include <Token.h>

//----------------------------------------------------------------------------
#define TERMINAL_VAR_GUID \
{0x560bf58a, 0x1e0d, 0x4d7e, 0x95, 0x3f, 0x29, 0x80, 0xa2, 0x61, 0xe0, 0x31}

#define SERIAL_PORTS_ENABLED_VAR_C_NAME         L"SerialPortsEnabledVar"
#define SIO_SERIAL_PORTS_LOCATION_VAR_C_NAME    L"SioSerialPortsLocationVar"
#define PCI_SERIAL_PORTS_LOCATION_VAR_C_NAME    L"PciSerialPortsLocationVar"
#define DEBUGGER_SERIAL_PORTS_ENABLED_VAR_C_NAME         L"DebuggerSerialPortsEnabledVar"

#define DEBUGGER_TERMINAL_VAR_GUID \
{0x97ca1a5b, 0xb760, 0x4d1f, 0xa5, 0x4b, 0xd1, 0x90, 0x92, 0x3, 0x2c, 0x90}

#define MAX_SIO_SERIAL_PORTS 10
#define MAX_PCI_SERIAL_PORTS 4
#define TERMINAL_PORT0 0
#define TERMINAL_PORT1 1
#define TERMINAL_PORT2 2
#define TERMINAL_PORT3 3
#define TERMINAL_PORT4 4
#define TERMINAL_PORT5 5
#define TERMINAL_PORT6 6
#define TERMINAL_PORT7 7
#define TERMINAL_PORT8 8
#define TERMINAL_PORT9 9

#define HARDWARE_FLOW_CONTROL_SETUP_OPTION 1
#define SOFTWARE_FLOW_CONTROL_SETUP_OPTION 2

#define PUTTY_VT100   0x1
#define PUTTY_LINUX   0x2
#define PUTTY_XTERMR6 0x4
#define PUTTY_SCO     0x8
#define PUTTY_ESCN    0x10
#define PUTTY_VT400   0x20

#pragma pack(1)

#ifndef TYPEDEF_DEBUGGER_SERIAL_PORTS_ENABLED_VAR
#define TYPEDEF_DEBUGGER_SERIAL_PORTS_ENABLED_VAR
    typedef struct {
        UINT8 PortEnabled;
    } DEBUGGER_SERIAL_PORTS_ENABLED_VAR;
#endif

#if (TOTAL_SERIAL_PORTS > 0)
#ifndef TYPEDEF_SERIAL_PORTS_ENABLED_VAR
#define TYPEDEF_SERIAL_PORTS_ENABLED_VAR
    typedef struct { 
        UINT8 PortsEnabled[ TOTAL_SERIAL_PORTS ];
    } SERIAL_PORTS_ENABLED_VAR;
#endif
#endif

#if (TOTAL_SIO_SERIAL_PORTS > 0)
#ifndef TYPEDEF_SIO_SERIAL_PORTS_LOCATION_VAR
#define TYPEDEF_SIO_SERIAL_PORTS_LOCATION_VAR
    typedef struct { 
        UINT8 PortUid[ TOTAL_SIO_SERIAL_PORTS ];
        UINT8 Valid[ TOTAL_SIO_SERIAL_PORTS ];
    } SIO_SERIAL_PORTS_LOCATION_VAR;
#endif
#endif

#if (TOTAL_PCI_SERIAL_PORTS > 0)
#ifndef TYPEDEF_PCI_SERIAL_PORTS_LOCATION_VAR
#define TYPEDEF_PCI_SERIAL_PORTS_LOCATION_VAR
    typedef struct { 
        UINT8 Segment[ TOTAL_PCI_SERIAL_PORTS ];
        UINT8 Bus[ TOTAL_PCI_SERIAL_PORTS ];
        UINT8 Device[ TOTAL_PCI_SERIAL_PORTS ];
        UINT8 Function[ TOTAL_PCI_SERIAL_PORTS ];
        UINT8 AmiPciSerialPresent[ TOTAL_PCI_SERIAL_PORTS ];
        UINT8 Port[ TOTAL_PCI_SERIAL_PORTS ];
    } PCI_SERIAL_PORTS_LOCATION_VAR;
#endif
#endif
    
#ifndef VFRCOMPILE
typedef struct {
    UINT16      VendorID;
    UINT16      DeviceID;
    UINTN      	UartClock;
    UINT8       MmioWidth;
}PCI_DEVICE_UART_CLOCK_WIDTH;
#endif

#pragma pack()

#ifdef __cplusplus
}
#endif
#endif

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
