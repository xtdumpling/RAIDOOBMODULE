//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file Spmi.c
    SPMI  ACPI  Table

**/

//---------------------------------------------------------------------------

#include "Spmi.h"
#if (!IPMI_DEFAULT_HOOK_SUPPORT)
#include "IpmiInitHooks.h"
#endif

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

EFI_ACPI_TABLE_PROTOCOL      *gAcpiTable = NULL;
UINTN                        gSpmiTblHandle = 0;

UINT8 SPMI_OEM_ID[6]     = ACPI_OEM_ID_MAK;
UINT8 SPMI_OEM_TBL_ID[8] = ACPI_OEM_TBL_ID_MAK;

//
// Service Processor Management Interface Table definition
//
EFI_ACPI_SERVICE_PROCESSOR_MANAGEMENT_INTERFACE_TABLE gSpmi = {
{
  EFI_ACPI_3_0_SERVICE_PROCESSOR_MANAGEMENT_INTERFACE_TABLE_SIGNATURE,
  sizeof (EFI_ACPI_SERVICE_PROCESSOR_MANAGEMENT_INTERFACE_TABLE),
  EFI_ACPI_SERVICE_PROCESSOR_MANAGEMENT_INTERFACE_TABLE_REVISION,
  //
  // Checksum will be updated at runtime
  //
  0x00,
  //
  // It is expected that these values will be programmed at runtime
  //
  ' ', ' ', ' ', ' ', ' ', ' ', // OEM ID
  0,  // OEM Table ID
  EFI_ACPI_OEM_SPMI_REVISION,
  0,  // Creator ID
  0,  // Creator Revision
 },
  //
  // Beginning of SPMI specific fields
  //
 {0,     // Interface Type
  0x01,  // Reserved should be 01h
  0,     // IPMI Specification Version
  SPMI_INTERRUPT_TYPE,
  SPMI_GPE,
  0,//Reserved
  PCI_DEVICE_FLAG,
  GLOBAL_SYSTEM_INTERRUPT,
  { // GAS Structure
    ACPI_ADDRESS_SPACE_TYPE_IO,
    REG_BIT_WIDTH_BYTE,
    REG_BIT_OFFSET0,
    SPMI_ACCESS_BYTE,
    0,  // Address
  },
  UID_BYTE1,
  UID_BYTE2,
  UID_BYTE3,
  UID_BYTE4,
  0 // Reserved
 }
};

#if (!IPMI_DEFAULT_HOOK_SUPPORT)
extern IPMI_OEM_SPMI_UPDATE_HOOK IPMI_OEM_SPMI_UPDATE_HOOK_LIST EndOfIpmiOemSpmiUpdateHookListFunctions;
IPMI_OEM_SPMI_UPDATE_HOOK* gIpmiOemSpmiUpdateHookList[] = {IPMI_OEM_SPMI_UPDATE_HOOK_LIST NULL};
#endif

//---------------------------------------------------------------------------

#if (!IPMI_DEFAULT_HOOK_SUPPORT)
/**
    Initiates the SPMI update hooks of OEM.

    @param BootServices     - Pointer the set of Boot services.
    @param SpmiData         - Pointer to the Spmi table data.

    @return EFI_STATUS      - Return status of the Hook.
*/

EFI_STATUS
IpmiOemSpmiUpdateHook (
  IN CONST EFI_BOOT_SERVICES            *BootServices,
  IN       VOID                         *SpmiData
)
{
    EFI_STATUS  Status;
    UINTN       Index;

    for (Index = 0; gIpmiOemSpmiUpdateHookList[Index]; Index++) {
        Status = gIpmiOemSpmiUpdateHookList[Index] (BootServices, SpmiData);
    }
    return Status;
}
#endif

/**

    This function will update the IPMI ASL object for IPMI specification Version

    @param IpmiSpecRevision Specification version number

    @return VOID

**/

VOID
UpdateIpmiAslObject (
  IN  UINT16 IpmiSpecRevision )

