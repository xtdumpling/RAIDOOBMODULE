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

/** @file IpmiSmbiosTypes.c
    Installs SMBIOS type 38 and 42 structures

**/

//---------------------------------------------------------------------------

#include "IpmiBmc.h"
#include <Library/UefiLib.h>
#include <Protocol/Smbios.h>
#include <IndustryStandard/SmBios.h>
#include <Protocol/IPMISelfTestLogProtocol.h>
#if (!IPMI_DEFAULT_HOOK_SUPPORT)
#include "IpmiInitHooks.h"
#endif

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

//
// Extern Variables
//
extern EFI_IPMI_BMC_INSTANCE_DATA   *gIpmiInstance;

///
/// Management Controller Host Interface Type Identifiers
///
typedef enum {
  MCHostInterfaceTypeReserved0     = 0x00,
  MCHostInterfaceTypeReserved1     = 0x01,
  MCHostInterfaceTypeKCS           = 0x02, ///< The Keyboard Controller Style.
  MCHostInterfaceType8250UART      = 0x03, ///< 8250 UART Register Compatible.
  MCHostInterfaceType16450UART     = 0x04, ///< 16450 UART Register Compatible
  MCHostInterfaceType16550UART     = 0x05, ///< 16550/16550A UART Register Compatible
  MCHostInterfaceType16650UART     = 0x06, ///< 16650/16650A UART Register Compatible
  MCHostInterfaceType16750UART     = 0x07, ///< 16750/16750A UART Register Compatible
  MCHostInterfaceType16850UART     = 0x08, ///< 16850/16850A UART Register Compatible
  MCHostInterfaceTypeOEM           = 0xF0, ///< OEM
  MCHostInterfaceTypeReserved      = 0xF1
} MC_HOST_INTERFACE_TYPE;

///
/// Management Controller Host Interface Protocol Identifiers
///
typedef enum {
  MCHostInterfaceProtocolReserved0     = 0x00,
  MCHostInterfaceProtocolReserved1     = 0x01,
  MCHostInterfaceProtocolIPMI          = 0x02, ///< IPMI : Intelligent Platform Management Interface.
  MCHostInterfaceProtocolMCTP          = 0x03, ///< MCTP : Management Component Transport Protocol.
  MCHostInterfaceProtocolOEM           = 0xF0, ///< OEM
  MCHostInterfaceProtocolReserved      = 0xF1
} MC_HOST_INTERFACE_PROTOCOL;

#pragma pack(1)
typedef struct {
  UINT8               ProtocolType;
  UINT32              ProtocolSpecificData;
} SMBIOS_TYPE42_MC_HOST_PROTOCOL;

typedef struct {
  SMBIOS_TABLE_TYPE42             SmbiosType42;
  UINT8                           NumberOfProtocols;
  SMBIOS_TYPE42_MC_HOST_PROTOCOL  Protocol[1];
} SMBIOS_TABLE_TYPE42_INSTANCE;
#pragma pack()

#define TYPE42_MC_HOST_INTERFACE_DATA  0xFF
#define TYPE42_MC_HOST_PROTOCOL_DATA   0xFFFFFFFF
//
// NV Storage Device Address
//
#define NV_STORAGE_DEVICE_ADDRESS  0xFF
//
// Base Address Map Bit
//
#define BASE_ADDRESS_MAP_BIT_MASK     (~BIT0)
#define MMIO_MAPPED_BASE_ADDRESS      0x00
#define IO_MAPPED_BASE_ADDRESS        0x01

//
// Byte difference
//
#define DIFFERENCEBYTE    0x1
#define DIFFERENCE4BYTE   0x4
#define DIFFERENCE16BYTE  0x10

//
// Base Address Modifier (This field is unused and set to 00h for SSIF)
//    bit  7:6 - Register spacing
//        00b =  interface registers are on successive byte boundaries
//        01b = interface registers are on 32-bit boundaries
//        10b =  interface registers are on 16-byte boundaries
//        11b = reserved
//
#define REGISTER_SPACING_BYTE_BOUNDARY              ( 0x00 << 6 )
#define REGISTER_SPACING_4BYTE_BOUNDARY             ( 0x01 << 6 )
#define REGISTER_SPACING_16BYTE_BOUNDARY            ( 0x02 << 6 )
#define REGISTER_SPACING_BYTE_BOUNDARY_RESERVED     ( 0x03 << 6 )

//
//    bit 5 - reserved. Return as 0b.
//    bit 4 -  LS-bit for addresses
//        0b = Address bit 0 = 0b
//        1b = Address bit 0 = 1b
//
#define BASE_ADDRESS_MODIFIER_LS_BIT_FOR_ADDRESS(BaseAddress) ( (BaseAddress & BIT0) << 4)

