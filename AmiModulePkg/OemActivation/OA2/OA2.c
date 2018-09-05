//****************************************************************************
//****************************************************************************
//**                                                                        **
//**             (C)Copyright 1985-2016, American Megatrends, Inc.          **
//**                                                                        **
//**                          All Rights Reserved.                          **
//**                                                                        **
//**             5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093          **
//**                                                                        **
//**                          Phone (770)-246-8600                          **
//**                                                                        **
//****************************************************************************
//****************************************************************************

//****************************************************************************
// $Header: $
//
// $Revision: $
//
// $Date: $
//****************************************************************************
/** @file OA2.c
    Updated the XSDT with the SLIC OEM Activation ACPI Table.
 
**/
//****************************************************************************


//---------------------------------------------------------------------------
#include <Token.h>
#include <AmiDxeLib.h>
#include <Protocol/AcpiTable.h>
#include <AmiHobs.h>
#include "OA2.h"

//---------------------------------------------------------------------------


/**
    This fills the PubKey and Marker structures of SLIC table.

    @param 
        *FfsGuid    - Pointer to GUID of the FFS file to read
        *Address    - Address of the buffer to read data into
        Size        - Size of the buffer

         
    @retval EFI_SUCCESS Successful
    @retval EFI_NOT_FOUND Couldn't find the binaries

**/

EFI_STATUS LocateAndLoadRawData(
    IN EFI_GUID *FfsGuid,
    IN VOID *Address,
    IN UINTN Size )
{
    EFI_STATUS                   Status;
    EFI_HANDLE                   *HandleBuffer = NULL;
    UINTN                        NumberOfHandles;
    UINTN                        Index;
    UINT32                       Authentication;
    EFI_GUID EfiFirmwareVolumeProtocolGuid = EFI_FIRMWARE_VOLUME2_PROTOCOL_GUID;
    EFI_FIRMWARE_VOLUME2_PROTOCOL *FwVolumeProtocol = NULL;

    // Locate the Firmware volume protocol
    Status = pBS->LocateHandleBuffer(
        ByProtocol,
        &EfiFirmwareVolumeProtocolGuid, 
        NULL,
        &NumberOfHandles,
        &HandleBuffer
    );
    if (EFI_ERROR(Status)) 
        return Status;
    
    // Find and read raw data
    for (Index = 0; Index < NumberOfHandles; Index++) {

        Status = pBS->HandleProtocol(
            HandleBuffer[Index],
            &EfiFirmwareVolumeProtocolGuid,
            &FwVolumeProtocol
        );
        if (EFI_ERROR(Status)) 
            continue;
        
        Status = FwVolumeProtocol->ReadSection(
            FwVolumeProtocol,
            FfsGuid,
            EFI_SECTION_RAW,
            0x0,
            &Address,
            &Size,
            &Authentication
        );
        if (Status == EFI_SUCCESS) 
            break;
    }
    
    pBS->FreePool(HandleBuffer);
    return Status;
}


/**
    This function preserves the Marker and PubKey binaries.

    @param VOID

    @retval EFI_STATUS based on result

**/

