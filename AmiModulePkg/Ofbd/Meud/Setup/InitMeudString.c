//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2012, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

//**********************************************************************
// $Header: $
//
// $Revision: $
//
// $Date: $
//**********************************************************************
// Revision History
// ----------------
// $Log:  $
// 
// 
//**********************************************************************
/** @file InitMeudString.c

**/
//**********************************************************************
#include "Efi.h"
#include "Token.h"
#include <Library/DebugLib.h>
#include <AmiLib.h>
#include <AmiDxeLib.h>
#include <AmiCspLib.h>
#include "../../Ofbd.h"
#include "../Meud.h"
#if (EFI_SPECIFICATION_VERSION > 0x20000)
#include <Protocol/HiiString.h>
#include <Protocol/HiiDatabase.h>
#else
#include <Protocol/Hii.h>
#endif
#include <Setup.h>
#include <Protocol/HiiConfigAccess.h>
#if defined(MEFwUpdLcl_SUPPORT) && (MEFwUpdLcl_SUPPORT == 1)
#include <Protocol/MeFwUpdLclProtocol.h>
#endif

extern EFI_GUID guidHII;
#if (EFI_SPECIFICATION_VERSION > 0x20000)
EFI_STATUS
MeudSetupCallbackFunction(
    EFI_HII_HANDLE  HiiHandle,
    UINT16          Class,
    UINT16          SubClass,
    UINT16          Key
);
#else
EFI_STATUS
MeudSetupCallbackFunction(
    IN  EFI_FORM_CALLBACK_PROTOCOL  *This,
    IN  UINT16                      KeyValue,
    IN  EFI_IFR_DATA_ARRAY          *Data,
    OUT EFI_HII_CALLBACK_PACKET     **Packet
);
EFI_FORM_CALLBACK_PROTOCOL MEUDSetupCallBack = { NULL,NULL,MEUDSetupCallbackFunction };
#endif
/**
    TSE Callbeck Function.
    To make ME enter Disable Mode.

    @param VOID

    @retval VOID

**/
#if (EFI_SPECIFICATION_VERSION > 0x20000)
EFI_STATUS
MEUDSetupCallbackFunction(
    EFI_HII_HANDLE  HiiHandle,
    UINT16          Class,
    UINT16          SubClass,
    UINT16          Key
)
#else
EFI_STATUS
MEUDSetupCallbackFunction(
    IN  EFI_FORM_CALLBACK_PROTOCOL  *This,
    IN  UINT16                      KeyValue,
    IN  EFI_IFR_DATA_ARRAY          *Data,
    OUT EFI_HII_CALLBACK_PACKET     **Packet
)
#endif
{
    CALLBACK_PARAMETERS *Callback;

	Callback = GetCallbackParameters();
	if( (Callback->Action == EFI_BROWSER_ACTION_RETRIEVE) ||
	    (Callback->Action == EFI_BROWSER_ACTION_FORM_OPEN) ||
	    (Callback->Action == EFI_BROWSER_ACTION_FORM_CLOSE) ||
	    (Callback->Action == EFI_BROWSER_ACTION_DEFAULT_STANDARD) ||
	    (Callback->Action == EFI_BROWSER_ACTION_DEFAULT_MANUFACTURING) )
	    return EFI_SUCCESS;

    DEBUG((DEBUG_INFO,"[ME Update] In Setup Callback item !! KeyValue = %x \n", Key));

#if defined(MEFwUpdLcl_SUPPORT) && (MEFwUpdLcl_SUPPORT == 1)
{
    EFI_STATUS                  Status;
    static EFI_GUID             MeFwUpdLclProtocolGuid = ME_FW_UPD_LOCAL_PROTOCOL_GUID;
    ME_FW_UPDATE_LOCAL_PROTOCOL *MeFwUpdProtocol = NULL;
    Status = pBS->LocateProtocol( &MeFwUpdLclProtocolGuid, \
                                        NULL, (VOID**)&MeFwUpdProtocol );
    if( !EFI_ERROR(Status) ) MeFwUpdProtocol->HmrfpoEnable( MeFwUpdProtocol );
}
#else
    IoWrite8( 0xB2, Disable_ME_SW_SMI );
#endif

    return EFI_SUCCESS;
}
/**
    Register a Setup Item CallBack Info.

    @param HiiHandle 
    @param Class 

    @retval VOID

**/
EFI_STATUS
InitMEUDInfo(
    EFI_HII_HANDLE  HiiHandle,
    UINT16          Class
)
{
	return EFI_SUCCESS;
}
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2012, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
