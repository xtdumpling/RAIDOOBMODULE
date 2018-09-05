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

/** @file TerminalAcpi.c
    This file contains function to disable the PCI redirection
    at ACPI enable call.

**/

//----------------------------------------------------------------------------
#include <Token.h>
#include <AmiDxeLib.h>
#include <Protocol/SmmSwDispatch2.h>
#include "TerminalSetupVar.h"
#include "AcpiModeEnable.h"

//----------------------------------------------------------------------------
#define     CFG_SPACE_INDEX_REG         0xCF8
#define     CFG_SPACE_DATA_REG          0xCFC
#define     PCI_BAR0                    0x0010
#define     INT08h_SEGMENT              0xF000
#define     INT08h_OFFSET               0xFEA5

EFI_SMM_SYSTEM_TABLE2            *gSmst2=NULL;
EFI_GUID gEfiAcpiEnDispatchProtocolGuid  = EFI_ACPI_EN_DISPATCH_PROTOCOL_GUID;

/**
    This routine will be called to disable PCI redirection at ACPI
    enable/disable call and reset Interrupt Enable Register to
    disable interrupt from PCI Serial device & replace actual INT08h.

    @param  DispatchHandle

    @retval VOID
**/

VOID
EFIAPI
TerminalAcpiEnableDisableCallBack (
    IN EFI_HANDLE    DispatchHandle
)
{
#if (TOTAL_PCI_SERIAL_PORTS > 0)
    EFI_STATUS  Status;
    UINT8       RegNum = PCI_BAR0;
    UINT32      PciAddr;
    UINT32      BarAddress;
    UINT8       i,DevFunc;

    UINT8   gTotalSioSerialPorts = TOTAL_SIO_SERIAL_PORTS;
    EFI_GUID gTerminalVarGuid   = TERMINAL_VAR_GUID;
    UINTN PciSerialPortsLocationVarSize =
                                    sizeof(PCI_SERIAL_PORTS_LOCATION_VAR);
    PCI_SERIAL_PORTS_LOCATION_VAR PciSerialPortsLocationVar;

    UINTN SerialPortsEnabledVarSize = sizeof(SERIAL_PORTS_ENABLED_VAR);
    UINT32 SerialPortsEnabledVarAttributes=0;
    SERIAL_PORTS_ENABLED_VAR SerialPortsEnabledVar;

    Status = pRS->GetVariable(SERIAL_PORTS_ENABLED_VAR_C_NAME,
                                &gTerminalVarGuid,
                                &SerialPortsEnabledVarAttributes,
                                &SerialPortsEnabledVarSize,
                                &SerialPortsEnabledVar);

    if (!EFI_ERROR(Status)) {
        Status = pRS->GetVariable(PCI_SERIAL_PORTS_LOCATION_VAR_C_NAME,
                                &gTerminalVarGuid,
                                NULL,
                                &PciSerialPortsLocationVarSize,
                                &PciSerialPortsLocationVar);

        if (!EFI_ERROR(Status)) {
            for (i = 0; i < TOTAL_PCI_SERIAL_PORTS; i++) {
                //
                // Look for the PCI device that are enabled.
                //
                if (SerialPortsEnabledVar.PortsEnabled[gTotalSioSerialPorts+i]) {
                    if ((PciSerialPortsLocationVar.Bus[i] == 0) &&
                        (PciSerialPortsLocationVar.Device[i] == 0) &&
                        (PciSerialPortsLocationVar.Function[i] == 0)) {
                        continue;
                    }

                    DevFunc = (PciSerialPortsLocationVar.Device[i] << 3) + PciSerialPortsLocationVar.Function[i];
                    PciAddr = ((UINT32) (( (UINT16) PciSerialPortsLocationVar.Bus[i] ) << 8) +  DevFunc) << 8;
                    PciAddr += (RegNum & 0xfc);
                    PciAddr |= 0x80000000;

                    IoWrite32(CFG_SPACE_INDEX_REG, PciAddr);
                    BarAddress = IoRead32(CFG_SPACE_DATA_REG);
                    //
                    // Reset the IER register of UART.
                    //
                    if(BarAddress & 1) {
                        IoWrite32(BarAddress, 0);
                    } else {
                        *(UINTN*)( (BarAddress & 0xfffffff0) + 1) = 0;
                    }
                }
            }
            //
            // Replace the Actual INT08h routine which was hooked by Legacy console Redirection
            //
            *(UINT16*)0x0020 = (UINT16)INT08h_OFFSET;
            *(UINT16*)0x0022 = (UINT16)INT08h_SEGMENT;

            IoWrite8(0x80,0x99);
        }
    }
#endif
    return;
}

