//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//  Rev. 1.02
//    Bug Fix:  Support a feature that Update SMBIOS Type 1 and 3 with Fru0.
//    Reason:   
//    Auditor:  Isaac Hsu
//    Date:     May/12/2017
//
//  Rev. 1.01
//    Bug Fix:  Fix FRU1 data can't update to smbios type 2.
//    Reason:
//    Auditor:  Kasber Chen
//    Date:     Oct/22/2016
//
//  Rev. 1.00
//    Bug Fix:  Initialize revision
//    Reason:   Get base code from Grantley
//    Auditor:  Kasber Chen
//    Date:     Jun/07/2016
//
//****************************************************************************
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

/** @file FruSmbiosTable.c
    Update SMBIOS type 1,2,3 structure

**/

//----------------------------------------------------------------------

#include "IpmiRedirFru.h"
#include <Protocol/Smbios.h>
#include <IndustryStandard/SmBios.h>

//----------------------------------------------------------------------

//
// Macro definitions
//
#define MAX_SMBIOS_STRING_SIZE  0x40

//
// Macro represent size of SmBios structure end.
// Every SmBios type ends with 0x0000.
//
#define SIZE_OF_SMBIOS_STRUCTURE_END_MACRO sizeof(UINT16)

#if SMCPKG_SUPPORT
#include "AmiLib.h"
extern EFI_RUNTIME_SERVICES *gRT;
extern EFI_GUID gAmiSmbiosNvramGuid;
#endif

/**
    Convert the Unicode string into Ascii string.

    @param UnicodeStr Unicode string pointer.
    @param AsciiStr Ascii string pointer

    @retval CHAR8* Ascii string pointer.

**/

UINT8
Unicode2AsciiConversion (
  OUT CHAR8    *AsciiStr,
  IN  CHAR16   *UnicodeStr )
{
    UINT8   Length = 0;

    while (TRUE) {
        *AsciiStr = (CHAR8) *(UnicodeStr++);
        Length++;
        if (*(AsciiStr++) == '\0') {
            return Length;
        }
    }
}

/**
    Convert the Valid  Unicode string into Ascii string. If strinig is invalid
    truncate to maximum size and converted.

    @param AsciiStr Ascii string pointer
    @param UnicodeStr Unicode string pointer.

    @retval CHAR8* AsciiStr - Ascii string pointer.

**/

VOID
GetValidSMBIOSString(
  OUT  CHAR8     *AsciiStr,
  IN   CHAR16    *UnicodeStr)
{
    CHAR16  TempUnicodeString[MAX_SMBIOS_STRING_SIZE];
    UINTN   Length;

    Length = StrSize (UnicodeStr);

    ZeroMem (&TempUnicodeString[0], Length);

    //
    // Validating string size
    //
    if ( Length >= MAX_SMBIOS_STRING_SIZE ) {

        //
        // Truncate size to maximum allowable
        //
        Length = MAX_SMBIOS_STRING_SIZE - 1;
    }
    StrnCpy (TempUnicodeString, UnicodeStr, Length);
    Unicode2AsciiConversion (AsciiStr, TempUnicodeString);
}

#if SMCPKG_SUPPORT
VOID
GetString(
     IN EFI_SMBIOS_TABLE_HEADER         *Record,
	 IN UINT8 Offset,
	 IN OUT CHAR16  *Buffer
	 )
{
	EFI_STATUS	Status = EFI_SUCCESS;
    UINT16	size = 0;
    UINT8	*stroffset = NULL;
	UINT8	stringCount;
	UINT8	i = 0;

    stringCount = *(((UINT8*)Record) + Offset);

	if(stringCount==0) return;

    stroffset = ((UINT8*)Record) + Record->Length;

	while((--stringCount)!=0)
	{
		while(*stroffset!=0) stroffset++;
		stroffset++;
	}

	while(*stroffset) 
	{
		Buffer[i++] = *stroffset;
		stroffset++;
	}
	Buffer[i] = L'\0'; 
}
#endif

/**
    Notification function for SMBIOS protocol.
    Update SMBIOS type 1,2,3 structure.

    @param Event Event which caused this handler
    @param Context Context passed during Event Handler registration

    @retval VOID

**/