{
    EFI_STATUS                Status;
    EFI_PHYSICAL_ADDRESS      DsdtAddr = 0;

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI:Getting DSDT\n"));
    Status = LibGetDsdt (&DsdtAddr, EFI_ACPI_TABLE_VERSION_ALL);
    if (EFI_ERROR (Status)){
        DEBUG ((EFI_D_ERROR,"IPMI:Failed to get DSDT\n"));
        return;
    }

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI:Updating SRVV ASL Object\n"));
    Status = UpdateAslNameObject ((ACPI_HDR*)DsdtAddr,(UINT8*)"SRVV", (UINT64)IpmiSpecRevision);
    if (EFI_ERROR (Status)) {
        DEBUG ((EFI_D_ERROR, "IPMI:Failed to update SRVV ASL Object\n"));
    }
}

/**
    Updates the SPMI table and creates the SPMI ACPI Table

    @param Event Event details
    @param Context Pointer to additional context

    @return VOID

**/

VOID
EFIAPI
PublishSpmiAcpiTables(
   IN  EFI_EVENT     Event,
   IN  VOID          *Context )
{
    EFI_STATUS                                                 Status = EFI_SUCCESS;
    EFI_ACPI_SERVICE_PROCESSOR_MANAGEMENT_INTERFACE_TABLE      *SpmiTable = NULL;
    EFI_BMC_SELF_TEST_LOG_PROTOCOL        *BmcSelfTestProtocol = NULL;
    UINT16                                IpmiSpecRevision = 0x00;

    Status = gBS->LocateProtocol (
                &gEfiAcpiTableProtocolGuid,
                NULL,
                (VOID **)&gAcpiTable);
    DEBUG((EFI_D_INFO, "Locate ACPI Table protocol Status: %r \n", Status));
    if(EFI_ERROR (Status)) {
        return;
    }

    Status = gBS->AllocatePool(
                 EfiRuntimeServicesData,
                 sizeof(EFI_ACPI_SERVICE_PROCESSOR_MANAGEMENT_INTERFACE_TABLE),
                 (VOID **)&SpmiTable);
    ASSERT(SpmiTable);
    if(!SpmiTable) {
        return;
    }

    //
    // Locate the BMC self test protocol
    //
    Status = gBS->LocateProtocol (
                &gEfiBmcSelfTestLogProtocolGuid,
                NULL,
                (VOID **)&BmcSelfTestProtocol );
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "gEfiBmcSelfTestLogProtocolGuid: Status:  %r\n", Status));

    //
    // Fill in table
    //
    gBS->CopyMem(
        SpmiTable,
        &gSpmi,
        sizeof(EFI_ACPI_SERVICE_PROCESSOR_MANAGEMENT_INTERFACE_TABLE));

    gBS->CopyMem(
            SpmiTable->AcpiHeader.OemId,
            SPMI_OEM_ID,
            6);
    gBS->CopyMem(
            (UINT8 *)&SpmiTable->AcpiHeader.OemTableId,
            SPMI_OEM_TBL_ID,
            8);
    SpmiTable->AcpiHeader.CreatorId = 0x2E494D41;//"AMI."

    //
    // Interface Type
    //
    if (IPMI_SYSTEM_INTERFACE == KCS_INTERFACE) { // KCS
        SpmiTable->SpmiData.InterfaceType = KCS_INTERFACE;
        SpmiTable->SpmiData.BaseAddress.Address = IPMI_KCS_BASE_ADDRESS;
// APTIOV_SERVER_OVERRIDE_START : SPMI RegBitWidth value as 8.
//    }

//    if (IPMI_SYSTEM_INTERFACE == BT_INTERFACE) { // BT
    } else if (IPMI_SYSTEM_INTERFACE == BT_INTERFACE) { // BT
// APTIOV_SERVER_OVERRIDE_END : SPMI RegBitWidth value as 8.
        SpmiTable->SpmiData.InterfaceType = BT_INTERFACE;
        SpmiTable->SpmiData.BaseAddress.Address = IPMI_BT_BASE_ADDRESS;
// APTIOV_SERVER_OVERRIDE_START : SPMI RegBitWidth value as 8.
#if 0
    }

    SpmiTable->SpmiData.BaseAddress.AddrSpcID = 0x01; // System I/O
    SpmiTable->SpmiData.BaseAddress.RegBitWidth = 0x00;
    SpmiTable->SpmiData.BaseAddress.RegBitOffs = 0x00;
    SpmiTable->SpmiData.BaseAddress.AccessSize = 0x01; // Byte Access
    if (IPMI_SYSTEM_INTERFACE == SSIF_INTERFACE) { // SSIF
#endif
    } else if (IPMI_SYSTEM_INTERFACE == SSIF_INTERFACE) { // SSIF
// APTIOV_SERVER_OVERRIDE_END : SPMI RegBitWidth value as 8.
        SpmiTable->SpmiData.InterfaceType = SSIF_INTERFACE;
        SpmiTable->SpmiData.BaseAddress.Address = IPMI_SSIF_SLAVE_ADDRESS;  // Saving 7 bit slave address
        SpmiTable->SpmiData.BaseAddress.AddrSpcID = 0x04; // SMBUS
// APTIOV_SERVER_OVERRIDE_START : SPMI RegBitWidth value as 8.
        SpmiTable->SpmiData.BaseAddress.RegBitWidth = 0x00;
// APTIOV_SERVER_OVERRIDE_END : SPMI RegBitWidth value as 8.
    }
    //
    // Update IPMI specification version
    //
    if ( BmcSelfTestProtocol != NULL ) {
        IpmiSpecRevision = (UINT16)(BmcSelfTestProtocol->GetDeviceIdResponse.SpecificationVersion & 0xF0);
        IpmiSpecRevision |= (UINT16)((BmcSelfTestProtocol->GetDeviceIdResponse.SpecificationVersion & 0x0F) << 8 );
        SpmiTable->SpmiData.SpecificationRevision = IpmiSpecRevision;
        UpdateIpmiAslObject (IpmiSpecRevision);
    }

