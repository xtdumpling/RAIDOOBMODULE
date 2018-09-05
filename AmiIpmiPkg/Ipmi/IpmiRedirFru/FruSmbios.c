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
//    Bug Fix:  SMIBOS Type 2 corruption if FRU1 BV filed is null. 
//    Reason:   Null string will corrupt SMBIOS type2. 
//    Auditor:  William Hsu
//    Date:     Apr/17/2017
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

/** @file FruSmbios.c
    Function implementations to publish FRU strings.

**/

//----------------------------------------------------------------------

#include "IpmiRedirFru.h"

//----------------------------------------------------------------------

extern EFI_IPMI_TRANSPORT      *gIpmiTransport;

#if FRU_SMBIOS_BINARY_TYPE_CODE_SUPPORT
/**
    Converts the low nibble of a byte  to hex unicode character.

    @param Nibble - The Nibble that needs to be converted as Hex Unicode
                     character.

    @return CHAR16 The Hex Unicode character of input nibble.

**/

CHAR16
NibbleToHexChar (
  IN UINT8      Nibble )
{
    Nibble &= 0x0F;
    if (Nibble <= 0x9) {
        return (CHAR16)(Nibble + L'0');
    }

    return (CHAR16)(Nibble - 0xA + L'A');
}

/**
    Converts binary input string to Unicode string.

    @param Str - The Output Unicode string.
    @param HexStringBufferLength - The Output Unicode string length.
    @param Buf - The Binary input string.
    @param Len - The Binary input string length.

    @retval EFI_BUFFER_TOO_SMALL Str string buffer length is not sufficient.
    @retval EFI_SUCCESS The Binary input string is converted successfully.

**/

EFI_STATUS
BufToHexString (
  IN OUT CHAR16         *Str,
  IN OUT UINTN          *HexStringBufferLength,
  IN UINT8              *Buf,
  IN UINTN              Len )
{
    UINTN       Idx;
    UINT8       Byte;
    UINTN       StrLen;

    //
    // Make sure string is either passed or allocate enough.
    // It takes 2 Unicode characters (4 bytes) to represent 1 byte of the binary buffer.
    // Plus the Unicode termination character.
    //
    StrLen = Len * 2;
    if (StrLen > ((*HexStringBufferLength) - 1)) {
        *HexStringBufferLength = StrLen + 1;
        return EFI_BUFFER_TOO_SMALL;
    }

    *HexStringBufferLength = StrLen + 1;
    //
    // Ends the string.
    //
    Str[StrLen] = L'\0';

    for (Idx = 0; Idx < Len; Idx++) {
        Byte = Buf[Idx];
        Str[StrLen - 1 - Idx * 2] = NibbleToHexChar (Byte);
        Str[StrLen - 2 - Idx * 2] = NibbleToHexChar ((UINT8)(Byte >> 4));
    }

    return EFI_SUCCESS;
}

/**
    Converts binary buffer to Unicode character.

    @param UnicodeStr - The Unicode string to be written to.
               The buffer must be large enough.
    @param BinPtr - The Binary string to be converted.
    @param Length - Length of the Binary string to be converted.

    @return CHAR16* The address to the Unicode string - same as UnicodeStr.

**/

CHAR16*
IpmiBinary2Unicode (
  OUT CHAR16         *UnicodeStr,
  IN  UINT8          *BinPtr,
  IN  UINTN          Length
  )
{
    EFI_STATUS    Status;
    UINTN         BufferLen;

    //
    // 1 byte of the binary buffer takes 2 unicode characters (4 bytes) for hex representation
    // BufferLen includes space for hex representation + "h" suffix + null terminator
    //
    BufferLen = (Length * 2 + 2) * sizeof (UINT16);

    Status = BufToHexString (UnicodeStr, &BufferLen, BinPtr, Length);

    if (EFI_ERROR(Status)) {
        UnicodeStr[0] = L'\0';
    } else {
        UnicodeStr[Length * 2] = L'h';
        UnicodeStr[Length * 2 + 1] = L'\0';
    }

    return UnicodeStr;
}
#endif

/**
    Converts 6bit ASCII characters to Unicode.

    @param UnicodeStr - The Unicode string to be written to. The buffer must
                     be large enough.
    @param AsciiStr - The 6bit ASCII string to be converted.
    @param Length   - Length of the ASCII string to be converted.

    @retval CHAR16* The address to the Unicode string - same as UnicodeStr.

**/

CHAR16*
Ipmi6bAscii2Unicode (
  OUT CHAR16         *UnicodeStr,
  IN  UINT8          *AsciiStr,
  IN  UINTN           Length )
{
    UINTN   i;
    UINTN   j;
    UINT32  Str;

    //
    // Unpack 3 6bit value (24 bits) to 4 8bit value at a time (3 bytes to 4 bytes)
    // Also 6bit to 8bit value has to be offset by 0x20.
    // i.e., 000000b -> 0x20, 000001b -> 0x21, etc.
    //

    //
    // Convert AsciiStr up to length that is multiple of 3
    //
    for (i = 0; i < Length / 3; i++) {
        Str = *((UINT32 *) (AsciiStr + i * 3));
        for (j = 0; j < 4; j++) {
            UnicodeStr[i * 4 + j] = (CHAR8) (Str & 0x3F) + 0x20;
            Str >>= 6;
        }
    }

    //
    // Convert rest of AsciiStr entries
    //
    Str = *((UINT32 *) (AsciiStr + i * 3));
    for (j = 0; j < (Length - ((Length / 3) * 3)); j++) {
        UnicodeStr[i * 4 + j] = (CHAR8) (Str & 0x3F) + 0x20;
        Str >>= 6;
    }

    UnicodeStr[i * 4 + j] = '\0';

    return UnicodeStr;
}

/**
    Converts ASCII characters to Unicode.

    @param UnicodeStr - The Unicode string to be written to. The buffer must
                       be large enough.
    @param AsciiStr - The ASCII string to be converted.
    @param Length   - Length of the ASCII string to be converted.

    @return CHAR16* The address to the Unicode string - same as UnicodeStr.

**/

CHAR16*
IpmiAscii2Unicode (
  OUT CHAR16         *UnicodeStr,
  IN  UINT8          *AsciiStr,
  IN  UINTN           Length )
{
    UINTN   i;

    for (i = 0; i < Length; i++) {
        UnicodeStr[i] = AsciiStr[i];
    }

    UnicodeStr[i] = '\0';

    return UnicodeStr;
}

/**
    Reads the FRU data from InputDataBuffer and convert it to unicode string

    @param InputDataBuffer Input FRU data buffer
    @param OutputUnicodeString Output Unicode string buffer

         
    @retval EFI_SUCCESS FRU data converted to unicode successfully
    @retval EFI_UNSUPPORTED FRU data type not supported
    @retval EFI_OUT_OF_RESOURCES Buffer allocation failed for unicode string

**/