VOID
EFIAPI
FruSmbiosTableUpdate (
  IN  EFI_EVENT                     Event,
  IN  VOID                          *Context )
{
    EFI_STATUS                      Status;
    EFI_STATUS                      StatusOfRequestedSmbiosType;
    EFI_SMBIOS_HANDLE               SmbiosHandle;
    EFI_SMBIOS_PROTOCOL             *SmbiosProtocol;
    EFI_SMBIOS_TYPE                 Type;
    EFI_SMBIOS_TABLE_HEADER         *Record;
#if SMC_UPDATE_DMI_WITH_FRU0_SUPPORT
    SMBIOS_TABLE_TYPE1              *Type1Structure;
#endif
    SMBIOS_TABLE_TYPE2              *Type2Structure;
#if SMC_UPDATE_DMI_WITH_FRU0_SUPPORT
    SMBIOS_TABLE_TYPE3              *Type3Structure;
#endif
    UINTN                           StringNumber;
    CHAR16                          *UnicodeString;
    CHAR8                           *SmBiosDefaultString = "To be filled by O.E.M.";
    CHAR16                          *PcdDefaultString = L"To be filled by O.E.M.To be filled by O.E.M.To be filled by O.E.M.";
    CHAR8                           AsciiString[MAX_SMBIOS_STRING_SIZE];
#if SMC_UPDATE_DMI_WITH_FRU0_SUPPORT
    EFI_GUID                        *Uuid = NULL;
    EFI_GUID                        DefaultUuid = {0};
#endif
#if SMCPKG_SUPPORT
    CHAR8	*SmBiosBoardDefaultString = "0123456789";
    CHAR16	S1[] = L"                ";
    CHAR8	Buffer[0x200];
    UINTN	BufferSize;
    CHAR16	TempStrBuffer[] = L"                                                              ";
    CHAR16	BOARD_NAME[] = L"                                                              ";
    CHAR16	MANUFACTURE_NAME[] = L"                                                                  ";
    CHAR16	BOARD_VERSION[]=L"           ";
    CHAR16	BOARD_SN[]=L"                ";
    CHAR16	SMC_Board_Manufacture_default[] = L"Supermicro";
    CHAR16	SMC_Board_Product_default[] = CONVERT_TO_WSTRING(PROJECT_BOARD_NAME);
    CHAR16	SMC_Board_version_default[] = L"0123456789";
    CHAR16	SMC_Board_sn_default[] = L"0123456789";
#if SMC_UPDATE_DMI_WITH_FRU0_SUPPORT
    // Type 1
    CHAR16  SystemManufacturer[] = L"                                                                  ";
    CHAR16  SystemProduct[] = L"                                                                  ";
    CHAR16  SystemVersion[] = L"                                                                  ";
    CHAR16  SystemSN[] = L"                                                                  ";
    CHAR16  SMC_System_Manufacture_default[] = L"Supermicro";
    CHAR16  SMC_System_Product_default[] = L"Super Server";
    CHAR16  SMC_System_Version_default[] = L"0123456789";
    CHAR16  SMC_System_SN_default[] = L"0123456789";

    // Type 3
    UINT8   ChassisType, PcdDefaultChassisType = 2;
    CHAR16  ChassisVersion[] = L"                                                                  ";
    CHAR16  ChassisSN[] = L"                                                                  ";
    CHAR16  ChassisAssetTag[] = L"                                                                  ";
    UINT8   SMC_Chassis_Type_default = 0x11;
    CHAR16  SMC_Chassis_Version_default[] = L"0123456789";
    CHAR16  SMC_Chassis_SN_default[] = L"0123456789";
    CHAR16  SMC_Chassis_AssetTag_default[] = L"To be filled by O.E.M.";
#endif
#endif

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "%a Entry...\n", __FUNCTION__));
    Status = gBS->LocateProtocol (
                &gEfiSmbiosProtocolGuid,
                NULL,
                (VOID **) &SmbiosProtocol);
    DEBUG ((EFI_D_INFO, "Locate SmbiosProtocol Status:%r \n", Status));
    if (EFI_ERROR (Status)) {
        return;
    }
    Status = gBS->CloseEvent (Event);
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "CloseEvent Status:%r \n", Status));
#if SMC_UPDATE_DMI_WITH_FRU0_SUPPORT
    //
    // Smbios Type 1 structure process start
    //
    SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
    Type = 0x01;
    StatusOfRequestedSmbiosType = SmbiosProtocol->GetNext (
                                      SmbiosProtocol,
                                      &SmbiosHandle,
                                      &Type,
                                      &Record,
                                      NULL);
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "SmbiosProtocol->GetNext Status:%r \n", StatusOfRequestedSmbiosType));
    if (EFI_ERROR(StatusOfRequestedSmbiosType)) {
        //
        // Type 1 structure is not available, needs to be created.
        //
        Type1Structure = AllocateZeroPool (sizeof (SMBIOS_TABLE_TYPE1) + SIZE_OF_SMBIOS_STRUCTURE_END_MACRO);
        if (Type1Structure == NULL) {
            return;
        }
        Type1Structure->Hdr.Type = EFI_SMBIOS_TYPE_SYSTEM_INFORMATION;
        Type1Structure->Hdr.Length = sizeof (SMBIOS_TABLE_TYPE1);
        Type1Structure->Hdr.Handle = 0xFFFF;//To be updated by SMBIOS driver.
    }else {
        Type1Structure = (SMBIOS_TABLE_TYPE1 *)Record;
    }
    Type1Structure->Manufacturer = 0x01;
    Type1Structure->ProductName = 0x02;
    Type1Structure->Version = 0x03;
    Type1Structure->SerialNumber = 0x04;

    //
    // Copy the Uuid
    //
    Uuid = (EFI_GUID *)PcdGetPtr (PcdSystemUuid);
    if(CompareGuid ((CONST EFI_GUID*)Uuid, (CONST EFI_GUID*)&DefaultUuid)== FALSE) {
        CopyMem (&(Type1Structure->Uuid), Uuid, sizeof(EFI_GUID));
    }
    //
    // Creating the Smbios Type 1 Structure.
    //
    if (EFI_ERROR (StatusOfRequestedSmbiosType)) {
        SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
        Status = SmbiosProtocol->Add (
                     SmbiosProtocol,
                     NULL,
                     &SmbiosHandle,
                     (EFI_SMBIOS_TABLE_HEADER*) Type1Structure);
        SERVER_IPMI_DEBUG ((EFI_D_INFO, "SmbiosProtocol->Add Status:%r \n", Status));
        if (EFI_ERROR (Status)) {
            return;
        }
    }

