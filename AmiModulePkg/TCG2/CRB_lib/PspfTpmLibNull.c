//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2011, American Megatrends, Inc.            **
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
// $Header: $
//
// $Revision: $
//
// $Date: $
//**********************************************************************
//<AMI_FHDR_START>
//
// Name:    PspfTpmLib.c
//
// Description: Null library for use with the PspfTpmLib
//
//<AMI_FHDR_END>
//**********************************************************************
#include <Uefi.h>

EFI_STATUS
GetPspBar1Addr (
    IN OUT   EFI_PHYSICAL_ADDRESS *PspMmio
)
{
    return EFI_UNSUPPORTED;
}


EFI_STATUS
iTpmExecuteCommand (
    IN       VOID                 *CommandBuffer,
    IN       UINT32               CommandSize,
    IN OUT   VOID                 *ResponseBuffer,
    IN OUT   UINT32               *ResponseSize
)
{

    return EFI_UNSUPPORTED;
}


EFI_STATUS
iTpmGetInfo (
    IN OUT   UINTN                *iTPMStatus
)
{
    return EFI_UNSUPPORTED;
}


//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2011, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