EFI_STATUS PreserveSlicBinaries(VOID)
{
    EFI_STATUS Status = EFI_NOT_FOUND;
    RECOVERY_IMAGE_HOB *RecoveryHob;
    EFI_GUID HobListGuid = HOB_LIST_GUID;
    EFI_GUID RecoveryHobGuid = AMI_RECOVERY_IMAGE_HOB_GUID;
    EFI_GUID PubKeyGuid = OEM_SLP_PUBLIC_KEY_GUID;
    EFI_GUID MarkerGuid = OEM_SLP_MARKER_GUID;
    EFI_CONFIGURATION_TABLE *Table = pST->ConfigurationTable;
    EFI_PHYSICAL_ADDRESS Addr;
    UINTN  i;
    UINT8  *FwPubKeyPtr = NULL;
    UINT8  *FwMarkerPtr = NULL;
    UINT8  *RecPubKeyPtr = NULL;
    UINT8  *RecMarkerPtr = NULL;

    // Find the Product Key place in the recovery HOB
    for( i = 0; i < pST->NumberOfTableEntries; i++, Table++ ) {

        if ( !MemCmp(&Table->VendorGuid, &HobListGuid, sizeof(EFI_GUID)) ) {

            RecoveryHob = Table->VendorTable;
            Status = FindNextHobByGuid(&RecoveryHobGuid, &RecoveryHob);
            if (!EFI_ERROR(Status)) {

                // Find Public Key and Marker binaries in the Recovery image
                Status = EFI_NOT_FOUND;

                for (Addr = RecoveryHob->Address + FV_MAIN_OFFSET; Addr < RecoveryHob->Address + RecoveryHob->ImageSize; Addr = Addr + 4 ) {
                    // Find the Public Key binary GUID
                    if ( !MemCmp((UINT8*)Addr, &PubKeyGuid, sizeof(EFI_GUID)) )
                        RecPubKeyPtr = (UINT8*)Addr;

                    // Find the Marker binary GUID
                    else if ( !MemCmp((UINT8*)Addr, &MarkerGuid, sizeof(EFI_GUID)) )
                        RecMarkerPtr = (UINT8*)Addr;

                    // if all binaries are found then find original places in the Firmware
                    if ( RecPubKeyPtr != NULL && RecMarkerPtr != NULL ) {

                        // Find Public Key and Marker binaries in the BIOS firmware
                        for ( Addr = FLASH_DEVICE_BASE_ADDRESS + FV_MAIN_OFFSET; Addr < FLASH_UPPER_ADDRESS; Addr = Addr + 4 ) { 
        
                            // Find the Public Key binary GUID
                            if ( !MemCmp((UINT8*)Addr, &PubKeyGuid, sizeof(EFI_GUID)) )
                                FwPubKeyPtr = (UINT8*)Addr;

                            // Find the Marker binary GUID
                            else if ( !MemCmp((UINT8*)Addr, &MarkerGuid, sizeof(EFI_GUID)) )
                                FwMarkerPtr = (UINT8*)Addr;
        
                            // Preserve the SLIC binaries
                            if ( FwPubKeyPtr != NULL && FwMarkerPtr != NULL ) {

                                MemCpy( 
                                    RecPubKeyPtr, 
                                    FwPubKeyPtr, 
                                    sizeof(OEM_PUBLIC_KEY_STRUCTURE) + sizeof(EFI_FFS_FILE_HEADER) + sizeof(UINT16)
                                );

                                MemCpy( 
                                    RecMarkerPtr, 
                                    FwMarkerPtr, 
                                    sizeof(WINDOWS_MARKER_STRUCTURE) + sizeof(EFI_FFS_FILE_HEADER) + sizeof(UINT16)
                                );

                                Status = EFI_SUCCESS;
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        }
    }
    TRACE((TRACE_ALWAYS,"PreserveSlicBinaries: Status = %r\n",Status));
    return Status;
}


/**
    This function publish SLIC table in the ACPI.

    @param VOID

    @retval EFI_SUCCESS - SLIC table has been published
	@retval EFI_NOT_FOUND - SLP Public Key Binaries or Marker or AcpiTableProtocol was Not Found
	@retval other EFI_ERROR - Unable to publish Acpi Table

**/

EFI_STATUS PublishSlicTable(VOID)
{
    EFI_STATUS Status;
    EFI_GUID PubKeyGuid = OEM_SLP_PUBLIC_KEY_GUID;
    EFI_GUID MarkerGuid = OEM_SLP_MARKER_GUID;
    EFI_ACPI_SLP SlpTable = {
        {SLIC_SIG, sizeof(EFI_ACPI_SLP), 0x01, 0x00, {0}, {0}, ACPI_OEM_REV, CREATOR_ID_AMI, CREATOR_REV_MS},
        {0},
        {0}
    };
    EFI_GUID EfiAcpiTableProtocolGuid = EFI_ACPI_TABLE_PROTOCOL_GUID;
    EFI_ACPI_TABLE_PROTOCOL *AcpiTableProtocol = NULL;
    UINTN  TableKey = 0;
    
    // Locate SLP Public Key binary
    Status = LocateAndLoadRawData(
        &PubKeyGuid, 
        &SlpTable.PubKey, 
        sizeof(SlpTable.PubKey)
    );
    if (EFI_ERROR(Status)) {
        TRACE((TRACE_ALWAYS, "SLP Public Key Binaries Not Found!\n"));
        return Status;
    }

    // Locate SLP Marker binary
    Status = LocateAndLoadRawData(
        &MarkerGuid, 
        &SlpTable.WinMarker, 
        sizeof(SlpTable.WinMarker)
    );
    if (EFI_ERROR(Status)) {
        TRACE((TRACE_ALWAYS, "SLP Marker Binaries are Not Found!\n"));
        return Status;
    }

    // The dummy SLIC will be not published
    if ( SlpTable.WinMarker.StructType == 0xFFFFFFFF && SlpTable.PubKey.StructType == 0xFFFFFFFF ) {
        TRACE((TRACE_ALWAYS, "OA2: Found dummy binaries. The SLIC will be not published!\n"));
        return EFI_ABORTED;
    }

    // Copy OEM ID and OEM Table ID from Marker's binary to SLIC header
    MemCpy( 
        (UINT8*)&SlpTable.Header.OemId,
        (UINT8*)&SlpTable.WinMarker.sOEMID,
        sizeof(SlpTable.WinMarker.sOEMID)
    );
    MemCpy( 
        (UINT8*)&SlpTable.Header.OemTblId,
        (UINT8*)&SlpTable.WinMarker.sOEMTABLEID,
        sizeof(SlpTable.WinMarker.sOEMTABLEID)
    );

    // Locate the ACPI table protocol
    Status = pBS->LocateProtocol(
        &EfiAcpiTableProtocolGuid,
        NULL, 
        &AcpiTableProtocol
    );
    if (EFI_ERROR(Status)) {
        TRACE((TRACE_ALWAYS, "Unable to locate AcpiTableProtocol!\n"));
        return Status;
    }

    // Publish SLIC to ACPI table
    Status = AcpiTableProtocol->InstallAcpiTable(
        AcpiTableProtocol,
        &SlpTable,
        sizeof(EFI_ACPI_SLP), 
        &TableKey
    );
    if (!EFI_ERROR(Status))
        TRACE((TRACE_ALWAYS, "SLIC table has been published.\n"));

    return Status;
}


/**
    This function is the entry point of the eModule.

    @param 
        ImageHandle  - Image handle
        *SystemTable - Pointer to the system table

    @retval EFI_SUCCESS - Slic Table was published (and preserved if we are in recovery mode)
	@retval EFI_ERROR - Slic table was not published for some reasons. 

**/

EFI_STATUS EFIAPI OA2_EntryPoint(
    IN EFI_HANDLE        ImageHandle,
    IN EFI_SYSTEM_TABLE  *SystemTable )
{
    EFI_STATUS Status = EFI_SUCCESS;

    InitAmiLib(ImageHandle, SystemTable);

    // Check if it's a Recovery Mode then preserve the Marker and PubKey binaries
    if (GetBootMode() == BOOT_IN_RECOVERY_MODE)
        Status = PreserveSlicBinaries();
    if (!EFI_ERROR(Status))
        Status = PublishSlicTable();

    return Status;
}


//****************************************************************************
//****************************************************************************
//**                                                                        **
//**             (C)Copyright 1985-2016, American Megatrends, Inc.          **
//**                                                                        **
//**                          All Rights Reserved.                          **
//**                                                                        **
//**             5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093          **
//**                                                                        **
//**                          Phone (770)-246-8600                          **
//**                                                                        **
//****************************************************************************
//****************************************************************************