#if (!IPMI_DEFAULT_HOOK_SUPPORT)
    Status = IpmiOemSpmiUpdateHook (gBS, (VOID *)&SpmiTable->SpmiData);
    if (EFI_ERROR (Status)) {
        SERVER_IPMI_DEBUG ((EFI_D_ERROR, "IpmiOemSpmiUpdateHook is failed and Status is %r and initializing unspecified values.\n", Status));
        SpmiTable->SpmiData.BaseAddress.Address = 0x00;
        SpmiTable->SpmiData.BaseAddress.AddrSpcID = 0x00; // System Memory.
    }
#endif

    //
    // Add SPMI table
    //
    Status = gAcpiTable->InstallAcpiTable(
                gAcpiTable,
                SpmiTable,
                sizeof (EFI_ACPI_SERVICE_PROCESSOR_MANAGEMENT_INTERFACE_TABLE),
                &gSpmiTblHandle);
    DEBUG((EFI_D_INFO, "AcpiTable->InstallAcpiTable(SPMI) = %r\n", Status));
    ASSERT_EFI_ERROR(Status);

    //
    // Free memory used for table
    //
    gBS->FreePool(SpmiTable);

    gBS->CloseEvent(Event);
}

/**

    Register notification callback on AcpiSupportprotocol and AcpiTableprotocol to publish SPMI table
    and update IPMI Specification Version ASL Object update.

    @return EFI_STATUS
**/

EFI_STATUS 
SpmiSupport (
    VOID )
{
    EFI_STATUS           Status;
    EFI_EVENT            AcpiEvent;
    
    Status = gBS->CreateEventEx(
                EVT_NOTIFY_SIGNAL,
                TPL_CALLBACK,
                PublishSpmiAcpiTables,
                NULL,
                &gEfiAcpiTableGuid,
                &AcpiEvent
                );
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: Adding PublishSpmiAcpiTables to gEfiAcpiTableGuid event group, Status: %r\n", Status));
    ASSERT_EFI_ERROR(Status); 

    //
    // If AcpiTable protocol has been installed we can use it right away
    //
    PublishSpmiAcpiTables( AcpiEvent, NULL);

    return EFI_SUCCESS;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
