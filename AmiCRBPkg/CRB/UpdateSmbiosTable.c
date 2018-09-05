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

//**********************************************************************
// $Header:
//
// $Revision:
//
// $Date:
//**********************************************************************
// Revision History
// ----------------
// $Log:
// 
//
// 
//**********************************************************************
//<AMI_FHDR_START>
//
// Name:  UpdateSmbiosTable.c
//
// Description:	SMBIOS table is updated here
//
//<AMI_FHDR_END>
//**********************************************************************

#include <UpdateSmbiosTable.h>

EFI_SMBIOS_PROTOCOL      *gEfiSmbiosProtocol;
EFI_IIO_SYSTEM_PROTOCOL  *gIioSystemProtocol;

SLOT_INFO gSlotInfo [] = {
#if PLATFORM_TYPE == 0    
          //{ Socket,          Stack,          Slot Type,                 Data bus width,      Dev/Func number}
            { SLOT1_SOCKET_NO, SLOT1_STACK_NO, SlotTypePciExpressGen3X8,  SlotDataBusWidth8X,  SLOT1_DEV_FUN_NO},
            { SLOT2_SOCKET_NO, SLOT2_STACK_NO, SlotTypePciExpressGen3X16, SlotDataBusWidth16X, SLOT2_DEV_FUN_NO},
            { SLOT3_SOCKET_NO, SLOT3_STACK_NO, SlotTypePciExpressGen3X8,  SlotDataBusWidth8X,  SLOT3_DEV_FUN_NO},
            { SLOT4_SOCKET_NO, SLOT4_STACK_NO, SlotTypePciExpressGen3X16, SlotDataBusWidth16X, SLOT4_DEV_FUN_NO},
            { SLOT5_SOCKET_NO, SLOT5_STACK_NO, SlotTypePciExpressGen3X8,  SlotDataBusWidth8X,  SLOT5_DEV_FUN_NO},
            { SLOT6_SOCKET_NO, SLOT6_STACK_NO, SlotTypePciExpressGen3X8,  SlotDataBusWidth8X,  SLOT6_DEV_FUN_NO},
            { SLOT7_SOCKET_NO, SLOT7_STACK_NO, SlotTypePciExpressGen3X8,  SlotDataBusWidth8X,  SLOT7_DEV_FUN_NO},
            { SLOT8_SOCKET_NO, SLOT8_STACK_NO, SlotTypePciExpressGen3X16, SlotDataBusWidth16X, SLOT8_DEV_FUN_NO},
#else
            { SLOT1_SOCKET_NO, SLOT1_STACK_NO, SlotTypePciExpressGen3X16, SlotDataBusWidth16X, SLOT1_DEV_FUN_NO},
            { SLOT2_SOCKET_NO, SLOT2_STACK_NO, SlotTypePciExpressGen3X16, SlotDataBusWidth16X, SLOT2_DEV_FUN_NO},
            { SLOT3_SOCKET_NO, SLOT3_STACK_NO, SlotTypePciExpressGen3X16, SlotDataBusWidth16X, SLOT3_DEV_FUN_NO},
            { SLOT4_SOCKET_NO, SLOT4_STACK_NO, SlotTypePciExpressGen3X16, SlotDataBusWidth16X, SLOT4_DEV_FUN_NO},
            { SLOT5_SOCKET_NO, SLOT5_STACK_NO, SlotTypePciExpressGen3X16, SlotDataBusWidth16X, SLOT5_DEV_FUN_NO},
            { SLOT6_SOCKET_NO, SLOT6_STACK_NO, SlotTypePciExpressGen3X16, SlotDataBusWidth16X, SLOT6_DEV_FUN_NO},
            { SLOT7_SOCKET_NO, SLOT7_STACK_NO, SlotTypePciExpressGen3X16, SlotDataBusWidth16X, SLOT7_DEV_FUN_NO},
#endif
};

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
//
// Name: AddStructureForAdditionalSlots
//
// Description: 
//  Add SMBIOS type 9 structures based on the count given.
//
// Input:    None.
//
// Output:
//     EFI_STATUS - Based on ADD API of EFI SMBIOS protocol.     
//
// Modified:    Nothing.
//
// Referrals:   None.
//
// Notes:       None.
//---------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS
AddStructureForAdditionalSlots (
    UINT8 AddSlotCount
)
{
    SMBIOS_TABLE_TYPE9       *Type9Table;
    EFI_STATUS               Status = EFI_SUCCESS;
    EFI_SMBIOS_HANDLE        SmbiosHandle;
    UINT8                    Index;

    Type9Table = AllocateZeroPool( sizeof(SMBIOS_TABLE_TYPE9));

    Type9Table->Hdr.Type = EFI_SMBIOS_TYPE_SYSTEM_SLOTS;
    Type9Table->Hdr.Length = sizeof(SMBIOS_TABLE_TYPE9);
    Type9Table->SlotDesignation = 1;
    SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
    
    for (Index=0; Index < AddSlotCount; Index++) {
        Status = gEfiSmbiosProtocol->Add( gEfiSmbiosProtocol, NULL, &SmbiosHandle, (EFI_SMBIOS_TABLE_HEADER*) &(Type9Table->Hdr));
        DEBUG(( DEBUG_INFO, __FUNCTION__ "Adding Slot %d structure status  - %r \n",Index,Status));
    }
    
    FreePool(Type9Table);
    return Status;
}

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
//
// Name: UpdateSmbiosType9Table
//
// Description: 
//  Update the SMBIOS TYPE 9 table with proper values
//
// Input:    None.
//
// Output:
//     EFI_STATUS
//
// Modified:    Nothing.
//
// Referrals:   None.
//
// Notes:       None.
//---------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS
EFIAPI
UpdateSmbiosType9Table (
    IN  EFI_EVENT Event,
    IN  VOID      *Context
)
{
    EFI_STATUS               Status;
    UINT8                    Index;
    UINT8                    SecBus;
    UINT32                   VendorIDDeviceID;
    UINTN                    StringNumber;
    SMBIOS_TABLE_TYPE9       *Type9Table;
    EFI_SMBIOS_HANDLE        SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
    EFI_SMBIOS_TYPE          SmbiosType = EFI_SMBIOS_TYPE_SYSTEM_SLOTS;
    CHAR8                    *SlotDesignator[] = {"Slot 1", "Slot 2", "Slot 3", "Slot 4", "Slot 5", "Slot 6", "Slot 7", "Slot 8"};
#if PLATFORM_TYPE == 1
    UINT8                    SlotIndex [] = {SOCKET_3_INDEX + PORT_3A_INDEX,
                                             SOCKET_0_INDEX + PORT_3A_INDEX,
                                             SOCKET_3_INDEX + PORT_2A_INDEX,
                                             SOCKET_1_INDEX + PORT_1A_INDEX,
                                             SOCKET_2_INDEX + PORT_3A_INDEX,
                                             SOCKET_1_INDEX + PORT_2A_INDEX,
                                             SOCKET_2_INDEX + PORT_1A_INDEX,
                                             SOCKET_0_INDEX + PORT_1A_INDEX
                             };
#endif
    DEBUG(( DEBUG_INFO, __FUNCTION__ " Updating Type 9\n"));

#if PLATFORM_TYPE == 1
   //Close the event if it is present
    if (Event != NULL)
        pBS->CloseEvent(Event);
#endif

#if defined NUMBER_OF_ADDITIONAL_SYSTEM_SLOTS && NUMBER_OF_ADDITIONAL_SYSTEM_SLOTS > 0
    //Add slots in addition to slots provided by SMBIOS module
    Status = AddStructureForAdditionalSlots(NUMBER_OF_ADDITIONAL_SYSTEM_SLOTS);
    if (EFI_ERROR(Status))
        return Status;
#endif

    for (Index = 0; Index < (sizeof(gSlotInfo)/ sizeof(SLOT_INFO)); Index++) {

        // Read Type 9 structure
        Status = gEfiSmbiosProtocol->GetNext( gEfiSmbiosProtocol, &SmbiosHandle, &SmbiosType, (EFI_SMBIOS_TABLE_HEADER **) &Type9Table, NULL);
        if (EFI_ERROR(Status))
            return Status;

        if (SmbiosHandle == SMBIOS_HANDLE_PI_RESERVED) 
            break;

        //Updating common data of all slots
        Type9Table->SlotLength = SlotLengthLong;
        Type9Table->SlotDesignation = 1;
        Type9Table->SlotCharacteristics1.Provides33Volts = 0x1;
        Type9Table->SlotCharacteristics1.SharedSlot = 0x0;
        Type9Table->SlotCharacteristics2.PmeSignalSupported = 0x1;

        //Updating other data
#if PLATFORM_TYPE == 0
        Type9Table->SlotID = Index + 1;
#else
        Status = pBS->LocateProtocol( &gEfiIioSystemProtocolGuid, NULL, &gIioSystemProtocol);
        if (EFI_ERROR(Status)) {
            return Status;
        }
        Type9Table->SlotID = gIioSystemProtocol->IioGlobalData->SetupData.SLOTPSP[SlotIndex[Index]];
#endif
        Type9Table->DevFuncNum = gSlotInfo[Index].DevFun;
        Type9Table->SlotType = gSlotInfo[Index].SlotType;
        Type9Table->SlotDataBusWidth = gSlotInfo[Index].SlotDataBusWidth;

        //Updating slot's dynamic data
        Status = AmiGetPcieSegmentBus ( gSlotInfo[Index].Socket, gSlotInfo[Index].Stack, (UINT8 *) &Type9Table->SegmentGroupNum, &Type9Table->BusNum);
        if (EFI_ERROR(Status))
            return Status;
        SecBus = PciExpressRead8 (PCI_EXPRESS_LIB_ADDRESS( Type9Table->BusNum, Type9Table->DevFuncNum >> 3, Type9Table->DevFuncNum & 0x7 ,PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET));

        if (SecBus == 0xFF){
            Type9Table->CurrentUsage = SlotUsageUnknown;
        } else {

            //Checking for Slot device availability
            VendorIDDeviceID = PciExpressRead32 (PCI_EXPRESS_LIB_ADDRESS(SecBus,0,0,0));
            if (VendorIDDeviceID == 0xFFFFFFFF) {
                Type9Table->CurrentUsage  = SlotUsageAvailable;
            } else {
                Type9Table->CurrentUsage = SlotUsageInUse;
            }
        }

        // Updating Slot designator
        StringNumber = (UINTN) Type9Table->SlotDesignation;
        Status = gEfiSmbiosProtocol->UpdateString ( gEfiSmbiosProtocol, &SmbiosHandle, &StringNumber, SlotDesignator[Type9Table->SlotID - 1]);
    }
    DEBUG(( DEBUG_INFO, __FUNCTION__ "Updating Type 9 Status %r \n", Status));
    return Status;
}

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
//
// Name: UpdateSmbiosTables
//
// Description: 
//  EFI call back for EFI SMBIOS protocol. Update SMBIOS Structures
//
// Input:
//    IN  EFI_EVENT Event    - Event created for the callback
//    IN  VOID      *Context - Any arguments passed
//
// Output:
//     EFI_STATUS
//
// Modified:    Nothing.
//
// Referrals:   None.
//
// Notes:       None.
//---------------------------------------------------------------------------
//<AMI_PHDR_END>