/**
    This function will register TerminalAcpiEnableDisableCallBack function for
    ACPI Mode enable and disable callback.

    @param  Protocol
    @param  Interface
    @param  Handle

    @retval EFI_STATUS

**/

EFI_STATUS
EFIAPI
CreateTerminalAcpiEnDisLink (
    IN CONST EFI_GUID  *Protocol,
    IN VOID            *Interface,
    IN EFI_HANDLE      Handle
)
{
    EFI_STATUS                  Status;
    EFI_ACPI_DISPATCH_PROTOCOL  *mAcpiEnDispatch;
    EFI_ACPI_DISPATCH_PROTOCOL  *mAcpiDisDispatch;
    EFI_HANDLE                  RegisterHandle;
    EFI_GUID gAcpiDisDispatchProtocolGuid = EFI_ACPI_DIS_DISPATCH_PROTOCOL_GUID;

    Status = gSmst2->SmmLocateProtocol( &gEfiAcpiEnDispatchProtocolGuid, \
                                  NULL, \
                                  &mAcpiEnDispatch );
    if(EFI_ERROR(Status)) {
    	return Status;
    }

    Status = mAcpiEnDispatch->Register( mAcpiEnDispatch, \
                                        TerminalAcpiEnableDisableCallBack, \
                                        &RegisterHandle );
    if(EFI_ERROR(Status)) {
       	return Status;
    }

    Status = gSmst2->SmmLocateProtocol( &gAcpiDisDispatchProtocolGuid, \
                                  NULL, \
                                  &mAcpiDisDispatch );
    if(EFI_ERROR(Status)) {
        return Status;
    }

    Status = mAcpiDisDispatch->Register( mAcpiDisDispatch, \
                                         TerminalAcpiEnableDisableCallBack, \
                                         &RegisterHandle );

    return Status;
}

/**
    This function will register CreateTerminalAcpiEnDisLink function for
    gEfiAcpiEnDispatchProtocolGuid for registering ACPI Callback.

    @param  ImageHandle
    @param  SystemTable

    @retval EFI_STATUS
    
**/

EFI_STATUS
EFIAPI
TerminalAcpiSmmEntry (
    IN EFI_HANDLE       ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable
)
{

    EFI_STATUS              Status;
    VOID                    *mAcpiReg;
    EFI_SMM_BASE2_PROTOCOL  *pSmmBase2;

    InitAmiSmmLib( ImageHandle, SystemTable );

    Status = pBS->LocateProtocol(&gEfiSmmBase2ProtocolGuid, NULL, &pSmmBase2);

    if (EFI_ERROR(Status)) {
        return EFI_SUCCESS;
    }

    Status = pSmmBase2->GetSmstLocation (pSmmBase2, &gSmst2);
    if (EFI_ERROR(Status)) {
        return EFI_SUCCESS;
    }

    gSmst2->SmmRegisterProtocolNotify (
                    &gEfiAcpiEnDispatchProtocolGuid,
                    CreateTerminalAcpiEnDisLink,
                    &mAcpiReg
                              );

    CreateTerminalAcpiEnDisLink(NULL,NULL,NULL);

    return EFI_SUCCESS;

}

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
