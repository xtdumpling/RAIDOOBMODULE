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

//*****************************************************************************
// $Header: $
//
// $Revision: $
//
// $Date: $
//*****************************************************************************
/** @file OA3.c
    Microsoft OEM Activation 3.0

**/
//*****************************************************************************


#include <AmiDxeLib.h>
#include <Protocol/AcpiTable.h>
#include <Ppi/FwVersion.h>
#include <AmiHobs.h>
#include "OA3.h"
#include <AcpiOemElinks.h>

//----------------------------------------------------------------------------



/**
    This function locate resource binary by GUID. 

    @param 
        pResourceGuid        - pointer to recourse GUID
        pResourceSectionGuid - pointer to recourse section GUID
        Address              - pointer to returning address of the resource

    @retval EFI_STATUS

**/

EFI_STATUS LocateSectionResource(
    IN  EFI_GUID *pResourceGuid,
    IN  EFI_GUID *pResourceSectionGuid,
    OUT UINT8 **Address )
{
    EFI_STATUS Status;
    EFI_HANDLE *HandleBuffer = NULL;
    UINTN  NumberOfHandles;
    UINTN  Index;
    UINT32 Authentication;
    UINTN  i;
    EFI_GUID *pGuid = NULL;
    UINTN  SectionSize;
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
        return EFI_NOT_FOUND;
    
    // Find and read raw data
    for (Index = 0; Index < NumberOfHandles; Index++) {

        Status = pBS->HandleProtocol(
            HandleBuffer[Index],
            &EfiFirmwareVolumeProtocolGuid,
            &FwVolumeProtocol
        );
        if (EFI_ERROR(Status)) 
            continue;
        
        i = 0;
        while(1) {
            SectionSize = 0;
            Status = FwVolumeProtocol->ReadSection(
                FwVolumeProtocol,
                pResourceGuid,
                EFI_SECTION_FREEFORM_SUBTYPE_GUID,
                i++,
                &pGuid,
                &SectionSize,
                &Authentication
            );
            if (EFI_ERROR(Status))
                break;
            else if ( !guidcmp(pGuid, pResourceSectionGuid ) ) {
                *Address = (UINT8*)pGuid;
                goto LocateSectionResource_exit;
            }
            pBS->FreePool(pGuid);
            pGuid = NULL;
        } 
    }
    
LocateSectionResource_exit:
    pBS->FreePool(HandleBuffer);
    return Status;
}

#if OEM_ACTIVATION_TABLE_LOCATION == 0
/**
    This function locates the Product Key. 

    @param Address - pointer to returning address of the Product Key structure.

    @retval EFI_STATUS

**/

EFI_STATUS GetProductKey( 
    IN EFI_PHYSICAL_ADDRESS StartAddr, 
    IN EFI_PHYSICAL_ADDRESS EndAddr, 
    OUT UINT8 **Address )
{
    EFI_STATUS Status = EFI_NOT_FOUND;
    EFI_GUID FidTableSectionGuid = FID_TABLE_SECTION_GUID;
    EFI_PHYSICAL_ADDRESS i;

    for ( i = StartAddr; i < EndAddr; i = i + 4 )
        if ( !MemCmp((UINT8*)(UINTN)i, &FidTableSectionGuid, sizeof(EFI_GUID)) ) {

            // skip the GUID
            i += sizeof(EFI_GUID);

            // Check version of the FID table
            if ( ((FW_VERSION*)(UINTN)i)->StructVersion < FID_SUPPORTED_VERSION ) {
                TRACE((TRACE_ALWAYS, "OEM Activation: FID table version %d unsupported!!!\n",((FW_VERSION*)i)->StructVersion));
                Status = EFI_UNSUPPORTED;
                break;
            }

            // skip the ACPI header
            *Address = (UINT8*)((FW_VERSION*)(UINTN)i)->OemActivationKey;
            TRACE((TRACE_ALWAYS, "OEM Activation: Product Key Address %X\n",*Address));

            Status = EFI_SUCCESS;
            break;
        }
    return Status;
}
#endif

/**
    This function preserves the Product Key.

    @param VOID

    @retval EFI_STATUS

**/