#if SMC_UPDATE_DMI_WITH_FRU0_SUPPORT
    GetString(Record, 4, SystemManufacturer); //default SystemManufacturer
    GetString(Record, 5, SystemProduct);      //default SystemProduct
    GetString(Record, 6, SystemVersion);      //default SystemVersion
    GetString(Record, 7, SystemSN);           //default SystemSN

    // Check SystemManufacturer changed by tool or BCP
    Swprintf(S1, L"DmiVar%02x%04x%02x%02x", 0x01, SmbiosHandle, 0x04, 0);
    BufferSize = sizeof(Buffer);
    Status = gRT->GetVariable(S1, &gAmiSmbiosNvramGuid, NULL, &BufferSize, &Buffer);
    if(!EFI_ERROR(Status)) {    //changed by tool
        DEBUG((-1, "Type 1 System Manufacture changed by tool.\n"));
        AsciiStrToUnicodeStr(Buffer, TempStrBuffer);
        BufferSize = StrSize(TempStrBuffer);
        PcdSetPtr(PcdSystemManufacturer, &BufferSize, (VOID*)TempStrBuffer);
    }
    else if(StrCmp(SMC_System_Manufacture_default, SystemManufacturer)) {
        DEBUG((-1, "Type 1 System Manufacture changed by BCP.\n"));
        BufferSize = StrSize(SystemManufacturer);
        PcdSetPtr(PcdSystemManufacturer, &BufferSize, (VOID*)SystemManufacturer);
    }

    // Check SystemProduct changed by tool or BCP
    Swprintf(S1, L"DmiVar%02x%04x%02x%02x", 0x01, SmbiosHandle, 0x05, 0);
    BufferSize = sizeof(Buffer);
    Status = gRT->GetVariable(S1, &gAmiSmbiosNvramGuid, NULL, &BufferSize, &Buffer);
    if(!EFI_ERROR(Status)) {    //changed by tool
        DEBUG((-1, "Type 1 System Product changed by tool.\n"));
        AsciiStrToUnicodeStr(Buffer, TempStrBuffer);
        BufferSize = StrSize(TempStrBuffer);
        PcdSetPtr(PcdSystemProductName, &BufferSize, (VOID*)TempStrBuffer);
    }
    else if(StrCmp(SMC_System_Product_default, SystemProduct)) {
        DEBUG((-1, "Type 1 System Product changed by BCP.\n"));
        BufferSize = StrSize(SystemProduct);
        PcdSetPtr(PcdSystemProductName, &BufferSize, (VOID*)SystemProduct);
    }

    // Check SystemVersion changed by tool or BCP
    Swprintf(S1, L"DmiVar%02x%04x%02x%02x", 0x01, SmbiosHandle, 0x06, 0);
    BufferSize = sizeof(Buffer);
    Status = gRT->GetVariable(S1, &gAmiSmbiosNvramGuid, NULL, &BufferSize, &Buffer);
    if(!EFI_ERROR(Status)) {    //changed by tool
        DEBUG((-1, "Type 1 System Version changed by tool.\n"));
        AsciiStrToUnicodeStr(Buffer, TempStrBuffer);
        BufferSize = StrSize(TempStrBuffer);
        PcdSetPtr(PcdSystemVersion, &BufferSize, (VOID*)TempStrBuffer);
    }
    else if(StrCmp(SMC_System_Version_default, SystemVersion)) {
        DEBUG((-1, "Type 1 System Version changed by BCP.\n"));
        BufferSize = StrSize(SystemVersion);
        PcdSetPtr(PcdSystemVersion, &BufferSize, (VOID*)SystemVersion);
    }

    // Check SystemSN changed by tool or BCP
    Swprintf(S1, L"DmiVar%02x%04x%02x%02x", 0x01, SmbiosHandle, 0x07, 0);
    BufferSize = sizeof(Buffer);
    Status = gRT->GetVariable(S1, &gAmiSmbiosNvramGuid, NULL, &BufferSize, &Buffer);
    if(!EFI_ERROR(Status)) {    //changed by tool
        DEBUG((-1, "Type 1 System Serial number changed by tool.\n"));
        AsciiStrToUnicodeStr(Buffer, TempStrBuffer);
        BufferSize = StrSize(TempStrBuffer);
        PcdSetPtr(PcdSystemSerialNumber, &BufferSize, (VOID*)TempStrBuffer);
    }
    else if(StrCmp(SMC_System_SN_default, SystemSN)) {
        DEBUG((-1, "Type 1 System Serial number changed by BCP.\n"));
        BufferSize = StrSize(SystemSN);
        PcdSetPtr(PcdSystemSerialNumber, &BufferSize, (VOID*)SystemSN);
    }
