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

/** @file InitTerminalStrings.c
    Contains InitTerminalStrings(), which is linked to 
    SetupStringInit in the sdl page.  Before entering setup,
    we do any modification or updating needed for the serial
    port name strings (STR_SIO_SERIAL_PORT0, etc.).

**/

//----------------------------------------------------------------------------
#include <AmiLib.h>
#include <AmiDxeLib.h>
#include <Setup.h>
#include <Token.h>
#include "TerminalSetupVar.h"

//----------------------------------------------------------------------------

#ifndef HII_LIBRARY_FUNCTIONS_SUPPORT
EFI_STATUS HiiLibGetString(
	IN EFI_HII_HANDLE HiiHandle, 
    IN STRING_REF StringId, 
	IN OUT UINTN *StringSize, 
    OUT EFI_STRING String
);
EFI_STATUS HiiLibSetString(
	IN EFI_HII_HANDLE HiiHandle, 
    IN STRING_REF StringId, 
    IN EFI_STRING String
);
#endif

STRING_REF gSioSerialPortStrRef[MAX_SIO_SERIAL_PORTS] = {
    STRING_TOKEN(STR_SIO_SERIAL_PORT0),
    STRING_TOKEN(STR_SIO_SERIAL_PORT1),
    STRING_TOKEN(STR_SIO_SERIAL_PORT2),
    STRING_TOKEN(STR_SIO_SERIAL_PORT3),
    STRING_TOKEN(STR_SIO_SERIAL_PORT4),
    STRING_TOKEN(STR_SIO_SERIAL_PORT5),
    STRING_TOKEN(STR_SIO_SERIAL_PORT6),
    STRING_TOKEN(STR_SIO_SERIAL_PORT7),
    STRING_TOKEN(STR_SIO_SERIAL_PORT8),
    STRING_TOKEN(STR_SIO_SERIAL_PORT9)
};

STRING_REF gPciSerialPortStrRef[MAX_PCI_SERIAL_PORTS] = {
    STRING_TOKEN(STR_PCI_SERIAL_PORT0),
    STRING_TOKEN(STR_PCI_SERIAL_PORT1),
    STRING_TOKEN(STR_PCI_SERIAL_PORT2),
    STRING_TOKEN(STR_PCI_SERIAL_PORT3)
};

UINT32 gTotalSioSerialPorts = TOTAL_SIO_SERIAL_PORTS;
UINT32 gTotalPciSerialPorts = TOTAL_PCI_SERIAL_PORTS;

EFI_GUID gTerminalVarGuid = TERMINAL_VAR_GUID;


/**
    Certain strings in the terminal setup page can only be
    updated at runtime, which is what this function does.
    The pci serial port names are changed to contain the
    location (device and function numbers) of the port.
    If any serial ports are disabled, "Disabled"
    is concatenated to their names.

    @param  HiiHandle
    @param  Class

    @retval VOID

**/