EFI_STATUS
GetUnicodeFromEncodedFru (
  IN UINT8          *InputDataBuffer,
  IN UINT8          Type,
  IN UINT8          Length,
  OUT CHAR16        **OutputUnicodeString )
{

    switch(Type) {
#if FRU_SMBIOS_BINARY_TYPE_CODE_SUPPORT
        case 0:
            //
            // Binary coding. Allocate space for hex representation + "h" suffix + null terminator
            //
            *OutputUnicodeString = AllocateRuntimeZeroPool ((Length * 2 + 2) * sizeof(UINT16));
            if (*OutputUnicodeString != NULL) {
                IpmiBinary2Unicode (*OutputUnicodeString, (InputDataBuffer + 1), Length);
                break;
            }
            return EFI_OUT_OF_RESOURCES;
#endif
        case 2:
            //
            // 6bit ASCII Packed coding
            //
            *OutputUnicodeString = AllocateRuntimeZeroPool ((((Length * 8) / 3) + 2));
            if (*OutputUnicodeString != NULL) {
                Ipmi6bAscii2Unicode (*OutputUnicodeString, (InputDataBuffer + 1), Length);
                break;
            }
            return EFI_OUT_OF_RESOURCES;
        case 3:
            //
            // ASCII coding
            //
            *OutputUnicodeString = AllocateRuntimeZeroPool (((Length * 2) + 2));
            if (*OutputUnicodeString != NULL) {
                IpmiAscii2Unicode (*OutputUnicodeString, (InputDataBuffer + 1), Length);
                break;
            }
            return EFI_OUT_OF_RESOURCES;
        default:
            return EFI_UNSUPPORTED;
    }
    return EFI_SUCCESS;
}

/**
    Reads the FRU data from InputDataBuffer and save it in OutputUnicodeString

    @param InputDataBuffer Input FRU data buffer
    @param OutputUnicodeString Output Unicode string buffer

    @retval EFI_SUCCESS FRU data converted to unicode successfully
    @retval EFI_UNSUPPORTED FRU data type not supported 
    @retval EFI_ERROR FRU Data not present
    @retval EFI_OUT_OF_RESOURCES Buffer allocation failed 

**/

EFI_STATUS
GetFRUData(
  IN  UINT8     *InputDataBuffer,
  OUT CHAR16    **OutputUnicodeString
)
{
    UINT8           Type = ( ((*InputDataBuffer) & FRU_DATA_TYPE_MASK) >> 0x06);
    UINT8           Length = ((*InputDataBuffer) & FRU_DATA_LENGTH_MASK);
    EFI_STATUS      Status;

    if( Length > 0 ) {
        Status = GetUnicodeFromEncodedFru (
                        InputDataBuffer,
                        Type,
                        Length,
                        OutputUnicodeString );
    } else {
        return EFI_NOT_FOUND;
    }
    return Status;

}

/**
    Reads the FRU information and publishes the FRU stings by updating the
    dynamic PCDs. If the FRU information is not present, publishes dummy records.

    @param This  - Pointer to the Redir FRU protocol
    @param GenerateDummySmbiosRecords - TRUE: Generates FRU records.
                                      - FALSE: Generates Dummy records.

    @retval VOID

**/

VOID
GenerateFruSmbiosData (
  IN EFI_SM_FRU_REDIR_PROTOCOL    *This,
  IN BOOLEAN                      GenerateDummySmbiosRecords )
{
    EFI_STATUS                                  Status;
    EFI_IPMI_FRU_GLOBAL                         *FruPrivate = NULL;
    IPMI_FRU_COMMON_HEADER                      FruCommonHeader = {0};
    UINTN                                       i;
    UINTN                                       Offset = 0;
    UINTN                                       Length;
    UINT8                                       *TempPtr;
    CHAR16                                      *StrPtr;
    CHAR16                                      *AssetTagString = NULL;
    CHAR16                                      *ManufacturerString = NULL;
    CHAR16                                      *ProductName = NULL;
    CHAR16                                      *ProductNameWithModelNo = NULL;
    UINTN                                       SizeofBuffer;
    UINT8                                       FruRecIndex = 0;
    IPMI_FRU_MULTI_RECORD_HEADER                MultiRecordHeader = {0};
    BOOLEAN                                     IsSystemUuidSubRecordType = FALSE;
    UINT8                                       SubRecordType = 0;
#if SMCPKG_SUPPORT == 0
    IPMI_SYSTEM_UUID_SUB_RECORD_WITH_CHECKSUM   SystemUuidSubRecordwithChecksum;
#endif

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "%a Entered... GenerateDummySmbiosRecords: %x\n", __FUNCTION__, GenerateDummySmbiosRecords));
    //
    // Only get the FRU information if we are going to create non dummy SMBIOS records
    //
    if ( !GenerateDummySmbiosRecords ) {
        //
        // Search through the FRU records in mIpmiFruGlobal->FruDeviceInfo to identify FRU_DEVICE_ID record
        //
        if (FRU_DEVICE_ID != 0xFF) {
            FruPrivate = INSTANCE_FROM_EFI_SM_IPMI_FRU_THIS (This);
            for (i = 0; i < FruPrivate->NumSlots; i++) {
                if ((FruPrivate->FruDeviceInfo[i].Valid) && (FruPrivate->FruDeviceInfo[i].FruDevice.FruDeviceId == FRU_DEVICE_ID)) {
                    FruRecIndex = (UINT8) i;
                    break;
                }
            }
        }

#if SMCPKG_SUPPORT
        FruRecIndex = 0;
#endif

        Status = EfiGetFruRedirData (This, FruRecIndex, 0, sizeof (IPMI_FRU_COMMON_HEADER), (UINT8*) &FruCommonHeader);
        if (EFI_ERROR (Status)) {
            return;
        }

        //
        // Do a validity check on the FRU header, since it may be all 0xFF(s) in
        // case there of no FRU programmed on the system.
        //
        if  ( CalculateCheckSum8 ((UINT8 *)&FruCommonHeader, sizeof (FruCommonHeader)) != 0 ){
            DEBUG((EFI_D_ERROR, "FRU header invalid.\n"));
            //
            //  The FRU information is bad so we need to create dummy SMBIOS records and return.
            //
            GenerateDummySmbiosRecords = TRUE;
        }
    }

#if SMCPKG_SUPPORT
    GenerateDummySmbiosRecords = 0;
#endif