#endif //SMC_UPDATE_DMI_WITH_FRU0_SUPPORT

    //
    // Type 1 Manufacturer String
    //
    SmbiosHandle = Type1Structure->Hdr.Handle;
    StringNumber = 0x01;
    UnicodeString = (CHAR16 *) PcdGetPtr (PcdSystemManufacturer);
    if ( StrCmp (UnicodeString, PcdDefaultString) == 0x00 ) {
#if SMC_UPDATE_DMI_WITH_FRU0_SUPPORT
        StrCpy(UnicodeString, SMC_System_Manufacture_default);
#else
        UnicodeString = (CHAR16 *) PcdGetPtr (PcdStrMfg);
#endif
    }
    GetValidSMBIOSString (AsciiString, UnicodeString);
    Status = SmbiosProtocol->UpdateString (
                 SmbiosProtocol,
                 &SmbiosHandle,
                 &StringNumber,
                 AsciiString);
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "SmbiosProtocol->UpdateString Status:%r \n", Status));
    //
    // Type 1 Product Name String
    //
    SmbiosHandle = Type1Structure->Hdr.Handle;
    StringNumber = 0x02;
    UnicodeString = (CHAR16 *) PcdGetPtr (PcdSystemProductName);
    if ( StrCmp (UnicodeString, PcdDefaultString) == 0x00 ) {
#if SMC_UPDATE_DMI_WITH_FRU0_SUPPORT
        StrCpy(UnicodeString, SMC_System_Product_default);
#else
        UnicodeString = (CHAR16 *) PcdGetPtr (PcdStrProduct);
#endif
    }
    GetValidSMBIOSString (AsciiString, UnicodeString);
    Status = SmbiosProtocol->UpdateString (
                 SmbiosProtocol,
                 &SmbiosHandle,
                 &StringNumber,
                 AsciiString);
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "SmbiosProtocol->UpdateString Status:%r \n", Status));
    //
    // Type 1 Version String
    //
    SmbiosHandle = Type1Structure->Hdr.Handle;
    StringNumber = 0x03;
    UnicodeString = (CHAR16 *) PcdGetPtr (PcdSystemVersion);
    if ( StrCmp (UnicodeString, PcdDefaultString) == 0x00 ) {
#if SMC_UPDATE_DMI_WITH_FRU0_SUPPORT
        StrCpy(UnicodeString, SMC_System_Version_default);
        GetValidSMBIOSString (AsciiString, UnicodeString);
#else
        AsciiStrCpy (AsciiString, SmBiosDefaultString);
#endif
    } else {
        GetValidSMBIOSString (AsciiString, UnicodeString);
    }
    Status = SmbiosProtocol->UpdateString (
                 SmbiosProtocol,
                 &SmbiosHandle,
                 &StringNumber,
                 AsciiString);
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "SmbiosProtocol->UpdateString Status:%r \n", Status));
    //
    // Type 1 Serial Number String
    //
    SmbiosHandle = Type1Structure->Hdr.Handle;
    StringNumber = 0x04;
    UnicodeString = (CHAR16 *) PcdGetPtr (PcdSystemSerialNumber);
    if ( StrCmp (UnicodeString, PcdDefaultString) == 0x00 ) {
#if SMC_UPDATE_DMI_WITH_FRU0_SUPPORT
        StrCpy(UnicodeString, SMC_System_SN_default);
        GetValidSMBIOSString (AsciiString, UnicodeString);
#else
        AsciiStrCpy (AsciiString, SmBiosDefaultString);
#endif
    } else {
        GetValidSMBIOSString (AsciiString, UnicodeString);
    }
    Status = SmbiosProtocol->UpdateString (
                SmbiosProtocol,
                &SmbiosHandle,
                &StringNumber,
                AsciiString);
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "SmbiosProtocol->UpdateString Status:%r \n", Status));
    if (EFI_ERROR (StatusOfRequestedSmbiosType)) {
        FreePool(Type1Structure);
    }
#endif //SMC_UPDATE_DMI_WITH_FRU0_SUPPORT
    //
    // Smbios Type 2 structure process start
    //
    SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
    Type = 0x02;
    StatusOfRequestedSmbiosType = SmbiosProtocol->GetNext (
                                    SmbiosProtocol,
                                    &SmbiosHandle,
                                    &Type,
                                    &Record,
                                    NULL);
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "SmbiosProtocol->GetNext Status:%r \n", StatusOfRequestedSmbiosType));
    if (EFI_ERROR (StatusOfRequestedSmbiosType)) {
        //
        // Type 2 structure is not available, needs to be created.
        //
        Type2Structure = AllocateZeroPool (sizeof(SMBIOS_TABLE_TYPE2) + SIZE_OF_SMBIOS_STRUCTURE_END_MACRO);
        if (Type2Structure == NULL) {
            return;
        }
        Type2Structure->Hdr.Type = EFI_SMBIOS_TYPE_BASEBOARD_INFORMATION;
        Type2Structure->Hdr.Length = sizeof (SMBIOS_TABLE_TYPE2);
        Type2Structure->Hdr.Handle = 0xFFFF;//To be updated by SMBIOS driver.
    } else {
        Type2Structure = (SMBIOS_TABLE_TYPE2 *)Record;
    }
    Type2Structure->Manufacturer = 0x01;
    Type2Structure->ProductName = 0x02;
    Type2Structure->Version = 0x03;
    Type2Structure->SerialNumber = 0x04;
#if SMCPKG_SUPPORT == 0
    Type2Structure->AssetTag = 0x05;
