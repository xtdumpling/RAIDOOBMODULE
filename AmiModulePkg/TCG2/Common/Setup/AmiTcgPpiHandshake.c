//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2010, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**     5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093            **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
//**********************************************************************
// $Header:  $
//
// $Revision: $
//
// $Date: $
//**********************************************************************
//**********************************************************************
//<AMI_FHDR_START>
//
// Name:
//
// Description:
//
//
//<AMI_FHDR_END>
//*************************************************************************

#include "Token.h"
#include <Efi.h>
#include <AmiTcg/TCGMisc.h>


extern EFI_GUID gAmiPpiGuid;
extern EFI_BOOT_SERVICES    *gBS;
extern EFI_SYSTEM_TABLE     *gST;
extern EFI_RUNTIME_SERVICES *gRT;
extern EFI_HANDLE           gImageHandle;

static BOOLEAN              ConInInitalised = FALSE;



VOID AmiTcgDummyTseFunction(IN EFI_EVENT Event, IN VOID *Context) {}

VOID AmiTcgInstallPpiGuid(VOID );

void ConsoleInCallback(
    IN EFI_EVENT ev,
    IN VOID      *ctx)
{
    EFI_STATUS              Status;
    EFI_EVENT                 Tcgev;
    static void                     *reg;

    if(ConInInitalised  == TRUE ) return;

    if( gST->ConIn == NULL  )
    {
        Status = gBS->CreateEvent( EFI_EVENT_NOTIFY_SIGNAL,
                                   TPL_CALLBACK,
                                   ConsoleInCallback,
                                   0,
                                   &Tcgev );

        if(Status)return;

        Status = gBS->RegisterProtocolNotify(
                     &gEfiSimpleTextInProtocolGuid,
                     Tcgev,
                     &reg );

        if(Status)return;
    }
    else
    {
        AmiTcgInstallPpiGuid();  //signal Ppi Event
        ConInInitalised =TRUE;
    }

}



//****************************************************************************************
//<AMI_PHDR_START>
//
// Procedure: Install AMI PPI GUID
//
// Description:
//
// Input:       VOID
//
// Output:      BOOLEAN
//
// Modified:
//
// Referrals:
//
// Notes:
//<AMI_PHDR_END>
//****************************************************************************************
VOID AmiTcgInstallPpiGuid(VOID )
{
    EFI_EVENT                   SigEvent;
    EFI_STATUS Status;

    if( gST->ConIn == NULL  )
    {
        ConsoleInCallback(NULL, NULL);
        return;

    }
    else
    {
        Status = gBS->CreateEventEx(
                     EVT_NOTIFY_SIGNAL,
                     TPL_CALLBACK,
                     AmiTcgDummyTseFunction,
                     NULL,
                     &gAmiPpiGuid,
                     &SigEvent);

        gBS->SignalEvent(SigEvent);
        gBS->CloseEvent(SigEvent);
    }
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2010, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**     5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093            **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
//**********************************************************************