#if SMCPKG_SUPPORT == 0
    //
    // Prepare Manufacturer data (SMBIOS Type 1).
    //

    Offset = FruCommonHeader.ProductInfoStartingOffset * 8;

    if ((Offset > 0) && !GenerateDummySmbiosRecords) {
        Length = 0;
        EfiGetFruRedirData (This, FruRecIndex, (Offset + 1), 1, (UINT8*) &Length);
        if (Length > 0) {
            Length = Length * 8;
            TempPtr = (UINT8*)AllocateRuntimeZeroPool (Length);
            if (TempPtr != NULL) {
                Status = EfiGetFruRedirData (This, FruRecIndex, Offset, Length, TempPtr);
                if (!EFI_ERROR (Status)) {
                    //
                    //  Validating Product Info Area Checksum before reading the Product Info
                    //  Area Data
                    //
                    if ( CalculateCheckSum8 (TempPtr, Length) == 0 ) {

                        //
                        // Product Manufacturer
                        //
                        Offset = PRODUCT_MFG_OFFSET;
                        Length = FRU_DATA_LENGTH_MASK & TempPtr[Offset];
                        Status = GetFRUData (&TempPtr[Offset], &ManufacturerString);
                        if (!EFI_ERROR (Status)) {
                            SizeofBuffer = StrSize (ManufacturerString);
                            SERVER_IPMI_DEBUG ((EFI_D_INFO, "SystemManufacturer: ManufacturerString: %S\n", ManufacturerString));
                            PcdSetPtr (PcdSystemManufacturer, &SizeofBuffer, (VOID*)ManufacturerString);
                            SERVER_IPMI_DEBUG ((EFI_D_INFO, "SystemManufacturer: %S\n", (CHAR16 *) PcdGetPtr (PcdSystemManufacturer)));
                        }

                        //
                        // Product Name. For SMBIOS it is actually concatenation of Product Name and Model Number or Part Number.
                        //
                        Offset = Offset + Length + 1;
                        Length = FRU_DATA_LENGTH_MASK & TempPtr[Offset];
                        Status = GetFRUData (&TempPtr[Offset], &ProductName);
                        Offset = Offset + Length + 1;
                        Length = FRU_DATA_LENGTH_MASK & TempPtr[Offset];

                        //
                        // Product Model Number, concatenate it to the existing (if exists) Product Name.
                        //
                        if (!EFI_ERROR (Status)) {
                            Status = GetFRUData (&TempPtr[Offset], &StrPtr);
                            if (!EFI_ERROR (Status)) {
                                ProductNameWithModelNo = (CHAR16*)AllocateRuntimeZeroPool ( StrSize (ProductName) + StrSize (StrPtr) + 2 ); // 2 bytes for space character L" "
                                if ( ProductNameWithModelNo != NULL ) {
                                    StrCpy (ProductNameWithModelNo, ProductName);
                                    StrCat (ProductNameWithModelNo, L" ");
                                    StrCat (ProductNameWithModelNo, StrPtr);
                                    FreePool(ProductName);
                                    FreePool(StrPtr);
                                    SizeofBuffer = StrSize(ProductNameWithModelNo);
                                    SERVER_IPMI_DEBUG ((EFI_D_INFO, "SystemProductName: StrPtr: %S\n", ProductNameWithModelNo));
                                    PcdSetPtr (PcdSystemProductName, &SizeofBuffer, (VOID*)ProductNameWithModelNo);
                                    SERVER_IPMI_DEBUG ((EFI_D_INFO, "SystemProductName: %S\n", (CHAR16 *) PcdGetPtr (PcdSystemProductName)));
                                    FreePool (ProductNameWithModelNo);
                                } else { // Free the allocated buffer for StrPtr
                                    FreePool (StrPtr);
                                }
                            } else { // Update the product name as Model No not available
                                SizeofBuffer = StrSize (ProductName);
                                SERVER_IPMI_DEBUG ((EFI_D_INFO, "SystemProductName: ProductName: %S\n", ProductName));
                                PcdSetPtr (PcdSystemProductName, &SizeofBuffer, (VOID*)ProductName);
                                SERVER_IPMI_DEBUG ((EFI_D_INFO, "SystemProductName: %S\n", (CHAR16 *) PcdGetPtr (PcdSystemProductName)));
                                FreePool (ProductName);
                            }
                        } 
                        //
                        // Product Version
                        //
                        Offset = Offset + Length + 1;
                        Length = FRU_DATA_LENGTH_MASK & TempPtr[Offset];
                        Status = GetFRUData (&TempPtr[Offset], &StrPtr);
                        if (!EFI_ERROR (Status)) {
                            SizeofBuffer = StrSize (StrPtr);
                            SERVER_IPMI_DEBUG ((EFI_D_INFO, "PcdSystemVersion: StrPtr: %S \n", StrPtr));
                            PcdSetPtr (PcdSystemVersion, &SizeofBuffer, (VOID*)StrPtr);
                            SERVER_IPMI_DEBUG ((EFI_D_INFO, "PcdSystemVersion: %S\n", (CHAR16 *) PcdGetPtr (PcdSystemVersion)));
                            FreePool (StrPtr);
                        }

                        //
                        // Product Serial Number
                        //
                        Offset = Offset + Length + 1;
                        Length = FRU_DATA_LENGTH_MASK & TempPtr[Offset];
                        Status = GetFRUData (&TempPtr[Offset], &StrPtr);
                        if (!EFI_ERROR (Status)) {
                            SizeofBuffer = StrSize (StrPtr);
                            SERVER_IPMI_DEBUG ((EFI_D_INFO, "PcdSystemSerialNumber: StrPtr: %S\n", StrPtr));
                            PcdSetPtr (PcdSystemSerialNumber, &SizeofBuffer, (VOID*)StrPtr);
                            SERVER_IPMI_DEBUG ((EFI_D_INFO, "PcdSystemSerialNumber: %S\n", (CHAR16 *) PcdGetPtr (PcdSystemSerialNumber)));
                            FreePool (StrPtr);
                        }

                        //
                        // Product Asset Tag
                        //
                        Offset = Offset + Length + 1;
                        Length = FRU_DATA_LENGTH_MASK & TempPtr[Offset];
                        Status = GetFRUData (&TempPtr[Offset], &AssetTagString);
                    }
                }
                FreePool (TempPtr);
            }
        }
    }