#endif
    //
    // Creating the Smbios Type 2 Structure.
    //
    if (EFI_ERROR (StatusOfRequestedSmbiosType)) {
        SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
        Status = SmbiosProtocol->Add (
                    SmbiosProtocol,
                    NULL,
                    &SmbiosHandle,
                    (EFI_SMBIOS_TABLE_HEADER*) Type2Structure);
        SERVER_IPMI_DEBUG ((EFI_D_INFO, "SmbiosProtocol->Add Status:%r \n", Status));
        if (EFI_ERROR (Status)) {
            return;
        }
    }
#if SMCPKG_SUPPORT
    GetString(Record, 4, MANUFACTURE_NAME);	//default MANUFACTURE_NAME
    GetString(Record, 5, BOARD_NAME);		//default BOARD_NAME
    GetString(Record, 6, BOARD_VERSION);	//default BOARD_VERSION
    GetString(Record, 7, BOARD_SN);		//default BOARD_SN
// check MANUFACTURE_NAME changed by tool or BCP
    Swprintf(S1, L"DmiVar%02x%04x%02x%02x", 0x02, SmbiosHandle, 0x04, 0);
    BufferSize = sizeof(Buffer);
    Status = gRT->GetVariable(S1, &gAmiSmbiosNvramGuid, NULL, &BufferSize, &Buffer);
    if(!Status){	//changed by tool
	DEBUG((-1, "Type 2 board Manufacture changed by tool.\n"));
	AsciiStrToUnicodeStr(Buffer, TempStrBuffer);
	BufferSize = StrSize(TempStrBuffer);
	PcdSetPtr(PcdBaseBoardManufacturer, &BufferSize, (VOID*)TempStrBuffer);
    }
    else if(StrCmp(SMC_Board_Manufacture_default, MANUFACTURE_NAME)){
	DEBUG((-1, "Type 2 board Manufacture changed by BCP.\n"));
	BufferSize = StrSize(MANUFACTURE_NAME);
	PcdSetPtr(PcdBaseBoardManufacturer, &BufferSize, (VOID*)MANUFACTURE_NAME);
    }
// check BOARD_NAME changed by tool or BCP
    Swprintf(S1, L"DmiVar%02x%04x%02x%02x", 0x02, SmbiosHandle, 0x05, 0);
    BufferSize = sizeof(Buffer);
    Status = gRT->GetVariable(S1, &gAmiSmbiosNvramGuid, NULL, &BufferSize, &Buffer);
    if(!Status){	//changed by tool
	DEBUG((-1, "Type 2 board name changed by tool.\n"));
    	AsciiStrToUnicodeStr(Buffer, TempStrBuffer);
    	BufferSize = StrSize(TempStrBuffer);
    	PcdSetPtr(PcdBaseBoardProductName, &BufferSize, (VOID*)TempStrBuffer);
    }
    else if(StrCmp(SMC_Board_Product_default, BOARD_NAME)){
	DEBUG((-1, "Type 2 board name changed by BCP.\n"));
    	BufferSize = StrSize(BOARD_NAME);
    	PcdSetPtr(PcdBaseBoardProductName, &BufferSize, (VOID*)BOARD_NAME);
    }
// check BOARD_VERSION changed by tool or BCP
    Swprintf(S1, L"DmiVar%02x%04x%02x%02x", 0x02, SmbiosHandle, 0x06, 0);
    BufferSize = sizeof(Buffer);
    Status = gRT->GetVariable( S1, &gAmiSmbiosNvramGuid, NULL, &BufferSize, &Buffer);
    if(!Status){	//changed by tool
	DEBUG((-1, "Type 2 board version changed by tool.\n"));
	AsciiStrToUnicodeStr(Buffer, TempStrBuffer);
	BufferSize = StrSize(TempStrBuffer);
	PcdSetPtr(PcdBaseBoardVersion, &BufferSize, (VOID*)TempStrBuffer);
    }
    else if(StrCmp(SMC_Board_version_default, BOARD_VERSION)){
	DEBUG((-1, "Type 2 board version changed by BCP.\n"));
	BufferSize = StrSize(BOARD_VERSION);
	PcdSetPtr(PcdBaseBoardVersion, &BufferSize, (VOID*)BOARD_VERSION);
    }
// check BOARD_SN changed by tool or BCP
    Swprintf(S1, L"DmiVar%02x%04x%02x%02x", 0x02, SmbiosHandle, 0x07, 0);
    BufferSize = sizeof(Buffer);
    Status = gRT->GetVariable(S1, &gAmiSmbiosNvramGuid, NULL, &BufferSize, &Buffer);
    if(!Status){	//changed by tool
	DEBUG((-1, "Type 2 board SerialNumber changed by tool.\n"));
    	AsciiStrToUnicodeStr(Buffer, TempStrBuffer);
    	BufferSize = StrSize(TempStrBuffer);
    	PcdSetPtr(PcdBaseBoardSerialNumber, &BufferSize, (VOID*)TempStrBuffer);
    }
    else if(StrCmp(SMC_Board_sn_default, BOARD_SN)){	//changed by BCP
	DEBUG((-1, "Type 2 board SerialNumber changed by BCP.\n"));
    	BufferSize = StrSize(BOARD_SN);
    	PcdSetPtr(PcdBaseBoardSerialNumber, &BufferSize, (VOID*)BOARD_SN);
    }