EFI_STATUS PreserveProductKey(VOID)
{
    EFI_STATUS Status = EFI_NOT_FOUND;
    RECOVERY_IMAGE_HOB *RecoveryHob; 
    EFI_GUID HobListGuid = HOB_LIST_GUID;
    EFI_GUID RecoveryHobGuid = AMI_RECOVERY_IMAGE_HOB_GUID;
    EFI_CONFIGURATION_TABLE *Table = pST->ConfigurationTable;
    UINTN  i;
    UINT8  *ProductKey;
    UINT8  *ProductKeyPreserve;

    // Find the Product Key place in the recovery HOB
    for( i = 0; i < pST->NumberOfTableEntries; i++, Table++ ) {

        if ( !MemCmp(&Table->VendorGuid, &HobListGuid, sizeof(EFI_GUID)) ) {

            RecoveryHob = Table->VendorTable;
            Status = FindNextHobByGuid(&RecoveryHobGuid, &RecoveryHob);
            if (!EFI_ERROR(Status)) {

#if OEM_ACTIVATION_TABLE_LOCATION == 0
                // Get the Product Key pointer in the recovery HOB
                Status = GetProductKey(
                    RecoveryHob->Address + FLASH_SIZE - FV_BB_SIZE, 
                    RecoveryHob->Address + FLASH_SIZE - sizeof(EFI_GUID), 
                    &ProductKeyPreserve
                );
                if (!EFI_ERROR(Status)) {
                    // Get the Product Key pointer in the Flash memory
                    Status = GetProductKey(FV_BB_BASE, FV_BB_END_ADDR, &ProductKey);
                    if (EFI_ERROR(Status))
                        break;
                    MemCpy( ProductKeyPreserve, ProductKey, sizeof(EFI_ACPI_MSDM_TABLE) - sizeof(ACPI_HDR) );
                }
#else
                ProductKeyPreserve = (UINT8*)(RecoveryHob->Address + FLASH_SIZE - OEM_ACTIVATION_TABLE_OFFSET);
                ProductKey = (UINT8*)OEM_ACTIVATION_TABLE_ADDRESS;
                MemCpy( ProductKeyPreserve, ProductKey, sizeof(EFI_ACPI_MSDM_TABLE) - sizeof(ACPI_HDR) );
                Status = EFI_SUCCESS;
#endif
            }
        }
    }
    TRACE((TRACE_ALWAYS,"PreserveProductKey: Status %r\n",Status));
    return Status;
}


/**
    This function is the entry point of the eModule.

    @param 
        ImageHandle  - Image handle
        *SystemTable - Pointer to the system table

    @retval EFI_STATUS

**/