#endif
    //
    // Prepare Base Board data (SMBIOS Type 2)
    //
    Offset = FruCommonHeader.BoardAreaStartingOffset * 8;
    if ((Offset > 0) && !GenerateDummySmbiosRecords) {
        Length = 0;
        EfiGetFruRedirData (This, FruRecIndex, (Offset + 1), 1, (UINT8*) &Length);
        if (Length > 0) {
            Length = Length * 8;
            TempPtr = (UINT8*) AllocateRuntimeZeroPool (Length);
            if (TempPtr != NULL) {
                Status = EfiGetFruRedirData (This, FruRecIndex, Offset, Length, TempPtr);
                if (!EFI_ERROR (Status)) {
                    //
                    //  Validating Board Info Area Checksum before reading the Board Info
                    //  Area Data
                    //
                    if ( CalculateCheckSum8 ( TempPtr, Length ) == 0 ) {
#if SMCPKG_SUPPORT == 0
                        //
                        //Fill the AssertTag
                        //
                        if ( AssetTagString != NULL ) {
                            SizeofBuffer = StrSize (AssetTagString);
                            SERVER_IPMI_DEBUG ((EFI_D_INFO, "AssetTagString: StrPtr: %S\n", StrPtr));
                            PcdSetPtr (PcdBaseBoardAssetTag, &SizeofBuffer, (VOID*)AssetTagString);
                            SERVER_IPMI_DEBUG ((EFI_D_INFO, "PcdBaseBoardAssetTag: %S\n", (CHAR16 *) PcdGetPtr (PcdBaseBoardAssetTag)));
                        }
#endif
                        //
                        // Board Manufacturer
                        //
                        Offset = BOARD_MFG_OFFSET;
                        Length = FRU_DATA_LENGTH_MASK & TempPtr[Offset];
                        Status = GetFRUData (&TempPtr[Offset], &StrPtr);
                        if (!EFI_ERROR (Status)) {
                            SizeofBuffer = StrSize (StrPtr);
                            SERVER_IPMI_DEBUG ((EFI_D_INFO, "PcdBaseBoardManufacturer: StrPtr: %S\n", StrPtr));
                            PcdSetPtr (PcdBaseBoardManufacturer, &SizeofBuffer, (VOID*)StrPtr);
                            SERVER_IPMI_DEBUG ((EFI_D_INFO, "PcdBaseBoardManufacturer: %S\n", (CHAR16 *) PcdGetPtr (PcdBaseBoardManufacturer)));
                            FreePool (StrPtr);
                        }

                        //
                        // Board Product Name
                        //
                        Offset = Offset + Length + 1;
                        Length = FRU_DATA_LENGTH_MASK & TempPtr[Offset];
                        Status = GetFRUData (&TempPtr[Offset], &StrPtr);
                        if (!EFI_ERROR (Status)) {
                            SizeofBuffer = StrSize (StrPtr);
                            SERVER_IPMI_DEBUG ((EFI_D_INFO, "PcdBaseBoardProductName: StrPtr: %S\n", StrPtr));
                            PcdSetPtr (PcdBaseBoardProductName, &SizeofBuffer, (VOID*)StrPtr);
                            SERVER_IPMI_DEBUG ((EFI_D_INFO, "PcdBaseBoardProductName: %S\n", (CHAR16 *) PcdGetPtr (PcdBaseBoardProductName)));
                            FreePool (StrPtr);
                        }

                        //
                        // Board Serial Number
                        //
                        Offset = Offset + Length + 1;
                        Length = FRU_DATA_LENGTH_MASK & TempPtr[Offset];
                        Status = GetFRUData (&TempPtr[Offset], &StrPtr);
                        if (!EFI_ERROR (Status)) {
                            SizeofBuffer = StrSize(StrPtr);
                            SERVER_IPMI_DEBUG ((EFI_D_INFO, "PcdBaseBoardSerialNumber: StrPtr: %S\n", StrPtr));
                            PcdSetPtr (PcdBaseBoardSerialNumber, &SizeofBuffer, (VOID*)StrPtr);
                            SERVER_IPMI_DEBUG ((EFI_D_INFO, "PcdBaseBoardSerialNumber: %S\n", (CHAR16 *) PcdGetPtr (PcdBaseBoardSerialNumber)));
                            FreePool (StrPtr);
                        }

                        //
                        // Board Version Number
                        //
                        Offset = Offset + Length + 1;
                        Length = FRU_DATA_LENGTH_MASK & TempPtr[Offset];
#if SMCPKG_SUPPORT
                        Offset = Offset + Length + 1;       //FRU File ID
                        Length = FRU_DATA_LENGTH_MASK & TempPtr[Offset];
                        Offset = Offset + Length + 1;       //SMC Board revision
                        Length = FRU_DATA_LENGTH_MASK & TempPtr[Offset];
#endif
                        Status = GetFRUData (&TempPtr[Offset], &StrPtr);

#if SMCPKG_SUPPORT                        
                        if (!EFI_ERROR (Status) && (StrPtr[0]!=00) ) { //Avoid null string.
#else    
                        if (!EFI_ERROR (Status)) {
#endif
                            SizeofBuffer = StrSize (StrPtr);
                            SERVER_IPMI_DEBUG ((EFI_D_INFO, "PcdBaseBoardVersion: StrPtr: %S\n", StrPtr));
                            PcdSetPtr (PcdBaseBoardVersion, &SizeofBuffer, (VOID*)StrPtr);
                            SERVER_IPMI_DEBUG ((EFI_D_INFO, "PcdBaseBoardVersion: %S\n", (CHAR16 *) PcdGetPtr (PcdBaseBoardVersion)));
                            FreePool (StrPtr);
                        }
                    }
                }
                FreePool (TempPtr);
            }
        }
    }
#if SMCPKG_SUPPORT == 0
    //
    // Prepare Chassis data (SMBIOS Type 3)
    //
    Offset = FruCommonHeader.ChassisInfoStartingOffset * 8;
    if ((Offset > 0) && !GenerateDummySmbiosRecords) {
        Length = 0;
        EfiGetFruRedirData (This, FruRecIndex, (Offset + 1), 1, (UINT8*) &Length);
        if (Length > 0) {
            Length = Length * 8;
            TempPtr = (UINT8*) AllocateRuntimeZeroPool (Length);
            if (TempPtr != NULL) {
                Status = EfiGetFruRedirData (This, FruRecIndex, Offset, Length, TempPtr);
                if (!EFI_ERROR (Status)) {
                    //
                    //  Validating Chasis Info Area Checksum before reading the Chasis Info
                    //  Area Data
                    //
                    if ( CalculateCheckSum8 ( TempPtr, Length ) == 0 ) {

                        //
                        // Update Chassis type and ChassisLockPresent bit
                        //
                        PcdSet8 (PcdChassisType, TempPtr[CHASSIS_TYPE_OFFSET] | (1<<16));
                        SERVER_IPMI_DEBUG ((EFI_D_INFO, "PcdChassisType: %x\n", PcdGet8 (PcdChassisType) ));

                        //
                        // Chassis Version
                        //
                        Offset = CHASSIS_PART_NUMBER;
                        Length = FRU_DATA_LENGTH_MASK & TempPtr[Offset];
                        Status = GetFRUData (&TempPtr[Offset], &StrPtr);
                        if (!EFI_ERROR (Status)) {
                            SizeofBuffer = StrSize(StrPtr);
                            SERVER_IPMI_DEBUG ((EFI_D_INFO, "PcdChassisVersion: StrPtr: %S\n", StrPtr));
                            PcdSetPtr (PcdChassisVersion, &SizeofBuffer, (VOID*)StrPtr);
                            SERVER_IPMI_DEBUG ((EFI_D_INFO, "PcdChassisVersion: %S\n", (CHAR16 *) PcdGetPtr (PcdChassisVersion)));
                            FreePool (StrPtr);
                        }

                        //
                        // Chassis Serial Number
                        //
                        Offset = Offset + Length + 1;
                        Length = FRU_DATA_LENGTH_MASK & TempPtr[Offset];
                        Status = GetFRUData (&TempPtr[Offset], &StrPtr);
                        if (!EFI_ERROR (Status)) {
                            SizeofBuffer = StrSize (StrPtr);
                            SERVER_IPMI_DEBUG ((EFI_D_INFO, "PcdChassisSerialNumber: StrPtr: %S\n", StrPtr));
                            PcdSetPtr (PcdChassisSerialNumber, &SizeofBuffer, (VOID*)StrPtr);
                            SERVER_IPMI_DEBUG ((EFI_D_INFO, "PcdChassisSerialNumber: %S\n", (CHAR16 *) PcdGetPtr (PcdChassisSerialNumber)));
                            FreePool (StrPtr);
                        }

                        //
                        // Data receive at product entry
                        // Fill the Manufacturer
                        //
                        if ( ManufacturerString != NULL ) {
                            SizeofBuffer = StrSize (ManufacturerString);
                            SERVER_IPMI_DEBUG ((EFI_D_INFO, "PcdChassisManufacturer: ManufacturerString: %S\n", ManufacturerString));
                            PcdSetPtr (PcdChassisManufacturer, &SizeofBuffer, (VOID*)ManufacturerString);
                            SERVER_IPMI_DEBUG ((EFI_D_INFO, "PcdChassisManufacturer: %S\n", (CHAR16 *) PcdGetPtr (PcdChassisManufacturer)));
                        }
                        //
                        // Fill the AssertTag
                        //
                        if ( AssetTagString != NULL ) {
                            SizeofBuffer = StrSize (AssetTagString);
                            SERVER_IPMI_DEBUG ((EFI_D_INFO, "PcdChassisAssetTag: AssetTagString: %S\n", AssetTagString));
                            PcdSetPtr (PcdChassisAssetTag, &SizeofBuffer, (VOID*)AssetTagString);
                            SERVER_IPMI_DEBUG ((EFI_D_INFO, "PcdChassisAssetTag: %S\n", (CHAR16 *) PcdGetPtr (PcdChassisAssetTag)));
                        }
                    }
                }
                FreePool (TempPtr);
            }
        }
    }
    //
    // Free the memory allocated
    //
    if ( ManufacturerString != NULL ) {
        FreePool (ManufacturerString);
    }
    if ( AssetTagString != NULL ) {
        FreePool (AssetTagString);
    }

    //
    // Prepare System UUID data (SMBIOS Type 1)
    //
    Offset = FruCommonHeader.MultiRecInfoStartingOffset * 8;
    if ((Offset > 0) && !GenerateDummySmbiosRecords) {
        SERVER_IPMI_DEBUG ((EFI_D_INFO, "Multirecord Info Area present\n"));
        do {

            //
            // Get the Multi Record Header
            //
            Status = EfiGetFruRedirData (This, FruRecIndex, Offset, sizeof(IPMI_FRU_MULTI_RECORD_HEADER), (UINT8*) &MultiRecordHeader);
            if (!EFI_ERROR(Status)) {

                //
                // Calculate Record Header Checksum
                //
                if ( CalculateCheckSum8 ((UINT8 *)&MultiRecordHeader, sizeof (IPMI_FRU_MULTI_RECORD_HEADER)) != 0 ){
                    DEBUG((EFI_D_ERROR, "FRU MultiRecord Record invalid.\n"));
                    break;
                }

                //
                // Check Record ID
                //
                if ((MultiRecordHeader.RecordTypeId != MANAGEMENT_ACCESS_RECORD) && (!(MultiRecordHeader.EndofList)) ) {
                    //
                    // If not Management Access Record, move to next record
                    //
                    Offset += sizeof(IPMI_FRU_MULTI_RECORD_HEADER) + MultiRecordHeader.RecordLength;
                    continue;
                } else { 
                    if ( MultiRecordHeader.RecordTypeId == MANAGEMENT_ACCESS_RECORD ) { // Management Access Record found
                        //
                        // If Management Access Record found, check System UUID sub record type
                        //
                        SERVER_IPMI_DEBUG ((EFI_D_INFO, "Management Access record found\n"));
                        Status = EfiGetFruRedirData (This, FruRecIndex, (Offset + sizeof(IPMI_FRU_MULTI_RECORD_HEADER)), 1, (UINT8*) &SubRecordType);
                        if (!(EFI_ERROR(Status))) {
                            if (SubRecordType == SYSTEM_UUID_SUB_RECORD_TYPE) {
                                //
                                // Calculate record checksum
                                //
                                SystemUuidSubRecordwithChecksum.RecordCheckSum = MultiRecordHeader.RecordChecksum; // Copying record CheckSum
                                Status = EfiGetFruRedirData (This, FruRecIndex, (Offset + sizeof(IPMI_FRU_MULTI_RECORD_HEADER)), sizeof(SystemUuidSubRecordwithChecksum)- RECORD_CHECKSUM_SIZE, (UINT8*) &(SystemUuidSubRecordwithChecksum.SubRecordId));
                                if (EFI_ERROR(Status)) {
                                    break;
                                }
                                if ((CalculateCheckSum8 ((UINT8 *)&SystemUuidSubRecordwithChecksum, sizeof(SystemUuidSubRecordwithChecksum))) == 0) {
                                    //
                                    // System UUID sub record type is found, if checksum is zero
                                    //
                                    SERVER_IPMI_DEBUG ((EFI_D_INFO, "System UUID Sub Record Type found and Checksum OK!!!\n"));
                                    IsSystemUuidSubRecordType = TRUE;
                                }
                                break;
                            } else if (!(MultiRecordHeader.EndofList)) {
                                //
                                // if System UUID sub record type is not found, move to next record
                                //
                                Offset += sizeof(IPMI_FRU_MULTI_RECORD_HEADER) + MultiRecordHeader.RecordLength;
                                continue;
                            } else {
                                //
                                // End of list is found, So no more records
                                //
                                break;
                            }
                        } else if (!(MultiRecordHeader.EndofList)) {
                            //
                            // Still records are available, so move to next
                            //
                            Offset += sizeof(IPMI_FRU_MULTI_RECORD_HEADER) + MultiRecordHeader.RecordLength;
                            continue;
                        } else { // No more records
                            break;
                        }
                    } else { // End of entire MultiRecord
                        break;
                    }
                }
            } // if (!EFI_ERROR(Status))
        } while(!EFI_ERROR(Status));

        if (IsSystemUuidSubRecordType) { // Management Access Record

            //
            // Copy the UUID in PCD
            //
            SizeofBuffer = sizeof (SystemUuidSubRecordwithChecksum.Uuid);
            SERVER_IPMI_DEBUG ((EFI_D_INFO, "UUID Size: %x\n", SizeofBuffer));
            PcdSetPtr (PcdSystemUuid, &SizeofBuffer, (VOID*)&(SystemUuidSubRecordwithChecksum.Uuid));
        } // if (IsSystemUuidSubRecordType)
    }
#endif
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "%a Exiting... \n", __FUNCTION__));

}