#endif
    //
    // Type 2 Manufacturer String
    //
    SmbiosHandle = Type2Structure->Hdr.Handle;
    StringNumber = 0x01;
    UnicodeString = (CHAR16 *) PcdGetPtr (PcdBaseBoardManufacturer);
    if ( StrCmp (UnicodeString, PcdDefaultString) == 0x00 ) {
        AsciiStrCpy (AsciiString, SmBiosDefaultString);
    } else {
        GetValidSMBIOSString (AsciiString, UnicodeString);
    }
    Status = SmbiosProtocol->UpdateString (
                SmbiosProtocol,
                &SmbiosHandle,
                &StringNumber,
                AsciiString);
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "SmbiosProtocol->UpdateString Status:%r \n", Status));
    //
    // Type 2 Product Name String
    //
//SMCPKG_SUPPORT    SmbiosHandle = Type2Structure->Hdr.Handle;
    StringNumber = 0x02;
    UnicodeString = (CHAR16 *) PcdGetPtr (PcdBaseBoardProductName);
    if ( StrCmp (UnicodeString, PcdDefaultString) == 0x00 ) {
        AsciiStrCpy (AsciiString, SmBiosDefaultString);
    } else {
        GetValidSMBIOSString (AsciiString, UnicodeString);
    }
    Status = SmbiosProtocol->UpdateString (
                SmbiosProtocol,
                &SmbiosHandle,
                &StringNumber,
                AsciiString);
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "SmbiosProtocol->UpdateString Status:%r \n", Status));
    //
    // Type 2 Version String
    //
//SMCPKG_SUPPORT    SmbiosHandle = Type2Structure->Hdr.Handle;
    StringNumber = 0x03;
    UnicodeString = (CHAR16 *) PcdGetPtr (PcdBaseBoardVersion);
    if ( StrCmp (UnicodeString, PcdDefaultString) == 0x00 ) {
	AsciiStrCpy (AsciiString, SmBiosBoardDefaultString);	//SMCPKG_SUPPORT
//SMCPKG_SUPPORT        AsciiStrCpy (AsciiString, SmBiosDefaultString);
    } else {
        GetValidSMBIOSString (AsciiString, UnicodeString);
    }
    Status = SmbiosProtocol->UpdateString (
                SmbiosProtocol,
                &SmbiosHandle,
                &StringNumber,
                AsciiString);
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "SmbiosProtocol->UpdateString Status:%r \n", Status));
    //
    // Type 2 Serial Number String
    //
//SMCPKG_SUPPORT    SmbiosHandle = Type2Structure->Hdr.Handle;
    StringNumber = 0x04;
    UnicodeString = (CHAR16 *) PcdGetPtr (PcdBaseBoardSerialNumber);
    if ( StrCmp (UnicodeString, PcdDefaultString) == 0x00 ) {
	AsciiStrCpy (AsciiString, SmBiosBoardDefaultString);	//SMCPKG_SUPPORT
//SMCPKG_SUPPORT        AsciiStrCpy (AsciiString, SmBiosDefaultString);
    } else {
        GetValidSMBIOSString (AsciiString, UnicodeString);
    }
    Status = SmbiosProtocol->UpdateString (
                SmbiosProtocol,
                &SmbiosHandle,
                &StringNumber,
                AsciiString);
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "SmbiosProtocol->UpdateString Status:%r \n", Status));
#if SMCPKG_SUPPORT == 0
    //
    // Type 2 AssetTag String
    //
    SmbiosHandle = Type2Structure->Hdr.Handle;
    StringNumber = 0x05;
    UnicodeString = (CHAR16 *) PcdGetPtr (PcdBaseBoardAssetTag);
    if ( StrCmp (UnicodeString, PcdDefaultString) == 0x00 ) {
        AsciiStrCpy (AsciiString, SmBiosDefaultString);
    } else {
        GetValidSMBIOSString (AsciiString, UnicodeString);
    }
    Status = SmbiosProtocol->UpdateString (
                SmbiosProtocol,
                &SmbiosHandle,
                &StringNumber,
                AsciiString);
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "SmbiosProtocol->UpdateString Status:%r \n", Status));
#endif
    if (EFI_ERROR (StatusOfRequestedSmbiosType)) {
        FreePool (Type2Structure);
    }
#if SMC_UPDATE_DMI_WITH_FRU0_SUPPORT
    //
    // Smbios Type 3 structure process start
    //
    SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
    Type = 0x03;
    StatusOfRequestedSmbiosType = SmbiosProtocol->GetNext (
                                    SmbiosProtocol,
                                    &SmbiosHandle,
                                    &Type,
                                    &Record,
                                    NULL);
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "SmbiosProtocol->GetNext Status:%r \n", Status));
    if (EFI_ERROR (StatusOfRequestedSmbiosType)) {
        //
        // Type 3 structure is not available, needs to be created.
        Type3Structure = AllocateZeroPool (sizeof (SMBIOS_TABLE_TYPE3) + SIZE_OF_SMBIOS_STRUCTURE_END_MACRO);
        if (Type3Structure == NULL) {
            return;
        }
        Type3Structure->Hdr.Type = EFI_SMBIOS_TYPE_SYSTEM_ENCLOSURE;
        Type3Structure->Hdr.Length = sizeof (SMBIOS_TABLE_TYPE3);
        Type3Structure->Hdr.Handle = 0xFFFF;//To be updated by SMBIOS driver.
    } else {
        Type3Structure = (SMBIOS_TABLE_TYPE3 *)Record;
    }
    Type3Structure->Manufacturer = 0x01;
    Type3Structure->Version = 0x02;
    Type3Structure->SerialNumber = 0x03;
    Type3Structure->AssetTag = 0x04;