EFI_STATUS EFIAPI OA3_EntryPoint(
    IN EFI_HANDLE        ImageHandle,
    IN EFI_SYSTEM_TABLE  *SystemTable )
{
#if OEM_ACTIVATION_TABLE_LOCATION == 0 
    EFI_GUID FidTableGuid = FID_TABLE_GUID;
    EFI_GUID FidTableSectionGuid = FID_TABLE_SECTION_GUID;
    FW_VERSION *Fid;
    UINT8  *FidSection = NULL;
#endif
    EFI_STATUS Status;
    EFI_ACPI_MSDM_TABLE *Msdm = NULL;
    EFI_OA3_MSDM_STRUCTURE MsdmVariable = {0};
    EFI_GUID AmiGlobalVariableGuid = AMI_GLOBAL_VARIABLE_GUID;
    UINTN  i;
    UINT8    MSDM_OEM_ID[6]     = ACPI_OEM_ID_MAK;     
    UINT8    MSDM_OEM_TBL_ID[8] = ACPI_OEM_TBL_ID_MAK; 
    
    InitAmiLib(ImageHandle, SystemTable);

    // Check if it's a Recovery Mode then preserve the Product Key
    if (GetBootMode() == BOOT_IN_RECOVERY_MODE) {
        Status = PreserveProductKey();
        return Status;
    }
#if OEM_ACTIVATION_TABLE_LOCATION == 0    
    // Locate the $FID table
    Status = LocateSectionResource(
        &FidTableGuid, 
        &FidTableSectionGuid, 
        &FidSection
    );
    if (EFI_ERROR(Status)) {
        TRACE((TRACE_ALWAYS, "OEM Activation: FID table Not Found!!!\n"));
        return Status;
    }
    Fid = (FW_VERSION*)(FidSection + sizeof(EFI_GUID)); // skip the GUID

    // Check version of the FID table
    if ( Fid->StructVersion < FID_SUPPORTED_VERSION ) {
        TRACE((TRACE_ALWAYS, "OEM Activation: FID table version %d is unsupported!!!\n",Fid->StructVersion));
        return EFI_UNSUPPORTED;
    }
#endif

    // Create MSDM table
    Status = pBS->AllocatePool( EfiACPIMemoryNVS, sizeof(EFI_ACPI_MSDM_TABLE), &Msdm );
    if (EFI_ERROR(Status))
        return Status;
    Msdm->Header.Signature  = MSDM_SIG;
    Msdm->Header.Length     = sizeof(EFI_ACPI_MSDM_TABLE);
    Msdm->Header.Revision   = MSDM_REVISION;
    Msdm->Header.OemRev     = ACPI_OEM_REV;
    Msdm->Header.CreatorId  = CREATOR_ID_AMI;
    Msdm->Header.CreatorRev = CREATOR_REV_MS;

    // Fill OEM ID and OEM Table ID 

    for (i=0; i<6; i++) Msdm->Header.OemId[i] = MSDM_OEM_ID[i];
    
    for (i=0; i<8; i++) Msdm->Header.OemTblId[i] = MSDM_OEM_TBL_ID[i];

    // Copy OEM Activation Key
    MemCpy( 
        (UINT8*)&Msdm->Version,
#if OEM_ACTIVATION_TABLE_LOCATION == 0
        (UINT8*)&Fid->OemActivationKey,         // Copy Key from the FID structure
#else
        (UINT8*)OEM_ACTIVATION_TABLE_ADDRESS,   // Copy Key from the NCB
#endif
        sizeof(EFI_ACPI_MSDM_TABLE) - sizeof(ACPI_HDR) 
    );
#if OEM_ACTIVATION_TABLE_LOCATION == 0
    pBS->FreePool(FidSection);
#endif

    // Check if there is empty Product Key 
    for ( i = 0; i < sizeof(Msdm->Data); i++ ) {

        if ( Msdm->Data[i] != 0xFF ) {

            EFI_ACPI_TABLE_PROTOCOL *AcpiTableProtocol = NULL;
            UINTN  TableKey = 0;

            // Locate the ACPI support protocol
            Status = pBS->LocateProtocol(
                &gEfiAcpiTableProtocolGuid, 
                NULL, 
                &AcpiTableProtocol
            );
            if (EFI_ERROR(Status))
                TRACE((TRACE_ALWAYS, "OEM Activation: Unable to locate AcpiSupportProtocol!\n"));
                
            else {
                // Publish MSDM ACPI table
                Status = AcpiTableProtocol->InstallAcpiTable(
                    AcpiTableProtocol,
                    Msdm,
                    sizeof(EFI_ACPI_MSDM_TABLE),
                    &TableKey
                );
                if (EFI_ERROR(Status))
                    TRACE((TRACE_ALWAYS, "OEM Activation: SetAcpiTable failed!\n"));
                else
                    TRACE((TRACE_ALWAYS, "OEM Activation: MSDM table has been published.\n"));
            }
            pBS->FreePool(Msdm);
            return Status;
        }
    }

    TRACE((TRACE_ALWAYS, "OEM Activation: Found empty Product Key.\n"));

    MsdmVariable.MsdmAddress = (EFI_PHYSICAL_ADDRESS)Msdm;
#if OEM_ACTIVATION_TABLE_LOCATION == 1
    MsdmVariable.ProductKeyAddress = (EFI_PHYSICAL_ADDRESS)OEM_ACTIVATION_TABLE_ADDRESS;
#endif

    Status = pRS->SetVariable(
        EFI_OA3_MSDM_VARIABLE,
        &AmiGlobalVariableGuid,
        EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
        sizeof(EFI_OA3_MSDM_STRUCTURE),
        &MsdmVariable
    );
    ASSERT_EFI_ERROR(Status);
    TRACE((TRACE_ALWAYS, "OEM Activation: MsdmAddress=%X\n",MsdmVariable.MsdmAddress));

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
