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
EFIAPI
PttHciReceive(
    OUT     UINT8     *FtpmBuffer,
    OUT     UINT32    *RespSize
)
{
    return EFI_UNSUPPORTED;
}


EFI_STATUS
EFIAPI
PttHciSend(
    IN     UINT8      *FtpmBuffer,
    IN     UINT32     DataLength
)
{

    return EFI_UNSUPPORTED;
}


BOOLEAN
EFIAPI
PttHciPresenceCheck()
{
    return FALSE;
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
