//****************************************************************************
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//****************************************************************************
//****************************************************************************
//  File History
//
//    Rev. 1.00
//    Reason:	Initialize revision.
//    Auditor:	Kasber Chen
//    Date:	10/03/2016
//
//**************************************************************************//
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <PciBus.h>
#include <Token.h>
#include "SmcLib.h"
#include "SmcCspLib.h"
#include "SmcAOC.h"

EFI_GUID gSmcAOCDxeProtocolGuid = EFI_SMC_AOC_DXE_PROTOCOL_GUID;

DXE_SMC_AOC_PROTOCOL	SmcAOCDxeProtocol = {
    SmcAOCBifurcation
};

EFI_STATUS
SmcAOCInit(
    IN	EFI_HANDLE		ImageHandle,
    IN	EFI_SYSTEM_TABLE	*SystemTable
)
{
    EFI_STATUS	Status = EFI_SUCCESS;
    EFI_HANDLE	Handle = NULL;
    
    DEBUG((-1, "SmcAOCInit start.\n"));

    Status = gBS->InstallProtocolInterface(
        			&Handle,
        			&gSmcAOCDxeProtocolGuid,
        			EFI_NATIVE_INTERFACE,
        			&SmcAOCDxeProtocol);
    DEBUG((-1, "InstallProtocolInterface gSmcAOCDxeProtocolGuid %r.\n", Status));

    DEBUG((-1, "SmcAOCInit end.\n"));
    
    return	EFI_SUCCESS;
}