//
// Interrupt Info Identifies the type and polarity of the interrupt
// associated with the IPMI system interface, if any.
//    bit 3 - 1b =  interrupt info specified
//        0b = interrupt info not specified
//    bit 2 - reserved. Return as 0b.
//    bit 1 - Interrupt Polarity.
//        1b = active high, 0b = active low.
//    bit 0 - Interrupt Trigger Mode.
//        1b = level, 0b = edge.
//
#define INTERRUPT_INFO_NOT_SPECIFIED          0x00
//
// Interrupt number for IPMI System Interface.
// 00h = unspecified / unsupported 
//

#define INTERRUPT_NUMBER_UNSPECIFIED          0x00

//
// Finding Register spacing value
//

#if (IPMI_SYSTEM_INTERFACE == KCS_INTERFACE)

#define REGISTERDIFFERNCE ( IPMI_KCS_COMMAND_PORT - IPMI_KCS_DATA_PORT )

#elif (IPMI_SYSTEM_INTERFACE == BT_INTERFACE)

#define REGISTERDIFFERNCE ( IPMI_BT_BUFFER_PORT - IPMI_BT_CTRL_PORT )

#endif

#if REGISTERDIFFERNCE == DIFFERENCEBYTE
#define  REGISTER_SPACING  REGISTER_SPACING_BYTE_BOUNDARY
#endif

#if REGISTERDIFFERNCE == DIFFERENCE4BYTE
#define REGISTER_SPACING  REGISTER_SPACING_4BYTE_BOUNDARY
#endif

#if REGISTERDIFFERNCE == DIFFERENCE16BYTE
#define  REGISTER_SPACING REGISTER_SPACING_16BYTE_BOUNDARY
#endif

//
// Base address Modifier and Interrupt INFO defines
//
#if (IPMI_SYSTEM_INTERFACE == KCS_INTERFACE)
#define BASE_ADDRESS_MODIFIER_INTERRUPT_INFO   \
              ( REGISTER_SPACING |\
                BASE_ADDRESS_MODIFIER_LS_BIT_FOR_ADDRESS(IPMI_KCS_BASE_ADDRESS) |\
                INTERRUPT_INFO_NOT_SPECIFIED \
               )
#elif (IPMI_SYSTEM_INTERFACE == BT_INTERFACE)
#define BASE_ADDRESS_MODIFIER_INTERRUPT_INFO   \
              ( REGISTER_SPACING |\
                BASE_ADDRESS_MODIFIER_LS_BIT_FOR_ADDRESS(IPMI_BT_BASE_ADDRESS) |\
                INTERRUPT_INFO_NOT_SPECIFIED \
              )
#else
#define BASE_ADDRESS_MODIFIER_INTERRUPT_INFO   0x00
#endif


//
// Macro represent size of SmBios structure end value.
// Every SmBios type ends with 0x0000.
//
#define SIZE_OF_SMBIOS_STRUCTURE_END_MACRO    sizeof(UINT16)

#if (!IPMI_DEFAULT_HOOK_SUPPORT)
extern IPMI_OEM_SMBIOS_TYPES_UPDATE_HOOK IPMI_OEM_SMBIOS_TYPES_UPDATE_HOOK_LIST EndOfIpmiOemSmbiosTypesHookListFunctions;
IPMI_OEM_SMBIOS_TYPES_UPDATE_HOOK* gIpmiOemSmbiosTypesUpdateHookList[] = { IPMI_OEM_SMBIOS_TYPES_UPDATE_HOOK_LIST NULL};
#endif

//---------------------------------------------------------------------------

#if (!IPMI_DEFAULT_HOOK_SUPPORT)
/**
    Initiates the Smbios Types structure update hooks of OEM.

    @param SmbiosTypeRecord   - Pointer to the Smbios types structure.

    @return EFI_STATUS        - Return status of the Hook.
*/

EFI_STATUS IpmiOemSmbiosTypesUpdateHook (
  IN OUT   VOID                 **SmbiosTypeRecord
)
{
    EFI_STATUS  Status;
    UINTN       Index;

    for (Index = 0; gIpmiOemSmbiosTypesUpdateHookList[Index]; Index++) {
        Status = gIpmiOemSmbiosTypesUpdateHookList [Index] (SmbiosTypeRecord);
    }
    return Status;
}
#endif

/**
    Notification function for SMBIOS protocol.
    Installs SMBIOS type 38 structure

    @param Event Event which caused this handler
    @param Context Context passed during Event Handler registration

    @return VOID

**/