VOID 
EFIAPI
InitTerminalStrings(
    IN EFI_HII_HANDLE HiiHandle, 
    IN UINT16 Class
)
{
#if (TOTAL_SERIAL_PORTS > 0)

    EFI_STATUS Status = EFI_SUCCESS;

    UINTN SerialPortsEnabledVarSize = sizeof(SERIAL_PORTS_ENABLED_VAR);
    SERIAL_PORTS_ENABLED_VAR SerialPortsEnabledVar;
    UINT32 SerialPortsEnabledVarAttributes=0;

#if (TOTAL_PCI_SERIAL_PORTS > 0)
    UINTN PciSerialPortsLocationVarSize 
                                = sizeof(PCI_SERIAL_PORTS_LOCATION_VAR);
    PCI_SERIAL_PORTS_LOCATION_VAR PciSerialPortsLocationVar; 
    UINT32 PciSerialPortsLocationVarAttributes=0;
#endif

    UINT32 i = 0;

    CHAR16 Str[512]=L"";
#if (TOTAL_PCI_SERIAL_PORTS > 0)
    CHAR16 Str2[512]=L"";
#endif
    UINTN Size=512;

    if (Class==ADVANCED_FORM_SET_CLASS) {
        Status = pRS->GetVariable(SERIAL_PORTS_ENABLED_VAR_C_NAME,
                                    &gTerminalVarGuid,
                                    &SerialPortsEnabledVarAttributes,
                                    &SerialPortsEnabledVarSize,
                                    &SerialPortsEnabledVar);

        if (EFI_ERROR(Status)) {
        return;
    }

#if (TOTAL_PCI_SERIAL_PORTS > 0)
        Status = pRS->GetVariable(PCI_SERIAL_PORTS_LOCATION_VAR_C_NAME,
                                    &gTerminalVarGuid,
                                    &PciSerialPortsLocationVarAttributes,
                                    &PciSerialPortsLocationVarSize,
                                    &PciSerialPortsLocationVar);
        ASSERT_EFI_ERROR(Status);

        if (EFI_ERROR(Status)) {
            return;
        }
#endif

        for (i = 0; i < gTotalSioSerialPorts; i++) {
            if (!SerialPortsEnabledVar.PortsEnabled[i]) {
                Size = 512;
                Status = HiiLibGetString(HiiHandle,
                                            gSioSerialPortStrRef[i],
                                            &Size, Str);
                if (!EFI_ERROR(Status)) {
                    StrCat(Str, L" (Disabled)");
                    InitString(HiiHandle,  gSioSerialPortStrRef[i], Str);
                }
            }
        }

#if (TOTAL_PCI_SERIAL_PORTS > 0)
        for (i = 0; i < gTotalPciSerialPorts; i++) {
            Size = 512;
            Status = HiiLibGetString(HiiHandle,
                                        gPciSerialPortStrRef[i],
                                        &Size, Str);
            if (!EFI_ERROR(Status)) {
                if(PciSerialPortsLocationVar.AmiPciSerialPresent[i] == TRUE) {
                    //
                    // If AMI Serial Protcocol was installed for this PCI Serial device,
                    // then display the device like COM3(Pci Bus2,Dev10,Func1,Port0)
                    // with Port information.
                    //
                    Swprintf(Str2, L"COM%X(Pci Bus%d,Dev%d,Func%d,Port%d)",
                                        gTotalSioSerialPorts+i,
                                        PciSerialPortsLocationVar.Bus[i],
                                        PciSerialPortsLocationVar.Device[i],
                                        PciSerialPortsLocationVar.Function[i],
                                        PciSerialPortsLocationVar.Port[i]);
                } else {
                    Swprintf(Str2, L"COM%X(Pci Bus%d,Dev%d,Func%d)",
                                        gTotalSioSerialPorts+i,
                                        PciSerialPortsLocationVar.Bus[i],
                                        PciSerialPortsLocationVar.Device[i],
                                        PciSerialPortsLocationVar.Function[i]);
                }

                StrCat(Str, Str2);
                if (!SerialPortsEnabledVar.PortsEnabled[gTotalSioSerialPorts+i]) {
                    StrCat(Str, L" (Disabled)");
                }
                InitString(HiiHandle, gPciSerialPortStrRef[i], Str);
            }
        }
#endif
    }
#endif
}

#ifndef HII_LIBRARY_FUNCTIONS_SUPPORT
#include <Protocol/Hii.h>

static EFI_HII_PROTOCOL *Hii = NULL;

static EFI_STATUS HiiLibGetString(
	IN EFI_HII_HANDLE HiiHandle,
    IN STRING_REF StringId,
	IN OUT UINTN *StringSize,
    OUT EFI_STRING String
)
{
    if (Hii == NULL && EFI_ERROR(pBS->LocateProtocol(&gEfiHiiProtocolGuid, NULL, &Hii)))
        return EFI_NOT_FOUND;

    return Hii->GetString(Hii, HiiHandle, StringId, TRUE, NULL, StringSize, String);
}

static EFI_STATUS HiiLibSetString(
	IN EFI_HII_HANDLE HiiHandle, 
    IN STRING_REF StringId,
    IN EFI_STRING String
)
{
    if (Hii == NULL && EFI_ERROR(pBS->LocateProtocol(&gEfiHiiProtocolGuid, NULL, &Hii)))
        return EFI_NOT_FOUND;

    return Hii->NewString(Hii, L"   ", HiiHandle, &StringId, String);
}
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
