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

/** @file FruSetupScreen.c
    Initializes FRU information in server management setup screen

**/
//----------------------------------------------------------------------

#include "Token.h"
#include <ServerMgmtSetupVariable.h>
#include <ServerMgmtSetupStrDefs.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>

//----------------------------------------------------------------------

//
// Macro definitions
//
typedef UINT16  STRING_REF;
#define STRING_TOKEN(t)    t

//
// Function Prototype
//
VOID InitString(EFI_HII_HANDLE HiiHandle, STRING_REF StrRef, CHAR16 *sFormat, ...);

/**
    This function initializes the FRU information related setup option values

    @param HiiHandle Handle to HII database
    @param Class Indicates the setup class

    @retval VOID

**/

VOID
InitFruStrings(
  IN EFI_HII_HANDLE     HiiHandle,
  IN UINT16             Class )
{

    CHAR16          *PcdDefaultString = L"To be filled by O.E.M.To be filled by O.E.M.To be filled by O.E.M.";
    CHAR16          *SdrDefaultString = L"Unknown";
    EFI_GUID        DefaultSystemUuid = {0x00};
    CHAR16          *UpdateStr;
    EFI_GUID        *Uuid;

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "%a: Class ID:  %x\n", __FUNCTION__, Class));

    //
    // Continue only for Server Mgmt setup page
    //
    if ( Class!= SERVER_MGMT_CLASS_ID ) {
        return;
    }

    //
    // SystemManufacturer
    //
    UpdateStr = (CHAR16 *) PcdGetPtr (PcdSystemManufacturer);
    if ( StrCmp (UpdateStr,PcdDefaultString) == 0x00 ) {
        UpdateStr = (CHAR16 *) PcdGetPtr (PcdStrMfg);
    }
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "SystemManufacturer: UpdateStr: %S\n", UpdateStr));
    InitString (
        HiiHandle,
        STRING_TOKEN(STR_SYS_MANUFACTURER_VAL),
        L"%s",
        UpdateStr );

    //
    // SystemProductName
    //
    UpdateStr = (CHAR16 *) PcdGetPtr (PcdSystemProductName);
    if ( StrCmp (UpdateStr, PcdDefaultString) == 0x00 ) {
        UpdateStr = (CHAR16 *) PcdGetPtr (PcdStrProduct);
    }
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "SystemProductName: UpdateStr: %S\n", UpdateStr));
    InitString (
        HiiHandle,
        STRING_TOKEN(STR_SYS_PROD_NAME_VAL),
        L"%s",
        UpdateStr );

    //
    // SystemVersion
    //
    UpdateStr = (CHAR16 *) PcdGetPtr (PcdSystemVersion);
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "SystemVersion: UpdateStr: %S\n", UpdateStr));
    if ( StrCmp (UpdateStr, PcdDefaultString) != 0x00 ) {
        InitString (
            HiiHandle,
            STRING_TOKEN(STR_SYS_VERSION_VAL),
            L"%s",
            UpdateStr );
    }

    //
    // SystemSerialNumber
    //
    UpdateStr = (CHAR16 *) PcdGetPtr (PcdSystemSerialNumber);
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "SystemSerialNumber: UpdateStr: %S\n", UpdateStr));
    if ( StrCmp (UpdateStr, PcdDefaultString) != 0x00 ) {
        InitString (
            HiiHandle,
            STRING_TOKEN(STR_SYS_SERIAL_NUM_VAL),
            L"%s",
            UpdateStr );
    }

    //
    // BaseBoardManufacturer
    //
    UpdateStr = (CHAR16 *) PcdGetPtr (PcdBaseBoardManufacturer);
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "BaseBoardManufacturer: UpdateStr: %S\n", UpdateStr));
    if ( StrCmp (UpdateStr, PcdDefaultString) != 0x00 ) {
        InitString (
            HiiHandle,
            STRING_TOKEN(STR_BRD_MANUFACTURER_VAL),
            L"%s",
            UpdateStr );
    }

    //
    // BaseBoardProductName
    //
    UpdateStr = (CHAR16 *) PcdGetPtr (PcdBaseBoardProductName);
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "BaseBoardProductName: UpdateStr: %S\n", UpdateStr));
    if ( StrCmp (UpdateStr, PcdDefaultString) != 0x00 ) {
        InitString (
            HiiHandle,
            STRING_TOKEN(STR_BRD_PROD_NAME_VAL),
            L"%s",
            UpdateStr );
    }

    //
    // BaseBoardVersion
    //
    UpdateStr = (CHAR16 *) PcdGetPtr (PcdBaseBoardVersion);
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "BaseBoardVersion: UpdateStr: %S\n", UpdateStr));
    if ( StrCmp (UpdateStr, PcdDefaultString) != 0x00 ) {
        InitString (
            HiiHandle,
            STRING_TOKEN(STR_BRD_VERSION_VAL),
            L"%s",
            UpdateStr );
    }

    //
    // BaseBoardSerialNumber
    //
    UpdateStr = (CHAR16 *) PcdGetPtr (PcdBaseBoardSerialNumber);
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "BaseBoardSerialNumber: UpdateStr: %S\n", UpdateStr));
    if ( StrCmp (UpdateStr, PcdDefaultString) != 0x00 ) {
        InitString (
            HiiHandle,
            STRING_TOKEN(STR_BRD_SERIAL_NUM_VAL),
            L"%s",
            UpdateStr );
    }

    //
    // ChassisManufacturer
    //
    UpdateStr = (CHAR16 *) PcdGetPtr (PcdChassisManufacturer);
    if ( StrCmp (UpdateStr, PcdDefaultString) == 0x00 ) {
        UpdateStr = (CHAR16 *) PcdGetPtr (PcdStrMfg);
    }
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "ChassisManufacturer: UpdateStr: %S\n", UpdateStr));
    InitString (
        HiiHandle,
        STRING_TOKEN(STR_CHA_MANUFACTURER_VAL),
        L"%s",
        UpdateStr );

    //
    // ChassisVersion
    //
    UpdateStr = (CHAR16 *) PcdGetPtr (PcdChassisVersion);
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "ChassisVersion: UpdateStr: %S\n", UpdateStr));
    if ( StrCmp (UpdateStr, PcdDefaultString) != 0x00 ) {
        InitString (
            HiiHandle,
            STRING_TOKEN(STR_CHA_VERSION_VAL),
            L"%s",
            UpdateStr );
    }

    //
    // ChassisSerialNumber
    //
    UpdateStr = (CHAR16 *) PcdGetPtr (PcdChassisSerialNumber);
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "ChassisSerialNumber: UpdateStr: %S\n", UpdateStr));
    if ( StrCmp (UpdateStr, PcdDefaultString) != 0x00 ) {
        InitString (
            HiiHandle,
            STRING_TOKEN(STR_CHA_SERIAL_NUM_VAL),
            L"%s",
            UpdateStr );
    }

    //
    // SdrVersionNo
    //
    UpdateStr = (CHAR16 *) PcdGetPtr (PcdSdrVersionNo);
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "SdrVersionNo: UpdateStr: %S\n", UpdateStr));
    if ( StrCmp (UpdateStr, SdrDefaultString) != 0x00 ) {
        InitString (
            HiiHandle,
            STRING_TOKEN(STR_SDR_VERSION_VAL),
            L"%s",
            UpdateStr );
    }

    //
    // System Unique ID
    //
    Uuid = (EFI_GUID *) PcdGetPtr (PcdSystemUuid);
    if (CompareGuid ((CONST EFI_GUID*)Uuid, (CONST EFI_GUID*)&DefaultSystemUuid) == FALSE) {
        SERVER_IPMI_DEBUG ((EFI_D_INFO, "UUID:%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x\n", Uuid->Data1, Uuid->Data2, Uuid->Data3, Uuid->Data4[0], Uuid->Data4[1], Uuid->Data4[2], Uuid->Data4[3], Uuid->Data4[4], Uuid->Data4[5], Uuid->Data4[6], Uuid->Data4[7]));
        InitString (
            HiiHandle,
            STRING_TOKEN(STR_SYSTEM_UUID_VAL),
            L"%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
            Uuid->Data1, Uuid->Data2, Uuid->Data3, Uuid->Data4[0], Uuid->Data4[1], Uuid->Data4[2], Uuid->Data4[3], Uuid->Data4[4], Uuid->Data4[5], Uuid->Data4[6], Uuid->Data4[7]);
    }

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "%a Exiting...\n", __FUNCTION__));
    return ;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