VOID
EFIAPI
InstallSmbiosTypesStructure (
  IN  EFI_EVENT                             Event,
  IN  VOID                                  *Context )
{
    EFI_STATUS                       Status;
    EFI_SMBIOS_HANDLE                SmbiosHandle;
    EFI_SMBIOS_PROTOCOL             *SmbiosProtocol;
    EFI_BMC_SELF_TEST_LOG_PROTOCOL  *BmcSelfTestProtocol = NULL;
    SMBIOS_TABLE_TYPE38             *Type38Record;
    SMBIOS_TABLE_TYPE42_INSTANCE    *Type42Record;

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "%a Entered...  \n", __FUNCTION__));

    Status = gBS->LocateProtocol (
                &gEfiSmbiosProtocolGuid,
                NULL,
                (VOID **) &SmbiosProtocol);
    DEBUG ((EFI_D_INFO, "gBS->LocateProtocol gEfiSmbiosProtocolGuid protocol  status %r\n", Status));
    if (EFI_ERROR (Status)) {
        return;
    }
    Status = gBS->CloseEvent (Event);
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "CloseEvent Status:%r \n", Status));
    //
    // Allocate memory for SmBiosType38 structure
    //
    Type38Record = (SMBIOS_TABLE_TYPE38 *)AllocateZeroPool (sizeof (SMBIOS_TABLE_TYPE38) + SIZE_OF_SMBIOS_STRUCTURE_END_MACRO);
    if (Type38Record == NULL) {
        SERVER_IPMI_DEBUG ((EFI_D_ERROR, "Allocating Space for SmBios Type38Record fails...\n"));
        return;
    }

    //
    // Fill Type 38 Structure
    //
    Type38Record->Hdr.Type = EFI_SMBIOS_TYPE_IPMI_DEVICE_INFORMATION;
    Type38Record->Hdr.Length = sizeof (SMBIOS_TABLE_TYPE38);
    Type38Record->Hdr.Handle = 0xFFFF;    //To be updated by SMBIOS driver.
    
    //
    // Update IPMI Specification Version
    //
    //
    // Locate the BMC self test protocol
    //
    Status = gBS->LocateProtocol (
                    &gEfiBmcSelfTestLogProtocolGuid,
                    NULL,
                    (VOID **)&BmcSelfTestProtocol );
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "gEfiBmcSelfTestLogProtocolGuid: Status:  %r\n", Status));

    if ((!EFI_ERROR (Status)) && BmcSelfTestProtocol->IsGetDeviceIdCommandPassed) {
        //
        // In IPMI device Id command SpecificationVersion Bits 7:4 hold the least significant digit, while
        // Bits 3:0 hold the most significant digit.
        // Example:  A value of 02h indicates version 2.0.
        // SMBIOS record IPMISpecificationRevision Bits 7:4 hold the most significant digit of the version, while
        // Bits 3:0 hold the least significant bits.
        // Example: A value of 20h indicates version 2.0.
        //
        Type38Record->IPMISpecificationRevision = ( BmcSelfTestProtocol->GetDeviceIdResponse.SpecificationVersion << 0x04 | BmcSelfTestProtocol->GetDeviceIdResponse.SpecificationVersion >> 0x04 );
    }

#if (IPMI_SYSTEM_INTERFACE == KCS_INTERFACE)        // KCS Interface
    Type38Record->InterfaceType = IPMIDeviceInfoInterfaceTypeKCS;
    Type38Record->BaseAddress = ( (IPMI_KCS_BASE_ADDRESS & BASE_ADDRESS_MAP_BIT_MASK)| IO_MAPPED_BASE_ADDRESS );
#elif (IPMI_SYSTEM_INTERFACE == BT_INTERFACE)   // BT Interface
    Type38Record->InterfaceType = IPMIDeviceInfoInterfaceTypeBT;
    Type38Record->BaseAddress = ( (IPMI_BT_BASE_ADDRESS & BASE_ADDRESS_MAP_BIT_MASK)| IO_MAPPED_BASE_ADDRESS );
#elif (IPMI_SYSTEM_INTERFACE == SSIF_INTERFACE)
    Type38Record->InterfaceType = 0x04;    // SSIF Interface. Since SMBIOS spec does not added SSIF it is hard coded here.
    Type38Record->BaseAddress = IPMI_SSIF_SLAVE_ADDRESS << 1;
#else
    Type38Record->InterfaceType = IPMIDeviceInfoInterfaceTypeUnknown ;//Unknown interface
#endif

    Type38Record->I2CSlaveAddress = gIpmiInstance->SlaveAddress;
    Type38Record->NVStorageDeviceAddress = NV_STORAGE_DEVICE_ADDRESS;
    Type38Record->BaseAddressModifier_InterruptInfo = (UINT8)BASE_ADDRESS_MODIFIER_INTERRUPT_INFO;
    Type38Record->InterruptNumber = INTERRUPT_NUMBER_UNSPECIFIED;
    