#if SMC_UPDATE_DMI_WITH_FRU0_SUPPORT
    ChassisType = Type3Structure->Type;

    // Check ChassisType changed by tool or BCP
    Swprintf(S1, L"DmiVar%02x%04x%02x%02x", 0x03, SmbiosHandle, 0x05, 0);
    BufferSize = sizeof(Buffer);
    Status = gRT->GetVariable(S1, &gAmiSmbiosNvramGuid, NULL, &BufferSize, &Buffer);
    if(!EFI_ERROR(Status)) {    //changed by tool
        DEBUG((-1, "Type 3 Chassis Type changed by tool.\n"));
        PcdSet8(PcdChassisType, Buffer[0]);
    }
    else if(SMC_Chassis_Type_default!=ChassisType) {
        DEBUG((-1, "Type 3 Chassis Type changed by BCP.\n"));
        PcdSet8(PcdChassisType, ChassisType);
    }

    if( PcdGet8(PcdChassisType) == PcdDefaultChassisType )
        Type3Structure->Type = SMC_Chassis_Type_default;
    else
#endif
        Type3Structure->Type = PcdGet8 (PcdChassisType);
    //
    // Creating the Smbios Type 3 Structure.
    //
    if (EFI_ERROR (StatusOfRequestedSmbiosType)) {
        SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
        Status = SmbiosProtocol->Add (
                    SmbiosProtocol,
                    NULL,
                    &SmbiosHandle,
                    (EFI_SMBIOS_TABLE_HEADER*) Type3Structure);
        SERVER_IPMI_DEBUG ((EFI_D_INFO, "SmbiosProtocol->Add Status:%r \n", Status));
        if (EFI_ERROR (Status)) {
            return;
        }
    }

#if SMC_UPDATE_DMI_WITH_FRU0_SUPPORT
    GetString(Record, 6, ChassisVersion);  //default ChassisVersion
    GetString(Record, 7, ChassisSN);       //default ChassisSN
    GetString(Record, 8, ChassisAssetTag); //default ChassisAssetTag

    // Check ChassisVersion changed by tool or BCP
    Swprintf(S1, L"DmiVar%02x%04x%02x%02x", 0x03, SmbiosHandle, 0x06, 0);
    BufferSize = sizeof(Buffer);
    Status = gRT->GetVariable(S1, &gAmiSmbiosNvramGuid, NULL, &BufferSize, &Buffer);
    if(!EFI_ERROR(Status)) {    //changed by tool
        DEBUG((-1, "Type 3 Chassis Version changed by tool.\n"));
        AsciiStrToUnicodeStr(Buffer, TempStrBuffer);
        BufferSize = StrSize(TempStrBuffer);
        PcdSetPtr(PcdChassisVersion, &BufferSize, (VOID*)TempStrBuffer);
    }
    else if(StrCmp(SMC_Chassis_Version_default, ChassisVersion)) {
        DEBUG((-1, "Type 3 Chassis Version changed by BCP.\n"));
        BufferSize = StrSize(ChassisVersion);
        PcdSetPtr(PcdChassisVersion, &BufferSize, (VOID*)ChassisVersion);
    }

    // Check ChassisSN changed by tool or BCP
    Swprintf(S1, L"DmiVar%02x%04x%02x%02x", 0x03, SmbiosHandle, 0x07, 0);
    BufferSize = sizeof(Buffer);
    Status = gRT->GetVariable(S1, &gAmiSmbiosNvramGuid, NULL, &BufferSize, &Buffer);
    if(!EFI_ERROR(Status)) {    //changed by tool
        DEBUG((-1, "Type 3 Chassis Serial number changed by tool.\n"));
        AsciiStrToUnicodeStr(Buffer, TempStrBuffer);
        BufferSize = StrSize(TempStrBuffer);
        PcdSetPtr(PcdChassisSerialNumber, &BufferSize, (VOID*)TempStrBuffer);
    }
    else if(StrCmp(SMC_Chassis_SN_default, ChassisSN)) {
        DEBUG((-1, "Type 3 Chassis Serial number changed by BCP.\n"));
        BufferSize = StrSize(ChassisSN);
        PcdSetPtr(PcdChassisSerialNumber, &BufferSize, (VOID*)ChassisSN);
    }

    // Check ChassisAssetTag changed by tool or BCP
    Swprintf(S1, L"DmiVar%02x%04x%02x%02x", 0x03, SmbiosHandle, 0x08, 0);
    BufferSize = sizeof(Buffer);
    Status = gRT->GetVariable(S1, &gAmiSmbiosNvramGuid, NULL, &BufferSize, &Buffer);
    if(!EFI_ERROR(Status)) {    //changed by tool
        DEBUG((-1, "Type 3 Chassis Asset Tag changed by tool.\n"));
        AsciiStrToUnicodeStr(Buffer, TempStrBuffer);
        BufferSize = StrSize(TempStrBuffer);
        PcdSetPtr(PcdChassisAssetTag, &BufferSize, (VOID*)TempStrBuffer);
    }
    else if(StrCmp(SMC_Chassis_AssetTag_default, ChassisAssetTag)) {
        DEBUG((-1, "Type 3 Chassis Asset Tag changed by BCP.\n"));
        BufferSize = StrSize(ChassisAssetTag);
        PcdSetPtr(PcdChassisAssetTag, &BufferSize, (VOID*)ChassisAssetTag);
    }
