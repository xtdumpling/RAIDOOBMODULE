//****************************************************************************
//****************************************************************************
//**                                                                        **
//**             (C)Copyright 1985-2012, American Megatrends, Inc.          **
//**                                                                        **
//**                          All Rights Reserved.                          **
//**                                                                        **
//**             5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093          **
//**                                                                        **
//**                          Phone (770)-246-8600                          **
//**                                                                        **
//****************************************************************************
//****************************************************************************


/** @file ExportHiiDb.c

**/


#include <AmiDxeLib.h>
#include "SmiVariable.h"
#include <Protocol/HiiDatabase.h>

//---------------------------------------------------------------------------


/**
    Routine which executes upon Ready to Boot event - updates
    HII export database location.

    @param 
        Event   - Event of this callback function.
        Context - Pointer to the context of this callback function.

    @retval VOID

**/

VOID SaveHiiDBBufferAddress(
    IN EFI_EVENT Event,
    IN VOID *Context )
{
    EFI_STATUS Status;
    EFI_GUID ExportDatabaseGuid = EFI_HII_EXPORT_DATABASE_GUID;
    EFI_GUID EfiHiiDatabaseProtocolGuid = EFI_HII_DATABASE_PROTOCOL_GUID;
    EXPORT_HIIDB_BLOCK ExportHiiDbBlock;
    VOID* ExportDatabasePtr = NULL;
    UINTN ExportDatabaseSize = 0;
    EFI_HII_DATABASE_PROTOCOL *HiiDatabase = NULL;

    Status = pBS->LocateProtocol(
        &EfiHiiDatabaseProtocolGuid,
        NULL, 
        &HiiDatabase
    );
    if (EFI_ERROR(Status))
        return;

    // call HII driver export database routine
    Status = HiiDatabase->ExportPackageLists(
        HiiDatabase, 
        0, 
        &ExportDatabaseSize, 
        ExportDatabasePtr
    );

    if (Status == EFI_BUFFER_TOO_SMALL) {

        // allocate memory for export database buffer
        Status = pBS->AllocatePool(
            EfiRuntimeServicesData, 
            ExportDatabaseSize, 
            &ExportDatabasePtr
        );
        if (EFI_ERROR(Status))
            return;

        // retrieve export database
        Status = HiiDatabase->ExportPackageLists(
            HiiDatabase, 
            0, 
            &ExportDatabaseSize, 
            ExportDatabasePtr
        );
        if (EFI_ERROR(Status))
            return;

        // create HiiDB EFI variable
        ExportHiiDbBlock.DataSize = (UINT32)ExportDatabaseSize;
        ExportHiiDbBlock.Data = (UINT32)ExportDatabasePtr;
        pRS->SetVariable(
            L"HiiDB",
            &ExportDatabaseGuid,
            EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
            sizeof(EXPORT_HIIDB_BLOCK),
            &ExportHiiDbBlock
        );
    }

    pBS->CloseEvent(Event);
}


/**
    Entry point of the ExportHiiDbEntryPoint 
 
    @param 
        ImageHandle - Image handle
        SystemTable - Pointer to the system table

    @retval EFI_STATUS

**/

EFI_STATUS ExportHiiDbEntryPoint(
    IN EFI_HANDLE ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable )
{
    EFI_STATUS Status;
    EFI_EVENT  ReadyToBootEvent;

    InitAmiLib(ImageHandle, SystemTable);

    Status = CreateReadyToBootEvent(
        TPL_CALLBACK,
        SaveHiiDBBufferAddress,
        NULL,
        &ReadyToBootEvent
    );
    ASSERT_EFI_ERROR(Status);
    return Status;
}

//****************************************************************************
//****************************************************************************
//**                                                                        **
//**             (C)Copyright 1985-2012, American Megatrends, Inc.          **
//**                                                                        **
//**                          All Rights Reserved.                          **
//**                                                                        **
//**             5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093          **
//**                                                                        **
//**                          Phone (770)-246-8600                          **
//**                                                                        **
//****************************************************************************
//****************************************************************************