#if (!IPMI_DEFAULT_HOOK_SUPPORT)
    //
    // Update Smbios Type 38 record with Oem Hooks if any.
    //
    Status = IpmiOemSmbiosTypesUpdateHook ((VOID **) &Type38Record);
    if (EFI_ERROR (Status)) {
        SERVER_IPMI_DEBUG ((EFI_D_ERROR, "IpmiOemSmbiosTypesUpdateHook TYPE 38 status is %r and initializing unspecified values.\n", Status));
        Type38Record->BaseAddress = 0x00;
        Type38Record->BaseAddressModifier_InterruptInfo = 0x00;
    }
#endif

    //
    // Now we have got the full SMBIOS 38 record, call SMBIOS protocol to add this record.
    //
    SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
    Status = SmbiosProtocol->Add (
                SmbiosProtocol,
                NULL,
                &SmbiosHandle,
                (EFI_SMBIOS_TABLE_HEADER*) Type38Record);
    DEBUG ((EFI_D_INFO, "SmbiosProtocol->Add Type38Record....  Status: %r \n", Status));
    FreePool (Type38Record);

    //
    // Allocate memory for SmBiosType42 structure
    //
    Type42Record = (SMBIOS_TABLE_TYPE42_INSTANCE *)AllocateZeroPool (sizeof (SMBIOS_TABLE_TYPE42_INSTANCE) + SIZE_OF_SMBIOS_STRUCTURE_END_MACRO);
    if (Type42Record == NULL) {
        SERVER_IPMI_DEBUG ((EFI_D_ERROR, "Allocating Space for SmBios Type42Record fails...\n"));
        return;
    }
    
    //
    // Fill Type 42 Structure
    //
    Type42Record->SmbiosType42.Hdr.Type = EFI_SMBIOS_TYPE_MANAGEMENT_CONTROLLER_HOST_INTERFACE;
    Type42Record->SmbiosType42.Hdr.Length = sizeof (SMBIOS_TABLE_TYPE42_INSTANCE);
    Type42Record->SmbiosType42.Hdr.Handle = 0xFFFF;    //To be updated by SMBIOS driver.
#if (IPMI_SYSTEM_INTERFACE == KCS_INTERFACE)        // KCS Interface    
    Type42Record->SmbiosType42.InterfaceType = MCHostInterfaceTypeKCS;  //TO BE DONE <Need to check with DMTF for other Interface types>
#else    
    Type42Record->SmbiosType42.InterfaceType = MCHostInterfaceTypeOEM;
#endif
    Type42Record->SmbiosType42.MCHostInterfaceData[0] = TYPE42_MC_HOST_INTERFACE_DATA;
    Type42Record->NumberOfProtocols = 0x01;
    Type42Record->Protocol[0].ProtocolType = MCHostInterfaceProtocolIPMI;
    Type42Record->Protocol[0].ProtocolSpecificData = TYPE42_MC_HOST_PROTOCOL_DATA;
    
    
#if (!IPMI_DEFAULT_HOOK_SUPPORT)
    //
    // Update Smbios Type 42 record with Oem Hooks if any.
    //
    Status = IpmiOemSmbiosTypesUpdateHook ((VOID **) &Type42Record);
    if (EFI_ERROR (Status)) {
        SERVER_IPMI_DEBUG ((EFI_D_ERROR, "IpmiOemSmbiosTypesUpdateHook TYPE 42 status is %r and initializing unspecified values.\n", Status));
    }
#endif
    
    //
    // Now we have got the full SMBIOS 42 record, call SMBIOS protocol to add this record.
    //
    SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
    Status = SmbiosProtocol->Add (
                    SmbiosProtocol,
                    NULL,
                    &SmbiosHandle,
                    (EFI_SMBIOS_TABLE_HEADER*) Type42Record);
        
    DEBUG ((EFI_D_INFO, "SmbiosProtocol->Add Type42Record....  Status: %r \n", Status));
    FreePool (Type42Record);

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "%a Exiting...  \n", __FUNCTION__));
    return;
}

/**

    Register notification callback on SMBIOS protocol to install SmBios type 38
    structure.

    @param VOID

    @return EFI_STATUS

**/

EFI_STATUS
IpmiSmbiosTypesSupport (
  VOID )
{
    VOID        *SmbiosProtocolRegistration;

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "%a Entered...   \n", __FUNCTION__));

    //
    // Create Notification event for SmbiosProtocol GUID
    //
    EfiCreateProtocolNotifyEvent (
        &gEfiSmbiosProtocolGuid,
        TPL_CALLBACK,
        InstallSmbiosTypesStructure,
        NULL,
        &SmbiosProtocolRegistration
        );

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "%a Exiting...\n", __FUNCTION__));

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