#endif //SMC_UPDATE_DMI_WITH_FRU0_SUPPORT

    //
    // Type 3 Manufacturer String
    //
    SmbiosHandle = Type3Structure->Hdr.Handle;
    StringNumber = 0x01;
    UnicodeString = (CHAR16 *) PcdGetPtr (PcdChassisManufacturer);
    if ( StrCmp (UnicodeString, PcdDefaultString) == 0x00 ) {
        UnicodeString = (CHAR16 *) PcdGetPtr (PcdStrMfg);
    }
    GetValidSMBIOSString (AsciiString, UnicodeString);
    Status = SmbiosProtocol->UpdateString (
                SmbiosProtocol,
                &SmbiosHandle,
                &StringNumber,
                AsciiString);
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "SmbiosProtocol->UpdateString Status:%r \n", Status));
    //
    // Type 3 Version String
    //
    SmbiosHandle = Type3Structure->Hdr.Handle;
    StringNumber = 0x02;
    UnicodeString = (CHAR16 *) PcdGetPtr (PcdChassisVersion);
    if ( StrCmp (UnicodeString, PcdDefaultString) == 0x00 ) {
#if SMC_UPDATE_DMI_WITH_FRU0_SUPPORT
        StrCpy(UnicodeString, SMC_Chassis_Version_default);
        GetValidSMBIOSString (AsciiString, UnicodeString);
#else
        AsciiStrCpy (AsciiString, SmBiosDefaultString);
#endif
    } else {
        GetValidSMBIOSString (AsciiString, UnicodeString);
    }
    Status = SmbiosProtocol->UpdateString (
                SmbiosProtocol,
                &SmbiosHandle,
                &StringNumber,
                AsciiString);
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "SmbiosProtocol->UpdateString Status:%r \n", Status));
    //
    // Type 3 SerialNumber String
    //
    SmbiosHandle = Type3Structure->Hdr.Handle;
    StringNumber = 0x03;
    UnicodeString = (CHAR16 *) PcdGetPtr (PcdChassisSerialNumber);
    if ( StrCmp (UnicodeString, PcdDefaultString) == 0x00 ) {
#if SMC_UPDATE_DMI_WITH_FRU0_SUPPORT
        StrCpy(UnicodeString, SMC_Chassis_SN_default);
        GetValidSMBIOSString (AsciiString, UnicodeString);
#else
        AsciiStrCpy (AsciiString, SmBiosDefaultString);
#endif
    } else {
        GetValidSMBIOSString (AsciiString, UnicodeString);
    }
    Status = SmbiosProtocol->UpdateString (
                SmbiosProtocol,
                &SmbiosHandle,
                &StringNumber,
                AsciiString);
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "SmbiosProtocol->UpdateString Status:%r \n", Status));
    //
    // Type 3 AssetTag String
    //
    SmbiosHandle = Type3Structure->Hdr.Handle;
    StringNumber = 0x04;
    UnicodeString = (CHAR16 *) PcdGetPtr (PcdChassisAssetTag);
    if ( StrCmp (UnicodeString, PcdDefaultString) == 0x00 ) {
#if SMC_UPDATE_DMI_WITH_FRU0_SUPPORT
        StrCpy(UnicodeString, SMC_Chassis_AssetTag_default);
        GetValidSMBIOSString (AsciiString, UnicodeString);
#else
        AsciiStrCpy (AsciiString, SmBiosDefaultString);
#endif
    } else {
        GetValidSMBIOSString (AsciiString, UnicodeString);
    }
    Status = SmbiosProtocol->UpdateString (
                SmbiosProtocol,
                &SmbiosHandle,
                &StringNumber,
                AsciiString);
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "SmbiosProtocol->UpdateString Status:%r \n", Status));
    if (EFI_ERROR (StatusOfRequestedSmbiosType)) {
        FreePool (Type3Structure);
    }
#endif //SMC_UPDATE_DMI_WITH_FRU0_SUPPORT
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "Exiting from %a\n", __FUNCTION__ ));
    return;
}

/**
    Register notification callback on SMBIOS protocol to update FRU Smbios
    structure.

    @param VOID

    @retval VOID

**/

EFI_STATUS
UpdateFruSmbiosTables (
  VOID )
{
    VOID    *SmbiosProtocolRegistration;

    //
    // Create Notification event for SmbiosProtocol GUID
    //
    EfiCreateProtocolNotifyEvent (
        &gEfiSmbiosProtocolGuid,
        TPL_CALLBACK,
        FruSmbiosTableUpdate,
        NULL,
        &SmbiosProtocolRegistration );

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
