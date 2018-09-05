//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//  Rev. 1.01
//    Bug Fix:  Correct SMI setting.
//    Reason:
//    Auditor:  Kasber Chen
//    Date:     Jul/18/2016
//
//  Rev. 1.00
//    Bug Fix:  Add SmcBMCSMI module.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Jul/13/2016
//
//**********************************************************************
//<AMI_FHDR_START>
//
// Name:  SmcBmcSMI.c
//
// Description:
//
//<AMI_FHDR_END>
//**********************************************************************

#include "Token.h"
#include <Library/IoLib.h>
#include <Library/PciLib.h>
#include <Library/DebugLib.h>
#include <Protocol/SmmGpiDispatch2.h>
#include <Library/SmmServicesTableLib.h>
#include <Protocol/IPMITransportProtocol.h>
#include <Include/IpmiNetFnAppDefinitions.h>
#include <Platform.h>
#include <GpioConfig.h>
#include <Library/GpioLib.h>
#include "Register/PchRegsPmc.h"
#include "Register/PchRegsGpio.h"

static EFI_IPMI_TRANSPORT *mIpmiTransport;

EFI_STATUS
SmcBmcSMIGpiSmi(
    IN	EFI_HANDLE	DispatchHandle,
    IN	CONST VOID	*DispatchContext,
    IN OUT	VOID	*CommonBuffer,
    IN OUT	UINTN	*CommonBufferSize
)
{
    EFI_STATUS Status;
    UINT8 ResponseData[4];
    UINT8 ResponseDataSize=4;
    UINT8 Data8;
    UINT8 Intrusion;

    DEBUG((-1, "SmcBmcSMIGpiSmi entry.\n"));
    Status = mIpmiTransport->SendIpmiCommand (
                 mIpmiTransport,
                 0x30,
                 0,
                 0x6e,
                 NULL,
                 0,
                 ResponseData,
                 &ResponseDataSize);
//Get the BMC data if intrusion be trigger
    Intrusion = ResponseData[0] & BIT1;
//Clear the intrusion pin & notify BMC.
    if(Intrusion) {
        Data8 = IoRead8 (PCH_TCO_BASE_ADDRESS + R_PCH_TCO2_STS);
		Data8&=~BIT1; // Clear status including INTRD_DET.
		IoWrite8 (PCH_TCO_BASE_ADDRESS + R_PCH_TCO2_STS, Data8);
        ResponseData[0] &=~BIT1;

        Status = mIpmiTransport->SendIpmiCommand (
                     mIpmiTransport,
                     0x30,
                     0,
                     0x6f,
                     ResponseData,
                     4,
                     NULL,
                     0);
    }
    DEBUG((-1, "SmcBmcSMIGpiSmi end.\n"));
    return Status;

}

EFI_STATUS
InitializeSmcBmcSMI(
    IN	EFI_HANDLE	ImageHandle,
    IN	EFI_SYSTEM_TABLE	*SystemTable
)
{
    EFI_STATUS	Status;
    EFI_SMM_GPI_DISPATCH2_PROTOCOL	*GpiDispatch = NULL;
    EFI_SMM_GPI_REGISTER_CONTEXT	GpiContext;
    EFI_HANDLE	GpiHandle;
    UINT32	mGpioSmiPad = SMI_BMC_PIN;

    DEBUG((-1, "InitializeSmcBmcSMI entry.\n"));

    Status = gSmst->SmmLocateProtocol (
                 &gEfiSmmIpmiTransportProtocolGuid,
                 NULL,
                 &mIpmiTransport);

    if (EFI_ERROR (Status)) {
        return Status;
    }

    Status = gSmst->SmmLocateProtocol (
                 &gEfiSmmGpiDispatch2ProtocolGuid,
                 NULL,
                 &GpiDispatch);

    if(EFI_ERROR(Status))	return Status;

    GpioGetGpiSmiNum(mGpioSmiPad, &GpiContext.GpiNum);
    GpioClearGpiSmiSts(mGpioSmiPad);

    Status = GpiDispatch->Register(
		    GpiDispatch,
		    SmcBmcSMIGpiSmi,
		    &GpiContext,
		    &GpiHandle);

    DEBUG((-1, "InitializeSmcBmcSMI end. Status = %r\n", Status));
    return EFI_SUCCESS;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2011, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