#if SMC_UPDATE_DMI_WITH_FRU0_SUPPORT
VOID
SmcGenerateFruSmbiosData (
  IN EFI_SM_FRU_REDIR_PROTOCOL            *This,
  IN BOOLEAN                              GenerateDummySmbiosRecords
)
{
    EFI_STATUS                                  Status;
    EFI_IPMI_FRU_GLOBAL                         *FruPrivate = NULL;
    IPMI_FRU_COMMON_HEADER                      FruCommonHeader = {0};
    UINTN                                       i;
    UINTN                                       Offset = 0;
    UINTN                                       Length;
    UINT8                                       *TempPtr;
    CHAR16                                      *StrPtr;
    CHAR16                                      *AssetTagString = NULL;
    CHAR16                                      *ManufacturerString = NULL;
    CHAR16                                      *ProductName = NULL;
    CHAR16                                      *ProductNameWithModelNo = NULL;
    UINTN                                       SizeofBuffer;
    UINT8                                       FruRecIndex = 0;
    IPMI_FRU_MULTI_RECORD_HEADER                MultiRecordHeader = {0};
    BOOLEAN                                     IsSystemUuidSubRecordType = FALSE;
    UINT8                                       SubRecordType = 0;
    IPMI_SYSTEM_UUID_SUB_RECORD_WITH_CHECKSUM   SystemUuidSubRecordwithChecksum;


    SERVER_IPMI_DEBUG ((EFI_D_INFO, "%a Entered... SmcGenerateDummySmbiosRecords: %x\n", __FUNCTION__, GenerateDummySmbiosRecords));
    //
    // Only get the FRU information if we are going to create non dummy SMBIOS records
    //
    if ( !GenerateDummySmbiosRecords ) {
        //
        // Search through the FRU records in mIpmiFruGlobal->FruDeviceInfo to identify FRU_DEVICE_ID record
        //
        if (FRU_DEVICE_ID != 0xFF) {
            FruPrivate = INSTANCE_FROM_EFI_SM_IPMI_FRU_THIS (This);
            for (i = 0; i < FruPrivate->NumSlots; i++) {
                if ((FruPrivate->FruDeviceInfo[i].Valid) && (FruPrivate->FruDeviceInfo[i].FruDevice.FruDeviceId == FRU_DEVICE_ID)) {
                    FruRecIndex = (UINT8) i;
                    break;
                }
            }
        }

        FruRecIndex = 0;
        Status = SmcEfiGetFruRedirData (This, FruRecIndex, 0, sizeof (IPMI_FRU_COMMON_HEADER), (UINT8*) &FruCommonHeader);
        if (EFI_ERROR (Status)) {
            return;
        }

        //
        // Do a validity check on the FRU header, since it may be all 0xFF(s) in
        // case there of no FRU programmed on the system.
        //
        if  ( CalculateCheckSum8 ((UINT8 *)&FruCommonHeader, sizeof (FruCommonHeader)) != 0 ){
            DEBUG((EFI_D_ERROR, "FRU header invalid.\n"));
            //
            //  The FRU information is bad so we need to create dummy SMBIOS records and return.
            //
            GenerateDummySmbiosRecords = TRUE;
        }
    }

    GenerateDummySmbiosRecords = 0;
    //
    // Prepare Manufacturer data (SMBIOS Type 1).
    //

    Offset = FruCommonHeader.ProductInfoStartingOffset * 8;

    if ((Offset > 0) && !GenerateDummySmbiosRecords) {
        Length = 0;
        SmcEfiGetFruRedirData (This, FruRecIndex, (Offset + 1), 1, (UINT8*) &Length);
        if (Length > 0) {
            Length = Length * 8;
            TempPtr = (UINT8*)AllocateRuntimeZeroPool (Length);
            if (TempPtr != NULL) {
                Status = SmcEfiGetFruRedirData (This, FruRecIndex, Offset, Length, TempPtr);
                if (!EFI_ERROR (Status)) {
                    //
                    //  Validating Product Info Area Checksum before reading the Product Info
                    //  Area Data
                    //
                    if ( CalculateCheckSum8 (TempPtr, Length) == 0 ) {

                        //
                        // Product Manufacturer
                        //
                        Offset = PRODUCT_MFG_OFFSET;
                        Length = FRU_DATA_LENGTH_MASK & TempPtr[Offset];
                        Status = GetFRUData (&TempPtr[Offset], &ManufacturerString);
                        if (!EFI_ERROR (Status)) {
                            SizeofBuffer = StrSize (ManufacturerString);
                            SERVER_IPMI_DEBUG ((EFI_D_INFO, "SystemManufacturer: ManufacturerString: %S\n", ManufacturerString));
                            PcdSetPtr (PcdSystemManufacturer, &SizeofBuffer, (VOID*)ManufacturerString);
                            SERVER_IPMI_DEBUG ((EFI_D_INFO, "SystemManufacturer: %S\n", (CHAR16 *) PcdGetPtr (PcdSystemManufacturer)));
                        }

                        //
                        // Product Name. For SMBIOS it is actually concatenation of Product Name and Model Number or Part Number.
                        //
                        Offset = Offset + Length + 1;
                        Length = FRU_DATA_LENGTH_MASK & TempPtr[Offset];
                        Status = GetFRUData (&TempPtr[Offset], &ProductName);
                        Offset = Offset + Length + 1;
                        Length = FRU_DATA_LENGTH_MASK & TempPtr[Offset];

                        //
                        // Product Model Number, concatenate it to the existing (if exists) Product Name.
                        //
                        if (!EFI_ERROR (Status)) {
                            Status = GetFRUData (&TempPtr[Offset], &StrPtr);
                            if (!EFI_ERROR (Status)) {
                                ProductNameWithModelNo = (CHAR16*)AllocateRuntimeZeroPool ( StrSize (ProductName) + StrSize (StrPtr) + 2 ); // 2 bytes for space character L" "
                                if ( ProductNameWithModelNo != NULL ) {
                                    StrCpy (ProductNameWithModelNo, ProductName);
                                    StrCat (ProductNameWithModelNo, L" ");
                                    StrCat (ProductNameWithModelNo, StrPtr);
                                    FreePool(ProductName);
                                    FreePool(StrPtr);
                                    SizeofBuffer = StrSize(ProductNameWithModelNo);
                                    SERVER_IPMI_DEBUG ((EFI_D_INFO, "SystemProductName: StrPtr: %S\n", ProductNameWithModelNo));
                                    PcdSetPtr (PcdSystemProductName, &SizeofBuffer, (VOID*)ProductNameWithModelNo);
                                    SERVER_IPMI_DEBUG ((EFI_D_INFO, "SystemProductName: %S\n", (CHAR16 *) PcdGetPtr (PcdSystemProductName)));
                                    FreePool (ProductNameWithModelNo);
                                } else { // Free the allocated buffer for StrPtr
                                    FreePool (StrPtr);
                                }
                            } else { // Update the product name as Model No not available
                                SizeofBuffer = StrSize (ProductName);
                                SERVER_IPMI_DEBUG ((EFI_D_INFO, "SystemProductName: ProductName: %S\n", ProductName));
                                PcdSetPtr (PcdSystemProductName, &SizeofBuffer, (VOID*)ProductName);
                                SERVER_IPMI_DEBUG ((EFI_D_INFO, "SystemProductName: %S\n", (CHAR16 *) PcdGetPtr (PcdSystemProductName)));
                                FreePool (ProductName);
                            }
                        } 
                        //
                        // Product Version
                        //
                        Offset = Offset + Length + 1;
                        Length = FRU_DATA_LENGTH_MASK & TempPtr[Offset];
                        Status = GetFRUData (&TempPtr[Offset], &StrPtr);
                        if (!EFI_ERROR (Status)) {
                            SizeofBuffer = StrSize (StrPtr);
                            SERVER_IPMI_DEBUG ((EFI_D_INFO, "PcdSystemVersion: StrPtr: %S \n", StrPtr));
                            PcdSetPtr (PcdSystemVersion, &SizeofBuffer, (VOID*)StrPtr);
                            SERVER_IPMI_DEBUG ((EFI_D_INFO, "PcdSystemVersion: %S\n", (CHAR16 *) PcdGetPtr (PcdSystemVersion)));
                            FreePool (StrPtr);
                        }

                        //
                        // Product Serial Number
                        //
                        Offset = Offset + Length + 1;
                        Length = FRU_DATA_LENGTH_MASK & TempPtr[Offset];
                        Status = GetFRUData (&TempPtr[Offset], &StrPtr);
                        if (!EFI_ERROR (Status)) {
                            SizeofBuffer = StrSize (StrPtr);
                            SERVER_IPMI_DEBUG ((EFI_D_INFO, "PcdSystemSerialNumber: StrPtr: %S\n", StrPtr));
                            PcdSetPtr (PcdSystemSerialNumber, &SizeofBuffer, (VOID*)StrPtr);
                            SERVER_IPMI_DEBUG ((EFI_D_INFO, "PcdSystemSerialNumber: %S\n", (CHAR16 *) PcdGetPtr (PcdSystemSerialNumber)));
                            FreePool (StrPtr);
                        }

                        //
                        // Product Asset Tag
                        //
                        Offset = Offset + Length + 1;
                        Length = FRU_DATA_LENGTH_MASK & TempPtr[Offset];
                        Status = GetFRUData (&TempPtr[Offset], &AssetTagString);
                    }
                }
                FreePool (TempPtr);
            }
        }
    }
#if 0
    //
    // Prepare Base Board data (SMBIOS Type 2)
    //
    Offset = FruCommonHeader.BoardAreaStartingOffset * 8;
    if ((Offset > 0) && !GenerateDummySmbiosRecords) {
        Length = 0;
        SmcEfiGetFruRedirData (This, FruRecIndex, (Offset + 1), 1, (UINT8*) &Length);
        if (Length > 0) {
            Length = Length * 8;
            TempPtr = (UINT8*) AllocateRuntimeZeroPool (Length);
            if (TempPtr != NULL) {
                Status = SmcEfiGetFruRedirData (This, FruRecIndex, Offset, Length, TempPtr);
                if (!EFI_ERROR (Status)) {
                    //
                    //  Validating Board Info Area Checksum before reading the Board Info
                    //  Area Data
                    //
                    if ( CalculateCheckSum8 ( TempPtr, Length ) == 0 ) {
                        //
                        //Fill the AssertTag
                        //
                        if ( AssetTagString != NULL ) {
                            SizeofBuffer = StrSize (AssetTagString);
                            SERVER_IPMI_DEBUG ((EFI_D_INFO, "AssetTagString: StrPtr: %S\n", StrPtr));
                            PcdSetPtr (PcdBaseBoardAssetTag, &SizeofBuffer, (VOID*)AssetTagString);
                            SERVER_IPMI_DEBUG ((EFI_D_INFO, "PcdBaseBoardAssetTag: %S\n", (CHAR16 *) PcdGetPtr (PcdBaseBoardAssetTag)));
                        }
                        //
                        // Board Manufacturer
                        //
                        Offset = BOARD_MFG_OFFSET;
                        Length = FRU_DATA_LENGTH_MASK & TempPtr[Offset];
                        Status = GetFRUData (&TempPtr[Offset], &StrPtr);
                        if (!EFI_ERROR (Status)) {
                            SizeofBuffer = StrSize (StrPtr);
                            SERVER_IPMI_DEBUG ((EFI_D_INFO, "PcdBaseBoardManufacturer: StrPtr: %S\n", StrPtr));
                            PcdSetPtr (PcdBaseBoardManufacturer, &SizeofBuffer, (VOID*)StrPtr);
                            SERVER_IPMI_DEBUG ((EFI_D_INFO, "PcdBaseBoardManufacturer: %S\n", (CHAR16 *) PcdGetPtr (PcdBaseBoardManufacturer)));
                            FreePool (StrPtr);
                        }

                        //
                        // Board Product Name
                        //
                        Offset = Offset + Length + 1;
                        Length = FRU_DATA_LENGTH_MASK & TempPtr[Offset];
                        Status = GetFRUData (&TempPtr[Offset], &StrPtr);
                        if (!EFI_ERROR (Status)) {
                            SizeofBuffer = StrSize (StrPtr);
                            SERVER_IPMI_DEBUG ((EFI_D_INFO, "PcdBaseBoardProductName: StrPtr: %S\n", StrPtr));
                            PcdSetPtr (PcdBaseBoardProductName, &SizeofBuffer, (VOID*)StrPtr);
                            SERVER_IPMI_DEBUG ((EFI_D_INFO, "PcdBaseBoardProductName: %S\n", (CHAR16 *) PcdGetPtr (PcdBaseBoardProductName)));
                            FreePool (StrPtr);
                        }

                        //
                        // Board Serial Number
                        //
                        Offset = Offset + Length + 1;
                        Length = FRU_DATA_LENGTH_MASK & TempPtr[Offset];
                        Status = GetFRUData (&TempPtr[Offset], &StrPtr);
                        if (!EFI_ERROR (Status)) {
                            SizeofBuffer = StrSize(StrPtr);
                            SERVER_IPMI_DEBUG ((EFI_D_INFO, "PcdBaseBoardSerialNumber: StrPtr: %S\n", StrPtr));
                            PcdSetPtr (PcdBaseBoardSerialNumber, &SizeofBuffer, (VOID*)StrPtr);
                            SERVER_IPMI_DEBUG ((EFI_D_INFO, "PcdBaseBoardSerialNumber: %S\n", (CHAR16 *) PcdGetPtr (PcdBaseBoardSerialNumber)));
                            FreePool (StrPtr);
                        }

                        //
                        // Board Version Number
                        //
                        Offset = Offset + Length + 1;
                        Length = FRU_DATA_LENGTH_MASK & TempPtr[Offset];
                        Status = GetFRUData (&TempPtr[Offset], &StrPtr);
                        if (!EFI_ERROR (Status)) {
                            SizeofBuffer = StrSize (StrPtr);
                            SERVER_IPMI_DEBUG ((EFI_D_INFO, "PcdBaseBoardVersion: StrPtr: %S\n", StrPtr));
                            PcdSetPtr (PcdBaseBoardVersion, &SizeofBuffer, (VOID*)StrPtr);
                            SERVER_IPMI_DEBUG ((EFI_D_INFO, "PcdBaseBoardVersion: %S\n", (CHAR16 *) PcdGetPtr (PcdBaseBoardVersion)));
                            FreePool (StrPtr);
                        }
                    }
                }
                FreePool (TempPtr);
            }
        }
    }
#endif
    //
    // Prepare Chassis data (SMBIOS Type 3)
    //
    Offset = FruCommonHeader.ChassisInfoStartingOffset * 8;
    if ((Offset > 0) && !GenerateDummySmbiosRecords) {
        Length = 0;
        SmcEfiGetFruRedirData (This, FruRecIndex, (Offset + 1), 1, (UINT8*) &Length);
        if (Length > 0) {
            Length = Length * 8;
            TempPtr = (UINT8*) AllocateRuntimeZeroPool (Length);
            if (TempPtr != NULL) {
                Status = SmcEfiGetFruRedirData (This, FruRecIndex, Offset, Length, TempPtr);
                if (!EFI_ERROR (Status)) {
                    //
                    //  Validating Chasis Info Area Checksum before reading the Chasis Info
                    //  Area Data
                    //
                    if ( CalculateCheckSum8 ( TempPtr, Length ) == 0 ) {

                        //
                        // Update Chassis type and ChassisLockPresent bit
                        //
                        PcdSet8 (PcdChassisType, TempPtr[CHASSIS_TYPE_OFFSET] | (1<<16));
                        SERVER_IPMI_DEBUG ((EFI_D_INFO, "PcdChassisType: %x\n", PcdGet8 (PcdChassisType) ));

                        //
                        // Chassis Version
                        //
                        Offset = CHASSIS_PART_NUMBER;
                        Length = FRU_DATA_LENGTH_MASK & TempPtr[Offset];
                        Status = GetFRUData (&TempPtr[Offset], &StrPtr);
                        if (!EFI_ERROR (Status)) {
                            SizeofBuffer = StrSize(StrPtr);
                            SERVER_IPMI_DEBUG ((EFI_D_INFO, "PcdChassisVersion: StrPtr: %S\n", StrPtr));
                            PcdSetPtr (PcdChassisVersion, &SizeofBuffer, (VOID*)StrPtr);
                            SERVER_IPMI_DEBUG ((EFI_D_INFO, "PcdChassisVersion: %S\n", (CHAR16 *) PcdGetPtr (PcdChassisVersion)));
                            FreePool (StrPtr);
                        }

                        //
                        // Chassis Serial Number
                        //
                        Offset = Offset + Length + 1;
                        Length = FRU_DATA_LENGTH_MASK & TempPtr[Offset];
                        Status = GetFRUData (&TempPtr[Offset], &StrPtr);
                        if (!EFI_ERROR (Status)) {
                            SizeofBuffer = StrSize (StrPtr);
                            SERVER_IPMI_DEBUG ((EFI_D_INFO, "PcdChassisSerialNumber: StrPtr: %S\n", StrPtr));
                            PcdSetPtr (PcdChassisSerialNumber, &SizeofBuffer, (VOID*)StrPtr);
                            SERVER_IPMI_DEBUG ((EFI_D_INFO, "PcdChassisSerialNumber: %S\n", (CHAR16 *) PcdGetPtr (PcdChassisSerialNumber)));
                            FreePool (StrPtr);
                        }

                        //
                        // Data receive at product entry
                        // Fill the Manufacturer
                        //
                        if ( ManufacturerString != NULL ) {
                            SizeofBuffer = StrSize (ManufacturerString);
                            SERVER_IPMI_DEBUG ((EFI_D_INFO, "PcdChassisManufacturer: ManufacturerString: %S\n", ManufacturerString));
                            PcdSetPtr (PcdChassisManufacturer, &SizeofBuffer, (VOID*)ManufacturerString);
                            SERVER_IPMI_DEBUG ((EFI_D_INFO, "PcdChassisManufacturer: %S\n", (CHAR16 *) PcdGetPtr (PcdChassisManufacturer)));
                        }
                        //
                        // Fill the AssertTag
                        //
                        if ( AssetTagString != NULL ) {
                            SizeofBuffer = StrSize (AssetTagString);
                            SERVER_IPMI_DEBUG ((EFI_D_INFO, "PcdChassisAssetTag: AssetTagString: %S\n", AssetTagString));
                            PcdSetPtr (PcdChassisAssetTag, &SizeofBuffer, (VOID*)AssetTagString);
                            SERVER_IPMI_DEBUG ((EFI_D_INFO, "PcdChassisAssetTag: %S\n", (CHAR16 *) PcdGetPtr (PcdChassisAssetTag)));
                        }
                    }
                }
                FreePool (TempPtr);
            }
        }
    }
    //
    // Free the memory allocated
    //
    if ( ManufacturerString != NULL ) {
        FreePool (ManufacturerString);
    }
    if ( AssetTagString != NULL ) {
        FreePool (AssetTagString);
    }

    //
    // Prepare System UUID data (SMBIOS Type 1)
    //
    Offset = FruCommonHeader.MultiRecInfoStartingOffset * 8;
    if ((Offset > 0) && !GenerateDummySmbiosRecords) {
        SERVER_IPMI_DEBUG ((EFI_D_INFO, "Multirecord Info Area present\n"));
        do {

            //
            // Get the Multi Record Header
            //
            Status = SmcEfiGetFruRedirData (This, FruRecIndex, Offset, sizeof(IPMI_FRU_MULTI_RECORD_HEADER), (UINT8*) &MultiRecordHeader);
            if (!EFI_ERROR(Status)) {

                //
                // Calculate Record Header Checksum
                //
                if ( CalculateCheckSum8 ((UINT8 *)&MultiRecordHeader, sizeof (IPMI_FRU_MULTI_RECORD_HEADER)) != 0 ){
                    DEBUG((EFI_D_ERROR, "FRU MultiRecord Record invalid.\n"));
                    break;
                }

                //
                // Check Record ID
                //
                if ((MultiRecordHeader.RecordTypeId != MANAGEMENT_ACCESS_RECORD) && (!(MultiRecordHeader.EndofList)) ) {
                    //
                    // If not Management Access Record, move to next record
                    //
                    Offset += sizeof(IPMI_FRU_MULTI_RECORD_HEADER) + MultiRecordHeader.RecordLength;
                    continue;
                } else { 
                    if ( MultiRecordHeader.RecordTypeId == MANAGEMENT_ACCESS_RECORD ) { // Management Access Record found
                        //
                        // If Management Access Record found, check System UUID sub record type
                        //
                        SERVER_IPMI_DEBUG ((EFI_D_INFO, "Management Access record found\n"));
                        Status = SmcEfiGetFruRedirData (This, FruRecIndex, (Offset + sizeof(IPMI_FRU_MULTI_RECORD_HEADER)), 1, (UINT8*) &SubRecordType);
                        if (!(EFI_ERROR(Status))) {
                            if (SubRecordType == SYSTEM_UUID_SUB_RECORD_TYPE) {
                                //
                                // Calculate record checksum
                                //
                                SystemUuidSubRecordwithChecksum.RecordCheckSum = MultiRecordHeader.RecordChecksum; // Copying record CheckSum
                                Status = SmcEfiGetFruRedirData (This, FruRecIndex, (Offset + sizeof(IPMI_FRU_MULTI_RECORD_HEADER)), sizeof(SystemUuidSubRecordwithChecksum)- RECORD_CHECKSUM_SIZE, (UINT8*) &(SystemUuidSubRecordwithChecksum.SubRecordId));
                                if (EFI_ERROR(Status)) {
                                    break;
                                }
                                if ((CalculateCheckSum8 ((UINT8 *)&SystemUuidSubRecordwithChecksum, sizeof(SystemUuidSubRecordwithChecksum))) == 0) {
                                    //
                                    // System UUID sub record type is found, if checksum is zero
                                    //
                                    SERVER_IPMI_DEBUG ((EFI_D_INFO, "System UUID Sub Record Type found and Checksum OK!!!\n"));
                                    IsSystemUuidSubRecordType = TRUE;
                                }
                                break;
                            } else if (!(MultiRecordHeader.EndofList)) {
                                //
                                // if System UUID sub record type is not found, move to next record
                                //
                                Offset += sizeof(IPMI_FRU_MULTI_RECORD_HEADER) + MultiRecordHeader.RecordLength;
                                continue;
                            } else {
                                //
                                // End of list is found, So no more records
                                //
                                break;
                            }
                        } else if (!(MultiRecordHeader.EndofList)) {
                            //
                            // Still records are available, so move to next
                            //
                            Offset += sizeof(IPMI_FRU_MULTI_RECORD_HEADER) + MultiRecordHeader.RecordLength;
                            continue;
                        } else { // No more records
                            break;
                        }
                    } else { // End of entire MultiRecord
                        break;
                    }
                }
            } // if (!EFI_ERROR(Status))
        } while(!EFI_ERROR(Status));

        if (IsSystemUuidSubRecordType) { // Management Access Record

            //
            // Copy the UUID in PCD
            //
            SizeofBuffer = sizeof (SystemUuidSubRecordwithChecksum.Uuid);
            SERVER_IPMI_DEBUG ((EFI_D_INFO, "UUID Size: %x\n", SizeofBuffer));
            PcdSetPtr (PcdSystemUuid, &SizeofBuffer, (VOID*)&(SystemUuidSubRecordwithChecksum.Uuid));
        } // if (IsSystemUuidSubRecordType)
    }

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "%a Exiting... \n", __FUNCTION__));

}
#endif //SMC_UPDATE_DMI_WITH_FRU0_SUPPORT

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