VOID
EFIAPI
UpdateSmbiosTables (
    IN  EFI_EVENT Event,
    IN  VOID      *Context
)
{
    EFI_STATUS       Status;

#if PLATFORM_TYPE == 1
    EFI_EVENT        UpdateSmbiosType9TableEvent;
    VOID             *UpdateSmbiosType9TableRegistration;
#endif
    DEBUG(( DEBUG_INFO, __FUNCTION__ "Updating SMBIOS structures \n" ));

    Status = pBS->LocateProtocol( &gEfiSmbiosProtocolGuid, NULL, &gEfiSmbiosProtocol);
    if (EFI_ERROR(Status))
        return;

    pBS->CloseEvent(Event);

#if PLATFORM_TYPE == 1
    Status = pBS->LocateProtocol( &gEfiIioSystemProtocolGuid, NULL, &gIioSystemProtocol);
    if (EFI_ERROR(Status)) {
    //EFI callback for IIO system Protocol is created if it is not located
    Status = pBS->CreateEvent( EVT_NOTIFY_SIGNAL, TPL_CALLBACK, UpdateSmbiosType9Table, NULL, &UpdateSmbiosType9TableEvent);
        if (!EFI_ERROR(Status)) {
            Status = pBS->RegisterProtocolNotify( &gEfiIioSystemProtocolGuid, UpdateSmbiosType9TableEvent, &UpdateSmbiosType9TableRegistration);
            if (EFI_ERROR(Status))
                return;
        }
    return;
    }
#endif

    //Update SMBIOS Type 9 Structures
    Status = UpdateSmbiosType9Table(NULL, NULL);
    return;
}

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
